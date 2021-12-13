#include<string>
#include<fast_io.h>
#include<fast_io_device.h>

int main()
{
	fast_io::u8ibuf_file ibf(u8"system.tbl");
	fast_io::u8obuf_file obf(u8"system.txt");
	std::u8string number,abi,name,entry_point;
	for(std::u8string str;scan<true>(ibf,fast_io::mnp::line_get(str));)
	{
		if(str.empty()||str.front()==u8'#')
			continue;
		fast_io::u8ibuffer_view ibv(str);
		scan(ibv,number,abi,name,entry_point);
		print(obf,u8"inline constexpr std::size_t linux_nr_",name,u8"{",number,u8"};\n");
	}
}
