#pragma once

// binfmt runtime — the type-erased output device the binfmt interpreter
// (binfmt_print) writes through. See spec.md in this directory.
//
// This is deliberately NOT a C++ stream and NOT part of fast_io's
// output_stream concept. It is a plain two-pointer C ABI handle plus a
// vtable of plain function pointers — no C++ virtuals, no mangled ABI
// surface — so C (and other languages) can produce a device, and so a
// translation unit that only runs compiled programs needs none of the
// per-device/per-type template bloat. It is library-internal: user code
// goes through the binfmt_* entry points, never the handle directly.
//
// Layout (fixed, ABI):
//   basic_binfmt_device<char_type>  { vtable const* vptr; void* deviceptr; }
//   ^ vptr at the front, the erased object pointer at the end.
//
// The device can offer a real output buffer: get_output_buffer_ptrs
// returns {curr, end} (nullptr pair when the device is unbuffered) and
// set_output_curr_ptr publishes the advanced cursor. scatter_write_all
// is the cold path — flush/short-device writes. The interpreter writes
// straight into the buffer while it has room and only falls back to
// scatter_write_all.
//
// Herbceptions only: the two throwing vtable entries carry a `throws`
// exception specification, so the handle must be compiled with
// -fherbceptions. Legacy C++ exceptions are not supported.

#if !defined(__HERBCEPTIONS__)
#error "fast_io::binfmt requires herbceptions (-fherbceptions); legacy C++ exceptions are not supported"
#endif

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "binfmt.h"

namespace fast_io::binfmt
{

// ---------------------------------------------------------------------------
// output buffer pointer pair — what get_output_buffer_ptrs hands back
// ---------------------------------------------------------------------------

template <::std::integral char_type>
struct basic_binfmt_output_buffer_ptrs
{
	using char_type_t = char_type;
	char_type *curr_ptr;
	char_type *end_ptr;
};

// ---------------------------------------------------------------------------
// vtable — three plain function pointers. The first argument of every
// entry is the erased object pointer (void* deviceptr), exactly the value
// stored in basic_binfmt_device::deviceptr.
// ---------------------------------------------------------------------------

template <::std::integral char_type>
struct basic_binfmt_device_vtable
{
	using char_type_t = char_type;

	void (*scatter_write_all)(void *deviceptr,
							  ::fast_io::basic_io_scatter_t<char_type> const *pscatters,
							  ::std::size_t len) throws;

	// null when the stream is not a buffer_output_stream: the interpreter
	// then never touches the buffer and goes straight through
	// scatter_write_all. Non-null means "buffer_output_stream" and the
	// interpreter writes directly into the returned pointers.
	basic_binfmt_output_buffer_ptrs<char_type> (*get_output_buffer_ptrs)(
		void *deviceptr) noexcept;

	void (*set_output_curr_ptr)(void *deviceptr, char_type *curr_ptr) noexcept;
};

// ---------------------------------------------------------------------------
// the type-erased device — vptr at the front, erased object ptr at the end
// ---------------------------------------------------------------------------

template <::std::integral char_type>
struct basic_binfmt_device
{
	using char_type_t = char_type;
	basic_binfmt_device_vtable<char_type> const *vptr;
	void *deviceptr;
};

// char_type aliases — the entry points eventually come in these flavours
using binfmt_device = basic_binfmt_device<char>;
using wbinfmt_device = basic_binfmt_device<wchar_t>;
using u8binfmt_device = basic_binfmt_device<char8_t>;
using u16binfmt_device = basic_binfmt_device<char16_t>;
using u32binfmt_device = basic_binfmt_device<char32_t>;

// ---------------------------------------------------------------------------
// invokers — thin free functions, same shape as the ibuffer_*/write family
// ---------------------------------------------------------------------------

template <::std::integral char_type>
[[gnu::always_inline]] inline void scatter_write_all(basic_binfmt_device<char_type> dev,
							  ::fast_io::basic_io_scatter_t<char_type> const *pscatters,
							  ::std::size_t len) throws
{
	dev.vptr->scatter_write_all(dev.deviceptr, pscatters, len);
}

template <::std::integral char_type>
[[gnu::always_inline]] inline basic_binfmt_output_buffer_ptrs<char_type>
get_output_buffer_ptrs(basic_binfmt_device<char_type> dev) noexcept
{
	return dev.vptr->get_output_buffer_ptrs(dev.deviceptr);
}

template <::std::integral char_type>
[[gnu::always_inline]] inline void set_output_curr_ptr(basic_binfmt_device<char_type> dev,
								char_type *curr_ptr) noexcept
{
	dev.vptr->set_output_curr_ptr(dev.deviceptr, curr_ptr);
}

// ---------------------------------------------------------------------------
// erasing a real fast_io output stream into a device. One vtable object is
// instantiated per (stream type, char_type) in whatever translation unit
// asks for it — that is the bloat boundary this header exists to keep
// behind fast_io_unit.
// ---------------------------------------------------------------------------

namespace details
{

template <typename stream_type>
inline void binfmt_device_scatter_write_all_impl(
	void *deviceptr,
	::fast_io::basic_io_scatter_t<typename stream_type::output_char_type> const *pscatters,
	::std::size_t len) throws
{
	::fast_io::operations::scatter_write_all(
		*static_cast<stream_type *>(deviceptr), pscatters, len);
}

template <typename stream_type>
inline basic_binfmt_output_buffer_ptrs<typename stream_type::output_char_type>
binfmt_device_get_output_buffer_ptrs_impl(void *deviceptr) noexcept
{
	// only ever instantiated/pointed-to for buffer_output_stream streams
	// (the unbuffered vtable stores nullptr here instead).
	auto &s{*static_cast<stream_type *>(deviceptr)};
	auto sref{::fast_io::operations::output_stream_ref(s)};
	return {obuffer_curr(sref), obuffer_end(sref)};
}

template <typename stream_type>
inline void binfmt_device_set_output_curr_ptr_impl(
	void *deviceptr, typename stream_type::output_char_type *curr_ptr) noexcept
{
	if constexpr (::fast_io::buffer_output_stream<stream_type>)
	{
		auto &s{*static_cast<stream_type *>(deviceptr)};
		obuffer_set_curr(::fast_io::operations::output_stream_ref(s), curr_ptr);
	}
	else
	{
		(void)deviceptr;
		(void)curr_ptr;
	}
}

template <typename stream_type>
inline constexpr basic_binfmt_device_vtable<typename stream_type::output_char_type>
binfmt_device_vtable_impl() noexcept
{
	if constexpr (::fast_io::buffer_output_stream<stream_type>)
	{
		return {
			.scatter_write_all = binfmt_device_scatter_write_all_impl<stream_type>,
			.get_output_buffer_ptrs = binfmt_device_get_output_buffer_ptrs_impl<stream_type>,
			.set_output_curr_ptr = binfmt_device_set_output_curr_ptr_impl<stream_type>,
		};
	}
	else
	{
		// not a buffer_output_stream: no output buffer to bind to, so the
		// interpreter must always go through scatter_write_all.
		return {
			.scatter_write_all = binfmt_device_scatter_write_all_impl<stream_type>,
			.get_output_buffer_ptrs = nullptr,
			.set_output_curr_ptr = nullptr,
		};
	}
}

} // namespace details

template <typename stream_type>
	requires ::fast_io::output_stream<stream_type>
inline constexpr basic_binfmt_device_vtable<typename stream_type::output_char_type>
	binfmt_device_vtable{details::binfmt_device_vtable_impl<stream_type>()};

template <typename stream_type>
	requires ::fast_io::output_stream<stream_type>
inline constexpr basic_binfmt_device<typename stream_type::output_char_type>
binfmt_device_of(stream_type &s) noexcept
{
	return {__builtin_addressof(binfmt_device_vtable<stream_type>),
			static_cast<void *>(__builtin_addressof(s))};
}

// ===========================================================================
// runtime — binfmt_vprint
//
// The format string is a *compiled binary program*, not text. The caller
// has already packed every argument into one compact cookie:
//
//     cookie layout
//       +0                         : ::std::size_t addr_offsets_to_cookie[elements_count]
//       +addr_offsets_to_cookie[i] : the bytes of element i
//
// So element i lives at (unsigned char*)cookie + ((size_t*)cookie)[i].
// That packing keeps positional indexing trivial and keeps the argument
// bytes dense (better cache locality) while staying a plain C ABI call.
// There are no varargs anywhere: the cookie is the only way elements enter
// the interpreter, which is exactly what makes the program both safe (all
// accesses bounds-checked) and fast.
//
// The interpreter itself is locale-agnostic. pct / chrono / plural / locale
// grouping are a separate future API; the no-float core below throws
// std::errc::invalid_argument for them and for anything still unhandled.
// ===========================================================================

namespace details
{

// the wire codes themselves (kinds, ops, field params, conv_type, ctype,
// align/sign, pct) live in binfmt.h — one definition, shared with the
// compilers.

// widest unsigned the interpreter carries through integer conversions
#if defined(__SIZEOF_INT128__)
using binfmt_uintmax = __uint128_t;
#else
using binfmt_uintmax = ::std::uint_least64_t;
#endif

// ---------------------------------------------------------------------------
// malformed / unsupported program
// ---------------------------------------------------------------------------

[[noreturn]] inline void binfmt_malformed() throws
{
	throw throws ::std::errc::invalid_argument;
}

// a program cursor and its payload views — plain data, free functions only

struct binfmt_prog
{
	char8_t const *cur;
	char8_t const *end;
};

struct binfmt_bytes
{
	char8_t const *ptr;
	::std::size_t len;
};

struct binfmt_str_view
{
	void const *ptr;
	::std::size_t len;
};

// field attributes decoded from a `field` node's parameter list

template <::std::integral char_type>
struct binfmt_field
{
	char_type const *fill;
	::std::size_t fill_len;
	::std::size_t width;
	::std::size_t prec;
	::std::size_t arg;
	::std::uint_least32_t align;
	::std::uint_least32_t sign;
	::std::uint_least32_t ctype;
	::std::uint_least32_t type;
	bool auto_arg;
	bool flag_alt;
	bool flag_zero;
	bool flag_group;
	bool flag_locale;
	bool flag_outdigits;
	bool flag_upper;
};

// ---------------------------------------------------------------------------
// uleb128 / payload readers
// ---------------------------------------------------------------------------

[[gnu::always_inline]] inline ::std::uint_least64_t binfmt_get_uleb(binfmt_prog &p) throws
{
	::std::uint_least64_t r{};
	unsigned shift{};
	while (p.cur != p.end)
	{
		auto const b{static_cast<::std::uint_least8_t>(*p.cur++)};
		r |= static_cast<::std::uint_least64_t>(b & 0x7Fu) << shift;
		if (!(b & 0x80u))
		{
			return r;
		}
		shift += 7;
		if (shift > 63)
		{
			binfmt_malformed();
		}
	}
	binfmt_malformed();
}

[[gnu::always_inline]] inline binfmt_bytes binfmt_get_bytes_payload(binfmt_prog &p) throws
{
	auto const n{static_cast<::std::size_t>(binfmt_get_uleb(p))};
	if (static_cast<::std::size_t>(p.end - p.cur) < n)
	{
		binfmt_malformed();
	}
	binfmt_bytes r{p.cur, n};
	p.cur += n;
	return r;
}

inline void binfmt_skip_payload(binfmt_prog &p, ::std::uint_least32_t kind) throws
{
	switch (kind)
	{
	case binfmt_kind_none:
		return;
	case binfmt_kind_uleb:
	case binfmt_kind_sleb:
		(void)binfmt_get_uleb(p);
		return;
	case binfmt_kind_bytes:
		(void)binfmt_get_bytes_payload(p);
		return;
	case binfmt_kind_list:
	{
		auto const n{binfmt_get_uleb(p)};
		for (::std::uint_least64_t i{}; i < n; ++i)
		{
			auto const tag{binfmt_get_uleb(p)};
			auto const k{static_cast<::std::uint_least32_t>(tag & 7u)};
			if (k == binfmt_kind_list)
			{
				binfmt_malformed();
			}
			binfmt_skip_payload(p, k);
		}
		return;
	}
	default:
		binfmt_malformed();
	}
}

// ---------------------------------------------------------------------------
// cookie element access — the whole point of the packed cookie
// ---------------------------------------------------------------------------

[[gnu::always_inline]] inline void const *binfmt_cookie_element(void const *cookie, ::std::size_t index) noexcept
{
	auto const off{static_cast<::std::size_t const *>(cookie)[index]};
	return static_cast<unsigned char const *>(cookie) + off;
}

template <typename T>
[[gnu::always_inline]] inline T binfmt_load(void const *p) noexcept
{
	T v;
	::std::memcpy(__builtin_addressof(v), p, sizeof(T));
	return v;
}

// ---------------------------------------------------------------------------
// unpacking a field node's parameter list
// ---------------------------------------------------------------------------

template <::std::integral char_type>
[[gnu::always_inline]] inline void binfmt_parse_field(binfmt_prog &p, binfmt_field<char_type> &f) throws
{
	// the field node's tag has already been consumed by the caller; the
	// payload is a list, so the first thing here is the param count.
	auto const n{binfmt_get_uleb(p)};
	for (::std::uint_least64_t i{}; i < n; ++i)
	{
		auto const ptag{binfmt_get_uleb(p)};
		auto const code{static_cast<::std::uint_least32_t>(ptag >> 3)};
		auto const kind{static_cast<::std::uint_least32_t>(ptag & 7u)};
		switch (code)
		{
		case binfmt_fp_arg:
			f.arg = static_cast<::std::size_t>(binfmt_get_uleb(p));
			f.auto_arg = false;
			break;
		case binfmt_fp_width:
			f.width = static_cast<::std::size_t>(binfmt_get_uleb(p));
			break;
		case binfmt_fp_prec:
			f.prec = static_cast<::std::size_t>(binfmt_get_uleb(p));
			break;
		case binfmt_fp_align:
			f.align = static_cast<::std::uint_least32_t>(binfmt_get_uleb(p));
			break;
		case binfmt_fp_sign:
			f.sign = static_cast<::std::uint_least32_t>(binfmt_get_uleb(p));
			break;
		case binfmt_fp_ctype:
			f.ctype = static_cast<::std::uint_least32_t>(binfmt_get_uleb(p));
			break;
		case binfmt_fp_type:
			f.type = static_cast<::std::uint_least32_t>(binfmt_get_uleb(p));
			break;
		case binfmt_fp_flag_alt:
			f.flag_alt = true;
			break;
		case binfmt_fp_flag_zero:
			f.flag_zero = true;
			break;
		case binfmt_fp_flag_group:
			f.flag_group = true;
			break;
		case binfmt_fp_flag_locale:
			f.flag_locale = true;
			break;
		case binfmt_fp_flag_outdigits:
			f.flag_outdigits = true;
			break;
		case binfmt_fp_flag_upper:
			f.flag_upper = true;
			break;
		case binfmt_fp_fill:
		{
			auto const b{binfmt_get_bytes_payload(p)};
			f.fill = reinterpret_cast<char_type const *>(b.ptr);
			f.fill_len = b.len / sizeof(char_type);
			break;
		}
		default:
			// chrono / element / anything else: keep walking the list,
			// the conversion dispatch rejects what it cannot do.
			binfmt_skip_payload(p, kind);
			break;
		}
	}
}

// ---------------------------------------------------------------------------
// writes through the device. If the device is a buffer_output_stream (its
// vtable carries a non-null get_output_buffer_ptrs) we write straight into
// the output buffer and publish the cursor — one memcpy + one store per
// write, no scatter indirection. Otherwise (null slot) every write goes
// through scatter_write_all.
// ---------------------------------------------------------------------------

template <::std::integral char_type>
[[gnu::always_inline]] inline void binfmt_write_units(basic_binfmt_device<char_type> dev,
							   char_type const *p, ::std::size_t n) throws
{
	if (n == 0)
	{
		return;
	}
	if (dev.vptr->get_output_buffer_ptrs != nullptr)
	{
		auto bufs{dev.vptr->get_output_buffer_ptrs(dev.deviceptr)};
		auto const avail{static_cast<::std::size_t>(bufs.end_ptr - bufs.curr_ptr)};
		if (n < avail)
		{
			set_output_curr_ptr(dev,
								static_cast<char_type *>(::std::memcpy(bufs.curr_ptr, p, n * sizeof(char_type))) + n);
			return;
		}
	}
	::fast_io::basic_io_scatter_t<char_type> sc{p, n};
	scatter_write_all(dev, __builtin_addressof(sc), 1);
}

template <::std::integral char_type>
[[gnu::always_inline]] inline void binfmt_write_fill(basic_binfmt_device<char_type> dev,
							  char_type const *fill, ::std::size_t fill_len,
							  ::std::size_t count) throws
{
	if (count == 0)
	{
		return;
	}
	if (fill_len == 1)
	{
		char_type chunk[64];
		for (auto &c : chunk)
		{
			c = fill[0];
		}
		while (count != 0)
		{
			auto const n{count < 64u ? count : 64u};
			binfmt_write_units(dev, chunk, n);
			count -= n;
		}
		return;
	}
	while (count != 0)
	{
		auto const n{count < fill_len ? count : fill_len};
		binfmt_write_units(dev, fill, n);
		count -= n;
	}
}

// ---------------------------------------------------------------------------
// integer conversion + padding (base, sign, showbase, width, fill, align)
// ---------------------------------------------------------------------------

// reverse-order digit extraction. `base` is a compile-time constant for the
// four bases the wire format can select, so the division becomes a
// multiply-shift instead of a hardware (or, worse, 128-bit) divide.
template <::std::integral char_type, typename UInt, typename Base>
[[gnu::always_inline]] inline ::std::size_t binfmt_extract_digits(char_type *tmp, UInt mag, Base,
										   bool upper) noexcept
{
	constexpr unsigned base{Base::value};
	::std::size_t n{};
	do
	{
		auto const d{static_cast<unsigned>(mag % base)};
		tmp[n++] = static_cast<char_type>(d < 10u
											  ? static_cast<unsigned>(u8'0') + d
											  : static_cast<unsigned>(upper ? u8'A' : u8'a') + (d - 10u));
		mag /= base;
	} while (mag != 0);
	return n;
}

// base-10 decimal via fast_io's own jeaiii algorithm — the exact routine
// fast_io's integer printer uses, so decimal is as fast here as it is there.
template <::std::integral char_type, typename UInt>
[[gnu::always_inline]] inline char_type *binfmt_write_dec(char_type *p, UInt mag) noexcept
{
	if constexpr (sizeof(UInt) <= sizeof(::std::uint_least32_t))
	{
		return ::fast_io::details::jeaiii::jeaiii_main_u32(
			p, static_cast<::std::uint_least32_t>(mag));
	}
	else if constexpr (sizeof(UInt) <= sizeof(::std::uint_least64_t))
	{
		return ::fast_io::details::jeaiii::jeaiii_main_u64(
			p, static_cast<::std::uint_least64_t>(mag));
	}
	else
	{
#if defined(__SIZEOF_INT128__)
		if (mag <= static_cast<UInt>(~static_cast<::std::uint_least64_t>(0)))
		{
			return ::fast_io::details::jeaiii::jeaiii_main_u64(
				p, static_cast<::std::uint_least64_t>(mag));
		}
#endif
		// >64 bits: cold path, plain reverse loop
		char_type tmp[48];
		::std::size_t n{};
		do
		{
			auto const d{static_cast<unsigned>(mag % 10u)};
			tmp[n++] = static_cast<char_type>(static_cast<unsigned>(u8'0') + d);
			mag /= 10u;
		} while (mag != 0);
		while (n != 0)
		{
			*p++ = tmp[--n];
		}
		return p;
	}
}

template <::std::integral char_type, typename UInt>
[[gnu::always_inline]] inline ::std::size_t binfmt_format_uint(char_type *out, bool neg, UInt mag,
										::std::uint_least32_t base, bool upper,
										bool showbase, ::std::uint_least32_t sign,
										::std::size_t *prefix_len) noexcept
{
	char_type *p{out};
	if (neg)
	{
		*p++ = static_cast<char_type>(u8'-');
	}
	else if (sign == binfmt_sign_plus)
	{
		*p++ = static_cast<char_type>(u8'+');
	}
	else if (sign == binfmt_sign_space)
	{
		*p++ = static_cast<char_type>(u8' ');
	}
	if (showbase && mag != 0)
	{
		if (base == 16)
		{
			*p++ = static_cast<char_type>(u8'0');
			*p++ = static_cast<char_type>(upper ? u8'X' : u8'x');
		}
		else if (base == 2)
		{
			*p++ = static_cast<char_type>(u8'0');
			*p++ = static_cast<char_type>(upper ? u8'B' : u8'b');
		}
		else if (base == 8)
		{
			*p++ = static_cast<char_type>(u8'0');
		}
	}
	*prefix_len = static_cast<::std::size_t>(p - out);
	char_type tmp[160];
	::std::size_t n{};
	switch (base)
	{
	case 16:
		n = binfmt_extract_digits<char_type>(tmp, mag, ::std::integral_constant<unsigned, 16>{}, upper);
		break;
	case 8:
		n = binfmt_extract_digits<char_type>(tmp, mag, ::std::integral_constant<unsigned, 8>{}, false);
		break;
	case 2:
		n = binfmt_extract_digits<char_type>(tmp, mag, ::std::integral_constant<unsigned, 2>{}, false);
		break;
	default:
		p = binfmt_write_dec<char_type>(p, mag);
		return static_cast<::std::size_t>(p - out);
	}
	while (n != 0)
	{
		*p++ = tmp[--n];
	}
	return static_cast<::std::size_t>(p - out);
}

template <::std::integral char_type>
[[gnu::always_inline]] inline void binfmt_write_padded(basic_binfmt_device<char_type> dev,
								char_type const *content, ::std::size_t len,
								::std::size_t prefix_len,
								char_type const *fill, ::std::size_t fill_len,
								::std::size_t width, ::std::uint_least32_t align,
								bool flag_zero, bool numeric) throws
{
	auto const pad{width > len ? width - len : 0u};
	char_type space{static_cast<char_type>(u8' ')};
	if (fill_len == 0)
	{
		fill = __builtin_addressof(space);
		fill_len = 1;
	}
	if (pad == 0)
	{
		binfmt_write_units(dev, content, len);
		return;
	}
	if (flag_zero && numeric && align != binfmt_align_left)
	{
		if (prefix_len != 0)
		{
			binfmt_write_units(dev, content, prefix_len);
		}
		char_type zero{static_cast<char_type>(u8'0')};
		binfmt_write_fill(dev, __builtin_addressof(zero), 1, pad);
		if (len > prefix_len)
		{
			binfmt_write_units(dev, content + prefix_len, len - prefix_len);
		}
		return;
	}
	switch (align)
	{
	case binfmt_align_left:
		binfmt_write_units(dev, content, len);
		binfmt_write_fill(dev, fill, fill_len, pad);
		break;
	case binfmt_align_center:
	{
		auto const left{pad / 2};
		binfmt_write_fill(dev, fill, fill_len, left);
		binfmt_write_units(dev, content, len);
		binfmt_write_fill(dev, fill, fill_len, pad - left);
		break;
	}
	default:
		binfmt_write_fill(dev, fill, fill_len, pad);
		binfmt_write_units(dev, content, len);
		break;
	}
}

// ---------------------------------------------------------------------------
// field emit — one conversion, one element already located in the cookie
// ---------------------------------------------------------------------------

template <::std::integral char_type>
inline ::std::size_t binfmt_unit_strlen(char_type const *p) noexcept
{
	char_type const *q{p};
	while (*q != 0)
	{
		++q;
	}
	return static_cast<::std::size_t>(q - p);
}

template <::std::integral char_type, typename UInt>
[[gnu::always_inline]] inline void binfmt_emit_int_mag(basic_binfmt_device<char_type> dev,
								binfmt_field<char_type> const &f, bool neg, UInt mag) throws
{
	::std::uint_least32_t base{10};
	switch (f.type)
	{
	case binfmt_conv_o:
		base = 8;
		break;
	case binfmt_conv_x:
		base = 16;
		break;
	case binfmt_conv_b:
		base = 2;
		break;
	default:
		break;
	}
	char_type content[192];
	::std::size_t prefix_len{};
	auto const len{binfmt_format_uint<char_type>(content, neg, mag, base, f.flag_upper,
												f.flag_alt,
												f.type == binfmt_conv_d ? f.sign : 0u,
												__builtin_addressof(prefix_len))};
	binfmt_write_padded(dev, content, len, prefix_len, f.fill, f.fill_len,
						f.width, f.align, f.flag_zero, true);
}

template <::std::integral char_type>
[[gnu::always_inline]] inline void binfmt_emit_int(basic_binfmt_device<char_type> dev,
							binfmt_field<char_type> const &f, void const *elem) throws
{
	bool const want_signed{f.type == binfmt_conv_d};
	switch (f.ctype)
	{
	case binfmt_ct_i32:
	case binfmt_ct_u32:
	{
		auto const v{binfmt_load<::std::uint_least32_t>(elem)};
		if (want_signed && (v >> 31) != 0)
		{
			binfmt_emit_int_mag(dev, f, true, static_cast<::std::uint_least32_t>(0u - v));
		}
		else
		{
			binfmt_emit_int_mag(dev, f, false, v);
		}
		return;
	}
	case binfmt_ct_i64:
	case binfmt_ct_u64:
	{
		auto const v{binfmt_load<::std::uint_least64_t>(elem)};
		if (want_signed && (v >> 63) != 0)
		{
			binfmt_emit_int_mag(dev, f, true, static_cast<::std::uint_least64_t>(0ull - v));
		}
		else
		{
			binfmt_emit_int_mag(dev, f, false, v);
		}
		return;
	}
#if defined(__SIZEOF_INT128__)
	case binfmt_ct_i128:
	case binfmt_ct_u128:
	{
		auto const v{binfmt_load<__uint128_t>(elem)};
		if (want_signed && (v >> 127) != 0)
		{
			binfmt_emit_int_mag(dev, f, true, static_cast<__uint128_t>(0) - v);
		}
		else
		{
			binfmt_emit_int_mag(dev, f, false, v);
		}
		return;
	}
#endif
	default:
		binfmt_malformed();
	}
}

template <::std::integral char_type>
inline void binfmt_emit_char(basic_binfmt_device<char_type> dev,
							 binfmt_field<char_type> const &f, void const *elem) throws
{
	char_type ch{};
	switch (f.ctype)
	{
	case binfmt_ct_c8:
	case binfmt_ct_cebc:
		if (sizeof(char_type) != 1)
		{
			binfmt_malformed();
		}
		ch = static_cast<char_type>(binfmt_load<::std::uint_least8_t>(elem));
		break;
	case binfmt_ct_c16:
		if (sizeof(char_type) != 2)
		{
			binfmt_malformed();
		}
		ch = static_cast<char_type>(binfmt_load<::std::uint_least16_t>(elem));
		break;
	case binfmt_ct_c32:
		if (sizeof(char_type) != 4)
		{
			binfmt_malformed();
		}
		ch = static_cast<char_type>(binfmt_load<::std::uint_least32_t>(elem));
		break;
	default:
		binfmt_malformed();
	}
	binfmt_write_padded(dev, __builtin_addressof(ch), 1, 0, f.fill, f.fill_len,
						f.width, f.align, false, false);
}

template <::std::integral char_type>
inline void binfmt_emit_string(basic_binfmt_device<char_type> dev,
							   binfmt_field<char_type> const &f, void const *elem) throws
{
	char_type const *sp{};
	::std::size_t sl{};
	switch (f.ctype)
	{
	case binfmt_ct_c8ptr:
	case binfmt_ct_ebcptr:
	case binfmt_ct_gbptr:
		if (sizeof(char_type) != 1)
		{
			binfmt_malformed();
		}
		sp = static_cast<char_type const *>(binfmt_load<void const *>(elem));
		if (sp == nullptr)
		{
			binfmt_malformed();
		}
		sl = binfmt_unit_strlen(sp);
		break;
	case binfmt_ct_c16ptr:
		if (sizeof(char_type) != 2)
		{
			binfmt_malformed();
		}
		sp = static_cast<char_type const *>(binfmt_load<void const *>(elem));
		if (sp == nullptr)
		{
			binfmt_malformed();
		}
		sl = binfmt_unit_strlen(sp);
		break;
	case binfmt_ct_c32ptr:
		if (sizeof(char_type) != 4)
		{
			binfmt_malformed();
		}
		sp = static_cast<char_type const *>(binfmt_load<void const *>(elem));
		if (sp == nullptr)
		{
			binfmt_malformed();
		}
		sl = binfmt_unit_strlen(sp);
		break;
	case binfmt_ct_c8view:
	case binfmt_ct_ebcview:
	case binfmt_ct_gbview:
	case binfmt_ct_c16view:
	case binfmt_ct_c32view:
	{
		auto const v{binfmt_load<binfmt_str_view>(elem)};
		if (v.ptr == nullptr)
		{
			binfmt_malformed();
		}
		sp = static_cast<char_type const *>(v.ptr);
		sl = v.len;
		break;
	}
	default:
		binfmt_malformed();
	}
	if (f.prec != 0 && sl > f.prec)
	{
		sl = f.prec;
	}
	binfmt_write_padded(dev, sp, sl, 0, f.fill, f.fill_len, f.width, f.align,
						false, false);
}

template <::std::integral char_type>
inline void binfmt_emit_pointer(basic_binfmt_device<char_type> dev,
								binfmt_field<char_type> const &f, void const *elem) throws
{
	if (f.ctype != binfmt_ct_ptr && f.ctype != binfmt_ct_fptr)
	{
		binfmt_malformed();
	}
	auto const v{binfmt_load<void const *>(elem)};
	char_type content[192];
	::std::size_t prefix_len{};
	auto const len{binfmt_format_uint<char_type>(content, false,
												static_cast<::std::uintptr_t>(
													reinterpret_cast<::std::uintptr_t>(v)),
												16, false, true, 0,
												__builtin_addressof(prefix_len))};
	binfmt_write_padded(dev, content, len, prefix_len, f.fill, f.fill_len,
						f.width, f.align, f.flag_zero, true);
}

template <::std::integral char_type>
[[gnu::always_inline]] inline void binfmt_emit_field(basic_binfmt_device<char_type> dev,
							  binfmt_field<char_type> const &f, void const *elem) throws
{
	switch (f.type)
	{
	case binfmt_conv_d:
	case binfmt_conv_u:
	case binfmt_conv_o:
	case binfmt_conv_x:
	case binfmt_conv_b:
		binfmt_emit_int(dev, f, elem);
		return;
	case binfmt_conv_c:
		binfmt_emit_char(dev, f, elem);
		return;
	case binfmt_conv_s:
		binfmt_emit_string(dev, f, elem);
		return;
	case binfmt_conv_p:
	case binfmt_conv_addr:
		binfmt_emit_pointer(dev, f, elem);
		return;
	default:
		// debug / mth / floats / ranges — not in the no-float core
		binfmt_malformed();
	}
}

// ---------------------------------------------------------------------------
// the core loop
// ---------------------------------------------------------------------------

template <::std::integral char_type>
inline void binfmt_vprint_impl(basic_binfmt_device<char_type> dev,
							   void const *base, ::std::size_t len,
							   ::std::size_t elements_count,
							   void const *cookie) throws
{
	binfmt_prog p{static_cast<char8_t const *>(base),
				  static_cast<char8_t const *>(base) + len};
	::std::size_t next_arg{};
	while (p.cur != p.end)
	{
		auto const tag{binfmt_get_uleb(p)};
		auto const code{static_cast<::std::uint_least32_t>(tag >> 3)};
		auto const kind{static_cast<::std::uint_least32_t>(tag & 7u)};
		switch (code)
		{
		case binfmt_op_literal:
		{
			if (kind != binfmt_kind_bytes)
			{
				binfmt_malformed();
			}
			auto const b{binfmt_get_bytes_payload(p)};
			if (b.len % sizeof(char_type) != 0)
			{
				binfmt_malformed();
			}
			binfmt_write_units(dev,
							   reinterpret_cast<char_type const *>(b.ptr),
							   b.len / sizeof(char_type));
			break;
		}
		case binfmt_op_field:
		{
			if (kind != binfmt_kind_list)
			{
				binfmt_malformed();
			}
			binfmt_field<char_type> f{};
			f.auto_arg = true;
			binfmt_parse_field(p, f);
			auto const idx{f.auto_arg ? next_arg++ : f.arg};
			if (idx >= elements_count)
			{
				binfmt_malformed();
			}
			binfmt_emit_field(dev, f, binfmt_cookie_element(cookie, idx));
			break;
		}
		default:
			// pct / plural / unknown: consume structurally, then reject.
			// (locale / chrono / plural are a separate future API)
			binfmt_skip_payload(p, kind);
			binfmt_malformed();
		}
	}
}

} // namespace details

// ---------------------------------------------------------------------------
// entry points — char_type flavours. elements_count + cookie, no varargs.
// ---------------------------------------------------------------------------

inline void binfmt_vprint(binfmt_device dev, void const *base, ::std::size_t len,
						  ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl(dev, base, len, elements_count, cookie);
}

inline void wbinfmt_vprint(wbinfmt_device dev, void const *base, ::std::size_t len,
						   ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl(dev, base, len, elements_count, cookie);
}

inline void u8binfmt_vprint(u8binfmt_device dev, void const *base, ::std::size_t len,
							::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl(dev, base, len, elements_count, cookie);
}

inline void u16binfmt_vprint(u16binfmt_device dev, void const *base, ::std::size_t len,
							 ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl(dev, base, len, elements_count, cookie);
}

inline void u32binfmt_vprint(u32binfmt_device dev, void const *base, ::std::size_t len,
							 ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl(dev, base, len, elements_count, cookie);
}

} // namespace fast_io::binfmt
