// binfmt_vprint benchmark — the interpreter only, no compiler in the loop.
//
// The compiled program is embedded as a byte literal, exactly what
// compile_c_printf("%zu\n", 4) produces:
//
//   field {ctype=u64, type=u}      13 02 61 05 69 02
//     tag  0x13 = (op_field=2 << 3) | kind_list=3
//     count 02
//     param ctype  0x61 = (fp_ctype=12 << 3) | kind_uleb=1, value 05 = ct_u64
//     param type   0x69 = (fp_type=13  << 3) | kind_uleb=1, value 02 = conv_u
//   literal "\n"                   0a 01 0a
//     tag  0x0a = (op_literal=1 << 3) | kind_bytes=2, len 01, '\n'
//
// So each call formats one ::std::size_t as unsigned decimal plus a newline.
// The native fast_io `print` of the same value is the reference point.

#include <cstddef>
#include <cstring>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{

inline constexpr char program[]{"\x13\x02\x61\x05\x69\x02\x0a\x01\x0a"};
inline constexpr ::std::size_t program_len{sizeof(program) - 1};

// cookie: one element. typetag stays from_format (0) because this program
// is the C printf front-end — the type (u64) is embedded in the program.
using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

constexpr ::std::size_t N{10000000};

void run() throws
{
	::fast_io::string out;

	// ---- binfmt -----------------------------------------------------------
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
		auto dev{::fast_io::binfmt::binfmt_device_of(oref)};

		cookie_t ck{};

		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			ck.field1 = i;
			::fast_io::binfmt::binfmt_vprint<::fast_io::binfmt::binfmt_policy_c_style>(dev, program, program_len, 1, &ck);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("binfmt_vprint : ", t1 - t0, "s  bytes=", out.size());
	}

	::std::size_t const binfmt_len{out.size()};

	// ---- native fast_io ---------------------------------------------------
	::fast_io::string ref;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, ref)};
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::fast_io::io::print(oref, i, "\n");
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("fast_io print : ", t1 - t0, "s  bytes=", ref.size());
	}

	if (binfmt_len == ref.size() &&
		::std::memcmp(out.data(), ref.data(), binfmt_len) == 0)
	{
		::fast_io::io::println("output matches fast_io print");
	}
	else
	{
		::fast_io::io::println("OUTPUT MISMATCH: binfmt=", binfmt_len, " ref=", ref.size());
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
