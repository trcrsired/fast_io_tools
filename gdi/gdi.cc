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



	menu menu;
	append_menu(menu.hmenu,0,0,L"New");
	append_menu(menu.hmenu,0,1,L"New");
	append_menu(menu.hmenu,0,2,L"New");
	append_menu(menu.hmenu,0,2,L"New");
	append_menu(menu.hmenu,0,2,L"New");
	append_menu(menu.hmenu,0,2,L"New");
	append_menu(menu.hmenu,0,2,L"New");
	append_menu(menu.hmenu,0,2,L"New");
	set_menu(window.hwnd,menu.hmenu);



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
catch(fast_io::error ec)
{
	perrln(ec);
	return 1;
}
#endif
