#pragma once

namespace fast_io
{
inline constexpr std::byte* write_some_bytes_define(posix_rw_proxy ent,std::byte* first,std::byte* last)
{
	return write(ent.fd,first,static_cast<::std::size_t>(last-first))+first;
}

namespace details
{

template<typename Func>
inline constexpr void write_all_common_impl(Func fnc,std::byte* first,std::byte* last) noexcept(noexcept(fnc(first,last)))
{
	for(;first!=last;first=fnc(first,last));
}

template<typename T>
inline constexpr void decay_write_all_impl(T t,std::byte const* first,std::byte const* last)
{
	write_all_common_impl([t](std::byte const* ilast ifirst,std::byte const* ilast)
	{
		return write_some_bytes_define(ifirst,ilast);
	});
}

template<typename T>
inline constexpr void decay_write_all_impl(T t,io_scatter_t const* scatters,::std::size_t n)
{
	for(auto i{scatters},e{i+n};i!=e;++i)
	{
		decay_write_all_impl(t,i->base,i->len);
	}
}
template<typename output,typename T>
inline constexpr void write_all_contiguous_impl(output&& out,T const* first,T const* last)
{
	::fast_io::details::write_all_common_impl(write_some_bytes_proxy(out),
		reinterpret_cast<std::byte const*>(first),
		reinterpret_cast<std::byte const*>(last));
}

template<typename output,::fast_io::freestanding::forward_iterator Iter>
inline constexpr void write_all_forward_iterator_impl(output out,Iter first,Iter last)
{
	for(;first!=last;++first)
	{
		auto p{__builtin_addressof(*first)};
		decay_write_all_impl(out,p,p+1);
	}
}


}

namespace operations
{

template<typename output,::fast_io::freestanding::forward_iterator Iter>
inline constexpr void write_all(output&& out,Iter first,Iter last)
{
	using nocvref_output = std::remove_cvref_t<output>;
	if constexpr(::std::is_pointer_v<Iter>&&::std::is_const_v<::std::remove_pointer_v>)
	{
		if constexpr(buffer_output_stream<nocvref_output>)
		{
			write_all_define(io_ref(out),first,last);
		}
		else if constexpr(write_some_bytes_opt<nocvref_output>)
		{
			::fast_io::details::write_all_contiguous_impl(write_some_bytes_proxy(out),
				reinterpret_cast<std::byte const*>(first),
				reinterpret_cast<std::byte const*>(last));
		}
	}
	else if constexpr(::fast_io::freestanding::contiguous_iterator<Iter>)
	{
		::fast_io::operations::write_all(out,::fast_io::freestanding::to_address(first),
				::fast_io::freestanding::to_address(last));
	}
	else
	{
		if constexpr(buffer_output_stream<nocvref_output>)
		{
			::fast_io::details::write_all_forward_iterator_impl(out,
				::fast_io::freestanding::to_address(first),
				::fast_io::freestanding::to_address(last));
		}
		else
		{
			::fast_io::details::write_all_forward_iterator_impl(write_some_bytes_proxy(out),first,last);
		}
	}
}

template<typename output>
inline constexpr void scatter_write_bytes_all(output&& out,io_scatters_t scatters)
{
	using nocvref_output = std::remove_cvref_t<output>;
	if constexpr(write_some_bytes_stream<nocvref_output>)
	{
		::fast_io::details::write_all_common_impl(write_some_bytes_proxy(out),
			reinterpret_cast<std::byte const*>(first),
			reinterpret_cast<std::byte const*>(last));
	}
}

template<typename output>
inline constexpr void scatter_write_all(output&& out,io_scatters_t scatters)
{
	using nocvref_output = std::remove_cvref_t<output>;
	if constexpr(write_some_bytes_stream<nocvref_output>)
	{
		::fast_io::details::write_all_common_impl(write_some_bytes_proxy(out),
			reinterpret_cast<std::byte const*>(first),
			reinterpret_cast<std::byte const*>(last));
	}
}

}

}