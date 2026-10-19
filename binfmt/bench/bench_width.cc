// Does the trailing '\n' cost "double"? Compare, over 10M iterations:
//   native  print(i)          vs  print(i, "\n")
//   binfmt  {type=u}          vs  {type=u} + literal "\n"
//
// binfmt programs (type in cookie, typetag = u64):
//   {type=u}         13 01 69 02
//   {type=u} + "\n"  13 01 69 02 0a 01 0a
//   native reference point is fast_io print.

#include <cstddef>
#include <cstring>
#include <cstdio>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{

inline constexpr char prog_only[]{"\x13\x01\x69\x02"};
inline constexpr char prog_nl[]{"\x13\x01\x69\x02\x0a\x01\x0a"};

constexpr ::std::size_t N{10000000};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

template <typename F>
void timed(char const *label, ::fast_io::string &out, F &&f) throws
{
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	f();
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println(::fast_io::mnp::os_c_str(label), ": ", t1 - t0,
						   "s  bytes=", out.size());
}

void bench_binfmt(char const *label, char const *prog, ::std::size_t plen,
				  bool nl, ::fast_io::string &out) throws
{
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	cookie_t ck{};
	ck.typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	(void)nl;
	timed(label, out, [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			ck.field1 = i;
			::fast_io::binfmt::binfmt_vprint(dev, prog, plen, 1, &ck);
		}
	});
}

void bench_native(char const *label, bool nl, ::fast_io::string &out) throws
{
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	timed(label, out, [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			if (nl)
			{
				::fast_io::io::print(oref, i, "\n");
			}
			else
			{
				::fast_io::io::print(oref, i);
			}
		}
	});
}

void run() throws
{
	// warm-up ordering alternated; each measured once (10M iters, ~0.2s+)
	{
		::fast_io::string o;
		bench_native("native print(i)        ", false, o);
	}
	{
		::fast_io::string o;
		bench_native("native print(i, nl)    ", true, o);
	}
	{
		::fast_io::string o;
		bench_binfmt("binfmt {type=u}        ", prog_only, sizeof(prog_only) - 1, false, o);
	}
	{
		::fast_io::string o;
		bench_binfmt("binfmt {type=u}+nl     ", prog_nl, sizeof(prog_nl) - 1, true, o);
	}

	// correctness vs native
	{
		::fast_io::string a, b, r;
		bench_binfmt("binfmt {type=u}        ", prog_only, sizeof(prog_only) - 1, false, a);
		{
			auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, r)};
			for (::std::size_t i{}; i != N; ++i)
			{
				::fast_io::io::print(oref, i);
			}
		}
		::fast_io::string a2;
		bench_binfmt("binfmt {type=u}+nl     ", prog_nl, sizeof(prog_nl) - 1, true, a2);
		::fast_io::string r2;
		{
			auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, r2)};
			for (::std::size_t i{}; i != N; ++i)
			{
				::fast_io::io::print(oref, i, "\n");
			}
		}
		::fast_io::io::println("field-only matches native: ",
							   a.size() == r.size() && ::std::memcmp(a.data(), r.data(), a.size()) == 0);
		::fast_io::io::println("field+nl   matches native: ",
							   a2.size() == r2.size() && ::std::memcmp(a2.data(), r2.data(), a2.size()) == 0);
	}
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
