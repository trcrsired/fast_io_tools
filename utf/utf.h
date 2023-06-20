#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::std::uint_least32_t utf8masks[3]
{
0b11100000'11000000'00000000'00000000u,
0b11110000'11000000'11000000'00000000u,
0b11111000'11000000'11000000'11000000u
};

inline constexpr ::std::uint_least32_t utf8maskscond[3]
{
0b11000000'10000000'00000000'00000000u,
0b11100000'10000000'10000000'00000000u,
0b11110000'10000000'10000000'10000000u
};

inline constexpr ::std::uint_least32_t utf8masks2[3]
{
0b00011111'00111111'00000000'00000000u,
0b00001111'00111111'00111111'00000000u,
0b00000111'00111111'00111111'00111111u,
};


inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_simd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst,char32_t *tolast) noexcept
{
	constexpr
		::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> cmp128{
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80};
	constexpr
		::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> zeros{};
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> simvec;
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> ret;
	for(;20<fromlast-fromfirst&&
		16<tolast-tofirst;)
	{
		simvec.load(fromfirst);

		auto res{(simvec&cmp128)==cmp128};
		if(is_all_zeros(res))
		{
			ret.value=__builtin_shufflevector(simvec.value,zeros.value,0,16,16,16,1,16,16,16,
				2,16,16,16,3,16,16,16);
			ret.store(tofirst);
			ret.value=__builtin_shufflevector(simvec.value,zeros.value,4,16,16,16,5,16,16,16,
				6,16,16,16,7,16,16,16);
			ret.store(tofirst+4);
			ret.value=__builtin_shufflevector(simvec.value,zeros.value,8,16,16,16,9,16,16,16,
				10,16,16,16,11,16,16,16);
			ret.store(tofirst+8);
			ret.value=__builtin_shufflevector(simvec.value,zeros.value,12,16,16,16,13,16,16,16,
				14,16,16,16,15,16,16,16);
			ret.store(tofirst+12);
			fromfirst+=16;
			tofirst+=16;
			continue;
		}
		unsigned czv{vector_mask_countr_zero(res)};
#if __has_cpp_attribute(assume)
		[[assume(czv<16)]];
#endif
		{
			unsigned i{};
			if(3<czv)
			{
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,0,16,16,16,1,16,16,16,
					2,16,16,16,3,16,16,16);
				ret.store(tofirst);
				i=4;
			}
			if(7<czv)
			{
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,4,16,16,16,5,16,16,16,
					6,16,16,16,7,16,16,16);
				ret.store(tofirst+4);
				i=8;
			}
			if(11<czv)
			{
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,8,16,16,16,9,16,16,16,
					10,16,16,16,11,16,16,16);
				ret.store(tofirst+8);
				i=12;
			}
#if __has_cpp_attribute(assume)
			[[assume(czv-i<4)]];
#endif
			for(;i!=czv;++i)
			{
				*tofirst=*fromfirst;
				++fromfirst;
				++tofirst;
			}
		}
		::std::uint_least32_t val;
		__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
		char unsigned v;
		if constexpr(::std::endian::little==::std::endian::native)
		{
			v=static_cast<char unsigned>(val);
			val=::std::byteswap(val);
		}
		else
		{
			v=static_cast<char unsigned>(val>>24u);
		}
		int length{::std::countl_one(v)};
		if(4<length)
		{
			*tofirst=0xFFFD;
			++fromfirst;
			++tofirst;
			continue;
		}
		auto lengthm2{length-2};
		::std::uint_least32_t mask{utf8masks[lengthm2]};
		::std::uint_least32_t maskcond{utf8maskscond[lengthm2]};
		if((val&mask)!=maskcond)
		{
			*tofirst=0xFFFD;
			fromfirst+=length;
			++tofirst;
			continue;
		}
		val&=utf8masks2[lengthm2];
		val>>=(static_cast<unsigned>(2-lengthm2)<<3);
		*tofirst=(val&0xFF)|
			((val&0xFF00)>>2)|
			((val&0xFF0000)>>4)|
			((val&0xFF000000)>>6);
		fromfirst+=length;
		++tofirst;
	}
	return {fromfirst,tofirst};
}

inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst,char32_t *tolast) noexcept
{
//	::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
	
	return ::fast_io::details::utf8_to_utf32_simd_impl(fromfirst,fromlast,tofirst,tolast);
}


}

}
