#include <cstddef>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{
// field {ctype=u64, type=u} + literal " = " + field + literal "\n"
inline constexpr char program[]{
	"\x13\x02\x61\x05\x69\x02"
	"\x0a\x03\x20\x3d\x20"
	"\x13\x02\x61\x05\x69\x02"
	"\x0a\x01\x0a"};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;
} // namespace

int main()
{
	try
	{
		// unbuffered stream (not buffer_output_stream): vtable get/set == nullptr
		::fast_io::io::print(::fast_io::c_stdout(), "buffer_output_stream(c_stdout) = ",
							 (int)::fast_io::buffer_output_stream<::fast_io::basic_c_family_io_observer<::fast_io::c_family::standard, char>>, "\n");

		auto out{::fast_io::c_stdout()};
		auto dev{::fast_io::binfmt::binfmt_device_of(out)};
		::fast_io::io::println("get_output_buffer_ptrs null = ",
							   (int)(dev.vptr->get_output_buffer_ptrs == nullptr));

		cookie_t ck[2]{};
		ck[0].field1 = 42;
		ck[1].field1 = 123456789;
		::fast_io::binfmt::binfmt_vprint<::fast_io::binfmt::binfmt_policy_c_style>(dev, program, sizeof(program) - 1, 2, ck);
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
