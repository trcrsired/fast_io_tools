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

bool check_mismatch()
{
	for (std::size_t n1{}; n1 != 30; ++n1)
	{
		std::uint64_t state{0xabcdef0123456789ull ^ n1};
		std::vector<std::uint64_t> a(n1);
		for (auto &value : a)
		{
			value = xorshift(state) % 5;
		}
		for (std::size_t n2{}; n2 != 30; ++n2)
		{
			std::vector<std::uint64_t> b(n2);
			for (auto &value : b)
			{
				value = xorshift(state) % 5;
			}
			auto const expected{
				std::mismatch(a.begin(), a.end(), b.begin(), b.end())};
			auto const actual{
				::fast_io::algorithms::mismatch(a, b)};
			if (expected.first - a.begin() != actual.first - a.begin() ||
				expected.second - b.begin() != actual.second - b.begin())
			{
				return false;
			}
		}
	}
	return true;
}

bool check_index_range()
{
	std::vector<std::uint64_t> a{0, 1, 2, 3, 4, 5, 6, 7};
	std::vector<std::uint64_t> b{0, 1, 9, 3, 4, 5};
	for (std::size_t f1{}; f1 <= a.size(); ++f1)
	{
		for (std::size_t l1{f1}; l1 <= a.size(); ++l1)
		{
			for (std::size_t f2{}; f2 <= b.size(); ++f2)
			{
				for (std::size_t l2{f2}; l2 <= b.size(); ++l2)
				{
					auto const expected{std::mismatch(
						a.begin() + static_cast<std::ptrdiff_t>(f1),
						a.begin() + static_cast<std::ptrdiff_t>(l1),
						b.begin() + static_cast<std::ptrdiff_t>(f2),
						b.begin() + static_cast<std::ptrdiff_t>(l2))};
					auto const actual{
						::fast_io::algorithms::mismatch_index_range(
							a, f1, l1, b, f2, l2)};
					if (actual.first != f1 + static_cast<std::size_t>(
							expected.first -
							(a.begin() + static_cast<std::ptrdiff_t>(f1))) ||
						actual.second != f2 + static_cast<std::size_t>(
							expected.second -
							(b.begin() + static_cast<std::ptrdiff_t>(f2))))
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool check_forward_list() throws(true)
{
	std::forward_list<int> a{1, 2, 3, 4};
	std::forward_list<int> b{1, 2, 9};
	auto const result{::fast_io::algorithms::mismatch(a, b)};
	return *result.first == 3 && *result.second == 9;
}

constexpr bool constexpr_test()
{
	int a[4]{1, 2, 3, 4};
	int b[4]{1, 2, 9, 4};
	auto const result{::fast_io::algorithms::mismatch_iters(
		a, a + 4, b, b + 4)};
	if (result.first != a + 2 || result.second != b + 2)
	{
		return false;
	}
	int c[2]{1, 2};
	auto const shorter{::fast_io::algorithms::mismatch_iters(
		a, a + 4, c, c + 2)};
	if (shorter.first != a + 2 || shorter.second != c + 2)
	{
		return false;
	}
	auto const empty{::fast_io::algorithms::mismatch_iters(
		a, a, c, c + 2)};
	if (empty.first != a || empty.second != c)
	{
		return false;
	}
	auto const indexed{
		::fast_io::algorithms::mismatch_index_range(a, 1, 4, b, 1, 4)};
	return indexed.first == 2 && indexed.second == 2;
}

static_assert(constexpr_test());

constexpr bool compare_only_test()
{
	int a[3]{1, 2, 3};
	int b[3]{1, 9, 3};
	auto const result{::fast_io::algorithms::mismatch_iters(
		a, a + 3, b, b + 3, compare_only{})};
	return result.first == a + 1 && result.second == b + 1;
}

static_assert(compare_only_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::mismatch(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::mismatch_index_range(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{},
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{})));

int main()
{
	if (!check_mismatch())
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
