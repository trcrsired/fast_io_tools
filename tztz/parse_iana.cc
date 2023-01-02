#include<string_view>
#include<fast_io_device.h>
#include<fast_io.h>
#include<fast_io_dsal/vector.h>
#include<string>
#include<map>

int main(int argc,char** argv)
{
	if(argc<3)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",fast_io::mnp::os_c_str(argv[0])," <input file> <output file>\n");
		return 1;
	}
	fast_io::u8ibuf_file ibf(fast_io::mnp::os_c_str(argv[1]));
#if 0
	if constexpr(false)
	{
		fast_io::u8obuf_file obf(fast_io::mnp::os_c_str(argv[2]));
		print(obf,u8"#pragma once\n\n");
		for(std::u8string_view line:line_scanner(ibf))
		{
			if(line.empty())
				continue;
			auto i{line.rfind(u8'\t')};
			if(i==SIZE_MAX)
			{
				print(obf,line,u8" : bad");
				continue;
			}
			print(obf,u8"#pragma comment(linker,\"/alternatename:",line.substr(0,i),u8"=",line.substr(i+1),u8"\")\n");
		}
	}
	else
#endif
	{
		std::map<std::u8string,std::u8string> map;
		for(auto& lineb:line_scanner(ibf))
		{
			std::u8string_view line(lineb);
			if(line.empty())
				continue;
			auto i{line.rfind(u8'\t')};
			if(i==SIZE_MAX)
			{
//				print(obf,line,u8" : bad");
				continue;
			}
			std::u8string_view a=line.substr(0,i);
			std::u8string_view b=line.substr(i+1);
			map.emplace(std::piecewise_construct,
				std::forward_as_tuple(b),
				std::forward_as_tuple(a));
		}
		fast_io::vector<std::u8string const*> vec,vec9xa;
		for(auto const & e : map)
		{
			vec.emplace_back(__builtin_addressof(e.second));
			vec9xa.emplace_back(__builtin_addressof(e.first));
		}

		fast_io::u8obuf_file obf(fast_io::mnp::os_c_str(argv[2]));
		auto lmd([&obf](auto& vec,auto extra)
		{
			for(auto it{vec.cbegin()};it!=vec.cend();++it)
			{
				if(it!=vec.cbegin())
				{
					print(obf,u8",\n");
				}
				auto &e{**it};
				print(obf,u8"{",extra,u8"\"",e,u8"\",",e.size(),u8"}");
			}
		});
		using namespace std::string_view_literals;
		print(obf,u8"#pragma once\n\ninline constexpr basic_io_scatter_t<char8_t> win32_registry_timezone_9xa[]\n{\n");
		lmd(vec9xa,u8"u8"sv);
		print(obf,u8"\n};\n\ninline constexpr basic_io_scatter_t<char16_t> win32_registry_timezone_ntw[]\n{\n");		
		lmd(vec9xa,u8"u"sv);
		print(obf,u8"\n};\n\ninline constexpr basic_io_scatter_t<char> iana_db_timezone[]\n{\n");
		lmd(vec,u8""sv);
		print(obf,u8"\n};\n\ninline constexpr basic_io_scatter_t<wchar_t> wiana_db_timezone[]\n{\n");
		lmd(vec,u8"L"sv);
		print(obf,u8"\n};\n\ninline constexpr basic_io_scatter_t<char8_t> u8iana_db_timezone[]\n{\n");
		lmd(vec,u8"u8"sv);
		print(obf,u8"\n};\n\ninline constexpr basic_io_scatter_t<char16_t> u16iana_db_timezone[]\n{\n");
		lmd(vec,u8"u"sv);
		print(obf,u8"\n};\n\ninline constexpr basic_io_scatter_t<char32_t> u32iana_db_timezone[]\n{\n");
		lmd(vec,u8"U"sv);
		print(obf,u8"\n};\n");
	}
}
