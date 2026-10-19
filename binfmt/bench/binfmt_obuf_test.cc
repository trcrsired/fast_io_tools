#include <cstddef>

#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{
// field {ctype=u64,type=u} + " = " + field {ctype=u64,type=u} + "\n"
inline constexpr char program[]{
	"\x13\x02\x61\x05\x69\x02"
	"\x0a\x03\x20\x3d\x20"
	"\x13\x02\x61\x05\x69\x02"
	"\x0a\x01\x0a"};

// cookie: one binfmt_cookie_element per field. The program embeds the
// type (printf front-end), so typetag stays from_format (0).
using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;
} // namespace

int main() try
{
	// ---- buffered: obuf_file is a buffer_output_stream -> direct writes ----
	{
		::fast_io::obuf_file f{u8"bench/build/obuf_out.txt", ::fast_io::open_mode::out};
		auto dev{::fast_io::binfmt::binfmt_device_of(f)};
		::fast_io::io::println("obuf_file get_output_buffer_ptrs null = ",
							   (int)(dev.vptr->get_output_buffer_ptrs == nullptr));
		cookie_t ck[2]{};
		ck[0].field1 = 42;
		ck[1].field1 = 123456789;
		::fast_io::binfmt::binfmt_vprint(dev, program, sizeof(program) - 1, 2, ck);
	}
	// ---- unbuffered: c_stdout is not a buffer_output_stream -> scatter ----
	{
		auto cs{::fast_io::c_stdout()};
		auto dev{::fast_io::binfmt::binfmt_device_of(cs)};
		::fast_io::io::println("c_stdout  get_output_buffer_ptrs null = ",
							   (int)(dev.vptr->get_output_buffer_ptrs == nullptr));
		cookie_t ck[2]{};
		ck[0].field1 = 7;
		ck[1].field1 = 987654321;
		::fast_io::binfmt::binfmt_vprint(dev, program, sizeof(program) - 1, 2, ck);
	}
	return 0;
}
catch throws(::std::error e)
{
	::std::fprintf(stderr, "error\n");
	(void)e;
	return 1;
}
