#pragma once

namespace fast_io::win32
{

using wndproc = std::intptr_t (__stdcall*)(void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept;

template<typename T>
struct basic_wndclass
{
std::uint32_t style{};
wndproc lpfnWndProc{};
int cbClsExtra{};
int cbWndExtra{}; 
void* hInstance{}; 
void* hIcon{};
void* hCursor{};
void* hbrBackground{};
T const* lpszMenuName{}; 
T const* lpszClassName{};
};

using u16wndclass = basic_wndclass<char16_t>;
using wndclass = basic_wndclass<char>;

[[gnu::dllimport,gnu::stdcall]]
extern std::uint16_t RegisterClassW(u16wndclass const*) noexcept __asm__("RegisterClassW");

[[gnu::dllimport,gnu::stdcall]]
extern std::uint16_t RegisterClassA(wndclass const*) noexcept __asm__("RegisterClassA");

[[gnu::dllimport,gnu::stdcall]]
extern int DestroyWindow(void*) noexcept __asm__("DestroyWindow");

[[gnu::dllimport,gnu::stdcall]]
extern void* CreateWindowExW(std::uint32_t,char16_t const*,char16_t const*,std::uint32_t,int,int,int,int,void*,void*,void*,void*) noexcept __asm__("CreateWindowExW");

[[gnu::dllimport,gnu::stdcall]]
extern void* CreateWindowExA(std::uint32_t,char16_t const*,char16_t const*,std::uint32_t,int,int,int,int,void*,void*,void*,void*) noexcept __asm__("CreateWindowExA");

[[gnu::dllimport,gnu::stdcall]]
extern std::intptr_t DefWindowProcW(void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept __asm__("DefWindowProcW");

[[gnu::dllimport,gnu::stdcall]]
extern std::intptr_t DefWindowProcA(void*,std::uint32_t,std::uintptr_t,std::intptr_t) noexcept __asm__("DefWindowProcA");

[[gnu::dllimport,gnu::stdcall]]
extern int SetWindowTextW(void*,void const*) noexcept __asm__("SetWindowTextW");

[[gnu::dllimport,gnu::stdcall]]
extern int SetWindowTextA(void*,void const*) noexcept __asm__("SetWindowTextA");

[[gnu::dllimport,gnu::stdcall]]
extern void* SetParent(void*,void*) noexcept __asm__("SetParent");

[[gnu::dllimport,gnu::stdcall]]
extern int ShowWindow(void*,int) noexcept __asm__("ShowWindow");

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

}
