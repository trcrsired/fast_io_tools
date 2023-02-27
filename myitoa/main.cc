#include<fast_io.h>
#include"digits_table.h"

int main()
{
	fast_io::out_buf obf{fast_io::out()};
	for(auto ch : ::fast_io::details::digits_table<char8_t,16>)
	{
		print(obf,fast_io::mnp::chvw(ch));
	}
	println(obf);
}
