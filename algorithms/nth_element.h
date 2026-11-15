#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt, typename Cmp>
concept nth_element_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::sort_iters_no_throw<Iter, Cmp> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp>;

template<typename R, typename Cmp>
concept nth_element_ranges_no_throw =
  ::std::ranges::random_access_range<R> &&
  ::fast_io::details::algorithms::nth_element_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept nth_element_index_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::nth_element_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
inline constexpr void nth_element_iters_impl(
  Iter first, Iter nth, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_iters_no_throw<
      Iter, Snt, Cmp>)
{
  // A complete sort also satisfies the nth-element postcondition.
  Iter last_iter{first};
  for (; last_iter != last; ++last_iter)
  {
  }
  ::fast_io::algorithms::sort_iters(first, last_iter, cmp);
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Rearrange `[first, last)` so that the element at `nth` is the one that would
// be there after a full sort, with smaller elements before it and larger ones
// after it.
template<::std::random_access_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void nth_element_iters(
  Iter first, Iter nth, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_iters_no_throw<
      Iter, Snt, Cmp>)
{
  ::fast_io::details::algorithms::nth_element_iters_impl(
    first, nth, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void nth_element(
  R &&r, ::std::ranges::iterator_t<R> nth, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_ranges_no_throw<R, Cmp>)
{
  ::fast_io::algorithms::nth_element_iters(
    ::std::ranges::begin(r), nth, ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void nth_element_index(
  R &&r, ::std::size_t firstidx, ::std::size_t nthidx,
  ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_index_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  if (nthidx < firstidx || lastidx < nthidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<
      ::fast_io::details::algorithms::range_throws_on_violations<R>>();
  }
  ::fast_io::algorithms::nth_element_iters(
    first,
    first + static_cast<::std::iter_difference_t<
      ::fast_io::details::algorithms::algo_index_iterator<R>>>(
      nthidx - firstidx),
    last, cmp);
}

} // namespace fast_io::algorithms
