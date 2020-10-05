#include"../../include/fast_io.h"
#include"../../include/fast_io_device.h"
#include<vector>

int main()
{
	using namespace std::string_view_literals;
	std::vector<std::string_view> vec
	{"archive"sv,"encrypted"sv,"hidden"sv,"compressed"sv,"normal"sv,"offline"sv,"posix_semantics"sv,"app"sv,"ate"sv,"binary"sv,
		"direct"sv,"directory"sv,"excl"sv,"in"sv,"random_access"sv,"no_block"sv,"system"sv,"temporary"sv,"no_recall"sv,"session_aware"sv,"inherit"sv,"out"sv,
		"follow"sv,"sync"sv,"trunc"sv,"no_atime"sv,"no_ctty"sv,"creat"sv,"large_file"sv};
	sort(vec.begin(),vec.end());
	fast_io::obuf_file ob("mode.txt");
	std::uint64_t value{};
	for(std::size_t i{};i!=vec.size();++i)
		println(ob,vec[i],"\t");
}