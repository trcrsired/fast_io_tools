#pragma once

namespace fast_io::win32_gdi::details
{

template<win32_family family,std::integral char_type,typename Func>
inline constexpr decltype(auto) to_gdi_path_impl(char_type const* t,std::size_t n,Func f)
{
	if constexpr(sizeof(char_type)==sizeof(char16_t))
	{
		using char16_may_alias_ptr_t [[gnu::may_alias]] = char16_t const*;
		return f(reinterpret_cast<char16_may_alias_ptr_t>(t));
	}
	else
	{
		::fast_io::win32_family_api_encoding_converter<family> converter(t,n);
		return f(converter.c_str());
	}
}

template<win32_family family,std::integral char_type,std::size_t n,typename Func>
inline constexpr decltype(auto) to_gdi_path(char_type const (&t)[n],Func f)
{
	return to_gdi_path_impl<family>(t,n-1,f);
}

template<win32_family family,typename T,typename Func>
requires (!std::is_array_v<std::remove_cvref_t<T>>&&std::ranges::contiguous_range<std::remove_cvref_t<T>>&&std::integral<::std::ranges::range_value_t<T>>&&requires(T t)
{
	t.c_str();
})
inline constexpr decltype(auto) to_gdi_path(T&& t,Func f)
{
	return to_gdi_path_impl<family>(::std::ranges::data(t),::std::ranges::size(t),f);
}

template<typename T>
concept constructible_to_win32_gdi_path = (std::is_array_v<std::remove_cvref_t<T>>&&std::integral<std::remove_extent_t<std::remove_cvref_t<T>>>)
||(::std::ranges::contiguous_range<std::remove_cvref_t<T>>&&std::integral<std::ranges::range_value_t<T>>&&requires(T&& t)
{
	t.c_str();
})||(std::is_pointer_v<std::remove_cvref_t<T>>&&std::integral<std::remove_pointer_t<std::remove_cvref_t<T>>>);

}
