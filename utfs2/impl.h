#pragma once


namespace fast_io
{

namespace details
{

template<char8_t lfch,::std::integral char_type,::std::size_t N,bool signed_disposition>
inline constexpr auto utfconstantsimd_impl() noexcept
{
	if constexpr(N==1)
	{
		return ::fast_io::details::empty{};
	}
	else
	{
		using simd_vector_type = ::fast_io::intrinsics::simd_vector<::std::uint_least8_t,N>;
#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
		constexpr
			simd_vector_type cmp{
			::std::bit_cast<simd_vector_type>(::fast_io::details::characters_array_impl<lfch,char_type,N,signed_disposition>)};
#else
		simd_vector_type cmp;
		cmp.load(::fast_io::details::characters_array_impl<lfch,char_type,N,signed_disposition>.data());
#endif
		return cmp;
	}
}

template<typename inputchartype,typename outputchartype>
struct deco_result_simd
{
	using input_char_type = inputchartype;
	using output_char_type = outputchartype;
	input_char_type const *fromit;
	output_char_type *toit;
	bool done;
};

}

}

#include"schemes.h"
#include"utf8.h"
#include"generic.h"
