#include<cstdint>
#include<type_traits>
#include"rtl_api.h"
#include"win32_definitions.h"
#include"apis.h"
#include<windows.h>

#define DEF(x) auto get_##x() {return fast_io::win32::x;} auto sys_##x() {return x;}
#define DEF_FROM(x,y) auto get_##x() {return fast_io::win32::x;} auto sys_##x() {return y;}
#define DEF_FROM_C_LINKER(x,y) auto get_##x() {return fast_io::win32::x;} auto sys_##x() {return fast_io_c_linker::y;}

__declspec(dllimport) extern "C" int __stdcall SystemFunction036(void*,std::uint32_t) noexcept;
namespace fast_io_c_linker
{
extern "C" __declspec(dllimport) errno_t my_getenv_s(
   size_t *pReturnValue,
   char* buffer,
   size_t numberOfElements,
   char const* varname
) noexcept
#if defined(__clang__) || defined(__GNUC__)
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
}

DEF(GetLastError)
DEF(LoadLibraryA)
DEF(LoadLibraryW)
DEF(FormatMessageA)
DEF(FormatMessageW)
DEF(CreateFileMappingA)
DEF(CreateFileMappingW)
DEF(MapViewOfFile)
DEF(SetEndOfFile)
DEF(UnmapViewOfFile)
DEF(WriteFile)
DEF(ReadFile)
DEF(SetFilePointerEx)
DEF(DuplicateHandle)
DEF(GetStdHandle)
DEF(CreatePipe)
DEF(FreeLibrary)
DEF(GetProcAddress)
DEF(GetModuleHandleA)
DEF(GetModuleHandleW)
DEF(WaitForSingleObject)
DEF(CancelIo)
DEF(GetFileInformationByHandle)
DEF(GetUserDefaultLocaleName)
DEF(GetSystemTimePreciseAsFileTime)
DEF(GetSystemTimeAsFileTime)
DEF(QueryUnbiasedInterruptTime)
DEF(QueryPerformanceCounter)
DEF(QueryPerformanceFrequency)
DEF(GetProcessTimes)
DEF(GetThreadTimes)
DEF(GetHandleInformation)
DEF(SetHandleInformation)
DEF(GetTempPathA)
DEF(GetTempPathW)
DEF(CreateFileA)
DEF(CreateFileW)
DEF(CreateIoCompletionPort)
DEF(SystemFunction036)
DEF(CloseHandle)
DEF(LockFileEx)
DEF(UnlockFileEx)
DEF(DeviceIoControl)
DEF(GetFileType)
DEF(GetACP)
DEF_FROM_C_LINKER(getenv_s,my_getenv_s)
DEF(MessageBoxA)
DEF(MessageBoxW)
DEF(GetConsoleMode)
DEF(SetConsoleMode)
DEF(ReadConsoleA)
DEF(ReadConsoleW)
DEF(WriteConsoleA)
DEF(WriteConsoleW)
DEF(GetConsoleScreenBufferInfo)
DEF(ScrollConsoleScreenBufferA)
DEF(ScrollConsoleScreenBufferW)
DEF(SetConsoleCursorPosition)
DEF(InitializeCriticalSection)
DEF(EnterCriticalSection)
DEF(TryEnterCriticalSection)
DEF(LeaveCriticalSection)
DEF(DeleteCriticalSection)