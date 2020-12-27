#include<cstdint>
#include<stdexcept>
#include<cstring>
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__x86_64__) || defined(__i386__)
#include<immintrin.h>
#endif

namespace intrinsics
{
template<typename T>
#if __cpp_lib_concepts >= 202002L
requires (std::unsigned_integral<T>)
#endif
inline constexpr bool add_carry(bool carry,T a,T b,T& out) noexcept
{
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#if __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
		return (out=a+b+carry)<=a;
	else
#endif
	{
		if constexpr(sizeof(T)==8)
#if defined(__x86_64__)
			return _addcarry_u64(carry,a,b,
#if !defined(__INTEL_COMPILER ) &&(defined(__GNUC__) || defined(__clang__))
			reinterpret_cast<unsigned long long*>(&out));
#else
			&out);
#endif
#else
			return (out=a+b+carry)<=a;
#endif

		else if constexpr(sizeof(T)==4)
			return _addcarry_u32(carry,a,b,reinterpret_cast<std::uint32_t*>(&out));
		else if constexpr(sizeof(T)==2)
			return _addcarry_u16(carry,a,b,reinterpret_cast<std::uint16_t*>(&out));
		else if constexpr(sizeof(T)==1)
			return _addcarry_u8(carry,a,b,reinterpret_cast<std::uint8_t*>(&out));
	}
#else
	return (out=a+b+carry)<=a;
#endif
}

template<typename T>
#if __cpp_lib_concepts >= 202002L
requires (std::unsigned_integral<T>)
#endif
inline constexpr bool sub_borrow(bool borrow,T a,T b,T& out) noexcept
{
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#if __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
		return (out=a-b-borrow)>=a;
	else
#endif
	{
		if constexpr(sizeof(T)==8)
#if defined(__x86_64__)
			return _subborrow_u64(borrow,a,b,
#if !defined(__INTEL_COMPILER ) &&(defined(__GNUC__) || defined(__clang__))
			reinterpret_cast<unsigned long long*>(&out));
#else
			&out);
#endif
#else
			return (out=a-b-borrow)>=a;
#endif

		if constexpr(sizeof(T)==4)
			return _subborrow_u32(borrow,a,b,reinterpret_cast<std::uint32_t*>(&out));
		else if constexpr(sizeof(T)==2)
			return _subborrow_u16(borrow,a,b,reinterpret_cast<std::uint16_t*>(&out));
		else if constexpr(sizeof(T)==1)
			return _subborrow_u8(borrow,a,b,reinterpret_cast<std::uint8_t*>(&out));
	}
#else
	return (out=a-b-borrow)>=a;
#endif
}

inline constexpr std::uint64_t umul(std::uint64_t a,std::uint64_t b,std::uint64_t& high) noexcept
{
#ifdef _MSC_VER
	return _umul128(a,b,std::addressof(high));
#else
#if __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
	{
		__uint128_t res{static_cast<__uint128_t>(a)*b};
		high=static_cast<std::uint64_t>(res>>64);
		return static_cast<std::uint64_t>(res);
	}
	else
	{
#endif
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ || __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		struct u64x2_t
		{
		#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			std::uint64_t high,low;
		#else
			std::uint64_t low,high;
		#endif
		};
		static_assert(sizeof(__uint128_t)==sizeof(u64x2_t));
		__uint128_t res{static_cast<__uint128_t>(a)*b};
		u64x2_t u;
		memcpy(&u,&res,sizeof(__uint128_t));
		high=u.high;
		return u.low;
#else
		__uint128_t res{static_cast<__uint128_t>(a)*b};
		high=static_cast<std::uint64_t>(res>>64);
		return static_cast<std::uint64_t>(res);
#endif
#if __cpp_lib_is_constant_evaluated >= 201811L
	}
#endif
#endif
}

}

template<std::integral char_type>
inline constexpr bool exec_charset_is_ebcdic() noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<char_type>,char>)
		return static_cast<char8_t>('A')!=u8'A';
#ifndef __MSDOS__
	else if constexpr(std::same_as<std::remove_cvref_t<char_type>,wchar_t>)
		return static_cast<char32_t>(L'A')!=U'A';
#endif
	else
		return false;
}

template<char8_t base,typename char_type>
inline constexpr std::uint64_t from_chars_u64(char_type* b,char_type* e,std::uint64_t& res)
{
	std::uint64_t overflow{b==e};
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	constexpr unsigned_char_type ten(base);
	for(;b!=e;++b)
	{
		unsigned_char_type result(*b);
		if constexpr(exec_charset_is_ebcdic<char_type>())
			result-=static_cast<unsigned_char_type>(240);
		else
			result-=static_cast<unsigned_char_type>(u8'0');
		if(ten<=result)
			break;
		std::uint64_t umulres;
		res=intrinsics::umul(res,ten,umulres);
		overflow|=umulres;
		overflow|=intrinsics::add_carry(0,res,static_cast<std::uint64_t>(result),res);
	}
	return overflow;
}

inline constexpr std::uint64_t from_chars_u64over10(char8_t* b,char8_t* e,std::uint64_t& res)
{
	return from_chars_u64<10>(b,e,res);
}
/*
https://godbolt.org/z/T1KGT5
*/