#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_minmax_test()
{
	int values[]{4, 1, 7, 1, 7, 2};
	if (::fast_io::algorithms::min_element(values) != values + 1)
	{
		return false;
	}
	if (::fast_io::algorithms::max_element(values) != values + 2)
	{
		return false;
	}
	auto elements{::fast_io::algorithms::minmax_element(values)};
	return elements.first == values + 1 && elements.second == values + 4;
}

static_assert(constexpr_minmax_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::min_element(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::minmax_element(
	::std::declval<std::vector<int> &>())));

int main()
{
	std::vector<std::string> values{"pear", "apple", "zebra", "apple"};
	if (::fast_io::algorithms::min_element_index_range(values, 0, 4) != 1)
	{
		return 1;
	}
	if (::fast_io::algorithms::max_element_index_range(values, 0, 4) != 2)
	{
		return 2;
	}
	auto range{::fast_io::algorithms::minmax_element_index_range(values, 0, 4)};
	if (range.first != 1 || range.second != 2)
	{
		return 3;
	}
	return 0;
}
