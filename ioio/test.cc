#include<fast_io.h>

#include"cookie.h"
#include<fast_io_device.h>

int main()
{
	fast_io::io_file file(fast_io::io_cookie_type<fast_io::obuf_file>,u8"c_unlocked_cookie.txt",fast_io::open_mode::out);
	for(std::size_t i{};i!=10000000;++i)
		println(file,i);
}
