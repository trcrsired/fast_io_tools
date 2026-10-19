// End-to-end status path: stock binfmt vs batched binfmt vs native fast_io vs
// fmt, on the three workloads from bench_status.cc. Both binfmt sides run the
// real call-site machinery (pack the cookie, call the shared interpreter) and
// write into a fast_io::string. Outputs are checked equal to native fast_io.

#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include <fmt/format.h>

#include "binfmt_status.h"
#include "binfmt_status_batch.h"

namespace
{

constexpr ::std::size_t N{10000000};
constexpr int TRIALS{3};

using clock_t_ = decltype(::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime));

void report(char const *label, clock_t_ dt) throws
{
	::fast_io::io::println("  ", ::fast_io::mnp::os_c_str(label), " : ", dt, "s   ",
						   dt / static_cast<::std::uint_least64_t>(N), "s/iter");
}

bool same(::fast_io::string const &a, ::fast_io::string const &b)
{
	return a.size() == b.size() && ::std::memcmp(a.data(), b.data(), a.size()) == 0;
}

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
	report(label, best);
}

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
	report(label, best);
}

// run the two binfmt sides, return the native string for the equality check
template <typename body_type>
void binfmt_pair(::fast_io::string &bm, ::fast_io::string &bb, body_type body) throws
{
	time_case(
		"binfmt stock ", bm,
		[](::fast_io::string &s) throws {
			return ::fast_io::mnp::binfmt(
				::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s));
		},
		body);
	time_case(
		"binfmt batch ", bb,
		[](::fast_io::string &s) throws {
			return ::fast_io::mnp::binfmt_batch(
				::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s));
		},
		body);
}

void bench_single() throws
{
	::fast_io::io::println("--- println one size_t ---");
	::fast_io::string bm;
	::fast_io::string bb;
	::fast_io::string nat;
	binfmt_pair(bm, bb, [](auto out) throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			::fast_io::io::println(out, i);
		}
	});
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
	::fast_io::io::println("  equal(stock,batch)=", same(bm, bb), " equal(batch,native)=", same(bb, nat));
}

void bench_multi() throws
{
	::fast_io::io::println("--- println mixed spans + ints ---");
	::fast_io::string bm;
	::fast_io::string bb;
	::fast_io::string nat;
	binfmt_pair(bm, bb, [](auto out) throws {
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
	::fast_io::io::println("  equal(stock,batch)=", same(bm, bb), " equal(batch,native)=", same(bb, nat));
}

void bench_scatter() throws
{
	::fast_io::io::println("--- println all spans ---");
	::fast_io::string bm;
	::fast_io::string bb;
	::fast_io::string nat;
	binfmt_pair(bm, bb, [](auto out) throws {
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
	::fast_io::io::println("  equal(stock,batch)=", same(bm, bb), " equal(batch,native)=", same(bb, nat));
}

} // namespace

int main()
{
	try
	{
		bench_single();
		bench_multi();
		bench_scatter();
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
