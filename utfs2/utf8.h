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

#if !defined(_MSC_VER) || defined(__clang__)

template<typename T,::std::size_t N>
inline deco_result_simd<char8_t,typename T::output_char_type> utf8_simd_case_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	using output_char_type = typename T::output_char_type;
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<::std::uint_least8_t,N>;
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
#endif
		auto cmp128{::fast_io::details::utfconstantsimd_impl<0x80u,char8_t,N,false>()};
	constexpr
		simd_vector_type zeros{};
	simd_vector_type simvec,ret;
	constexpr
		::std::size_t ndiffmask{static_cast<::std::size_t>(N-1u)};
	constexpr
		int ndiffshift{::std::bit_width(ndiffmask)};
	::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
	::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
	if(todiff<fromdiff)
	{
		fromdiff=todiff;
	}
	::std::size_t ndiff{(fromdiff>>ndiffshift)};
	for(;ndiff;--ndiff)
	{
		simvec.load(fromfirst);
		if(!::fast_io::intrinsics::is_all_zeros((simvec&cmp128)==cmp128))
		{
			break;
		}
		if constexpr(sizeof(output_char_type)==4)
		{
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
		}
		else if constexpr(sizeof(output_char_type)==2)
		{
			if constexpr(N==16)
			{
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,
					0,16,1,16,2,16,3,16,
					4,16,5,16,6,16,7,16);
				ret.store(tofirst);
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,
					8,16,9,16,10,16,11,16,
					12,16,13,16,14,16,15,16);
				ret.store(tofirst+8);
			}
			else if constexpr(N==32)
			{
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,
					0,32,1,32,2,32,3,32,4,32,5,32,6,32,7,32,
					8,32,9,32,10,32,11,32,12,32,13,32,14,32,15,32);
				ret.store(tofirst);
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,
					16,32,17,32,18,32,19,32,20,32,21,32,22,32,23,32,
					24,32,25,32,26,32,27,32,28,32,29,32,30,32,31,32);
				ret.store(tofirst+16);
			}
			else if constexpr(N==64)
			{
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,
					0,64,1,64,2,64,3,64,4,64,5,64,6,64,7,64,
					8,64,9,64,10,64,11,64,12,64,13,64,14,64,15,64,
					16,64,17,64,18,64,19,64,20,64,21,64,22,64,23,64,
					24,64,25,64,26,64,27,64,28,64,29,64,30,64,31,64);
				ret.store(tofirst);
				ret.value=__builtin_shufflevector(simvec.value,zeros.value,
					32,64,33,64,34,64,35,64,36,64,37,64,38,64,39,64,
					40,64,41,64,42,64,43,64,44,64,45,64,46,64,47,64,
					48,64,49,64,50,64,51,64,52,64,53,64,54,64,55,64,
					56,64,57,64,58,64,59,64,60,64,61,64,62,64,63,64);
				ret.store(tofirst+32);
			}
		}
		else if constexpr(sizeof(output_char_type)==1)
		{
			simvec.store(tofirst);
		}
		fromfirst+=N;
		tofirst+=N;
	}
	return {fromfirst,tofirst,ndiff==0u};
}

#endif

template<typename T>
inline deco_result<char8_t,typename T::output_char_type> utf8_generic_unchecked_impl(
	char8_t const *fromfirst,typename T::output_char_type *tofirst,::std::size_t fromn) noexcept
{
	char8_t const *fromlast{fromfirst+fromn};
	typename T::output_char_type *tolast{tofirst+fromn};
	using output_char_type = typename T::output_char_type;
	constexpr
		::std::size_t
		invalidcodepointslen{T::invalid_code_points_len};
	constexpr
		bool ecisebcdic{T::encoding_is_ebcdic};
	using output_char_type = typename T::output_char_type;
	while(fromfirst<fromlast&&tofirst<tolast)
	{
		::std::uint_least32_t val;
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy
#else
		::std::memcpy
#endif
#else
		::std::memcpy
#endif
		(__builtin_addressof(val),fromfirst,sizeof(val));
		constexpr
			::std::uint_least32_t firstdigitmask{::std::endian::big==::std::endian::native?0x80000000u:0x00000080u};
		if(!(firstdigitmask&val))
		{
			::std::uint_least32_t valmask{val&0x80808080u};
			if(!valmask)
			{
				::std::uint_least32_t low{val&0xFFFFu};
				::std::uint_least32_t high{val>>16u};
				if constexpr(ecisebcdic)
				{
					if constexpr(::std::endian::big==::std::endian::native)
					{
						*tofirst=::fast_io::details::bm_i8_to_ebcdic[(high&0xFF)];
						tofirst[1]=::fast_io::details::bm_i8_to_ebcdic[(high>>8u)];
						tofirst[2]=::fast_io::details::bm_i8_to_ebcdic[(low&0xFF)];
						tofirst[3]=::fast_io::details::bm_i8_to_ebcdic[(low>>8u)];
					}
					else
					{
						*tofirst=::fast_io::details::bm_i8_to_ebcdic[(low&0xFF)];
						tofirst[1]=::fast_io::details::bm_i8_to_ebcdic[(low>>8u)];
						tofirst[2]=::fast_io::details::bm_i8_to_ebcdic[(high&0xFF)];
						tofirst[3]=::fast_io::details::bm_i8_to_ebcdic[(high>>8u)];
					}						
				}
				else if constexpr(sizeof(output_char_type)==1u)
				{
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy)
					__builtin_memcpy
#else
					::std::memcpy
#endif
#else
					::std::memcpy
#endif
					(tofirst,__builtin_addressof(val),sizeof(val));
				}
				else
				{
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
				}
				fromfirst+=sizeof(val);
				tofirst+=sizeof(val);
#if !defined(_MSC_VER) || defined(__clang__)
				constexpr
				::std::size_t N{::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size};
				if constexpr(N!=0&&!ecisebcdic)
				{
					auto [fromit,toit,done]=utf8_simd_case_impl<T,N>(fromfirst,fromlast,tofirst,tolast);
					fromfirst=fromit;
					tofirst=toit;
					if(done)
					{
						break;
					}
				}
#endif
				::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff2{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff2<fromdiff2)
				{
					fromdiff2=todiff2;
				}
				::std::size_t ndiff2{(fromdiff2>>2u)};
				do
				{
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy)
					__builtin_memcpy
#else
					::std::memcpy
#endif
#else
					::std::memcpy
#endif
					(__builtin_addressof(val),fromfirst,sizeof(val));
					if((val&0x80808080u)!=0u)
					{
						break;
					}
					::std::uint_least32_t low{val&0xFFFFu};
					::std::uint_least32_t high{val>>16u};
					if constexpr(ecisebcdic)
					{
						if constexpr(::std::endian::big==::std::endian::native)
						{
							*tofirst=::fast_io::details::bm_i8_to_ebcdic[(high&0xFF)];
							tofirst[1]=::fast_io::details::bm_i8_to_ebcdic[(high>>8u)];
							tofirst[2]=::fast_io::details::bm_i8_to_ebcdic[(low&0xFF)];
							tofirst[3]=::fast_io::details::bm_i8_to_ebcdic[(low>>8u)];
						}
						else
						{
							*tofirst=::fast_io::details::bm_i8_to_ebcdic[(low&0xFF)];
							tofirst[1]=::fast_io::details::bm_i8_to_ebcdic[(low>>8u)];
							tofirst[2]=::fast_io::details::bm_i8_to_ebcdic[(high&0xFF)];
							tofirst[3]=::fast_io::details::bm_i8_to_ebcdic[(high>>8u)];
						}						
					}
					else if constexpr(sizeof(output_char_type)==1u)
					{
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy)
						__builtin_memcpy
#else
						::std::memcpy
#endif
#else
						::std::memcpy
#endif
						(tofirst,__builtin_addressof(val),sizeof(val));
					}
					else
					{
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
					}
					fromfirst+=sizeof(val);
					tofirst+=sizeof(val);
					--ndiff2;
				}
				while(ndiff2);
				if(!ndiff2)
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
				if constexpr(ecisebcdic)
				{
					*tofirst=::fast_io::details::bm_i8_to_ebcdic[val&0xFFu];
				}
				else
				{
					*tofirst=val&0xFFu;
				}
				++fromfirst;
				val>>=8u;
			}
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy)
			__builtin_memcpy
#else
			::std::memcpy
#endif
#else
			::std::memcpy
#endif
			(__builtin_addressof(val),fromfirst,sizeof(val));
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
			if constexpr(::std::same_as<output_char_type,char32_t>)
			{
				*tofirst=0xFFFD;
				++tofirst;
			}
			else
			{
				T::get_invalid_code_points(tofirst);
				tofirst+=invalidcodepointslen;
			}
			++fromfirst;
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
			auto ret=T::get_code_point_unchecked(tofirst,val);
			tofirst+=ret;
		}
		fromfirst+=length;
	}
	return {fromfirst,tofirst};
}

template<typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_generic_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	constexpr bool ecisebcdic{T::encoding_is_ebcdic};
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
		constexpr std::size_t N{
#if !defined(_MSC_VER) || defined(__clang__)
			ecisebcdic?::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size:0u
#endif
		};
		constexpr
			::std::uint_least32_t decisiondiff{N+8};
		if(decisiondiff<mndiff)
		{
			mndiff-=decisiondiff;
			auto [fromit,toit]=utf8_generic_unchecked_impl<T>(fromfirst,tofirst,mndiff);
			fromfirst=fromit;
			tofirst=toit;
		}
	}
	}
	constexpr
		::std::size_t
		invalidcodepointslen{T::invalid_code_points_len};
	constexpr
		::std::size_t invdcpm1{invalidcodepointslen-1u};
	constexpr
		::std::size_t
		mxcodepointslen{T::max_code_points_len};
	constexpr
		auto encode{T::encode};
	for(;fromfirst!=fromlast&&tofirst!=tolast;++tofirst)
	{
		char8_t v0{*fromfirst};
		if(v0<0x80u)
		{
			if constexpr(ecisebcdic)
			{
				*tofirst=::fast_io::details::bm_i8_to_ebcdic[v0];
			}
			else
			{
				*tofirst=v0;
			}
			++fromfirst;
			continue;
		}
		int length{::std::countl_one(static_cast<char unsigned>(v0))};
		if(length==1||4<length)
		{
			if constexpr(invalidcodepointslen==1)
			{
				T::get_invalid_code_points(tofirst);
			}
			else
			{
				::std::size_t diff{static_cast<::std::size_t>(tolast-tofirst)};
				if(diff < length)
				{
					break;
				}
				T::get_invalid_code_points(tofirst);
				tofirst+=invdcpm1;
			}
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
				if constexpr(invalidcodepointslen==1)
				{
					T::get_invalid_code_points(tofirst);
				}
				else
				{
					::std::size_t diff{static_cast<::std::size_t>(tolast-tofirst)};
					if(diff < length)
					{
						break;
					}
					T::get_invalid_code_points(tofirst);
					tofirst+=invdcpm1;
				}
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
		if constexpr(T::encode==::fast_io::manipulators::encoding::utf32)
		{
			*tofirst=val;
		}
		else if constexpr(T::max_code_points_len==1)
		{
			T::get_code_point_unchecked(tofirst,val);
		}
		else
		{
			auto ret=T::get_code_point(tofirst,static_cast<::std::size_t>(tolast-tofirst),val);
			if(ret==0)
			{
				break;
			}
			tofirst+=ret-1;
		}
	}
	return {fromfirst,tofirst};
}

}

}
