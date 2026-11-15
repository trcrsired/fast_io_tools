#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_swap_ranges_test()
{
	int a[]{1, 2, 3, 4};
	int b[]{5, 6, 7, 8};
	auto end{::fast_io::algorithms::swap_ranges_iters(a, a + 2, b + 2)};
	return end == b + 4 && a[0] == 7 && a[1] == 8 && b[2] == 1 && b[3] == 2;
}

static_assert(constexpr_swap_ranges_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::swap_ranges(
	::std::declval<std::vector<int> &>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::swap_ranges_index_range(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{},
	::std::declval<int *>())));

int main()
{
	std::vector<std::string> a{"a", "b", "c", "d"};
	std::vector<std::string> b{"w", "x", "y", "z"};
	::fast_io::algorithms::swap_ranges_index_range(a, 1, 3, b.begin() + 1);
	if (a[0] != "a" || a[1] != "x" || a[2] != "y" || a[3] != "d")
	{
		return 1;
	}
	if (b[1] != "b" || b[2] != "c")
	{
		return 2;
	}
	return 0;
}
