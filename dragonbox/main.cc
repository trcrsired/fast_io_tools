#include"../../fast_io/include/fast_io.h"
#include"rounding.h"
#include"common.h"
#include"easy_cases.h"
#include"large_int.h"

int main()
{
	std::array<char,30> buffer;
	auto it=fast_io::details::fp::large_int::fp_to_chars<fast_io::floating_mode::general,false,u8'.'>(buffer.data(),400000000000.0);
	write(fast_io::win32_stdout(),buffer.data(),it);
}