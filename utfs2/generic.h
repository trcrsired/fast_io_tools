#pragma once

namespace fast_io
{

template<::fast_io::manipulators::encoding fromencoding,
	::fast_io::manipulators::encoding toencoding,typename fromchartype,
	typename tochartype>
inline constexpr deco_result<fromchartype,tochartype> code_cvt_api(
	fromchartype const *fromfirst,fromchartype const *fromlast,
	tochartype *tofirst,tochartype *tolast) noexcept
{
	if constexpr(fromencoding==::fast_io::manipulators::encoding::utf8)
	{
		return ::fast_io::details::utf8_generic_impl<::fast_io::details::schemecodeconverter<toencoding>>(fromfirst,fromlast,tofirst,tolast);
	}
	else if constexpr(fromencoding==::fast_io::manipulators::encoding::utf32)
	{
		return ::fast_io::details::utf32_generic_impl<::fast_io::details::schemecodeconverter<toencoding>>(fromfirst,fromlast,tofirst,tolast);
	}
}


namespace decorators
{

template<::fast_io::manipulators::encoding fromencoding,
	::fast_io::manipulators::encoding toencoding>
struct basic_code_cvt_decorator_model
{
	using input_char_type = typename ::fast_io::details::schemecodeconverter<fromencoding>::output_char_type;
	using output_char_type = typename ::fast_io::details::schemecodeconverter<toencoding>::output_char_type;
	
	static inline constexpr ::std::size_t remained_max = 8u;
	static inline constexpr ::std::size_t output_final_max = ::fast_io::details::schemecodeconverter<toencoding>::invalid_code_points_len;

	static inline constexpr auto process_chars(input_char_type const* fromfirst,input_char_type const* fromlast,
					output_char_type* tofirst,output_char_type* tolast) noexcept
	{
		return ::fast_io::code_cvt_api<fromencoding,toencoding>(fromfirst,fromlast,tofirst,tolast);
	}
	static inline constexpr output_char_type* do_final(input_char_type const* fromfirst,input_char_type const* fromlast,output_char_type *tofirst) noexcept
	{
		if(fromfirst==fromlast)
		{
			return tofirst;
		}
		::fast_io::details::schemecodeconverter<toencoding>::get_invalid_code_points(tofirst);
		return tofirst+output_final_max;
	}
};

template<::fast_io::manipulators::encoding fromencoding,
	::fast_io::manipulators::encoding toencoding>
using basic_code_cvt_decorator = ::fast_io::deco_partial_adapter<::fast_io::decorators::basic_code_cvt_decorator_model<fromencoding,toencoding>>;


using utf8_to_utf32 = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::utf32>;
using utf8_to_utf32le = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::utf32le>;
using utf8_to_utf32be = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::utf32be>;
using utf8_to_utf16 = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::utf16>;
using utf8_to_utf16le = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::utf16le>;
using utf8_to_utf16be = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::utf16be>;
using utf8_to_utf_ebcdic = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::utf_ebcdic>;
using utf8_to_gb18030 = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf8,::fast_io::manipulators::encoding::gb18030>;


using utf32_to_utf8 = basic_code_cvt_decorator<::fast_io::manipulators::encoding::utf32,::fast_io::manipulators::encoding::utf8>;

using utf8_utf32 = ::fast_io::basic_bidirectional_decorator_adaptor<utf32_to_utf8,utf8_to_utf32>;
using utf8_gb18030 = ::fast_io::basic_bidirectional_decorator_adaptor<utf8_to_gb18030,utf8_to_gb18030>;

}

}

