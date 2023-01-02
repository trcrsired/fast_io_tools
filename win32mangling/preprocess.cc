#include <fast_io.h>
#include <fast_io_device.h>
#include <string_view>

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		if (argc == 0)
		{
			return 1;
		}
		perr("Usage: ", fast_io::mnp::os_c_str(argv[0]), " <input file> <output file>\n");
		return 1;
	}
	fast_io::u8ibuf_file ibf(fast_io::mnp::os_c_str(argv[1]));
	fast_io::u8obuf_file obf(fast_io::mnp::os_c_str(argv[2]));
	bool additional_line{};
	for (auto& linesc : line_scanner(ibf))
	{
		std::u8string_view line{ linesc };
		if (line.empty())
			continue;
		line.remove_prefix(std::min(line.find_first_not_of(u8' '), line.size()));
		line.remove_prefix(std::min(line.find_first_not_of(u8'\t'), line.size()));
		// it do not work for arm, but still can be handled by regex substition
		if (line.starts_with(u8"mov"))
		{
			println(obf, line);
			if (additional_line)
			{
				println(obf);
				additional_line = false;
			}
			else
				additional_line = true;
		}
	}
}
