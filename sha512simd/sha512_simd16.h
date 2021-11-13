#pragma once

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline void sha512_simd16(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,2> simd,simd_temp,simd_temp1;
	constexpr simd_vector<char,16> byteswap_simd{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};

	constexpr bool is_little_endian{std::endian::native==std::endian::little};

	std::uint_least64_t wt[80];
	std::uint_least64_t w[78];
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
		simd.load(blocks_start);
		simd_temp.load(K512);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt);

		sha512_scalar_round(wt[0],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[1],h,a,b,c,d,e,f,g);
		simd.load(blocks_start+16);
		simd_temp.load(K512+2);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+2);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+2);

		sha512_scalar_round(wt[2],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[3],f,g,h,a,b,c,d,e);
		simd.load(blocks_start+32);
		simd_temp.load(K512+4);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+4);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+4);

		sha512_scalar_round(wt[4],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[5],d,e,f,g,h,a,b,c);
		simd.load(blocks_start+48);
		simd_temp.load(K512+6);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+6);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+6);

		sha512_scalar_round(wt[6],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[7],b,c,d,e,f,g,h,a);
		simd.load(blocks_start+64);
		simd_temp.load(K512+8);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+8);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+8);

		sha512_scalar_round(wt[8],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[9],h,a,b,c,d,e,f,g);
		simd.load(blocks_start+80);
		simd_temp.load(K512+10);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+10);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+10);

		sha512_scalar_round(wt[10],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[11],f,g,h,a,b,c,d,e);
		simd.load(blocks_start+96);
		simd_temp.load(K512+12);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+12);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+12);

		sha512_scalar_round(wt[12],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[13],d,e,f,g,h,a,b,c);
		simd.load(blocks_start+112);
		simd_temp.load(K512+14);
		if constexpr(is_little_endian)
			simd.shuffle(byteswap_simd);
		simd.store(w+14);
		simd_temp1=simd;
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+14);

		sha512_scalar_round(wt[14],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[15],b,c,d,e,f,g,h,a);
		simd_temp.load(w+1);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+9);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+16);
		simd_temp1=simd;
		simd_temp.load(K512+16);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+16);

		sha512_scalar_round(wt[16],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[17],h,a,b,c,d,e,f,g);
		simd_temp.load(w+3);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+2);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+11);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+18);
		simd_temp1=simd;
		simd_temp.load(K512+18);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+18);

		sha512_scalar_round(wt[18],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[19],f,g,h,a,b,c,d,e);
		simd_temp.load(w+5);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+4);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+13);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+20);
		simd_temp1=simd;
		simd_temp.load(K512+20);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+20);

		sha512_scalar_round(wt[20],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[21],d,e,f,g,h,a,b,c);
		simd_temp.load(w+7);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+6);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+15);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+22);
		simd_temp1=simd;
		simd_temp.load(K512+22);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+22);

		sha512_scalar_round(wt[22],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[23],b,c,d,e,f,g,h,a);
		simd_temp.load(w+9);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+8);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+17);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+24);
		simd_temp1=simd;
		simd_temp.load(K512+24);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+24);

		sha512_scalar_round(wt[24],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[25],h,a,b,c,d,e,f,g);
		simd_temp.load(w+11);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+10);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+19);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+26);
		simd_temp1=simd;
		simd_temp.load(K512+26);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+26);

		sha512_scalar_round(wt[26],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[27],f,g,h,a,b,c,d,e);
		simd_temp.load(w+13);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+12);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+21);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+28);
		simd_temp1=simd;
		simd_temp.load(K512+28);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+28);

		sha512_scalar_round(wt[28],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[29],d,e,f,g,h,a,b,c);
		simd_temp.load(w+15);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+14);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+23);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+30);
		simd_temp1=simd;
		simd_temp.load(K512+30);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+30);

		sha512_scalar_round(wt[30],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[31],b,c,d,e,f,g,h,a);
		simd_temp.load(w+17);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+16);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+25);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+32);
		simd_temp1=simd;
		simd_temp.load(K512+32);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+32);

		sha512_scalar_round(wt[32],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[33],h,a,b,c,d,e,f,g);
		simd_temp.load(w+19);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+18);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+27);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+34);
		simd_temp1=simd;
		simd_temp.load(K512+34);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+34);

		sha512_scalar_round(wt[34],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[35],f,g,h,a,b,c,d,e);
		simd_temp.load(w+21);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+20);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+29);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+36);
		simd_temp1=simd;
		simd_temp.load(K512+36);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+36);

		sha512_scalar_round(wt[36],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[37],d,e,f,g,h,a,b,c);
		simd_temp.load(w+23);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+22);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+31);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+38);
		simd_temp1=simd;
		simd_temp.load(K512+38);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+38);

		sha512_scalar_round(wt[38],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[39],b,c,d,e,f,g,h,a);
		simd_temp.load(w+25);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+24);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+33);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+40);
		simd_temp1=simd;
		simd_temp.load(K512+40);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+40);

		sha512_scalar_round(wt[40],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[41],h,a,b,c,d,e,f,g);
		simd_temp.load(w+27);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+26);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+35);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+42);
		simd_temp1=simd;
		simd_temp.load(K512+42);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+42);

		sha512_scalar_round(wt[42],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[43],f,g,h,a,b,c,d,e);
		simd_temp.load(w+29);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+28);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+37);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+44);
		simd_temp1=simd;
		simd_temp.load(K512+44);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+44);

		sha512_scalar_round(wt[44],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[45],d,e,f,g,h,a,b,c);
		simd_temp.load(w+31);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+30);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+39);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+46);
		simd_temp1=simd;
		simd_temp.load(K512+46);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+46);

		sha512_scalar_round(wt[46],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[47],b,c,d,e,f,g,h,a);
		simd_temp.load(w+33);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+32);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+41);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+48);
		simd_temp1=simd;
		simd_temp.load(K512+48);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+48);

		sha512_scalar_round(wt[48],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[49],h,a,b,c,d,e,f,g);
		simd_temp.load(w+35);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+34);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+43);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+50);
		simd_temp1=simd;
		simd_temp.load(K512+50);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+50);

		sha512_scalar_round(wt[50],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[51],f,g,h,a,b,c,d,e);
		simd_temp.load(w+37);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+36);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+45);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+52);
		simd_temp1=simd;
		simd_temp.load(K512+52);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+52);

		sha512_scalar_round(wt[52],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[53],d,e,f,g,h,a,b,c);
		simd_temp.load(w+39);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+38);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+47);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+54);
		simd_temp1=simd;
		simd_temp.load(K512+54);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+54);

		sha512_scalar_round(wt[54],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[55],b,c,d,e,f,g,h,a);
		simd_temp.load(w+41);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+40);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+49);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+56);
		simd_temp1=simd;
		simd_temp.load(K512+56);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+56);

		sha512_scalar_round(wt[56],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[57],h,a,b,c,d,e,f,g);
		simd_temp.load(w+43);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+42);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+51);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+58);
		simd_temp1=simd;
		simd_temp.load(K512+58);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+58);

		sha512_scalar_round(wt[58],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[59],f,g,h,a,b,c,d,e);
		simd_temp.load(w+45);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+44);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+53);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+60);
		simd_temp1=simd;
		simd_temp.load(K512+60);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+60);

		sha512_scalar_round(wt[60],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[61],d,e,f,g,h,a,b,c);
		simd_temp.load(w+47);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+46);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+55);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+62);
		simd_temp1=simd;
		simd_temp.load(K512+62);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+62);

		sha512_scalar_round(wt[62],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[63],b,c,d,e,f,g,h,a);
		simd_temp.load(w+49);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+48);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+57);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+64);
		simd_temp1=simd;
		simd_temp.load(K512+64);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+64);

		sha512_scalar_round(wt[64],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[65],h,a,b,c,d,e,f,g);
		simd_temp.load(w+51);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+50);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+59);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+66);
		simd_temp1=simd;
		simd_temp.load(K512+66);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+66);

		sha512_scalar_round(wt[66],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[67],f,g,h,a,b,c,d,e);
		simd_temp.load(w+53);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+52);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+61);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+68);
		simd_temp1=simd;
		simd_temp.load(K512+68);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+68);

		sha512_scalar_round(wt[68],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[69],d,e,f,g,h,a,b,c);
		simd_temp.load(w+55);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+54);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+63);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+70);
		simd_temp1=simd;
		simd_temp.load(K512+70);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+70);

		sha512_scalar_round(wt[70],c,d,e,f,g,h,a,b);
		sha512_scalar_round(wt[71],b,c,d,e,f,g,h,a);
		simd_temp.load(w+57);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+65);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+72);
		simd_temp1=simd;
		simd_temp.load(K512+72);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+72);

		sha512_scalar_round(wt[72],a,b,c,d,e,f,g,h);
		sha512_scalar_round(wt[73],h,a,b,c,d,e,f,g);
		simd_temp.load(w+59);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+58);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+67);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+74);
		simd_temp1=simd;
		simd_temp.load(K512+74);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+74);

		sha512_scalar_round(wt[74],g,h,a,b,c,d,e,f);
		sha512_scalar_round(wt[75],f,g,h,a,b,c,d,e);
		simd_temp.load(w+61);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+60);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+69);
		simd.wrap_add_assign(simd_temp);
		simd.store(w+76);
		simd_temp1=simd;
		simd_temp.load(K512+76);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+76);

		sha512_scalar_round(wt[76],e,f,g,h,a,b,c,d);
		sha512_scalar_round(wt[77],d,e,f,g,h,a,b,c);
		simd_temp.load(w+63);
		simd=(((((simd>>42)^simd)>>13)^simd)>>6)^(((simd<<42)^simd)<<3);
		simd_temp=(((((simd_temp>>1)^simd_temp)>>6)^simd_temp)>>1)^(((simd_temp<<7)^simd_temp)<<56);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+62);
		simd.wrap_add_assign(simd_temp);
		simd_temp.load(w+71);
		simd.wrap_add_assign(simd_temp);
		simd_temp1=simd;
		simd_temp.load(K512+78);
		simd_temp1.wrap_add_assign(simd_temp);
		simd_temp1.store(wt+78);

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
