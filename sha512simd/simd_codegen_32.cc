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

	simd_vector<std::uint_least64_t,)abc",magic,u8R"abc(> simd,simd_temp,simd_temp1;
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
	std::uint_least64_t w[)abc",80-magic,u8R"abc(];
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
		simd_temp.load(K512);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt);

)abc");
		}
		else
		{
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
			if(i<16)
			{
				print(obf,
u8R"abc(		simd.load(blocks_start+)abc",i*8u,u8R"abc();
		simd_temp.load(K512+)abc",i,u8R"abc();
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+)abc",i,u8R"abc();
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+)abc",i,u8");\n");
			}
			else if(i<80)
			{
print(obf,
u8R"abc(		simd_temp=simd_vector<std::uint_least64_t,4>{simd[0],simd[1],0,0};
		simd_temp.load(w+)abc",i-15,u8R"abc();
		simd_temp=(simd_temp>>19)^(simd_temp<<45)^(simd_temp>>61)^(simd_temp<<3)^(simd_temp>>6);
		simd_temp=(simd_temp>>1)^(simd_temp<<63)^(simd_temp>>8)^(simd_temp<<56)^(simd_temp>>7);
		simd_temp.wrap_add_assign(simd_temp);
		simd_temp.load(w)abc");
		if(i!=16)
			print(obf,u8"+",i-16);
		print(obf,u8R"abc();
		simd_temp.wrap_add_assign(simd_temp);
		simd_temp.load(w+)abc",i-5,u8R"abc();
		simd_temp.wrap_add_assign(simd_temp);
		simd.wrap_add_assign(simd_vector<std::uint_least64_t,4>{0,0,simd_temp[0],simd_temp[1]});
		simd.wrap_add_assign(simd_temp);
		simd=simd_vector<std::uint_least64_t,4>{simd_temp[0],simd_temp[1],simd[2],simd[3]};
)abc");
		if(i+magic==80)
		{
print(obf,
u8R"abc(		simd_temp.load(K512+)abc",i,u8R"abc();
		simd.wrap_add_assign(simd_temp);
		simd.store(wt+)abc",i,u8");\n");
		}
		else
		{
print(obf,u8R"abc(		simd.store(w+)abc",i,u8R"abc();
		simd_temp1=simd;
		simd_temp.load(K512+)abc",i,u8R"abc();
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+)abc",i,u8");\n");
		}
			}
			println(obf);
		}
	}
	print(obf,u8R"abc(		a=(*state+=a);
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
	fast_io::u8obuf_file obf(u8"sha512_simd32.h");
	obf.reopen(u8"sha512_simd32.h");
	code_gen(obf,4);
}
