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

struct rec16
{
	std::uint64_t key;
	std::uint64_t payload;
};

struct big
{
	std::uint64_t key;
	std::uint64_t payload[127];
};

char const *shift_name(int index) noexcept
{
	switch (index)
	{
	case 0:
		return "1";
	case 1:
		return "n/8";
	case 2:
		return "n/4";
	case 3:
		return "n/2";
	case 4:
		return "7n/8";
	case 5:
		return "n-1";
	default:
		return "?";
	}
}

std::size_t shift_of(int index, std::size_t n) noexcept
{
	switch (index)
	{
	case 0:
		return 1;
	case 1:
		return n / 8;
	case 2:
		return n / 4;
	case 3:
		return n / 2;
	case 4:
		return n - 1 - n / 8;
	case 5:
		return n - 1;
	default:
		return 0;
	}
}

std::uint64_t checksum(std::vector<std::uint64_t> const &values) noexcept
{
	std::uint64_t sum{};
	for (auto const value : values)
	{
		sum += value;
	}
	return sum;
}

std::uint64_t checksum(std::vector<rec16> const &values) noexcept
{
	std::uint64_t sum{};
	for (auto const &value : values)
	{
		sum += value.key;
	}
	return sum;
}

std::uint64_t checksum(std::vector<big> const &values) noexcept
{
	std::uint64_t sum{};
	for (auto const &value : values)
	{
		sum += value.key;
	}
	return sum;
}

template <typename T>
bool verify(std::vector<T> const &values, std::size_t shift) noexcept
{
	std::size_t const n{values.size()};
	if (values[0].key != shift)
	{
		return false;
	}
	if (values[n - shift].key != 0)
	{
		return false;
	}
	for (std::size_t i{}; i != n; ++i)
	{
		if (values[i].key != (i + shift) % n)
		{
			return false;
		}
	}
	return true;
}

template <>
bool verify<std::uint64_t>(std::vector<std::uint64_t> const &values, std::size_t shift) noexcept
{
	std::size_t const n{values.size()};
	if (values[0] != shift || values[n - shift] != 0)
	{
		return false;
	}
	for (std::size_t i{}; i != n; ++i)
	{
		if (values[i] != (i + shift) % n)
		{
			return false;
		}
	}
	return true;
}

template <typename T>
double bench(std::vector<T> const &original, std::size_t shift, bool use_std,
	int reps, std::uint64_t &sink)
{
	std::vector<T> work(original.size());
	double best{1e100};
	for (int r{}; r != reps; ++r)
	{
		std::memcpy(work.data(), original.data(), original.size() * sizeof(T));
		auto const start{std::chrono::steady_clock::now()};
		if (use_std)
		{
			std::rotate(work.begin(),
				work.begin() + static_cast<std::ptrdiff_t>(shift), work.end());
		}
		else
		{
			::fast_io::algorithms::rotate(
				work, work.begin() + static_cast<std::ptrdiff_t>(shift));
		}
		auto const stop{std::chrono::steady_clock::now()};
		double const ns{static_cast<double>(
			std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count())};
		if (ns < best)
		{
			best = ns;
		}
		if (!verify(work, shift))
		{
			std::abort();
		}
		sink ^= checksum(work);
	}
	return best / static_cast<double>(original.size());
}

template <typename T>
void run_type(char const *name, std::vector<std::size_t> const &sizes,
	std::uint64_t &sink)
{
	for (auto const n : sizes)
	{
		std::vector<T> original;
		original.reserve(n);
		for (std::size_t i{}; i != n; ++i)
		{
			if constexpr (::std::same_as<T, std::uint64_t>)
			{
				original.push_back(i);
			}
			else
			{
				T value{};
				value.key = i;
				original.push_back(value);
			}
		}
		int const reps{n >= 1000000 ? 20 : (sizeof(T) >= 512 ? 8 : 100)};
		for (int s{}; s != 6; ++s)
		{
			auto const shift{shift_of(s, n)};
			double const std_ns{bench(original, shift, true, reps, sink)};
			double const fast_ns{bench(original, shift, false, reps, sink)};
			std::printf("%s,%s,%zu,%.4f,%.4f\n", name, shift_name(s), n,
				std_ns, fast_ns);
		}
	}
}

} // namespace

int main(int argc, char **argv)
{
	bool const quick{argc > 1 && std::strcmp(argv[1], "--quick") == 0};
	std::uint64_t sink{};

	std::vector<std::size_t> const small_sizes{
		quick ? std::vector<std::size_t>{1000, 10000}
			  : std::vector<std::size_t>{1000, 10000, 100000, 1000000}};
	std::vector<std::size_t> const big_sizes{
		quick ? std::vector<std::size_t>{1000}
			  : std::vector<std::size_t>{1000, 10000, 100000}};

	run_type<std::uint64_t>("u64", small_sizes, sink);
	run_type<rec16>("rec16", small_sizes, sink);
	run_type<big>("big1k", big_sizes, sink);

	std::fprintf(stderr, "sink=%llu\n", static_cast<unsigned long long>(sink));
	return 0;
}
