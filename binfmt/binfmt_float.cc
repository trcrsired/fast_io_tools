// binfmt floating-point helper — out-of-line, char-only.
//
// Everything here is intentionally compiled once for `char`. The
// shortest/precision emitters are large templates; the five char_type
// flavours widen the ASCII result while writing it out (see
// binfmt_print.h / binfmt_batch.h), so this translation unit holds only
// 6 formats x 10 conversions, not 5 x 6 x 10.
//
// f16/bf16 are decoded through two 2-byte stand-in types with their own
// iec559_traits specializations: fast_io's generic punning and
// to_decimal<flt> dispatch then pick binary16/bfloat16 exactly as they
// would for a language type, so the helper does not depend on _Float16 /
// __bf16 being available.

#include <fast_io.h>

#include <cstring>

#include "binfmt_float.h"
#include "binfmt_print.h"
#include "binfmt_batch.h"

namespace fast_io::details
{

struct binfmt_binary16
{
	::std::uint_least16_t bits;
};

struct binfmt_bfloat16
{
	::std::uint_least16_t bits;
};

template <>
struct iec559_traits<binfmt_binary16>
{
	using mantissa_type = ::std::uint_least16_t;
	inline static constexpr ::std::size_t mbits{10};
	inline static constexpr ::std::size_t ebits{5};
	inline static constexpr ::std::uint_least32_t m10digits{5};
	inline static constexpr ::std::uint_least32_t m2hexdigits{3};
	inline static constexpr ::std::uint_least32_t e10digits{2};
	inline static constexpr ::std::uint_least32_t e2hexdigits{2};
	inline static constexpr ::std::uint_least32_t e10max{4};
};

template <>
struct iec559_traits<binfmt_bfloat16>
{
	using mantissa_type = ::std::uint_least16_t;
	inline static constexpr ::std::size_t mbits{7};
	inline static constexpr ::std::size_t ebits{8};
	inline static constexpr ::std::uint_least32_t m10digits{4};
	inline static constexpr ::std::uint_least32_t m2hexdigits{2};
	inline static constexpr ::std::uint_least32_t e10digits{2};
	inline static constexpr ::std::uint_least32_t e2hexdigits{2};
	inline static constexpr ::std::uint_least32_t e10max{38};
};

} // namespace fast_io::details

namespace fast_io::binfmt
{

namespace details
{

// sign-bit clear. Every supported format keeps its sign as the top bit of
// its raw storage; the x87 80-bit format keeps it in bit 15 of the second
// 16-bit word (the exponent word).
template <typename flt>
inline flt binfmt_float_abs(flt f) noexcept
{
	if constexpr (sizeof(flt) <= 8u)
	{
		using uint_type = ::std::conditional_t<
			sizeof(flt) == 2u, ::std::uint16_t,
			::std::conditional_t<sizeof(flt) == 4u, ::std::uint32_t, ::std::uint64_t>>;
		auto u{__builtin_bit_cast(uint_type, f)};
		u &= static_cast<uint_type>(~static_cast<uint_type>(uint_type{1} << (sizeof(uint_type) * 8u - 1u)));
		return __builtin_bit_cast(flt, u);
	}
#if defined(__LDBL_MANT_DIG__) && __LDBL_MANT_DIG__ == 64 && defined(__SIZEOF_LONG_DOUBLE__) && \
	__SIZEOF_LONG_DOUBLE__ == 16
	else if constexpr (::std::same_as<::std::remove_cv_t<flt>, long double>)
	{
		using storage_type = ::fast_io::details::float80_storage<
			sizeof(flt) - sizeof(::std::uint_least64_t) - sizeof(::std::uint_least16_t)>;
		auto s{__builtin_bit_cast(storage_type, f)};
		s.exponent &= 0x7fffu;
		return __builtin_bit_cast(flt, s);
	}
#endif
	else
	{
#if defined(__SIZEOF_INT128__)
		auto u{__builtin_bit_cast(__uint128_t, f)};
		u &= ~(static_cast<__uint128_t>(1) << 127u);
		return __builtin_bit_cast(flt, u);
#else
		return f;
#endif
	}
}

// one format x one conversion. All decoration flags are false here; sign,
// base prefix, zero padding and uppercase are handled by the caller, so
// each (type, conv) pair has exactly one instantiation.
template <typename flt>
inline char *binfmt_float_write_conv(char *iter, flt v, ::std::uint_least32_t conv,
									 ::std::size_t prec) throws
{
	using ::fast_io::manipulators::floating_format;
	switch (conv)
	{
	case binfmt_conv_dec:
		return ::fast_io::details::print_rsvflt_define_impl<false, false, false, false,
															 floating_format::decimal>(iter, v);
	case binfmt_conv_fix:
		return ::fast_io::details::print_rsvflt_define_impl<false, false, false, false,
															 floating_format::fixed>(iter, v);
	case binfmt_conv_sci:
		return ::fast_io::details::print_rsvflt_define_impl<false, false, false, false,
															 floating_format::scientific>(iter, v);
	case binfmt_conv_gen:
		return ::fast_io::details::print_rsvflt_define_impl<false, false, false, false,
															 floating_format::general>(iter, v);
	case binfmt_conv_hexf:
		return ::fast_io::details::print_rsvhexfloat_define_impl<false, false, false, false, false,
																  false>(iter, v);
	case binfmt_conv_decp:
		return ::fast_io::details::print_precision_flt_define_impl<false, false, false, false,
																	floating_format::decimal>(iter, v, prec);
	case binfmt_conv_fixp:
		return ::fast_io::details::print_precision_flt_define_impl<false, false, false, false,
																	floating_format::fixed>(iter, v, prec);
	case binfmt_conv_scip:
		return ::fast_io::details::print_precision_flt_define_impl<false, false, false, false,
																	floating_format::scientific>(iter, v, prec);
	case binfmt_conv_genp:
		return ::fast_io::details::print_precision_flt_define_impl<false, false, false, false,
																	floating_format::general>(iter, v, prec);
	case binfmt_conv_hexfp:
		return ::fast_io::details::print_precision_hexfloat_define_impl<false, false, false, false, false,
																		 false>(iter, v, prec);
	default:
		throw throws ::std::errc::invalid_argument;
	}
}

template <typename flt>
inline binfmt_float_result binfmt_float_format_value(char *out, binfmt_float_request const &r,
													 flt v) throws
{
	using trait = ::fast_io::details::iec559_traits<flt>;
	constexpr ::std::uint_least32_t exponent_mask{static_cast<::std::uint_least32_t>(
		(static_cast<::std::uint_least32_t>(1) << trait::ebits) - 1u)};
	auto const punned{::fast_io::details::get_punned_result(v)};
	bool const nonfinite{punned.exponent == exponent_mask};

	::std::size_t p{};
	if (punned.sign)
	{
		out[p++] = '-';
	}
	else if (r.sign == binfmt_sign_plus)
	{
		out[p++] = '+';
	}
	else if (r.sign == binfmt_sign_space)
	{
		out[p++] = ' ';
	}
	// printf and fast_io's hexfloat print no base prefix for inf/nan.
	bool const showbase{r.showbase && !nonfinite};
	if (showbase)
	{
		out[p++] = '0';
		out[p++] = r.upper ? 'X' : 'x';
	}
	char *const content{out + p};
	char *const end{binfmt_float_write_conv(content, binfmt_float_abs(v), r.conv, r.prec)};
	if (r.upper)
	{
		char *q{out};
		// fast_io prints a 128-bit mantissa (f80 through __float128, and
		// f128 itself) through a split integral routine that drops the
		// uppercase flag for the digits, while the base prefix and the
		// exponent letter stay uppercase. Reproduce that exactly.
		bool const split_mantissa{
			!nonfinite && (r.ctype == binfmt_ct_f80 || r.ctype == binfmt_ct_f128) &&
			(r.conv == binfmt_conv_hexf || r.conv == binfmt_conv_hexfp)};
		if (split_mantissa)
		{
			while (q != end && *q != 'p' && *q != 'P')
			{
				++q;
			}
		}
		for (; q != end; ++q)
		{
			if (*q >= 'a' && *q <= 'z')
			{
				*q = static_cast<char>(*q - ('a' - 'A'));
			}
		}
	}
	return {static_cast<::std::size_t>(end - out), p, nonfinite};
}

inline long double binfmt_float_decode_f80(binfmt_float_request const &r) throws
{
#if defined(__LDBL_MANT_DIG__) && __LDBL_MANT_DIG__ == 64 && defined(__SIZEOF_LONG_DOUBLE__) && \
	__SIZEOF_LONG_DOUBLE__ == 16
	unsigned char raw[16];
	::std::memcpy(raw, &r.bits0, 8u);
	::std::memcpy(raw + 8u, &r.bits1, 8u);
	long double v;
	::std::memcpy(&v, raw, sizeof(v));
	return v;
#else
	(void)r;
	throw throws ::std::errc::invalid_argument;
#endif
}

#if defined(__SIZEOF_INT128__) && (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__))
inline __float128 binfmt_float_decode_f128(binfmt_float_request const &r) noexcept
{
	unsigned char raw[16];
	::std::memcpy(raw, &r.bits0, 8u);
	::std::memcpy(raw + 8u, &r.bits1, 8u);
	__float128 v;
	::std::memcpy(&v, raw, sizeof(v));
	return v;
}
#endif

} // namespace details

binfmt_float_result binfmt_float_format(char *out, binfmt_float_request const &r) throws
{
	if (r.prec > binfmt_float_max_precision)
	{
		throw throws ::std::errc::invalid_argument;
	}
	using namespace details;
	switch (r.ctype)
	{
	case binfmt_ct_f16:
	{
		auto const u{static_cast<::std::uint_least16_t>(r.bits0)};
		return binfmt_float_format_value(out, r, __builtin_bit_cast(::fast_io::details::binfmt_binary16, u));
	}
	case binfmt_ct_bf16:
	{
		auto const u{static_cast<::std::uint_least16_t>(r.bits0)};
		return binfmt_float_format_value(out, r, __builtin_bit_cast(::fast_io::details::binfmt_bfloat16, u));
	}
	case binfmt_ct_f32:
	{
		auto const u{static_cast<::std::uint_least32_t>(r.bits0)};
		return binfmt_float_format_value(out, r, __builtin_bit_cast(float, u));
	}
	case binfmt_ct_f64:
	{
		auto const u{static_cast<::std::uint_least64_t>(r.bits0)};
		return binfmt_float_format_value(out, r, __builtin_bit_cast(double, u));
	}
	case binfmt_ct_f80:
		return binfmt_float_format_value(out, r, binfmt_float_decode_f80(r));
#if defined(__SIZEOF_INT128__) && (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__))
	case binfmt_ct_f128:
		return binfmt_float_format_value(out, r, binfmt_float_decode_f128(r));
#endif
	default:
		throw throws ::std::errc::invalid_argument;
	}
}

// ---------------------------------------------------------------------------
// float-enabled interpreter entries. These are the only difference from the
// ordinary entries: the same core loop, instantiated with a policy that
// accepts binary float fields. Defined here so libbinfmt_float is the only
// image that pulls the float helper in.
// ---------------------------------------------------------------------------

void binfmt_status_vprint_float(binfmt_device dev, void const *base, ::std::size_t len,
								::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void wbinfmt_status_vprint_float(wbinfmt_device dev, void const *base, ::std::size_t len,
								 ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void u8binfmt_status_vprint_float(u8binfmt_device dev, void const *base, ::std::size_t len,
								  ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void u16binfmt_status_vprint_float(u16binfmt_device dev, void const *base, ::std::size_t len,
								   ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void u32binfmt_status_vprint_float(u32binfmt_device dev, void const *base, ::std::size_t len,
								   ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void binfmt_status_batch_vprint_float(binfmt_device dev, void const *base, ::std::size_t len,
									  ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void wbinfmt_status_batch_vprint_float(wbinfmt_device dev, void const *base, ::std::size_t len,
									   ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void u8binfmt_status_batch_vprint_float(u8binfmt_device dev, void const *base, ::std::size_t len,
										::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void u16binfmt_status_batch_vprint_float(u16binfmt_device dev, void const *base, ::std::size_t len,
										 ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

void u32binfmt_status_batch_vprint_float(u32binfmt_device dev, void const *base, ::std::size_t len,
										 ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_batched_impl<binfmt_policy_c_style_float>(dev, base, len, elements_count, cookie);
}

} // namespace fast_io::binfmt
