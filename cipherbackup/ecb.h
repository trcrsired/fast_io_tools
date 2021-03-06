#pragma once

namespace fast_io::crypto
{

template<typename ciphert>
class ecb
{
public:
	using cipher_type = ciphert;
	using key_type = std::span<std::byte const, cipher_type::key_size>;
	using block_type = std::span<std::byte, cipher_type::block_size>;
	inline static constexpr std::size_t block_size = cipher_type::block_size;

public:
	cipher_type cipher;
	[[deprecated("ECB should NEVER EVER be used. The disadvantage of this method is a lack of diffusion. Because ECB encrypts identical plaintext blocks into identical ciphertext blocks, it does not hide data patterns well. In some senses, it doesn't provide serious message confidentiality, and it is not recommended for use in cryptographic protocols at all. https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Electronic_Codebook_(ECB)")]] ecb(key_type key):cipher(key)
	{
	}
	inline void operator()(std::span<std::byte, block_size> text)
	{
		auto ret(cipher(text.data()));
		memcpy(text.data(),ret.data(),block_size);
	}
};

template<buffer_output_stream T, typename Enc, std::size_t sz = 4096>
using oecb = otransform<T, block_processor<block_cipher<ecb<Enc>>>, typename T::char_type, sz>;

template<buffer_input_stream T, typename Enc, std::size_t sz = 4096>
using iecb = itransform<T, block_processor<block_cipher<ecb<Enc>>>, typename T::char_type, sz>;

}
