#pragma once

namespace fast_io::details
{

struct pesudo_uint_least128_little_endian_t
{
	std::uint_least64_t low{},high{};
};

struct pesudo_uint_least128_big_endian_t
{
	std::uint_least64_t high{},low{};
};

using pesudo_uint_least128_t=std::conditional_t<::std::endian::native==::std::endian::big,pesudo_uint_least128_big_endian_t,pesudo_uint_least128_little_endian_t>;

template<typename T,typename counter_type,std::endian edian>
class basic_md5_sha_context_impl
{
public:
	using hash_function_type = T;
	static inline constexpr std::size_t block_size{T::block_size};
private:
	T hasher{};
	counter_type counter{};
	std::size_t buffer_offset{};
	std::byte buffer[block_size];
	constexpr void update_impl(std::byte const* first,std::size_t blocks_bytes) noexcept
	{
		hasher.update_blocks(first,first+blocks_bytes);
		constexpr std::uint_least64_t eight{8u};
		if constexpr(std::same_as<counter_type,details::pesudo_uint_least128_t>)
		{
			static_assert(sizeof(std::size_t)<=sizeof(std::uint_least64_t));
			std::uint_least64_t const blocks_bytes_u64{static_cast<std::uint_least64_t>(blocks_bytes)};
			using namespace ::fast_io::details::intrinsics;
			std::uint_least64_t high;
			std::uint_least64_t const low{umul(blocks_bytes_u64,eight,high)};
			add_carry(add_carry(false,counter.low,low,counter.low),counter.high,high,counter.high);
		}
		else
		{
			counter+=static_cast<counter_type>(blocks_bytes)*eight;
		}
	}
	constexpr void update_cold_impl(std::byte const* first,std::size_t diff) noexcept
	{
		std::size_t const buffer_space{static_cast<std::size_t>(buffer_offset)};
		std::size_t const buffer_remain_space{static_cast<std::size_t>(block_size-buffer_offset)};
		if(buffer_remain_space!=block_size)
		{
			::fast_io::details::non_overlapped_copy_n(first,buffer_remain_space,buffer+buffer_space);
			diff-=buffer_remain_space;
			first+=buffer_remain_space;
			this->update_impl(first,block_size);
		}
		std::size_t const blocks_bytes{(diff/block_size)*block_size};
		this->update_impl(first,blocks_bytes);
		first+=blocks_bytes;
		diff-=blocks_bytes;
		::fast_io::details::non_overlapped_copy_n(first,diff,buffer);
		buffer_offset=diff;
	}
	inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	void append_sentinal(std::size_t final_block_offset) noexcept
	{
		counter_type ct{counter};
		std::uint_least64_t const val{static_cast<std::uint_least64_t>(final_block_offset*8u)};
		if constexpr(std::same_as<counter_type,details::pesudo_uint_least128_t>)
		{
			using namespace ::fast_io::details::intrinsics;
			constexpr std::uint_least64_t zero{};
			add_carry(add_carry(false,ct.low,val,ct.low),ct.high,zero,ct.high);
		}
		else
		{
			ct+=val;
		}
		if constexpr(std::endian::native!=edian)
		{
			if constexpr(std::same_as<counter_type,details::pesudo_uint_least128_t>)
			{
				ct.low=::fast_io::byte_swap(ct.low);
				ct.high=::fast_io::byte_swap(ct.high);
			}
			else
			{
				ct=::fast_io::byte_swap(ct);
			}
		}
		constexpr std::size_t start_pos{block_size-sizeof(counter_type)};
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			using c_array_type = std::byte[sizeof(counter_type)];
			auto a{__builtin_bit_cast(c_array_type,this->counter)};
			for(std::size_t i{};i!=sizeof(counter_type);++i)
			{
				this->buffer[start_pos+i]=a[i];
			}
		}
		else
#endif
		{
			::fast_io::details::my_memcpy(buffer+start_pos,__builtin_addressof(ct),sizeof(counter_type));
		}
		this->update_impl(buffer,buffer+block_size);
	}
public:
	inline constexpr void update(std::byte const* block_first,std::byte const* block_last) noexcept
	{
		std::size_t const diff{static_cast<std::size_t>(block_last-block_first)};
		std::size_t const buffer_remain_space{static_cast<std::size_t>(block_size-buffer_offset)};
		if(diff<buffer_remain_space)[[likely]]
		{
			::fast_io::details::non_overlapped_copy_n(block_first,diff,buffer+buffer_offset);
			buffer_offset+=diff;
		}
		else
		{
			this->update_cold_impl(block_first,diff);
		}
	}
	constexpr void reset() noexcept
	{
		hasher.reset();
		counter={};
		buffer_offset=0;
	}
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	void do_final() noexcept
	{
		constexpr std::size_t counter_type_size{sizeof(counter)};
		constexpr std::size_t sz{block_size-counter_type_size};
		std::size_t const buffer_offs{this->buffer_offset};
		std::size_t buffer_off{buffer_offs};
		*buffer[buffer_off]=std::byte{0x80};
		++buffer_off;
		std::size_t const diff{block_size-buffer_off};
		if(sz<diff)
		{
			::fast_io::none_secure_clear(this->data,static_cast<std::size_t>(sz-buffer_off));
			this->append_sentinal(buffer_offs);
			return;
		}
		::fast_io::none_secure_clear(this->data,static_cast<std::size_t>(block_size-buffer_off));
		this->hasher.update_blocks(this->buffer,this->buffer+block_size);
		::fast_io::none_secure_clear(this->data,sz);
		this->append_sentinal(buffer_offs);
	}
	constexpr hash_function_type& hash() noexcept
	{
		return hasher;
	}
	constexpr hash_function_type const& hash() const noexcept
	{
		return hasher;
	}
};

}

namespace fast_io
{
using sha256_context=::fast_io::details::basic_md5_sha_context_impl<::fast_io::sha256,std::uint_least64_t,::std::endian::big>;
}

