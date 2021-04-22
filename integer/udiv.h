#pragma once

namespace fast_io::details::intrinsics
{

template<std::unsigned_integral U>
struct udiv_result
{
	using unsigned_type = U;
	U quotient_low,quotient_high;
	U remainder_low,remainder_high;
};

template<typename U>
inline constexpr U shiftright_naive(U low_part,U high_part,std::uint8_t shift) noexcept
{
	U const value{(static_cast<U>(1)<<shift)-1};
	constexpr std::uint8_t bits{sizeof(U)*8};
	U const v{high_part&value};
	if(shift==0)
		return low_part;
	return (low_part>>shift)|(v<<(bits-shift));
}

template<typename U>
inline constexpr U shiftright_naive(U low_part,U high_part,std::uint8_t shift) noexcept
{
	U const value{(static_cast<U>(1)<<shift)-1};
	constexpr std::uint8_t bits{sizeof(U)*8};
	U const v{high_part&value};
	if(shift==0)
		return low_part;
	return (low_part>>shift)|(v<<(bits-shift));
}


template<typename U>
inline constexpr U shiftright(U low_part,U high_part,std::uint8_t shift) noexcept
{
//	1<<shift
//	__shiftright128
#if __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
		return shiftright_naive(low_part,high_part,shift);
	else
#endif
	{
#if defined(_MSC_VER) && (defined(__x86_64__) || defined(__i386__))
		if constexpr(sizeof(U)==8&&sizeof(std::size_t)>=8)
		{
			return __shiftright128(low_part,high_part,shift);
		}
		else
#endif
		{
			return shiftright_naive(low_part,high_part,shift);
		}
	}
}

/*
https://code.woboq.org/llvm/compiler-rt/lib/builtins/udivmodti4.c.html#__udivmodti4
*/
template<std::unsigned_integral U>
inline constexpr udiv_result<U> udivmod(U n_low, U n_high,U d_low,U d_high) noexcept
{
	// special cases, X is unknown, K != 0
	if (a_high == 0)
	{
		if (b_high == 0)
		{
			return {n_low/d_low,0,n_low%d_low,0};
		}
		return {0,0,n_low,0};
	}
	constexpr std::uint32_t n_udwords_bits{sizeof(U) * CHAR_BIT};
	constexpr std::uint32_t n_utwords_bits{n_udwords_bits<<1};
	constexpr std::uint32_t n_udwords_bits_m1{n_udwords_bits-1};
	constexpr std::uint32_t n_udwords_bits_m2{n_udwords_bits-2};
	// n_high != 0
	U q_low,q_high,r_low,r_high;
	std::uint32_t sr;
	if (d_low == 0)
	{
		if (d_high == 0)[[unlikely]]
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_trap)
			__builtin_trap();
#elif __has_builtin(__builtin_abort)
			__builtin_abort();
#else
			std::abort();
#endif
#elif defined(_MSC_VER)
			__debug_break();
#else
			std::abort();
#endif
		}
		// d_high != 0
		if (n_low == 0)
		{
			// K 0
			// ---
			// K 0
			return {n_high / d_high,0,0,n_high % d_high};
		}
		// K K
		// ---
		// K 0
		if ((d_high & (d_high - 1)) == 0)
		{
#if 0
			return {n_high >> __builtin_ctzll(d_high),0,n_low,n_high & (d_high - 1)};
#else
			return {n_high >> static_cast<std::uint32_t>(std::countl_zero(d_high)},0,n_low,n_high & (d_high - 1)};
#endif

		}
		// K K
		// ---
		// K 0
#if 0
		sr=__builtin_clzll(d_high) - __builtin_clzll(n_high);
#else
		sr=static_cast<std::uint32_t>(std::countl_zero(d_high) - std::countl_zero(n_high));
#endif

		// 0 <= sr <= n_udword_bits - 2 or sr large
		if (sr > n_udwords_bits_m2)
			return {0,0,n_low,n_high};
		++sr;
		// 1 <= sr <= n_udword_bits - 1
		// q.all = n.all << (n_utword_bits - sr);
		q_low=0;
		q_high=n_low << (n_udword_bits - sr);
		// r.all = n.all >> sr;
		r_high=n_high >> sr;
		r_low=(n_high << (n_udword_bits - sr)) | (n_low >> sr);
	}
	else /* d_low != 0 */
	{
		if (d_high == 0)
		{
			// K X
			// ---
			// 0 K
			if ((d_low & (d_low - 1)) == 0) /* if d is a power of 2 */
			{
				U rem{n_low & (d_low - 1)};
				if (d_low == 1)
					return {n_low,n_high,rem,0};
#if 0
				sr = __builtin_ctzll(d_low);
#else
				sr = std::countl_zero(d_low);
#endif
				q_high = n_high >> sr;
				q_low = (n_high << (n_udword_bits - sr)) | (n_low >> sr);
				return {q_low,q_high,rem,0};
			}
			// K X
			// ---
			// 0 K
#if 0
			sr = 1 + n_udword_bits + __builtin_clzll(d_low) - __builtin_clzll(n_high);
#else
			sr = 1 + n_udword_bits + static_cast<std::uint32_t>(std::countl_zero(d_low)) - static_cast<std::uint32_t>(std::countl_zero(n_high));
#endif
			// 2 <= sr <= n_utword_bits - 1
			// q.all = n.all << (n_utword_bits - sr);
			// r.all = n.all >> sr;
			if (sr == n_udword_bits)
			{
				q_low = 0;
				q_high = n_low;
				r_high = 0;
				r_low = n_high;
			}
			else if (sr < n_udword_bits) /* 2 <= sr <= n_udword_bits - 1 */
			{
				q_low = 0;
				q_high = n_low << (n_udword_bits - sr);
				r_high = n_high >> sr;
				r_low = (n_high << (n_udword_bits - sr)) | (n_low >> sr);
			}
			else /* n_udword_bits + 1 <= sr <= n_utword_bits - 1 */
			{
				q_low = n_low << (n_utword_bits - sr);
				q_high = (n_high << (n_utword_bits - sr)) |
						(n_low >> (sr - n_udword_bits));
				r_high = 0;
				r_low = n_high >> (sr - n_udword_bits);
			}
		}
		else
		{
			// K X
			// ---
			// K K
#if 0
			sr = __builtin_clzll(d_high) - __builtin_clzll(n_high);
#else
			sr = static_cast<std::uint32_t>(std::countl_zero(d_high)) - static_cast<std::uint32_t>(std::countl_zero(n_high));
#endif
			// 0 <= sr <= n_udword_bits - 1 or sr large
			if (sr > n_udwords_bits_m1)
				return {0,0,n_low,n_high};
			++sr;
			// 1 <= sr <= n_udword_bits
			// q.all = n.all << (n_utword_bits - sr);
			// r.all = n.all >> sr;
			q_low = 0;
			if (sr == n_udword_bits)
			{
				q_high = n_low;
				r_high = 0;
				r_low = n_high;
			}
			else
			{
				r_high = n_high >> sr;
				r_low = (n_high << (n_udword_bits - sr)) | (n_low >> sr);
				q_high = n_low << (n_udword_bits - sr);
			}
		}
	}
	// Not a special case
	// q and r are initialized with:
	// q.all = n.all << (n_utword_bits - sr);
	// r.all = n.all >> sr;
	// 1 <= sr <= n_utword_bits - 1

	bool carry{};
	for (;sr;--sr)
	{
		// r:q = ((r:q)	<< 1) | carry
		r_high = (r_high << 1) | (r_low >> n_udwords_bits_m1);
		r_low = (r_low << 1) | (q_high >> n_udwords_bits_m1);
		q_high = (q_high << 1) | (q_low >> n_udwords_bits_m1);
		q_low = (q_low << 1) | carry;


		U s_low,s_high;
		sub_borrow(sub_borrow(true,d_low,r_low,s_low),d_high,r_high,s_high);
		s_low=shiftright(s_low,s_high,n_udwords_bits_m1);
		s_high>>=n_udwords_bits_m1;
//		ti_int const s{(ti_int)(d.all - r.all - 1) >> n_udwords_bits_m1};
		carry = s_low & 1;
		s_low&=d_low;
		s_high&=d_high;
		sub_borrow(sub_borrow(false,r_low,s_low,r_low),r_high,s_high,r_high);
//		r.all -= d.all & s;
	}
	q_high = (q_high << 1) | (q_low >> n_udwords_bits_m1);
	q_low = (q_low << 1) | carry;
	return {q_low,q_high,r_low,r_high};
}

}