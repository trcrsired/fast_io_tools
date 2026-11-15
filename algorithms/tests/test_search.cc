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

struct compare_only
{
	template <typename T, typename U>
	constexpr auto compare(T const &a, U const &b) const noexcept
	{
		return a <=> b;
	}
};

std::uint64_t xorshift(std::uint64_t &state) noexcept
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return state;
}

bool check_search_families()
{
	for (std::size_t n{}; n != 60; ++n)
	{
		std::uint64_t state{0x0123456789abcdefull ^ n};
		std::vector<std::uint64_t> haystack(n);
		for (auto &value : haystack)
		{
			value = xorshift(state) % 7;
		}
		for (std::size_t m{}; m != 7; ++m)
		{
			std::vector<std::uint64_t> needle(m);
			for (auto &value : needle)
			{
				value = xorshift(state) % 7;
			}
			auto const expected_search{
				std::search(haystack.begin(), haystack.end(),
					needle.begin(), needle.end())};
			auto const actual_search{
				::fast_io::algorithms::search(haystack, needle)};
			if (expected_search - haystack.begin() !=
				actual_search - haystack.begin())
			{
				return false;
			}

			auto const expected_end{
				std::find_end(haystack.begin(), haystack.end(),
					needle.begin(), needle.end())};
			auto const actual_end{
				::fast_io::algorithms::find_end(haystack, needle)};
			if (expected_end - haystack.begin() !=
				actual_end - haystack.begin())
			{
				return false;
			}

			auto const expected_first{
				std::find_first_of(haystack.begin(), haystack.end(),
					needle.begin(), needle.end())};
			auto const actual_first{
				::fast_io::algorithms::find_first_of(haystack, needle)};
			if (expected_first - haystack.begin() !=
				actual_first - haystack.begin())
			{
				return false;
			}
		}

		auto const expected_adjacent{
			std::adjacent_find(haystack.begin(), haystack.end())};
		auto const actual_adjacent{
			::fast_io::algorithms::adjacent_find(haystack)};
		if (expected_adjacent - haystack.begin() !=
			actual_adjacent - haystack.begin())
		{
			return false;
		}

		for (std::size_t count{}; count != 5; ++count)
		{
			for (std::uint64_t value{}; value != 7; ++value)
			{
				auto const expected{
					std::search_n(haystack.begin(), haystack.end(),
						count, value)};
				auto const actual{
					::fast_io::algorithms::search_n(
						haystack, count, value)};
				if (expected - haystack.begin() !=
					actual - haystack.begin())
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool check_index_range()
{
	std::vector<std::uint64_t> haystack{
		1, 2, 1, 2, 1, 2, 3, 1, 2, 4};
	std::vector<std::uint64_t> needle{1, 2};
	for (std::size_t f1{}; f1 <= haystack.size(); ++f1)
	{
		for (std::size_t l1{f1}; l1 <= haystack.size(); ++l1)
		{
			auto const hb{
				haystack.begin() + static_cast<std::ptrdiff_t>(f1)};
			auto const he{
				haystack.begin() + static_cast<std::ptrdiff_t>(l1)};
			if (::fast_io::algorithms::search_index_range(
					haystack, f1, l1, needle, 0, needle.size()) !=
				f1 + static_cast<std::size_t>(
					std::search(hb, he, needle.begin(), needle.end()) - hb))
			{
				return false;
			}
			if (::fast_io::algorithms::find_end_index_range(
					haystack, f1, l1, needle, 0, needle.size()) !=
				f1 + static_cast<std::size_t>(
					std::find_end(hb, he, needle.begin(), needle.end()) - hb))
			{
				return false;
			}
			if (::fast_io::algorithms::find_first_of_index_range(
					haystack, f1, l1, needle, 0, 1) !=
				f1 + static_cast<std::size_t>(
					std::find_first_of(hb, he, needle.begin(),
						needle.begin() + 1) - hb))
			{
				return false;
			}
			if (::fast_io::algorithms::adjacent_find_index_range(
					haystack, f1, l1) !=
				f1 + static_cast<std::size_t>(
					std::adjacent_find(hb, he) - hb))
			{
				return false;
			}
			for (std::size_t count{}; count != 4; ++count)
			{
				if (::fast_io::algorithms::search_n_index_range(
						haystack, f1, l1, count, std::uint64_t{1}) !=
					f1 + static_cast<std::size_t>(
						std::search_n(hb, he, count, std::uint64_t{1}) - hb))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool check_forward_list() throws(true)
{
	std::forward_list<int> haystack{1, 2, 3, 4, 3, 4, 5};
	std::forward_list<int> needle{3, 4};
	auto const searched{::fast_io::algorithms::search(haystack, needle)};
	auto const last{::fast_io::algorithms::find_end(haystack, needle)};
	auto const first{::fast_io::algorithms::find_first_of(
		haystack, std::forward_list<int>{4, 9})};
	auto const adjacent{::fast_io::algorithms::adjacent_find(
		haystack, ::fast_io::algorithms::compare_three_way{})};
	auto const repeated{::fast_io::algorithms::search_n(haystack, 2, 3)};
	return *searched == 3 && *last == 3 && *first == 4 &&
		adjacent == haystack.end() && repeated == haystack.end();
}

constexpr bool constexpr_test()
{
	int haystack[8]{1, 2, 3, 1, 2, 3, 4, 5};
	int needle[2]{1, 2};
	if (::fast_io::algorithms::search_iters(
			haystack, haystack + 8, needle, needle + 2) != haystack)
	{
		return false;
	}
	if (::fast_io::algorithms::find_end_iters(
			haystack, haystack + 8, needle, needle + 2) != haystack + 3)
	{
		return false;
	}
	if (::fast_io::algorithms::find_first_of_iters(
			haystack, haystack + 8, haystack + 6, haystack + 7) !=
		haystack + 6)
	{
		return false;
	}
	if (::fast_io::algorithms::adjacent_find_iters(
			haystack, haystack + 8) != haystack + 8)
	{
		return false;
	}
	if (::fast_io::algorithms::search_n_iters(
			haystack, haystack + 8, 1, 3) != haystack + 2)
	{
		return false;
	}
	if (::fast_io::algorithms::search_n_iters(
			haystack, haystack + 8, 3, 3) != haystack + 8)
	{
		return false;
	}
	if (::fast_io::algorithms::search_index_range(
			haystack, 1, 8, needle, 0, 2) != 3)
	{
		return false;
	}
	if (::fast_io::algorithms::find_end_index_range(
			haystack, 0, 6, needle, 0, 2) != 3)
	{
		return false;
	}
	return ::fast_io::algorithms::search_n_index_range(
		haystack, 1, 6, 1, 3) == 2;
}

static_assert(constexpr_test());

constexpr bool compare_only_test()
{
	int haystack[5]{1, 2, 1, 2, 3};
	int needle[2]{1, 2};
	return ::fast_io::algorithms::search_iters(
			haystack, haystack + 5, needle, needle + 2,
			compare_only{}) == haystack &&
		::fast_io::algorithms::find_end_iters(
			haystack, haystack + 5, needle, needle + 2,
			compare_only{}) == haystack + 2 &&
		::fast_io::algorithms::search_n_iters(
			haystack, haystack + 5, 1, 3, compare_only{}) ==
			haystack + 4;
}

static_assert(compare_only_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::search(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::find_end(
	::std::declval<int (&)[8]>(), ::std::declval<int (&)[2]>())));
static_assert(noexcept(::fast_io::algorithms::find_first_of(
	::std::declval<int (&)[8]>(), ::std::declval<int (&)[2]>())));
static_assert(noexcept(::fast_io::algorithms::adjacent_find(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::search_n(
	::std::declval<std::vector<int> &>(), std::size_t{}, 1)));
static_assert(noexcept(::fast_io::algorithms::search_index_range(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{},
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{})));

int main()
{
	if (!check_search_families())
	{
		return 1;
	}
	if (!check_index_range())
	{
		return 2;
	}
	try
	{
		if (!check_forward_list())
		{
			return 3;
		}
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		return 4;
	}
	return 0;
}
