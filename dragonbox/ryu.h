#pragma once

namespace fast_io::details::fp::ryu
{
template<std::floating_point F>
inline constexpr floating_point_result<F> ryu_main(typename floating_traits<F>::mantissa_type mantissa,typename floating_traits<F>::exponent_type exponent) noexcept
{
	using namespace fast_io::details::ryu;
	using floating_type = std::remove_cvref_t<F>;
	using floating_trait = floating_traits<floating_type>;
	using mantissa_type = typename floating_traits<F>::mantissa_type;
	using exponent_type = typename floating_trait::exponent_type;
	using signed_exponent_type = std::make_signed_t<exponent_type>;

	auto const r2(init_repm2<F>(mantissa,static_cast<signed_exponent_type>(exponent)));
	bool const accept_bounds(!(r2.m&1));
	auto const mv(r2.m<<2);
	exponent_type mm_shift;
#ifdef __SIZEOF_INT128__
	if constexpr(std::same_as<std::remove_cvref_t<F>,long double>)
		mm_shift=(mantissa!=static_cast<mantissa_type>(1)<<(floating_trait::mantissa_bits-1))||exponent==0;
	else
#endif
		mm_shift=(mantissa||static_cast<signed_exponent_type>(exponent)<2);
	//vr,vp,vm
	mantissa_type vr,vp,vm;
	signed_exponent_type e10{};
	bool vm_is_trailing_zeros{},vr_is_trailing_zeros{};
	char8_t last_removed_digit{};
	if(0<=r2.e)
	{
		exponent_type q(log10_pow2(r2.e));
		if constexpr(!std::same_as<floating_type,float>)
			q-=(3<r2.e);
		e10=static_cast<signed_exponent_type>(q);
		signed_exponent_type const k(floating_trait::pow5_inv_bitcount + pow5bits(q) - 1);
		signed_exponent_type const i(-r2.e+static_cast<signed_exponent_type>(q)+k);
#ifdef __SIZEOF_INT128__
		if constexpr(std::same_as<std::remove_cvref_t<F>,long double>)
		{
			auto pow5arr{generic_compute_pow5_inv(q)};
			vr = mul_shift_generic(mv, pow5arr, i);
			vp = mul_shift_generic(mv+2, pow5arr, i);
			vm = mul_shift_generic(mv-1-mm_shift, pow5arr, i);
		}
		else
#endif
		if constexpr(std::same_as<floating_type,double>)
			vr=mul_shift_all(r2.m,pow5<F,true>::inv_split[q],i,vp,vm,mm_shift);
		else if constexpr(std::same_as<floating_type,float>)
		{
			vr = mul_pow5_inv_div_pow2(mv, q, i);
			vp = mul_pow5_inv_div_pow2(mv+2, q, i);
			vm = mul_pow5_inv_div_pow2(mv-1-mm_shift, q, i);
			if (q != 0 && (vp - 1) / 10 <= vm / 10)
			{
				std::int32_t const l = floating_trait::pow5_inv_bitcount + pow5bits(static_cast<std::int32_t>(q - 1)) - 1;
				last_removed_digit = static_cast<char8_t>(mul_pow5_inv_div_pow2(mv, q - 1, -r2.e + static_cast<std::int32_t>(q) - 1 + l) % 10);
			}
		}
		if(q<=floating_trait::floor_log5)//here
		{
			if(!(mv%5))
				vr_is_trailing_zeros=multiple_of_power_of_5(mv,q);
			else if(accept_bounds)
				vm_is_trailing_zeros=multiple_of_power_of_5(mv-1-mm_shift,q);
			else
				vp-=multiple_of_power_of_5(mv+2,q);
		}
	}
	else
	{
		exponent_type abs_e2(static_cast<exponent_type>(-r2.e));
		exponent_type q(log10_pow5(abs_e2));
		if constexpr(!std::same_as<floating_type,float>)
			q-=1<abs_e2;
		signed_exponent_type const signed_q(static_cast<signed_exponent_type>(q));
		e10=signed_q+r2.e;
		signed_exponent_type const i(-r2.e-signed_q);
		signed_exponent_type const k(pow5bits(i)-floating_trait::pow5_bitcount);
		signed_exponent_type j(signed_q-k);
#ifdef __SIZEOF_INT128__
		if constexpr(std::same_as<floating_type,long double>)
		{
			auto pow5arr{generic_compute_pow5(i)};
			vr = mul_shift_generic(mv, pow5arr, j);
			vp = mul_shift_generic(mv+2, pow5arr, j);
			vm = mul_shift_generic(mv-1-mm_shift, pow5arr, j);
		}
		else
#endif
		if constexpr(std::same_as<floating_type,double>)
			vr=mul_shift_all(r2.m,pow5<F,true>::split[i],j,vp,vm,mm_shift);
		else if constexpr(std::same_as<floating_type,float>)
		{
			vr = mul_pow5_div_pow2(mv, static_cast<std::uint32_t>(i), j);
			vp = mul_pow5_div_pow2(mv+2, static_cast<std::uint32_t>(i), j);
			vm = mul_pow5_div_pow2(mv-1-mm_shift, static_cast<std::uint32_t>(i), j);
			if (q != 0 && (vp - 1) / 10 <= vm / 10)
			{
				j = static_cast<std::int32_t>(q) - 1 - (pow5bits(i + 1) - floating_trait::pow5_bitcount);
				last_removed_digit = static_cast<char8_t>(mul_pow5_div_pow2(mv,static_cast<std::uint32_t>(i + 1), j) % 10);
			}
		}
		if(q<2)
		{
			vr_is_trailing_zeros=true;
			if(accept_bounds)
				vm_is_trailing_zeros=mm_shift==1;
			else
				--vp;
		}
		else if(q<floating_trait::bound)
		{
			if constexpr(std::same_as<floating_type,float>)
				vr_is_trailing_zeros=multiple_of_power_of_2(mv,q-1);
			else
				vr_is_trailing_zeros=multiple_of_power_of_2(mv,q);
		}
	}
	signed_exponent_type removed{};
	if(vm_is_trailing_zeros||vr_is_trailing_zeros)
	{
		for(;;)
		{
			mantissa_type const vpdiv10(vp/10);
			mantissa_type const vmdiv10(vm/10);
			auto const vmmod10(static_cast<char8_t>(vm%10));
			if(vpdiv10 <= vmdiv10)
				break;
			mantissa_type const vrdiv10(vr/10);
			auto const vrmod10(static_cast<char8_t>(vr%10));
			vm_is_trailing_zeros&=!vmmod10;
			vr_is_trailing_zeros&=!last_removed_digit;
			last_removed_digit=static_cast<char8_t>(vrmod10);
			vr=vrdiv10;
			vp=vpdiv10;
			vm=vmdiv10;
			++removed;
		}
		if(vm_is_trailing_zeros)
		{
			if constexpr(std::same_as<floating_type,float>)
			{
//https://github.com/ulfjack/ryu/issues/156 I am not sure whether it works. But I am going to adapt it.
				while (vm % 10 == 0)
				{
					vm /= 10;
					vr = vm;
					last_removed_digit = 0;
					++removed;
				}
			}
			else
			{
				for(;;)
				{
					mantissa_type const vmdiv10(vm/10);
					auto const vmmod10(static_cast<char8_t>(vm%10));
					if(vmmod10)
						break;
					mantissa_type const vpdiv10(vp/10);
					mantissa_type const vrdiv10(vr/10);
					auto const vrmod10(vr%10);
					vr_is_trailing_zeros&=!last_removed_digit;
					last_removed_digit=static_cast<char8_t>(vrmod10);
					vr=vrdiv10;
					vp=vpdiv10;
					vm=vmdiv10;
					++removed;
				}
			}
		}
		if(vr_is_trailing_zeros&&last_removed_digit==5&&!(vr&1))
			last_removed_digit=4;
		vr += ((vr==vm&&(!accept_bounds || !vm_is_trailing_zeros))|| 4 < last_removed_digit);
	}
	else
	{
		if constexpr(std::same_as<floating_type,float>)
		{
			for (;;)
			{
				mantissa_type const vpdiv10(vp/10);
				mantissa_type const vmdiv10(vm/10);
				if(vpdiv10<=vmdiv10)
					break;
				mantissa_type const vrdiv10(vr/10);
				auto const vrmod10(vr%10);
				last_removed_digit=vrmod10;
				vr=vrdiv10;
				vp=vpdiv10;
				vm=vmdiv10;
				++removed;
			}
			vr += (vr == vm || last_removed_digit >= 5);
		}
		else
		{
			bool round_up{};
			mantissa_type const vpdiv100(vp/100);
			mantissa_type const vmdiv100(vm/100);
			if(vmdiv100<vpdiv100)
			{
				mantissa_type const vrdiv100(vr/100);
				auto const vrmod100(vr%100);
				round_up=50<=vrmod100;
				vr=vrdiv100;
				vp=vpdiv100;
				vm=vmdiv100;
				removed+=2;
			}
			for (;;)
			{
				mantissa_type const vpdiv10(vp/10);
				mantissa_type const vmdiv10(vm/10);
				if(vpdiv10<=vmdiv10)
					break;
				mantissa_type const vrdiv10(vr/10);
				auto const vrmod10(vr%10);
				round_up=5<=vrmod10;
				vr=vrdiv10;
				vp=vpdiv10;
				vm=vmdiv10;
				++removed;
			}
			vr+=(vr==vm||round_up);
		}
	}
	return {vr,e10+removed};
}

}