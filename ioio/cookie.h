#pragma once

namespace fast_io
{

template<std::integral ch_type>
struct basic_io_cookie_vtable_t
{
	using char_type = ch_type;
	using buffer_pointer_funptr_t = char_type* (*)(std::uintptr_t) noexcept;
	using set_buffer_pointer_funptr_t = void (*)(std::uintptr_t,char_type*) noexcept;
	using read_funptr_t = char_type* (*)(std::uintptr_t,char_type*,char_type*);
	using ibuffer_underflow_funptr_t = bool (*)(std::uintptr_t);
	using write_funptr_t = void (*)(std::uintptr_t,char_type const*,char_type const*);
	using obuffer_overflow_funptr_t = void (*)(std::uintptr_t,char_type);
	using seek_funptr_t = std::uintmax_t (*)(std::uintptr_t,std::intmax_t,seekdir);
	using clone_funptr_t = std::uintptr_t (*)(std::uintptr_t);
	using close_funptr_t =  void (*)(std::uintptr_t) noexcept;

	buffer_pointer_funptr_t ibuffer_begin_funptr{},ibuffer_curr_funptr{},ibuffer_end_funptr{};
	set_buffer_pointer_funptr_t ibuffer_set_curr_funptr{};
	ibuffer_underflow_funptr_t ibuffer_underflow_funptr{};
	read_funptr_t read_funptr{};
	buffer_pointer_funptr_t obuffer_begin_funptr{},obuffer_curr_funptr{},obuffer_end_funptr{};
	set_buffer_pointer_funptr_t obuffer_set_curr_funptr{};
	obuffer_overflow_funptr_t obuffer_overflow_funptr{};
	write_funptr_t write_funptr{};
	seek_funptr_t seek_funptr{};
	clone_funptr_t clone_funptr{};
	close_funptr_t close_funptr{};
};

template<std::integral ch_type>
struct basic_io_cookie_object_t
{
	using char_type = ch_type;
	basic_io_cookie_vtable_t<ch_type> const* vptr{};
	std::uintptr_t cookie{};
};

namespace details
{

inline std::uintmax_t io_cookie_seek_no_impl(std::intmax_t,seekdir)
{
	throw_posix_error(EINVAL);
	return 0;
}

inline std::uintmax_t io_cookie_clone_impl(std::uintmax_t) noexcept
{
	throw_posix_error(EINVAL);
	return 0;
}

template<std::integral char_type>
inline constexpr char_type* invalid_cookie_bufferptr_impl(std::uintptr_t) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr void invalid_cookie_write_impl(std::uintptr_t,char_type const*,char_type const*)
{
	throw_posix_error(EINVAL);
}

template<std::integral char_type>
inline constexpr void invalid_cookie_obuffer_overflow_impl(std::uintptr_t,char_type)
{
	throw_posix_error(EINVAL);
}

template<std::integral char_type>
inline constexpr void invalid_cookie_buffer_set_curr_impl(std::uintptr_t,char_type*) noexcept{}

template<std::integral char_type>
inline constexpr char_type* invalid_cookie_read_impl(std::uintptr_t,char_type*,char_type*)
{
	throw_posix_error(EINVAL);
}

inline bool invalid_io_underflow_impl(std::uintmax_t)
{
	throw_posix_error(EINVAL);
	return false;
}


template<stream hd_type>
inline constexpr basic_io_cookie_vtable_t<typename hd_type::char_type> cal_io_cookie_vtable() noexcept
{
	using char_type = typename hd_type::char_type;
	basic_io_cookie_vtable_t<char_type> tb{};
	if constexpr(input_stream<hd_type>)
	{
		tb.read_funptr=[](std::uintptr_t h,char_type* first,char_type* last) -> char_type*
		{
			return read(*(hd_type*)h,first,last);
		};
	}
	else
	{
		tb.read_funptr=invalid_cookie_read_impl<char_type>;
	}
	if constexpr(buffer_input_stream<hd_type>)
	{
		tb.ibuffer_begin_funptr=[](std::uintptr_t h) noexcept ->char_type*
		{
			return ibuffer_begin(*(hd_type*)h);
		};
		tb.ibuffer_curr_funptr=[](std::uintptr_t h) noexcept ->char_type*
		{
			return ibuffer_curr(*(hd_type*)h);
		};
		tb.ibuffer_end_funptr=[](std::uintptr_t h) noexcept ->char_type*
		{
			return ibuffer_end(*(hd_type*)h);
		};
		tb.ibuffer_set_curr_funptr=[](std::uintptr_t h,char_type* p) noexcept
		{
			ibuffer_set_curr(*(hd_type*)h,p);
		};
		tb.ibuffer_underflow_funptr=[](std::uintptr_t h) ->bool
		{
			return ibuffer_underflow(*(hd_type*)h);
		};
	}
	else
	{
		tb.ibuffer_begin_funptr=invalid_cookie_bufferptr_impl<char_type>;
		tb.ibuffer_curr_funptr=invalid_cookie_bufferptr_impl<char_type>;
		tb.ibuffer_end_funptr=invalid_cookie_bufferptr_impl<char_type>;
		tb.ibuffer_set_curr_funptr=invalid_cookie_buffer_set_curr_impl<char_type>;
		tb.ibuffer_underflow_funptr=invalid_io_underflow_impl;
	}

	if constexpr(output_stream<hd_type>)
	{
		tb.write_funptr=[](std::uintptr_t h,char_type const* first,char_type const* last)
		{
			write(*(hd_type*)h,first,last);
		};
	}
	else
		tb.write_funptr=invalid_cookie_write_impl;
	if constexpr(buffer_output_stream<hd_type>)
	{
		tb.obuffer_begin_funptr=[](std::uintptr_t h) noexcept ->char_type*
		{
			return obuffer_begin(*(hd_type*)h);
		};
		tb.obuffer_curr_funptr=[](std::uintptr_t h) noexcept ->char_type*
		{
			return obuffer_curr(*(hd_type*)h);
		};
		tb.obuffer_end_funptr=[](std::uintptr_t h) noexcept ->char_type*
		{
			return obuffer_end(*(hd_type*)h);
		};
		tb.obuffer_set_curr_funptr=[](std::uintptr_t h,char_type* p) noexcept
		{
			obuffer_set_curr(*(hd_type*)h,p);
		};
		tb.obuffer_overflow_funptr=[](std::uintptr_t h,char_type ch) -> void
		{
			obuffer_overflow(*(hd_type*)h,ch);
		};
	}
	else
	{
		tb.obuffer_begin_funptr=invalid_cookie_bufferptr_impl<char_type>;
		tb.obuffer_curr_funptr=invalid_cookie_bufferptr_impl<char_type>;
		tb.obuffer_end_funptr=invalid_cookie_bufferptr_impl<char_type>;
		tb.obuffer_set_curr_funptr=invalid_cookie_buffer_set_curr_impl<char_type>;
		if constexpr(output_stream<hd_type>)
		{
			tb.obuffer_overflow_funptr=[](std::uintptr_t h,char_type ch)
			{
				write(*(hd_type*)h,__builtin_addressof(ch),__builtin_addressof(ch)+1);
			};
		}
		else
		{
			tb.obuffer_overflow_funptr=invalid_cookie_obuffer_overflow_impl<char_type>;
		}
	}

	if constexpr(random_access_stream<hd_type>)
		tb.seek_funptr=[](std::uintptr_t h,std::intmax_t offset,seekdir s)
		{
			return seek(*(hd_type*)h,offset,s);
		};
	else
		tb.seek_funptr=io_cookie_seek_no_impl;
	if constexpr(std::is_copy_constructible_v<hd_type>)
	{
		tb.clone_funptr=[](std::uintptr_t h)->std::uintptr_t
		{
			return (std::uintptr_t)(new hd_type(*(hd_type*)h));
		};
	}
	else
		tb.clone_funptr=io_cookie_clone_impl;
	tb.close_funptr=[](std::uintptr_t h) noexcept
	{
		delete (hd_type*)h;
	};
	return tb;
}
}

template<stream hd_type>
inline constexpr basic_io_cookie_vtable_t<typename hd_type::char_type> io_cookie_vtable{details::cal_io_cookie_vtable<hd_type>()};

template<std::integral ch_type>
class basic_io_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = basic_io_cookie_object_t<ch_type>;
	basic_io_cookie_object_t<ch_type> cookie{};
	explicit constexpr operator bool() noexcept
	{
		return cookie.vptr;
	}
	constexpr native_handle_type native_handle() noexcept
	{
		return cookie;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{cookie};
		cookie={};
		return temp;
	}
};

template<std::integral ch_type>
inline constexpr basic_io_io_observer<ch_type> io_value_handle(basic_io_io_observer<ch_type> biob) noexcept
{
	return biob;
}


template<std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<ch_type,char>||std::same_as<ch_type,::fast_io::freestanding::iter_value_t<Iter>>)
inline constexpr Iter read(basic_io_io_observer<ch_type> biob,Iter first,Iter last)
{
	if constexpr(std::same_as<Iter,ch_type*>)
	{
		return biob.cookie.vptr->read_funptr(biob.cookie.cookie,first,last);
	}
	else
	{
		return (biob.cookie.vptr->read_funptr(biob.cookie.cookie,
		reinterpret_cast<ch_type*>(::fast_io::freestanding::to_address(first)),
		reinterpret_cast<ch_type*>(::fast_io::freestanding::to_address(last)))-::fast_io::freestanding::to_address(first))/sizeof(*first)+first;
	}
}

template<std::integral ch_type>
inline constexpr ch_type* ibuffer_begin(basic_io_io_observer<ch_type> biob) noexcept
{
	return biob.cookie.vptr->ibuffer_begin_funptr(biob.cookie.cookie);
}

template<std::integral ch_type>
inline constexpr ch_type* ibuffer_curr(basic_io_io_observer<ch_type> biob) noexcept
{
	return biob.cookie.vptr->ibuffer_curr_funptr(biob.cookie.cookie);
}

template<std::integral ch_type>
inline constexpr ch_type* ibuffer_end(basic_io_io_observer<ch_type> biob) noexcept
{
	return biob.cookie.vptr->ibuffer_end_funptr(biob.cookie.cookie);
}

template<std::integral ch_type>
inline constexpr void ibuffer_set_curr(basic_io_io_observer<ch_type> biob,ch_type* ptr) noexcept
{
	biob.cookie.vptr->ibuffer_set_curr_funptr(biob.cookie.cookie,ptr);
}

template<std::integral ch_type>
inline constexpr bool ibuffer_underflow(basic_io_io_observer<ch_type> biob)
{
	return biob.cookie.vptr->ibuffer_underflow_funptr(biob.cookie.cookie);
}

template<std::integral ch_type>
inline constexpr std::uintmax_t seek(basic_io_io_observer<ch_type> biob,std::intmax_t offset)
{
	return biob.cookie.vptr->seek_funptr(biob.cookie.cookie,offset);
}

template<std::integral ch_type,::fast_io::freestanding::contiguous_iterator Iter>
requires (std::same_as<ch_type,char>||std::same_as<ch_type,::fast_io::freestanding::iter_value_t<Iter>>)
inline constexpr void write(basic_io_io_observer<ch_type> biob,Iter first,Iter last)
{
	if constexpr(std::same_as<ch_type,::fast_io::freestanding::iter_value_t<Iter>>)
	{
		biob.cookie.vptr->write_funptr(biob.cookie.cookie,::fast_io::freestanding::to_address(first),::fast_io::freestanding::to_address(last));
	}
	else
		biob.cookie.vptr->write_funptr(biob.cookie.cookie,
		reinterpret_cast<ch_type const*>(::fast_io::freestanding::to_address(first)),
		reinterpret_cast<ch_type const*>(::fast_io::freestanding::to_address(last)));
}

template<std::integral ch_type>
inline constexpr ch_type* obuffer_begin(basic_io_io_observer<ch_type> biob) noexcept
{
	return biob.cookie.vptr->obuffer_begin_funptr(biob.cookie.cookie);
}

template<std::integral ch_type>
inline constexpr ch_type* obuffer_curr(basic_io_io_observer<ch_type> biob) noexcept
{
	return biob.cookie.vptr->obuffer_curr_funptr(biob.cookie.cookie);
}

template<std::integral ch_type>
inline constexpr ch_type* obuffer_end(basic_io_io_observer<ch_type> biob) noexcept
{
	return biob.cookie.vptr->obuffer_end_funptr(biob.cookie.cookie);
}

template<std::integral ch_type>
inline constexpr void obuffer_set_curr(basic_io_io_observer<ch_type> biob,ch_type* ptr) noexcept
{
	biob.cookie.vptr->obuffer_set_curr_funptr(biob.cookie.cookie,ptr);
}

template<std::integral ch_type>
inline constexpr void obuffer_overflow(basic_io_io_observer<ch_type> biob,ch_type ch)
{
	biob.cookie.vptr->obuffer_overflow(biob.cookie.cookie,ch);
}



template<std::integral ch_type>
class basic_io_io_handle:public basic_io_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = basic_io_cookie_object_t<ch_type>;
	explicit constexpr basic_io_io_handle(native_handle_type cookie1):basic_io_io_observer<char_type>{cookie1}{}

	constexpr basic_io_io_handle(basic_io_io_handle const& other):basic_io_io_observer<char_type>{{other.cookie.vptr,other.cookie.vptr?other.cookie.vptr->clone_funptr(other.cookie.cookie):static_cast<std::uintptr_t>(0)}}
	{
	}
	constexpr basic_io_io_handle& operator=(basic_io_io_handle const& other)
	{
		auto other_vptr{other.cookie.vptr};
		std::uintptr_t other_cookie{other.cookie.cookie};
		std::uintptr_t cloned_cookie{};
		if(other_vptr)
			cloned_cookie=other_vptr->clone_funptr(other_cookie);
		if(this->cookie.vptr)
#if __has_cpp_attribute(likely)
[[likely]]
#endif
			this->cookie.vptr->close_funptr(this->cookie.cookie);
		this->cookie.vptr=other_vptr;
		this->cookie=cloned_cookie;
		return *this;
	}
	constexpr basic_io_io_handle(basic_io_io_handle&& other) noexcept:basic_io_io_observer<char_type>{other.cookie}
	{
		other.cookie={};
	}

	constexpr void close() noexcept
	{
		if(this->cookie.vptr)
#if __has_cpp_attribute(likely)
[[likely]]
#endif
		{
			this->cookie.vptr->close_funptr(this->cookie.cookie);
			this->cookie={};
		}
	}

	constexpr basic_io_io_handle& operator=(basic_io_io_handle&& other) noexcept
	{
		if(__builtin_addressof(other)==this)
#if __has_cpp_attribute(unlikely)
[[unlikely]]
#endif
			return *this;
		if(this->cookie.vptr)
#if __has_cpp_attribute(likely)
[[likely]]
#endif
			this->cookie.vptr->close_funptr(this->cookie.cookie);
		this->cookie=other.cookie;
		other->cookie={};
	}
};

namespace details
{
template<typename T,typename... Args>
inline constexpr std::uintptr_t create_io_io_cookie_impl(Args&& ...args)
{
	if constexpr(value_based_stream<T>)
	{
		return (std::uintptr_t)T(::fast_io::freestanding::forward<Args>(args)...).release();
	}
	else
		return (std::uintptr_t)new T(::fast_io::freestanding::forward<Args>(args)...);
}

}

template<std::integral ch_type>
class basic_io_file:public basic_io_io_handle<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = basic_io_cookie_object_t<ch_type>;
	explicit constexpr basic_io_file(native_handle_type cookie):basic_io_io_handle<char_type>{cookie}{}

	template<stream T,typename... Args>
	requires std::constructible_from<T,Args...>
	constexpr basic_io_file(io_cookie_type_t<T>,Args&& ...args):basic_io_io_handle<ch_type>(native_handle_type{__builtin_addressof(io_cookie_vtable<T>),details::create_io_io_cookie_impl<T>(::fast_io::freestanding::forward<Args>(args)...)})
	{}
	constexpr basic_io_file(io_dup_t,basic_io_io_observer<ch_type> biob):basic_io_io_handle<ch_type>(native_handle_type{biob.cookie.vptr,biob.cookie.vptr?biob.cookie.vptr->clone_funptr(biob.cookie.cookie):static_cast<std::uintptr_t>(0)}){}

	constexpr basic_io_file(basic_io_file const&) = default;
	constexpr basic_io_file& operator=(basic_io_file const&) = default;
	constexpr basic_io_file(basic_io_file&&) noexcept = default;
	constexpr basic_io_file& operator=(basic_io_file&&) noexcept = default;

	constexpr ~basic_io_file()
	{
		if(this->cookie.vptr)
#if __has_cpp_attribute(unlikely)
[[likely]]
#endif
			this->cookie.vptr->close_funptr(this->cookie.cookie);
	}
};

using io_io_observer = basic_io_io_observer<char>;
using io_io_handle = basic_io_io_handle<char>;
using io_file = basic_io_file<char>;

using wio_io_observer = basic_io_io_observer<wchar_t>;
using wio_io_handle = basic_io_io_handle<wchar_t>;
using wio_file = basic_io_file<wchar_t>;

using u8io_io_observer = basic_io_io_observer<char8_t>;
using u8io_io_handle = basic_io_io_handle<char8_t>;
using u8io_file = basic_io_file<char8_t>;

using u16io_io_observer = basic_io_io_observer<char16_t>;
using u16io_io_handle = basic_io_io_handle<char16_t>;
using u16io_file = basic_io_file<char16_t>;

using u32io_io_observer = basic_io_io_observer<char32_t>;
using u32io_io_handle = basic_io_io_handle<char32_t>;
using u32io_file = basic_io_file<char32_t>;

}
