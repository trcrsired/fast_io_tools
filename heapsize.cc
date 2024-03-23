
namespace details
{

inline constexpr ::std::size_t guess_platform_malloc_bucket_size_impl(::std::size_t n) noexcept
{
	constexpr ::std::size_t minimum_heap_size_guess{sizeof(void*)*3};//3 pointer size
	if (n<=minimum_heap_size_guess)
	{
		return minimum_heap_size_guess;
	}
	constexpr
		::std::size_t mxval{::std::numeric_limits<::std::size_t>::max()};
	constexpr
		::std::size_t mxdv2{mxval>>1u};
	if (mxdv2 < n) [[unlikely]]
	{
		return mxval;
	}
	::std::size_t nbceil{::std::bit_ceil(n)};
	return nbceil-sizeof(minimum_heap_size_guess);
}

}