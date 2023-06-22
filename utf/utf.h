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

[[__gnu__::__noinline__]]
inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_simd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst) noexcept
{
	constexpr
		::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> cmp128{
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80};
	constexpr
		::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> zeros{};
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> simvec;
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> ret;
	for(;fromfirst<fromlast;)
	{
		char32_t vf0{*fromfirst};
		if(vf0<0x80)
		{
			simvec.load(fromfirst);
			auto res{(simvec&cmp128)==cmp128};
			if(is_all_zeros(res))
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
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
			for(unsigned i{};i!=czv;++i)
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
		if(length==1||4<length)
		{
			*tofirst=0xFFFD;
			++fromfirst;
			++tofirst;
			continue;
		}
		auto lengthm2{length-2};

		::std::uint_least32_t mask{utf8masks[lengthm2]};
		::std::uint_least32_t maskcond{utf8maskscond[lengthm2]};
		if((val&mask)==maskcond)
		{
			val&=utf8masks2[lengthm2];
			val>>=(static_cast<unsigned>(2-lengthm2)<<3);
			val=(val&0xFF)|
				((val&0xFF00)>>2)|
				((val&0xFF0000)>>4)|
				((val&0xFF000000)>>6);
		}
		else
		{
			val=0xFFFD;
		}
		*tofirst=val;
		++tofirst;
		fromfirst+=length;
	}
	return {fromfirst,tofirst};
}

inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst,char32_t *tolast) noexcept
{
	::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
	::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};

	::std::size_t mndiff{todiff};
	if(fromdiff<mndiff)
	{
		mndiff=fromdiff;
	}
	if(20<mndiff)
	{
		auto [fromit,toit]=utf8_to_utf32_simd_impl(fromfirst,fromlast,tofirst);
		fromfirst=fromit;
		tofirst=toit;
	}
	for(;fromfirst!=fromlast&&tofirst!=tolast;++tofirst)
	{
		using namespace fast_io::mnp;
		
		char8_t v0{*fromfirst};
		if(v0<0x80u)
		{
			*tofirst=v0;
			++fromfirst;
			continue;
		}
		int length{::std::countl_one(static_cast<char unsigned>(v0))};
		if(length==1||4<length)
		{
			*tofirst=0xFFFD;
			++fromfirst;
			continue;
		}
		int lengthm1{length-1};
		if((fromlast-fromfirst)<length)
#if __has_cpp_attribute(unlikely)
		[[unlikely]]
#endif
		{
			auto fromit{fromfirst};
			for(++fromit;lengthm1&&((*fromit&0b11000000)==0b10000000);--lengthm1)
			{
				++fromit;
			}
			if(lengthm1)
			{
				*tofirst=0xFFFD;
				fromfirst=fromit;
				continue;
			}
			break;
		}
		char32_t val{v0&(0b11111111u>>length)};//length and length-1 should be the same here
		auto ed{fromfirst+length};
		for(++fromfirst;lengthm1;--lengthm1)
		{
			char8_t vff{*fromfirst};
			if((vff&0b11000000)==0b10000000)
			{
				vff&=0b00111111;
			}
			else
#if __has_cpp_attribute(unlikely)
			[[unlikely]]
#endif
			{
				break;
			}
			val=(val<<6)|vff;
			++fromfirst;
		}
		if(lengthm1)
		{
			val=0xFFFD;
		}
		*tofirst=val;
	}
	return {fromfirst,tofirst};
}

}

}
