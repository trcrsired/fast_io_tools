#pragma once
namespace fast_io::win32::nt::details
{
#if 0
struct file_disposition_information
{
	char unsigned delete_file{};
};

struct file_disposition_information_ex
{
	std::uint32_t flags{};
};
#endif

inline constexpr nt_open_mode calculate_nt_delete_flag(nt_at_flags flags) noexcept
{
	nt_open_mode mode{
	.DesiredAccess=0x00010000,	//FILE_GENERIC_READ
	.FileAttributes=0x80,		//FILE_READ_ATTRIBUTES
	.ShareAccess=0x00000007,	//FILE_SHARE_DELETE
	.CreateDisposition=0x00000001, /*OPEN_EXISTING	=>	FILE_OPEN*/
	.CreateOptions=0x00001000		/*FILE_DELETE_ON_CLOSE*/
	};
	if((flags&nt_at_flags::symlink_nofollow)!=nt_at_flags::symlink_nofollow)
	{
		mode.CreateOptions |= 0x00200000;//FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}
	if((flags&nt_at_flags::removedir)==nt_at_flags::removedir)
	{
		mode.CreateOptions |= 0x00004000;		//FILE_OPEN_FOR_BACKUP_INTENT
		mode.CreateOptions |= 0x00000001;		//FILE_DIRECTORY_FILE
	}
	else
	{
		mode.CreateOptions |= 0x00000040;		//FILE_NON_DIRECTORY_FILE 0x00000040
	}
	return mode;
}

template<bool zw>
inline void nt_unlinkat_impl(void* dirhd,wchar_t const* path_c_str,std::size_t path_size,nt_at_flags flags);
#if 0
{
#if 0
	if((flags&nt_at_flags::empty_path)==nt_at_flags::empty_path && path_size==0)
	{
		io_status_block block;
		file_disposition_information_ex dispositioninformationex{.flags=
/*FILE_DISPOSITION_DELETE|FILE_DISPOSITION_POSIX_SEMANTICS*/
		0x00000001|0x00000002};
		auto status{nt_set_information_file<zw>(dirhd,
		__builtin_addressof(block),
		__builtin_addressof(dispositioninformationex),
		static_cast<std::uint32_t>(sizeof(dispositioninformationex)),
		file_information_class::FileDispositionInformationEx)};
		if(status)
			throw_nt_error(status);
	}
	else
	{
#endif
		auto status{nt_close<zw>(nt_call_callback(dirhd,path_c_str,path_size,nt_create_callback<zw>{calculate_nt_delete_flag(flags)}))};
		if(status)
			throw_nt_error(status);
#if 0
	}
#endif
}