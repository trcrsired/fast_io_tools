#pragma once

namespace fast_io::win32
{

struct overlapped
{
std::conditional_t<(sizeof(std::uintptr_t)>4),std::uint64_t,std::uint32_t> Internal,InternalHigh;
union dummy_union_name_t
{
struct dummy_struct_name_t
{
std::uint32_t Offset;
std::uint32_t OffsetHigh;
} dummy_struct_name;
void* Pointer;
}
dummy_union_name;
void* hEvent;
};
struct security_attributes
{
std::uint32_t nLength;
void* lpSecurityDescriptor;
int bInheritHandle;
};
struct startupinfo
{
std::uint32_t cb;
wchar_t* lpReserved;
wchar_t* lpDesktop;
wchar_t* lpTitle;
std::uint32_t dwX;
std::uint32_t dwY;
std::uint32_t dwXSize;
std::uint32_t dwYSize;
std::uint32_t dwXCountChars;
std::uint32_t dwYCountChars;
std::uint32_t dwFillAttribute;
std::uint32_t dwFlags;
std::uint16_t wShowWindow;
std::uint16_t cbReserved2;
int* lpReserved2;
void* hStdInput;
void* hStdOutput;
void* hStdError;
};
struct process_information
{
void* hProcess;
void* hThread;
std::uint32_t dwProcessId;
std::uint32_t dwThreadId;
};
/*
https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getfileinformationbyhandleex
*/
struct file_standard_info
{
std::int64_t AllocationSize;
std::int64_t EndOfFile;
std::uint32_t NumberOfLinks;
int DeletePending;
int Directory;
};

struct file_attribute_tag_info
{
std::uint32_t FileAttributes;
std::uint32_t ReparseTag;
};
enum class file_info_by_handle_class
{
FileBasicInfo,
FileStandardInfo,
FileNameInfo,
FileRenameInfo,
FileDispositionInfo,
FileAllocationInfo,
FileEndOfFileInfo,
FileStreamInfo,
FileCompressionInfo,
FileAttributeTagInfo,
FileIdBothDirectoryInfo,
FileIdBothDirectoryRestartInfo,
FileIoPriorityHintInfo,
FileRemoteProtocolInfo,
FileFullDirectoryInfo,
FileFullDirectoryRestartInfo,
FileStorageInfo,
FileAlignmentInfo,
FileIdInfo,
FileIdExtdDirectoryInfo,
FileIdExtdDirectoryRestartInfo,
FileDispositionInfoEx,
FileRenameInfoEx,
FileCaseSensitiveInfo,
FileNormalizedNameInfo,
MaximumFileInfoByHandleClass
};

struct filetime
{
std::uint32_t dwLowDateTime,dwHighDateTime;
};

struct by_handle_file_information
{
std::uint32_t    dwFileAttributes;
filetime	 ftCreationTime;
filetime	 ftLastAccessTime;
filetime	 ftLastWriteTime;
std::uint32_t    dwVolumeSerialNumber;
std::uint32_t    nFileSizeHigh;
std::uint32_t    nFileSizeLow;
std::uint32_t    nNumberOfLinks;
std::uint32_t    nFileIndexHigh;
std::uint32_t    nFileIndexLow;
};

struct coord
{
	std::int16_t X, Y;
};

struct small_rect
{
	std::int16_t Left, Top, Right, Bottom;
};

struct char_info
{
	char16_t character;
	std::uint16_t Attrib;
};

struct console_screen_buffer_info
{
	coord Size, CursorPosition;
	std::uint16_t Attrib;
	small_rect Window;
	coord MaxWindowSize;
};

}