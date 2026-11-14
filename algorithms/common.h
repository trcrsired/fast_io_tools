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

template<bool throwing_violations>
inline constexpr ::std::size_t algo_index_check(::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
{
  if (lastidx < firstidx || n < lastidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
  }
  return lastidx;
}

template<bool throwing_violations, typename T>
inline constexpr ::fast_io::details::algorithms::iter_pairs<T*, T*> algo_index_to_ptrs(
  T* dt, ::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
{
  ::std::size_t const last{
    ::fast_io::details::algorithms::algo_index_check<throwing_violations>(n, firstidx, lastidx)};
  return {dt + firstidx, dt + last};
}

template<bool throwing_violations>
inline constexpr ::fast_io::details::algorithms::iter_pairs<::std::byte const*, ::std::byte const*> algo_index_to_ptrs_common(
  ::std::byte const* dt, ::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx, ::std::size_t sizeoft)
  FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
{
  ::std::size_t const last{
    ::fast_io::details::algorithms::algo_index_check<throwing_violations>(n, firstidx, lastidx)};
  return {dt + firstidx * sizeoft, dt + last * sizeoft};
}

template<typename R>
inline constexpr ::fast_io::allocator_adapter_flags range_allocator_flags = []() constexpr
{
  using type = ::std::remove_cvref_t<R>;
  if constexpr(requires{
    type::allocator_flags;
  })
  {
    static_assert(::std::same_as<::std::remove_cv_t<decltype(type::allocator_flags)>, ::fast_io::allocator_adapter_flags>);
    return type::allocator_flags;
  }
  else
  {
    return ::fast_io::allocator_adapter_flags::none;
  }
}();

template<typename R>
inline constexpr bool range_throws_on_violations{
  (::fast_io::details::algorithms::range_allocator_flags<R> &
   ::fast_io::allocator_adapter_flags::throws_on_violations) != ::fast_io::allocator_adapter_flags::none};

template<typename R>
struct algo_index_iterator_impl
{
  using type = decltype(::std::ranges::begin(::std::declval<R&>()));
};

template<typename R>
  requires ::std::ranges::contiguous_range<R>
struct algo_index_iterator_impl<R>
{
  using type = decltype(::std::to_address(::std::ranges::data(::std::declval<R&>())));
};

template<typename R>
using algo_index_iterator = typename ::fast_io::details::algorithms::algo_index_iterator_impl<R>::type;

template<typename R>
using algo_index_iters = ::fast_io::details::algorithms::iter_pairs<
  ::fast_io::details::algorithms::algo_index_iterator<R>,
  ::fast_io::details::algorithms::algo_index_iterator<R>>;

template<typename R>
inline constexpr bool algo_index_source_no_throw = []() constexpr
{
  if constexpr(::std::ranges::contiguous_range<R>)
  {
    return noexcept(::std::ranges::data(::std::declval<R&>())) &&
           noexcept(::std::ranges::size(::std::declval<R&>())) &&
           noexcept(::std::to_address(::std::ranges::data(::std::declval<R&>())));
  }
  else
  {
    return noexcept(::std::ranges::begin(::std::declval<R&>())) &&
           noexcept(::std::ranges::size(::std::declval<R&>())) &&
           noexcept(::std::ranges::begin(::std::declval<R&>()) + ::std::declval<::std::size_t>());
  }
}();

template<::std::ranges::random_access_range R>
inline constexpr ::fast_io::details::algorithms::algo_index_iters<R> algo_index_to_iters(
  R&& r, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::algorithms::range_throws_on_violations<R> ||
    !::fast_io::details::algorithms::algo_index_source_no_throw<R>)
{
  constexpr bool throwing_violations{
    ::fast_io::details::algorithms::range_throws_on_violations<R>};
  if constexpr(::std::ranges::contiguous_range<R>)
  {
    using pointer_type = ::fast_io::details::algorithms::algo_index_iterator<R>;
    using pointer_pairs = ::fast_io::details::algorithms::iter_pairs<pointer_type, pointer_type>;
    if consteval
    {
      return ::fast_io::details::algorithms::algo_index_to_ptrs<throwing_violations>(
        ::std::to_address(::std::ranges::data(r)), ::std::ranges::size(r), firstidx, lastidx);
    }
    else
    {
      static_assert(sizeof(pointer_pairs) == sizeof(::fast_io::details::algorithms::iter_pairs<::std::byte const*, ::std::byte const*>));
      static_assert(::std::is_trivially_copyable_v<pointer_pairs>);
      return ::std::bit_cast<pointer_pairs>(
        ::fast_io::details::algorithms::algo_index_to_ptrs_common<throwing_violations>(
          reinterpret_cast<::std::byte const*>(::std::to_address(::std::ranges::data(r))),
          ::std::ranges::size(r), firstidx, lastidx, sizeof(::std::ranges::range_value_t<R>)));
    }
  }
  else
  {
    ::std::size_t const last{
      ::fast_io::details::algorithms::algo_index_check<throwing_violations>(
        ::std::ranges::size(r), firstidx, lastidx)};
    auto bg{::std::ranges::begin(r)};
    return ::fast_io::details::algorithms::iter_pairs<decltype(bg), decltype(bg)>{bg + firstidx, bg + last};
  }
}

template<typename Iter, typename Snt, bool decay_contiguous, bool decay_sized>
struct iter_types_impl
{
  using iterator = Iter;
  using sentinel = Snt;
  static inline constexpr bool decays{};
};

template<typename Iter, typename Snt>
struct iter_types_impl<Iter, Snt, true, false>
{
  using iterator = decltype(::std::to_address(::std::declval<Iter&>()));
  using sentinel = decltype(::std::to_address(::std::declval<Snt&>()));
  static inline constexpr bool decays{true};
};

template<typename Iter, typename Snt>
struct iter_types_impl<Iter, Snt, false, true>
{
  using iterator = decltype(::std::to_address(::std::declval<Iter&>()));
  using sentinel = iterator;
  static inline constexpr bool decays{true};
};

template<typename Iter, typename Snt>
using iter_types = iter_types_impl<
  Iter, Snt,
  ::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt>,
  ::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> && ::std::sized_sentinel_for<Snt, Iter>>;

template<typename Iter, typename Snt>
concept iter_decay_source_no_throw =
  (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt> &&
   requires(Iter& first, Snt& last)
   {
     { ::std::to_address(first) } noexcept;
     { ::std::to_address(last) } noexcept;
   }) ||
  (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> &&
   ::std::sized_sentinel_for<Snt, Iter> &&
   requires(Iter& first, Snt& last)
   {
     { ::std::to_address(first) } noexcept;
     { last - first } noexcept;
   });

template<typename Iter, typename Snt>
inline constexpr bool copy_source_last_no_throw =
  (::std::same_as<Iter, Snt> &&
   requires(Iter& it) {
     { ::std::to_address(it) } noexcept;
   }) ||
  (::std::contiguous_iterator<Snt> &&
   requires(Iter& first, Snt& last) {
     { ::std::to_address(first) } noexcept;
     { ::std::to_address(last) } noexcept;
   }) ||
  (::std::sized_sentinel_for<Snt, Iter> &&
   requires(Iter& first, Snt& last) {
     { ::std::to_address(first) } noexcept;
     { last - first } noexcept;
   });

template<typename Iter, typename Snt>
inline constexpr auto copy_source_last(Iter first, Snt last) noexcept
{
  if constexpr (::std::same_as<Iter, Snt> || ::std::contiguous_iterator<Snt>)
  {
    return ::std::to_address(last);
  }
  else
  {
    return ::std::to_address(first) + (last - first);
  }
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr bool copy_can_decay{
  ::std::contiguous_iterator<Iter> &&
  ::std::contiguous_iterator<OIter> &&
  ::fast_io::details::algorithms::copy_source_last_no_throw<Iter, Snt> &&
  requires(OIter& dest, ::std::iter_difference_t<OIter> difference) {
    { ::std::to_address(dest) } noexcept;
    { dest + difference } noexcept;
  }};

template<typename OIter, typename T>
struct uninitialized_guard
{
  OIter first;
  OIter current;
  inline constexpr ~uninitialized_guard() noexcept
  {
    if constexpr (!::std::is_trivially_destructible_v<T>)
    {
      for (; first != current; ++first)
      {
        __builtin_addressof(*first)->~T();
      }
    }
  }
};

}
