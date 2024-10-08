#include<cstdint>
#include<type_traits>
#include<cerrno>
#include"rtl_api.h"
#include"win32_definitions.h"
#include"apis.h"
#define WIN32_LEAN_AND_MEAN
#include<winsock2.h>
#include<windows.h>
#include<winsock.h>
#include<ws2tcpip.h>
#include<wincrypt.h>

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

extern "C" __declspec(dllimport) errno_t my__wgetenv_s(
   size_t *pReturnValue,
   char16_t* buffer,
   size_t numberOfElements,
   char16_t const* varname
) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
asm("_wgetenv_s")
#else
asm("__wgetenv_s")
#endif
#else
asm("_wgetenv_s")
#endif
#endif
;
}

DEF(GetLastError)
DEF(LoadLibraryA)
DEF(LoadLibraryW)
DEF(LoadLibraryExA)
DEF(LoadLibraryExW)
DEF(FormatMessageA)
DEF(FormatMessageW)
DEF(CreateFileMappingA)
DEF(CreateFileMappingW)
DEF(MapViewOfFile)
DEF(SetEndOfFile)
DEF(UnmapViewOfFile)
DEF(WriteFile)
DEF(ReadFile)
DEF(SetFilePointer)
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
DEF(GetUserDefaultLCID)
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
DEF(GetEnvironmentVariableA)
DEF(GetEnvironmentVariableW)
DEF_FROM_C_LINKER(getenv_s,my_getenv_s)
DEF_FROM_C_LINKER(_wgetenv_s,my__wgetenv_s)
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
DEF(WSADuplicateSocketA)
DEF(WSADuplicateSocketW)
DEF(WSACleanup)
DEF(WSAStartup)
DEF(WSAGetLastError)
DEF(closesocket)
DEF(WSASocketW)
DEF(WSASocketA)
DEF(bind)
DEF(listen)
DEF(WSAAccept)
DEF(ioctlsocket)
DEF(sendto)
DEF(WSASend)
DEF(WSASendMsg)
DEF(WSASendTo)
DEF(recv)
DEF(recvfrom)
DEF(WSARecv)
DEF(WSARecvFrom)
DEF(connect)
DEF(WSAConnect)
DEF(shutdown)
DEF(GetCurrentProcessId)
DEF(FlushFileBuffers)
DEF(GetQueuedCompletionStatus)
DEF(freeaddrinfo)
DEF(FreeAddrInfoW)
DEF(getaddrinfo)
DEF(GetAddrInfoW)
DEF(CryptAcquireContextA)
DEF(CryptAcquireContextW)
DEF(CryptReleaseContext)
DEF(CryptGenRandom)
DEF(RegOpenKeyA)
DEF(RegOpenKeyW)
DEF(RegQueryValueExA)
DEF(RegQueryValueExW)
DEF(RegCloseKey)
DEF(GetTimeZoneInformation)
DEF(SetConsoleCP)
DEF(SetConsoleOutputCP)
DEF(GetConsoleCP)
DEF(GetConsoleOutputCP)
DEF(AcquireSRWLockExclusive)
DEF(TryAcquireSRWLockExclusive)
DEF(ReleaseSRWLockExclusive)