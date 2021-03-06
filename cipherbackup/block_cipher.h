#pragma once

namespace fast_io::crypto
{

template<typename mcph, typename padding_method = pkcs7_padding<mcph::block_size>>
class block_cipher
{
public:
	using mode_cipher_type = mcph;
	inline static constexpr std::size_t block_size = mode_cipher_type::block_size;
	mode_cipher_type cipher;
	padding_method padding;
	template<typename... Args>
	requires std::constructible_from<mode_cipher_type,Args...>
	block_cipher(Args&& ...args):cipher(std::forward<Args>(args)...){}
	inline void operator()(std::span<std::byte, block_size> text)
	{
		cipher(text);
	}
	auto digest(std::span<std::byte const> inp)
	{
		std::size_t remaining_length(block_size - (inp.end() - inp.begin()));
		auto text(padding(inp, remaining_length));
		cipher(text);
		return text;
	}
};

}