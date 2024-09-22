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

int main(int argc, char** argv) {
	fast_io::dir_file srcdir;
	if (argc != 2) {
		srcdir = fast_io::dir_file{u8"../l10n/localedata/"};
		print(fast_io::err(), "Missing locale directory argument. Switch to default (a.k.a. ../l10n/localedata/).\n");
	} else {
		srcdir = fast_io::dir_file{argv[1]};
	}
	lexer lex_er{srcdir};
	for (auto entry : recursive(at(srcdir))) {
		auto filename{std::u8string_view{u8filename(entry)}};
		using namespace std::string_view_literals;
		if (filename == u8"."sv || filename == u8".."sv)
			continue;
		if (fnm == u8"cns11643_stroke" || fnm == u8"i18n_ctype")
			continue;
		if (fnm.starts_with(u8"iso14651") ||fnm.starts_with(u8"translit"))
			continue;
		auto& ref{lex_er.parse_file(std::u8string{filename}})};
		fast_io::u8obuf_file raw{fast_io::u8concat(u8"intermedia/", filename)};
		update_info(ref);
		output_raw_file(ref, raw);
	}
} 
