#include<fast_io.h>
#include"jeaiii_method_len.h"

int main()
{
	char buffer[40];
	constexpr __uint128_t c{fast_io::to<__uint128_t>("340282366920938463463374607431768211455")};
	::fast_io::details::jeaiii::jeaiii_main_len(buffer,c,39);
	write(fast_io::c_stdout(),buffer,buffer+39);
	println(fast_io::c_stdout());
	constexpr std::uint_least64_t test_ryu_mode{99999999999999999ULL};
	::fast_io::details::jeaiii::jeaiii_main_len<true>(buffer,test_ryu_mode,17);
	write(fast_io::c_stdout(),buffer,buffer+17);
}
