#include <ranges>
#include <algorithm>
#include <span>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fast_io.h>
#include <fast_io_device.h>
using namespace fast_io::io;
#include "localedef.h"
#include "lexing.hpp"
#include "parsing.hpp"
#include "writing.hpp"

void parse_and_write(std::u8string const& filename, lexer::locale_content_t const& ref, char* encoding_name, char* char_width_name) {
	std::u8string_view encoding{reinterpret_cast<char8_t*>(encoding_name)};
	std::u8string_view char_width{reinterpret_cast<char8_t*>(char_width_name)};
	writer write_er{fast_io::u8concat(filename, u8".", encoding, u8"-", char_width)};
	if (encoding == u8"UTF") {
		if (char_width == u8"8") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::utf, char8_t>(ref)};
			write_er.write_file(all);
		} else if (char_width == u8"16") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::utf, char16_t>(ref)};
			write_er.write_file(all);
		} else if (char_width == u8"32") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::utf, char32_t>(ref)};
			write_er.write_file(all);
		} else {
			print(fast_io::u8err(), u8"Invalid char width: ", char_width, u8".\n");
		}
	} else if (encoding == u8"UTF_EBCDIC") {
		if (char_width == u8"8") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::utf_ebcdic, char8_t>(ref)};
			write_er.write_file(all);
		} else if (char_width == u8"16") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::utf_ebcdic, char16_t>(ref)};
			write_er.write_file(all);
		} else if (char_width == u8"32") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::utf_ebcdic, char32_t>(ref)};
			write_er.write_file(all);
		} else {
			print(fast_io::u8err(), u8"Invalid char width: ", char_width, u8".\n");
		}
	} else if (encoding == u8"GB18030") {
		if (char_width == u8"8") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::gb18030, char8_t>(ref)};
			write_er.write_file(all);
		} else if (char_width == u8"16") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::gb18030, char16_t>(ref)};
			write_er.write_file(all);
		} else if (char_width == u8"32") {
			auto const& all{parser{}.parse_file<fast_io::encoding_scheme::gb18030, char32_t>(ref)};
			write_er.write_file(all);
		} else {
			print(fast_io::u8err(), u8"Invalid char width: ", char_width, u8".\n");
		}
	} else {
		print(fast_io::u8err(), u8"Invalid encoding: ", encoding, u8".\n");
	}
}

int main(int argc, char** argv) {
	if (argc != 5) {
		print(fast_io::err(),
			"Usage: ", fast_io::mnp::os_c_str(argv[0]), " <locale_filename> <encoding_name> <char_width> <target_dir>\n"
				"\tencoding_name: UTF, UTF_EBCDIC or GB18030\n"
				"\tchar_width: 8, 16 or 32");
		return EXIT_FAILURE;
	}
	std::u8string filename{reinterpret_cast<char8_t*>(argv[1])};
	auto slash{filename.rfind(u8'/')};
	if (slash == std::u8string::npos) {
		slash = filename.rfind(u8'\\');
		if (slash == std::u8string::npos) {
			panic(fast_io::err(), "Invalid filename.");
		}
	}
	std::u8string dir_filename{filename.substr(0, slash)};
	std::u8string raw_filename{filename.substr(slash + 1)};
	fast_io::dir_file dir{dir_filename};
	lexer lex_er{dir};
	auto& ref{lex_er.parse_file(raw_filename)};
	std::u8string target_dir{reinterpret_cast<char8_t*>(argv[4])};
	if (target_dir.back() != u8'/' && target_dir.back() != u8'\\') {
		target_dir.push_back(u8'/');
	}
	parse_and_write(fast_io::u8concat(target_dir, raw_filename), ref, argv[2], argv[3]);
}
