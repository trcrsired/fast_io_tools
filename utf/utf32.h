#pragma once

namespace fast_io::details
{

inline constexpr ::std::size_t utf32_to_utf8_code_point_impl(char8_t *dst, ::std::size_t szdpt, char32_t cdpt) noexcept
{
	if (cdpt <= 0x7FF)
	{
		if(szdpt<2)
		{
			return 0;
		}
		*dst = static_cast<char8_t>(0xC0 | ((cdpt >> 6) & 0x1F));
		dst[1] = static_cast<char8_t>(0x80 | (cdpt        & 0x3F));
		return 2;
	}
	else if (cdpt <= 0xFFFF)
	{
		if(szdpt<3)
		{
			return 0;
		}
		*dst = static_cast<char8_t>(0xE0 | ((cdpt >> 12) & 0x0F));
		dst[1] = static_cast<char8_t>(0x80 | ((cdpt >> 6)  & 0x3F));
		dst[2] = static_cast<char8_t>(0x80 | (cdpt         & 0x3F));
		return 3;
	}
	else if (cdpt <= 0x10FFFF)
	{
		if(szdpt<3)
		{
			return 0;
		}
		*dst = static_cast<char8_t>(0xF0 | ((cdpt >> 18) & 0x07));
		dst[1] = static_cast<char8_t>(0x80 | ((cdpt >> 12) & 0x3F));
		dst[2] = static_cast<char8_t>(0x80 | ((cdpt >> 6)  & 0x3F));
		dst[3] = static_cast<char8_t>(0x80 | (cdpt         & 0x3F));
		return 4;
	}
	else
	{
		if(szdpt<3)
		{
			return 0;
		}
		*dst = static_cast<char8_t>(0xEF);
		dst[1] = static_cast<char8_t>(0xBF);
		dst[2] = static_cast<char8_t>(0xBD);
		return 3;
	}
}

template<::std::size_t N,typename T>
inline constexpr deco_result<char32_t,typename T::output_char_type> utf32_to_other_simd_tolast_impl(
	char32_t const *fromfirst,char32_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	using char_type = typename T::output_char_type;
	using simd_vector_type = ::fast_io::intrinsics::simd_vector<::std::uint_least8_t,N>;
	using temp_simd_vector_type = ::fast_io::intrinsics::simd_vector<::std::uint_least32_t,(N/sizeof(char32_t))>;
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
		temp_simd_vector_type cmp128{
		std::bit_cast<temp_simd_vector_type>(characters_array_impl<0x80u,char32_t,(N/sizeof(char32_t))>)};
#else
	temp_simd_vector_type cmp128;
	cmp128.load(characters_array_impl<0x80u,char32_t,(N/sizeof(char32_t))>.data());
#endif
	simd_vector_type svec0,svec1,svec2,svec3;
	while(fromfirst<fromlast&&tofirst<tolast)
	{
		char32_t v0{*fromfirst};
		if(v0<0x80u)
		{
			*tofirst=static_cast<char_type>(v0);
			++tofirst;
			++fromfirst;
			::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
			::std::size_t todiff2{static_cast<::std::size_t>(tolast-tofirst)};
			if(todiff2<fromdiff2)
			{
				fromdiff2=todiff2;
			}
			constexpr
				::std::size_t ndiffmask{static_cast<::std::size_t>(N-1u)};
			constexpr
				int ndiffshift{::std::bit_width(ndiffmask)};
			::std::size_t ndiff{(fromdiff2>>ndiffshift)};
			for(;ndiff;--ndiff)
			{
				if constexpr(N==16)
				{
					svec0.load(fromfirst);
					svec1.load(fromfirst+4);
					svec2.load(fromfirst+8);
					svec3.load(fromfirst+12);
				}
				else if constexpr(N==32)
				{
					svec0.load(fromfirst);
					svec1.load(fromfirst+8);
					svec2.load(fromfirst+16);
					svec3.load(fromfirst+24);
				}
				else
				{
					svec0.load(fromfirst);
					svec1.load(fromfirst+16);
					svec2.load(fromfirst+32);
					svec3.load(fromfirst+48);
				}
				if(!is_all_zeros((static_cast<temp_simd_vector_type>(svec0)<cmp128)|
					(static_cast<temp_simd_vector_type>(svec1)<cmp128)|
					(static_cast<temp_simd_vector_type>(svec2)<cmp128)|
					(static_cast<temp_simd_vector_type>(svec3)<cmp128)))
				{
					break;
				}
				if constexpr(N==16)
				{
					svec0.value=__builtin_shufflevector(svec0.value,svec1.value,
					0,4,8,12,16,20,24,28,-1,-1,-1,-1,-1,-1,-1,-1);
					svec2.value=__builtin_shufflevector(svec2.value,svec3.value,
					-1,-1,-1,-1,-1,-1,-1,-1,0,4,8,12,16,20,24,28);
					svec0.value=__builtin_shufflevector(svec0.value,svec2.value,
					0,1,2,3,4,5,6,7,24,25,26,27,28,29,30,31);
				}
				else if constexpr(N==32)
				{
					svec0.value=__builtin_shufflevector(svec0.value,svec1.value,
					0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
					-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
					svec2.value=__builtin_shufflevector(svec2.value,svec3.value,
					-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
					0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60);
					svec0.value=__builtin_shufflevector(svec0.value,svec2.value,
					0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
					48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63);
				}
				else if constexpr(N==64)
				{
					svec0.value=__builtin_shufflevector(svec0.value,svec1.value,
					0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
					64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,
					-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
					-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
					svec2.value=__builtin_shufflevector(svec2.value,svec3.value,
					-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
					-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
					0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
					64,68,72,76,80,84,88,92,96,100,104,108,112,116,120);
					svec0.value=__builtin_shufflevector(svec0.value,svec2.value,
					0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
					16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
					96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
					112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127);
				}
				svec0.store(tofirst);
				fromfirst+=N;
				tofirst+=N;
			}
			if(!ndiff)
			{
				break;
			}
			::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
			::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
			if(todiff<fromdiff)
			{
				fromdiff=todiff;
			}
			for(;fromdiff;--fromdiff)
			{
				v0=*fromfirst;
				if(0x80u<=v0)
				{
					break;
				}
				*tofirst=static_cast<char_type>(v0);
				++tofirst;
				++fromfirst;
			}
			if(!fromdiff)
			{
				break;
			}
		}
		auto ret{T::get_code_point(tofirst,static_cast<std::size_t>(tolast-tofirst),v0)};
		if(!ret)
		{
			break;
		}
		tofirst+=ret;
		++fromfirst;
	}
	return {fromfirst,tofirst};
}

template<typename T>
inline constexpr deco_result<char32_t,typename T::output_char_type> utf32_to_other_impl(
	char32_t const *fromfirst,char32_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	using char_type = typename T::output_char_type;
	if constexpr(::std::numeric_limits<::std::uint_least8_t>::digits==8)
	{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if(!__builtin_is_constant_evaluated())
#endif
	{
		constexpr std::size_t N{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
		if constexpr(N==16||N==32||N==64)
		{
			auto [fromit,toit]=utf32_to_other_simd_tolast_impl<N,T>(fromfirst,fromlast,tofirst,tolast);
			fromfirst=fromit;
			tofirst=toit;
		}
	}
	}
	while(fromfirst!=fromlast&&tofirst!=tolast)
	{
		char32_t v0{*fromfirst};
		if(v0<0x80u)
		{
			*tofirst=static_cast<char_type>(v0);
			++tofirst;
			++fromfirst;
			::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
			::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
			if(todiff<fromdiff)
			{
				fromdiff=todiff;
			}
			for(;fromdiff;--fromdiff)
			{
				v0=*fromfirst;
				if(0x80u<=v0)
				{
					break;
				}
				*tofirst=static_cast<char_type>(v0);
				++tofirst;
				++fromfirst;
			}
			if(!fromdiff)
			{
				break;
			}
		}
		auto ret{T::get_code_point(tofirst,static_cast<std::size_t>(tolast-tofirst),v0)};
		if(!ret)
		{
			break;
		}
		tofirst+=ret;
		++fromfirst;
	}
	return {fromfirst,tofirst};
}

struct utf32_to_utf8_simple
{
	using output_char_type = char8_t;
	static inline constexpr ::std::size_t invalid_code_points_len = 3;
	static inline constexpr ::std::size_t max_code_points_len = 4;
	static inline constexpr void get_invalid_code_points(char8_t* ptr) noexcept
	{
		::fast_io::details::copy_string_literal(u8"\xEF\xBF\xBD",ptr);
	}
	static inline constexpr ::std::size_t get_code_point(char8_t *firstit,::std::size_t n,char32_t v0) noexcept
	{
		return utf32_to_utf8_code_point_impl(firstit,n,v0);
	}
	static inline constexpr ::std::size_t get_code_point_unchecked(char8_t *firstit,char32_t v0) noexcept
	{
		return utf32_to_utf8_code_point_impl(firstit,4,v0);
	}
};

inline constexpr deco_result<char32_t,char8_t> utf32_to_utf8_impl(
	char32_t const *fromfirst,char32_t const *fromlast,char8_t *tofirst,char8_t *tolast) noexcept
{
	return utf32_to_other_impl<utf32_to_utf8_simple>(fromfirst,fromlast,tofirst,tolast);
}


inline constexpr deco_result<char32_t,char16_t> utf32_to_utf16_impl(
	char32_t const *fromfirst,char32_t const *fromlast,char16_t *tofirst,char16_t *tolast) noexcept
{
	return utf32_to_other_impl<utf8_to_utf16_simple>(fromfirst,fromlast,tofirst,tolast);
}

}
