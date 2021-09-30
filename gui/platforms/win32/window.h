#pragma once

namespace fast_io::win32_gdi
{

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

constexpr window_styles operator&(window_styles x, window_styles y) noexcept
{
using utype = typename std::underlying_type<window_styles>::type;
return static_cast<window_styles>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr window_styles operator|(window_styles x, window_styles y) noexcept
{
using utype = typename std::underlying_type<window_styles>::type;
return static_cast<window_styles>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr window_styles operator^(window_styles x, window_styles y) noexcept
{
using utype = typename std::underlying_type<window_styles>::type;
return static_cast<window_styles>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr window_styles operator~(window_styles x) noexcept
{
using utype = typename std::underlying_type<window_styles>::type;
return static_cast<window_styles>(~static_cast<utype>(x));
}

inline constexpr window_styles& operator&=(window_styles& x, window_styles y) noexcept{return x=x&y;}

inline constexpr window_styles& operator|=(window_styles& x, window_styles y) noexcept{return x=x|y;}

inline constexpr window_styles& operator^=(window_styles& x, window_styles y) noexcept{return x=x^y;}

namespace details
{

inline void set_window_text_common(void* hwnd,char16_t const* p)
{
	if(!::fast_io::win32::SetWindowTextW(hwnd,p))
		throw_win32_error();
}

#if 0
template<constructible_to_path path>
inline void set_window_text_impl(void* hwmd,path&& p)
{

}
#endif

inline void set_parent_impl(void* hwndchild,void* hwndnewparent)
{
	if(!::fast_io::win32::SetParent(hwndchild,hwndnewparent))
		throw_win32_error();
}
#if 0
inline void close_window(void* hwnd)
{
	if(!::fast_io::win32::CloseWindow(hwnd))
		throw_win32_error();
}
#endif
template<int cmd>
inline bool show_window_impl(void* hwnd) noexcept
{
	return ::fast_io::win32::ShowWindow(hwnd,cmd);
}

}

inline constexpr int cw_usedefault{static_cast<int>(static_cast<std::uint32_t>(0x80000000))};

class window
{
public:
	using char_type = char16_t;
	using native_handle_type = void*;
	native_handle_type hwnd{};
	window(std::uint32_t dxExStyle,char16_t const* lpclassname,char16_t const* lpwindowname,
		std::uint32_t dwstyle,int x,int y,int nwidth,int nheight,void* hwndparent,void* hmenu,void* hinstance,void* lpParam):
		hwnd{::fast_io::win32::CreateWindowExW(dxExStyle,lpclassname,lpwindowname,dwstyle,
		x,y,nwidth,nheight,hwndparent,hmenu,hinstance,lpParam)}
	{
		if(hwnd==nullptr)
			::fast_io::throw_win32_error();
	}
	explicit window(context& ctx):window(0,ctx.wc.lpszClassName,u"",
		static_cast<std::uint32_t>(window_styles::overlappedwindow),
		cw_usedefault,
		cw_usedefault,
		cw_usedefault,
		cw_usedefault,
		nullptr,nullptr,ctx.wc.hInstance,nullptr)
	{}
	constexpr native_handle_type release() noexcept
	{
		auto temp{hwnd};
		hwnd=nullptr;
		return temp;
	}
	window(window const&)=delete;
	window& operator=(window const&)=delete;
	window(window&& other) noexcept:hwnd{other.hwnd}
	{
		other.hwnd=nullptr;
	}
	window& operator=(window&& other) noexcept
	{
		if(__builtin_addressof(other)==this)[[unlikely]]
			return *this;
		if(this->hwnd)[[likely]]
			::fast_io::win32::DestroyWindow(this->hwnd);
		this->hwnd=other.hwnd;
		return *this;
	}
	void destroy()
	{
		if(this->hwnd)[[likely]]
		{
			int ret{::fast_io::win32::DestroyWindow(this->hwnd)};
			this->hwnd=nullptr;
			if(!ret)[[unlikely]]
				throw_win32_error();
		}
	}
	template<typename T>
	requires ::fast_io::win32_gdi::details::constructible_to_win32_gdi_path<T>
	void set_title(T&& t)	//to fix
	{
		::fast_io::win32_gdi::details::to_gdi_path<win32_family::wide_nt>(t,
		[this](char16_t const* p)
		{
			::fast_io::win32_gdi::details::set_window_text_common(this->hwnd,p);
		});
	}
	void set_parent(window const& w)
	{
		::fast_io::win32_gdi::details::set_parent_impl(this->hwnd,w.hwnd);
	}
#if 0
	void minimize()
	{
		::fast_io::win32_gdi::details::close_window(this->hwnd);
	}
#endif
	bool show() noexcept
	{
		return ::fast_io::win32_gdi::details::show_window_impl<5>(this->hwnd);
	}
	bool hide() noexcept
	{
		return ::fast_io::win32_gdi::details::show_window_impl<0>(this->hwnd);
	}
	~window()
	{
		if(this->hwnd)[[likely]]
			::fast_io::win32::DestroyWindow(this->hwnd);
	}
};

}
