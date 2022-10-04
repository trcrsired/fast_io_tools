#pragma once

inline constexpr ::fast_io::intrinsics::simd_vector<char unsigned,16> leb128comparemask{
		0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
		0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F};

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline ::fast_io::parse_result<char unsigned const*> scn_cnt_define_find_overflow_simd16_impl(char unsigned const* begin,char unsigned const* end) noexcept
{
	using namespace fast_io::intrinsics;
	using signed_simd_vector =
	::fast_io::intrinsics::simd_vector<char,16>;
	using unsigned_simd_vector =
	::fast_io::intrinsics::simd_vector<char unsigned,16>;
	unsigned_simd_vector chunk;
	constexpr char unsigned constant{128};
	for(;end-begin<=16;begin+=16)
	{
		chunk.load(begin);

		unsigned_simd_vector cmpres{chunk<=leb128comparemask};
		::std::uint_least32_t mask{static_cast<::std::uint_least32_t>(__builtin_ia32_pmovmskb128(
			static_cast<signed_simd_vector>(cmpres)))};
		::std::uint_least32_t const incr{static_cast<::std::uint_least32_t>(std::countr_zero(static_cast<std::uint_least16_t>(mask)))};
		if(incr!=16)
		{
			return {begin+incr,parse_code::overflow};
		}
	}
	for(;begin<end;++begin)
	{
		if(constant<=*begin)
		{
			return {begin,parse_code::overflow};
		}
	}
	return {end,parse_code::invalid};
}

template <::fast_io::details::my_integral I>
inline ::fast_io::parse_result<char unsigned const*>
	scn_cnt_define_leb128_simd16_impl(char unsigned const* begin,char unsigned const* end, I& t) noexcept
{

	using namespace fast_io::intrinsics;
	using signed_simd_vector =
	::fast_io::intrinsics::simd_vector<char,16>;
	using unsigned_simd_vector =
	::fast_io::intrinsics::simd_vector<char unsigned,16>;
	unsigned_simd_vector chunk;
	chunk.load(begin);
	unsigned_simd_vector cmpres{chunk<=leb128comparemask};
	::std::uint_least32_t mask{static_cast<::std::uint_least32_t>(__builtin_ia32_pmovmskb128(
		static_cast<signed_simd_vector>(cmpres)))};
	::std::uint_least32_t const incr{static_cast<::std::uint_least32_t>(std::countr_zero(static_cast<std::uint_least16_t>(mask)))};
	constexpr std::uint_least32_t u32lenval{static_cast<std::uint_least32_t>(::fast_io::details::leb128_length_val<I>)};
	if(incr>=u32lenval)
	{
		if(incr==16)[[unlikely]]
		{
			return scn_cnt_define_find_overflow_simd16_impl(begin+16,end);
		}
		return {begin+incr,parse_code::overflow};
	}
	::fast_io::intrinsics::simd_vector<char,16> shuffle_mask;
	shuffle_mask.load(::fast_io::details::sse_shift_table+incr);
	chunk=static_cast<unsigned_simd_vector>(signed_simd_vector{__builtin_ia32_pshufb128(static_cast<signed_simd_vector>(chunk),shuffle_mask)});
	chunk&=leb128comparemask;
	constexpr unsigned_simd_vector combine8_to_16_mask1{64,1,64,1,64,1,64,1,64,1,64,1,64,1,64,1};
	::fast_io::intrinsics::simd_vector<short,8> chunk2{__builtin_ia32_pmaddubsw128(static_cast<signed_simd_vector>(chunk),
		static_cast<signed_simd_vector>(combine8_to_16_mask1))};
	constexpr unsigned_simd_vector combine8_to_16_mask2{64,0,64,0,64,0,64,0,64,0,64,0,64,0,64,0};
	::fast_io::intrinsics::simd_vector<short,8> chunk3{__builtin_ia32_pmaddubsw128(static_cast<signed_simd_vector>(chunk),
		static_cast<signed_simd_vector>(combine8_to_16_mask2))};

	chunk3+=chunk2;
	constexpr
		::fast_io::intrinsics::simd_vector<short,8> combine16_to_32_mask{0x4000,1,0x4000,1,0x4000,1,0x4000,1};
	::fast_io::intrinsics::simd_vector<int,4> chunk4{__builtin_ia32_pmaddwd128(chunk3,combine16_to_32_mask)};
	std::uint_least32_t const high{static_cast<unsigned>(chunk4[1])},
		middle{static_cast<unsigned>(chunk4[2])},
		low{static_cast<unsigned>(chunk4[3])};


	char unsigned final_char{static_cast<char unsigned>(begin[incr])};
	if(incr==11&&1<final_char)
	{
		return {begin+incr+1,parse_code::invalid};
	}
	t=(final_char<<(incr*7))+(static_cast<std::uint_least64_t>(high)<<56)+(static_cast<std::uint_least64_t>(middle)<<28)+low;
	return {begin+incr+1,parse_code::ok};
}