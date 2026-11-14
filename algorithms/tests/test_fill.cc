#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstdint>
#include <deque>
#include <list>
#include <string>
#include <vector>

namespace
{

struct pair16
{
	std::uint64_t a;
	std::uint64_t b;

	friend bool operator==(pair16 const &, pair16 const &) noexcept = default;
};

constexpr bool constexpr_fill_test()
{
	int a[8]{};
	auto const result{::fast_io::algorithms::fill_iters(a, a + 8, 42)};
	if (result != a + 8)
	{
		return false;
	}
	for (auto const value : a)
	{
		if (value != 42)
		{
			return false;
		}
	}
	::fast_io::algorithms::fill_iters(a + 2, a + 6, -7);
	return a[0] == 42 && a[1] == 42 && a[2] == -7 && a[5] == -7 &&
		   a[6] == 42 && a[7] == 42;
}

static_assert(constexpr_fill_test());

constexpr bool constexpr_fill_n_test()
{
	char s[9]{};
	auto const result{::fast_io::algorithms::fill_n_iters(s, 9, 'x')};
	if (result != s + 9)
	{
		return false;
	}
	for (auto const value : s)
	{
		if (value != 'x')
		{
			return false;
		}
	}
	::fast_io::algorithms::fill_n_iters(s, 3, '\n');
	return s[0] == '\n' && s[2] == '\n' && s[3] == 'x' && s[8] == 'x';
}

static_assert(constexpr_fill_n_test());

constexpr bool constexpr_fill_index_test()
{
	int a[8]{1, 2, 3, 4, 5, 6, 7, 8};
	::fast_io::algorithms::fill_index(a, 1, 5, 0);
	if (a[0] != 1 || a[1] != 0 || a[4] != 0 || a[5] != 6 || a[7] != 8)
	{
		return false;
	}
	::fast_io::algorithms::fill_index(a, 0, 0, 99);
	return a[0] == 1;
}

static_assert(constexpr_fill_index_test());

bool check_index()
{
	for (std::size_t n{}; n != 40; ++n)
	{
		std::vector<std::uint64_t> source(n);
		for (std::size_t i{}; i != n; ++i)
		{
			source[i] = i * 0x9e3779b97f4a7c15ull;
		}
		for (std::size_t first{}; first <= n; ++first)
		{
			for (std::size_t last{first}; last <= n; ++last)
			{
				std::vector<std::uint64_t> actual{source};
				std::vector<std::uint64_t> expected{source};
				::fast_io::algorithms::fill_index(actual, first, last, 0xdeadbeefull);
				for (std::size_t i{first}; i != last; ++i)
				{
					expected[i] = 0xdeadbeefull;
				}
				if (actual != expected)
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool check_byte_fill()
{
	std::int8_t bytes[37];
	for (auto &value : bytes)
	{
		value = 5;
	}
	::fast_io::algorithms::fill(bytes, static_cast<std::int8_t>(-1));
	for (auto const value : bytes)
	{
		if (value != -1)
		{
			return false;
		}
	}

	char chars[9]{};
	::fast_io::algorithms::fill_n_iters(chars, 9, static_cast<char>(0x80));
	for (auto const value : chars)
	{
		if (static_cast<unsigned char>(value) != 0x80)
		{
			return false;
		}
	}

	std::byte raw[16]{};
	::fast_io::algorithms::fill(raw, std::byte{0xab});
	for (auto const value : raw)
	{
		if (value != std::byte{0xab})
		{
			return false;
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::fill(
	::std::declval<int (&)[8]>(), 1)));
static_assert(noexcept(::fast_io::algorithms::fill(
	::std::declval<std::vector<int> &>(), 1)));
static_assert(!noexcept(::fast_io::algorithms::fill(
	::std::declval<std::vector<std::string> &>(),
	::std::declval<std::string const &>())));
static_assert(noexcept(::fast_io::algorithms::fill_n(
	::std::declval<std::vector<int> &>(), std::size_t{}, 1)));
static_assert(noexcept(::fast_io::algorithms::fill_index(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{}, 1)));
static_assert(!noexcept(::fast_io::algorithms::fill(
	::std::declval<std::deque<int> &>(), 1)));
static_assert(!noexcept(::fast_io::algorithms::fill(
	::std::declval<std::list<int> &>(), 1)));

int main()
{
	int a[8]{1, 2, 3, 4, 5, 6, 7, 8};
	if (::fast_io::algorithms::fill_iters(a + 2, a + 5, 0) != a + 5)
	{
		return 1;
	}
	if (a[1] != 2 || a[2] != 0 || a[4] != 0 || a[5] != 6)
	{
		return 2;
	}

	std::vector<int> v(10, 3);
	if (::fast_io::algorithms::fill(v, 9) != v.end())
	{
		return 3;
	}
	for (auto const value : v)
	{
		if (value != 9)
		{
			return 4;
		}
	}
	if (::fast_io::algorithms::fill_n(v, 4, 1) != v.begin() + 4)
	{
		return 5;
	}
	for (std::size_t i{}; i != v.size(); ++i)
	{
		if (v[i] != (i < 4 ? 1 : 9))
		{
			return 6;
		}
	}

	std::vector<pair16> records(6);
	::fast_io::algorithms::fill(records, pair16{11, 12});
	for (auto const &record : records)
	{
		if (!(record == pair16{11, 12}))
		{
			return 7;
		}
	}

	std::string text{"hello world"};
	::fast_io::algorithms::fill(text, 'z');
	if (text != "zzzzzzzzzzz")
	{
		return 8;
	}

	if (!check_byte_fill())
	{
		return 9;
	}
	if (!check_index())
	{
		return 10;
	}

	try
	{
		std::deque<int> deque_values(5, 1);
		::fast_io::algorithms::fill(deque_values, 7);
		for (auto const value : deque_values)
		{
			if (value != 7)
			{
				return 11;
			}
		}

		std::list<int> list_values(5, 1);
		::fast_io::algorithms::fill(list_values, 7);
		for (auto const value : list_values)
		{
			if (value != 7)
			{
				return 12;
			}
		}
	}
	catch throws(::std::error)
	{
		return 13;
	}
	return 0;
}
