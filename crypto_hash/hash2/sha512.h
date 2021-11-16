#pragma once
/*
https://github.com/noloader/SHA-Intrinsics/blob/master/sha256-x86.c
*/

namespace fast_io
{
namespace details::sha512
{
inline constexpr std::uint_least64_t K512[]
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
inline constexpr void sha512_scalar_round(std::uint_least64_t T1k,std::uint_least64_t a,std::uint_least64_t b,std::uint_least64_t c,std::uint_least64_t& __restrict d,
				std::uint_least64_t e,std::uint_least64_t f,std::uint_least64_t g,std::uint_least64_t& __restrict h) noexcept
{
	T1k+=h+Sigma1(e)+Ch(e,f,g);
	h=Sigma0(a)+Maj(a,b,c);
	d+=T1k;
	h+=T1k;
}

inline constexpr void sha512_do_constexpr_function(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::size_t blocks_bytes) noexcept
{
	std::uint_least64_t a{state[0]}, b{state[1]}, c{state[2]}, d{state[3]}, e{state[4]}, f{state[5]}, g{state[6]}, h{state[7]}, s0, s1, T1, T2;
	std::uint_least64_t X[16];
	for(auto data(blocks_start),ed(blocks_start+blocks_bytes);data!=ed;)
	{
		std::uint32_t i{};
		for (; i < 16; ++i)
		{
			if (std::is_constant_evaluated())
			{
				std::uint_least64_t value{};
				for(std::size_t j{};j!=8;++j)
				{
					value<<=8u;
					value|=std::to_integer<std::uint_least64_t>(data[j]);
				}
				X[i] = value;
			}
			else
			{
				std::uint_least64_t value;
				::fast_io::details::my_memcpy(__builtin_addressof(value),data,8);
				X[i] = big_endian(value);
			}
			data += 8;

			T1 = h;
			T1 += Sigma1(e);
			T1 += Ch(e, f, g);
			T1 += K512[i];
			T1 += X[i];

			T2 = Sigma0(a);
			T2 += Maj(a, b, c);

			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		for (i = 16; i < 80; ++i)
		{
			s0 = X[(i + 1) & 0x0f];
			s0 = sigma0(s0);
			s1 = X[(i + 14) & 0x0f];
			s1 = sigma1(s1);

			T1 = X[i & 0xf] += s0 + s1 + X[(i + 9) & 0xf];
			T1 += h + Sigma1(e) + Ch(e, f, g) + K512[i];
			T2 = Sigma0(a) + Maj(a, b, c);

			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		a=(state[0] += a);
		b=(state[1] += b);
		c=(state[2] += c);
		d=(state[3] += d);
		e=(state[4] += e);
		f=(state[5] += f);
		g=(state[6] += g);
		h=(state[7] += h);
	}
}

#if !(defined(__OPTIMIZE_SIZE__) || (defined(_MSC_VER) && !defined(__clang__)))
#if __has_builtin(__builtin_ia32_pshufb256) && !defined(DISABLE_SIMD32)
#include"sha512_simd32_extract.h"
#elif __has_builtin(__builtin_ia32_pshufb128) && defined(__SSE3__)
#include"sha512_simd16.h"
#endif

#endif
}
class sha512_function
{
public:
	using digest_type = ::fast_io::freestanding::array<std::uint_least64_t,8>;
	static inline constexpr digest_type digest_initial_value{0x6a09e667f3bcc908, 0xbb67ae8584caa73b,0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,0x510e527fade682d1, 0x9b05688c2b3e6c1f,0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
	static inline constexpr std::size_t block_size{128};
	inline
#if __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void operator()(std::uint_least64_t* __restrict state,std::byte const* blocks_start,std::size_t blocks_bytes) noexcept
	{
#if defined(__OPTIMIZE_SIZE__) || (defined(_MSC_VER) && !defined(__clang__))
/*
optimization of msvc is very bad
*/
		details::sha512::sha512_do_constexpr_function(state,blocks_start,blocks_bytes);
#else
#if __cpp_if_consteval >= 202106L
		if consteval
		{
			::fast_io::details::sha512::sha512_do_constexpr_function(state,blocks_start,blocks_bytes);
		}
		else
#elif __cpp_lib_is_constant_evaluated >= 201811L
		if(std::is_constant_evaluated())
		{
			::fast_io::details::sha512::sha512_do_constexpr_function(state,blocks_start,blocks_bytes);
		}
		else
#endif
		{
#if __has_builtin(__builtin_ia32_pshufb256) && !defined(DISABLE_SIMD32)
			::fast_io::details::sha512::sha512_simd32(state,blocks_start,blocks_start+blocks_bytes);
#elif __has_builtin(__builtin_ia32_pshufb128) && defined(__SSE3__)
			::fast_io::details::sha512::sha512_simd16(state,blocks_start,blocks_start+blocks_bytes);
#else
			::fast_io::details::sha512::sha512_do_constexpr_function(state,blocks_start,blocks_bytes);
#endif

		}
#endif
	}
};


}