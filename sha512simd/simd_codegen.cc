#include<fast_io.h>
#include<fast_io_device.h>


inline void code_gen(fast_io::u8obuf_file& obf,std::size_t magic,bool transform=false)
{
	constexpr char8_t perm[]{u8'a',u8'b',u8'c',u8'd',u8'e',u8'f',u8'h',u8'g'};
	using namespace fast_io::mnp;
	std::size_t vec_size{static_cast<std::size_t>(magic*8u)};
	print(obf,
u8R"abc(#pragma once

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline void sha512_simd)abc",vec_size,u8R"abc((std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,)abc",magic,u8R"abc(> simd;
	simd_vector<std::uint_least64_t,)abc",magic,u8R"abc(> simd_temp;
	simd_vector<std::uint_least64_t,)abc",magic,u8R"abc(> simd_constants_load;
	constexpr simd_vector<char,)abc",vec_size,u8R"abc(> byteswap_simd{)abc");
	
	for(std::size_t i{};i!=magic;++i)
	{
		for(std::size_t j{8};j--;)
		{
			if(!(i==0&&j==7))
				print(obf,u8",");
			print(obf,i*8+j);
		}
	}

	print(obf,u8R"abc(};

	constexpr bool is_little_endian{std::endian::native==std::endian::little};

	std::uint_least64_t wt[80];
	std::uint_least64_t w[")abc",static_cast<std::size_t>(80u-magic),u8R"abc(];
	std::uint_least64_t a{state[0]};
	std::uint_least64_t b{state[1]};
	std::uint_least64_t c{state[2]};
	std::uint_least64_t d{state[3]};
	std::uint_least64_t e{state[4]};
	std::uint_least64_t f{state[5]};
	std::uint_least64_t g{state[6]};
	std::uint_least64_t h{state[7]};

	for(;blocks_start!=blocks_last;blocks_start+=128)
	{
)abc");
	for(std::size_t i{};i<=80u;i+=magic)
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
			println(obf);
		}
	}
	print(obf,u8R"abc(a=(*state+=a);
		b=(state[1]+=b);
		c=(state[2]+=c);
		d=(state[3]+=d);
		e=(state[4]+=e);
		f=(state[5]+=f);
		g=(state[6]+=g);
		h=(state[7]+=h);
	}
}
)abc");
}

int main()
{
	fast_io::u8obuf_file obf(u8"sha512_simd16.h");
	code_gen(obf,2,true);
	obf.reopen(u8"sha512_simd32.h");
	code_gen(obf,4);
	obf.reopen(u8"sha512_simd64.h");
	code_gen(obf,8);
}
