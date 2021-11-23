#include<fast_io.h>
#include"jeaiii_method.h"

int main()
{
	char buffer[40];
	constexpr __uint128_t c{fast_io::to<__uint128_t>("340282366920938463463374607431768211455")};
	auto ptr{::fast_io::details::jeaiii::jeaiii_main(buffer,c)};
	write(fast_io::c_stdout(),buffer,ptr);
}
