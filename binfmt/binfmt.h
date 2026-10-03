#pragma once

// binfmt — compiled binary format-string program format.
// See readme.md in this directory for the full specification.
//
// Header-only, freestanding-friendly: only <cstdint>/<cstddef>/<cstring>
// plus std::string as the encoder's byte sink. The decoder works purely
// on byte spans and does bounds checking on every read.

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>

namespace fast_io_i18n
{
namespace binfmt
{

using u8 = ::std::uint8_t;
using u32 = ::std::uint32_t;
using i32 = ::std::int32_t;

inline constexpr u32 format_version{1};

enum class family : u8
{
	fmt = 1,	// fmt.dev / std::format replacement fields
	pct = 2,	// strftime/chrono % directives + generic %-letter slots
	stdio = 3,	// printf conversions
};

enum class opcode : u8
{
	end = 0,
	literal = 1,
	field_fmt = 2,
	field_stdio = 3,
	field_pct = 4,
};

// Safety here is structural, not enforced: the op set cannot express
// writes, lookups, or dynamic specs — there are no encodings for named
// args, arg-driven width/precision, !r-style conversions, or %n/%m, and
// an argref is only an index (arg content can never become format code).
// The blob is trusted compiled data — same trust level as generated .cc
// tables — so the decoder does bounds-checking for well-formedness only.

enum class argref_kind : u8
{
	automatic = 0,	// fmt {} / stdio next arg
	index = 1,	// fmt {N} / stdio %N$ (0-based; bound-checked by user)
};

enum class dynparam_kind : u8
{
	none = 0,
	value = 1,	// literal value only — no arg-driven params
};

enum class fmt_align : u8
{
	none = 0,
	left = 1,	// <
	right = 2,	// >
	center = 3,	// ^
};

enum class fmt_sign : u8
{
	none = 0,
	plus = 1,
	minus = 2,
	space = 3,
};

// FIELD_FMT flags bitfield
inline constexpr u8 fmt_flag_alternate{1u << 0};	// '#'
inline constexpr u8 fmt_flag_zero{1u << 1};		// '0'
inline constexpr u8 fmt_flag_locale{1u << 2};	// 'L'

enum class spec_kind : u8
{
	none = 0,
	standard = 1,
	chrono = 2,	// payload: embedded pct blob
};

// FIELD_STDIO flags bitfield
inline constexpr u8 stdio_flag_minus{1u << 0};
inline constexpr u8 stdio_flag_plus{1u << 1};
inline constexpr u8 stdio_flag_space{1u << 2};
inline constexpr u8 stdio_flag_alternate{1u << 3};	// '#'
inline constexpr u8 stdio_flag_zero{1u << 4};
inline constexpr u8 stdio_flag_group{1u << 5};		// '\''
inline constexpr u8 stdio_flag_glibc_i{1u << 6};	// 'I' outdigits

enum class stdio_length : u8
{
	none = 0,
	hh = 1,
	h = 2,
	l = 3,
	ll = 4,
	j = 5,
	z = 6,
	t = 7,
	L = 8,
	q = 9,	// glibc alias for ll
	w = 10,	// C23 intN_t — length_bits carries N
	wf = 11,	// C23 int_fastN_t — length_bits carries N
};

// FIELD_PCT bitfields
inline constexpr u8 pct_pad_minus{1u << 0};	// '-'
inline constexpr u8 pct_pad_underscore{1u << 1};	// '_'
inline constexpr u8 pct_pad_zero{1u << 2};	// '0'
inline constexpr u8 pct_case_upper{1u << 0};	// '^'
inline constexpr u8 pct_case_swap{1u << 1};	// '#'

enum class pct_modifier : u8
{
	none = 0,
	era = 1,		// E
	alternative = 2,	// O
};

// ---------------------------------------------------------------------------
// payload structs (encoder side) — strings as views; the builder copies
// ---------------------------------------------------------------------------

struct argref
{
	argref_kind kind{argref_kind::automatic};
	u32 index{};
};

struct dynparam
{
	dynparam_kind kind{dynparam_kind::none};
	u32 value{};
};

struct fmt_standard_spec
{
	::std::string_view fill{};	// 0..4 bytes, one code point
	fmt_align align{fmt_align::none};
	fmt_sign sign{fmt_sign::none};
	u8 flags{};
	dynparam width{};
	dynparam precision{};
	u8 type{};			// ASCII type char, 0 = none
};

struct field_fmt_desc
{
	argref arg{};
	spec_kind spec{spec_kind::none};
	fmt_standard_spec standard{};		// when spec==standard
	::std::string_view chrono_blob{};	// when spec==chrono: complete blob
};

struct field_stdio_desc
{
	argref arg{};				// automatic or index only
	u8 flags{};
	stdio_length length{stdio_length::none};
	u8 length_bits{};
	u8 conv{};				// ASCII conversion char
	dynparam width{};
	dynparam precision{};
};

struct field_pct_desc
{
	u8 conv{};				// ASCII conversion char
	u8 pad_flags{};
	u8 case_flags{};
	pct_modifier modifier{pct_modifier::none};
	u8 colons{};
	bool has_width{};
	u32 width{};
};

// ---------------------------------------------------------------------------
// encoder
// ---------------------------------------------------------------------------

class builder
{
public:
	explicit builder(family fam) noexcept
	{
		buf_.resize(8);
		write_u32_at(0, static_cast<u32>(fam) | (format_version << 8));
	}

	void op_literal(::std::string_view bytes)
	{
		put_u8(static_cast<u8>(opcode::literal));
		put_u32(static_cast<u32>(bytes.size()));
		buf_.append(bytes.data(), bytes.size());
	}

	void op_field_fmt(field_fmt_desc const& d)
	{
		put_u8(static_cast<u8>(opcode::field_fmt));
		put_argref(d.arg);
		put_u8(static_cast<u8>(d.spec));
		switch (d.spec)
		{
		case spec_kind::standard:
		{
			auto const& s{d.standard};
			put_u8(static_cast<u8>(s.fill.size()));
			buf_.append(s.fill.data(), s.fill.size());
			put_u8(static_cast<u8>(s.align));
			put_u8(static_cast<u8>(s.sign));
			put_u8(s.flags);
			put_dynparam(s.width);
			put_dynparam(s.precision);
			put_u8(s.type);
			break;
		}
		case spec_kind::chrono:
			put_u32(static_cast<u32>(d.chrono_blob.size()));
			buf_.append(d.chrono_blob.data(), d.chrono_blob.size());
			break;
		default:
			break;
		}
	}

	void op_field_stdio(field_stdio_desc const& d)
	{
		put_u8(static_cast<u8>(opcode::field_stdio));
		put_argref(d.arg);
		put_u8(d.flags);
		put_u8(static_cast<u8>(d.length));
		put_u8(d.length_bits);
		put_u8(d.conv);
		put_dynparam(d.width);
		put_dynparam(d.precision);
	}

	void op_field_pct(field_pct_desc const& d)
	{
		put_u8(static_cast<u8>(opcode::field_pct));
		put_u8(d.conv);
		put_u8(d.pad_flags);
		put_u8(d.case_flags);
		put_u8(static_cast<u8>(d.modifier));
		put_u8(d.colons);
		put_u8(d.has_width ? 1 : 0);
		if (d.has_width)
			put_u32(d.width);
	}

	[[nodiscard]] ::std::string finish()
	{
		put_u8(static_cast<u8>(opcode::end));
		write_u32_at(4, static_cast<u32>(buf_.size()) - 8u);
		return ::std::move(buf_);
	}

private:
	::std::string buf_;

	void put_u8(u8 v)
	{
		buf_.push_back(static_cast<char>(v));
	}
	void put_u32(u32 v)
	{
		char b[4];
		::std::memcpy(b, __builtin_addressof(v), 4);
		buf_.append(b, 4);
	}
	void write_u32_at(::std::size_t pos, u32 v)
	{
		::std::memcpy(buf_.data() + pos, __builtin_addressof(v), 4);
	}
	void put_argref(argref const& a)
	{
		put_u8(static_cast<u8>(a.kind));
		if (a.kind == argref_kind::index)
			put_u32(a.index);
	}
	void put_dynparam(dynparam const& p)
	{
		put_u8(static_cast<u8>(p.kind));
		if (p.kind == dynparam_kind::value)
			put_u32(p.value);
	}
};

// ---------------------------------------------------------------------------
// decoder — bounds-checked cursor over a blob
// ---------------------------------------------------------------------------

struct reader
{
	u8 const* cur{};
	u8 const* end{};

	bool get_u8(u8& v) noexcept
	{
		if (cur >= end)
			return false;
		v = *cur++;
		return true;
	}
	bool get_u32(u32& v) noexcept
	{
		if (static_cast<::std::size_t>(end - cur) < 4)
			return false;
		::std::memcpy(__builtin_addressof(v), cur, 4);
		cur += 4;
		return true;
	}
	bool get_bytes(::std::string_view& v, u32 n) noexcept
	{
		if (static_cast<::std::size_t>(end - cur) < n)
			return false;
		v = {reinterpret_cast<char const*>(cur), n};
		cur += n;
		return true;
	}
};

// Parse a blob header. On success `r` is positioned at the first op.
inline bool open_program(u8 const* blob, ::std::size_t blob_size,
			 family& fam, reader& r) noexcept
{
	if (blob_size < 8)
		return false;
	u32 fv{}, size{};
	::std::memcpy(__builtin_addressof(fv), blob, 4);
	::std::memcpy(__builtin_addressof(size), blob + 4, 4);
	fam = static_cast<family>(fv & 0xff);
	u32 ver{(fv >> 8) & 0xff};
	if (ver != format_version || 8ull + size > blob_size)
		return false;
	r.cur = blob + 8;
	r.end = r.cur + size;
	return true;
}

inline bool open_program(::std::string_view blob, family& fam, reader& r) noexcept
{
	return open_program(reinterpret_cast<u8 const*>(blob.data()), blob.size(), fam, r);
}

// Decoded op views — all string_view/name members point into the blob.
struct dec_argref
{
	argref_kind kind{};
	u32 index{};
};

struct dec_dynparam
{
	dynparam_kind kind{};
	u32 value{};
};

struct dec_field_fmt
{
	dec_argref arg{};
	spec_kind spec{};
	// standard:
	::std::string_view fill{};
	fmt_align align{};
	fmt_sign sign{};
	u8 flags{};
	dec_dynparam width{}, precision{};
	u8 type{};
	// chrono:
	::std::string_view chrono_blob{};
};

struct dec_field_stdio
{
	dec_argref arg{};
	u8 flags{};
	stdio_length length{};
	u8 length_bits{};
	u8 conv{};
	dec_dynparam width{}, precision{};
};

struct dec_field_pct
{
	u8 conv{};
	u8 pad_flags{};
	u8 case_flags{};
	pct_modifier modifier{};
	u8 colons{};
	bool has_width{};
	u32 width{};
};

inline bool read_argref(reader& r, dec_argref& a) noexcept
{
	u8 k{};
	if (!r.get_u8(k))
		return false;
	a.kind = static_cast<argref_kind>(k);
	switch (a.kind)
	{
	case argref_kind::automatic:
		return true;
	case argref_kind::index:
		return r.get_u32(a.index);
	default:
		return false;
	}
}

inline bool read_dynparam(reader& r, dec_dynparam& p) noexcept
{
	u8 k{};
	if (!r.get_u8(k))
		return false;
	p.kind = static_cast<dynparam_kind>(k);
	switch (p.kind)
	{
	case dynparam_kind::none:
		return true;
	case dynparam_kind::value:
		return r.get_u32(p.value);
	default:
		return false;
	}
}

// Read the next op. Returns opcode; for field ops the matching out-param
// is filled. Returns opcode::end at END. Returns false-style error via
// `bad` out-param: true = malformed blob.
inline opcode next_op(reader& r, bool& bad,
		      ::std::string_view* literal = nullptr,
		      dec_field_fmt* fmt = nullptr,
		      dec_field_stdio* stdio_f = nullptr,
		      dec_field_pct* pct = nullptr) noexcept
{
	bad = false;
	u8 op{};
	if (!r.get_u8(op))
	{
		bad = true;
		return opcode::end;
	}
	switch (static_cast<opcode>(op))
	{
	case opcode::end:
		if (r.cur != r.end)
			bad = true;	// END must be the last byte
		return opcode::end;
	case opcode::literal:
	{
		u32 n{};
		if (!literal || !r.get_u32(n) || !r.get_bytes(*literal, n))
			bad = true;
		return opcode::literal;
	}
	case opcode::field_fmt:
	{
		if (!fmt)
		{
			bad = true;
			return opcode::field_fmt;
		}
		u8 sk{};
		if (!read_argref(r, fmt->arg) || !r.get_u8(sk))
		{
			bad = true;
			return opcode::field_fmt;
		}
		fmt->spec = static_cast<spec_kind>(sk);
		switch (fmt->spec)
		{
		case spec_kind::none:
			return opcode::field_fmt;
		case spec_kind::standard:
		{
			u8 fl{}, al{}, sg{};
			if (!r.get_u8(fl) || fl > 4 || !r.get_bytes(fmt->fill, fl) ||
			    !r.get_u8(al) || !r.get_u8(sg) || !r.get_u8(fmt->flags) ||
			    !read_dynparam(r, fmt->width) ||
			    !read_dynparam(r, fmt->precision) || !r.get_u8(fmt->type))
				bad = true;
			fmt->align = static_cast<fmt_align>(al);
			fmt->sign = static_cast<fmt_sign>(sg);
			return opcode::field_fmt;
		}
		case spec_kind::chrono:
		{
			u32 n{};
			if (!r.get_u32(n) || !r.get_bytes(fmt->chrono_blob, n))
				bad = true;
			return opcode::field_fmt;
		}
		default:
			bad = true;
			return opcode::field_fmt;
		}
	}
	case opcode::field_stdio:
	{
		if (!stdio_f || !read_argref(r, stdio_f->arg))
		{
			bad = true;
			return opcode::field_stdio;
		}
		u8 ln{};
		if (!r.get_u8(stdio_f->flags) || !r.get_u8(ln) ||
		    !r.get_u8(stdio_f->length_bits) || !r.get_u8(stdio_f->conv) ||
		    !read_dynparam(r, stdio_f->width) ||
		    !read_dynparam(r, stdio_f->precision))
			bad = true;
		stdio_f->length = static_cast<stdio_length>(ln);
		return opcode::field_stdio;
	}
	case opcode::field_pct:
	{
		if (!pct)
		{
			bad = true;
			return opcode::field_pct;
		}
		u8 mod{}, wk{};
		if (!r.get_u8(pct->conv) || !r.get_u8(pct->pad_flags) ||
		    !r.get_u8(pct->case_flags) || !r.get_u8(mod) ||
		    !r.get_u8(pct->colons) || !r.get_u8(wk))
		{
			bad = true;
			return opcode::field_pct;
		}
		pct->modifier = static_cast<pct_modifier>(mod);
		pct->has_width = wk == 1;
		if (wk != 0 && wk != 1)
		{
			bad = true;
			return opcode::field_pct;
		}
		if (pct->has_width && !r.get_u32(pct->width))
			bad = true;
		return opcode::field_pct;
	}
	default:
		bad = true;
		return opcode::end;
	}
}

} // namespace binfmt
} // namespace fast_io_i18n
