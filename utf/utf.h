#pragma once

namespace fast_io
{

namespace details
{

struct utf8mask
{
::std::uint_least32_t mask1,mask2,mask3;
};

inline constexpr utf8mask utf8masks[3]
{
{0b11100000'11000000'00000000'00000000u,0b11000000'10000000'00000000'00000000u,0b00011111'00111111'00000000'00000000u},
{0b11110000'11000000'11000000'00000000u,0b11100000'10000000'10000000'00000000u,0b00001111'00111111'00111111'00000000u},
{0b11111000'11000000'11000000'11000000u,0b11110000'10000000'10000000'10000000u,0b00000111'00111111'00111111'00111111u},
};

inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_nosimd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst) noexcept
{
	for(;fromfirst<fromlast;)
	{
		::std::uint_least32_t val;
		__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
		constexpr
			::std::uint_least32_t firstdigitmask{::std::endian::big==::std::endian::native?0x80000000u:0x00000080u};
		if(!(firstdigitmask&val))
		{
			::std::uint_least32_t valmask{val&0x80808080u};
			if(!valmask)
			{
				::std::uint_least32_t low{val&0xFFFFu};
				::std::uint_least32_t high{val>>16u};
				if constexpr(::std::endian::big==::std::endian::native)
				{
					*tofirst=(high&0xFF);
					tofirst[1]=(high>>8u);
					tofirst[2]=(low&0xFF);
					tofirst[3]=(low>>8u);
				}
				else
				{
					*tofirst=(low&0xFF);
					tofirst[1]=(low>>8u);
					tofirst[2]=(high&0xFF);
					tofirst[3]=(high>>8u);
				}
				fromfirst+=4;
				tofirst+=4;
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
				bool nmod{(fromdiff&0x3u)!=0u};
				::std::size_t ndiff{(fromdiff>>2)+nmod};
				do
				{
					__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
					if((val&0x80808080u)!=0u)
					{
						break;
					}
					low=val&0xFFFFu;
					high=val>>16u;
					if constexpr(::std::endian::big==::std::endian::native)
					{
						*tofirst=(high&0xFF);
						tofirst[1]=(high>>8u);
						tofirst[2]=(low&0xFF);
						tofirst[3]=(low>>8u);
					}
					else
					{
						*tofirst=(low&0xFF);
						tofirst[1]=(low>>8u);
						tofirst[2]=(high&0xFF);
						tofirst[3]=(high>>8u);
					}
					fromfirst+=4;
					tofirst+=4;
					--ndiff;
				}
				while(ndiff);
				if(!ndiff)
				{
					break;
				}
			}
			if constexpr(::std::endian::big==::std::endian::native)
			{
				val=::fast_io::byte_swap(val);
			}
			for(;!(val&0x80u);++tofirst)
			{
				*tofirst=val&0xFFu;
				++fromfirst;
				val>>=8u;
			}
			__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
		}
		char unsigned v;
		if constexpr(::std::endian::little==::std::endian::native)
		{
			v=static_cast<char unsigned>(val);
			val=::fast_io::byte_swap(val);
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
		auto [mask1,mask2,mask3]=utf8masks[lengthm2];
		if((val&mask1)==mask2)
		{
			val&=mask3;
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

template<::std::size_t N>
inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_simd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst) noexcept
{
	if constexpr(N!=16&&N!=32&&N!=64)
	{
		return utf8_to_utf32_nosimd_impl(fromfirst,fromlast,tofirst);
	}
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<::std::uint_least8_t,N>;
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
		::fast_io::intrinsics::simd_vector<::std::uint_least8_t,N> cmp128{
		std::bit_cast<simd_vector_type>(characters_array_impl<0x80u,char8_t,N>)};
#else
	simd_vector_type cmp128;
	cmp128.load(characters_array_impl<0x80u,char8_t,N>.data());
#endif
	constexpr
		simd_vector_type zeros{};
	simd_vector_type simvec;
	simd_vector_type ret;
	simd_vector_type res;
	for(;fromfirst<fromlast;)
	{
		::std::uint_least32_t val;
		__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
		constexpr
			::std::uint_least32_t firstdigitmask{::std::endian::big==::std::endian::native?0x80000000u:0x00000080u};
		if(!(firstdigitmask&val))
		{
			::std::uint_least32_t valmask{val&0x80808080u};
			if(!valmask)
			{
				::std::uint_least32_t low{val&0xFFFFu};
				::std::uint_least32_t high{val>>16u};
				if constexpr(::std::endian::big==::std::endian::native)
				{
					*tofirst=(high&0xFF);
					tofirst[1]=(high>>8u);
					tofirst[2]=(low&0xFF);
					tofirst[3]=(low>>8u);
				}
				else
				{
					*tofirst=(low&0xFF);
					tofirst[1]=(low>>8u);
					tofirst[2]=(high&0xFF);
					tofirst[3]=(high>>8u);
				}
				fromfirst+=4;
				tofirst+=4;
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t ndiff{(fromdiff>>4)+((fromdiff&0xFu)!=0u)};
				do
				{
					simvec.load(fromfirst);
					res=((simvec&cmp128)==cmp128);
					if(!is_all_zeros(res))
					{
						break;
					}
					if constexpr(N==16)
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
					}
					else if constexpr(N==32)
					{
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,0,32,32,32,1,32,32,32,2,32,32,32,3,32,32,32,4,32,32,32,5,32,32,32,6,32,32,32,7,32,32,32);
						ret.store(tofirst);
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,8,32,32,32,9,32,32,32,10,32,32,32,11,32,32,32,12,32,32,32,13,32,32,32,14,32,32,32,15,32,32,32);
						ret.store(tofirst+8);
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,16,32,32,32,17,32,32,32,18,32,32,32,19,32,32,32,20,32,32,32,21,32,32,32,22,32,32,32,23,32,32,32);
						ret.store(tofirst+16);
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,24,32,32,32,25,32,32,32,26,32,32,32,27,32,32,32,28,32,32,32,29,32,32,32,30,32,32,32,31,32,32,32);
						ret.store(tofirst+24);
					}
					else if constexpr(N==64)
					{
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,0,64,64,64,1,64,64,64,2,64,64,64,3,64,64,64,4,64,64,64,5,64,64,64,6,64,64,64,7,64,64,64,8,64,64,64,9,64,64,64,10,64,64,64,11,64,64,64,12,64,64,64,13,64,64,64,14,64,64,64,15,64,64,64);
						ret.store(tofirst);
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,16,64,64,64,17,64,64,64,18,64,64,64,19,64,64,64,20,64,64,64,21,64,64,64,22,64,64,64,23,64,64,64,24,64,64,64,25,64,64,64,26,64,64,64,27,64,64,64,28,64,64,64,29,64,64,64,30,64,64,64,31,64,64,64);
						ret.store(tofirst+16);
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,32,64,64,64,33,64,64,64,34,64,64,64,35,64,64,64,36,64,64,64,37,64,64,64,38,64,64,64,39,64,64,64,40,64,64,64,41,64,64,64,42,64,64,64,43,64,64,64,44,64,64,64,45,64,64,64,46,64,64,64,47,64,64,64);
						ret.store(tofirst+32);
						ret.value=__builtin_shufflevector(simvec.value,zeros.value,48,64,64,64,49,64,64,64,50,64,64,64,51,64,64,64,52,64,64,64,53,64,64,64,54,64,64,64,55,64,64,64,56,64,64,64,57,64,64,64,58,64,64,64,59,64,64,64,60,64,64,64,61,64,64,64,62,64,64,64,63,64,64,64);
						ret.store(tofirst+48);
					}
					fromfirst+=N;
					tofirst+=N;
					--ndiff;
				}
				while(ndiff);
				if(!ndiff)
				{
					break;
				}
				fromdiff=static_cast<::std::size_t>(fromlast-fromfirst);
				ndiff=((fromdiff>>2)+((fromdiff&0x3u)!=0u));
				do
				{
					__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
					if((val&0x80808080u)!=0u)
					{
						break;
					}
					low=val&0xFFFFu;
					high=val>>16u;
					if constexpr(::std::endian::big==::std::endian::native)
					{
						*tofirst=(high&0xFF);
						tofirst[1]=(high>>8u);
						tofirst[2]=(low&0xFF);
						tofirst[3]=(low>>8u);
					}
					else
					{
						*tofirst=(low&0xFF);
						tofirst[1]=(low>>8u);
						tofirst[2]=(high&0xFF);
						tofirst[3]=(high>>8u);
					}
					fromfirst+=4;
					tofirst+=4;
					--ndiff;
				}
				while(ndiff);
				if(!ndiff)
				{
					break;
				}
			}
			if constexpr(::std::endian::big==::std::endian::native)
			{
				val=::fast_io::byte_swap(val);
			}
			for(;!(val&0x80u);++tofirst)
			{
				*tofirst=val&0xFFu;
				++fromfirst;
				val>>=8u;
			}
			__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
		}
		char unsigned v;
		if constexpr(::std::endian::little==::std::endian::native)
		{
			v=static_cast<char unsigned>(val);
			val=::fast_io::byte_swap(val);
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
		auto [mask1,mask2,mask3]=utf8masks[lengthm2];
		if((val&mask1)==mask2)
		{
			val&=mask3;
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
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if(!__builtin_is_constant_evaluated())
#endif
	{
		::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
		::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};

		::std::size_t mndiff{todiff};
		if(fromdiff<mndiff)
		{
			mndiff=fromdiff;
		}
		constexpr std::size_t N{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
		constexpr
			::std::uint_least32_t decisiondiff{N+8};
		if(decisiondiff<mndiff)
		{
			if constexpr(16<=N)
			{
				auto [fromit,toit]=utf8_to_utf32_simd_impl<N>(fromfirst,fromlast,tofirst);
				fromfirst=fromit;
				tofirst=toit;
			}
			else
			{
				auto [fromit,toit]=utf8_to_utf32_nosimd_impl(fromfirst,fromlast,tofirst);
				fromfirst=fromit;
				tofirst=toit;
			}
		}	
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
