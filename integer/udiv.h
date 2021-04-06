#pragma once

namespace fast_io::details
{

template<std::unsigned_integral U>
struct udiv_result
{
	using unsigned_type = U;
	U quotient_low,quotient_high;
	U remainder_low,remainder_high;
};

/*
https://code.woboq.org/llvm/compiler-rt/lib/builtins/udivmodti4.c.html#__udivmodti4
*/
template<std::unsigned_integral U>
inline constexpr udiv_result<U> udivmodti4(U n_low, U n_high,U d_low,U d_high) noexcept
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
				if (rem)
					*rem = n_low & (d_low - 1);
				if (d_low == 1)
					return {n_low,n_high,};
#if 0
				sr = __builtin_ctzll(d_low);
#else

#endif
				q_high = n_high >> sr;
				q_low = (n_high << (n_udword_bits - sr)) | (n_low >> sr);
				return q.all;
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



		ti_int const s{(ti_int)(d.all - r.all - 1) >> n_udwords_bits_m1};
		carry = s & 1;
		r.all -= d.all & s;
	}
	q.all = (q.all << 1) | carry;
	return {q_low,q_high,r_low,r_high};
}

}