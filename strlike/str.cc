#include<string>
#include<fast_io.h>
#include"strlike.h"
#include"strlike_reference_wrapper.h"
#include"strng.h"
#include"concat_general.h"

int main()
{
//	std::string str;
//	auto oref{io_strlike_ref(fast_io::io_alias,str)};
//	static_assert(fast_io::buffer_output_stream<decltype(oref)>);
//	print(oref,"Hello\n",20,"agsasg\n");
	print(fast_io::out(),fast_io::test_concat("abc","def",5,"gd"));
}
