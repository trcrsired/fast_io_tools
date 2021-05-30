#include<fast_io.h>
#include<fast_io_device.h>
#include"win32gdi.h"

/*
https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
*/

int main()
{
	fast_io::posix_tzset();
	fast_io::win32::wndclass wc{
	.lpfnWndProc=[](void* hwnd,std::uint32_t msg,std::uintptr_t wparam,std::intptr_t lparam) noexcept -> std::intptr_t
	{
		auto tsp{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
		println(local(tsp)," ",std::source_location::current()," hwnd:",hwnd," msg:",msg," wparam:",wparam," lparam:",lparam);
		return fast_io::win32::DefWindowProcW(hwnd,msg,wparam,lparam);
	},
	.hInstance=fast_io::win32::GetProcessInstanceHandle(),
	.lpszClassName=u"LOL🤣"
	};
	if(!fast_io::win32::RegisterClassW(__builtin_addressof(wc)))
		fast_io::throw_win32_error();
	fast_io::win32::win32_window_file window(0,u"LOL🤣",u"GDI🤣🤣🤣🤣",
		static_cast<std::uint32_t>(fast_io::win32::window_styles::overlappedwindow),
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		nullptr,nullptr,fast_io::win32::GetProcessInstanceHandle(),nullptr);
	fast_io::win32::ShowWindow(window.hwnd,10);
	for(fast_io::win32::msg msg{};;)
	{
		int val{fast_io::win32::GetMessageW(__builtin_addressof(msg),nullptr,0,0)};
		if(val==-1)
			fast_io::throw_win32_error();
		else if(val==0)
			break;
		fast_io::win32::TranslateMessage(__builtin_addressof(msg));
		auto tsp{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
		println(local(tsp)," ",std::source_location::current()," hwnd:",msg.hwnd," msg:",msg.message," wparam:",msg.wparam," lparam:",msg.lparam);
		fast_io::win32::DispatchMessageW(__builtin_addressof(msg));
	}
}
