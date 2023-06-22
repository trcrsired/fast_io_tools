#include<fast_io.h>
#include<fast_io_device.h>

int main(int argc,char** argv)
{
	using namespace fast_io::io;
	if(argc<2)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",::fast_io::mnp::os_c_str(*argv)," <number>\n");
		return 1;
	}
	::fast_io::u8obuf_file obf(u8"genutfsimd.txt");

	::std::size_t n{::fast_io::to<::std::size_t>(::fast_io::mnp::os_c_str(argv[1]))};

	for(::std::size_t i{},in{n/8};i!=in;++i)
	{
		::std::size_t start{i*(n/4)};
		print(obf,
u8R"(						ret.value=__builtin_shufflevector(simvec.value,zeros.value)");
		for(::std::size_t j{};j!=n;++j)
		{
			::std::size_t toprval{n};
			if((j%4)==0)
			{
				toprval=start+j/4;
			}
			print(obf,u8",",toprval);
		}
		print(obf,
u8R"();
						ret.store(tofirst)");

		if(i)
		{
			print(obf,u8"+",start);
		}
		print(obf,u8");\n");
	}
}