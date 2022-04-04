#pragma once

namespace fast_io_i18n
{

inline void output_era_year_month_day(fast_io::buffer_output_stream auto& storage,std::string_view year_month_day_str,bool is_end)
{
	using namespace std::string_view_literals;
	std::string_view s;
	if(is_end)
		s="end"sv;
	else
		s="start"sv;
	auto y{year_month_day_str.find('/')};
	if(y==std::string_view::npos)
		panicln("incorrect ymd str",year_month_day_str);
	std::size_t y_start{};
	bool is_negative{};
	if(y_start!=year_month_day_str.size()&&year_month_day_str[y_start]=='-')
	{
		++y_start;
		is_negative=true;
	}
	for(;y_start+1<y&&year_month_day_str[y_start]=='0';++y_start);
	print(storage,",.",s,"_date_year=");
	if(is_negative)
		print(storage,"-");
	write(storage,year_month_day_str.cbegin()+y_start,year_month_day_str.cbegin()+y);
	for(;y!=year_month_day_str.size()&&year_month_day_str[y]=='/';++y);
	auto m{year_month_day_str.find('/',y)};
	if(m==std::string_view::npos)
		panicln("incorrect ymd str",year_month_day_str);
	for(;y+1<m&&year_month_day_str[y]=='0';++y);
	print(storage,",.",s,"_date_month=",std::string_view(year_month_day_str.data()+y,year_month_day_str.data()+m));
	for(;m!=year_month_day_str.size()&&year_month_day_str[m]=='/';++m);
	for(;m+1<year_month_day_str.size()&&year_month_day_str[m]=='0';++m);
	print(storage,",.",s,"_date_day=",std::string_view(year_month_day_str.cbegin()+m,year_month_day_str.cend()));
}

inline void output_single_era_split(fast_io::buffer_output_stream auto& storage,std::string_view era_string,std::string_view string_prefix)
{
	std::vector<std::size_t> v;
	v.reserve(5);
	if(era_string.size()>2&&era_string.front()=='\"'&&era_string.back()=='\"')
	{
		era_string.remove_prefix(1);
		era_string.remove_suffix(1);
	}
	for(std::size_t i{};i!=era_string.size();++i)
	{
		if(era_string[i]==':')
			v.emplace_back(i);
	}
	if(v.size()<5)
		panicln("incorrect era string: ",era_string);
	if(era_string.front()=='+')
		print(storage,"{.direction=true,");
	else if(era_string.front()=='-')
		print(storage,"{");
	else
		panicln("incorrect era string: ",era_string);
	std::string_view offset_str{era_string.data()+v.front()+1,era_string.data()+v[1]};
	if(!offset_str.empty())
		print(storage,".offset=",offset_str);
	output_era_year_month_day(storage,std::string_view(era_string.data()+v[1]+1,era_string.data()+v[2]),false);

	std::string_view end_str{era_string.data()+v[2]+1,era_string.data()+v[3]};
	if(end_str=="+*")
		print(storage,",.end_date_special=1");
	else if(end_str=="-*")
		print(storage,",.end_date_special=-1");
	else
		output_era_year_month_day(storage,end_str,true);
	std::string_view era_name_str{era_string.data()+v[3]+1,era_string.data()+v[4]};
	if(!era_name_str.empty())
		print(storage,",.era_name=tsc(",string_prefix,"\"",era_name_str,"\")");
	std::string_view era_format_str{era_string.data()+v[4]+1,era_string.data()+era_string.size()};
	if(!era_format_str.empty())
		print(storage,",.era_format=tsc(",string_prefix,"\"",era_format_str,"\")");
	print(storage,",.era=tsc(",string_prefix,"\"",era_string,"\")}");
}

}