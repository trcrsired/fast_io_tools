#pragma once

namespace fast_io
{

namespace details
{

inline constexpr void U64TO8(std::byte *p, std::uint64_t v) noexcept {
	p[0] = static_cast<std::byte>((v      ) & 0xff);
	p[1] = static_cast<std::byte>((v >>  8) & 0xff);
	p[2] = static_cast<std::byte>((v >> 16) & 0xff);
	p[3] = static_cast<std::byte>((v >> 24) & 0xff);
	p[4] = static_cast<std::byte>((v >> 32) & 0xff);
	p[5] = static_cast<std::byte>((v >> 40) & 0xff);
	p[6] = static_cast<std::byte>((v >> 48) & 0xff);
	p[7] = static_cast<std::byte>((v >> 56) & 0xff);
}
inline constexpr std::uint64_t U8TO64(std::byte const *p) noexcept {
	return
		(((std::uint64_t)(std::to_integer<std::uint64_t>(p[0]))      ) |
		(( std::uint64_t)(std::to_integer<std::uint64_t>(p[1])) <<  8) |
		(( std::uint64_t)(std::to_integer<std::uint64_t>(p[2])) << 16) |
		(( std::uint64_t)(std::to_integer<std::uint64_t>(p[3])) << 24) |
		(( std::uint64_t)(std::to_integer<std::uint64_t>(p[4])) << 32) |
		(( std::uint64_t)(std::to_integer<std::uint64_t>(p[5])) << 40) |
		(( std::uint64_t)(std::to_integer<std::uint64_t>(p[6])) << 48) |
		(( std::uint64_t)(std::to_integer<std::uint64_t>(p[7])) << 56));
}

#if defined(_MSC_VER)
//TODO: MSVC support
	#include <intrin.h>

	struct uint128_t {
		std::uint64_t lo;
		std::uint64_t hi;
	};

	#define MUL(out, x, y) out.lo = _umul128((x), (y), &out.hi)
	#define ADD(out, in) { std::uint64_t t = out.lo; out.lo += in.lo; out.hi += (out.lo < t) + in.hi; }
	#define ADDLO(out, in) { std::uint64_t t = out.lo; out.lo += in; out.hi += (out.lo < t); }
	#define SHR(in, shift) (__shiftright128(in.lo, in.hi, (shift)))
	#define LO(in) (in.lo)

	#define POLY1305_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
	#if defined(__SIZEOF_INT128__)
		typedef unsigned __int128 uint128_t;
	#else
		typedef unsigned uint128_t __attribute__((mode(TI)));
	#endif

	inline constexpr void MUL(uint128_t& out, std::uint64_t x, std::uint64_t y) {out = static_cast<uint128_t>(x) * static_cast<uint128_t>(y);}
	inline constexpr void ADD(uint128_t& out, uint128_t x) {out += x;}
	inline constexpr void ADDLO(uint128_t& out, std::uint64_t x) {out += x;}
	inline constexpr auto SHR(uint128_t& in, auto shift) {return (std::uint64_t )(in >> (shift));}
	inline constexpr auto LO(uint128_t& in) {return (std::uint64_t)(in);}

#endif

}

struct poly1305
{
	inline static constexpr std::size_t block_size = 16;
	using key_type = ::fast_io::freestanding::array<std::byte,block_size>;
	key_type key;
	struct poly1305_state_internal_t {
		std::uint64_t r[3];
		std::uint64_t h[3];
		std::uint64_t pad[2];
		std::size_t leftover;
		unsigned char buffer[block_size];
		unsigned char final;
	} internal_state;
	::fast_io::freestanding::array<unsigned char, 16> mac;
	poly1305(std::span<std::byte const> init_key)
	{
		key_type key = {};
		if (init_key.size()>32) {
			::fast_io::details::my_memcpy(key.data(),init_key.data(),32);
		} else {
			::fast_io::details::my_memcpy(key.data(),init_key.data(),init_key.size());
		}
		poly1305_state_internal_t *st = &internal_state;
		std::uint64_t t0,t1;

		/* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
		t0 = details::U8TO64(&key[0]);
		t1 = details::U8TO64(&key[8]);

		st->r[0] = ( t0                    ) & 0xffc0fffffff;
		st->r[1] = ((t0 >> 44) | (t1 << 20)) & 0xfffffc0ffff;
		st->r[2] = ((t1 >> 24)             ) & 0x00ffffffc0f;

		/* h = 0 */
		st->h[0] = 0;
		st->h[1] = 0;
		st->h[2] = 0;

		/* save pad for later */
		st->pad[0] = details::U8TO64(&key[16]);
		st->pad[1] = details::U8TO64(&key[24]);

		st->leftover = 0;
		st->final = 0;
	}
	poly1305(::fast_io::freestanding::string_view key):poly1305(std::as_bytes(std::span{key.data(),key.size()})){}
	std::size_t block_init(std::span<std::byte,block_size> sp)
	{
		return 0;
	}
	void process_single_block(std::span<std::byte const> process_block)
	{
		poly1305_state_internal_t *st = &internal_state;
		const std::uint64_t hibit = (st->final) ? 0 : ((std::uint64_t)1 << 40); /* 1 << 128 */
		std::uint64_t r0,r1,r2;
		std::uint64_t s1,s2;
		std::uint64_t h0,h1,h2;
		std::uint64_t c;
		uint128_t d0,d1,d2,d;

		r0 = st->r[0];
		r1 = st->r[1];
		r2 = st->r[2];

		h0 = st->h[0];
		h1 = st->h[1];
		h2 = st->h[2];

		s1 = r1 * (5 << 2);
		s2 = r2 * (5 << 2);

		using namespace details;

		std::byte const * m(process_block.data());
		{
			std::uint64_t t0,t1;

			/* h += m[i] */
			t0 = U8TO64(&m[0]);
			t1 = U8TO64(&m[8]);

			h0 += (( t0                    ) & 0xfffffffffff);
			h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff);
			h2 += (((t1 >> 24)             ) & 0x3ffffffffff) | hibit;

			/* h *= r */
			MUL(d0, h0, r0); MUL(d, h1, s2); ADD(d0, d); MUL(d, h2, s1); ADD(d0, d);
			MUL(d1, h0, r1); MUL(d, h1, r0); ADD(d1, d); MUL(d, h2, s2); ADD(d1, d);
			MUL(d2, h0, r2); MUL(d, h1, r1); ADD(d2, d); MUL(d, h2, r0); ADD(d2, d);

			/* (partial) h %= p */
						c = SHR(d0, 44); h0 = LO(d0) & 0xfffffffffff;
			ADDLO(d1, c); c = SHR(d1, 44); h1 = LO(d1) & 0xfffffffffff;
			ADDLO(d2, c); c = SHR(d2, 42); h2 = LO(d2) & 0x3ffffffffff;
			h0  += c * 5; c = (h0 >> 44);  h0 =    h0  & 0xfffffffffff;
			h1  += c;
		}

		st->h[0] = h0;
		st->h[1] = h1;
		st->h[2] = h2;
	}
	void operator()(std::span<std::byte const> process_blocks)
	{
		std::size_t num_blocks(process_blocks.size() / block_size);
		for (std::size_t i(0); i < num_blocks; ++i) {
			process_single_block(process_blocks.subspan(i * block_size, block_size));
		}
	}
	void operator()(std::span<std::byte const,block_size> process_block)
	{
		process_single_block(process_block);
	}
	void digest(std::span<std::byte const> final_block)
	{
		poly1305_state_internal_t *st = &internal_state;
		std::uint64_t h0,h1,h2,c;
		std::uint64_t g0,g1,g2;
		std::uint64_t t0,t1;

		// pad zero and process last block
		::fast_io::freestanding::array<std::byte, block_size> block_padded = {};
		::fast_io::details::my_memcpy(block_padded.data(), final_block.data(), final_block.size());
		(*this)(std::span<std::byte const,block_size>(block_padded));

		/* fully carry h */
		h0 = st->h[0];
		h1 = st->h[1];
		h2 = st->h[2];

					c = (h1 >> 44); h1 &= 0xfffffffffff;
		h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
		h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
		h1 += c;     c = (h1 >> 44); h1 &= 0xfffffffffff;
		h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
		h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
		h1 += c;

		/* compute h + -p */
		g0 = h0 + 5; c = (g0 >> 44); g0 &= 0xfffffffffff;
		g1 = h1 + c; c = (g1 >> 44); g1 &= 0xfffffffffff;
		g2 = h2 + c - ((std::uint64_t)1 << 42);

		/* select h if h < p, or h + -p if h >= p */
		c = (g2 >> ((sizeof(std::uint64_t) * 8) - 1)) - 1;
		g0 &= c;
		g1 &= c;
		g2 &= c;
		c = ~c;
		h0 = (h0 & c) | g0;
		h1 = (h1 & c) | g1;
		h2 = (h2 & c) | g2;

		/* h = (h + pad) */
		t0 = st->pad[0];
		t1 = st->pad[1];

		h0 += (( t0                    ) & 0xfffffffffff)    ; c = (h0 >> 44); h0 &= 0xfffffffffff;
		h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff) + c; c = (h1 >> 44); h1 &= 0xfffffffffff;
		h2 += (((t1 >> 24)             ) & 0x3ffffffffff) + c;                 h2 &= 0x3ffffffffff;

		/* mac = h % (2^128) */
		h0 = ((h0      ) | (h1 << 44));
		h1 = ((h1 >> 20) | (h2 << 24));

		details::U64TO8(reinterpret_cast<std::byte *>(&mac[0]), h0);
		details::U64TO8(reinterpret_cast<std::byte *>(&mac[8]), h1);
	}
};

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,poly1305>)
{
	return 32;
}

template<std::integral char_type,::fast_io::freestanding::random_access_iterator caiter>
inline constexpr caiter print_reserve_define(io_reserve_type_t<char_type,poly1305>,caiter iter,auto& i)
{
	auto to_hex([](unsigned char ch){return ch>=10?ch-10+'A':ch+'0';});
	constexpr std::size_t offset{1};
	for(auto e : i.mac)
	{
		unsigned char hi(e>>4);
		unsigned char lo(e&0x0F);
		*iter = to_hex(hi);
		iter+=1;
		*iter = to_hex(lo);
		iter+=1;
	}
	return iter;
}


}