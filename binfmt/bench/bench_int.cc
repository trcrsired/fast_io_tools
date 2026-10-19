// Isolate the integer path from the parser.
//
//   full field-only vprint : parser + dispatch + format + device write
//   binfmt_emit_int(...)   : dispatch + format + device write   (no parser)
//   binfmt_emit_int_mag(..): format + device write              (no ctype switch)
//   binfmt_write_dec(..)   : pure jeaiii digits into a stack buffer (no write)
//
// All over 10M ::std::size_t, base 10.

#include <cstddef>
#include <cstdint>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace d = ::fast_io::binfmt::details;
namespace b = ::fast_io::binfmt;

namespace
{

constexpr ::std::size_t N{10000000};

inline constexpr char field_prog[]{"\x13\x01\x69\x02"};
inline constexpr char8_t uleb_two[]{u8'\x69', u8'\x02'};

using field_t = d::binfmt_field<char>;
using ce_t = b::binfmt_cookie_element;

::fast_io::string sink;

template <typename F>
void timed(char const *label, F &&f) throws
{
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	f();
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println(::fast_io::mnp::os_c_str(label), " : ", t1 - t0,
						   "s  bytes=", sink.size());
}

void bench_vprint() throws
{
	sink.clear();
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, sink)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	ce_t ce{};
	ce.typetag = b::binfmt_typetag::u64;
	timed("vprint field-only ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			ce.field1 = i;
			b::binfmt_vprint(dev, field_prog, sizeof(field_prog) - 1, 1, &ce);
		}
	});
}

void bench_emit_int() throws
{
	sink.clear();
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, sink)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	field_t f{};
	f.type = b::binfmt_conv_u;
	f.ctype = b::binfmt_ct_u64;
	ce_t ce{};
	timed("emit_int only      ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			ce.field1 = i;
			d::binfmt_emit_int(dev, f, ce);
		}
	});
}

void bench_emit_mag() throws
{
	sink.clear();
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, sink)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	field_t f{};
	f.type = b::binfmt_conv_u;
	timed("emit_int_mag only  ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			d::binfmt_emit_int_mag(dev, f, false, static_cast<::std::uint_least64_t>(i));
		}
	});
}

void bench_empty() throws
{
	sink.clear();
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, sink)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	timed("empty program      ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			b::binfmt_vprint(dev, "", 0, 0, nullptr);
		}
	});
}

void bench_lit0() throws
{
	sink.clear();
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, sink)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	timed("1 zero-lit turn    ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			b::binfmt_vprint(dev, "\x0a\x00", 2, 0, nullptr);
		}
	});
}

void bench_parse_field() throws
{
	::std::uint_least64_t acc{};
	auto const *pay{reinterpret_cast<char8_t const *>(field_prog) + 1};
	auto const *pend{reinterpret_cast<char8_t const *>(field_prog) + 4};
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	for (::std::size_t i{}; i != N; ++i)
	{
		field_t f{};
		d::binfmt_prog p{pay, pend};
		d::binfmt_parse_field(p, f);
		acc += f.type + f.ctype;
	}
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println("parse_field only   : ", t1 - t0, "s  bytes=", sink.size());
	if (acc == 12345)
	{
		::fast_io::io::println("x");
	}
}

void bench_field_init() throws
{
	::std::uint_least64_t acc{};
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	for (::std::size_t i{}; i != N; ++i)
	{
		field_t f{};
		f.type = static_cast<::std::uint_least32_t>(i);
		acc += f.type + f.ctype;
	}
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println("field{} zero-init  : ", t1 - t0, "s  bytes=", sink.size());
	if (acc == 12345)
	{
		::fast_io::io::println("x");
	}
}

void bench_uleb() throws
{
	::std::uint_least64_t acc{};
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	for (::std::size_t i{}; i != N; ++i)
	{
		d::binfmt_prog p{uleb_two, uleb_two + 2};
		acc += d::binfmt_get_uleb(p);
	}
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println("1x get_uleb alone  : ", t1 - t0, "s  bytes=", sink.size());
	if (acc == 12345)
	{
		::fast_io::io::println("x");
	}
}

void bench_dec_only() throws
{
	// pure jeaiii digit generation, no device. not really I/O; just the
	// cost of turning a size_t into decimal bytes.
	char buf[64];
	::std::size_t acc{};
	timed("write_dec only     ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			auto *end{d::binfmt_write_dec<char>(buf, static_cast<::std::uint_least64_t>(i))};
			acc += static_cast<::std::size_t>(end - buf);
		}
	});
	if (acc == 12345) // never true; keeps the loop honest
	{
		::fast_io::io::println("x");
	}
}

} // namespace

int main()
{
	try
	{
		bench_empty();
		bench_lit0();
		bench_vprint();
		bench_parse_field();
		bench_field_init();
		bench_uleb();
		bench_emit_int();
		bench_emit_mag();
		bench_dec_only();
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
