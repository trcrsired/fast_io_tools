# fast_io::algorithms — design plan

Draft lives in `fast_io_tools`; only moves into `fast_io` after the design is
settled.

Target: replace the parts of the C++ standard algorithm library that are not
usable under `-fno-exceptions` / `-fherbceptions`, while keeping binary size and
icache pressure under control.

Baseline: C++23 freestanding. `<ranges>`, `<iterator>`, `std::to_address`,
`std::bit_cast` and `if consteval` are all available and allowed.

Header layout: one algorithm family per header.

- `common.h`: shared index helpers, flag lookup, iterator/range concepts.
- `for_each.h`: the `for_each` family; includes `common.h`.
- `find.h`: the `find` family; includes `common.h`.
- `impl.h`: umbrella that includes every algorithm header.

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

## Comparator design

### Contract

A comparator is an object with a named member:

- `cmp.compare(a, b)`

It returns a comparison category from `<=>`.

Algorithms only use:

- `cmp.compare(a, b) < 0`
- `cmp.compare(a, b) == 0`

The default comparator is a named empty type built on fast_io’s own
`compare_three_way`, because the standard one is not marked for herbceptions.

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
