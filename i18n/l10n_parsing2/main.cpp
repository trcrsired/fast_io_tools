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

int main() {
	fast_io::dir_file srcdir{u8"../l10n/localedata/"};
	lexer lex_er{srcdir};
	parser parse_er;
	for (auto entry : recursive(at(srcdir))) {
		auto filename{std::u8string_view{u8filename(entry)}};
		auto& ref{lex_er.parse_file(std::u8string{std::u8string_view{u8filename(entry)}})};
		fast_io::u8obuf_file raw{fast_io::u8concat(u8"intermedia/", u8filename(entry), u8".txt")};
		output_raw_file(ref, raw);
		update_info(ref);
		auto all{parse_er.parse_file<wchar_t>(ref)};
		writer write_er{fast_io::u8concat(u8"result/", u8filename(entry), u8".UTF-8")};
		write_er.write_file(all);
	}
} 
