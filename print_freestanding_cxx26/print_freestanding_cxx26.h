#pragma once

namespace fast_io::details
{

template <::std::integral char_type, typename T = char_type>
inline constexpr basic_io_scatter_t<T> line_scatter_common{
	__builtin_addressof(char_literal_v<u8'\n', char_type>),
	::std::same_as<T, void> ? sizeof(char_type) : 1};

template <::std::size_t first, ::std::size_t last>
inline constexpr auto create_index_array_range() noexcept
{
	::fast_io::containers::array<::std::size_t,
								 static_cast<::std::size_t>(last - first)>
		arr;
	::std::size_t i{first};
	for (auto &e : arr)
	{
		e = i;
		++i;
	}
	return arr;
}

template <::std::size_t n>
inline constexpr auto create_index_array() noexcept
{
	constexpr auto arr{::fast_io::details::create_index_array_range<0, n>()};
	return arr;
}

/*
Returns the first print_index that has print_define only. returns
sizeof...(Args) if we do not find any print_index one
*/
template <::std::integral char_type, typename... Args>
inline consteval ::std::size_t first_print_define_index() noexcept
{
	template for (constexpr auto i :
				  ::fast_io::details::create_index_array<sizeof...(Args)>())
	{
		using ArgsIType = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (!(::fast_io::printable<char_type, ArgsIType> ||
						::fast_io::reserve_printable<char_type, ArgsIType> ||
						::fast_io::dynamic_reserve_printable<char_type,
															 ArgsIType> ||
						::fast_io::scatter_printable<char_type, ArgsIType> ||
						::fast_io::reserve_scatters_printable<char_type,
															  ArgsIType> ||
						::fast_io::context_printable<char_type, ArgsIType>))
		{
			return i;
		}
	}
	return sizeof...(Args);
}

template <typename... Args>
consteval auto make_nonnull_index_sequence() noexcept
{
	constexpr ::std::size_t n = sizeof...(Args);

	// Compute count without mutation
	constexpr ::std::size_t count{
		((!::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::io_null_t>
			  ? 1zu
			  : 0zu) +
		 ...)};

	// Build compact index array
	::fast_io::containers::array<::std::size_t, count> tmp{};

	::std::size_t pos2{};

	// Fill tmp using index_sequence
	[&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
		(([&] {
			 using T = ::std::remove_cvref_t<Args...[pos]>;
			 if constexpr (!::std::same_as<T, ::fast_io::io_null_t>)
			 {
				 tmp[pos2++] = pos;
			 }
		 }()),
		 ...);
	}(::std::make_index_sequence<n>{});

	// Convert array ? index_sequence
	return [&]<::std::size_t... idx>(::std::index_sequence<idx...>) {
		return ::std::index_sequence<tmp[idx]...>{};
	}(::std::make_index_sequence<count>{});
}

template <::std::integral char_type, typename... Args>
consteval auto compute_total_normal_reserved_size_or_scatters_cache_count(
	bool compute_scatters_count) noexcept
{
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	::std::size_t total_sz{};
	template for (constexpr auto pos :
				  ::fast_io::details::create_index_array<sizeof...(Args)>())
	{
		using arg_type = ::std::remove_cvref_t<Args...[pos]>;
		::std::size_t sz{};
		if (compute_scatters_count)
		{
			if constexpr (!::std::same_as<::fast_io::basic_io_scatter_t<char_type>,
										  arg_type> &&
						  ::fast_io::scatter_printable<char_type, arg_type>)
			{
				sz = 1zu;
			}
			else if constexpr (::fast_io::reserve_scatters_printable<char_type,
																	 arg_type>)
			{
				sz = print_reserve_scatters_size(
						 ::fast_io::io_reserve_type<char_type, arg_type>)
						 .scatters_size;
			}
		}
		else
		{
			if constexpr (::fast_io::reserve_printable<char_type, arg_type>)
			{
				sz =
					print_reserve_size(::fast_io::io_reserve_type<char_type, arg_type>);
			}
			else if constexpr (::fast_io::reserve_scatters_printable<char_type,
																	 arg_type>)
			{
				sz = print_reserve_scatters_size(
						 ::fast_io::io_reserve_type<char_type, arg_type>)
						 .reserve_size;
			}
		}
		if (static_cast<::std::size_t>(mx - sz) < total_sz)
		{
			::fast_io::fast_terminate();
		}
		total_sz += sz;
	}

	return total_sz;
}

template <::std::integral char_type, typename... Args>
consteval auto compute_total_normal_reserved_size() noexcept
{
	return ::fast_io::details::
		compute_total_normal_reserved_size_or_scatters_cache_count<char_type,
																   Args...>(
			false);
}

template <::std::integral char_type, typename... Args>
consteval auto compute_total_scatters_count() noexcept
{
	return ::fast_io::details::
		compute_total_normal_reserved_size_or_scatters_cache_count<char_type,
																   Args...>(true);
}

} // namespace fast_io::details

namespace fast_io::operations::decay
{
template <bool line, typename outputstmtype, typename... Args>
inline constexpr decltype(auto)
print_freestanding_decay2(outputstmtype optstm,
						  Args... args) FAST_IO_HERBCEPTIONS_THROWS
{
	using output_char_type = typename outputstmtype::output_char_type;
	if constexpr ((::std::same_as<::std::remove_cvref_t<Args>,
								  ::fast_io::io_null_t> ||
				   ...))
	{
		constexpr auto seq =
			::fast_io::details::make_nonnull_index_sequence<Args...>();
		return [&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
			return ::fast_io::operations::decay::print_freestanding_decay2<line>(
				optstm, args...[pos]...);
		}(seq);
	}
	else if constexpr (::fast_io::operations::decay::defines::
						   has_status_print_define<outputstmtype>)
	{
		return status_print_define<line>(optstm, args...);
	}
	else if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			return ::fast_io::operations::decay::char_put_decay(
				optstm, ::fast_io::char_literal_v<u8'\n', output_char_type>);
		}
		else
		{
			return;
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::
						   has_output_or_io_stream_mutex_ref_define<
							   outputstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::print_freestanding_decay2<line>(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),
			args...);
	}
	else
	{
		constexpr ::std::size_t split_pos{
			::fast_io::details::first_print_define_index<output_char_type, Args...>()};
		if constexpr (split_pos != sizeof...(Args))
		{
			// Left side: 0 .. split_pos-1
			if constexpr (split_pos != 0)
			{
				[&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
					::fast_io::operations::decay::print_freestanding_decay2<line>(
						optstm, args...[pos]...);
				}(::std::make_index_sequence<split_pos>{});
			}
			// Middle element: split_pos
			using mid_type = ::std::remove_cvref_t<Args...[split_pos]>;
			print_define(optstm, args...[split_pos]);

			// Right side: split_pos+1 .. n-1
			if constexpr (split_pos + 1 == sizeof...(Args))
			{
				if constexpr (line)
				{
					return ::fast_io::operations::decay::char_put_decay(
						optstm, ::fast_io::char_literal_v<u8'\n', output_char_type>);
				}
				else
				{
					return;
				}
			}
			else
			{
				[&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
					::fast_io::operations::decay::print_freestanding_decay2<line>(
						optstm, args...[split_pos + pos]...);
				}(::std::make_index_sequence<sizeof...(Args) - split_pos - 1zu>{});
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
			constexpr ::std::size_t total_normal_reserved_size{
				::fast_io::details::compute_total_normal_reserved_size<
					output_char_type, Args...>()};
#if 0
			constexpr
				::std::size_t total_scatters_cached_count{::fast_io::details::compute_total_scatters_cached_count<output_char_type, Args...>()};
			::fast_io::containers::array<::fast_io::basic_io_scatter_t<output_char_type>, total_scatters_cached_count> scatters_cache FAST_IO_INDETERMINATE;
#endif
			constexpr bool use_dynamic_storage{
				(::fast_io::dynamic_reserve_printable<output_char_type,
													  ::std::remove_cvref_t<Args>> ||
				 ...)};
			::std::size_t total_reserved_size{total_normal_reserved_size};
			constexpr bool is_buffer_output_stream{
				::fast_io::operations::decay::defines::has_obuffer_basic_operations<
					outputstmtype>};
			output_char_type *curr FAST_IO_INDETERMINATE, *end FAST_IO_INDETERMINATE;
			if constexpr (is_buffer_output_stream)
			{
				curr = obuffer_curr(optstm);
				end = obuffer_end(optstm);
			}
			::std::size_t buffer_remained_spaces{
				static_cast<::std::size_t>(end - curr)};
			constexpr ::std::size_t szmx{
				::std::numeric_limits<::std::size_t>::max()};
			bool buffer_enough_space{total_reserved_size < buffer_remained_spaces};
			if (buffer_enough_space)
			{
				template for (constexpr auto i :
							  ::fast_io::details::create_index_array<sizeof...(
								  Args)>())
				{
					using argtype = ::std::remove_cvref_t<Args...[i]>;
					if constexpr (!::fast_io::reserve_printable<output_char_type,
																argtype>)
					{
						::std::size_t argsz;
						if constexpr (::fast_io::dynamic_reserve_printable<
										  output_char_type, argtype>)
						{
							argsz = print_reserve_size(
								::fast_io::io_reserve_type<output_char_type, argtype>,
								args...[i]);
							if (static_cast<::std::size_t>(szmx - argsz) <
								total_reserved_size)
							{
								::fast_io::fast_terminate();
							}
							total_reserved_size += argsz;
						}
						else if constexpr (::std::same_as<argtype,
														  ::fast_io::basic_io_scatter_t<
															  output_char_type>>)
						{
							argsz = args...[i].len;
						}
						if (buffer_remained_spaces <= argsz)
						{
							buffer_enough_space &= false;
						}
						buffer_remained_spaces -= argsz;
					}
				}
			}
			::fast_io::containers::array<
				output_char_type,
				use_dynamic_storage ? 0zu : total_normal_reserved_size>
				buffer FAST_IO_INDETERMINATE;
			::std::conditional_t<
				use_dynamic_storage,
				::fast_io::details::local_operator_new_array_ptr<output_char_type>,
				::fast_io::details::empty>
				dynamic_buffer;
			output_char_type *it{curr};
			if constexpr (!is_buffer_output_stream)
			{
				if constexpr (use_dynamic_storage)
				{
					it = dynamic_buffer.ptr = ::fast_io::details::allocate_iobuf_space<
						output_char_type,
						typename ::fast_io::details::local_operator_new_array_ptr<
							output_char_type>::allocator_type>(total_reserved_size);
					dynamic_buffer.size = total_reserved_size;
				}
				else
				{
					it = buffer.data();
				}
			}
			else if (!buffer_enough_space)
			{
				if constexpr (use_dynamic_storage)
				{
					it = dynamic_buffer.ptr = ::fast_io::details::allocate_iobuf_space<
						output_char_type,
						typename ::fast_io::details::local_operator_new_array_ptr<
							output_char_type>::allocator_type>(total_reserved_size);
					dynamic_buffer.size = total_reserved_size;
				}
				else
				{
					it = buffer.data();
				}
			}
			constexpr ::std::size_t requested_scatters{sizeof...(Args)}; // to fix
			constexpr bool only_one_scatter{requested_scatters < 2zu};
			::fast_io::containers::array<
				::fast_io::basic_io_scatter_t<output_char_type>,
				(only_one_scatter ? 0zu : requested_scatters)>
				scatters FAST_IO_INDETERMINATE;
			auto scatterbase{scatters.data()};
			auto scatterptr{scatterbase};
			output_char_type *bufferbase{it};
			::std::conditional_t<only_one_scatter, ::fast_io::details::empty,
								 output_char_type *>
				last_pos FAST_IO_INDETERMINATE;
			if constexpr (!only_one_scatter)
			{
				last_pos = bufferbase;
			}
			template for (constexpr auto i :
						  ::fast_io::details::create_index_array<sizeof...(
							  Args)>())
			{
				constexpr bool islastwithlf{line && (i + 1zu == sizeof...(Args))};
				using argtype = ::std::remove_cvref_t<Args...[i]>;
				if constexpr (::fast_io::reserve_printable<output_char_type,
														   argtype> ||
							  ::fast_io::dynamic_reserve_printable<output_char_type,
																   argtype>)
				{
					it = print_reserve_define(
						::fast_io::io_reserve_type<output_char_type, argtype>, it,
						args...[i]);
					if constexpr (islastwithlf)
					{
						*it = ::fast_io::char_literal_v<u8'\n', output_char_type>;
						++it;
					}
					if constexpr (!only_one_scatter &&
								  (static_cast<::std::size_t>(i + 1zu) ==
								   sizeof...(Args)
#if 0

										   ||
									   (static_cast<::std::size_t>(i + 1zu) !=
										   sizeof...(Args)&&!::fast_io::reserve_printable<
											output_char_type,
											::std::remove_cvref_t<Args...[i + 1zu]>> &&
										!::fast_io::dynamic_reserve_printable<
											output_char_type,
											::std::remove_cvref_t<Args...[i + 1zu]>>)
#endif
									   ))
					{
						using argtype = ::std::remove_cvref_t<Args...[i]>;
						if constexpr ((::fast_io::reserve_printable<output_char_type,
																	argtype> ||
									   ::fast_io::dynamic_reserve_printable<
										   output_char_type, argtype>))
						{
							if (buffer_enough_space) [[likely]]
							{
								continue;
							}
							*scatterptr = {last_pos,
										   static_cast<::std::size_t>(it - last_pos)};
							++scatterptr;
							if constexpr (static_cast<::std::size_t>(i + 1zu) !=
										  sizeof...(Args))
							{
								last_pos = it; // To do. eliminate this branch when all
											   // parameters after it do not exist
							}
						}
					}
				}
				else if constexpr (::std::same_as<argtype,
												  ::fast_io::basic_io_scatter_t<
													  output_char_type>>)
				{
					if (buffer_enough_space) [[likely]]
					{
						it = ::fast_io::details::copy_scatter(args...[i], it);
						if constexpr (islastwithlf)
						{
							*it = ::fast_io::char_literal_v<u8'\n', output_char_type>;
							++it;
						}
					}
					else
					{
						*scatterptr = args...[i];
						++scatterptr;
						if constexpr (islastwithlf)
						{
							*it = {::fast_io::char_literal_v<u8'\n', output_char_type>,
								   1zu};
							++it;
						}
					}
				}
			}
			if constexpr (is_buffer_output_stream)
			{
				if (buffer_enough_space) [[likely]]
				{
					obuffer_set_curr(optstm, it);
					return;
				}
			}
			if constexpr (only_one_scatter)
			{
				::fast_io::operations::decay::write_all_decay(optstm, bufferbase, it);
			}
			else
			{
				::fast_io::operations::decay::scatter_write_all_decay(
					optstm, scatterbase,
					static_cast<::std::size_t>(scatterptr - scatterbase));
			}
		}
	}
}

} // namespace fast_io::operations::decay
