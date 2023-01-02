#include<cstdint>
#include<fast_io.h>
#include<array>

namespace fast_io::win32
{

extern ::std::int_least32_t RegOpenKeyW(::std::uintptr_t,char16_t const*,::std::uintptr_t*) noexcept __asm__("RegOpenKeyW");

extern ::std::int_least32_t RegQueryValueExW(::std::uintptr_t,char16_t const*,::std::uint_least32_t*,::std::uint_least32_t*,void*,::std::uint_least32_t*) noexcept __asm__("RegQueryValueExW");

extern ::std::int_least32_t RegOpenKeyA(::std::uintptr_t,char16_t const*,::std::uintptr_t*) noexcept __asm__("RegOpenKeyA");

extern ::std::int_least32_t RegQueryValueExA(::std::uintptr_t,char16_t const*,::std::uint_least32_t*,::std::uint_least32_t*,void*,::std::uint_least32_t*) noexcept __asm__("RegQueryValueExA");

extern ::std::int_least32_t RegCloseKey(::std::uintptr_t) noexcept __asm__("RegCloseKey");

}

int main()
{
#if 0
	::fast_io::win32::time_dynamic_zone_information tdzi;
	std::uint_least32_t ret{::fast_io::win32::GetDynamicTimeZoneInformation(__builtin_addressof(tdzi))};
	println(fast_io::out(),"ret:",ret,
	"\n",fast_io::mnp::code_cvt(fast_io::mnp::os_c_str(tdzi.StandardName)),
	"\n",fast_io::mnp::code_cvt(fast_io::mnp::os_c_str(tdzi.DaylightName)),
	"\n",fast_io::mnp::code_cvt(fast_io::mnp::os_c_str(tdzi.TimeZoneKeyName)));
#endif

	std::uintptr_t k;
	auto win32retcode{::fast_io::win32::RegOpenKeyW(0x80000002 /*HKEY_LOCAL_MACHINE*/,u"SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation",__builtin_addressof(k))};
	if(win32retcode)
	{
		perrln(std::source_location::current()," ",win32retcode);
		return 1;
	}
	std::array<char16_t,64u> buffer;
	constexpr std::uint_least32_t bfbytes{buffer.size()*2};
	std::uint_least32_t cbdata{bfbytes};
	win32retcode=::fast_io::win32::RegQueryValueExW(k,u"StandardName",nullptr,nullptr,buffer.data(),__builtin_addressof(cbdata));
	if(win32retcode)
	{
		if(win32retcode!=0xEA /*ERROR_MORE_DATA*/)
		{
			perrln(std::source_location::current()," ",win32retcode);
			return 1;
		}
		perrln("cbdata:",cbdata);

		std::uint_least32_t const toallocate{(cbdata&0x1)+(cbdata>>1)};
		::fast_io::details::local_operator_new_array_ptr<char16_t,::fast_io::generic_allocator_adapter<::fast_io::win32_heapalloc_allocator>> uptr(toallocate);
		win32retcode=::fast_io::win32::RegQueryValueExW(k,u"StandardName",nullptr,nullptr,uptr.ptr,__builtin_addressof(cbdata));
		if(win32retcode)
		{
			perrln(std::source_location::current()," ",win32retcode);
			return 1;
		}
		println("not in buffer: ",fast_io::mnp::code_cvt(fast_io::mnp::os_c_str_n(uptr.ptr,toallocate-1)));
	}
	else
	{
		std::uint_least32_t const stringsize{(cbdata>>1)-1};
		println("cbdata:",cbdata,"\tin buffer: ",fast_io::mnp::code_cvt(fast_io::mnp::os_c_str_n(buffer.data(),stringsize)));
	}

}