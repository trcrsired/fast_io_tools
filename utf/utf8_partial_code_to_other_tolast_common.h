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
if constexpr(issimd)
{
#include"utf8_partial_code_simd.h"
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
