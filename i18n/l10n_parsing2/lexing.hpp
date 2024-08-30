#pragma once

static constexpr std::string utf16_to_utf8(char16_t unicode_char) {
	std::string utf8_str;
	if (unicode_char <= 0x7F) {
		utf8_str.push_back(static_cast<char>(unicode_char));
	} else if (unicode_char <= 0x7FF) {
		utf8_str.push_back(static_cast<char>(0xC0 | ((unicode_char >> 6) & 0x1F)));
		utf8_str.push_back(static_cast<char>(0x80 | (unicode_char & 0x3F)));
	} else {
		utf8_str.push_back(static_cast<char>(0xE0 | ((unicode_char >> 12) & 0x0F)));
		utf8_str.push_back(static_cast<char>(0x80 | ((unicode_char >> 6) & 0x3F)));
		utf8_str.push_back(static_cast<char>(0x80 | (unicode_char & 0x3F)));
	}
	return utf8_str;
}

struct lexer {
#define FAST_IO_I18N_LINE_TRIM_START(line, comment_char, col_cnt)													\
	{																												\
		std::size_t i{};																							\
		for (; i != line.size() && fast_io::char_category::is_c_space(line[i]); ++i);								\
		line = line.substr(i);																						\
		if (line.empty() || line.front() == comment_char)															\
			continue;																								\
		col_cnt = i;																								\
	}
#define FAST_IO_I18N_LINE_TRIM_END(line, comment_char)																\
	{																												\
		line = line.substr(0, line.find_first_of(comment_char, 0));													\
		std::size_t i{line.size()};																					\
		for (; i != 0 && fast_io::char_category::is_c_space(line[i - 1]); --i);										\
		line = line.substr(0, i);																					\
	}

	using locale_entry_t = std::unordered_map<std::u8string, std::u8string>;
	using locale_content_t = std::unordered_map<std::u8string, locale_entry_t>;

	struct worker {
		lexer& env;
		fast_io::u8ibuf_file file;
		std::u8string curr_line;
		char8_t comment_char{u8'%'};
		char8_t escape_char{u8'/'};
		std::size_t line_cnt{};
		std::size_t col_cnt{};
		worker(lexer* lex, fast_io::u8ibuf_file f) noexcept : env(*lex), file(std::move(f)) {}

		[[noreturn]] void panic_with_loc(std::u8string_view reason = u8"") noexcept {
			panic(fast_io::u8err(), u8"error parsing file at line: ", line_cnt, u8", col: ", col_cnt, u8". ", reason);
		}

		char8_t get_comment_or_escaping_character(std::u8string_view stvw) noexcept {
			for (auto const e : stvw)
				if (!fast_io::char_category::is_c_space(e))
					return e;
			panic_with_loc(u8"Invalid comment char or escape char define. ");
		}

		bool refresh_line_if_EOL_escape() {
			if (curr_line.size() != 1)
				return false;
			if (curr_line.front() != escape_char)
				return false;
			std::size_t i{1};
			++col_cnt;
			while (true) {
				if (!scan<true>(file, fast_io::mnp::line_get(curr_line)))
					panic_with_loc(u8"Unexpected EOF after an escape char. ");
				++line_cnt;
				if (curr_line.empty())
					break;
				if (curr_line[0] == escape_char)
					continue;
				break;
			}
			col_cnt = 0;
			return true;
		}
		void skip_whitespace_multiline() {
			bool refreshed{};
			do {
				std::size_t i{};
				for (; i != curr_line.size() && fast_io::char_category::is_c_space(curr_line[i]); ++i, ++col_cnt);
				curr_line = curr_line.substr(i);
				if (!curr_line.empty())
					refreshed = refresh_line_if_EOL_escape();
			} while (refreshed && !curr_line.empty() && fast_io::char_category::is_c_space(curr_line.front()));
			return;
		}
		void consume_comment() {
			if (curr_line.empty())
				return;
			if (curr_line.front() != comment_char)
				return;
			if (curr_line.back() == escape_char)
				curr_line = escape_char;
			else
				curr_line = u8"";
		}

		void parse_value_escaped_string(std::u8string& retval) {
			for (std::size_t i{}; i != curr_line.size(); ++i, ++col_cnt) {
				if (curr_line[i] == u8'\"') {
					retval.push_back(u8'\"');
					curr_line = curr_line.substr(i + 1);
					++col_cnt;
					return;
				} else if (curr_line[i] == u8'<' && i + 6 < curr_line.size() && curr_line[i + 1] == u8'U' && curr_line[i + 6] == u8'>') {
					char16_t code;
					auto [itr, err] {fast_io::parse_by_scan(curr_line.data() + i + 2, curr_line.data() + i + 6, fast_io::mnp::hex_get<true, true>(code))};
					if (err != fast_io::parse_code::ok || itr != curr_line.data() + i + 6)
						panic_with_loc(u8"Invalid Unicode escape sequence. ");
					std::u16string utf16_str{1, code};
					std::u8string utf8_str;
					print(fast_io::u8ostring_ref{&utf8_str}, fast_io::mnp::code_cvt<fast_io::encoding_scheme::utf, fast_io::encoding_scheme::utf>(utf16_str));
					retval.append_range(utf8_str);
					i += 6;
					col_cnt += 6;
				} else if (curr_line[i] == escape_char) {
					if (i == curr_line.size() - 1) {
						curr_line = escape_char;
						refresh_line_if_EOL_escape();
						return parse_value_escaped_string(retval);
					}
					++i;
					++col_cnt;
					retval.push_back(u8'/');
					if (curr_line[i] == escape_char)
						retval.push_back(u8'/');
					else
						retval.push_back(curr_line[i]);
				} else {
					retval.push_back(curr_line[i]);
				}
			}
			panic_with_loc(u8"String not enclosed by a double quotation. ");
		}
		void parse_value_non_escaped_string(std::u8string& retval) {
			std::size_t i{};
			while (true) {
				for (; i != curr_line.size() && !fast_io::char_category::is_c_space(curr_line[i]) && curr_line[i] != comment_char; ++i, ++col_cnt);
				if (i == curr_line.size()) {
					--i;
					if (curr_line[i] != escape_char) {
						retval.append(curr_line);
						curr_line = curr_line.substr(i + 1);
						return;
					}
					retval.append_range(curr_line.substr(0, i));
					curr_line = escape_char;
					refresh_line_if_EOL_escape();
				} else if (curr_line[i] == comment_char) {
					retval.append(curr_line.substr(0, i));
					curr_line = curr_line.substr(i);
					consume_comment();
					return;
				} else
					break;
			}
			retval.append(curr_line.substr(0, i));
			curr_line = curr_line.substr(i);
			return;
		}

		void parse_key_value(locale_entry_t& entry) {
			// discard category
			if (curr_line.starts_with(u8"category "))
				return;
			std::size_t i{};
			for (; i != curr_line.size() && !fast_io::char_category::is_c_space(curr_line[i]); ++i, ++col_cnt);
			if (i == curr_line.size()) {
				--i;
				if (curr_line[i] != escape_char)
					panic_with_loc(u8"Missing value in key-value pair. ");
			}
			// key found at [0, i). rest: whitespaces value (whitespaces ';' whitespaces value)+
			auto& content{entry.insert_or_assign(curr_line.substr(0, i), u8"").first->second};
			curr_line = curr_line.substr(i);
			while (true) {
				skip_whitespace_multiline();
				if (curr_line.empty())
					panic_with_loc(u8"Missing value in key-value pair. ");
				if (curr_line.front() == u8'\"') {
					content.push_back(u8'\"');
					curr_line = curr_line.substr(1);
					++col_cnt;
					parse_value_escaped_string(content);
				}
				else
					parse_value_non_escaped_string(content);
				skip_whitespace_multiline();
				consume_comment();
				if (curr_line.empty())
					return;
				if (curr_line.front() == u8';') {
					curr_line = curr_line.substr(1);
					++col_cnt;
					content.push_back(u8';');
					continue;
				} else
					refresh_line_if_EOL_escape();
			}
		}

		void parse_lc_category(std::u8string category_name, locale_entry_t& entry) {
			if (category_name == u8"LC_COLLATE" || category_name == u8"LC_CTYPE") { // discard lc_collate and lc_ctype
				for (; scan<true>(file, fast_io::mnp::line_get(curr_line)); ++line_cnt, col_cnt = 0) {
					FAST_IO_I18N_LINE_TRIM_START(curr_line, comment_char, col_cnt);
					if (curr_line.starts_with(u8"END ")) {
						curr_line = curr_line.substr(4);
						col_cnt += 4;
						FAST_IO_I18N_LINE_TRIM_END(curr_line, comment_char);
						if (curr_line.empty() || curr_line == category_name)
							return;
						else
							panic_with_loc(u8"Unpaired open and closed category name. ");
					}
				}
				panic_with_loc(fast_io::u8concat(u8"Category: LC_", category_name, u8" not closed. "));
			}
			for (; scan<true>(file, fast_io::mnp::line_get(curr_line)); ++line_cnt, col_cnt = 0) {
				FAST_IO_I18N_LINE_TRIM_START(curr_line, comment_char, col_cnt);
				if (curr_line.starts_with(u8"copy ")) {
					curr_line = curr_line.substr(5);
					col_cnt += 5;
					std::size_t i{};
					for (; i != curr_line.size() && fast_io::char_category::is_c_space(curr_line[i]); ++i);
					col_cnt += i;
					if (i == curr_line.size() || curr_line[i] != '\"')
						panic_with_loc(u8"Expect to-copy filename, which should be enclosed by a double quotation. ");
					std::size_t j{i + 1};
					for (; j != curr_line.size() && curr_line[j] != '\"'; ++j);
					col_cnt += j;
					if (j == curr_line.size())
						panic_with_loc(u8"Filename should be enclosed by a double quotation. ");
					entry.insert_range(env.parse_file(curr_line.substr(i + 1, j - i - 1))[category_name]);
				} else if (curr_line.starts_with(u8"END ")) {
					curr_line = curr_line.substr(4);
					col_cnt += 4;
					FAST_IO_I18N_LINE_TRIM_END(curr_line, comment_char);
					if (curr_line.empty() || curr_line == category_name)
						return;
					else
						panic_with_loc(u8"Unpaired open and closed category name. ");
				} else
					parse_key_value(entry);
			}
			panic_with_loc(u8"Unreachable. Something must have gone wrong. ");
		}

		locale_content_t& parse_file(locale_content_t& entries) {
			for (; scan<true>(file, fast_io::mnp::line_get(curr_line)); ++line_cnt, col_cnt = 0) {
				FAST_IO_I18N_LINE_TRIM_START(curr_line, comment_char, col_cnt);
				if (curr_line.starts_with(u8"comment_char ")) {
					col_cnt += 13;
					comment_char = get_comment_or_escaping_character(curr_line.substr(13));
					continue;
				} else if (curr_line.starts_with(u8"escape_char")) {
					col_cnt += 12;
					escape_char = get_comment_or_escaping_character(curr_line.substr(12));
					continue;
				} else
					break;
			}
			do {
				FAST_IO_I18N_LINE_TRIM_START(curr_line, comment_char, col_cnt);
				if (curr_line.starts_with(u8"category ")) // discard category
					continue;
				if (curr_line.starts_with(u8"LC_")) {
					FAST_IO_I18N_LINE_TRIM_END(curr_line, comment_char);
					parse_lc_category(curr_line, entries[curr_line]);
				}
				++line_cnt;
				col_cnt = 0;
			} while (scan<true>(file, fast_io::mnp::line_get(curr_line)));
			return entries;
		}
	};

	fast_io::native_io_observer directory;
	std::map<std::u8string, locale_content_t> cache;
	lexer(fast_io::native_io_observer df) noexcept
		: directory(std::move(df)) {}

	locale_content_t& parse_file(std::u8string const& filename) {
		auto cached_data{cache.find(filename)};
		if (cached_data != cache.cend())
			return cached_data->second;
		auto& entries{cache[filename]};
		fast_io::u8ibuf_file imm{at(directory), filename};
		return worker{this, std::move(imm)}.parse_file(entries);
	}

#undef FAST_IO_I18N_LINE_TRIM_END
#undef FAST_IO_I18N_LINE_TRIM_START
};

inline void update_info(lexer::locale_content_t& mp) noexcept {
	lexer::locale_entry_t new_info;
	auto& my_info{mp.try_emplace(u8"LC_IDENTIFICATION", std::move(new_info)).first->second};
#define FAST_IO_I18N_APPEND(X, Y)											\
	if (auto itr{my_info.find(u8""#X)}; itr != my_info.end()) {				\
		itr->second.append(u8"; "#Y);										\
	} else {																\
		my_info.insert({u8""#X, u8""#Y});									\
	}
#define FAST_IO_I18N_APPEND_CONCAT(X, Y)									\
	if (u8""#Y[0] != u8'\"') fast_io::fast_terminate();						\
	if (auto itr{my_info.find(u8""#X)}; itr != my_info.end()) {				\
		if (itr->second != u8"\"\"") {										\
			auto& str{itr->second};											\
			str.back() = u8';';												\
			str.append(std::u8string_view{u8""#Y}.substr(1));				\
		} else {															\
			itr->second = u8""#Y;											\
		}																	\
	} else {																\
		my_info.insert({u8""#X, u8""#Y});									\
	}
	FAST_IO_I18N_APPEND_CONCAT(source, "fast_io");
	FAST_IO_I18N_APPEND_CONCAT(contact, "fast_io");
	FAST_IO_I18N_APPEND_CONCAT(email, "euloanty@live.com");
	FAST_IO_I18N_APPEND_CONCAT(address, "https:////github.com//cppfastio//fast_io//");
#undef FAST_IO_I18N_APPEND_CONCAT
#undef FAST_IO_I18N_APPEND
	if (auto itr{mp.find(u8"LC_ADDRESS")}; itr != mp.end()) {
		auto& lc_address{itr->second};
		if (auto itr1{lc_address.find(u8"country_num")}; itr1 != lc_address.end()) {
			auto& country_num{itr1->second};
			if (country_num.front() != u8'\"') {
				country_num = fast_io::u8concat(u8"\"", country_num, u8"\"");
			}
		}
		if (auto itr1{lc_address.find(u8"country_isbn")}; itr1 != lc_address.end()) {
			auto& country_isbn{itr1->second};
			if (country_isbn.front() != u8'\"') {
				country_isbn = fast_io::u8concat(u8"\"", country_isbn, u8"\"");
			}
		}
	}
}

template <typename T>
inline void output_raw_file(lexer::locale_content_t const& mp, T& obf) noexcept {
	for (auto const& e : mp) {
		struct category_printer {
			T& obf;
			std::u8string_view name;
			category_printer(T& o, std::u8string_view n) noexcept : obf(o), name(n) {
				println(obf, name);
			}
			~category_printer() noexcept {
				print(obf, u8"END ", name, u8"\n\n");
			}
		} cat(obf, e.first);
		for (auto const& e1 : e.second) {
			println(obf, e1.first, u8" ", e1.second);
		}
	}
}
