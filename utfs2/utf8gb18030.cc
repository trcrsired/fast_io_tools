#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include"impl.h"
#include"gb18030.h"
using namespace fast_io::io;

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	::fast_io::iobuf_io_file obf(::fast_io::io_cookie_type<::fast_io::native_file>,u8"io_file_deco.txt",::fast_io::open_mode::out);
	::fast_io::operations::add_io_decos(obf,::fast_io::decorators::utf8_gb18030{});
	for(std::size_t i{};i!=N;++i)
		println(obf,i);
	}
}
