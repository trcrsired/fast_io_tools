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
inline constexpr char16_t l10n_char16_nt_prefix[]=u"fast_io_i18n_data\\locale\\";
inline constexpr char8_t l10n_char8_9x_prefix[]=u8"fast_io_i18n_data\\locale\\";
inline constexpr char8_t l10n_char8_posix_prefix[]=u8"fast_io_i18n_data/locale/";

inline void create_l10n2_execuation_charset_case_common_case(l10n2& t,native_char_type const* filename_c_str,std::size_t filename_c_str_len)
{
	if(filename_c_str_len>20u)
		throw_posix_error(EINVAL);
	if constexpr(sizeof(native_char_type)==2)
	{
		constexpr std::size_t len{l10n_char16_nt_prefix};
//		constexpr char16_t prefix{};
	}
	else
	{

	}
}

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