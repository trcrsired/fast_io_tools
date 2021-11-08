#include<string>
#include<fast_io.h>
#include"strlike.h"
#include"strlike_reference_wrapper.h"
#include"strng.h"

int main()
{
	std::string str;
	auto oref{io_strlike_ref(fast_io::io_alias,str)};
	print(oref,"Hello\n",124,"agsasg\n");
	print(str);
}
