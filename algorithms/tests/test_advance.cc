#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstddef>
#include <forward_list>
#include <list>
#include <vector>

namespace
{

constexpr bool constexpr_advance_test()
{
	int a[8]{};
	int *p{a};
	::fast_io::algorithms::advance_iters(p, 5);
	if (p != a + 5)
	{
		return false;
	}
	::fast_io::algorithms::advance_iters(p, -3);
	if (p != a + 2)
	{
		return false;
	}
	::fast_io::algorithms::advance_iters(p, 0);
	return p == a + 2;
}

static_assert(constexpr_advance_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::advance_iters(
	::std::declval<int *&>(), std::ptrdiff_t{})));
static_assert(noexcept(::fast_io::algorithms::advance_iters(
	::std::declval<::std::vector<int>::iterator &>(), std::ptrdiff_t{})));
static_assert(!noexcept(::fast_io::algorithms::advance_iters(
	::std::declval<::std::list<int>::iterator &>(), std::ptrdiff_t{})));

int main()
{
	{
		std::vector<int> values(10);
		auto it{values.begin()};
		::fast_io::algorithms::advance_iters(it, 4);
		if (it != values.begin() + 4)
		{
			return 1;
		}
		::fast_io::algorithms::advance_iters(it, -1);
		if (it != values.begin() + 3)
		{
			return 2;
		}
		::fast_io::algorithms::advance_iters(it, 0);
		if (it != values.begin() + 3)
		{
			return 3;
		}
	}

	try
	{
		std::list<int> list_values{1, 2, 3, 4, 5};
		auto list_it{list_values.begin()};
		::fast_io::algorithms::advance_iters(list_it, 3);
		if (*list_it != 4)
		{
			return 4;
		}
		::fast_io::algorithms::advance_iters(list_it, -2);
		if (*list_it != 2)
		{
			return 5;
		}

		std::forward_list<int> forward_values{1, 2, 3, 4, 5};
		auto forward_it{forward_values.begin()};
		::fast_io::algorithms::advance_iters(forward_it, 2);
		if (*forward_it != 3)
		{
			return 6;
		}
	}
	catch throws(::std::error)
	{
		return 7;
	}
	return 0;
}
