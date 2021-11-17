#pragma once

namespace fast_io
{

namespace details
{

template<typename T>
concept test_default_constructible= requires(T&& t)
{
	t=T();
};
template<typename T>
concept test_default_constructible2= requires(T&& t)
{
	t=T{};
};

template<typename T>
concept test_default_constructible3= requires(T&& t)
{
	t={};
};
}

template<::fast_io::freestanding::input_or_output_iterator Iter>
inline constexpr Iter constexpr_sanitize_input(Iter first,[[maybe_unused]] Iter last) noexcept
{
#if defined(__GNUC__) && !defined(__clang__)
	if(first==last)
		return first;
	using it_value_t = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(::std::is_default_constructible_v<it_value_t>)
	{
		for(;first!=last;)
		{
			if constexpr(::fast_io::details::test_default_constructible<it_value_t>)
			{
				*first=it_value_t();
			}
			else if constexpr(::fast_io::details::test_default_constructible<it_value_t>)
			{
				*first=it_value_t{};
			}
			else if constexpr(::fast_io::details::test_default_constructible3<it_value_t>)
			{
				*first={};
			}
			else
			{
				return first;
			}
		}
	}
	else
	{
		return first;
	}
#else
	return first;
#endif
}

template<::fast_io::freestanding::input_or_output_iterator Iter>
inline constexpr Iter constexpr_sanitize_output([[maybe_unused]] Iter first,Iter last) noexcept
{
#if defined(__GNUC__) && !defined(__clang__)
	using it_value_t = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(::std::is_default_constructible_v<it_value_t>)
	{
		for(;first!=last;++first)
		{
			[[maybe_unused]] auto t{*first};
		}
	}
	return last;
#else
	return last;
#endif
}

}
