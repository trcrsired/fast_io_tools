#pragma once

namespace fast_io::win32_gdi
{

namespace details
{

inline void win32_gdi_loop_event()
{
	for(::fast_io::win32::msg msg{};;)
	{
		int val{::fast_io::win32::GetMessageW(__builtin_addressof(msg),nullptr,0,0)};
		if(val==-1)
			::fast_io::throw_win32_error();
		else if(val==0)
			break;
		::fast_io::win32::TranslateMessage(__builtin_addressof(msg));
		::fast_io::win32::DispatchMessageW(__builtin_addressof(msg));
	}
}

}

struct context
{
	::fast_io::win32::u16wndclass wc{
	.lpfnWndProc=[](void* hwnd,std::uint32_t msg,std::uintptr_t wparam,std::intptr_t lparam) noexcept -> std::intptr_t
	{
//		debug_println(std::source_location::current()," hwnd:",hwnd," msg:",msg," wparam:",wparam," lparam:",lparam);
		switch(msg)
		{
		case 2://WM_DESTROY
			::fast_io::win32::PostQuitMessage(0);
			return 0;
		default:
		return ::fast_io::win32::DefWindowProcW(hwnd,msg,wparam,lparam);
		}
	},
	.hInstance=::fast_io::win32::GetProcessInstanceHandle(),
	.lpszClassName=u"fast_io"
	};
	explicit context()
	{
		if(!::fast_io::win32::RegisterClassW(__builtin_addressof(wc)))
			fast_io::throw_win32_error();
	}
	void loop_event()
	{
		::fast_io::win32_gdi::details::win32_gdi_loop_event();
	}
};

}
