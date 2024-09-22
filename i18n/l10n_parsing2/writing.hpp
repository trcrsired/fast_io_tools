#pragma once

struct writer {
	struct header {
		char8_t magic_number[8]{u8'F', u8'A', u8'S', u8'T', u8'I', u8'O', u8'L', u8'C'};
		scatter_size_type major_version{0};
		scatter_size_type minor_version{0};
		scatter_size_type patch_version{0};
		fast_io::unix_timestamp time{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	};
	struct offsets {
		scatter_size_type offset[6];
	};

	fast_io::u8obuf_file bin;
	header const head;

	template <typename ...Args>
		requires(std::constructible_from<fast_io::obuf_file, Args...>)
	writer(Args&& ...args) noexcept : bin{std::forward<Args>(args)...}, head{} {}

	template <std::integral char_type>
	void write_file(basic_lc_all<char_type> const& all) {
		auto const header_size{static_cast<scatter_size_type>(sizeof(header) + sizeof(offsets))};
		auto const all_size{static_cast<scatter_size_type>(sizeof(all))};
#define SIZE(name, type) auto const name{static_cast<scatter_size_type>(basic_scatter<type>::buffer_size() * sizeof(type))}
		SIZE(sct_char_size, char_type);
		SIZE(sct_size_size, std::size_t);
		SIZE(sct_era_size, basic_lc_time_era<char_type>);
		SIZE(sct_sct_char_size, basic_scatter<char_type>);
#undef SIZE
		offsets offs{
			header_size,
			header_size + all_size,
			header_size + all_size + sct_char_size,
			header_size + all_size + sct_char_size + sct_size_size,
			header_size + all_size + sct_char_size + sct_size_size + sct_era_size,
			header_size + all_size + sct_char_size + sct_size_size + sct_era_size + sct_sct_char_size
		};
		auto header_begin{reinterpret_cast<char8_t const*>(std::addressof(head))};
		auto header_end{header_begin + header_size};
		auto offs_begin{reinterpret_cast<char8_t const*>(std::addressof(offs))};
		auto offs_end{offs_begin + sizeof(offsets)};
		auto all_begin{reinterpret_cast<char8_t const*>(std::addressof(all))};
		auto all_end{all_begin + all_size};
		fast_io::operations::write_all(bin, header_begin, header_end);
		fast_io::operations::write_all(bin, offs_begin, offs_end);
		fast_io::operations::write_all(bin, all_begin, all_end);
#define WRITE(type) fast_io::operations::write_all(bin, reinterpret_cast<char8_t const*>(std::to_address(basic_scatter<type>::buffer.begin())), reinterpret_cast<char8_t const*>(std::to_address(basic_scatter<type>::buffer.end())));
		WRITE(char_type);
		WRITE(std::size_t);
		WRITE(basic_lc_time_era<char_type>);
		WRITE(basic_scatter<char_type>);
#undef WRITE
	}
};
