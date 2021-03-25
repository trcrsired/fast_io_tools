#include"../../fast_io/include/fast_io.h"
#include"../../fast_io/include/fast_io_device.h"
#include<map>

struct parse_line_result
{
std::size_t a=-1;
std::size_t b=-1;
std::size_t c=-1;
std::size_t d=-1;
};

inline constexpr std::size_t skip_tab(std::u8string_view token) noexcept
{
	std::size_t i{};
	for(;i!=token.size()&&token[i]==u8'\t';++i);
	return i;
}

inline constexpr parse_line_result parse(std::u8string_view view) noexcept
{
	auto atoken{find_next_token(view)};
	std::u8string_view atoken_vw(view.substr(atoken));
	if(atoken)[[unlikely]]
		fast_io::fast_terminate();
	
}

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
	std::map<std::u8string,std::u8string> set;
	for(auto line:line_generator(ibf))
	{
		if(line.empty())
			continue;
		if(a!=u8"#define"sv||b.size()<4)
		{
			perrln(fast_io::u8err(),u8"unknown:",a,u8" ",b,u8" ",c);
			continue;
		}
		std::u8string sstr(b.substr(3));
		for(auto & e : sstr)
			e=fast_io::char_category::to_c_lower(e);
		if(set.contains(sstr)||set.contains(sstr))
		{
			continue;
		}
		set.emplace(std::move(sstr));
	}
	fast_io::u8obuf_file obf(argv[2]);
	std::size_t i{};
	for(auto const& e : set)
	{
		println(obf,e,u8"=",i);
		++i;
	}
}