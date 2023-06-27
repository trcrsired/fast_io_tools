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


template<typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_to_other_nosimd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst) noexcept
{
	using output_char_type = typename T::output_char_type;
	constexpr
		::std::size_t
		invalidcodepointslen{T::invalid_code_points_len};
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
#include"utf8_partial_code_copyto4.h"
				::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
#include"utf8_partial_code_nosimd.h"
			}
#include"utf8_partial_code_remain_chars.h"
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
#include"utf8_partial_code_invalid_code_point_unchecked.h"
			continue;
		}
#include"utf8_partial_code_mask_code_unchecked.h"
	}
	return {fromfirst,tofirst};
}

template<::std::size_t N,typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_to_other_simd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst) noexcept
{
	if constexpr(N!=16&&N!=32&&N!=64)
	{
		return utf8_to_other_nosimd_impl<T>(fromfirst,fromlast,tofirst);
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
	using output_char_type = typename T::output_char_type;
	constexpr
		::std::size_t
		invalidcodepointslen{T::invalid_code_points_len};
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
#include"utf8_partial_code_copyto4.h"
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
#include"utf8_partial_code_simd.h"
				::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
#include"utf8_partial_code_nosimd.h"
			}
#include"utf8_partial_code_remain_chars.h"
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
#include"utf8_partial_code_invalid_code_point_unchecked.h"
			continue;
		}
#include"utf8_partial_code_mask_code_unchecked.h"
	}
	return {fromfirst,tofirst};
}


template<typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_to_other_nosimd_tolast_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	constexpr
		::std::size_t
		invalidcodepointslen{T::invalid_code_points_len};
	constexpr
		::std::size_t invdcpm1{invalidcodepointslen-1u};
	constexpr
		::std::size_t
		mxcodepointslen{T::max_code_points_len};
	using output_char_type = typename T::output_char_type;
	while(fromfirst<fromlast&&tofirst<tolast)
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
#include"utf8_partial_code_copyto4.h"
				::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff2{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff2<fromdiff2)
				{
					fromdiff2=todiff2;
				}
#include"utf8_partial_code_nosimd.h"
			}
#include"utf8_partial_code_remain_chars.h"
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
#include"utf8_partial_code_invalid_code_point.h"
			continue;
		}
#include"utf8_partial_code_mask_code.h"
	}
	return {fromfirst,tofirst};
}

template<::std::size_t N,typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_to_other_simd_tolast_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	if constexpr(N!=16&&N!=32&&N!=64)
	{
		return utf8_to_other_nosimd_last_impl<T>(fromfirst,fromlast,tofirst,tolast);
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
	using output_char_type = typename T::output_char_type;
	constexpr
		::std::size_t
		invalidcodepointslen{T::invalid_code_points_len};
	constexpr
		::std::size_t invdcpm1{invalidcodepointslen-1u};
	constexpr
		::std::size_t
		mxcodepointslen{T::max_code_points_len};
	while(fromfirst<fromlast&&tofirst<tolast)
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
#include"utf8_partial_code_copyto4.h"
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff<fromdiff)
				{
					fromdiff=todiff;
				}
#include"utf8_partial_code_simd.h"
				::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff2{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff2<fromdiff2)
				{
					fromdiff2=todiff2;
				}
#include"utf8_partial_code_nosimd.h"
			}
#include"utf8_partial_code_remain_chars.h"
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
#include"utf8_partial_code_invalid_code_point.h"
			continue;
		}
#include"utf8_partial_code_mask_code.h"
	}
	return {fromfirst,tofirst};
}


template<typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf8_to_other_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	using output_char_type = typename T::output_char_type;
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
			if constexpr(16<=N)
			{
				if constexpr(::std::same_as<output_char_type,char32_t>)
				{
					auto [fromit,toit]=utf8_to_other_simd_impl<N,T>(fromfirst,fromfirst+mndiff,tofirst);
					fromfirst=fromit;
					tofirst=toit;
				}
				else
				{
					auto [fromit,toit]=utf8_to_other_simd_tolast_impl<N,T>(fromfirst,fromfirst+mndiff,tofirst,tofirst+mndiff);
					fromfirst=fromit;
					tofirst=toit;
				}

			}
			else
			{
				if constexpr(::std::same_as<output_char_type,char32_t>)
				{
					auto [fromit,toit]=utf8_to_other_nosimd_impl<T>(fromfirst,fromfirst+mndiff,tofirst);
					fromfirst=fromit;
					tofirst=toit;
				}
				else
				{
					auto [fromit,toit]=utf8_to_other_nosimd_tolast_impl<T>(fromfirst,fromfirst+mndiff,tofirst,tofirst+mndiff);
					fromfirst=fromit;
					tofirst=toit;
				}
			}
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
#include"utf8_partial_code_invalid_code_point.h"
			continue;
		}
		::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
		if(fromdiff<static_cast<unsigned>(length))
		{
			break;
		}
		char32_t val;
		__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
		if constexpr(::std::endian::big!=::std::endian::native)
		{
			val=::fast_io::byte_swap(val);
		}
#include"utf8_partial_code_mask_code.h"
	}
	return {fromfirst,tofirst};
}

struct utf8_to_utf32_simple
{
	using output_char_type = char32_t;
	static inline constexpr ::std::size_t invalid_code_points_len = 1;
	static inline constexpr ::std::size_t max_code_points_len = 1;
};

struct utf8_to_utf16_simple
{
	using output_char_type = char16_t;
	static inline constexpr ::std::size_t invalid_code_points_len = 1;
	static inline constexpr ::std::size_t max_code_points_len = 2;
	static inline constexpr void get_invalid_code_points(char16_t* ptr) noexcept
	{
		*ptr=0xFFFD;
	}
	static inline constexpr ::std::size_t get_code_point(char16_t* tofirst,::std::size_t dfsz,char32_t codepoint) noexcept
	{
		if(codepoint < 0xD800 || (codepoint > 0xDFFF && codepoint < 0x10000))
		{
			*tofirst=static_cast<char16_t>(codepoint);
			return 1;
		}
		if(dfsz<2)
		{
			return 0;
		}
		codepoint-=0x010000;
		*tofirst = char16_t(((0b1111'1111'1100'0000'0000 & codepoint) >> 10) + 0xD800);
		tofirst[1] = char16_t(((0b0000'0000'0011'1111'1111 & codepoint) >> 00) + 0xDC00);
		return 2;
	}
	static inline constexpr ::std::size_t get_code_point_unchecked(char16_t* tofirst,char32_t codepoint) noexcept
	{
		if constexpr(false)
		{
			return get_code_point(tofirst,2,codepoint);
		}
		else
		{
			if(codepoint < 0xD800 || (codepoint > 0xDFFF && codepoint < 0x10000))
			{
				*tofirst=static_cast<char16_t>(codepoint);
				return 1;
			}
			codepoint-=0x010000;
			*tofirst = char16_t(((0b1111'1111'1100'0000'0000 & codepoint) >> 10) + 0xD800);
			tofirst[1] = char16_t(((0b0000'0000'0011'1111'1111 & codepoint) >> 00) + 0xDC00);
			return 2;
		}
	}
};

namespace codecvt::gb18030
{
::std::size_t get_gb18030_code_units_unhappy_pdstsz(char32_t, char*, ::std::size_t) noexcept;
::std::size_t get_gb18030_code_units_unhappy(char32_t, char*) noexcept;
}

struct utf8_to_gb18030_simple
{
	using output_char_type = char;
	static inline constexpr ::std::size_t invalid_code_points_len = 4;
	static inline constexpr ::std::size_t max_code_points_len = 4;
	static inline constexpr void get_invalid_code_points(char* ptr) noexcept
	{
		::fast_io::details::copy_string_literal("\x84\x31\xA4\x37",ptr);
	}
	static inline constexpr ::std::size_t get_code_point(char *tofirst,::std::size_t dfsz,char32_t codepoint) noexcept
	{
		return ::fast_io::details::codecvt::gb18030::get_gb18030_code_units_unhappy_pdstsz(codepoint,tofirst,dfsz);
	}
	static inline constexpr ::std::size_t get_code_point_unchecked(char *tofirst,char32_t codepoint) noexcept
	{
		return ::fast_io::details::codecvt::gb18030::get_gb18030_code_units_unhappy(codepoint,tofirst);
	}
};

inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst,char32_t *tolast) noexcept
{
	return utf8_to_other_impl<utf8_to_utf32_simple>(fromfirst,fromlast,tofirst,tolast);
}

inline constexpr deco_result<char8_t,char> utf8_to_gb18030_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char *tofirst,char *tolast) noexcept
{
	return utf8_to_other_impl<utf8_to_gb18030_simple>(fromfirst,fromlast,tofirst,tolast);
}

inline constexpr deco_result<char8_t,char16_t> utf8_to_utf16_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char16_t *tofirst,char16_t *tolast) noexcept
{
	return utf8_to_other_impl<utf8_to_utf16_simple>(fromfirst,fromlast,tofirst,tolast);
}


}

}
