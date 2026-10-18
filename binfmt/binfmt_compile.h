#pragma once

// binfmt compilers — text -> the compiled binary program (binfmt.h).
//
// Every front-end lowers to the SAME format: compile_fmt (fmt /
// std::format grammar), compile_c_printf (C printf grammar), plus the
// strftime (compile_time) and generic %-directive (compile_generic_pct)
// slots. They run offline and write the program straight through the same
// type-erased device the interpreter reads (binfmt_print.h), so any language
// or IO device that can hand over a basic_binfmt_device<char> is a compile
// target. There is NO string-returning entry point: to collect the bytes use
// fast_io's concat over a device-backed stream.
//
// There is no encoder object either. A node is `[uleb128 tag][payload]`
// (tag = (code << 3) | kind) and is written by plain free functions either to
// the caller's device or to a scratch byte buffer. A field's parameter list
// needs its child count up front, so the front-ends collect parameters as
// *values* (binfmt_param) and emit them afterwards — the only byte buffers
// are the literal runs and the rare nested element/chrono programs.
//
// Anything on the format-string attack surface (%n, dynamic width, named
// args, ...) is a hard error here — never an encoding.
//
// Programs are UTF-8 source-charset; a container emitter transcodes
// literal/fill payloads to the target charset afterwards.
//
// Herbceptions only: malformed / unsupported input is `throw throws`
// (std::errc). Legacy C++ exceptions are not supported.

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_core_impl/char_category/char_category.h>

#include "binfmt.h"
#include "binfmt_print.h"

namespace fast_io::binfmt
{

// ---------------------------------------------------------------------------
// diagnostics — there is no message channel; a hard error is just the throw.
// ---------------------------------------------------------------------------

[[noreturn]] inline void fail() throws
{
	throw throws ::std::errc::invalid_argument;
}

namespace details
{

// ---------------------------------------------------------------------------
// sinks — a caller device, or a plain byte buffer for nested programs
// ---------------------------------------------------------------------------

inline void sink_write(binfmt_device dev, char const *p, ::std::size_t n) throws
{
	if (n == 0)
	{
		return;
	}
	// fast path: copy straight into the device's output buffer while it has
	// room, publishing the advanced cursor; the cold path is scatter_write_all.
	auto bufs{get_output_buffer_ptrs(dev)};
	if (bufs.curr_ptr != nullptr)
	{
		auto const avail{static_cast<::std::size_t>(bufs.end_ptr - bufs.curr_ptr)};
		auto const chunk{avail < n ? avail : n};
		::std::memcpy(bufs.curr_ptr, p, chunk);
		set_output_curr_ptr(dev, bufs.curr_ptr + chunk);
		p += chunk;
		n -= chunk;
	}
	if (n != 0)
	{
		::fast_io::basic_io_scatter_t<char> sc{p, n};
		scatter_write_all(dev, __builtin_addressof(sc), 1);
	}
}

inline void sink_write(::fast_io::vector<char> &out, char const *p, ::std::size_t n) throws
{
	out.reserve(out.size() + n);
	for (::std::size_t i{}; i != n; ++i)
	{
		out.push_back_unchecked(p[i]);
	}
}

// ---------------------------------------------------------------------------
// node writers — free functions over any sink
// ---------------------------------------------------------------------------

template <typename Sink>
inline void put_leb(Sink &sk, ::std::uint_least64_t v) throws
{
	// unsigned LEB128 — at most 10 bytes for 64 bits
	char tmp[10];
	unsigned n{};
	do
	{
		::std::uint_least8_t b{static_cast<::std::uint_least8_t>(v & 0x7fu)};
		v >>= 7;
		if (v != 0)
		{
			b |= 0x80u;
		}
		tmp[n++] = static_cast<char>(b);
	} while (v != 0);
	sink_write(sk, tmp, n);
}

template <typename Sink>
inline void put_tag(Sink &sk, ::std::uint_least32_t code, ::std::uint_least32_t kind) throws
{
	put_leb(sk, (static_cast<::std::uint_least64_t>(code) << 3) | kind);
}

template <typename Sink>
inline void put_none(Sink &sk, ::std::uint_least32_t code) throws
{
	put_tag(sk, code, binfmt_kind_none);
}

template <typename Sink>
inline void put_uleb(Sink &sk, ::std::uint_least32_t code, ::std::uint_least64_t v) throws
{
	put_tag(sk, code, binfmt_kind_uleb);
	put_leb(sk, v);
}

template <typename Sink>
inline void put_sleb(Sink &sk, ::std::uint_least32_t code, ::std::int_least64_t v) throws
{
	put_tag(sk, code, binfmt_kind_sleb);
	// signed LEB128 — at most 10 bytes for 64 bits
	char tmp[10];
	unsigned n{};
	bool more{true};
	while (more)
	{
		::std::uint_least8_t b{static_cast<::std::uint_least8_t>(v & 0x7f)};
		v >>= 7;
		if ((v == 0 && (b & 0x40) == 0) || (v == -1 && (b & 0x40) != 0))
		{
			more = false;
		}
		else
		{
			b |= 0x80u;
		}
		tmp[n++] = static_cast<char>(b);
	}
	sink_write(sk, tmp, n);
}

template <typename Sink>
inline void put_bytes(Sink &sk, ::std::uint_least32_t code, ::fast_io::string_view bytes) throws
{
	put_tag(sk, code, binfmt_kind_bytes);
	put_leb(sk, bytes.size());
	sink_write(sk, bytes.data(), bytes.size());
}

template <typename Sink>
inline void put_literal(Sink &sk, ::fast_io::string_view bytes) throws
{
	put_bytes(sk, binfmt_op_literal, bytes);
}

template <typename Sink>
inline void put_list_begin(Sink &sk, ::std::uint_least32_t code, ::std::uint_least64_t nchildren) throws
{
	put_tag(sk, code, binfmt_kind_list);
	put_leb(sk, nchildren);
}

// ---------------------------------------------------------------------------
// one field parameter as a *value*, not encoded bytes. The front-ends know
// how many parameters a field has, so the list count needs no second pass.
// Only fill / element / chrono carry bytes; element and chrono build their
// nested program into `owned` so the view stays valid through moves.
// ---------------------------------------------------------------------------

struct binfmt_param
{
	::std::uint_least32_t code;
	::std::uint_least32_t kind;
	::std::uint_least64_t value;
	::fast_io::string_view view;
	::fast_io::vector<char> owned;
	bool owns;
};

inline binfmt_param param_none(::std::uint_least32_t code)
{
	binfmt_param p;
	p.code = code;
	p.kind = binfmt_kind_none;
	p.value = 0;
	p.view = {};
	p.owns = false;
	return p;
}

inline binfmt_param param_uleb(::std::uint_least32_t code, ::std::uint_least64_t v)
{
	binfmt_param p;
	p.code = code;
	p.kind = binfmt_kind_uleb;
	p.value = v;
	p.view = {};
	p.owns = false;
	return p;
}

inline binfmt_param param_bytes(::std::uint_least32_t code, ::fast_io::string_view b)
{
	binfmt_param p;
	p.code = code;
	p.kind = binfmt_kind_bytes;
	p.value = 0;
	p.view = b;
	p.owns = false;
	return p;
}

inline binfmt_param param_owned(::std::uint_least32_t code, ::fast_io::vector<char> &&b)
{
	binfmt_param p;
	p.code = code;
	p.kind = binfmt_kind_bytes;
	p.value = 0;
	p.view = {};
	p.owned = ::std::move(b);
	p.owns = true;
	return p;
}

template <typename Sink>
inline void emit_param(Sink &sk, binfmt_param const &p) throws
{
	switch (p.kind)
	{
	case binfmt_kind_none:
		put_none(sk, p.code);
		return;
	case binfmt_kind_uleb:
		put_uleb(sk, p.code, p.value);
		return;
	case binfmt_kind_sleb:
		put_sleb(sk, p.code, static_cast<::std::int_least64_t>(p.value));
		return;
	case binfmt_kind_bytes:
	{
		::fast_io::string_view const b{
			p.owns ? ::fast_io::string_view{p.owned.data(), p.owned.size()} : p.view};
		put_bytes(sk, p.code, b);
		return;
	}
	default:
		return;
	}
}

template <typename Sink>
inline void emit_param_list(Sink &sk, ::std::uint_least32_t code,
			    ::fast_io::vector<binfmt_param> const &params) throws
{
	put_list_begin(sk, code, static_cast<::std::uint_least64_t>(params.size()));
	for (auto const &p : params)
	{
		emit_param(sk, p);
	}
}

// ---------------------------------------------------------------------------
// shared lexical helpers
// ---------------------------------------------------------------------------
// classification comes from fast_io's char_category (is_c_digit / is_c_alpha)

// bytes of the UTF-8 code point starting at s[i]; 0 = invalid
inline constexpr ::std::size_t utf8_cp_size(::fast_io::u8string_view s,
					    ::std::size_t i) noexcept
{
	::std::size_t const n{s.size()};
	char8_t c{s[i]};
	if (c < 0x80)
	{
		return 1;
	}
	::std::size_t const cn{static_cast<::std::size_t>(
		(c >> 5) == 0x6	  ? 2
		: (c >> 4) == 0xE ? 3
		: (c >> 3) == 0x1E ? 4
				   : 0)};
	if (cn == 0 || cn > n - i)
	{
		return 0;
	}
	for (::std::size_t k{1}; k != cn; ++k)
	{
		if ((s[i + k] >> 6) != 0x2)
		{
			return 0;
		}
	}
	return cn;
}

inline ::std::uint_least32_t parse_u32(::fast_io::u8string_view s,
				       ::std::size_t &i, ::std::size_t j) throws
{
	::std::uint_least64_t v{};
	for (; i != j && ::fast_io::char_category::is_c_digit(s[i]); ++i)
	{
		v = v * 10u + (s[i] - u8'0');
		if (v > 0xFFFFFFFEu)
		{
			fail();
		}
	}
	return static_cast<::std::uint_least32_t>(v);
}

// ---------------------------------------------------------------------------
// literal runs — collected into a scratch buffer, flushed as one literal node
// ---------------------------------------------------------------------------

inline void lit_push(::fast_io::vector<char> &lit, char8_t c) throws
{
	lit.push_back(static_cast<char>(c));
}

template <typename Sink>
inline void lit_flush(::fast_io::vector<char> &lit, Sink &sk) throws
{
	if (!lit.is_empty())
	{
		put_literal(sk, ::fast_io::string_view{lit.data(), lit.size()});
		lit.clear();
	}
}

// ---------------------------------------------------------------------------
// printf target ABI (compile-time target)
// ---------------------------------------------------------------------------

struct c_abi
{
	bool lp64;	// long/ptrdiff/size are 64-bit (LP64 vs LLP64)
	bool wchar32;	// wchar_t is 32-bit (Linux) vs 16 (Windows)
	bool ld80;	// long double is x87 80-bit (vs f128 on arm/ppc)
};

inline c_abi default_c_abi() noexcept
{
	c_abi a;
	a.lp64 = true;
	a.wchar32 = true;
	a.ld80 = true;
	return a;
}

inline ::std::uint_least32_t length_int_ctype(unsigned len_kind, bool u, c_abi abi) noexcept
{
	// len_kind: 0 none/h/hh (promote to i32), 1 l, 2 ll/j/z/t/q/w64, 3 L
	switch (len_kind)
	{
	case 0:
		return u ? binfmt_ct_u32 : binfmt_ct_i32;
	case 1:
		return abi.lp64 ? (u ? binfmt_ct_u64 : binfmt_ct_i64)
				: (u ? binfmt_ct_u32 : binfmt_ct_i32);
	case 2:
		return u ? binfmt_ct_u64 : binfmt_ct_i64;
	default:
		return u ? binfmt_ct_u128 : binfmt_ct_i128;
	}
}

// ---------------------------------------------------------------------------
// pct (strftime) compiler — LC_TIME fmts + generic %-letter slots
// ---------------------------------------------------------------------------

// returns 0 when the letter is not an strftime conversion
inline ::std::uint_least32_t pct_conv_lookup(char8_t c) noexcept
{
	switch (c)
	{
	case u8'a': return binfmt_pct_a;
	case u8'A': return binfmt_pct_A;
	case u8'b': return binfmt_pct_b;
	case u8'h': return binfmt_pct_b; // %h canonicalizes to %b
	case u8'B': return binfmt_pct_B;
	case u8'c': return binfmt_pct_c;
	case u8'C': return binfmt_pct_C;
	case u8'd': return binfmt_pct_d;
	case u8'e': return binfmt_pct_e;
	case u8'F': return binfmt_pct_F;
	case u8'G': return binfmt_pct_G;
	case u8'H': return binfmt_pct_H;
	case u8'I': return binfmt_pct_I;
	case u8'j': return binfmt_pct_j;
	case u8'k': return binfmt_pct_k;
	case u8'l': return binfmt_pct_l;
	case u8'm': return binfmt_pct_m;
	case u8'M': return binfmt_pct_M;
	case u8'p': return binfmt_pct_p;
	case u8'P': return binfmt_pct_P;
	case u8'r': return binfmt_pct_r;
	case u8'R': return binfmt_pct_R;
	case u8's': return binfmt_pct_s;
	case u8'S': return binfmt_pct_S;
	case u8'T': return binfmt_pct_T;
	case u8'u': return binfmt_pct_u;
	case u8'U': return binfmt_pct_U;
	case u8'V': return binfmt_pct_V;
	case u8'w': return binfmt_pct_w;
	case u8'W': return binfmt_pct_W;
	case u8'x': return binfmt_pct_x;
	case u8'X': return binfmt_pct_X;
	case u8'Y': return binfmt_pct_Y;
	case u8'z': return binfmt_pct_z;
	case u8'Z': return binfmt_pct_Z;
	case u8'+': return binfmt_pct_plus;
	default: return 0;
	}
}

struct pct_options
{
	bool allow_nested_fmt; // chrono specs: '{...}' fields allowed
};

// ---------------------------------------------------------------------------
// forward declarations — the fmt field parser and the pct compiler are
// mutually recursive (a chrono spec may embed {} fields)
// ---------------------------------------------------------------------------

template <typename Sink>
void compile_pct_impl(Sink &sk, ::fast_io::u8string_view s, pct_options opts,
		      unsigned depth) throws;

template <typename Sink>
::std::size_t parse_fmt_field(Sink &sk, ::fast_io::u8string_view s, ::std::size_t pos,
			      unsigned depth) throws;

// ---------------------------------------------------------------------------
// fmt front-end ({} fields) — parameter values only
// ---------------------------------------------------------------------------

struct fmt_spec
{
	::fast_io::vector<binfmt_param> params;
	bool ok;
};

// standard spec parse -> field parameters; on any non-standard content
// returns ok=false (caller retries as chrono pct program)
inline fmt_spec parse_fmt_standard(::fast_io::u8string_view spec, unsigned depth) throws
{
	fmt_spec f;
	f.ok = true;
	::std::size_t i{};
	::std::size_t const n{spec.size()};
	// [fill]align
	::std::uint_least32_t al{};
	auto align_of = [](char8_t c) noexcept -> ::std::uint_least32_t {
		switch (c)
		{
		case u8'<': return binfmt_align_left;
		case u8'>': return binfmt_align_right;
		case u8'^': return binfmt_align_center;
		default: return 0;
		}
	};
	if (i != n && align_of(spec[i]))
	{
		al = align_of(spec[i]);
		++i;
	}
	else if (::std::size_t cp{utf8_cp_size(spec, i)};
		 cp != 0 && i + cp < n && align_of(spec[i + cp]))
	{
		// fill bytes = raw code point bytes in source charset
		f.params.push_back(param_bytes(
			binfmt_fp_fill,
			::fast_io::string_view{reinterpret_cast<char const *>(spec.data() + i), cp}));
		al = align_of(spec[i + cp]);
		i += cp + 1;
	}
	if (al)
	{
		f.params.push_back(param_uleb(binfmt_fp_align, al));
	}
	// sign
	if (i != n)
	{
		::std::uint_least32_t sg{};
		switch (spec[i])
		{
		case u8'+': sg = binfmt_sign_plus; break;
		case u8'-': sg = binfmt_sign_minus; break;
		case u8' ': sg = binfmt_sign_space; break;
		default: break;
		}
		if (sg)
		{
			f.params.push_back(param_uleb(binfmt_fp_sign, sg));
			++i;
		}
	}
	// '#'
	if (i != n && spec[i] == u8'#')
	{
		f.params.push_back(param_none(binfmt_fp_flag_alt));
		++i;
	}
	// '0'
	if (i != n && spec[i] == u8'0')
	{
		f.params.push_back(param_none(binfmt_fp_flag_zero));
		++i;
	}
	// width — digits only; '{' dynamic width rejected
	if (i != n && spec[i] == u8'{')
	{
		fail();
	}
	if (i != n && ::fast_io::char_category::is_c_digit(spec[i]))
	{
		f.params.push_back(param_uleb(binfmt_fp_width, parse_u32(spec, i, n)));
	}
	// precision
	bool has_prec{};
	if (i != n && spec[i] == u8'.')
	{
		++i;
		has_prec = true;
		if (i != n && spec[i] == u8'{')
		{
			fail();
		}
		if (i == n || !::fast_io::char_category::is_c_digit(spec[i]))
		{
			fail();
		}
		f.params.push_back(param_uleb(binfmt_fp_prec, parse_u32(spec, i, n)));
	}
	// 'L'
	if (i != n && spec[i] == u8'L')
	{
		f.params.push_back(param_none(binfmt_fp_flag_locale));
		++i;
	}
	if (i == n)
	{
		return f;
	}
	// type / range
	bool upper{};
	// returns 0 when the letter is not a standard conversion
	auto type_of = [](char8_t c) noexcept -> ::std::uint_least32_t {
		switch (c)
		{
		case u8'b': case u8'B': return binfmt_conv_b;
		case u8'c': return binfmt_conv_c;
		case u8'd': return binfmt_conv_d;
		case u8'o': return binfmt_conv_o;
		case u8'x': case u8'X': return binfmt_conv_x;
		case u8's': return binfmt_conv_s;
		case u8'?': return binfmt_conv_debug;
		case u8'p': case u8'P': return binfmt_conv_p;
		case u8'a': case u8'A': return binfmt_conv_hexf;
		case u8'e': case u8'E': return binfmt_conv_scip;
		case u8'f': case u8'F': return binfmt_conv_fixp;
		case u8'g': case u8'G': return binfmt_conv_genp;
		default: return 0;
		}
	};
	char8_t tc{spec[i]};
	upper = (tc >= u8'A' && tc <= u8'Z');
	auto ty{type_of(tc)};
	if (ty != 0)
	{
		// float letters with absent precision bake the fmt default
		switch (ty)
		{
		case binfmt_conv_fixp:
		case binfmt_conv_scip:
		case binfmt_conv_genp:
			if (!has_prec)
			{
				f.params.push_back(param_uleb(binfmt_fp_prec, 6));
			}
			break;
		default:
			break;
		}
		if (ty == binfmt_conv_hexf && has_prec)
		{
			ty = binfmt_conv_hexfp;
		}
		f.params.push_back(param_uleb(binfmt_fp_type, ty));
		if (upper)
		{
			f.params.push_back(param_none(binfmt_fp_flag_upper));
		}
		++i;
	}
	else if (tc == u8'n' || tc == u8'm' || tc == u8':')
	{
		// range presentation: n naked, m map, ':' default with element spec
		::std::uint_least32_t rt{tc == u8'n' ? binfmt_conv_rngn
					: tc == u8'm' ? binfmt_conv_rngm
						      : binfmt_conv_rng};
		++i;
		if (i != n && spec[i] == u8':')
		{
			++i;
			// element spec follows — its own field-children grammar
			fmt_spec inner{parse_fmt_standard(spec.subview_unchecked(i), depth + 1)};
			if (!inner.ok)
			{
				f.ok = false;
				return f;
			}
			::fast_io::vector<char> eb;
			for (auto const &p : inner.params)
			{
				emit_param(eb, p);
			}
			f.params.push_back(param_owned(binfmt_fp_element, ::std::move(eb)));
			i = n;
		}
		else if (i != n)
		{
			f.ok = false;
			return f;
		}
		f.params.push_back(param_uleb(binfmt_fp_type, rt));
	}
	else
	{
		f.ok = false;
		return f;
	}
	if (i != n)
	{
		f.ok = false;
		return f;
	}
	return f;
}

// ---------------------------------------------------------------------------
// the pct program writer
// ---------------------------------------------------------------------------

template <typename Sink>
inline void compile_pct_impl(Sink &sk, ::fast_io::u8string_view s, pct_options opts,
			     unsigned depth) throws
{
	if (depth > 8)
	{
		fail();
	}
	::fast_io::vector<char> lit;
	::std::size_t const n{s.size()};
	for (::std::size_t i{}; i != n;)
	{
		char8_t c{s[i]};
		if (c == u8'{' && opts.allow_nested_fmt)
		{
			lit_flush(lit, sk);
			i = parse_fmt_field(sk, s, i, depth);
			continue;
		}
		if (c != u8'%')
		{
			lit_push(lit, c);
			++i;
			continue;
		}
		lit_flush(lit, sk);
		++i;
		if (i == n)
		{
			lit_push(lit, u8'%');
			break;
		}
		switch (s[i])
		{
		case u8'%': lit_push(lit, u8'%'); ++i; continue;
		case u8'n': lit_push(lit, u8'\n'); ++i; continue;
		case u8't': lit_push(lit, u8'\t'); ++i; continue;
		default: break;
		}
		::std::uint_least32_t pad{};
		::std::uint_least32_t casef{};
		for (; i != n; ++i)
		{
			switch (s[i])
			{
			case u8'-': pad |= 1; continue;
			case u8'_': pad |= 2; continue;
			case u8'0': pad |= 4; continue;
			case u8'^': casef |= 1; continue;
			case u8'#': casef |= 2; continue;
			default: break;
			}
			break;
		}
		::std::uint_least32_t width{};
		bool has_width{};
		if (i != n && ::fast_io::char_category::is_c_digit(s[i]))
		{
			width = parse_u32(s, i, n);
			has_width = true;
		}
		::std::uint_least32_t colons{};
		while (i != n && s[i] == u8':' && colons < 3)
		{
			++colons;
			++i;
		}
		::std::uint_least32_t modifier{};
		if (i != n && s[i] == u8'E')
		{
			modifier = binfmt_pct_modifier_era;
			++i;
		}
		else if (i != n && s[i] == u8'O')
		{
			modifier = binfmt_pct_modifier_alt;
			++i;
		}
		if (i == n)
		{
			fail();
		}
		char8_t conv{s[i++]};
		if (colons != 0 && conv != u8'z')
		{
			fail();
		}
		// 2-digit year forms substitute the 4-digit equivalent
		// (spec Part 6): %y->%Y, %Oy->%OY, %Ey->%EY (same encoding),
		// %g->%G, %D->%m/%d/%Y
		if (conv == u8'D')
		{
			put_uleb(sk, binfmt_op_pct, binfmt_pct_m);
			put_literal(sk, "/");
			put_uleb(sk, binfmt_op_pct, binfmt_pct_d);
			put_literal(sk, "/");
			put_uleb(sk, binfmt_op_pct, binfmt_pct_Y);
			continue;
		}
		if (conv == u8'y')
		{
			conv = u8'Y';
		}
		else if (conv == u8'g')
		{
			conv = u8'G';
		}
		::std::uint_least32_t const cv{pct_conv_lookup(conv)};
		if (cv == 0)
		{
			fail();
		}
		if (!pad && !casef && !modifier && !has_width && !colons)
		{
			// scalar form: tag + uleb conv — 2 bytes
			put_uleb(sk, binfmt_op_pct, cv);
			continue;
		}
		::fast_io::vector<binfmt_param> params;
		params.push_back(param_uleb(binfmt_pctp_conv, cv));
		if (modifier)
		{
			params.push_back(param_uleb(binfmt_pctp_modifier, modifier));
		}
		if (pad)
		{
			params.push_back(param_uleb(binfmt_pctp_pad, pad));
		}
		if (casef)
		{
			params.push_back(param_uleb(binfmt_pctp_case, casef));
		}
		if (colons)
		{
			params.push_back(param_uleb(binfmt_pctp_colons, colons));
		}
		if (has_width)
		{
			params.push_back(param_uleb(binfmt_fp_width, width));
		}
		emit_param_list(sk, binfmt_op_pct, params);
	}
	lit_flush(lit, sk);
}

// ---------------------------------------------------------------------------
// one {…} field -> a `field` list node written to the sink
// ---------------------------------------------------------------------------

template <typename Sink>
inline ::std::size_t parse_fmt_field(Sink &sk, ::fast_io::u8string_view s, ::std::size_t pos,
				     unsigned depth) throws
{
	if (depth > 8)
	{
		fail();
	}
	::std::size_t i{pos + 1};
	::std::size_t const n{s.size()};
	::fast_io::vector<binfmt_param> params;
	// arg id
	if (i != n && ::fast_io::char_category::is_c_digit(s[i]))
	{
		params.push_back(param_uleb(binfmt_fp_arg, parse_u32(s, i, n)));
	}
	else if (i != n && s[i] != u8':' && s[i] != u8'}' && s[i] != u8'!')
	{
		fail();
	}
	if (i != n && s[i] == u8'!')
	{
		fail();
	}
	if (i != n && s[i] == u8':')
	{
		++i;
		::std::size_t const spec_begin{i};
		unsigned lvl{};
		for (; i != n; ++i)
		{
			if (s[i] == u8'{')
			{
				++lvl;
			}
			else if (s[i] == u8'}')
			{
				if (lvl == 0)
				{
					break;
				}
				--lvl;
			}
		}
		if (i == n)
		{
			fail();
		}
		::fast_io::u8string_view spec{s.subview_unchecked(spec_begin, i - spec_begin)};
		fmt_spec f{parse_fmt_standard(spec, depth)};
		if (f.ok)
		{
			for (auto &p : f.params)
			{
				params.push_back(::std::move(p));
			}
		}
		else
		{
			// the whole spec is a chrono (pct) program
			::fast_io::vector<char> blob;
			compile_pct_impl(blob, spec, pct_options{.allow_nested_fmt = true}, depth + 1);
			params.push_back(param_owned(binfmt_fp_chrono, ::std::move(blob)));
		}
	}
	if (i >= n || s[i] != u8'}')
	{
		fail();
	}
	emit_param_list(sk, binfmt_op_field, params);
	return i + 1;
}

// ---------------------------------------------------------------------------
// fmt front-end driver
// ---------------------------------------------------------------------------

template <typename Sink>
inline void compile_fmt_sink(Sink &sk, ::fast_io::u8string_view s) throws
{
	::fast_io::vector<char> lit;
	::std::size_t const n{s.size()};
	for (::std::size_t i{}; i != n;)
	{
		char8_t c{s[i]};
		if (c == u8'{')
		{
			if (i + 1 != n && s[i + 1] == u8'{')
			{
				lit_push(lit, u8'{');
				i += 2;
				continue;
			}
			lit_flush(lit, sk);
			i = parse_fmt_field(sk, s, i, 0);
			continue;
		}
		if (c == u8'}')
		{
			if (i + 1 != n && s[i + 1] == u8'}')
			{
				lit_push(lit, u8'}');
				i += 2;
				continue;
			}
			fail();
		}
		lit_push(lit, c);
		++i;
	}
	lit_flush(lit, sk);
}

// ---------------------------------------------------------------------------
// printf front-end — compiles into the same field nodes as fmt
// ---------------------------------------------------------------------------

template <typename Sink>
inline void compile_c_printf_impl(Sink &sk, ::fast_io::u8string_view s, c_abi abi) throws
{
	::fast_io::vector<char> lit;
	::std::size_t const n{s.size()};
	for (::std::size_t i{}; i != n;)
	{
		char8_t c{s[i]};
		if (c != u8'%')
		{
			lit_push(lit, c);
			++i;
			continue;
		}
		lit_flush(lit, sk);
		++i;
		if (i == n)
		{
			fail();
		}
		if (s[i] == u8'%')
		{
			lit_push(lit, u8'%');
			++i;
			continue;
		}
		::fast_io::vector<binfmt_param> params;
		// [argnum$]
		if (::fast_io::char_category::is_c_digit(s[i]))
		{
			::std::size_t j{i};
			while (j != n && ::fast_io::char_category::is_c_digit(s[j]))
			{
				++j;
			}
			if (j != n && s[j] == u8'$')
			{
				::std::uint_least32_t v{parse_u32(s, i, j)};
				if (v == 0)
				{
					fail();
				}
				params.push_back(param_uleb(binfmt_fp_arg, v - 1u));
				i = j + 1;
			}
		}
		// flags
		for (; i != n; ++i)
		{
			switch (s[i])
			{
			case u8'-':
				params.push_back(param_uleb(binfmt_fp_align, binfmt_align_left));
				continue;
			case u8'+':
				params.push_back(param_uleb(binfmt_fp_sign, binfmt_sign_plus));
				continue;
			case u8' ':
				params.push_back(param_uleb(binfmt_fp_sign, binfmt_sign_space));
				continue;
			case u8'#':
				params.push_back(param_none(binfmt_fp_flag_alt));
				continue;
			case u8'0':
				params.push_back(param_none(binfmt_fp_flag_zero));
				continue;
			case u8'\'':
				params.push_back(param_none(binfmt_fp_flag_group));
				continue;
			case u8'I':
				params.push_back(param_none(binfmt_fp_flag_outdigits));
				continue;
			default:
				break;
			}
			break;
		}
		// width
		if (i != n && s[i] == u8'*')
		{
			fail();
		}
		if (i != n && ::fast_io::char_category::is_c_digit(s[i]))
		{
			params.push_back(param_uleb(binfmt_fp_width, parse_u32(s, i, n)));
		}
		// precision
		bool has_prec{};
		::std::uint_least32_t prec{};
		if (i != n && s[i] == u8'.')
		{
			++i;
			has_prec = true;
			if (i != n && s[i] == u8'*')
			{
				fail();
			}
			if (i != n && ::fast_io::char_category::is_c_digit(s[i]))
			{
				prec = parse_u32(s, i, n);
			}
			// bare '.' -> 0
		}
		// length
		unsigned len_kind{}; // 0 none/h/hh, 1 l, 2 ll/j/z/t/q/w, 3 L
		bool len_wide{};	 // l before c/s
		if (i != n)
		{
			switch (s[i])
			{
			case u8'h':
				++i;
				if (i != n && s[i] == u8'h')
				{
					++i;
				}
				break;
			case u8'l':
				++i;
				len_kind = 1;
				len_wide = true;
				if (i != n && s[i] == u8'l')
				{
					++i;
					len_kind = 2;
				}
				break;
			case u8'j':
			case u8'z':
			case u8't':
			case u8'q':
				++i;
				len_kind = 2;
				break;
			case u8'L':
				++i;
				len_kind = 3;
				break;
			case u8'w':
			{
				++i;
				if (i != n && s[i] == u8'f')
				{
					++i;
				}
				auto bits{parse_u32(s, i, n)};
				len_kind = bits == 64 ? 2u : 0u; // w8/w16/w32 promote to int
				break;
			}
			default:
				break;
			}
		}
		if (i == n)
		{
			fail();
		}
		char8_t conv{s[i++]};
		switch (conv)
		{
		case u8'n':
		case u8'm':
		case u8'[':
			fail();
		default:
			break;
		}
		bool upper{conv >= u8'A' && conv <= u8'Z'};
		::std::uint_least32_t ct{binfmt_ct_other};
		::std::uint_least32_t ty{};
		switch (conv)
		{
		case u8'd':
		case u8'i':
			ty = binfmt_conv_d;
			ct = length_int_ctype(len_kind, false, abi);
			break;
		case u8'u':
			ty = binfmt_conv_u;
			ct = length_int_ctype(len_kind, true, abi);
			break;
		case u8'o':
			ty = binfmt_conv_o;
			ct = length_int_ctype(len_kind, true, abi);
			break;
		case u8'x':
		case u8'X':
			ty = binfmt_conv_x;
			ct = length_int_ctype(len_kind, true, abi);
			break;
		case u8'b':
		case u8'B':
			ty = binfmt_conv_b;
			ct = length_int_ctype(len_kind, true, abi);
			break;
		case u8'f':
		case u8'F':
			ty = binfmt_conv_fixp;
			if (!has_prec)
			{
				has_prec = true;
				prec = 6;
			}
			ct = len_kind == 3 ? (abi.ld80 ? binfmt_ct_f80 : binfmt_ct_f128)
					 : binfmt_ct_f64;
			break;
		case u8'e':
		case u8'E':
			ty = binfmt_conv_scip;
			if (!has_prec)
			{
				has_prec = true;
				prec = 6;
			}
			ct = len_kind == 3 ? (abi.ld80 ? binfmt_ct_f80 : binfmt_ct_f128)
					 : binfmt_ct_f64;
			break;
		case u8'g':
		case u8'G':
			ty = binfmt_conv_genp;
			if (!has_prec)
			{
				has_prec = true;
				prec = 6;
			}
			ct = len_kind == 3 ? (abi.ld80 ? binfmt_ct_f80 : binfmt_ct_f128)
					 : binfmt_ct_f64;
			break;
		case u8'a':
		case u8'A':
			ty = has_prec ? binfmt_conv_hexfp : binfmt_conv_hexf;
			ct = len_kind == 3 ? (abi.ld80 ? binfmt_ct_f80 : binfmt_ct_f128)
					 : binfmt_ct_f64;
			break;
		case u8'c':
			ty = binfmt_conv_c;
			ct = len_wide ? (abi.wchar32 ? binfmt_ct_c32 : binfmt_ct_c16)
				      : binfmt_ct_c8;
			break;
		case u8'C': // glibc alias for %lc
			ty = binfmt_conv_c;
			ct = abi.wchar32 ? binfmt_ct_c32 : binfmt_ct_c16;
			break;
		case u8's':
			ty = binfmt_conv_s;
			ct = len_wide ? (abi.wchar32 ? binfmt_ct_c32ptr : binfmt_ct_c16ptr)
				      : binfmt_ct_c8ptr;
			break;
		case u8'S': // glibc alias for %ls
			ty = binfmt_conv_s;
			ct = abi.wchar32 ? binfmt_ct_c32ptr : binfmt_ct_c16ptr;
			break;
		case u8'p':
			ty = binfmt_conv_p;
			ct = binfmt_ct_ptr;
			break;
		default:
			fail();
		}
		if (has_prec)
		{
			params.push_back(param_uleb(binfmt_fp_prec, prec));
		}
		if (ct != binfmt_ct_other)
		{
			params.push_back(param_uleb(binfmt_fp_ctype, ct));
		}
		params.push_back(param_uleb(binfmt_fp_type, ty));
		if (upper)
		{
			params.push_back(param_none(binfmt_fp_flag_upper));
		}
		emit_param_list(sk, binfmt_op_field, params);
	}
	lit_flush(lit, sk);
}

// ---------------------------------------------------------------------------
// generic %-letter slots (LC_NAME.name_fmt, LC_ADDRESS.postal_fmt,
// LC_TELEPHONE.tel_*_fmt): each %X letter is an argument selector, so it
// compiles to field{arg=letter}. No %n/%t escapes, no strftime conv table:
// the letter IS the argument identity.
// ---------------------------------------------------------------------------

template <typename Sink>
inline void compile_generic_pct_impl(Sink &sk, ::fast_io::u8string_view s) throws
{
	::fast_io::vector<char> lit;
	::std::size_t const n{s.size()};
	for (::std::size_t i{}; i != n;)
	{
		char8_t c{s[i]};
		if (c != u8'%')
		{
			lit_push(lit, c);
			++i;
			continue;
		}
		lit_flush(lit, sk);
		++i;
		if (i == n)
		{
			fail();
		}
		char8_t c2{s[i]};
		++i;
		if (c2 == u8'%')
		{
			lit_push(lit, u8'%');
			continue;
		}
		if (!::fast_io::char_category::is_c_alpha(c2))
		{
			fail();
		}
		::fast_io::vector<binfmt_param> params;
		params.push_back(param_uleb(binfmt_fp_arg, c2));
		emit_param_list(sk, binfmt_op_field, params);
	}
	lit_flush(lit, sk);
}

} // namespace details

// ---------------------------------------------------------------------------
// entry points — a device and a C byte range. No string is ever returned:
// drive a device (fast_io's concat over a device-backed stream collects the
// bytes if a buffer is what the caller wants).
// ---------------------------------------------------------------------------

inline void compile_fmt(binfmt_device dev, char const *base, ::std::size_t len) throws
{
	details::compile_fmt_sink(
		dev, ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(base), len});
}

inline void compile_c_printf(binfmt_device dev, char const *base, ::std::size_t len) throws
{
	details::compile_c_printf_impl(
		dev, ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(base), len},
		details::default_c_abi());
}

inline void compile_time(binfmt_device dev, char const *base, ::std::size_t len) throws
{
	details::compile_pct_impl(
		dev, ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(base), len}, {}, 0);
}

inline void compile_generic_pct(binfmt_device dev, char const *base, ::std::size_t len) throws
{
	details::compile_generic_pct_impl(
		dev, ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(base), len});
}

} // namespace fast_io::binfmt
