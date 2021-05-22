#include<cstdint>
#include<type_traits>
#include"nt_impl.h"


#define DEF2(x,y) auto get_##x() {return fast_io::win32::nt::x;} auto sys_##y() {return fast_io_nt_c_linkers::y;}

#define DEF0(x) auto get_##x() {return fast_io::win32::nt::x;} auto sys_##x() {return fast_io_nt_c_linkers::x;}

#define DEF(x) auto get_Nt##x() {return fast_io::win32::nt::Nt##x;} auto sys_Nt##x() {return fast_io_nt_c_linkers::Nt##x;}\
	 auto get_Zw##x() {return fast_io::win32::nt::Zw##x;} auto sys_Zw##x() {return fast_io_nt_c_linkers::Zw##x;}


DEF2(rtl_nt_status_to_dos_error,RtlNtStatusToDosError)
DEF(Close)
DEF(CreateFile)
DEF(CreateSection)
DEF(QueryInformationProcess)
DEF(WriteFile)
DEF(ReadFile)
DEF(QueryDirectoryFile)
DEF(QuerySection)
DEF(QueryInformationFile)
DEF(SetInformationFile)
DEF(DuplicateObject)
DEF(WaitForSingleObject)
DEF(SetSystemTime)
DEF2(rtl_dos_path_name_to_nt_path_name_u,RtlDosPathNameToNtPathName_U)
DEF2(rtl_free_unicode_string,RtlFreeUnicodeString)
DEF0(RtlInitializeCriticalSection)
DEF0(RtlEnterCriticalSection)
DEF0(RtlTryEnterCriticalSection)
DEF0(RtlLeaveCriticalSection)
DEF0(RtlDeleteCriticalSection)
DEF0(RtlCreateUserThread)
