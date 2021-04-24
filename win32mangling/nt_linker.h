#pragma once

#if defined(_MSC_VER)
#pragma comment(lib,"ntdll.lib")
#endif

namespace fast_io::win32::nt
{

__declspec(dllimport) extern std::uint32_t rtl_nt_status_to_dos_error(std::uint32_t status) noexcept;

__declspec(dllimport) extern std::uint32_t __stdcall NtClose(void*) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwClose(void*) noexcept;

template<bool zw>
inline std::uint32_t nt_close(void* handle) noexcept
{
	if constexpr(zw)
		return ZwClose(handle);
	else
		return NtClose(handle);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtCreateFile(void**,std::uint32_t,object_attributes*,io_status_block*,std::int64_t*,
			std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,void*,std::uint32_t) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwCreateFile(void**,std::uint32_t,object_attributes*,io_status_block*,std::int64_t*,
			std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,void*,std::uint32_t) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==11)
inline std::uint32_t nt_create_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwCreateFile(args...);
	else
		return NtCreateFile(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtCreateSection(void** __restrict,std::uint32_t,object_attributes* __restrict,
		std::uint64_t*,std::uint32_t,std::uint32_t,void* __restrict) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwCreateSection(void** __restrict,std::uint32_t,object_attributes* __restrict,
		std::uint64_t*,std::uint32_t,std::uint32_t,void* __restrict) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==7)
inline std::uint32_t nt_create_section(Args... args) noexcept
{
	if constexpr(zw)
		return ZwCreateSection(args...);
	else
		return NtCreateSection(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtQueryInformationProcess(void* __restrict,
		process_information_class,process_basic_information*,
		std::uint32_t,std::uint32_t*) noexcept;

__declspec(dllimport) extern std::uint32_t __stdcall ZwQueryInformationProcess(void* __restrict,
		process_information_class,process_basic_information*,
		std::uint32_t,std::uint32_t*) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint32_t nt_query_information_process(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQueryInformationProcess(args...);
	else
		return NtQueryInformationProcess(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtWriteFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;

__declspec(dllimport) extern std::uint32_t __stdcall ZwWriteFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==9)
inline std::uint32_t nt_write_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwWriteFile(args...);
	else
		return NtWriteFile(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtReadFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;

__declspec(dllimport) extern std::uint32_t __stdcall ZwReadFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==9)
inline std::uint32_t nt_read_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwReadFile(args...);
	else
		return NtReadFile(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtQueryDirectoryFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void*,std::uint32_t,file_information_class,int,unicode_string*,int) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwQueryDirectoryFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void*,std::uint32_t,file_information_class,int,unicode_string*,int) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==11)
inline std::uint32_t nt_query_directory_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQueryDirectoryFile(args...);
	else
		return NtQueryDirectoryFile(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtQuerySection(void*,section_information_class,void*,std::size_t,std::size_t*) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwQuerySection(void*,section_information_class,void*,std::size_t,std::size_t*) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint32_t nt_query_section(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQuerySection(args...);
	else
		return NtQuerySection(args...);
}


__declspec(dllimport) extern std::uint32_t __stdcall NtQueryInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwQueryInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint32_t nt_query_information_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQueryInformationFile(args...);
	else
		return NtQueryInformationFile(args...);
}


__declspec(dllimport) extern std::uint32_t __stdcall NtSetInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwSetInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint32_t nt_set_information_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwSetInformationFile(args...);
	else
		return NtSetInformationFile(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtDuplicateObject(void*,void*,void*,void**,std::uint32_t,std::uint32_t,std::uint32_t) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwDuplicateObject(void*,void*,void*,void**,std::uint32_t,std::uint32_t,std::uint32_t) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==7)
inline std::uint32_t nt_duplicate_object(Args... args) noexcept
{
	if constexpr(zw)
		return ZwDuplicateObject(args...);
	else
		return NtDuplicateObject(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtWaitForSingleObject(void*,int,std::uint64_t*) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwWaitForSingleObject(void*,int,std::uint64_t*) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==3)
inline std::uint32_t nt_wait_for_single_object(Args... args) noexcept
{
	if constexpr(zw)
		return ZwDuplicateObject(args...);
	else
		return NtDuplicateObject(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall NtSetSystemTime(std::uint64_t*,std::uint64_t*) noexcept;
__declspec(dllimport) extern std::uint32_t __stdcall ZwSetSystemTime(std::uint64_t*,std::uint64_t*) noexcept;

template<bool zw,typename... Args>
requires (sizeof...(Args)==2)
inline std::uint32_t nt_set_system_time(Args... args) noexcept
{
	if constexpr(zw)
		return ZwSetSystemTime(args...);
	else
		return NtSetSystemTime(args...);
}

__declspec(dllimport) extern std::uint32_t __stdcall rtl_dos_path_name_to_nt_path_name_u(wchar_t const*,unicode_string*,wchar_t const**,rtl_relative_name_u*) noexcept;

__declspec(dllimport) extern void __stdcall rtl_free_unicode_string(unicode_string*) noexcept;

__declspec(dllimport) extern void __stdcall RtlInitializeCriticalSection(void*) noexcept;

__declspec(dllimport) extern void __stdcall RtlEnterCriticalSection(void*) noexcept;

__declspec(dllimport) extern int __stdcall RtlTryEnterCriticalSection(void*) noexcept;

__declspec(dllimport) extern void __stdcall RtlLeaveCriticalSection(void*) noexcept;

__declspec(dllimport) extern void __stdcall RtlDeleteCriticalSection(void*) noexcept;

}