#include<cstdint>
#include<type_traits>
#include"rtl_api.h"
#include"win32_definitions.h"
#include"apis.h"
#include<windows.h>

auto get_RtlSecureZeroMemory() noexcept
{
	return fast_io::win32::RtlSecureZeroMemory;
}

[[gnu::dllimport]] extern "C" void* __stdcall RtlSecureZeroMemorym(void* __restrict,std::size_t) noexcept;

auto sys_RtlSecureZeroMemorym() noexcept
{
	return RtlSecureZeroMemorym;
}

auto get_GetLastError() noexcept
{
	return fast_io::win32::GetLastError;
}

auto sys_GetLastError() noexcept
{
	return GetLastError;
}

auto get_LoadLibraryW() noexcept
{
	return fast_io::win32::LoadLibraryW;
}

auto sys_LoadLibraryW() noexcept
{
	return LoadLibraryW;
}

auto get_FormatMessageW() noexcept
{
	return fast_io::win32::FormatMessageW;
}

auto sys_FormatMessageW() noexcept
{
	return FormatMessageW;
}


auto get_CreateFileMappingW() noexcept
{
	return fast_io::win32::CreateFileMappingW;
}

auto sys_CreateFileMappingW() noexcept
{
	return CreateFileMappingW;
}

auto get_MapViewOfFile() noexcept
{
	return fast_io::win32::MapViewOfFile;
}

auto sys_MapViewOfFile() noexcept
{
	return MapViewOfFile;
}

auto get_SetEndOfFile() noexcept
{
	return fast_io::win32::SetEndOfFile;
}

auto sys_SetEndOfFile() noexcept
{
	return SetEndOfFile;
}

auto get_UnmapViewOfFile() noexcept
{
	return fast_io::win32::UnmapViewOfFile;
}

auto sys_UnmapViewOfFile() noexcept
{
	return UnmapViewOfFile;
}

auto get_WriteFile() noexcept
{
	return fast_io::win32::WriteFile;
}

auto sys_WriteFile() noexcept
{
	return WriteFile;
}


auto get_ReadFile() noexcept
{
	return fast_io::win32::ReadFile;
}

auto sys_ReadFile() noexcept
{
	return ReadFile;
}

auto get_SetFilePointerEx() noexcept
{
	return fast_io::win32::SetFilePointerEx;
}

auto sys_SetFilePointerEx() noexcept
{
	return SetFilePointerEx;
}

auto get_DuplicateHandle() noexcept
{
	return fast_io::win32::DuplicateHandle;
}

auto sys_DuplicateHandle() noexcept
{
	return DuplicateHandle;
}

auto get_GetStdHandle() noexcept
{
	return fast_io::win32::GetStdHandle;
}

auto sys_GetStdHandle() noexcept
{
	return GetStdHandle;
}

auto get_CreatePipe() noexcept
{
	return fast_io::win32::CreatePipe;
}

auto sys_CreatePipe() noexcept
{
	return CreatePipe;
}

auto get_FreeLibrary() noexcept
{
	return fast_io::win32::FreeLibrary;
}

auto sys_FreeLibrary() noexcept
{
	return FreeLibrary;
}

auto get_GetProcAddress() noexcept
{
	return fast_io::win32::GetProcAddress;
}

auto sys_GetProcAddress() noexcept
{
	return GetProcAddress;
}


auto get_GetModuleHandleW() noexcept
{
	return fast_io::win32::GetModuleHandleW;
}

auto sys_GetModuleHandleW() noexcept
{
	return GetModuleHandleW;
}

auto get_LoadLibraryA() noexcept
{
	return fast_io::win32::LoadLibraryA;
}

auto sys_LoadLibraryA() noexcept
{
	return LoadLibraryA;
}

auto get_WaitForSingleObject() noexcept
{
	return fast_io::win32::WaitForSingleObject;
}

auto sys_WaitForSingleObject() noexcept
{
	return WaitForSingleObject;
}


auto get_CancelIo() noexcept
{
	return fast_io::win32::CancelIo;
}

auto sys_CancelIo() noexcept
{
	return CancelIo;
}

auto get_GetFileInformationByHandle() noexcept
{
	return fast_io::win32::GetFileInformationByHandle;
}

auto sys_GetFileInformationByHandle() noexcept
{
	return GetFileInformationByHandle;
}

auto get_GetUserDefaultLocaleName() noexcept
{
	return fast_io::win32::GetUserDefaultLocaleName;
}

auto sys_GetUserDefaultLocaleName() noexcept
{
	return GetUserDefaultLocaleName;
}

auto get_GetSystemTimePreciseAsFileTime() noexcept
{
	return fast_io::win32::GetSystemTimePreciseAsFileTime;
}

auto sys_GetSystemTimePreciseAsFileTime() noexcept
{
	return GetSystemTimePreciseAsFileTime;
}

auto get_QueryUnbiasedInterruptTime() noexcept
{
	return fast_io::win32::QueryUnbiasedInterruptTime;
}

auto sys_QueryUnbiasedInterruptTime() noexcept
{
	return QueryUnbiasedInterruptTime;
}

auto get_GetProcessTimes() noexcept
{
	return fast_io::win32::GetProcessTimes;
}

auto sys_GetProcessTimes() noexcept
{
	return GetProcessTimes;
}

auto get_GetThreadTimes() noexcept
{
	return fast_io::win32::GetThreadTimes;
}

auto sys_GetThreadTimes() noexcept
{
	return GetThreadTimes;
}

auto get_GetHandleInformation() noexcept
{
	return fast_io::win32::GetHandleInformation;
}

auto sys_GetHandleInformation() noexcept
{
	return GetHandleInformation;
}

auto get_SetHandleInformation() noexcept
{
	return fast_io::win32::SetHandleInformation;
}

auto sys_SetHandleInformation() noexcept
{
	return SetHandleInformation;
}

auto get_GetTempPathW() noexcept
{
	return fast_io::win32::GetTempPathW;
}

auto sys_GetTempPathW() noexcept
{
	return GetTempPathW;
}

auto get_CreateFileW() noexcept
{
	return fast_io::win32::CreateFileW;
}

auto sys_CreateFileW() noexcept
{
	return CreateFileW;
}

auto get_CreateIoCompletionPort() noexcept
{
	return fast_io::win32::CreateIoCompletionPort;
}

auto sys_CreateIoCompletionPort() noexcept
{
	return CreateIoCompletionPort;
}

auto get_SystemFunction036() noexcept
{
	return fast_io::win32::SystemFunction036;
}

extern "C" __declspec(dllimport)  int __stdcall SystemFunction036(void*,std::uint32_t) noexcept;

auto sys_SystemFunction036() noexcept
{
	return SystemFunction036;
}

auto get_CloseHandle() noexcept
{
	return fast_io::win32::CloseHandle;
}

auto sys_CloseHandle() noexcept
{
	return CloseHandle;
}

auto get_LockFileEx() noexcept
{
	return fast_io::win32::LockFileEx;
}

auto sys_LockFileEx() noexcept
{
	return LockFileEx;
}


auto get_UnlockFileEx() noexcept
{
	return fast_io::win32::UnlockFileEx;
}

auto sys_UnlockFileEx() noexcept
{
	return UnlockFileEx;
}


auto get_DeviceIoControl() noexcept
{
	return fast_io::win32::DeviceIoControl;
}

auto sys_DeviceIoControl() noexcept
{
	return DeviceIoControl;
}

auto get_GetFileType() noexcept
{
	return fast_io::win32::GetFileType;
}

auto sys_GetFileType() noexcept
{
	return GetFileType;
}

auto get_GetACP() noexcept
{
	return fast_io::win32::GetACP;
}

auto sys_GetACP() noexcept
{
	return GetACP;
}

auto get_getenv_s() noexcept
{
	return fast_io::win32::getenv_s;
}
extern "C" __declspec(dllimport) errno_t __cdecl mgetenv_s(std::size_t *,char* buffer,std::size_t ,char const *) noexcept;


auto sys_mgetenv_s() noexcept
{
	return mgetenv_s;
}

auto get_MessageBoxW() noexcept
{
	return fast_io::win32::MessageBoxW;
}

auto sys_MessageBoxW() noexcept
{
	return MessageBoxW;
}