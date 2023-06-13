#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::std::uint_least32_t utf8masks[3]{
		0b00111111'00011111u,0b00111111'00111111'00001111u,0b00111111'00111111'00111111'00000111u};

inline constexpr ::std::uint_least32_t utf8masks2[3]{
		0b11111111'11111111'10111111'11011111u,
		0b11111111'10111111'10111111'11101111u,
		0b10111111'10111111'10111111'11110111u};

inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_simd_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst,char32_t *tolast) noexcept
{
	constexpr
		::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> cmp128{
		0x128,0x128,0x128,0x128,0x128,0x128,0x128,0x128,
		0x128,0x128,0x128,0x128,0x128,0x128,0x128,0x128};
{
	constexpr
		::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> zeros{};
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> simvec;
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t,16> ret;
	for(;20<static_cast<::std::size_t>(fromlast-fromfirst)&&
		16<static_cast<::std::size_t>(tolast-tofirst);)
	{
		simvec.load(fromfirst);
		ret.value=__builtin_shufflevector(simvec.value,zeros,0,16,16,16,1,16,16,16,
			2,16,16,16,3,16,16,16);
		ret.store(tofirst);
		ret.value=__builtin_shufflevector(simvec.value,zeros,4,16,16,16,5,16,16,16,
			6,16,16,16,7,16,16,16);
		ret.store(tofirst+4);
		ret.value=__builtin_shufflevector(simvec.value,zeros,8,16,16,16,9,16,16,16,
			10,16,16,16,11,16,16,16);
		ret.store(tofirst+8);
		ret.value=__builtin_shufflevector(simvec.value,zeros,12,16,16,16,13,16,16,16,
			14,16,16,16,15,16,16,16);
		ret.store(tofirst+12);
		auto res{(simvec&cmp128)!=cmp128};
		if(is_all_zeros(res))
		{
			fromfirst+=16;
			tofirst+=16;
			continue;
		}
		unsigned czv{vector_mask_countr_zero(res)};
		fromfirst+=czv;
		tofirst+=czv;
		::std::uint_least8_t v{static_cast<::std::uint_least8_t>(*fromfirst)};
		int length{::std::countl_one(v)-1};
		if(!length||4<length)
		{
			*tofirst=0xFFFD;
			++fromfirst;
			++tofirst;
			continue
		}
		::std::uint_least32_t val;
		__builtin_memcmpy(__builtin_addressof(val),fromfirst,4);
		::std::uint_least32_t mask2{utf8masks2[length]};
		if((val|mask2)!=mask2)
		{
			*tofirst=0xFFFD;
			fromfirst+=length;
			++tofirst;
			continue;
		}
		::std::uint_least32_t mask1{utf8masks[length]};
//		val=(val&mask1);
//		val=;
		}

	}

inline constexpr deco_result<char8_t,char32_t> utf8_to_utf32_impl(
	char8_t const *fromfirst,char8_t const *fromlast,
	char32_t *tofirst,char32_t *tolast) noexcept
{
//	::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
	
	return ::fast_io::details::utf8_to_utf32_simd_impl(fromfirst,fromlast,tofirst,tolast);
}

}

}

}
