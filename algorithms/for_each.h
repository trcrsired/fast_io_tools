#pragma once

#include "common.h"

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt, typename F>
concept iter_impl_no_throw =
  ::std::input_iterator<Iter> && ::std::sentinel_for<Snt, Iter> &&
  requires(Iter& first, Snt& last, F& f)
{
  { first != last } noexcept;
  { ++first } noexcept;
  { f(*first) } noexcept;
};

template<typename Iter, typename Snt, typename F>
concept iter_no_throw =
  (iter_types<Iter, Snt>::decays &&
   iter_decay_source_no_throw<Iter, Snt> &&
   iter_impl_no_throw<
     typename iter_types<Iter, Snt>::iterator,
     typename iter_types<Iter, Snt>::sentinel, F>) ||
  (!iter_types<Iter, Snt>::decays &&
   iter_impl_no_throw<Iter, Snt, F>);

template<typename R, typename F>
concept ranges_no_throw =
  ::std::ranges::range<R> &&
  requires(R& r)
  {
    { ::std::ranges::begin(r) } noexcept;
    { ::std::ranges::end(r) } noexcept;
  } &&
  ::std::is_nothrow_move_constructible_v<F> &&
  iter_no_throw<::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, F>;

template<typename R, typename F>
concept for_each_index_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::algo_index_source_no_throw<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
inline constexpr void for_each_iters_impl(Iter first, Snt last, F& f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::iter_no_throw<Iter, Snt, F>)
{
  if constexpr(::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    ::std::contiguous_iterator<Snt>)
  {
    ::fast_io::details::algorithms::for_each_iters_impl(
      ::std::to_address(first), ::std::to_address(last), f);
  }
  else if constexpr(::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    !::std::contiguous_iterator<Snt> && ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const ptr{::std::to_address(first)};
    ::fast_io::details::algorithms::for_each_iters_impl(ptr, ptr + count, f);
  }
  else if constexpr(requires{
    for_each_iters_define(first, last, f);
  })
  {
    for_each_iters_define(first, last, f);
  }
  else
  {
    for(;first!=last;++first)
    {
      f(*first);
    }
  }
}

}

namespace fast_io::algorithms
{
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
inline constexpr void for_each_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::iter_no_throw<Iter, Snt, F>)
{
  ::fast_io::details::algorithms::for_each_iters_impl(first, last, f);
}

template<::std::ranges::range R, typename F>
inline constexpr void for_each(R&& r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::ranges_no_throw<R, F>)
{
  ::fast_io::algorithms::for_each_iters(::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr void for_each_index(R&& r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::for_each_index_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::for_each_iters(first, last, ::std::move(f));
}

}
