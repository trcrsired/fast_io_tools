#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <forward_list>
#include <list>
#include <string>
#include <vector>

namespace
{

struct move_only
{
	int value;
	move_only() = default;
	explicit move_only(int v) : value{v} {}
	move_only(move_only const &) = delete;
	move_only &operator=(move_only const &) = delete;
	move_only(move_only &&) noexcept = default;
	move_only &operator=(move_only &&) noexcept = default;
	friend bool operator==(move_only const &a, move_only const &b) noexcept
	{
		return a.value == b.value;
	}
};

std::uint64_t xorshift(std::uint64_t &state) noexcept
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return state;
}

template <typename T>
bool equal_prefix(std::vector<T> const &expected, std::vector<T> const &actual,
	std::size_t size)
{
	for (std::size_t i{}; i != size; ++i)
	{
		if (!(expected[i] == actual[i]))
		{
			return false;
		}
	}
	return true;
}

bool check_remove()
{
	for (std::size_t n{}; n != 80; ++n)
	{
		std::uint64_t state{0x1234567890abcdefull ^ n};
		std::vector<std::uint64_t> source(n);
		for (auto &value : source)
		{
			value = xorshift(state) % 13;
		}
		for (std::uint64_t value{}; value != 13; ++value)
		{
			std::vector<std::uint64_t> expected{source};
			auto const expected_end{
				std::remove(expected.begin(), expected.end(), value)};
			auto const expected_size{
				static_cast<std::size_t>(expected_end - expected.begin())};

			std::vector<std::uint64_t> actual{source};
			auto const actual_end{
				::fast_io::algorithms::remove(actual, value)};
			auto const actual_size{
				static_cast<std::size_t>(actual_end - actual.begin())};
			if (expected_size != actual_size)
			{
				return false;
			}
			if (!equal_prefix(expected, actual, expected_size))
			{
				return false;
			}
		}
	}
	return true;
}

bool check_remove_if()
{
	for (std::size_t n{}; n != 80; ++n)
	{
		std::uint64_t state{0xabcdef0123456789ull ^ n};
		std::vector<std::uint64_t> source(n);
		for (auto &value : source)
		{
			value = xorshift(state) % 29;
		}
		auto const predicate = [](std::uint64_t value) noexcept {
			return (value % 3) == 0;
		};
		std::vector<std::uint64_t> expected{source};
		auto const expected_end{
			std::remove_if(expected.begin(), expected.end(), predicate)};
		auto const expected_size{
			static_cast<std::size_t>(expected_end - expected.begin())};

		std::vector<std::uint64_t> actual{source};
		auto const actual_end{
			::fast_io::algorithms::remove_if(actual, predicate)};
		auto const actual_size{
			static_cast<std::size_t>(actual_end - actual.begin())};
		if (expected_size != actual_size)
		{
			return false;
		}
		if (!equal_prefix(expected, actual, expected_size))
		{
			return false;
		}
	}
	return true;
}

bool check_non_contiguous() throws(true)
{
	std::forward_list<int> values{1, 2, 3, 2, 4, 2, 5};
	auto const end{
		::fast_io::algorithms::remove_if(
			values, [](int value) noexcept { return value == 2; })};
	int const expected[]{1, 3, 4, 5};
	auto it{values.begin()};
	for (int value : expected)
	{
		if (it == end || *it != value)
		{
			return false;
		}
		++it;
	}
	if (it != end)
	{
		return false;
	}

	std::list<move_only> moves;
	for (int i{}; i != 12; ++i)
	{
		moves.push_back(move_only{i % 4});
	}
	auto const move_end{
		::fast_io::algorithms::remove(moves, move_only{2})};
	int expected_move[]{0, 1, 3, 0, 1, 3, 0, 1, 3};
	auto move_it{moves.begin()};
	for (int value : expected_move)
	{
		if (move_it == move_end || move_it->value != value)
		{
			return false;
		}
		++move_it;
	}
	return move_it == move_end;
}

bool check_index_range()
{
	for (std::size_t n{}; n != 40; ++n)
	{
		std::uint64_t state{0x5555aaaa5555aaaall ^ n};
		std::vector<std::uint64_t> source(n);
		for (auto &value : source)
		{
			value = xorshift(state) % 11;
		}
		for (std::size_t first{}; first <= n; ++first)
		{
			for (std::size_t last{first}; last <= n; ++last)
			{
				for (std::uint64_t value{}; value < 11; value += 5)
				{
					std::vector<std::uint64_t> expected{source};
					auto const expected_end{
						std::remove(expected.begin() +
							static_cast<std::ptrdiff_t>(first),
							expected.begin() +
							static_cast<std::ptrdiff_t>(last),
							value)};
					auto const expected_size{
						static_cast<std::size_t>(expected_end - expected.begin())};

					std::vector<std::uint64_t> actual{source};
					auto const actual_size{
						::fast_io::algorithms::remove_index_range(
							actual, first, last, value)};
					if (expected_size != actual_size)
					{
						return false;
					}
					if (!equal_prefix(expected, actual, expected_size))
					{
						return false;
					}
				}

				std::vector<std::uint64_t> expected{source};
				auto const expected_end{std::remove_if(
					expected.begin() + static_cast<std::ptrdiff_t>(first),
					expected.begin() + static_cast<std::ptrdiff_t>(last),
					[](std::uint64_t v) noexcept { return (v % 2) == 0; })};
				auto const expected_size{
					static_cast<std::size_t>(expected_end - expected.begin())};

				std::vector<std::uint64_t> actual{source};
				auto const actual_size{
					::fast_io::algorithms::remove_if_index_range(
						actual, first, last,
						[](std::uint64_t v) noexcept { return (v % 2) == 0; })};
				if (expected_size != actual_size)
				{
					return false;
				}
				if (!equal_prefix(expected, actual, expected_size))
				{
					return false;
				}
			}
		}
	}
	return true;
}

constexpr bool constexpr_test()
{
	int a[8]{1, 2, 1, 3, 1, 4, 1, 5};
	auto const end{::fast_io::algorithms::remove(a, 1)};
	if (end != a + 4)
	{
		return false;
	}
	int const expected[]{2, 3, 4, 5};
	for (int i{}; i != 4; ++i)
	{
		if (a[i] != expected[i])
		{
			return false;
		}
	}

	int b[6]{1, 2, 3, 4, 5, 6};
	auto const if_end{::fast_io::algorithms::remove_if(
		b, [](int value) constexpr noexcept { return (value % 2) == 0; })};
	if (if_end != b + 3)
	{
		return false;
	}
	int const if_expected[]{1, 3, 5};
	for (int i{}; i != 3; ++i)
	{
		if (b[i] != if_expected[i])
		{
			return false;
		}
	}

	int c[5]{1, 2, 3, 4, 5};
	if (::fast_io::algorithms::remove_index_range(c, 1, 4, 3) != 3)
	{
		return false;
	}
	return c[1] == 2 && c[2] == 4;
}

static_assert(constexpr_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::remove(
	::std::declval<std::vector<int> &>(), 1)));
static_assert(noexcept(::fast_io::algorithms::remove_if(
	::std::declval<std::vector<int> &>(),
	[](int value) noexcept { return value == 1; })));
static_assert(noexcept(::fast_io::algorithms::remove_index_range(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{}, 1)));

int main()
{
	if (!check_remove())
	{
		return 1;
	}
	if (!check_remove_if())
	{
		return 2;
	}
	try
	{
		if (!check_non_contiguous())
		{
			return 3;
		}
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		return 5;
	}
	if (!check_index_range())
	{
		return 4;
	}
	return 0;
}
