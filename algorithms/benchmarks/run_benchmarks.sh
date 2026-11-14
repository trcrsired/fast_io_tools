#!/usr/bin/env bash
# Build and run both sort benchmarks, then print a merged comparison.
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
config="${CONFIG:-$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg}"
cxx="${CXX:-clang++}"
args="${BENCH_ARGS:-}"

"$cxx" --config="$config" -fherbceptions -lherbceptions -O3 -march=native \
	-fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables \
	-fno-ident "$here/bench_sort.cc" -o "$here/bench_sort"

(
	cd "$here/rust_sort"
	cargo build --release
)

"$here/bench_sort" $args > "$here/cpp.csv" 2> "$here/cpp.sink"
"$here/rust_sort/target/release/sort_bench" $args > "$here/rust.csv"

python3 "$here/summarize.py" "$here/cpp.csv" "$here/rust.csv"
