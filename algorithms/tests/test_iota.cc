#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <list>
#include <vector>

namespace
{

constexpr bool constexpr_iota_test()
{
	int a[4]{};
	int const end{::fast_io::algorithms::iota_iters(a, a + 4, 10)};
	if (end != 14)
	{
		return false;
	}
	if (a[0] != 10 || a[1] != 11 || a[2] != 12 || a[3] != 13)
	{
		return false;
	}
	long long b[3]{};
	long long const end2{::fast_io::algorithms::iota_iters(b, b + 3, 5LL)};
	if (end2 != 8 || b[0] != 5 || b[1] != 6 || b[2] != 7)
	{
		return false;
	}
	// An empty range returns the initial value unchanged.
	if (::fast_io::algorithms::iota_iters(a, a, 42) != 42)
	{
		return false;
	}
	// Checked subrange.
	int c[5]{9, 9, 9, 9, 9};
	int const end3{::fast_io::algorithms::iota_index_range(c, 1, 4, 3)};
	return end3 == 6 && c[0] == 9 && c[1] == 3 && c[2] == 4 &&
		c[3] == 5 && c[4] == 9;
}

static_assert(constexpr_iota_test());

} // namespace

int main()
{
	std::vector<int> v(5);
	int const end{::fast_io::algorithms::iota(v, 1)};
	if (end != 6)
	{
		return 1;
	}
	for (std::size_t i{}; i != v.size(); ++i)
	{
		if (v[i] != static_cast<int>(i) + 1)
		{
			return 2;
		}
	}

	std::vector<int> sub{7, 7, 7, 7};
	int const sub_end{::fast_io::algorithms::iota_index_range(sub, 1, 3, 20)};
	if (sub_end != 22 || sub[0] != 7 || sub[1] != 20 || sub[2] != 21 ||
		sub[3] != 7)
	{
		return 3;
	}

	std::list<int> l(3);
	int const list_end{::fast_io::algorithms::iota(l, 4)};
	if (list_end != 7)
	{
		return 4;
	}
	int expected{4};
	for (int value : l)
	{
		if (value != expected)
		{
			return 5;
		}
		++expected;
	}
	return 0;
}
