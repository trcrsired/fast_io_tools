#pragma once

// Unstable in-place sort, ported from ipnsort by Lukas Bergdoll and Orson
// Peters (sort-research-rs).
//
// Components:
//   - insertion sort for very small inputs
//   - ascending/descending run detection
//   - introsort-style quicksort with a recursion depth limit
//   - glidesort-style adaptive pseudomedian pivot selection
//   - branchless cyclic Lomuto partition for small/medium elements
//   - branchy cyclic Hoare partition for large elements
//   - ancestor pivot tracking for low-cardinality inputs
//   - branchless heapsort fallback
//   - sorting networks + bidirectional merge small sort for small elements
//   - merge-based small sort for medium trivially copyable elements
//
// Elements must be movable. The fast small sorts are used for trivially
// copyable elements only; everything else falls back to insertion sort for
// small subranges.

namespace fast_io::details::algorithms
{

template<typename Cmp>
concept sort_comparator_pass_no_throw =
  ::std::is_nothrow_copy_constructible_v<Cmp> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::std::is_nothrow_default_constructible_v<Cmp>;

template<typename Cmp, typename T>
concept sort_compare_no_throw =
  requires(Cmp cmp, T const &a, T const &b)
{
  { cmp.compare(a, b) < 0 } noexcept;
  { cmp.compare(b, a) < 0 } noexcept;
};

// Comparators may provide `equal(a, b)` as a cheaper equality test. When it is
// absent, equality falls back to `compare(a, b) == 0`.
template<typename Cmp, typename T>
inline constexpr bool sort_equal_no_throw_v = []() constexpr
{
  if constexpr (requires(Cmp cmp, T const &a, T const &b) { cmp.equal(a, b); })
  {
    return ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
           requires(Cmp cmp, T const &a, T const &b)
    {
      { cmp.equal(a, b) } noexcept;
    };
  }
  else
  {
    return ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
           requires(Cmp cmp, T const &a, T const &b)
    {
      { cmp.compare(a, b) == 0 } noexcept;
    };
  }
}();

template<typename Cmp, typename T>
inline constexpr bool sort_elements_equal(Cmp cmp, T const &a, T const &b)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_equal_no_throw_v<Cmp, T>)
{
  if constexpr (requires { cmp.equal(a, b); })
  {
    return cmp.equal(a, b);
  }
  else
  {
    return cmp.compare(a, b) == 0;
  }
}

template<typename Cmp, typename T>
concept sort_ops_no_throw =
  ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
  ::fast_io::details::algorithms::move_no_throw<T>;

template<typename Cmp, typename T>
concept sort_copy_ops_no_throw =
  ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
  ::std::is_nothrow_copy_constructible_v<T> &&
  ::std::is_nothrow_copy_assignable_v<T> &&
  ::std::is_nothrow_destructible_v<T>;

template<typename Iter, typename Cmp>
concept sort_ops_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_ops_no_throw<
    Cmp, ::std::iter_value_t<Iter>>;

template<typename Iter, typename Cmp>
concept sort_copy_ops_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_copy_ops_no_throw<
    Cmp, ::std::iter_value_t<Iter>>;

template<typename Iter, typename Cmp>
inline constexpr bool sort_equal_iter_no_throw_v{
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_equal_no_throw_v<
    Cmp, ::std::iter_value_t<Iter>>};

template<typename Iter, typename Cmp>
concept sort_iters_no_throw =
  ::std::random_access_iterator<Iter> &&
  ::fast_io::details::algorithms::decrement_iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_compare_no_throw<
    Cmp, ::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::sort_equal_no_throw_v<
    Cmp, ::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::move_no_throw<
    ::std::iter_value_t<Iter>>;

template<typename R, typename Cmp>
concept sort_index_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::sort_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp> &&
  ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>;

template<typename T>
inline constexpr bool sort_network_small_v{
  ::std::is_trivially_copyable_v<T> &&
  ::std::is_nothrow_copy_constructible_v<T> &&
  ::std::is_nothrow_copy_assignable_v<T> &&
  sizeof(T) <= sizeof(::std::uint64_t) &&
  sizeof(T) * 32 <= 4096};

template<typename T>
inline constexpr bool sort_general_small_v{
  !::fast_io::details::algorithms::sort_network_small_v<T> &&
  ::std::is_trivially_copyable_v<T> &&
  ::std::is_nothrow_copy_constructible_v<T> &&
  ::std::is_nothrow_copy_assignable_v<T> &&
  sizeof(T) * 48 <= 4096};

template<typename T>
inline constexpr ::std::ptrdiff_t sort_small_sort_threshold_v{
  (::fast_io::details::algorithms::sort_network_small_v<T> ||
   ::fast_io::details::algorithms::sort_general_small_v<T>)
    ? 32
    : 16};

template<typename Cmp, typename T>
inline constexpr bool sort_small_ops_no_throw = []() constexpr
{
  if constexpr (::fast_io::details::algorithms::sort_network_small_v<T> ||
                ::fast_io::details::algorithms::sort_general_small_v<T>)
  {
    return ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
           ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
           ::std::is_nothrow_copy_constructible_v<T> &&
           ::std::is_nothrow_copy_assignable_v<T> &&
           ::std::is_nothrow_destructible_v<T>;
  }
  else
  {
    return ::fast_io::details::algorithms::sort_ops_no_throw<Cmp, T>;
  }
}();

template<typename T>
inline void sort_swap(T &a, T &b)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_no_throw<T>)
{
  T temp{::std::move(a)};
  a = ::std::move(b);
  b = ::std::move(temp);
}

// Insert the element at `tail` into the already sorted range
// [`begin`, `tail`), using the hole technique.
template<typename Iter, typename Cmp>
inline void sort_insert_tail(
  Iter base, ::std::iter_difference_t<Iter> begin,
  ::std::iter_difference_t<Iter> tail, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  difference_type sift{static_cast<difference_type>(tail - 1)};
  if (!(cmp.compare(base[tail], base[sift]) < 0))
  {
    return;
  }
  value_type temp{::std::move(base[tail])};
  difference_type gap{tail};
  for (;;)
  {
    base[gap] = ::std::move(base[sift]);
    gap = sift;
    if (sift == begin)
    {
      break;
    }
    --sift;
    if (!(cmp.compare(temp, base[sift]) < 0))
    {
      break;
    }
  }
  base[gap] = ::std::move(temp);
}

// Sort `v[0, len)` assuming `v[0, offset)` is already sorted.
template<typename Iter, typename Cmp>
inline void sort_insertion_sort_shift_left(
  Iter v, ::std::iter_difference_t<Iter> len,
  ::std::iter_difference_t<Iter> offset, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  for (difference_type tail{offset}; tail < len; ++tail)
  {
    difference_type sift{static_cast<difference_type>(tail - 1)};
    if (!(cmp.compare(v[tail], v[sift]) < 0))
    {
      continue;
    }
    value_type temp{::std::move(v[tail])};
    difference_type gap{tail};
    for (;;)
    {
      v[gap] = ::std::move(v[sift]);
      gap = sift;
      if (sift == 0)
      {
        break;
      }
      --sift;
      if (!(cmp.compare(temp, v[sift]) < 0))
      {
        break;
      }
    }
    v[gap] = ::std::move(temp);
  }
}

template<typename Iter, typename Cmp>
inline void sort_swap_if_less(
  Iter v, ::std::iter_difference_t<Iter> a,
  ::std::iter_difference_t<Iter> b, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  using value_type = ::std::iter_value_t<Iter>;
  static_assert(::std::is_trivially_copyable_v<value_type>);
  value_type temp_a{v[a]};
  value_type temp_b{v[b]};
  bool const swap{cmp.compare(temp_b, temp_a) < 0};
  v[a] = swap ? temp_b : temp_a;
  v[b] = swap ? temp_a : temp_b;
}

// Optimal sorting network for 9 elements.
template<typename Iter, typename Cmp>
inline void sort9_optimal(Iter v, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 4, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 4, 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 7, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 1, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 6, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 4, 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 6, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 6, cmp);
}

// Optimal sorting network for 13 elements.
template<typename Iter, typename Cmp>
inline void sort13_optimal(Iter v, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 12, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 6, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 4, 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 7, 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 8, 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 7, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 9, 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 11, 12, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 4, 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 8, 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 10, 12, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 4, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 6, 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 9, 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 0, 1, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 6, 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 7, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 10, 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 9, 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 1, 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 6, 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 2, 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 4, 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 6, 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 8, 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 3, 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v, 5, 6, cmp);
}

template<typename T>
struct sort_construct_dest
{
  T *first;
  template<typename I>
  inline void write(I i, T const &value) const
    noexcept(::std::is_nothrow_copy_constructible_v<T>)
  {
    ::new (static_cast<void *>(first + i)) T(value);
  }
};

template<typename Iter>
struct sort_assign_dest
{
  Iter first;
  template<typename I>
  inline void write(I i, ::std::iter_value_t<Iter> const &value) const
    noexcept(noexcept(first[i] = value))
  {
    first[i] = value;
  }
};

template<typename Dest, typename T>
concept sort_dest_write_no_throw =
  requires(Dest const &dest, ::std::ptrdiff_t i, T const &value)
{
  { dest.write(i, value) } noexcept;
};

// Merge two adjacent sorted runs using a merge from both ends. Each source
// element is read exactly once, and each destination element is written
// exactly once. The order invariant is checked at the end; a broken strict
// weak ordering traps.
template<typename Src, typename Dest, typename Cmp>
inline void sort_bidirectional_merge(
  Src src, ::std::iter_difference_t<Src> len, Dest dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::iter_no_throw<Src> &&
      ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
      ::fast_io::details::algorithms::sort_compare_no_throw<
        Cmp, ::std::iter_value_t<Src>> &&
      ::fast_io::details::algorithms::sort_dest_write_no_throw<
        Dest, ::std::iter_value_t<Src>>))
{
  using difference_type = ::std::iter_difference_t<Src>;
  if (len < 2)
  {
    if (len == 1)
    {
      dest.write(static_cast<::std::ptrdiff_t>(0), src[0]);
    }
    return;
  }
  difference_type const len_div_2{static_cast<difference_type>(len / 2)};
  difference_type left{0};
  difference_type right{len_div_2};
  difference_type dst{0};
  difference_type left_rev{static_cast<difference_type>(len_div_2 - 1)};
  difference_type right_rev{static_cast<difference_type>(len - 1)};
  difference_type dst_rev{static_cast<difference_type>(len - 1)};
  for (difference_type i{0}; i != len_div_2; ++i)
  {
    bool const take_left{!(cmp.compare(src[right], src[left]) < 0)};
    if (take_left)
    {
      dest.write(static_cast<::std::ptrdiff_t>(dst), src[left]);
      ++left;
    }
    else
    {
      dest.write(static_cast<::std::ptrdiff_t>(dst), src[right]);
      ++right;
    }
    ++dst;

    bool const take_right_rev{!(cmp.compare(src[right_rev], src[left_rev]) < 0)};
    if (take_right_rev)
    {
      dest.write(static_cast<::std::ptrdiff_t>(dst_rev), src[right_rev]);
      --right_rev;
    }
    else
    {
      dest.write(static_cast<::std::ptrdiff_t>(dst_rev), src[left_rev]);
      --left_rev;
    }
    --dst_rev;
  }
  difference_type const left_end{static_cast<difference_type>(left_rev + 1)};
  difference_type const right_end{static_cast<difference_type>(right_rev + 1)};
  if (len % 2 != 0)
  {
    bool const left_nonempty{left < left_end};
    if (left_nonempty)
    {
      dest.write(static_cast<::std::ptrdiff_t>(dst), src[left]);
      ++left;
    }
    else
    {
      dest.write(static_cast<::std::ptrdiff_t>(dst), src[right]);
      ++right;
    }
  }
  if (left != left_end || right != right_end) [[unlikely]]
  {
    __builtin_trap();
  }
}

template<typename T, ::std::size_t N>
struct sort_scratch
{
  alignas(T) ::std::byte storage[sizeof(T) * N];
  inline T *data() noexcept
  {
    return reinterpret_cast<T *>(storage);
  }
};

// Stable sort of 4 elements into uninitialized `dst`.
template<typename Iter, typename Cmp>
inline void sort4_stable(
  Iter v, ::std::iter_value_t<Iter> *dst, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  bool const c1{cmp.compare(v[1], v[0]) < 0};
  bool const c2{cmp.compare(v[3], v[2]) < 0};
  difference_type const a{c1 ? 1 : 0};
  difference_type const b{c1 ? 0 : 1};
  difference_type const c{static_cast<difference_type>(2 + (c2 ? 1 : 0))};
  difference_type const d{static_cast<difference_type>(2 + (c2 ? 0 : 1))};
  bool const c3{cmp.compare(v[c], v[a]) < 0};
  bool const c4{cmp.compare(v[d], v[b]) < 0};
  difference_type const min{c3 ? c : a};
  difference_type const max{c4 ? b : d};
  difference_type const unknown_left{c3 ? a : (c4 ? c : b)};
  difference_type const unknown_right{c4 ? d : (c3 ? b : c)};
  bool const c5{cmp.compare(v[unknown_right], v[unknown_left]) < 0};
  difference_type const lo{c5 ? unknown_right : unknown_left};
  difference_type const hi{c5 ? unknown_left : unknown_right};
  ::new (static_cast<void *>(dst + 0)) value_type(v[min]);
  ::new (static_cast<void *>(dst + 1)) value_type(v[lo]);
  ::new (static_cast<void *>(dst + 2)) value_type(v[hi]);
  ::new (static_cast<void *>(dst + 3)) value_type(v[max]);
}

// Stable sort of 8 elements into uninitialized `dst`, using `temp` for 8
// intermediate elements.
template<typename Iter, typename Cmp>
inline void sort8_stable(
  Iter v, ::std::iter_value_t<Iter> *dst,
  ::std::iter_value_t<Iter> *temp, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  using value_type = ::std::iter_value_t<Iter>;
  ::fast_io::details::algorithms::sort4_stable(v, temp, cmp);
  ::fast_io::details::algorithms::sort4_stable(v + 4, temp + 4, cmp);
  ::fast_io::details::algorithms::sort_bidirectional_merge(
    temp, 8,
    ::fast_io::details::algorithms::sort_construct_dest<value_type>{dst}, cmp);
}

template<typename Iter, typename Cmp>
inline void sort_small_sort_fallback(
  Iter v, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  if (len >= 2)
  {
    ::fast_io::details::algorithms::sort_insertion_sort_shift_left(v, len, 1, cmp);
  }
}

template<typename Iter, typename Cmp>
inline void sort_small_sort_general(
  Iter v, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  if (len < 2)
  {
    return;
  }
  static_assert(::fast_io::details::algorithms::sort_general_small_v<value_type>);
  ::fast_io::details::algorithms::sort_scratch<value_type, 48> scratch;
  value_type *s{scratch.data()};
  difference_type const len_div_2{static_cast<difference_type>(len / 2)};
  difference_type presorted_len;
  if constexpr (sizeof(value_type) <= 16)
  {
    if (len >= 16)
    {
      ::fast_io::details::algorithms::sort8_stable(v, s, s + len, cmp);
      ::fast_io::details::algorithms::sort8_stable(
        v + len_div_2, s + len_div_2, s + len + 8, cmp);
      presorted_len = 8;
    }
    else if (len >= 8)
    {
      ::fast_io::details::algorithms::sort4_stable(v, s, cmp);
      ::fast_io::details::algorithms::sort4_stable(v + len_div_2, s + len_div_2, cmp);
      presorted_len = 4;
    }
    else
    {
      ::new (static_cast<void *>(s)) value_type(v[0]);
      ::new (static_cast<void *>(s + len_div_2)) value_type(v[len_div_2]);
      presorted_len = 1;
    }
  }
  else
  {
    if (len >= 8)
    {
      ::fast_io::details::algorithms::sort4_stable(v, s, cmp);
      ::fast_io::details::algorithms::sort4_stable(v + len_div_2, s + len_div_2, cmp);
      presorted_len = 4;
    }
    else
    {
      ::new (static_cast<void *>(s)) value_type(v[0]);
      ::new (static_cast<void *>(s + len_div_2)) value_type(v[len_div_2]);
      presorted_len = 1;
    }
  }
  for (difference_type k{0}; k != 2; ++k)
  {
    difference_type const offset{k != 0 ? len_div_2 : 0};
    difference_type const desired_len{
      k != 0 ? static_cast<difference_type>(len - len_div_2) : len_div_2};
    for (difference_type i{presorted_len}; i != desired_len; ++i)
    {
      ::new (static_cast<void *>(s + offset + i)) value_type(v[offset + i]);
      ::fast_io::details::algorithms::sort_insert_tail(
        s, offset, static_cast<difference_type>(offset + i), cmp);
    }
  }
  ::fast_io::details::algorithms::sort_bidirectional_merge(
    s, len, ::fast_io::details::algorithms::sort_assign_dest<Iter>{v}, cmp);
}

template<typename Iter, typename Cmp>
inline void sort_small_sort_network(
  Iter v, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  if (len < 2)
  {
    return;
  }
  static_assert(::fast_io::details::algorithms::sort_network_small_v<value_type>);
  difference_type const len_div_2{static_cast<difference_type>(len / 2)};
  bool const no_merge{len < 18};
  difference_type region_start{0};
  difference_type region_len{no_merge ? len : len_div_2};
  for (;;)
  {
    difference_type presorted_len;
    if (region_len >= 13)
    {
      ::fast_io::details::algorithms::sort13_optimal(v + region_start, cmp);
      presorted_len = 13;
    }
    else if (region_len >= 9)
    {
      ::fast_io::details::algorithms::sort9_optimal(v + region_start, cmp);
      presorted_len = 9;
    }
    else
    {
      presorted_len = 1;
    }
    ::fast_io::details::algorithms::sort_insertion_sort_shift_left(
      v + region_start, region_len, presorted_len, cmp);
    if (no_merge)
    {
      return;
    }
    if (region_start != 0)
    {
      break;
    }
    region_start = len_div_2;
    region_len = static_cast<difference_type>(len - len_div_2);
  }
  ::fast_io::details::algorithms::sort_scratch<value_type, 32> scratch;
  value_type *s{scratch.data()};
  ::fast_io::details::algorithms::sort_bidirectional_merge(
    v, len, ::fast_io::details::algorithms::sort_construct_dest<value_type>{s}, cmp);
  for (difference_type i{0}; i != len; ++i)
  {
    v[i] = s[i];
  }
}

template<typename Iter, typename Cmp>
inline void sort_small_sort(
  Iter v, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::iter_no_throw<Iter> &&
      ::fast_io::details::algorithms::sort_small_ops_no_throw<
        Cmp, ::std::iter_value_t<Iter>>))
{
  using value_type = ::std::iter_value_t<Iter>;
  if constexpr (::fast_io::details::algorithms::sort_network_small_v<value_type>)
  {
    ::fast_io::details::algorithms::sort_small_sort_network(v, len, cmp);
  }
  else if constexpr (::fast_io::details::algorithms::sort_general_small_v<value_type>)
  {
    ::fast_io::details::algorithms::sort_small_sort_general(v, len, cmp);
  }
  else
  {
    ::fast_io::details::algorithms::sort_small_sort_fallback(v, len, cmp);
  }
}

template<typename Iter, typename Cmp>
inline void sort_heapsort_sift_down(
  Iter first, ::std::iter_difference_t<Iter> n,
  ::std::iter_difference_t<Iter> node, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  for (;;)
  {
    difference_type child{static_cast<difference_type>((node << 1) + 1)};
    if (child >= n)
    {
      break;
    }
    if (child + 1 < n)
    {
      child += (cmp.compare(first[child], first[child + 1]) < 0) ? 1 : 0;
    }
    if (!(cmp.compare(first[node], first[child]) < 0))
    {
      break;
    }
    ::fast_io::details::algorithms::sort_swap(first[node], first[child]);
    node = child;
  }
}

// Branchless-heapsort style fallback, interleaving heap construction and
// extraction. Used when the quicksort depth limit is exhausted.
template<typename Iter, typename Cmp>
inline void sort_heapsort(
  Iter first, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  for (difference_type i{static_cast<difference_type>(len + len / 2)}; i != 0;)
  {
    --i;
    difference_type sift_idx;
    if (i >= len)
    {
      sift_idx = static_cast<difference_type>(i - len);
    }
    else
    {
      if (i != 0)
      {
        ::fast_io::details::algorithms::sort_swap(first[0], first[i]);
      }
      sift_idx = 0;
    }
    difference_type const sub_n{static_cast<difference_type>(i < len ? i : len)};
    ::fast_io::details::algorithms::sort_heapsort_sift_down(
      first, sub_n, sift_idx, cmp);
  }
}

template<typename Iter, typename Cmp>
inline ::std::iter_difference_t<Iter> sort_median3(
  Iter v, ::std::iter_difference_t<Iter> a,
  ::std::iter_difference_t<Iter> b, ::std::iter_difference_t<Iter> c, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  bool const x{cmp.compare(v[a], v[b]) < 0};
  bool const y{cmp.compare(v[a], v[c]) < 0};
  if (x == y)
  {
    bool const z{cmp.compare(v[b], v[c]) < 0};
    return (z != x) ? c : b;
  }
  return a;
}

template<typename Iter, typename Cmp>
inline ::std::iter_difference_t<Iter> sort_median3_rec(
  Iter v, ::std::iter_difference_t<Iter> a,
  ::std::iter_difference_t<Iter> b, ::std::iter_difference_t<Iter> c,
  ::std::iter_difference_t<Iter> n, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (n * 8 >= 64)
  {
    difference_type const n8{static_cast<difference_type>(n / 8)};
    a = ::fast_io::details::algorithms::sort_median3_rec(
      v, a, static_cast<difference_type>(a + n8 * 4),
      static_cast<difference_type>(a + n8 * 7), n8, cmp);
    b = ::fast_io::details::algorithms::sort_median3_rec(
      v, b, static_cast<difference_type>(b + n8 * 4),
      static_cast<difference_type>(b + n8 * 7), n8, cmp);
    c = ::fast_io::details::algorithms::sort_median3_rec(
      v, c, static_cast<difference_type>(c + n8 * 4),
      static_cast<difference_type>(c + n8 * 7), n8, cmp);
  }
  return ::fast_io::details::algorithms::sort_median3(v, a, b, c, cmp);
}

// Adaptive pseudomedian pivot selection from glidesort. Samples an
// approximation of the median of roughly sqrt(n) elements.
template<typename Iter, typename Cmp>
inline ::std::iter_difference_t<Iter> sort_choose_pivot(
  Iter v, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (len < 8) [[unlikely]]
  {
    __builtin_trap();
  }
  difference_type const len_div_8{static_cast<difference_type>(len / 8)};
  difference_type const a{0};
  difference_type const b{static_cast<difference_type>(len_div_8 * 4)};
  difference_type const c{static_cast<difference_type>(len_div_8 * 7)};
  if (len < 64)
  {
    return ::fast_io::details::algorithms::sort_median3(v, a, b, c, cmp);
  }
  return ::fast_io::details::algorithms::sort_median3_rec(
    v, a, b, c, len_div_8, cmp);
}

// Branchless cyclic Lomuto partition for small and medium elements.
// `equal` selects the equal-element partition: elements <= pivot go left.
template<bool equal, typename Iter, typename Cmp>
inline ::std::iter_difference_t<Iter> sort_partition_lomuto(
  Iter first, ::std::iter_difference_t<Iter> len,
  ::std::iter_value_t<Iter> const *__restrict pivot_ptr, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  difference_type const m{static_cast<difference_type>(len - 1)};
  Iter const base{first + 1};
  value_type gap_value{::std::move(base[0])};
  difference_type gap_pos{0};
  difference_type num_lt{0};
  difference_type right{1};
  // ipnsort unrolls the partition loop by two for element sizes up to 16
  // bytes. The unrolled form gives the out-of-order engine two independent
  // comparison chains per iteration.
  constexpr difference_type unroll_len{(sizeof(value_type) <= 16) ? 2 : 1};
  difference_type const unroll_end{
    static_cast<difference_type>(m - (unroll_len - 1))};
  while (right < unroll_end)
  {
    for (difference_type k{}; k != unroll_len; ++k)
    {
      bool right_is_lt;
      if constexpr (equal)
      {
        right_is_lt = !(cmp.compare(*pivot_ptr, base[right]) < 0);
      }
      else
      {
        right_is_lt = cmp.compare(base[right], *pivot_ptr) < 0;
      }
      difference_type const left{num_lt};
      if constexpr (::fast_io::details::algorithms::copy_trivial_v<value_type>)
      {
        base[gap_pos] = base[left];
        base[left] = base[right];
      }
      else
      {
        if (left != gap_pos)
        {
          base[gap_pos] = ::std::move(base[left]);
        }
        base[left] = ::std::move(base[right]);
      }
      gap_pos = right;
      num_lt += right_is_lt ? 1 : 0;
      ++right;
    }
  }
  while (right < m)
  {
    bool right_is_lt;
    if constexpr (equal)
    {
      right_is_lt = !(cmp.compare(*pivot_ptr, base[right]) < 0);
    }
    else
    {
      right_is_lt = cmp.compare(base[right], *pivot_ptr) < 0;
    }
    difference_type const left{num_lt};
    if constexpr (::fast_io::details::algorithms::copy_trivial_v<value_type>)
    {
      base[gap_pos] = base[left];
      base[left] = base[right];
    }
    else
    {
      if (left != gap_pos)
      {
        base[gap_pos] = ::std::move(base[left]);
      }
      base[left] = ::std::move(base[right]);
    }
    gap_pos = right;
    num_lt += right_is_lt ? 1 : 0;
    ++right;
  }
  bool final_is_lt;
  if constexpr (equal)
  {
    final_is_lt = !(cmp.compare(*pivot_ptr, gap_value) < 0);
  }
  else
  {
    final_is_lt = cmp.compare(gap_value, *pivot_ptr) < 0;
  }
  difference_type const left{num_lt};
  if constexpr (::fast_io::details::algorithms::copy_trivial_v<value_type>)
  {
    base[gap_pos] = base[left];
    base[left] = gap_value;
  }
  else
  {
    if (left != gap_pos)
    {
      base[gap_pos] = ::std::move(base[left]);
    }
    base[left] = ::std::move(gap_value);
  }
  num_lt += final_is_lt ? 1 : 0;
  return num_lt;
}

template<typename T>
struct sort_gap
{
  alignas(T) ::std::byte storage[sizeof(T)];
  ::std::ptrdiff_t pos{};
  bool active{};
  inline ~sort_gap() noexcept
  {
    if (active)
    {
      ptr()->~T();
    }
  }
  inline T *ptr() noexcept
  {
    return reinterpret_cast<T *>(storage);
  }
  inline void emplace(T &&value)
    noexcept(::std::is_nothrow_move_constructible_v<T>)
  {
    ::new (static_cast<void *>(ptr())) T(::std::move(value));
    active = true;
  }
};

// Branchy cyclic Hoare partition for large elements. Fewer element moves
// than pairwise swapping, which matters when copies are expensive.
template<bool equal, typename Iter, typename Cmp>
inline ::std::iter_difference_t<Iter> sort_partition_hoare(
  Iter first, ::std::iter_difference_t<Iter> len,
  ::std::iter_value_t<Iter> const *__restrict pivot_ptr, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  difference_type const m{static_cast<difference_type>(len - 1)};
  Iter const base{first + 1};
  difference_type left{0};
  difference_type right{m};
  ::fast_io::details::algorithms::sort_gap<value_type> gap;
  for (;;)
  {
    while (left < right)
    {
      bool left_is_lt;
      if constexpr (equal)
      {
        left_is_lt = !(cmp.compare(*pivot_ptr, base[left]) < 0);
      }
      else
      {
        left_is_lt = cmp.compare(base[left], *pivot_ptr) < 0;
      }
      if (!left_is_lt)
      {
        break;
      }
      ++left;
    }
    for (;;)
    {
      --right;
      if (left >= right)
      {
        break;
      }
      bool right_is_lt;
      if constexpr (equal)
      {
        right_is_lt = !(cmp.compare(*pivot_ptr, base[right]) < 0);
      }
      else
      {
        right_is_lt = cmp.compare(base[right], *pivot_ptr) < 0;
      }
      if (right_is_lt)
      {
        break;
      }
    }
    if (left >= right)
    {
      break;
    }
    if (!gap.active)
    {
      gap.emplace(::std::move(base[left]));
    }
    else if (left != gap.pos)
    {
      base[gap.pos] = ::std::move(base[left]);
    }
    gap.pos = right;
    base[left] = ::std::move(base[right]);
    ++left;
  }
  if (gap.active)
  {
    base[gap.pos] = ::std::move(*gap.ptr());
    gap.ptr()->~value_type();
    gap.active = false;
  }
  return left;
}

template<bool equal, typename Iter, typename Cmp>
inline ::std::iter_difference_t<Iter> sort_partition(
  Iter first, ::std::iter_difference_t<Iter> len,
  ::std::iter_difference_t<Iter> pivot_pos, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  if (len < 2)
  {
    return 0;
  }
  if (pivot_pos >= len) [[unlikely]]
  {
    __builtin_trap();
  }
  if (pivot_pos != 0)
  {
    ::fast_io::details::algorithms::sort_swap(first[0], first[pivot_pos]);
  }
  difference_type num_lt;
  auto const *const pivot_ptr{::std::addressof(first[0])};
  if constexpr (sizeof(value_type) <= 96)
  {
    num_lt = ::fast_io::details::algorithms::sort_partition_lomuto<equal>(
      first, len, pivot_ptr, cmp);
  }
  else
  {
    num_lt = ::fast_io::details::algorithms::sort_partition_hoare<equal>(
      first, len, pivot_ptr, cmp);
  }
  if (num_lt != 0)
  {
    ::fast_io::details::algorithms::sort_swap(first[0], first[num_lt]);
  }
  return num_lt;
}

template<typename Iter, typename Cmp>
inline void sort_quicksort(
  Iter first, ::std::iter_difference_t<Iter> len,
  Iter ancestor, bool has_ancestor,
  ::std::iter_difference_t<Iter> limit, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::sort_equal_iter_no_throw_v<Iter, Cmp>))
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  for (;;)
  {
    if (len <= ::fast_io::details::algorithms::sort_small_sort_threshold_v<value_type>)
    {
      ::fast_io::details::algorithms::sort_small_sort(first, len, cmp);
      return;
    }
    if (limit == 0)
    {
      ::fast_io::details::algorithms::sort_heapsort(first, len, cmp);
      return;
    }
    --limit;
    difference_type const pivot_pos{
      ::fast_io::details::algorithms::sort_choose_pivot(first, len, cmp)};
    if (has_ancestor &&
        ::fast_io::details::algorithms::sort_elements_equal(
          cmp, *ancestor, first[pivot_pos]))
    {
      difference_type const num_lt{
        ::fast_io::details::algorithms::sort_partition<true>(
          first, len, pivot_pos, cmp)};
      first += static_cast<difference_type>(num_lt + 1);
      len -= static_cast<difference_type>(num_lt + 1);
      has_ancestor = false;
      continue;
    }
    difference_type const num_lt{
      ::fast_io::details::algorithms::sort_partition<false>(
        first, len, pivot_pos, cmp)};
    ::fast_io::details::algorithms::sort_quicksort(
      first, num_lt, ancestor, has_ancestor, limit, cmp);
    first += static_cast<difference_type>(num_lt + 1);
    len -= static_cast<difference_type>(num_lt + 1);
    ancestor = first - 1;
    has_ancestor = true;
  }
}

struct sort_run
{
  ::std::ptrdiff_t len;
  bool reversed;
};

template<typename Iter, typename Cmp>
inline ::fast_io::details::algorithms::sort_run sort_find_existing_run(
  Iter first, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (len < 2)
  {
    return {static_cast<::std::ptrdiff_t>(len), false};
  }
  using value_type = ::std::iter_value_t<Iter>;
  difference_type run_len{2};
  bool const descending{cmp.compare(first[1], first[0]) < 0};
  if (descending)
  {
    if constexpr (sizeof(value_type) > 8)
    {
      while (run_len + 1 < len)
      {
        if (!(cmp.compare(first[run_len], first[run_len - 1]) < 0))
        {
          break;
        }
        ++run_len;
        if (!(cmp.compare(first[run_len], first[run_len - 1]) < 0))
        {
          break;
        }
        ++run_len;
      }
      if (run_len < len && cmp.compare(first[run_len], first[run_len - 1]) < 0)
      {
        ++run_len;
      }
    }
    else
    {
      while (run_len < len && cmp.compare(first[run_len], first[run_len - 1]) < 0)
      {
        ++run_len;
      }
    }
  }
  else
  {
    if constexpr (sizeof(value_type) > 8)
    {
      while (run_len + 1 < len)
      {
        if (cmp.compare(first[run_len], first[run_len - 1]) < 0)
        {
          break;
        }
        ++run_len;
        if (cmp.compare(first[run_len], first[run_len - 1]) < 0)
        {
          break;
        }
        ++run_len;
      }
      if (run_len < len && !(cmp.compare(first[run_len], first[run_len - 1]) < 0))
      {
        ++run_len;
      }
    }
    else
    {
      while (run_len < len && !(cmp.compare(first[run_len], first[run_len - 1]) < 0))
      {
        ++run_len;
      }
    }
  }
  return {static_cast<::std::ptrdiff_t>(run_len), descending};
}

inline ::std::ptrdiff_t sort_floor_log2(::std::size_t n) noexcept
{
  ::std::ptrdiff_t r{};
  while (n > 1)
  {
    n >>= 1;
    ++r;
  }
  return r;
}

template<::std::random_access_iterator Iter, typename Cmp>
inline void sort_iters_impl(Iter first, Iter last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>))
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
  {
    ::fast_io::details::algorithms::sort_iters_impl(
      ::std::to_address(first), ::std::to_address(last), cmp);
    return;
  }
  else
  {
    difference_type const len{static_cast<difference_type>(last - first)};
    if (len < 2)
    {
      return;
    }
    constexpr difference_type max_len_always_insertion_sort{20};
    if (len <= max_len_always_insertion_sort)
    {
      ::fast_io::details::algorithms::sort_insertion_sort_shift_left(
        first, len, 1, cmp);
      return;
    }
    auto const run{
      ::fast_io::details::algorithms::sort_find_existing_run(first, len, cmp)};
    if (run.len == static_cast<::std::ptrdiff_t>(len))
    {
      if (run.reversed)
      {
        if constexpr (::std::is_pointer_v<Iter>)
        {
          ::fast_io::details::algorithms::reverse_ptr(first, len);
        }
        else
        {
          ::fast_io::details::algorithms::reverse_iters_impl(
            first, first + len);
        }
      }
      return;
    }
    difference_type const limit{
      static_cast<difference_type>(
        2 * ::fast_io::details::algorithms::sort_floor_log2(
              static_cast<::std::size_t>(len)))};
    ::fast_io::details::algorithms::sort_quicksort(
      first, len, first, false, limit, cmp);
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Unstable, in-place, O(N log N) worst-case sort.
template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline void sort_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>))
{
  ::fast_io::details::algorithms::sort_iters_impl(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline void sort(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>))
{
  ::fast_io::algorithms::sort_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline void sort_index(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_index_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::sort_iters(first, last, cmp);
}

} // namespace fast_io::algorithms
