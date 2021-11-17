#include<fast_io.h>

template<std::integral ch_type>
struct basic_compile_time_os_file_structure
{
	using char_type = ch_type;
};

using compile_time_os_file_structure = basic_compile_time_os_file_structure<char>;

template<>
inline constexpr void read() noexcept
{
	
}

template<std::integral char_type,::fast_io::freestanding::input_or_output_iterator Iter>
inline constexpr void write(basic_compile_time_os_file_structure<char_type>,Iter first,Iter last) noexcept
{
	constexpr_sanitize_input();
}

struct compile_time_os_file_handle
{
	compile_time_os_file_structure* compile_time_handle{};
};

template<::std::integral char_type>
struct basic_buffered_compile_time_os_file_handle
{
	::fast_io::basic_iobuf<compile_time_os_file_structure>* compile_time_buffered_handle{};
};



template<::std::integral char_type>
struct test
{
	union
	{
		std::FILE* fp{};
		basic_compile_time_os_file_handle<char_type> compile_time_fp;
	};
};

int main()
{
	test<char> t;
	t.fp=nullptr;
}