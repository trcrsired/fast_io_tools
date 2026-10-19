// policy_check — exercise the compile-time binfmt_policy feature flags.
//
//   {} / {N} / literals are fmt shapes: work under every policy.
//   %zu        is a C shape (carries ctype): only accepted when policy.c_style.
//
// Runs the same programs through the default policy and through
// binfmt_policy_fmt_only and checks the outputs / the rejection.

#include <cstddef>
#include <cstring>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{

using ::fast_io::binfmt::binfmt_cookie_element;
using ::fast_io::binfmt::binfmt_policy_default;
using ::fast_io::binfmt::binfmt_policy_fmt_only;

inline constexpr char prog_fmt[]{"\x13\x00\x0a\x01\x0a"};				// {}\n
inline constexpr char prog_printf[]{"\x13\x02\x61\x05\x69\x02\x0a\x01\x0a"}; // %zu\n

bool run_prog(char const *prog, ::std::size_t plen, bool c_style,
			  ::fast_io::string &out) throws
{
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	binfmt_cookie_element ck{};
	ck.typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	ck.field1 = 12345u;
	try
	{
		if (c_style)
		{
			::fast_io::binfmt::binfmt_vprint<binfmt_policy_default>(dev, prog, plen, 1, &ck);
		}
		else
		{
			::fast_io::binfmt::binfmt_vprint<binfmt_policy_fmt_only>(dev, prog, plen, 1, &ck);
		}
		return true;
	}
	catch throws(::std::error)
	{
		return false;
	}
}

void run() throws
{
	{
		::fast_io::string a;
		bool const ok{run_prog(prog_fmt, sizeof(prog_fmt) - 1, false, a)};
		bool const val{a.size() == 6 && ::std::memcmp(a.data(), "12345\n", 6) == 0};
		::fast_io::io::println("fmt   {} under fmt-only : accepted=", ok, " correct=", val);
	}
	{
		::fast_io::string a;
		bool const ok{run_prog(prog_fmt, sizeof(prog_fmt) - 1, true, a)};
		bool const val{a.size() == 6 && ::std::memcmp(a.data(), "12345\n", 6) == 0};
		::fast_io::io::println("fmt   {} under default  : accepted=", ok, " correct=", val);
	}
	{
		::fast_io::string a;
		bool const ok{run_prog(prog_printf, sizeof(prog_printf) - 1, false, a)};
		::fast_io::io::println("printf %zu under fmt-only: accepted=", ok, " (want 0)");
	}
	{
		::fast_io::string a;
		bool const ok{run_prog(prog_printf, sizeof(prog_printf) - 1, true, a)};
		bool const val{a.size() == 6 && ::std::memcmp(a.data(), "12345\n", 6) == 0};
		::fast_io::io::println("printf %zu under default : accepted=", ok, " correct=", val);
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
