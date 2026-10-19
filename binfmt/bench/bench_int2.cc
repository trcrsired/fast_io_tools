// Anti-hoist micros: vary inputs / escape addresses so nothing folds.

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
constexpr ::std::size_t K{256};

using field_t = d::binfmt_field<char>;
using ce_t = b::binfmt_cookie_element;

::fast_io::string sink;

alignas(64) char field_payload[K * 3]; // [count=1][ptag=0x69][value=j]
alignas(64) char uleb_buf[K * 2];	   // [0x69][j]

void init_bufs()
{
	for (::std::size_t j{}; j != K; ++j)
	{
		field_payload[j * 3 + 0] = 1;
		field_payload[j * 3 + 1] = 0x69;
		field_payload[j * 3 + 2] = static_cast<char>(j & 0x7f);
		uleb_buf[j * 2 + 0] = 0x69;
		uleb_buf[j * 2 + 1] = static_cast<char>(j & 0x7f);
	}
}

// address escapes -> the whole 72-byte field must be materialised
__attribute__((noinline)) ::std::uint_least64_t consume_field(field_t const *f) noexcept
{
	volatile ::std::uint_least64_t s{};
	s = s + f->ctype + f->width + f->type + f->align + f->sign + f->prec + f->arg;
	s = s + f->auto_arg + f->flag_alt + f->flag_zero + f->flag_group +
		f->flag_locale + f->flag_outdigits + f->flag_upper + f->fill_len;
	s = s + static_cast<::std::uint_least64_t>(reinterpret_cast<::std::uintptr_t>(f->fill));
	return s;
}

__attribute__((noinline)) ::std::uint_least64_t consume_u64(::std::uint_least64_t v) noexcept
{
	volatile ::std::uint_least64_t s{v};
	return s;
}

template <typename F>
void timed(char const *label, F &&f) throws
{
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	f();
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	::fast_io::io::println(::fast_io::mnp::os_c_str(label), " : ", t1 - t0, "s");
}

void bench_field_init() throws
{
	::std::uint_least64_t acc{};
	timed("field{} zero-init  ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			field_t f{};
			f.type = static_cast<::std::uint_least32_t>(i);
			acc += consume_field(__builtin_addressof(f));
		}
	});
	(void)acc;
}

void bench_parse_field() throws
{
	::std::uint_least64_t acc{};
	timed("parse_field only   ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			field_t f{};
			d::binfmt_prog p{reinterpret_cast<char8_t const *>(field_payload + (i % K) * 3),
							 reinterpret_cast<char8_t const *>(field_payload + (i % K) * 3) + 3};
			d::binfmt_parse_field(p, f);
			acc += consume_field(__builtin_addressof(f));
		}
	});
	(void)acc;
}

void bench_uleb() throws
{
	::std::uint_least64_t acc{};
	timed("1x get_uleb alone  ", [&]() throws {
		for (::std::size_t i{}; i != N; ++i)
		{
			d::binfmt_prog p{reinterpret_cast<char8_t const *>(uleb_buf + (i % K) * 2),
							 reinterpret_cast<char8_t const *>(uleb_buf + (i % K) * 2) + 2};
			acc += consume_u64(d::binfmt_get_uleb(p));
		}
	});
	(void)acc;
}

} // namespace

int main()
{
	init_bufs();
	try
	{
		bench_field_init();
		bench_parse_field();
		bench_uleb();
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
