#include<fast_io.h>
#include<fast_io_device.h>

inline void prt_rg(fast_io::u8obuf_file& obf,char8_t ch_start,char8_t ch_last)
{
	for(char8_t ch{ch_start};ch<=ch_last;++ch)
		if(ch==u8'\\')[[unlikely]]
			print(obf,u8"case u8\'\\':return L\'\\';");
		else
			print(obf,u8"case u8\'",fast_io::mnp::chvw(ch),u8"\':return L\'",fast_io::mnp::chvw(ch),u8"\';");
}

int main()
{
	fast_io::u8obuf_file obf(u8"none_ascii.h");
	print(obf,u8R"abc(case u8'\0':return L'\0';case u8'\n':return L'\n';case u8'\t':return L'\t';)abc");
	prt_rg(obf,u8' ',u8'~');
	print(obf,u8R"abc(default: fast_terminate(); return L'\0';)abc");
}