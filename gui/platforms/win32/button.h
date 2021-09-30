#pragma once

namespace fast_io::win32_gdi
{

enum class button_styles:std::uint_least32_t
{
text=0,
defpushbutton=1,
checkbox=2,
autocheckbox=3,
radiobutton=4,
threestate=5,
auto3state=6,
groupbox=7,
userbutton=8,
autoradiobutton=9,
ownerdraw=11,
typemask=15,
rightbutton=32,
icon=64,
bitmap=128,
left=256,
right=512,
top=1024,
bottom=2048,
vcenter=3072,
pushlike=4096,
multiline=8192,
notify=16384,
flat=32768,
};

class button:public window
{
public:
	explicit button(context& ctx,window& w):
		window(ctx,0,u"Button",u"",
			static_cast<std::uint32_t>(
			static_cast<std::uint32_t>(window_styles::tabstop|window_styles::visible|window_styles::child)|
			static_cast<std::uint32_t>(button_styles::defpushbutton)),10,10,100,100,
			w.hwnd,nullptr,ctx.wc.hInstance,nullptr)
	{}
};

}
