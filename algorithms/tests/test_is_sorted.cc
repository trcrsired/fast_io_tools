#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <vector>

namespace
{

constexpr bool constexpr_is_sorted_test()
{
	int a[]{1, 2, 2, 3};
	if (!::fast_io::algorithms::is_sorted(a))
	{
		return false;
	}
	if (::fast_io::algorithms::is_sorted_until(a) != a + 4)
	{
		return false;
	}
	int b[]{1, 3, 2, 4};
	auto until{::fast_io::algorithms::is_sorted_until(b)};
	return until == b + 2 && !::fast_io::algorithms::is_sorted(b) &&
		::fast_io::algorithms::is_sorted_until_index_range(b, 0, 2) == 2;
}

static_assert(constexpr_is_sorted_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::is_sorted(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::is_sorted_until(
	::std::declval<std::vector<int> &>())));

int main()
{
	std::vector<int> values{1, 2, 3, 5, 4, 6};
	if (::fast_io::algorithms::is_sorted(values))
	{
		return 1;
	}
	if (::fast_io::algorithms::is_sorted_until_index_range(values, 1, 4) != 4)
	{
		return 2;
	}
	if (!::fast_io::algorithms::is_sorted_index_range(values, 1, 4))
	{
		return 3;
	}
	return 0;
}
