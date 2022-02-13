#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#include<windows.h>
#define X(w, e) {ERROR_##w, #e }
#include<cstdint>
#include<cerrno>
#include<vector>
#include<map>
#include<string_view>
#include<algorithm>

struct errmap_t
{
	std::uint_least32_t win32_error{};
	char const* posix_code_str{};
};

inline constexpr errmap_t errmap[]
{

  /* FIXME: Some of these choices are arbitrary! */
  X (ACCESS_DENIED,		EACCES),
  X (ACTIVE_CONNECTIONS,	EAGAIN),
  X (ALREADY_EXISTS,		EEXIST),
  X (BAD_DEVICE,		ENODEV),
  X (BAD_EXE_FORMAT,		ENOEXEC),
  X (BAD_NETPATH,		ENOENT),
  X (BAD_NET_NAME,		ENOENT),
  X (BAD_NET_RESP,		ENOSYS),
  X (BAD_PATHNAME,		ENOENT),
  X (BAD_PIPE,			EINVAL),
  X (BAD_UNIT,			ENODEV),
  X (BAD_USERNAME,		EINVAL),
  X (BEGINNING_OF_MEDIA,	EIO),
  X (BROKEN_PIPE,		EPIPE),
  X (BUSY,			EBUSY),
  X (BUS_RESET,			EIO),
  X (CALL_NOT_IMPLEMENTED,	ENOSYS),
  X (CANCELLED,			EINTR),
  X (CANNOT_MAKE,		EPERM),
	{424, "EINVAL"},
  X (CHILD_NOT_COMPLETE,	EBUSY),
  X (COMMITMENT_LIMIT,		EAGAIN),
  X (CONNECTION_REFUSED,	ECONNREFUSED),
  X (CRC,			EIO),
  X (DEVICE_DOOR_OPEN,		EIO),
  X (DEVICE_IN_USE,		EAGAIN),
  X (DEVICE_REQUIRES_CLEANING,	EIO),
  X (DEV_NOT_EXIST,		ENOENT),
  X (DIRECTORY,			ENOTDIR),
  X (DIR_NOT_EMPTY,		ENOTEMPTY),
  X (DISK_CORRUPT,		EIO),
  X (DISK_FULL,			ENOSPC),
  X (DS_GENERIC_ERROR,		EIO),
  X (DUP_NAME,			ENOTUNIQ),
  X (EAS_DIDNT_FIT,		ENOSPC),
  X (EAS_NOT_SUPPORTED,		ENOTSUP),
  X (EA_LIST_INCONSISTENT,	EINVAL),
  X (EA_TABLE_FULL,		ENOSPC),
  X (END_OF_MEDIA,		ENOSPC),
  X (EOM_OVERFLOW,		EIO),
  X (EXE_MACHINE_TYPE_MISMATCH,	ENOEXEC),
  X (EXE_MARKED_INVALID,	ENOEXEC),
  X (FILEMARK_DETECTED,		EIO),
  X (FILENAME_EXCED_RANGE,	ENAMETOOLONG),
  X (FILE_CORRUPT,		EEXIST),
  X (FILE_EXISTS,		EEXIST),
  X (FILE_INVALID,		ENXIO),
  X (FILE_NOT_FOUND,		ENOENT),
  X (HANDLE_DISK_FULL,		ENOSPC),
  X (HANDLE_EOF,		ENODATA),
  X (INVALID_ADDRESS,		EINVAL),
  X (INVALID_AT_INTERRUPT_TIME,	EINTR),
  X (INVALID_BLOCK_LENGTH,	EIO),
  X (INVALID_DATA,		EINVAL),
  X (INVALID_DRIVE,		ENODEV),
  X (INVALID_EA_NAME,		EINVAL),
  X (INVALID_EXE_SIGNATURE,	ENOEXEC),
  X (INVALID_FUNCTION,		EBADRQC),
  X (INVALID_HANDLE,		EBADF),
  X (INVALID_NAME,		ENOENT),
  X (INVALID_PARAMETER,		EINVAL),
  X (INVALID_SIGNAL_NUMBER,	EINVAL),
  X (IOPL_NOT_ENABLED,		ENOEXEC),
  X (IO_DEVICE,			EIO),
  X (IO_INCOMPLETE,		EAGAIN),
  X (IO_PENDING,		EAGAIN),
  X (LOCK_VIOLATION,		EBUSY),
  X (MAX_THRDS_REACHED,		EAGAIN),
  X (META_EXPANSION_TOO_LONG,	EINVAL),
  X (MOD_NOT_FOUND,		ENOENT),
  X (MORE_DATA,			EMSGSIZE),
  X (NEGATIVE_SEEK,		EINVAL),
  X (NETNAME_DELETED,		ENOENT),
  X (NOACCESS,			EFAULT),
  X (NONE_MAPPED,		EINVAL),
  X (NONPAGED_SYSTEM_RESOURCES,	EAGAIN),
  X (NOT_CONNECTED,		ENOLINK),
  X (NOT_ENOUGH_MEMORY,		ENOMEM),
  X (NOT_ENOUGH_QUOTA,		EIO),
  X (NOT_OWNER,			EPERM),
  X (NOT_READY,			ENOMEDIUM),
  X (NOT_SAME_DEVICE,		EXDEV),
  X (NOT_SUPPORTED,		ENOSYS),
  X (NO_DATA,			EPIPE),
  X (NO_DATA_DETECTED,		EIO),
  X (NO_MEDIA_IN_DRIVE,		ENOMEDIUM),
  X (NO_MORE_FILES,		ENMFILE),
  X (NO_MORE_ITEMS,		ENMFILE),
  X (NO_MORE_SEARCH_HANDLES,	ENFILE),
  X (NO_PROC_SLOTS,		EAGAIN),
  X (NO_SIGNAL_SENT,		EIO),
  X (NO_SYSTEM_RESOURCES,	EFBIG),
  X (NO_TOKEN,			EINVAL),
  X (OPEN_FAILED,		EIO),
  X (OPEN_FILES,		EAGAIN),
  X (OUTOFMEMORY,		ENOMEM),
  X (PAGED_SYSTEM_RESOURCES,	EAGAIN),
  X (PAGEFILE_QUOTA,		EAGAIN),
  X (PATH_NOT_FOUND,		ENOENT),
  X (PIPE_BUSY,			EBUSY),
  X (PIPE_CONNECTED,		EBUSY),
  X (PIPE_LISTENING,		ECOMM),
  X (PIPE_NOT_CONNECTED,	ECOMM),
  X (POSSIBLE_DEADLOCK,		EDEADLOCK),
  X (PRIVILEGE_NOT_HELD,	EPERM),
  X (PROCESS_ABORTED,		EFAULT),
  X (PROC_NOT_FOUND,		ESRCH),
  X (REM_NOT_LIST,		ENONET),
  X (SECTOR_NOT_FOUND,		EINVAL),
  X (SEEK,			EINVAL),
  X (SERVICE_REQUEST_TIMEOUT,	EBUSY),
  X (SETMARK_DETECTED,		EIO),
  X (SHARING_BUFFER_EXCEEDED,	ENOLCK),
  X (SHARING_VIOLATION,		EBUSY),
  X (SIGNAL_PENDING,		EBUSY),
  X (SIGNAL_REFUSED,		EIO),
  X (SXS_CANT_GEN_ACTCTX,	ELIBBAD),
  X (THREAD_1_INACTIVE,		EINVAL),
  X (TIMEOUT,			EBUSY),
  X (TOO_MANY_LINKS,		EMLINK),
  X (TOO_MANY_OPEN_FILES,	EMFILE),
  X (UNEXP_NET_ERR,		EIO),
  X (WAIT_NO_CHILDREN,		ECHILD),
  X (WORKING_SET_QUOTA,		EAGAIN),
  X (WRITE_PROTECT,		EROFS)
};

#include<fast_io.h>
#include<fast_io_device.h>

int main()
{
	std::map<std::string_view,std::vector<std::size_t>> mp;
	for(auto const & e : errmap)
	{
		auto& p {mp[e.posix_code_str]};
		p.push_back(e.win32_error);
//		mp[e.posix_code].push_back(e.win32_error);
	}
	fast_io::obuf_file obf(u8"win32_error_to_errno.h");
	fast_io::obuf_file obf2(u8"errno_to_win32_error.h");
	print(obf,R"abc(#pragma once
case 0: return 0;
)abc");
	print(obf2,R"abc(#pragma once
case 0: return 0;
)abc");
	for(auto& e : mp)
	{
		print(obf,"#if defined(",e.first,")\n");
		std::ranges::sort(e.second);
		if(!e.second.empty())
		{
			for(auto const& e1 : e.second)
				print(obf,"case ",e1,":");
			print(obf,"return ",e.first,";\n#endif\n");

			print(obf2,"#if defined(",e.first,")\ncase ",e.first,": return ",e.second[0],";\n#endif\n");
		}
//		print(obf,u8"case ",e.win32_error,u8": return ",e.posix_code,u8";\n");
	}
//	print(obf,u8"default: return EINVAL;\n");
}