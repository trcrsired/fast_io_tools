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

/*
Returns the first print_index that has print_define only. returns sizeof...(Args) if we do not find any print_index one
*/
template<::std::integral char_type, typename... Args>
inline consteval ::std::size_t first_print_define_index() noexcept
{
	template for(constexpr auto i: ::fast_io::details::create_index_array<sizeof...(Args)>())
	{
		using ArgsIType = Args...[i];
		if constexpr(!(::fast_io::printable<char_type, ArgsIType> || ::fast_io::reserve_printable<char_type, ArgsIType> ||
	  ::fast_io::dynamic_reserve_printable<char_type, ArgsIType> || ::fast_io::scatter_printable<char_type, ArgsIType> ||
	  ::fast_io::reserve_scatters_printable<char_type, ArgsIType> || ::fast_io::context_printable<char_type, ArgsIType>))
		{
			return i;
		}
	}
	return sizeof...(Args);
}

template<typename... Args>
consteval auto make_nonnull_index_sequence() noexcept
{
    constexpr ::std::size_t n = sizeof...(Args);

    // Compute count without mutation
    constexpr ::std::size_t count{(( !::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::io_null_t> ? 1zu : 0zu ) + ...)};

    // Build compact index array
    ::fast_io::freestanding::array<::std::size_t, count> tmp{};

    ::std::size_t pos2{};

    // Fill tmp using index_sequence
    [&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
        (([&]{
            using T = ::std::remove_cvref_t<Args...[pos]>;
            if constexpr (!::std::same_as<T, ::fast_io::io_null_t>)
                tmp[pos2++] = pos;
        }()), ...);
    }(::std::make_index_sequence<n>{});

    // Convert array ? index_sequence
    return [&]<::std::size_t... idx>(::std::index_sequence<idx...>) {
        return ::std::index_sequence<tmp[idx]...>{};
    }(::std::make_index_sequence<count>{});
}

}

namespace fast_io::operations::decay
{
template <bool line, typename outputstmtype, typename... Args>
inline constexpr decltype(auto) print_freestanding_decay2(outputstmtype optstm, Args... args) FAST_IO_HERBCEPTIONS_THROWS
{
	using output_char_type = typename outputstmtype::output_char_type;
	if constexpr ((::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::io_null_t> || ...))
	{
		constexpr auto seq = ::fast_io::details::make_nonnull_index_sequence<Args...>();
		return [&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
			return ::fast_io::operations::decay::print_freestanding_decay2<line>(optstm, args...[pos]...);
		}(seq);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_status_print_define<outputstmtype>)
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
	else
	{
		constexpr ::std::size_t split_pos{::fast_io::details::first_print_define_index<outputstmtype, Args...>()};
		if constexpr (split_pos != n)
		{
			// Left side: 0 .. split_pos-1
			if constexpr...(pos != 0)
			{
				[&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
					::fast_io::operations::decay::print_freestanding_decay2<line>(optstm, args...[pos]...);
				}(::std::make_index_sequence<split_pos>{});
			}
			// Middle element: split_pos
			using mid_type = ::std::remove_cvref_t<Args...[split_pos]>;
			print_define(optstm, args...[split_pos]);

			// Right side: split_pos+1 .. n-1
			if constexpr(split_pos + 1 == n)
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
			else
			{
				[&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
					::fast_io::operations::decay::print_freestanding_decay2<line>(optstm, args...[split_pos + pos]...);
				}(::std::make_index_sequence<n - split_pos - 1>{});
			}
		}
		else
#if 0
		else if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype>)
		{

		}
		else
#endif
		{
			constexpr bool is_buffer_output_stream{::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype>};
			template for(constexpr auto i: ::fast_io::details::create_index_array<sizeof...(Args)>())
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

}
