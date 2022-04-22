#pragma once
#include<cstddef>
#include<cstdint>

namespace fast_io_i18n
{

template<typename T>
struct basic_io_scatter_t
{
	T const* base{};
	std::size_t len{};
};

struct iso6709_coordinate
{
	std::int_least16_t degrees{};
	std::uint_least8_t minutes{};
	std::uint_least8_t seconds{};
	std::uint_least64_t subseconds{};
};

struct iso8601_timestamp
{
	std::int_least64_t year{};
	std::uint_least8_t month{};
	std::uint_least8_t day{};
	std::uint_least8_t hours{};
	std::uint_least8_t minutes{};
	std::uint_least8_t seconds{};
	std::uint_least64_t subseconds{};
};

struct iso6709_coordinates
{
	iso6709_coordinate latitude{};
	iso6709_coordinate longtitude{};
};

struct tz_stdoff_t
{
	std::int_least64_t seconds;
	std::uint_least64_t subseconds;
};

template<typename char_type>
struct basic_tz_zone_tab_entry
{
	basic_io_scatter_t<char_type> code{};
	iso6709_coordinates coordinates{};
	basic_io_scatter_t<char_type> tz{};
	basic_io_scatter_t<char_type> comments{};
};

template<typename char_type>
struct basic_tz_zone1970_tab_entry
{
	basic_io_scatter_t<char_type> codes{};
	iso6709_coordinates coordinates{};
	basic_io_scatter_t<char_type> tz{};
	basic_io_scatter_t<char_type> comments{};
};

template<typename char_type>
struct basic_tz_iso3166_tab_entry
{
	basic_io_scatter_t<char_type> code{};
	basic_io_scatter_t<char_type> name{};
};

struct tz_leap_seconds_list_entry
{
	intiso_t unix_timestamp{};
	intiso_t offset{};
};

template<typename char_type>
struct basic_tz_backward_entry
{
	basic_io_scatter_t<char_type> target{};
	basic_io_scatter_t<char_type> link_name{};
};

template<typename char_type>
struct basic_tz_factory_entry
{
	basic_io_scatter_t<char_type> name{};
	tz_stdoff_t stdoff{};
	basic_io_scatter_t<char_type> rules{};
	basic_io_scatter_t<char_type> format{};
};

enum class tz_rule_on_category
{
day,first_weekday,last_weekday,weekday_less_equal_than,weekday_greater_equal_than,unknown
};

struct tz_rule_on_weekday_comparision
{
	std::uint_least8_t weekday{};//weekday starts at 1, ends at 7 following iso8601 rule
	std::uint_least8_t day_of_month{};
};

struct tz_rule_on
{
tz_rule_on_category category;
union
{
	std::uint_least8_t day;
	std::uint_least8_t first_weekday;
	std::uint_least8_t last_weekday;
	tz_rule_on_weekday_comparision weekday_less_than_equal;
	tz_rule_on_weekday_comparision weekday_greater_than_equal;
};
};

template<typename char_type>
struct basic_tz_rule_entry
{
	basic_io_scatter_t<char_type> name{};
	intiso_t from{};
	intiso_t to{};
	std::uint_least8_t in{};
	tz_rule_on on{};
	tz_stdoff_t at{};
	tz_stdoff_t save{};
	basic_io_scatter_t<char_type> letter_s{};
};

template<typename char_type>
struct basic_tz_zone_data
{
	tz_stdoff_t stdoff{};
	basic_io_scatter_t<basic_tz_rule_entry<char_type>> rules{};
	basic_io_scatter_t<char_type> format{};
	iso8601_timestamp until{};
};

template<typename char_type>
struct basic_tz_zone_entry
{
	basic_io_scatter_t<char_type> name{};
	basic_io_scatter_t<basic_tz_zone_data<char_type>> datas{};
	basic_io_scatter_t<std::size_t> links{};
	std::size_t linked{SIZE_MAX};
};

template<typename char_type>
struct basic_tz_all
{
	basic_io_scatter_t<char_type> version{};
	basic_io_scatter_t<basic_tz_backward_entry<char_type>> backward{};
	basic_io_scatter_t<basic_tz_zone_data<char_type>> datas{};
	basic_io_scatter_t<basic_tz_iso3166_tab_entry<char_type>> iso3166_tab{};
	basic_io_scatter_t<tz_leap_seconds_list_entry> leap_seconds_list{};
	basic_io_scatter_t<basic_tz_zone_tab_entry<char_type>> zone_tab{};
	basic_io_scatter_t<basic_tz_zone1970_tab_entry<char_type>> zone1970_tab{};
};
#if 0
using tz_all=basic_tz_all<char>;
using wtz_all=basic_tz_all<wchar_t>;
using u8tz_all=basic_tz_all<char8_t>;
using u16tz_all=basic_tz_all<char16_t>;
using u32tz_all=basic_tz_all<char32_t>;
#endif
struct tz_timezone
{
	basic_tz_all<char> const* all{};
	basic_tz_all<wchar_t> const* wall{};
	basic_tz_all<char8_t> const* u8all{};
	basic_tz_all<char16_t> const* u16all{};
	basic_tz_all<char32_t> const* u32all{};
};

template<typename char_type,std::size_t N>
inline constexpr basic_io_scatter_t<char_type> tsc(char_type const (&a)[N]) noexcept
{
	return {a,N-1};
}

template<typename char_type,std::size_t N>
inline constexpr basic_io_scatter_t<char_type> tac(char_type const (&a)[N]) noexcept
{
	return {a,N};
}

}