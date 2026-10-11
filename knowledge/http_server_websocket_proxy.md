# fast_io async HTTP server + WebSocket + CORS-bypass proxy — session handoff

Continuation of `async_tls_herbceptions.md` (read that first — toolchain,
herbceptions rules, async op conventions are there and NOT repeated). This
covers the async static HTTP server, the RFC 6455 wire layer, the
net_service/SIGPIPE design, the pwa-player merged proxy, and every pitfall
hit along the way.

## Build invocations used

```
# Linux, warnings-clean gate:
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg -fherbceptions -lherbceptions \
	-Wall -Wextra -Wpedantic -Wmisleading-indentation -Wunused -Wuninitialized \
	-Wshadow -Wconversion -fsyntax-only <file>.cc

# Windows MSVC target (syntax check; note -fherbbceptions typo sometimes in user cmd):
clang++ --config=$HOME/herbcfgs/x86_64-windows-msvc.cfg -fherbceptions -lherbceptions \
	<same warning flags> -fsyntax-only <file>.cc
```

Both configs must pass with **zero warnings** — the user requires warning
cleanliness on both, and warnings in *library headers* instantiated by the
example count too (fix the header, not just the TU).

## Files created / touched

| File | Holds |
|---|---|
| `examples/0017.network/async/http-server.cc` | node.js-style async static HTTP server (~1100 lines) |
| `examples/0017.network/async/ws_echo_server.cc` | standalone WS echo server (same supervisor shape) |
| `examples/0017.network/async/ws_client.cc` | WS client: DNS, connect, handshake verify, masked frames |
| `include/fast_io_crypto/websocket/impl.h` | RFC 6455 wire layer — see API table below |
| `include/fast_io_core_impl/socket/lna.h` | LNA (local-network-access) address classification |
| `include/fast_io_core_impl/iso/imf_date.h` | IMF-fixdate HTTP `Date`/`Last-Modified` (+ `mnp::imf_date_get` scanner) |
| `include/fast_io_hosted/platforms/posix_ifaddrs.h`, `win32_ifaddrs.h` | interface enumeration without raw getifaddrs/GAA in examples |
| `include/fast_io_core_impl/mode.h` | `net_service_flags` enum + operators |
| `include/fast_io_hosted/platforms/posix_netmode.h` | `posix_net_service`, `posix_empty_network_service`, `posix_socket_ignore_sigpipe_impl` |
| `include/fast_io_hosted/platforms/posix_netop.h` | `MSG_NOSIGNAL` on send/sendto, dual-stack `tcp_listen`, `getpeername` |
| `include/fast_io_hosted/platforms/win32_netop.h` (approx) | `win32_wsa_service` flag-first ctor |
| `include/fast_io_freestanding_impl/io_buffer/output_async.h` + async ops | schedulers hold `net_service`; `0zu` fixes |
| `pwa-player/cors-bypass/pwa-player-server.cc` | merged static server + forward CORS-bypass proxy (separate repo `/home/cqwrteur/libraries/pwa-player`) |
| `pwa-player/.clang-format` | copied from fast_io, force-added past `.gitignore` |
| `pwa-player/source/sw.js` | `PWAPLAYER_VERSION` bumped 480→481 |

Commits (local only): fast_io `3343c58a` http-server, `6083d5bc` websocket,
`206c1400` warning cleanup; pwa-player `c46d5bf` merged server.
User rule: **no co-author / no sign-off trailers, ever.** Short imperative
lowercase messages.

## Async server architecture (the shape that works)

- `accept_loop` is a supervisor coroutine: build the listener ONCE outside,
  then loop restarting only the accept coroutine on mid-run errors. **Never
  rebind inside the retry loop** — a held port (`EADDRINUSE` from a stale
  process) becomes an infinite spin.
- Each accepted connection → **detached** `io_async_task` session coroutine.
  A detached frame's `unhandled_herbception` slot silently swallows errors —
  that is BY DESIGN ("there is nobody to report to"); EPIPE from a dead peer
  just kills the frame. Add an explicit `catch throws(::std::error)` inside
  the session if you need visibility.
- `accept` on the listener yields `native_socket_file` — wrap it in
  `u8iobuf_socket_file` before buffered/async ops.
- All I/O through the buffered stream: `co_await fi::io::async_scan(...)`,
  `async_print`, `async_scatter_pwrite_all_bytes`, `async_pread_all_bytes`.
  The buffered stream preserves leftover pipelined bytes across scans — the
  WS frame scanner relies on this after the HTTP upgrade.

## HTTP server internals worth reusing

- Request head: `u8http_header_buffer` + `async_scan` gives method/uri/proto
  plus a header map; capture `Range`, `If-*`, `Origin`, `Upgrade`,
  `Connection`, `Sec-WebSocket-*` from it.
- Static serving uses **`fi::at(rootdir)` + fstatat/openat** — never
  concatenate paths. `rootdir` must outlive all requests (see Bugs).
- Byte ranges parsed via `parse_by_scan` (this is where it belongs — a real
  grammar); integer CLI args via `fi::scan` on an `ibuffer_view` (throws on
  failure; main's `catch throws` reports).
- 302 directory redirect: splice `/` BEFORE the `?` — `Location: url_path +
  "/"` on the raw target turned `/dir?x=1` into `/dir?x=1/` → redirect loop.
- CORS: `--cors` / `--cors=<origin>`; OPTIONS preflight answers 200 + full
  AC headers; pwa-player *requires* it (`fetch(mode:"cors")`,
  `crossOrigin="anonymous"`).
- LNA: `--lna` restricts sources to loopback/local/public/allow-list via
  `socket/lna.h` classification of `getpeername` result.
- Banner + directory listings: build into a `u8ostring_ref_fast_io` and emit
  with ONE `fi::io::print(fi::u8err(), banner)` — multiple `perr` calls are
  multiple write syscalls (user explicitly asked for single-write output).

## WebSocket wire layer (`fast_io_crypto/websocket/impl.h`)

Lives in crypto umbrella (handshake needs sha1 — `fast_io.h` doesn't pull
crypto; `fast_io_crypto.h` needs `fast_io.h` included FIRST).

| API | Purpose |
|---|---|
| `mnp::websocket_frame_header_get(hdr)` | scan manip; streaming ≤14-byte header parse across split buffers; returns partial/ok/eof like `imf_date_get` |
| `websocket_frame_header` | fin/mask/opcode/payload_length/mask_key |
| `websocket_opcode` | enum |
| `websocket_encode_frame_header[_masked]` | 2..14-byte wire header |
| `websocket_apply_mask` | XOR mask/unmask in place |
| `websocket_accept_key` | sha1(key+GUID)→base64 (28 chars; byte-exact vs RFC 6455 §1.3) |
| `websocket_b64_encode` | shared b64 (extracted so clients can build keys) |

Parser validates RSV bits, reserved opcodes, fragmented/oversized control
frames, 64-bit-length top bit. **Mask policy is the caller's** — server
rejects `!fh.mask` with close 1002; client always masks.

Server upgrade path: require GET + `Upgrade: websocket` + `upgrade` as a
comma-list **token** in `Connection` (not substring) + key + `Version: 13`
→ `101` + `Sec-WebSocket-Accept` → frame loop in the same coroutine (the
stream's buffered leftovers carry into it). Echo preserves `fin`; ping→pong
same payload; close→reply+return; >16MiB→close 1009.

Client: `native_dns_file` + `fi::to_ip(ent, port)` → `tcp_connect` → wrap in
`u8iobuf_socket_file`; 16-byte nonce from `u8native_white_hole` →
`websocket_b64_encode` → key; after the 101, recompute `websocket_accept_key`
and VERIFY equality. Frame writes: `websocket_encode_frame_header_masked` +
`websocket_apply_mask` + single `async_scatter_pwrite_all_bytes`.

Known gaps (deliberate): no subprotocol negotiation, no permessage-deflate,
continuations pass through un-reassembled.

## net_service / SIGPIPE — the final design (user-driven, remember this)

`EXIT: 141` on aborted video streams = SIGPIPE, delivered before ANY C++
error machinery — no catch sees it. Browsers abort constantly on seek.
`MSG_NOSIGNAL` is the elegant per-op answer but **io_uring
`WRITEV`/`SPLICE`/`WRITE` have no msg_flags field** — disposition drop is the
only thing covering the hot path. Final API:

```cpp
// include/fast_io_core_impl/mode.h
enum class net_service_flags : ::std::uint_least32_t
{
	none = 0,
	posix_no_ignore_sigpipe = 1U << 0   // opt OUT of the SIGPIPE drop
};
// full operator set like open_mode: & | ^ ~ &= |= ^=  — every name spelled
// ::fast_io::net_service_flags inside the operators (user requirement)
```

- `posix_net_service(net_service_flags = none)` — drops SIGPIPE at
  construction by default; `{posix_no_ignore_sigpipe}` keeps a program's own
  disposition.
- `win32_wsa_service(net_service_flags = none, version = 514)` — **name kept
  exactly**; flag first param (documented no-op, no SIGPIPE on win32);
  winsock version second. `net_service` alias → this on win32.
- `posix_empty_network_service(net_service_flags = none)` — dummy accepts the
  flag; `net_service` alias → it on **Cygwin** (win32-shaped net, no winsock,
  posix netop chain not included under `_WIN32`).
- Schedulers `linux_io_uring` + `posix_thread_pool` each hold
  `posix_net_service net_service{}` unconditionally — covers sync fallback
  AND io_uring paths; the flag machinery on `io_async_t` was REVERTED (it
  stays a plain tag).
- `MSG_NOSIGNAL` stays on explicit `send`/`sendto` impls — right answer where
  a flag exists.
- `tcp_listen`/`tcp_connect` do NOT touch signal state (no per-call syscalls,
  no racing a user's own handler).

**Trap that cost real time:** `#if defined(SIGPIPE)` inside `posix_netop.h`
compiled the helper to an EMPTY function — `<signal.h>` wasn't in that part
of the include chain. Fixed by adding `<signal.h>` to the `__has_include`
block in `posix/impl.h`. If a macro-guarded helper silently does nothing,
suspect the header isn't included, not the logic.

## pwa-player-server (cors-bypass proxy) — design + hard rules

Forward proxy at `/http://host/path` and `/https://host/path`; static serving
is OFF by default (`--server` opt-in — the binary's job is the proxy, not
file sharing). `--bypass` enables the proxy role; `--cors[=origin]`,
`-p/--port`, `-c/--cache`, `--lna`, `--no-index`, `--no-dir`, `-h`.

HLS correctness (user's explicit concern — live playlists advance
`#EXT-X-MEDIA-SEQUENCE` and a stale manifest makes players request wrong
segments → 404s/freeze):

- **`Connection: close` + fresh socket per upstream request** — a response
  can NEVER be misbound to a different request. Structural fix, not a cache
  policy. Costs a TCP connect per request; correctness wins for a dev proxy.
- **Zero caching** anywhere; manifest responses force `Cache-Control:
  no-store` so the BROWSER can't cache playlists either.
- Manifest URLs (segments AND `URI="..."` attrs like `#EXT-X-KEY`) resolve
  against the URL of the RESPONSE being rewritten — i.e. post-redirect.
- Sequential per connection (session loop is sequential) → no interleaving.
- `Content-Length`/`Transfer-Encoding` regenerated from what we actually
  emit — hop-by-hop honesty; Range forwarded verbatim upstream.
- Redirects followed internally up to 10.
- HTTPS upstream: `u8iobuf_native_tls_socket_file` + `fop::handshake(h,
  hostname)` — same round-trip code as http (upstream stream is a template
  param). Real cert validation; NO `--insecure` escape hatch (possible future
  work for self-signed upstreams).
- Upstream errors now print `proxy: <err>` to stderr — earlier a dead
  coroutine lost its buffered output and closed silently.

## fast_io API gotchas (each of these bit me once)

- `u8string`: has `.append`/`push_back`; **NO `operator+`/`+=`** for concat.
- `u8string_view`: **NO `find(char)`/`npos`** — scan manually or loop.
- `fi::scan(ibuffer_view{...}, val)` is the house style for integer parse —
  don't reach for `parse_by_scan` except real grammars (e.g. byte ranges).
- `os_c_str`/`c_str` paths need NUL-terminated storage — a `u8string_view`
  name field is NOT safe to hand over.
- `native_dns_file`/`to_ip` give `fi::ip` — don't double-convert.
- `char_type` of u8 streams is `char8_t`, not `std::byte` — casts needed for
  byte buffers, and `perrln` on a `u8string_view` needs the **u8** err
  stream, not the char one.
- `noexcept_call` DOES invoke (checked asm) — it wasn't the culprit when
  signal() seemed ignored; the `#if defined(SIGPIPE)` guard was.
- `mnp::hex`, `mnp::hex_get`, `mnp::crlf` exist for chunk parsing — don't
  hand-roll.
- `FAST_IO_HERBCEPTIONS_THROWS` is library-internal — examples write plain
  `throws` on coroutines/functions.
- Listener type must be `u8socket_file`-flavored to match wrapped sessions.
- In `io_scatter_t` verify member names before use (`base`/`len`).
- `io_async_task` return values: check what the detached/session task
  actually supports before designing coroutines that return results —
  proxy relay ended up returning `bool` for keep-alive instead of a
  nonexistent flag set.

## Warning-cleanup playbook (both targets, zero warnings achieved)

- `0` as size_t param → `0zu` (scatter/transmit/common call sites).
- Instantiation-note noise: filter `warning:` lines only; the real errors
  hide among `note: in instantiation` spam.
- Anonymous union member with a named struct inside → can't declare the
  struct in the union; hoist it ABOVE (linux_liburing sqe fix).
- `WSAGetLastError`/`WSASend` are `int`: comparing to `0u` causes
  -Wconversion itself; cast the RESULT to u32 at assignment, keep `==0`.
- Param shadowing a field (`worker_cap`) → rename the param.
- Deleting an "unused" function (`days_from_civil`) — I clipped the adjacent
  `month_names` table → win32 build error. Check the diff before assuming a
  removal is clean.
- Zero-size arrays (`salt` in ktls) need `__extension__` under -Wpedantic.
- `nfds_t{1}`-style explicit-typed literals for narrow types.
- `posix_statx_timestamp_opt` unused params: name-and-comment or drop name.

## Debugging techniques that saved the session

- `pkill -f "pwa-server"` matches the INVOKING SHELL's own cmdline → kills
  your own exec. Use `pkill -x <name>` or exact pgrep.
- Backgrounded servers keep the exec pipe open (timeouts) — use
  `setsid ... </dev/null >log 2>&1 &`, then poll the log/port in SEPARATE
  calls.
- `printf` probes to a redirected file are stdio-buffered — `fflush` or they
  never appear (my RELAY/TXDONE probes looked dead but weren't).
- Stale binary on the port → pgrep finds the OLD process, tests hit old
  code, new instance spins EADDRINUSE. Kill by port, verify banner/log
  timestamps before trusting results.
- Test harnesses: hand-rolled python WS client (the protocol is plain bytes —
  no lib needed) and `python3 -m http.server` as a controllable upstream.

## Two nasty bugs worth remembering

1. **CORS block leading `\r\n`**: `bypass_cors_block()` started with CRLF;
   appended after complete `k: v\r\n` header lines it produced a blank line
   → header block ended early → cors headers + Content-Length + body all
   arrived AS BODY bytes (the "304 bytes data" mystery). Fix: no leading
   separator in the block; emit `\r\n` explicitly at call sites.
2. **`fi::at(rootdir)` dangling**: `rootdir` scoped inside `if (cfg.http)`;
   `at()` BORROWS the handle → destroyed at scope end → every static lookup
   404 while the proxy still worked. Hoist handles to program lifetime.
