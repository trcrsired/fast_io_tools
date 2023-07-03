#pragma once

namespace fast_io
{

template<std::integral char_type,std::uint_least8_t N>
struct code_cvt_unfinished_states
{
	char8_t buffer[N]{};
	::std::uint_least8_t remained{};
};

namespace details
{

template<std::integral char_type,std::integral to_char_type,std::uint_least8_t N,typename F>
inline constexpr deco_result_t<char_type,to_char_type> codecvtremain(code_cvt_unfinished_states<char_type,N>& cvt,
					char_type const* fromfirst,
					char_type const* fromlast,
					to_char_type* tofirst,
					to_char_type* tolast,
					F fn)
{
	auto remn{cvt.remained};
	if(remn!=0)
	{
		::std::size_t diff{static_cast<::std::size_t>(fromlast-fromfirst)};
		::std::size_t remnspace{N-remn};
		if(remnspace<diff)
		{			
		}
	}
	auto [fromit,toit] = fn(fromfirst,fromlast,tofirst,tolast);
	if(toit==tolast)
	{
		return {fromit,toit};
	}
	cvt.remained=
		static_cast<::std::uint_least8_t>(
			::fast_io::details::non_overlapped_copy(fromit,fromlast,cvt.buffer)-cvt.buffer);
	return {fromlast,toit};
}

}

}
