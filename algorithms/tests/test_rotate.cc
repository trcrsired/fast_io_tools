#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <deque>
#include <forward_list>
#include <list>
#include <string>
#include <vector>

namespace
{

struct rec16
{
	std::uint64_t key;
	std::uint64_t payload;

	friend bool operator==(rec16 const &, rec16 const &) noexcept = default;
};

struct huge_trivial
{
	std::uint64_t key;
	std::uint64_t payload[15];

	friend constexpr bool operator==(huge_trivial const &, huge_trivial const &) noexcept = default;
};

struct move_only
{
	int value;
	move_only() = default;
	explicit move_only(int v) : value{v} {}
	move_only(move_only const &) = delete;
	move_only &operator=(move_only const &) = delete;
	move_only(move_only &&) noexcept = default;
	move_only &operator=(move_only &&) noexcept = default;
};

std::uint64_t xorshift(std::uint64_t &state) noexcept
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return state;
}

template <typename T>
bool check_rotate_vector(std::vector<T> const &source, std::size_t middle)
{
	std::vector<T> expected{source};
	std::rotate(expected.begin(),
		expected.begin() + static_cast<std::ptrdiff_t>(middle), expected.end());
	std::vector<T> actual{source};
	auto const result{
		::fast_io::algorithms::rotate(actual, actual.begin() +
			static_cast<std::ptrdiff_t>(middle))};
	if (!(actual == expected))
	{
		return false;
	}
	// The returned iterator must point at the old first element.
	return static_cast<std::size_t>(result - actual.begin()) ==
		source.size() - middle;
}

bool check_all_sizes()
{
	std::uint64_t state{0x9e3779b97f4a7c15ull};
	for (std::size_t n{}; n != 90; ++n)
	{
		std::vector<rec16> source16(n);
		for (auto &value : source16)
		{
			value.key = xorshift(state);
			value.payload = xorshift(state);
		}
		std::vector<huge_trivial> source_big(n);
		for (auto &value : source_big)
		{
			value.key = xorshift(state);
			for (auto &word : value.payload)
			{
				word = xorshift(state);
			}
		}
		for (std::size_t middle{}; middle <= n; ++middle)
		{
			if (!check_rotate_vector(source16, middle))
			{
				return false;
			}
			if (!check_rotate_vector(source_big, middle))
			{
				return false;
			}
		}
	}

	// Larger inputs take the swap path instead of the small-side buffer.
	std::vector<std::uint64_t> source64(1000);
	for (std::size_t i{}; i != source64.size(); ++i)
	{
		source64[i] = xorshift(state);
	}
	for (std::size_t middle{}; middle <= source64.size(); middle += 7)
	{
		if (!check_rotate_vector(source64, middle))
		{
			return false;
		}
	}
	if (!check_rotate_vector(source64, source64.size() - 1))
	{
		return false;
	}

	for (std::size_t n{}; n != 30; ++n)
	{
		std::vector<std::string> source(n);
		for (auto &value : source)
		{
			value = ::std::to_string(xorshift(state));
		}
		for (std::size_t middle{}; middle <= n; ++middle)
		{
			if (!check_rotate_vector(source, middle))
			{
				return false;
			}
		}
	}

	for (std::size_t n{}; n != 40; ++n)
	{
		std::vector<move_only> source;
		source.reserve(n);
		for (std::size_t i{}; i != n; ++i)
		{
			source.emplace_back(static_cast<int>(i));
		}
		for (std::size_t middle{}; middle <= n; ++middle)
		{
			std::vector<move_only> actual;
			actual.reserve(n);
			for (auto &value : source)
			{
				actual.push_back(move_only{value.value});
			}
			auto const result{
				::fast_io::algorithms::rotate(actual, actual.begin() +
					static_cast<std::ptrdiff_t>(middle))};
			if (static_cast<std::size_t>(result - actual.begin()) != n - middle)
			{
				return false;
			}
			for (std::size_t i{}; i != n; ++i)
			{
				if (actual[i].value != static_cast<int>((i + middle) % n) &&
					!(n == 0))
				{
					return false;
				}
			}
		}
	}
	return true;
}

template <typename T>
bool check_index_range(std::vector<T> const &source)
{
	for (std::size_t first{}; first <= source.size(); ++first)
	{
		for (std::size_t middle{first}; middle <= source.size(); ++middle)
		{
			for (std::size_t last{middle}; last <= source.size(); ++last)
			{
				std::vector<T> expected{source};
				std::rotate(
					expected.begin() + static_cast<std::ptrdiff_t>(first),
					expected.begin() + static_cast<std::ptrdiff_t>(middle),
					expected.begin() + static_cast<std::ptrdiff_t>(last));
				std::vector<T> actual{source};
				auto const result{
					::fast_io::algorithms::rotate_index(actual, first, middle, last)};
				if (!(actual == expected))
				{
					return false;
				}
				if (result != first + (last - middle))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool check_index()
{
	std::uint64_t state{0x1234567890abcdefull};
	for (std::size_t n{}; n != 16; ++n)
	{
		std::vector<std::uint64_t> source(n);
		for (auto &value : source)
		{
			value = xorshift(state);
		}
		if (!check_index_range(source))
		{
			return false;
		}
	}
	std::vector<std::string> strings(9);
	for (auto &value : strings)
	{
		value = ::std::to_string(xorshift(state));
	}
	return check_index_range(strings);
}

constexpr bool constexpr_rotate_gcd_test()
{
	int a[10]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto const result{::fast_io::algorithms::rotate_iters(a, a + 3, a + 10)};
	if (result != a + 7)
	{
		return false;
	}
	int const expected[10]{3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
	for (int i{}; i != 10; ++i)
	{
		if (a[i] != expected[i])
		{
			return false;
		}
	}
	return true;
}

constexpr bool constexpr_rotate_swap_test()
{
	int a[40]{};
	for (int i{}; i != 40; ++i)
	{
		a[i] = i;
	}
	auto const result{::fast_io::algorithms::rotate_iters(a, a + 13, a + 40)};
	if (result != a + 27)
	{
		return false;
	}
	for (int i{}; i != 40; ++i)
	{
		if (a[i] != (i + 13) % 40)
		{
			return false;
		}
	}
	return true;
}

constexpr bool constexpr_rotate_big_test()
{
	huge_trivial a[6]{};
	for (int i{}; i != 6; ++i)
	{
		a[i].key = static_cast<std::uint64_t>(i);
		for (int j{}; j != 15; ++j)
		{
			a[i].payload[j] = static_cast<std::uint64_t>(i * 100 + j);
		}
	}
	::fast_io::algorithms::rotate_iters(a, a + 2, a + 6);
	for (int i{}; i != 6; ++i)
	{
		int const original{(i + 2) % 6};
		if (a[i] != huge_trivial{static_cast<std::uint64_t>(original),
			{static_cast<std::uint64_t>(original * 100 + 0),
				static_cast<std::uint64_t>(original * 100 + 1),
				static_cast<std::uint64_t>(original * 100 + 2),
				static_cast<std::uint64_t>(original * 100 + 3),
				static_cast<std::uint64_t>(original * 100 + 4),
				static_cast<std::uint64_t>(original * 100 + 5),
				static_cast<std::uint64_t>(original * 100 + 6),
				static_cast<std::uint64_t>(original * 100 + 7),
				static_cast<std::uint64_t>(original * 100 + 8),
				static_cast<std::uint64_t>(original * 100 + 9),
				static_cast<std::uint64_t>(original * 100 + 10),
				static_cast<std::uint64_t>(original * 100 + 11),
				static_cast<std::uint64_t>(original * 100 + 12),
				static_cast<std::uint64_t>(original * 100 + 13),
				static_cast<std::uint64_t>(original * 100 + 14)}})
		{
			return false;
		}
	}
	return true;
}

constexpr bool constexpr_rotate_index_test()
{
	int a[7]{0, 1, 2, 3, 4, 5, 6};
	auto const result{::fast_io::algorithms::rotate_index(a, 1, 3, 6)};
	if (result != 4)
	{
		return false;
	}
	int const expected[7]{0, 3, 4, 5, 1, 2, 6};
	for (int i{}; i != 7; ++i)
	{
		if (a[i] != expected[i])
		{
			return false;
		}
	}
	return true;
}

constexpr bool constexpr_rotate_empty_test()
{
	int a[1]{42};
	if (::fast_io::algorithms::rotate_iters(a, a, a) != a)
	{
		return false;
	}
	if (::fast_io::algorithms::rotate_iters(a, a + 1, a + 1) != a)
	{
		return false;
	}
	return a[0] == 42;
}

static_assert(constexpr_rotate_gcd_test());
static_assert(constexpr_rotate_swap_test());
static_assert(constexpr_rotate_big_test());
static_assert(constexpr_rotate_index_test());
static_assert(constexpr_rotate_empty_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::rotate(
	::std::declval<int (&)[8]>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::rotate(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int>::iterator>())));
static_assert(noexcept(::fast_io::algorithms::rotate(
	::std::declval<std::vector<std::string> &>(),
	::std::declval<std::vector<std::string>::iterator>())));
static_assert(noexcept(::fast_io::algorithms::rotate_iters(
	::std::declval<int *>(), ::std::declval<int *>(),
	::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::rotate_index(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{},
	std::size_t{})));
static_assert(!noexcept(::fast_io::algorithms::rotate(
	::std::declval<std::list<int> &>(), ::std::declval<std::list<int>::iterator>())));
static_assert(!noexcept(::fast_io::algorithms::rotate(
	::std::declval<std::deque<int> &>(),
	::std::declval<std::deque<int>::iterator>())));
static_assert(!noexcept(::fast_io::algorithms::rotate(
	::std::declval<std::forward_list<int> &>(),
	::std::declval<std::forward_list<int>::iterator>())));


static_assert(noexcept(::fast_io::algorithms::rotate_copy(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int> &>().begin(),
	::std::declval<int *>())));

int main()
{
	{
		int const source[]{1, 2, 3, 4};
		int out[4]{};
		auto const end{::fast_io::algorithms::rotate_copy_iters(
			source, source + 2, source + 4, out)};
		if (end != out + 4 || out[0] != 3 || out[1] != 4 ||
			out[2] != 1 || out[3] != 2)
		{
			return 9;
		}
	}
	if (!check_all_sizes())
	{
		return 1;
	}
	if (!check_index())
	{
		return 2;
	}

	try
	{
		std::deque<int> deque_values;
		for (int i{}; i != 17; ++i)
		{
			deque_values.push_back(i);
		}
		auto const result{
			::fast_io::algorithms::rotate(deque_values, deque_values.begin() + 5)};
		if (static_cast<std::size_t>(result - deque_values.begin()) != 12)
		{
			return 3;
		}
		for (int i{}; i != 17; ++i)
		{
			if (deque_values[static_cast<std::size_t>(i)] != (i + 5) % 17)
			{
				return 4;
			}
		}

		std::list<int> list_values;
		for (int i{}; i != 11; ++i)
		{
			list_values.push_back(i);
		}
		auto list_middle{list_values.begin()};
		for (int i{}; i != 4; ++i)
		{
			++list_middle;
		}
		auto const list_result{
			::fast_io::algorithms::rotate(list_values, list_middle)};
		int expected{4};
		for (auto const value : list_values)
		{
			if (value != expected % 11)
			{
				return 5;
			}
			++expected;
		}
		if (*list_result != 0)
		{
			return 6;
		}

		std::forward_list<int> forward_values;
		auto forward_it{forward_values.before_begin()};
		for (int i{}; i != 11; ++i)
		{
			forward_it = forward_values.insert_after(forward_it, i);
		}
		auto forward_middle{forward_values.begin()};
		for (int i{}; i != 7; ++i)
		{
			++forward_middle;
		}
		auto const forward_result{
			::fast_io::algorithms::rotate(forward_values, forward_middle)};
		expected = 7;
		for (auto const value : forward_values)
		{
			if (value != expected % 11)
			{
				return 7;
			}
			++expected;
		}
		if (*forward_result != 0)
		{
			return 8;
		}

		::fast_io::algorithms::rotate_index(deque_values, 2, 6, 12);
	}
	catch throws(::std::error)
	{
		return 9;
	}
	return 0;
}
