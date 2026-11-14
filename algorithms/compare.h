#pragma once

namespace fast_io::algorithms
{

// Default three-way comparator. The standard one is not marked for
// herbceptions, so wrap fast_io's freestanding replacement behind the named
// `compare` member that every algorithm in this library expects.
struct compare_three_way
{
  template<typename T, typename U>
  inline constexpr auto compare(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(::fast_io::freestanding::compare_three_way{}(t, u)))
  {
    return ::fast_io::freestanding::compare_three_way{}(t, u);
  }

  // Optional equality fast path. Algorithms that need an equality test use
  // `equal` when a comparator provides it, and otherwise fall back to
  // `compare(a, b) == 0`.
  template<typename T, typename U>
  inline constexpr bool equal(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(
      !noexcept(::fast_io::freestanding::compare_three_way{}(t, u) == 0))
  {
    return ::fast_io::freestanding::compare_three_way{}(t, u) == 0;
  }
};

} // namespace fast_io::algorithms
