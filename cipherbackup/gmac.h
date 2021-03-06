#pragma once

namespace fast_io
{

namespace crypto::details
{

inline constexpr std::uint64_t gf128_last4[16] = {
    0x0000, 0x1c20, 0x3840, 0x2460, 0x7080, 0x6ca0, 0x48c0, 0x54e0,
    0xe100, 0xfd20, 0xd940, 0xc560, 0x9180, 0x8da0, 0xa9c0, 0xb5e0  };

inline constexpr void gf128_mult(std::array<std::uint64_t, 16> const& HL,
								 std::array<std::uint64_t, 16> const& HH,
                				 std::span<std::byte const> const& x,    // pointer to 128-bit input vector
                				 std::span<std::byte>& output)    // pointer to 128-bit output vector
{
    std::uint8_t rem;

    std::uint8_t lo(static_cast<std::uint8_t>(static_cast<std::uint8_t>(x[15]) & 0x0f));
    std::uint8_t hi(static_cast<std::uint8_t>(static_cast<std::uint8_t>(x[15]) >> 4));
    std::uint64_t zh(static_cast<std::uint64_t>(HH[lo]));
    std::uint64_t zl(static_cast<std::uint64_t>(HL[lo]));

    for (std::size_t i(16); i--;) {
        lo = static_cast<std::uint8_t>(static_cast<std::uint8_t>(x[i]) & 0x0f);
        hi = static_cast<std::uint8_t>(static_cast<std::uint8_t>(x[i]) >> 4);

        if (i != 15) {
            rem = static_cast<std::uint8_t>(zl & 0x0f);
            zl = (zh << 60) | (zl >> 4);
            zh = (zh >> 4);
            zh ^= static_cast<std::uint64_t>(gf128_last4[rem] << 48);
            zh ^= HH[lo];
            zl ^= HL[lo];
        }
        rem = static_cast<std::uint8_t>(zl & 0x0f);
        zl = (zh << 60) | (zl >> 4);
        zh = (zh >> 4);
        zh ^= static_cast<std::uint64_t>(gf128_last4[rem] << 48);
        zh ^= HH[hi];
        zl ^= HL[hi];
    }
	output[0    ] = static_cast<std::byte>( (zh >> 32) >> 24 );
	output[1    ] = static_cast<std::byte>( (zh >> 32) >> 16 );
	output[2    ] = static_cast<std::byte>( (zh >> 32) >>  8 );
	output[3    ] = static_cast<std::byte>( (zh >> 32)       );
	output[4    ] = static_cast<std::byte>( (zh) >> 24 );
	output[5    ] = static_cast<std::byte>( (zh) >> 16 );
	output[6    ] = static_cast<std::byte>( (zh) >>  8 );
	output[7    ] = static_cast<std::byte>( (zh)       );

	output[8    ] = static_cast<std::byte>( (zl >> 32) >> 24 );
	output[9    ] = static_cast<std::byte>( (zl >> 32) >> 16 );
	output[10   ] = static_cast<std::byte>( (zl >> 32) >>  8 );
	output[11   ] = static_cast<std::byte>( (zl >> 32)       );
	output[12   ] = static_cast<std::byte>( (zl) >> 24 );
	output[13   ] = static_cast<std::byte>( (zl) >> 16 );
	output[14   ] = static_cast<std::byte>( (zl) >>  8 );
	output[15   ] = static_cast<std::byte>( (zl)       );
}

inline constexpr std::uint64_t GET_UINT32_BE(std::array<std::byte, 16> const& h, std::size_t i) {
	return  (static_cast<std::uint32_t>(static_cast<std::uint32_t>(h[i])    ) << 24 ) |
			(static_cast<std::uint32_t>(static_cast<std::uint32_t>(h[i]) + 1) << 16 ) |
			(static_cast<std::uint32_t>(static_cast<std::uint32_t>(h[i]) + 2) <<  8 ) |
			(static_cast<std::uint32_t>(static_cast<std::uint32_t>(h[i]) + 3)       );
}

}

template<typename block_cipher>
requires(block_cipher::block_size == 16)
struct gmac
{
	using block_type = std::array<std::byte, 16>;
	block_type state;
	std::unique_ptr<block_cipher> cipher;
    std::array<std::uint64_t, 16> HL;        // precalculated lo-half HTable
    std::array<std::uint64_t, 16> HH;        // precalculated lo-half HTable
	inline static constexpr std::size_t block_size = 16;
	std::uint64_t plain_size, cipher_size;
	gmac(std::span<std::byte const> init_key)
	{
		cipher.reset(new block_cipher(std::span<std::byte const, block_size>(init_key)));
		block_type empty;
		memset(empty.data(), 0, block_size);
		state = (*cipher)(std::to_address(empty.data()));

		std::uint64_t hi = fast_io::crypto::details::GET_UINT32_BE(state, 0);
		std::uint64_t lo = fast_io::crypto::details::GET_UINT32_BE(state, 4);
		std::uint64_t vh = (hi << 32) | lo;

		hi = fast_io::crypto::details::GET_UINT32_BE(state, 8);
		lo = fast_io::crypto::details::GET_UINT32_BE(state, 12);
		std::uint64_t vl = (hi << 32) | lo;

		HL[8] = vl;                // 8 = 1000 corresponds to 1 in GF(2^128)
		HH[8] = vh;
		HH[0] = 0;                 // 0 corresponds to 0 in GF(2^128)
		HL[0] = 0;

		std::size_t i, j;

		for ( i = 4; i > 0; i >>= 1 ) {
			uint32_t T = (uint32_t) ( vl & 1 ) * 0xe1000000U;
			vl  = ( vh << 63 ) | ( vl >> 1 );
			vh  = ( vh >> 1 ) ^ ( (uint64_t) T << 32);
			HL[i] = vl;
			HH[i] = vh;
		}
		for (i = 2; i < 16; i <<= 1 ) {
			uint64_t *HiL = &HL[i], *HiH = &HH[i];
			vh = *HiH;
			vl = *HiL;
			for( j = 1; j < i; ++j ) {
				HiH[j] = vh ^ HH[j];
				HiL[j] = vl ^ HL[j];
			}
		}
	}

	gmac(std::string_view key):gmac(std::as_bytes(std::span{key.data(),key.size()})){}

	// std::size_t block_init(std::span<std::byte,block_size> sp)
	// {
	// 	memcpy(sp.data(),inner_key.data(),sizeof(key_type));
	// 	return sizeof(key_type);
	// }

	template<bool plain = false>
	void operator()(std::span<std::byte const,block_size> process_block)
	{
		fast_xor_assignment(std::span(state), process_block);
		fast_io::crypto::details::gf128_mult(HL, HH, state, state);
		if constexpr (plain) {
			plain_size += block_size;
		} else {
			cipher_size += block_size;
		}
	}

	template<bool plain = false>
	void digest(std::span<std::byte const> final_block, std::span<std::byte const> nonce)
	{
		std::array<std::byte, block_size> padded{0};
		memcpy(padded.data(), final_block.data(), final_block.size());
		this->operator()<plain>(padded);
		padded[ 0] = static_cast<std::byte>(cipher_size >> 56);
		padded[ 1] = static_cast<std::byte>(cipher_size >> 48);
		padded[ 2] = static_cast<std::byte>(cipher_size >> 40);
		padded[ 3] = static_cast<std::byte>(cipher_size >> 32);
		padded[ 4] = static_cast<std::byte>(cipher_size >> 24);
		padded[ 5] = static_cast<std::byte>(cipher_size >> 16);
		padded[ 6] = static_cast<std::byte>(cipher_size >>  8);
		padded[ 7] = static_cast<std::byte>(cipher_size      );
		padded[ 8] = static_cast<std::byte>(plain_size >> 56);
		padded[ 9] = static_cast<std::byte>(plain_size >> 48);
		padded[10] = static_cast<std::byte>(plain_size >> 40);
		padded[11] = static_cast<std::byte>(plain_size >> 32);
		padded[12] = static_cast<std::byte>(plain_size >> 24);
		padded[13] = static_cast<std::byte>(plain_size >> 16);
		padded[14] = static_cast<std::byte>(plain_size >>  8);
		padded[15] = static_cast<std::byte>(plain_size      );
		this->operator()<true>(padded);
		memset(padded.data(), 0, block_size);
		memcpy(padded.data(), nonce.data(), nonce.size());
		auto state2((*cipher)(std::to_address(padded.data())));
		fast_xor_assignment(std::span(state), state2);
		// now state is tag
	}

};


template<std::integral char_type,typename T>
requires reserve_printable<char_type,T>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,gmac<T>>)
{
	return print_reserve_size(io_reserve_type<char_type,T>);
}

template<std::integral char_type,reserve_printable T,std::random_access_iterator caiter>
inline constexpr caiter print_reserve_define(io_reserve_type_t<char_type,gmac<T>>,caiter iter,auto& i)
{
	return print_reserve_define(io_reserve_type<char_type,T>,iter,i.function);
}

}