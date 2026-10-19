// binfmt status-print indirector vs native fast_io.
//
// Same workloads as bench_all, but the binfmt side goes through the public
// status hook — ::fast_io::mnp::binfmt(oref) — so it exercises the real
// call-site path: pack the cookie, call the shared interpreter in
// libbinfmt, and (for a println) let the program emit the newline.
//
//   println one size_t
//   println "hello " i " world " i*3+1 " hello"
//   println "alpha" "beta" "gamma"        (all spans -> scatter fast path)
//
// Both write into a fast_io::string; outputs are checked equal. Each case is
// run three times and the best is reported, so one-time buffer growth does
// not dominate the comparison.

#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include <fmt/format.h>

#include "binfmt_status.h"

namespace
{

constexpr ::std::size_t N{10000000};
constexpr int TRIALS{3};

using clock_t_ = decltype(::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime));

void report(char const *label, clock_t_ dt, ::std::size_t bytes) throws
{
	::fast_io::io::println("  ", ::fast_io::mnp::os_c_str(label), " : ", dt, "s  bytes=", bytes);
}

void check(char const *what, ::fast_io::string const &a, ::fast_io::string const &b) throws
{
	bool const ok{a.size() == b.size() && ::std::memcmp(a.data(), b.data(), a.size()) == 0};
	::fast_io::io::println("  ", ::fast_io::mnp::os_c_str(what), " equal : ", ok);
}

// run `body` TRIALS times against a reused buffer, report the fastest
template <typename make_type, typename body_type>
void time_case(char const *label, ::fast_io::string &s, make_type make, body_type body) throws
{
	clock_t_ best{};
	bool first{true};
	for (int t{}; t != TRIALS; ++t)
	{
		s.clear();
		auto out{make(s)};
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		body(out);
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		if (first || (t1 - t0) < best)
		{
			best = t1 - t0;
			first = false;
		}
	}
	report(label, best, s.size());
}

// run `body` TRIALS times against a reused std::string, report the fastest
template <typename body_type>
void time_std(char const *label, ::std::string &s, body_type body) throws
{
	clock_t_ best{};
	bool first{true};
	for (int t{}; t != TRIALS; ++t)
	{
		s.clear();
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		body();
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		if (first || (t1 - t0) < best)
		{
			best = t1 - t0;
			first = false;
		}
	}
	report(label, best, s.size());
}

void bench_single() throws
{
	::fast_io::io::println("--- println one size_t ---");
	::fast_io::string bm;
	::fast_io::string nat;
	time_case(
		"binfmt-status", bm,
		[](::fast_io::string &s) throws {
			return ::fast_io::mnp::binfmt(
				::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s));
		},
		[](auto out) throws { for (::std::size_t i{}; i != N; ++i) { ::fast_io::io::println(out, i); } });
	time_case(
		"fast_io      ", nat,
		[](::fast_io::string &s) throws {
			return ::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s);
		},
		[](auto out) throws { for (::std::size_t i{}; i != N; ++i) { ::fast_io::io::println(out, i); } });
	::std::string fm;
	time_std("fmt          ", fm, [&] {
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "{}\n", i);
		}
	});
	check("single", bm, nat);
}

void bench_multi() throws
{
	::fast_io::io::println("--- println mixed spans + ints ---");
	::fast_io::string bm;
	::fast_io::string nat;
	time_case(
		"binfmt-status", bm,
		[](::fast_io::string &s) throws {
			return ::fast_io::mnp::binfmt(
				::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s));
		},
		[](auto out) throws {
			for (::std::size_t i{}; i != N; ++i)
			{
				::fast_io::io::println(out, "hello ", i, " world ", i * 3 + 1, " hello");
			}
		});
	time_case(
		"fast_io      ", nat,
		[](::fast_io::string &s) throws {
			return ::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s);
		},
		[](auto out) throws {
			for (::std::size_t i{}; i != N; ++i)
			{
				::fast_io::io::println(out, "hello ", i, " world ", i * 3 + 1, " hello");
			}
		});
	::std::string fm;
	time_std("fmt          ", fm, [&] {
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "hello {} world {} hello\n", i, i * 3 + 1);
		}
	});
	check("multi", bm, nat);
}

void bench_scatter() throws
{
	::fast_io::io::println("--- println all spans ---");
	::fast_io::string bm;
	::fast_io::string nat;
	time_case(
		"binfmt-status", bm,
		[](::fast_io::string &s) throws {
			return ::fast_io::mnp::binfmt(
				::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s));
		},
		[](auto out) throws {
			for (::std::size_t i{}; i != N; ++i)
			{
				::fast_io::io::println(out, "alpha", "beta", "gamma");
			}
		});
	time_case(
		"fast_io      ", nat,
		[](::fast_io::string &s) throws {
			return ::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s);
		},
		[](auto out) throws {
			for (::std::size_t i{}; i != N; ++i)
			{
				::fast_io::io::println(out, "alpha", "beta", "gamma");
			}
		});
	::std::string fm;
	time_std("fmt          ", fm, [&] {
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "alphabetagamma\n");
		}
	});
	check("scatter", bm, nat);
}

void run() throws
{
	bench_single();
	bench_multi();
	bench_scatter();
}

} // namespace

int main()
{
	try
	{
		run();
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
