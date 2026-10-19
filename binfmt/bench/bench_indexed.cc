// "hello {2} world {1} fmtslow\n" — explicit (non-sequential) argument
// indices, which defeat the auto-arg fast paths and exercise the general
// field parser with an `arg` parameter.
//
// compile_fmt emits, per field, `13 01 09 <idx>`:
//   13 = field, 01 = one param, 09 = (fp_arg<<3)|uleb, idx.
//
// binfmt side packs three cookie elements and references [2] then [1].

#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_compile.h"
#include "binfmt_print.h"

#include <fmt/format.h>
#include <fmt/base.h>

namespace
{

constexpr ::std::size_t N{10000000};

// "hello " {arg=2} " world " {arg=1} " fmtslow\n"
inline constexpr char prog_indexed[]{
	"\x0a\x06hello "
	"\x13\x01\x09\x02"
	"\x0a\x07 world "
	"\x13\x01\x09\x01"
	"\x0a\x09 fmtslow\n"};
inline constexpr ::std::size_t prog_indexed_len{sizeof(prog_indexed) - 1};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

void run() throws
{
	// ---- binfmt ----
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
			::fast_io::binfmt::binfmt_vprint(dev, prog_indexed, prog_indexed_len, 3, ck);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("binfmt \"hello {2} world {1} fmtslow\\n\" : ",
							   t1 - t0, "s  bytes=", bm.size());
	}

	// ---- fmt ----
	::std::string fm;
	{
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			fmt::format_to(::std::back_inserter(fm), "hello {2} world {1} fmtslow\n",
						   i, i + 1, i + 2);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("fmt    \"hello {2} world {1} fmtslow\\n\" : ",
							   t1 - t0, "s  bytes=", fm.size());
	}

	bool const ok{bm.size() == fm.size() &&
				  ::std::memcmp(bm.data(), fm.data(), bm.size()) == 0};
	::fast_io::io::println("matches fmt : ", ok);
	if (!ok)
	{
		::fast_io::io::println("binfmt[", ::fast_io::mnp::os_c_str(bm.data()), "]");
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
