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

constexpr bool constexpr_nth_element_large_test()
{
	int a[100]{};
	for (int i{}; i != 100; ++i)
	{
		a[i] = (i * 37 + 11) % 17;
	}
	::fast_io::algorithms::nth_element(a, a + 50);
	int const pivot{a[50]};
	std::size_t less{};
	std::size_t equal{};
	for (int value : a)
	{
		if (value < pivot)
		{
			++less;
		}
		else if (value == pivot)
		{
			++equal;
		}
	}
	if (less > 50 || 50 >= less + equal)
	{
		return false;
	}
	for (int i{}; i != 50; ++i)
	{
		if (a[i] > pivot)
		{
			return false;
		}
	}
	for (int i{51}; i != 100; ++i)
	{
		if (a[i] < pivot)
		{
			return false;
		}
	}
	return true;
}

static_assert(constexpr_nth_element_test());
static_assert(constexpr_nth_element_large_test());

bool check_source(std::vector<int> const &source)
{
	for (std::size_t nth{}; nth != source.size(); ++nth)
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
	return true;
}

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
		if (!check_source(source))
		{
			return false;
		}
	}
	return true;
}

bool check_patterns()
{
	std::uint64_t state{0x13198a2e03707344ull};
	for (std::size_t n : {1u, 2u, 3u, 7u, 8u, 63u, 64u, 65u, 200u})
	{
		std::vector<int> ascending(n);
		std::vector<int> descending(n);
		std::vector<int> equal(n, 7);
		std::vector<int> duplicate_heavy(n);
		for (std::size_t i{}; i != n; ++i)
		{
			ascending[i] = static_cast<int>(i);
			descending[i] = static_cast<int>(n - i);
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			duplicate_heavy[i] = (state % 10 == 0) ? 1 : 0;
		}
		if (!check_source(ascending) || !check_source(descending) ||
			!check_source(equal) || !check_source(duplicate_heavy))
		{
			return false;
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::nth_element(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>().begin())));

int main()
{
	if (!check_random() || !check_patterns())
	{
		return 1;
	}
	std::vector<std::string> values{"d", "a", "c", "b"};
	::fast_io::algorithms::nth_element_index_range(values, 0, 1, 4);
	if (values[1] != "b")
	{
		return 2;
	}
	return 0;
}
