#include<string>
#include<fast_io.h>
#include<vector>

inline constexpr bool test()
{
	std::vector<fast_io::c_file> vec;
	vec.emplace_back("a.txt",fast_io::open_mode::out);
	print(vec.front(),"Hello World\n",241,"dgs\n");
	vec.emplace_back("b.txt",fast_io::open_mode::out);
	print(vec.back(),"Hello World\n",241,"dgs\n");
	for(std::size_t i{};i!=100;++i)
	{//all sorts of compiler bugs
		vec.emplace_back(fast_io::concat(i,".txt"),fast_io::open_mode::out);
		println(vec.back(),"Hello World ",i);
	}
	return true;
}

static_assert(test());

int main()
{
	test();
}
