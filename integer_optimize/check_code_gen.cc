#include<fast_io.h>
#include"jeaiii_method.h"

[[gnu::flatten]]
char8_t* jeaiii_main(char8_t* ptr,std::uint_least64_t n) noexcept
{
	return ::fast_io::details::jeaiii::jeaiii_main(ptr,n);
}