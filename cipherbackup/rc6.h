#pragma once


namespace fast_io::crypto::rc6
{

template<bool encrypt, std::size_t keysize, std::size_t rounds = 20>
struct rc6
{
    static std::size_t constexpr block_size = 16;
	static std::size_t constexpr key_size = keysize;
    static constexpr std::uint32_t lgw = sizeof(std::uint32_t) * 8 - 1 - __builtin_clz(sizeof(std::uint32_t) * 8);
    static constexpr std::uint32_t shift_mask = (1u << (lgw + 1)) - 1;
	std::array<std::uint32_t, 2 * rounds + 4> round_keys{};
	constexpr rc6(std::span<std::byte const, keysize> key_span)
	{
		std::array<std::uint32_t, keysize / 4> tmp_key;
		memcpy(tmp_key.data(), key_span.data(), keysize);
        round_keys[0] = 0xB7E15163;
        for (std::size_t i(1); i != 2 * rounds + 4; ++i) {
            round_keys[i] = round_keys[i - 1] + 0x9E3779B9;
        }

        std::uint32_t A{}, B{};
        std::size_t i{}, j{};
        std::size_t v(3 * std::max(keysize / 4, 2 * rounds + 4));
        for (std::size_t s(1); s <= v; ++s) {
            A = round_keys[i] = std::rotl(round_keys[i] + A + B, 3);
            B = tmp_key[j] = std::rotl(tmp_key[j] + A + B, (A + B) & shift_mask);
            i = (i + 1) % (2 * rounds + 4);
            j = (j + 1) % (keysize / 4);
        }
	}
	auto operator()(std::byte const *plaintext_or_ciphertext)
	{
		if constexpr (encrypt) {
			std::array<std::uint32_t, 4> ciphertext{};
			std::array<std::byte, 16> ciphertext_bytes{};
			memcpy(std::to_address(ciphertext.data()), plaintext_or_ciphertext, 16);
			auto& A(ciphertext[0]);
			auto& B(ciphertext[1]);
			auto& C(ciphertext[2]);
			auto& D(ciphertext[3]);

			B = B + round_keys[0];
			D = D + round_keys[1];
			for (std::size_t i(1); i <= rounds; ++i) {
				std::uint32_t t(std::rotl((B * ((B << 1) | 1)), lgw));
				std::uint32_t u(std::rotl((D * ((D << 1) | 1)), lgw));
				A = (std::rotl(A ^ t, u & shift_mask)) + round_keys[2 * i];
				C = (std::rotl(C ^ u, t & shift_mask)) + round_keys[2 * i + 1];
				std::uint32_t tmp(A);
				A = B;
				B = C;
				C = D;
				D = tmp;
			}
			A = A + round_keys[2 * rounds + 2];
			C = C + round_keys[2 * rounds + 3];
			memcpy(std::to_address(ciphertext_bytes.data()), std::to_address(ciphertext.data()), 16);
			return ciphertext_bytes;
		} else {
			std::array<std::uint32_t, 4> plaintext{};
			std::array<std::byte, 16> plaintext_bytes{};
			memcpy(std::to_address(plaintext.data()), plaintext_or_ciphertext, 16);
			auto& A(plaintext[0]);
			auto& B(plaintext[1]);
			auto& C(plaintext[2]);
			auto& D(plaintext[3]);

			C = C - round_keys[2 * rounds + 3];
			A = A - round_keys[2 * rounds + 2];

			for (std::size_t i(rounds); i >= 1; --i) {
				std::uint32_t tmp(A);
				A = D;
				D = C;
				C = B;
				B = tmp;
				std::uint32_t u(std::rotl(D * ((D << 1) | 1), lgw));
				std::uint32_t t(std::rotl(B * ((B << 1) | 1), lgw));
				C = std::rotr(C - round_keys[2 * i + 1], t & shift_mask) ^ u;
				A = std::rotr(A - round_keys[2 * i], u & shift_mask) ^ t;
			}
			D = D - round_keys[1]; 
			B = B - round_keys[0];
			memcpy(std::to_address(plaintext_bytes.data()), std::to_address(plaintext.data()), 16);
			return plaintext_bytes;
		}
	}
};

using rc6_enc_128 = rc6<true, 16>;
using rc6_dec_128 = rc6<false, 16>;

using rc6_enc_192 = rc6<true, 24>;
using rc6_dec_192 = rc6<false, 24>;

using rc6_enc_256 = rc6<true, 32>;
using rc6_dec_256 = rc6<false, 32>;

}

