#include<string>
#include<fast_io.h>
#include"strlike.h"
#include"strlike_reference_wrapper.h"
#include"strng.h"

int main()
{
	std::string str;
	auto oref{io_strlike_ref(fast_io::io_alias,str)};
//	static_assert(fast_io::buffer_output_stream<decltype(oref)>);
	print(oref,"Hello\n",20,"agsasg\n");
	print(fast_io::out(),str);
}
