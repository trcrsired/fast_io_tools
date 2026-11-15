#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_nth_element_test()
{
	int a[]{5, 1, 4, 2, 3};
	::fast_io::algorithms::nth_element(a, a + 2);
	if (a[2] != 3)
	{
		return false;
	}
	for (int i{}; i != 2; ++i)
	{
		if (a[i] > a[2])
		{
			return false;
		}
	}
	for (int i{3}; i != 5; ++i)
	{
		if (a[i] < a[2])
		{
			return false;
		}
	}
	return true;
}

static_assert(constexpr_nth_element_test());

bool check_random()
{
	std::uint64_t state{0x243f6a8885a308d3ull};
	for (std::size_t n{}; n != 40; ++n)
	{
		std::vector<int> source(n);
		for (auto &value : source)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 20);
		}
		for (std::size_t nth{}; nth != n; ++nth)
		{
			auto expected{source};
			std::nth_element(expected.begin(), expected.begin() + nth, expected.end());
			auto actual{source};
			::fast_io::algorithms::nth_element(actual, actual.begin() + nth);
			if (expected[nth] != actual[nth] ||
				!std::is_partitioned(actual.begin(), actual.begin() + nth,
					[&](int value) { return value <= actual[nth]; }) ||
				!std::is_partitioned(actual.begin() + nth, actual.end(),
					[&](int value) { return value >= actual[nth]; }))
			{
				return false;
			}
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::nth_element(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>().begin())));

int main()
{
	if (!check_random())
	{
		return 1;
	}
	std::vector<std::string> values{"d", "a", "c", "b"};
	::fast_io::algorithms::nth_element_index(values, 0, 1, 4);
	if (values[1] != "b")
	{
		return 2;
	}
	return 0;
}
