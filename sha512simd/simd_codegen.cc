#include<fast_io.h>
#include<fast_io_device.h>

inline void code_gen(fast_io::u8obuf_file& obf,std::size_t magic,bool transform=false)
{
	constexpr char8_t perm[]{u8'a',u8'b',u8'c',u8'd',u8'e',u8'f',u8'h',u8'g'};
	using namespace fast_io::mnp;

	for(std::size_t i{};i<=80;i+=magic)
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
		simd.load(w+)abc",i-2,u8");\n");
		if(transform)
			print(obf,u8R"abc(		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);)abc");
		else
			print(obf,u8R"abc(		simd=(simd>>19)^(simd<<45)^(simd>>61)^(simd<<3)^(simd>>6);
		simd_temp=(simd_temp>>1)^(simd_temp<<63)^(simd_temp>>8)^(simd_temp<<56)^(simd_temp>>7);)abc");			
		print(obf,u8R"abc(
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w)abc");
		if(i!=16)
			print(obf,u8"+",i-16);
		print(obf,u8R"abc();
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+)abc",i-7,u8R"abc();
		simd.wrap_add_assign(simd_temp);
)abc");
		if(i+magic!=80)
			print(obf,u8R"abc(		simd.store(w+)abc",i,u8");\n");
		print(obf,u8R"abc(		simd_constants_load.load(K512+)abc",i,u8R"abc();
		simd.wrap_add_assign(simd_constants_load);
		simd.store(wt+)abc",i,u8");\n");
			}
			for(std::size_t j{};j!=magic;++j)
			{
				std::size_t pos{i-magic+j};
				print(obf,u8"\t\tsha512_scalar_round(wt[",i-magic+j,u8"]");
				for(std::size_t k{};k!=8u;++k)
				{
					print(obf,u8",",chvw(u8'a'+static_cast<char8_t>(((k+8u-pos)%8u)%8u)));
				}
				print(obf,u8");\n");
			}
			if(i!=80)
				println(obf);
		}
	}
}

int main()
{
	fast_io::u8obuf_file obf(u8"simd16_res.txt");
	code_gen(obf,2,true);
	obf.reopen(u8"simd32_res.txt");
	code_gen(obf,4);
	obf.reopen(u8"simd64_res.txt");
	code_gen(obf,8);
}
