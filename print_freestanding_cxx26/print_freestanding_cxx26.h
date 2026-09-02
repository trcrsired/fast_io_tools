#pragma once

namespace fast_io::details
{

template<::std::size_t first, ::std::size_t last>
inline constexpr auto create_index_array_range() noexcept
{
    ::fast_io::freestanding::array<::std::size_t, static_cast<::std::size_t>(last-first)> arr;
    ::std::size_t i{first};
    for(auto& e : arr)
    {
        e = i;
        ++i;
    }
    return arr;
}

template<::std::size_t n>
inline constexpr auto create_index_array() noexcept
{
    constexpr auto arr{::fast_io::details::create_index_array_range<0,n>()};
    return arr;
}


}

namespace fast_io::operations::decay
{
template <bool line, typename outputstmtype, typename... Args>
inline constexpr decltype(auto) print_freestanding_decay2(outputstmtype optstm, Args... args) FAST_IO_HERBCEPTIONS_THROWS
{
	using output_char_type = typename outputstmtype::output_char_type;
	if constexpr (::fast_io::operations::decay::defines::has_status_print_define<outputstmtype>)
	{
		return status_print_define<line>(optstm, args...);
	}
	else if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			return ::fast_io::operations::decay::char_put_decay(optstm, ::fast_io::char_literal_v<u8'\n', output_char_type>);
		}
		else
		{
			return;
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outputstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::print_freestanding_decay2<line>(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm), args...);
	}
#if 0
	else if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype>)
	{

	}
	else
#endif
	{
		constexpr bool is_buffer_output_stream{::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype>};
		template for(auto const i: ::fast_io::details::create_index_array<sizeof...(Args)>())
		{

            using ArgsIType = Args...[i];
			constexpr bool islastwithlf{line&&(i+1zu == sizeof...(Args))};
			if constexpr(::fast_io::reserve_printable<output_char_type, ArgsIType>)
			{
				constexpr ::std::size_t reserved_space{print_reserve_size(::fast_io::io_reserve_type<output_char_type, ArgsIType>)
					+ static_cast<::std::size_t>(islastwithlf)};
				static_assert(reserved_space<=::std::numeric_limits<::std::ptrdiff_t>::max(), "reserve space too big");
				constexpr ::std::ptrdiff_t ptrdiff_reserved_space{static_cast<::std::ptrdiff_t>(reserved_space)};
				output_char_type buffer[reserved_space]
#if __has_cpp_attribute(indeterminate)
                [[indeterminate]]
#endif
                ;
				auto first{buffer};
				auto it{buffer};
				bool buffer_space_enough{};
				if constexpr (is_buffer_output_stream)
				{
					auto curr=obuffer_curr(optstm);
					auto end=obuffer_end(optstm);
					buffer_space_enough = (ptrdiff_reserved_space < end-curr);
				}
				it = print_reserve_define(::fast_io::io_reserve_type<output_char_type, ArgsIType>, it, args...[i]);
				if constexpr(islastwithlf)
				{
					*it = ::fast_io::char_literal_v<u8'\n', output_char_type>;
					++it;
				}
				if constexpr (is_buffer_output_stream)
				{
					if (buffer_space_enough) [[likely]]
					{
						obuffer_set_curr(out, it);
						continue;
					}
				}
				::fast_io::operations::decay::write_all_decay(optstm, first, it);
			}
		}
	}
}

}
