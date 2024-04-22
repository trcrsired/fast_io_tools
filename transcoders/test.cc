#include<concepts>
#include<string>
#include<string_view>
#include<iostream>
#if 0
template<::std::integral fromchtype,::std::integral tochtype>
struct transcode_result
{
	using from_char_type = fromchtype;
	using to_char_type = tochtype;
	from_char_type const* from_ptr;
	to_char_type* to_ptr;
	::std::size_t required_to_buffer_size;
};

struct transcode_bytes_result
{
	::std::byte const* from_ptr;
	::std::byte* to_ptr;
	::std::size_t required_to_buffer_size;
};

template<::std::integral chtype>
struct basic_lf_to_crlf
{
	using char_type = chtype;
	static inline constexpr char_type* transcode(char_type const* fromfirst,char_type const* fromlast,
			char_type* tofirst,char_type* tolast) noexcept
	{

	}
	static inline constexpr ::std::size_t transcode_max_size() noexcept
	{
		return 4;
	}
	static inline constexpr char_type* transcode_final(char_type* tofirst) noexcept
	{
		return tofirst;
	}
};
#else
template<::std::integral fromchtype,::std::integral tochtype>
struct transcode_result
{
	using from_char_type = fromchtype;
	using to_char_type = tochtype;
	from_char_type const* from_ptr;
	to_char_type* to_ptr;
};
#endif


template<::std::integral fromchtype,::std::integral tochtype>
struct transcode_result
{
	using from_char_type = fromchtype;
	using to_char_type = tochtype;
	from_char_type const* from_ptr;
	to_char_type* to_ptr;
};

template<::std::integral chtype>
struct basic_transcopy
{
	using char_type = chtype;
	using from_char_type = char_type;
	using to_char_type = char_type;
	static inline constexpr ::std::size_t transcode_size(char_type const* fromfirst,char_type const* fromlast, ::std::size_t imsize) noexcept
	{
		for(;fromfirst!=fromlast&&tofirst!=tolast;);
//		return static_cast<::std::size_t>(fromlast-fromfirst);
	}
	static inline constexpr transcode_result<from_char_type,to_char_type> transcode(char_type const* fromfirst,char_type const* fromlast,
			char_type* tofirst,char_type* tolast) noexcept
	{
		for(;fromfirst!=fromlast&&tofirst!=tolast;)
		{
			*tofirst=*fromfirst;
			++fromfirst;
			++tofirst;
		}
		return {fromfirst,tofirst};
	}
	static inline constexpr ::std::size_t transcode_eof_size(char_type const* fromfirst,char_type const* fromlast, ::std::size_t imsize) noexcept
	{
		return static_cast<::std::size_t>(fromlast-fromfirst);
	}
	static inline constexpr transcode_result<from_char_type,to_char_type> transcode_eof(char_type const* fromfirst,char_type const* fromlast,
			char_type* tofirst,char_type* tolast) noexcept
	{
		for(;fromfirst!=fromlast&&tofirst!=tolast;)
		{
			*tofirst=*fromfirst;
			++fromfirst;
			++tofirst;
		}
		return {fromfirst,tofirst};
	}
};

inline std::string test_transcode(::std::string_view strvw)
{
	using transcoder = basic_transcopy<char>;
	constexpr ::std::size_t mxsz{transcoder::transcode_max_size};
	auto strvwbg{strvw.data()};
	auto strvwed{strvwbg+strvw.size()};
	::std::string str;
	str.reserve(mxsz);
	for(transcoder tc;;)
	{
		auto [from_ptr, toptr] = tc.transcode(strvwbg,strvwed,buffer,buffer+mxsz);
		
	}
//	for(;;)
}

int main()
{

}