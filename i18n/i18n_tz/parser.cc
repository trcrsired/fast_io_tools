struct coord
{
	bool negative{};
	std::u8string_view d,m,s;
};

inline constexpr void print_define(fast_io::io_reserve_type_t<char8_t,coord>,fast_io::buffer_output_stream auto bos,coord c)
{
	print(bos,u8"{");
	if(c.negative)
		print(bos,u8"-");
	if(c.d.empty())
		print(bos,u8"0");
	else
		print(bos,c.d);
	if(!c.m.empty())
		print(bos,u8",",c.m);
	else
		print(bos,u8",0");
	if(!c.s.empty())
		print(bos,u8",",c.s);
	else
		print(bos,u8",0");
	print(bos,u8",0}");
}

struct coord2
{
	coord longitute,latitute;
};


inline constexpr void print_define(fast_io::io_reserve_type_t<char8_t,coord2>,fast_io::buffer_output_stream auto bos,coord2 const& c)
{
	print(bos,u8"{",c.longitute,u8",",c.latitute,u8"}");
}

inline constexpr std::u8string_view remove_zeros(std::u8string_view sb) noexcept
{
	for(;!sb.empty()&&sb.front()==u8'0';sb.remove_prefix(1));
	return sb;
}

inline constexpr coord parse_coord(std::u8string_view total)
{
	if(total.size()<5)
		fast_io::throw_posix_error(EINVAL);
	if((total.front()!=u8'+')&&(total.front()!=u8'-'))
		fast_io::throw_posix_error(EINVAL);
	bool negative{total.front()==u8'-'};
	std::u8string_view d;
	std::u8string_view m;
	std::u8string_view s;
	switch(total.size())
	{
	case 5:
		d=total.substr(1,2);
		m=total.substr(3,2);
	break;
	case 6:
		d=total.substr(1,3);
		m=total.substr(4,2);
	break;
	case 7:
		d=total.substr(1,2);
		m=total.substr(3,2);
		s=total.substr(5,2);
	break;
	default:
		d=total.substr(1,3);
		m=total.substr(4,2);
		s=total.substr(6);
	}
	return {negative,remove_zeros(d),remove_zeros(m),remove_zeros(s)};
}


inline constexpr coord2 parse_geo_data(std::u8string_view total)
{
	if(total.size()<8)
		fast_io::throw_posix_error(EINVAL);
	auto it=std::find_if(total.cbegin()+1,total.cend(),[](char8_t ch)
	{
		return (ch==u8'+')|(ch==u8'-');
	});
	if(it==total.cend())
		fast_io::throw_posix_error(EINVAL);
	return {parse_coord(std::u8string_view(total.cbegin(),it)),
		parse_coord(std::u8string_view(it,total.cend()))};
}

template<std::size_t N,std::size_t special=0>
requires (sizeof(N)!=0)
inline auto read_general(fast_io::native_io_observer df,std::string_view filename)
{
	using namespace std::string_view_literals;
	fast_io::u8ibuf_file u8ibf(at(df),filename);
	std::vector<std::array<std::u8string,N>> data;
	for(std::u8string_view strvw:line_scanner(u8ibf))
	{
#if 0
		if(strvw.empty()||strvw.front()==u8'#')
		{
			if constexpr(special==1)
			{
				if(strvw!=u8"#Link	America/Regina		Canada/East-Saskatchewan"sv)
					continue;
			}
			else
				continue;
		}
		fast_io::u8ibuffer_view istrvw(strvw);
		if constexpr(special==2)
		{
			constexpr std::size_t nm1{N-1};
			auto& arr{data.emplace_back()};
			for(std::size_t i{};i!=nm1;++i)
			{
				scan(istrvw,arr[i]);
			}
			using namespace fast_io::manipulators;
			scan(istrvw,space_skipper{});
			fast_io::ostring_ref ref{arr.back()};
			transmit(ref,istrvw);
		}
		else
			for(auto& e : data.emplace_back())
				scan(istrvw,e);
#endif
	}
	return data;
}

inline std::u8string read_version(fast_io::native_io_observer df,std::string_view filename)
{
	fast_io::u8ibuf_file u8ibf(at(df),filename);
	std::u8string str;
	scan(u8ibf,str);
	return str;
}
inline void print_zone_tab_character(fast_io::u8obuf_file &obf,std::u8string_view literal_tag,std::u8string_view tab_name,std::span<std::array<std::u8string,4>> zone_tab)
{
	print(obf,u8"};\ninline constexpr basic_tz_",tab_name,u8"_entry<char_type> tz_",tab_name,u8"_global[]{");
	for(std::size_t i{};i!=zone_tab.size();++i)
	{
		if(i)
			print(obf,u8",");
		print(obf,u8"{tsc(",literal_tag,u8"\"",zone_tab[i][0],u8"\"),",
		parse_geo_data(zone_tab[i][1]),
		u8",tsc(",literal_tag,u8"\"",zone_tab[i][2],u8"\"),");
		if(zone_tab[i][3].empty())
			print(obf,u8"{}");
		else
			print(obf,u8"tsc(",literal_tag,u8"\"",zone_tab[i][3],u8"\")");
		print(obf,u8"}");
	}
}
#if 0
struct data_zone
{
	std::u8string zone;
	std::vector<std::array<std::u8string,4>>
};

inline void read_rule(fast_io::u8obuf_file& obf,std::u8string_view version)
{
}
#endif
inline void tz_generate_u8str(fast_io::u8obuf_file& obf,
	std::size_t tag,
	std::u8string_view version,
	std::span<std::array<std::u8string,3>> backward,
	std::span<std::array<std::u8string,2>> iso3166_tab,
	std::span<std::array<std::u8string,4>> zone_tab,
	std::span<std::array<std::u8string,4>> zone1970_tab)
{
	std::u8string_view char_type_tag;
	std::u8string_view literal_tag;
	std::u8string_view prefix_tag;
	switch(tag)
	{
	case 1:
	char_type_tag=u8"wchar_t";
	literal_tag=u8"L";
	prefix_tag=u8"w";
	break;
	case 2:
	char_type_tag=u8"char8_t";
	literal_tag=u8"u8";
	prefix_tag=u8"u8";
	break;
	case 3:
	char_type_tag=u8"char16_t";
	literal_tag=u8"u";
	prefix_tag=u8"u16";
	break;
	case 4:
	char_type_tag=u8"char32_t";
	literal_tag=u8"U";
	prefix_tag=u8"u32";
	break;
	default:
	char_type_tag=u8"char";
	};
	print(obf,u8"namespace ",prefix_tag,u8"tz_data_namespace\n{\nusing char_type = ",char_type_tag,u8";\ninline constexpr basic_tz_backward_entry<char_type> tz_backward_global[]{");
	for(std::size_t i{};i!=backward.size();++i)
	{
		if(i)
			print(obf,u8",");
		print(obf,u8"{tsc(",literal_tag,u8"\"",backward[i][1],u8"\"),tsc(",literal_tag,u8"\"",backward[i][2],u8"\")}");
	}
	print(obf,u8"};\ninline constexpr basic_tz_iso3166_tab_entry<char_type> iso3166_tab_global[]{");
	for(std::size_t i{};i!=iso3166_tab.size();++i)
	{
		if(i)
			print(obf,u8",");
		print(obf,u8"{tsc(",literal_tag,u8"\"",iso3166_tab[i][0],u8"\"),tsc(",literal_tag,u8"\"",iso3166_tab[i][1],u8"\")}");
	}
	print_zone_tab_character(obf,literal_tag,u8"zone_tab",zone_tab);
	print_zone_tab_character(obf,literal_tag,u8"zone1970_tab",zone1970_tab);
	print(obf,u8"};\ninline constexpr basic_tz_all<char_type> tz_all_global{tsc(",literal_tag,u8"\"",version,u8"\"),tac(tz_backward_global),{},tac(iso3166_tab_global),tac(tz_leap_seconds_list_global),tac(tz_zone_tab_global),tac(tz_zone1970_tab_global)};\n}\n\n");
}

int main(int argc,char** argv)
{
	if (argc < 3)
	{
		if (argc == 0)
		{
			return 1;
		}
		perrln("Usage: ",fast_io::mnp::os_c_str(*argv)," <tzdata dir> <tz result>");
		return 1;
	}
	fast_io::dir_file df(fast_io::mnp::os_c_str(argv[1]));
	std::u8string version{read_version(df,"version")};
	auto backward{read_general<3,1>(df,"backward")};
	auto iso3166_tab{read_general<2>(df,"iso3166.tab")};
	auto leap_seconds_list{read_general<2>(df,"leap-seconds.list")};
	auto zone_tab{read_general<4,2>(df,"zone.tab")};
	auto zone1970_tab{read_general<4,2>(df,"zone1970.tab")};
	fast_io::dir_file tz_result_df(fast_io::mnp::os_c_str(argv[2]));
	{
	fast_io::u8obuf_file u8obf(at(tz_result_df),"C.cc");
	print(u8obf,u8R"(#include"../tzdef.h"

namespace fast_io_i18n
{
namespace
{

inline constexpr tz_leap_seconds_list_entry tz_leap_seconds_list_global[]{)");
	for(std::size_t i{};i!=leap_seconds_list.size();++i)
	{
		if(i)
			print(u8obf,u8",");
		print(u8obf,u8"{",leap_seconds_list[i][0],u8",",leap_seconds_list[i][1],u8"}");
	}
	print(u8obf,u8"};\n");
	for(std::size_t i{};i!=5;++i)
		tz_generate_u8str(u8obf,i,version,backward,iso3166_tab,zone_tab,zone1970_tab);
print(u8obf,u8R"(}
}
)");
	}
	{
	fast_io::native_file inv(at(tz_result_df),"C.cc",fast_io::open_mode::in);
	fast_io::native_file onv(at(tz_result_df),"POSIX.cc",fast_io::open_mode::out);
	transmit(onv,inv);
	}
}
