#include <cstdio>

#include <fast_io_dsal/string.h>

#include "binfmt_compile.h"
#include "binfmt_print.h"

using namespace ::fast_io::binfmt;

namespace
{

// compile a printf program into a byte buffer, then run it with one size_t+int
// cookie and return the produced text
::fast_io::string run_printf(char const *fmt, ::std::size_t n) throws
{
	::fast_io::string prog;
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, prog)};
		compile_c_printf(binfmt_device_of(oref), fmt, n);
	}
	// printf front-end: the type is embedded in the program, so the cookie
	// element just carries the value (typetag stays from_format == 0).
	binfmt_cookie_element ck{};
	ck.field1 = 42;
	::fast_io::string out;
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	binfmt_vprint<binfmt_policy_c_style>(binfmt_device_of(oref), prog.data(), prog.size(), 1, &ck);
	return out;
}

} // namespace

int main()
{
	try
	{
		auto r1{run_printf("x = %d\n", 6)};
		::std::printf("[%.*s]", static_cast<int>(r1.size()), r1.data());

		auto r2{run_printf("%#012x", 7)};
		::std::printf("[%.*s]", static_cast<int>(r2.size()), r2.data());
		return 0;
	}
	catch throws(::std::error e)
	{
		::std::printf("caught error\n");
		return 1;
	}
}
