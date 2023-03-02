#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<vector>
#include<algorithm>

int main()
{
	fast_io::ibuf_file ibf("open_mode.in.txt");
	fast_io::obuf_file obf("open_mode.out.txt");
	std::vector<std::pair<std::string,std::string>> vec;
	for(std::string str1,str2;scan<true>(ibf,str1,fast_io::mnp::line_get(str2));vec.emplace_back(std::move(str1),std::move(str2)));		
	std::ranges::sort(vec);
	print(obf,"none=0,\n//\t*indicates that the open mode has not been evaluated yet\n");
	for(std::size_t i{};i!=vec.size();++i)
		println(obf,vec[i].first,"=UINT64_C(1)<<",i,",\n//",vec[i].second);
}
