#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <forward_list>
#include <vector>

namespace
{

struct is_even
{
	bool operator()(int value) const noexcept { return (value % 2) == 0; }
};

bool check_all_of()
{
	std::vector<int> values{2, 4, 6, 8};
	if (!::fast_io::algorithms::all_of(values, is_even{}))
	{
		return false;
	}
	values.back() = 9;
	if (::fast_io::algorithms::all_of(values, is_even{}))
	{
		return false;
	}
	return ::fast_io::algorithms::all_of(
		values, [](int) noexcept { return true; });
}

bool check_any_of()
{
	std::vector<int> values{1, 3, 4, 5};
	if (!::fast_io::algorithms::any_of(values, is_even{}))
	{
		return false;
	}
	if (::fast_io::algorithms::any_of(values, [](int) noexcept { return false; }))
	{
		return false;
	}
	std::vector<int> empty;
	return !::fast_io::algorithms::any_of(empty, is_even{});
}

bool check_none_of()
{
	std::vector<int> values{1, 3, 5};
	if (!::fast_io::algorithms::none_of(values, is_even{}))
	{
		return false;
	}
	values.push_back(6);
	if (::fast_io::algorithms::none_of(values, is_even{}))
	{
		return false;
	}
	std::vector<int> empty;
	return ::fast_io::algorithms::none_of(empty, is_even{});
}

bool check_index_range()
{
	std::vector<int> values{1, 2, 3, 4, 5, 6};
	for (std::size_t first{}; first <= values.size(); ++first)
	{
		for (std::size_t last{first}; last <= values.size(); ++last)
		{
			auto const predicate = [](int value) noexcept {
				return value > 3;
			};
			if (::fast_io::algorithms::all_of_index_range(
					values, first, last, predicate) !=
				std::all_of(values.begin() + static_cast<std::ptrdiff_t>(first),
					values.begin() + static_cast<std::ptrdiff_t>(last),
					predicate))
			{
				return false;
			}
			if (::fast_io::algorithms::any_of_index_range(
					values, first, last, predicate) !=
				std::any_of(values.begin() + static_cast<std::ptrdiff_t>(first),
					values.begin() + static_cast<std::ptrdiff_t>(last),
					predicate))
			{
				return false;
			}
			if (::fast_io::algorithms::none_of_index_range(
					values, first, last, predicate) !=
				std::none_of(values.begin() + static_cast<std::ptrdiff_t>(first),
					values.begin() + static_cast<std::ptrdiff_t>(last),
					predicate))
			{
				return false;
			}
		}
	}
	return true;
}

bool check_forward_list() throws(true)
{
	std::forward_list<int> values{2, 4, 6};
	return ::fast_io::algorithms::all_of(values, is_even{}) &&
		!::fast_io::algorithms::any_of(values, [](int) noexcept { return false; }) &&
		::fast_io::algorithms::none_of(values, [](int) noexcept { return false; });
}

constexpr bool constexpr_test()
{
	int a[5]{2, 4, 6, 8, 10};
	if (!::fast_io::algorithms::all_of(
			a, [](int value) constexpr noexcept { return (value % 2) == 0; }))
	{
		return false;
	}
	if (!::fast_io::algorithms::any_of(
			a, [](int value) constexpr noexcept { return value == 6; }))
	{
		return false;
	}
	if (!::fast_io::algorithms::none_of(
			a, [](int value) constexpr noexcept { return value == 7; }))
	{
		return false;
	}
	if (::fast_io::algorithms::all_of_index_range(
			a, 1, 4, [](int value) constexpr noexcept { return value >= 4; }) != true)
	{
		return false;
	}
	return ::fast_io::algorithms::any_of_index_range(
		a, 0, 3, [](int value) constexpr noexcept { return value == 6; });
}

static_assert(constexpr_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::all_of(
	::std::declval<std::vector<int> &>(),
	[](int value) noexcept { return value == 1; })));
static_assert(noexcept(::fast_io::algorithms::any_of_index_range(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{},
	[](int value) noexcept { return value == 1; })));
static_assert(noexcept(::fast_io::algorithms::none_of(
	::std::declval<int (&)[4]>(),
	[](int value) noexcept { return value == 1; })));

int main()
{
	if (!check_all_of())
	{
		return 1;
	}
	if (!check_any_of())
	{
		return 2;
	}
	if (!check_none_of())
	{
		return 3;
	}
	if (!check_index_range())
	{
		return 4;
	}
	try
	{
		if (!check_forward_list())
		{
			return 5;
		}
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		return 6;
	}
	return 0;
}
