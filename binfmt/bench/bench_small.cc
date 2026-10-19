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
//
// The sink is a raw basic_concat_buffer (not basic_string: driving
// fast_io::string through operations::scatter_write_all trips an unrelated
// NUL-terminator bug in that path).

#include <cstddef>
#include <cstring>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{

inline constexpr char program[]{"\x13\x02\x61\x05\x69\x02\x0a\x01\x0a"};
inline constexpr ::std::size_t program_len{sizeof(program) - 1};

// cookie: one binfmt_cookie_element; the program embeds the type (u64)
using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

constexpr ::std::size_t N{100000};

using buffer_type = ::fast_io::details::basic_concat_buffer<char>;

::std::size_t buffer_size(buffer_type &b) noexcept
{
	auto const *data{strlike_begin(::fast_io::io_strlike_type<char, buffer_type>, b)};
	auto const *curr{strlike_curr(::fast_io::io_strlike_type<char, buffer_type>, b)};
	return static_cast<::std::size_t>(curr - data);
}

void run() throws
{
	buffer_type out;

	// ---- binfmt -----------------------------------------------------------
	{
		auto oref{io_strlike_ref(::fast_io::io_alias, out)};
		auto dev{::fast_io::binfmt::binfmt_device_of(oref)};

		cookie_t ck{};

		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			ck.field1 = i;
			::fast_io::binfmt::binfmt_vprint<::fast_io::binfmt::binfmt_policy_c_style>(dev, program, program_len, 1, &ck);
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("binfmt_vprint : ", t1 - t0, "s  bytes=", buffer_size(out));
	}

	auto const *binfmt_data{strlike_begin(::fast_io::io_strlike_type<char, buffer_type>, out)};
	::std::size_t const binfmt_len{buffer_size(out)};

	// ---- native fast_io ---------------------------------------------------
	buffer_type ref;
	{
		auto oref{io_strlike_ref(::fast_io::io_alias, ref)};
		auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::fast_io::io::print(oref, i, "\n");
		}
		auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
		::fast_io::io::println("fast_io print : ", t1 - t0, "s  bytes=", buffer_size(ref));
	}

	auto const *ref_data{strlike_begin(::fast_io::io_strlike_type<char, buffer_type>, ref)};
	::std::size_t const ref_len{buffer_size(ref)};

	if (binfmt_len == ref_len &&
		::std::memcmp(binfmt_data, ref_data, binfmt_len) == 0)
	{
		::fast_io::io::println("output matches fast_io print");
	}
	else
	{
		::fast_io::io::println("OUTPUT MISMATCH: binfmt=", binfmt_len, " ref=", ref_len);
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
