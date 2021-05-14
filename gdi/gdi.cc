#include<fast_io.h>
#include<fast_io_device.h>

namespace fast_io::win32
{

using wndproc = std::intptr_t __stdcall(*)(void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept;

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

extern [[gnu::dllimport,gnu::stdcall]] std::uint16_t RegisterClassW()

}

extern "C" [[gnu::stdcall]] int WinMain(void* hinstance,void* hprevinstance,wchar_t* lpcmdline,int ncmdshow) noexcept
{
	fast_io::win32::wndclass wc{
	.lpfnWndProc=[](void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept -> std::intptr_t
	{

	},
	.hInstance=hinstance,
	.lpszClassName=u"开心就好"
	};
	
	return 0;
}
