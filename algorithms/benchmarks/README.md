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
geometric mean over all rows is about parity (~0.99). The C++ port is faster
on descending and large-element inputs and within a few percent on random
16-byte records.

These details were needed to reach and pass parity:

- The branchless Lomuto partition loop is unrolled by two for elements up to
  16 bytes.
- The pivot is passed to the partition helpers as a `T const *__restrict`
  parameter. With a reference or a local `__restrict` pointer the compiler
  cannot keep the pivot in a register across the partition writes and reloads
  it on every comparison. Only a restrict-qualified function parameter
  survives inlining as a `noalias` promise.
- The run scan is unrolled by two for element sizes above 8 bytes. For 8-byte
  elements the single-element loop is faster in the full sort; unrolling it
  there regresses the fully ascending case.
- A fully descending run is reversed with a 4-element block loop for
  trivially copyable elements up to 16 bytes. This vectorizes to element
  shuffles and is faster than the scalar pairwise swap (and faster than
  Rust's `slice::reverse` here).

`libc++ std::sort` is much faster than both on `s95`, because its
pattern-defeating / partial insertion path exploits the long sorted prefix.
`ipnsort` and this port deliberately use a single pass for run detection and
then sort normally; they do not have the same `s95` optimization. They win by
a wide margin on `random`, `d20`, `p5` and `descending`.
