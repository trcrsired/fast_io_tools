#pragma once

namespace fast_io::details::jeaiii
{

template<std::size_t n,std::integral char_type>
inline constexpr void jeaiii_hash(char_type* iter,std::uint_least32_t u,std::uint_least32_t len) noexcept
{
	if constexpr(n==7)
	{
		switch(len)
		{
			case 1:
			{
				jeaiii_c<0>(iter,u);
				return;
			}
			case 2:
			{
				jeaiii_c<1>(iter,u);
				return;
			}
			case 3:
			{
				jeaiii_c<2>(iter,u);
				return;
			}
			case 4:
			{
				jeaiii_c<3>(iter,u);
				return;
			}
			case 5:
			{
				jeaiii_c<4>(iter,u);
				return;
			}
			case 6:
			{
				jeaiii_c<5>(iter,u);
				return;
			}
			case 7:
			{
				jeaiii_c<6>(iter,u);
				return;
			}
			default:
			{
				jeaiii_c<7>(iter,u);
				return;
			}
		}
	}
	else if constexpr(n==9)
	{
		switch(len)
		{
			case 1:
			{
				jeaiii_c<0>(iter,u);
				return;
			}
			case 2:
			{
				jeaiii_c<1>(iter,u);
				return;
			}
			case 3:
			{
				jeaiii_c<2>(iter,u);
				return;
			}
			case 4:
			{
				jeaiii_c<3>(iter,u);
				return;
			}
			case 5:
			{
				jeaiii_c<4>(iter,u);
				return;
			}
			case 6:
			{
				jeaiii_c<5>(iter,u);
				return;
			}
			case 7:
			{
				jeaiii_c<6>(iter,u);
				return;
			}
			case 8:
			{
				jeaiii_c<7>(iter,u);
				return;
			}
			case 9:
			{
				jeaiii_c<8>(iter,u);
				return;
			}
			default:
			{
				jeaiii_c<9>(iter,u);
				return;
			}
		}
	}
	else
	{
		static_assert(n==SIZE_MAX,"no supported");
	}
}

template<bool ryu_mode=false,bool recursive=false,std::integral char_type,::fast_io::details::my_unsigned_integral U>
inline constexpr void jeaiii_main_len(char_type* iter,U n,std::uint_least32_t len) noexcept
{
	if constexpr(sizeof(U)>sizeof(std::uint_least64_t)&&sizeof(U)==16)//__uint128_t
	{
		if(static_cast<std::uint_least64_t>(n>>64u)==0)
		{
			return jeaiii_main_len(iter,static_cast<std::uint_least64_t>(n),len);
		}
		constexpr std::uint_least32_t full_length{19u};
		constexpr std::uint_least64_t divisor{10000000000000000000ull};
		U a{n/divisor};
		std::uint_least64_t u{static_cast<std::uint_least64_t>(n%divisor)};
		std::uint_least64_t alow{static_cast<std::uint_least64_t>(a)};
		if constexpr(ryu_mode)
		{
			iter=jeaiii_main_len(iter,static_cast<std::uint_least64_t>(alow),len-full_length);
		}
		else
		{
			if(a!=static_cast<U>(alow))
			{
				std::uint_least32_t v{static_cast<std::uint_least32_t>(a/divisor)};
				std::uint_least64_t m{static_cast<std::uint_least64_t>(a%divisor)};
				jeaiii_c<0>(iter,v);
				++iter;
				alow=m;
				jeaiii_main_len<false,true>(iter,static_cast<std::uint_least64_t>(alow),full_length);
				iter+=full_length;
			}
			else
			{
				std::uint_least32_t len_sub{len-full_length};
				jeaiii_main_len(iter,static_cast<std::uint_least64_t>(alow),len_sub);
				iter+=len_sub;
			}
		}
		jeaiii_main_len<false,true>(iter,static_cast<std::uint_least64_t>(u),full_length);
	}
	else if constexpr(sizeof(U)==sizeof(std::uint_least64_t))
	{
		constexpr std::uint_least32_t full_length{9u};
		constexpr std::uint_least32_t divisor{1000000000u};
		if constexpr(recursive)
		{
			std::uint_least64_t high{n/divisor};
			std::uint_least32_t low{static_cast<std::uint_least32_t>(n%divisor)};
			std::uint_least32_t high_first{static_cast<std::uint_least32_t>(high/divisor)};
			std::uint_least32_t high_low{static_cast<std::uint_least32_t>(high%divisor)};
			jeaiii_c<0>(iter,high_first);
			++iter;
			iter=jeaiii_f<8>(jeaiii_f<8>(iter,high_low),low);
		}
		else
		{
			if(static_cast<std::uint_least32_t>(n>>32u)==0)
			{
				return jeaiii_hash<9>(iter,static_cast<std::uint_least32_t>(n),len);
			}
			std::uint_least64_t a{n/divisor};
			std::uint_least32_t u{static_cast<std::uint_least32_t>(n%divisor)};
			std::uint_least32_t alow{static_cast<std::uint_least32_t>(a)};
			if constexpr(ryu_mode)
			{
				std::uint_least32_t len_sub{len-full_length};
				jeaiii_hash<7>(iter,alow,len_sub);
				iter+=len_sub;
			}
			else
			{
				if(a!=static_cast<std::uint_least64_t>(alow))
				{
					std::uint_least32_t v{static_cast<std::uint_least32_t>(a/divisor)};
					alow=static_cast<std::uint_least32_t>(a%divisor);
					if(v<10u)
					{
						jeaiii_c<0>(iter,v);
						++iter;
					}
					else
					{
						jeaiii_w(iter,v);
						iter+=2;
					}
					jeaiii_f<8>(iter,alow);
					iter+=full_length;
				}
				else
				{
					std::uint_least32_t len_sub{len-full_length};
					jeaiii_hash<9>(iter,alow,len_sub);
					iter+=len_sub;
				}
			}
			jeaiii_f<8>(iter,u);
		}
	}
	else
	{
		static_assert(!recursive);
		if constexpr(ryu_mode)
		{
			jeaiii_hash<7>(iter,n,len);
		}
		else
		{
			jeaiii_hash<9>(iter,n,len);
		}
	}
}

}
