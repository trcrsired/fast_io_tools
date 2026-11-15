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

constexpr bool constexpr_partial_sort_test()
{
	int a[]{5, 1, 4, 2, 3};
	::fast_io::algorithms::partial_sort(a, a + 3);
	if (a[0] != 1 || a[1] != 2 || a[2] != 3)
	{
		return false;
	}
	int b[]{5, 1, 4, 2, 3};
	int out[3]{};
	auto end{::fast_io::algorithms::partial_sort_copy_iters(
		b, b + 5, out, out + 3)};
	return end == out + 3 && out[0] == 1 && out[1] == 2 && out[2] == 3;
}

static_assert(constexpr_partial_sort_test());

bool check_random()
{
	std::uint64_t state{0x9e3779b97f4a7c15ull};
	for (std::size_t n{}; n != 40; ++n)
	{
		std::vector<int> source(n);
		for (auto &value : source)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 30);
		}
		for (std::size_t middle{}; middle <= n; ++middle)
		{
			auto expected{source};
			std::partial_sort(expected.begin(), expected.begin() + middle, expected.end());
			auto actual{source};
			::fast_io::algorithms::partial_sort(actual, actual.begin() + middle);
			if (!std::equal(expected.begin(), expected.begin() + middle, actual.begin()) ||
				!std::is_sorted(actual.begin(), actual.begin() + middle))
			{
				return false;
			}
			for (std::size_t i{middle}; i != n; ++i)
			{
				if (middle != 0 && actual[i] < actual[middle - 1])
				{
					return false;
				}
			}

			std::vector<int> expected_out(n), actual_out(n);
			auto expected_end{std::partial_sort_copy(source.begin(), source.end(),
				expected_out.begin(), expected_out.begin() + middle)};
			auto actual_end{::fast_io::algorithms::partial_sort_copy_iters(
				source.begin(), source.end(), actual_out.begin(),
				actual_out.begin() + middle)};
			if (!std::equal(expected_out.begin(), expected_end,
				actual_out.begin(), actual_end) ||
				(expected_end - expected_out.begin()) !=
					(actual_end - actual_out.begin()))
			{
				return false;
			}
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::partial_sort(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int> &>().begin())));
static_assert(noexcept(::fast_io::algorithms::partial_sort_copy_iters(
	::std::declval<int *>(), ::std::declval<int *>(),
	::std::declval<int *>(), ::std::declval<int *>())));

int main()
{
	if (!check_random())
	{
		return 1;
	}
	std::vector<std::string> values{"e", "c", "a", "d", "b"};
	::fast_io::algorithms::partial_sort_index_range(values, 0, 2, 5);
	if (values[0] != "a" || values[1] != "b")
	{
		return 2;
	}
	std::vector<std::string> out(2);
	auto end{::fast_io::algorithms::partial_sort_copy(
		values, out)};
	if (end != out.end() || out[0] != "a" || out[1] != "b")
	{
		return 3;
	}
	return 0;
}
