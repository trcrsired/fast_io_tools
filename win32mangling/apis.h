#pragma once

namespace fast_io::win32
{

__declspec(dllimport) extern std::uint32_t __stdcall GetLastError(void) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetLastError@0")
#else
asm("_GetLastError@0")
#endif
#else
asm("GetLastError")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall LoadLibraryA(char const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("LoadLibraryA@4")
#else
asm("_LoadLibraryA@4")
#endif
#else
asm("LoadLibraryA")
#endif
#endif
;

__declspec(dllimport) extern void * __stdcall LoadLibraryW(char16_t const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("LoadLibraryW@4")
#else
asm("_LoadLibraryW@4")
#endif
#else
asm("LoadLibraryW")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall LoadLibraryExA(char const*,void*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("LoadLibraryExA@12")
#else
asm("_LoadLibraryExA@12")
#endif
#else
asm("LoadLibraryExA")
#endif
#endif
;

__declspec(dllimport) extern void * __stdcall LoadLibraryExW(char16_t const*,void*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("LoadLibraryExW@12")
#else
asm("_LoadLibraryExW@12")
#endif
#else
asm("LoadLibraryExW")
#endif
#endif
;

// Used to retrieve a locale-specific message string for some error code
__declspec(dllimport) extern std::uint32_t __stdcall FormatMessageA(std::uint32_t, char const*, std::uint32_t,std::uint32_t, char*, std::uint32_t, void /*va_list*/ *) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("FormatMessageA@28")
#else
asm("_FormatMessageA@28")
#endif
#else
asm("FormatMessageA")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall FormatMessageW(std::uint32_t, char16_t const*, std::uint32_t,std::uint32_t, char16_t*, std::uint32_t, void /*va_list*/ *) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("FormatMessageW@28")
#else
asm("_FormatMessageW@28")
#endif
#else
asm("FormatMessageW")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall CreateFileMappingA(void*,security_attributes*,std::uint32_t,std::uint32_t,std::uint32_t,char const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CreateFileMappingA@24")
#else
asm("_CreateFileMappingA@24")
#endif
#else
asm("CreateFileMappingA")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall CreateFileMappingW(void*,security_attributes*,std::uint32_t,std::uint32_t,std::uint32_t,char16_t const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CreateFileMappingW@24")
#else
asm("_CreateFileMappingW@24")
#endif
#else
asm("CreateFileMappingW")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall MapViewOfFile(void*,std::uint32_t,std::uint32_t,std::uint32_t,std::size_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("MapViewOfFile@20")
#else
asm("_MapViewOfFile@20")
#endif
#else
asm("MapViewOfFile")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall SetEndOfFile(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("SetEndOfFile@4")
#else
asm("_SetEndOfFile@4")
#endif
#else
asm("SetEndOfFile")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall UnmapViewOfFile(void const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("UnmapViewOfFile@4")
#else
asm("_UnmapViewOfFile@4")
#endif
#else
asm("UnmapViewOfFile")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WriteFile(void*,void const*,std::uint32_t,std::uint32_t*,overlapped*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("WriteFile@20")
#else
asm("_WriteFile@20")
#endif
#else
asm("WriteFile")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall ReadFile(void*,void const*,std::uint32_t,std::uint32_t*,overlapped*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
asm("ReadFile@20")
#else
asm("ReadFile")
#endif
#endif
;

__declspec(dllimport) extern std::uint_least32_t __stdcall SetFilePointer(void*,std::int_least32_t,std::int_least32_t*,std::uint_least32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("SetFilePointerEx@16")
#else
asm("_SetFilePointerEx@16")
#endif
#else
asm("SetFilePointerEx")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall SetFilePointerEx(void*,std::int64_t,std::int64_t*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("SetFilePointerEx@20")
#else
asm("_SetFilePointerEx@20")
#endif
#else
asm("SetFilePointerEx")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall DuplicateHandle(void*,void*,void*,void**,std::uint32_t,int,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("DuplicateHandle@28")
#else
asm("_DuplicateHandle@28")
#endif
#else
asm("DuplicateHandle")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall GetStdHandle(std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetStdHandle@4")
#else
asm("_GetStdHandle@4")
#endif
#else
asm("GetStdHandle")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall CreatePipe(void**,void**,security_attributes*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CreatePipe@16")
#else
asm("_CreatePipe@16")
#endif
#else
asm("CreatePipe")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall FreeLibrary(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("FreeLibrary@4")
#else
asm("_FreeLibrary@4")
#endif
#else
asm("FreeLibrary")
#endif
#endif
;

using farproc = intptr_t(__stdcall*)() noexcept;

__declspec(dllimport) extern farproc __stdcall GetProcAddress(void*,char const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetProcAddress@8")
#else
asm("_GetProcAddress@8")
#endif
#else
asm("GetProcAddress")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall GetModuleHandleA(char const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetModuleHandleA@4")
#else
asm("_GetModuleHandleA@4")
#endif
#else
asm("GetModuleHandleA")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall GetModuleHandleW(char16_t const*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetModuleHandleW@4")
#else
asm("_GetModuleHandleW@4")
#endif
#else
asm("GetModuleHandleW")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall WaitForSingleObject(void*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("WaitForSingleObject@8")
#else
asm("_WaitForSingleObject@8")
#endif
#else
asm("WaitForSingleObject")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall CancelIo(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CancelIo@4")
#else
asm("_CancelIo@4")
#endif
#else
asm("CancelIo")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall GetFileInformationByHandle(void* __restrict,by_handle_file_information* __restrict) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetFileInformationByHandle@8")
#else
asm("_GetFileInformationByHandle@8")
#endif
#else
asm("GetFileInformationByHandle")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall GetUserDefaultLocaleName(char16_t*,int) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetUserDefaultLocaleName@8")
#else
asm("_GetUserDefaultLocaleName@8")
#endif
#else
asm("GetUserDefaultLocaleName")
#endif
#endif
;

__declspec(dllimport) extern ::std::uint_least32_t __stdcall GetUserDefaultLCID(void) noexcept;

__declspec(dllimport) extern void __stdcall GetSystemTimePreciseAsFileTime(filetime*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetSystemTimePreciseAsFileTime@4")
#else
asm("_GetSystemTimePreciseAsFileTime@4")
#endif
#else
asm("GetSystemTimePreciseAsFileTime")
#endif
#endif
;

__declspec(dllimport) extern void __stdcall GetSystemTimeAsFileTime(filetime*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetSystemTimeAsFileTime@4")
#else
asm("_GetSystemTimeAsFileTime@4")
#endif
#else
asm("GetSystemTimeAsFileTime")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall QueryUnbiasedInterruptTime(std::uint64_t*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("QueryUnbiasedInterruptTime@4")
#else
asm("_QueryUnbiasedInterruptTime@4")
#endif
#else
asm("QueryUnbiasedInterruptTime")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall QueryPerformanceCounter(std::int64_t*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("QueryPerformanceCounter@4")
#else
asm("_QueryPerformanceCounter@4")
#endif
#else
asm("QueryPerformanceCounter")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall QueryPerformanceFrequency(std::int64_t*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("QueryPerformanceFrequency@4")
#else
asm("_QueryPerformanceFrequency@4")
#endif
#else
asm("QueryPerformanceFrequency")
#endif
#endif
;


__declspec(dllimport) extern int __stdcall GetProcessTimes(void*,filetime*,filetime*,filetime*,filetime*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetProcessTimes@20")
#else
asm("_GetProcessTimes@20")
#endif
#else
asm("GetProcessTimes")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall GetThreadTimes(void*,filetime*,filetime*,filetime*,filetime*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetThreadTimes@20")
#else
asm("_GetThreadTimes@20")
#endif
#else
asm("GetThreadTimes")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall GetHandleInformation(void*,std::uint32_t*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetHandleInformation@8")
#else
asm("_GetHandleInformation@8")
#endif
#else
asm("GetHandleInformation")
#endif
#endif
;
__declspec(dllimport) extern int __stdcall SetHandleInformation(void*,std::uint32_t,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("SetHandleInformation@12")
#else
asm("_SetHandleInformation@12")
#endif
#else
asm("SetHandleInformation")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall GetTempPathA(std::uint32_t,char* buffer) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetTempPathA@8")
#else
asm("_GetTempPathA@8")
#endif
#else
asm("GetTempPathA")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall GetTempPathW(std::uint32_t,char16_t* buffer) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetTempPathW@8")
#else
asm("_GetTempPathW@8")
#endif
#else
asm("GetTempPathW")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall CreateFileA(char const*,std::uint32_t,std::uint32_t,security_attributes*,std::uint32_t,std::uint32_t,void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CreateFileA@28")
#else
asm("_CreateFileA@28")
#endif
#else
asm("CreateFileA")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall CreateFileW(char16_t const*,std::uint32_t,std::uint32_t,security_attributes*,std::uint32_t,std::uint32_t,void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CreateFileW@28")
#else
asm("_CreateFileW@28")
#endif
#else
asm("CreateFileW")
#endif
#endif
;

__declspec(dllimport) extern void* __stdcall CreateIoCompletionPort(void*,void*,std::uintptr_t,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CreateIoCompletionPort@16")
#else
asm("_CreateIoCompletionPort@16")
#endif
#else
asm("CreateIoCompletionPort")
#endif
#endif
;
__declspec(dllimport) extern int __stdcall SystemFunction036(void*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("SystemFunction036@8")
#else
asm("_SystemFunction036@8")
#endif
#else
asm("SystemFunction036")
#endif
#endif
;
__declspec(dllimport) extern int __stdcall CloseHandle(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("CloseHandle@4")
#else
asm("_CloseHandle@4")
#endif
#else
asm("CloseHandle")
#endif
#endif
;
__declspec(dllimport) extern int __stdcall LockFileEx(void*,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,overlapped*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("LockFileEx@24")
#else
asm("_LockFileEx@24")
#endif
#else
asm("LockFileEx")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall UnlockFileEx(void*,std::uint32_t,std::uint32_t,std::uint32_t,overlapped*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("UnlockFileEx@20")
#else
asm("_UnlockFileEx@20")
#endif
#else
asm("UnlockFileEx")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall DeviceIoControl(void*,std::uint32_t,void*,std::uint32_t,void*,std::uint32_t,void*,overlapped*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("DeviceIoControl@32")
#else
asm("_DeviceIoControl@32")
#endif
#else
asm("DeviceIoControl")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall GetFileType(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetFileType@4")
#else
asm("_GetFileType@4")
#endif
#else
asm("GetFileType")
#endif
#endif
;


__declspec(dllimport) extern std::uint32_t __stdcall GetACP() noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetACP@0")
#else
asm("_GetACP@0")
#endif
#else
asm("GetACP")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(__gnu__::__dllimport__)
[[__gnu__::__dllimport__]]
#endif
extern ::std::uint_least32_t __stdcall GetEnvironmentVariableA(char const*,char*,::std::uint_least32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetEnvironmentVariableA@12")
#else
__asm__("_GetEnvironmentVariableA@12")
#endif
#else
__asm__("GetEnvironmentVariableA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(__gnu__::__dllimport__)
[[__gnu__::__dllimport__]]
#endif
extern ::std::uint_least32_t __stdcall GetEnvironmentVariableW(char16_t const*,char16_t*,::std::uint_least32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetEnvironmentVariableW@12")
#else
__asm__("_GetEnvironmentVariableW@12")
#endif
#else
__asm__("GetEnvironmentVariableW")
#endif
#endif
;

#if !defined(__CYGWIN__)
__declspec(dllimport) extern errno_t __cdecl getenv_s(std::size_t *,char* buffer,std::size_t ,char const *) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("getenv_s")
#else
asm("_getenv_s")
#endif
#else
asm("getenv_s")
#endif
#endif
;
#endif

__declspec(dllimport) extern errno_t __cdecl _wgetenv_s(std::size_t*,char16_t*,std::size_t,char16_t const *) noexcept;

__declspec(dllimport) extern std::uint32_t __stdcall MessageBoxA(void*,char const*,char const*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("MessageBoxA@16")
#else
asm("_MessageBoxA@16")
#endif
#else
asm("MessageBoxA")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall MessageBoxW(void*,char16_t const*,char16_t const*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("MessageBoxW@16")
#else
asm("_MessageBoxW@16")
#endif
#else
asm("MessageBoxW")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall GetConsoleMode(void *, std::uint32_t *) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetConsoleMode@8")
#else
asm("_GetConsoleMode@8")
#endif
#else
asm("GetConsoleMode")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall SetConsoleMode(void *, std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("SetConsoleMode@8")
#else
asm("_SetConsoleMode@8")
#endif
#else
asm("SetConsoleMode")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall ReadConsoleA(void*,void*,std::uint32_t,std::uint32_t*,void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("ReadConsoleA@20")
#else
asm("_ReadConsoleA@20")
#endif
#else
asm("ReadConsoleA")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall ReadConsoleW(void*,void*,std::uint32_t,std::uint32_t*,void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("ReadConsoleW@20")
#else
asm("_ReadConsoleW@20")
#endif
#else
asm("ReadConsoleW")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WriteConsoleA(void*,void const*,std::uint32_t,std::uint32_t*,void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("WriteConsoleA@20")
#else
asm("_WriteConsoleA@20")
#endif
#else
asm("WriteConsoleA")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WriteConsoleW(void*,void const*,std::uint32_t,std::uint32_t*,void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("WriteConsoleW@20")
#else
asm("_WriteConsoleW@20")
#endif
#else
asm("WriteConsoleW")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall GetConsoleScreenBufferInfo(void *, console_screen_buffer_info *) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("GetConsoleScreenBufferInfo@8")
#else
asm("_GetConsoleScreenBufferInfo@8")
#endif
#else
asm("GetConsoleScreenBufferInfo")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall ScrollConsoleScreenBufferA(void *, small_rect const *, small_rect const *, coord, char_info const *) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("ScrollConsoleScreenBufferA@20")
#else
asm("_ScrollConsoleScreenBufferA@20")
#endif
#else
asm("ScrollConsoleScreenBufferA")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall ScrollConsoleScreenBufferW(void *, small_rect const *, small_rect const *, coord, char_info const *) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("ScrollConsoleScreenBufferW@20")
#else
asm("_ScrollConsoleScreenBufferW@20")
#endif
#else
asm("ScrollConsoleScreenBufferW")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall SetConsoleCursorPosition(void *, coord) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("SetConsoleCursorPosition@8")
#else
asm("_SetConsoleCursorPosition@8")
#endif
#else
asm("SetConsoleCursorPosition")
#endif
#endif
;


__declspec(dllimport) extern void __stdcall InitializeCriticalSection(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("InitializeCriticalSection@4")
#else
asm("_InitializeCriticalSection@4")
#endif
#else
asm("InitializeCriticalSection")
#endif
#endif
;

__declspec(dllimport) extern void __stdcall EnterCriticalSection(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("EnterCriticalSection@4")
#else
asm("_EnterCriticalSection@4")
#endif
#else
asm("EnterCriticalSection")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall TryEnterCriticalSection(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("TryEnterCriticalSection@4")
#else
asm("_TryEnterCriticalSection@4")
#endif
#else
asm("TryEnterCriticalSection")
#endif
#endif
;

__declspec(dllimport) extern void __stdcall LeaveCriticalSection(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("LeaveCriticalSection@4")
#else
asm("_LeaveCriticalSection@4")
#endif
#else
asm("LeaveCriticalSection")
#endif
#endif
;

__declspec(dllimport) extern void __stdcall DeleteCriticalSection(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("DeleteCriticalSection@4")
#else
asm("_DeleteCriticalSection@4")
#endif
#else
asm("DeleteCriticalSection")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WSADuplicateSocketA(void*,std::uint32_t,wsaprotocol_infoa*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSADuplicateSocketA@12")
#else
asm("_WSADuplicateSocketA@12")
#endif
#else
asm("WSADuplicateSocketA")
#endif
#endif
;

__declspec(dllimport) extern void __stdcall WSADuplicateSocketW(void*,std::uint32_t,wsaprotocol_infow*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSADuplicateSocketW@12")
#else
asm("_WSADuplicateSocketW@12")
#endif
#else
asm("WSADuplicateSocketW")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WSACleanup() noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSACleanup@0")
#else
asm("_WSACleanup@0")
#endif
#else
asm("WSACleanup")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WSAStartup(std::uint32_t,wsadata*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSAStartup@8")
#else
asm("_WSAStartup@8")
#endif
#else
asm("WSAStartup")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WSAGetLastError() noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSAGetLastError@0")
#else
asm("_WSAGetLastError@0")
#endif
#else
asm("WSAGetLastError")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall closesocket(std::uintptr_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("closesocket@4")
#else
asm("_closesocket@4")
#endif
#else
asm("closesocket")
#endif
#endif
;

__declspec(dllimport) extern std::uintptr_t __stdcall WSASocketW(int,int,int,wsaprotocol_infow*,std::uint32_t,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSASocketW@24")
#else
asm("_WSASocketW@24")
#endif
#else
asm("WSASocketW")
#endif
#endif
;

__declspec(dllimport) extern std::uintptr_t __stdcall WSASocketA(int,int,int,wsaprotocol_infoa*,std::uint32_t,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSASocketA@24")
#else
asm("_WSASocketA@24")
#endif
#else
asm("WSASocketA")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall bind(std::uintptr_t,void const*,int) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("bind@12")
#else
asm("_bind@12")
#endif
#else
asm("bind")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall listen(std::uintptr_t,int) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("listen@8")
#else
asm("_listen@8")
#endif
#else
asm("listen")
#endif
#endif
;

__declspec(dllimport) extern std::uintptr_t __stdcall WSAAccept(std::uintptr_t,void const*,int*,lpconditionproc,std::uintptr_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSAAccept@20")
#else
asm("_WSAAccept@20")
#endif
#else
asm("WSAAccept")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall ioctlsocket(std::uintptr_t,long,std::uint32_t*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("ioctlsocket@12")
#else
asm("_ioctlsocket@12")
#endif
#else
asm("ioctlsocket")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	sendto(::std::size_t, char const *, int, int, void const *, int) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("sendto@24")
#else
	__asm__("_sendto@24")
#endif
#else
	__asm__("sendto")
#endif
#endif
		;


__declspec(dllimport) extern int __stdcall WSASend(std::uintptr_t,wsabuf*,std::uint32_t,std::uint32_t*,std::uint32_t,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSASend@28")
#else
asm("_WSASend@28")
#endif
#else
asm("WSASend")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WSASendMsg(std::uintptr_t,wsamsg*,std::uint32_t,std::uint32_t*,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSASendMsg@24")
#else
asm("_WSASendMsg@24")
#endif
#else
asm("WSASendMsg")
#endif
#endif
;


__declspec(dllimport) extern int __stdcall WSASendTo(std::uintptr_t,wsabuf*,std::uint32_t,std::uint32_t*,std::uint32_t,void const*,int,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSASendTo@36")
#else
asm("_WSASendTo@36")
#endif
#else
asm("WSASendTo")
#endif
#endif
;


__declspec(dllimport) extern int __stdcall recv(std::uintptr_t,char* buf,int len,int flags) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("recv@16")
#else
asm("_recv@16")
#endif
#else
asm("recv")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	recvfrom(::std::size_t, char *, int, int, void *, int *) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("recvfrom@24")
#else
	__asm__("_recvfrom@24")
#endif
#else
	__asm__("recvfrom")
#endif
#endif
		;

__declspec(dllimport) extern int __stdcall WSARecv(std::uintptr_t,wsabuf*,std::uint32_t,std::uint32_t*,std::uint32_t*,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSARecv@28")
#else
asm("_WSARecv@28")
#endif
#else
asm("WSARecv")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WSARecvFrom(std::uintptr_t,wsabuf*,std::uint32_t,std::uint32_t*,std::uint32_t*,void*,int*,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSARecvFrom@36")
#else
asm("_WSARecvFrom@36")
#endif
#else
asm("WSARecvFrom")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall connect(std::uintptr_t,void const*,int) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("connect@12")
#else
asm("_connect@12")
#endif
#else
asm("connect")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall WSAConnect(std::uintptr_t,void const*,int,wsabuf*,wsabuf*,qualityofservice*,qualityofservice*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("WSAConnect@28")
#else
asm("_WSAConnect@28")
#endif
#else
asm("WSAConnect")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall shutdown(std::uintptr_t,void const*,int) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("shutdown@12")
#else
asm("_shutdown@12")
#endif
#else
asm("shutdown")
#endif
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall GetCurrentProcessId() noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("GetCurrentProcessId@0")
#else
asm("_GetCurrentProcessId@0")
#endif
#else
asm("GetCurrentProcessId")
#endif
#endif
;

__declspec(dllimport) extern int __stdcall FlushFileBuffers(void*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("FlushFileBuffers@4")
#else
asm("_FlushFileBuffers@4")
#endif
#else
asm("FlushFileBuffers")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern int __stdcall GetQueuedCompletionStatus(void*,std::uint32_t*,std::uintptr_t*,overlapped*,std::uint32_t) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("GetQueuedCompletionStatus@20")
#else
asm("_GetQueuedCompletionStatus@20")
#endif
#else
asm("GetQueuedCompletionStatus")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern void __stdcall freeaddrinfo(win32_addrinfo_9xa*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("freeaddrinfo@4")
#else
asm("_freeaddrinfo@4")
#endif
#else
asm("freeaddrinfo")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern void __stdcall FreeAddrInfoW(win32_addrinfo_ntw*) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("FreeAddrInfoW@4")
#else
asm("_FreeAddrInfoW@4")
#endif
#else
asm("FreeAddrInfoW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern int __stdcall getaddrinfo(char const*,char const*,win32_addrinfo_9xa const*,win32_addrinfo_9xa**) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("getaddrinfo@16")
#else
asm("_getaddrinfo@16")
#endif
#else
asm("getaddrinfo")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern int __stdcall GetAddrInfoW(char16_t const*,char16_t const*,win32_addrinfo_ntw const*,win32_addrinfo_ntw**) noexcept
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("GetAddrInfoW@16")
#else
asm("_GetAddrInfoW@16")
#endif
#else
asm("GetAddrInfoW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern int __stdcall CryptAcquireContextA(::std::uintptr_t*,char8_t const*,char8_t const*,::std::uint_least32_t,::std::uint_least32_t) noexcept;
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptAcquireContextA@20")
#else
__asm__("_CryptAcquireContextA@20")
#endif
#else
__asm__("CryptAcquireContextA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern int __stdcall CryptAcquireContextW(::std::uintptr_t*,char16_t const*,char16_t const*,::std::uint_least32_t,::std::uint_least32_t) noexcept;
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptAcquireContextW@20")
#else
__asm__("_CryptAcquireContextW@20")
#endif
#else
__asm__("CryptAcquireContextW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern int __stdcall CryptReleaseContext(::std::uintptr_t,::std::uint_least32_t) noexcept;
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptReleaseContext@8")
#else
__asm__("_CryptReleaseContext@8")
#endif
#else
__asm__("CryptReleaseContext")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern int __stdcall CryptGenRandom(::std::uintptr_t,::std::uint_least32_t,char unsigned*) noexcept;
#if 0
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptGenRandom@12")
#else
__asm__("_CryptGenRandom@12")
#endif
#else
__asm__("CryptGenRandom")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegOpenKeyA(::std::uintptr_t,char8_t const*,::std::uintptr_t*) noexcept
#if 0
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegOpenKeyA@12")
#else
__asm__("_RegOpenKeyA@12")
#endif
#else
__asm__("RegOpenKeyA")
#endif
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegOpenKeyW(::std::uintptr_t,char16_t const*,::std::uintptr_t*) noexcept
#if 0
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegOpenKeyW@12")
#else
__asm__("_RegOpenKeyW@12")
#endif
#else
__asm__("RegOpenKeyW")
#endif
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegQueryValueExA(::std::uintptr_t,char8_t const*,::std::uint_least32_t*,::std::uint_least32_t*,void*,::std::uint_least32_t*) noexcept
#if 0
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegQueryValueExA@24")
#else
__asm__("_RegQueryValueExA@24")
#endif
#else
__asm__("RegQueryValueExA")
#endif
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegQueryValueExW(::std::uintptr_t,char16_t const*,::std::uint_least32_t*,::std::uint_least32_t*,void*,::std::uint_least32_t*) noexcept
#if 0
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegQueryValueExW@24")
#else
__asm__("_RegQueryValueExW@24")
#endif
#else
__asm__("RegQueryValueExW")
#endif
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegCloseKey(::std::uintptr_t) noexcept
#if 0
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegCloseKey@4")
#else
__asm__("_RegCloseKey@4")
#endif
#else
__asm__("RegCloseKey")
#endif
#endif
#endif
;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	GetTimeZoneInformation(time_zone_information *) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("GetTimeZoneInformation@4")
#else
	__asm__("_GetTimeZoneInformation@4")
#endif
#else
	__asm__("GetTimeZoneInformation")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	SetConsoleCP(::std::uint_least32_t) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("SetConsoleCP@4")
#else
	__asm__("_SetConsoleCP@4")
#endif
#else
	__asm__("SetConsoleCP")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	SetConsoleOutputCP(::std::uint_least32_t) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("SetConsoleOutputCP@4")
#else
	__asm__("_SetConsoleOutputCP@4")
#endif
#else
	__asm__("SetConsoleOutputCP")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	GetConsoleCP() noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("GetConsoleCP@0")
#else
	__asm__("_GetConsoleCP@0")
#endif
#else
	__asm__("GetConsoleCP")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	GetConsoleOutputCP() noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("GetConsoleOutputCP@0")
#else
	__asm__("_GetConsoleOutputCP@0")
#endif
#else
	__asm__("GetConsoleOutputCP")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	AcquireSRWLockExclusive(void *) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("AcquireSRWLockExclusive@4")
#else
	__asm__("_AcquireSRWLockExclusive@4")
#endif
#else
	__asm__("AcquireSRWLockExclusive")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	TryAcquireSRWLockExclusive(void *) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("TryAcquireSRWLockExclusive@4")
#else
	__asm__("_TryAcquireSRWLockExclusive@4")
#endif
#else
	__asm__("TryAcquireSRWLockExclusive")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ReleaseSRWLockExclusive(void *) noexcept
#if 0
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ReleaseSRWLockExclusive@4")
#else
	__asm__("_ReleaseSRWLockExclusive@4")
#endif
#else
	__asm__("ReleaseSRWLockExclusive")
#endif
#endif
		;

}
