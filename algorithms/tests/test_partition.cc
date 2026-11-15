#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <forward_list>
#include <vector>

namespace
{

struct even
{
	inline constexpr bool operator()(int value) const noexcept
	{
		return value % 2 == 0;
	}
};

constexpr bool constexpr_partition_test()
{
	int a[]{1, 2, 3, 4, 5, 6};
	auto point{::fast_io::algorithms::partition(a, even{})};
	if (point != a + 3 || !::fast_io::algorithms::is_partitioned(a, even{}))
	{
		return false;
	}
	if (::fast_io::algorithms::partition_point(a, even{}) != point)
	{
		return false;
	}
	int b[]{1, 2, 3, 4, 5, 6};
	auto stable{::fast_io::algorithms::stable_partition(b, even{})};
	if (stable != b + 3 || b[0] != 2 || b[1] != 4 || b[2] != 6 ||
		b[3] != 1 || b[4] != 3 || b[5] != 5)
	{
		return false;
	}
	int c[6]{};
	int d[6]{};
	auto result{::fast_io::algorithms::partition_copy_iters(
		b, b + 6, c, d, even{})};
	return result.first == c + 3 && result.second == d + 3;
}

static_assert(constexpr_partition_test());

bool check_random()
{
	std::uint64_t state{0x243f6a8885a308d3ull};
	for (std::size_t n{}; n != 50; ++n)
	{
		std::vector<int> source(n);
		for (auto &value : source)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 7);
		}
		auto expected{source};
		auto actual{source};
		auto expected_point{std::stable_partition(expected.begin(), expected.end(), even{})};
		auto actual_point{::fast_io::algorithms::stable_partition(actual, even{})};
		if (expected != actual ||
			(expected_point - expected.begin()) !=
				(actual_point - actual.begin()))
		{
			return false;
		}
	}
	return true;
}

bool check_forward_list() throws(true)
{
	std::forward_list<int> values{1, 2, 3, 4, 5, 6};
	auto point{::fast_io::algorithms::stable_partition(values, even{})};
	int const expected[]{2, 4, 6, 1, 3, 5};
	std::size_t i{};
	for (auto const value : values)
	{
		if (value != expected[i])
		{
			return false;
		}
		++i;
	}
	return i == 6 && *point == 1;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::partition(
	::std::declval<std::vector<int> &>(), even{})));
static_assert(noexcept(::fast_io::algorithms::stable_partition(
	::std::declval<std::vector<int> &>(), even{})));

int main()
{
	if (!check_random())
	{
		return 1;
	}
	try
	{
		if (!check_forward_list())
		{
			return 2;
		}
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		return 2;
	}
	std::vector<int> values{1, 2, 3, 4, 5};
	if (::fast_io::algorithms::partition_index_range(values, 1, 4, even{}) != 3)
	{
		return 3;
	}
	return 0;
}
