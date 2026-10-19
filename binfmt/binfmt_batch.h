#pragma once

// ===========================================================================
// binfmt_batch.h — single-device-interaction interpreter front end.
//
// The stock interpreter (binfmt_print.h) talks to the device once per emitted
// piece: get_output_buffer_ptrs + set_output_curr_ptr for a
// buffer_output_stream, or scatter_write_all otherwise. The mixed status line
// "hello {} world {} hello\n" is six pieces, so it is six device round trips
// per call. That per-node vtable traffic is the largest cost in the
// interpreter and the reason it loses to fmt on an equal sink.
//
// This front end keeps the program format, the cookie, the readers, the field
// parser and the number formatting of binfmt_print.h exactly as they are, and
// re-implements only the *write* half around one hoisted cursor:
//
//   * a buffer_output_stream: fetch its buffer ONCE, memcpy every piece
//     straight into it through a local cursor, and publish ONCE with
//     set_output_curr_ptr. This is the same shape as fast_io's own
//     print_freestanding — no intermediate copy, one device interaction.
//   * an unbuffered stream: gather pieces into a local contiguous buffer and
//     publish it ONCE with scatter_write_all.
//   * if the cursor runs out (or a piece is bigger than the stream buffer),
//     publish what is pending, then either refetch the stream buffer or hand
//     the oversized piece to scatter_write_all, and continue.
//   * a custom thunk owns its own device writes and forces a publish first.
//
// An ordinary program therefore reaches the real device exactly twice (one
// fetch, one publish) on a buffered stream, regardless of piece count. The old
// interpreter is untouched; both live side by side.
// ===========================================================================

#include "binfmt_print.h"

namespace fast_io::binfmt
{

namespace details
{

// units of the fallback buffer used when the stream has no output buffer.
inline constexpr ::std::size_t binfmt_batch_local_units{2048};

template <::std::integral char_type>
struct binfmt_batch_writer
{
	basic_binfmt_device<char_type> dev;
	char_type *curr;
	char_type *end;
	char_type *local; // fallback buffer base (nullptr when the stream has one)
	bool buffered;
};

template <::std::integral char_type>
inline ::std::size_t binfmt_batch_avail(binfmt_batch_writer<char_type> const &w) noexcept
{
	return static_cast<::std::size_t>(w.end - w.curr);
}

// fetch the stream's buffer (buffered) or reset to the local base (unbuffered).
template <::std::integral char_type>
inline void binfmt_batch_refresh(binfmt_batch_writer<char_type> &w) noexcept
{
	if (w.buffered)
	{
		auto const b{::fast_io::binfmt::get_output_buffer_ptrs(w.dev)};
		w.curr = b.curr_ptr;
		w.end = b.end_ptr;
	}
	else
	{
		w.curr = w.local;
	}
}

// publish everything pending and refetch. Order is preserved: buffered output
// is committed with set_output_curr_ptr, unbuffered with one scatter.
template <::std::integral char_type>
inline void binfmt_batch_publish(binfmt_batch_writer<char_type> &w) throws
{
	if (w.buffered)
	{
		::fast_io::binfmt::set_output_curr_ptr(w.dev, w.curr);
		binfmt_batch_refresh(w);
		return;
	}
	if (w.curr != w.local)
	{
		auto const used{static_cast<::std::size_t>(w.curr - w.local)};
		w.curr = w.local;
		::fast_io::basic_io_scatter_t<char_type> sc{w.local, used};
		::fast_io::binfmt::scatter_write_all(w.dev, __builtin_addressof(sc), 1);
	}
}

// return `n` contiguous units the caller may write into and then commit by
// advancing curr. Nullptr means the piece is larger than the whole stream
// buffer / local buffer and must go through scatter_write_all directly.
template <::std::integral char_type>
inline char_type *binfmt_batch_make_room(binfmt_batch_writer<char_type> &w,
										 ::std::size_t n) throws
{
	if (binfmt_batch_avail(w) >= n)
	{
		return w.curr;
	}
	binfmt_batch_publish(w);
	if (binfmt_batch_avail(w) >= n)
	{
		return w.curr;
	}
	return nullptr;
}

// copy `n` units into the stream, using the hoisted cursor when they fit and a
// single scatter when they do not. `p` must be valid for the duration of this
// call only (it is consumed immediately).
template <::std::integral char_type>
inline void binfmt_batch_put(binfmt_batch_writer<char_type> &w,
							 char_type const *p, ::std::size_t n) throws
{
	if (n == 0)
	{
		return;
	}
	if (binfmt_batch_avail(w) >= n)
	{
		w.curr = static_cast<char_type *>(::std::memcpy(w.curr, p, n * sizeof(char_type))) + n;
		return;
	}
	binfmt_batch_publish(w);
	if (binfmt_batch_avail(w) >= n)
	{
		w.curr = static_cast<char_type *>(::std::memcpy(w.curr, p, n * sizeof(char_type))) + n;
		return;
	}
	::fast_io::basic_io_scatter_t<char_type> sc{p, n};
	::fast_io::binfmt::scatter_write_all(w.dev, __builtin_addressof(sc), 1);
	binfmt_batch_refresh(w);
}

template <::std::integral char_type>
inline char_type *binfmt_batch_put_fill(char_type *d, char_type const *fill,
										::std::size_t fill_len,
										::std::size_t count) noexcept
{
	if (fill_len == 1)
	{
		for (::std::size_t i{}; i != count; ++i)
		{
			d[i] = fill[0];
		}
		return d + count;
	}
	while (count != 0)
	{
		if (count < fill_len)
		{
			::std::memcpy(d, fill, count * sizeof(char_type));
			return d + count;
		}
		::std::memcpy(d, fill, fill_len * sizeof(char_type));
		d += fill_len;
		count -= fill_len;
	}
	return d;
}

// write a complete padded field as one contiguous run through the cursor.
// Falls back to the stock per-piece writer only for a field wider than the
// stream buffer.
template <::std::integral char_type>
inline void binfmt_batch_padded(binfmt_batch_writer<char_type> &w,
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
	auto const total{len + pad};
	char_type *blk{binfmt_batch_make_room(w, total)};
	if (blk == nullptr)
	{
		binfmt_write_padded(w.dev, content, len, prefix_len, fill, fill_len,
							width, align, flag_zero, numeric);
		binfmt_batch_refresh(w);
		return;
	}
	char_type *d{blk};
	if (pad == 0)
	{
		::std::memcpy(d, content, len * sizeof(char_type));
	}
	else if (flag_zero && numeric && align != binfmt_align_left)
	{
		if (prefix_len != 0)
		{
			::std::memcpy(d, content, prefix_len * sizeof(char_type));
			d += prefix_len;
		}
		char_type zero{static_cast<char_type>(u8'0')};
		d = binfmt_batch_put_fill(d, __builtin_addressof(zero), 1, pad);
		if (len > prefix_len)
		{
			::std::memcpy(d, content + prefix_len, (len - prefix_len) * sizeof(char_type));
		}
	}
	else
	{
		switch (align)
		{
		case binfmt_align_left:
			::std::memcpy(d, content, len * sizeof(char_type));
			d += len;
			d = binfmt_batch_put_fill(d, fill, fill_len, pad);
			break;
		case binfmt_align_center:
		{
			auto const left{pad / 2};
			d = binfmt_batch_put_fill(d, fill, fill_len, left);
			::std::memcpy(d, content, len * sizeof(char_type));
			d += len;
			d = binfmt_batch_put_fill(d, fill, fill_len, pad - left);
			break;
		}
		default:
			d = binfmt_batch_put_fill(d, fill, fill_len, pad);
			::std::memcpy(d, content, len * sizeof(char_type));
			d += len;
			break;
		}
	}
	w.curr = blk + total;
}

template <::std::integral char_type, typename UInt>
inline void binfmt_batch_emit_int_mag(binfmt_batch_writer<char_type> &w,
									  binfmt_field const &f,
									  ::std::uint_least32_t ty, bool neg, UInt mag) throws
{
	::std::uint_least32_t base{10};
	switch (ty)
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
												ty == binfmt_conv_d ? f.sign : 0u,
												__builtin_addressof(prefix_len))};
	binfmt_batch_padded(w, content, len, prefix_len, binfmt_field_fill<char_type>(f),
						binfmt_field_fill_len<char_type>(f), f.width, f.align,
						f.flag_zero, true);
}

template <::std::integral char_type>
inline void binfmt_batch_emit_int(binfmt_batch_writer<char_type> &w,
								  binfmt_field const &f,
								  ::std::uint_least32_t ty,
								  binfmt_cookie_element const &ce) throws
{
	auto const want_signed{ty == binfmt_conv_d};
	switch (f.ctype)
	{
	case binfmt_ct_i32:
	case binfmt_ct_u32:
	{
		auto const v{static_cast<::std::uint_least32_t>(ce.field1)};
		auto const neg{want_signed && (v >> 31) != 0};
		binfmt_batch_emit_int_mag(w, f, ty, neg,
								  neg ? static_cast<::std::uint_least32_t>(0u - v) : v);
		return;
	}
	case binfmt_ct_i64:
	case binfmt_ct_u64:
	{
		auto const v{static_cast<::std::uint_least64_t>(ce.field1)};
		auto const neg{want_signed && (v >> 63) != 0};
		binfmt_batch_emit_int_mag(w, f, ty, neg,
								  neg ? static_cast<::std::uint_least64_t>(0ull - v) : v);
		return;
	}
#if defined(__SIZEOF_INT128__)
	case binfmt_ct_i128:
	case binfmt_ct_u128:
	{
		auto const v{(static_cast<__uint128_t>(ce.field2) << 64) |
					 static_cast<__uint128_t>(ce.field1)};
		auto const neg{want_signed && (v >> 127) != 0};
		binfmt_batch_emit_int_mag(w, f, ty, neg, neg ? static_cast<__uint128_t>(0) - v : v);
		return;
	}
#endif
	default:
		binfmt_malformed();
	}
}

// decimal magnitude with nothing to pad: format straight into the cursor.
template <::std::integral char_type>
inline void binfmt_batch_emit_dec_plain(binfmt_batch_writer<char_type> &w, bool neg,
										binfmt_mag_t mag) throws
{
	char_type *p{binfmt_batch_make_room(w, 48)};
	if (p == nullptr)
	{
		char_type buf[48];
		char_type *q{buf};
		if (neg)
		{
			*q++ = static_cast<char_type>(u8'-');
		}
		q = binfmt_write_dec<char_type>(q, mag);
		binfmt_batch_put(w, buf, static_cast<::std::size_t>(q - buf));
		return;
	}
	char_type *q{p};
	if (neg)
	{
		*q++ = static_cast<char_type>(u8'-');
	}
	q = binfmt_write_dec<char_type>(q, mag);
	w.curr = q;
}

// a custom thunk writes through the device itself, so pending output must
// reach the device first to keep program order.
template <::std::integral char_type>
inline void binfmt_batch_emit_custom(binfmt_batch_writer<char_type> &w,
									 binfmt_cookie_element const &ce) throws
{
	auto const fn{reinterpret_cast<binfmt_custom_write_fn<char_type>>(ce.field1)};
	if (fn == nullptr)
	{
		binfmt_malformed();
	}
	binfmt_batch_publish(w);
	fn(reinterpret_cast<void *>(ce.field2), w.dev);
	binfmt_batch_refresh(w);
}

template <binfmt_policy policy, ::std::integral char_type>
inline void binfmt_batch_emit_field(binfmt_batch_writer<char_type> &w,
									binfmt_field const &f,
									binfmt_cookie_element const &ce) throws;

// emit one `{}`-shaped field for a cookie element. Integers and same-width
// spans are the overwhelmingly common cookie flavours and are handled without
// materialising a binfmt_field; everything else falls back to the dispatch.
template <binfmt_policy policy, ::std::integral char_type>
inline void binfmt_batch_emit_default_at(binfmt_batch_writer<char_type> &w,
										 binfmt_cookie_element const &ce) throws
{
	auto const t{static_cast<::std::uint_least32_t>(ce.typetag)};
	if (t == static_cast<::std::uint_least32_t>(binfmt_typetag::from_format))
	{
		binfmt_malformed();
	}
	if (t == static_cast<::std::uint_least32_t>(binfmt_typetag::custom))
	{
		binfmt_batch_emit_custom(w, ce);
		return;
	}
	auto const ct{t - 1u};
	{
		bool neg;
		binfmt_mag_t mag;
		if (binfmt_default_int_mag(ct, ce, neg, mag))
		{
			binfmt_batch_emit_dec_plain(w, neg, mag);
			return;
		}
	}
	if (ct == binfmt_ct_c8view)
	{
		if constexpr (sizeof(char_type) == 1)
		{
			binfmt_batch_put(w, reinterpret_cast<char_type const *>(ce.field1),
							 static_cast<::std::size_t>(ce.field2));
			return;
		}
	}
	else if (ct == binfmt_ct_c16view)
	{
		if constexpr (sizeof(char_type) == 2)
		{
			binfmt_batch_put(w, reinterpret_cast<char_type const *>(ce.field1),
							 static_cast<::std::size_t>(ce.field2));
			return;
		}
	}
	else if (ct == binfmt_ct_c32view)
	{
		if constexpr (sizeof(char_type) == 4)
		{
			binfmt_batch_put(w, reinterpret_cast<char_type const *>(ce.field1),
							 static_cast<::std::size_t>(ce.field2));
			return;
		}
	}
	binfmt_field f{binfmt_default_field};
	f.ctype = ct;
	binfmt_batch_emit_field<policy>(w, f, ce);
}

template <binfmt_policy policy, ::std::integral char_type>
inline void binfmt_batch_emit_default_node(binfmt_batch_writer<char_type> &w,
										   void const *cookie,
										   ::std::size_t elements_count,
										   ::std::size_t &next_arg) throws
{
	auto const idx{next_arg++};
	if (idx >= elements_count)
	{
		binfmt_malformed();
	}
	binfmt_batch_emit_default_at<policy>(w, binfmt_cookie_at(cookie, idx));
}

template <binfmt_policy policy, ::std::integral char_type>
inline void binfmt_batch_emit_plain_node(binfmt_batch_writer<char_type> &w,
										 void const *cookie,
										 ::std::size_t elements_count,
										 ::std::size_t &next_arg,
										 binfmt_field base) throws
{
	auto const idx{next_arg++};
	if (idx >= elements_count)
	{
		binfmt_malformed();
	}
	auto const &ce{binfmt_cookie_at(cookie, idx)};
	auto const t{static_cast<::std::uint_least32_t>(ce.typetag)};
	binfmt_field f{base};
	if (t != static_cast<::std::uint_least32_t>(binfmt_typetag::from_format))
	{
		if (t == static_cast<::std::uint_least32_t>(binfmt_typetag::custom))
		{
			binfmt_malformed();
		}
		f.ctype = t - 1u;
	}
	binfmt_batch_emit_field<policy>(w, f, ce);
}

template <::std::integral char_type>
inline void binfmt_batch_emit_char(binfmt_batch_writer<char_type> &w,
								   binfmt_field const &f,
								   binfmt_cookie_element const &ce) throws
{
	char_type ch{};
	switch (f.ctype)
	{
	case binfmt_ct_c8:
	case binfmt_ct_cebc:
		if constexpr (sizeof(char_type) != 1)
		{
			binfmt_malformed();
		}
		ch = static_cast<char_type>(static_cast<::std::uint_least8_t>(ce.field1));
		break;
	case binfmt_ct_c16:
		if constexpr (sizeof(char_type) != 2)
		{
			binfmt_malformed();
		}
		ch = static_cast<char_type>(static_cast<::std::uint_least16_t>(ce.field1));
		break;
	case binfmt_ct_c32:
		if constexpr (sizeof(char_type) != 4)
		{
			binfmt_malformed();
		}
		ch = static_cast<char_type>(static_cast<::std::uint_least32_t>(ce.field1));
		break;
	default:
		binfmt_malformed();
	}
	binfmt_batch_padded(w, __builtin_addressof(ch), 1, 0,
						binfmt_field_fill<char_type>(f),
						binfmt_field_fill_len<char_type>(f), f.width, f.align, false,
						false);
}

template <::std::integral char_type>
inline void binfmt_batch_emit_string(binfmt_batch_writer<char_type> &w,
									 binfmt_field const &f,
									 binfmt_cookie_element const &ce) throws
{
	char_type const *sp{};
	::std::size_t sl{};
	switch (f.ctype)
	{
	case binfmt_ct_c8ptr:
	case binfmt_ct_ebcptr:
	case binfmt_ct_gbptr:
		if constexpr (sizeof(char_type) != 1)
		{
			binfmt_malformed();
		}
		sp = reinterpret_cast<char_type const *>(ce.field1);
		if (sp == nullptr)
		{
			binfmt_malformed();
		}
		sl = binfmt_unit_strlen(sp);
		break;
	case binfmt_ct_c16ptr:
		if constexpr (sizeof(char_type) != 2)
		{
			binfmt_malformed();
		}
		sp = reinterpret_cast<char_type const *>(ce.field1);
		if (sp == nullptr)
		{
			binfmt_malformed();
		}
		sl = binfmt_unit_strlen(sp);
		break;
	case binfmt_ct_c32ptr:
		if constexpr (sizeof(char_type) != 4)
		{
			binfmt_malformed();
		}
		sp = reinterpret_cast<char_type const *>(ce.field1);
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
		sp = reinterpret_cast<char_type const *>(ce.field1);
		sl = static_cast<::std::size_t>(ce.field2);
		break;
	}
	default:
		binfmt_malformed();
	}
	if (f.prec != 0 && sl > f.prec)
	{
		sl = f.prec;
	}
	if (f.width == 0)
	{
		binfmt_batch_put(w, sp, sl);
		return;
	}
	binfmt_batch_padded(w, sp, sl, 0, binfmt_field_fill<char_type>(f),
						binfmt_field_fill_len<char_type>(f), f.width, f.align, false,
						false);
}

template <::std::integral char_type>
inline void binfmt_batch_emit_pointer(binfmt_batch_writer<char_type> &w,
									  binfmt_field const &f,
									  binfmt_cookie_element const &ce) throws
{
	if (f.ctype != binfmt_ct_ptr && f.ctype != binfmt_ct_fptr)
	{
		binfmt_malformed();
	}
	char_type content[192];
	::std::size_t prefix_len{};
	auto const len{binfmt_format_uint<char_type>(content, false,
												static_cast<::std::uintptr_t>(ce.field1),
												16, false, true, 0,
												__builtin_addressof(prefix_len))};
	binfmt_batch_padded(w, content, len, prefix_len,
						binfmt_field_fill<char_type>(f),
						binfmt_field_fill_len<char_type>(f), f.width, f.align,
						f.flag_zero, true);
}

template <binfmt_policy policy, ::std::integral char_type>
inline void binfmt_batch_emit_field(binfmt_batch_writer<char_type> &w,
									binfmt_field const &f,
									binfmt_cookie_element const &ce) throws
{
	static_assert(!policy.floating_point,
				  "binfmt: floating-point fields are not implemented yet");
	auto const ty{f.type != 0u ? f.type : binfmt_default_type(f.ctype)};
	switch (ty)
	{
	case binfmt_conv_d:
	case binfmt_conv_u:
	case binfmt_conv_o:
	case binfmt_conv_x:
	case binfmt_conv_b:
		binfmt_batch_emit_int(w, f, ty, ce);
		return;
	case binfmt_conv_c:
		binfmt_batch_emit_char(w, f, ce);
		return;
	case binfmt_conv_s:
		binfmt_batch_emit_string(w, f, ce);
		return;
	case binfmt_conv_p:
	case binfmt_conv_addr:
		binfmt_batch_emit_pointer(w, f, ce);
		return;
	default:
		binfmt_malformed();
	}
}

template <binfmt_policy policy, ::std::integral char_type>
inline void binfmt_batch_vprint_impl(binfmt_batch_writer<char_type> &w,
									 void const *base, ::std::size_t len,
									 ::std::size_t elements_count,
									 void const *cookie) throws
{
	binfmt_prog p{static_cast<char8_t const *>(base),
				  static_cast<char8_t const *>(base) + len};
	::std::size_t next_arg{};
	while (p.cur != p.end)
	{
		auto const c{p.cur};
		auto const rem{static_cast<::std::size_t>(p.end - c)};
		if (rem >= 2 && c[0] == 0x13u && c[1] == 0x00u)
		{
			if (rem >= sizeof(binfmt_pat_empty_nl) &&
				::std::memcmp(c, binfmt_pat_empty_nl, sizeof(binfmt_pat_empty_nl)) == 0)
			{
				p.cur += sizeof(binfmt_pat_empty_nl);
				binfmt_batch_emit_default_node<policy>(w, cookie, elements_count, next_arg);
				char_type nl{static_cast<char_type>(u8'\n')};
				binfmt_batch_put(w, __builtin_addressof(nl), 1);
			}
			else
			{
				p.cur += sizeof(binfmt_pat_empty);
				binfmt_batch_emit_default_node<policy>(w, cookie, elements_count, next_arg);
			}
			continue;
		}
		if (rem >= 3 && c[0] == 0x0au)
		{
			auto const l{static_cast<::std::size_t>(static_cast<::std::uint_least8_t>(c[1]))};
			if (l < 0x80u && rem >= 2 + l && l % sizeof(char_type) == 0)
			{
				p.cur += 2 + l;
				binfmt_batch_put(w, reinterpret_cast<char_type const *>(c + 2),
								 l / sizeof(char_type));
				continue;
			}
		}
		else if (c[0] == 0x13u)
		{
			if (rem >= sizeof(binfmt_pat_arg) &&
				::std::memcmp(c, binfmt_pat_arg, sizeof(binfmt_pat_arg)) == 0)
			{
				auto const a{static_cast<::std::size_t>(static_cast<::std::uint_least8_t>(c[3]))};
				if (a < 0x80u)
				{
					p.cur += sizeof(binfmt_pat_arg) + 1u;
					if (a >= elements_count)
					{
						binfmt_malformed();
					}
					binfmt_batch_emit_default_at<policy>(w, binfmt_cookie_at(cookie, a));
					continue;
				}
			}
			if constexpr (policy.c_style)
			{
				if (rem >= sizeof(binfmt_pat_u64_nl) &&
					::std::memcmp(c, binfmt_pat_u64_nl, sizeof(binfmt_pat_u64_nl)) == 0)
				{
					p.cur += sizeof(binfmt_pat_u64_nl);
					binfmt_batch_emit_plain_node<policy>(w, cookie, elements_count, next_arg,
														 binfmt_plain_u64_field);
					char_type nl{static_cast<char_type>(u8'\n')};
					binfmt_batch_put(w, __builtin_addressof(nl), 1);
					continue;
				}
				if (rem >= sizeof(binfmt_pat_u64) &&
					::std::memcmp(c, binfmt_pat_u64, sizeof(binfmt_pat_u64)) == 0)
				{
					p.cur += sizeof(binfmt_pat_u64);
					binfmt_batch_emit_plain_node<policy>(w, cookie, elements_count, next_arg,
														 binfmt_plain_u64_field);
					continue;
				}
			}
		}
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
			binfmt_batch_put(w, reinterpret_cast<char_type const *>(b.ptr),
							 b.len / sizeof(char_type));
			break;
		}
		case binfmt_op_field:
		{
			if (kind != binfmt_kind_list)
			{
				binfmt_malformed();
			}
			binfmt_field f{};
			f.auto_arg = true;
			binfmt_parse_field<policy>(p, f);
			auto const idx{f.auto_arg ? next_arg++ : f.arg};
			if (idx >= elements_count)
			{
				binfmt_malformed();
			}
			auto const &ce{binfmt_cookie_at(cookie, idx)};
			auto const ctag{static_cast<::std::uint_least32_t>(ce.typetag)};
			if (ctag == static_cast<::std::uint_least32_t>(binfmt_typetag::custom))
			{
				binfmt_batch_emit_custom(w, ce);
				break;
			}
			if (ctag != static_cast<::std::uint_least32_t>(binfmt_typetag::from_format))
			{
				f.ctype = ctag - 1u;
			}
			binfmt_batch_emit_field<policy>(w, f, ce);
			break;
		}
		default:
			binfmt_skip_payload(p, kind);
			binfmt_malformed();
		}
	}
}

template <binfmt_policy policy, ::std::integral char_type>
inline void binfmt_vprint_batched_impl(basic_binfmt_device<char_type> dev,
									   void const *base, ::std::size_t len,
									   ::std::size_t elements_count,
									   void const *cookie) throws
{
	char_type local[binfmt_batch_local_units];
	binfmt_batch_writer<char_type> w{dev, local, local + binfmt_batch_local_units,
									 local, false};
	w.buffered = dev.vptr->get_output_buffer_ptrs != nullptr;
	if (w.buffered)
	{
		auto const b{::fast_io::binfmt::get_output_buffer_ptrs(dev)};
		w.curr = b.curr_ptr;
		w.end = b.end_ptr;
		w.local = nullptr;
	}
	binfmt_batch_vprint_impl<policy>(w, base, len, elements_count, cookie);
	binfmt_batch_publish(w);
}

} // namespace details

// ---------------------------------------------------------------------------
// entry points — same shape as binfmt_vprint. A buffered stream sees one buffer
// fetch and one publish; an unbuffered stream sees one scatter_write_all.
// ---------------------------------------------------------------------------

template <binfmt_policy policy = binfmt_policy_default>
inline void binfmt_vprint_batched(binfmt_device dev, void const *base,
								  ::std::size_t len, ::std::size_t elements_count,
								  void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<policy>(dev, base, len, elements_count, cookie);
}

template <binfmt_policy policy = binfmt_policy_default>
inline void wbinfmt_vprint_batched(wbinfmt_device dev, void const *base,
								   ::std::size_t len, ::std::size_t elements_count,
								   void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<policy>(dev, base, len, elements_count, cookie);
}

template <binfmt_policy policy = binfmt_policy_default>
inline void u8binfmt_vprint_batched(u8binfmt_device dev, void const *base,
									::std::size_t len, ::std::size_t elements_count,
									void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<policy>(dev, base, len, elements_count, cookie);
}

template <binfmt_policy policy = binfmt_policy_default>
inline void u16binfmt_vprint_batched(u16binfmt_device dev, void const *base,
									 ::std::size_t len, ::std::size_t elements_count,
									 void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<policy>(dev, base, len, elements_count, cookie);
}

template <binfmt_policy policy = binfmt_policy_default>
inline void u32binfmt_vprint_batched(u32binfmt_device dev, void const *base,
									 ::std::size_t len, ::std::size_t elements_count,
									 void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<policy>(dev, base, len, elements_count, cookie);
}

} // namespace fast_io::binfmt
