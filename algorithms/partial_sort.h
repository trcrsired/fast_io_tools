#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt, typename Cmp>
concept partial_sort_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::sort_iters_no_throw<Iter, Cmp> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp>;

template<typename R, typename Cmp>
concept partial_sort_ranges_no_throw =
  ::std::ranges::random_access_range<R> &&
  ::fast_io::details::algorithms::partial_sort_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept partial_sort_index_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::partial_sort_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter, typename Snt, typename OIter, typename OSnt,
  typename Cmp>
concept partial_sort_copy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::sentinel_no_throw<OIter, OSnt> &&
  ::std::random_access_iterator<OIter> &&
  ::fast_io::details::algorithms::sort_iters_no_throw<OIter, Cmp> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  requires(Iter &src, OIter &dest)
{
  { *dest = *src } noexcept;
  { ::std::iter_value_t<OIter>(*src) } noexcept;
};

template<typename R1, typename R2, typename Cmp>
concept partial_sort_copy_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  ::fast_io::details::algorithms::partial_sort_copy_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>, Cmp>;

template<typename R1, typename R2, typename Cmp>
concept partial_sort_copy_index_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::fast_io::details::algorithms::partial_sort_copy_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
inline constexpr void partial_sort_iters_impl(
  Iter first, Iter middle, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_iters_no_throw<
      Iter, Snt, Cmp>)
{
  // A complete sort also satisfies the partial-sort postcondition.
  Iter last_iter{first};
  for (; last_iter != last; ++last_iter)
  {
  }
  ::fast_io::algorithms::sort_iters(first, last_iter, cmp);
}

template<typename Iter, typename Snt, typename OIter, typename OSnt,
  typename Cmp>
inline constexpr OIter partial_sort_copy_iters_impl(
  Iter first, Snt last, OIter d_first, OSnt d_last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_copy_iters_no_throw<
      Iter, Snt, OIter, OSnt, Cmp>)
{
  OIter d_end{d_first};
  while (first != last && d_end != d_last)
  {
    *d_end = *first;
    ++d_end;
    ++first;
  }
  if (d_end == d_first)
  {
    return d_first;
  }
  ::fast_io::algorithms::sort_iters(d_first, d_end, cmp);
  if (first == last)
  {
    return d_end;
  }
  OIter const max_pos{
    [&]() constexpr
    {
      OIter pos{d_end};
      --pos;
      return pos;
    }()};
  for (; first != last; ++first)
  {
    if (!(0 > cmp.compare(*first, *max_pos)))
    {
      continue;
    }
    ::std::iter_value_t<OIter> value(*first);
    OIter pos{max_pos};
    while (pos != d_first)
    {
      OIter prev{pos};
      --prev;
      if (!(0 > cmp.compare(value, *prev)))
      {
        break;
      }
      *pos = ::std::move(*prev);
      pos = prev;
    }
    *pos = ::std::move(value);
  }
  return d_end;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Sort the smallest elements so that `[first, middle)` contains them in order
// and no element in `[middle, last)` compares less than one in `[first,
// middle)`.
template<::std::random_access_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void partial_sort_iters(
  Iter first, Iter middle, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_iters_no_throw<
      Iter, Snt, Cmp>)
{
  ::fast_io::details::algorithms::partial_sort_iters_impl(
    first, middle, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void partial_sort(
  R &&r, ::std::ranges::iterator_t<R> middle, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_ranges_no_throw<R, Cmp>)
{
  ::fast_io::algorithms::partial_sort_iters(
    ::std::ranges::begin(r), middle, ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void partial_sort_index(
  R &&r, ::std::size_t firstidx, ::std::size_t middleidx,
  ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_index_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  if (middleidx < firstidx || lastidx < middleidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<
      ::fast_io::details::algorithms::range_throws_on_violations<R>>();
  }
  ::fast_io::algorithms::partial_sort_iters(
    first,
    first + static_cast<::std::iter_difference_t<
      ::fast_io::details::algorithms::algo_index_iterator<R>>>(
      middleidx - firstidx),
    last, cmp);
}

// Copy as many elements as fit into `[d_first, d_last)` and sort them.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  ::std::random_access_iterator OIter, ::std::sentinel_for<OIter> OSnt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr OIter partial_sort_copy_iters(
  Iter first, Snt last, OIter d_first, OSnt d_last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_copy_iters_no_throw<
      Iter, Snt, OIter, OSnt, Cmp>)
{
  return ::fast_io::details::algorithms::partial_sort_copy_iters_impl(
    first, last, d_first, d_last, cmp);
}

template<::std::ranges::input_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::ranges::iterator_t<R2> partial_sort_copy(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_copy_ranges_no_throw<
      R1, R2, Cmp>)
{
  return ::fast_io::algorithms::partial_sort_copy_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t partial_sort_copy_index(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partial_sort_copy_index_no_throw<
      R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  auto const result{::fast_io::algorithms::partial_sort_copy_iters(
    first1, last1, first2, last2, cmp)};
  return firstidx2 + static_cast<::std::size_t>(result - first2);
}

} // namespace fast_io::algorithms
