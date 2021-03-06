#include"../../fast_io/include/fast_io.h"
#include"../../fast_io/include/fast_io_device.h"
#include"token.h"
#include<map>


int main(int argc,char** argv) noexcept
{
	using namespace fast_io::mnp;
	if(argc<3)
	{
		perr("Usage: ",chvw(argv[0])," <input file> <output file>\n");
		return 1;
	}
	fast_io::u8ibuf_file ibf(argv[1]);
	using namespace std::string_view_literals;
	std::map<std::u8string,std::u8string> map;
	for(auto line:line_generator(ibf))
	{
		if(line.empty())
			continue;
		auto tok{parse_token(line)};
		std::u8string_view first_token(line.cbegin()+tok.first.start,line.cbegin()+tok.first.last);
		if(first_token.starts_with(u8"//"sv))
			continue;
		if(first_token!=u8"#define"sv)
		{
			perrln(fast_io::u8err(),u8"unknown:",line);
			continue;
		}
		std::u8string second(line.data()+tok.second.start,line.data()+tok.second.last);
		for(auto & e : second)
			e=fast_io::char_category::to_c_lower(e);
		if(tok.third.last!=tok.third.start)
		{
			std::u8string third(line.cbegin()+tok.third.start,line.cbegin()+tok.third.last);
			for(auto & e : third)
				e=fast_io::char_category::to_c_lower(e);
			if(map.contains(third))
				continue;
		}
		map.try_emplace(std::move(second),line.cbegin()+tok.fourth.start,line.cbegin()+tok.fourth.last);
	}
	std::u8string buffer;
	fast_io::u8ostring_ref ref{&buffer};
	
	for(auto const& e : map)
	{
		print(ref,e.first,u8",");
		if(!e.second.empty())
			println(ref,u8"\t",e.second);
		else
			println(ref);
	}
	fast_io::u8obuf_file obf(argv[2]);
	print(obf,auto_indent(buffer));
}
