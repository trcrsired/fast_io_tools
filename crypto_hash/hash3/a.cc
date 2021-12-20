#include<fast_io_core.h>
#include<fast_io_crypto.h>
#include"md5_sha_hash_context.h"


template<std::endian end,std::unsigned_integral U>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void sha256_512_digest_to_ptr(U const* digest,std::size_t n,std::byte* ptr) noexcept
{
	constexpr std::size_t usz{sizeof(U)};
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		for(std::size_t i{};i!=n;++i)
		{
			U v{digest[i]};
			if constexpr(::std::endian::native==end)
			{
				v=::fast_io::byte_swap(v);
			}
			::fast_io::freestanding::array<::std::byte,usz> va{::fast_io::bit_cast<::fast_io::freestanding::array<::std::byte,usz>>(digest)};
			for(std::size_t j{};j!=usz;++j)
			{
				*ptr=va[j];
				++ptr;
			}
		}
	}
	else
#endif
	{
		if constexpr(::std::endian::native==end)
		{
			::fast_io::details::my_memcpy(ptr,digest,n);
		}
		else
		{
#if (!defined(_MSC_VER) || defined(__clang__)) && (defined(__SSE4_2__) || defined(__wasm_simd128__))
			constexpr std::size_t factor{16u/usz};
			static_assert(16u%usz==0&&usz!=16u);
			::fast_io::intrinsics::simd_vector<U,factor> s;
			std::size_t nd2{n/factor};
			for(std::size_t i{};i!=nd2;++i)
			{
				s.load(digest);
				s.swap_endian();
				s.store(ptr);
				ptr+=usz;
			}
#else
			for(std::size_t i{};i!=n;++i)
			{
				auto v{::fast_io::byte_swap(digest[i])};
				::fast_io::details::my_memcpy(ptr,__builtin_addressof(v),usz);
				ptr+=usz;
			}
#endif
		}
	}
}

inline constexpr void sha512_digest_to_ptr(std::uint_least64_t const* digest,std::byte* ptr) noexcept
{
	constexpr std::size_t sz{8};
	sha256_512_digest_to_ptr<std::endian::big>(digest,sz,ptr);
}

struct sha512_initializer
{
	static inline constexpr std::size_t digest_size{64};
	static inline constexpr ::fast_io::sha512 initialize_value{0x6a09e667f3bcc908, 0xbb67ae8584caa73b,0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,0x510e527fade682d1, 0x9b05688c2b3e6c1f,0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
	static inline constexpr void digest_to_ptr(std::uint_least64_t const* digest,std::byte* ptr) noexcept
	{
		sha512_digest_to_ptr(digest,ptr);
	}
};

struct sha512_256_initializer
{
	static inline constexpr std::size_t digest_size{32};
	static inline constexpr ::fast_io::sha512 initialize_value{0x22312194fc2bf72c,0x9f555fa3c84c64c2,0x2393b86b6f53b151,0x963877195940eabd,0x96283ee2a88effe3,0xbe5e1e2553863992,0x2b0199fc2c85b8aa,0x0eb72ddc81c52ca2};
	static inline constexpr void digest_to_ptr(std::uint_least64_t const* digest,std::byte* ptr) noexcept
	{
		sha512_digest_to_ptr(digest,ptr);
	}
};

using sha512_context = ::fast_io::details::basic_md5_sha_context_impl<::fast_io::sha512,sha512_initializer,128>;

namespace fast_io::manipulators
{

template<typename T>
concept crypto_hash_context = requires(T t,std::byte* ptr)
{
	T::digest_size;
	t.reset();
	t.transform(ptr,ptr);
	t.do_final();
	t.digest_to_ptr(ptr);
};

enum class digest_format
{
lower,
upper,
raw_bytes
};

template<digest_format d,typename T>
struct hash_digest_t
{
	using manip_tag = manip_tag_t;
	using reference_type = T;
	reference_type reference;
};

template<crypto_hash_context ctx>
inline constexpr hash_digest_t<digest_format::lower,ctx const&> hash_digest(ctx const& r) noexcept
{
	return {r};
}

template<crypto_hash_context ctx>
inline constexpr hash_digest_t<digest_format::upper,ctx const&> hash_digest_upper(ctx const& r) noexcept
{
	return {r};
}

template<crypto_hash_context ctx>
inline constexpr hash_digest_t<digest_format::raw_bytes,ctx const&> hash_digest_raw_bytes(ctx const& r) noexcept
{
	return {r};
}

template<digest_format d,std::size_t digest_size>
requires (static_cast<std::size_t>(d)<static_cast<std::size_t>(3))
inline constexpr std::size_t cal_crypto_hash_resrv_size() noexcept
{
	static_assert(digest_size<=SIZE_MAX/2);
	constexpr std::size_t v{d==digest_format::raw_bytes?digest_size:(digest_size<<1u)};
	return v;
}

template<digest_format d,std::size_t digest_size>
inline constexpr std::size_t crypto_hash_resrv_size_cache{cal_crypto_hash_resrv_size<d,digest_size>()};

template<std::integral char_type,digest_format d,typename T>
requires (static_cast<std::size_t>(d)<static_cast<std::size_t>(3))
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,hash_digest_t<d,T>>) noexcept
{
	return crypto_hash_resrv_size_cache<d,std::remove_cvref_t<T>::digest_size>;
}

template<digest_format d,typename T,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter prv_srv_hash_df_common_impl(Iter iter,T const& t) noexcept
{
	constexpr std::size_t digest_size{std::remove_cvref_t<T>::digest_size};
	std::byte buffer[digest_size];
	t.digest_to_ptr(buffer);
	if constexpr(d==digest_format::raw_bytes)
	{
		return non_overlapped_copy_n(buffer,digest_size,iter);
	}
	else
	{
		return ::fast_io::details::crypto_hash_pr_df_impl<d==digest_format::upper>(buffer,buffer+digest_size,iter);
	}
} 

template<digest_format d,typename T,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter prv_srv_hash_df_impl(Iter iter,T const& t) noexcept
{
	if constexpr(::fast_io::freestanding::contiguous_iterator<Iter>&&!::std::is_pointer_v<Iter>)
	{
		return prv_srv_hash_df_impl<d>(::fast_io::freestanding::to_address(iter),t)-::fast_io::freestanding::to_address(iter)+iter;
	}
	else
	{
		using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
		constexpr std::size_t digest_size{std::remove_cvref_t<T>::digest_size};
		if constexpr(d==digest_format::raw_bytes)
		{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
			if consteval
#else
			if(__builtin_is_constant_evaluated())
#endif
			{
				return prv_srv_hash_df_common_impl<d>(iter,t);
			}
			else
#endif
			{
				if constexpr(sizeof(char_type)==1&&::std::is_pointer_v<Iter>)
				{
					t.digest_to_ptr(reinterpret_cast<std::byte*>(iter));
					return t+digest_size;
				}
				else
				{
					return prv_srv_hash_df_common_impl<d>(iter,t);
				}
			}
		}
		else
		{
			return prv_srv_hash_df_common_impl<d>(iter,t);
		}
	}
}

template<digest_format d,typename T,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter print_reserve_define(
	::fast_io::io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,hash_digest_t<d,T>>,
	Iter iter,::fast_io::manipulators::hash_digest_t<d,T> t) noexcept
{
	return prv_srv_hash_df_impl<d>(iter,t.reference);
}

}

void test_sha512_ctx(sha512_context* ctx,std::byte const* first,std::byte const* last,std::byte* digest) noexcept
{
	ctx->update(first,last);
	ctx->do_final();
	ctx->digest_to_ptr(digest);
}


#if 0
int main()
{

}
#endif