# sort benchmarks

`bench_sort.cc` benchmarks `fast_io::algorithms::sort` against libc++'s
`std::sort` with the same comparator. `rust_sort/` does the same for
`ipnsort` and Rust's `slice::sort_unstable`. Both harnesses share the same
xorshift generator and the same pattern definitions, so the rows can be
matched across languages.

```
./run_benchmarks.sh            # full run
./run_benchmarks.sh --quick    # smaller sizes
```

The C++ side is built with the project's libc++ config and
`-O3 -march=native`. The Rust side uses `cargo build --release` (thin LTO).
`run_benchmarks.sh` writes `cpp.csv` and `rust.csv` and prints a merged table.

Columns in the merged table are nanoseconds per element (best of several
repetitions). `fast/ipn` is C++ `fast_io::algorithms::sort` divided by Rust
`ipnsort`; values near 1.0 mean parity. `fast/libstd` is libc++
`std::sort` divided by C++ `fast_io`; values above 1.0 mean `fast_io` is
faster.

Types:

- `u64`: native 64-bit unsigned integer.
- `rec16`: 16-byte trivially copyable record, compared by its first field.
- `big1k`: 1 KiB trivially copyable record, compared by its first field.

Patterns:

- `ascending`, `descending`.
- `random`: uniform random values.
- `d20`: uniform random values in `0..=20`.
- `p5`: 95% zero with 5% random values.
- `s95`: 95% ascending prefix followed by 5% random values.

## Notes

`fast_io::algorithms::sort` is a direct port of `ipnsort`, so the interesting
comparison is `fast/ipn`: values near 1.0 mean the C++ port and the Rust
implementation do the same amount of work. On the development machine the
geometric mean over all rows is about 1.04.

Two details were needed to reach parity:

- The branchless Lomuto partition loop is unrolled by two for elements up to
  16 bytes.
- For trivially copyable elements the pivot is copied into local storage. With
  a reference into the array the compiler must assume that the writes to the
  partition suffix can alias `first[0]`, and reloads the pivot on every
  comparison. Copying it once per partition keeps it in a register.

The largest remaining gap is `descending`, where the C++ port uses a simple
pairwise reverse instead of a specialized slice reverse.

`libc++ std::sort` is much faster than both on `s95`, because its
pattern-defeating / partial insertion path exploits the long sorted prefix.
`ipnsort` and this port deliberately use a single pass for run detection and
then sort normally; they do not have the same `s95` optimization. They win by
a wide margin on `random`, `d20` and `p5`.
