#pragma once

namespace fast_io::details::fp::large_int
{

template<floating_mode flt_mode,bool uppercase_e,char8_t decimal_point,std::random_access_iterator iter_type,std::floating_point fp_type>
requires (decimal_point==u8'.'||decimal_point==u8',')
inline constexpr iter_type fp_to_chars(iter_type result, fp_type d) noexcept
{
	using floating_type = std::remove_cvref_t<fp_type>;
	using char_type = std::remove_reference_t<decltype(*result)>;
	using floating_trait = floating_traits<floating_type>;
	using mantissa_type = typename floating_trait::mantissa_type;
	using exponent_type = typename floating_trait::exponent_type;
	using signed_exponent_type = std::make_signed_t<exponent_type>;

	auto const bits{std::bit_cast<mantissa_type>(d)};

	bool const sign((bits >> (floating_trait::mantissa_bits + floating_trait::exponent_bits)) & 1u);
	mantissa_type const mantissa(bits & ((static_cast<mantissa_type>(1u) << floating_trait::mantissa_bits) - 1u));
	exponent_type const exponent(static_cast<exponent_type>(((bits >> floating_trait::mantissa_bits) & floating_trait::exponent_max)));

	// Case distinction; exit early for the easy cases.
	if(exponent == floating_trait::exponent_max)
		return nan_inf_case(result,sign,mantissa);
	if(!exponent&&!mantissa)
		return zero_case<flt_mode,uppercase_e>(result,sign);
	{
	auto const r2(init_rep<floating_type,false>(mantissa,static_cast<signed_exponent_type>(exponent)));
	if(-static_cast<std::int32_t>(floating_trait::mantissa_bits)<=r2.e&&r2.e<=0)[[likely]]
	{
		mantissa_type const mask{(static_cast<mantissa_type>(1) << -r2.e) - 1};
		if (!(r2.m & mask))[[likely]]
			return integer_case<flt_mode,uppercase_e,decimal_point>(result,sign,(r2.m>>-r2.e));
	}
	}
	auto res{fast_io::details::fp::ryu::ryu_main<floating_type>(mantissa,exponent)};
	return normal_case<flt_mode,uppercase_e,decimal_point>(result,sign,res.significand,res.exponent);
}

}