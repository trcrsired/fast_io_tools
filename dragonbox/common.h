#pragma once

namespace fast_io::details::fp
{

template<std::floating_point T>
struct floating_traits
{
};


template<>
struct floating_traits<float>
{
	using mantissa_type = std::uint32_t;
	using exponent_type = std::uint32_t;
	static inline constexpr exponent_type exponent_bits = 8;
	static inline constexpr exponent_type mantissa_bits = sizeof(float)*8-1-exponent_bits;
	static inline constexpr exponent_type exponent_max = (static_cast<exponent_type>(1)<<exponent_bits)-1;
	static inline constexpr exponent_type bias = (static_cast<exponent_type>(1)<<(exponent_bits - 1)) - 1;
	static inline constexpr exponent_type pow5_inv_bitcount= 59;
	static inline constexpr exponent_type pow5_bitcount= 61;
	static inline constexpr exponent_type floor_log5 = 9;
	static inline constexpr exponent_type bound = 31;//ryu to do. use a tigher bound
	static inline constexpr exponent_type digits10=9;
	static inline constexpr mantissa_type carry10=0x989680;
	static inline constexpr int32_t minimum_exp=-45;
	static inline constexpr int32_t maximum_exp=39;
	static inline constexpr std::size_t required_buffer_size{105};
};

template<>
struct floating_traits<double>
{
	using mantissa_type = std::uint64_t;
	using exponent_type = std::uint32_t;
	static inline constexpr exponent_type exponent_bits = 11;
	static inline constexpr exponent_type mantissa_bits = sizeof(double)*8-1-exponent_bits;
	static inline constexpr exponent_type exponent_max = (static_cast<exponent_type>(1)<<exponent_bits)-1;
	static inline constexpr exponent_type bias = (static_cast<exponent_type>(1)<<(exponent_bits - 1)) - 1;
	static inline constexpr exponent_type pow5_inv_bitcount= 125;
	static inline constexpr exponent_type pow5_bitcount= 125;
	static inline constexpr exponent_type floor_log5 = 21;
	static inline constexpr exponent_type bound = 63;//ryu to do. use a tigher bound
	static inline constexpr exponent_type digits10=17;
	static inline constexpr mantissa_type carry10=0x2386F26FC10000;
	static inline constexpr int32_t minimum_exp=-323;
	static inline constexpr int32_t maximum_exp=309;
	static inline constexpr std::size_t required_buffer_size{769};
};

#ifdef __SIZEOF_INT128__
template<>
struct floating_traits<long double>
{
	using mantissa_type = __uint128_t;
	using exponent_type = std::uint32_t;
	static inline constexpr exponent_type exponent_bits = 15;
	static inline constexpr exponent_type mantissa_bits = 64;
	static inline constexpr exponent_type exponent_max = (static_cast<exponent_type>(1)<<exponent_bits)-1;
	static inline constexpr exponent_type bias = (static_cast<exponent_type>(1)<<(exponent_bits - 1)) - 1;
	static inline constexpr exponent_type pow5_inv_bitcount= 249;
	static inline constexpr exponent_type pow5_bitcount= 249;
	static inline constexpr exponent_type floor_log5 = 55;
	static inline constexpr exponent_type bound = 127;//ryu to do. use a tigher bound
	static inline constexpr exponent_type digits10=21;
	static inline constexpr int32_t minimum_exp=-4950;
	static inline constexpr int32_t maximum_exp=4931;
//	static inline constexpr mantissa_type carry10=10000000000000000ull;
};
#endif


template<my_integral mantissaType,std::integral exponentType>
struct unrep
{
	using mantissa_type = mantissaType;
	using exponent_type = exponentType;
	mantissa_type m;
	exponent_type e;
};

template<std::floating_point floating,bool ignore_exp0=false,my_unsigned_integral mantissaType,std::signed_integral exponentType>
inline constexpr unrep<mantissaType,exponentType> init_rep(mantissaType mantissa,exponentType exponent)
{
	if constexpr(sizeof(floating)==16)
	{
		if constexpr(!ignore_exp0)
		{
			if(!exponent)
				return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits)};
		}
		return {mantissa,static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits))};
	}	
	else
	{
		if constexpr(!ignore_exp0)
		{
			if(!exponent)
				return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits)};
		}
		return {static_cast<mantissaType>((static_cast<mantissaType>(1)<<floating_traits<floating>::mantissa_bits)|mantissa),
			static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits))};
	}
}


template<bool uppercase_e=false,bool four_digits=false,std::random_access_iterator Iter>
inline constexpr Iter output_exp(std::int32_t exp,Iter result)
{
	using char_type = std::iter_value_t<Iter>;
	if constexpr(uppercase_e)
	{
		if constexpr(std::same_as<char_type,char>)
			*result='E';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'E';
		else
			*result=u8'E';
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			*result='e';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'e';
		else
			*result=u8'e';
	}
	++result;
	if(exp<0)
	{
		if constexpr(std::same_as<char_type,char>)
			*result='-';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'-';
		else
			*result=u8'-';
		exp=-exp;
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			*result='+';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'+';
		else
			*result=u8'+';
	}
	++result;
	std::uint32_t unsigned_exp(exp);
#ifdef FAST_IO_OPTIMIZE_SIZE
	std::size_t len{2};
	if constexpr(four_digits)
	{
		if(1000<=unsigned_exp)[[unlikely]]
			len=4;
		else if(100<=unsigned_exp)[[unlikely]]
			len=3;
	}
	else
	{
		if(100<=unsigned_exp)[[unlikely]]
			len=3;
	}
	auto str{result+len};
	for(std::size_t i{};i!=len;++i)
	{
		std::make_unsigned_t<T> const temp(unsigned_exp/10);
		char_type const res(unsigned_exp%10);
		if constexpr(std::same_as<char_type,char>)
			*--str='0'+res;
		else if constexpr(std::same_as<char_type,wchar_t>)
			*--str=L'0'+res;
		else
			*--str=u8'0'+res;
		unsigned_exp=temp;
	}
	return result+len;
#elif defined(FAST_IO_OPTIMIZE_TIME)
	if constexpr(four_digits)
	{
	if(1000<=unsigned_exp)[[unlikely]]
		return non_overlapped_copy_n(jiaendu::static_tables<char_type>::table4[unsigned_exp].data(),4,result);
	}
	if(100<=unsigned_exp)[[unlikely]]
		return non_overlapped_copy_n(jiaendu::static_tables<char_type>::table3[unsigned_exp].data(),3,result);
	return non_overlapped_copy_n(jiaendu::static_tables<char_type>::table2[unsigned_exp].data(),2,result);
#else
	constexpr auto& tb{shared_static_base_table<char_type,10,false>::table};
	if constexpr(four_digits)
	{
	if(1000<=unsigned_exp)[[unlikely]]
	{
		auto u{unsigned_exp%100u};
		auto v{unsigned_exp/100u};
		non_overlapped_copy_n(tb[v].data(),2,result);
		non_overlapped_copy_n(tb[u].data(),2,result+2);
		return result+4;
	}
	}
	if(100<=unsigned_exp)[[unlikely]]
	{
		auto u{unsigned_exp%100u};
		if constexpr(std::same_as<char_type,char>)
			*result=unsigned_exp/100u+'0';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=unsigned_exp/100u+L'0';
		else
			*result=unsigned_exp/100u+u8'0';
		non_overlapped_copy_n(tb[u].data(),2,result+1);
		return result+3;
	}
	return non_overlapped_copy_n(tb[unsigned_exp].data(),2,result);
#endif
}

template<std::floating_point floating,my_unsigned_integral mantissaType,std::signed_integral exponentType>
inline constexpr unrep<mantissaType,exponentType> init_repm2(mantissaType mantissa,exponentType exponent)
{
	if constexpr(std::same_as<floating,long double>)
	{
	if(!exponent)
		return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+1)};
	return {mantissa,
		static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+1))};
	}
	else
	{
	if(!exponent)
		return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+2)};
	return {static_cast<mantissaType>((static_cast<mantissaType>(1)<<floating_traits<floating>::mantissa_bits)|mantissa),
		static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+2))};
	}
}



}