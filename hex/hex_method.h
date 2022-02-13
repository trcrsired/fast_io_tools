#pragma once

/*
Algorithm: fast_io hex
Author: cqwrteur
*/

namespace fast_io::details::fast_io_hex
{

template<std::size_t n,bool upper,std::integral char_type>
inline constexpr void iohex_hash_n_ul32(char_type* iter,std::uint_least32_t u,std::uint_least32_t len) noexcept
{
	constexpr auto tb{::fast_io::details::get_shared_inline_constexpr_base_table<char_type,16,upper>().element};
	constexpr std::uint_least32_t mask15{15};
	constexpr std::uint_least32_t mask255{255};
	if constexpr(n==8)
	{
		switch(len)
		{
			case 1:
			{
				*iter=tb[u][1];
				return;
			}
			case 2:
			{
				non_overlapped_copy_n(tb[u].element,2,iter);
				return;
			}
			case 3:
			{
				constexpr std::uint_least32_t shift{4};
				non_overlapped_copy_n(tb[u>>shift].element,2,iter);
				iter[2]=tb[u&mask15][1];
				return;
			}
			case 4:
			{
				constexpr std::uint_least32_t shift{8};
				non_overlapped_copy_n(tb[u>>shift].element,2,iter);
				non_overlapped_copy_n(tb[u&mask255].element,2,iter+2);
				return;
			}
			case 5:
			{
				constexpr std::uint_least32_t shift{12};
				non_overlapped_copy_n(tb[u>>shift].element,2,iter);
				constexpr std::uint_least32_t shift2{4};
				non_overlapped_copy_n(tb[(u>>shift2)&mask255].element,2,iter+2);
				iter[4]=tb[u&mask15][1];
				return;
			}
			case 6:
			{
				constexpr std::uint_least32_t shift{16};
				non_overlapped_copy_n(tb[u>>shift].element,2,iter);
				constexpr std::uint_least32_t shift2{8};
				non_overlapped_copy_n(tb[(u>>shift2)&mask255].element,2,iter+2);
				non_overlapped_copy_n(tb[u&mask255].element,2,iter+4);
				return;
			}
			case 7:
			{
				constexpr std::uint_least32_t shift{20};
				non_overlapped_copy_n(tb[u>>shift].element,2,iter);
				constexpr std::uint_least32_t shift2{12};
				non_overlapped_copy_n(tb[(u>>shift2)&mask255].element,2,iter+2);
				constexpr std::uint_least32_t shift3{4};
				non_overlapped_copy_n(tb[(u>>shift3)&mask255].element,2,iter+4);
				iter[6]=tb[u&mask15][1];
				return;
			}
			default:
			{
				constexpr std::uint_least32_t shift{24};
				non_overlapped_copy_n(tb[u>>shift].element,2,iter);
				constexpr std::uint_least32_t shift2{16};
				non_overlapped_copy_n(tb[(u>>shift2)&mask255].element,2,iter+2);
				constexpr std::uint_least32_t shift3{8};
				non_overlapped_copy_n(tb[(u>>shift3)&mask255].element,2,iter+4);
				non_overlapped_copy_n(tb[u&mask255].element,2,iter+6);
				return;
			}
		}
	}
	else
	{
		static_assert(n==SIZE_MAX,"Unsupported n");
	}
}

template<::fast_io::details::my_unsigned_integral U>
inline constexpr unsigned iohex_len(U u) noexcept
{
	constexpr U one_constant{1};
	constexpr unsigned log2base{static_cast<unsigned>(std::countr_zero(16u))};
	constexpr unsigned digits{static_cast<unsigned>(std::numeric_limits<U>::digits)-1u};
	unsigned const len{((digits-static_cast<unsigned>(std::countl_zero(u|one_constant)))>>2u)+1u};
	return len;
}

template<bool upper,std::integral char_type>
inline constexpr unsigned iohex_search_n_ul32(char_type* iter,std::uint_least32_t u) noexcept
{
	unsigned const len{iohex_len(u)};
	iohex_hash_n_ul32<8,upper>(iter,u,len);
	return len;
}

template<bool upper,std::integral char_type>
inline constexpr void iohex_ul8_full(char_type* iter,std::uint_least8_t u) noexcept
{
	constexpr auto tb{::fast_io::details::get_shared_inline_constexpr_base_table<char_type,16,upper>().element};
	non_overlapped_copy_n(tb[u],2,iter);
}

template<bool upper,std::integral char_type>
inline constexpr void iohex_ul16_full(char_type* iter,std::uint_least16_t u) noexcept
{
	constexpr auto tb{::fast_io::details::get_shared_inline_constexpr_base_table<char_type,16,upper>().element};
	constexpr std::uint_least32_t shift3{8};
	constexpr std::uint_least32_t mask255{255};
	non_overlapped_copy_n(tb[(u>>shift3)&mask255],2,iter);
	non_overlapped_copy_n(tb[u&mask255],2,iter+2);
}

template<bool upper,std::integral char_type>
inline constexpr void iohex_ul32_full(char_type* iter,std::uint_least32_t u) noexcept
{
	constexpr auto tb{::fast_io::details::get_shared_inline_constexpr_base_table<char_type,16,upper>().element};
	constexpr std::uint_least32_t mask255{255};
	constexpr std::uint_least32_t shift{24};
	non_overlapped_copy_n(tb[u>>shift].element,2,iter);
	constexpr std::uint_least32_t shift2{16};
	non_overlapped_copy_n(tb[(u>>shift2)&mask255].element,2,iter+2);
	constexpr std::uint_least32_t shift3{8};
	non_overlapped_copy_n(tb[(u>>shift3)&mask255].element,2,iter+4);
	non_overlapped_copy_n(tb[u&mask255].element,2,iter+6);
}

template<bool upper,std::integral char_type>
inline constexpr void iohex_ul64_full(char_type* iter,std::uint_least64_t u) noexcept
{
	iohex_ul32_full<upper>(iter,static_cast<std::uint_least32_t>(u>>32u));
	iohex_ul32_full<upper>(iter+8,static_cast<std::uint_least32_t>(u));
}

template<bool upper,std::integral char_type,::fast_io::details::my_unsigned_integral U>
requires (sizeof(U)<=16)
inline constexpr void iohex_ul_full(char_type* iter,U u) noexcept
{
	if constexpr(sizeof(U)==16)
	{
		iohex_ul64_full<upper>(iter,static_cast<std::uint_least64_t>(u>>64u));
		iohex_ul64_full<upper>(iter+16,static_cast<std::uint_least64_t>(u));
	}
	else if constexpr(sizeof(U)==8)
	{
		iohex_ul64_full<upper>(iter,u);
	}
	else if constexpr(sizeof(U)==4)
	{
		iohex_ul32_full<upper>(iter,u);
	}
	else if constexpr(sizeof(U)==2)
	{
		iohex_ul16_full<upper>(iter,u);
	}
	else
	{
		static_assert(sizeof(U)==1,"unknown size");
		iohex_ul8_full<upper>(iter,u);
	}
}

template<bool upper,::fast_io::freestanding::contiguous_iterator contiter,::fast_io::details::my_unsigned_integral U>
requires (sizeof(U)<=16)
inline constexpr void iohex_hash_n(contiter iter,U u,std::uint_least32_t len) noexcept
{
	if constexpr(std::is_pointer_v<contiter>)
	{
		if constexpr(sizeof(U)==16)
		{
			std::uint_least64_t value{static_cast<std::uint_least64_t>(u)};
			if(static_cast<std::uint_least64_t>(u)!=u)
			{
				iohex_ul64_full<upper>(iter+(len-=16u),value);
				value=static_cast<std::uint_least64_t>(u>>64u);
			}
			iohex_hash_n<upper>(iter,value,len);
		}
		else if constexpr(sizeof(U)==8)
		{
			std::uint_least32_t value{static_cast<std::uint_least32_t>(u)};
			if(static_cast<std::uint_least32_t>(u)!=u)
			{
				iohex_ul32_full<upper>(iter+(len-=8u),value);
				value=static_cast<std::uint_least32_t>(u>>32u);
			}
			iohex_hash_n_ul32<8,upper>(iter,value,len);
		}
		else
		{
			iohex_hash_n_ul32<8,upper>(iter,static_cast<std::uint_least32_t>(u),len);
		}
	}
	else
	{
		iohex_hash_n<upper>(::fast_io::freestanding::to_address(iter),u,len);
	}
}

template<bool upper,::fast_io::freestanding::contiguous_iterator contiter,my_unsigned_integral U>
inline constexpr contiter iohex(contiter iter,U u)
{
	if constexpr(std::is_pointer_v<contiter>)
	{
		if constexpr(sizeof(U)==16)
		{
			std::uint_least64_t value{static_cast<std::uint_least64_t>(u)};
			unsigned len,total_len;
			if(static_cast<std::uint_least64_t>(u)==u)
			{
				total_len=len=iohex_len(value);
			}
			else
			{
				std::uint_least64_t high{static_cast<std::uint_least64_t>(u>>64u)};
				total_len=(len=iohex_len(high))+16u;
				iohex_ul64_full<upper>(iter+len,value);
				value=high;
			}
			iohex_hash_n<upper>(iter,value,len);
			return iter+total_len;
		}
		else if constexpr(sizeof(U)==8)
		{
			std::uint_least32_t value{static_cast<std::uint_least32_t>(u)};
			unsigned len,total_len;
			if(static_cast<std::uint_least32_t>(u)==u)
			{
				total_len=len=iohex_len(value);
			}
			else
			{
				std::uint_least32_t high{static_cast<std::uint_least32_t>(u>>32u)};
				total_len=(len=iohex_len(high))+8u;
				iohex_ul32_full<upper>(iter+len,value);
				value=high;
			}
			iohex_hash_n_ul32<8,upper>(iter,value,len);
			return iter+total_len;
		}
		else
		{
			return iter+iohex_search_n_ul32<upper>(iter,static_cast<std::uint_least32_t>(u));
		}
	}
	else
	{
		return iohex<upper>(::fast_io::freestanding::to_address(iter),u)-::fast_io::freestanding::to_address(iter)+iter;
	}
}

}
