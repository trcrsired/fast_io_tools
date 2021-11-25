#pragma once

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd16_byte_swap_message_2rounds(
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict simd,
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict simd_temp,
	std::byte const* __restrict blocks_start,std::uint_least64_t* __restrict w,std::uint_fast16_t round) noexcept
{
	using namespace ::fast_io::intrinsics;

	simd.load(blocks_start+(round<<3u));
	simd_temp.load(::fast_io::details::sha512::K512+round);
	if constexpr(std::endian::native==std::endian::little)
	{
		constexpr simd_vector<char,16> byteswap_simd16{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};
		simd.shuffle(byteswap_simd16);
	}
	simd.store(w+round);
	simd_temp.wrap_add_assign(simd);
//	simd_temp.store(wt+round);
}

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd16_compute_message_2rounds(
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict simd,
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict simd_temp,
	std::uint_least64_t* __restrict w,std::uint_fast8_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
//	simd_vector<std::uint_least64_t,2> simd_temp;
	simd_temp.load(w+(round-15));
#if 1
	simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
	simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
#else
	simd=(simd>>19)^(simd<<45)^(simd>>61)^(simd<<3)^(simd>>6);
	simd_temp=(simd_temp>>1)^(simd_temp<<63)^(simd_temp>>8)^(simd_temp<<56)^(simd_temp>>7);
#endif
	simd.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-16));
	simd.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-7));
	simd.wrap_add_assign(simd_temp);
	simd.store(w+round);
	simd_temp.load(::fast_io::details::sha512::K512+round);
	simd_temp.wrap_add_assign(simd);
//	simd_temp.store(wt+round);
}

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline void sha512_simd16(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,2> simd;
	simd_vector<std::uint_least64_t,2> simd_temp;

//	std::uint_least64_t wt[80];
	std::uint_least64_t w[80];
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
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,0);
		sha512_scalar_round(simd_temp[0],a,b,c,d,e,f,g,h);
		sha512_scalar_round(simd_temp[1],h,a,b,c,d,e,f,g);
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,2);

		sha512_scalar_round(simd_temp[0],g,h,a,b,c,d,e,f);
		sha512_scalar_round(simd_temp[1],f,g,h,a,b,c,d,e);
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,4);

		sha512_scalar_round(simd_temp[0],e,f,g,h,a,b,c,d);
		sha512_scalar_round(simd_temp[1],d,e,f,g,h,a,b,c);
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,6);

		sha512_scalar_round(simd_temp[0],c,d,e,f,g,h,a,b);
		sha512_scalar_round(simd_temp[1],b,c,d,e,f,g,h,a);
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,8);

		sha512_scalar_round(simd_temp[0],a,b,c,d,e,f,g,h);
		sha512_scalar_round(simd_temp[1],h,a,b,c,d,e,f,g);
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,10);

		sha512_scalar_round(simd_temp[0],g,h,a,b,c,d,e,f);
		sha512_scalar_round(simd_temp[1],f,g,h,a,b,c,d,e);
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,12);

		sha512_scalar_round(simd_temp[0],e,f,g,h,a,b,c,d);
		sha512_scalar_round(simd_temp[1],d,e,f,g,h,a,b,c);
		sha512_simd16_byte_swap_message_2rounds(simd,simd_temp,blocks_start,w,14);

		sha512_scalar_round(simd_temp[0],c,d,e,f,g,h,a,b);
		sha512_scalar_round(simd_temp[1],b,c,d,e,f,g,h,a);


		for(std::uint_fast8_t i{16};i!=64;i+=16)
		{
			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+2);
			sha512_scalar_round(simd_temp[0],a,b,c,d,e,f,g,h);
			sha512_scalar_round(simd_temp[1],h,a,b,c,d,e,f,g);
			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+4);

			sha512_scalar_round(simd_temp[0],g,h,a,b,c,d,e,f);
			sha512_scalar_round(simd_temp[1],f,g,h,a,b,c,d,e);
			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+6);

			sha512_scalar_round(simd_temp[0],e,f,g,h,a,b,c,d);
			sha512_scalar_round(simd_temp[1],d,e,f,g,h,a,b,c);
			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+8);

			sha512_scalar_round(simd_temp[0],c,d,e,f,g,h,a,b);
			sha512_scalar_round(simd_temp[1],b,c,d,e,f,g,h,a);

			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+10);

			sha512_scalar_round(simd_temp[0],a,b,c,d,e,f,g,h);
			sha512_scalar_round(simd_temp[1],h,a,b,c,d,e,f,g);
			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+12);

			sha512_scalar_round(simd_temp[0],g,h,a,b,c,d,e,f);
			sha512_scalar_round(simd_temp[1],f,g,h,a,b,c,d,e);
			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+14);

			sha512_scalar_round(simd_temp[0],e,f,g,h,a,b,c,d);
			sha512_scalar_round(simd_temp[1],d,e,f,g,h,a,b,c);
			sha512_simd16_compute_message_2rounds(simd,simd_temp,w,i+16);

			sha512_scalar_round(simd_temp[0],c,d,e,f,g,h,a,b);
			sha512_scalar_round(simd_temp[1],b,c,d,e,f,g,h,a);
		}

		sha512_simd16_compute_message_2rounds(simd,w,wt,66);
		sha512_scalar_round(wt[64],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[65],h,a,b,c,d,e,f,g);

		sha512_simd16_compute_message_2rounds(simd,w,wt,68);
		sha512_scalar_round(wt[66],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[67],f,g,h,a,b,c,d,e);

		sha512_simd16_compute_message_2rounds(simd,w,wt,70);
		sha512_scalar_round(wt[68],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[69],d,e,f,g,h,a,b,c);

		sha512_simd16_compute_message_2rounds(simd,w,wt,72);
		sha512_scalar_round(wt[70],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[71],b,c,d,e,f,g,h,a);

		sha512_simd16_compute_message_2rounds(simd,w,wt,74);
		sha512_scalar_round(wt[72],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[73],h,a,b,c,d,e,f,g);

		sha512_simd16_compute_message_2rounds(simd,w,wt,76);
		sha512_scalar_round(wt[74],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[75],f,g,h,a,b,c,d,e);

		sha512_simd16_compute_message_2rounds<true>(simd,w,wt,78);
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
