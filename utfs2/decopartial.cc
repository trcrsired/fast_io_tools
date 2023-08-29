#include<fast_io.h>
#include"impl.h"
#include"gb18030.h"

int main()
{
	::fast_io::iobuf_io_file file(::fast_io::io_cookie_type<::fast_io::native_file>,u8"io_file_deco.txt",::fast_io::open_mode::out);

	::fast_io::operations::add_io_decos(file,::fast_io::decorators::utf8_gb18030{});
	::fast_io::io::print(file,"Hello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\n");
}
