#pragma once

namespace fast_io
{

namespace manipulators
{

template<typename value_type>
struct basic_leb128_get_put
{
	using manip_tag = manip_tag_t;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	value_type reference;
};

template<::fast_io::details::my_integral T>
inline constexpr auto leb128_put(T t) noexcept
{
	if constexpr(::fast_io::details::my_unsigned_integral<T>)
	{
		if constexpr(sizeof(T)<=sizeof(unsigned))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<unsigned>{static_cast<unsigned>(t)};
		}
		else if constexpr(sizeof(T)<=sizeof(::std::size_t))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<::std::size_t>{static_cast<::std::size_t>(t)};
		}
		else
		{
			return ::fast_io::manipulators::basic_leb128_get_put<std::remove_cvref_t<T>>{t};
		}
	}
	else
	{
		if constexpr(sizeof(T)<=sizeof(int))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<int>{static_cast<int>(t)};
		}
		else if constexpr(sizeof(T)<=sizeof(::std::ptrdiff_t))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<::std::ptrdiff_t>{static_cast<::std::ptrdiff_t>(t)};
		}
		else
		{
			return ::fast_io::manipulators::basic_leb128_get_put<std::remove_cvref_t<T>>{t};
		}
	}
}

template<::fast_io::details::my_integral T>
inline constexpr auto leb128_get(T & t) noexcept
{
	return basic_leb128_get_put<T*>{ __builtin_addressof(t) };
}

}

template<std::integral char_type,::fast_io::details::my_integral T>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_leb128_get_put<T>>) noexcept
{
	constexpr std::size_t digits{std::numeric_limits<T>::digits + (::fast_io::details::my_signed_integral<T>)};
	constexpr std::size_t seven{7};
	constexpr std::size_t digitsdiv7{digits/seven};
	constexpr bool notsevenmul{(digits%seven)!=0};
	constexpr std::size_t urret{notsevenmul+digitsdiv7};
	return urret;
}

namespace details
{

template<::fast_io::details::my_unsigned_integral T,std::size_t n>
requires (n%7==0&&n!=0)
inline constexpr T calculate_v_leb128_mask_impl() noexcept
{
	T t{};
	for(std::size_t i{7};i!=n;i+=7)
	{
		t<<=8;
		t|=0x80;
	}
	if constexpr(::std::endian::native!=::std::endian::little)
	{
		t=::fast_io::byte_swap(t);
	}
	return t;
}

template<::fast_io::details::my_unsigned_integral T,std::size_t n>
inline constexpr T leb128_mask{calculate_v_leb128_mask_impl<T,n>()};

template<std::integral char_type,::fast_io::details::my_integral T>
inline constexpr char_type* pr_rsv_leb128_impl(char_type* iter,T value) noexcept
{
	constexpr bool islittle{::std::endian::native==::std::endian::little};
	using unsigned_char_type =
		::std::make_unsigned_t<char_type>;
	if constexpr(::fast_io::details::my_unsigned_integral<T>)
	{
#if __cpp_if_consteval > 202106L
		if !consteval
#else
		if(!std::is_constant_evaluated())
#endif
		{
			if constexpr(sizeof(char_type)==1&&(islittle||::std::endian::native==::std::endian::big)&&false)
			{
				if constexpr(::std::numeric_limits<T>::digits == 64)
				{
					{
						T temp{static_cast<T>(value&0x7f)};
						if constexpr(islittle)
						{
							temp+=static_cast<T>(((value>>7)&0x7f)<<8);
							temp+=static_cast<T>(((value>>14)&0x7f)<<16);
							temp+=static_cast<T>(((value>>21)&0x7f)<<24);
							temp+=static_cast<T>(((value>>28)&0x7f)<<32);
							temp+=static_cast<T>(((value>>35)&0x7f)<<40);
							temp+=static_cast<T>(((value>>42)&0x7f)<<48);
							temp+=static_cast<T>(((value>>49)&0x7f)<<56);
						}
						else
						{
							temp<<=56;
							temp+=static_cast<T>(((value>>7)&0x7f)<<48);
							temp+=static_cast<T>(((value>>14)&0x7f)<<40);
							temp+=static_cast<T>(((value>>21)&0x7f)<<32);
							temp+=static_cast<T>(((value>>28)&0x7f)<<24);
							temp+=static_cast<T>(((value>>35)&0x7f)<<16);
							temp+=static_cast<T>(((value>>42)&0x7f)<<8);
							temp+=static_cast<T>((value>>49)&0x7f);
						}
						::std::uint_fast8_t itdiff;
						if(value<(compile_pow2<T,35>))
						{
							if(value<(compile_pow2<T,14>))
							{
								if(value<(compile_pow2<T,7>))
								{
									temp+=leb128_mask<T,7>;
									itdiff=1;
								}
								else
								{
									temp+=leb128_mask<T,14>;
									itdiff=2;
								}
							}
							else if(value<(compile_pow2<T,28>))
							{
								if(value<(compile_pow2<T,21>))
								{
									temp+=leb128_mask<T,21>;
									itdiff=3;
								}
								else
								{
									temp+=leb128_mask<T,28>;
									itdiff=4;
								}
							}
							else
							{
								temp+=leb128_mask<T,35>;
								itdiff=5;
							}
						}
						else
						{
							if(value<(compile_pow2<T,56>))
							{
								if(value<(compile_pow2<T,42>))
								{
									temp+=leb128_mask<T,42>;
									itdiff=6;
								}
								else if(value<(compile_pow2<T,49>))
								{
									temp+=leb128_mask<T,49>;
									itdiff=7;
								}
								else
								{
									temp+=leb128_mask<T,56>;
									itdiff=8;
								}
							}
							else
							{
								temp+=leb128_mask<T,70>;
								unsigned_char_type const tmp1{static_cast<unsigned_char_type>(value>>56)};
								iter[8]=tmp1;
								if(tmp1<(compile_pow2<unsigned_char_type,7>))
								{
									itdiff=9;
								}
								else
								{
									iter[9]=0;
									itdiff=10;
								}
							}
						}
						__builtin_memcpy(iter,__builtin_addressof(temp),sizeof(temp));
						return iter+itdiff;
					}

				}
#if 0
				else if constexpr(::std::numeric_limits<T>::digits == 32)
				{
					T temp{static_cast<T>(value&0x7f)};
					if constexpr(::std::endian::native==::std::endian::little)
					{
						temp|=static_cast<T>(((value>>7)&0x7f)<<8);
						temp|=static_cast<T>(((value>>14)&0x7f)<<16);
						temp|=static_cast<T>(((value>>21)&0x7f)<<24);
					}
					else
					{
						temp<<=24;
						temp|=static_cast<T>(((value>>7)&0x7f)<<16);
						temp|=static_cast<T>(((value>>14)&0x7f)<<8);
						temp|=static_cast<T>((value>>21)&0x7f);
					}
					__builtin_memcpy(iter,__builtin_addressof(temp),sizeof(T));
					if((compile_pow2<T,28>)<=value)
					{
						iter[4]=static_cast<char_type>(static_cast<unsigned_char_type>(value>>28)|0x80);
						return iter+5;
					}
					else if((compile_pow2<T,21>)<=value)
					{
						return iter+4;
					}
					else if((compile_pow2<T,14>)<=value)
					{
						return iter+3;
					}
					else if((compile_pow2<T,7>)<=value)
					{
						return iter+2;
					}
					return iter+1;
				}
#endif
			}
		}	
		for(;;++iter)
		{
			unsigned_char_type temp{static_cast<unsigned_char_type>(value&0x7f)};
			value >>= 7;
			if(!value)
			{
				*iter=temp;
				return ++iter;
			}
			*iter=static_cast<unsigned_char_type>(temp|0x80);
		}
	}
	else
	{
		for(;;++iter)
		{
			unsigned_char_type temp{static_cast<unsigned_char_type>(value&0x7f)};
			value >>= 7;
			bool iszero{!value};
			if(iszero|(value==-1))
			{
				bool const m{(temp&0x40)!=0};
				if(iszero!=m)
				{
					*iter=temp;
					return ++iter;
				}
			}
			*iter=temp|0x80;
		}
	}
}

}

template<std::integral char_type,::fast_io::details::my_integral T>
inline constexpr char_type* print_reserve_define(
	io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_leb128_get_put<T>>,
	char_type *iter,
	::fast_io::manipulators::basic_leb128_get_put<T> v) noexcept
{
	return ::fast_io::details::pr_rsv_leb128_impl(iter,v.reference);
}

struct leb128_scan_state_t
{
	std::uint_least64_t group_count{};
};

template <std::integral char_type, typename I>
inline constexpr
io_type_t<leb128_scan_state_t> scan_context_type(io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I>>) noexcept
{
	return {};
}

namespace details
{
template <std::integral char_type, ::fast_io::details::my_integral I>
inline constexpr parse_result<char_type const*> scn_ctx_define_leb128_impl(
	std::uint_least64_t& group_count,
	char_type const* begin, char_type const* end,
	I& t) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	using U = ::fast_io::details::my_make_unsigned_t<I>;
	constexpr auto digits{ std::numeric_limits<U>::digits };
	constexpr auto remains{ digits % 7 };
	U tmp{};
	std::uint_least64_t cnt{ group_count };
	if (cnt == 0)
		t = 0;
	for (; begin != end; cnt += 7)
	{
		bool sign = static_cast<unsigned_char_type>(*begin) & 0x80;
		std::uint_fast8_t byte{static_cast<std::uint_fast8_t>(static_cast<unsigned_char_type>(*begin) & 0x7f)};
		++begin;
		if constexpr (::fast_io::details::my_signed_integral<I>)
		{
			constexpr auto lower_limit{ remains ? 1u << (remains - 1) : 0 };
			constexpr auto upper_limit{ remains ? (0xffu << (remains - 1)) & 0x7f : 0x7f };
			if (cnt > digits - 7 && (cnt > digits || (byte >= lower_limit && byte < upper_limit))) [[unlikely]]
				return { begin, parse_code::overflow };
		}
		else
		{
			if (cnt > digits - 7 && (cnt > digits || byte >= (1u << remains))) [[unlikely]]
				return { begin, parse_code::overflow };
		}
		tmp |= static_cast<U>(byte) << cnt;
		if (!sign)
		{
			if constexpr (::fast_io::details::my_signed_integral<I>)
			{
				if (byte & 0x40 && cnt < digits - 7)
					tmp |= static_cast<U>(-1) << (cnt + 7);
			}
			t |= tmp;
			return { begin, parse_code::ok };
		}
	}
	t |= tmp;
	group_count = cnt;
	return { begin, parse_code::partial };
}

template <std::integral char_type, ::fast_io::details::my_integral I>
inline constexpr parse_result<char_type const*>
	scn_cnt_define_leb128_impl(char_type const* begin, char_type const* end,I& t) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	using U = ::fast_io::details::my_make_unsigned_t<I>;
	constexpr auto digits{ std::numeric_limits<U>::digits };
	constexpr auto remains{ digits % 7 };
	U tmp{};
	std::uint_least64_t cnt{};
	t = 0;
	for (; begin != end; cnt += 7)
	{
		bool sign = static_cast<unsigned_char_type>(*begin) & 0x80;
		std::uint_fast8_t byte{static_cast<std::uint_fast8_t>(static_cast<unsigned_char_type>(*begin) & 0x7f)};
		++begin;
		if constexpr (::fast_io::details::my_signed_integral<I>)
		{
			constexpr auto lower_limit{ remains ? 1u << (remains - 1) : 0 };
			constexpr auto upper_limit{ remains ? (0xffu << (remains - 1)) & 0x7f : 0x7f };
			if (cnt > digits - 7 && (cnt > digits || (byte >= lower_limit && byte < upper_limit))) [[unlikely]]
				return { begin, parse_code::overflow };
		}
		else
		{
			if (cnt > digits - 7 && (cnt > digits || byte >= (1u << remains))) [[unlikely]]
				return { begin, parse_code::overflow };
		}
		tmp |= static_cast<U>(byte) << cnt;
		if (!sign)
		{
			if constexpr (::fast_io::details::my_signed_integral<I>)
			{
				if (byte & 0x40 && cnt < digits - 7)
					tmp |= static_cast<U>(-1) << (cnt + 7);
			}
			t |= tmp;
			return { begin, parse_code::ok };
		}
	}
	return { begin, parse_code::invalid };
}

}

template <std::integral char_type, ::fast_io::details::my_integral I>
inline constexpr parse_result<char_type const*> scan_context_define(
	io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I*>>,
	leb128_scan_state_t& state,
	char_type const* begin, char_type const* end,
	manipulators::basic_leb128_get_put<I*> t) noexcept
{
	return ::fast_io::details::scn_ctx_define_leb128_impl(state.group_count,begin,end,*t.reference);
}

template <std::integral char_type, ::fast_io::details::my_integral I>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I*>>, leb128_scan_state_t&, manipulators::basic_leb128_get_put<I*>) noexcept
{
	return parse_code::end_of_file;
}

template <std::integral char_type, ::fast_io::details::my_integral I>
inline constexpr
parse_result<char_type const*> scan_contiguous_define(
	io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I*>>,
	char_type const* begin, char_type const* end,
	manipulators::basic_leb128_get_put<I*> t) noexcept
{
	return ::fast_io::details::scn_cnt_define_leb128_impl(begin,end,*t.reference);
}

}
