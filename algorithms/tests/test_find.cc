#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstdint>
#include <vector>

struct pair16
{
	std::uint64_t a;
	std::uint64_t b;
	auto operator==(pair16 const &) const noexcept -> bool = default;
};

struct with_padding
{
	char c;
	int i;
	auto operator==(with_padding const &) const noexcept -> bool = default;
};

constexpr bool constexpr_find_test()
{
	int a[8]{1, 2, 3, 4, 5, 6, 7, 8};
	auto const it{::fast_io::algorithms::find(a, 5)};
	if (it != a + 4)
	{
		return false;
	}
	return ::fast_io::algorithms::find(a, 9) == a + 8;
}

static_assert(constexpr_find_test());

constexpr bool constexpr_find_index_range_test()
{
	int a[8]{1, 2, 3, 4, 5, 6, 7, 8};
	if (::fast_io::algorithms::find_index_range(a, 1, 6, 4) != 3)
	{
		return false;
	}
	return ::fast_io::algorithms::find_index_range(a, 1, 6, 99) == 6;
}

static_assert(constexpr_find_index_range_test());

int find_width_tests();

constexpr bool partial_pair_test()
{
	pair16 p[4]{pair16{5, 0}, pair16{5, 1}, pair16{5, 6}, pair16{5, 7}};
	return ::fast_io::algorithms::find(p, pair16{5, 6}) == p + 2 &&
	       ::fast_io::algorithms::find(p, pair16{5, 99}) == p + 4;
}

static_assert(partial_pair_test());

int main()
{
	int a[16]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	if (::fast_io::algorithms::find(a, 11) != a + 11)
	{
		return 1;
	}
	if (::fast_io::algorithms::find(a, -1) != a + 16)
	{
		return 2;
	}
	if (::fast_io::algorithms::find_iters(a, a + 16, 3) != a + 3)
	{
		return 3;
	}
	if (::fast_io::algorithms::find_index_range(a, 0, 16, 15) != 15)
	{
		return 4;
	}
	if (::fast_io::algorithms::find_index_range(a, 0, 16, 16) != 16)
	{
		return 5;
	}

	std::vector<int> v{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	if (::fast_io::algorithms::find(v, 2) != v.begin() + 7)
	{
		return 6;
	}
	if (::fast_io::algorithms::find_iters(v.begin(), v.end(), 9) != v.begin())
	{
		return 7;
	}
	if (::fast_io::algorithms::find_index_range(v, 2, 10, 0) != 9)
	{
		return 8;
	}
	if (::fast_io::algorithms::find_index_range(v, 2, 10, 100) != 10)
	{
		return 9;
	}

	float const fs[8]{1.0f, 2.0f, -0.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
	if (::fast_io::algorithms::find(fs, 0.0f) != fs + 2)
	{
		return 10;
	}

	pair16 const ps[4]{{1, 2}, {3, 4}, {5, 6}, {7, 8}};
	if (::fast_io::algorithms::find(ps, pair16{5, 6}) != ps + 2)
	{
		return 11;
	}
	if (::fast_io::algorithms::find(ps, pair16{5, 7}) != ps + 4)
	{
		return 12;
	}

	with_padding const ws[4]{{1, 2}, {3, 4}, {5, 6}, {7, 8}};
	if (::fast_io::algorithms::find(ws, with_padding{3, 4}) != ws + 1)
	{
		return 13;
	}
	if (int const r{find_width_tests()}; r != 0)
	{
		return r;
	}
}

static_assert(noexcept(::fast_io::algorithms::find(
	::std::declval<int (&)[8]>(), 1)));
static_assert(noexcept(::fast_io::algorithms::find_index_range(
	::std::declval<int (&)[8]>(), ::std::size_t{}, ::std::size_t{}, 1)));

static_assert(noexcept(::fast_io::algorithms::find(
	::std::declval<::std::vector<int> &>(), 1)));
static_assert(noexcept(::fast_io::algorithms::find_index_range(
	::std::declval<::std::vector<int> &>(), ::std::size_t{}, ::std::size_t{}, 1)));

static_assert([]() constexpr {
	char s[17]{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q'};
	return ::fast_io::algorithms::find(s, 'q') == s + 16 &&
	       ::fast_io::algorithms::find(s, 'z') == s + 17;
}());

int find_width_tests()
{
	char s[33]{};
	s[32] = 'x';
	if (::fast_io::algorithms::find(s, 'x') != s + 32)
	{
		return 20;
	}
	std::uint64_t u[17]{};
	u[16] = 0x1122334455667788ull;
	if (::fast_io::algorithms::find(u, 0x1122334455667788ull) != u + 16)
	{
		return 21;
	}
	pair16 p[9]{};
	p[8] = pair16{42, 43};
	if (::fast_io::algorithms::find(p, pair16{42, 43}) != p + 8)
	{
		return 22;
	}
	pair16 partial[32]{};
	for (std::size_t i{}; i < 32; ++i)
	{
		partial[i] = pair16{7, i};
	}
	if (::fast_io::algorithms::find(partial, pair16{7, 99}) != partial + 32)
	{
		return 23;
	}
	if (::fast_io::algorithms::find(partial, pair16{7, 17}) != partial + 17)
	{
		return 24;
	}
	return 0;
}
