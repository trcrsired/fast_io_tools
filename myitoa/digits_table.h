#pragma once

namespace fast_io::details
{

template<::std::integral char_type,::std::size_t base>
requires (2<=base&&base<=36)
inline constexpr auto generate_digits_table() noexcept
{
	constexpr char8_t char8base{static_cast<char8_t>(base)};
	constexpr
		::std::size_t n{base*base*2};
	constexpr
		::std::size_t base2{base<<1};
	::fast_io::freestanding::array<char_type,n> tb;
	for(char8_t i{};i!=char8base;++i)
	{
		::std::size_t const ibase{static_cast<::std::size_t>(i)*base2};
		for(char8_t j{};j!=char8base;++j)
		{
			::std::size_t const jsz{static_cast<::std::size_t>(j)};
			::std::size_t const ibpjsz{ibase+(jsz<<1)};
			tb[ibpjsz]=::fast_io::char_literal<char_type>(i);
			tb[ibpjsz+1]=::fast_io::char_literal<char_type>(j);
		}
	}
	return tb;
}

template<::std::integral char_type,::std::size_t base>
inline constexpr auto digits_table_impl{generate_digits_table<char_type,base>()};

template<::std::integral char_type,::std::size_t base>
inline constexpr auto& get_digits_table() noexcept
{
	if(::fast_io::details::is_ebcdic<char_type>)
	{
		return digits_table_impl<char_type,base>;
	}
	else if constexpr(sizeof(char_type)==sizeof(char8_t))
	{
		return digits_table_impl<char8_t,base>;
	}
	else if constexpr(sizeof(char_type)==sizeof(char16_t))
	{
		return digits_table_impl<char16_t,base>;
	}
	else if constexpr(sizeof(char_type)==sizeof(char32_t))
	{
		return digits_table_impl<char32_t,base>;
	}
	else
	{
		return digits_table_impl<char_type,base>;
	}
}

template<::std::integral char_type,::std::size_t base>
requires (2<=base&&base<=36)
inline constexpr auto& digits_table{get_digits_table<char_type,base>()};

}
