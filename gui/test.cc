#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include"platforms/impl.h"

int main()
try
{
	using namespace fast_io::native_ui;
	context ctx;
	window w{ctx};
	w.set_title(U"我是好人");
	button b{ctx,w};
	b.set_title(u8"按钮");
	w.show();
	b.add_callback([&](message m)
	{
		auto e{m.action()};
		if(is_mouse(e))
		{
			if(e==action::mouse_left_button_down)
			{
				b.set_title("按钮按了");
			}
			else if(e==action::mouse_left_button_up)
			{
				b.set_title("按钮");
			}
		}
		else if(is_keyboard(e))
		{
			w.set_title(fast_io::u32concat(U"您刚才按了",fast_io::mnp::chvw(m.key()),U"键"));
		}
	});
	ctx.loop_event();
}
catch(fast_io::win32_error e)
{
	perrln(fast_io::u16box(),e);
	return 1;
}