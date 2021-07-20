#pragma once

namespace fast_io
{

template<std::integral ch_type>
struct basic_http_header_line
{
	using char_type = ch_type;
	char_type const* first_begin{};
	char_type const* first_end{};
	char_type const* second_begin{};
	char_type const* second_end{};
	char_type const* third_begin{};
	char_type const* third_end{};
	inline constexpr basic_io_scatter_t<char_type> first() const noexcept
	{
		return {first_begin,static_cast<std::size_t>(first_end-first_begin)};
	}
	inline constexpr basic_io_scatter_t<char_type> second() const noexcept
	{
		return {second_begin,static_cast<std::size_t>(second_end-second_begin)};
	}
	inline constexpr basic_io_scatter_t<char_type> third() const noexcept
	{
		return {third_begin,static_cast<std::size_t>(third_end-third_begin)};
	}
	inline constexpr bool is_status_line() const noexcept
	{
		return third_begin;
	}
};

enum class http_line_stage
{
first_crlf,
};

template<std::integral char_type>
struct basic_http_line_buffer
{
	char_type buffer[8192];
	std::size_t pos{};
	sta
};

namespace details
{

template<::fast_io::freestanding::input_or_output_iterator Iter>
inline constexpr Iter find_crlf(Iter first,Iter last) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	for(;first!=last;++first)
		if(*first==sign_ch<u8'\r',char_type>)
		{
			auto next{first};
			++next;
			if(next!=last&&*next==sign_ch<u8'\n',char_type>)
				return first;
			first=next;
		}
	return first;
}

template<::fast_io::freestanding::input_or_output_iterator Iter>
inline constexpr void seperate_http_line(Iter first,Iter last,basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>& line) noexcept
{
	first=find_non_c_space(first,last);
	auto start_of_line{first};
	for(;first!=last&&((!::fast_io::char_category::is_c_space(*first))&&(*first!=sign_ch<u8':',char_type>));++first);
	auto first_part_end{first};
	first=find_non_c_space(first,last);
	if(first!=last&&*first==sign_ch<u8':',char_type>)
	{
		++first;
		line.third_end=line.third_begin=nullptr;
		line.second_begin=first;
		line.second_end=last;
		line.first_end=first_part_end;
		line.first_start=start_of_line;
	}
	else
	{
		auto second_start{first};
		first=find_c_space(first,last);
		line.third_begin=first;
		if(first!=last)
			++line.third_begin;
		line.third_end=last;
		line.second_begin=second_start;
		line.second_end=first;
		line.first_start=start_of_line;
		line.first_end=first_part_end;
	}
}

template<::fast_io::freestanding::random_access_iterator Iter>
inline constexpr parse_result<Iter> scan_contiguous_define_http_header_line_impl(Iter first,Iter last,basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>& line) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	std::size_t diff{static_cast<std::size_t>(last-first)};
	if(diff<4u)
		return {last,parse_code::partial};
	if(*first==sign_ch<u8'\r',char_type>&&first[1]==sign_ch<u8'\n',char_type>)
	{
		first+=2;
		if(*first==sign_ch<u8'\r',char_type>&&first[1]==sign_ch<u8'\n',char_type>)
		{
			return {first+=2,parse_code::eof};
		}
	}
	auto it{details::find_crlf(first,last)};
	if(it==last)
		return {last,parse_code::partial};
	details::seperate_http_line(first,it,line);
	return {it,parse_code::ok};
}

template<::fast_io::freestanding::random_access_iterator Iter>
inline constexpr parse_result<Iter> scan_context_define_http_header_line_impl(Iter first,Iter last,basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>& line) noexcept
{
	if(first==last)
		return {first,parse_code::partial};
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if(line.pos==1&&*line.buffer==sign_ch<u8'\r',char_type>)
	{
		if(*first==sign_ch<u8'\n',char_type>)
		{

		}
//		return {first,parse_code::partial};
	}
}

}

template<std::integral char_type>
inline constexpr auto scan_context_type(io_generator_type_t<char_type,parameter<basic_http_header_line<char_type>&>>) noexcept
{
	return io_type_t<basic_http_line_buffer<char_type>>;
}

template<::fast_io::freestanding::random_access_iterator Iter>
inline constexpr parse_result<Iter> scan_contiguous_define(io_generator_type_t<::fast_io::freestanding::iter_value_t<Iter>,parameter<basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>&>>,Iter begin,Iter end,parameter<basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>&> t) noexcept
{
	if constexpr(::fast_io::freestanding::contiguous_iterator<Iter>&&!std::is_pointer_v<Iter>)
	{
		auto [it,ec] = details::scan_contiguous_define_http_header_line_impl(::fast_io::freestanding::to_address(begin),::fast_io::freestanding::to_address(end),t.reference);
		return {it-::fast_io::freestanding::to_address(begin)+begin,ec};
	}
	else
		return details::scan_contiguous_define_http_header_line_impl(begin,end,t.reference);
}


template<::fast_io::freestanding::input_iterator Iter,typename State,details::my_integral T>
inline constexpr parse_result<Iter> scan_context_define2(io_generator_type_t<::fast_io::freestanding::iter_value_t<Iter>,parameter<basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>&>>,State& state,Iter begin,Iter end,parameter<basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>&> t) noexcept
{
	return details::scan_context_define_http_header_line_impl(state,begin,end,t.reference);
}

template<std::integral char_type,typename State,details::my_integral T>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,parameter<basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>&>>,State& state,parameter<basic_http_header_line<::fast_io::freestanding::iter_value_t<Iter>>&> t) noexcept
{
	return parse_code::invalid;
}

#if 0

template<buffer_input_stream input>
requires (value_based_stream<input>)
struct http_header_line_iterator
{
	using char_type = typename input::char_type;
	using line_type = basic_http_header_line<char_type>;
	using handle_type = input;

#if __has_cpp_attribute(no_unique_address)
[[no_unique_address]]
#endif
	line_type line{};
#if __has_cpp_attribute(no_unique_address)
[[no_unique_address]]
#endif
	handle_type handle;
#if __has_cpp_attribute(no_unique_address)
[[no_unique_address]]
#endif
	std::conditional_t<contiguous_input_stream<input>,details::empty,http_line_buffer> buffer;
	inline constexpr http_header_line_iterator& operator++()
	{
		if constexpr(contiguous_input_stream<input>)
		{
			auto curr{ibuffer_curr(handle)};
			auto ed{ibuffer_end(handle)};
			auto it{details::find_crlf(curr,ed)};
			if(static_cast<std::size_t>(ed-it)<4)
				throw_parse_code(parse_code::invalid);
			auto cit{it+2};
			if(*cit==sign_ch<u8'\r',char_type>)
			{
				++cit;
				if(*cit==sign_ch<u8'\n',char_type>)
				{
					++cit;
					ibuffer_set_curr(handle,cit);
					handle.release();
					return *this;
				}
			}
			details::seperate_http_line(curr,it,line);
			ibuffer_set_curr(handle,it);
		}
		else
		{
			auto curr{ibuffer_curr(handle)};
			auto ed{ibuffer_end(handle)};
			auto it{details::find_crlf(curr,ed)};

		}
		return *this;
	}
	inline constexpr basic_http_header_line<char_type>& operator*() noexcept
	{
		return line;
	}
	inline constexpr basic_http_header_line<char_type> const& operator*() const noexcept
	{
		return line;
	}
	inline constexpr basic_http_header_line<char_type>* operator->() noexcept
	{
		return __builtin_addressof(line);
	}
	inline constexpr basic_http_header_line<char_type> const* operator->() const noexcept
	{
		return __builtin_addressof(line);
	}
	http_header_line_iterator(http_header_line_iterator const&)=delete
	http_header_line_iterator& operator=(http_header_line_iterator const&)=delete
};
#endif


}
