// benchmark: C-style type embedded in the program vs typetag in the cookie.
//
//   c-style  field {ctype=u64, type=u}  13 02 61 05 69 02   (type in program)
//   typetag  field {type=u}             13 01 69 02         (type in cookie)
//
// Both format one ::std::size_t as unsigned decimal plus a newline. If the
// typetag form is faster, the C-style ctype param (one uleb + one parse
// switch arm per field) is pure interpreter overhead we could drop.

#include <cstddef>
#include <cstring>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{

inline constexpr char prog_cstyle[]{"\x13\x02\x61\x05\x69\x02\x0a\x01\x0a"};
inline constexpr char prog_typetag[]{"\x13\x00\x0a\x01\x0a"};

constexpr ::std::size_t N{10000000};

using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;

void bench(char const *label, char const *prog, ::std::size_t plen,
		   bool use_typetag, ::fast_io::string &out) throws
{
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};

	cookie_t ck{};
	if (use_typetag)
	{
		ck.typetag = ::fast_io::binfmt::binfmt_typetag::u64;
	}

	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	for (::std::size_t i{}; i != N; ++i)
	{
		ck.field1 = i;
		::fast_io::binfmt::binfmt_vprint(dev, prog, plen, 1, &ck);
	}
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println(::fast_io::mnp::os_c_str(label), ": ", t1 - t0, "s  bytes=", out.size());
}

void run() throws
{
	// two orderings to shake out warm-up / allocator noise
	{
		::fast_io::string a;
		bench("c-style  (run 1)", prog_cstyle, sizeof(prog_cstyle) - 1, false, a);
	}
	{
		::fast_io::string b;
		bench("typetag  (run 1)", prog_typetag, sizeof(prog_typetag) - 1, true, b);
	}
	{
		::fast_io::string a;
		bench("c-style  (run 2)", prog_cstyle, sizeof(prog_cstyle) - 1, false, a);
	}
	{
		::fast_io::string b;
		bench("typetag  (run 2)", prog_typetag, sizeof(prog_typetag) - 1, true, b);
	}

	// ---- correctness: both must equal native -----------------------------
	::fast_io::string ca, cb, ref;
	bench("c-style  (check)", prog_cstyle, sizeof(prog_cstyle) - 1, false, ca);
	bench("typetag  (check)", prog_typetag, sizeof(prog_typetag) - 1, true, cb);
	{
		auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, ref)};
		for (::std::size_t i{}; i != N; ++i)
		{
			::fast_io::io::print(oref, i, "\n");
		}
	}
	bool const ok_c{ca.size() == ref.size() &&
					::std::memcmp(ca.data(), ref.data(), ca.size()) == 0};
	bool const ok_t{cb.size() == ref.size() &&
					::std::memcmp(cb.data(), ref.data(), cb.size()) == 0};
	::fast_io::io::println("c-style matches native : ", ok_c);
	::fast_io::io::println("typetag matches native : ", ok_t);
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
