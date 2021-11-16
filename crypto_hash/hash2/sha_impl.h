#pragma once

namespace fast_io
{
namespace details
{
struct pesudo_uint128_t
{
	std::uint64_t low{},high{};
};

}

template<typename T,bool endian_reverse=true,std::size_t transform_counter_bytes=8>
requires (transform_counter_bytes==8 || transform_counter_bytes==16)
class sha
{
public:
	using function_type = T;
	using digest_type = typename T::digest_type;
	digest_type digest_block = T::digest_initial_value;
	static inline constexpr std::size_t block_size = T::block_size;
	static_assert(block_size>(transform_counter_bytes/8));
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	T function;
	using transform_counter_type =
#ifdef __SIZEOF_INT128__
	std::conditional_t<transform_counter_bytes==8,std::uint64_t,__uint128_t>;
#else
	std::conditional_t<transform_counter_bytes==8,std::uint64_t,details::pesudo_uint128_t>;
#endif
	transform_counter_type transform_counter{};
	constexpr void operator()(std::byte const* process_blocks,std::size_t process_block_bytes) noexcept//This is multiple blocks
	{
		function(digest_block.data(),process_blocks,process_block_bytes);
		if constexpr(std::same_as<transform_counter_type,details::pesudo_uint128_t>)
			details::intrinsics::add_carry(details::intrinsics::add_carry(false,transform_counter.low,static_cast<std::uint64_t>(process_block_bytes)<<3,transform_counter.low),
				transform_counter.high,static_cast<std::uint64_t>(0),transform_counter.high);
		else
			transform_counter+=static_cast<std::uint64_t>(process_block_bytes)<<3;
	}
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	void digest(std::byte const* final_block,std::size_t final_block_bytes) noexcept//contracts: final_block.size()<block_size
	{
		transform_counter_type temp{transform_counter};
		if constexpr(std::same_as<transform_counter_type,details::pesudo_uint128_t>)
			details::intrinsics::add_carry(details::intrinsics::add_carry(false,temp.low,(static_cast<std::uint64_t>(final_block_bytes)<<3),temp.low),
				temp.high,static_cast<std::uint64_t>(0),temp.high);
		else
			temp+=static_cast<transform_counter_type>(final_block_bytes)<<3;
		freestanding::array<std::byte,(block_size<<1)> blocks{};
		details::non_overlapped_copy_n(final_block,final_block_bytes,blocks.data());
		blocks[final_block_bytes]=std::byte{0x80};
		std::byte* end_of_padding_block{blocks.data()+blocks.size()};
		if(final_block_bytes+transform_counter_bytes<block_size)
			end_of_padding_block=blocks.data()+block_size;
		if constexpr(endian_reverse)
		{
			if constexpr(std::same_as<transform_counter_type,details::pesudo_uint128_t>&&std::endian::native!=std::endian::big)
			{
				std::uint64_t high{big_endian(temp.high)};
				std::uint64_t low{big_endian(temp.low)};
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
				if(std::is_constant_evaluated())
				{
					using type_punning_array = freestanding::array<std::byte,sizeof(high)>;
					type_punning_array high_array{std::bit_cast<type_punning_array>(high)};
					type_punning_array low_array{std::bit_cast<type_punning_array>(low)};
					details::non_overlapped_copy_n(high_array.data(),sizeof(high),end_of_padding_block-(sizeof(high)+sizeof(low)));
					details::non_overlapped_copy_n(low_array.data(),sizeof(low),end_of_padding_block-sizeof(low));
				}
				else
				{
#endif
					details::my_memcpy(end_of_padding_block-(sizeof(high)+sizeof(low)),__builtin_addressof(high),sizeof(high));
					details::my_memcpy(end_of_padding_block-sizeof(low),__builtin_addressof(low),sizeof(low));
				}
			}
			else
			{
				temp=big_endian(temp);
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
				if(std::is_constant_evaluated())
				{
					using type_punning_array = freestanding::array<std::byte,sizeof(temp)>;
					type_punning_array temp_array{std::bit_cast<type_punning_array>(temp)};
					details::non_overlapped_copy_n(temp_array.data(),sizeof(temp),end_of_padding_block-sizeof(temp));
				}
				else
#endif
					details::my_memcpy(end_of_padding_block-sizeof(temp),__builtin_addressof(temp),sizeof(temp));

			}
		}
		else
		{
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
			if(std::is_constant_evaluated())
			{
				using type_punning_array = freestanding::array<std::byte,transform_counter_bytes>;
				type_punning_array temp_array{std::bit_cast<type_punning_array>(temp)};
				details::non_overlapped_copy_n(temp_array.data(),sizeof(temp),end_of_padding_block-sizeof(temp));
			}
			else
#endif
				details::my_memcpy(end_of_padding_block-transform_counter_bytes,__builtin_addressof(temp),transform_counter_bytes);
		}
		function(digest_block.data(),blocks.data(),static_cast<std::size_t>(end_of_padding_block-blocks.data()));
	}
};


template<std::integral char_type,typename T,bool endian_reverse,std::size_t transform_counter_bytes>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,sha<T,endian_reverse,transform_counter_bytes>>) noexcept
{
	return sizeof(typename T::digest_type)*2;
}

template<std::integral char_type,::fast_io::freestanding::random_access_iterator caiter,typename T,bool endian_reverse,std::size_t transform_counter_bytes>
inline constexpr caiter print_reserve_define(io_reserve_type_t<char_type,sha<T,endian_reverse,transform_counter_bytes>>,caiter iter,sha<T,endian_reverse,transform_counter_bytes> const& i) noexcept
{
	return details::crypto_hash_main_reserve_define_common_impl<false,endian_reverse>(i.digest_block.data(),i.digest_block.data()+i.digest_block.size(),iter);
}

#if 0
template<std::integral char_type,typename T,bool endian_reverse>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,manipulators::base_full_t<16,true,sha<T,endian_reverse> const&>>) noexcept
{
	return sizeof(typename T::digest_type)*2;
}

template<std::integral char_type,::fast_io::freestanding::random_access_iterator caiter,typename T,bool endian_reverse,std::size_t transform_counter_bytes>
inline constexpr caiter print_reserve_define(io_reserve_type_t<char_type,manipulators::base_full_t<16,true,sha<T,endian_reverse,transform_counter_bytes> const&>>,caiter iter,manipulators::base_full_t<16,true,sha<T,endian_reverse,transform_counter_bytes> const&> i) noexcept
{
	return details::crypto_hash_main_reserve_define_common_impl<true,endian_reverse>(i.reference.digest_block.data(),i.reference.digest_block.data()+i.reference.digest_block.size(),iter);


#if 0
namespace manipulators
{

template<typename T,bool endian_reverse,std::size_t transform_counter_bytes>
inline constexpr scalar<16,true,::fast_io::sha<T,endian_reverse,transform_counter_bytes> const&> upper(::fast_io::sha<T,endian_reverse,transform_counter_bytes> const& res) noexcept
{
	return {res};
}

template<typename T,bool endian_reverse,std::size_t transform_counter_bytes>
inline constexpr parameter<::fast_io::sha<T,endian_reverse,transform_counter_bytes> const&> lower(::fast_io::sha<T,endian_reverse,transform_counter_bytes> const& res) noexcept
{
	return {res};
}

}
#endif
}
