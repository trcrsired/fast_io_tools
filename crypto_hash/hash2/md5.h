#pragma once

namespace fast_io
{

namespace details::md5
{

enum class operation
{
F,G,H,I
};

template<operation op>
inline constexpr auto unit(auto const x,auto const y,auto const z) noexcept
{
	if constexpr(op==operation::F)
		return (x&y)|(~x&z);
	else if constexpr(op==operation::G)
		return (x&z)|(y&~z);
	else if constexpr(op==operation::H)
		return x^y^z;
	else
		return y^(x|(~z));
}

template<operation op>
inline constexpr auto uu_impl(auto a,auto b,auto c,auto d,auto x,auto s,auto ac) noexcept
{
	return std::rotl(a+unit<op>(b,c,d)+x+ac,s)+b;
}

template<operation op>
inline constexpr void uu(auto& a,auto b,auto c,auto d,auto x,auto s,auto ac) noexcept
{
	a=uu_impl<op>(a,b,c,d,x,s,ac);
}

inline
#if __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void md5_main(std::uint32_t * __restrict state,std::byte const* __restrict block_start,std::size_t block_bytes) noexcept
{
	std::uint32_t x[16];
	std::uint32_t a{*state},b{state[1]},c{state[2]},d{state[3]};
	constexpr std::size_t block_size{64};
	for(auto block(block_start),ed(block_start+block_bytes);block!=ed;block+=block_size)
	{
#if __cpp_lib_is_constant_evaluated >= 201811L
		if(std::is_constant_evaluated())
		{
			for(std::size_t j{};j!=16;++j)
			{
				auto dj{block+j*4};
				x[j]=(std::to_integer<std::uint32_t>(*dj))|(std::to_integer<std::uint32_t>(dj[1])<<8)|
					(std::to_integer<std::uint32_t>(dj[2])<<16)|std::to_integer<std::uint32_t>(dj[3]<<24);
			}
		}
		else
#endif
		{
			my_memcpy(x,block,block_size);
		}
		uu<operation::F>(a, b, c, d, x[ 0], 7, 0xd76aa478u);
		uu<operation::F>(d, a, b, c, x[ 1], 12, 0xe8c7b756u);
		uu<operation::F>(c, d, a, b, x[ 2], 17, 0x242070dbu);
		uu<operation::F>(b, c, d, a, x[ 3], 22, 0xc1bdceeeu);
		uu<operation::F>(a, b, c, d, x[ 4], 7, 0xf57c0fafu);
		uu<operation::F>(d, a, b, c, x[ 5], 12, 0x4787c62au);
		uu<operation::F>(c, d, a, b, x[ 6], 17, 0xa8304613u);
		uu<operation::F>(b, c, d, a, x[ 7], 22, 0xfd469501u);
		uu<operation::F>(a, b, c, d, x[ 8], 7, 0x698098d8u);
		uu<operation::F>(d, a, b, c, x[ 9], 12, 0x8b44f7afu);
		uu<operation::F>(c, d, a, b, x[10], 17, 0xffff5bb1u);
		uu<operation::F>(b, c, d, a, x[11], 22, 0x895cd7beu);
		uu<operation::F>(a, b, c, d, x[12], 7, 0x6b901122u);
		uu<operation::F>(d, a, b, c, x[13], 12, 0xfd987193u);
		uu<operation::F>(c, d, a, b, x[14], 17, 0xa679438eu);
		uu<operation::F>(b, c, d, a, x[15], 22, 0x49b40821u);

		/* Round 2 */
		uu<operation::G>(a, b, c, d, x[ 1], 5, 0xf61e2562u);
		uu<operation::G>(d, a, b, c, x[ 6], 9, 0xc040b340u);
		uu<operation::G>(c, d, a, b, x[11], 14, 0x265e5a51u);
		uu<operation::G>(b, c, d, a, x[ 0], 20, 0xe9b6c7aau);
		uu<operation::G>(a, b, c, d, x[ 5], 5, 0xd62f105du);
		uu<operation::G>(d, a, b, c, x[10], 9,  0x2441453u);
		uu<operation::G>(c, d, a, b, x[15], 14, 0xd8a1e681u);
		uu<operation::G>(b, c, d, a, x[ 4], 20, 0xe7d3fbc8u);
		uu<operation::G>(a, b, c, d, x[ 9], 5, 0x21e1cde6u);
		uu<operation::G>(d, a, b, c, x[14], 9, 0xc33707d6u);
		uu<operation::G>(c, d, a, b, x[ 3], 14, 0xf4d50d87u);
		uu<operation::G>(b, c, d, a, x[ 8], 20, 0x455a14edu);
		uu<operation::G>(a, b, c, d, x[13], 5, 0xa9e3e905u);
		uu<operation::G>(d, a, b, c, x[ 2], 9, 0xfcefa3f8u);
		uu<operation::G>(c, d, a, b, x[ 7], 14, 0x676f02d9u);
		uu<operation::G>(b, c, d, a, x[12], 20, 0x8d2a4c8au);

		/* Round 3 */
		uu<operation::H>(a, b, c, d, x[ 5], 4, 0xfffa3942u);
		uu<operation::H>(d, a, b, c, x[ 8], 11, 0x8771f681u);
		uu<operation::H>(c, d, a, b, x[11], 16, 0x6d9d6122u);
		uu<operation::H>(b, c, d, a, x[14], 23, 0xfde5380cu);
		uu<operation::H>(a, b, c, d, x[ 1], 4, 0xa4beea44u);
		uu<operation::H>(d, a, b, c, x[ 4], 11, 0x4bdecfa9u);
		uu<operation::H>(c, d, a, b, x[ 7], 16, 0xf6bb4b60u);
		uu<operation::H>(b, c, d, a, x[10], 23, 0xbebfbc70u);
		uu<operation::H>(a, b, c, d, x[13], 4, 0x289b7ec6u);
		uu<operation::H>(d, a, b, c, x[ 0], 11, 0xeaa127fau);
		uu<operation::H>(c, d, a, b, x[ 3], 16, 0xd4ef3085u);
		uu<operation::H>(b, c, d, a, x[ 6], 23,  0x4881d05u);
		uu<operation::H>(a, b, c, d, x[ 9], 4, 0xd9d4d039u);
		uu<operation::H>(d, a, b, c, x[12], 11, 0xe6db99e5u);
		uu<operation::H>(c, d, a, b, x[15], 16, 0x1fa27cf8u);
		uu<operation::H>(b, c, d, a, x[ 2], 23, 0xc4ac5665u);

		/* Round 4 */
		uu<operation::I>(a, b, c, d, x[ 0], 6, 0xf4292244u);
		uu<operation::I>(d, a, b, c, x[ 7], 10, 0x432aff97u);
		uu<operation::I>(c, d, a, b, x[14], 15, 0xab9423a7u);
		uu<operation::I>(b, c, d, a, x[ 5], 21, 0xfc93a039u);
		uu<operation::I>(a, b, c, d, x[12], 6, 0x655b59c3u);
		uu<operation::I>(d, a, b, c, x[ 3], 10, 0x8f0ccc92u);
		uu<operation::I>(c, d, a, b, x[10], 15, 0xffeff47du);
		uu<operation::I>(b, c, d, a, x[ 1], 21, 0x85845dd1u);
		uu<operation::I>(a, b, c, d, x[ 8], 6, 0x6fa87e4fu);
		uu<operation::I>(d, a, b, c, x[15], 10, 0xfe2ce6e0u);
		uu<operation::I>(c, d, a, b, x[ 6], 15, 0xa3014314u);
		uu<operation::I>(b, c, d, a, x[13], 21, 0x4e0811a1u);
		uu<operation::I>(a, b, c, d, x[ 4], 6, 0xf7537e82u);
		uu<operation::I>(d, a, b, c, x[11], 10, 0xbd3af235u);
		uu<operation::I>(c, d, a, b, x[ 2], 15, 0x2ad7d2bbu);
		uu<operation::I>(b, c, d, a, x[ 9], 21, 0xeb86d391u);

		a=(*state+=a);
		b=(state[1]+=b);
		c=(state[2]+=c);
		d=(state[3]+=d);
	}
}

}

class md5_function
{
public:
	using digest_type = ::fast_io::freestanding::array<std::uint32_t,4>;
	static inline constexpr digest_type digest_initial_value{0x67452301,0xefcdab89,0x98badcfe,0x10325476};
	static inline constexpr std::size_t block_size{64};
#if __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void operator()(std::uint32_t *state,std::byte const* block_start,std::size_t block_bytes) noexcept
	{
		details::md5::md5_main(state,block_start,block_bytes);
	}
};

}