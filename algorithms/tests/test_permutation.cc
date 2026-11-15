#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <list>
#include <vector>

namespace
{

constexpr bool constexpr_permutation_test()
{
	int a[]{1, 2, 3};
	if (!::fast_io::algorithms::is_permutation(a, a))
	{
		return false;
	}
	int b[]{2, 1, 3};
	if (!::fast_io::algorithms::is_permutation(a, b))
	{
		return false;
	}
	int c[]{1, 1, 2};
	if (::fast_io::algorithms::is_permutation(a, c))
	{
		return false;
	}
	if (!::fast_io::algorithms::is_permutation_prefix_iters(
		a, a + 2, b, b + 3))
	{
		return false;
	}
	if (!::fast_io::algorithms::next_permutation(a))
	{
		return false;
	}
	if (a[0] != 1 || a[1] != 3 || a[2] != 2)
	{
		return false;
	}
	if (!::fast_io::algorithms::prev_permutation(a))
	{
		return false;
	}
	return a[0] == 1 && a[1] == 2 && a[2] == 3;
}

static_assert(constexpr_permutation_test());

bool check_permutations(std::vector<int> values)
{
	std::vector<int> expected{values};
	std::vector<int> actual{values};
	for (;;)
	{
		if (expected != actual)
		{
			return false;
		}
		bool const expected_next{std::next_permutation(expected.begin(), expected.end())};
		bool const actual_next{::fast_io::algorithms::next_permutation(actual)};
		if (expected_next != actual_next)
		{
			return false;
		}
		if (!expected_next)
		{
			break;
		}
	}
	return ::fast_io::algorithms::is_permutation(expected, actual) &&
		expected == actual;
}

bool check_random()
{
	std::uint64_t state{0x1234567890abcdefull};
	for (std::size_t n{}; n != 8; ++n)
	{
		std::vector<int> values(n);
		for (auto &value : values)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 4);
		}
		std::sort(values.begin(), values.end());
		if (!check_permutations(values))
		{
			return false;
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::next_permutation(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::is_permutation(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>())));

int main()
{
	if (!check_random())
	{
		return 1;
	}
	std::list<int> values{1, 2, 3};
	if (!::fast_io::algorithms::next_permutation(values))
	{
		return 2;
	}
	int const expected[]{1, 3, 2};
	std::size_t i{};
	for (auto const value : values)
	{
		if (value != expected[i++])
		{
			return 3;
		}
	}
	return 0;
}
