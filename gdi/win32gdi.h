#pragma once


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

[[gnu::dllimport,gnu::stdcall]]
extern std::intptr_t DefWindowProcW(void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept __asm__("DefWindowProcW");

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

inline constexpr int cw_usedefault(0x80000000);

class win32_window_file
{
public:
	void *hwnd{};
	constexpr win32_window_file() noexcept = default;
	win32_window_file(std::uint32_t dxExStyle,char16_t const* lpclassname,char16_t const* lpwindowname,
		std::uint32_t dwstyle,int x,int y,int nwidth,int nheight,void* hwndparent,void* hmenu,void* hinstance,void* lpParam):
		hwnd(CreateWindowExW(dxExStyle,lpclassname,lpwindowname,dwstyle,
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

[[gnu::dllimport,gnu::stdcall]]
extern int PostQuitMessage(int) noexcept __asm__("PostQuitMessage");

[[gnu::pure]]
inline void* GetProcessInstanceHandle() noexcept
{
	return GetModuleHandleW(nullptr);
}


[[gnu::dllimport,gnu::stdcall]]
extern void* CreateMenu() noexcept __asm__("CreateMenu");

[[gnu::dllimport,gnu::stdcall]]
extern int DestroyMenu(void*) noexcept __asm__("DestroyMenu");

[[gnu::dllimport,gnu::stdcall]]
extern int SetMenu(void*,void*) noexcept __asm__("SetMenu");

[[gnu::dllimport,gnu::stdcall]]
extern int AppendMenuW(void*,std::uint32_t,std::uintptr_t,wchar_t const*) noexcept __asm__("AppendMenuW");

[[gnu::dllimport,gnu::stdcall]]
extern int AppendMenuA(void*,std::uint32_t,std::uintptr_t,char const*) noexcept __asm__("AppendMenuA");


struct menu
{
	void* hmenu{};
	menu():hmenu(CreateMenu())
	{
		if(hmenu==nullptr)
			throw_win32_error();
	}
	menu(menu const&)=delete;
	menu& operator=(menu const&)=delete;
	~menu()
	{
		DestroyMenu(hmenu);
	}
};

inline void set_menu(void* hwnd,void* hmenu)
{
	if(!SetMenu(hwnd,hmenu))
		throw_win32_error();
}

inline void append_menu(void* hmenu,std::uint32_t uflags,std::uintptr_t uIDNewItem,wchar_t const* lpNewItem)
{
	if(!AppendMenuW(hmenu,uflags,uIDNewItem,lpNewItem))
		throw_win32_error();
}

}