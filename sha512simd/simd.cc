#include<fast_io.h>
#include<array>
#include<fast_io_crypto.h>

//void test(std::uint_least64_t,std::uint_least64_t) noexcept;

inline constexpr void sha512_scalar_round(std::uint_least64_t T1k,std::uint_least64_t a,std::uint_least64_t b,std::uint_least64_t c,std::uint_least64_t& __restrict d,
				std::uint_least64_t e,std::uint_least64_t f,std::uint_least64_t g,std::uint_least64_t& __restrict h) noexcept
{
	using namespace fast_io::details::sha512;
	T1k+=h+Sigma1(e)+Ch(e,f,g);
	h=Sigma0(a)+Maj(a,b,c);
	d+=T1k;
	h+=T1k;
}

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
void sha512_simd16(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,2> simd16;
	simd_vector<std::uint_least64_t,2> simd16_temp;
	simd_vector<std::uint_least64_t,2> simd16_temp2;
	simd_vector<std::uint_least64_t,2> simd16_constants_load;
	constexpr simd_vector<char,16> byteswap_simd16{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};

	std::uint_least64_t w[80];
	std::uint_least64_t a{state[0]};
	std::uint_least64_t b{state[1]};
	std::uint_least64_t c{state[2]};
	std::uint_least64_t d{state[3]};
	std::uint_least64_t e{state[4]};
	std::uint_least64_t f{state[5]};
	std::uint_least64_t g{state[6]};
	std::uint_least64_t h{state[7]};

	for(;blocks_start!=blocks_last;blocks_start+=80)
	{
	simd16.load(blocks_start);
	simd16_constants_load.load(K512);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w);

	simd16.load(blocks_start+2);
	simd16_constants_load.load(K512+2);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+2);
	sha512_scalar_round(w[0],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[1],h,a,b,c,d,e,f,g);

	simd16.load(blocks_start+4);
	simd16_constants_load.load(K512+4);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+4);
	sha512_scalar_round(w[2],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[3],f,g,h,a,b,c,d,e);

	simd16.load(blocks_start+6);
	simd16_constants_load.load(K512+6);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+6);
	sha512_scalar_round(w[4],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[5],d,e,f,g,h,a,b,c);

	simd16.load(blocks_start+8);
	simd16_constants_load.load(K512+8);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+8);
	sha512_scalar_round(w[6],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[7],b,c,d,e,f,g,h,a);

	simd16.load(blocks_start+10);
	simd16_constants_load.load(K512+10);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+10);
	sha512_scalar_round(w[8],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[9],h,a,b,c,d,e,f,g);

	simd16.load(blocks_start+12);
	simd16_constants_load.load(K512+12);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+12);
	sha512_scalar_round(w[10],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[11],f,g,h,a,b,c,d,e);

	simd16.load(blocks_start+14);
	simd16_constants_load.load(K512+14);
	simd16.shuffle(byteswap_simd16);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.store(w+14);
	sha512_scalar_round(w[12],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[13],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+1);
	simd16_constants_load.load(K512+16);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+9);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+16);
	sha512_scalar_round(w[14],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[15],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+3);
	simd16_constants_load.load(K512+18);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+11);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+18);
	sha512_scalar_round(w[16],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[17],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+5);
	simd16_constants_load.load(K512+20);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+13);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+20);
	sha512_scalar_round(w[18],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[19],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+7);
	simd16_constants_load.load(K512+22);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+15);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+22);
	sha512_scalar_round(w[20],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[21],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+9);
	simd16_constants_load.load(K512+24);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+17);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+24);
	sha512_scalar_round(w[22],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[23],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+11);
	simd16_constants_load.load(K512+26);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+19);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+26);
	sha512_scalar_round(w[24],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[25],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+13);
	simd16_constants_load.load(K512+28);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+21);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+28);
	sha512_scalar_round(w[26],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[27],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+15);
	simd16_constants_load.load(K512+30);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+23);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+30);
	sha512_scalar_round(w[28],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[29],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+17);
	simd16_constants_load.load(K512+32);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+25);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+32);
	sha512_scalar_round(w[30],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[31],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+19);
	simd16_constants_load.load(K512+34);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+27);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+34);
	sha512_scalar_round(w[32],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[33],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+21);
	simd16_constants_load.load(K512+36);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+29);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+36);
	sha512_scalar_round(w[34],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[35],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+23);
	simd16_constants_load.load(K512+38);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+31);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+38);
	sha512_scalar_round(w[36],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[37],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+25);
	simd16_constants_load.load(K512+40);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+33);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+40);
	sha512_scalar_round(w[38],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[39],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+27);
	simd16_constants_load.load(K512+42);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+35);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+42);
	sha512_scalar_round(w[40],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[41],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+29);
	simd16_constants_load.load(K512+44);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+37);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+44);
	sha512_scalar_round(w[42],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[43],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+31);
	simd16_constants_load.load(K512+46);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+39);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+46);
	sha512_scalar_round(w[44],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[45],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+33);
	simd16_constants_load.load(K512+48);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+41);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+48);
	sha512_scalar_round(w[46],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[47],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+35);
	simd16_constants_load.load(K512+50);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+43);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+50);
	sha512_scalar_round(w[48],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[49],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+37);
	simd16_constants_load.load(K512+52);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+45);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+52);
	sha512_scalar_round(w[50],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[51],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+39);
	simd16_constants_load.load(K512+54);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+47);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+54);
	sha512_scalar_round(w[52],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[53],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+41);
	simd16_constants_load.load(K512+56);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+49);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+56);
	sha512_scalar_round(w[54],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[55],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+43);
	simd16_constants_load.load(K512+58);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+51);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+58);
	sha512_scalar_round(w[56],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[57],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+45);
	simd16_constants_load.load(K512+60);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+53);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+60);
	sha512_scalar_round(w[58],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[59],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+47);
	simd16_constants_load.load(K512+62);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+55);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+62);
	sha512_scalar_round(w[60],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[61],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+49);
	simd16_constants_load.load(K512+64);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+57);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+64);
	sha512_scalar_round(w[62],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[63],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+51);
	simd16_constants_load.load(K512+66);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+59);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+66);
	sha512_scalar_round(w[64],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[65],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+53);
	simd16_constants_load.load(K512+68);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+61);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+68);
	sha512_scalar_round(w[66],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[67],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+55);
	simd16_constants_load.load(K512+70);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+63);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+70);
	sha512_scalar_round(w[68],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[69],d,e,f,g,h,a,b,c);

	simd16_temp.load(w+57);
	simd16_constants_load.load(K512+72);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+65);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+72);
	sha512_scalar_round(w[70],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[71],b,c,d,e,f,g,h,a);

	simd16_temp.load(w+59);
	simd16_constants_load.load(K512+74);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+67);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+74);
	sha512_scalar_round(w[72],a,b,c,d,e,f,g,h);
	sha512_scalar_round(w[73],h,a,b,c,d,e,f,g);

	simd16_temp.load(w+61);
	simd16_constants_load.load(K512+76);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+69);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+76);
	sha512_scalar_round(w[74],g,h,a,b,c,d,e,f);
	sha512_scalar_round(w[75],f,g,h,a,b,c,d,e);

	simd16_temp.load(w+63);
	simd16_constants_load.load(K512+78);
	simd16=((((((simd16>>42)^simd16)>>13)^simd16)>>6)^((simd16<<42)^simd16))<<3;
	simd16_temp=((((((simd16_temp>>1)^simd16_temp)>>6)^simd16_temp)>>1)^((simd16_temp<<7)^simd16_temp))<<56;
	simd16_temp2.load(w+71);
	simd16.wrap_add_assign(simd16_constants_load);
	simd16.wrap_add_assign(simd16_temp);
	simd16.wrap_add_assign(simd16_temp2);
	simd16.store(w+78);
	sha512_scalar_round(w[76],e,f,g,h,a,b,c,d);
	sha512_scalar_round(w[77],d,e,f,g,h,a,b,c);


	sha512_scalar_round(w[78],c,d,e,f,g,h,a,b);
	sha512_scalar_round(w[79],b,c,d,e,f,g,h,a);

	

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

#if 0
inline constexpr std::array<std::byte,80> construct_buffer() noexcept
{
	std::array<std::byte,80> buffer;
	for(std::size_t i{};i!=80;++i)
		buffer[i]=std::byte{static_cast<std::uint_least8_t>(i)};
	return buffer;
}

int main()
{
	std::uint_least64_t states[8]{0x6a09e667f3bcc908, 0xbb67ae8584caa73b,0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,0x510e527fade682d1, 0x9b05688c2b3e6c1f,0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
	auto blocks_start{construct_buffer()};
	sha512_simd_sse(states,blocks_start.data());
}
#endif