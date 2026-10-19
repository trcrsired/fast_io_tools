// {fmt} benchmark — 10M size_t, mirroring the binfmt benchmark.
//
//   fmt::format_to(back_inserter(s), "{}\n", i)   -> data + newline
//   fmt::format_to(back_inserter(s), "{}",   i)   -> data only
//
// and the same with a fmt::memory_buffer and with FMT_COMPILE.
// Compare to binfmt:  {type=u}+nl ~0.39s, {type=u} ~0.30s (10M).

#include <cstddef>
#include <cstdio>
#include <chrono>
#include <string>
#include <iterator>

#include <fmt/format.h>
#include <fmt/compile.h>
#include <fmt/base.h>

namespace
{

constexpr ::std::size_t N{10000000};

template <typename F>
double timed(F &&f)
{
	auto const t0{::std::chrono::steady_clock::now()};
	f();
	auto const t1{::std::chrono::steady_clock::now()};
	return ::std::chrono::duration<double>(t1 - t0).count();
}

void report(char const *label, double s, ::std::size_t bytes)
{
	::std::printf("%-34s : %.6fs  bytes=%zu\n", label, s, bytes);
}

} // namespace

int main()
{
	// warm up the formatter / libc
	{
		::std::string w;
		for (::std::size_t i{}; i != 100000; ++i)
		{
			fmt::format_to(::std::back_inserter(w), "{}\n", i);
		}
	}

	// ---- std::string + back_inserter, "{}\n" ----
	for (int rep{}; rep != 2; ++rep)
	{
		::std::string s;
		double const t{timed([&] {
			for (::std::size_t i{}; i != N; ++i)
			{
				fmt::format_to(::std::back_inserter(s), "{}\n", i);
			}
		})};
		report("fmt std::string    \"{}\\n\"", t, s.size());
	}

	// ---- std::string + back_inserter, "{}" ----
	for (int rep{}; rep != 2; ++rep)
	{
		::std::string s;
		double const t{timed([&] {
			for (::std::size_t i{}; i != N; ++i)
			{
				fmt::format_to(::std::back_inserter(s), "{}", i);
			}
		})};
		report("fmt std::string    \"{}\"  ", t, s.size());
	}

	// ---- memory_buffer + back_inserter, "{}\n" ----
	for (int rep{}; rep != 2; ++rep)
	{
		fmt::memory_buffer buf;
		double const t{timed([&] {
			for (::std::size_t i{}; i != N; ++i)
			{
				fmt::format_to(::std::back_inserter(buf), "{}\n", i);
			}
		})};
		report("fmt memory_buffer  \"{}\\n\"", t, buf.size());
	}

	// ---- FMT_COMPILE, std::string, "{}\n" ----
	for (int rep{}; rep != 2; ++rep)
	{
		::std::string s;
		double const t{timed([&] {
			for (::std::size_t i{}; i != N; ++i)
			{
				fmt::format_to(::std::back_inserter(s), FMT_COMPILE("{}\n"), i);
			}
		})};
		report("fmt FMT_COMPILE    \"{}\\n\"", t, s.size());
	}

	// ---- fmt::format allocating each call (for reference) ----
	{
		::std::size_t total{};
		double const t{timed([&] {
			for (::std::size_t i{}; i != N; ++i)
			{
				total += fmt::format("{}\n", i).size();
			}
		})};
		report("fmt::format (alloc each call)", t, total);
	}

	return 0;
}
