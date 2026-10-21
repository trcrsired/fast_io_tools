# fast_io async HTTP server benchmark

Date: 2026-10-21

Async `examples/0017.network/async/http-server.cc` across four backends:
Linux io_uring, WASI inside WAVM (soft memory tagging and untagged),
Windows inside Wine (IOCP, not IoRing — see caveats), and node's
`http-server`. Raw runs are in `results/`, harness in `harness/`. The
io_uring backend now transmits files of 64 KiB and up as two splices
through a private pipe (file→pipe→socket); its own section is below
(`results/pipe-off.json`, `results/pipe-on.json` hold the before/after
runs).

## Environment

| | |
|---|---|
| CPU | AMD Ryzen 5 3600, 6C/12T (12 logical CPUs) |
| Kernel | 7.2.8-arch1-2 |
| LLVM | `$HOME/softwares/llvm_native_with_gcc` (memtag branch toolchain, mt-2 build) |
| WAVM | `0.0.0-prerelease`, runtime features: threads, networks, memtag-soft, unwind |
| Wine | `wine-11.18-137-g1977760e` |
| Node | v26.10.0 (`http-server` with `-c-1 -s`) |

## Workload and method

- Files: `/index.html` 995 B (small), `/mid.bin` 64 KiB (mid),
  `/output.mp4` 877 KiB (large).
- Closed-loop HTTP/1.1 keep-alive load generator (`harness/loadgen.c`,
  epoll): 64 conns small, 32 mid, 16 large; 4 s warmup, two 15 s measured
  runs per point. No client think time, so latency is service latency at
  that concurrency.
- Server CPU from `/proc/<pid>/stat` over the measured window; RSS =
  `VmRSS`, peak = `VmHWM` of the host process (for WAVM/Wine that is the
  whole host process, not just guest/Windows memory).
- The loadgen reconnects and re-arms epoll when a server closes a keep-alive
  connection; cpp-httplib's default 100-request keep-alive cap is disabled so
  every server runs the full window.
- Both runs per point below; throughput shown as the mean, latency/CPU
  from the same pair.

## TCP_NODELAY

Headers and body are separate writes. With Nagle on, the second small
write waits for the peer's delayed ACK (~40 ms), which dominated small
responses. `http-server.cc` now sets `TCP_NODELAY` on every accepted
socket (`set_tcp_nodelay`, per-socket because Wine does not inherit
socket options across `accept()`).

| measurement | before | after |
|---|---|---|
| Linux, 1 conn, p50 | ~41 ms | 0.034 ms |
| Linux, 64 conn, small rps | ~1.5 k | 12.8 k (default allocator), 41.8 k (tuned) |
| Wine, small p50 | 42.0 ms | 31.5 ms (Wine's completion floor) |

A standalone Windows probe (`setsockopt`/`getsockopt` TCP_NODELAY under
Wine) returned 1, so the remaining Wine latency is not Nagle: Wine 11.18
reports completions on a ~15.6 ms granularity (small p50 31.5 ms ≈ 2
ticks, mid 16.2 ms ≈ 1 tick).

## Results (mean of two runs)

### Small — 995 B, 64 connections

| variant | binary | rps | MB/s | p50 ms | p99 ms | CPU % | idle MiB | peak MiB | final MiB |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| linux-tuned | 102 KiB | 41 838 | 51.7 | 1.47 | 2.05 | 78.6 | 3.1 | 27.7 | 27.7 |
| wasm nomtg | 134 KiB | 31 809 | 39.1 | 0.148 | 1.91 | 310.1 | 104.3 | 121.1 | 121.1 |
| wasm memtag | 138 KiB | 29 952 | 36.8 | 0.148 | 1.84 | 287.6 | 170.6 | 195.8 | 186.1 |
| linux (default) | 102 KiB | 12 830 | 15.9 | 4.86 | 6.24 | 92.1 | 3.1 | 12.6 | 4.6 |
| node http-server | 122 KiB¹ | 11 118 | 14.7 | 5.23 | 10.80 | 126.0 | 71.9 | 242.7 | 162.5 |
| wine (IOCP) | 109 KiB | 2 025 | 2.50 | 31.52 | 36.55 | 46.7 | 9.8 | 28.7 | 24.6 |

### Mid — 64 KiB, 32 connections

| variant | binary | rps | MB/s | p50 ms | p99 ms | CPU % |
|---|---:|---:|---:|---:|---:|---:|
| linux-tuned | 102 KiB | 42 600 | 2 802 | 0.74 | 0.94 | 165.2 |
| linux (default) | 102 KiB | 42 343 | 2 785 | 0.75 | 0.95 | 165.0 |
| wasm nomtg | 134 KiB | 27 794 | 1 828 | 0.190 | 2.36 | 355.6 |
| wasm memtag | 138 KiB | 26 116 | 1 718 | 0.190 | 2.61 | 332.1 |
| node http-server | 122 KiB¹ | 9 113 | 600 | 3.17 | 7.25 | 173.6 |
| wine (IOCP) | 109 KiB | 1 972 | 130 | 16.22 | 18.72 | 48.4 |

### Large — 877 KiB, 16 connections

| variant | binary | rps | MB/s | p50 ms | p99 ms | CPU % |
|---|---:|---:|---:|---:|---:|---:|
| linux-tuned | 102 KiB | 7 771 | 6 817 | 1.91 | 4.12 | 87.5 |
| linux (default) | 102 KiB | 7 316 | 6 418 | 2.02 | 4.46 | 86.6 |
| wasm nomtg | 134 KiB | 3 999 | 3 509 | 2.35 | 26.97 | 507.9 |
| wasm memtag | 138 KiB | 3 900 | 3 422 | 2.29 | 25.16 | 487.2 |
| node http-server | 122 KiB¹ | 1 720 | 1 510 | 8.74 | 14.63 | 183.1 |
| wine (IOCP) | 109 KiB | 719 | 631 | 22.21 | 25.74 | 57.1 |

Binary is the stripped LTO ELF for the Linux rows, the `.wasm` module for
WASI, the `.exe` for Wine, and node's `http-server` package (¹ node itself
is 59 MiB).

## Other simple servers (same harness)

Same files, same client, defaults except TCP_NODELAY: cpp-httplib
(`set_tcp_nodelay(true)`, keep-alive cap disabled), Mongoose (built-in
nodelay), Python (ThreadingHTTPServer, nodelay enabled), Go (net/http
sets nodelay), Rust `simple-http-server` and Simple-Web-Server left at
their defaults. Throughput is mean rps at 64/32/16 conns; CPU is per size
in percent of one core; peak RSS is `VmHWM`.

| server (concurrency model) | binary | OS threads | small rps | mid rps | large rps | large GB/s | p50 small | CPU % sm/mid/lg | peak RSS |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| cpp-httplib (thread pool) | 400 KiB | 12→45 | 121 567 | 54 146 | 6 267 | 5.5 | 0.33 ms | 432/347/336 | 10.8 MiB |
| Go net/http FileServer (goroutines) | 8.4 MiB | 6→18 | 77 706 | 49 424 | 7 157 | 6.3 | 0.73 ms | 575/392/105 | 24.2 MiB |
| Mongoose (single event loop) | 202 KiB | 1 | 63 673 | 11 298 | 1 168 | 1.0 | 0.99 ms | 84/81/85 | 3.9 MiB |
| Simple-Web-Server (1 io_context thread) | 525 KiB | 1 | 54 021 | 4 452 | 332 | 0.3 | 1.11 ms | 88/96/97 | 21.5 MiB |
| Python ThreadingHTTPServer (thread per conn) | 482 B | 1→47 | 5 813 | 5 506 | 3 308 | 2.9 | 6.86 ms | 133/144/315 | 47.8 MiB |
| Rust simple-http-server (axum/tokio, 3 workers) | 3.3 MiB | 4→7 | 1 548 | 31 419 | 4 555 | 4.0 | 41.38 ms | 18/552/597 | 15.3 MiB |
| *reference:* fast_io linux-tuned (1 event loop) | 102 KiB | 1→5 | 41 838 | 42 600 | 7 771 | 6.7 | 1.47 ms | 79/165/88 | 27.7 MiB |
| *reference:* fast_io WASM untagged (thread pool) | 134 KiB | 5→20 | 31 809 | 27 794 | 3 999 | 3.5 | 0.15 ms | 310/356/508 | 121.1 MiB |
| *reference:* node http-server | 122 KiB¹ | 7→11 | 11 118 | 9 113 | 1 720 | 1.5 | 5.23 ms | 126/174/183 | 242.7 MiB |
| *reference:* Wine IOCP | 109 KiB | 1 | 2 025 | 1 972 | 719 | 0.6 | 31.52 ms | 47/48/57 | 28.7 MiB |

Binary is the linked artifact (fast_io's Linux and Wine builds are
LTO+stripped; the others are not), the `.wasm` module for WASI, and the
server script/package for interpreted servers. The fast_io WASM memtag
build is 138 KiB. ¹ Node's own executable is 59 MiB; CPython's
interpreter is excluded from the Python row.

OS threads are `Threads:` in `/proc/<pid>/status`, sampled every 200 ms
from just after listen until 5 s of the 64-connection small load, shown as
idle→peak (a single number when it did not change). For WAVM and Wine this
is the host process. Goroutines are not threads: Go's 18 are runtime
threads (M's), not its ~64 goroutines. cpp-httplib's default pool starts at
`max(8, cores-1)` = 11 workers and grows to 4× that (44) under 64
keep-alive connections; Python is one thread per connection. fast_io's
io_uring row adds up to 4 kernel `iou-wrk-*` threads for ops the ring punts
to io-wq.

- **Aggregate winner is multi-threaded**: cpp-httplib leads small and mid
  (54k vs fast_io's 42.6k on mid); on large, fast_io's spliced path now
  leads at 7.8k rps/6.8 GB/s against Go's 7.2k/6.3 GB/s while using less
  CPU. The other rows spend 3.4–5.7 cores, so the comparison against the
  single-threaded fast_io loop is cores-vs-cores, not a like-for-like CPU
  budget.
- **Per core, Mongoose is the standout**: 63.7k rps on 995 B at 0.84 core
  (~76k/core), 1.0 GB/s on 877 KiB at 0.85 core, and only 3.9 MiB peak
  RSS. fast_io tuned is ~53k rps/core small and ~7.8 GB/s/core large.
- **Go's large-file number comes from `sendfile`** (105 % CPU, 6.3 GB/s);
  the same mechanism is why Python jumps to 2.9 GB/s on large files while
  staying at ~5.5k rps on small ones. fast_io's pipe-splice path overtakes
  it (6.8 GB/s at 0.88 core) — see the section below.
- **Rust `simple-http-server` is Nagle-bound on small files**: 41.4 ms p50,
  1.5k rps, while its mid/large numbers are fine. It never sets
  TCP_NODELAY, so each headers+body pair waits for the delayed ACK. It also
  buffers whole files (no streaming) and burns 5.5–6 cores under load.
- **Simple-Web-Server collapses on large files** (332 rps, 292 MB/s): the
  handler materializes the whole 877 KiB file in a `std::string` per
  request and routes through `std::regex`; small files are fine on its one
  thread (54k rps, 0.88 core).
- **The linked Go repo is not a file server.** `andyjessop/simple-go-server`
  is a `/posts` JSON CRUD API hard-coded to `:8080`, so it cannot serve
  this workload; the Go row above is `net/http` FileServer/ServeContent.

## io_uring file → pipe → socket transmit

io_uring has no sendfile op, and `IORING_OP_SPLICE` needs a pipe on at
least one end, so a `file → socket` splice returns `EINVAL` and the
backend falls back to pread+send through a bounce buffer. The synchronous
path uses `sendfile(2)`; that gap is why Go and Python used to beat fast_io
on large files above.

The known workaround (`johalun/uring-sendfile`) is two splices through a
private pipe, file→pipe and pipe→socket. The demo on this machine (2.1 MB,
warm cache, loopback, median of 7) takes 0.94 ms per transfer against
1.23 ms for the bounce path and 0.87 ms for Go's sendfile. It is one-shot
and uses blocking sockets, so two probes checked what a server needs
(`harness/others/splice_*.c`):

- **splice is not poll-armed.** With a full socket send buffer on a
  non-blocking socket, `pipe→socket` completes immediately with `-EAGAIN`;
  io_uring does not wait for writability on its behalf.
- **a linked chain dies on that EAGAIN.** After a 1 MiB fill and a 128 KiB
  drain the next linked op gets `-EAGAIN`, every remaining linked op
  returns `-ECANCELED`, and ~896 KiB stays stranded in the pipe
  (`res 1048576, 131072, -125, -125, …`).

The backend path (`linux_io_uring.h`) therefore drives one op at a time
(fill → drain → `IORING_OP_POLL_ADD(POLLOUT)` on `EAGAIN`) and re-arms its
linked timeout per op. It is gated to `api → sock` transfers with a known
bound of at least 64 KiB, and falls back to the bounce engine if the
first fill reports the input unspliceable. This is the default transmit
path; transfers below the threshold stay on pread+send, where pipe setup
costs more than the copy it saves.

| workload (tuned malloc, mean of two runs) | bounce engine | pipe-splice | delta |
|---|---|---|---|
| 995 B, 64 conns | 40 704 rps | 41 704 rps | unchanged (below threshold) |
| 64 KiB, 32 conns | 25 465 rps / 1.68 GB/s, p50 1.22 ms | 42 618 rps / 2.80 GB/s, p50 0.74 ms | +67 % rps |
| 877 KiB, 16 conns | 2 808 rps / 2.46 GB/s, p50 5.54 ms | 7 515 rps / 6.59 GB/s, p50 1.97 ms | +168 % rps |
| 2.1 MB, 1 conn (median of 7 curls) | 1.31 ms | 0.82 ms | −38 % |

The rows above were measured behind a temporary switch. With the switch
gone and the path unconditional, the same three points re-measure at
41 838 / 42 600 / 7 771 rps (`results/linux-tuned.json`), so the gain is
stable.

The 877 KiB point holds at 0.88 core while moving 2.7× the bytes — ahead
of Go's sendfile (6.3 GB/s at 1.04 core). The 64 KiB point rises from 0.88
to 1.65 cores: each request now pays pipe setup plus two splice ops in the
server's threads. Peak RSS drops slightly (27.9 → 27.7 MiB). The untuned
build gains the most on large files (1.7k → 7.3k rps): the splice path
never allocates a per-request bounce buffer, so malloc tuning stops
mattering above the threshold.

Exactness: 995 B / 64 KiB / 877 KiB / 2.1 MB bodies byte-identical,
`Range: bytes=100000-199999` exact, a slow reader with `SO_RCVBUF=4096`
(forces the EAGAIN→poll retry on every chunk) exact, and 20 mid-transfer
client aborts leave no descriptor behind (6 before/after).

## Observations

- **The untuned linux build is allocator-bound only on small responses.**
  At 64 connections every 995 B response grows and trims its buffers
  through glibc, costing ~25 minor faults/request; setting
  `MALLOC_TRIM_THRESHOLD_`, `MALLOC_MMAP_THRESHOLD_` and `MALLOC_TOP_PAD_`
  makes it 3.3× faster at the cost of holding ~28 MiB instead of returning
  pages. Mid and large responses no longer depend on that tuning: the
  pipe-splice path avoids the per-request bounce buffer, so the untuned
  build already reaches 42.3k rps mid and 7.3k rps large. io_uring itself
  stays at one event loop thread (plus `iou-wrk-*` only for blocking ops).
- **WASM runs the thread-pool backend**, so WAVM spreads requests over
  2.9–5.1 cores; its p50 beats native at high concurrency (0.15 ms vs
  1.47 ms small) but p99/max occasionally spike (seconds) because of host
  scheduling. Per core, native is far ahead (≈53 k rps/core vs ≈10 k).
- **memtag costs ~6 % throughput on small/mid and ~2.5 % on large**, and
  ~66 MiB more idle RSS (+70 MiB peak). Checked into the runtime as
  `--enable memtag-soft` with a `-fsanitize=memtag` build.
- **Wine cannot test IoRing.** Wine 11.18 exposes neither `CreateIoRing`
  nor `NtCreateIoRing`; fast_io's runtime-loaded `win32_ioring` therefore
  falls back to `win32_file` (IOCP), which is what these numbers measure.
  The IOCP timer granularity (~15.6 ms) sets the latency floor and the
  server sits at 47–57 % CPU waiting on completions, so this row says
  nothing about real Windows IoRing performance.
- **node** uses 1.3–1.8 cores and 72–243 MiB RSS; its tail latency is
  worse than every native/wasm row at small and mid sizes.

## Sync example check

The change is confined to the async example. The sync examples
(`examples/0017.network/tcp_server.cc`, `tcp_client.cc`) were rebuilt with
`-fherbceptions -lherbceptions -fno-exceptions -fno-rtti` and ran against
each other without breakage. Note the sample server reads one message and
closes; a client that writes after the close sees ECONNRESET, which is
inherent to that example and not a regression.

## Reproduce

```sh
# linux native (the pipe-splice transmit path needs no flag)
clang++ @$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg -O3 -fherbceptions \
  -lherbceptions -fno-exceptions -fno-rtti -flto=thin -s \
  $HOME/libraries/fast_io/examples/0017.network/async/http-server.cc \
  -o http-server-linux-uring

# wasm, memtag (mt-2 sysroot) — --max-memory is required for shared memory
clang++ @$HOME/cfgs/wasm32-wasip1-threads-noeh.cfg -O3 -fherbceptions \
  -lherbceptions -fno-exceptions -fno-rtti -flto=thin -s \
  -Wl,--max-memory=268435456 \
  $HOME/libraries/fast_io/examples/0017.network/async/http-server.cc \
  -o http-server-mtg-lto-maxmem.wasm

# wasm, untagged
clang++ @$HOME/cfgs/wasm32-wasip1-threads-noeh-nomtg.cfg -O3 -fherbceptions \
  -lherbceptions -Wl,--max-memory=268435456 \
  $HOME/libraries/fast_io/examples/0017.network/async/http-server.cc \
  -o http-server-nomtg-maxmem.wasm

# windows
clang++ @$HOME/cfgs/x86_64-windows-msvc.cfg -O3 -flto=thin -s \
  $HOME/libraries/fast_io/examples/0017.network/async/http-server.cc \
  -o http-server-win.exe

# run each variant (bench.py also does warmup + two 15 s runs)
python3 harness/bench.py linux-tuned 19131
wavm run --enable threads --enable networks --enable memtag-soft \
  --mount-root ./root ./http-server-mtg-lto-maxmem.wasm -p 19102 .
python3 harness/bench.py wasm-mtg 19102

# the splice probes are one-off demos:
#   cc splice_pollarm_probe.c -o splice_pollarm_probe -luring -pthread
#   cc splice_nonblock_probe.c -o splice_nonblock_probe -luring -pthread
# (see harness/others)
```

`harness/bench.py <variant> <port>` launches the server, loads it with
`loadgen`, and prints the JSON found in `results/`.

Other servers (sources in `harness/others`):

```sh
clang++ -std=c++17 -O3 -pthread -I cpp-httplib httplib_server.cc -o httplib_server
clang   -std=gnu11 -O3 -I mongoose mongoose/mongoose.c mongoose_server.c -o mongoose_server -pthread
clang++ -std=c++20 -O2 -DUSE_STANDALONE_ASIO -I Simple-Web-Server -I asio/asio/include \
        sws_server.cc -o sws_server -pthread
cargo build --release    # in TheWaWaR/simple-http-server
go build -o go_fileserver go_fileserver.go
python3 harness/bench.py httplib 19111
```

## Caveats

- Single machine; load generator shares cores with the server, so absolute
  rps is a lower bound for all rows. Comparisons are same-machine and stable
  across the two runs (spread < 8 % except where noted).
- The other-server rows use each project's default concurrency (httplib's
  `max(8, cores-1)` pool growing to 4×, 1 Mongoose loop, 1 Simple-Web-Server
  thread, Python thread per connection, all Go cores, axum/tokio workers).
  Aggregate rps therefore reflects thread count as much as per-request
  cost.
- WAVM/Wine RSS is the host process, not guest/Windows accounting.
- No production workload: 1 KiB–877 KiB static files, no TLS, no logging,
  no disk pressure (files are cached).
- Windows numbers are IOCP under Wine, not IoRing on Windows.
