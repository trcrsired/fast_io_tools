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

constexpr bool constexpr_set_test()
{
	int a[]{1, 2, 3, 4, 5};
	int b[]{2, 4, 6};
	int out[8]{};
	if (!::fast_io::algorithms::includes_iters(a, a + 5, b, b + 2))
	{
		return false;
	}
	auto end{::fast_io::algorithms::set_union_iters(a, a + 5, b, b + 3, out)};
	if (end != out + 6 || out[0] != 1 || out[5] != 6)
	{
		return false;
	}
	int inter[3]{};
	end = ::fast_io::algorithms::set_intersection_iters(a, a + 5, b, b + 3, inter);
	if (end != inter + 2 || inter[0] != 2 || inter[1] != 4)
	{
		return false;
	}
	int diff[3]{};
	end = ::fast_io::algorithms::set_difference_iters(a, a + 5, b, b + 3, diff);
	if (end != diff + 3 || diff[0] != 1 || diff[2] != 5)
	{
		return false;
	}
	int sym[8]{};
	end = ::fast_io::algorithms::set_symmetric_difference_iters(
		a, a + 5, b, b + 3, sym);
	return end == sym + 4 && sym[0] == 1 && sym[3] == 6;
}

static_assert(constexpr_set_test());

bool check_random()
{
	std::uint64_t state{0x243f6a8885a308d3ull};
	for (std::size_t n{}; n != 30; ++n)
	{
		std::vector<int> a(n), b(n);
		for (auto &value : a)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 10);
		}
		for (auto &value : b)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<int>(state % 10);
		}
		std::sort(a.begin(), a.end());
		std::sort(b.begin(), b.end());
		if (std::includes(a.begin(), a.end(), b.begin(), b.end()) !=
			::fast_io::algorithms::includes(a, b))
		{
			return false;
		}
		std::vector<int> expected(2 * n), actual(2 * n);
		auto e{std::set_union(a.begin(), a.end(), b.begin(), b.end(), expected.begin())};
		auto r{::fast_io::algorithms::set_union_index(
			a, 0, n, b, 0, n, actual.begin())};
		if (!std::equal(expected.begin(), e, actual.begin(), r))
		{
			return false;
		}
		e = std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), expected.begin());
		r = ::fast_io::algorithms::set_intersection(a, b, actual.begin());
		if (!std::equal(expected.begin(), e, actual.begin(), r))
		{
			return false;
		}
		e = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), expected.begin());
		r = ::fast_io::algorithms::set_difference(a, b, actual.begin());
		if (!std::equal(expected.begin(), e, actual.begin(), r))
		{
			return false;
		}
		e = std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), expected.begin());
		r = ::fast_io::algorithms::set_symmetric_difference(a, b, actual.begin());
		if (!std::equal(expected.begin(), e, actual.begin(), r))
		{
			return false;
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::set_union(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>(),
	::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::includes(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>())));

int main()
{
	if (!check_random())
	{
		return 1;
	}
	std::list<int> a{1, 3, 5};
	std::list<int> b{2, 3, 4};
	std::vector<int> out(8);
	auto end{::fast_io::algorithms::set_union(a, b, out.begin())};
	int const expected[]{1, 2, 3, 4, 5};
	return std::equal(expected, expected + 5, out.begin(), end) ? 0 : 2;
}
