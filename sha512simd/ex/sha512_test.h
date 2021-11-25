#pragma once

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd16_byte_swap_message_2rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict simd,
	std::byte const* __restrict blocks_start,std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::uint_fast16_t round) noexcept
{
#if 0
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,2> simd_temp;
	simd.load(blocks_start+(round<<3u));
	simd_temp.load(::fast_io::details::sha512::K512+round);
	if constexpr(std::endian::native==std::endian::little)
	{
		constexpr simd_vector<char,16> byteswap_simd16{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};
		simd.shuffle(byteswap_simd16);
	}
	simd.store(w+round);
	simd_temp.wrap_add_assign(simd);
	simd_temp.store(wt+round);
#endif
}

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd16_compute_message_2rounds(
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict simd,
	std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::uint_fast8_t round) noexcept
{
#if 0
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,2> simd_temp;
	simd_temp.load(w+(round-15));
	simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
	simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
	simd.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-16));
	simd.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-7));
	simd.wrap_add_assign(simd_temp);
	simd.store(w+round);
	simd_temp.load(::fast_io::details::sha512::K512+round);
	simd_temp.wrap_add_assign(simd);
	simd_temp.store(wt+round);
#endif
}

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline void sha512_simd16(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,2> simd;

	std::uint_least64_t wt[80];
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
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,0);
		sha512_scalar_round(wt[0],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[1],h,a,b,c,d,e,f,g);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,2);

		sha512_scalar_round(wt[2],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[3],f,g,h,a,b,c,d,e);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,4);

		sha512_scalar_round(wt[4],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[5],d,e,f,g,h,a,b,c);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,6);

		sha512_scalar_round(wt[6],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[7],b,c,d,e,f,g,h,a);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,8);

		sha512_scalar_round(wt[8],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[9],h,a,b,c,d,e,f,g);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,10);

		sha512_scalar_round(wt[10],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[11],f,g,h,a,b,c,d,e);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,12);

		sha512_scalar_round(wt[12],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[13],d,e,f,g,h,a,b,c);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt,14);

		sha512_scalar_round(wt[14],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[15],b,c,d,e,f,g,h,a);


		for(std::uint_fast8_t i{16};i!=80;i+=8)
		{
			sha512_simd16_compute_message_2rounds(simd,w,wt,i);
			sha512_scalar_round(wt[i],a,b,c,d,e,f,g,h);
			sha512_scalar_round(wt[i+1],h,a,b,c,d,e,f,g);
			sha512_simd16_compute_message_2rounds(simd,w,wt,i+2);

			sha512_scalar_round(wt[i+2],g,h,a,b,c,d,e,f);
			sha512_scalar_round(wt[i+3],f,g,h,a,b,c,d,e);
			sha512_simd16_compute_message_2rounds(simd,w,wt,i+4);

			sha512_scalar_round(wt[i+4],e,f,g,h,a,b,c,d);
			sha512_scalar_round(wt[i+5],d,e,f,g,h,a,b,c);
			sha512_simd16_compute_message_2rounds(simd,w,wt,i+6);

			sha512_scalar_round(wt[i+6],c,d,e,f,g,h,a,b);
			sha512_scalar_round(wt[i+7],b,c,d,e,f,g,h,a);
#if 0
			sha512_simd16_compute_message_2rounds(simd,w,wt,i+8);

			sha512_scalar_round(wt[i+8],a,b,c,d,e,f,g,h);
			sha512_scalar_round(wt[i+9],h,a,b,c,d,e,f,g);
			sha512_simd16_compute_message_2rounds(simd,w,wt,i+10);

			sha512_scalar_round(wt[i+10],g,h,a,b,c,d,e,f);
			sha512_scalar_round(wt[i+11],f,g,h,a,b,c,d,e);
			sha512_simd16_compute_message_2rounds(simd,w,wt,i+12);

			sha512_scalar_round(wt[i+12],e,f,g,h,a,b,c,d);
			sha512_scalar_round(wt[i+13],d,e,f,g,h,a,b,c);
			sha512_simd16_compute_message_2rounds(simd,w,wt,i+14);

			sha512_scalar_round(wt[i+14],c,d,e,f,g,h,a,b);
			sha512_scalar_round(wt[i+15],b,c,d,e,f,g,h,a);

#endif
		}
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
