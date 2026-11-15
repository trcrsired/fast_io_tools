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
	int const a{3};
	int const b{7};
	if (::fast_io::algorithms::min(a, b) != 3 ||
		::fast_io::algorithms::max(a, b) != 7)
	{
		return false;
	}
	auto range{::fast_io::algorithms::minmax(a, b)};
	if (range.first != 3 || range.second != 7)
	{
		return false;
	}
	if (::fast_io::algorithms::clamp(10, 1, 5) != 5 ||
		::fast_io::algorithms::clamp(-1, 1, 5) != 1)
	{
		return false;
	}
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

static_assert(noexcept(::fast_io::algorithms::min(1, 2)));
static_assert(noexcept(::fast_io::algorithms::min_element(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::minmax_element(
	::std::declval<std::vector<int> &>())));

int main()
{
	std::vector<std::string> values{"pear", "apple", "zebra", "apple"};
	if (::fast_io::algorithms::min_element_index(values, 0, 4) != 1)
	{
		return 1;
	}
	if (::fast_io::algorithms::max_element_index(values, 0, 4) != 2)
	{
		return 2;
	}
	auto range{::fast_io::algorithms::minmax_element_index(values, 0, 4)};
	if (range.first != 1 || range.second != 2)
	{
		return 3;
	}
	return 0;
}
