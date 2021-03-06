
#pragma once

#ifdef __x86_64__
//requires -maes -msse2
#include <wmmintrin.h>  // for intrinsics for AES-NI

namespace fast_io::crypto::aes
{

namespace detail
{

inline __m128i aes_128_key_exp_common(__m128i key) {
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	return key;
}

inline __m128i aes_128_key_expansion(__m128i key, __m128i keygened) {
	keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3, 3, 3, 3));
	key = aes_128_key_exp_common(key);
	return _mm_xor_si128(key, keygened);
}

inline __m128i aes_128_key_exp(__m128i k, int rcon) {
	return aes_128_key_expansion(k, _mm_aeskeygenassist_si128(k, rcon));
}

inline __m128i aes_192_key_exp_2(__m128i key, __m128i key2) {
	key = _mm_shuffle_epi32(key, 0xff);
	key2 = _mm_xor_si128(key2, _mm_slli_si128(key2, 4));
	return _mm_xor_si128(key, key2);
}
inline __m128i aes_192_key_exp(__m128i key, __m128i key2, int I) {
	return _mm_xor_si128(aes_128_key_exp_common(key), _mm_shuffle_epi32(_mm_aeskeygenassist_si128(key2, I), 0x55));
}

inline __m128i aes_256_key_exp(__m128i key, __m128i key2, int I) {
	return _mm_xor_si128(aes_128_key_exp_common(key), _mm_shuffle_epi32(_mm_aeskeygenassist_si128(key2, I), 0xff));
}

inline __m128i aes_256_key_exp_2(__m128i key, __m128i key2) {
	return _mm_xor_si128(aes_128_key_exp_common(key), _mm_shuffle_epi32(_mm_aeskeygenassist_si128(key2, 0x00), 0xff));
}

}

template<bool encrypt, std::size_t keysize, std::size_t use_aesni = 2>
struct aes
{
	static std::size_t constexpr block_size = 16;
	static std::size_t constexpr key_size = keysize;
	__m128i key_schedule[15];
	__m128i key_schedule_dec[15];
	aes(std::span<std::byte const, keysize> key_span)
	{
		std::byte const *key(key_span.data());
		// key schedule for encryption
		if constexpr (keysize == 16)
		{
			key_schedule[0]  = _mm_loadu_si128(static_cast<__m128i const*>(static_cast<void const*>(key)));
			key_schedule[1]  = detail::aes_128_key_exp(key_schedule[0], 0x01);
			key_schedule[2]  = detail::aes_128_key_exp(key_schedule[1], 0x02);
			key_schedule[3]  = detail::aes_128_key_exp(key_schedule[2], 0x04);
			key_schedule[4]  = detail::aes_128_key_exp(key_schedule[3], 0x08);
			key_schedule[5]  = detail::aes_128_key_exp(key_schedule[4], 0x10);
			key_schedule[6]  = detail::aes_128_key_exp(key_schedule[5], 0x20);
			key_schedule[7]  = detail::aes_128_key_exp(key_schedule[6], 0x40);
			key_schedule[8]  = detail::aes_128_key_exp(key_schedule[7], 0x80);
			key_schedule[9]  = detail::aes_128_key_exp(key_schedule[8], 0x1B);
			key_schedule[10] = detail::aes_128_key_exp(key_schedule[9], 0x36);
		}
		else if constexpr (keysize == 24)
		{
			__m128i temp[2]{};
			__m128i key2{};
			memcpy(std::addressof(key2), key, 8);
			memcpy(key_schedule, key, 24);
			//key_schedule[0] = _mm_loadu_si128(static_cast<__m128i const*>(static_cast<void const*>(key)));
			key_schedule[1] = _mm_loadu_si128(std::addressof(key2));
			temp[0] = detail::aes_192_key_exp(key_schedule[0], key_schedule[1], 0x01);
			temp[1] = detail::aes_192_key_exp_2(temp[0], key_schedule[1]);
			key_schedule[1] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[1]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[2] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]), reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[3] = detail::aes_192_key_exp(temp[0], temp[1], 0x02);
			key_schedule[4] = detail::aes_192_key_exp_2(key_schedule[3], temp[1]);
			temp[0] = detail::aes_192_key_exp(key_schedule[3], key_schedule[4], 0x04);
			temp[1] = detail::aes_192_key_exp_2(temp[0], key_schedule[4]);
			key_schedule[4] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[4]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[5] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]), reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[6] = detail::aes_192_key_exp(temp[0], temp[1], 0x08);
			key_schedule[7] = detail::aes_192_key_exp_2(key_schedule[6], temp[1]);
			temp[0] = detail::aes_192_key_exp(key_schedule[6], key_schedule[7], 0x10);
			temp[1] = detail::aes_192_key_exp_2(temp[0], key_schedule[7]);
			key_schedule[7] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[7]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[8] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]), reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[9] = detail::aes_192_key_exp(temp[0], temp[1], 0x20);
			key_schedule[10] = detail::aes_192_key_exp_2(key_schedule[9], temp[1]);
			temp[0] = detail::aes_192_key_exp(key_schedule[9], key_schedule[10], 0x40);
			temp[1] = detail::aes_192_key_exp_2(temp[0], key_schedule[10]);
			key_schedule[10] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[10]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[11] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]),reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[12] = detail::aes_192_key_exp(temp[0], temp[1], 0x80);
		}
		else if constexpr (keysize == 32)
		{
			memcpy(key_schedule, key, 32);
			//key_schedule[0] = _mm_loadu_si128(static_cast<__m128i const*>(static_cast<void const*>(key)));
			key_schedule[1] = _mm_loadu_si128(static_cast<__m128i const*>(static_cast<void const*>(key + 16)));
			key_schedule[2] = detail::aes_256_key_exp(key_schedule[0], key_schedule[1], 0x01);
			key_schedule[3] = detail::aes_256_key_exp_2(key_schedule[1], key_schedule[2]);
			key_schedule[4] = detail::aes_256_key_exp(key_schedule[2], key_schedule[3], 0x02);
			key_schedule[5] = detail::aes_256_key_exp_2(key_schedule[3], key_schedule[4]);
			key_schedule[6] = detail::aes_256_key_exp(key_schedule[4], key_schedule[5], 0x04);
			key_schedule[7] = detail::aes_256_key_exp_2(key_schedule[5], key_schedule[6]);
			key_schedule[8] = detail::aes_256_key_exp(key_schedule[6], key_schedule[7], 0x08);
			key_schedule[9] = detail::aes_256_key_exp_2(key_schedule[7], key_schedule[8]);
			key_schedule[10] = detail::aes_256_key_exp(key_schedule[8], key_schedule[9], 0x10);
			key_schedule[11] = detail::aes_256_key_exp_2(key_schedule[9], key_schedule[10]);
			key_schedule[12] = detail::aes_256_key_exp(key_schedule[10], key_schedule[11], 0x20);
			key_schedule[13] = detail::aes_256_key_exp_2(key_schedule[11], key_schedule[12]);
			key_schedule[14] = detail::aes_256_key_exp(key_schedule[12], key_schedule[13], 0x40);
		}

		// key schedule for decryption
		if constexpr (!encrypt)
		{
			if constexpr (keysize == 16)
			{
				key_schedule_dec[0] = key_schedule[10];
				key_schedule_dec[1] = _mm_aesimc_si128(key_schedule[9]);
				key_schedule_dec[2] = _mm_aesimc_si128(key_schedule[8]);
				key_schedule_dec[3] = _mm_aesimc_si128(key_schedule[7]);
				key_schedule_dec[4] = _mm_aesimc_si128(key_schedule[6]);
				key_schedule_dec[5] = _mm_aesimc_si128(key_schedule[5]);
				key_schedule_dec[6] = _mm_aesimc_si128(key_schedule[4]);
				key_schedule_dec[7] = _mm_aesimc_si128(key_schedule[3]);
				key_schedule_dec[8] = _mm_aesimc_si128(key_schedule[2]);
				key_schedule_dec[9] = _mm_aesimc_si128(key_schedule[1]);
				key_schedule_dec[10] = key_schedule[0];
			}
			else if constexpr (keysize == 24)
			{
				key_schedule_dec[ 0] = key_schedule[12];
				key_schedule_dec[ 1] = _mm_aesimc_si128(key_schedule[11]);
				key_schedule_dec[ 2] = _mm_aesimc_si128(key_schedule[10]);
				key_schedule_dec[ 3] = _mm_aesimc_si128(key_schedule[ 9]);
				key_schedule_dec[ 4] = _mm_aesimc_si128(key_schedule[ 8]);
				key_schedule_dec[ 5] = _mm_aesimc_si128(key_schedule[ 7]);
				key_schedule_dec[ 6] = _mm_aesimc_si128(key_schedule[ 6]);
				key_schedule_dec[ 7] = _mm_aesimc_si128(key_schedule[ 5]);
				key_schedule_dec[ 8] = _mm_aesimc_si128(key_schedule[ 4]);
				key_schedule_dec[ 9] = _mm_aesimc_si128(key_schedule[ 3]);
				key_schedule_dec[10] = _mm_aesimc_si128(key_schedule[ 2]);
				key_schedule_dec[11] = _mm_aesimc_si128(key_schedule[ 1]);
				key_schedule_dec[12] = key_schedule[0];
			}
			else if constexpr (keysize == 32)
			{
				key_schedule_dec[ 0] = key_schedule[14];
				key_schedule_dec[ 1] = _mm_aesimc_si128(key_schedule[13]);
				key_schedule_dec[ 2] = _mm_aesimc_si128(key_schedule[12]);
				key_schedule_dec[ 3] = _mm_aesimc_si128(key_schedule[11]);
				key_schedule_dec[ 4] = _mm_aesimc_si128(key_schedule[10]);
				key_schedule_dec[ 5] = _mm_aesimc_si128(key_schedule[ 9]);
				key_schedule_dec[ 6] = _mm_aesimc_si128(key_schedule[ 8]);
				key_schedule_dec[ 7] = _mm_aesimc_si128(key_schedule[ 7]);
				key_schedule_dec[ 8] = _mm_aesimc_si128(key_schedule[ 6]);
				key_schedule_dec[ 9] = _mm_aesimc_si128(key_schedule[ 5]);
				key_schedule_dec[10] = _mm_aesimc_si128(key_schedule[ 4]);
				key_schedule_dec[11] = _mm_aesimc_si128(key_schedule[ 3]);
				key_schedule_dec[12] = _mm_aesimc_si128(key_schedule[ 2]);
				key_schedule_dec[13] = _mm_aesimc_si128(key_schedule[ 1]);
				key_schedule_dec[14] = key_schedule[0];
			}
		}
	}

	constexpr auto operator()(std::byte const *plaintext_or_ciphertext)
	{
		__m128i m(_mm_loadu_si128(static_cast<__m128i const*>(static_cast<void const*>(plaintext_or_ciphertext))));
		if constexpr (encrypt)
		{
			m = _mm_xor_si128       (m, key_schedule[ 0]);
			m = _mm_aesenc_si128    (m, key_schedule[ 1]);
			m = _mm_aesenc_si128    (m, key_schedule[ 2]);
			m = _mm_aesenc_si128    (m, key_schedule[ 3]);
			m = _mm_aesenc_si128    (m, key_schedule[ 4]);
			m = _mm_aesenc_si128    (m, key_schedule[ 5]);
			m = _mm_aesenc_si128    (m, key_schedule[ 6]);
			m = _mm_aesenc_si128    (m, key_schedule[ 7]);
			m = _mm_aesenc_si128    (m, key_schedule[ 8]);
			m = _mm_aesenc_si128    (m, key_schedule[ 9]);
			if constexpr (keysize == 16) {
				m = _mm_aesenclast_si128(m, key_schedule[10]);
			} else if constexpr (keysize == 24) {
				m = _mm_aesenc_si128    (m, key_schedule[10]);
				m = _mm_aesenc_si128    (m, key_schedule[11]);
				m = _mm_aesenclast_si128(m, key_schedule[12]);
			} else if constexpr (keysize == 32) {
				m = _mm_aesenc_si128    (m, key_schedule[10]);
				m = _mm_aesenc_si128    (m, key_schedule[11]);
				m = _mm_aesenc_si128    (m, key_schedule[12]);
				m = _mm_aesenc_si128    (m, key_schedule[13]);
				m = _mm_aesenclast_si128(m, key_schedule[14]);
			}
		}
		else
		{
			m = _mm_xor_si128       (m, key_schedule_dec[ 0]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 1]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 2]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 3]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 4]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 5]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 6]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 7]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 8]);
			m = _mm_aesdec_si128    (m, key_schedule_dec[ 9]);
			if constexpr (keysize == 16) {
				m = _mm_aesdeclast_si128(m, key_schedule_dec[10]);
			} else if constexpr (keysize == 24) {
				m = _mm_aesdec_si128    (m, key_schedule_dec[10]);
				m = _mm_aesdec_si128    (m, key_schedule_dec[11]);
				m = _mm_aesdeclast_si128(m, key_schedule_dec[12]);
			} else if constexpr (keysize == 32) {
				m = _mm_aesdec_si128    (m, key_schedule_dec[10]);
				m = _mm_aesdec_si128    (m, key_schedule_dec[11]);
				m = _mm_aesdec_si128    (m, key_schedule_dec[12]);
				m = _mm_aesdec_si128    (m, key_schedule_dec[13]);
				m = _mm_aesdeclast_si128(m, key_schedule_dec[14]);
			}
		}
		std::array<std::byte, 16> result;
		_mm_storeu_si128(static_cast<__m128i*>(static_cast<void*>(result.data())), m);
		return result;
	}
};

using aes_enc_128 = aes<true, 16>;
using aes_dec_128 = aes<false, 16>;

using aes_enc_192 = aes<true, 24>;
using aes_dec_192 = aes<false, 24>;

using aes_enc_256 = aes<true, 32>;
using aes_dec_256 = aes<false, 32>;

}

#endif // __x86_64__
