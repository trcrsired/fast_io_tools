#pragma once

#include<fast_io_unit/gb18030.h>

namespace fast_io::details
{

namespace codecvt::gb18030
{
constexpr ::std::size_t get_gb18030_code_units_unhappy_pdstsz(char32_t, char*, ::std::size_t) noexcept;
}

constexpr void schemecodeconverter<::fast_io::manipulators::encoding::gb18030>::get_invalid_code_points(output_char_type* tofirst) noexcept
{
	::fast_io::details::codecvt::gb18030::get_gb18030_invalid_code_units(tofirst);
}

constexpr ::std::size_t schemecodeconverter<::fast_io::manipulators::encoding::gb18030>::get_code_point(output_char_type* firstit,::std::size_t n,char32_t v0) noexcept
{
	return ::fast_io::details::codecvt::gb18030::get_gb18030_code_units_unhappy_pdstsz(v0,firstit,n);
}

constexpr ::std::size_t schemecodeconverter<::fast_io::manipulators::encoding::gb18030>::get_code_point_unchecked(output_char_type* firstit,char32_t v0) noexcept
{
	return ::fast_io::details::codecvt::gb18030::get_gb18030_code_units_unhappy_pdstsz(v0,firstit,max_code_points_len);
}

}
