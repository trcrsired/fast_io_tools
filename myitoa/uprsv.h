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
		auto const *digitstb{digits_table<char_type,10,false>.data()};

	constexpr
		::std::uint_least32_t mask24{(UINT32_C(1)<<UINT32_C(24))-UINT32_C(1)};

	constexpr
		::std::uint_least32_t magic24{(UINT32_C(1)<<UINT32_C(24))/UINT32_C(100)+UINT32_C(1)};

	constexpr
		::std::uint_least32_t onee8{UINT32_C(100000000)};

	constexpr
		::std::uint_least32_t magic32{UINT32_C(429497)};

	constexpr
		::std::uint_least64_t magic48{UINT32_C(281474977)};

	constexpr
		::std::size_t tocopybytes{sizeof(char_type)*2u};

	bool const lessthan1e8{value<onee8};

	::std::uint_least32_t const first8{lessthan1e8?value:value/onee8};

	if(first8 < UINT32_C(10000))
	{
		if(first8 < UINT32_C(100))
		{
			bool const lessthan10{first8 < UINT32_C(10)};
			::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan10+(first8<<1),tocopybytes);
			iter+=1+(!lessthan10);
		}
		else
		{
			::std::uint_least32_t const f0{static_cast<::std::uint_least32_t>(first8*magic24)};
			::std::uint_least32_t const f2{static_cast<::std::uint_least32_t>(((f0&mask24)*UINT32_C(100))>>UINT32_C(24))};
			::std::uint_least32_t const f01{static_cast<::std::uint_least32_t>(f0>>UINT32_C(24))};
#if __has_cpp_attribute(assume)
			[[assume(f01<UINT32_C(100))]];
#endif
			bool const lessthan10{f01 < UINT32_C(10)};
			::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan10+(f01<<1),tocopybytes);
			iter+=1+(!lessthan10);
			::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(f2<<1),tocopybytes);
			iter+=2;
		}
	}
	else
	{
		::std::uint_least32_t low,high;
		if(first8 < UINT32_C(1000000))
		{
			low = ::fast_io::details::intrinsics::umul_least_32(first8,magic32,high);
#if __has_cpp_attribute(assume)
			[[assume(high<UINT32_C(100))]];
#endif
			bool const lessthan10{high < UINT32_C(10)};
			::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan10+(high<<1),tocopybytes);
			iter+=1+(!lessthan10);
		}
		else
		{
			::std::uint_least64_t const temp{(magic48*first8)>>16};
			low = ::fast_io::details::intrinsics::unpack_ul64(temp,high);
#if __has_cpp_attribute(assume)
			[[assume(high<UINT32_C(100))]];
#endif
			bool const lessthan10{high < UINT32_C(10)};
			::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan10+(high<<1),tocopybytes);
			iter+=1+(!lessthan10);
			::std::uint_least32_t high;
			low = ::fast_io::details::intrinsics::umul_least_32(low,UINT32_C(100),high);
#if __has_cpp_attribute(assume)
			[[assume(high<UINT32_C(100))]];
#endif
			::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(high<<1),tocopybytes);
			iter+=2;
		}

		low = ::fast_io::details::intrinsics::umul_least_32(low,UINT32_C(100),high);
#if __has_cpp_attribute(assume)
		[[assume(high<UINT32_C(100))]];
#endif
		::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(high<<1),tocopybytes);

		high = ::fast_io::details::intrinsics::umulh_least_32(low,UINT32_C(100));
#if __has_cpp_attribute(assume)
		[[assume(high<UINT32_C(100))]];
#endif
		::fast_io::details::intrinsics::typed_memcpy(iter+2,digitstb+(high<<1),tocopybytes);

		iter+=4;
	}
	if(!lessthan1e8)
	{
#if __has_cpp_attribute(assume)
		[[assume(first8<UINT32_C(4295))]];
#endif
		::std::uint_least32_t const f0{value-first8*onee8};
		::std::uint_least64_t temp{(magic48*f0)>>16};
		::std::uint_least32_t high;
		::std::uint_least32_t low{::fast_io::details::intrinsics::unpack_ul64(temp,high)};
#if __has_cpp_attribute(assume)
		[[assume(high<UINT32_C(100))]];
#endif
		::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(high<<1),tocopybytes);

		low = ::fast_io::details::intrinsics::umul_least_32(low,UINT32_C(100),high);
#if __has_cpp_attribute(assume)
		[[assume(high<UINT32_C(100))]];
#endif
		::fast_io::details::intrinsics::typed_memcpy(iter+2,digitstb+(high<<1),tocopybytes);

		low = ::fast_io::details::intrinsics::umul_least_32(low,UINT32_C(100),high);
#if __has_cpp_attribute(assume)
		[[assume(high<UINT32_C(100))]];
#endif
		::fast_io::details::intrinsics::typed_memcpy(iter+4,digitstb+(high<<1),tocopybytes);

		high = ::fast_io::details::intrinsics::umulh_least_32(low,UINT32_C(100));
#if __has_cpp_attribute(assume)
		[[assume(high<UINT32_C(100))]];
#endif
		::fast_io::details::intrinsics::typed_memcpy(iter+6,digitstb+(high<<1),tocopybytes);

		iter+=8;
	}
	return iter;
}

#if 0
template<::std::integral char_type>
inline constexpr char_type* uprsv64_impl(char_type *iter,::std::uint_least32_t value) noexcept
{

}
#endif
}
