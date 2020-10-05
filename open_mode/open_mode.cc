#include<fast_io.h>
#include<fast_io_device.h>
#include<vector>

int main()
{
	fast_io::ibuf_file ibf("open_mode.in.txt");
	fast_io::obuf_file obf("open_mode.out.txt");
	std::vector<std::pair<std::string,std::string>> vec;
	for(std::string str1,str2;scan<true>(ibf,str1,fast_io::line(str2));vec.emplace_back(std::move(str1),std::move(str2)));		
	std::ranges::sort(vec);
	print(obf,"none=0,\n");
	for(std::size_t i{};i!=vec.size();++i)
		println(obf,vec[i].first,"=1<<",i,",\n\\\\",vec[i].second);
}

