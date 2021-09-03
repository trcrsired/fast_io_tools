#pragma once

namespace fast_io
{

template<std::integral ch_type>
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[gnu::forceinline]]
#endif
inline constexpr ch_type char_literal(char8_t ch) noexcept
{
#if 'A' != u8'A' || L'A' !=u8'A'
#include"none_ascii.h"
#endif
	{
		using unsigned_t = ::std::make_unsigned_t<ch_type>;
		return static_cast<unsigned_t>(ch);
	}
}

template<std::integral ch_type,std::size_t n>
requires (n!=0)
inline constexpr ::fast_io::freestanding::array<ch_type,n-1> c_string_literal_to_char_array(char8_t const (&literal)[n]) noexcept
{
	constexpr std::size_t nm1{n-1};
	::fast_io::freestanding::array<ch_type,nm1> buffer;
#if 'A' != u8'A' || L'A' !=u8'A'
	if constexpr(::fast_io::details::is_ebcdic<ch_type>)
	{
		for(std::size_t i{};i!=nm1;++i)
			buffer[i]=char_literal(literal[i]);
	}
	else
#endif
	{
		using unsigned_t = ::std::make_unsigned_t<ch_type>;
		for(std::size_t i{};i!=nm1;++i)
			buffer[i]=static_cast<ch_type>(static_cast<unsigned_t>(literal[i]));
	}
	return buffer;
}

template<std::size_t n>
struct literal_ir
{
	char8_t buffer[n];
};

template<char8_t const* cstr>
requires (n!=0)
inline constexpr ::fast_io::literal_ir<n-1> calculate_literal_ir(char8_t const (&literal)[n]) noexcept
{
	return {};
}

template<char8_t const* cstr>
inline constexpr literal_ir<n> literal_ir_values{};

//template<std::integral ch_type,std::size_t n>
//inline constexpr ::fast_io::freestanding::array<ch_type,n> literal_array_reference

}
