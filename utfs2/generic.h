#pragma once

namespace fast_io
{

template<::fast_io::manipulators::encoding fromencoding,
	::fast_io::manipulators::encoding toencoding,typename fromchartype,
	typename tochartype>
inline constexpr deco_result<fromchartype,tochartype> code_cvt_api(
	fromchartype *fromfirst,fromchartype *fromlast,
	tochartype *tofirst,tochartype *tolast) noexcept
{
	if constexpr(fromencoding==::fast_io::manipulators::encoding::utf8)
	{
		return ::fast_io::details::utf8_generic_impl<::fast_io::details::schemecodeconverter<toencoding>>(fromfirst,fromlast,tofirst,tolast);
	}
}

}
