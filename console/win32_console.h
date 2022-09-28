#pragma once

namespace fast_io
{

template<win32_family family,std::integral ch_type>
requires ((family==win32_family::wide_nt&&sizeof(ch_type) == 2) || (family!=win32_family::wide_nt&&sizeof(ch_type) == 1))
struct basic_win32_console_io_observer
{
	using char_type = ch_type;
	using native_handle_type = void*;
	void* handle{};
	constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	explicit constexpr operator bool() const noexcept
	{
		return handle!=nullptr;
	}
	template<nt_family family2>
	explicit constexpr operator basic_nt_family_io_observer<family2,char_type>() const noexcept
	{
		return basic_nt_family_io_observer<family2,char_type>{handle};
	}
	template<win32_family family2>
	explicit constexpr operator basic_win32_family_io_observer<family2,char_type>() const noexcept
	{
		return basic_win32_family_io_observer<family2,char_type>{handle};
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle=nullptr;
		return temp;
	}
};

template<win32_family family,std::integral ch_type>
requires ((family==win32_family::wide_nt&&sizeof(ch_type) == 2) || (family!=win32_family::wide_nt&&sizeof(ch_type) == 1))
inline constexpr basic_win32_console_io_observer<family,ch_type> io_value_handle(basic_win32_console_io_observer<family,ch_type> other) noexcept
{
	return other;
}

namespace win32::details
{

template<win32_family family>
inline std::size_t console_write_chars_impl(void* handle,void* first,std::size_t to_write)
{
	if constexpr(sizeof(std::uint_least32_t)<sizeof(std::size_t))
	{
		constexpr std::size_t mx{std::numeric_limits<std::uint_least32_t>::max()};
		if(to_write>mx)
			to_write=mx;
	}
	::std::uint_least32_t number_of_chars_written;
	if constexpr(family==win32_family::wide_nt)
	{
		if(!win32::WriteConsoleW(handle,first,static_cast<std::uint_least32_t>(to_write),__builtin_addressof(number_of_chars_written),nullptr))
		{
			throw_win32_error();
		}
	}
	else
	{
		if(!win32::WriteConsoleA(handle,first,static_cast<std::uint_least32_t>(to_write),__builtin_addressof(number_of_chars_written),nullptr))
		{
			throw_win32_error();
		}
	}
	return number_of_chars_written;
}

template<win32_family family>
inline std::size_t console_read_chars_impl(void* handle,void* first,std::size_t to_read)
{
	if constexpr(sizeof(std::uint_least32_t)<sizeof(std::size_t))
	{
		constexpr std::size_t mx{std::numeric_limits<std::uint_least32_t>::max()};
		if(to_read>mx)
			to_read=mx;
	}
	::std::uint_least32_t number_of_chars_read;
	if constexpr(family==win32_family::wide_nt)
	{
		if(!win32::ReadConsoleW(handle,first,static_cast<std::uint_least32_t>(to_read),__builtin_addressof(number_of_chars_read),nullptr))
		{
			throw_win32_error();
		}
	}
	else
	{
		if(!win32::ReadConsoleA(handle,first,static_cast<std::uint_least32_t>(to_read),__builtin_addressof(number_of_chars_read),nullptr))
		{
			throw_win32_error();
		}
	}
	return number_of_chars_read;
}

template<win32_family family>
inline std::size_t console_write_chars_stdout_impl(void* first,std::size_t to_write)
{
	return console_write_chars_impl<family>(fast_io::win32::GetStdHandle(win32_stdout_number),first,to_write);
}

template<win32_family family>
inline std::size_t console_read_chars_stdin_impl(void* first,std::size_t to_read)
{
	return console_read_chars_impl<family>(fast_io::win32::GetStdHandle(win32_stdin_number),first,to_read);
}

}

template<win32_family family,std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<ch_type,::fast_io::freestanding::iter_value_t<Iter>>)
[[nodiscard]] inline Iter read(basic_win32_console_io_observer<family,ch_type> handle,Iter begin,Iter end)
{
	return begin+win32::details::console_read_chars_impl(handle.handle,::fast_io::freestanding::to_address(begin),static_cast<std::size_t>(end-begin));
}

template<win32_family family,std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<ch_type,::fast_io::freestanding::iter_value_t<Iter>>)
inline Iter write(basic_win32_console_io_observer<ch_type> handle,Iter cbegin,Iter cend)
{
	return cbegin+win32::details::console_write_chars_impl(handle.handle,::fast_io::freestanding::to_address(cbegin),static_cast<std::size_t>(cend-cbegin));
}

using u16win32_console_io_observer_ntw = basic_win32_console_io_observer<win32_family::wide_nt,char16_t>;

inline u16win32_console_io_observer_ntw u16win32_console_stdin_ntw() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdin_number)};
}

inline u16win32_console_io_observer_ntw u16win32_console_stdout_ntw() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdout_number)};
}

inline u16win32_console_io_observer_ntw u16win32_console_stderr_ntw() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stderr_number)};
}

template<win32_family family,std::integral ch_type>
requires (family==win32_family::wide_nt&&sizeof(ch_type) == 2 || family!=win32_family::wide_nt&&sizeof(ch_type) == 1)
struct basic_win32_console_stdio
{
	using char_type = ch_type;
};

template<win32_family family,std::integral ch_type>
inline constexpr basic_win32_console_stdio<family,ch_type> io_value_handle(basic_win32_console_stdio<family,ch_type> other) noexcept
{
	return other;
}

template<std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<ch_type,::fast_io::freestanding::iter_value_t<Iter>>)
[[nodiscard]] inline Iter read(basic_win32_console_stdio<ch_type> handle,Iter begin,Iter end)
{
	return begin+win32::details::console_read_chars_stdin_impl(::fast_io::freestanding::to_address(begin),static_cast<std::size_t>(end-begin));
}

template<std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<ch_type,::fast_io::freestanding::iter_value_t<Iter>>)
inline Iter write(basic_win32_console_stdio<ch_type> handle,Iter cbegin,Iter cend)
{
	return cbegin+win32::details::console_write_chars_stdout_impl(::fast_io::freestanding::to_address(cbegin),static_cast<std::size_t>(cend-cbegin));
}

#if !defined(__CYGWIN__)

using native_console_file = std::conditional_t<win32_family::native==win32_family::ansi_9x,
	u8win32_io_observer,
	u16win32_console_io_observer_ntw>;

inline native_console_file native_console_stdin() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdin_number)};
}

inline native_console_file native_console_stdout() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stdout_number)};
}

inline native_console_file native_console_stderr() noexcept
{
	return {fast_io::win32::GetStdHandle(win32_stderr_number)};
}

#endif

}
