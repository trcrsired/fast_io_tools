#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <deque>
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

struct big_trivial
{
	std::uint64_t key;
	std::uint64_t payload[3];

	friend bool operator==(big_trivial const &, big_trivial const &) noexcept = default;
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
bool equal_ranges(std::vector<T> const &a, std::vector<T> const &b)
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (std::size_t i{}; i != a.size(); ++i)
	{
		if (!(a[i] == b[i]))
		{
			return false;
		}
	}
	return true;
}

template <typename T>
bool check_vector(std::vector<T> const &source)
{
	std::vector<T> expected{source};
	std::reverse(expected.begin(), expected.end());
	std::vector<T> actual{source};
	::fast_io::algorithms::reverse(actual);
	return equal_ranges(expected, actual);
}

bool check_all_sizes()
{
	for (std::size_t n{}; n != 70; ++n)
	{
		std::vector<std::uint64_t> source(n);
		std::uint64_t state{0x9e3779b97f4a7c15ull ^ n};
		for (auto &value : source)
		{
			value = xorshift(state);
		}
		if (!check_vector(source))
		{
			return false;
		}

		std::vector<rec16> source16(n);
		for (auto &value : source16)
		{
			value.key = xorshift(state);
			value.payload = xorshift(state);
		}
		if (!check_vector(source16))
		{
			return false;
		}

		std::vector<big_trivial> source_big(n);
		for (auto &value : source_big)
		{
			value.key = xorshift(state);
			for (auto &word : value.payload)
			{
				word = xorshift(state);
			}
		}
		if (!check_vector(source_big))
		{
			return false;
		}

		std::vector<std::string> source_string(n);
		for (std::size_t i{}; i != n; ++i)
		{
			source_string[i] = ::std::to_string(xorshift(state));
		}
		if (!check_vector(source_string))
		{
			return false;
		}

		std::vector<move_only> source_move(n);
		for (std::size_t i{}; i != n; ++i)
		{
			source_move[i] = move_only{static_cast<int>(i)};
		}
		std::vector<move_only> expected_move;
		expected_move.reserve(n);
		for (std::size_t i{}; i != n; ++i)
		{
			expected_move.push_back(move_only{static_cast<int>(n - 1 - i)});
		}
		::fast_io::algorithms::reverse(source_move);
		if (source_move.size() != expected_move.size())
		{
			return false;
		}
		for (std::size_t i{}; i != n; ++i)
		{
			if (source_move[i].value != expected_move[i].value)
			{
				return false;
			}
		}
	}
	return true;
}

bool check_index()
{
	std::uint64_t state{0x1234567890abcdefull};
	for (std::size_t n{}; n != 40; ++n)
	{
		std::vector<std::uint64_t> source(n);
		for (auto &value : source)
		{
			value = xorshift(state);
		}
		for (std::size_t first{}; first <= n; ++first)
		{
			for (std::size_t last{first}; last <= n; ++last)
			{
				std::vector<std::uint64_t> expected{source};
				std::reverse(expected.begin() + static_cast<std::ptrdiff_t>(first),
					expected.begin() + static_cast<std::ptrdiff_t>(last));
				std::vector<std::uint64_t> actual{source};
				::fast_io::algorithms::reverse_index(actual, first, last);
				if (!equal_ranges(expected, actual))
				{
					return false;
				}
			}
		}
	}
	return true;
}

constexpr bool constexpr_reverse_test()
{
	int a[9]{1, 2, 3, 4, 5, 6, 7, 8, 9};
	::fast_io::algorithms::reverse(a);
	if (a[0] != 9 || a[4] != 5 || a[8] != 1)
	{
		return false;
	}
	::fast_io::algorithms::reverse_index(a, 1, 8);
	return a[0] == 9 && a[1] == 2 && a[7] == 8 && a[8] == 1;
}

constexpr bool constexpr_reverse_empty_test()
{
	int a[1]{42};
	::fast_io::algorithms::reverse_iters(a, a);
	::fast_io::algorithms::reverse_iters(a, a + 1);
	return a[0] == 42;
}

static_assert(constexpr_reverse_test());
static_assert(constexpr_reverse_empty_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::reverse(
	::std::declval<int (&)[8]>())));
static_assert(noexcept(::fast_io::algorithms::reverse(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::reverse(
	::std::declval<std::vector<std::string> &>())));
static_assert(noexcept(::fast_io::algorithms::reverse_iters(
	::std::declval<int *>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::reverse_index(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{})));
static_assert(!noexcept(::fast_io::algorithms::reverse(
	::std::declval<std::list<int> &>())));
static_assert(!noexcept(::fast_io::algorithms::reverse(
	::std::declval<std::deque<int> &>())));

int main()
{
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
		std::list<int> list_values;
		for (int i{}; i != 9; ++i)
		{
			list_values.push_back(i);
		}
		::fast_io::algorithms::reverse(list_values);
		int expected{8};
		for (auto const value : list_values)
		{
			if (value != expected)
			{
				return 3;
			}
			--expected;
		}

		std::deque<int> deque_values;
		for (int i{}; i != 9; ++i)
		{
			deque_values.push_back(i);
		}
		::fast_io::algorithms::reverse(deque_values);
		expected = 8;
		for (auto const value : deque_values)
		{
			if (value != expected)
			{
				return 4;
			}
			--expected;
		}
		::fast_io::algorithms::reverse_index(deque_values, 1, 8);
	}
	catch throws(::std::error)
	{
		return 5;
	}
	return 0;
}
