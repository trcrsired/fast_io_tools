#include"parsing.h"

int main(int argc,char** argv)
{
	if(argc<3)
	{
		if(argc==0)
		{
			return 1;
		}
		perrln("Usage: ",fast_io::mnp::os_c_str(*argv)," <localedata directory> <result directory>");
		return 1;
	}
	fast_io::dir_file df(fast_io::mnp::os_c_str(argv[1]));
	fast_io::dir_file resdf(fast_io::mnp::os_c_str(argv[2]));
	std::unordered_map<std::string,std::unordered_map<std::string,std::unordered_map<std::string,std::string>>> cache;
	for(auto ent: current(at(df)))
	{
		auto fnm{fast_io::concat(ent)};
		using namespace std::string_view_literals;
		if(fnm=="."sv||fnm==".."sv)
			continue;
		if(fnm=="cns11643_stroke"||fnm=="i18n_ctype")
			continue;
		if(fnm.starts_with("iso14651")||fnm.starts_with("translit"))
			continue;
		auto& result{fast_io_i18n::parsing_file(df,fnm,cache)};
		fast_io::obuf_file obf(at(resdf),fast_io::concat(fnm,".cc"));
		print(obf,"\xEF\xBB\xBF");
		fast_io_i18n::output_result(result,obf,fnm);
	}
}