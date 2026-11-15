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

std::uint64_t xorshift(std::uint64_t &state) noexcept
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return state;
}

bool check_count()
{
	for (std::size_t n{}; n != 80; ++n)
	{
		std::uint64_t state{0x123456789abcdefull ^ n};
		std::vector<std::uint64_t> values(n);
		for (auto &value : values)
		{
			value = xorshift(state) % 17;
		}
		for (std::uint64_t target{}; target != 17; ++target)
		{
			auto const expected{
				std::count(values.begin(), values.end(), target)};
			auto const actual{
				::fast_io::algorithms::count(values, target)};
			if (expected != actual)
			{
				return false;
			}
		}
	}
	return true;
}

bool check_count_if()
{
	for (std::size_t n{}; n != 80; ++n)
	{
		std::uint64_t state{0xfedcba987654321ull ^ n};
		std::vector<std::uint64_t> values(n);
		for (auto &value : values)
		{
			value = xorshift(state) % 31;
		}
		auto const predicate = [](std::uint64_t value) noexcept {
			return (value % 3) == 0 || value > 20;
		};
		auto const expected{
			std::count_if(values.begin(), values.end(), predicate)};
		auto const actual{
			::fast_io::algorithms::count_if(values, predicate)};
		if (expected != actual)
		{
			return false;
		}
	}
	return true;
}

bool check_index()
{
	std::vector<std::uint64_t> values{0, 1, 2, 1, 3, 1, 4, 1, 5};
	for (std::size_t first{}; first <= values.size(); ++first)
	{
		for (std::size_t last{first}; last <= values.size(); ++last)
		{
			auto const begin{
				values.begin() + static_cast<std::ptrdiff_t>(first)};
			auto const end{
				values.begin() + static_cast<std::ptrdiff_t>(last)};
			if (::fast_io::algorithms::count_index(
					values, first, last, 1) !=
				static_cast<std::size_t>(std::count(begin, end, 1)))
			{
				return false;
			}
			auto const predicate = [](std::uint64_t value) noexcept {
				return (value % 2) == 0;
			};
			if (::fast_io::algorithms::count_if_index(
					values, first, last, predicate) !=
				static_cast<std::size_t>(
					std::count_if(begin, end, predicate)))
			{
				return false;
			}
		}
	}
	return true;
}

bool check_forward_list() throws(true)
{
	std::forward_list<int> values{1, 2, 1, 3, 1};
	return ::fast_io::algorithms::count(values, 1) == 3 &&
		::fast_io::algorithms::count_if(
			values, [](int value) noexcept { return value != 1; }) == 2;
}

constexpr bool constexpr_test()
{
	int a[8]{1, 2, 1, 3, 1, 4, 1, 5};
	if (::fast_io::algorithms::count(a, 1) != 4)
	{
		return false;
	}
	if (::fast_io::algorithms::count_index(a, 1, 7, 1) != 3)
	{
		return false;
	}
	if (::fast_io::algorithms::count_if(
			a, [](int value) constexpr noexcept { return (value % 2) == 0; }) != 2)
	{
		return false;
	}
	return ::fast_io::algorithms::count_if_index(
		a, 0, 4,
		[](int value) constexpr noexcept { return value > 1; }) == 2;
}

static_assert(constexpr_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::count(
	::std::declval<std::vector<int> &>(), 1)));
static_assert(noexcept(::fast_io::algorithms::count_if(
	::std::declval<std::vector<int> &>(),
	[](int value) noexcept { return value == 1; })));
static_assert(noexcept(::fast_io::algorithms::count_index(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{}, 1)));

int main()
{
	if (!check_count())
	{
		return 1;
	}
	if (!check_count_if())
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
