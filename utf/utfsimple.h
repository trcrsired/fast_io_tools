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
#if 0
template<typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_to_other_nosimd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst) noexcept
{
	using output_char_type = typename T::output_char_type;
	while(fromfirst<fromlast)
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
				fromfirst+=sizeof(val);
				tofirst+=sizeof(val);
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
					fromfirst+=sizeof(val);
					tofirst+=sizeof(val);
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
			++fromfirst;
			if constexpr(::std::same_as<output_char_type,char32_t>)
			{
				*tofirst=0xFFFD;
				++tofirst;
			}
			else
			{
				tofirst=T::get_invalid_code_point(tofirst);
			}
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
		if constexpr(::std::same_as<output_char_type,char32_t>)
		{
			*tofirst=val;
			++tofirst;
		}
		else
		{
			tofirst=T::get_code_point(tofirst,val);
		}
		fromfirst+=length;
	}
	return {fromfirst,tofirst};
}
#endif

template<typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_to_other_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	using output_char_type = typename T::output_char_type;
#if 0
	if constexpr(::std::numeric_limits<::std::uint_least8_t>::digits==8)
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
			mndiff-=decisiondiff;
#if 0
			if constexpr(16<=N)
			{
				auto [fromit,toit]=utf8_to_utf32_simd_impl<N>(fromfirst,fromfirst+mndiff,tofirst);
				fromfirst=fromit;
				tofirst=toit;
			}
			else
#endif
			{
				auto [fromit,toit]=utf8_to_other_nosimd_impl<T>(fromfirst,fromfirst+mndiff,tofirst);
				fromfirst=fromit;
				tofirst=toit;
			}
		}	
	}
	}
#endif
	if constexpr(::std::same_as<output_char_type,char32_t>)
	{
		for(;fromfirst!=fromlast&&tofirst!=tolast;++tofirst)
		{
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
	}
	else
	{
		constexpr
			::std::size_t
			invalidcodepointslen{T::invalid_code_points_len};
		constexpr
			::std::size_t invdcpm1{invalidcodepointslen-1u};
		constexpr
			::std::size_t
			mxcodepointslen{T::max_code_points_len};
		for(;fromfirst!=fromlast&&tofirst!=tolast;)
		{
			char8_t v0{*fromfirst};
			if(v0<0x80u)
			{
				*tofirst=v0;
				++tofirst;
				++fromfirst;
				continue;
			}
			int length{::std::countl_one(static_cast<char unsigned>(v0))};
			if(length==1||4<length)
			{
				++fromfirst;
				if constexpr(1<invalidcodepointslen)
				{
					::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
					if(todiff<invdcpm1)
					{
						break;
					}
				}
				tofirst=T::get_invalid_code_points(tofirst);
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
					fromfirst=fromit;
					if constexpr(1<invalidcodepointslen)
					{
						::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
						if(todiff<invdcpm1)
						{
							break;
						}
					}
					tofirst=T::get_invalid_code_points(tofirst);
					continue;
				}
				break;
			}
			auto fmfirst{fromfirst};
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
				if constexpr(1<invalidcodepointslen)
				{
					::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
					if(todiff<invdcpm1)
					{
						break;
					}
				}
				tofirst=T::get_invalid_code_points(tofirst);
			}
			else
			{
				auto tofrst{T::get_code_points(tofirst,tolast,val)};
				if(tofrst==tofirst)
				{
					fromfirst=fmfirst;
					break;
				}
			}
		}
	}
	return {fromfirst,tofirst};
}

}

}
