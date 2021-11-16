#pragma once

namespace fast_io
{
template<typename func,bool endian_reverse>
struct hmac;

namespace details
{

template<typename range_type,typename func,bool endian_reverse>
inline constexpr void initialize_hmac(hmac<func,endian_reverse>& obj,range_type const* init_key_data, std::size_t init_key_size);

}

template<typename func,bool endian_reverse>
struct hmac
{
	using function_type = func;
	using key_type = ::fast_io::freestanding::array<std::byte,func::block_size>;
	function_type function;
	key_type inner_key;
	key_type outer_key{};
	static inline constexpr std::size_t block_size = function_type::block_size;

#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	hmac(std::byte const* init_key_data, std::size_t init_key_size) noexcept
	{
		details::initialize_hmac<std::byte>(*this,init_key_data,init_key_size);
	}
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	hmac(::fast_io::freestanding::string_view key) noexcept
	{
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
		if(std::is_constant_evaluated())
			::fast_io::details::initialize_hmac<char>(*this,key.data(),key.size());
		else
#endif
			::fast_io::details::initialize_hmac<std::byte>(*this,reinterpret_cast<std::byte const*>(key.data()),key.size()*sizeof(char));
	}
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	hmac(::fast_io::freestanding::wstring_view key) noexcept
	{
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
		if(std::is_constant_evaluated())
			::fast_io::details::initialize_hmac<wchar_t>(*this,key.data(),key.size());
		else
#endif
			::fast_io::details::initialize_hmac<std::byte>(*this,reinterpret_cast<std::byte const*>(key.data()),key.size()*sizeof(wchar_t));
	}
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	hmac(::fast_io::freestanding::u8string_view key) noexcept
	{
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
		if(std::is_constant_evaluated())
			::fast_io::details::initialize_hmac<char8_t>(*this,key.data(),key.size());
		else
#endif
			::fast_io::details::initialize_hmac<std::byte>(*this,reinterpret_cast<std::byte const*>(key.data()),key.size()*sizeof(char8_t));
	}
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	hmac(::fast_io::freestanding::u16string_view key) noexcept
	{
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
		if(std::is_constant_evaluated())
			::fast_io::details::initialize_hmac<char16_t>(*this,key.data(),key.size());
		else
#endif
			::fast_io::details::initialize_hmac<std::byte>(*this,reinterpret_cast<std::byte const*>(key.data()),key.size()*sizeof(char16_t));
	}
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	hmac(::fast_io::freestanding::u32string_view key) noexcept
	{
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
		if(std::is_constant_evaluated())
			::fast_io::details::initialize_hmac<char32_t>(*this,key.data(),key.size());
		else
#endif
			::fast_io::details::initialize_hmac<std::byte>(*this,reinterpret_cast<std::byte const*>(key.data()),key.size()*sizeof(char32_t));;
	}
	constexpr std::size_t block_init(std::byte* sp) noexcept
	{
		::fast_io::details::compile_time_type_punning_copy_n(inner_key.data(),sizeof(key_type),sp);
		return sizeof(key_type);
	}
	constexpr void operator()(std::byte const* process_blocks,std::size_t block_bytes) noexcept
	{
		function(process_blocks,block_bytes);
	}
	constexpr void digest(std::byte const* final_process_blocks,std::size_t final_block_bytes) noexcept
	{
		function.digest(final_process_blocks,final_block_bytes);
		for(auto & e : outer_key)
			e^=std::byte{0x5c};
		auto digest_block{function.digest_block};
		if constexpr(endian_reverse)
			for(auto& e : digest_block)
				e=big_endian(e);
		function={};
		hash_processor processor(function);
		details::hash_processor_impl::hash_write_impl<std::byte>(processor,outer_key.data(),outer_key.data()+outer_key.size());
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
		if(std::is_constant_evaluated())
		{
			auto blck{std::bit_cast<::fast_io::freestanding::array<std::byte,sizeof(digest_block)>>(digest_block)};
			details::hash_processor_impl::hash_write_impl<std::byte>(processor,blck.data(),blck.data()+blck.size());
		}
		else
#endif
			details::hash_processor_impl::hash_write_impl<std::byte>(processor,
				reinterpret_cast<std::byte const*>(digest_block.data()),
				reinterpret_cast<std::byte const*>(digest_block.data()+digest_block.size()));
		processor.do_final();
	}
};

namespace details
{
template<typename range_type,typename func,bool endian_reverse>
inline 
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
constexpr
#endif
void initialize_hmac(hmac<func,endian_reverse>& obj,range_type const* init_key_data, std::size_t init_key_size)
{
	auto& inner_key{obj.inner_key};
	auto& outer_key{obj.outer_key};
	constexpr std::size_t block_size{func::block_size};
	if(block_size<init_key_size)
	{
		hash_processor processor(obj.function);
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
		if(std::is_constant_evaluated())
			hash_processor_impl::hash_write_impl<range_type>(processor,init_key_data,init_key_data+init_key_size);
		else
#endif
		{
			hash_processor_impl::hash_write_impl<std::byte>(processor,reinterpret_cast<std::byte const*>(init_key_data),reinterpret_cast<std::byte const*>(init_key_data+init_key_size));
		}
		processor.do_final();
		if constexpr(endian_reverse)
			for(auto & e : obj.function.digest_block)
				e=byte_swap(e);
		compile_time_type_punning_copy_n(obj.function.digest_block.data(),sizeof(obj.function.digest_block),outer_key.data());
		obj.function={};
	}
	else
		compile_time_type_punning_copy_n(init_key_data,init_key_size,outer_key.data());
	for(std::size_t i{};i!=inner_key.size();++i)
		inner_key[i]=outer_key[i]^std::byte{0x36};
}
}

template<std::integral char_type,typename T,bool endian_reverse>
requires reserve_printable<char_type,T>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,hmac<T,endian_reverse>>) noexcept
{
	return print_reserve_size(io_reserve_type<char_type,T>);
}

template<std::integral char_type,typename T,bool endian_reverse,::fast_io::freestanding::random_access_iterator caiter>
requires reserve_printable<char_type,T>
inline constexpr caiter print_reserve_define(io_reserve_type_t<char_type,hmac<T,endian_reverse>>,caiter iter,auto& i) noexcept
{
	return print_reserve_define(io_reserve_type<char_type,T>,iter,i.function);
}

using hmac_sha1
[[deprecated("SHA1 is no longer a secure algorithm. See wikipedia https://en.wikipedia.org/wiki/SHA-1")]]
=hmac<sha<sha1_function>,true>;

using hmac_sha256=hmac<sha256,true>;

using hmac_sha512=hmac<sha512,true>;


using hmac_md5 
[[deprecated("The weaknesses of MD5 have been exploited in the field, most infamously by the Flame malware in 2012. See wikipedia https://en.wikipedia.org/wiki/MD5")]]
=hmac<sha<md5_function,false>,false>;


}
