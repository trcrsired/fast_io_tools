#pragma once
//Referenced: https://en.wikipedia.org/wiki/Salsa20
namespace fast_io::crypto
{

namespace details::chacha20
{

template<std::unsigned_integral T>
inline constexpr void qr(T& a, T& b,T& c,T& d)
{
	a += b;  d ^= a;  d = std::rotl(d,16);
	c += d;  b ^= c;  b = std::rotl(b,12);
	a += b;  d ^= a;  d = std::rotl(d, 8);
	c += d;  b ^= c;  b = std::rotl(b, 7);
}

inline constexpr auto chacha_block(std::span<std::uint32_t const,16> in)
{
	std::array<std::uint32_t,16> x{};
	my_copy_n(in.data(),in.size(),x.data());
//	memcpy(x.data(),in.data(),sizeof(x));
	// 10 loops × 2 rounds/loop = 20 rounds
	for (std::size_t i{}; i!=10; ++i)
	{
		// Odd round
		qr(x[0], x[4], x[ 8], x[12]); // column 0
		qr(x[1], x[5], x[ 9], x[13]); // column 1
		qr(x[2], x[6], x[10], x[14]); // column 2
		qr(x[3], x[7], x[11], x[15]); // column 3
		// Even round
		qr(x[0], x[5], x[10], x[15]); // diagonal 1 (main diagonal)
		qr(x[1], x[6], x[11], x[12]); // diagonal 2
		qr(x[2], x[7], x[ 8], x[13]); // diagonal 3
		qr(x[3], x[4], x[ 9], x[14]); // diagonal 4
	}
	for(std::size_t i{};i!=x.size();++i)
		x[i]+=in[i];
	return x;
}

}

template<bool encrypt>
struct chacha20
{

};

}