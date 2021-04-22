
#if 0
namespace fast_io
{

struct io_alloc_t
{};

inline constexpr io_alloc_t io_alloc{};

namespace win32
{

[[gnu::dllimport,gnu::stdcall]]
extern void* HeapAlloc(void*,std::uint32_t,std::size_t) noexcept asm("HeapAlloc");
[[gnu::dllimport,gnu::stdcall]]
extern int HeapFree(void*,std::uint32_t,void*) noexcept asm("HeapFree");
[[gnu::dllimport,gnu::stdcall]]
extern void* HeapReAlloc(void*,std::uint32_t,void*,std::size_t) noexcept asm("HeapReAlloc");

namespace details
{

inline void* alloc_impl(std::size_t count) noexcept
{
	void * heap{std::bit_cast<void*>(std::uintptr_t(1))};
	auto ptr{HeapAlloc(heap,0,count)};
	if(ptr==nullptr)[[unlikely]]
		fast_terminate();
	return ptr;
}

inline void dealloc_impl(void* ptr) noexcept
{
	void * heap{std::bit_cast<void*>(std::uintptr_t(1))};
	HeapFree(heap,0,ptr);
}

inline void* realloc_impl(void* ptr,std::size_t count) noexcept
{
	void * heap{std::bit_cast<void*>(std::uintptr_t(1))};
	ptr=HeapReAlloc(heap,0,ptr,count);
	if(ptr==nullptr)[[unlikely]]
		fast_terminate();
	return ptr;
}

}

}

namespace details
{

inline void* alloc_impl(std::size_t count) noexcept
{
	if(count==0)[[unlikely]]
		count=1;
	auto ptr{__builtin_malloc(count)};
	if(ptr==nullptr)[[unlikely]]
		fast_terminate();
	return ptr;
}

inline void* realloc_impl(void* ptr,std::size_t count) noexcept
{
	if(count==0)[[unlikely]]
		count=1;
	ptr=__builtin_realloc(ptr,count);
	if(ptr=nullptr)[[unlikely]]
		fast_terminate();
	return ptr;
}

inline void* new_impl(std::size_t count,fast_io::io_alloc_t) noexcept
{
#ifdef _WIN32
	return ::fast_io::win32::details::alloc_impl(count);
#else
	return ::fast_io::details::alloc_impl(count);
#endif
}

inline void* new_realloc_impl(std::size_t count,void* oldptr,fast_io::io_alloc_t) noexcept
{
#ifdef _WIN32
	return ::fast_io::win32::details::alloc_impl(count);
#else
	return ::fast_io::details::alloc_impl(count);
#endif
}

inline void delete_impl(void* ptr,std::size_t,fast_io::io_alloc_t) noexcept
{
#ifdef _WIN32
	::fast_io::win32::details::dealloc_impl(ptr);
#else
	__builtin_free(ptr);
#endif
}

}

}
#if __has_cpp_attribute(gnu::weak)
[[gnu::weak]]
#endif
void* operator new(std::size_t count,fast_io::io_alloc_t) noexcept;

#if __has_cpp_attribute(gnu::weak)
[[gnu::weak]]
#endif
void* operator new(std::size_t count,void* oldptr,fast_io::io_alloc_t) noexcept;

#if __has_cpp_attribute(gnu::weak)
[[gnu::weak]]
#endif
void operator delete(void* oldptr,std::size_t,fast_io::io_alloc_t) noexcept;
#endif
