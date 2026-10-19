#include <cstdio>
#include <cstddef>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

inline constexpr char program[]{"\x13\x02\x61\x05\x69\x02\x0a\x01\x0a"};
inline constexpr ::std::size_t program_len{sizeof(program) - 1};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

void run() throws
{
	::fast_io::string a;
	::fast_io::string b;
	auto oa{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, a)};
	auto ob{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, b)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oa)};
	cookie_t ck{};
	for (::std::size_t i{}; i != 12; ++i)
	{
		ck.field1 = i;
		::fast_io::binfmt::binfmt_vprint<::fast_io::binfmt::binfmt_policy_c_style>(dev, program, program_len, 1, &ck);
	}
	for (::std::size_t i{}; i != 12; ++i)
	{
		::fast_io::io::print(ob, i, '\n');
	}
	::fast_io::io::println("binfmt size=", a.size(), " native size=", b.size());
	::std::fprintf(stderr, "binfmt[%s]\n", a.c_str());
	::std::fprintf(stderr, "native[%s]\n", b.c_str());
}

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
