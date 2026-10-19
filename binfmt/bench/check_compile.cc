// What does compile_fmt actually emit for "{}" / "{}\n" / "{:x}"?
// Dump the program bytes, then round-trip through the interpreter.

#include <cstddef>
#include <cstdio>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_compile.h"
#include "binfmt_print.h"

namespace
{

void dump(::fast_io::string const &s)
{
	for (auto const ch : s)
	{
		::std::printf("%02x ", static_cast<unsigned char>(ch));
	}
	::std::printf("\n");
}

void probe(char const *fmt) throws
{
	::fast_io::string prog;
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, prog)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	::fast_io::binfmt::compile_fmt(dev, fmt, ::fast_io::cstr_len(fmt));
	::std::printf("fmt %-10s -> ", fmt);
	dump(prog);
	// round trip: three u64 elements, typetag from cookie
	::fast_io::string out;
	auto oref2{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev2{::fast_io::binfmt::binfmt_device_of(oref2)};
	::fast_io::binfmt::binfmt_cookie_element ck[3]{};
	ck[0].typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	ck[0].field1 = 42;
	ck[1].typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	ck[1].field1 = 7;
	ck[2].typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	ck[2].field1 = 9;
	try
	{
		::fast_io::binfmt::binfmt_vprint(dev2, prog.data(), prog.size(), 3, ck);
		::std::printf("   run -> [%.*s]\n", static_cast<int>(out.size()), out.data());
	}
	catch throws(::std::error e)
	{
		::std::printf("   run -> THREW\n");
		(void)e;
	}
}

} // namespace

int main() try
{
	probe("{}");
	probe("{}\n");
	probe("{:x}");
	probe("hello {} world {} hello\n");
	probe("{} {}");
	probe("{:d}");
	probe("hello {2} world {1} fmtslow\n");
	probe("{2:>10}");
	probe("{2:>10}\n");
	probe("hello {2:>10} world {1} end\n");
	return 0;
}
catch throws(::std::error e)
{
	::std::printf("fatal\n");
	(void)e;
	return 1;
}
