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

constexpr bool constexpr_merge_test()
{
	int a[]{1, 3, 5};
	int b[]{2, 4, 6};
	int out[6]{};
	auto end{::fast_io::algorithms::merge_iters(a, a + 3, b, b + 3, out)};
	if (end != out + 6 || out[0] != 1 || out[5] != 6)
	{
		return false;
	}
	int c[]{1, 3, 5, 2, 4, 6};
	::fast_io::algorithms::inplace_merge_iters(c, c + 3, c + 6);
	for (int i{1}; i != 6; ++i)
	{
		if (c[i - 1] > c[i])
		{
			return false;
		}
	}
	return true;
}

static_assert(constexpr_merge_test());

bool check_random()
{
	std::uint64_t state{0x9e3779b97f4a7c15ull};
	for (std::size_t n{}; n != 40; ++n)
	{
		std::vector<int> a(n), b(n);
		for (auto &value : a)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 20);
		}
		for (auto &value : b)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 20);
		}
		std::sort(a.begin(), a.end());
		std::sort(b.begin(), b.end());
		std::vector<int> expected(2 * n), actual(2 * n);
		auto expected_end{std::merge(a.begin(), a.end(), b.begin(), b.end(), expected.begin())};
		auto actual_end{::fast_io::algorithms::merge(a, b, actual.begin())};
		if (!std::equal(expected.begin(), expected_end, actual.begin(), actual_end))
		{
			return false;
		}
		for (std::size_t split{}; split <= n; ++split)
		{
			auto inplace{std::vector<int>(n)};
			for (std::size_t i{}; i != n; ++i)
			{
				state ^= state << 13;
				state ^= state >> 7;
				state ^= state << 17;
				inplace[i] = static_cast<int>(state % 20);
			}
			std::sort(inplace.begin(), inplace.begin() + split);
			std::sort(inplace.begin() + split, inplace.end());
			auto expected_inplace{inplace};
			std::inplace_merge(expected_inplace.begin(),
				expected_inplace.begin() + split, expected_inplace.end());
			::fast_io::algorithms::inplace_merge(inplace, inplace.begin() + split);
			if (inplace != expected_inplace)
			{
				return false;
			}
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::merge(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>(),
	::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::inplace_merge(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int> &>().begin())));

int main()
{
	if (!check_random())
	{
		return 1;
	}
	std::list<int> values{1, 3, 5, 2, 4, 6};
	auto middle{values.begin()};
	std::advance(middle, 3);
	::fast_io::algorithms::inplace_merge_iters(values.begin(), middle, values.end());
	int const expected[]{1, 2, 3, 4, 5, 6};
	std::size_t i{};
	for (auto const value : values)
	{
		if (value != expected[i++])
		{
			return 2;
		}
	}
	return 0;
}
