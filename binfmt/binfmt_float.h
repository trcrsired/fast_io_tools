#pragma once

// binfmt floating-point formatting helper.
//
// The interpreter must not carry a formatter instantiation per character
// flavour: the shortest/precision emitters are large templates, and
// instantiating them for five char_types x six formats x ten conversions
// would dwarf the whole interpreter. So all formatting happens once, in
// `char`, inside libbinfmt, and the interpreter only widens the resulting
// ASCII run while it writes it out.
//
// The request carries the raw cookie bytes, not a typed float: the cookie
// is `{size_t field1, size_t field2}` = 16 contiguous bytes, which is the
// exact storage of every supported format (f16/bf16/f32/f64 fit in field1,
// f80/f128 use both). Decoding is therefore a memcpy, and this header stays
// free of fast_io and of every float type.
//
// Errors (unsupported ctype/conv, precision beyond the exact-expansion
// capacity) throw std::errc::invalid_argument.

#if !defined(__HERBCEPTIONS__)
#error "fast_io::binfmt requires herbceptions (-fherbceptions); legacy C++ exceptions are not supported"
#endif

#include <cstddef>
#include <cstdint>

namespace fast_io::binfmt
{

// Maximum significant/fractional precision the exact decimal expansion can
// serve (fast_io's fp_digits_capacity). Larger requests are rejected.
inline constexpr ::std::size_t binfmt_float_max_precision{11566u};

// Worst-case output length: 3 prefix units (sign + 0x), the fixed-format
// cache (sign + widest integer part + point + exact digits), and the
// requested fractional digits. Exact digits and requested precision are
// both capped at binfmt_float_max_precision; the widest e10max (binary128)
// is 4966.
inline constexpr ::std::size_t binfmt_float_capacity{
	3u + 1u + 4966u + 2u + binfmt_float_max_precision + binfmt_float_max_precision};

struct binfmt_float_request
{
	::std::size_t bits0;		   // cookie field1: low 64 bits
	::std::size_t bits1;		   // cookie field2: high 64 bits (f80/f128 only)
	::std::uint_least32_t ctype;   // binfmt_ct_f16 .. binfmt_ct_f128
	::std::uint_least32_t conv;	   // binfmt_conv_dec .. binfmt_conv_hexfp
	::std::uint_least32_t sign;	   // binfmt_sign_* (0 none)
	::std::size_t prec;			   // ignored unless the conv carries a precision
	bool upper;					   // uppercase digits/exponent/base prefix
	bool showbase;				   // 0x prefix for the hexfloat conversions
};

struct binfmt_float_result
{
	::std::size_t len;		  // total units written to `out`
	::std::size_t prefix_len; // sign + base prefix, i.e. where zero padding goes
	bool nonfinite;			  // inf/nan: never zero-padded
};

// Format one value into `out`, which must hold binfmt_float_capacity units.
binfmt_float_result binfmt_float_format(char *out, binfmt_float_request const &r) throws;

} // namespace fast_io::binfmt
