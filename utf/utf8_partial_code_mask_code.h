#include"utf8_partial_code_mask_common.h"
if constexpr(::std::same_as<output_char_type,char32_t>)
{
	*tofirst=val;
	++tofirst;
}
else
{
	auto toit=T::get_code_point(tofirst,tolast,val);
	if(toit==tofirst)
	{
		break;
	}
	tofirst=toit;
}
fromfirst+=length;
