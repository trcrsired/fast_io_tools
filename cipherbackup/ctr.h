#pragma once

namespace fast_io::crypto
{

template<typename ciphert,bool big_endian = false>
class ctr
{
public:
	using cipher_type = ciphert;
	using key_type = std::span<std::byte const, cipher_type::key_size>;
	using block_type = std::span<std::byte, cipher_type::block_size>;
	using nonce_type = std::span<std::byte const, cipher_type::block_size-8>;
	using counter_type = std::uint64_t;
	inline static constexpr std::size_t block_size = cipher_type::block_size;
public:
	alignas(16) std::array<std::byte,cipher_type::block_size> nonce_block{};
	cipher_type cipher;
	ctr(key_type key, nonce_type nc):cipher(key)
	{
		details::my_copy(nc.begin(), nc.end(), nonce_block.data());
	}
	ctr(key_type key, nonce_type nc, std::uint64_t start_counter):cipher(key)
	{
		details::my_copy(nc.begin(), nc.end(), nonce_block.data());
		if constexpr((std::endian::little==std::endian::native&&big_endian)||
			(std::endian::big==std::endian::native&&!big_endian))
		{
			std::uint64_t v(fast_io::details::byte_swap(start_counter));
			memcpy(nonce_block.data()+(cipher_type::block_size-8),std::addressof(v),8);
		}
		else
			memcpy(nonce_block.data()+(cipher_type::block_size-8),std::addressof(start_counter),8);
	}
	inline void operator()(std::span<std::byte, block_size> text)
	{
		std::uint64_t counter;
		memcpy(std::addressof(counter),nonce_block.data()+(cipher_type::block_size-8),8);
		if constexpr((std::endian::little==std::endian::native&&big_endian)||
			(std::endian::big==std::endian::native&&!big_endian))
		{
			std::uint64_t v(fast_io::details::byte_swap(counter));
			memcpy(nonce_block.data()+(cipher_type::block_size-8),std::addressof(v),8);
		}
		alignas(16) std::array<std::byte,cipher_type::block_size> res(cipher(nonce_block.data()));
		fast_xor_assignment(text,std::span(res));
		++counter;
		memcpy(nonce_block.data()+(cipher_type::block_size-8),std::addressof(counter),8);
	}
};
/*
template<buffer_output_stream T, typename Enc, std::size_t sz = 4096>
using octr_little_endian = otransform<T, block_processor<block_cipher<ctr<Enc,false>>>, typename T::char_type, sz>;

template<buffer_input_stream T, typename Enc, std::size_t sz = 4096>
using ictr_little_endian = itransform<T, block_processor<block_cipher<ctr<Enc,false>>>, typename T::char_type, sz>;

template<buffer_output_stream T, typename Enc, std::size_t sz = 4096>
using octr_big_endian = otransform<T, block_processor<block_cipher<ctr<Enc,true>>>, typename T::char_type, sz>;

template<buffer_input_stream T, typename Enc, std::size_t sz = 4096>
using ictr_big_endian = itransform<T, block_processor<block_cipher<ctr<Enc,true>>>, typename T::char_type, sz>;
*/
}
