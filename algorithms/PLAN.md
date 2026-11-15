# fast_io::algorithms — design plan

Draft lives in `fast_io_tools`; only moves into `fast_io` after the design is
settled.

Target: replace the parts of the C++ standard algorithm library that are not
usable under `-fno-exceptions` / `-fherbceptions`, while keeping binary size and
icache pressure under control.

Baseline: C++23 freestanding. `<ranges>`, `<iterator>`, `std::to_address`,
`std::bit_cast` and `if consteval` are all available and allowed.

Header layout: one algorithm family per header.

- `advance.h`: `advance_iters`.
- `common.h`: shared index helpers, flag lookup, iterator/range concepts, and
  element traits shared by several families.
- `compare.h`: the default `compare_three_way` comparator.
- `construct.h`: the uninitialized default/value construction family.
- `copy.h`: the copy family, including the uninitialized copy variants.
- `destroy.h`: the destruction family (`destroy_at`, `destroy`, `destroy_n`).
- `fill.h`: the `fill` family, including the uninitialized fill variants.
- `find.h`: the `find` family.
- `for_each.h`: the `for_each` family.
- `heap.h`: the heap family.
- `relocate.h`: the relocate family, including the uninitialized move variants.
- `reverse.h`: the `reverse` family.
- `sort.h`: the unstable sort.
- `impl.h`: umbrella. It is the only header that uses `#include`. It includes
  `common.h` and `compare.h` first and then every algorithm header in
  dependency order. Family headers contain no includes at all and are not meant
  to be included on their own.
- `tests/`: one test translation unit per family. Tests include `../impl.h`
  with a relative path so the directory can move into `fast_io` unchanged.
- `benchmarks/`: C++ and Rust benchmark harnesses for `sort`.

One family per header does not mean a family must reimplement its
dependencies. Families call each other's internal implementation when that
avoids a second copy of the same code, which keeps binary size and
instruction-cache pressure down. `sort` uses `reverse` for a fully descending
input; both share the same instantiation instead of carrying a private reverse.
`destroy` uses `advance` to skip trivially destructible ranges in constant
time instead of carrying a second advance loop.

## Non-negotiable decisions

- Namespace is `fast_io::algorithms`.
- `fast_io::freestanding` remains the standard-library replacement layer.
- Every function is marked `inline`. This is for ODR safety, not optimization.
- No `noinline`. No attribute whose purpose is to force call emission.
- No overload sets. One function per name. Internal dispatch uses
  `if constexpr`, not additional overloads.
- Names stay as already written:
  - `for_each_iters`
  - `for_each`
  - `for_each_index`
- `_index` APIs always validate.
- Iterator APIs never validate.
- No `_unchecked` variants. The convention already encodes the policy.
- `f` receives the element (`f(*first)`), never the iterator.
- Comparators use `<=>`, not `<`, and are passed by value.
- Comparator invocation goes through a named `compare` member, never through
  `operator()`.
- `operator()` is only accepted as a lambda adapter input, and is erased to a
  function pointer at the API boundary.
- Unrecoverable internal failure uses `__builtin_trap()`.
- Construction into uninitialized storage uses placement new. `std::construct_at`
  is not used; placement new works under herbceptions and in constant
  evaluation.
- All `throws` specifications must be correct.
- Use `FAST_IO_HERBCEPTIONS_THROWS_IF(cond)` on every function.
- With herbceptions enabled it is `throws(cond)`; `throws(false)` is
  `noexcept(true)`.
- `noexcept(!(cond))` is only the fallback expansion for compilers without
  herbceptions. It is not the design spelling of the contract.

## Existing API

Keep these signatures; fix the bodies and specifications.

- `for_each_iters(Iter first, Snt last, F f)`
- `for_each(R&& r, F f)`
- `for_each_index(R&& r, size_t firstidx, size_t lastidx, F f)`

Internal helpers may change freely.

## Violation policy

`throwing_violations` comes from the container’s exposed allocator flag.

- The range/container exposes one allocator flag value.
- Read it from `remove_cvref_t<R>`.
- `throwing_violations` is true only when
  `(flags & allocator_adapter_flags::throws_on_violations) != allocator_adapter_flags::none`.
- Use a bitwise test, not equality. Flags can combine, e.g.
  `throws_on_violations | secure_clear`.
- If the range does not expose the flag, use termination semantics.
- Do not fall back to inspecting `allocator_type`.
- Read the flag before any contiguous decay, otherwise the policy is lost when
  the range is reduced to pointers.

Consequences:

- fast_io containers with `throws_on_violations` throw from `_index` APIs.
- C arrays, views, `std::vector`, foreign ranges and anything without the flag
  terminate.
- Iterator APIs never consult the flag, because they never check.

Containers that need updating when this moves into fast_io:

- `vector`, `sized_vector`: forward the allocator adapter flags.
- `array`, `span`, `index_span`: forward the template `flags` argument.
- Anything else: omit the member, which selects termination.

## Index normalization

`_index` APIs need one shared checking routine.

Two layers:

1. Typed wrapper, used in constant evaluation:
   - takes `T*`, element count, first index, last index
   - returns a typed pointer pair
2. Shared runtime checker:
   - takes `std::byte const*`, element count, first index, last index,
     and a runtime `sizeof(T)`
   - returns `std::byte const*` pair
   - parameterized only by `throwing_violations`
   - caller `bit_cast`s the result back to the typed pair

Requirements:

- `static_assert` that the byte pair and typed pair have the same size and are
  trivially copyable.
- `sizeoft` stays a runtime argument. Making it a template parameter
  reintroduces per-`T` instantiations.
- `bit_cast` is required because the shared checker only sees `const byte*`;
  bit-casting avoids casting away constness.
- The non-contiguous path also goes through the shared checker, then performs
  only the typed iterator arithmetic locally.
- `algo_index_to_iters` must read the range flag; the current hardcoded
  `false` is wrong.
- Contiguous branch must use `to_address(ranges::data(r))`, not assume
  `ranges::data(r)` is already a pointer.

Resolved semantics (matches `vector::erase_index`):

- `[firstidx, lastidx)` is half-open.
- `firstidx == lastidx` is a valid empty range.
- `lastidx == size` is valid; the upper bound is inclusive of `size`.
- `npos` is not special-cased.
- The check is: `lastidx < firstidx || n < lastidx`.
- The exposed flag member is a static
  `::fast_io::allocator_adapter_flags allocator_flags`.
  If absent, the range is treated as `none` (termination).

## Contiguous decay

The rule is: normalize contiguous iteration to `T*` before the heavy
instantiation.

- `for_each(vec.begin(), vec.end())` and `for_each(ptr, ptr + n)` must call the
  same pointer specializations.
- Contiguous non-pointer iterators decay via `to_address`.
- Do not call `to_address` on a sentinel unless the sentinel is itself a
  contiguous iterator.
- If the sentinel is sized, compute the length first and use
  `to_address(first) + n`.
- If neither condition holds, stay with the typed iterator path.
- For ranges, prefer `data + size`, which is always valid for a contiguous
  range and avoids sentinel conversion.

Decay happens after the violation policy has been read from the original
range/container.

## Iterator noexcept detection

No-throw detection for iterators is deliberately minimal and lives in
`common.h`.

- `iter_no_throw<Iter>` only checks that `++it` is `noexcept`.
- `sentinel_no_throw<Iter, Snt>` adds `sentinel_for<Snt, Iter>`.
- `output_iter_no_throw<OIter, T>` adds `output_iterator` and uses the same
  `++it` check.
- `!=`, `to_address`, iterator arithmetic, and sentinel subtraction are assumed
  non-throwing. If an iterator throws from those operations it is unsupported.
- Ranges are checked through their iterator: `iter_no_throw<iterator_t<R>>`.
- Element operations stay in the algorithm concepts: callback invocation,
  comparison, assignment, construction, and destruction.
- `algo_index_to_iters` is only marked `throws` for the violation policy.

## `advance`

- API: `advance_iters(Iter& it, iter_difference_t<Iter> n)`.
- Random-access iterators advance with `it += n`; other iterators walk one step
  at a time. Negative counts are supported for bidirectional iterators; a
  negative count on a weaker iterator is a contract violation and traps.
- Iterator arithmetic (`+=`, `-`), `!=` and sentinel subtraction are assumed
  non-throwing, as elsewhere. The `throws` contract only adds the `++`/`--`
  checks.
- `destroy` reuses the advance implementation so a trivially destructible
  range is skipped in constant time on random-access iterators instead of
  walking it.

## `for_each_iters`

- No checks.
- `f(*first)`.
- Contiguous non-pointer iterators decay to pointers and re-enter the pointer
  path.
- `throws` must include:
  - callback invocation on `iter_reference_t<Iter>` (not `iter_value_t`)
  - dereference
  - increment
  - comparison
  - any conversion used by decay
- The decay path forwards `f` by reference through the recursive call.
- The decay path converts `last` with `to_address` (contiguous sentinel) or
  computes the pointer end from `last - first` (sized sentinel).

## `for_each`

- Stays a thin delegation to `for_each_iters(ranges::begin(r), ranges::end(r), f)`.
- No checks and no flag lookup.
- No separate contiguous branch is required; `for_each_iters` handles decay.

## `for_each_index`

- Checked entry point.
- Read the range’s allocator flag to select `throwing_violations`.
- Obtain the iterator/pointer pair from `algo_index_to_iters` with both indices.
- Delegate to `for_each_iters`.
- `throws` includes the violation policy and the callback.

## `find`

- API: `find_iters(first, last, value)`, `find(r, value)`,
  `find_index(r, firstidx, lastidx, value)`.
- `find_iters` returns `Iter`; `find` returns `ranges::iterator_t<R>`;
  `find_index` returns `std::size_t` and uses `lastidx` as the not-found
  sentinel.
- Comparison is `operator==`, not `<=>`.
- The value is decayed to `T` internally when `T` is trivially copyable,
  non-array, and `sizeof(T) <= 2 * sizeof(std::size_t)`.
- Contiguous iterators decay to pointers and re-enter the pointer path.
- Hosted glibc builds forward 1-byte searches to `__builtin_memchr` and 4-byte
  searches to `__builtin_wmemchr` when the value is a bitwise-searchable type
  with matching size/alignment. This mirrors `std::find` and avoids losing to
  libc's runtime-dispatched AVX2 code in non-`-march=native` builds. The
  forwarding is skipped during constant evaluation.
- SIMD path: used only when the element type is trivially copyable,
  `has_unique_object_representations_v<T>` is true, the element size is
  1/2/4/8/16, the element size is at most two `size_t`s, and a SIMD width is
  available. It loads `simd_vector` blocks and scans the lane mask for the
  first matching element.
- The SIMD loop is unrolled four vectors at a time and ORs the comparison
  masks before the first branch, so a full scan costs one branch per four
  vectors.
- SIMD compares object representations bitwise; the scalar fallback preserves
  normal `operator==` semantics (for example `-0.0f == 0.0f` and padded
  types).
- `_index` validates through `algo_index_to_iters`, so `lastidx < firstidx` and
  `size < lastidx` are violations; `lastidx == size` is allowed.
- `throws` includes the violation policy, the comparison, and the final
  `found - first`.

## `copy`

- API pairs: `copy_iters` / `copy`, `copy_n_iters` / `copy_n`,
  `copy_backward_iters` / `copy_backward`, `non_overlapped_copy_iters` /
  `non_overlapped_copy`, `non_overlapped_copy_n_iters` /
  `non_overlapped_copy_n`, `uninitialized_copy_iters` / `uninitialized_copy`,
  `uninitialized_copy_n_iters` / `uninitialized_copy_n`,
  `uninitialized_copy_backward_iters` / `uninitialized_copy_backward`.
- `_iters` variants take iterators in every position; the unprefixed variants
  take a range plus the destination. The `_n` range variants copy the first
  `count` elements of the range.
- Assignment paths use `copy` (overlap-safe, `my_memmove`) and
  `non_overlapped_copy` (overlap not allowed, `my_memcpy`).
- `uninitialized_copy*` placement-constructs into uninitialized storage and
  destroys the partial result on failure through `uninitialized_guard`.
- Byte fast path: both value types trivially copyable and either the same value
  type or both integral/`std::byte` with equal size. Constant evaluation takes
  the typed loop instead of the `memmove`/`memcpy` call.
- Contiguous iterators decay to pointers; `to_address` and iterator arithmetic
  are assumed non-throwing.
- `throws` includes assignment/construction/destruction (copy has no index
  entry point).

## `fill`

- API pairs: `fill_iters` / `fill`, `fill_n_iters` / `fill_n`, plus
  `fill_index`.
- `fill_iters` takes an iterator/sentinel pair and returns the end iterator;
  `fill` takes a range and returns `ranges::iterator_t<R>`. `fill_n` fills the
  first `count` elements of a range and returns the iterator one past the last
  element written, matching `copy_n`.
- `fill_index(r, firstidx, lastidx, value)` validates the indices through
  `algo_index_to_iters` and fills `[firstidx, lastidx)`.
- Contiguous iterators decay to pointers and re-enter the pointer path, so
  `fill(vec, value)` and `fill(vec.data(), vec.data() + vec.size(), value)`
  share one instantiation.
- A one-byte element whose value is also one byte is filled with
  `my_memset`: every target byte receives the same byte, which is exactly what
  element assignment produces. This covers `char`, `std::byte`, and the fixed
  width byte integers. The call is skipped during constant evaluation.
- Wider elements use a plain assignment loop. Clang turns the loop into a
  broadcast store plus unrolled vector stores for 4/8/16-byte elements, so no
  explicit SIMD path is needed.
- `throws` includes the iterator increment, the element assignment, and, for
  `fill_index`, the violation policy and the final index arithmetic.

### `uninitialized_fill`

- API pairs: `uninitialized_fill_iters` / `uninitialized_fill`,
  `uninitialized_fill_n_iters` / `uninitialized_fill_n`, plus
  `uninitialized_fill_index`.
- Placement-constructs copies of the value into uninitialized storage and
  returns the end of the written range.
- One-byte elements use `my_memset`, because constructing a one-byte object
  from a one-byte value writes that same byte. Constant evaluation falls back
  to the placement-new loop.
- Everything else uses a placement-new loop, which clang turns into broadcast
  and unrolled vector stores for small trivially copyable types.
- A partially constructed range is destroyed through `uninitialized_guard` when
  a constructor throws.

## `construct`

- API pairs: `uninitialized_default_construct_iters` /
  `uninitialized_default_construct`, `uninitialized_default_construct_n_iters` /
  `uninitialized_default_construct_n`, and the corresponding
  `uninitialized_value_construct*` pair.
- Default construction performs `T t;`; value construction performs `T()`.
- `uninitialized_default_construct` on a trivially default constructible type
  may write zeros: default initialization leaves the values indeterminate, so
  zero is a permitted outcome and the whole range becomes one `memset`.
  Value-initialization of a trivially default constructible type
  zero-initializes, so the same fast path is exact there.
- Non-trivial types use a placement-new loop guarded by
  `uninitialized_guard`, so a throwing constructor destroys the prefix.
- Contiguous iterators decay to pointers. `_n` range variants construct the
  first `count` elements of the range.
- `uninitialized_default_construct_index` and
  `uninitialized_value_construct_index` bound-check a random-access range and
  construct `[firstidx, lastidx)`.

## `destroy`

- API: `destroy_at(T*)`, `destroy_iters` / `destroy`, `destroy_n_iters` /
  `destroy_n`, plus `destroy_index`.
- A trivially destructible type is a strict no-op: no destructor call and no
  loop body. `destroy_iters` returns `last` directly when the iterator and
  sentinel are the same type, and advances with iterator arithmetic when the
  range is random access and sized.
- `destroy_n_iters` calls `advance_iters` for a trivially destructible value
  type, so a pointer or random-access iterator advances in constant time
  instead of being walked.
- Array element types are destroyed element by element through `destroy_at`.
- `destroy_index` bound-checks `[firstidx, lastidx)` and destroys that
  subrange.
- Herbceptions forbid throwing destructors, so the destructor check is
  effectively always satisfied; it is still written explicitly.

## `relocate`

- API pairs: `uninitialized_relocate_iters` / `uninitialized_relocate`,
  `uninitialized_relocate_n_iters` / `uninitialized_relocate_n`,
  `uninitialized_relocate_backward_iters` / `uninitialized_relocate_backward`,
  `uninitialized_move_iters` / `uninitialized_move`,
  `uninitialized_move_n_iters` / `uninitialized_move_n`,
  `uninitialized_move_backward_iters` / `uninitialized_move_backward`.
- Relocate move-constructs into uninitialized storage and then destroys the
  source elements; move only constructs.
- Byte fast path: both value types satisfy
  `is_trivially_copyable_or_relocatable_v` and either the same value type or
  both integral/`std::byte` with equal size. Uses `my_memmove`, so overlapping
  in-place shifts are handled.
- Generic path constructs through `uninitialized_guard` and destroys the source
  element after each successful construction.
- Container-specific `uninitialized_relocate_define` /
  `uninitialized_relocate_backward_define` customization points are deferred
  until these algorithms move into `fast_io` core.
- `throws` includes move construction and destruction (relocate has no index
  entry point).

## `heap`

- APIs: `is_heap_until_iters` / `is_heap_until`, `is_heap_iters` / `is_heap`,
  `make_heap_iters` / `make_heap`, `push_heap_iters` / `push_heap`,
  `pop_heap_iters` / `pop_heap`, `sort_heap_iters` / `sort_heap`, and the
  matching `_index` entries: `is_heap_until_index`, `is_heap_index`,
  `make_heap_index`, `push_heap_index`, `pop_heap_index`, `sort_heap_index`.
- `is_heap_until_index` returns the index of the first element that breaks the
  heap property, or `lastidx` when the subrange is a heap. The mutating index
  entries operate on `[firstidx, lastidx)`; an empty subrange is a no-op.
- `is_heap_until` returns the first iterator that breaks the heap property;
  `is_heap` is `is_heap_until(first, last) == last`.
- Random-access iterators only. The comparator is the last parameter and uses a
  defaulted template argument plus a defaulted value (`Cmp cmp = Cmp{}`), so the
  single-function-per-name rule still holds.
- `make_heap` builds the heap bottom-up; `push_heap` sifts the last element up;
  `pop_heap` swaps the root with the last element and sifts down; `sort_heap`
  repeatedly pops the root onto the shrinking tail.
- Sift-down uses the hole technique: one move construction and one move
  assignment per level, never a swap chain.
- Contiguous non-pointer iterators decay to pointers. Non-contiguous
  random-access iterators (for example `std::deque`) stay on the indexed path.
- `throws` includes the comparator invocation and the by-value comparator pass;
  the mutating algorithms additionally include element move construction and
  move assignment. `is_heap` / `is_heap_until` only compare.
- The defaulted form requires `Cmp` to be nothrow default-constructible; the
  pass-by-value contract requires nothrow copy and move construction.
- No `_index` entry point: heap algorithms are whole-range or iterator based and
  the container violation policy does not apply.

## `reverse`

- APIs: `reverse_iters` / `reverse` / `reverse_index`.
- `reverse_iters` takes a bidirectional iterator/sentinel pair; `reverse` takes
  a `bidirectional_range`; `reverse_index` reverses `[firstidx, lastidx)` of a
  random-access range and validates the indices.
- Contiguous ranges decay to pointers and reverse from both ends. A
  non-common sized sentinel is walked to the last element once and then handled
  as an iterator pair. Non-contiguous bidirectional ranges (`std::list`) swap
  in place while walking inward.
- Trivially copyable elements up to 16 bytes use a 4-element block swap at each
  end. This lets the compiler emit vector loads/stores with element shuffles
  instead of a scalar swap chain. Larger and non-trivial elements use the
  ordinary hole-free swap.
- `throws` covers the iterator increment and decrement plus element move
  construction, move assignment and destruction.
- `sort` calls `reverse`'s implementation for a fully descending input so the
  two families share one instantiation instead of each carrying a private
  reverse.

## `sort`

- APIs: `sort_iters` / `sort` / `sort_index`.
- `sort_index` bound-checks `[firstidx, lastidx)` and sorts that subrange.
- Unstable, in-place, `O(N log N)` worst case, random-access iterators only.
- Every entry point and helper is `constexpr`. During constant evaluation the
  algorithm uses insertion sort for small subranges and the Lomuto partition
  for every element size: the fast small sorts need uninitialized scratch and
  the Hoare partition stores a gap element in a byte buffer, and neither can be
  expressed in a constant expression because obtaining the typed pointer needs
  `reinterpret_cast`. Those paths are guarded with `if consteval`, so runtime
  codegen is unchanged.
- Ported from ipnsort (sort-research-rs) by Lukas Bergdoll and Orson Peters.
- Entry: return for `N < 2`; insertion sort for `N <= 20`; detect a fully
  ascending or descending run; otherwise quicksort with a depth limit of
  `2 * floor(log2(N))` and a heapsort fallback. The limit is computed as
  `2 * (::std::bit_width(N) - 1)` from `<bit>`; `impl.h` includes `<bit>` since
  family headers carry no includes.
- Pivot selection is the glidesort adaptive pseudomedian sampler (median of 3
  below 64 elements, recursive sampling of roughly `N^0.528` elements above).
- Quicksort recurses into the left side and loops on the right. The ancestor
  pivot of the parent partition is tracked. When the chosen pivot is equal to
  the ancestor, the slice is split with an equal-element partition and only the
  strictly greater side is sorted. This gives `O(N log K)` comparisons for `K`
  distinct values.
- Partition: branchless cyclic Lomuto for `sizeof(T) <= 96`, branchy cyclic
  Hoare for larger elements. Both use one moving gap instead of a swap chain.
- The Lomuto loop is unrolled by two for element sizes up to 16 bytes, giving
  two independent comparison chains per iteration (as in ipnsort).
- The pivot is passed to the partition helpers as a `T const *__restrict`
  parameter. The partition never writes index 0, so the promise is valid and
  the compiler keeps the pivot in a register. A local `__restrict` pointer or
  a reference is not sufficient: the attribute has to be on a function
  parameter to survive inlining as `noalias`.
- The run-detection scan is unrolled by two for element sizes above 8 bytes.
  For 8-byte elements the single-element form is faster in the full sort; the
  unrolled form regresses the fully ascending case there.
- A fully descending run is reversed through `reverse.h`: `reverse_ptr` for
  the pointer iterator case and `reverse_iters_impl` for non-contiguous
  random-access iterators. This reuses the reverse family's 4-element block
  loop, which LLVM turns into element shuffles, and keeps a second reverse out
  of `sort.h`.
- Small sorts: sorting networks plus bidirectional merge for trivially
  copyable elements up to 8 bytes; the same merge scheme with 4/8 element
  stable networks for trivially copyable elements up to 85 bytes; insertion
  sort otherwise. The two fast paths are in-place (fixed stack scratch, at
  most 4 KiB) and never allocate.
- The fallback heapsort interleaves heap construction and extraction and uses
  the branchless child selection from ipnsort.
- Comparators are taken by value. Every comparison goes through
  `cmp.compare(a, b)` and a three-way test such as `< 0`.
- Equality checks use `cmp.equal(a, b)` when the comparator provides it, and
  fall back to `cmp.compare(a, b) == 0` otherwise. The ancestor-pivot check is
  the equality site in the algorithm. `compare_three_way` provides `equal`.
- Contiguous non-pointer iterators decay to pointers. Non-contiguous
  random-access iterators (for example `std::deque`) use the generic indexed
  path.
- `throws` includes the by-value comparator pass, `compare`, `equal` when used,
  move construction/assignment/destruction, and the iterator `++` gate.
- A broken strict weak ordering detected by the merge invariant traps with
  `__builtin_trap()`.
- The element copy paths are restricted to trivially copyable *and* copy
  constructible/assignable types. A move-only type can still be trivially
  copyable in the standard sense, so the check must include copyability, not
  only `is_trivially_copyable_v`.

Benchmarks live in `benchmarks/`:

- `bench_sort.cc` compares `fast_io::algorithms::sort` with libc++ `std::sort`
  using the same comparator, pattern, size and repetition policy.
- `rust_sort/` compares `ipnsort::sort` with `slice::sort_unstable` on the same
  generated data. Both harnesses use the same xorshift generator and the same
  pattern definitions, so rows can be matched across languages.
- Patterns: ascending, descending, random, d20, p5 (95% zero), s95 (95%
  ascending followed by random values).
- Reported numbers are the best of several repetitions of nanoseconds per
  element. The work buffer is reset outside the timed region.

Reference results on the development machine (`-O3 -march=native`, 1e6 `u64`
random): `fast_io::algorithms::sort` ~17.5 ns/element, Rust `ipnsort`
~17.4 ns/element, libc++ `std::sort` ~56.6 ns/element. For 16-byte records at
1e5 random: ~20 ns/element versus Rust ipnsort ~18.7 ns/element and libc++
`std::sort` ~50 ns/element. The geometric mean across the full pattern and
size matrix is about parity (~0.99x the time of ipnsort); the C++ port is
faster on descending and large-element inputs, and within a few percent on
random records.

## Comparator design

### Contract

A comparator is an object with a named member:

- `cmp.compare(a, b)`

It returns a comparison category from `<=>`.

Algorithms only use:

- `cmp.compare(a, b) < 0`
- `cmp.compare(a, b) == 0`

A comparator may additionally provide:

- `cmp.equal(a, b)`

`equal` is an optional equality fast path. When it exists, equality checks
prefer it; otherwise they fall back to `cmp.compare(a, b) == 0`. The default
comparator provides `equal`. The algorithm's `throws` condition includes
`equal` when it is the selected path.

The default comparator is `::fast_io::algorithms::compare_three_way` from
`compare.h`, a named empty type built on fast_io’s own `compare_three_way`,
because the standard one is not marked for herbceptions.

Comparators are taken by value.

Ordering itself may throw:

- forming the ordering through `<=>`
- invoking `cmp.compare`
- applying `< 0` or `== 0` to a user-provided ordering type

All of those belong in the `throws(...)` condition. Do not assume an ordering
type is non-throwing just because the standard ordering categories are.

### No `operator()` in the algorithm core

Never instantiate an algorithm body on a closure type.

- Public function sees the user callable.
- If it has `.compare`, use it directly.
- Otherwise it is a lambda-like callable and is erased.

This is one function with internal `if constexpr`, not two overloads.

### Erasure

Erased comparator is a small value object containing:

- an optional context pointer for captures
- a function pointer / trampoline
- a `compare` member that calls the trampoline

Captureless lambda: null context, plain function pointer.
Capturing lambda: context pointer plus trampoline; no allocation.

The trampoline signature uses `void const*` or byte pointers. The heavy
algorithm is instantiated on the erased comparator type, not on the closure
type, so two identical lambdas produce one heavy instantiation.

### `throws` through erasure

The trampoline and the erased comparator carry a compile-time may-throw bit.
An erased comparator that may throw and one that may not are distinct types, so
the heavy core can be declared with `throws(false)` or `throws(...)` correctly
without per-lambda duplication.

- Public adapter spec is computed from the original callable.
- Heavy core spec is computed from the erased comparator.
- `throws(false)` is the spelling used for the non-throwing instantiations.

### Named comparators

A named comparator with a real `.compare` is already a deliberate type and may
bypass erasure. This is not a duplication problem because the type was chosen
explicitly. The internal dispatch still happens inside one function.

### Partial ordering

Algorithms that require a strict weak ordering cannot accept incomparable
elements. Either reject with a concept or detect and trap. Do not silently
pretend incomparable elements are equivalent.

### Projections

Do not add a separate `std::ranges`-style projector parameter to the algorithm
signatures.

Reasons:

- `cmp.compare(a, b)` already receives the whole elements, so the comparator can
  extract whatever key it wants. A projector is redundant for the core design.
- Every extra callable parameter is another template axis and another source of
  duplicate instantiations. A projector parameter would multiply the problem
  the comparator erasure is meant to solve.
- A lambda projector is exactly the closure-type bloat we are removing.
- `throws` stays simpler when there is only one user callable in the core.

If projection ergonomics are needed later, the projection belongs inside a
comparator adapter:

- member pointers (`&T::field`) or similar canonical values
- a named comparator object with a real `.compare`
- the same erasure/canonicalization rules as any other comparator

It must not become an extra algorithm parameter.

### `compare_by`

Chosen design: `compare_by<Projection, Comparator = compare_three_way>`.

It is an ordinary comparator type:

- stores `Projection` and `Comparator` by value
- exposes `compare(a, b)`
- evaluates `cmp.compare(proj(a), proj(b))`
- default `Comparator` is fast_io’s three-way comparator

Class template argument deduction keeps the call site short:

- `sort(r, compare_by{&T::field})`
- `sort(r, compare_by{&T::field, compare_three_way{}})`
- `sort(r, compare_by{&T::field, compare_descending{}})`

The algorithm still sees exactly one comparator argument with `.compare`. No
projector template axis is added to the algorithm. `compare_by` itself is the
projector use case, expressed inside the comparator concept.

`throws` composition is natural:

- projection invocation
- comparator invocation
- ordering formation and comparison

are all included in `compare_by::compare`’s `throws` condition.

Canonicalization rules:

- member pointers and named projection types are already canonical values
- captureless lambda projections can be erased to a function pointer
- capturing/generic projections go through the same erased-callable machinery
- if the projection and comparator are erased together, the heavy algorithm
  core is still instantiated once per erased comparator type, not per lambda

Trade-off: a compile-time member pointer can be applied directly without an
indirect call, but then the algorithm core is instantiated per projection type.
Erasing the projection removes those instantiations at the cost of an indirect
call. `compare_by` follows the same policy as the comparator itself: named
projections stay static, lambdas get erased.

Multi-key ordering is not a separate algorithm parameter. If needed, it is a
projection that produces a tuple-like key, or a dedicated named comparator with
its own `.compare`.

## Element-type deduplication

`int` and `long` are separate template instantiations even when they are
layout-compatible and the comparator semantics are the same.

Two levels:

### Comparator-level

Erase the comparator to a signature over `void const*` / byte pointers. This
shares the comparator call shape across element types, but the algorithm body
still depends on `T` for moves and swaps.

### Layout-level, trivial types only

For trivially copyable or trivially relocatable element types, the algorithm
body can operate on:

- `std::byte*`
- element count
- runtime `sizeof(T)`
- runtime `alignof(T)`
- erased comparator over `void const*`

Then `int` and `long` with the same size/alignment share one algorithm body.
Only the per-type comparator thunk differs.

Restrictions:

- valid only for trivially copyable / trivially relocatable types
- object lifetime rules still apply
- alignment must be respected
- non-trivial types must stay on the typed path
- comparison still needs a per-type thunk

The intended split:

- trivial/relocatable elements: byte-based shared core
- non-trivial elements: typed core

This is the same philosophy as the shared index checker: one runtime body, type
information passed as data.

## Failure model

Three distinct tiers:

1. Caller contract violation
   - `_index` APIs only
   - allocator flag selects throw vs terminate
   - no flag means terminate

2. User operation failure
   - iterator operations
   - comparator `compare`
   - callback `f`
   - element move / swap / assignment
   - propagated and reflected in `throws`

3. Unrecoverable internal failure
   - comparator is not a strict weak ordering
   - algorithm invariant broken
   - an operation failure would leave the algorithm unable to restore its
     invariants
   - `__builtin_trap()`

`__builtin_trap()` is not for ordinary out-of-range access.

## `throws` rules

Every public function uses `FAST_IO_HERBCEPTIONS_THROWS_IF(cond)`. Under
herbceptions that expands to `throws(cond)`; `throws(false)` means non-throwing.
The `noexcept(!(cond))` form exists only as the fallback for compilers without
herbceptions.

- Never use standard algorithms that are not marked for herbceptions as part of
  a throwing path.
- Include:
  - violation policy
  - iterator operations actually used
  - callback on `iter_reference_t<Iter>`
  - comparator `compare` invocation
  - ordering formation (`<=>`)
  - ordering comparison (`< 0`, `== 0`)
  - projection invocation through `compare_by`
  - move / swap / assignment inside the algorithm

## Defects in the original `algorithms.h`

- Full-width commas `，` on lines 21 and 29.
- Raw `throws(...)` in several signatures; must use the fast_io macros.
- `for_each_iters` decay recursion omits `f`.
- `for_each_iters` calls `f(first)` instead of `f(*first)`.
- `for_each_iters` calls `to_address(last)` without requiring `Snt` to be a
  contiguous iterator.
- `for_each_index` omits `firstidx` and `lastidx` when calling
  `algo_index_to_iters`.
- `algo_index_to_iters` hardcodes `throwing_violations = false`.
- `algo_index_to_iters` assumes `ranges::data(r)` is a pointer.
- Non-contiguous branch duplicates the bounds check per iterator type.
- `bit_cast` layout assumptions are not asserted.
- Header has no includes.
- `violation_report` currently lives in `fast_io_dsal`, which is a
  layering problem once this header moves into core.

## Remaining decisions

1. Comparator erased-signature shape and whether named comparators bypass
   erasure.
2. Whether the trivial-element byte core is part of the first implementation.
3. Core-level home for the contract-violation reporter.
4. Return values for the three public functions (`void` or final state).
5. Exposing `allocator_flags` on the fast_io containers themselves.

## Implementation order

1. Fix syntax, includes, and `throws` macros.
2. Add allocator-flag lookup and wire it into `for_each_index` /
   `algo_index_to_iters`.
3. Correct the index path and the shared byte checker.
4. Correct `for_each_iters` decay, callback semantics, and iterator `throws`.
5. Add the comparator adapter and erased comparator.
6. Add the trivial-element byte core.

## Open dialogs (processed / dropped)

- No `noinline`; the dedup story is source-level instantiation identity, not
  forced call emission.
- No renamed APIs and no `_unchecked` variants.
- No overload sets.
- Lambda bloat is handled by type-erasing callables at the boundary.
