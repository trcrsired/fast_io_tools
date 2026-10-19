// correctness: the batched interpreter must reproduce the stock interpreter's
// bytes exactly for every argument shape the status path can produce, on both
// a buffer_output_stream device and an unbuffered one, plus hand-crafted
// padded / zero-filled / showbase field programs (which the status front-end
// does not currently emit).

#include <cstring>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_batch.h"
#include "binfmt_status.h"

using namespace ::fast_io::binfmt;

namespace
{

struct sink
{
	char buf[1u << 16];
	char *cur{};
};

void sc(void *dp, ::fast_io::basic_io_scatter_t<char> const *ps, ::std::size_t n) throws
{
	auto &s{*static_cast<sink *>(dp)};
	for (::std::size_t i{}; i != n; ++i)
	{
		::std::memcpy(s.cur, ps[i].base, ps[i].len);
		s.cur += ps[i].len;
	}
}

::fast_io::binfmt::basic_binfmt_output_buffer_ptrs<char> gt(void *dp) noexcept
{
	auto &s{*static_cast<sink *>(dp)};
	return {s.cur, s.buf + sizeof(s.buf)};
}

void st(void *dp, char *p) noexcept
{
	static_cast<sink *>(dp)->cur = p;
}

inline constexpr basic_binfmt_device_vtable<char> vt{
	.scatter_write_all = sc, .get_output_buffer_ptrs = gt, .set_output_curr_ptr = st};

inline constexpr basic_binfmt_device_vtable<char> vt_unbuf{
	.scatter_write_all = sc, .get_output_buffer_ptrs = nullptr, .set_output_curr_ptr = nullptr};

bool run_pair(basic_binfmt_device_vtable<char> const *v, void const *prog,
			  ::std::size_t plen, ::std::size_t k, binfmt_cookie_element const *cookie) throws
{
	sink a;
	sink b;
	a.cur = a.buf;
	b.cur = b.buf;
	basic_binfmt_device<char> da{v, &a};
	basic_binfmt_device<char> db{v, &b};
	binfmt_vprint(da, prog, plen, k, cookie);
	binfmt_vprint_batched(db, prog, plen, k, cookie);
	auto const al{static_cast<::std::size_t>(a.cur - a.buf)};
	auto const bl{static_cast<::std::size_t>(b.cur - b.buf)};
	bool const ok{al == bl && ::std::memcmp(a.buf, b.buf, al) == 0};
	if (!ok)
	{
		::fast_io::io::println("    stock  =[", ::fast_io::mnp::os_c_str(a.buf, al), "]");
		::fast_io::io::println("    batched=[", ::fast_io::mnp::os_c_str(b.buf, bl), "]");
	}
	return ok;
}

template <bool line, typename... Args>
void cmp(char const *what, Args... args) throws
{
	constexpr ::std::size_t k{sizeof...(Args)};
	auto const &prog{::fast_io::binfmt::details::binfmt_program<line, char, Args...>};
	binfmt_cookie_element cookie[k == 0u ? 1u : k];
	if constexpr (k != 0u)
	{
		template for (constexpr auto i : ::fast_io::details::index_array_range<0, k>)
		{
			::fast_io::binfmt::details::binfmt_pack_one<char>(cookie[i], args...[i]);
		}
	}
	bool ok{run_pair(&vt, prog.data(), prog.size(), k, cookie)};
	ok = run_pair(&vt_unbuf, prog.data(), prog.size(), k, cookie) && ok;
	::fast_io::io::println(ok ? "ok   " : "FAIL ", ::fast_io::mnp::os_c_str(what));
}

template <::std::size_t N>
void padded(char const *what, char8_t const (&prog)[N], ::std::uint_least64_t v) throws
{
	binfmt_cookie_element cookie[1];
	cookie[0].typetag = static_cast<binfmt_typetag>(binfmt_ct_u64 + 1u);
	cookie[0].field1 = v;
	bool ok{run_pair(&vt, prog, N, 1, cookie)};
	ok = run_pair(&vt_unbuf, prog, N, 1, cookie) && ok;
	::fast_io::io::println(ok ? "ok   " : "FAIL ", ::fast_io::mnp::os_c_str(what));
}

// field node: 13 <count> [ (code<<3)|kind payload ]...
// type=u(2), align=left(1), fill='*', width=10
inline constexpr char8_t prog_left[]{
	0x13u, 0x04u, 0x69u, 0x02u, 0x19u, 0x01u, 0x12u, 0x01u, 0x2au, 0x51u, 0x0au};
// type=u, fill='.', width=8 (default right)
inline constexpr char8_t prog_right[]{
	0x13u, 0x03u, 0x69u, 0x02u, 0x12u, 0x01u, 0x2eu, 0x51u, 0x08u};
// type=u, align=center(3), fill='-', width=9
inline constexpr char8_t prog_center[]{
	0x13u, 0x04u, 0x69u, 0x02u, 0x19u, 0x03u, 0x12u, 0x01u, 0x2du, 0x51u, 0x09u};
// type=u, flag_zero(0x30), width=8
inline constexpr char8_t prog_zero[]{
	0x13u, 0x03u, 0x69u, 0x02u, 0x30u, 0x51u, 0x08u};
// type=x(4), flag_alt(0x28), width=10
inline constexpr char8_t prog_hexalt[]{
	0x13u, 0x03u, 0x69u, 0x04u, 0x28u, 0x51u, 0x0au};
// type=x(4), flag_alt, flag_zero, width=10
inline constexpr char8_t prog_hexaltzero[]{
	0x13u, 0x04u, 0x69u, 0x04u, 0x28u, 0x30u, 0x51u, 0x0au};
// type=u, prec=3 via binfmt_fp_prec(11) on a wide short of nothing — skip;

} // namespace

int main()
{
	try
	{
		cmp<true>("int", 42);
		cmp<true>("neg", -123456);
		cmp<true>("mixed",
				  ::fast_io::basic_io_scatter_t<char>{"hello ", 6}, ::std::size_t{12345},
				  ::fast_io::basic_io_scatter_t<char>{" world ", 7}, -7,
				  ::fast_io::basic_io_scatter_t<char>{" end", 4});
		cmp<true>("hex", ::fast_io::mnp::hex(0xdeadbeefu));
		cmp<true>("oct", ::fast_io::mnp::oct(0755u));
		cmp<true>("bin", ::fast_io::mnp::bin(0b1011u));
		cmp<true>("hexsigned", ::fast_io::mnp::hex(-5));
		cmp<true>("spans", ::fast_io::basic_io_scatter_t<char>{"alpha", 5},
				  ::fast_io::basic_io_scatter_t<char>{"beta", 4},
				  ::fast_io::basic_io_scatter_t<char>{"gamma", 5});
		cmp<true>("chars", ::fast_io::basic_io_scatter_t<char>{"a", 1}, 'b',
				  ::fast_io::basic_io_scatter_t<char>{"c", 1});
		cmp<true>("u64", ::std::uint_least64_t{0x1122334455667788ull});
		cmp<true>("i128", static_cast<__int128>(-123456789012345678LL));
		cmp<true>("u128", static_cast<unsigned __int128>(0xfedcba9876543210ull));
		cmp<true>("bool", true, false);
		cmp<true>("base16", ::fast_io::mnp::base<16>(255u));
		cmp<true>("base2", ::fast_io::mnp::base<2>(255u));
		cmp<false>("print-none");
		cmp<false>("print-int", 7);
		cmp<false>("print-mixed", ::fast_io::basic_io_scatter_t<char>{"x=", 2}, 3,
				   ::fast_io::basic_io_scatter_t<char>{" y=", 3},
				   ::fast_io::basic_io_scatter_t<char>{"z", 1});

		padded("pad-left", prog_left, 42);
		padded("pad-right", prog_right, 42);
		padded("pad-center", prog_center, 42);
		padded("pad-zero", prog_zero, 42);
		padded("pad-hexalt", prog_hexalt, 255);
		padded("pad-hexaltzero", prog_hexaltzero, 255);
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln("caught: ", e);
		return 1;
	}
}
