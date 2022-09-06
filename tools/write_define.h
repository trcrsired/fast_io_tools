inline constexpr std::byte* write_some_bytes_define(basic_posix_io_observer<char> ent,std::byte* first,std::byte* last)
{
	return _write(end,first,last);
}

template<typename Func>
inline constexpr void write_all_common_impl(Func fnc,std::byte* first,std::byte* last) noexcept(noexcept(fnc(first,last)))
{
	for(;first!=last;first=fnc(first,last));
}

namespace operations
{

inline constexpr void write_all()
{
	
}

}