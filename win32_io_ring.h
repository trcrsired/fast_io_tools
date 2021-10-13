#pragma once

namespace fast_io::win32
{

struct ioring_create_flags
{
	std::uint32_t required{};
	std::uint32_t advisory{};
};

[[gnu::dllimport]]
std::uint32_t __stdcall CreateIoRing(std::uint32_t ioringVersion,ioring_create_flags flags,std::uint32_t submissionQueueSize,std::uint32_t completionQueueSize,void**) noexcept;

[[gnu::dllimport]]
std::uint32_t __stdcall CloseIoRing(void*) noexcept;

}



namespace fast_io
{

namespace details
{

inline void throw_ring_hresult_error(std::uint32_t hres)
{
	std::uint32_t ntstatus{0xC000000D};
	if((hres&0x10000000)==0x10000000)[[likely]]
		ntstatus=(hres&~0x10000000);
	throw_nt_error(ntstatus);
}

inline void* create_win32_io_ring()
{
	void* h{};
	std::uint32_t hres{::fast_io::win32::CreateIoRing(1,{},16,16,__builtin_addressof(h))};
	if(hres)
		throw_ring_hresult_error(hres);
	return h;
}

}

struct win32_io_ring_observer
{
	using native_handle_type = void*;
	native_handle_type h{};
};

inline void ring_submit_read(win32_io_ring_observer wirob,nt_at_entry ent,void* buffer,std::size_t buffer_size,void* userdata)
{
	
}

struct ring_default_setting_t
{
	explicit inline constexpr ring_default_setting_t() noexcept = default;
};

inline constexpr ring_default_setting_t ring_default_setting{};

class win32_io_ring_file:private win32_io_ring_observer
{
public:
	using native_handle_type = typename win32_io_ring_observer::native_handle_type;
	using win32_io_ring_observer::h;
	constexpr win32_io_ring_file() noexcept = default;
	explicit constexpr win32_io_ring_file(native_handle_type h) noexcept: win32_io_ring_observer{h}{}
	explicit constexpr win32_io_ring_file(decltype(nullptr)) noexcept=delete;
	explicit constexpr win32_io_ring_file(ring_default_setting_t):win32_io_ring_file(::fast_io::details::create_win32_io_ring()){}
	win32_io_ring_file(win32_io_ring_file const&) = delete;
	win32_io_ring_file& operator=(win32_io_ring_file const&) = delete;
	void close()
	{
		if(this->h)[[likely]]
		{
			std::uint32_t res{::fast_io::win32::CloseIoRing(this->h)};
			this->h=nullptr;
			if(res)[[unlikely]]
				::fast_io::details::throw_ring_hresult_error(res);
		}
	}
	~win32_io_ring_file()
	{
		if(this->h)[[likely]]
			::fast_io::win32::CloseIoRing(this->h);
	}
};

}
