#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<string_view>
#include<map>

int main(int argc,char** argv)
{
	if(argc < 3)
	{
		if(argc == 0)
		{
			return 1;
		}
		perrln("Usage:",fast_io::mnp::os_c_str(*argv)," <txt file> <output_file>");
		return 1;
	}
	fast_io::u8ibuf_file uf(fast_io::mnp::os_c_str(argv[1]));
	std::map<std::u8string,std::u8string> map;
	for(std::u8string_view line: line_scanner(uf))
	{
		auto start{line.data()};
		auto ed{start+line.size()};

		if(start==ed)
		{
			continue;
		}
		for(;start!=ed&&fast_io::char_category::is_c_space(*start);++start);
		for(;start!=ed&&fast_io::char_category::is_c_space(ed[-1]);--ed);
		if(start==ed)
		{
			continue;
		}
		auto fnd{start};
		for(;fnd!=ed&&!fast_io::char_category::is_c_space(*fnd);++fnd);
		auto fdp1{fnd};
		for(;fnd!=ed&&fast_io::char_category::is_c_space(*fnd);++fnd);
		if(fnd==ed)
		{
			continue;
		}
		map.emplace(std::piecewise_construct,std::forward_as_tuple(start,fdp1),std::forward_as_tuple(fnd,ed));
	}
	fast_io::u8obuf_file obf(fast_io::mnp::os_c_str(argv[2]));
	for(auto& [key,value]: map)
	{
		println(obf,key,u8" ",value);
	}
}
