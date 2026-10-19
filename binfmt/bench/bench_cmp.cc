// binfmt vs {fmt}, mixed in one TU (built with -fherbceptions, not -fno-exceptions).
//
//   "hello {} world {} hello\n"  with two size_t   (multi-field / multi-literal)
//   "{}" + "\n"                  with one size_t   (the plain benchmark shape)
//
// binfmt programs use the typetag form (type in the cookie, no ctype param),
// auto-arg so field i binds to cookie element i.

#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

#include <fmt/format.h>
#include <fmt/base.h>

namespace
{

constexpr ::std::size_t N{10000000};

// "hello " {} " world " {} " hello\n"   — the exact bytes compile_fmt emits
inline constexpr char prog_multi[]{
	"\x0a\x06hello "
	"\x13\x00"
	"\x0a\x07 world "
	"\x13\x00"
	"\x0a\x07 hello\n"};
inline constexpr ::std::size_t prog_multi_len{sizeof(prog_multi) - 1};

// {} "\n"
inline constexpr char prog_simple[]{"\x13\x00\x0a\x01\x0a"};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

void bench_binfmt_multi(::fast_io::string &out) throws
{
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	cookie_t ck[2]{};
	ck[0].typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	ck[1].typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	for (::std::size_t i{}; i != N; ++i)
	{
		ck[0].field1 = i;
		ck[1].field1 = i * 3 + 1;
		::fast_io::binfmt::binfmt_vprint(dev, prog_multi, prog_multi_len, 2, ck);
	}
}

void bench_binfmt_simple(::fast_io::string &out) throws
{
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	cookie_t ck{};
	ck.typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	for (::std::size_t i{}; i != N; ++i)
	{
		ck.field1 = i;
		::fast_io::binfmt::binfmt_vprint(dev, prog_simple, sizeof(prog_simple) - 1, 1, &ck);
	}
}

void run() throws
{
	// ---- binfmt multi ----
	::fast_io::string bm;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		bench_binfmt_multi(bm);
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("binfmt \"hello {} world {} hello\\n\" : ", t1 - t0, "s  bytes=", bm.size());
	}

	// ---- fmt multi ----
	::std::string fm;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "hello {} world {} hello\n", i, i * 3 + 1);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("fmt    \"hello {} world {} hello\\n\" : ", t1 - t0, "s  bytes=", fm.size());
	}

	// ---- binfmt simple ----
	::fast_io::string bs;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		bench_binfmt_simple(bs);
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("binfmt \"{}\"+\"\\n\"                : ", t1 - t0, "s  bytes=", bs.size());
	}

	// ---- fmt simple ----
	::std::string fs;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fs), "{}\n", i);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("fmt    \"{}\"+\"\\n\"                : ", t1 - t0, "s  bytes=", fs.size());
	}

	// ---- correctness ----
	bool const m_ok{bm.size() == fm.size() &&
					::std::memcmp(bm.data(), fm.data(), bm.size()) == 0};
	bool const s_ok{bs.size() == fs.size() &&
					::std::memcmp(bs.data(), fs.data(), bs.size()) == 0};
	::fast_io::io::println("multi matches fmt : ", m_ok);
	::fast_io::io::println("simple matches fmt: ", s_ok);
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
