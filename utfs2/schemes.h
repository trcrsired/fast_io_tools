#pragma once

namespace fast_io
{

namespace manipulators
{

enum class encoding
{
utf8,
utf16_be,
utf16_le,
utf16=(::std::endian::big==::std::endian::native?utf16_be:utf16_le),
utf32_be,
utf32_le,
utf32=(::std::endian::big==::std::endian::native?utf32_be:utf32_le),
utf_ebcdic,
gb18030
};

}

namespace details
{

template<::fast_io::manipulators::encoding>
struct schemecodeconverter
{
};

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

template<bool noswapcode>
inline constexpr ::std::size_t utf32_to_utf16_code_point_impl(char16_t *dst, ::std::size_t szdpt, char32_t cdpt) noexcept
{
	if (cdpt < 0x10000)
	{
		if(szdpt<1)
		{
			return 0;
		}
		if constexpr(noswapcode)
		{
			*dst = static_cast<char16_t>(cdpt);
		}
		else
		{
			*dst = ::fast_io::byte_swap(static_cast<char16_t>(cdpt));
		}
		return 1;
	}
	else
	{
		if(szdpt<2)
		{
			return 0;
		}
		char16_t low{static_cast<char16_t>(0xD7C0 + (cdpt >> 10))};
		char16_t high{static_cast<char16_t>(0xDC00 + (cdpt & 0x3FF))};
		if constexpr(noswapcode)
		{
			*dst = low;
			dst[1] = high;
		}
		else
		{
			*dst = ::fast_io::byte_swap(low);
			dst[1] = ::fast_io::byte_swap(high);
		}
		return 2;
	}
}

template<>
struct schemecodeconverter<::fast_io::manipulators::encoding::utf8>
{
	using output_char_type = char8_t;
	static inline constexpr ::fast_io::manipulators::encoding encode{::fast_io::manipulators::encoding::utf8};
	static inline constexpr ::std::size_t invalid_code_points_len = 3;
	static inline constexpr ::std::size_t max_code_points_len = 4;
	inline constexpr void get_invalid_code_points(output_char_type* tofirst) noexcept
	{
		::fast_io::details::copy_string_literal(u8"\xEF\xBF\xBD",tofirst);
	}
	inline constexpr ::std::size_t get_code_point(output_char_type *firstit,::std::size_t n,char32_t v0) noexcept
	{
		return ::fast_io::details::utf32_to_utf8_code_point_impl(firstit,n,v0);
	}
};

template<>
struct schemecodeconverter<::fast_io::manipulators::encoding::utf16_le>
{
	using output_char_type = char16_t;
	static inline constexpr ::fast_io::manipulators::encoding encode{::fast_io::manipulators::encoding::utf16_le};
	static inline constexpr ::std::size_t invalid_code_points_len = 1;
	static inline constexpr ::std::size_t max_code_points_len = 2;
	inline constexpr void get_invalid_code_points(output_char_type* tofirst) noexcept
	{
		if constexpr(::std::endian::little==::std::endian::native)
		{
			*tofirst=0xFFFD;
		}
		else
		{
			*tofirst=0xFDFF;
		}
	}
	inline constexpr ::std::size_t get_code_point(output_char_type *firstit,::std::size_t n,char32_t v0) noexcept
	{
		return ::fast_io::details::utf32_to_utf16_code_point_impl<::std::endian::little==::std::endian::native>(firstit,n,v0);
	}
};

template<>
struct schemecodeconverter<::fast_io::manipulators::encoding::utf16_be>
{
	using output_char_type = char16_t;
	static inline constexpr ::fast_io::manipulators::encoding encode{::fast_io::manipulators::encoding::utf16_be};
	static inline constexpr ::std::size_t invalid_code_points_len = 1;
	static inline constexpr ::std::size_t max_code_points_len = 2;
	inline constexpr void get_invalid_code_points(output_char_type* tofirst) noexcept
	{
		if constexpr(::std::endian::big==::std::endian::native)
		{
			*tofirst=0xFFFD;
		}
		else
		{
			*tofirst=0xFDFF;
		}
	}
	inline constexpr ::std::size_t get_code_point(output_char_type *firstit,::std::size_t n,char32_t v0) noexcept
	{
		return ::fast_io::details::utf32_to_utf16_code_point_impl<::std::endian::big==::std::endian::native>(firstit,n,v0);
	}
};

template<>
struct schemecodeconverter<::fast_io::manipulators::encoding::utf32_le>
{
	using output_char_type = char32_t;
	static inline constexpr ::fast_io::manipulators::encoding encode{::fast_io::manipulators::encoding::utf32_le};
	static inline constexpr ::std::size_t invalid_code_points_len = 1;
	static inline constexpr ::std::size_t max_code_points_len = 1;
};

template<>
struct schemecodeconverter<::fast_io::manipulators::encoding::utf32_be>
{
	using output_char_type = char32_t;
	static inline constexpr ::fast_io::manipulators::encoding encode{::fast_io::manipulators::encoding::utf32_be};
	static inline constexpr ::std::size_t invalid_code_points_len = 1;
	static inline constexpr ::std::size_t max_code_points_len = 1;
};
#if 0
template<>
struct schemecodeconverter<::fast_io::manipulators::encoding::utf_ebcdic>
{
	using output_char_type = char;
};

template<>
struct schemecodeconverter<::fast_io::manipulators::encoding::gb18030>
{
	using output_char_type = char;
	inline constexpr ::std::size_t get_code_point() noexcept
	{

	}
};
#endif
}

}
