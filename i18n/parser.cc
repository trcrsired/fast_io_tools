#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<string_view>
#include<map>

int main(int argc,char** argv)
{
	if(argc < 2)
	{
		if(argc == 0)
		{
			return 1;
		}
		perrln("Usage:",fast_io::mnp::os_c_str(*argv)," <txt file>");
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
	fast_io::u8obuf_file obf(u8"win32_lcid_table.h");
	print(obf,
u8R"abc(if constexpr(::std::same_as<char_type,char16_t>)
{
switch(lcid)
{
)abc"
);
	for(auto& [key,value]: map)
	{
		println(obf,u8"case ",key,u8":return copy_string_literal(u\"",value,u8"\",p);");
	}
	println(obf,
u8R"az(default:*p=u'C';return p+1;
}
}
else
{
switch(lcid)
{
)az"
);
	for(auto& [key,value]: map)
	{
		println(obf,u8"case ",key,u8":return copy_string_literal(u8\"",value,u8"\",p);");
	}
	print(obf,
u8R"az(default:*p=u8'C';return p+1;
}
}
)az");
}
