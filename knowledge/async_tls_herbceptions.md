# fast_io async + herbceptions + TLS — session handoff knowledge

Everything learned in the async/herbceptions working sessions. Read this first
in a new session.

## Toolchain & build invocations

- **Compiler**: plain `clang++` in PATH is the right one —
  `/home/cqwrteur/softwares/llvm_native_with_gcc/bin/clang++`
  (fork `github.com:trcrsired/llvm-project`, herbceptions + `template for` +
  P1306 expansion statements). Do NOT use `~/toolchains_build/llvm_herbceptions/build/bin/clang++`
  — that tree lacks `template for`.
- **Do NOT pass `-std=c++23`** — `template for`/pack indexing need the default
  (c++2c) mode. `-fsyntax-only` with no `-std=` works.
- Canonical flags:
  `clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg -fherbceptions -lherbceptions`
  Configs in `~/herbcfgs/` for every target (darwin, windows-msvc, wasi, etc.).
- fast_io repo: `/home/cqwrteur/libraries/fast_io_kilo/fast_io`, branch
  `herbceptions`. Only clang++ -fherbceptions is supported — library forbids
  legacy `throw x` EH entirely.

## Herbceptions rules (user-enforced)

- `throw throws expr` takes a **VALUE** (`std::error`/`cxx_std_error`), never a
  type. `catch throws(::std::error e)` catches. Never `throw` legacy EH.
- `throw_posix_error()`/`throw_win32_error(code)`/`throw_nt_error(status)`/
  `throw_openssl_error()` helpers exist; `details::async_make_error(errc|
  win32_errc|nt code)` builds a `cxx_std_error` for callback paths.
- `cxx_std_error` is an OWNING {domain,code} handle: callee takes ownership,
  `e.release()` hands it out, `details::async_dispose_error` /
  `cxx_std_error_guard` cleans up — dropping it leaks payload for boxed domains.
- Mark functions `throws` (=`FAST_IO_HERBCEPTIONS_THROWS`).

## Async architecture (all in fast_io_core_impl/operations/)

- Concepts: `refs/async.h`. Decay + awaiters: `asyncimpl/{pread,pwrite,scatter,
  transmit,flush,scan,accept,connect,close,task,common}.h`. Public API:
  `asyncimpl/ptrops.h`. Aggregation: `asyncimpl/impl.h`.
- Convention: `async_X_callback_define(sched, timeout, stm, ..., cb)` found by
  ADL; cb always takes `::std::cxx_std_error` first (then results). `timeout` is
  `posix_statx_timestamp_opt` IMMEDIATELY after scheduler. Awaiter base:
  `details::async_awaiter_result<T>` in task.h (handles inline-completion +
  suspended-resume race via `suspended`/`done`).
- Public wrappers reduce through `async_scheduler_ref(sched)` +
  `input/output/io_stream_ref(stm)` — decay layer sees only small by-value
  observers.
- Mock-friendly: callbacks may fire synchronously inside submission; awaiters
  handle it via `async_suspend_done()`.

## async_connect (committed a9fdca1a)

- `async_connect_define(sched, timeout, sockstm, void const *addr, size_t len,
  cb)` → `cb(cxx_std_error)`. Socket NOT consumed — stays caller's on success
  AND failure (failed connect → platform's unspecified state; close+recreate).
- Backends COPY addr into the op cookie at submission (sockaddr_storage, 128B
  cap → `errc::invalid_argument` if larger). Awaiter also copies addr BY VALUE —
  convenience overloads build temporary sockaddrs.
- io_uring: real `IORING_OP_CONNECT` (`io_uring_prep_connect` exists in
  linux_liburing.h) + linked timeout → timed_out on deadline cancel.
- IOCP: `ConnectEx` via WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER) GUID
  {25a207b9-ddf3-4660-8ee9-76e58c74063e} → wire bytes `b9 07 a2 25 f3 dd 60 46
  8e e9 76 e5 8c 74 06 3e`. Requires: socket WSA_FLAG_OVERLAPPED (== sockets
  created with open_mode::no_block; to_win32_sock_open_mode maps no_block→0x01)
  + pre-BOUND socket (we bind peer-family wildcard) + SO_UPDATE_CONNECT_CONTEXT
  (0x7010) on success. `connectex_func` typedef added in win32_definitions.h.
- thread_pool: blocking connect() on a worker (bounded by SYN timeout — OK,
  unlike reads which park forever).
- IoRing: N/A — no socket ops (files only), same as accept.
- Convenience overloads `async_connect(sched,to,sock,ipv4|ipv6|ip)` live in
  `posix_netop.h` and `win32_network/socket_file.h` inside `namespace
  operations` (per-platform family constants — can't be in core ptrops).
- Tests: async_generic.cc mock (addr-copy check), async_io_uring.cc real
  loopback + ECONNREFUSED + ipv4 overload; wine IOCP test /tmp/iocp_connect_test.cc;
  pool test /tmp/pool_connect_test.cc.

## async_close (committed 06a34677)

- `async_close_define(sched, timeout, stm, cb)` — op OWNS the handle at
  submission: public wrapper calls `release()` on owning streams; buffered
  streams flush pending output async first, then close handle recursively
  (`io_buffer/close_async.h`).
- io_uring: real IORING_OP_CLOSE + link timeout; timeout-cancelled close is
  finished synchronously (no fd leak). pool: worker sys_close, expired ops
  still close + report timed_out. IOCP: QueueUserWorkItem CloseHandle/
  closesocket/NtClose/ZwClose + PostQueuedCompletionStatus ferry. IoRing:
  worker close + no-match cancel-request sqe ferry.

## Backends status

- `posix/async/linux_io_uring.h` — real kernel ops: rw, scatter, transmit
  (splice), accept, connect, close; linked-timeout machinery
  (io_uring_timeout_link_block, io_uring_arm_timeout, io_uring_submit_guard,
  io_uring_new_state — cookie ctor args are member order).
- `posix/async/thread_pool.h` — posix_thread_pool; worker-run ops; deadline
  applies only until dequeue. `io_async` = pool on non-Linux POSIX (Darwin).
  NOTE: pthread_mutex_init/cond_init RETURN the error code (don't set errno) —
  use `throw_posix_error(ec)`, not errno variant.
- `win32_iocp.h` — `io_async` default on Windows (win32_io_observer,
  win32_file-scheduler opened with fi::io_async); AcceptEx/ConnectEx resolved
  via WSAIoctl; timer = CreateThreadpoolTimer + CancelIoEx; worker+posted-
  packet ferry for close.
- `win32_ioring.h` — opt-in (`win32_ioring{fi::io_async}`), runtime-resolved
  ioringapi imports (kernelbase then apiset), fails gracefully w/ win32_errc
  MOD_NOT_FOUND on Wine/old Windows. File ops only (READ/WRITE/FLUSH/CANCEL).
  Pump waits on SetIoRingCompletionEvent manual-reset event — do NOT use
  SubmitIoRing(waitOperations>0): E_INVALIDARG when SQ empty; worker-ferried
  completions only arrive via the event.
- Timers/timeouts compose everywhere via per-op deadline machinery.

## TLS plan (decided: implement ourselves, no openssl)

User hates external deps. fast_io_crypto inventory:

- HAVE: AES block (aes.h+scalar), ChaCha20 stream (scalar+SIMD), X25519,
  Ed25519 (field/verify/keypair), SHA-1/256/512 (scalar+SIMD+SHA-NI), PKCS7,
  CRC32, MD5, CSPRNG via white_hole (getrandom/arc4random/urandom/BCrypt).
- MISSING: AEAD (GHASH for GCM; Poly1305 for ChaCha20-Poly1305), HMAC, HKDF,
  NIST P-256/P-384 field + ECDSA verify, X.509/DER parser + chain validation +
  SAN hostname check, trust store loading (PEM bundle on posix; Crypt32 or
  bundled on win32).
- DONE: bignum/RSA — see "RSA" section below.
- Scope: TLS 1.3 ONLY, client first. X25519 key exchange; AES-128-GCM +
  ChaCha20-Poly1305 record AEAD; cert sigs Ed25519+ECDSA-P256+RSA; SAN hostname
  verify. No 1.2, no resumption/0-RTT initially. Server mode later.
- Danger area is X.509 chain validation + bignum, NOT the record layer.
- kTLS: own impl holds traffic secrets+seq at Finished → setsockopt(SOL_TLS,
  TLS_TX/TLS_RX) directly, no extraction API. After upgrade socket runs through
  EXISTING async ops unchanged incl. transmit (sendfile encrypts in-kernel) and
  async_close. Control records (KeyUpdate/alerts/close_notify) need recvmsg/
  sendmsg cmsg TLS_GET/SET_RECORD_TYPE handling.
- Existing stubs: include/fast_io_crypto/tls/{version,cipher_suite,
  client_hello}.h (cipher constants, partial client_hello, TLS1.2/1.3 enum).
- tls stream should be a dedicated `basic_tls_io_stream` layered stream with
  async handshake/read/write/shutdown pump, NOT openssl_driver's SSL_set_fd
  blocking path. openssl_driver exists (basic_ssl_file + BIO) but sync-only.

## Transcoder/decorator archaeology (fast_io_sucked_clone/fast_io_new)

5 concept kinds historically: device, types, manipulators, strlike, transcoders
(were "decorators"). Old model: `basic_io_deco_filter<handle, traits,
decorators_tuple>` — bounded `process_chars(from,l, to,l) → {from_it,to_it}`
through scratch buffers; `deco_partial_adapter` for split multi-byte units;
eol converter is the example; print-side `deco_reserve_*` transforms into the
format buffer.

Fork's rewrite (`transcodeimpl/`, ~7k lines) defines the RIGHT protocol core:
- `transcode_process_define(engine, f,l, t,l) → {from_next,to_next,
  need_input|need_output}`
- `transcode_sync_flush_define` (nonterminal), `transcode_finish_define`
  (terminal, validation+trailers)
- `transcode_{min,max}_output_size_define(transcode_reserve<T>, phase)`
- Directional buffered adapters basic_i/otranscoder + duplex basic_iotranscoder
  with input-underflow→output-sync-flush tie and open/finished/cancelled/failed
  state machine.
- Lesson: protocol worth porting (~types.h+concepts.h, ~300 ln); the 7k-line
  generic adapter tower (bindings×phases×ref-normalization×tuple chaining) is
  why it "never designed well" — rebuild adapters on our basic_io_buffer.

TLS-vs-transcoder verdict: reuse protocol SHAPE for record layer internally,
but TLS needs its own stream — handshake is a multi-RTT state machine, control
records aren't payload, need_input can't co_await (protocol is synchronous),
zero-copy transmit can't cross a userspace cipher (kTLS fixes).

## Remote machines & testing

- Mac mini (aarch64-darwin24): `ssh tfnhtar@ombhntlars-mac-mini.local`. Its
  local clang is too old + rejects 4-part lld version (workaround
  -mlinker-version=2.50); its installed libherbceptions.1.dylib is STALE
  (missing ___cxa_error_domain_posix) — ship ours + DYLD_LIBRARY_PATH=/tmp/…
  Cross-compile from here with aarch64-apple-darwin24.cfg works.
- Windows 11 ARM64: `ssh unlvs@earneh` — real hardware, IoRing works there;
  scp target dir must exist (no /tmp). aarch64-windows-msvc.cfg.
- Wine locally: `wine x.exe` for x86_64-windows-msvc builds — IOCP works,
  IoRing api absent (graceful throw path verified).
- Darwin io_async = thread_pool (no kqueue backend — by design: kqueue is
  readiness not completion; not worth it for non-server platform).

## Known gotchas

- Coroutine hazard: NEVER `co_await` inside a lambda-coroutine stored in a
  temporary — `this` dangles (ASan stack-use-after-scope). Use named coroutine
  functions (coro_close/coro_connect pattern in tests).
- `SubmitIoRing(waitOperations=1)` = E_INVALIDARG on empty SQ — use
  SetIoRingCompletionEvent + WaitForSingleObject for worker-ferried CQEs.
- `win32_iocp_associate` must precede any overlapped op on a socket;
  ConnectEx/AcceptEx need WSA_FLAG_OVERLAPPED sockets (open_mode::no_block).
- ConnectEx needs the socket bound first — we wildcard-bind the peer family.
- CIR (-fclangir): upstream NYI gaps — `Veto await_suspend` (bool await_suspend,
  all our awaiters) and cir.coroutine region issues; NOT herbceptions bugs.
- fixed earlier: UBSan nonnull checks on throws-return aggregates (clang
  commits 55e5f82569ee + e0b9701c5e07 on herbception-analysis in llvm fork).
- Commit style: plain message, NO signature/co-author trailers.

## Current git state (herbceptions branch tip)

- a9fdca1a async_connect, 06a34677 async_close, 205ace32 win32_ioring opt-in
  backend + posix thread_pool, 56fbe63b timeout-after-scheduler convention.
- Uncommitted: examples/0017.network/async/http.cc (function-try rewrite —
  pre-existing), various /tmp test files + untracked benchmark artifacts.
- Verified green: async_generic, async_io_uring (ASan), pool close/connect,
  wine iocp close/connect, win arm64 ioring rw+close+timeout, mac pool close.

## Next steps (TLS implementation order)

1. crypto gaps: HKDF → Poly1305 → GHASH → P-256/P-384 (RSA verify done)
2. TLS 1.3 record layer + client handshake state machine
3. X.509 DER + chain + SAN hostname + PEM trust store
4. basic_tls_io_stream + async handshake/read/write/shutdown pumps (async_close
   → close_notify then underlying close)
5. kTLS upgrade path on Linux (SOL_TLS setsockopt with our secrets)

## RSA (implemented)

Files: `include/fast_io_crypto/rsa/{number,montgomery,emsa,rsa,impl}.h`,
wired into `fast_io_crypto.h`. Internals live in `namespace
fast_io::details::rsa` (NOT `fast_io::rsa` — that name is the public
`class rsa`; a same-named namespace+class cannot coexist). Public API
matches `class ed25519` style: `rsa::verify_context` (holds modulus limbs,
r2, n0inv, exponent bytes, limb/bit/byte counts), `verify_init_to_ptr`,
`public_op_to_ptr` (raw RSAVP1), `verify_pkcs1v15_to_ptr<hasher>`,
`verify_pss_to_ptr<hasher>` (salt_size param; TLS uses digest_size).

- Number representation: `value_type = u64` limbs ALWAYS (unlike
  field_number's i386-u32 split — runtime-length loops have no unrolled
  carry schedule to control). Little-endian limb arrays, runtime `nl`.
- All limb ops are plain `for` loops (user instruction: NO `template for`
  or `#pragma unroll` for carry chains — let the compiler decide). This
  clang does NOT unroll addc loops even at -O3 -funroll-loops; rolled form
  is movzbl+btl+adcq+setb per limb (~8 insns). If hot later, revisit.
- Montgomery domain: n0inv via Newton `x *= 2 - n0*x` ×5 (equivalent to
  openssl's `(R*R^-1-1)/n0` extended-gcd dance for a single limb); r2 via
  `limbs_pow2_mod` in number.h — a WORD-LEVEL power-of-two division
  (same value openssl's BN_mod produces; replaces an earlier bit-doubling
  loop: init 234µs → 6µs for RSA-2048). Each step: rem·B mod n via one
  `udivbigbysmalltosmalldefault` 2:1 quotient-digit estimate on a
  normalized divisor + mul-sub + ≤2 add-backs (Knuth D bound).
  `limbs_mod_double` (shl1 via addc(v,v)) survives for sub-word bits.
- REDC is SOS-style matching bn_from_montgomery_word: full product in
  t[0..2nl], then n mul-add passes with carry ripple (t needs 2nl+1
  limbs), final masked conditional subtract.
- montgomery_pow = binary L2R square-and-multiply over exponent BYTES
  (public data → variable time, no window/blinding). Scratch 5*nl+2 limbs.
- Caps: modulus ≤ 8192 bits (1024B, 128 limbs), exponent ≤ 8 bytes (DER
  e is ~always 010001). Rejects: even modulus, s >= n, oversized fields.
- EMSA: `pkcs1v15_digest_info<hasher>::prefix` table (md5/sha1/sha224/256/
  384/512/sha512_224/sha512_256), emsa_pkcs1v15_check enforces PS>=8;
  emsa_pss_verify<ctx> does MGF1 + trailer/leftmost-bit/0x01 checks;
  verify_pss slices last emlen bytes of the k-byte RSAVP1 output and
  requires dropped prefix bytes to be zero.
- Everything constexpr — RSA-512 modpow static_assert runs in default
  constexpr budget (RSA-2048 needs -fconstexpr-steps bump, don't put that
  in tests).
- Verified: openssl RSA-2048 PKCS1v1.5 + PSS KATs, tamper rejection,
  python pow() fuzz over 1024/2048/3072/4096-bit moduli, ASan+UBSan clean.
- Bench (x86-64): verify ~60µs, init ~6µs; openssl asm does 15µs verify
  (our generic-C gap is the missing ADX/mulx asm inner loop; -march=native
  does NOT auto-generate ADX for the addc pattern — only ~9 mulx insns).
- Test: tests/0043.rsa/rsa.cc (self-contained hex vectors + static_assert).
- DH note: montgomery_pow IS finite-field DH (g^x mod p) mathematically,
  but it's variable-time — secret exponents need blinding/constant-time
  before it can be used for FFDHE. TLS 1.3 barely uses ffdhe anyway.
