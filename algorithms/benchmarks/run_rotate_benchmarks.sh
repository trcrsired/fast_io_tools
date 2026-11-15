#!/usr/bin/env bash
# Build and run both rotate benchmarks, then print a merged comparison.
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
config="${CONFIG:-$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg}"
cxx="${CXX:-clang++}"
args="${BENCH_ARGS:-}"

"$cxx" --config="$config" -fherbceptions -lherbceptions -O3 -march=native \
	-fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables \
	-fno-ident "$here/bench_rotate.cc" -o "$here/bench_rotate"

(
	cd "$here/rust_rotate"
	cargo build --release
)

"$here/bench_rotate" $args > "$here/cpp_rotate.csv" 2> "$here/cpp_rotate.sink"
"$here/rust_rotate/target/release/rotate_bench" $args > "$here/rust_rotate.csv"

python3 "$here/summarize_rotate.py" "$here/cpp_rotate.csv" "$here/rust_rotate.csv"
