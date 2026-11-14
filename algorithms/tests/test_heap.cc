#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <deque>
#include <vector>

struct greater_three_way
{
	template <typename T, typename U>
	inline constexpr auto compare(T const &a, U const &b) const noexcept
	{
		return b <=> a;
	}
};

struct throwing_compare
{
	template <typename T, typename U>
	inline constexpr auto compare(T const &a, U const &b) const
	{
		return a <=> b;
	}
};

struct move_only
{
	int value;
	move_only(int v) : value{v} {}
	move_only(move_only &&) noexcept = default;
	move_only &operator=(move_only &&) noexcept = default;
	move_only(move_only const &) = delete;
	move_only &operator=(move_only const &) = delete;
	auto operator<=>(move_only const &) const noexcept = default;
};

namespace
{

template <typename T, typename Cmp>
bool heap_property(T const *a, std::size_t n, Cmp cmp)
{
	for (std::size_t i{1}; i < n; ++i)
	{
		if (cmp.compare(a[(i - 1) / 2], a[i]) < 0)
		{
			return false;
		}
	}
	return true;
}

bool ascending(int const *a, std::size_t n)
{
	for (std::size_t i{1}; i < n; ++i)
	{
		if (a[i - 1] > a[i])
		{
			return false;
		}
	}
	return true;
}

bool descending(int const *a, std::size_t n)
{
	for (std::size_t i{1}; i < n; ++i)
	{
		if (a[i - 1] < a[i])
		{
			return false;
		}
	}
	return true;
}

} // namespace

constexpr bool constexpr_heap_test()
{
	int a[7]{5, 1, 4, 2, 8, 3, 7};
	if (::fast_io::algorithms::is_heap(a))
	{
		return false;
	}
	::fast_io::algorithms::make_heap(a);
	if (!::fast_io::algorithms::is_heap(a))
	{
		return false;
	}
	if (::fast_io::algorithms::is_heap_until(a) != a + 7)
	{
		return false;
	}
	::fast_io::algorithms::sort_heap(a);
	for (int i{1}; i < 7; ++i)
	{
		if (a[i - 1] > a[i])
		{
			return false;
		}
	}

	int b[4]{1, 2, 3, 9};
	::fast_io::algorithms::make_heap_iters(b, b + 3);
	// b[3] is the pushed element; [b, b + 3) is a heap.
	::fast_io::algorithms::push_heap_iters(b, b + 4);
	if (b[0] != 9)
	{
		return false;
	}
	::fast_io::algorithms::pop_heap_iters(b, b + 4);
	if (b[3] != 9)
	{
		return false;
	}

	int c[3]{1, 2, 3};
	::fast_io::algorithms::make_heap(c, greater_three_way{});
	if (c[0] != 1)
	{
		return false;
	}
	::fast_io::algorithms::sort_heap(c, greater_three_way{});
	return c[0] == 3 && c[1] == 2 && c[2] == 1;
}

static_assert(constexpr_heap_test());

constexpr bool constexpr_heap_until_test()
{
	int a[5]{9, 5, 7, 1, 100};
	return ::fast_io::algorithms::is_heap_until(a) == a + 4 &&
				 !::fast_io::algorithms::is_heap(a);
}

static_assert(constexpr_heap_until_test());

constexpr bool constexpr_empty_test()
{
	int a[1]{42};
	::fast_io::algorithms::make_heap_iters(a, a);
	::fast_io::algorithms::push_heap_iters(a, a);
	::fast_io::algorithms::pop_heap_iters(a, a);
	::fast_io::algorithms::sort_heap_iters(a, a);
	::fast_io::algorithms::make_heap_iters(a, a + 1);
	::fast_io::algorithms::push_heap_iters(a, a + 1);
	::fast_io::algorithms::pop_heap_iters(a, a + 1);
	::fast_io::algorithms::sort_heap_iters(a, a + 1);
	return a[0] == 42;
}

static_assert(constexpr_empty_test());

static_assert(noexcept(::fast_io::algorithms::make_heap(
	::std::declval<int (&)[8]>())));
static_assert(noexcept(::fast_io::algorithms::sort_heap(
	::std::declval<::std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::is_heap_iters(
	::std::declval<int *>(), ::std::declval<int *>())));
static_assert(!noexcept(::fast_io::algorithms::make_heap_iters(
	::std::declval<int *>(), ::std::declval<int *>(), throwing_compare{})));

int main()
{
	std::uint32_t seed{12345u};
	auto next = [&seed]() {
		seed = seed * 1664525u + 1013904223u;
		return static_cast<int>(seed % 1000u);
	};

	// make_heap / push_heap / pop_heap / sort_heap on a pointer range.
	int a[65]{};
	for (std::size_t i{}; i < 64; ++i)
	{
		a[i] = next();
	}
	::fast_io::algorithms::make_heap_iters(a, a + 64);
	if (!heap_property(a, 64, ::fast_io::algorithms::compare_three_way{}))
	{
		return 1;
	}
	if (a[0] != *std::max_element(a, a + 64))
	{
		return 2;
	}
	a[64] = 5000;
	::fast_io::algorithms::push_heap_iters(a, a + 65);
	if (a[0] != 5000 || !heap_property(a, 65, ::fast_io::algorithms::compare_three_way{}))
	{
		return 3;
	}
	::fast_io::algorithms::pop_heap_iters(a, a + 65);
	if (a[64] != 5000 || !heap_property(a, 64, ::fast_io::algorithms::compare_three_way{}))
	{
		return 4;
	}
	::fast_io::algorithms::sort_heap_iters(a, a + 64);
	if (!ascending(a, 64))
	{
		return 5;
	}

	// Range overloads and a non-contiguous random-access container.
	std::deque<int> d;
	for (std::size_t i{}; i < 100; ++i)
	{
		d.push_back(next());
	}
	if (::fast_io::algorithms::is_heap(d))
	{
		return 6;
	}
	::fast_io::algorithms::make_heap(d);
	if (!::fast_io::algorithms::is_heap(d))
	{
		return 7;
	}
	if (::fast_io::algorithms::is_heap_until(d) != d.end())
	{
		return 8;
	}
	::fast_io::algorithms::sort_heap(d);
	if (!std::is_sorted(d.begin(), d.end()))
	{
		return 9;
	}

	// Descending comparator produces a min-heap and a descending sort.
	int b[32]{};
	for (std::size_t i{}; i < 32; ++i)
	{
		b[i] = next();
	}
	::fast_io::algorithms::make_heap(b, greater_three_way{});
	if (!heap_property(b, 32, greater_three_way{}) || b[0] != *std::min_element(b, b + 32))
	{
		return 10;
	}
	::fast_io::algorithms::sort_heap(b, greater_three_way{});
	if (!descending(b, 32))
	{
		return 11;
	}

	// vector range overloads with a comparator.
	std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};
	::fast_io::algorithms::make_heap(v, greater_three_way{});
	if (v.front() != 1 || !::fast_io::algorithms::is_heap(v, greater_three_way{}))
	{
		return 12;
	}
	::fast_io::algorithms::pop_heap(v, greater_three_way{});
	if (v.back() != 1)
	{
		return 13;
	}
	v.pop_back();
	::fast_io::algorithms::push_heap(v, greater_three_way{});
	if (!::fast_io::algorithms::is_heap(v, greater_three_way{}))
	{
		return 14;
	}

	// Move-only element type.
	move_only m[6]{move_only{5}, move_only{1}, move_only{4}, move_only{2},
								 move_only{8}, move_only{3}};
	::fast_io::algorithms::make_heap_iters(m, m + 6);
	if (m[0].value != 8)
	{
		return 15;
	}
	::fast_io::algorithms::sort_heap_iters(m, m + 6);
	for (std::size_t i{1}; i < 6; ++i)
	{
		if (m[i - 1].value > m[i].value)
		{
			return 16;
		}
	}

	return 0;
}
