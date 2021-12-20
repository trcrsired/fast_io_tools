#include<fast_io_core.h>
#include<fast_io_crypto.h>
#include"md5_sha_hash_context.h"

#if 0
template<std::endian end,std::unsigned_integral U>
inline void sha256_512_do_final_to_ptr_simd16(U* digest,std::size_t n,std::byte* ptr) noexcept
{
	if constexpr(::std::endian::native==end)
	{
		::fast_io::details::my_memcpy(ptr,digest,n);
	}
	else
	{
		constexpr std::size_t ul64sz{sizeof(::std::uint_least64_t)};
		constexpr std::size_t factor{16u/ul64sz};
		::fast_io::details::intrinsics::simd_vector<U,factor> s;
		std::size_t nd2{n/factor};
		for(std::size_t i{};i!=nd2;++i)
		{
			s.load(digest);
			s.swap_endian();
			s.store(ptr);
			ptr+=ul64sz;
		}
	}
}
#endif

template<std::endian end,std::unsigned_integral U>
inline constexpr void sha256_512_do_final_to_ptr(U* digest,std::size_t n,std::byte* ptr) noexcept
{
	constexpr std::size_t usz{sizeof(U)};
	if consteval
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
	{
		if constexpr(::std::endian::native==end)
		{
			::fast_io::details::my_memcpy(ptr,digest,n);
		}
		else
		{
			for(std::size_t i{};i!=n;++i)
			{
				std::uint_least64_t v{::fast_io::byte_swap(digest[i])};
				::fast_io::details::my_memcpy(ptr,__builtin_addressof(v),usz);
				ptr+=usz;
			}
		}
	}
}

inline constexpr void sha512_do_final_to_ptr(std::uint_least64_t* digest,std::byte* ptr) noexcept
{
	constexpr std::size_t sz{8};
	sha256_512_do_final_to_ptr<std::endian::big>(digest,sz,ptr);
}

struct sha512_initializer
{
	static inline constexpr std::size_t digest_size{64};
	static inline constexpr ::fast_io::sha512 initialize_value{0x6a09e667f3bcc908, 0xbb67ae8584caa73b,0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,0x510e527fade682d1, 0x9b05688c2b3e6c1f,0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
	static inline constexpr void do_final_to_ptr(std::uint_least64_t* digest,std::byte* ptr) noexcept
	{
		sha512_do_final_to_ptr(digest,ptr);
	}
};

struct sha512_256_initializer
{
	static inline constexpr std::size_t digest_size{32};
	static inline constexpr ::fast_io::sha512 initialize_value{0x22312194fc2bf72c,0x9f555fa3c84c64c2,0x2393b86b6f53b151,0x963877195940eabd,0x96283ee2a88effe3,0xbe5e1e2553863992,0x2b0199fc2c85b8aa,0x0eb72ddc81c52ca2};
	static inline constexpr void do_final_to_ptr(std::uint_least64_t* digest,std::byte* ptr) noexcept
	{
		sha512_do_final_to_ptr(digest,ptr);
	}
};

using sha512_ctx = ::fast_io::details::basic_md5_sha_context_impl<::fast_io::sha512,sha512_initializer,__uint128_t>;

#if 0
int main()
{

}
#endif

void test_sha512_ctx(sha512_ctx* ctx,std::byte const* first,std::byte const* last,std::byte* digest) noexcept
{
	ctx->update(first,last);
	ctx->do_final_to_ptr(digest);
}