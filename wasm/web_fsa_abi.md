# Web File System Access syscall ABI (brainstorm)

Status: design sketch, not code. Nothing here is wired into fast_io or
fast_io_tools; the C++ and JS below are illustrative and do not compile as-is.
Names are placeholders. `wasm/externref.cc` is the toolchain probe.

## Scope

- First target: W3C File System Access in a browser (Edge/Chrome), wasm32.
- The C++ surface mirrors FSA naming: directory handle, file handle, sync
  access handle, permissions, pickers.
- JS exposes the C ABI as wasm imports, declared with clang
  `import_module`/`import_name` attributes, never `fast_io::system_call<>`.
- Only callback-style syscalls. No coroutine layer in the ABI; wrapping is
  user code.
- Other sandboxes (Win32 isolation, Android, Flatpak, macOS) stay out of this
  note; later `native_sandboxed_file = posix_file` (or nt equivalent) returns
  an fd opened in private/external storage.

## The constraint that drives the design

`__externref_t`:

- may only appear as a parameter, return value, or local; a reference can
  never be stored in linear memory, so it cannot be a struct member, live in
  a coroutine frame, or sit in a container;
- has no arithmetic and no `ref.eq` (that is GC); null can only come from JS,
  and C++ cannot test for it, so absence is signalled by `status`;
- in an import, the JS side receives the actual object, not an opaque token;
- in an export, JS may pass the actual object.

Consequence: a persistent file type cannot store the reference. It stores a
slot. The ABI, however, can traffic entirely in references, so the slot never
becomes part of the syscall vocabulary.

## The split

- `__externref_t`: ABI currency.
- `slot_t`: storage representation inside the C++ file type.
- glue-side ref table: refcount bookkeeping, addressed only by
  `retain`/`ref`/`release`, invisible to both the C++ API and the FSA-shaped
  operations.

```
C++ call                  wasm import               JS glue
--------                  -----------               -------
web_file{slot}            fsa_sync_read(ref,...)    ref = refs[slot]
   |                          |                         |
   +-- web_ref(slot) ---------+------------------------>+
                                                       |
       actual FileSystemSyncAccessHandle --------------+
```

## ABI sketch

```cpp
namespace fast_io::web {

using status_t = std::int32_t;
using slot_t   = std::uint32_t;

#define FAST_IO_WEB_SYSCALL(name) \
  __attribute__((import_module("fast_io.fs"), import_name(#name)))

extern "C" {
// ---- glue: reference <-> slot, for persistence only ----
FAST_IO_WEB_SYSCALL(web_retain)  slot_t        web_retain(__externref_t ref);
FAST_IO_WEB_SYSCALL(web_ref)     __externref_t web_ref(slot_t slot);
FAST_IO_WEB_SYSCALL(web_release) void          web_release(slot_t slot);

// ---- control plane: Promise-returning FSA methods, callback style ----
// return 0: accepted, completion fires exactly once
// return negative: synchronous rejection, no completion
FAST_IO_WEB_SYSCALL(fsa_get_file_handle)
status_t fsa_get_file_handle(__externref_t dir,
                             char8_t const* name, std::uint32_t name_len,
                             std::uint32_t create, void* ctx);

FAST_IO_WEB_SYSCALL(fsa_create_sync_access_handle)
status_t fsa_create_sync_access_handle(__externref_t file,
                                       std::uint32_t readwrite, void* ctx);

FAST_IO_WEB_SYSCALL(fsa_query_permission)
status_t fsa_query_permission(__externref_t h, std::uint32_t mode, void* ctx);

// ---- data plane: real objects, no slots, no promises ----
FAST_IO_WEB_SYSCALL(fsa_sync_read)
std::int32_t fsa_sync_read(__externref_t sync, std::uint64_t at,
                           void* buf, std::uint32_t len);

FAST_IO_WEB_SYSCALL(fsa_sync_write)
std::int32_t fsa_sync_write(__externref_t sync, std::uint64_t at,
                            void const* buf, std::uint32_t len);

FAST_IO_WEB_SYSCALL(fsa_sync_truncate)
std::int32_t fsa_sync_truncate(__externref_t sync, std::uint64_t size);

FAST_IO_WEB_SYSCALL(fsa_sync_flush)
std::int32_t fsa_sync_flush(__externref_t sync);

// close() returns a Promise in the current spec, hence callback style
FAST_IO_WEB_SYSCALL(fsa_sync_close)
status_t fsa_sync_close(__externref_t sync, void* ctx);
}
```

Rules:

- callback status: 0 success, negative -errno, 1 iterator end;
- `a`/`b` carry returned references, `w0`/`w1` carry scalars; large results
  use out-parameters;
- every sync import is wrapped in JS `try/catch` that converts the
  DOMException to a negative status, because a throwing import traps wasm;
- `uint64_t` parameters arrive in JS as BigInt; splitting `at` into
  `at_lo`/`at_hi` is an alternative if BigInt is unwanted.

## Completion

No callback pointer is passed through the ABI. One exported dispatcher is
called by JS with the actual objects:

```cpp
using web_complete_t = void (*)(void* ctx, status_t status,
                                __externref_t a, __externref_t b,
                                std::uint32_t w0, std::uint32_t w1);

struct web_request {
  bool wants_handle;
  void (*finish)(web_request*, status_t, slot_t, std::uint32_t, std::uint32_t);
};

__attribute__((export_name("fast_io_web_complete")))
extern "C" void fast_io_web_complete(void* ctx, status_t st,
                                     __externref_t a, __externref_t b,
                                     std::uint32_t w0, std::uint32_t w1) noexcept {
  auto* r = static_cast<web_request*>(ctx);
  slot_t slot = 0;
  if (st == 0 && r->wants_handle) {
    slot = web_retain(a);   // a is only valid during this call
  }
  r->finish(r, st, slot, w0, w1);
}
```

The request state lives in C++ linear memory and owns the coroutine/continuation
that the user layer will build. The completion is the only point where a
reference turns into a slot.

## File type sketch

```cpp
class web_file {
  slot_t slot_{};
public:
  web_file() noexcept = default;
  explicit web_file(slot_t s) noexcept : slot_(s) {}
  web_file(web_file const&) = delete;
  web_file& operator=(web_file const&) = delete;

  web_file(web_file&& o) noexcept : slot_(std::exchange(o.slot_, 0)) {}
  web_file& operator=(web_file&& o) noexcept {
    if (this != &o) { reset(); slot_ = std::exchange(o.slot_, 0); }
    return *this;
  }
  ~web_file() { reset(); }

  void reset() noexcept {
    if (slot_) { web_release(slot_); slot_ = 0; }
  }
  explicit operator bool() const noexcept { return slot_ != 0; }

  std::int32_t read_at(void* p, std::uint32_t n, std::uint64_t at) const {
    return fsa_sync_read(web_ref(slot_), at, p, n);
  }
  std::int32_t write_at(void const* p, std::uint32_t n, std::uint64_t at) const {
    return fsa_sync_write(web_ref(slot_), at, p, n);
  }
  std::int32_t truncate(std::uint64_t n) const {
    return fsa_sync_truncate(web_ref(slot_), n);
  }
  std::int32_t flush() const {
    return fsa_sync_flush(web_ref(slot_));
  }
};
```

The type is move-only, storable, and usable from a coroutine frame. Every
operation still crosses the ABI with a reference, not a slot.

## JS glue sketch

```js
const refs = [undefined];
let next = 1;

const imports = { "fast_io.fs": {
  web_retain(ref)   { const s = next++; refs[s] = ref; return s; },
  web_ref(slot)     { return refs[slot]; },
  web_release(slot) { refs[slot] = undefined; },

  // dir arrives as the FileSystemDirectoryHandle itself
  fsa_get_file_handle(dir, ptr, len, create, ctx) {
    dir.getFileHandle(decode(ptr, len), { create: !!create }).then(
      h => wasm.fast_io_web_complete(ctx, 0, h, null, 0, 0),
      e => wasm.fast_io_web_complete(ctx, -errno(e), null, null, 0, 0));
    return 0;
  },

  // sync arrives as the FileSystemSyncAccessHandle itself
  fsa_sync_read(sync, at, ptr, len) {
    try {
      return sync.read(new Uint8Array(memory.buffer, ptr, len), { at: Number(at) });
    } catch (e) {
      return -errno(e);
    }
  }
}};
// wasm.fast_io_web_complete is resolved lazily; imports are built before
// instantiation and the instance does not exist yet.
```

In-flight lifetime is covered by the promise closure: the object stays alive
until the dispatcher runs. `web_retain` is only needed for handles that
outlive a single operation, i.e. once per open.

## Alternatives

- **A. Integer slots everywhere (previous sketch).** No `externref`, so it
  works on any clang; every operation does a JS-side slot lookup, and the
  handle table is part of the syscall vocabulary.
- **B. This sketch.** Externref at the boundary, slot only for storage. Ops
  read like FSA methods, the glue is object-typed, and the ref table is an
  implementation detail. Cost: one extra `web_ref` import per data-plane
  operation, and clang `__externref_t` support is required.
- **C. Externref-only, no slots.** A borrowed file type valid only inside one
  call. No storage, so incompatible with fast_io file semantics and with the
  user's coroutine wrappers.

## Open questions

1. Does the local clang accept `__externref_t` as parameter/return/local
   (`wasm/externref.cc` probe)? If not, B defers and A is the fallback.
2. Is one extra import per data-plane call acceptable, or should the data
   plane take `slot_t` and confine externref to the control plane?
3. Root acquisition (`navigator.storage.getDirectory()`), pickers
   (`showOpenFilePicker` etc., which need a user gesture), and directory
   iteration (`entries()` as repeated completions with status 1) are not
   designed here.
4. `query_permission`/`request_permission` state and mode mapping; where do
   `granted`/`denied`/`prompt` live in the C++ API?
5. Error mapping: DOMException name to errno, and whether fast_io's error
   domain from fast_io_tools/error should own it.
6. Naming and layering: `fast_io::web_file` vs `web::file`, and whether the
   directory/file handle types are distinct or one `web_handle` plus kind.
7. Realm/threads: the ref table belongs to the realm that owns the handles
   (the dedicated worker for OPFS sync access handles); `web_file` must not
   cross that boundary.
8. Slot lifetime policy: explicit release only, or `FinalizationRegistry` as
   a backstop for abort paths; monotonic slot ids to avoid aliasing.
