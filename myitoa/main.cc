#include<fast_io.h>
#include"digits_table.h"

int main()
{
	fast_io::out_buf_type obf{fast_io::out()};
	for(auto ch : ::fast_io::details::digits_table<wchar_t,10,true>)
	{
		print(obf,fast_io::mnp::chvw(ch));
	}
	println(obf);
}
