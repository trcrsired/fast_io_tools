#include<fast_io.h>
#include<array>
#include<fast_io_crypto.h>

//void test(std::uint_least64_t,std::uint_least64_t) noexcept;

inline constexpr void sha512_scalar_round(std::uint64_t T1k,std::uint64_t a,std::uint64_t b,std::uint64_t c,std::uint64_t& __restrict d,
				std::uint64_t e,std::uint64_t f,std::uint64_t g,std::uint64_t& __restrict h) noexcept
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
void sha512_simd_sse(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,2> simd16;
	simd_vector<std::uint_least64_t,2> simd16_constants_load;
	constexpr simd_vector<char,16> byteswap_simd16{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};

	std::uint_least64_t w[16];
	std::uint_least64_t a{state[0]};
	std::uint_least64_t b{state[1]};
	std::uint_least64_t c{state[2]};
	std::uint_least64_t d{state[3]};
	std::uint_least64_t e{state[4]};
	std::uint_least64_t f{state[5]};
	std::uint_least64_t g{state[6]};
	std::uint_least64_t h{state[7]};

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

	

	a=(*state+=a);
	b=(state[1]+=b);
	c=(state[2]+=c);
	d=(state[3]+=d);
	e=(state[4]+=e);
	f=(state[5]+=f);
	g=(state[6]+=g);
	h=(state[7]+=h);

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