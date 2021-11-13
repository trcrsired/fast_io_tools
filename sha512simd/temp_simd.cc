#include<fast_io.h>
#include<fast_io_device.h>

int main()
{
	fast_io::u8obuf_file obf(u8"simd16_res.txt");
	constexpr char8_t perm[]{u8'a',u8'b',u8'c',u8'd',u8'e',u8'f',u8'h',u8'g'};
	using namespace fast_io::mnp;

	for(std::size_t i{};i!=82;i+=2)
	{
		if(i==0)
		{
			print(obf,
u8R"abc(	simd16.load(blocks_start);
	simd16_constants_load.load(K512);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w);

)abc");
		}
		else if(i<80)
		{
			if(i<16)
			{
				print(obf,
u8R"abc(	simd16.load(blocks_start+)abc",i,u8R"abc();
	simd16_constants_load.load(K512+)abc",i,u8R"abc();
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+)abc",i,u8");");
				for(std::size_t j{0};j!=2;++j)
				{
					print(obf,u8"\n\tsha512_scalar_round(w[",i-2+j,u8"]");
					for(std::size_t k{};k!=8u;++k)
					{
						print(obf,u8",",chvw(u8'a'+static_cast<char8_t>(((k+8u-(i-2u+j))%8u)%8u)));
					}
					print(obf,u8");");
				}
				print(obf,u8"\n\n");
			}
		}
	}
}