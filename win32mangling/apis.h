#pragma once

namespace fast_io::win32
{

__declspec(dllimport) extern std::uint32_t __stdcall GetLastError(void) noexcept
#if 0
asm("GetLastError")
#endif
;

__declspec(dllimport) extern void * __stdcall LoadLibraryW(wchar_t const*) noexcept
#if 0
asm("LoadLibraryW")
#endif
;

// Used to retrieve a locale-specific message string for some error code
__declspec(dllimport) extern std::uint32_t __stdcall FormatMessageW(std::uint32_t, void const*, std::uint32_t,std::uint32_t, wchar_t*, std::uint32_t, void /*va_list*/ *) noexcept
#if 0
asm("FormatMessageW")
#endif
;

__declspec(dllimport) extern void* __stdcall CreateFileMappingW(void*,security_attributes*,std::uint32_t,std::uint32_t,std::uint32_t,wchar_t const*) noexcept
#if 0
asm("CreateFileMappingW")
#endif
;

__declspec(dllimport) extern void* __stdcall MapViewOfFile(void*,std::uint32_t,std::uint32_t,std::uint32_t,std::size_t) noexcept
#if 0
asm("MapViewOfFile")
#endif
;

__declspec(dllimport) extern int __stdcall SetEndOfFile(void*) noexcept
#if 0
asm("SetEndOfFile")
#endif
;

__declspec(dllimport) extern int __stdcall UnmapViewOfFile(void const*) noexcept
#if 0
asm("UnmapViewOfFile")
#endif
;

__declspec(dllimport) extern int __stdcall WriteFile(void*,void const*,std::uint32_t,std::uint32_t*,overlapped*) noexcept
#if 0
asm("WriteFile")
#endif
;

__declspec(dllimport) extern int __stdcall ReadFile(void*,void const*,std::uint32_t,std::uint32_t*,overlapped*) noexcept
#if 0
asm("ReadFile")
#endif
;

__declspec(dllimport) extern int __stdcall SetFilePointerEx(void*,std::int64_t,std::int64_t*,std::uint32_t) noexcept
#if 0
asm("SetFilePointerEx")
#endif
;

__declspec(dllimport) extern int __stdcall DuplicateHandle(void*,void*,void*,void**,std::uint32_t,int,std::uint32_t) noexcept
#if 0
asm("DuplicateHandle")
#endif
;

__declspec(dllimport) extern void* __stdcall GetStdHandle(std::uint32_t) noexcept
#if 0
asm("GetStdHandle")
#endif
;

__declspec(dllimport) extern int __stdcall CreatePipe(void**,void**,security_attributes*,std::uint32_t) noexcept
#if 0
asm("CreatePipe")
#endif
;

__declspec(dllimport) extern int __stdcall FreeLibrary(void*) noexcept
#if 0
asm("FreeLibrary")
#endif
;

using farproc = intptr_t(__stdcall*)() noexcept;

__declspec(dllimport) extern farproc __stdcall GetProcAddress(void*,char const*) noexcept
#if 0
asm("GetProcAddress")
#endif
;

__declspec(dllimport) extern void* __stdcall GetModuleHandleW(wchar_t const*) noexcept
#if 0
asm("GetModuleHandleW")
#endif
;

__declspec(dllimport) extern void* __stdcall LoadLibraryA(char const*) noexcept
#if 0
asm("LoadLibraryA")
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall WaitForSingleObject(void*,std::uint32_t) noexcept
#if 0
asm("WaitForSingleObject")
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall CancelIo(void*) noexcept
#if 0
asm("CancelIo")
#endif
;

__declspec(dllimport) extern int __stdcall GetFileInformationByHandle(void* __restrict,by_handle_file_information* __restrict) noexcept
#if 0
asm("GetFileInformationByHandle")
#endif
;

__declspec(dllimport) extern int __stdcall GetUserDefaultLocaleName(wchar_t*,int) noexcept
#if 0
asm("GetUserDefaultLocaleName")
#endif
;

__declspec(dllimport) extern void __stdcall GetSystemTimePreciseAsFileTime(filetime*) noexcept
#if 0
asm("GetSystemTimePreciseAsFileTime")
#endif
;

__declspec(dllimport) extern int __stdcall QueryUnbiasedInterruptTime(std::uint64_t* unbiasedtime) noexcept
#if 0
asm("QueryUnbiasedInterruptTime")
#endif
;

__declspec(dllimport) extern int __stdcall GetProcessTimes(void*,filetime*,filetime*,filetime*,filetime*) noexcept
#if 0
asm("GetProcessTimes")
#endif
;

__declspec(dllimport) extern int __stdcall GetThreadTimes(void*,filetime*,filetime*,filetime*,filetime*) noexcept
#if 0
asm("GetThreadTimes")
#endif
;

__declspec(dllimport) extern int __stdcall GetHandleInformation(void*,std::uint32_t*) noexcept
#if 0
asm("GetHandleInformation")
#endif
;
__declspec(dllimport) extern int __stdcall SetHandleInformation(void*,std::uint32_t,std::uint32_t) noexcept
#if 0
asm("SetHandleInformation")
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall GetTempPathW(std::uint32_t,wchar_t* buffer) noexcept
#if 0
asm("GetTempPathW")
#endif
;

__declspec(dllimport) extern void* __stdcall CreateFileW(wchar_t const*,std::uint32_t,std::uint32_t,security_attributes*,std::uint32_t,std::uint32_t,void*) noexcept
#if 0
asm("CreateFileW")
#endif
;

__declspec(dllimport) extern void* __stdcall CreateIoCompletionPort(void*,void*,std::uintptr_t,std::uint32_t) noexcept
#if 0
asm("CreateIoCompletionPort")
#endif
;
__declspec(dllimport) extern int __stdcall SystemFunction036(void*,std::uint32_t) noexcept
#if 0
asm("SystemFunction036")
#endif
;
__declspec(dllimport) extern int __stdcall CloseHandle(void*) noexcept
#if 0
asm("CloseHandle")
#endif
;
__declspec(dllimport) extern int __stdcall LockFileEx(void*,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,overlapped*) noexcept
#if 0
asm("LockFileEx")
#endif
;

__declspec(dllimport) extern int __stdcall UnlockFileEx(void*,std::uint32_t,std::uint32_t,std::uint32_t,overlapped*) noexcept
#if 0
asm("UnlockFileEx")
#endif
;

__declspec(dllimport) extern int __stdcall DeviceIoControl(void*,std::uint32_t,void*,std::uint32_t,void*,std::uint32_t,void*,overlapped*) noexcept
#if 0
asm("DeviceIoControl")
#endif
;

__declspec(dllimport) extern std::uint32_t __stdcall GetFileType(void*) noexcept
#if 0
asm("GetFileType")
#endif
;


__declspec(dllimport) extern std::uint32_t __stdcall GetACP() noexcept
#if 0
asm("GetACP")
#endif
;

__declspec(dllimport) extern errno_t __stdcall getenv_s(std::size_t *,char* buffer,std::size_t ,char const *) noexcept
#if 0
asm("getenv_s")
#endif
;

}