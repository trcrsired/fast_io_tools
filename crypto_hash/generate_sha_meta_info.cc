#include<string>
#include<fast_io.h>
#include<fast_io_device.h>

int main(int argc,char** argv)
{
	if(argc<3)
	{
		perr("Usage:",fast_io::mnp::os_c_str(*argv)," <file> <outfile>\n");
		return 1;
	}
	fast_io::u8ibuf_file ibf(fast_io::mnp::os_c_str(argv[1]));
	fast_io::u8obuf_file obf(fast_io::mnp::os_c_str(argv[2]));
	for(std::u8string str;scan<true>(ibf,str);)
	{
		print(obf,u8"struct ",str);
		for(auto& e : str)
			e=::fast_io::char_category::to_c_upper(e);
		print(obf,u8R"abc(_meta_info
{
	using ctx_type = )abc",str,u8R"abc(_CTX;
	static inline std::size_t digest_length{)abc",str,u8R"abc(_DIGEST_LENGTH};
	static inline void init_function(ctx_type* c) noexcept
	{
		::fast_io::noexcept_call()abc",str,u8R"abc(_Init,c);
	}
	static inline void update_function(ctx_type* c,void const* data,std::size_t len) noexcept
	{
		::fast_io::noexcept_call()abc",str,u8R"abc(_Update,c,data,len);
	}
	static inline void final_function(char unsigned* md,ctx_type* c) noexcept
	{
		::fast_io::noexcept_call()abc",str,u8R"abc(_Final,md,c);
	}
};

)abc");
	}
}
