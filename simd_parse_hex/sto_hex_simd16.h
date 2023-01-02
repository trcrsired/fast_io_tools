#pragma once

template<typename T>
inline constexpr T create_simd_vector_with_single_value(typename T::value_type value) noexcept
{
	::fast_io::freestanding::array<typename T::value_type,T::size()> arr;
	for(auto& e : arr)
	{
		e=value;
	}
	return std::bit_cast<T>(arr);
}

inline constexpr ::fast_io::intrinsics::simd_vector<char unsigned,16> create_simd16_vector(char unsigned value) noexcept
{
	return create_simd_vector_with_single_value<::fast_io::intrinsics::simd_vector<char unsigned,16>>(value);
}

template<std::integral char_type>
inline constexpr char_type const* skip_hexdigits(char_type const* first,char_type const* last) noexcept
{
	using ::fast_io::intrinsics::simd_vector;
	constexpr char8_t zero_constant{static_cast<char8_t>(char_literal_v<u8'0',char_type>)};

	constexpr char8_t A_constant{static_cast<char8_t>(char_literal_v<u8'A',char_type>)};

	constexpr char8_t a_constant{static_cast<char8_t>(char_literal_v<u8'a',char_type>)};

	constexpr simd_vector<char unsigned,16> zerovec{create_simd16_vector(zero_constant)};
	constexpr simd_vector<char unsigned,16> Avec{create_simd16_vector(A_constant)};
	constexpr simd_vector<char unsigned,16> avec{create_simd16_vector(a_constant)};

	constexpr simd_vector<char unsigned,16> tenvec{create_simd16_vector(10)};
	constexpr simd_vector<char unsigned,16> sixvec{create_simd16_vector(6)};
	
	for(simd_vector<char unsigned,16> data16;16<=last-first;first+=16)
	{
		data16.load(first);
		simd_vector<char unsigned,16> datamzero{data16-zerovec};
		simd_vector<char unsigned,16> datamA{data16-Avec};
		simd_vector<char unsigned,16> datama{data16-avec};
		simd_vector<char unsigned,16> datain09{datamzero<tenvec};
		simd_vector<char unsigned,16> datainAE{datamA<sixvec};
		simd_vector<char unsigned,16> datainae{datama<sixvec};

		simd_vector<char unsigned,16> maskvec{datain09+datainAE+datainae};
		unsigned digits{vector_mask_countr_one(maskvec)};
		if(digits!=16)
		{
			return first+digits;
		}
	}
	return skip_digits<16>(first,last);
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__hot__)
[[__gnu__::__hot__]]
#endif
inline parse_result<char_type const*> sto_hex_simd16_impl(char_type const* buffer,char_type const* buffer_end,std::uint_least64_t &res) noexcept
{
	constexpr char8_t zero_constant{static_cast<char8_t>(char_literal_v<u8'0',char_type>)};

	constexpr char8_t A_constant{static_cast<char8_t>(char_literal_v<u8'A',char_type>)};

	constexpr char8_t a_constant{static_cast<char8_t>(char_literal_v<u8'a',char_type>)};
	
	
	using ::fast_io::intrinsics::simd_vector;

	simd_vector<char unsigned,16> data16;
	data16.load(buffer);

	constexpr simd_vector<char unsigned,16> zerovec{create_simd16_vector(zero_constant)};
	constexpr simd_vector<char unsigned,16> Avec{create_simd16_vector(A_constant)};
	constexpr simd_vector<char unsigned,16> avec{create_simd16_vector(a_constant)};
	

	constexpr simd_vector<char unsigned,16> tenvec{create_simd16_vector(10)};
	constexpr simd_vector<char unsigned,16> sixvec{create_simd16_vector(6)};
	
	simd_vector<char unsigned,16> datamzero{data16-zerovec};
	simd_vector<char unsigned,16> datamA{data16-Avec};
	simd_vector<char unsigned,16> datama{data16-avec};
	simd_vector<char unsigned,16> datain09{datamzero<tenvec};
	simd_vector<char unsigned,16> datainAE{datamA<sixvec};
	simd_vector<char unsigned,16> datainae{datama<sixvec};
	datamA+=tenvec;
	datama+=tenvec;
	simd_vector<char unsigned,16> resdata{(datamzero&datain09)+(datamA&datainAE)+(datama&datainae)};

	simd_vector<char unsigned,16> maskvec{datain09+datainAE+datainae};
	
	unsigned digits{vector_mask_countr_one(maskvec)};
#if 0
	if(digits==0)
	{
		return {buffer,parse_code::invalid};
	}
#endif
	simd_vector<char unsigned,16> shiftingvec;
	shiftingvec.load(simd16_shift_table+digits);
	using namespace fast_io::intrinsics;
#if 0
	simd_vector<char unsigned,16> shifted{(typename simd_vector<char unsigned,16>::vec_type)__builtin_ia32_pshufb128((x86_64_v16qi)resdata.value,(x86_64_v16qi)shiftingvec.value)};
	simd_vector<std::uint_least16_t,8> shifted1;
	{
		simd_vector<std::uint_least8_t,16> v1{
			__builtin_shufflevector(shifted.value,shifted.value,
			15,15,13,13,
			11,11,9,9,
			7,7,5,5,
			3,3,1,1)};
		simd_vector<std::uint_least8_t,16> v2{
			__builtin_shufflevector(shifted.value,shifted.value,
			14,14,12,12,
			10,10,8,8,
			6,6,4,4,
			2,2,0,0)};
		constexpr simd_vector<std::uint_least8_t,16> shiftv2
		{4,0,4,0,
		4,0,4,0,
		4,0,4,0,
		4,0,4,0};
		v2<<=shiftv2;
		v1+=v2;
		constexpr simd_vector<std::uint_least8_t,16> maskv1
		{0xFF,0,0xFF,0,
		0xFF,0,0xFF,0,
		0xFF,0,0xFF,0,
		0xFF,0,0xFF,0};
		v1&=maskv1;
		shifted1=static_cast<simd_vector<std::uint_least16_t,8>>(v1);
	}
	simd_vector<std::uint_least32_t,4> shifted2;
	{
		simd_vector<std::uint_least16_t,8> v1{
			__builtin_shufflevector(shifted1.value,shifted1.value,
			0,0,2,2,4,4,6,6)};
		simd_vector<std::uint_least16_t,8> v2{
			__builtin_shufflevector(shifted1.value,shifted1.value,
			1,1,3,3,5,5,7,7)};
		constexpr simd_vector<std::uint_least16_t,8> shiftv2
		{8,0,8,0,
		8,0,8,0};
		v2<<=shiftv2;
		v2+=v1;
		constexpr simd_vector<std::uint_least16_t,8> maskv2
		{0xFFFF,0,0xFFFF,0,
		0xFFFF,0,0xFFFF,0};
		v2&=maskv2;
		shifted2=static_cast<simd_vector<std::uint_least32_t,4>>(v2);
	}
	simd_vector<std::uint_least64_t,2> shifted3;
	{
		simd_vector<std::uint_least32_t,4> v1{
			__builtin_shufflevector(shifted2.value,shifted2.value,
			0,0,2,2)};
		simd_vector<std::uint_least32_t,4> v2{
			__builtin_shufflevector(shifted2.value,shifted2.value,
			1,1,3,3)};
		constexpr simd_vector<std::uint_least32_t,4> shiftv2
		{16,0,16,0};
		v2<<=shiftv2;
		v2+=v1;
		constexpr simd_vector<std::uint_least32_t,4> maskv2
		{0xFFFFFFFF,0,0xFFFFFFFF,0};
		v2&=maskv2;
		shifted3=static_cast<simd_vector<std::uint_least64_t,2>>(v2);
	}
	res=(shifted3.back()<<32u)+shifted3.front();
#else
	res=0;
#endif
	if(digits==16)[[unlikely]]
	{
		auto first{buffer+16};
		auto none_digits_it{skip_hexdigits(first,buffer_end)};
		if(first!=none_digits_it)
		{
			return {none_digits_it,parse_code::overflow};
		}
	}
	return {buffer+digits,parse_code::ok};
}
