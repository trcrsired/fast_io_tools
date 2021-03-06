#pragma once

#ifdef __x86_64__
#include <wmmintrin.h>  // for intrinsics for AES-NI
#endif 

namespace fast_io::crypto
{

template<std::size_t N>
requires(N==16||N==32||N==48||N==64)
inline void fast_xor_assignment(std::span<std::byte,N> spa,std::span<std::byte,N> spb)
{
#ifdef __x86_64__
	_mm_storeu_si128(reinterpret_cast<__m128i*>(spa.data()),
		_mm_xor_si128(_mm_loadu_si128(reinterpret_cast<__m128i*>(spa.data())),
		_mm_loadu_si128(reinterpret_cast<__m128i*>(spb.data()))));
	if constexpr(16<N)
	{
		_mm_storeu_si128(reinterpret_cast<__m128i*>(spa.data()+16),
		_mm_xor_si128(_mm_loadu_si128(reinterpret_cast<__m128i*>(spa.data())),
		_mm_loadu_si128(reinterpret_cast<__m128i*>(spb.data()+16))));
	}
	if constexpr(32<N)
	{
		_mm_storeu_si128(reinterpret_cast<__m128i*>(spa.data()+16),
		_mm_xor_si128(_mm_loadu_si128(reinterpret_cast<__m128i*>(spa.data())),
		_mm_loadu_si128(reinterpret_cast<__m128i*>(spb.data()+16))));
	}
	if constexpr(48<N)
	{
		_mm_storeu_si128(reinterpret_cast<__m128i*>(spa.data()+16),
		_mm_xor_si128(_mm_loadu_si128(reinterpret_cast<__m128i*>(spa.data())),
		_mm_loadu_si128(reinterpret_cast<__m128i*>(spb.data()+16))));
	}

#else
	for(std::size_t i{};i!=N;++i)
		spa[i]^=spb[i];
#endif
}


}
