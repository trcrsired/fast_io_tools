#include"utf8_partial_code_mask_common.h"
if constexpr(::std::same_as<output_char_type,char32_t>)
{
	*tofirst=val;
	++tofirst;
}
else
{
	auto ret=T::get_code_point(tofirst,static_cast<::std::size_t>(tolast-tofirst),val);
	if(!ret)
	{
		break;
	}
	tofirst+=ret;
}
fromfirst+=length;
