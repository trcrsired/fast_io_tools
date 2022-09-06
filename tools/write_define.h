inline constexpr std::byte* write_some_bytes_define(posix_io_observer ent,std::byte* first,std::byte* last)
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


}

namespace operations
{

template<typename output,std::integral char_type>
inline constexpr void write_all(output&& out,char_type const* first,char_type const* last)
{
	using nocvref_output = std::remove_cvref_t<output>;
	if constexpr(buffer_output_stream<nocvref_output>)
	{

	}
	else if constexpr(write_some_bytes_stream<nocvref_output>)
	{
		::fast_io::details::write_all_common_impl(write_some_bytes_proxy(out),
			reinterpret_cast<std::byte const*>(first),
			reinterpret_cast<std::byte const*>(last));
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
