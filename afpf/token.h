#pragma once

struct token
{
std::size_t start=0;
std::size_t last=0;
};

struct tokens
{
token first,second,third,fourth;
};

inline constexpr std::size_t skip_tab(std::u8string_view token,std::size_t i) noexcept
{
	for(;i!=token.size()&&fast_io::char_category::is_c_space(token[i]);++i);
	return i;
}

inline constexpr std::size_t skip_to_tab(std::u8string_view token,std::size_t i) noexcept
{
	for(;i!=token.size()&&!fast_io::char_category::is_c_space(token[i]);++i);
	return i;
}

inline constexpr token get_next_token(std::u8string_view view,std::size_t start) noexcept
{
	auto s{skip_tab(view,start)};
	auto last{skip_to_tab(view,s)};
	return {s,last};
}

inline constexpr tokens parse_token(std::u8string_view view) noexcept
{
	token first{get_next_token(view,0)};
	token second{get_next_token(view,first.last)};
	token third{get_next_token(view,second.last)};
	token fourth{skip_tab(view,third.last),view.size()};
	return {first,second,third,fourth};
}
