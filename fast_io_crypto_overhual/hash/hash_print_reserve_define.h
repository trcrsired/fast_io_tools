#pragma once

namespace fast_io
{


namespace manipulators
{

struct crypto_hash_format
{
bool uppercase{};
std::endian endian{};
std::endian hash_endian{};
};

template<crypto_hash_format format,typename T,std::size_t n>
requires (std::unsigned_integral<T>||std::same_as<T,std::byte>)
struct crypto_hash_carrier
{
	T const* ptr{};
};

}
namespace details
{
template<bool uppercase,std::endian ed,std::endian hed>
inline constexpr ::fast_io::manipulators::crypto_hash_format hash_format{uppercase,ed,hed};

}

template<std::integral char_type,::fast_io::manipulators::crypto_hash_format format,typename T,std::size_t n>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,::fast_io::manipulators::crypto_hash_carrier<format,T,n>>) noexcept
{
	return sizeof(T)*n*2u;
}

template<std::integral char_type,::fast_io::manipulators::crypto_hash_format format,typename T,std::size_t n,::fast_io::freestanding::random_access_iterator Iter>	
inline constexpr Iter print_reserve_define(io_reserve_type_t<char_type,
	::fast_io::manipulators::crypto_hash_carrier<format,T,n>>,
	Iter iter,
	::fast_io::manipulators::crypto_hash_carrier<format,T,n> carr) noexcept
{
	return ::fast_io::details::crypto_hash_main_reserve_define_common_impl<format.uppercase,(format.endian!=format.hash_endian)>(carr.ptr,carr.ptr+n,iter);
}

template<typename T>
concept crypto_hasher = requires(T t,std::byte const* first,std::byte const* last)
{
	T::hash_endian;
	T::block_size;
	T::state_size;
	typename T::state_value_type;
	t.state;
	t.update_blocks(first,last);
};

template<crypto_hasher T>
inline constexpr ::fast_io::manipulators::crypto_hash_carrier<
	::fast_io::details::hash_format<false,T::hash_endian,T::hash_endian>,
	typename T::state_value_type,T::state_size>
	print_alias_define(io_alias_t,T const& e) noexcept
{
	return {e.state};
}

namespace manipulators
{

template<crypto_hasher T>
inline constexpr ::fast_io::manipulators::crypto_hash_carrier<
	::fast_io::details::hash_format<true,T::hash_endian,T::hash_endian>,
	typename T::state_value_type,T::state_size>
	uppercase(T const& e) noexcept
{
	return {e.state};
}

}

}
