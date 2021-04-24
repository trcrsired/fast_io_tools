#pragma once
#define FAST_IO_NT_C_LINKER
#include"nt_definitions.h"
#undef FAST_IO_NT_C_LINKER

namespace fast_io_nt_c_linkers
{


extern "C" [[gnu::dllimport]] std::uint32_t __stdcall RtlNtStatusToDosError(std::uint32_t status) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtClose(void*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwClose(void*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtCreateFile(void**,std::uint32_t,object_attributes*,io_status_block*,std::int64_t*,
			std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,void*,std::uint32_t) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwCreateFile(void**,std::uint32_t,object_attributes*,io_status_block*,std::int64_t*,
			std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,void*,std::uint32_t) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtCreateSection(void** __restrict,std::uint32_t,object_attributes* __restrict,
		std::uint64_t*,std::uint32_t,std::uint32_t,void* __restrict) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwCreateSection(void** __restrict,std::uint32_t,object_attributes* __restrict,
		std::uint64_t*,std::uint32_t,std::uint32_t,void* __restrict) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtQueryInformationProcess(void* __restrict,
		process_information_class,process_basic_information*,
		std::uint32_t,std::uint32_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwQueryInformationProcess(void* __restrict,
		process_information_class,process_basic_information*,
		std::uint32_t,std::uint32_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtWriteFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwWriteFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtReadFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwReadFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint32_t,std::int64_t*,std::uint32_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtQueryDirectoryFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void*,std::uint32_t,file_information_class,int,unicode_string*,int) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwQueryDirectoryFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void*,std::uint32_t,file_information_class,int,unicode_string*,int) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtQuerySection(void*,section_information_class,void*,std::size_t,std::size_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwQuerySection(void*,section_information_class,void*,std::size_t,std::size_t*) noexcept;

extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtQueryInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwQueryInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;

extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtSetInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwSetInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint32_t,file_information_class) noexcept;

extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtDuplicateObject(void*,void*,void*,void**,std::uint32_t,std::uint32_t,std::uint32_t) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwDuplicateObject(void*,void*,void*,void**,std::uint32_t,std::uint32_t,std::uint32_t) noexcept;

extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtWaitForSingleObject(void*,int,std::uint64_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwWaitForSingleObject(void*,int,std::uint64_t*) noexcept;

extern "C" [[gnu::dllimport]] std::uint32_t __stdcall NtSetSystemTime(std::uint64_t*,std::uint64_t*) noexcept;
extern "C" [[gnu::dllimport]] std::uint32_t __stdcall ZwSetSystemTime(std::uint64_t*,std::uint64_t*) noexcept;

extern "C" [[gnu::dllimport]] std::uint32_t __stdcall RtlDosPathNameToNtPathName_U(wchar_t const*,unicode_string*,wchar_t const**,rtl_relative_name_u*) noexcept;

extern "C" [[gnu::dllimport]] void __stdcall RtlFreeUnicodeString(unicode_string*) noexcept;

extern "C" [[gnu::dllimport]] void __stdcall RtlInitializeCriticalSection(void*) noexcept;
extern "C" [[gnu::dllimport]] void __stdcall RtlEnterCriticalSection(void*) noexcept;
extern "C" [[gnu::dllimport]] int __stdcall RtlTryEnterCriticalSection(void*) noexcept;
extern "C" [[gnu::dllimport]] void __stdcall RtlLeaveCriticalSection(void*) noexcept;
extern "C" [[gnu::dllimport]] void __stdcall RtlDeleteCriticalSection(void*) noexcept;

}