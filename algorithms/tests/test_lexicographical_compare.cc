#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <forward_list>
#include <string>
#include <vector>

namespace
{

struct reverse_compare
{
	template <typename T, typename U>
	constexpr auto compare(T const &a, U const &b) const noexcept
	{
		return b <=> a;
	}
};

bool check_equal()
{
	std::vector<int> a{1, 2, 3};
	std::vector<int> b{1, 2, 3};
	return !::fast_io::algorithms::lexicographical_compare(a, b) &&
		!::fast_io::algorithms::lexicographical_compare(b, a) &&
		::fast_io::algorithms::lexicographical_compare_three_way(a, b) == 0;
}

bool check_ordering()
{
	std::vector<int> a{1, 2, 3};
	std::vector<int> b{1, 2, 4};
	std::vector<int> c{1, 2};
	std::vector<int> d{1, 2, 3, 0};
	return ::fast_io::algorithms::lexicographical_compare(a, b) &&
		!::fast_io::algorithms::lexicographical_compare(b, a) &&
		::fast_io::algorithms::lexicographical_compare(c, a) &&
		::fast_io::algorithms::lexicographical_compare(a, d) &&
		::fast_io::algorithms::lexicographical_compare_three_way(a, b) < 0 &&
		::fast_io::algorithms::lexicographical_compare_three_way(b, a) > 0 &&
		::fast_io::algorithms::lexicographical_compare_three_way(c, a) < 0 &&
		::fast_io::algorithms::lexicographical_compare_three_way(a, d) < 0;
}

bool check_containers()
{
	std::string a{"abc"};
	std::string b{"abd"};
	std::array<int, 3> c{1, 2, 3};
	std::array<long, 3> d{1, 2, 3};
	return ::fast_io::algorithms::lexicographical_compare(a, b) &&
		!::fast_io::algorithms::lexicographical_compare(b, a) &&
		!::fast_io::algorithms::lexicographical_compare(c, d);
}

bool check_forward_list() throws(true)
{
	std::array<int, 3> c{1, 2, 3};
	std::forward_list<int> e{1, 2, 4};
	return !::fast_io::algorithms::lexicographical_compare(
			c, e, reverse_compare{}) &&
		::fast_io::algorithms::lexicographical_compare(
			e, c, reverse_compare{}) &&
		::fast_io::algorithms::lexicographical_compare_three_way(c, e) < 0;
}

bool check_empty()
{
	std::vector<int> empty;
	std::vector<int> a{1};
	return !::fast_io::algorithms::lexicographical_compare(empty, empty) &&
		::fast_io::algorithms::lexicographical_compare(empty, a) &&
		!::fast_io::algorithms::lexicographical_compare(a, empty) &&
		::fast_io::algorithms::lexicographical_compare_three_way(empty, empty) == 0 &&
		::fast_io::algorithms::lexicographical_compare_three_way(empty, a) < 0 &&
		::fast_io::algorithms::lexicographical_compare_three_way(a, empty) > 0;
}

bool check_index_range()
{
	std::uint64_t state{0x9e3779b97f4a7c15ull};
	for (std::size_t n1{}; n1 != 8; ++n1)
	{
		std::vector<std::uint64_t> a(n1);
		for (auto &value : a)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = state % 7;
		}
		for (std::size_t n2{}; n2 != 8; ++n2)
		{
			std::vector<std::uint64_t> b(n2);
			for (auto &value : b)
			{
				state ^= state << 13;
				state ^= state >> 7;
				state ^= state << 17;
				value = state % 7;
			}
			for (std::size_t f1{}; f1 <= n1; ++f1)
			{
				for (std::size_t l1{f1}; l1 <= n1; ++l1)
				{
					for (std::size_t f2{}; f2 <= n2; ++f2)
					{
						for (std::size_t l2{f2}; l2 <= n2; ++l2)
						{
							bool const expected{std::lexicographical_compare(
								a.begin() + static_cast<std::ptrdiff_t>(f1),
								a.begin() + static_cast<std::ptrdiff_t>(l1),
								b.begin() + static_cast<std::ptrdiff_t>(f2),
								b.begin() + static_cast<std::ptrdiff_t>(l2))};
							bool const actual{
								::fast_io::algorithms::lexicographical_compare_index_range(
									a, f1, l1, b, f2, l2)};
							if (expected != actual)
							{
								return false;
							}
							auto const left_begin{
								a.begin() + static_cast<std::ptrdiff_t>(f1)};
							auto const left_end{
								a.begin() + static_cast<std::ptrdiff_t>(l1)};
							auto const right_begin{
								b.begin() + static_cast<std::ptrdiff_t>(f2)};
							auto const right_end{
								b.begin() + static_cast<std::ptrdiff_t>(l2)};
							auto const expected_three{
								std::lexicographical_compare(
									left_begin, left_end, right_begin, right_end)
									? std::strong_ordering::less
									: (std::lexicographical_compare(
										right_begin, right_end, left_begin,
										left_end)
										? std::strong_ordering::greater
										: std::strong_ordering::equal)};
							auto const actual_three{
								::fast_io::algorithms::
									lexicographical_compare_three_way_index_range(
										a, f1, l1, b, f2, l2)};
							if (expected_three != actual_three)
							{
								return false;
							}
						}
					}
				}
			}
		}
	}
	return true;
}

constexpr bool constexpr_test()
{
	int a[3]{1, 2, 3};
	int b[3]{1, 2, 4};
	int c[2]{1, 2};
	if (!::fast_io::algorithms::lexicographical_compare(c, a))
	{
		return false;
	}
	if (!::fast_io::algorithms::lexicographical_compare(a, b))
	{
		return false;
	}
	if (::fast_io::algorithms::lexicographical_compare(b, a))
	{
		return false;
	}
	auto const c1{
		::fast_io::algorithms::lexicographical_compare_three_way_iters(
			a, a + 3, b, b + 3)};
	auto const c2{
		::fast_io::algorithms::lexicographical_compare_three_way_iters(
			a, a + 3, a, a + 3)};
	return c1 < 0 && c2 == 0;
}

static_assert(constexpr_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::lexicographical_compare(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::lexicographical_compare_three_way(
	::std::declval<int (&)[4]>(),
	::std::declval<int (&)[4]>())));
static_assert(noexcept(::fast_io::algorithms::lexicographical_compare_index_range(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{},
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{})));

int main()
{
	if (!check_equal())
	{
		return 1;
	}
	if (!check_ordering())
	{
		return 2;
	}
	if (!check_containers())
	{
		return 3;
	}
	try
	{
		if (!check_forward_list())
		{
			return 6;
		}
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		return 7;
	}
	if (!check_empty())
	{
		return 4;
	}
	if (!check_index_range())
	{
		return 5;
	}
	return 0;
}
