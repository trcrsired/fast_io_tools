#pragma once

namespace fast_io
{

namespace details::blake3
{
template<std::size_t a,std::size_t b,std::size_t c,std::size_t d>
inline void g(std::uint32_t<std::uint32_t,16> state, uint32_t x, uint32_t y)
{
	state[a] = state[a] + state[b] + x;
	state[d] = std::rotr(state[d] ^ state[a], 16);
	state[c] = state[c] + state[d];
	state[b] = std::rotr(state[b] ^ state[c], 12);
	state[a] = state[a] + state[b] + y;
	state[d] = std::rotr(state[d] ^ state[a], 8);
	state[c] = state[c] + state[d];
	state[b] = std::rotr(state[b] ^ state[c], 7);
}
}


class blake3_function
{
public:
	using digest_type = ::fast_io::freestanding::array<std::uint32_t,16>;
	static inline constexpr digest_type digest_initial_value{0x6a09e667f3bcc908, 0xbb67ae8584caa73b,0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,0x510e527fade682d1, 0x9b05688c2b3e6c1f,0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
	static inline constexpr std::size_t block_size{64};
	void operator()(std::span<std::uint32_t,16> state,std::span<std::byte const,block_size> blocks)
	{
	}
};

}