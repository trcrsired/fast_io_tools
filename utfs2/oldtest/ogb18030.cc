#include<fast_io.h>
#include<fast_io_unit/gb18030.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>

int main()
{
	constexpr std::size_t N(1000000u);
	{
	::fast_io::timer t(u8"output");
	::fast_io::ogb18030_file obf(u8"ogb18030.txt",::fast_io::open_mode::out);
	for(std::size_t i{};i!=N;++i)
		::fast_io::io::print(obf,"Hello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\n");
	}
}