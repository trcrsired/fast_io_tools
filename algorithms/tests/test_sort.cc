#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

namespace
{

enum class sort_pattern
{
	ascending,
	descending,
	random,
	all_equal,
	d20,
	sorted95
};

struct xorshift
{
	std::uint64_t state;
	explicit xorshift(std::uint64_t seed) : state{seed ? seed : 1} {}
	std::uint64_t next() noexcept
	{
		state ^= state << 13;
		state ^= state >> 7;
		state ^= state << 17;
		return state;
	}
};

std::vector<std::uint64_t> make_keys(std::size_t n, sort_pattern pattern, std::uint64_t seed)
{
	xorshift rng{seed};
	std::vector<std::uint64_t> keys(n);
	for (std::size_t i{}; i != n; ++i)
	{
		switch (pattern)
		{
		case sort_pattern::ascending:
			keys[i] = i;
			break;
		case sort_pattern::descending:
			keys[i] = n - 1 - i;
			break;
		case sort_pattern::random:
			keys[i] = rng.next();
			break;
		case sort_pattern::all_equal:
			keys[i] = 42;
			break;
		case sort_pattern::d20:
			keys[i] = rng.next() % 21;
			break;
		case sort_pattern::sorted95:
			keys[i] = i < n * 95 / 100 ? i : rng.next();
			break;
		}
	}
	return keys;
}

template <typename T, typename Cmp>
bool equivalent(Cmp cmp, T const &a, T const &b)
{
	return !(cmp.compare(a, b) < 0) && !(cmp.compare(b, a) < 0);
}

bool reports{};

void fail() noexcept
{
	reports = true;
}

struct rec16
{
	std::uint64_t key;
	std::uint64_t payload;
};

struct rec16_cmp
{
	auto compare(rec16 const &a, rec16 const &b) const noexcept
	{
		return a.key <=> b.key;
	}
	bool equal(rec16 const &a, rec16 const &b) const noexcept
	{
		return a.key == b.key;
	}
};

struct rec32
{
	std::uint64_t key;
	std::uint64_t payload[3];
};

struct rec32_cmp
{
	auto compare(rec32 const &a, rec32 const &b) const noexcept
	{
		return a.key <=> b.key;
	}
};

struct big_trivial
{
	std::uint64_t key;
	unsigned char padding[120];
};

struct big_trivial_cmp
{
	auto compare(big_trivial const &a, big_trivial const &b) const noexcept
	{
		return a.key <=> b.key;
	}
};

struct move_only
{
	int key;
	explicit move_only(int k) : key{k} {}
	move_only(move_only &&) noexcept = default;
	move_only &operator=(move_only &&) noexcept = default;
	move_only(move_only const &) = delete;
	move_only &operator=(move_only const &) = delete;
};

struct move_only_cmp
{
	auto compare(move_only const &a, move_only const &b) const noexcept
	{
		return a.key <=> b.key;
	}
};

struct big_nontrivial
{
	int key;
	std::string text;
	unsigned char padding[80];
	explicit big_nontrivial(int k) : key{k}, text{std::to_string(k)} {}
	big_nontrivial(big_nontrivial &&) noexcept = default;
	big_nontrivial &operator=(big_nontrivial &&) noexcept = default;
};

struct big_nontrivial_cmp
{
	auto compare(big_nontrivial const &a, big_nontrivial const &b) const noexcept
	{
		return a.key <=> b.key;
	}
};

template <typename T, typename Make, typename Cmp>
void check_vector(std::size_t n, sort_pattern pattern, Make make, Cmp cmp, std::uint64_t seed)
{
	auto const keys{make_keys(n, pattern, seed)};
	std::vector<T> a;
	std::vector<T> b;
	a.reserve(n);
	b.reserve(n);
	for (auto const key : keys)
	{
		a.emplace_back(make(key));
		b.emplace_back(make(key));
	}
	std::sort(b.begin(), b.end(), [&](T const &x, T const &y) { return cmp.compare(x, y) < 0; });
	::fast_io::algorithms::sort(a, cmp);
	if (a.size() != b.size())
	{
		fail();
		return;
	}
	for (std::size_t i{}; i != a.size(); ++i)
	{
		if (!equivalent(cmp, a[i], b[i]))
		{
			fail();
			return;
		}
	}
}

template <typename T, typename Make, typename Cmp>
void check_deque(std::size_t n, sort_pattern pattern, Make make, Cmp cmp, std::uint64_t seed) throws(true)
{
	auto const keys{make_keys(n, pattern, seed)};
	std::deque<T> a;
	std::deque<T> b;
	for (auto const key : keys)
	{
		a.emplace_back(make(key));
		b.emplace_back(make(key));
	}
	std::sort(b.begin(), b.end(), [&](T const &x, T const &y) { return cmp.compare(x, y) < 0; });
	::fast_io::algorithms::sort(a, cmp);
	if (a.size() != b.size())
	{
		fail();
		return;
	}
	auto ia{a.begin()};
	auto ib{b.begin()};
	for (; ia != a.end(); ++ia, ++ib)
	{
		if (!equivalent(cmp, *ia, *ib))
		{
			fail();
			return;
		}
	}
}

constexpr std::size_t sizes[]{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15, 16, 17, 18, 19,
	20, 21, 24, 31, 32, 33, 47, 48, 63, 64, 65, 96, 97, 127, 128,
	129, 200, 255, 256, 257};

constexpr sort_pattern patterns[]{
	sort_pattern::ascending,
	sort_pattern::descending,
	sort_pattern::random,
	sort_pattern::all_equal,
	sort_pattern::d20,
	sort_pattern::sorted95};

template <bool deque_too, typename T, typename Make, typename Cmp>
void check_all(std::size_t max_size, Make make, Cmp cmp) throws(deque_too)
{
	std::uint64_t seed{0x9e3779b97f4a7c15ull};
	for (auto const n : sizes)
	{
		if (n > max_size)
		{
			break;
		}
		for (auto const pattern : patterns)
		{
			check_vector<T>(n, pattern, make, cmp, seed);
			if constexpr (deque_too)
			{
				check_deque<T>(n, pattern, make, cmp, seed);
			}
			++seed;
		}
	}
	for (auto const n : {std::size_t{1000}, std::size_t{10000}, std::size_t{100000}})
	{
		if (n > max_size)
		{
			break;
		}
		for (auto const pattern : patterns)
		{
			check_vector<T>(n, pattern, make, cmp, seed);
			++seed;
		}
	}
}

struct counting_equal_cmp
{
	int *equal_calls;
	auto compare(std::uint64_t a, std::uint64_t b) const noexcept
	{
		return a <=> b;
	}
	bool equal(std::uint64_t a, std::uint64_t b) const noexcept
	{
		++*equal_calls;
		return a == b;
	}
};

struct throwing_compare
{
	template <typename T, typename U>
	auto compare(T const &a, U const &b) const
	{
		return a <=> b;
	}
};

bool check_sort_index()
{
	std::uint64_t state{0x243f6a8885a308d3ull};
	for (std::size_t n{}; n != 24; ++n)
	{
		std::vector<std::int64_t> source(n);
		for (auto &value : source)
		{
			state ^= state << 13;
			state ^= state >> 7;
			state ^= state << 17;
			value = static_cast<std::int64_t>(state % 97);
		}
		for (std::size_t first{}; first <= n; ++first)
		{
			for (std::size_t last{first}; last <= n; ++last)
			{
				auto expected{source};
				std::sort(
					expected.begin() + static_cast<std::ptrdiff_t>(first),
					expected.begin() + static_cast<std::ptrdiff_t>(last));
				auto actual{source};
				::fast_io::algorithms::sort_index(actual, first, last);
				if (actual != expected)
				{
					return false;
				}
			}
		}
	}
	return true;
}

} // namespace

static_assert(noexcept(::fast_io::algorithms::sort(
	::std::declval<::std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::sort_iters(
	::std::declval<int *>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::sort_index(
	::std::declval<::std::vector<int> &>(), ::std::size_t{},
	::std::size_t{})));
static_assert(!noexcept(::fast_io::algorithms::sort_iters(
	::std::declval<int *>(), ::std::declval<int *>(),
	::std::declval<throwing_compare>())));

int main()
{
	using ::fast_io::algorithms::compare_three_way;

	check_all<false, std::int64_t>(100000,
		[](std::uint64_t key) { return static_cast<std::int64_t>(key); },
		compare_three_way{});

	try
	{
		check_all<true, rec16>(100000,
			[](std::uint64_t key) { return rec16{key, key}; },
			rec16_cmp{});
	}
	catch throws(::std::error)
	{
		return 1;
	}

	check_all<false, rec32>(100000,
		[](std::uint64_t key) { return rec32{key, {key, key, key}}; },
		rec32_cmp{});

	check_all<false, big_trivial>(200,
		[](std::uint64_t key) {
			big_trivial value{};
			value.key = key;
			return value;
		},
		big_trivial_cmp{});

	check_all<false, move_only>(100000,
		[](std::uint64_t key) { return move_only{static_cast<int>(key % 1000000)}; },
		move_only_cmp{});

	check_all<false, big_nontrivial>(200,
		[](std::uint64_t key) { return big_nontrivial{static_cast<int>(key % 1000000)}; },
		big_nontrivial_cmp{});

	check_all<false, std::string>(10000,
		[](std::uint64_t key) { return std::to_string(key); },
		compare_three_way{});

	// Raw pointer entry point.
	{
		auto const keys{make_keys(10000, sort_pattern::random, 123)};
		std::vector<std::int64_t> a;
		std::vector<std::int64_t> b;
		a.reserve(keys.size());
		b.reserve(keys.size());
		for (auto const key : keys)
		{
			a.push_back(static_cast<std::int64_t>(key));
			b.push_back(static_cast<std::int64_t>(key));
		}
		std::sort(b.begin(), b.end());
		::fast_io::algorithms::sort_iters(a.data(), a.data() + a.size());
		if (a != b)
		{
			fail();
		}
	}

	// The optional equality hook must actually be used for duplicate-heavy
	// inputs where ancestor pivot tracking kicks in.
	{
		int equal_calls{};
		auto const keys{make_keys(10000, sort_pattern::d20, 77)};
		std::vector<std::uint64_t> a{keys};
		::fast_io::algorithms::sort(a, counting_equal_cmp{&equal_calls});
		if (!std::is_sorted(a.begin(), a.end()))
		{
			fail();
		}
		if (equal_calls == 0)
		{
			fail();
		}
	}

	if (!check_sort_index())
	{
		return 2;
	}

	return reports ? 1 : 0;
}
