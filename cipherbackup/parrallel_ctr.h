#pragma once

#include<jthread>
#include<vector>

namespace fast_io
{

template<typename ciphert,std::size_t blocks=64,std::size_t threads=8,bool big_endian = false>
class parrallel_ctr
{
public:
	using cipher_type = ciphert;
	using key_type = std::span<std::byte const, cipher_type::key_size>;
	using block_type = std::span<std::byte, cipher_type::block_size>;
	using nonce_type = std::span<std::byte const, cipher_type::block_size-8>;
	using counter_type = std::uint64_t;
	inline static constexpr std::size_t block_size = cipher_type::block_size*blocks*threads;
	cipher_type cipher;
	std::vector<std::jthread> jthreads;
	alignas(16) std::array<std::byte,cipher_type::block_size-8> nonce{};
	std::uint64_t counter{};
public:
	parrallel_ctr(key_type key, nonce_type nc):cipher(key)
	{
		details::my_copy(nc.begin(), nc.end(), nonce.data());
		jthreads.reserve(threads);
	}
	parrallel_ctr(key_type key, nonce_type nc, std::uint64_t start_counter):cipher(key),counter(start_counter)
	{
		details::my_copy(nc.begin(), nc.end(), nonce.data());
		jthreads.reserve(threads);
	}
	inline void operator()(std::span<std::byte, cipher_type::block_size> text)
	{
		for(std::size_t i{};i!=jthreads.capacity();++i)
			jthreads.emplace_back([=](std::uint64_t local_counter)
			{
				alignas(16) std::array<std::byte,cipher_type::block_size> nonce_block{};
				memcpy(nonce_block.data(),nonce.data(),cipher_type::block_size-8);
				memcpy(nonce_block.data()+(cipher_type::block_size-8),std::addressof(local_counter),8);
				for(std::size_t )
				if constexpr((std::endian::little==std::endian::native&&big_endian)||
					(std::endian::big==std::endian::native&&!big_endian))
				{
					std::uint64_t v(fast_io::details::byte_swap(counter));
					memcpy(nonce_block.data()+(cipher_type::block_size-8),std::addressof(v),8);
				}
				alignas(16) std::array<std::byte,cipher_type::block_size> res(cipher(nonce_block.data()));
				fast_xor_assignment(text,std::span(res));
				++local_counter;
				memcpy(nonce_block.data()+(cipher_type::block_size-8),std::addressof(local_counter),8);
			},text.subspan());
		jthreads.clear();
	}
}

}