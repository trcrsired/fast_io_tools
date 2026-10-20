# binfmt — agent handoff / context dump

This document is a complete state dump so another agent can pick up without the
original conversation. It covers the goal, the design rules, the code map, the
current committed state, measurements, and open items.

Last updated: 2026-10-20. Working tree baseline commit: `a00b76e`.

---

## 1. Objective

`fast_io_tools/binfmt` is a **binary program format** (not a text format string)
for formatting. fmt/printf are only front-ends that *compile* to this format.
Layout spec: `binfmt/spec.md`.

The feature being built is a **fast_io "status-print" wrapper**: route static
`print`/`println` through a **dynamic runtime interpreter** instead of inlining
the formatter at every call site. The reason is code size: a program with many
`println`s should not carry a formatter per call site, while still being fast
and much smaller than `{fmt}`.

The work in this session:

1. The status path (`mnp::binfmt`) was working but ~7–12× slower than native
   fast_io, and slower than fmt on the mixed line.
2. Root cause: the interpreter called the device **once per emitted piece**
   (6 pieces for `"hello {} world {} hello\n"`).
3. Fix: `binfmt_batch.h` — a hoisted-cursor writer that makes an ordinary
   program touch the device **once** (buffered) or **once via
   `scatter_write_all`** (unbuffered). Result: batched now beats fmt on all
   three benchmark workloads, byte-identical output.

---

## 2. Repos and toolchain

- Main repo (work here): `/home/cqwrteur/libraries/fast_io_kilo/fast_io_tools`
  - working dir: `<repo>/binfmt`
  - branch `master`
- Dependency repo: `/home/cqwrteur/libraries/fast_io_kilo/fast_io`
  - has one relevant fix committed: `83138302`
- No `perf` available. Use `posix_clock_gettime`.

Toolchain (the user's own herbceptions build):

```
HL=/home/cqwrteur/toolchains_build/gcc_herbceptions_install
CXX=/home/cqwrteur/softwares/llvm_native_with_gcc/bin/clang++
STRIP=/home/cqwrteur/softwares/llvm_native_with_gcc/bin/llvm-strip
INC="-I /home/cqwrteur/libraries/fast_io_kilo/fast_io/include -I . -I $HL/include"
LD="-L $HL/lib64 -lherbceptions -Wl,-rpath,$HL/lib64"
FLAGS="-O2 -std=c++26 -fherbceptions"
```

fmt (prebuilt static lib):

```
FMT_INC=-I /home/cqwrteur/softwares/fmt/include
FMT_LIB="-no-pie /home/cqwrteur/softwares/fmt/lib/libfmt.a -lpthread"
```

**Link order matters:** the source (`foo.cc`) must come before `$FMT_LIB`.

### Build commands

```bash
# header test / bench
$CXX $FLAGS $INC $FMT_INC bench/foo.cc -o bench/build/foo $FMT_LIB $LD

# shared interpreter libs (build dir bench/build is gitignored)
$CXX $FLAGS $INC -fPIC -shared binfmt.cc       -o bench/build/libbinfmt.so       $LD
$CXX $FLAGS $INC -fPIC -shared binfmt_batch.cc -o bench/build/libbinfmt_batch.so $LD
$CXX $FLAGS $INC -fPIC -shared binfmt_split.cc -o bench/build/libbinfmt_split.so $LD

# status bench (links the libs)
$CXX $FLAGS $INC $FMT_INC bench/bench_status_batch.cc \
  -o bench/build/bench_status_batch \
  -L bench/build -lbinfmt_batch -lbinfmt -Wl,-rpath,$PWD/bench/build $FMT_LIB $LD
```

Do not use `/tmp`. Put throwaway diagnostics in `binfmt/bench/build/` (ignored);
put committed test/bench sources in `binfmt/bench/` (tracked).

---

## 3. Hard design rules (the user enforces these)

- C++26 only: `template for`, pack indexing `args...[i]`. Pre-C++26 is
  unsupported here.
- Herbceptions only: functions marked `throws`; use `try { } catch throws`.
  **No legacy C++ exceptions.**
- `std::optional` is forbidden. Prefer raw allocation over `fast_io::string`
  in library code.
- No `...` / varargs in the API (`vprint` takes a cookie/array, not `...`).
- Device = plain C-ABI struct `{ vtable const* vptr; void* deviceptr; }`;
  free functions only.
- `get_output_buffer_ptrs` vtable slot is `noexcept`; non-`buffer_output_stream`
  streams get `nullptr` in that slot (and in `set_output_curr_ptr`).
- `char_type` flavours: `binfmt_*`, `wbinfmt_*`, `u8binfmt_*`, `u16binfmt_*`,
  `u32binfmt_*`.
- **No locale in this API.**
- Errors: `throw throws ::std::errc::invalid_argument;` with no diagnostic.
- Decimal formatting must use fast_io's own jeaiii routine.
- Character classification via `::fast_io::char_category::is_c_digit` /
  `is_c_alpha`.
- Use `::fast_io::string_view` / `u8string_view`, never `std::`.
- Never use `/tmp`.
- **No inlining hints.** Fix structure instead. Prefer `if constexpr` over
  `if (sizeof...)`.
- Runtime-site policy is a struct: `struct binfmt_policy { bool c_style; bool
  floating_point; };`
- Types live in the **cookie** (`typetag`), not in the program — except in the
  "split" experiment where types are a separate compile-time array.
- Pass args **by value**, never by (const) reference.
- Do not initialize the cookie; use `FAST_IO_INDETERMINATE`. Do not write cookie
  `field2` for ≤64-bit scalars.
- If all args are `basic_io_scatter_t`s, fall back to `scatter_write_all`
  directly (the status path already does this).
- Program/newline must be namespace-scope `inline constexpr` variable templates,
  **not** function-local statics.
- **Duplicate new code; do not remove or modify old code** while prototyping.
  The old interpreter/status path must keep working unchanged.
- Say `binfmt_indirector` / `indirect`, never "imbue".
- Newline is embedded in the program as a literal node for the binfmt path.
- Reference model for batching: fast_io's C++26
  `fast_io_core_impl/operations/printimpl/print_freestanding.h` (not the C++20
  one).

---

## 4. Code map (`binfmt/`)

| File | Role |
|---|---|
| `binfmt.h` | Core types: `binfmt_cookie_element`, `binfmt_typetag`, typetag/ct values, op/kind/conv/fp constants, arg traits. |
| `binfmt_print.h` | **Stock interpreter** (header): parse + dispatch + emit + per-piece device writes. Unchanged. |
| `binfmt_status.h` | **Stock status path**: `binfmt_indirector`, `status_print_define`, cookie packing, program generation, `mnp::binfmt`. Unchanged. |
| `binfmt.cc` | Out-of-line stock interpreter entries: `binfmt_status_vprint*` (one per char_type). |
| `binfmt_split.h` / `binfmt_split.cc` / `binfmt_status_split.h` | **Split experiment** (types as a separate compile-time array). Proven **larger** at scale; uncommitted, left on disk as a dead end. Do not build on it. |
| `binfmt_batch.h` | **Batched interpreter**: hoisted-cursor writer; re-implements the write half only. Committed. |
| `binfmt_batch.cc` | Out-of-line batched entries: `binfmt_status_batch_vprint*`. Committed. |
| `binfmt_status_batch.h` | Batched status path + `mnp::binfmt_batch`, reusing stock arg mapping/program/cookie packing. Committed. |
| `spec.md` | Binary program format spec. |

### Cookie model

```cpp
enum class binfmt_typetag : std::uint_least32_t {
    from_format = 0,   // type decided by the program's field node
    custom = 1,        // dynamic arg: field1 = thunk ptr, field2 = &arg
    // standard types >= 2 are (binfmt_ct_* + 1)
};

struct binfmt_cookie_element {   // 24 bytes
    binfmt_typetag typetag;
    std::size_t field1;
    std::size_t field2;
};
```

- `binfmt_ct_u64 + 1 == 6`, `binfmt_ct_c8view + 1 == 29`,
  `binfmt_ct_c16view + 1 == 30`, `binfmt_ct_c32view + 1 == 31`.
- Dynamic args (floats, pointers, exotic manipulators) become
  `{typetag=custom, field1=thunk, field2=&arg}`; the interpreter calls
  `binfmt_custom_write_fn<char_type>`.

### Program shape (status-generated)

- `{}` field node: bytes `0x13 0x00`.
- `{}` + newline: `0x13 0x00` then literal `0x0a 0x01 0x0a` (`\n`).
- Literal node starts `0x0a`.
- Field node with params: `0x13 <uleb count>` followed by param entries
  `(code<<3)|kind` + payload. Relevant fp codes:
  `type=13, ctype=12, width=10, prec=11, align=3, fill=2, sign=4,
  flag_alt=5, flag_zero=6, flag_group=7, arg=1`.

---

## 5. Current committed state

Commit `a00b76e` — "binfmt_batch: one device interaction per program via a
hoisted cursor". Adds:

- `binfmt_batch.h`
- `binfmt_batch.cc`
- `binfmt_status_batch.h`
- `bench/binfmt_batch_verify.cc`
- `bench/binfmt_batch_status_test.cc`
- `bench/bench_status_batch.cc`

Nothing pre-existing was modified. Earlier commits in the series:
`997faab`, `04591ae`, `ad5b532`, `dca9615`, `8e1be6c`, `9b3e1a8`, `eca643b`,
`824ec96`, `806c37b`, `e254f8c`, `5fb9780`, `a6e78d8`.

Deliberately **not** committed (still untracked on disk):

- `binfmt_split.cc`, `binfmt_split.h`, `binfmt_status_split.h` — dead end.
- Pre-existing untracked: `bench/float_check.cc`, `binfmt.o`, `binfmt.s`,
  `main.cc`, plus `async/`, `print_freestanding_cxx26/`, `transmit/` elsewhere.
  **Leave these alone unless the user says otherwise.**

`bench/build/` is gitignored (`bench/.gitignore` contains `build/`).

---

## 6. The batching work

### Problem

Stock `binfmt_vprint_impl` emits each piece through the device vtable:

- buffer_output_stream: `get_output_buffer_ptrs` + `set_output_curr_ptr`
  **per piece**;
- unbuffered: `scatter_write_all` **per piece**.

The mixed line is 6 pieces ⇒ 6 (or 12) vtable calls/line. That per-node dispatch
is the dominant cost (the actual memcpy and jeaiii are cheap).

### Approaches tried and rejected (measured)

1. **Wrapper device over a stack buffer** (vtable stubs that buffer locally,
  then one flush). Redirects the per-node calls to cheap stubs but still does
  ~10 indirect calls/line. Result: 0.94 s vs 0.91 s for stock — no gain.
2. **Scatter list + scratch arena** (append one scatter per piece, one
  `scatter_write_all`). Many small device copies beat zero: mixed 69 ns vs
  59 ns for the contiguous buffer — worse.

### Final design (`binfmt_batch.h`)

Keep program format, cookie, readers, field parser, jeaiii formatting from
`binfmt_print.h`; re-implement only the write half around one hoisted cursor
`{dev, curr, end, local, buffered}`:

- **`buffer_output_stream`** (vtable slot non-null): fetch the buffer once,
  `memcpy` every piece straight into it through `curr`, publish **once** with
  `set_output_curr_ptr`. No intermediate copy. This mirrors fast_io's own
  `print_freestanding`.
- **unbuffered** (slot null): gather into a local `char_type[2048]`, publish
  **once** with `scatter_write_all`.
- On cursor exhaustion: publish, refetch (buffered) or reset local (unbuffered).
  If a single piece is larger than the whole stream buffer, publish and hand
  that piece to `scatter_write_all`; then refetch.
- Padded fields are built as one contiguous run through the cursor; the stock
  `binfmt_write_padded` is the fallback for a field wider than the stream buffer.
- A **custom thunk** owns its own device writes ⇒ force a publish before it,
  refresh afterwards, to preserve program order.
- Common-case fast paths avoid constructing `binfmt_field`: integers
  (`binfmt_default_int_mag` → direct decimal) and same-width spans
  (`c8view`/`c16view`/`c32view`) are emitted directly.

Result: ordinary program ⇒ **one buffer fetch + one publish** (buffered) or
**one `scatter_write_all`** (unbuffered), independent of piece count.

`binfmt_status_batch.h` duplicates only the thin status plumbing (indirector,
`status_print_define`, lock handling, out-of-line entry dispatch) and reuses the
stock `details::binfmt_program`, `binfmt_pack_one`, `binfmt_is_scatter_of`.

---

## 7. Measurements

All N = 10,000,000, into a `fast_io::string` via `io_strlike_ref`, best of 3.
Output verified byte-identical to native fast_io.

End-to-end status path (ns/iter):

| workload | fast_io | binfmt stock | binfmt batch | fmt |
|---|---:|---:|---:|---:|
| `println one size_t` | 3.5 | 22.4 | **21.7** | 26.7 |
| `println "hello " i " world " i*3+1 " hello"` | 6.8 | 106.6 | **61.4** | 71.7 |
| `println "alpha" "beta" "gamma"` | 1.65 | 4.9 | **5.0** | 22.4 |

Direct interpreter on a raw contiguous device (ns/iter, N=10M, from
`bench/build/batch_cases.cc` / `batch_sink.cc`):

| workload | stock | batch | fmt |
|---|---:|---:|---:|
| one size_t | 19.0 | **16.8** | 18.5 |
| all spans | 58.4 | **26.1** | 13.6 |
| mixed | 92.4 | **54.9** | 47.4 |

Caveat on the fmt comparison: the fmt side is a **runtime format string**
(`fmt::format_to(back_inserter(...), "literal", args...)`), *not* `FMT_COMPILE`.
For the all-spans case fmt receives a single literal with no args, so it is a
plain memcpy — not an apples-to-apples workload. In the real status path that
case short-circuits to one `scatter_write_all` before the interpreter runs, and
both binfmt paths beat fmt there (4.9/5.0 vs 22.4).

### Binary size (llvm-strip --strip-unneeded)

| library | before | after | `.text` |
|---|---:|---:|---:|
| `libbinfmt.so` (stock) | 94,576 | **84,304** | 53,546 |
| `libbinfmt_batch.so` (batched) | 117,352 | **104,912** | 69,786 |
| `libbinfmt_split.so` | 94,656 | **84,304** | — |

The batched `.so` is +20,608 B. This is the one-time interpreter image, not
per-call-site cost. The +16 KB `.text` exists **only because both write halves
are kept side by side** (the batched header also links the stock
`binfmt_write_padded`/`write_units`/`write_fill` as the oversized-field
fallback, per the "don't modify old code" rule). Folding the batched path in as
default and deleting the old write half should remove that overhead.

---

## 8. Correctness tests

`bench/binfmt_batch_verify.cc` (ex-`batch_smoke.cc`): compares the stock and
batched interpreters byte-for-byte on the **same** program + cookie, on **both a
buffered and an unbuffered device**. 24 cases pass:

`int, neg, mixed, hex, oct, bin, hexsigned (custom path), spans, chars, u64,
i128, u128, bool, base16, base2, print-none, print-int, print-mixed,
pad-left, pad-right, pad-center, pad-zero, pad-hexalt, pad-hexaltzero`.

The last six are hand-crafted field programs (padding/align/fill/zero/showbase)
because the status front-end does not currently emit width/fill.

`bench/binfmt_batch_status_test.cc` (ex-`batch_status_smoke.cc`): end-to-end
`mnp::binfmt` vs `mnp::binfmt_batch` through `println`/`print` into
`fast_io::string`, all byte-identical.

`bench/bench_status_batch.cc`: performance + equality vs fast_io/fmt.

---

## 9. Open items / natural next steps

1. **Make batching the default.** Currently it is a side-by-side duplicate
   (`mnp::binfmt_batch`). Folding it in means:
   - replace the write half of `binfmt_vprint_impl` / emit helpers with the
     hoisted-cursor writer;
   - delete the duplicated batched emit cluster and the split experiment;
   - then `libbinfmt.so` should be ≤ the current 84 KB.
   Ask the user before touching the old code — the standing rule is duplicate,
   don't modify.
2. **Floating point** is not implemented. `binfmt_policy.floating_point`
   static_asserts false. A test-only `double` custom thunk references an
   undefined `fast_io::details::to_decimal_binary64` when linked standalone.
3. **Grouping / locale** are not handled by the interpreter (group flag is
   parsed but ignored). Not in the API by design for locale.
4. **Split files** (`binfmt_split.*`) are an uncommitted dead end. Decide whether
   to commit as a rejected alternative, or delete.
5. Per-call-site code size (the original motivation) has not been re-measured
   after batching; use `bench/status_size.cc` / `status_size_ref.cc` as a
   starting point if needed.

---

## 10. Environment gotchas

- `FAST_IO_INDETERMINATE` is not visible after `<fast_io.h>`.
  `binfmt_status.h` brackets it with `push_macros.h`/`pop_macros.h` (no include
  guard, so re-including `push_macros.h` works).
- `[[indeterminate]]` is unsupported by this clang 24.
- `FAST_IO_HERBCEPTIONS_THROWS` expands empty after `<fast_io.h>`; write bare
  `throws`.
- Lambda exception spec needs a parameter list: `[&]() throws { ... }`, not
  `[&] throws { ... }`.
- `print_freestanding_okay` rejects `void*`/`void const*` and lvalue
  `char const*&`; use `Args&&` forwarding.
- `::fast_io::posix_clock_gettime(...)` returns `posix_statx_timestamp64`
  (`{tv_sec, tv_nsec}`), with `operator-` and `operator/(ts, uint_least64_t)`.
  Print `dt` and `dt / N` directly; do **not** cast to `double` (that pulls in
  `to_decimal_binary64`, which is unresolved in standalone bench links).
- `io_strlike_reference_wrapper<fast_io::string>` **is** a
  `buffer_output_stream`, so the batched path takes the one-fetch/one-publish
  branch in the benchmarks.
- `fmt::format_to(back_inserter(s), "...", ...)` uses a runtime format string
  (no `FMT_COMPILE`); keep that in mind when comparing to fmt.
- `fmt` static lib must appear after the source file on the link line.

---

## 11. Verified claims from this session

- Stock call site cost ≈ 0: real call site ≈ direct status-program vprint.
- Per-node device interactions: 5–6 per mixed line for stock.
- Batch is ~1.7× faster than stock on the mixed line and byte-identical.
- Batched status path beats fmt on single / mixed / all-spans.
- Remaining gap to fast_io is interpreter dispatch + per-call cookie packing;
  only full static specialization (fast_io) removes it, at the cost of code size.
