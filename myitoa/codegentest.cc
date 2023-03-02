#include<fast_io.h>
#include"digits_table.h"
#include"uprsv.h"

char8_t* testgen32(char8_t* ptr,::std::uint_least32_t value) noexcept
{
	return ::fast_io::details::uprsv32_impl(ptr,value);
}
