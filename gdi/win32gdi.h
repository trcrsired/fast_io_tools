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

[[gnu::dllimport,gnu::stdcall]]
extern void* GetDC(void*) noexcept __asm__("GetDC");
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

struct pixel_format_descriptor
{
std::uint16_t  nSize;
std::uint16_t  nVersion;
std::uint32_t dwFlags;
std::uint8_t  iPixelType;
std::uint8_t  cColorBits;
std::uint8_t  cRedBits;
std::uint8_t  cRedShift;
std::uint8_t  cGreenBits;
std::uint8_t  cGreenShift;
std::uint8_t  cBlueBits;
std::uint8_t  cBlueShift;
std::uint8_t  cAlphaBits;
std::uint8_t  cAlphaShift;
std::uint8_t  cAccumBits;
std::uint8_t  cAccumRedBits;
std::uint8_t  cAccumGreenBits;
std::uint8_t  cAccumBlueBits;
std::uint8_t  cAccumAlphaBits;
std::uint8_t  cDepthBits;
std::uint8_t  cStencilBits;
std::uint8_t  cAuxBuffers;
std::uint8_t  iLayerType;
std::uint8_t  bReserved;
std::uint16_t dwLayerMask;
std::uint16_t dwVisibleMask;
std::uint16_t dwDamageMask;
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

struct win32_dc_observer
{
	void* hdc{};
};

namespace details
{
inline win32_dc_observer get_dc_throw(void* hwnd)
{
	if(hwnd==nullptr)
		throw_win32_error();
	return {GetDC(hwnd)};
}
}

inline win32_dc_observer get_dc(win32_window_file const& h)
{
	return details::get_dc_throw(h.hwnd);
}

[[gnu::dllimport,gnu::stdcall]]
extern void* wglCreateContext(void*) noexcept __asm__("wglCreateContext");

[[gnu::dllimport,gnu::stdcall]]
extern int wglDeleteContext(void*) noexcept __asm__("wglDeleteContext");

[[gnu::dllimport,gnu::stdcall]]
extern int wglCopyContext(void*,void*,std::uint32_t) noexcept __asm__("wglCopyContext");

[[gnu::dllimport,gnu::stdcall]]
extern int ChoosePixelFormat(void*,pixel_format_descriptor const*) noexcept __asm__("ChoosePixelFormat");


[[gnu::dllimport,gnu::stdcall]]
extern int SetPixelFormat(void*,int,pixel_format_descriptor const*) noexcept __asm__("SetPixelFormat");

[[gnu::dllimport,gnu::stdcall]]
extern int wglMakeCurrent(void*,void*) noexcept __asm__("wglMakeCurrent");

namespace details
{

inline void* dup_glrc_context(void* glrc)
{
	void* new_glrc{wglCreateContext(glrc)};
	if(new_glrc==nullptr)
		throw_win32_error();
	int ret{wglCopyContext(glrc,new_glrc,0x000FFFFF)};
	if(!ret)
	{
		wglDeleteContext(new_glrc);
		throw_win32_error();
	}
	return new_glrc;
}

}

struct win32_glrc
{
	void* glrc{};
	constexpr win32_glrc() noexcept = default;
	win32_glrc(win32_dc_observer parameter):glrc(wglCreateContext(parameter.hdc))
	{
		if(glrc==nullptr)[[unlikely]]
			throw_win32_error();
	}
	win32_glrc(::std::nullptr_t)=delete;
	win32_glrc(win32_glrc const& other):glrc{::fast_io::win32::details::dup_glrc_context(other.glrc)}{}
	win32_glrc& operator=(win32_glrc const& other)
	{
		auto temp{::fast_io::win32::details::dup_glrc_context(other.glrc)};
		if(glrc)
			wglDeleteContext(glrc);
		glrc=temp;
		return *this;
	}
	constexpr win32_glrc(win32_glrc&& other) noexcept:glrc{other.glrc}
	{
		other.glrc=nullptr;
	}
	win32_glrc& operator=(win32_glrc&& other) noexcept
	{
		if(__builtin_addressof(other)==this)
			return *this;
		if(glrc)
			wglDeleteContext(glrc);
		glrc=other.glrc;
		other.glrc=nullptr;
	}
	void close()
	{
		if(glrc)
		{
			int ret{wglDeleteContext(glrc)};
			glrc=nullptr;
			if(ret)
				throw_win32_error();
		}
	}
	~win32_glrc()
	{
		if(glrc)
			wglDeleteContext(glrc);
	}
};

namespace details
{

inline void wgl_make_current_impl(void* hdc,void* hglrc)
{
	if(!wglMakeCurrent(hdc,hglrc))
		throw_win32_error();
}

}

inline constexpr pixel_format_descriptor opengl_pixel_descriptor
{
sizeof(pixel_format_descriptor),  //  size of this pfd  
1,                     // version number  
4 |   // support window  
32 |   // support OpenGL  
1,      // double buffered  
0,         // RGBA type  
24,                    // 24-bit color depth  
0, 0, 0, 0, 0, 0,      // color bits ignored  
0,                     // no alpha buffer  
0,                     // shift bit ignored  
0,                     // no accumulation buffer  
0, 0, 0, 0,            // accum bits ignored  
32,                    // 32-bit z-buffer      
0,                     // no stencil buffer  
0,                     // no auxiliary buffer  
0,        // main layer  
0,                     // reserved  
0, 0, 0                // layer masks ignored  
};


inline void wgl_make_current(win32_dc_observer dc,win32_glrc const& w)
{
	::fast_io::win32::details::wgl_make_current_impl(dc.hdc,w.glrc);
}

inline auto choose_pixel_format(win32_dc_observer dc,pixel_format_descriptor const& ppfd)
{
	auto ret{ChoosePixelFormat(dc.hdc,__builtin_addressof(ppfd))};
	if(!ret)
		throw_win32_error();
	return ret;
}

inline void choose_opengl_pixel_format(win32_dc_observer dc)
{
	auto format{choose_pixel_format(dc,opengl_pixel_descriptor)};
	if(!SetPixelFormat(dc.hdc,format,__builtin_addressof(opengl_pixel_descriptor)))
		throw_win32_error();
}

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