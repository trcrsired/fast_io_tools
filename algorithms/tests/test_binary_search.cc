#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace
{

struct greater
{
	template <typename T, typename U>
	inline constexpr auto compare(T const &a, U const &b) const noexcept
	{
		return b <=> a;
	}
};

constexpr bool constexpr_binary_search_test()
{
	int a[]{1, 2, 2, 2, 4, 5};
	if (::fast_io::algorithms::lower_bound(a, 2) != a + 1)
	{
		return false;
	}
	if (::fast_io::algorithms::upper_bound(a, 2) != a + 4)
	{
		return false;
	}
	auto range{::fast_io::algorithms::equal_range(a, 2)};
	if (range.first != a + 1 || range.second != a + 4)
	{
		return false;
	}
	if (!::fast_io::algorithms::binary_search(a, 4) ||
		::fast_io::algorithms::binary_search(a, 3))
	{
		return false;
	}
	int b[]{5, 4, 3, 2, 1};
	return ::fast_io::algorithms::lower_bound(b, 4, greater{}) == b + 1 &&
		::fast_io::algorithms::binary_search(b, 3, greater{});
}

static_assert(constexpr_binary_search_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::lower_bound(
	::std::declval<std::vector<int> &>(), 1)));
static_assert(noexcept(::fast_io::algorithms::equal_range(
	::std::declval<std::vector<int> &>(), 1)));

int main()
{
	std::uint64_t state{0x1234567890abcdefull};
	std::vector<int> values;
	for (int i{}; i != 200; ++i)
	{
		state ^= state << 13;
		state ^= state >> 7;
		state ^= state << 17;
		values.push_back(static_cast<int>(state % 50));
	}
	std::sort(values.begin(), values.end());
	for (int value{}; value != 55; ++value)
	{
		auto e1{std::equal_range(values.begin(), values.end(), value)};
		auto e2{::fast_io::algorithms::equal_range_index(
			values, 0, values.size(), value)};
		if ((e1.first - values.begin()) != static_cast<std::ptrdiff_t>(e2.first) ||
			(e1.second - values.begin()) != static_cast<std::ptrdiff_t>(e2.second))
		{
			return 1;
		}
		if (std::binary_search(values.begin(), values.end(), value) !=
			::fast_io::algorithms::binary_search_index(
				values, 0, values.size(), value))
		{
			return 2;
		}
	}
	return 0;
}
