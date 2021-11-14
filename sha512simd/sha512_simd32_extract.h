#pragma once

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd32_byte_swap_message_4rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,4>& __restrict simd,
	std::byte const* __restrict blocks_start,std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::size_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,4> simd_temp;
	simd.load(blocks_start+(round*8u));
	simd_temp.load(K512+round);
	if constexpr(std::endian::native==std::endian::little)
	{
		constexpr simd_vector<char,32> byteswap_simd32{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,23,22,21,20,19,18,17,16,31,30,29,28,27,26,25,24};
		simd.shuffle(byteswap_simd32);
	}
	simd.store(w+round);
	simd_temp.wrap_add_assign(simd);
	simd_temp.store(wt+round);
}

template<bool nostorew=false>
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd32_compute_message_4rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,4>& __restrict simd,
	std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::size_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,4> simd_temp{simd_vector<std::uint_least64_t,4>{simd[0],simd[1],0,0}};
	simd_temp.load(w+(round-15));
	simd_temp=(simd_temp>>19)^(simd_temp<<45)^(simd_temp>>61)^(simd_temp<<3)^(simd_temp>>6);
	simd_temp=(simd_temp>>1)^(simd_temp<<63)^(simd_temp>>8)^(simd_temp<<56)^(simd_temp>>7);
	simd_temp.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-16));
	simd_temp.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-5));
	simd_temp.wrap_add_assign(simd_temp);
	simd.wrap_add_assign(simd_vector<std::uint_least64_t,4>{0,0,simd_temp[0],simd_temp[1]});
	simd.wrap_add_assign(simd_temp);
	simd=simd_vector<std::uint_least64_t,4>{simd_temp[0],simd_temp[1],simd[2],simd[3]};
	if constexpr(nostorew)
		simd.store(w+round);
	simd_temp.load(K512+round);
	simd_temp.wrap_add_assign(simd);
	simd_temp.store(wt+round);
}

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline void sha512_simd32(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,4> simd;

	std::uint_least64_t wt[80];
	std::uint_least64_t w[76];
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
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,0);

		sha512_scalar_round(wt[0],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[1],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[2],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[3],f,g,h,a,b,c,d,e);
		
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,4);

		sha512_scalar_round(wt[4],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[5],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[6],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[7],b,c,d,e,f,g,h,a);

		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,8);

		sha512_scalar_round(wt[8],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[9],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[10],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[11],f,g,h,a,b,c,d,e);

		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,12);

		sha512_scalar_round(wt[12],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[13],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[14],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[15],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,16);

		sha512_scalar_round(wt[16],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[17],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[18],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[19],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds(simd,w,wt,20);

		sha512_scalar_round(wt[20],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[21],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[22],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[23],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,24);

		sha512_scalar_round(wt[24],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[25],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[26],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[27],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds(simd,w,wt,28);

		sha512_scalar_round(wt[28],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[29],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[30],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[31],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,32);

		sha512_scalar_round(wt[32],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[33],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[34],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[35],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds(simd,w,wt,36);

		sha512_scalar_round(wt[36],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[37],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[38],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[39],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,40);

		sha512_scalar_round(wt[40],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[41],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[42],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[43],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds(simd,w,wt,44);

		sha512_scalar_round(wt[44],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[45],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[46],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[47],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,48);

		sha512_scalar_round(wt[48],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[49],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[50],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[51],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds(simd,w,wt,52);

		sha512_scalar_round(wt[52],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[53],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[54],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[55],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,56);

		sha512_scalar_round(wt[56],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[57],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[58],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[59],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds(simd,w,wt,60);

		sha512_scalar_round(wt[60],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[61],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[62],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[63],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,64);

		sha512_scalar_round(wt[64],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[65],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[66],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[67],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds(simd,w,wt,68);

		sha512_scalar_round(wt[68],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[69],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[70],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[71],b,c,d,e,f,g,h,a);

		sha512_simd32_compute_message_4rounds(simd,w,wt,72);

		sha512_scalar_round(wt[72],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[73],h,a,b,c,d,e,f,g);
		sha512_scalar_round(wt[74],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[75],f,g,h,a,b,c,d,e);

		sha512_simd32_compute_message_4rounds<true>(simd,w,wt,76);

		sha512_scalar_round(wt[76],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[77],d,e,f,g,h,a,b,c);
		sha512_scalar_round(wt[78],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[79],b,c,d,e,f,g,h,a);

		a=(*state+=a);
		b=(state[1]+=b);
		c=(state[2]+=c);
		d=(state[3]+=d);
		e=(state[4]+=e);
		f=(state[5]+=f);
		g=(state[6]+=g);
		h=(state[7]+=h);
	}
}
