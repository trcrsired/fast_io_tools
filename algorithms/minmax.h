#pragma once

namespace fast_io::details::algorithms
{

template<typename Cmp, typename T>
concept minmax_compare_no_throw =
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  requires(Cmp cmp, T const &a, T const &b)
{
  { 0 > cmp.compare(a, b) } noexcept;
};

template<typename Iter, typename Cmp>
concept minmax_element_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  requires(Cmp cmp, Iter &a, Iter &b)
{
  { 0 > cmp.compare(*a, *b) } noexcept;
};

template<typename Iter, typename Snt, typename Cmp>
concept minmax_element_sentinel_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::minmax_element_iters_no_throw<Iter, Cmp>;

template<typename R, typename Cmp>
concept minmax_element_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept minmax_element_index_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
inline constexpr Iter min_element_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
      Iter, Snt, Cmp>)
{
  if (first == last)
  {
    return first;
  }
  Iter result{first};
  ++first;
  for (; first != last; ++first)
  {
    if (0 > cmp.compare(*first, *result))
    {
      result = first;
    }
  }
  return result;
}

template<typename Iter, typename Snt, typename Cmp>
inline constexpr Iter max_element_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
      Iter, Snt, Cmp>)
{
  if (first == last)
  {
    return first;
  }
  Iter result{first};
  ++first;
  for (; first != last; ++first)
  {
    if (0 > cmp.compare(*result, *first))
    {
      result = first;
    }
  }
  return result;
}

template<typename Iter, typename Snt, typename Cmp>
inline constexpr ::std::pair<Iter, Iter> minmax_element_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
      Iter, Snt, Cmp>)
{
  if (first == last)
  {
    return {first, first};
  }
  Iter min{first};
  Iter max{first};
  ++first;
  for (; first != last; ++first)
  {
    if (0 > cmp.compare(*first, *min))
    {
      min = first;
    }
    if (!(0 > cmp.compare(*first, *max)))
    {
      max = first;
    }
  }
  return {min, max};
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr T const &min(T const &a, T const &b, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_compare_no_throw<Cmp, T>)
{
  return 0 > cmp.compare(b, a) ? b : a;
}

template<typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr T const &max(T const &a, T const &b, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_compare_no_throw<Cmp, T>)
{
  return 0 > cmp.compare(a, b) ? b : a;
}

template<typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<T const &, T const &> minmax(
  T const &a, T const &b, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_compare_no_throw<Cmp, T>)
{
  if (0 > cmp.compare(b, a))
  {
    return {b, a};
  }
  return {a, b};
}

template<typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr T const &clamp(
  T const &value, T const &lo, T const &hi, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_compare_no_throw<Cmp, T>)
{
  if (0 > cmp.compare(hi, lo)) [[unlikely]]
  {
    __builtin_trap();
  }
  if (0 > cmp.compare(value, lo))
  {
    return lo;
  }
  if (0 > cmp.compare(hi, value))
  {
    return hi;
  }
  return value;
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr Iter min_element_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
      Iter, Snt, Cmp>)
{
  return ::fast_io::details::algorithms::min_element_iters_impl(
    first, last, cmp);
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::ranges::iterator_t<R> min_element(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_ranges_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::min_element_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t min_element_index(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_index_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::min_element_iters(
    first, last, cmp)};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr Iter max_element_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
      Iter, Snt, Cmp>)
{
  return ::fast_io::details::algorithms::max_element_iters_impl(
    first, last, cmp);
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::ranges::iterator_t<R> max_element(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_ranges_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::max_element_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t max_element_index(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_index_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::max_element_iters(
    first, last, cmp)};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<Iter, Iter> minmax_element_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_sentinel_no_throw<
      Iter, Snt, Cmp>)
{
  return ::fast_io::details::algorithms::minmax_element_iters_impl(
    first, last, cmp);
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<
  ::std::ranges::iterator_t<R>, ::std::ranges::iterator_t<R>>
minmax_element(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_ranges_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::minmax_element_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<::std::size_t, ::std::size_t>
minmax_element_index(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::minmax_element_index_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::minmax_element_iters(
    first, last, cmp)};
  return {
    firstidx + static_cast<::std::size_t>(result.first - first),
    firstidx + static_cast<::std::size_t>(result.second - first)};
}

} // namespace fast_io::algorithms
