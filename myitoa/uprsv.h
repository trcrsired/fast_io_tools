#pragma once

/*
fast_io's own uprsv (uint print reserve) algorithm
Referenced from xjkp's MyItoA algorithm:
https://gitee.com/xjkp2283572185/MyStd/blob/master/MyIntegral/MyItoA.hpp
*/

namespace fast_io::details
{

template<::std::integral char_type>
inline constexpr char_type* uprsv32_impl(char_type *iter,::std::uint_least32_t value) noexcept
{
	constexpr
		char_type *digitstb{digits_table<char_type,10,false>.data()};

	constexpr
		::std::uint_least32_t mask24{UINT32_C(1)<<UINT32_C(24)-UINT32_C(1)};

	constexpr
		::std::uint_least32_t magic24{(UINT32_C(1)<<UINT32_C(24))/UINT32_C(100)+UINT32_C(1)};

	constexpr
		::std::uint_least32_t onee8{UINT32_C(100000000)};

	constexpr
		::std::uint_least32_t magic32{UINT32_C(4295015706)};


	bool const lessthan1e8{value<onee8};

	::std::uint_least32_t const first8{lessthan1e8?value:value/onee8};

	if(first8 < UINT32_C(10000))
	{
		if(first8 < UINT32_C(100))
		{
			bool const lessthan10{first8 < UINT32_C(10)};
			::fast_io::details::non_overlapped_copy_n(digitstb+((first8+lessthan10)<<UINT32_C(1)),2u,iter);
			iter+=!lessthan10;
			++iter;
		}
		else
		{
			::std::uint_least32_t const f0{static_cast<::std::uint_least32_t>(first8*magic24)};
			::std::uint_least32_t const f2{static_cast<::std::uint_least32_t>(((f0&mask24)*UINT32_C(100))>>UINT32_C(24))};
			::std::uint_least32_t const f01{static_cast<::std::uint_least32_t>(f0>>UINT32_C(24))};
			bool const lessthan10{f01 < UINT32_C(10)};
			::fast_io::details::non_overlapped_copy_n(digitstb+((first8+lessthan10)<<1),2u,iter);
			iter+=!lessthan10;
			::fast_io::details::non_overlapped_copy_n(digitstb+(f2<<1),2u,iter);
			iter+=2;
		}
	}
	else
	{
		if(first8 < UINT32_C(1000000))
		{
			::std::uint_least32_t f0high;
			::std::uint_least32_t const f0low
			{
				::fast_io::details::intrinsics::umul_least_32(first8,magic32,f0high)
			};
			bool const lessthan10{f0high < UINT32_C(10)};
			::fast_io::details::non_overlapped_copy_n(digitstb+((first8+lessthan10)<<1),2u,iter);
			iter+=!lessthan10;
			::std::uint_least32_t f2high;
			::std::uint_least32_t const f2low
			{
				::fast::details::intrinsics::umul_least_32(f0low,UINT32_C(100),f2high)
			};
			::fast_io::details::non_overlapped_copy_n(digitstb+(f2high<<1),2u,iter);
			::std::uint_least32_t f4high;
			::fast::details::intrinsics::umul_least_32(f2low,UINT32_C(100),f4high);
			::fast_io::details::non_overlapped_copy_n(digitstb+(f4high<<1),2u,iter);
			iter+=4;
		}
		else
		{
			::std::uint_least32_t f0high;
			::std::uint_least32_t const f0low
			{
				::fast_io::details::intrinsics::umul_least_32(first8,magic32,f0high)
			};
			bool const lessthan10{f0high < UINT32_C(10)};
			::fast_io::details::non_overlapped_copy_n(digitstb+((first8+lessthan10)<<1),2u,iter);
			iter+=!lessthan10;
			::std::uint_least32_t f2high;
			::std::uint_least32_t const f2low
			{
				::fast::details::intrinsics::umul_least_32(f0low,UINT32_C(100),f2high)
			};
			::fast_io::details::non_overlapped_copy_n(digitstb+(f2high<<1),2u,iter);
			::std::uint_least32_t f4high;
			::fast::details::intrinsics::umul_least_32(f2low,UINT32_C(100),f4high);
			::fast_io::details::non_overlapped_copy_n(digitstb+(f4high<<1),2u,iter);
			iter+=6;
		}
	}
//	if()
}

#if 0
template<::std::integral char_type>
inline constexpr char_type* uprsv64_impl(char_type *iter,::std::uint_least32_t value) noexcept
{

}
#endif
}
