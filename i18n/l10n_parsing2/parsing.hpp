#pragma once

template <typename T>
concept my_locale_char_type =
	std::same_as<T, char> ||
	std::same_as<T, wchar_t> ||
	std::same_as<T, char8_t> ||
	std::same_as<T, char16_t> ||
	std::same_as<T, char32_t>;

struct parser {
	using locale_entry_t = typename lexer::locale_entry_t;
	using locale_content_t = typename lexer::locale_content_t;

	static char8_t const* parse_escaped_string_content(char8_t const* begin, char8_t const* end, char8_t* __restrict out) {
		constexpr auto escape_char{u8'/'};
		if (begin == end)
			return begin;
		for (auto itr{begin}; itr != end; ++itr, ++out) {
			if (*itr != escape_char) {
				*out = *itr;
				continue;
			}
			++itr;
			if (itr == end)
				return begin;
			switch (*itr) {
			case 'n': *out = u8'\n'; break;
			case 't': *out = u8'\t'; break;
			case 'r': *out = u8'\r'; break;
			case 'v': *out = u8'\v'; break;
			case 'b': *out = u8'\b'; break;
			case 'f': *out = u8'\f'; break;
			case 'a': *out = u8'\a'; break;
			case 'e': *out = u8'\033'; break;
			default: *out = *itr; break;
			}
		}
		return end;
	}
	template <fast_io::encoding_scheme encoding, my_locale_char_type char_type>
	static char8_t const* parse_era_string_content(char8_t const* begin, char8_t const* end, basic_scatter<char_type>& member) {
		if (begin == end)
			return begin;
		std::u8string tmp(end - begin, u8'\0');
		auto itr{parse_escaped_string_content(begin, end, tmp.data())};
		if (itr != end)
			return begin;
		std::basic_string<char_type> result;
		print(fast_io::basic_ostring_ref<char_type>{std::addressof(result)},
			fast_io::mnp::code_cvt<fast_io::encoding_scheme::utf, encoding>(tmp));
		member.rva = member.buffer_size();
		member.buffer.append_range(result);
		member.length = member.buffer_size() - member.rva;
		return end;
	}
	template <bool end_date, std::integral char_type>
	static bool parse_era_date(char8_t const* begin, char8_t const* end, basic_lc_time_era<char_type>& member) {
		if (!fast_io::char_category::is_c_digit(*begin)) {
			if constexpr (end_date) {
				if (std::u8string_view{begin, end} == u8"+*") {
					member.end_date_special = 1;
					member.end_date_year = member.end_date_month = member.end_date_day = 0;
					return true;
				} else if (std::u8string_view{begin, end} == u8"-*") {
					member.end_date_special = -1;
					member.end_date_year = member.end_date_month = member.end_date_day = 0;
					return true;
				}
			}
			if (*begin != u8'-')
				return false;
		}
		if constexpr (end_date)
			member.end_date_special = 0;
		fast_io::parse_result<char8_t const*> res;
		if constexpr (end_date)
			res = fast_io::parse_by_scan(begin, end, fast_io::mnp::dec_get<true, true>(member.end_date_year));
		else
			res = fast_io::parse_by_scan(begin, end, fast_io::mnp::dec_get<true, true>(member.start_date_year));
		if (res.code != fast_io::parse_code::ok)
			return false;
		auto& itr{res.iter};
		if (itr == end || *itr != u8'/')
			return false;
		++itr;
		if constexpr (end_date)
			res = fast_io::parse_by_scan(itr, end, fast_io::mnp::dec_get<true, true>(member.end_date_month));
		else
			res = fast_io::parse_by_scan(itr, end, fast_io::mnp::dec_get<true, true>(member.start_date_month));
		if (res.code != fast_io::parse_code::ok)
			return false;
		if (itr == end || *itr != u8'/')
			return false;
		++itr;
		if constexpr (end_date)
			res = fast_io::parse_by_scan(itr, end, fast_io::mnp::dec_get<true, true>(member.end_date_day));
		else
			res = fast_io::parse_by_scan(itr, end, fast_io::mnp::dec_get<true, true>(member.start_date_day));
		if (res.code != fast_io::parse_code::ok)
			return false;
		if (itr != end)
			return false;
		return true;
	}

	std::u8string const* get_raw(locale_content_t const& content, std::u8string_view tier1, std::u8string tier2) {
		std::u8string tmp{u8"LC_"};
		std::ranges::transform(tier1, std::back_inserter(tmp), fast_io::char_category::to_c_upper<char8_t>);
		if (auto itr1{content.find(tmp)}; itr1 == content.end())
			return nullptr;
		else if (auto itr2{itr1->second.find(tier2)}; itr2 == itr1->second.end())
			return nullptr;
		else
			return std::addressof(itr2->second);
	}

	static char8_t const* parse_escaped_string(char8_t const* begin, char8_t const* end, char8_t* __restrict out) {
		constexpr auto escape_char{u8'/'};
		auto itr{begin};
		if (itr == end || *itr != u8'\"')
			return begin;
		++itr;
		for (; itr != end; ++itr, ++out) {
			if (*itr == escape_char) {
				++itr;
				if (itr == end)
					return begin;
				switch (*itr) {
				case 'n': *out = u8'\n'; break;
				case 't': *out = u8'\t'; break;
				case 'r': *out = u8'\r'; break;
				case 'v': *out = u8'\v'; break;
				case 'b': *out = u8'\b'; break;
				case 'f': *out = u8'\f'; break;
				case 'a': *out = u8'\a'; break;
				case 'e': *out = u8'\033'; break;
				default: *out = *itr; break;
				}
				continue;
			} else if (*itr == '"')
				return ++itr;
			else
				*out = *itr;
		}
		return begin;
	}
	template <fast_io::encoding_scheme encoding, my_locale_char_type char_type>
	char8_t const* parse_member_partial(char8_t const* begin, char8_t const* end, basic_scatter<char_type>& member) {
		if (begin == end)
			return begin;
		std::u8string tmp(end - begin, u8'\0');
		auto itr{parse_escaped_string(begin, end, tmp.data())};
		if (itr == end);
		else if (*itr == u8';')
			++itr;
		else
			return begin;
		std::basic_string<char_type> result;
		print(fast_io::basic_ostring_ref<char_type>{std::addressof(result)},
			fast_io::mnp::code_cvt<fast_io::encoding_scheme::utf, encoding>(tmp));
		member.rva = member.buffer_size();
		member.buffer.append_range(result);
		member.length = member.buffer_size() - member.rva;
		return itr;
	}
	template <fast_io::encoding_scheme encoding, typename T>
	char8_t const* parse_member_partial(char8_t const* begin, char8_t const* end, basic_scatter<T>& member) {
		if (begin == end)
			return begin;
		T tmp;
		auto itr{begin};
		member.rva = member.buffer_size();
		while (true) {
			auto itr1{parse_member_partial<encoding>(itr, end, tmp)};
			if (itr1 == itr)
				return begin;
			itr = itr1;
			member.buffer.push_back(tmp);
			if (itr == end)
				break;
		}
		member.length = member.buffer_size() - member.rva;
		if (member.length == 0)
			return begin;
		return end;
	}
	template <fast_io::encoding_scheme encoding, typename T, std::size_t N>
	char8_t const* parse_member_partial(char8_t const* begin, char8_t const* end, T(&member)[N]) {
		if (begin == end)
			return begin;
		auto itr{begin};
		if constexpr (N > 1) {
			for (std::size_t i{}; i != N - 1; ++i) {
				auto itr1{parse_member_partial<encoding>(itr, end, member[i])};
				if (itr1 == itr || itr1 == end)
					return begin;
				itr = itr1;
			}
		}
		auto itr1{parse_member_partial<encoding>(itr, end, member[N - 1])};
		if (itr1 == itr || itr1 != end)
			return begin;
		return end;
	}
	template <fast_io::encoding_scheme encoding, std::integral T>
	char8_t const* parse_member_partial(char8_t const* begin, char8_t const* end, T& member) {
		if (begin == end)
			return begin;
		auto [itr, err] {fast_io::parse_by_scan(begin, end, member)};
		if (err != fast_io::parse_code::ok) {
			if constexpr (std::unsigned_integral<T>) {
				if (std::u8string_view{begin, end}.starts_with(u8"-1")) {
					member = static_cast<T>(-1);
					itr += 2;
				} else
					return begin;
			} else
				return begin;
		}
		if (itr == end)
			return end;
		if (*itr != u8';')
			return begin;
		return ++itr;
	}
	template <fast_io::encoding_scheme encoding, std::integral char_type>
	char8_t const* parse_member_partial(char8_t const* begin, char8_t const* end, basic_lc_time_era<char_type>& member) {
		std::u8string tmp(end - begin, u8'\0');
		auto itr{parse_escaped_string(begin, end, tmp.data())};
		if (itr == begin || tmp.size() < 5)
			return begin;
		if (itr == end);
		else if (*itr == u8';')
			++itr;
		else
			return begin;
		std::vector<std::size_t> colon_positions;
		colon_positions.reserve(5);
		for (std::size_t i{}; i != tmp.size(); ++i) {
			if (tmp[i] == u8':')
				colon_positions.push_back(i);
		}
		if (colon_positions.size() != 5)
			return begin;
		if (tmp.front() == u8'+')
			member.offset = true;
		else if (tmp.front() == u8'-')
			member.offset = false;
		else
			return begin;
		if (auto [itr, err] {fast_io::parse_by_scan(tmp.data() + colon_positions[0] + 1, tmp.data() + colon_positions[1], member.offset)};
			err != fast_io::parse_code::ok || itr != tmp.data() + colon_positions[1])
			return begin;
		if (!parse_era_date<false>(tmp.data() + colon_positions[1] + 1, tmp.data() + colon_positions[2], member))
			return begin;
		if (!parse_era_date<true>(tmp.data() + colon_positions[2] + 1, tmp.data() + colon_positions[3], member))
			return begin;
		if (parse_era_string_content<encoding>(tmp.data() + colon_positions[3] + 1, tmp.data() + colon_positions[4], member.era_name)
			== tmp.data() + colon_positions[3] + 1)
			return begin;
		if (parse_era_string_content<encoding>(tmp.data() + colon_positions[4] + 1, std::to_address(tmp.end()), member.era_format)
			== tmp.data() + colon_positions[4] + 1)
			return begin;
		return itr;
	}
	template <fast_io::encoding_scheme encoding, typename T>
	char8_t const* parse_member_partial(char8_t const* begin, char8_t const* end, T& member) {
		if constexpr (requires (T t) {
			{ t.ndays } -> std::same_as<std::size_t&>;
			{ t.first_day } -> std::same_as<std::int_least64_t&>;
			{ t.first_week } -> std::same_as<std::size_t&>;
		}) {
			if (begin == end)
				return begin;
			auto itr{parse_member_partial<encoding>(begin, end, member.ndays)};
			if (itr == begin)
				return begin;
			itr = parse_member_partial<encoding>(itr, end, member.first_day);
			if (itr == begin)
				return begin;
			itr = parse_member_partial<encoding>(itr, end, member.first_week);
			if (itr == end)
				return end;
			if (*itr != u8';')
				return begin;
			return ++itr;
		} else
			return begin;
	}

	template <fast_io::encoding_scheme encoding, typename T>
	bool parse_member(std::u8string const& content, T& member) {
		if (parse_member_partial<encoding>(std::to_address(content.begin()), std::to_address(content.end()), member) == std::to_address(content.begin()))
			return false;
		return true;
	}

	template <fast_io::encoding_scheme encoding, std::integral char_type>
	basic_lc_all<char_type> parse_file(locale_content_t const& content) {
		basic_lc_all<char_type> retval;
		std::u8string const* tmp;
#define FAST_IO_I18N_PARSE_MEMBER(tier1, tier2)																		\
		do {																										\
			if (tmp = get_raw(content, u8""#tier1, u8""#tier2); tmp == nullptr) break;								\
			if (!parse_member<encoding>(*tmp, retval.tier1.tier2)) panic("parsing failed. invalid grammar");		\
		} while(0)
		FAST_IO_I18N_PARSE_MEMBER(identification, title);
		FAST_IO_I18N_PARSE_MEMBER(identification, source);
		FAST_IO_I18N_PARSE_MEMBER(identification, address);
		FAST_IO_I18N_PARSE_MEMBER(identification, contact);
		FAST_IO_I18N_PARSE_MEMBER(identification, email);
		FAST_IO_I18N_PARSE_MEMBER(identification, tel);
		FAST_IO_I18N_PARSE_MEMBER(identification, fax);
		FAST_IO_I18N_PARSE_MEMBER(identification, language);
		FAST_IO_I18N_PARSE_MEMBER(identification, territory);
		FAST_IO_I18N_PARSE_MEMBER(identification, audience);
		FAST_IO_I18N_PARSE_MEMBER(identification, application);
		FAST_IO_I18N_PARSE_MEMBER(identification, abbreviation);
		FAST_IO_I18N_PARSE_MEMBER(identification, revision);
		FAST_IO_I18N_PARSE_MEMBER(identification, date);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_curr_symbol);
		FAST_IO_I18N_PARSE_MEMBER(monetary, currency_symbol);
		FAST_IO_I18N_PARSE_MEMBER(monetary, mon_decimal_point);
		FAST_IO_I18N_PARSE_MEMBER(monetary, mon_thousands_sep);
		FAST_IO_I18N_PARSE_MEMBER(monetary, mon_grouping);
		FAST_IO_I18N_PARSE_MEMBER(monetary, positive_sign);
		FAST_IO_I18N_PARSE_MEMBER(monetary, negative_sign);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_frac_digits);
		FAST_IO_I18N_PARSE_MEMBER(monetary, frac_digits);
		FAST_IO_I18N_PARSE_MEMBER(monetary, p_cs_precedes);
		FAST_IO_I18N_PARSE_MEMBER(monetary, p_sep_by_space);
		FAST_IO_I18N_PARSE_MEMBER(monetary, n_cs_precedes);
		FAST_IO_I18N_PARSE_MEMBER(monetary, n_sep_by_space);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_p_cs_precedes);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_p_sep_by_space);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_n_cs_precedes);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_n_sep_by_space);
		FAST_IO_I18N_PARSE_MEMBER(monetary, p_sign_posn);
		FAST_IO_I18N_PARSE_MEMBER(monetary, n_sign_posn);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_p_sign_posn);
		FAST_IO_I18N_PARSE_MEMBER(monetary, int_n_sign_posn);
		FAST_IO_I18N_PARSE_MEMBER(numeric, decimal_point);
		FAST_IO_I18N_PARSE_MEMBER(numeric, thousands_sep);
		FAST_IO_I18N_PARSE_MEMBER(numeric, grouping);
		FAST_IO_I18N_PARSE_MEMBER(time, abday);
		FAST_IO_I18N_PARSE_MEMBER(time, day);
		FAST_IO_I18N_PARSE_MEMBER(time, abmon);
		FAST_IO_I18N_PARSE_MEMBER(time, ab_alt_mon);
		FAST_IO_I18N_PARSE_MEMBER(time, mon);
		FAST_IO_I18N_PARSE_MEMBER(time, d_t_fmt);
		FAST_IO_I18N_PARSE_MEMBER(time, d_fmt);
		FAST_IO_I18N_PARSE_MEMBER(time, t_fmt);
		FAST_IO_I18N_PARSE_MEMBER(time, t_fmt_ampm);
		FAST_IO_I18N_PARSE_MEMBER(time, date_fmt);
		FAST_IO_I18N_PARSE_MEMBER(time, am_pm);
		FAST_IO_I18N_PARSE_MEMBER(time, era);
		FAST_IO_I18N_PARSE_MEMBER(time, era_d_fmt);
		FAST_IO_I18N_PARSE_MEMBER(time, era_d_t_fmt);
		FAST_IO_I18N_PARSE_MEMBER(time, era_t_fmt);
		FAST_IO_I18N_PARSE_MEMBER(time, alt_digits);
		FAST_IO_I18N_PARSE_MEMBER(time, week);
		FAST_IO_I18N_PARSE_MEMBER(time, first_weekday);
		FAST_IO_I18N_PARSE_MEMBER(time, first_workday);
		FAST_IO_I18N_PARSE_MEMBER(time, cal_direction);
		FAST_IO_I18N_PARSE_MEMBER(time, timezone);
		FAST_IO_I18N_PARSE_MEMBER(messages, yesexpr);
		FAST_IO_I18N_PARSE_MEMBER(messages, noexpr);
		FAST_IO_I18N_PARSE_MEMBER(messages, yesstr);
		FAST_IO_I18N_PARSE_MEMBER(messages, nostr);
		FAST_IO_I18N_PARSE_MEMBER(paper, width);
		FAST_IO_I18N_PARSE_MEMBER(paper, height);
		FAST_IO_I18N_PARSE_MEMBER(telephone, tel_int_fmt);
		FAST_IO_I18N_PARSE_MEMBER(telephone, tel_dom_fmt);
		FAST_IO_I18N_PARSE_MEMBER(telephone, int_select);
		FAST_IO_I18N_PARSE_MEMBER(telephone, int_prefix);
		FAST_IO_I18N_PARSE_MEMBER(name, name_fmt);
		FAST_IO_I18N_PARSE_MEMBER(name, name_gen);
		FAST_IO_I18N_PARSE_MEMBER(name, name_miss);
		FAST_IO_I18N_PARSE_MEMBER(name, name_mr);
		FAST_IO_I18N_PARSE_MEMBER(name, name_mrs);
		FAST_IO_I18N_PARSE_MEMBER(name, name_ms);
		FAST_IO_I18N_PARSE_MEMBER(address, postal_fmt);
		FAST_IO_I18N_PARSE_MEMBER(address, country_name);
		FAST_IO_I18N_PARSE_MEMBER(address, country_post);
		FAST_IO_I18N_PARSE_MEMBER(address, country_ab2);
		FAST_IO_I18N_PARSE_MEMBER(address, country_ab3);
		FAST_IO_I18N_PARSE_MEMBER(address, country_num);
		FAST_IO_I18N_PARSE_MEMBER(address, country_car);
		FAST_IO_I18N_PARSE_MEMBER(address, country_isbn);
		FAST_IO_I18N_PARSE_MEMBER(address, lang_name);
		FAST_IO_I18N_PARSE_MEMBER(address, lang_ab);
		FAST_IO_I18N_PARSE_MEMBER(address, lang_term);
		FAST_IO_I18N_PARSE_MEMBER(address, lang_lib);
		FAST_IO_I18N_PARSE_MEMBER(measurement, measurement);
		FAST_IO_I18N_PARSE_MEMBER(keyboard, keyboards);
#undef FAST_IO_I18N_PARSE_MEMBER
		return retval;
	}
};
