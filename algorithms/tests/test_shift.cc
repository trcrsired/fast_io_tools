#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace
{

std::uint64_t xorshift(std::uint64_t &state) noexcept
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return state;
}

constexpr bool constexpr_shift_test()
{
	int a[]{1, 2, 3, 4, 5};
	auto left_end{::fast_io::algorithms::shift_left(a, 2)};
	if (left_end != a + 3 || a[0] != 3 || a[1] != 4 || a[2] != 5)
	{
		return false;
	}
	int b[]{1, 2, 3, 4, 5};
	auto right_first{::fast_io::algorithms::shift_right(b, 2)};
	return right_first == b + 2 && b[2] == 1 && b[3] == 2 && b[4] == 3;
}

static_assert(constexpr_shift_test());

bool check_shift()
{
	for (std::size_t n{}; n != 32; ++n)
	{
		std::uint64_t state{0x9e3779b97f4a7c15ull ^ n};
		std::vector<std::int64_t> source(n);
		for (auto &value : source)
		{
			value = static_cast<std::int64_t>(xorshift(state) % 100);
		}
		for (std::size_t amount{}; amount != n + 3; ++amount)
		{
			auto expected{source};
			auto expected_end{
				::std::shift_left(expected.begin(), expected.end(), amount)};
			auto actual{source};
			auto actual_end{
				::fast_io::algorithms::shift_left(actual, amount)};
			if (expected != actual ||
				(expected_end - expected.begin()) !=
					(actual_end - actual.begin()))
			{
				return false;
			}

			expected = source;
			auto expected_first{
				::std::shift_right(expected.begin(), expected.end(), amount)};
			actual = source;
			auto actual_first{
				::fast_io::algorithms::shift_right(actual, amount)};
			if (expected != actual ||
				(expected_first - expected.begin()) !=
					(actual_first - actual.begin()))
			{
				return false;
			}

			expected = source;
			actual = source;
			auto expected_idx{
				::fast_io::algorithms::shift_left_index_range(expected, 0, n, amount)};
			auto actual_idx{
				::fast_io::algorithms::shift_left_index_range(actual, 0, n, amount)};
			if (expected != actual || expected_idx != actual_idx)
			{
				return false;
			}
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::shift_left(
	::std::declval<std::vector<int> &>(), 1)));
static_assert(noexcept(::fast_io::algorithms::shift_right(
	::std::declval<std::vector<int> &>(), 1)));

int main()
{
	if (!check_shift())
	{
		return 1;
	}
	int a[]{1, 2, 3, 4};
	::fast_io::algorithms::shift_right_index_range(a, 1, 4, 1);
	if (a[0] != 1 || a[1] != 2 || a[2] != 2 || a[3] != 3)
	{
		return 2;
	}
	return 0;
}
