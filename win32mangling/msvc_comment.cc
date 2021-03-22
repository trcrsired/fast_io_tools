#include<fast_io_device.h>
#include<fast_io.h>

int main(int argc,char** argv)
{
	if(argc<3)
	{
		perr("Usage: ",fast_io::mnp::chvw(argv[0])," <input file> <output file>\n");
		return 1;
	}
	fast_io::u8ibuf_file ibf(argv[1]);
	fast_io::u8obuf_file obf(argv[2]);
	print(obf,u8"#pragma once\n\n");
	for(auto line:line_generator(ibf))
	{
		if(line.empty())
			continue;
		auto i{line.rfind(u8' ')};
		if(i==SIZE_MAX)
		{
			print(obf,line,u8" : bad");
			continue;
		}
		print(obf,u8"#pragma comment(linker,\"/alternatename:",line.substr(0,i),u8"=",line.substr(i+1),u8"\")\n");
	}
}