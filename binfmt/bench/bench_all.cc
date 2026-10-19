// binfmt vs fast_io native vs {fmt} vs FMT_COMPILE vs std::format.
//
// Same three workloads, same 10M iterations:
//
//   "{}"                              one size_t
//   "hello {} world {} hello\n"       two size_t
//   "hello {2} world {1} fmtslow\n"   two size_t, explicit (non-sequential) args
//
// Each library writes into its own native container (fast_io::string for
// binfmt and fast_io, std::string for fmt / std::format), which is the
// library-vs-library comparison. Every backend's output is checked against
// the fmt result.

#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

#include <fmt/format.h>
#include <fmt/base.h>
#include <fmt/compile.h>

#if defined(__has_include)
#if __has_include(<format>)
#include <format>
#define HAVE_STD_FORMAT 1
#endif
#endif

namespace
{

constexpr ::std::size_t N{10000000};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

// ---- compiled binfmt programs (exact compile_fmt output) -------------------
inline constexpr char prog_single[]{"\x13\x00"};
inline constexpr char prog_multi[]{
	"\x0a\x06hello "
	"\x13\x00"
	"\x0a\x07 world "
	"\x13\x00"
	"\x0a\x07 hello\n"};
inline constexpr char prog_indexed[]{
	"\x0a\x06hello "
	"\x13\x01\x09\x02"
	"\x0a\x07 world "
	"\x13\x01\x09\x01"
	"\x0a\x09 fmtslow\n"};

// "hello " {arg=2, align=right, width=10} " world " {arg=1} " end\n"
inline constexpr char prog_width[]{
	"\x0a\x06hello "
	"\x13\x03\x09\x02\x19\x02\x51\x0a"
	"\x0a\x07 world "
	"\x13\x01\x09\x01"
	"\x0a\x05 end\n"};

// ---- timing ---------------------------------------------------------------

using clock_t_ = decltype(::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime));

void report(char const *label, clock_t_ dt, ::std::size_t bytes) throws
{
	::fast_io::io::println("  ", ::fast_io::mnp::os_c_str(label), " : ", dt, "s  bytes=", bytes);
}

// ===========================================================================
// "{}"  one size_t
// ===========================================================================

void bench_single() throws
{
	::fast_io::io::println("--- \"{}\" ---");

	::fast_io::string bm;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, bm)};
		auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
		cookie_t ck{};
		ck.typetag = ::fast_io::binfmt::binfmt_typetag::u64;
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			ck.field1 = i;
			::fast_io::binfmt::binfmt_vprint(dev, prog_single, sizeof(prog_single) - 1, 1, &ck);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("binfmt      ", t1 - t0, bm.size());
	}

	::fast_io::string nat;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, nat)};
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::fast_io::io::print(oref, i);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("fast_io     ", t1 - t0, nat.size());
	}

	::std::string fm;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "{}", i);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("fmt         ", t1 - t0, fm.size());
	}

	::std::string fc;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fc), FMT_COMPILE("{}"), i);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("FMT_COMPILE ", t1 - t0, fc.size());
	}

#if defined(HAVE_STD_FORMAT)
	::std::string sd;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::std::format_to(::std::back_inserter(sd), "{}", i);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("std::format ", t1 - t0, sd.size());
	}
#endif

	bool const ok{bm.size() == fm.size() && ::std::memcmp(bm.data(), fm.data(), bm.size()) == 0};
	::fast_io::io::println("  binfmt == fmt : ", ok);
}

// ===========================================================================
// "hello {} world {} hello\n"  two size_t
// ===========================================================================

void bench_multi() throws
{
	::fast_io::io::println("--- \"hello {} world {} hello\\n\" ---");

	::fast_io::string bm;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, bm)};
		auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
		cookie_t ck[2]{};
		ck[0].typetag = ::fast_io::binfmt::binfmt_typetag::u64;
		ck[1].typetag = ::fast_io::binfmt::binfmt_typetag::u64;
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			ck[0].field1 = i;
			ck[1].field1 = i * 3 + 1;
			::fast_io::binfmt::binfmt_vprint(dev, prog_multi, sizeof(prog_multi) - 1, 2, ck);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("binfmt      ", t1 - t0, bm.size());
	}

	::fast_io::string nat;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, nat)};
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::fast_io::io::print(oref, "hello ", i, " world ", i * 3 + 1, " hello\n");
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("fast_io     ", t1 - t0, nat.size());
	}

	::std::string fm;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "hello {} world {} hello\n", i, i * 3 + 1);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("fmt         ", t1 - t0, fm.size());
	}

	::std::string fc;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fc),
						   FMT_COMPILE("hello {} world {} hello\n"), i, i * 3 + 1);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("FMT_COMPILE ", t1 - t0, fc.size());
	}

#if defined(HAVE_STD_FORMAT)
	::std::string sd;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::std::format_to(::std::back_inserter(sd), "hello {} world {} hello\n", i, i * 3 + 1);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("std::format ", t1 - t0, sd.size());
	}
#endif

	bool const ok{bm.size() == fm.size() && ::std::memcmp(bm.data(), fm.data(), bm.size()) == 0};
	::fast_io::io::println("  binfmt == fmt : ", ok);
}

// ===========================================================================
// "hello {2} world {1} fmtslow\n"  two size_t, indexed
// ===========================================================================
void bench_indexed() throws
{
	::fast_io::io::println("--- \"hello {2} world {1} fmtslow\\n\" ---");

	::fast_io::string bm;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, bm)};
		auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
		cookie_t ck[3]{};
		for (auto &e : ck)
		{
			e.typetag = ::fast_io::binfmt::binfmt_typetag::u64;
		}
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			ck[0].field1 = i;
			ck[1].field1 = i + 1;
			ck[2].field1 = i + 2;
			::fast_io::binfmt::binfmt_vprint(dev, prog_indexed, sizeof(prog_indexed) - 1, 3, ck);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("binfmt      ", t1 - t0, bm.size());
	}

	::fast_io::string nat;
	{
		// fast_io has no arg indices: emit the same values in the same order.
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, nat)};
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::fast_io::io::print(oref, "hello ", i + 2, " world ", i + 1, " fmtslow\n");
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("fast_io     ", t1 - t0, nat.size());
	}

	::std::string fm;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "hello {2} world {1} fmtslow\n",
						   i, i + 1, i + 2);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("fmt         ", t1 - t0, fm.size());
	}

	::std::string fc;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fc),
						   FMT_COMPILE("hello {2} world {1} fmtslow\n"), i, i + 1, i + 2);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("FMT_COMPILE ", t1 - t0, fc.size());
	}

#if defined(HAVE_STD_FORMAT)
	::std::string sd;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::std::format_to(::std::back_inserter(sd), "hello {2} world {1} fmtslow\n",
							 i, i + 1, i + 2);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("std::format ", t1 - t0, sd.size());
	}
#endif

	bool const ok{bm.size() == fm.size() && ::std::memcmp(bm.data(), fm.data(), bm.size()) == 0};
	::fast_io::io::println("  binfmt == fmt : ", ok);
}

// ===========================================================================
// "hello {2:>10} world {1} end\n"  indexed + width spec on arg 2
// ===========================================================================
void bench_width() throws
{
	::fast_io::io::println("--- \"hello {2:>10} world {1} end\\n\" ---");

	::fast_io::string bm;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, bm)};
		auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
		cookie_t ck[3]{};
		for (auto &e : ck)
		{
			e.typetag = ::fast_io::binfmt::binfmt_typetag::u64;
		}
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			ck[0].field1 = i;
			ck[1].field1 = i + 1;
			ck[2].field1 = i + 2;
			::fast_io::binfmt::binfmt_vprint(dev, prog_width, sizeof(prog_width) - 1, 3, ck);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("binfmt      ", t1 - t0, bm.size());
	}

	// NOTE: fast_io native width (`mnp::right`) cannot be measured here.
	// print_reserve_size(width_t) is declared noexcept but calls the
	// throws-annotated print_reserve_size_width_impl, which is ill-formed
	// under herbceptions (see fast_io_freestanding_impl/width.h:494).

	::std::string fm;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "hello {2:>10} world {1} end\n", i, i + 1,
						   i + 2);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("fmt         ", t1 - t0, fm.size());
	}

	::std::string fc;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fc),
						   FMT_COMPILE("hello {2:>10} world {1} end\n"), i, i + 1, i + 2);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("FMT_COMPILE ", t1 - t0, fc.size());
	}

#if defined(HAVE_STD_FORMAT)
	::std::string sd;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::std::format_to(::std::back_inserter(sd), "hello {2:>10} world {1} end\n", i, i + 1,
							 i + 2);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		report("std::format ", t1 - t0, sd.size());
	}
#endif

	bool const ok{bm.size() == fm.size() && ::std::memcmp(bm.data(), fm.data(), bm.size()) == 0};
	::fast_io::io::println("  binfmt == fmt : ", ok);
}

void run() throws
{
	bench_single();
	bench_multi();
	bench_indexed();
	bench_width();
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
