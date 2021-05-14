#include<fast_io.h>
#include<fast_io_device.h>

namespace fast_io::win32
{

using wndproc = std::intptr_t (__stdcall*)(void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept;

struct wndclass
{
std::uint32_t style{}; 
wndproc lpfnWndProc{};
int cbClsExtra{};
int cbWndExtra{}; 
void* hInstance{}; 
void* hIcon{};
void* hCursor{};
void* hbrBackground{};
char16_t const* lpszMenuName{}; 
char16_t const* lpszClassName{};
};

[[gnu::dllimport,gnu::stdcall]]
extern std::uint16_t RegisterClassW(wndclass const*) noexcept __asm__("RegisterClassW");

[[gnu::dllimport,gnu::stdcall]]
extern int DestroyWindow(void*) noexcept __asm__("DestroyWindow");

[[gnu::dllimport,gnu::stdcall]]
extern void* CreateWindowExW(std::uint32_t,char16_t const*,char16_t const*,std::uint32_t,int,int,int,int,void*,void*,void*,void*) noexcept __asm__("CreateWindowExW");

/*
Referenced from https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
*/
enum class window_styles:std::uint32_t
{
border=0x00800000L,
caption=0x00C00000L,
child=0x40000000L,
childwindow=0x40000000L,
clipchildren=0x02000000L,
clipsiblings=0x04000000L,
disabled=0x08000000L,
dlgframe=0x00400000L,
group=0x00020000L,
hscroll=0x00100000L,
iconic=0x20000000L,
maximize=0x01000000L,
maximizebox=0x00010000L,
minimize=0x20000000L,
minimizebox=0x00020000L,
overlapped=0x00000000L,
popup=0x80000000L,
sizebox=0x00040000L,
sysmenu=0x00080000L,
tabstop=0x00010000L,
thickframe=0x00040000L,
visible=0x10000000L,
vscroll=0x00200000L,
overlappedwindow=overlapped|caption|sysmenu|thickframe|minimizebox|maximizebox
};

constexpr int cw_usedefault(0x80000000);

class win32_window_file
{
public:
	void *hwnd{};
	constexpr win32_window_file() noexcept = default;
	win32_window_file(std::uint32_t dxExStyle,char16_t const* lpclassname,char16_t const* lpwindowname,
		window_styles dwstyle,int x,int y,int nwidth,int nheight,void* hwndparent,void* hmenu,void* hinstance,void* lpParam):
		hwnd(CreateWindowExW(dxExStyle,lpclassname,lpwindowname,static_cast<std::uint32_t>(dwstyle),
		x,y,nwidth,nheight,hwndparent,hmenu,hinstance,lpParam))
	{
		if(hwnd==nullptr)
			throw_win32_error();
	}
	win32_window_file(win32_window_file const&)=delete;
	win32_window_file& operator=(win32_window_file const&)=delete;
	win32_window_file(win32_window_file&& other) noexcept:hwnd(other.hwnd)
	{
		other.hwnd=nullptr;
	}
	win32_window_file& operator=(win32_window_file&& other) noexcept
	{
		if(__builtin_addressof(other)==this)[[unlikely]]
			return *this;
		if(this->hwnd)[[likely]]
			DestroyWindow(this->hwnd);
		this->hwnd=other.hwnd;
		return *this;
	}
	~win32_window_file()
	{
		if(this->hwnd)[[likely]]
			DestroyWindow(this->hwnd);
	}
};

struct point
{
std::int32_t x,y;
};

struct msg
{
void* hwnd;
std::uint32_t message;
std::uintptr_t wparam;
std::intptr_t lparam;
std::uint32_t time;
point pt;
std::uint32_t lPrivate;
};

[[gnu::dllimport,gnu::stdcall]]
extern int GetMessageW(msg*,void*,std::uint32_t,std::uint32_t) noexcept __asm__("GetMessageW");

[[gnu::dllimport,gnu::stdcall]]
extern int TranslateMessage(msg const*) noexcept __asm__("TranslateMessage");

[[gnu::dllimport,gnu::stdcall]]
extern std::intptr_t DispatchMessageW(msg const*) noexcept __asm__("DispatchMessageW");

[[gnu::dllimport,gnu::stdcall]]
extern int ShowWindow(void*,int) noexcept __asm__("ShowWindow");
}

/*
https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
*/

extern "C" [[gnu::stdcall]] int WinMain(void* hinstance,void* hprevinstance,wchar_t* lpcmdline,int ncmdshow) noexcept
{
try
{
	fast_io::win32::wndclass wc{
	.lpfnWndProc=[](void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept -> std::intptr_t
	{
		return 0;
	},
	.hInstance=hinstance,
	.lpszClassName=u"LOL🤣"
	};
	if(!fast_io::win32::RegisterClassW(__builtin_addressof(wc)))
		fast_io::throw_win32_error();
	fast_io::win32::win32_window_file window(0,u"LOL🤣",u"GDI🤣🤣🤣🤣",fast_io::win32::window_styles::overlappedwindow,
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		fast_io::win32::cw_usedefault,
		nullptr,nullptr,hinstance,nullptr);
	if(!fast_io::win32::ShowWindow(window.hwnd,5))
		fast_io::throw_win32_error();
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
	return 0;
}
catch(fast_io::nt_error e)
{
	perrln(fast_io::u16box(),e);
	return 1;
}
catch(fast_io::win32_error e)
{
	perrln(fast_io::u16box(),e);
	return 1;
}
catch(fast_io::posix_error e)
{
	perrln(fast_io::box(),e);
	return 1;
}
catch(std::exception const& e)
{
	perrln(fast_io::box(),e);
	return 1;
}
}
