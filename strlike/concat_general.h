#pragma once

namespace fast_io::details::decay
{

template<bool line,std::integral ch_type,typename T,typename Arg>
inline constexpr T basic_general_concat_decay_impl_precise(Arg arg)
{
	std::size_t precise_size{print_reserve_precise_size(io_reserve_type<ch_type,Arg>,arg)};
	std::size_t precise_size_with_line{precise_size};
	if constexpr(line)
		++precise_size_with_line;
	T str;
	constexpr std::size_t local_cap{strlike_sso_size(io_reserve_type<ch_type,T>)};
	if(local_cap<precise_size_with_line)
		strlike_reserve(io_strlike_type<ch_type,T>,str,local_cap);
	auto first{strlike_begin(io_strlike_type<ch_type,T>,str)};
	print_reserve_precise_define(io_reserve_type<ch_type,Arg>,first,precise_size,arg);
	auto ptr{first+precise_size};
	if constexpr(line)
	{
		*ptr=char_literal_v<u8'\n',ch_type>;
		++ptr;
	}
	strlike_set_curr(io_strlike_type<ch_type,T>,str,ptr);
	return str;
}

template<bool line,std::integral ch_type,typename T,typename... Args>
inline constexpr T basic_general_concat_decay_impl(Args ...args)
{
	if constexpr(sizeof...(Args)==0)
	{
		if constexpr(line)
		{
			return strlike_construct_single_character_define(io_strlike_type<ch_type,T>,char_literal_v<u8'\n',ch_type>);
		}
		else
		{
			return {};
		}
	}
	else if constexpr(((reserve_printable<ch_type,Args>||scatter_printable<ch_type,Args>||dynamic_reserve_printable<ch_type,Args>)&&...))
	{
		constexpr std::size_t sz{calculate_scatter_reserve_size<ch_type,Args...>()};
		if constexpr(line)
			static_assert(sz!=SIZE_MAX,"overflow\n");
		constexpr std::size_t sz_with_line{sz+static_cast<std::size_t>(line)};
		if constexpr((reserve_printable<ch_type,Args>&&...))
		{
			if constexpr(sso_buffer_strlike<ch_type,T>)
			{
				constexpr std::size_t local_cap{strlike_sso_size(io_reserve_type<ch_type,T>)};
				constexpr bool not_enough_space{(local_cap<sz_with_line)};
				if constexpr(not_enough_space && ((sizeof...(Args)==1)&&(precise_reserve_printable<ch_type,Args>&&...)))
				{
					return basic_general_concat_decay_impl_precise<line,ch_type,T>(args...);
				}
				else
				{
					T str;
					if constexpr(not_enough_space)
						strlike_reserve(io_strlike_type<ch_type,T>,str,sz_with_line);
					strlike_set_curr(io_strlike_type<ch_type,T>,str,
						print_reserve_define_chain_impl<line>(strlike_begin(io_strlike_type<ch_type,T>,str),args...));
					return str;
				}
			}
			else
			{
				T str;
				strlike_reserve(io_strlike_type<ch_type,T>,str,sz_with_line);
				strlike_set_curr(io_strlike_type<ch_type,T>,str,
					print_reserve_define_chain_impl<line>(strlike_begin(io_strlike_type<ch_type,T>,str),args...));
				return str;
			}
		}
		else
		{
			if constexpr((!line)&&sizeof...(args)==1&&(scatter_printable<ch_type,Args>&&...))
			{
				basic_io_scatter_t<ch_type> scatter{print_scatter_define_extract_one<ch_type>(args...)};
				return strlike_construct_define(io_strlike_type<ch_type,T>(scatter.base,scatter.base+scatter.len));
			}
			else
			{
				std::size_t total_size{::fast_io::details::intrinsics::add_or_overflow_die(sz_with_line,calculate_scatter_dynamic_reserve_size_with_scatter<ch_type>(args...))};
				T str;
				strlike_reserve(io_strlike_type<ch_type,T>,str,total_size);
				strlike_set_curr(io_strlike_type<ch_type,T>,str,
					print_reserve_define_chain_scatter_impl<line>(strlike_begin(io_strlike_type<ch_type,T>,str),args...));
				return str;
			}
		}
	}
	else
	{
		T str;
		auto oref{io_strlike_ref(fast_io::io_alias,str)};
		::fast_io::print_freestanding_decay_no_status<line>(oref,args...);
		return str;
	}
}

template<bool line,std::integral ch_type,typename T,typename Arg>
inline constexpr void basic_general_concat_decay_ref_impl_precise(T& str,Arg arg)
{
	std::size_t precise_size{print_reserve_precise_size(io_reserve_type<ch_type,Arg>,arg)};
	std::size_t precise_size_with_line{precise_size};
	if constexpr(line)
		++precise_size_with_line;
	constexpr std::size_t local_cap{strlike_sso_size(io_reserve_type<ch_type,T>)};
	if(local_cap<precise_size_with_line)
		strlike_reserve(io_strlike_type<ch_type,T>,str,local_cap);
	auto first{strlike_begin(io_strlike_type<ch_type,T>,str)};
	print_reserve_precise_define(io_reserve_type<ch_type,Arg>,first,precise_size,arg);
	auto ptr{first+precise_size};
	if constexpr(line)
	{
		*ptr=char_literal_v<u8'\n',ch_type>;
		++ptr;
	}
	strlike_set_curr(io_strlike_type<ch_type,T>,str,ptr);
}

template<bool line,std::integral ch_type,typename T,typename... Args>
inline constexpr void basic_general_concat_decay_ref_impl(T& str,Args ...args)
{

	if constexpr(((reserve_printable<ch_type,Args>||scatter_printable<ch_type,Args>||dynamic_reserve_printable<ch_type,Args>)&&...))
	{
		constexpr std::size_t sz{calculate_scatter_reserve_size<ch_type,Args...>()};
		if constexpr(line)
			static_assert(sz!=SIZE_MAX,"overflow\n");
		constexpr std::size_t sz_with_line{sz+static_cast<std::size_t>(line)};
		if constexpr((reserve_printable<ch_type,Args>&&...))
		{
			if constexpr(sso_buffer_strlike<ch_type,T>)
			{
				constexpr std::size_t local_cap{strlike_sso_size(io_reserve_type<ch_type,T>)};
				constexpr bool not_enough_space{(local_cap<sz_with_line)};
				if constexpr(not_enough_space && ((sizeof...(Args)==1)&&(precise_reserve_printable<ch_type,Args>&&...)))
				{
					return basic_general_concat_decay_ref_impl_precise<line,ch_type,T>(str,args...);
				}
				else
				{
					if constexpr(not_enough_space)
						strlike_reserve(io_strlike_type<ch_type,T>,str,sz_with_line);
					strlike_set_curr(io_strlike_type<ch_type,T>,str,
						print_reserve_define_chain_impl<line>(strlike_begin(io_strlike_type<ch_type,T>,str),args...));
				}
			}
			else
			{
				strlike_reserve(io_strlike_type<ch_type,T>,str,sz_with_line);
				strlike_set_curr(io_strlike_type<ch_type,T>,str,
					print_reserve_define_chain_impl<line>(strlike_begin(io_strlike_type<ch_type,T>,str),args...));
			}
		}
		else
		{
			if constexpr((!line)&&sizeof...(args)==1&&(scatter_printable<ch_type,Args>&&...))
			{
				basic_io_scatter_t<ch_type> scatter{print_scatter_define_extract_one<ch_type>(args...)};
				strlike_construct_define(io_strlike_type<ch_type,T>(scatter.base,scatter.base+scatter.len));
			}
			else
			{
				std::size_t total_size{::fast_io::details::intrinsics::add_or_overflow_die(sz_with_line,calculate_scatter_dynamic_reserve_size_with_scatter<ch_type>(args...))};
				strlike_reserve(io_strlike_type<ch_type,T>,str,total_size);
				strlike_set_curr(io_strlike_type<ch_type,T>,str,
					print_reserve_define_chain_scatter_impl<line>(strlike_begin(io_strlike_type<ch_type,T>,str),args...));
			}
		}
	}
	else
	{
		auto oref{io_strlike_ref(fast_io::io_alias,str)};
		::fast_io::print_freestanding_decay_no_status<line>(oref,args...);
	}
}

template<bool line,std::integral ch_type,typename T,typename... Args>
inline constexpr T basic_general_concat_phase1_decay_impl(Args ...args)
{
	if constexpr(sizeof...(Args)==0)
	{
		if constexpr(line)
		{
			return strlike_construct_single_character_define(io_strlike_type<ch_type,T>,char_literal_v<u8'\n',ch_type>);
		}
		else
		{
			return {};
		}
	}
	else
	{
		if constexpr(buffer_strlike<ch_type,T>)
		{
			T str;
			basic_general_concat_decay_ref_impl<line,ch_type>(str,args...);
			return str;
		}
		else
		{
			basic_concat_buffer<ch_type> buffer;
			basic_general_concat_decay_ref_impl<line,ch_type>(buffer,args...);
			return strlike_construct_define(io_strlike_type<ch_type,T>,buffer.buffer_begin,buffer.buffer_curr);
		}
	}
}

}

namespace fast_io
{

template<bool line,std::integral char_type,typename T,typename ...Args>
requires strlike<char_type,T>
inline constexpr T basic_general_concat(Args&& ...args)
{
	return ::fast_io::details::decay::basic_general_concat_phase1_decay_impl<false,char_type,T>(
		io_print_forward<char_type>(io_print_alias(args))...);
}

template<typename ...Args>
inline constexpr std::string test_concat(Args&& ...args)
{
	return ::fast_io::details::decay::basic_general_concat_decay_impl<false,char,std::string>(io_print_forward<char>(io_print_alias(args))...);
}

template<typename ...Args>
inline constexpr std::string test_concat2(Args&& ...args)
{
	return ::fast_io::basic_general_concat<false,char,std::string>(io_print_forward<char>(io_print_alias(args))...);
}

}
