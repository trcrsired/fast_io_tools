#include<fast_io.h>
#if 1
#include<immintrin.h>
#endif
/*
inline std::uint64_t parse_8_chars(char8_t const* string) noexcept
{
  std::uint64_t chunk = 0;
  __builtin_memcpy(__builtin_addressof(chunk), string, sizeof(chunk));
debug_println(std::source_location::current()," ",chunk);

  // 1-byte mask trick (works on 4 pairs of single digits)
  std::uint64_t lower_digits = (chunk & 0x0f000f000f000f00) >> 8;
  std::uint64_t upper_digits = (chunk & 0x000f000f000f000f) * 10;
  chunk = lower_digits + upper_digits;
debug_println(std::source_location::current()," ",lower_digits," ",upper_digits," ",chunk);
  // 2-byte mask trick (works on 2 pairs of two digits)
  lower_digits = (chunk & 0x00ff000000ff0000) >> 16;
  upper_digits = (chunk & 0x000000ff000000ff) * 100;
  chunk = lower_digits + upper_digits;
debug_println(std::source_location::current()," ",lower_digits," ",upper_digits," ",chunk);

  // 4-byte mask trick (works on pair of four digits)
  lower_digits = (chunk & 0x0000ffff00000000) >> 32;
  upper_digits = (chunk & 0x000000000000ffff) * 10000;
  chunk = lower_digits + upper_digits;
debug_println(std::source_location::current()," ",lower_digits," ",upper_digits," ",chunk);

  return chunk;
}
*/

#if 0

inline char8_t detect_length(char8_t const* buffer) noexcept
{
	using namespace fast_io::intrinsics;
	x86_64_v16qu input;
	__builtin_memcpy(__builtin_addressof(input),buffer,16);
	x86_64_v16qu const v176{176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176};
	x86_64_v16qu const t0{input-v176};
	x86_64_v16qs const minus118{-118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118};
	x86_64_v16qs const mask{(x86_64_v16qs)t0<minus118};
	debug_println(std::source_location::current()," mask=",(x86_64_m128i)mask);
	std::uint16_t v{static_cast<std::uint16_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)mask))};
	debug_println(std::source_location::current()," v=",fast_io::mnp::ubin_full(v));
	x86_64_v8hi __builtin_ia32_pblendw128(,v);
	return std::countr_one(v);
}


inline char8_t sse_parse(char8_t const* buffer) noexcept
{
	using namespace fast_io::intrinsics;
	x86_64_v16qu input;
	__builtin_memcpy(__builtin_addressof(input),buffer,16);
	x86_64_v16qu const v176{176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176};
	x86_64_v16qu const t0{input-v176};
	x86_64_v16qs const minus118{-118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118};
	x86_64_v16qs const mask{(x86_64_v16qs)t0<minus118};
	debug_println(std::source_location::current()," mask=",(x86_64_m128i)mask);
	std::uint16_t v{static_cast<std::uint16_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)mask))};
	debug_println(std::source_location::current()," v=",fast_io::mnp::ubin_full(v));

	return std::countr_one(v);
}

#else

inline char8_t detect_length(char8_t const* buffer) noexcept
{
	__m128i input = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer));
	__m128i const t0 = _mm_sub_epi8(input, _mm_set1_epi8(176));
	__m128i const mask = _mm_cmplt_epi8(t0, _mm_set1_epi8(-118));
	std::uint16_t v{static_cast<std::uint16_t>(_mm_movemask_epi8(mask))};
	return std::countr_one(v);
}

inline ::fast_io::parse_result<char8_t const*> sse_parse(char8_t const* buffer,char8_t const* buffer_end,std::uint64_t &res) noexcept
{
	__m128i chunk = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer));
	__m128i const t0 = _mm_sub_epi8(chunk, _mm_set1_epi8(176));
	__m128i const mask = _mm_cmplt_epi8(t0, _mm_set1_epi8(-118));
	std::uint16_t v{static_cast<std::uint16_t>(_mm_movemask_epi8(mask))};
	std::uint32_t digits{static_cast<std::uint32_t>(std::countr_one(v))};
	if(digits==0)
	{
		return {buffer,parse_code::invalid};
	}
	std::uint32_t rshift_digits{16-digits};
	constexpr char8_t zero_constant{u8'0'};
	chunk = _mm_sub_epi8(chunk, _mm_set1_epi8(zero_constant));
	chunk = _mm_slli_si128(chunk,rshift_digits);
	chunk = _mm_maddubs_epi16(chunk, _mm_set_epi8(1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10));
	chunk = _mm_madd_epi16(chunk, _mm_set_epi16(1,100,1,100,1,100,1,100));
	chunk = _mm_packus_epi32(chunk, chunk);
	chunk = _mm_madd_epi16(chunk,_mm_set_epi16(0,0,0,0,1,10000,1,10000));
	std::uint64_t result{((chunk[0] & 0xffffffff) * 100000000) + (chunk[0] >> 32)};

	if(digits==16)[[unlikely]]
	{
		__m128i chunk1 = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer+16));
		__m128i const t0_1 = _mm_sub_epi8(chunk1, _mm_set1_epi8(176));
		__m128i const mask_1 = _mm_cmplt_epi8(t0_1, _mm_set1_epi8(-118));
		std::uint16_t v1{static_cast<std::uint16_t>(_mm_movemask_epi8(mask1))};
		std::uint32_t digits1{static_cast<std::uint32_t>(std::countr_one(v1))};
//18446744073709551615 20 digits
		switch(digits1)
		{
		case 3:
		{
			res=result*1000ULL+static_cast<std::uint64_t>(buffer[16]-zero)*100ULL+static_cast<std::uint64_t>(buffer[17]-zero)*10ULL
			+static_cast<std::uint64_t>(buffer[18]-zero);
			return {buffer+19,parse_code::ok};
		}
		case 2:
		{
			res=result*100+static_cast<std::uint64_t>(buffer[16]-zero)*10ULL+static_cast<std::uint64_t>(buffer[17]-zero);
			return {buffer+18,parse_code::ok};
		}
		case 1:
		{
			res=result*10+static_cast<std::uint64_t>(buffer[16]-zero);
			return {buffer+17,parse_code::ok};
		}
		case 0:
		{
			res=result;
			return {buffer+16,parse_code::ok};
		}
		case 4:
		{
			constexpr std::uint64_t risky_value{UINT64_MAX/10000ULL};
			constexpr std::uint64_t risky_mod{UINT64_MAX%10000ULL};
			if(result>risky_value)
				return {buffer+20,parse_code::overflow};
			std::uint64_t partial{static_cast<std::uint64_t>(buffer[16]-zero)*1000ULL+static_cast<std::uint64_t>(buffer[17]-zero)*100ULL
			+static_cast<std::uint64_t>(buffer[18]-zero)*10ULL+static_cast<std::uint64_t>(buffer[19]-zero)};
			
			if(result==risky_value&&risky_mod<partial)
				return {buffer+20,parse_code::overflow};
			res=result*10000ULL+partial;
			return {buffer+20,parse_code::okay};
		}
		case 16:
		{
			auto it{buffer+32};
			for(it;16<=buffer_end-it;)
			{
				__m128i chunk2 = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer+16));
				__m128i const t0_2 = _mm_sub_epi8(chunk1, _mm_set1_epi8(176));
				__m128i const mask_2 = _mm_cmplt_epi8(t0_1, _mm_set1_epi8(-118));
				std::uint16_t v2{static_cast<std::uint16_t>(_mm_movemask_epi8(mask2))};
				std::uint32_t digits2{static_cast<std::uint32_t>(std::countr_one(v2))};
			};
		}
		default:
		{
			return {buffer+digits+16,parse_code::overflow};
		}
		}
	}
	res=result;
	return {buffer+digits,parse_code::ok};
}

#endif

int main()
{
	char8_t buffer[]=u8"99999999999 1                                                                                          ";
//	std::uint64_t digit(parse_8_chars(buffer));
	println(sse_parse(buffer));
	return 0;
}