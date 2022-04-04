#pragma once

#include"era.h"

namespace fast_io_i18n
{

inline char get_comment_or_escaping_character(std::string_view stvw,std::size_t size)
{
	auto substr{stvw.substr(size)};
	for(auto const e : substr)
		if(!fast_io::char_category::is_c_space(e))
			return e;
	panic(stvw," illegal\n");
}

inline void seperating(char comment_char,std::string_view line,std::unordered_map<std::string,std::string>& ret_unordered,std::string const& filename)
{
	auto it{line.cbegin()};
	for(;it!=line.cend()&&!fast_io::char_category::is_c_space(*it);++it);
	auto ed{line.cend()};
	for(;ed!=line.cbegin();--ed)
		if(ed[-1]=='\"')
		{
			ed=line.cend();
			break;
		}
		else if(ed[-1]==comment_char)
		{
			for(--ed;ed!=line.cbegin()&&fast_io::char_category::is_c_space(ed[-1]);--ed);
			
			break;
		}
	if(ed==line.cbegin())
		ed=line.cend();
	if(ed<it)
		panic("in file:",filename," ",line," illegal\n");
	line=line.substr(0,static_cast<std::size_t>(ed-line.cbegin()));
	std::string key(line.cbegin(),it);
	for(;it!=line.cend()&&fast_io::char_category::is_c_space(*it);++it);
	if(line.cend()-it<=2||(*it!='\"'||line.back()!='\"'))
	{
		ret_unordered.try_emplace(key,it,line.cend());
		return;
	}
	std::string_view value_str(it+1,line.cend()-1);
	{
		auto i{value_str.cbegin()};
		for(;i!=value_str.cend()&&*i!='<';++i);
		if(i==value_str.cend())
		{
			ret_unordered.try_emplace(key,it,line.cend());
			return;
		}
	}
	std::string new_value_str;
	new_value_str.reserve(static_cast<std::size_t>(static_cast<std::size_t>(line.cend()-it)*4u)+2);
	fast_io::ostring_ref new_value_str_ref{__builtin_addressof(new_value_str)};
	auto curr_ptr(obuffer_curr(new_value_str_ref));
	*curr_ptr='\"';
	++curr_ptr;
	for(auto i{value_str.cbegin()};i!=value_str.cend();++i)
	{
		if(*i=='<'&&(3<value_str.cend()-i))
		{
			if(i[1]!='U')
				panicln("in file:",filename," unknown character:",i[1]);
			auto j{i+2};
			for(;j!=value_str.cend()&&*j!='>';++j);
			if(j==value_str.cend())
				panic("in file:",filename," ",line," illegal\n");
			auto ip2{i+2};
			for(;ip2!=j;)
				if(*ip2=='0')
				{
					if(ip2+1==j)
						break;
					++ip2;
				}
				else
					break;
			fast_io::ibuffer_view isv(ip2,j);
			char32_t ch{};
			scan(isv,fast_io::manipulators::hex_get(ch));
			curr_ptr+=fast_io::get_utf_code_units(ch,std::to_address(curr_ptr));
			i=j;
		}
		else
		{
			*curr_ptr=*i;
			++curr_ptr;
		}
	}
	*curr_ptr='\"';
	++curr_ptr;
	obuffer_set_curr(new_value_str_ref,curr_ptr);
	ret_unordered.try_emplace(key,std::move(new_value_str));
}


struct escape_char_paramter
{
	char escape_char{};
	bool has_escaped{};
};

template<std::random_access_iterator Iter>
inline Iter find_next_line(Iter first,Iter last,std::string& line_str_buffer,escape_char_paramter escape_char)
{
	line_str_buffer.clear();
	for(auto line_start{first},it{first};it!=last;)
	{
		for(;it!=last&&*it!='\n';++it);
		if(!escape_char.has_escaped)
		{
			return it;
		}
		if(it==last||it[-1]!=escape_char.escape_char)
		{
			if(line_start!=first)
				line_str_buffer.append(line_start,it);
			return it;
		}
		line_str_buffer.append(line_start,it-1);
		++it;
		line_start=it;
	}
	return last;
}

inline std::unordered_map<std::string,std::string>
	parsing_file_search(fast_io::native_io_observer df,std::string const& filename,
	std::unordered_map<std::string,std::unordered_map<std::string,std::unordered_map<std::string,std::string>>>& cache,
	std::string const& find_category)
{
	auto cached_data{cache.find(filename)};
	if(cached_data!=cache.cend())
	{
		auto i{cached_data->second.find(find_category)};
		if(i!=cached_data->second.cend())
			return i->second;
	}

	fast_io::native_file_loader fmp(at(df),filename);
	fast_io::ibuffer_view imm(fmp);
	bool skipping_lines{};
	std::string current_category;
	std::unordered_map<std::string,std::string> ret_unordered{};
	bool escaping{};
	std::string str;
	char comment_char{'%'};
	char escape_char{'/'};
	bool has_escaped{};
	std::string escape_char_buffer;
	for(auto nit(fmp.cbegin());nit!=fmp.cend();)
	{
		auto next_line_it{find_next_line(nit,fmp.cend(),escape_char_buffer,{escape_char,has_escaped})};
		std::string_view line(nit,next_line_it);
		if(!escape_char_buffer.empty())
			line=escape_char_buffer;
		nit=next_line_it;
		if(nit!=fmp.cend())
			++nit;
		if(line.empty())
			continue;
		if(line.size()>13&&line.substr(0,13)=="comment_char ")
		{
			comment_char=get_comment_or_escaping_character(line,13);
			continue;
		}
		if(line.size()>12&&line.substr(0,12)=="escape_char ")
		{
			escape_char=get_comment_or_escaping_character(line,12);
			has_escaped=true;
			continue;
		}
		if(!escaping&&line.front()==comment_char)
			continue;
		if(line.starts_with("LC_"))
		{
			std::size_t i{};
			for(;i!=line.size()&&((!fast_io::char_category::is_c_space(line[i]))&(line[i]!=comment_char));++i);
			line=line.substr(0,i);
			if(line!=find_category)
				skipping_lines=true;
			continue;
		}
		if(9<line.size()&&line.substr(0,9)=="category ")
			continue;
		if(4<line.size()&&line.substr(0,4)=="END ")
		{
			if(current_category==find_category)
				return ret_unordered;
			skipping_lines=false;
			continue;
		}
		if(skipping_lines)
			continue;
		if(line.back()==escape_char)
		{
			str.append(line.cbegin(),line.cend()-1);
			escaping=true;
			continue;
		}
		struct clean
		{
			std::string* ptr{};
			constexpr clean() noexcept=default;
			clean(clean const&)=delete;
			clean& operator=(clean const&)=delete;
			constexpr ~clean()
			{
				if(ptr)
					ptr->clear();
			}
		};
		clean cleaner;
		if(escaping)
		{
			str.append(line.cbegin(),line.cend());
			escaping=false;
			cleaner.ptr=std::addressof(str);
			line=str;
		}
		if(5<line.size()&&line.substr(0,5)=="copy ")
		{
			auto it{line.cbegin()+5};
			for(;fast_io::char_category::is_c_space(*it);++it);
			auto subvw{line.substr(static_cast<std::size_t>(it-line.cbegin()))};
			if(subvw.size()<2||subvw.front()!='\"'||subvw.back()!='\"')
				panic("in file:",filename," ",line," ",current_category," illegal\n");
			std::string new_filename(subvw.substr(1,subvw.size()-2));
			return parsing_file_search(df,new_filename,cache,find_category);
		}
		seperating(comment_char,line,ret_unordered,filename);
	}
	return ret_unordered;

}

inline std::unordered_map<std::string,std::unordered_map<std::string,std::string>> const&
	parsing_file(fast_io::native_io_observer df,std::string const& filename,
	std::unordered_map<std::string,std::unordered_map<std::string,std::unordered_map<std::string,std::string>>>& cache)
{
	auto cached_data{cache.find(filename)};
	if(cached_data!=cache.cend())
		return cached_data->second;
	fast_io::native_file_loader fmp(at(df),filename);
	bool skipping_lines{};
	std::string current_category;
	std::unordered_map<std::string,std::unordered_map<std::string,std::string>> unord;
	std::unordered_map<std::string,std::string>* ptr{};
	bool escaping{};
	std::string str;
	char comment_char{'%'};
	char escape_char{'/'};
	bool has_escaped{};
	std::string escape_char_buffer;
	for(auto nit(fmp.cbegin());nit!=fmp.cend();)
	{
		auto next_line_it{find_next_line(nit,fmp.cend(),escape_char_buffer,{escape_char,has_escaped})};
		std::string_view line(nit,next_line_it);
		if(!escape_char_buffer.empty())
			line=escape_char_buffer;
		nit=next_line_it;
		if(nit!=fmp.cend())
			++nit;
		if(line.empty())
			continue;
		if(line.size()>13&&line.substr(0,13)=="comment_char ")
		{
			comment_char=get_comment_or_escaping_character(line,13);
			continue;
		}
		if(line.size()>12&&line.substr(0,12)=="escape_char ")
		{
			escape_char=get_comment_or_escaping_character(line,12);
			has_escaped=true;
			continue;
		}
		if(!escaping&&line.front()==comment_char)
			continue;
		if(line=="LC_COLLATE"||line=="LC_CTYPE")
		{
			skipping_lines=true;
			continue;
		}
		if(9<line.size()&&line.substr(0,9)=="category ")
			continue;
		if(3<line.size()&&line.substr(0,3)=="LC_")
		{
			std::size_t i{};
			for(;i!=line.size()&&((!fast_io::char_category::is_c_space(line[i]))&(line[i]!=comment_char));++i);
			line=line.substr(0,i);
			current_category=line;
			ptr=std::addressof(unord.try_emplace(current_category).first->second);
			continue;
		}
		if(4<line.size()&&line.substr(0,4)=="END ")
		{
			skipping_lines=false;
			continue;
		}
		if(skipping_lines)
			continue;
		if(line.back()==escape_char)
		{
			str.append(line.cbegin(),line.cend()-1);
			escaping=true;
			continue;
		}
		struct clean
		{
			std::string* ptr{};
			constexpr clean() noexcept=default;
			clean(clean const&)=delete;
			clean& operator=(clean const&)=delete;
			constexpr ~clean()
			{
				if(ptr)
					ptr->clear();
			}
		};
		clean cleaner;
		if(escaping)
		{
			str.append(line.cbegin(),line.cend());
			escaping=false;
			cleaner.ptr=std::addressof(str);
			line=str;
		}
		if(5<line.size()&&line.substr(0,5)=="copy ")
		{
			auto it{line.cbegin()+5};
			for(;fast_io::char_category::is_c_space(*it);++it);
			auto subvw{line.substr(static_cast<std::size_t>(it-line.cbegin()))};
			if(subvw.size()<2||subvw.front()!='\"'||subvw.back()!='\"')
				panic("in file:",filename," ",line," ",current_category," illegal\n");
			std::string new_filename(subvw.substr(1,subvw.size()-2));
			*ptr=parsing_file_search(df,new_filename,cache,current_category);
			continue;
		}
		seperating(comment_char,line,*ptr,filename);
	}
	return cache.try_emplace(filename,std::move(unord)).first->second;
}

inline std::vector<std::string_view> string_view_splits(std::string_view strv)
{
	std::vector<std::string_view> vec;
	auto it{strv.cbegin()};
	for(;it!=strv.cend()&&fast_io::char_category::is_c_space(*it);++it);
	if(it==strv.cend())
		return {};
	if(*it=='\"')
	{
		for(auto i{it};i!=strv.cend();)
		{
			for(;i!=strv.cend()&&*i!='\"';++i);
			if(i==strv.cend())
				break;
			auto j{i+1};
			if(j==strv.cend())
				break;
			for(;j!=strv.cend()&&*j!='\"';++j);
			if(j==strv.cend())
				panic("unknown strv:",strv);
			vec.emplace_back(i,j+1);
			i=j+1;
		}
	}
	else
	{
		for(;it!=strv.cend();)
		{
			auto j{it};
			for(;it!=strv.cend()&&fast_io::char_category::is_c_space(*it);++it);
			if(it==strv.cend())
				break;
			for(;j!=strv.end()&&*j!=';';++j);
			vec.emplace_back(it,j);
			if(j==strv.end())
				break;
			it=j+1;
		}
	}
	return vec;
}

inline void log_data(std::unordered_map<std::string,std::unordered_map<std::string,std::string>> const& mp,fast_io::obuf_file& obf)
{
	bool first_category{true};
	for(auto const& e : mp)
	{
		if(first_category)
			first_category=false;
		else
			println(obf);
		println(obf,"category: ",e.first);
		for(auto const & e1 : e.second)
		{
			auto splits{string_view_splits(e1.second)};
			print(obf,e1.first," -> ");
			if(splits.size()==1)
				println(obf,splits.front());
			else
			{
				print(obf,"[",splits.size(),"]\n");
				for(auto const & s:splits)
					println(obf,"\t",s);
			}
		}
	}
}

inline std::vector<std::pair<std::string,std::vector<std::string>>> list_all_items(
	std::unordered_map<std::string,std::unordered_map<std::string,std::unordered_map<std::string,std::string>>> const& mp)
{
	std::unordered_map<std::string,std::unordered_set<std::string>> all_items_map;
	for(auto const& e2: mp)
		for(auto const& e : e2.second)
		{
			auto& category_map{all_items_map[e.first]};
			for(auto const & e1 : e.second)
				category_map.emplace(e1.first);
		}
	std::vector<std::pair<std::string,std::vector<std::string>>> all_items;
	all_items.reserve(all_items_map.size());
	for(auto const& e : all_items_map)
		std::ranges::sort(all_items.emplace_back(std::piecewise_construct,
			std::forward_as_tuple(e.first),
			std::forward_as_tuple(e.second.cbegin(),e.second.cend())).second);
	std::ranges::sort(all_items);
	return all_items;
}

inline void list_all_category(std::vector<std::pair<std::string,std::vector<std::string>>> const& mp,fast_io::buffer_output_stream auto& obf)
{
	bool first_category{true};
	for(auto const& e : mp)
	{
		if(first_category)
			first_category=false;
		else
			println(obf);
		println(obf,"category: ",e.first);
		for(auto const& e1 : e.second)
			println(obf,e1);
	}
}

inline void output_splits(std::span<std::string_view> splits,fast_io::buffer_output_stream auto& storage,std::string_view string_prefix)
{
	if(splits[0][0]=='\"')
	{
		for(auto i{splits.begin()};i!=splits.end();++i)
		{
			if(i!=splits.begin())
				print(storage,",");
			print(storage,"tsc(",string_prefix,*i,")");
		}
	}
	else
	{
		for(auto i{splits.begin()};i!=splits.end();++i)
		{
			if(i!=splits.begin())
				print(storage,",");
			print(storage,*i);
		}
	}
}

inline void output_era_splits(std::span<std::string_view> splits,fast_io::buffer_output_stream auto& storage,std::string_view string_prefix)
{
	for(auto i{splits.begin()};i!=splits.end();++i)
	{
		if(i!=splits.begin())
			print(storage,",");
		output_single_era_split(storage,*i,string_prefix);
	}
}

inline void grouping_compress(std::vector<std::string_view>& grouping)
{
	if(1<grouping.size())
	{
		std::size_t i{1};
		for(;i<grouping.size();++i)
		{
			if(grouping[i]!=grouping[0])
				break;
		}
		if(i==grouping.size())
		{
			auto sv{grouping.front()};
			grouping.clear();
			grouping.push_back(sv);
		}
	}
	if(grouping.size()==1&&(grouping[0]=="-1"||grouping[0]=="0"))
	{
		grouping.clear();
		return;
	}

}

inline void minus_1_to_largest_compress(std::vector<std::string_view>& grouping)
{
	constexpr std::string_view grouping_negative("SIZE_MAX");
	for(auto& e : grouping)
		if(e=="-1")
			e=grouping_negative;
}

inline void output_single_item(
	std::string_view cat,
	std::string_view item_name,
	std::unordered_map<std::string,std::string> const& mp,
	std::unordered_map<std::string,std::unordered_map<std::string,std::string>> const& mp2,
	fast_io::buffer_output_stream auto& obf,
	fast_io::buffer_output_stream auto& storage,
	std::string_view prefix,std::string_view string_prefix,std::string_view char_type_name,
	bool& first)
{
	auto fd{mp.find(std::string{item_name})};
	if(fd==mp.cend())
		return;
	auto splits{string_view_splits(fd->second)};
	if(splits.empty())
		return;
	bool is_grouping{item_name=="grouping"||item_name=="mon_grouping"};
	if(is_grouping)
	{
		grouping_compress(splits);
		if(splits.empty())
			return;
	}
	minus_1_to_largest_compress(splits);
	if(!first)
		print(obf,",");
	print(obf,".",item_name,"=");
	if(!is_grouping&&splits.size()==1&&item_name!="era")
	{
		auto split{splits[0]};
		if(split[0]=='\"')
		{
			if(cat=="identification")
			{
				if(item_name=="source")
				{
					if(split=="\"\"")
						print(obf,"tsc(",string_prefix,"\"fast_io\")");
					else
						print(obf,"tsc(",string_prefix,"\"",split.substr(1,split.size()-2),"\\t\\t;\\t\\t","fast_io\")");
					first=false;
					return;
				}
				else if(item_name=="contact")
				{
					if(split=="\"\"")
						print(obf,"tsc(",string_prefix,"\"fast_io\")");
					else
						print(obf,"tsc(",string_prefix,"\"",split.substr(1,split.size()-2),"\\t\\t;\\t\\t","fast_io\")");
					first=false;
					return;
				}
				else if(item_name=="email")
				{
					if(split=="\"\"")
						print(obf,"tsc(",string_prefix,"\"euloanty@live.com\")");
					else
						print(obf,"tsc(",string_prefix,"\"",split.substr(1,split.size()-2),";euloanty@live.com\")");
					first=false;
					return;						
				}
				else if(item_name=="address")
				{
					if(split=="\"\"")
					{
						print(obf,"tsc(",string_prefix,"\"https://gitee.com/qabeowjbtkwb/fast_io\\t\\t;\\t\\thttps://github.com/cppfastio/fast_io\")");
					}
					else
					{
						std::string str;
						for(std::size_t i{1};i+1<split.size();)
						{
							if(i+2<split.size())
							{
								if(split[i]=='/'&&split[i+1]=='/')
								{
									str.push_back('/');
									i+=2;
									continue;
								}
							}
							str.push_back(split[i]);
							++i;
						}
						print(obf,"tsc(",string_prefix,"\"",str,"\\t\\t;\\t\\thttps://gitee.com/qabeowjbtkwb/fast_io\\t\\t;\\t\\thttps://github.com/cppfastio/fast_io\")");
					}
					first=false;
					return;
				}
			}
			print(obf,"tsc(",string_prefix,split,")");
		}
		else if(cat=="address")
		{
			if(item_name=="country_num")
			{
				for(;(!split.empty())&&(split.front()=='0');split=split.substr(1));
				if(split.empty())
					print(obf,"0");
				else
					print(obf,split);
			}
			else if(item_name=="country_isbn")
			{
				if(split[0]=='\"')
					print(obf,"tsc(",string_prefix,split,")");
				else
					print(obf,"tsc(",string_prefix,"\"",split,"\")");
			}
			else
				print(obf,split);
		}
		else
			print(obf,split);
	}
	else
	{
		bool is_era{item_name=="era"};
		if(is_grouping||is_era||item_name=="alt_digits"||item_name=="timezone")
		{
			if(is_grouping)
			{
				if(item_name=="mon_grouping")
				{
					auto iden{mp2.find("LC_NUMERIC")};
					if(iden!=mp2.cend())[[likely]]
					{
						auto grouping{iden->second.find("grouping")};
						if(grouping!=iden->second.cend())[[likely]]
						{
							auto grouping_splits{string_view_splits(grouping->second)};
							grouping_compress(grouping_splits);
							minus_1_to_largest_compress(grouping_splits);
							if(grouping_splits==splits)
							{
								print(obf,"{numeric_grouping_storage,",splits.size(),"}");
								first=false;
								return;
							}
						}
					}
				}
			}
			print(obf,"{");
			if(splits[0][0]=='\"')
				print(obf,prefix);
			print(obf,cat,"_",item_name,"_storage,",splits.size(),"}");
			if(splits[0][0]=='\"'||prefix.empty())
			{
				print(storage,"inline constexpr ");
				if(splits[0][0]=='\"')
				{
					if(is_era)
						print(storage,"basic_lc_time_era<",char_type_name,"> ");		
					else
						print(storage,"basic_io_scatter_t<",char_type_name,"> ");
				}
				else
					print(storage,"std::size_t ");
				print(storage,prefix,cat,"_",item_name,"_storage[]{");
				if(is_era)[[unlikely]]
					output_era_splits(splits,storage,string_prefix);
				else
					output_splits(splits,storage,string_prefix);
				print(storage,"};\n\n");
			}
		}
		else
		{
			print(obf,"{");
			output_splits(splits,obf,string_prefix);
			print(obf,"}");
		}
	}
	first=false;
}

inline void output_identification(std::unordered_map<std::string,std::unordered_map<std::string,std::string>> const& mp,
	fast_io::buffer_output_stream auto& obf,
	fast_io::buffer_output_stream auto& storage,
	std::string_view prefix,std::string_view string_prefix,std::string_view char_type_name,
	std::string const& lc_cat,std::string_view cat,std::span<std::string_view const> span,
	bool& first,std::string_view name)
{
	auto it(mp.find(lc_cat));
	if(it==mp.cend())
		return;
	if(!first)
		print(obf,",");
	auto& second(it->second);
	print(obf,".",cat,"={");
	bool local_first{true};
	if(cat=="identification")
	{
		print(obf,".name=tsc(",string_prefix,"\"",name,"\"),.encoding=tsc(FAST_IO_LOCALE_",string_prefix,"ENCODING)");
		local_first = false;
	}
	for(auto const& e : span)
		output_single_item(cat,e,second,mp,obf,storage,prefix,string_prefix,char_type_name,local_first);
	print(obf,"}");
	first=false;
}

inline void output_single_char_type(std::unordered_map<std::string,std::unordered_map<std::string,std::string>> const& mp,
	fast_io::buffer_output_stream auto& obf,
	std::string_view prefix,std::string_view string_prefix,std::string_view char_type_name,std::string_view name)
{
	std::string str;
	str.reserve(100000);
	fast_io::ostring_ref ostrref{__builtin_addressof(str)};
	print(ostrref,"inline constexpr ",prefix,"lc_all ",prefix,"lc_all_global{");
	bool first{true};
	{
		constexpr std::string_view array[]{"title","source","address",
		"contact","email","tel","fax","language","territory","audience","application",
		"abbreviation","revision","date"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_IDENTIFICATION","identification",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"int_curr_symbol","currency_symbol",
		"mon_decimal_point","mon_thousands_sep","mon_grouping","positive_sign",
		"negative_sign","int_frac_digits","frac_digits","p_cs_precedes","p_sep_by_space",
		"n_cs_precedes","n_sep_by_space","int_p_cs_precedes","int_p_sep_by_space",
		"int_n_cs_precedes","int_n_sep_by_space","p_sign_posn","n_sign_posn",
		"int_p_sign_posn","int_n_sign_posn"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_MONETARY","monetary",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"decimal_point","thousands_sep","grouping"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_NUMERIC","numeric",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"abday","day","abmon","ab_alt_mon","mon","d_t_fmt","d_fmt","t_fmt","t_fmt_ampm","date_fmt","am_pm","era","era_d_fmt","era_d_t_fmt","era_t_fmt","alt_digits","week","first_weekday","first_workday","cal_direction","timezone"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_TIME","time",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"yesexpr","noexpr","yesstr","nostr"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_MESSAGES","messages",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"width","height"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_PAPER","paper",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"tel_int_fmt","tel_dom_fmt","int_select","int_prefix"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_TELEPHONE","telephone",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"name_fmt","name_gen","name_miss","name_mr","name_mrs","name_ms"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_NAME","name",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"postal_fmt","country_name","country_post","country_ab2","country_ab3","country_num","country_car","country_isbn","lang_name","lang_ab","lang_term","lang_lib"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_ADDRESS","address",std::span{array},first,name);
	}
	{
		constexpr std::string_view array[]{"measurement"};
		output_identification(mp,ostrref,obf,prefix,string_prefix,char_type_name,"LC_MEASUREMENT","measurement",std::span{array},first,name);
	}
	print(obf,str,"};\n\n");
}

inline void output_result(std::unordered_map<std::string,std::unordered_map<std::string,std::string>> const& mp,fast_io::buffer_output_stream auto& obf,std::string_view name)
{
	print(obf,
R"abc(#include"../localedef.h"

namespace fast_io_i18n
{
namespace
{

)abc");
	output_single_char_type(mp,obf,"","","char",name);
	output_single_char_type(mp,obf,"w","L","wchar_t",name);
	output_single_char_type(mp,obf,"u8","u8","char8_t",name);
	output_single_char_type(mp,obf,"u16","u","char16_t",name);
	output_single_char_type(mp,obf,"u32","U","char32_t",name);
	print(obf,R"abc(
}
}

#include"../main.h")abc");
}

}
