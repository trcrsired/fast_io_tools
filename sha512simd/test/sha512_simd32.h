#pragma once

inline constexpr auto create_k512x2() noexcept
{
	constexpr std::size_t N{sizeof(K512)/sizeof(std::uint_least64_t)};
	::fast_io::freestanding::array<std::uint_least64_t,N*2> a;
	for(std::size_t i{};i!=N;i+=2)
	{
		std::size_t i2{i*2};
		a[i2+2]=a[i2]=K512[i];
		a[i2+3]=a[i2+1]=K512[i+1];
	}
	return a;
}

inline constexpr auto k512x2{create_k512x2()};

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd32_byte_swap_message_2rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,4>& __restrict simd,
	std::byte const* __restrict blocks_start,std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::size_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,2> simd16_temp;
	simd_vector<std::uint_least64_t,4> simd_temp;
	simd16_temp.load(blocks_start+(round<<3u));
	std::size_t twice_round{round*2};
	simd_temp.load(k512x2.element+twice_round);
	if constexpr(std::endian::native==std::endian::little)
	{
		constexpr simd_vector<char,16> byteswap_simd16{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};
		simd16_temp.shuffle(byteswap_simd16);
	}
	simd=simd_vector<std::uint_least64_t,4>{simd16_temp[0],simd16_temp[1],simd16_temp[0],simd16_temp[1]};
	simd.store(w+twice_round);
	simd_temp.wrap_add_assign(simd);
	simd_temp.store(wt+twice_round);
}

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd32_compute_message_2rounds(
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,4>& __restrict simd,
	std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::size_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,4> simd_temp;
	simd_temp.load(w+(round-28));
	simd=(simd>>19)^(simd<<45)^(simd>>61)^(simd<<3)^(simd>>6);
	simd_temp=(simd_temp>>1)^(simd_temp<<63)^(simd_temp>>8)^(simd_temp<<56)^(simd_temp>>7);
	simd.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-32));
	simd.wrap_add_assign(simd_temp);
	simd_temp.load(w+(round-14));
	simd.wrap_add_assign(simd_temp);
	simd.store(w+round);
	simd_temp.load(k512x2.element+round);
	simd_temp.wrap_add_assign(simd);
	simd_temp.store(wt+round);
	debug_println(simd," ",simd_temp);
}

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline void sha512_simd32(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,4> simd;
	std::uint_least64_t wt[160];
	std::uint_least64_t w[160];
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
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,0);
		sha512_scalar_round(wt[0],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[1],h,a,b,c,d,e,f,g);
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,2);

		sha512_scalar_round(wt[4],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[5],f,g,h,a,b,c,d,e);
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,4);

		sha512_scalar_round(wt[8],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[9],d,e,f,g,h,a,b,c);
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,6);

		sha512_scalar_round(wt[12],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[13],b,c,d,e,f,g,h,a);
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,8);

		sha512_scalar_round(wt[16],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[17],h,a,b,c,d,e,f,g);
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,10);

		sha512_scalar_round(wt[20],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[21],f,g,h,a,b,c,d,e);
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,12);

		sha512_scalar_round(wt[24],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[25],d,e,f,g,h,a,b,c);
		sha512_simd32_byte_swap_message_2rounds(simd,blocks_start,w,wt,14);
		
		sha512_scalar_round(wt[28],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[29],b,c,d,e,f,g,h,a);

		for(std::size_t i{32};i!=160;i+=32)
		{
			sha512_simd32_compute_message_2rounds(simd,w,wt,i);
			sha512_scalar_round(wt[i],a,b,c,d,e,f,g,h);
			sha512_scalar_round(wt[i+1],h,a,b,c,d,e,f,g);
			sha512_simd32_compute_message_2rounds(simd,w,wt,i+4);

			sha512_scalar_round(wt[i+4],g,h,a,b,c,d,e,f);
			sha512_scalar_round(wt[i+5],f,g,h,a,b,c,d,e);
			sha512_simd32_compute_message_2rounds(simd,w,wt,i+8);

			sha512_scalar_round(wt[i+8],e,f,g,h,a,b,c,d);
			sha512_scalar_round(wt[i+9],d,e,f,g,h,a,b,c);
			sha512_simd32_compute_message_2rounds(simd,w,wt,i+12);

			sha512_scalar_round(wt[i+12],c,d,e,f,g,h,a,b);
			sha512_scalar_round(wt[i+13],b,c,d,e,f,g,h,a);

			sha512_simd32_compute_message_2rounds(simd,w,wt,i+16);

			sha512_scalar_round(wt[i+16],a,b,c,d,e,f,g,h);
			sha512_scalar_round(wt[i+17],h,a,b,c,d,e,f,g);
			sha512_simd32_compute_message_2rounds(simd,w,wt,i+20);

			sha512_scalar_round(wt[i+20],g,h,a,b,c,d,e,f);
			sha512_scalar_round(wt[i+21],f,g,h,a,b,c,d,e);
			sha512_simd32_compute_message_2rounds(simd,w,wt,i+24);

			sha512_scalar_round(wt[i+24],e,f,g,h,a,b,c,d);
			sha512_scalar_round(wt[i+25],d,e,f,g,h,a,b,c);
			sha512_simd32_compute_message_2rounds(simd,w,wt,i+28);

			sha512_scalar_round(wt[i+28],c,d,e,f,g,h,a,b);
			sha512_scalar_round(wt[i+29],b,c,d,e,f,g,h,a);
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
