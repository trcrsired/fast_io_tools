#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace
{

enum class pattern
{
	ascending,
	descending,
	random,
	d20,
	p5,
	s95
};

char const *pattern_name(pattern p) noexcept
{
	switch (p)
	{
	case pattern::ascending:
		return "ascending";
	case pattern::descending:
		return "descending";
	case pattern::random:
		return "random";
	case pattern::d20:
		return "d20";
	case pattern::p5:
		return "p5";
	case pattern::s95:
		return "s95";
	}
	return "?";
}

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

std::vector<std::uint64_t> make_keys(std::size_t n, pattern p)
{
	xorshift rng{0x9e3779b97f4a7c15ull};
	std::vector<std::uint64_t> keys(n);
	for (std::size_t i{}; i != n; ++i)
	{
		switch (p)
		{
		case pattern::ascending:
			keys[i] = i;
			break;
		case pattern::descending:
			keys[i] = n - 1 - i;
			break;
		case pattern::random:
			keys[i] = rng.next();
			break;
		case pattern::d20:
			keys[i] = rng.next() % 21;
			break;
		case pattern::p5:
			keys[i] = (rng.next() % 100) < 5 ? rng.next() : 0;
			break;
		case pattern::s95:
			keys[i] = i < n * 95 / 100 ? i : rng.next();
			break;
		}
	}
	return keys;
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

struct big
{
	std::uint64_t key;
	std::uint64_t payload[127];
};

struct big_cmp
{
	auto compare(big const &a, big const &b) const noexcept
	{
		return a.key <=> b.key;
	}
	bool equal(big const &a, big const &b) const noexcept
	{
		return a.key == b.key;
	}
};

template <typename T>
std::uint64_t checksum(T const &value) noexcept
{
	return static_cast<std::uint64_t>(value.key);
}

template <>
std::uint64_t checksum<std::uint64_t>(std::uint64_t const &value) noexcept
{
	return value;
}

template <typename T, typename Cmp>
double bench(std::vector<T> const &original, Cmp cmp, bool use_std, int reps)
{
	std::vector<T> work(original.size());
	double best{1e100};
	std::uint64_t sink{};
	for (int r{}; r != reps; ++r)
	{
		std::copy(original.begin(), original.end(), work.begin());
		auto const start{std::chrono::steady_clock::now()};
		if (use_std)
		{
			std::sort(work.begin(), work.end(), [&](T const &a, T const &b) {
				return cmp.compare(a, b) < 0;
			});
		}
		else
		{
			::fast_io::algorithms::sort(work, cmp);
		}
		auto const stop{std::chrono::steady_clock::now()};
		double const ns{static_cast<double>(
			std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count())};
		if (ns < best)
		{
			best = ns;
		}
		for (std::size_t i{1}; i < work.size(); ++i)
		{
			if (cmp.compare(work[i], work[i - 1]) < 0)
			{
				std::abort();
			}
		}
		std::uint64_t sum{};
		for (auto const &element : work)
		{
			sum += checksum(element);
		}
		sink ^= sum;
	}
	std::fprintf(stderr, "sink=%llu\n", static_cast<unsigned long long>(sink));
	return best / static_cast<double>(original.size());
}

template <typename T, typename Make, typename Cmp>
void run_type(char const *name, std::vector<std::size_t> const &sizes,
	std::vector<pattern> const &patterns, Make make, Cmp cmp,
	std::uint64_t reps_small, std::uint64_t reps_large)
{
	for (auto const n : sizes)
	{
		int const reps{n >= 1000000 ? static_cast<int>(reps_large)
									: static_cast<int>(reps_small)};
		for (auto const p : patterns)
		{
			auto const keys{make_keys(n, p)};
			std::vector<T> original;
			original.reserve(n);
			for (auto const key : keys)
			{
				original.emplace_back(make(key));
			}
			double const std_ns{bench(original, cmp, true, reps)};
			double const fast_ns{bench(original, cmp, false, reps)};
			std::printf("%s,%s,%s,%zu,%.3f,%.3f,%.3f\n", name, pattern_name(p),
				"std", n, std_ns, fast_ns, std_ns / fast_ns);
		}
	}
}

} // namespace

int main(int argc, char **argv)
{
	bool const quick{argc > 1 && std::strcmp(argv[1], "--quick") == 0};
	int const reps{quick ? 2 : 4};

	run_type<std::uint64_t>("u64",
		quick ? std::vector<std::size_t>{1000, 10000, 100000}
			  : std::vector<std::size_t>{1000, 10000, 100000, 1000000},
		{pattern::ascending, pattern::descending, pattern::random,
			pattern::d20, pattern::p5, pattern::s95},
		[](std::uint64_t key) { return key; },
		::fast_io::algorithms::compare_three_way{}, reps, reps);

	run_type<rec16>("rec16",
		{10000, 100000},
		{pattern::ascending, pattern::descending, pattern::random,
			pattern::d20, pattern::p5, pattern::s95},
		[](std::uint64_t key) { return rec16{key, key}; },
		rec16_cmp{}, reps, reps);

	run_type<big>("big1k",
		{1000, 10000},
		{pattern::ascending, pattern::random},
		[](std::uint64_t key) {
			big value{};
			value.key = key;
			for (std::size_t i{}; i != 127; ++i)
			{
				value.payload[i] = key + i;
			}
			return value;
		},
		big_cmp{}, reps, reps);

	return 0;
}
