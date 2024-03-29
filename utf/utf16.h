#pragma once

namespace fast_io::details
{
inline constexpr bool is_utf16_surrogate(char16_t uc) noexcept { return (uc - 0xd800u) < 2048u; }
inline constexpr bool is_utf16_high_surrogate(char16_t uc) noexcept { return (uc & 0xfffffc00) == 0xd800; }
inline constexpr bool is_utf16_low_surrogate(char16_t uc) noexcept { return (uc & 0xfffffc00) == 0xdc00; }

inline constexpr char32_t utf16_surrogate_to_utf32(char16_t high, char16_t low) noexcept
{ 
	return static_cast<char32_t>((static_cast<std::uint_least32_t>(high) << 10u) + low - 0x35fdc00u); 
}

template<typename T>
inline constexpr deco_result<char16_t,typename T::output_char_type> utf16_to_other_impl(
	char16_t const *fromfirst,char16_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	using char_type = typename T::output_char_type;
#if 0
	if constexpr(::std::numeric_limits<::std::uint_least8_t>::digits==8)
	{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if(!__builtin_is_constant_evaluated())
#endif
	{
		constexpr std::size_t N{::fast_io::details::optimal_simd_vector_run_with_cpu_instruction_size};
		if constexpr(N==16||N==32||N==64)
		{
			auto [fromit,toit]=utf16_to_other_simd_tolast_impl<N,T>(fromfirst,fromlast,tofirst,tolast);
			fromfirst=fromit;
			tofirst=toit;
		}
	}
	}
#endif
	while(fromfirst!=fromlast&&tofirst!=tolast)
	{
		char16_t v0{*fromfirst};
		if constexpr(!::std::same_as<char_type,char32_t>)
		{
			if(v0<0x80u)
			{
				*tofirst=static_cast<char_type>(v0);
				++tofirst;
				++fromfirst;
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff<fromdiff)
				{
					fromdiff=todiff;
				}
				for(;fromdiff;--fromdiff)
				{
					v0=*fromfirst;
					if(0x80u<=v0)
					{
						break;
					}
					*tofirst=static_cast<char_type>(v0);
					++tofirst;
					++fromfirst;
				}
				if(!fromdiff)
				{
					break;
				}
			}

		}
		if(!is_utf16_surrogate(v0))
		{
			if constexpr(::std::same_as<char_type,char32_t>)
			{
				*tofirst=static_cast<char_type>(v0);
				++tofirst;
				++fromfirst;
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff<fromdiff)
				{
					fromdiff=todiff;
				}
				for(;fromdiff;--fromdiff)
				{
					v0=*fromfirst;
					if(is_utf16_surrogate(v0))
					{
						break;
					}
					*tofirst=static_cast<char_type>(v0);
					++tofirst;
					++fromfirst;
				}
				if(!fromdiff)
				{
					break;
				}
			}
			else
			{
				auto ret{T::get_code_point(tofirst,static_cast<std::size_t>(tolast-tofirst),v0)};
				if(!ret)
				{
					break;
				}
				tofirst+=ret;
				++fromfirst;
				continue;
			}
		}
		char32_t codepoint{0xFFFD};
		::std::size_t tomov{1};
		if (is_utf16_high_surrogate(v0))
		{
			if (fromfirst+1 == fromlast )
			{
				break;
			}
			auto v1{fromfirst[1]};
			if (is_utf16_low_surrogate(v1))
			{
				codepoint = utf16_surrogate_to_utf32(v0,v1);
				++tomov;
			}
		}
		if constexpr(::std::same_as<char_type,char32_t>)
		{
			*tofirst=codepoint;
			++tofirst;
			fromfirst+=tomov;
		}
		else
		{
			auto ret{T::get_code_point(tofirst,static_cast<std::size_t>(tolast-tofirst),codepoint)};
			if(!ret)
			{
				break;
			}
			tofirst+=ret;
			fromfirst+=tomov;
		}
	}
	return {fromfirst,tofirst};
}

inline constexpr deco_result<char16_t,char8_t> utf16_to_utf8_impl(
	char16_t const *fromfirst,char16_t const *fromlast,
	char8_t *tofirst,char8_t *tolast) noexcept
{
	return utf16_to_other_impl<utf32_to_utf8_simple>(fromfirst,fromlast,tofirst,tolast);
}

inline constexpr deco_result<char16_t,char32_t> utf16_to_utf32_impl(
	char16_t const *fromfirst,char16_t const *fromlast,
	char32_t *tofirst,char32_t *tolast) noexcept
{
	return utf16_to_other_impl<utf8_to_utf32_simple>(fromfirst,fromlast,tofirst,tolast);
}

}
