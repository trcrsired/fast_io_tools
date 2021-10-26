#include<fast_io.h>

namespace fast_io
{

class l10n2
{
public:
	using dll_file_type = dll_filetype;
	lc_locale loc;
	native_dll_file dll_file;
	constexpr l10n2() noexcept=default;
	template<::fast_io::constructible_to_os_c_str T>
	constexpr explicit l10n2(T const& t)
	{}
};

namespace details
{
inline constexpr char16_t i18n_char16_locale_prefix[]=u"fast_io_i18n_data_locale_";
inline constexpr char8_t i18n_char8_locale_prefix[]=u8"fast_io_i18n_data_locale_";

inline constexpr char16_t i18n_char16_tz_prefix[]=u"fast_io_i18n_data_tz_";
inline constexpr char8_t i18n_char8_tz_prefix[]=u8"fast_io_i18n_data_tz_";

inline constexpr std::size_t restricted_maximum_i18n_filename_len{20};

template<bool istz>
inline constexpr auto& get_i18n_prefix() noexcept
{
	if constexpr(sizeof(native_char_type)==sizeof(char16_t))
	{
		if constexpr(istz)
			return i18n_char16_tz_prefix;
		else
			return i18n_char16_locale_prefix;
	}
	else
	{
		if constexpr(istz)
			return i18n_char8_tz_prefix;
		else
			return i18n_char8_locale_prefix;
	}
}

inline constexpr auto const& get_i18n_exec_suffix() noexcept
{
	inline constexpr encoding_scheme scheme{::fast_io::details::execution_charset_encoding_scheme<native_char_type>()};
	if constexpr(sizeof(native_char_type)==sizeof(char16_t))
	{
		if constexpr(scheme==encoding::utf_ebcdic)
			return u".UTF-EBCDIC.so";
		else if constexpr(scheme==encoding::gb18030)
			return u".GB18030.so"
		else
			return u".UTF-8.so";
	}
	else
	{
		if constexpr(scheme==encoding::utf_ebcdic)
			return u8".UTF-EBCDIC.so";
		else if constexpr(scheme==encoding::gb18030)
			return u8".GB18030.so"
		else
			return u8".UTF-8.so";
	}
}

inline constexpr void sanitize_execuation_charset_case(native_char_type const* filename_c_str,std::size_t filename_c_str_len)
{
	if(filename_c_str_len>restricted_maximum_l10n_filename_len)
		throw_posix_error(EINVAL);
	for(auto i{filename_c_str},e{filename_c_str+filename_c_str_len};i!=e;++i)
	{
		if(*i==u8'\\'||*i==u8'/'||*i==u8'0'||*i==u8'.')
			throw_posix_error(EINVAL);
	}
}

template<bool istz>
inline void load_to_l10n2(l10n2& t,native_char_type const* cstr)
{
	t.dll_file=native_dll_file(::fast_io::mnp::os_c_str(cstr),::fast_io::dll_mode::posix_rtld_global|::fast_io::dll_mode::posix_rtld_now);
	
}

template<bool istz>
inline void create_l10n2_execuation_charset_case_common_case(l10n2& t,native_char_type const* filename_c_str,std::size_t filename_c_str_len)
{
	sanitize_execuation_charset_case(filename_c_str,filename_c_str_len);
	constexpr auto& prefix{get_prefix<is_tz>()};
	constexpr std::size_t no_nullterminated_prefix_size{cal_array_size(prefix)};
	constexpr auto& suffix{get_i18n_exec_suffix<is_tz>()};
	constexpr std::size_t allocated_size{no_nullterminated_prefix_size+(restricted_maximum_l10n_filename_len+1u)};
	native_char_type buffer[allocated_size];
	auto it{copy_string_literal(prefix,buffer)};
	it=non_overlapped_copy_n(filename_c_str,filename_c_str_len,it);
	*it=0;
	
}

inline native_char_type const* 

inline void create_l10n2_execuation_charset_case(l10n2& t,native_char_type const* filename_c_str,std::size_t filename_c_str_len)
{
	if(filename_c_str_len==0)
	{

	}
	else
	{

	}
}

struct l10n_execuation_charset_impl_context
{
	l10n2* context{};
	void operator()(native_char_type const* filename_c_str,std::size_t filename_c_str_len)
	{
		return create_l10n2_execuation_charset_case(*context,filename,filename_c_str_len);
	}
};

template<::fast_io::constructible_to_os_c_str T>
inline void create_l10n2_execuation_charset_case(l10n2& context,T const& t)
{
//	return 
}

}

}

int main()
{

}