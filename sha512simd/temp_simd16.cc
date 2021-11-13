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
u8R"abc(		simd.load(blocks_start);
		simd_constants_load.load(K512);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w);
		simd.wrap_add_assign(simd_constants_load);
		simd.store(wt);

)abc");
		}
		else
		{
			if(i<16)
			{
				print(obf,
u8R"abc(		simd.load(blocks_start+)abc",i*8u,u8R"abc();
		simd_constants_load.load(K512+)abc",i,u8R"abc();
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+)abc",i,u8R"abc();
		simd.wrap_add_assign(simd_constants_load);
		simd.store(wt+)abc",i,u8");\n");
			}
			else if(i<80)
			{
				print(obf,
u8R"abc(		simd_temp.load(w+)abc",i-15,u8R"abc();
		simd.load(w+)abc",i-2,u8R"abc();
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w)abc");
		if(i!=16)
			print(obf,u8"+",i-16);
		print(obf,u8R"abc();
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+)abc",i-7,u8R"abc();
		simd.wrap_add_assign(simd_temp);
		simd.store(w+)abc",i,u8R"abc();
		simd_constants_load.load(K512+)abc",i,u8R"abc();
		simd.wrap_add_assign(simd_constants_load);
		simd.store(wt+)abc",i,u8");\n");
			}
			for(std::size_t j{};j!=2;++j)
			{
				print(obf,u8"\t\tsha512_scalar_round(wt[",i-2+j,u8"]");
				for(std::size_t k{};k!=8u;++k)
				{
					print(obf,u8",",chvw(u8'a'+static_cast<char8_t>(((k+8u-(i-2u+j))%8u)%8u)));
				}
				print(obf,u8");\n");
			}
			println(obf);
		}
	}
}