#pragma once
/*
https://github.com/noloader/SHA-Intrinsics/blob/master/sha256-x86.c
*/

namespace fast_io
{
namespace details::sha512
{
#if 0
inline constexpr std::uint64_t K512[]
{
0x428a2f98d728ae22, 0x7137449123ef65cd,
0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
0x3956c25bf348b538, 0x59f111f1b605d019,
0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
0xd807aa98a3030242, 0x12835b0145706fbe,
0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
0x72be5d74f27b896f, 0x80deb1fe3b1696b1,
0x9bdc06a725c71235, 0xc19bf174cf692694,
0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
0x2de92c6f592b0275, 0x4a7484aa6ea6e483,
0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
0x983e5152ee66dfab, 0xa831c66d2db43210,
0xb00327c898fb213f, 0xbf597fc7beef0ee4,
0xc6e00bf33da88fc2, 0xd5a79147930aa725,
0x06ca6351e003826f, 0x142929670a0e6e70,
0x27b70a8546d22ffc, 0x2e1b21385c26c926,
0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
0x650a73548baf63de, 0x766a0abb3c77b2a8,
0x81c2c92e47edaee6, 0x92722c851482353b,
0xa2bfe8a14cf10364, 0xa81a664bbc423001,
0xc24b8b70d0f89791, 0xc76c51a30654be30,
0xd192e819d6ef5218, 0xd69906245565a910,
0xf40e35855771202a, 0x106aa07032bbd1b8,
0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
0x748f82ee5defb2fc, 0x78a5636f43172f60,
0x84c87814a1f0ab72, 0x8cc702081a6439ec,
0x90befffa23631e28, 0xa4506cebde82bde9,
0xbef9a3f7b2c67915, 0xc67178f2e372532b,
0xca273eceea26619c, 0xd186b8c721c0c207,
0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
0x06f067aa72176fba, 0x0a637dc5a2c898a6,
0x113f9804bef90dae, 0x1b710b35131c471b,
0x28db77f523047d84, 0x32caab7b40c72493,
0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};
#endif


inline constexpr auto Sigma0(auto x) noexcept
{
	return std::rotr(x,28)^std::rotr(x,34)^std::rotr(x,39);
}
inline constexpr auto Sigma1(auto x) noexcept
{
	return std::rotr(x,14)^std::rotr(x,18)^std::rotr(x,41);
}

inline constexpr auto sigma0(auto x) noexcept
{
	return std::rotr(x,1)^std::rotr(x,8)^(x>>7);
}

inline constexpr auto sigma1(auto x) noexcept
{
	return std::rotr(x,19)^std::rotr(x,61)^(x>>6);
}

inline constexpr auto Ch(auto x,auto y,auto z) noexcept
{
	return (x&y)^((~x)&z);
}

inline constexpr auto Maj(auto x,auto y,auto z) noexcept
{
	return (x&y)^(x&z)^(y&z);
}


#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
#if __has_cpp_attribute(gnu::hot)
[[gnu::hot]]
#endif
inline constexpr void round_0_15(std::uint64_t T1,std::uint64_t a,std::uint64_t b,std::uint64_t c,std::uint64_t& __restrict d,
				std::uint64_t e,std::uint64_t f,std::uint64_t g,std::uint64_t& __restrict h,std::uint64_t k) noexcept
{
	T1+=h+Sigma1(e)+Ch(e,f,g)+k;
	h=Sigma0(a)+Maj(a,b,c);
	d+=T1;
	h+=T1;
}

template<std::size_t j>
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
#if __has_cpp_attribute(gnu::hot)
[[gnu::hot]]
#endif
inline constexpr void round_16_80(std::uint64_t a,std::uint64_t b,std::uint64_t c,std::uint64_t& __restrict d,
				std::uint64_t e,std::uint64_t f,std::uint64_t g,std::uint64_t& __restrict h,std::uint64_t k,std::uint64_t* __restrict X) noexcept
{
	constexpr std::size_t j0{j&0x0f};
	constexpr std::size_t j1{(j+1)&0x0f};
	constexpr std::size_t j14{(j+14)&0x0f};
	constexpr std::size_t j9{(j+9)&0x0f};
	round_0_15(X[j0]+=sigma0(X[j1])+sigma1(X[j14])+X[j9],a,b,c,d,e,f,g,h,k);
}


inline void sha512_do_function(std::uint64_t* __restrict state,std::byte const* __restrict blocks_start,std::size_t blocks_bytes) noexcept
{
//	std::uint64_t X[16];

	using uint64_may_alias
#if __has_cpp_attribute(gnu::may_alias)
	[[gnu::may_alias]]
#endif
	= std::uint64_t;

	std::uint64_t sa{*state},sb{state[1]},sc{state[2]},sd{state[3]},se{state[4]},sf{state[5]},sg{state[6]},sh{state[7]};

	std::uint64_t x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15;

	for(auto data(blocks_start),ed(blocks_start+blocks_bytes);data!=ed;data+=128)
	{
		std::uint64_t a{sa},b{sb},c{sc},d{sd},e{se},f{sf},g{sg},h{sh};
		uint64_may_alias const* W{reinterpret_cast<std::uint64_t const*>(data)};
#if 0
		round_0_15(X[0]=big_endian(W[0]),a,b,c,d,e,f,g,h,0x428a2f98d728ae22);
		round_0_15(X[1]=big_endian(W[1]),h,a,b,c,d,e,f,g,0x7137449123ef65cd);
		round_0_15(X[2]=big_endian(W[2]),g,h,a,b,c,d,e,f,0xb5c0fbcfec4d3b2f);
		round_0_15(X[3]=big_endian(W[3]),f,g,h,a,b,c,d,e,0xe9b5dba58189dbbc);
		round_0_15(X[4]=big_endian(W[4]),e,f,g,h,a,b,c,d,0x3956c25bf348b538);
		round_0_15(X[5]=big_endian(W[5]),d,e,f,g,h,a,b,c,0x59f111f1b605d019);
		round_0_15(X[6]=big_endian(W[6]),c,d,e,f,g,h,a,b,0x923f82a4af194f9b);
		round_0_15(X[7]=big_endian(W[7]),b,c,d,e,f,g,h,a,0xab1c5ed5da6d8118);
		round_0_15(X[8]=big_endian(W[8]),a,b,c,d,e,f,g,h,0xd807aa98a3030242);
		round_0_15(X[9]=big_endian(W[9]),h,a,b,c,d,e,f,g,0x12835b0145706fbe);
		round_0_15(X[10]=big_endian(W[10]),g,h,a,b,c,d,e,f,0x243185be4ee4b28c);
		round_0_15(X[11]=big_endian(W[11]),f,g,h,a,b,c,d,e,0x550c7dc3d5ffb4e2);
		round_0_15(X[12]=big_endian(W[12]),e,f,g,h,a,b,c,d,0x72be5d74f27b896f);
		round_0_15(X[13]=big_endian(W[13]),d,e,f,g,h,a,b,c,0x80deb1fe3b1696b1);
		round_0_15(X[14]=big_endian(W[14]),c,d,e,f,g,h,a,b,0x9bdc06a725c71235);
		round_0_15(X[15]=big_endian(W[15]),b,c,d,e,f,g,h,a,0xc19bf174cf692694);
#else
		round_0_15(x0=big_endian(W[0]),a,b,c,d,e,f,g,h,0x428a2f98d728ae22);
		round_0_15(x1=big_endian(W[1]),h,a,b,c,d,e,f,g,0x7137449123ef65cd);
		round_0_15(x2=big_endian(W[2]),g,h,a,b,c,d,e,f,0xb5c0fbcfec4d3b2f);
		round_0_15(x3=big_endian(W[3]),f,g,h,a,b,c,d,e,0xe9b5dba58189dbbc);
		round_0_15(x4=big_endian(W[4]),e,f,g,h,a,b,c,d,0x3956c25bf348b538);
		round_0_15(x5=big_endian(W[5]),d,e,f,g,h,a,b,c,0x59f111f1b605d019);
		round_0_15(x6=big_endian(W[6]),c,d,e,f,g,h,a,b,0x923f82a4af194f9b);
		round_0_15(x7=big_endian(W[7]),b,c,d,e,f,g,h,a,0xab1c5ed5da6d8118);
		round_0_15(x8=big_endian(W[8]),a,b,c,d,e,f,g,h,0xd807aa98a3030242);
		round_0_15(x9=big_endian(W[9]),h,a,b,c,d,e,f,g,0x12835b0145706fbe);
		round_0_15(x10=big_endian(W[10]),g,h,a,b,c,d,e,f,0x243185be4ee4b28c);
		round_0_15(x11=big_endian(W[11]),f,g,h,a,b,c,d,e,0x550c7dc3d5ffb4e2);
		round_0_15(x12=big_endian(W[12]),e,f,g,h,a,b,c,d,0x72be5d74f27b896f);
		round_0_15(x13=big_endian(W[13]),d,e,f,g,h,a,b,c,0x80deb1fe3b1696b1);
		round_0_15(x14=big_endian(W[14]),c,d,e,f,g,h,a,b,0x9bdc06a725c71235);
		round_0_15(x15=big_endian(W[15]),b,c,d,e,f,g,h,a,0xc19bf174cf692694);
#endif


#if 0
                round_16_80<0>(a,b,c,d,e,f,g,h,0xe49b69c19ef14ad2,X);
                round_16_80<1>(h,a,b,c,d,e,f,g,0xefbe4786384f25e3,X);
                round_16_80<2>(g,h,a,b,c,d,e,f,0xfc19dc68b8cd5b5,X);
                round_16_80<3>(f,g,h,a,b,c,d,e,0x240ca1cc77ac9c65,X);
                round_16_80<4>(e,f,g,h,a,b,c,d,0x2de92c6f592b0275,X);
                round_16_80<5>(d,e,f,g,h,a,b,c,0x4a7484aa6ea6e483,X);
                round_16_80<6>(c,d,e,f,g,h,a,b,0x5cb0a9dcbd41fbd4,X);
                round_16_80<7>(b,c,d,e,f,g,h,a,0x76f988da831153b5,X);
                round_16_80<8>(a,b,c,d,e,f,g,h,0x983e5152ee66dfab,X);
                round_16_80<9>(h,a,b,c,d,e,f,g,0xa831c66d2db43210,X);
                round_16_80<10>(g,h,a,b,c,d,e,f,0xb00327c898fb213f,X);
                round_16_80<11>(f,g,h,a,b,c,d,e,0xbf597fc7beef0ee4,X);
                round_16_80<12>(e,f,g,h,a,b,c,d,0xc6e00bf33da88fc2,X);
                round_16_80<13>(d,e,f,g,h,a,b,c,0xd5a79147930aa725,X);
                round_16_80<14>(c,d,e,f,g,h,a,b,0x6ca6351e003826f,X);
                round_16_80<15>(b,c,d,e,f,g,h,a,0x142929670a0e6e70,X);
                round_16_80<0>(a,b,c,d,e,f,g,h,0x27b70a8546d22ffc,X);
                round_16_80<1>(h,a,b,c,d,e,f,g,0x2e1b21385c26c926,X);
                round_16_80<2>(g,h,a,b,c,d,e,f,0x4d2c6dfc5ac42aed,X);
                round_16_80<3>(f,g,h,a,b,c,d,e,0x53380d139d95b3df,X);
                round_16_80<4>(e,f,g,h,a,b,c,d,0x650a73548baf63de,X);
                round_16_80<5>(d,e,f,g,h,a,b,c,0x766a0abb3c77b2a8,X);
                round_16_80<6>(c,d,e,f,g,h,a,b,0x81c2c92e47edaee6,X);
                round_16_80<7>(b,c,d,e,f,g,h,a,0x92722c851482353b,X);
                round_16_80<8>(a,b,c,d,e,f,g,h,0xa2bfe8a14cf10364,X);
                round_16_80<9>(h,a,b,c,d,e,f,g,0xa81a664bbc423001,X);
                round_16_80<10>(g,h,a,b,c,d,e,f,0xc24b8b70d0f89791,X);
                round_16_80<11>(f,g,h,a,b,c,d,e,0xc76c51a30654be30,X);
                round_16_80<12>(e,f,g,h,a,b,c,d,0xd192e819d6ef5218,X);
                round_16_80<13>(d,e,f,g,h,a,b,c,0xd69906245565a910,X);
                round_16_80<14>(c,d,e,f,g,h,a,b,0xf40e35855771202a,X);
                round_16_80<15>(b,c,d,e,f,g,h,a,0x106aa07032bbd1b8,X);
                round_16_80<0>(a,b,c,d,e,f,g,h,0x19a4c116b8d2d0c8,X);
                round_16_80<1>(h,a,b,c,d,e,f,g,0x1e376c085141ab53,X);
                round_16_80<2>(g,h,a,b,c,d,e,f,0x2748774cdf8eeb99,X);
                round_16_80<3>(f,g,h,a,b,c,d,e,0x34b0bcb5e19b48a8,X);
                round_16_80<4>(e,f,g,h,a,b,c,d,0x391c0cb3c5c95a63,X);
                round_16_80<5>(d,e,f,g,h,a,b,c,0x4ed8aa4ae3418acb,X);
                round_16_80<6>(c,d,e,f,g,h,a,b,0x5b9cca4f7763e373,X);
                round_16_80<7>(b,c,d,e,f,g,h,a,0x682e6ff3d6b2b8a3,X);
                round_16_80<8>(a,b,c,d,e,f,g,h,0x748f82ee5defb2fc,X);
                round_16_80<9>(h,a,b,c,d,e,f,g,0x78a5636f43172f60,X);
                round_16_80<10>(g,h,a,b,c,d,e,f,0x84c87814a1f0ab72,X);
                round_16_80<11>(f,g,h,a,b,c,d,e,0x8cc702081a6439ec,X);
                round_16_80<12>(e,f,g,h,a,b,c,d,0x90befffa23631e28,X);
                round_16_80<13>(d,e,f,g,h,a,b,c,0xa4506cebde82bde9,X);
                round_16_80<14>(c,d,e,f,g,h,a,b,0xbef9a3f7b2c67915,X);
                round_16_80<15>(b,c,d,e,f,g,h,a,0xc67178f2e372532b,X);
                round_16_80<0>(a,b,c,d,e,f,g,h,0xca273eceea26619c,X);
                round_16_80<1>(h,a,b,c,d,e,f,g,0xd186b8c721c0c207,X);
                round_16_80<2>(g,h,a,b,c,d,e,f,0xeada7dd6cde0eb1e,X);
                round_16_80<3>(f,g,h,a,b,c,d,e,0xf57d4f7fee6ed178,X);
                round_16_80<4>(e,f,g,h,a,b,c,d,0x6f067aa72176fba,X);
                round_16_80<5>(d,e,f,g,h,a,b,c,0xa637dc5a2c898a6,X);
                round_16_80<6>(c,d,e,f,g,h,a,b,0x113f9804bef90dae,X);
                round_16_80<7>(b,c,d,e,f,g,h,a,0x1b710b35131c471b,X);
                round_16_80<8>(a,b,c,d,e,f,g,h,0x28db77f523047d84,X);
                round_16_80<9>(h,a,b,c,d,e,f,g,0x32caab7b40c72493,X);
                round_16_80<10>(g,h,a,b,c,d,e,f,0x3c9ebe0a15c9bebc,X);
                round_16_80<11>(f,g,h,a,b,c,d,e,0x431d67c49c100d4c,X);
                round_16_80<12>(e,f,g,h,a,b,c,d,0x4cc5d4becb3e42b6,X);
                round_16_80<13>(d,e,f,g,h,a,b,c,0x597f299cfc657e2a,X);
                round_16_80<14>(c,d,e,f,g,h,a,b,0x5fcb6fab3ad6faec,X);
                round_16_80<15>(b,c,d,e,f,g,h,a,0x6c44198c4a475817,X);
#else
		
#endif
		sa+=a;
		sb+=b;
		sc+=c;
		sd+=d;
		se+=e;
		sf+=f;
		sg+=g;
		sh+=h;
	}
	*state=sa;
	state[1]=sb;
	state[2]=sc;
	state[3]=sd;
	state[4]=se;
	state[5]=sf;
	state[6]=sg;
	state[7]=sh;
}

}
class sha512_function
{
public:
	using digest_type = ::fast_io::freestanding::array<std::uint64_t,8>;
	static inline constexpr digest_type digest_initial_value{0x6a09e667f3bcc908, 0xbb67ae8584caa73b,0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,0x510e527fade682d1, 0x9b05688c2b3e6c1f,0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
	static inline constexpr std::size_t block_size{128};
	void operator()(std::uint64_t* __restrict state,std::byte const* blocks_start,std::size_t blocks_bytes) noexcept
	{
		details::sha512::sha512_do_function(state,blocks_start,blocks_bytes);
	}
};


}