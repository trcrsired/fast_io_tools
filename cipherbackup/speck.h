#pragma once


namespace fast_io::crypto::speck
{

namespace details
{

inline constexpr std::pair<uint64_t, uint64_t> speck_round(uint64_t x, uint64_t y, uint64_t k) {
    x = std::rotr(x, 8);
    x += y;
    x ^= k;
    y = std::rotl(y, 3);
    y ^= x;
    return {x, y};
}

inline constexpr std::pair<uint64_t, uint64_t> speck_back(uint64_t x, uint64_t y, uint64_t k) {
    y ^= x;
    y = std::rotr(y, 3);
    x ^= k;
    x -= y;
    x = std::rotl(x, 8);
    return {x, y};
}

}

template<bool encrypt, std::size_t blocksize, std::size_t keysize, std::size_t rounds>
struct speck
{
    static std::size_t constexpr block_size = blocksize;
    static std::size_t constexpr key_size = keysize;
    std::array<uint64_t, rounds + 1> key_schedule{};
    constexpr speck(std::span<std::byte const, keysize> key_span)
    {
        std::byte const *key(key_span.data());
        std::array<uint64_t, key_size / sizeof(uint64_t)> subkeys{};
        memcpy(subkeys.data(), key, key_size);

        key_schedule[0] = subkeys[0];
        for (uint64_t i = 0; i != rounds - 1; ++i) {
            auto [a, b] = details::speck_round(subkeys[1], subkeys[0], i);
            if constexpr (key_size == 32)
            {
                subkeys[0] = b;
                subkeys[1] = subkeys[2];
                subkeys[2] = subkeys[3];
                subkeys[3] = a;
            } else if constexpr (key_size == 24) {
                subkeys[0] = b;
                subkeys[1] = subkeys[2];
                subkeys[2] = a;
            } else {
                subkeys[0] = b;
                subkeys[1] = a;
            }
            key_schedule[i + 1] = subkeys[0];
        }
    }
    auto operator()(std::byte const *plaintext_or_ciphertext)
    {
        if constexpr (encrypt) {
            std::array<std::byte, block_size> ciphertext{};
            std::array<uint64_t, 2> cipher_as_uint64_t;
            std::array<uint64_t, 2> plain_as_uint64_t;
            //memcpy(cipher_as_uint64_t.data(), ciphertext.data(), blocksize);
            //auto cipher_as_uint64_t = static_cast<uint64_t*>(static_cast<void*>(ciphertext.data()));
            memcpy(plain_as_uint64_t.data(), plaintext_or_ciphertext, blocksize);
            //auto plain_as_uint64_t = static_cast<uint64_t const*>(static_cast<void const*>(plaintext_or_ciphertext));

            cipher_as_uint64_t[0] = plain_as_uint64_t[0];
            cipher_as_uint64_t[1] = plain_as_uint64_t[1];
            for (std::size_t i = 0; i != rounds; ++i) {
                auto [a, b] = details::speck_round(cipher_as_uint64_t[1], cipher_as_uint64_t[0], key_schedule[i]);
                cipher_as_uint64_t[1] = a;
                cipher_as_uint64_t[0] = b;
            }
            memcpy(ciphertext.data(), cipher_as_uint64_t.data(), blocksize);
            return ciphertext;
        } else {
            std::array<std::byte, block_size> plaintext{};

            std::array<uint64_t, 2> cipher_as_uint64_t;
            std::array<uint64_t, 2> plain_as_uint64_t;
            //memcpy(plain_as_uint64_t.data(), plaintext.data(), blocksize);
            memcpy(cipher_as_uint64_t.data(), plaintext_or_ciphertext, blocksize);

            //auto plain_as_uint64_t = static_cast<uint64_t*>(static_cast<void*>(plaintext.data()));
            //auto cipher_as_uint64_t = static_cast<uint64_t const*>(static_cast<void const*>(plaintext_or_ciphertext));

            plain_as_uint64_t[0] = cipher_as_uint64_t[0];
            plain_as_uint64_t[1] = cipher_as_uint64_t[1];
            for (std::size_t i(rounds); i--;) {
                auto [a, b] = details::speck_back(plain_as_uint64_t[1], plain_as_uint64_t[0], key_schedule[i]);
                plain_as_uint64_t[1] = a;
                plain_as_uint64_t[0] = b;
            }

            memcpy(plaintext.data(), plain_as_uint64_t.data(), blocksize);
            return plaintext;
        }
    }
};

using speck_enc_128_128 = speck<true, 16, 16, 32>;
using speck_dec_128_128 = speck<false, 16, 16, 32>;

using speck_enc_128_192 = speck<true, 16, 24, 33>;
using speck_dec_128_192 = speck<false, 16, 24, 33>;

using speck_enc_128_256 = speck<true, 16, 32, 34>;
using speck_dec_128_256 = speck<false, 16, 32, 34>;

}

