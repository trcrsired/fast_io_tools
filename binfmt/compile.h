#pragma once

// binfmt compilers — text format string -> binary program, run offline
// (localedef/codegen time). Anything on the format-string attack surface
// is a hard error here, not a blob encoding.
//
// STL-only on purpose: this runs in build tooling, and may later run in a
// host-side localedef utility.

#include "binfmt.h"

#include <stdexcept>

namespace fast_io_i18n
{
namespace binfmt
{

struct compile_error : ::std::runtime_error
{
	::std::size_t pos{};
	compile_error(::std::size_t p, char const* msg)
	    : ::std::runtime_error(msg), pos(p)
	{
	}
};

namespace details
{

inline constexpr bool is_digit(char c) noexcept
{
	return '0' <= c && c <= '9';
}
inline constexpr bool is_alpha(char c) noexcept
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

// UTF-8 encoded length of the code point starting at s[i]; 0 = invalid.
inline constexpr ::std::size_t utf8_cp_size(::std::string_view s,
					    ::std::size_t i) noexcept
{
	unsigned char c = static_cast<unsigned char>(s[i]);
	if (c < 0x80)
		return 1;
	::std::size_t n = (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xE ? 3
			      : (c >> 3) == 0x1E  ? 4
						  : 0;
	if (n == 0 || i + n > s.size())
		return 0;
	for (::std::size_t k{1}; k < n; ++k)
		if ((static_cast<unsigned char>(s[i + k]) >> 6) != 0x2)
			return 0;
	return n;
}

// digits -> u32 with cap; consumes [i..j). Throws on overflow/cap.
inline u32 parse_capped_u32(::std::string_view s, ::std::size_t& i,
			    ::std::size_t j, u32 cap, char const* what)
{
	u64 v{};
	for (; i < j && is_digit(s[i]); ++i)
	{
		v = v * 10u + static_cast<u32>(s[i] - '0');
		if (v > cap)
			throw compile_error(i, what);
	}
	return static_cast<u32>(v);
}

class litbuf
{
public:
	void push(char c)
	{
		lit_.push_back(c);
	}
	void push(::std::string_view v)
	{
		lit_.append(v);
	}
	void flush(builder& b)
	{
		if (!lit_.empty())
		{
			b.op_literal(lit_);
			lit_.clear();
		}
	}

private:
	::std::string lit_;
};

// Shared: parse a fmt replacement field starting at s[pos]=='{'; emits the
// op into b; returns position past the closing '}'. `depth` bounds chrono
// recursion.
::std::size_t parse_fmt_field(::std::string_view s, ::std::size_t pos,
			      builder& b, unsigned depth);

inline argref parse_fmt_argid(::std::string_view s, ::std::size_t& i,
			      ::std::size_t j)
{
	if (i == j || s[i] == ':' || s[i] == '!' || s[i] == '}')
		return {argref_kind::automatic, {}};
	if (is_digit(s[i]))
	{
		u32 v = parse_capped_u32(s, i, j, 0xFFFFFFFFu - 1u,
					 "arg index too large");
		return {argref_kind::index, v};
	}
	throw compile_error(i, "named args are not supported");
}

// Standard format_spec parse; fills out `f`; throws on failure (caller
// falls back to chrono for non-standard specs).
inline void parse_fmt_standard(::std::string_view spec, fmt_standard_spec& f)
{
	::std::size_t i{};
	::std::size_t const n{spec.size()};
	auto align_of = [](char c) -> fmt_align {
		switch (c)
		{
		case '<':
			return fmt_align::left;
		case '>':
			return fmt_align::right;
		case '^':
			return fmt_align::center;
		default:
			return fmt_align::none;
		}
	};
	// [fill]align
	if (i < n && align_of(spec[i]) != fmt_align::none)
	{
		f.align = align_of(spec[i]);
		++i;
	}
	else if (::std::size_t cp{utf8_cp_size(spec, i)}; cp != 0 && i + cp < n &&
		 align_of(spec[i + cp]) != fmt_align::none)
	{
		f.fill = spec.substr(i, cp);
		f.align = align_of(spec[i + cp]);
		i += cp + 1;
	}
	// sign
	if (i < n)
	{
		switch (spec[i])
		{
		case '+':
			f.sign = fmt_sign::plus;
			break;
		case '-':
			f.sign = fmt_sign::minus;
			break;
		case ' ':
			f.sign = fmt_sign::space;
			break;
		default:
			break;
		}
		if (f.sign != fmt_sign::none)
			++i;
	}
	// '#'
	if (i < n && spec[i] == '#')
	{
		f.flags |= fmt_flag_alternate;
		++i;
	}
	// '0'
	if (i < n && spec[i] == '0')
	{
		f.flags |= fmt_flag_zero;
		++i;
	}
	// width: digits only — '{' dynamic width is rejected
	if (i < n && spec[i] == '{')
		throw compile_error(i, "dynamic width not supported");
	if (i < n && is_digit(spec[i]))
	{
		::std::size_t st{i};
		u32 v = parse_capped_u32(spec, i, n, 0xFFFFFFFFu,
					 "width too large");
		(void)st;
		f.width = {dynparam_kind::value, v};
	}
	// precision
	if (i < n && spec[i] == '.')
	{
		++i;
		if (i < n && spec[i] == '{')
			throw compile_error(i, "dynamic precision not supported");
		if (i == n || !is_digit(spec[i]))
			throw compile_error(i, "bare '.' precision");
		u32 v = parse_capped_u32(spec, i, n, 0xFFFFFFFFu,
					 "precision too large");
		f.precision = {dynparam_kind::value, v};
	}
	// 'L'
	if (i < n && spec[i] == 'L')
	{
		f.flags |= fmt_flag_locale;
		++i;
	}
	// type
	if (i < n)
	{
		switch (spec[i])
		{
		case 'b':
		case 'B':
		case 'c':
		case 'd':
		case 'o':
		case 'x':
		case 'X':
		case 's':
		case '?':
		case 'a':
		case 'A':
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
		case 'p':
		case 'P':
			f.type = static_cast<u8>(spec[i]);
			++i;
			break;
		default:
			break;
		}
	}
	if (i != n)
		throw compile_error(i, "trailing junk in spec");
}

struct pct_options
{
	bool generic_letters{false};	// name_fmt/postal_fmt/tel_*: any alpha conv
	bool allow_nested_fmt{false}; // chrono specs: '{...}' fields allowed
};

::std::string compile_pct_impl(::std::string_view s, pct_options opts,
			       unsigned depth);

inline ::std::size_t parse_fmt_field(::std::string_view s, ::std::size_t pos,
				     builder& b, unsigned depth)
{
	if (depth > 8)
		throw compile_error(pos, "nested format depth exceeded");
	::std::size_t i{pos + 1};
	field_fmt_desc f{};
	f.arg = parse_fmt_argid(s, i, s.size());
	if (i < s.size() && s[i] == '!')
		throw compile_error(i, "!r/!a/!s conversions are not supported");
	if (i < s.size() && s[i] == ':')
	{
		++i;
		// spec runs to the '}' at brace-depth 0
		::std::size_t const spec_begin{i};
		unsigned lvl{};
		for (; i < s.size(); ++i)
		{
			if (s[i] == '{')
				++lvl;
			else if (s[i] == '}')
			{
				if (lvl == 0)
					break;
				--lvl;
			}
		}
		if (i == s.size())
			throw compile_error(pos, "unterminated '{'");
		::std::string_view spec{s.substr(spec_begin, i - spec_begin)};
		try
		{
			parse_fmt_standard(spec, f.standard);
			f.spec = spec_kind::standard;
		}
		catch (compile_error const&)
		{
			f.spec = spec_kind::chrono;
			f.chrono_blob = compile_pct_impl(
			    spec, pct_options{.generic_letters = false,
					      .allow_nested_fmt = true},
			    depth + 1);
		}
	}
	else if (i < s.size() && s[i] == '}')
	{
		f.spec = spec_kind::none;
	}
	else
	{
		throw compile_error(i, "expected ':' or '}' in field");
	}
	// i must be at '}'
	if (i >= s.size() || s[i] != '}')
		throw compile_error(i, "unterminated '{'");
	b.op_field_fmt(f);
	return i + 1;
}

// ---- strftime / generic %-directive compiler -------------------------------

// Strict-mode conv set: POSIX + glibc directives ('n','t','%' are folded to
// literals and never emitted).
inline constexpr bool pct_conv_valid(char c) noexcept
{
	switch (c)
	{
	case 'a':
	case 'A':
	case 'b':
	case 'B':
	case 'c':
	case 'C':
	case 'd':
	case 'D':
	case 'e':
	case 'F':
	case 'g':
	case 'G':
	case 'h':
	case 'H':
	case 'I':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'M':
	case 'p':
	case 'P':
	case 'r':
	case 'R':
	case 's':
	case 'S':
	case 'T':
	case 'u':
	case 'U':
	case 'V':
	case 'w':
	case 'W':
	case 'x':
	case 'X':
	case 'y':
	case 'Y':
	case 'z':
	case 'Z':
	case '+':
		return true;
	default:
		return false;
	}
}

inline ::std::string compile_pct_impl(::std::string_view s, pct_options opts,
				      unsigned depth)
{
	if (depth > 8)
		throw compile_error(0, "nested format depth exceeded");
	builder b{family::pct};
	litbuf lit;
	for (::std::size_t i{}; i < s.size();)
	{
		char c{s[i]};
		if (c == '{' && opts.allow_nested_fmt)
		{
			lit.flush(b);
			i = parse_fmt_field(s, i, b, depth);
			continue;
		}
		if (c != '%')
		{
			lit.push(c);
			++i;
			continue;
		}
		lit.flush(b);
		++i;
		if (i == s.size())
		{
			lit.push('%');
			break;
		}
		// literal folds
		switch (s[i])
		{
		case '%':
			lit.push('%');
			++i;
			continue;
		case 'n':
			lit.push('\n');
			++i;
			continue;
		case 't':
			lit.push('\t');
			++i;
			continue;
		default:
			break;
		}
		field_pct_desc d{};
		// pad + case flags
		for (; i < s.size(); ++i)
		{
			switch (s[i])
			{
			case '-':
				d.pad_flags |= pct_pad_minus;
				continue;
			case '_':
				d.pad_flags |= pct_pad_underscore;
				continue;
			case '0':
				d.pad_flags |= pct_pad_zero;
				continue;
			case '^':
				d.case_flags |= pct_case_upper;
				continue;
			case '#':
				d.case_flags |= pct_case_swap;
				continue;
			default:
				break;
			}
			break;
		}
		// width
		if (i < s.size() && is_digit(s[i]))
		{
			d.width = parse_capped_u32(s, i, s.size(),
						   0xFFFFFFFFu,
						   "width too large");
			d.has_width = true;
		}
		// %:z colons
		while (i < s.size() && s[i] == ':' && d.colons < 3)
		{
			++d.colons;
			++i;
		}
		// modifier
		if (i < s.size() && s[i] == 'E')
		{
			d.modifier = pct_modifier::era;
			++i;
		}
		else if (i < s.size() && s[i] == 'O')
		{
			d.modifier = pct_modifier::alternative;
			++i;
		}
		if (i == s.size())
			throw compile_error(i, "dangling '%'");
		char conv{s[i]};
		++i;
		if (d.colons != 0 && conv != 'z')
			throw compile_error(i - 1, "%: is only valid with z");
		if (opts.generic_letters ? !is_alpha(conv) : !pct_conv_valid(conv))
			throw compile_error(i - 1, "unsupported % directive");
		d.conv = static_cast<u8>(conv);
		b.op_field_pct(d);
	}
	lit.flush(b);
	return b.finish();
}

} // namespace details

// ---------------------------------------------------------------------------
// public compilers
// ---------------------------------------------------------------------------

// fmt.dev / std::format grammar.
inline ::std::string compile_fmt(::std::string_view s)
{
	builder b{family::fmt};
	details::litbuf lit;
	for (::std::size_t i{}; i < s.size();)
	{
		char c{s[i]};
		if (c == '{')
		{
			if (i + 1 < s.size() && s[i + 1] == '{')
			{
				lit.push('{');
				i += 2;
				continue;
			}
			lit.flush(b);
			i = details::parse_fmt_field(s, i, b, 0);
			continue;
		}
		if (c == '}')
		{
			if (i + 1 < s.size() && s[i + 1] == '}')
			{
				lit.push('}');
				i += 2;
				continue;
			}
			throw compile_error(i, "unmatched '}'");
		}
		lit.push(c);
		++i;
	}
	lit.flush(b);
	return b.finish();
}

// strftime / chrono %-directive grammar (LC_TIME d_t_fmt, d_fmt, t_fmt,
// t_fmt_ampm, date_fmt, era_*_fmt).
inline ::std::string compile_time(::std::string_view s)
{
	return details::compile_pct_impl(s, {}, 0);
}

// Generic %-letter slots (LC_NAME.name_fmt, LC_ADDRESS.postal_fmt,
// LC_TELEPHONE.tel_*_fmt): any alpha conv is a slot-defined substitution.
inline ::std::string compile_generic_pct(::std::string_view s)
{
	return details::compile_pct_impl(
	    s, details::pct_options{.generic_letters = true}, 0);
}

// printf grammar — safe subset.
inline ::std::string compile_stdio(::std::string_view s)
{
	builder b{family::stdio};
	details::litbuf lit;
	for (::std::size_t i{}; i < s.size();)
	{
		char c{s[i]};
		if (c != '%')
		{
			lit.push(c);
			++i;
			continue;
		}
		lit.flush(b);
		++i;
		if (i == s.size())
			throw compile_error(i, "dangling '%'");
		if (s[i] == '%')
		{
			lit.push('%');
			++i;
			continue;
		}
		field_stdio_desc d{};
		// [argnum$]
		if (details::is_digit(s[i]))
		{
			::std::size_t j{i};
			while (j < s.size() && details::is_digit(s[j]))
				++j;
			if (j < s.size() && s[j] == '$')
			{
				if (j == i)
					throw compile_error(i, "empty argnum$");
				u32 v = details::parse_capped_u32(
				    s, i, j, 0xFFFFFFFEu, "arg index too large");
				if (v == 0)
					throw compile_error(i, "argnum$ is 1-based");
				d.arg = {argref_kind::index, v - 1u};
				i = j + 1;
			}
		}
		// flags
		for (; i < s.size(); ++i)
		{
			switch (s[i])
			{
			case '-':
				d.flags |= stdio_flag_minus;
				continue;
			case '+':
				d.flags |= stdio_flag_plus;
				continue;
			case ' ':
				d.flags |= stdio_flag_space;
				continue;
			case '#':
				d.flags |= stdio_flag_alternate;
				continue;
			case '0':
				d.flags |= stdio_flag_zero;
				continue;
			case '\'':
				d.flags |= stdio_flag_group;
				continue;
			case 'I':
				d.flags |= stdio_flag_glibc_i;
				continue;
			default:
				break;
			}
			break;
		}
		// width
		if (i < s.size() && s[i] == '*')
			throw compile_error(i, "dynamic '*' width not supported");
		if (i < s.size() && details::is_digit(s[i]))
			d.width = {dynparam_kind::value,
				   details::parse_capped_u32(
				       s, i, s.size(), 0xFFFFFFFFu,
				       "width too large")};
		// precision
		if (i < s.size() && s[i] == '.')
		{
			++i;
			if (i < s.size() && s[i] == '*')
				throw compile_error(i,
						    "dynamic '*' precision not supported");
			if (i < s.size() && details::is_digit(s[i]))
				d.precision = {dynparam_kind::value,
					       details::parse_capped_u32(
						   s, i, s.size(),
						   0xFFFFFFFFu,
						   "precision too large")};
			else
				d.precision = {dynparam_kind::value, 0};
		}
		// length
		if (i < s.size())
		{
			switch (s[i])
			{
			case 'h':
				if (i + 1 < s.size() && s[i + 1] == 'h')
				{
					d.length = stdio_length::hh;
					i += 2;
				}
				else
				{
					d.length = stdio_length::h;
					++i;
				}
				break;
			case 'l':
				if (i + 1 < s.size() && s[i + 1] == 'l')
				{
					d.length = stdio_length::ll;
					i += 2;
				}
				else
				{
					d.length = stdio_length::l;
					++i;
				}
				break;
			case 'j':
				d.length = stdio_length::j;
				++i;
				break;
			case 'z':
				d.length = stdio_length::z;
				++i;
				break;
			case 't':
				d.length = stdio_length::t;
				++i;
				break;
			case 'L':
				d.length = stdio_length::L;
				++i;
				break;
			case 'q':
				d.length = stdio_length::q;
				++i;
				break;
			case 'w':
			{
				bool fast{false};
				::std::size_t k{i + 1};
				if (k < s.size() && s[k] == 'f')
				{
					fast = true;
					++k;
				}
				u32 bits = details::parse_capped_u32(
				    s, k, s.size(), 64, "bad w length bits");
				if (bits != 8 && bits != 16 && bits != 32 && bits != 64)
					throw compile_error(i, "bad w length bits");
				d.length = fast ? stdio_length::wf : stdio_length::w;
				d.length_bits = static_cast<u8>(bits);
				i = k;
				break;
			}
			default:
				break;
			}
		}
		if (i == s.size())
			throw compile_error(i, "dangling conversion");
		char conv{s[i]};
		++i;
		switch (conv)
		{
		case 'n': // write-back: never encodable
		case 'm': // glibc strerror: out of the safe subset
		case '[': // scanf-only
			throw compile_error(i - 1, "unsafe conversion rejected");
		default:
			break;
		}
		switch (conv)
		{
		case 'd':
		case 'i':
		case 'u':
		case 'o':
		case 'x':
		case 'X':
		case 'b':
		case 'B':
		case 'f':
		case 'F':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		case 'a':
		case 'A':
		case 'c':
		case 's':
		case 'p':
		case 'C':
		case 'S':
			break;
		default:
			throw compile_error(i - 1, "unknown conversion");
		}
		d.conv = static_cast<u8>(conv);
		b.op_field_stdio(d);
	}
	lit.flush(b);
	return b.finish();
}

} // namespace binfmt
} // namespace fast_io_i18n
