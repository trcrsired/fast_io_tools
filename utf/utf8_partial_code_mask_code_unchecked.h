#include"utf8_partial_code_mask_common.h"
if constexpr(::std::same_as<output_char_type,char32_t>)
{
	*tofirst=val;
	++tofirst;
}
else
{
	tofirst+=T::get_code_point_unchecked(tofirst,val);
}
fromfirst+=length;
