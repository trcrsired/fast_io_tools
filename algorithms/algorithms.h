#pragma once

namespace fast_io::details::algorithms
{

template<::std::input_iterator Iter,
  ::std::sentinel_for<Iter> Snt>
struct iter_pairs
{
  using iterator = Iter;
  using sentinel = Snt;
  Iter first;
  Snt last;
};

template<bool throwing_violations,typename T>
inline constexpr ::fast_io::details::algorithms::iter_pairs<T*> algo_index_to_ptrs(T *dt, ::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx) throws(throwing_violations)
{
  if (lastidx<=firstidx || n <= lastidx) [[unlikely]]
::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
  return {dt+firstidx， dt+lastidx};
}

template<bool throwing_violations>
inline ::fast_io::details::algorithms::iter_pairs<::std::byte const*> algo_index_to_ptrs_common(::std::byte const* dt, ::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx, ::std::size_t sizeoft) throws(throwing_violations)
{
  if (lastidx<=firstidx || n <= lastidx) [[unlikely]]
::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
  return {dt+firstidx*sizeoft， dt+lastidx*sizeoft};
}

template<::std::ranges::random_access_range R>
inline constexpr auto algo_index_to_iters(R&& r, ::std::size_t firstidx, ::std::size_t lastidx) throws /*conditional throws by respect r's allocator flags*/
{
  constexpr bool throwing_violations{};
  if constexpr(::std::contiguous_range<R>)
  {
    if consteval
    {
      return ::fast_io::details::algorithms::algo_index_to_ptrs<throwing_violations>(::std::ranges::data(r), ::std::ranges::size(r), firstidx, lastidx);
    }
    else
    {
      return ::std::bit_cast<::fast_io::details::algorithms::iter_pairs<decltype(::std::ranges::data(r))>>(::fast_io::details::algorithms::algo_index_to_ptrs_common<throwing_violations>(reinterpret_cast<::std::byte const*>(::std::ranges::data(r)), ::std::ranges::size(r),firstidx,lastidx,sizeof(::std::range_value_t<R>)));
    }
  }
  else
  {
  if (lastidx<=firstidx || ::std::ranges::size(r) <= lastidx) [[unlikely]]
::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
    auto bg{::std::ranges::begin(r)};
    return {bg+firstidx, bg+lastidx};
  }
}

}

namespace fast_io::algorithms
{
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
inline constexpr void for_each_iters(Iter first, Snt last, F f) throws(!std::is_nothrow_invokable_v<F, ::std::iter_value_t<Iter>>)
{
  if constexpr(::std::contiguous_iterator<Iter>&&
    !::std::is_pointer_v<Iter>)
  {   ::fast_io::algorithms::for_each_iters(::std::to_address(first),::std::to_address(last));
  }
  else if constexpr(requires()
  {
    for_each_iters_define(first, last, f);
  })
  {
    for_each_iters_define(first, last, f);
  }
  else
  {
    for(;first!=last;++first)
    {
      f(first);
    }
  }
}

template<::std::ranges::range R, typename F>
inline constexpr void for_each(R&& r, F f) throws /*conditional throws*/
{ ::fast_io::algorithms::for_each_iters(::std::ranges::begin(r), ::std::ranges::end(r), f);
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr void for_each_index(R&& r, ::std::size_t firstidx, ::std::size_t lastidx, F f) throws /*conditional throws*/
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(::std::forward<R>(r));
 ::fast_io::algorithms::for_each_iters(first, last, f);
}

}
