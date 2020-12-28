#include"../../fast_io/include/fast_io.h"
#include"rounding.h"
#include"common.h"
#include"easy_cases.h"
#include"ryu.h"
#include"large_int.h"

int main()
{
	std::array<char,30> buffer;
	auto it=fast_io::details::fp::large_int::fp_to_chars<fast_io::floating_mode::general,false,u8'.'>(buffer.data(),4.2);
	write(fast_io::out(),buffer.data(),it);
//	print(fast_io::out(),4.2);
}