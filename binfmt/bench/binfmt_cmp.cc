#include <cstddef>
#include <cstdio>

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
	constexpr ::std::size_t N{100000};
	for (::std::size_t i{}; i != N; ++i)
	{
		ck.field1 = i;
		::fast_io::binfmt::binfmt_vprint(dev, program, program_len, 1, &ck);
	}
	for (::std::size_t i{}; i != N; ++i)
	{
		::fast_io::io::print(ob, i, "\n");
	}
	::fast_io::io::println("binfmt=", a.size(), " native=", b.size());
	for (::std::size_t i{}; i != 44; ++i)
	{
		char ca{i < a.size() ? a[i] : '?'};
		char cb{i < b.size() ? b[i] : '?'};
		::std::fprintf(stderr, "%2zu %02x/%02x  %c|%c\n", i,
					   static_cast<unsigned>(static_cast<unsigned char>(ca)),
					   static_cast<unsigned>(static_cast<unsigned char>(cb)), ca, cb);
	}
}

int main()
{
	try
	{
		run();
		::std::fflush(stdout);
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
