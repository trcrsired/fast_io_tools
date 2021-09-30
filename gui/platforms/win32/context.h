#pragma once

namespace fast_io::win32_gdi
{

enum class action:std::uint_least32_t
{
keyboard_key_down=0x0100,
keyboard_key_up,
keyboard_char,
keyboard_dead_char,
keyboard_sys_keydown,
keyboard_sys_keyup,
keyboard_sys_char,
keyboard_sys_deadchar,
keyboard_sys_keylast,
keyboard_unicode_char,
keyboard_ime_start_composition,
keyboard_ime_end_composition,
keyboard_ime_composition,
keyboard_imekeylast,

mouse_left_button_down=0x0201,
mouse_left_button_up,
mouse_left_button_double_click,
mouse_right_button_down,
mouse_right_button_up,
mouse_right_button_double_click,
mouse_middle_button_down,
mouse_middle_button_up,
mouse_middle_button_double_click,
mouse_wheel,
mouse_extend_button_down,
mouse_extend_button_up,
mouse_extend_button_double_click
};

inline constexpr bool is_mouse(action act) noexcept
{
	return static_cast<std::uint_least32_t>(static_cast<std::uint_least32_t>(act)-0x0201)<=static_cast<std::uint_least32_t>(12u);
}

inline constexpr bool is_keyboard(action act) noexcept
{
	return static_cast<std::uint_least32_t>(static_cast<std::uint_least32_t>(act)-0x0100)<=static_cast<std::uint_least32_t>(14u);
}

inline constexpr bool is_pen(action) noexcept
{
	return false;
}

inline constexpr bool is_touch(action) noexcept
{
	return false;
}

struct message
{
	using native_handle_type = ::fast_io::win32::msg*;
	native_handle_type msg{};
	constexpr ::fast_io::win32_gdi::action action() noexcept
	{
		return static_cast<::fast_io::win32_gdi::action>(msg->message);
	}
	constexpr char32_t key() noexcept
	{
		return static_cast<char32_t>(msg->wparam);
	}
	constexpr std::int_least32_t x() const noexcept
	{
		return msg->pt.x;
	};
	constexpr std::int_least32_t y() const noexcept
	{
		return msg->pt.y;
	};
};

struct context
{
	::fast_io::win32::u16wndclass wc{
	.lpfnWndProc=[](void* hwnd,std::uint32_t msg,std::uintptr_t wparam,std::intptr_t lparam) noexcept -> std::intptr_t
	{
//		debug_println(std::source_location::current()," hwnd:",hwnd," msg:",msg," wparam:",wparam," lparam:",lparam);
		switch(msg)
		{
		case 2://WM_DESTROY
		{
			::fast_io::win32::PostQuitMessage(0);
			return 0;
		}
		case 3:
		{

		}
		default:
		return ::fast_io::win32::DefWindowProcW(hwnd,msg,wparam,lparam);
		}
	},
	.hInstance=::fast_io::win32::GetProcessInstanceHandle(),
	.lpszClassName=u"fast_io"
	};
	using multimap_type = std::unordered_multimap<std::uintptr_t,std::function<void(message)>>;
	multimap_type multimap;
	explicit context()
	{
		if(!::fast_io::win32::RegisterClassW(__builtin_addressof(wc)))
			fast_io::throw_win32_error();
	}
	void loop_event()
	{
		for(::fast_io::win32::msg msg{};;)
		{
			int val{::fast_io::win32::GetMessageW(__builtin_addressof(msg),nullptr,0,0)};
			if(val==-1)
				::fast_io::throw_win32_error();
			else if(val==0)
				break;
			auto hwnd{msg.hwnd};
			std::uintptr_t window_pointer{__builtin_bit_cast(std::uintptr_t,hwnd)};
			for(auto [first,last] = multimap.equal_range(window_pointer);first!=last;++first)
				first->second(message{__builtin_addressof(msg)});
			::fast_io::win32::TranslateMessage(__builtin_addressof(msg));
			::fast_io::win32::DispatchMessageW(__builtin_addressof(msg));
		}
	}
};

}
