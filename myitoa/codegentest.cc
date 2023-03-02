#include<fast_io.h>
#include"digits_table.h"
#include"uprsv2.h"

char8_t* testgen64(char8_t* ptr,::std::uint_least64_t value) noexcept
{
	return ::fast_io::details::uprsv64_impl(ptr,value);
}
