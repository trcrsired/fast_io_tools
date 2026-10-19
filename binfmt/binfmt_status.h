#pragma once

// fast_io integration for binfmt — a status-print imbuer.
//
//   ::fast_io::io::print(::fast_io::mnp::binfmt(out), "user ", uid, " = ", n);
//
// The point is size. The ordinary fast_io print pipeline inlines a whole
// formatter at every call site; for a program that logs in a hundred
// places that is a hundred copies of the same code. Here a call site
// instead packs each argument into one binfmt cookie element and calls the
// shared interpreter (binfmt_vprint), which is one implementation in one
// translation unit (libbinfmt). Per call site only the packing and one
// call survive — the formatting itself is shared.
//
// There is NO format string and the *types are not embedded in the
// program*: a plain field node (`13 00`) plus a cookie whose typetag is the
// argument's C type is enough for everything the interpreter can render.
// The program only carries rendering choices a static manipulator asks
// for (base/conv, alignment, flags).
//
// Anything the interpreter has no native shape for yet — floating point
// today, user types later — becomes a binfmt "custom" element: the cookie
// holds a thunk + an opaque context and the interpreter hands the element
// back to that thunk, which writes through the device using the ordinary
// fast_io print path. So a static manipulator is always respected (the
// thunk is exactly the normal fast_io call), while the common integer and
// string cases stay on the shared interpreter. This mirrors fast_io's own
// herbception error reporting, which binds the output into a callback.
//
// C++26 only: this uses `template for` and pack indexing.

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <fast_io.h>
#include <fast_io_dsal/array.h>

#include "binfmt.h"
#include "binfmt_print.h"

namespace fast_io::binfmt
{

// ---------------------------------------------------------------------------
// shared interpreter entries — defined once in binfmt.cc / libbinfmt.so.
// The status-print path calls these instead of instantiating the interpreter
// here, so every translation unit shares one implementation.
// ---------------------------------------------------------------------------

void binfmt_status_vprint(binfmt_device dev, void const *base, ::std::size_t len,
						  ::std::size_t elements_count, void const *cookie) throws;
void wbinfmt_status_vprint(wbinfmt_device dev, void const *base, ::std::size_t len,
						   ::std::size_t elements_count, void const *cookie) throws;
void u8binfmt_status_vprint(u8binfmt_device dev, void const *base, ::std::size_t len,
							::std::size_t elements_count, void const *cookie) throws;
void u16binfmt_status_vprint(u16binfmt_device dev, void const *base, ::std::size_t len,
							 ::std::size_t elements_count, void const *cookie) throws;
void u32binfmt_status_vprint(u32binfmt_device dev, void const *base, ::std::size_t len,
							 ::std::size_t elements_count, void const *cookie) throws;

// ---------------------------------------------------------------------------
// the imbuer — bound to a real fast_io output stream (like lc_imbuer)
// ---------------------------------------------------------------------------

template <typename stm>
struct binfmt_imbuer
{
	using handle_type = stm;
	using output_char_type = typename handle_type::output_char_type;
	handle_type handle{};
};

namespace details
{

// ---------------------------------------------------------------------------
// descriptor — how one argument maps onto a wire field + a cookie element
// ---------------------------------------------------------------------------

enum class binfmt_arg_kind : ::std::uint_least8_t
{
	dynamic, // no native shape: pack a custom thunk
	scalar,  // integer stored in the cookie element
	view,    // basic_io_scatter_t stored in the cookie element
};

struct binfmt_arg_desc
{
	binfmt_arg_kind kind{binfmt_arg_kind::dynamic};
	::std::uint_least32_t tag{};  // cookie typetag (binfmt_ct_* + 1)
	::std::uint_least32_t conv{}; // program `type` param (0 = derive from ctype)
	::std::uint_least32_t align{};
	bool alt{};
	bool zero{};
	bool group{};
	bool upper{};
	bool plus{};
};

// integral ctype tag (binfmt_ct_* + 1), chosen by size and signedness
template <typename T>
consteval ::std::uint_least32_t binfmt_integral_tag() noexcept
{
	using U = ::std::remove_cv_t<T>;
	if constexpr (::std::signed_integral<U>)
	{
		if constexpr (sizeof(U) <= 4u)
		{
			return static_cast<::std::uint_least32_t>(binfmt_typetag::i32);
		}
		else if constexpr (sizeof(U) <= 8u)
		{
			return static_cast<::std::uint_least32_t>(binfmt_typetag::i64);
		}
		else
		{
			return static_cast<::std::uint_least32_t>(binfmt_typetag::i128);
		}
	}
	else
	{
		if constexpr (sizeof(U) <= 4u)
		{
			return static_cast<::std::uint_least32_t>(binfmt_typetag::u32);
		}
		else if constexpr (sizeof(U) <= 8u)
		{
			return static_cast<::std::uint_least32_t>(binfmt_typetag::u64);
		}
		else
		{
			return static_cast<::std::uint_least32_t>(binfmt_typetag::u128);
		}
	}
}

// scatter view tag, chosen by element width
template <typename C>
consteval ::std::uint_least32_t binfmt_view_tag() noexcept
{
	if constexpr (sizeof(C) == 1u)
	{
		return static_cast<::std::uint_least32_t>(binfmt_typetag::c8view);
	}
	else if constexpr (sizeof(C) == 2u)
	{
		return static_cast<::std::uint_least32_t>(binfmt_typetag::c16view);
	}
	else if constexpr (sizeof(C) == 4u)
	{
		return static_cast<::std::uint_least32_t>(binfmt_typetag::c32view);
	}
	else
	{
		return 0u;
	}
}

// the conversion the interpreter derives from a ctype tag with no `type`
// parameter on the wire
constexpr ::std::uint_least32_t binfmt_default_conv(::std::uint_least32_t tag) noexcept
{
	switch (tag)
	{
	case static_cast<::std::uint_least32_t>(binfmt_typetag::i32):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::i64):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::i128):
		return binfmt_conv_d;
	case static_cast<::std::uint_least32_t>(binfmt_typetag::u32):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::u64):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::u128):
		return binfmt_conv_u;
	case static_cast<::std::uint_least32_t>(binfmt_typetag::c8view):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::c16view):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::c32view):
		return binfmt_conv_s;
	case static_cast<::std::uint_least32_t>(binfmt_typetag::c8):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::c16):
	case static_cast<::std::uint_least32_t>(binfmt_typetag::c32):
		return binfmt_conv_c;
	default:
		return 0u;
	}
}

// a scalar_manip_t<flags, integral>: decode the manipulator's rendering
// choices. Anything the interpreter cannot do yet (floating format,
// boolalpha, percent, chrono, ...) stays dynamic and rides the custom path.
template <::fast_io::manipulators::scalar_flags flags, typename T>
consteval binfmt_arg_desc binfmt_scalar_desc() noexcept
{
	binfmt_arg_desc d{};
	if constexpr (!::std::integral<T> ||
				  flags.alphabet ||
				  flags.percentage != ::fast_io::manipulators::percentage_flag::none ||
				  flags.floating != ::fast_io::manipulators::floating_format::fixed ||
				  flags.time_flag != ::fast_io::manipulators::lc_time_flag::none ||
				  flags.line)
	{
		return d;
	}
	else
	{
		d.kind = binfmt_arg_kind::scalar;
		d.tag = binfmt_integral_tag<T>();
		// binfmt's x/o/b conversions are unsigned; fast_io renders a signed
		// value as a magnitude with a leading sign. Keep that one on the
		// ordinary path so the output matches exactly.
		if constexpr (::std::signed_integral<T>)
		{
			if (flags.base != 10u)
			{
				d.kind = binfmt_arg_kind::dynamic;
				return d;
			}
		}
		switch (flags.base)
		{
		case 10u:
			d.conv = ::std::signed_integral<T> ? binfmt_conv_d : binfmt_conv_u;
			break;
		case 16u:
			d.conv = binfmt_conv_x;
			break;
		case 8u:
			d.conv = binfmt_conv_o;
			break;
		case 2u:
			d.conv = binfmt_conv_b;
			break;
		default:
			d.kind = binfmt_arg_kind::dynamic;
			return d;
		}
		d.alt = flags.showbase;
		d.group = flags.comma;
		d.upper = flags.uppercase;
		d.plus = flags.showpos;
		switch (flags.placement)
		{
		case ::fast_io::manipulators::scalar_placement::left:
			d.align = binfmt_align_left;
			break;
		case ::fast_io::manipulators::scalar_placement::right:
			d.align = binfmt_align_right;
			break;
		case ::fast_io::manipulators::scalar_placement::middle:
			d.align = binfmt_align_center;
			break;
		default:
			break;
		}
		return d;
	}
}

// argument -> descriptor. The primary template is dynamic: every type with no
// native wire shape is handed to the custom thunk.
template <typename T>
struct binfmt_arg_traits
{
	inline static constexpr binfmt_arg_desc desc{};
};

template <typename C>
struct binfmt_arg_traits<::fast_io::basic_io_scatter_t<C>>
{
	consteval static binfmt_arg_desc make() noexcept
	{
		binfmt_arg_desc d{};
		constexpr auto tag{binfmt_view_tag<C>()};
		if constexpr (tag == 0u)
		{
			return d;
		}
		else
		{
			d.kind = binfmt_arg_kind::view;
			d.tag = tag;
			d.conv = binfmt_conv_s;
			return d;
		}
	}
	inline static constexpr binfmt_arg_desc desc{make()};
};

template <::fast_io::manipulators::scalar_flags flags, typename T>
struct binfmt_arg_traits<::fast_io::manipulators::scalar_manip_t<flags, T>>
{
	inline static constexpr binfmt_arg_desc desc{binfmt_scalar_desc<flags, T>()};
};

// a single-character view (a one-character literal like "\n") is a native
// character field, not an opaque custom element.
template <typename C>
struct binfmt_arg_traits<::fast_io::manipulators::chvw_t<C>>
{
	consteval static binfmt_arg_desc make() noexcept
	{
		binfmt_arg_desc d{};
		if constexpr (!::std::integral<C>)
		{
			return d;
		}
		else
		{
			d.kind = binfmt_arg_kind::scalar;
			if constexpr (sizeof(C) == 1u)
			{
				d.tag = static_cast<::std::uint_least32_t>(binfmt_typetag::c8);
			}
			else if constexpr (sizeof(C) == 2u)
			{
				d.tag = static_cast<::std::uint_least32_t>(binfmt_typetag::c16);
			}
			else if constexpr (sizeof(C) == 4u)
			{
				d.tag = static_cast<::std::uint_least32_t>(binfmt_typetag::c32);
			}
			else
			{
				d.kind = binfmt_arg_kind::dynamic;
				return d;
			}
			d.conv = binfmt_conv_c;
			return d;
		}
	}
	inline static constexpr binfmt_arg_desc desc{make()};
};

// ---------------------------------------------------------------------------
// program generation — one plain field node per argument, types in cookies
// ---------------------------------------------------------------------------

constexpr ::std::size_t binfmt_uleb_len(::std::uint_least64_t v) noexcept
{
	::std::size_t n{1u};
	while (v >= 0x80u)
	{
		v >>= 7u;
		++n;
	}
	return n;
}

constexpr void binfmt_put_uleb(char8_t *b, ::std::size_t &n,
							   ::std::uint_least64_t v) noexcept
{
	for (;;)
	{
		auto const x{static_cast<::std::uint_least8_t>(v & 0x7Fu)};
		v >>= 7u;
		if (v != 0u)
		{
			b[n++] = static_cast<char8_t>(x | 0x80u);
		}
		else
		{
			b[n++] = static_cast<char8_t>(x);
			return;
		}
	}
}

constexpr bool binfmt_is_plain(binfmt_arg_desc const &d) noexcept
{
	return d.conv == binfmt_default_conv(d.tag) && d.align == 0u && !d.alt && !d.zero &&
		   !d.group && !d.upper && !d.plus;
}

constexpr ::std::size_t binfmt_field_node_size(binfmt_arg_desc const &d) noexcept
{
	if (d.kind == binfmt_arg_kind::dynamic || binfmt_is_plain(d))
	{
		return 2u; // `13 00`
	}
	::std::size_t s{2u}; // tag + param count (both < 0x80)
	if (d.conv != 0u)
	{
		s += 1u + binfmt_uleb_len(d.conv);
	}
	if (d.align != 0u)
	{
		s += 1u + binfmt_uleb_len(d.align);
	}
	if (d.plus)
	{
		s += 1u + binfmt_uleb_len(binfmt_sign_plus);
	}
	if (d.alt)
	{
		++s;
	}
	if (d.zero)
	{
		++s;
	}
	if (d.group)
	{
		++s;
	}
	if (d.upper)
	{
		++s;
	}
	return s;
}

constexpr void binfmt_emit_field_node(char8_t *b, ::std::size_t &n,
									  binfmt_arg_desc const &d) noexcept
{
	if (d.kind == binfmt_arg_kind::dynamic || binfmt_is_plain(d))
	{
		b[n++] = static_cast<char8_t>((binfmt_op_field << 3) | binfmt_kind_list);
		b[n++] = 0x00u;
		return;
	}
	::std::uint_least32_t cnt{};
	if (d.conv != 0u)
	{
		++cnt;
	}
	if (d.align != 0u)
	{
		++cnt;
	}
	if (d.plus)
	{
		++cnt;
	}
	if (d.alt)
	{
		++cnt;
	}
	if (d.zero)
	{
		++cnt;
	}
	if (d.group)
	{
		++cnt;
	}
	if (d.upper)
	{
		++cnt;
	}
	b[n++] = static_cast<char8_t>((binfmt_op_field << 3) | binfmt_kind_list);
	binfmt_put_uleb(b, n, cnt);
	if (d.conv != 0u)
	{
		b[n++] = static_cast<char8_t>((binfmt_fp_type << 3) | binfmt_kind_uleb);
		binfmt_put_uleb(b, n, d.conv);
	}
	if (d.align != 0u)
	{
		b[n++] = static_cast<char8_t>((binfmt_fp_align << 3) | binfmt_kind_uleb);
		binfmt_put_uleb(b, n, d.align);
	}
	if (d.plus)
	{
		b[n++] = static_cast<char8_t>((binfmt_fp_sign << 3) | binfmt_kind_uleb);
		binfmt_put_uleb(b, n, binfmt_sign_plus);
	}
	if (d.alt)
	{
		b[n++] = static_cast<char8_t>((binfmt_fp_flag_alt << 3) | binfmt_kind_none);
	}
	if (d.zero)
	{
		b[n++] = static_cast<char8_t>((binfmt_fp_flag_zero << 3) | binfmt_kind_none);
	}
	if (d.group)
	{
		b[n++] = static_cast<char8_t>((binfmt_fp_flag_group << 3) | binfmt_kind_none);
	}
	if (d.upper)
	{
		b[n++] = static_cast<char8_t>((binfmt_fp_flag_upper << 3) | binfmt_kind_none);
	}
}

template <typename... Args>
consteval ::std::size_t binfmt_prog_size() noexcept
{
	::std::size_t n{};
	template for (constexpr auto i : ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		n += binfmt_field_node_size(binfmt_arg_traits<::std::remove_cvref_t<Args...[i]>>::desc);
	}
	return n;
}

template <typename... Args>
consteval auto binfmt_make_prog() noexcept
{
	::fast_io::array<char8_t, binfmt_prog_size<Args...>()> a{};
	::std::size_t n{};
	template for (constexpr auto i : ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		binfmt_emit_field_node(a.data(), n,
							   binfmt_arg_traits<::std::remove_cvref_t<Args...[i]>>::desc);
	}
	return a;
}

// ---------------------------------------------------------------------------
// the custom path: an adapter that turns a binfmt device into a fast_io
// output stream, so a thunk can run the ordinary print pipeline into it
// ---------------------------------------------------------------------------

template <::std::integral char_type>
struct basic_binfmt_ostream
{
	using output_char_type = char_type;
	basic_binfmt_device<char_type> dev;
};

template <::std::integral char_type>
inline constexpr basic_binfmt_ostream<char_type>
output_stream_ref_define(basic_binfmt_ostream<char_type> os) noexcept
{
	return os;
}

template <::std::integral char_type>
inline void write_all_overflow_define(basic_binfmt_ostream<char_type> os,
									  char_type const *p, ::std::size_t n) throws
{
	binfmt_write_units(os.dev, p, n);
}

template <::std::integral char_type>
inline ::std::size_t write_some_overflow_define(basic_binfmt_ostream<char_type> os,
												char_type const *p, ::std::size_t n) throws
{
	binfmt_write_units(os.dev, p, n);
	return n;
}

// the thunk an unsupported argument packs: run the ordinary fast_io print
// path for that one argument, into the device the interpreter handed us.
// This is where a floating-point argument (or any user type) is honoured by
// simply calling the normal APIs.
template <typename Arg, ::std::integral char_type>
inline void binfmt_custom_thunk(void *ctx, basic_binfmt_device<char_type> dev) throws
{
	basic_binfmt_ostream<char_type> os{dev};
	::fast_io::operations::decay::print_freestanding_decay<false>(
		os, *static_cast<Arg const *>(ctx));
}

// ---------------------------------------------------------------------------
// packing a cookie element
// ---------------------------------------------------------------------------

template <::std::integral char_type, typename Arg>
inline void binfmt_pack_one(binfmt_cookie_element &ce, Arg const &a) noexcept
{
	using U = ::std::remove_cvref_t<Arg>;
	constexpr binfmt_arg_desc d{binfmt_arg_traits<U>::desc};
	if constexpr (d.kind == binfmt_arg_kind::dynamic)
	{
		ce.typetag = ::fast_io::binfmt::binfmt_typetag::custom;
		ce.field1 = reinterpret_cast<::std::size_t>(&binfmt_custom_thunk<U, char_type>);
		ce.field2 = reinterpret_cast<::std::size_t>(
			static_cast<void const *>(__builtin_addressof(a)));
	}
	else if constexpr (d.kind == binfmt_arg_kind::view)
	{
		ce.typetag = static_cast<::fast_io::binfmt::binfmt_typetag>(d.tag);
		ce.field1 = reinterpret_cast<::std::size_t>(a.base);
		ce.field2 = a.len;
	}
	else
	{
		ce.typetag = static_cast<::fast_io::binfmt::binfmt_typetag>(d.tag);
		auto const v{a.reference};
		if constexpr (sizeof(v) > 8u)
		{
#if defined(__SIZEOF_INT128__)
			auto const uv{static_cast<unsigned __int128>(v)};
			ce.field1 = static_cast<::std::size_t>(uv);
			ce.field2 = static_cast<::std::size_t>(uv >> 64u);
#endif
		}
		else
		{
			ce.field1 = static_cast<::std::size_t>(v);
			ce.field2 = 0u;
		}
	}
}

template <::std::integral char_type>
inline void binfmt_write_newline(basic_binfmt_device<char_type> dev) throws
{
	char_type nl{static_cast<char_type>(u8'\n')};
	::fast_io::basic_io_scatter_t<char_type> sc{__builtin_addressof(nl), 1u};
	scatter_write_all(dev, __builtin_addressof(sc), 1u);
}

// ---------------------------------------------------------------------------
// the whole call: one program, one dense cookie, one interpreter call
// ---------------------------------------------------------------------------

template <bool line, typename output, typename... Args>
inline void binfmt_print_all(binfmt_imbuer<output> imb, Args... args) throws
{
	using char_type = typename output::output_char_type;
	auto const dev{binfmt_device_of(imb.handle)};
	constexpr ::std::size_t k{sizeof...(Args)};
	if constexpr (k != 0u)
	{
		constexpr auto prog{binfmt_make_prog<Args...>()};
		binfmt_cookie_element cookie[k]{};
		template for (constexpr auto i : ::fast_io::details::index_array_range<0, k>)
		{
			binfmt_pack_one<char_type>(cookie[i], args...[i]);
		}
		if constexpr (::std::same_as<char_type, char>)
		{
			binfmt_status_vprint(dev, prog.data(), prog.size(), k, cookie);
		}
		else if constexpr (::std::same_as<char_type, wchar_t>)
		{
			wbinfmt_status_vprint(dev, prog.data(), prog.size(), k, cookie);
		}
		else if constexpr (::std::same_as<char_type, char8_t>)
		{
			u8binfmt_status_vprint(dev, prog.data(), prog.size(), k, cookie);
		}
		else if constexpr (::std::same_as<char_type, char16_t>)
		{
			u16binfmt_status_vprint(dev, prog.data(), prog.size(), k, cookie);
		}
		else if constexpr (::std::same_as<char_type, char32_t>)
		{
			u32binfmt_status_vprint(dev, prog.data(), prog.size(), k, cookie);
		}
		else
		{
			binfmt_vprint_impl<binfmt_policy_default>(dev, prog.data(), prog.size(), k, cookie);
		}
	}
	if constexpr (line)
	{
		binfmt_write_newline(dev);
	}
}

} // namespace details

template <typename stm>
inline constexpr binfmt_imbuer<stm> output_stream_ref_define(binfmt_imbuer<stm> t) noexcept
{
	return t;
}

template <typename stm>
	requires(::std::is_lvalue_reference_v<stm> || ::std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(stm &&out) throws
{
	using reftype = decltype(::fast_io::operations::output_stream_ref(out));
	return binfmt_imbuer<reftype>{::fast_io::operations::output_stream_ref(out)};
}

template <bool line, typename output, typename... Args>
inline constexpr void status_print_define(binfmt_imbuer<output> imb, Args... args) throws
{
	details::binfmt_print_all<line>(imb, args...);
}

} // namespace fast_io::binfmt

namespace fast_io::manipulators
{

template <typename stm>
	requires(::std::is_lvalue_reference_v<stm> || ::std::is_trivially_copyable_v<stm>)
inline constexpr auto binfmt(stm &&out) throws
{
	return ::fast_io::binfmt::imbue(static_cast<stm &&>(out));
}

} // namespace fast_io::manipulators
