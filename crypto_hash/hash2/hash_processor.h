#pragma once

namespace fast_io
{

namespace details
{

class compress_current_position
{};

template<typename Func>
concept hash_require_block_init=requires(Func& func,std::byte* ptr)
{
	func.block_init(ptr);
};

}

template<std::integral ch_type,typename Func>
class basic_hash_processor
{
public:
	using char_type = ch_type;
	using function_type = Func;
	function_type* function{};
	inline static constexpr std::size_t block_size = function_type::block_size;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	::fast_io::freestanding::array<std::byte,block_size> temporary_buffer{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	std::conditional_t<block_size==0,details::compress_current_position,std::size_t> current_position{};
	constexpr basic_hash_processor(function_type& func) noexcept:function(__builtin_addressof(func))
	{
		if constexpr(details::hash_require_block_init<function_type>)
			current_position=func.block_init(temporary_buffer.data());
	}
	constexpr void do_final() noexcept
	{
		if constexpr(block_size!=0)
			function->digest(temporary_buffer.data(),current_position);
		else
			function->digest();
	}
	constexpr void reset() noexcept
	{
		*function={};
		if constexpr(details::hash_require_block_init<function_type>)
			current_position=function->block_init(temporary_buffer.data());
		else
			current_position=0;
	}
	constexpr basic_hash_processor(basic_hash_processor const&)=default;
	constexpr basic_hash_processor& operator=(basic_hash_processor const&)=default;
	constexpr basic_hash_processor(basic_hash_processor&&) noexcept=default;
	constexpr basic_hash_processor& operator=(basic_hash_processor&&) noexcept=default;
#if __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	~basic_hash_processor()
	{
#if __cpp_lib_is_constant_evaluated >= 201811L
		if(!std::is_constant_evaluated())
#endif
		{
			secure_clear(this,sizeof(basic_hash_processor<ch_type,Func>));
		}
	}
};

namespace details::hash_processor_impl
{


template<typename range_type,typename function_type>
inline constexpr void hash_write_cold_path_impl_common(function_type& func,std::size_t& current_position,std::byte* temp_buffer_data,range_type const* begin,range_type const* end) noexcept
{
	constexpr std::size_t blk_size{function_type::block_size};
	if(current_position)
	{
		std::size_t to_copy{blk_size-current_position};
		compile_time_type_punning_copy_n(begin,to_copy,temp_buffer_data+current_position);
		func(temp_buffer_data,blk_size);
		begin+=to_copy;
		current_position={};
	}
	std::size_t const total_bytes{static_cast<std::size_t>(end-begin)};
	std::size_t const blocks(total_bytes/blk_size);
	std::size_t const blocks_bytes(blocks*blk_size);
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	if(std::is_constant_evaluated())
	{
		for(std::size_t i{};i!=blocks;++i)
		{
			::fast_io::freestanding::array<std::byte,blk_size> block;
			compile_time_type_punning_copy_n(begin+i,blk_size,block.data());
			func(block.data(),blk_size);
		}
	}
	else
#endif
		func(reinterpret_cast<std::byte const*>(begin),blocks_bytes);
	std::size_t const to_copy(total_bytes-blocks_bytes);
	compile_time_type_punning_copy_n(end-to_copy,to_copy,temp_buffer_data);
	current_position=to_copy;
}

template<typename range_type,std::integral char_type,typename Func>
inline constexpr void hash_write_cold_path_impl(basic_hash_processor<char_type,Func>& out,range_type const* first,range_type const* last) noexcept
{
	hash_write_cold_path_impl_common<range_type>(*out.function,out.current_position,out.temporary_buffer.data(),first,last);
}

template<typename range_type,std::integral ch_type,typename function_type>
inline 
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
constexpr
#endif
void hash_write_impl(basic_hash_processor<ch_type,function_type>& out,range_type const* first,range_type const* last)
{
	if constexpr(function_type::block_size==0)
	{
		if constexpr(std::same_as<range_type,std::byte>)
			out->function(first,static_cast<std::size_t>(last-first));
		else
		{
			for(auto i{first};i!=last;++i)
			{
				::fast_io::freestanding::array<std::byte,sizeof(range_type)> arr{std::bit_cast<::fast_io::freestanding::array<std::byte,sizeof(range_type)>>(*i)};
				out->function(arr.data(),arr.size());
			}
		}
	}
	else
	{
		std::size_t const bytes{static_cast<std::size_t>(last-first)*sizeof(range_type)};
		std::size_t to_copy{function_type::block_size-out.current_position};
		if(bytes<to_copy)[[likely]]
		{
			compile_time_type_punning_copy_n(first,bytes,out.temporary_buffer.data()+out.current_position);
			out.current_position+=bytes;
			return;
		}
		hash_write_cold_path_impl<range_type>(out,first,last);
	}
}

}

template<std::integral ch_type,typename Func,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<::fast_io::freestanding::iter_value_t<Iter>,ch_type>||std::same_as<ch_type,char>)
inline
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
constexpr
#endif
void write(basic_hash_processor<ch_type,Func>& out,Iter begin,Iter end)
{
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
	if(std::is_constant_evaluated())
	{
		details::hash_processor_impl::hash_write_impl<ch_type>(out,::fast_io::freestanding::to_address(begin),::fast_io::freestanding::to_address(end));
	}
	else
#endif
	{
		details::hash_processor_impl::hash_write_impl<std::byte>(out,
			reinterpret_cast<std::byte const*>(::fast_io::freestanding::to_address(begin)),
			reinterpret_cast<std::byte const*>(::fast_io::freestanding::to_address(end)));
	}
}

template<std::integral ch_type,typename Func>
inline
#if __cpp_lib_is_constant_evaluated >= 201811L && __cpp_lib_bit_cast >= 201806L
constexpr
#endif
void scatter_write(basic_hash_processor<ch_type,Func>& out,io_scatters_t sp)
{
	for(std::size_t i{};i!=sp.len;++i)
		details::hash_processor_impl::hash_write_impl<std::byte>(out,reinterpret_cast<std::byte const*>(sp.base[i].base),reinterpret_cast<std::byte const*>(sp.base[i].base)+sp.base[i].len);
}

template<typename Func>
class hash_processor:public basic_hash_processor<char,Func>
{
public:
	using typename basic_hash_processor<char,Func>::char_type;
	using typename basic_hash_processor<char,Func>::function_type;
	using basic_hash_processor<char,Func>::block_size;
	constexpr hash_processor(Func& func) noexcept:basic_hash_processor<char,Func>(func){}
};

template<typename Func>
hash_processor(Func& func)->hash_processor<Func>;

template<std::integral char_type,typename T>
inline constexpr void require_secure_clear(basic_hash_processor<char_type,T>&){}

}
