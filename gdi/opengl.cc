#include<fast_io.h>
#include<fast_io_device.h>
#include"win32gdi.h"

/*
https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
*/

int main()
#ifdef __cpp_exceptions
try
#endif
{
	using namespace fast_io::win32;
	fast_io::win32::wndclass wc{
	.lpfnWndProc=[](void* hwnd,std::uint32_t msg,std::uintptr_t wparam,std::intptr_t lparam) noexcept -> std::intptr_t
	{
		switch(msg)
		{
		case 2://WM_DESTROY
			PostQuitMessage(0);
			return 0;
		default:
		return fast_io::win32::DefWindowProcW(hwnd,msg,wparam,lparam);
		}
	},
	.hInstance=fast_io::win32::GetProcessInstanceHandle(),
	.lpszClassName=u"OpenGL"
	};
	if(!fast_io::win32::RegisterClassW(__builtin_addressof(wc)))
		fast_io::throw_win32_error();
	fast_io::win32::win32_window_file window(0,u"OpenGL",u"OpenGL",
		static_cast<std::uint32_t>(fast_io::win32::window_styles::overlappedwindow),
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		nullptr,nullptr,fast_io::win32::GetProcessInstanceHandle(),nullptr);
	
	auto window_dc{get_dc(window)};
	choose_opengl_pixel_format(window_dc);
	fast_io::win32::win32_glrc gl{window_dc};
	wgl_make_current(window_dc,gl);

	
	fast_io::win32::ShowWindow(window.hwnd,10);
	for(fast_io::win32::msg msg{};;)
	{
		int val{fast_io::win32::GetMessageW(__builtin_addressof(msg),nullptr,0,0)};
		if(val==-1)
			fast_io::throw_win32_error();
		else if(val==0)
			break;
		fast_io::win32::TranslateMessage(__builtin_addressof(msg));
		fast_io::win32::DispatchMessageW(__builtin_addressof(msg));
	}
}
#ifdef __cpp_exceptions
catch(fast_io::win32_error ec)
{
	perrln(ec);
	return 1;
}
#endif