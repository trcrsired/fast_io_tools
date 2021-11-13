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
u8R"abc(
	simd16.load(blocks_start);
	simd16_constants_load.load(K512);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w);

)abc");
		}
		else if(i<16)
		{
			print(obf,
u8R"abc(	simd16.load(blocks_start+)abc",i,u8R"abc();
	simd16_constants_load.load(K512+)abc",i,u8R"abc();
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+)abc",i,u8R"abc();
	sha512_scalar_round(w[)abc",i-2,u8"]");
			for(std::size_t k{};k!=8u;++k)
			{
				print(obf,u8",",chvw(u8'a'+static_cast<char8_t>((i+k-2u)%8u)));
			}
			print(obf,
u8R"abc();
	sha512_scalar_round(w[)abc",i-1u,u8"]");
			for(std::size_t k{};k!=8u;++k)
			{
				print(obf,u8",",chvw(u8'a'+static_cast<char8_t>((i+k-1u)%8u)));
			}
			print(obf,
u8R"abc();

)abc");
		}
		else if(i<16)
		{
		}
		else
		{
		}
	}
#if 0
	for(std::size_t i{};i!=4;++i)
		for(std::size_t j{};j!=16;++j)
		{
			print("\t\tround_0_15((x",j,"+=sigma0(x",(j+1)%16,")+sigma1(x",(j+14)%16,")+x",(j+9)%16,"),");
			for(std::size_t k{};k!=8;++k)
			{
				print(chvw('a'+(8+k-j)%8),",");
			}
			print("0x",hex(K512[i*16+j]),");\n");
		}
#endif
}