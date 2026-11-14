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
};

} // namespace fast_io::algorithms
