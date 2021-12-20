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

template<typename T,std::size_t counterbits>
requires (counterbits==64||counterbits==128)
struct md5_sha_common_impl
{
	static inline constexpr std::size_t block_size{T::block_size};
	static inline constexpr std::endian hash_endian{T::hash_endian};
	using counter_type = std::conditional_t<counterbits==64,::std::uint_least64_t,
#if __SIZEOF_INT128__
	__uint128_t
#else
	::fast_io::details::pesudo_uint_least128_t
#endif
	>;
	T hasher;
	counter_type counter;
	std::size_t buffer_offset;
	std::byte buffer[block_size];
	constexpr void update_impl(std::byte const* first,std::size_t blocks_bytes) noexcept
	{
		this->hasher.update_blocks(first,first+blocks_bytes);
		if constexpr(std::same_as<counter_type,::fast_io::details::pesudo_uint_least128_t>)
		{
			static_assert(sizeof(std::size_t)<=sizeof(std::uint_least64_t));
			std::uint_least64_t const blocks_bytes_u64{static_cast<std::uint_least64_t>(blocks_bytes)};
			using namespace ::fast_io::details::intrinsics;
			constexpr std::uint_least64_t zero{};
			add_carry(add_carry(false,counter.low,blocks_bytes_u64,counter.low),counter.high,zero,counter.high);
		}
		else
		{
			counter+=static_cast<counter_type>(blocks_bytes);
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
			this->update_impl(buffer,block_size);
		}
		std::size_t const blocks_bytes{(diff/block_size)*block_size};
		this->update_impl(first,blocks_bytes);
		first+=blocks_bytes;
		diff-=blocks_bytes;
		::fast_io::details::non_overlapped_copy_n(first,diff,buffer);
		buffer_offset=diff;
	}
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	void append_sentinal(std::size_t final_block_offset) noexcept
	{
		counter_type ct{counter};
		std::uint_least64_t const val{static_cast<std::uint_least64_t>(final_block_offset)};
		if constexpr(std::same_as<counter_type,::fast_io::details::pesudo_uint_least128_t>)
		{
			using namespace ::fast_io::details::intrinsics;
			constexpr std::uint_least64_t zero{};
			add_carry(add_carry(false,ct.low,val,ct.low),ct.high,zero,ct.high);
			add_carry(add_carry(false,ct.low,ct.low,ct.low),ct.high,ct.high,ct.high);//*2
			add_carry(add_carry(false,ct.low,ct.low,ct.low),ct.high,ct.high,ct.high);//*4
			add_carry(add_carry(false,ct.low,ct.low,ct.low),ct.high,ct.high,ct.high);//*8
		}
		else
		{
			ct+=val;
			ct*=8u;
		}
		if constexpr(std::endian::native!=hash_endian)
		{
			if constexpr(std::same_as<counter_type,::fast_io::details::pesudo_uint_least128_t>)
			{
				auto t{::fast_io::byte_swap(ct.low)};
				ct.low=::fast_io::byte_swap(ct.high);
				ct.high=t;
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
			auto a{std::bit_cast<::fast_io::freestanding::array<std::byte,sizeof(counter_type)>>(this->counter)};
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
		this->hasher.update_blocks(buffer,buffer+block_size);
	}
	inline constexpr void update(std::byte const* block_first,std::byte const* block_last) noexcept
	{
		std::size_t const diff{static_cast<std::size_t>(block_last-block_first)};
		std::size_t const buffer_remain_space{static_cast<std::size_t>(block_size-buffer_offset)};
		if(diff<buffer_remain_space)[[likely]]
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
			if(diff>=block_size)
				__builtin_unreachable();
#endif
#endif
			::fast_io::details::non_overlapped_copy_n(block_first,diff,buffer+buffer_offset);
			buffer_offset+=diff;
		}
		else
		{
			this->update_cold_impl(block_first,diff);
		}
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
		buffer[buffer_off]=std::byte{0x80};
		++buffer_off;
		if(buffer_off<=sz)
		{
			std::size_t const to_fill{static_cast<std::size_t>(sz-buffer_off)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
			if(to_fill>sz)
				__builtin_unreachable();
#endif
#endif
			::fast_io::none_secure_clear(this->buffer+buffer_off,to_fill);
			this->append_sentinal(buffer_offs);
			return;
		}
		std::size_t const to_fill{static_cast<std::size_t>(block_size-buffer_off)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
		if(to_fill>counter_type_size)
			__builtin_unreachable();
#endif
#endif
		::fast_io::none_secure_clear(this->buffer+buffer_off,to_fill);
		this->hasher.update_blocks(this->buffer,this->buffer+block_size);
		::fast_io::none_secure_clear(this->buffer,sz);
		this->append_sentinal(buffer_offs);
	}
};

template<typename T,typename initializer,std::size_t counterbits>
class basic_md5_sha_context_impl
{
	md5_sha_common_impl<T,counterbits> hasher;
public:
	explicit constexpr basic_md5_sha_context_impl() noexcept
	{
		this->reset();
	}
	static inline constexpr std::size_t digest_size{initializer::digest_size};
	constexpr void update(std::byte const* block_first,std::byte const* block_last) noexcept
	{
		hasher.update(block_first,block_last);
	}
	constexpr void reset() noexcept
	{
		hasher.hasher=initializer::initialize_value;
		hasher.counter={};
		hasher.buffer_offset=0;
	}
	constexpr void do_final() noexcept
	{
		hasher.do_final();
	}
	constexpr void digest_to_ptr(std::byte* digest) const noexcept
	{
		initializer::digest_to_ptr(hasher.hasher.state,digest);
	}
};

template<typename T>
inline void update_multiple_blocks(T* __restrict ctx,io_scatter_t const* base,std::size_t n) noexcept
{
	for(std::size_t i{};i!=n;++i)
	{
		io_scatter_t e{base[i]};
		ctx->update(reinterpret_cast<std::byte const*>(e.base),reinterpret_cast<std::byte const*>(e.base)+e.len);
	}
}

}

namespace fast_io
{
template<::std::integral ch_type,typename T>
struct basic_crypto_hash_as_file
{
	using char_type = ch_type;
	using manip_tag = manip_tag_t;
	using native_handle_type = T*;
	T* ptr{};
};

template<::std::integral ch_type,typename T>
inline constexpr basic_crypto_hash_as_file<ch_type,T> io_value_handle(basic_crypto_hash_as_file<ch_type,T> t) noexcept
{
	return t;
}

template<std::integral char_type,typename T>
inline constexpr void require_secure_clear(basic_crypto_hash_as_file<char_type,T>) noexcept{}

template<::std::integral ch_type,typename T,::fast_io::freestanding::contiguous_iterator Iter>
inline constexpr void write(basic_crypto_hash_as_file<ch_type,T> t,Iter first,Iter last) noexcept
{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		for(;first!=last;++first)
		{
			auto a{std::bit_cast<::fast_io::freestanding::array<std::byte,sizeof(*first)>>(*first)};
			t.ptr->update(a.data(),a.data()+sizeof(*first));
		}
	}
	else
#endif
	{
		t.ptr->update(reinterpret_cast<std::byte const*>(::fast_io::freestanding::to_address(first)),
			reinterpret_cast<std::byte const*>(::fast_io::freestanding::to_address(last)));
	}
}

template<::std::integral ch_type,typename T>
inline void scatter_write(basic_crypto_hash_as_file<ch_type,T> t,io_scatters_t scatters) noexcept
{
	::fast_io::details::update_multiple_blocks(t.ptr,scatters.base,scatters.len);
}

namespace manipulators
{

template<std::integral char_type,typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char_type,T> basic_as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char,T> as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<wchar_t,T> was_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char8_t,T> u8as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char16_t,T> u16as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

template<typename T>
inline constexpr ::fast_io::basic_crypto_hash_as_file<char32_t,T> u32as_file(T& hashctx) noexcept
{
	return {__builtin_addressof(hashctx)};
}

}

}
