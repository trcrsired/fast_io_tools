#include <cstddef>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{
constexpr ::std::size_t N{10000000};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

// literal "abcdefg\n"
inline constexpr char lit_prog[]{"\x0a\x08\x61\x62\x63\x64\x65\x66\x67\x0a"};
// field {ctype=u64, type=u}
inline constexpr char field_prog[]{"\x13\x02\x61\x05\x69\x02"};
// field + literal newline
inline constexpr char full_prog[]{"\x13\x02\x61\x05\x69\x02\x0a\x01\x0a"};

void bench(char const *prog, ::std::size_t plen) throws
{
	::fast_io::string out;
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	cookie_t ck{};
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	for (::std::size_t i{}; i != N; ++i)
	{
		ck.field1 = i;
		::fast_io::binfmt::binfmt_vprint<::fast_io::binfmt::binfmt_policy_c_style>(dev, prog, plen, 1, &ck);
	}
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println(t1 - t0, "s  bytes=", out.size());
}
} // namespace

int main()
{
	try
	{
		::fast_io::io::println("literal-only:");
		bench(lit_prog, sizeof(lit_prog) - 1);
		::fast_io::io::println("field-only:");
		bench(field_prog, sizeof(field_prog) - 1);
		::fast_io::io::println("field+lit:");
		bench(full_prog, sizeof(full_prog) - 1);
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
