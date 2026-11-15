#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <forward_list>
#include <vector>

namespace
{

struct is_negative
{
	bool operator()(int value) const noexcept { return value < 0; }
};

bool check_find_if()
{
	std::vector<int> values{3, 5, -1, 7, -2};
	auto const expected{
		std::find_if(values.begin(), values.end(), is_negative{})};
	auto const actual{
		::fast_io::algorithms::find_if(values, is_negative{})};
	if (expected != actual)
	{
		return false;
	}
	auto const missing{::fast_io::algorithms::find_if(
		values, [](int value) noexcept { return value > 100; })};
	if (missing != values.end())
	{
		return false;
	}
	return true;
}

bool check_find_if_not()
{
	std::vector<int> values{3, 5, 7, -1, 9};
	auto const expected{
		std::find_if_not(values.begin(), values.end(), is_negative{})};
	auto const actual{
		::fast_io::algorithms::find_if_not(values, is_negative{})};
	if (expected != actual)
	{
		return false;
	}
	auto const missing{::fast_io::algorithms::find_if_not(
		values, [](int value) noexcept { return value > 0; })};
	if (missing != values.begin() + 3)
	{
		return false;
	}
	return true;
}

bool check_index()
{
	std::vector<int> values{1, 2, 3, 4, 5, 6, 7, 8};
	for (std::size_t first{}; first <= values.size(); ++first)
	{
		for (std::size_t last{first}; last <= values.size(); ++last)
		{
			auto const begin{
				values.begin() + static_cast<std::ptrdiff_t>(first)};
			auto const end{
				values.begin() + static_cast<std::ptrdiff_t>(last)};
			auto const predicate = [](int value) noexcept {
				return value > 4;
			};
			auto const expected{std::find_if(begin, end, predicate)};
			if (::fast_io::algorithms::find_if_index(
					values, first, last, predicate) !=
				first + static_cast<std::size_t>(expected - begin))
			{
				return false;
			}
			auto const inverse = [](int value) noexcept {
				return value <= 4;
			};
			auto const expected_inverse{
				std::find_if_not(begin, end, inverse)};
			if (::fast_io::algorithms::find_if_not_index(
					values, first, last, inverse) !=
				first + static_cast<std::size_t>(
					expected_inverse - begin))
			{
				return false;
			}
		}
	}
	return true;
}

bool check_forward_list() throws(true)
{
	std::forward_list<int> values{2, 4, 5, 8};
	auto const found{::fast_io::algorithms::find_if(
		values, [](int value) noexcept { return (value % 2) != 0; })};
	if (found == values.end() || *found != 5)
	{
		return false;
	}
	auto const not_found{::fast_io::algorithms::find_if_not(
		values, [](int value) noexcept { return (value % 2) == 0; })};
	return not_found != values.end() && *not_found == 5;
}

constexpr bool constexpr_test()
{
	int a[6]{2, 4, 5, 8, 9, 10};
	auto const found{::fast_io::algorithms::find_if(
		a, [](int value) constexpr noexcept { return (value % 2) != 0; })};
	if (found != a + 2)
	{
		return false;
	}
	auto const not_found{::fast_io::algorithms::find_if_not(
		a, [](int value) constexpr noexcept { return (value % 2) == 0; })};
	if (not_found != a + 2)
	{
		return false;
	}
	if (::fast_io::algorithms::find_if_index(
			a, 3, 6,
			[](int value) constexpr noexcept { return value == 9; }) != 4)
	{
		return false;
	}
	return ::fast_io::algorithms::find_if_not_index(
		a, 0, 3,
		[](int value) constexpr noexcept { return value == 4; }) == 0;
}

static_assert(constexpr_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::find_if(
	::std::declval<std::vector<int> &>(),
	[](int value) noexcept { return value == 1; })));
static_assert(noexcept(::fast_io::algorithms::find_if_not(
	::std::declval<int (&)[4]>(),
	[](int value) noexcept { return value == 1; })));
static_assert(noexcept(::fast_io::algorithms::find_if_index(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{},
	[](int value) noexcept { return value == 1; })));

int main()
{
	if (!check_find_if())
	{
		return 1;
	}
	if (!check_find_if_not())
	{
		return 2;
	}
	if (!check_index())
	{
		return 3;
	}
	try
	{
		if (!check_forward_list())
		{
			return 4;
		}
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		return 5;
	}
	return 0;
}
