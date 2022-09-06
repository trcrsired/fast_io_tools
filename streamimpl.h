#pragma once

namespace fast_io
{
namespace details
{
template<buffer_output_stream stm,
	typename Traits = ::std::char_traits<typename stm::char_type>>
requires (::std::is_trivially_copyable_v<stm>&&value_based_stream<stm>)
class basic_streambuf_output_binder final:public std::basic_streambuf<typename stm::char_type,Traits>
{
public:
	using native_handle_type = stm;
	using char_type = typename stm::char_type;
	using traits_type = Traits;
	using int_type = typename traits_type::int_type;
	native_handle_type ref;
	explicit basic_streambuf_output_binder(native_handle_type t): ref(t)
	{
		this->update_buffer_ptrs();
	}
	basic_streambuf_output_binder(basic_streambuf_output_binder const&) = delete;
	basic_streambuf_output_binder& operator=(basic_streambuf_output_binder const&) = delete;
	~basic_streambuf_output_binder()
	{
		obuffer_set_curr(ref,this->pptr());
	}
private:
	void update_buffer_ptrs()
	{
		char_type* currptr=obuffer_curr(ref);
		char_type* endptr=obuffer_end(ref);
		if constexpr(noline_buffer_output_stream<native_handle_type>)
		{
			if(endptr<currptr)
			{
				currptr=endptr;
			}	
		}
		this->setp(currptr,endptr);
	}
	std::streamsize xsputn(char_type const* ptr, std::streamsize sz) override final
	{
		write(ref,ptr,ptr+sz);
		char_type* currptr=obuffer_curr(ref);
		char_type* endptr=obuffer_end(ref);
		this->update_buffer_ptrs();
		return sz;
	}
	int_type overflow(int_type ch = traits_type::eof()) override final
	{
		obuffer_set_curr(ref,this->pptr());
		if(ch!=traits_type::eof())
		{
			obuffer_overflow(ref,traits_type::to_char_type(ch));
		}
		this->update_buffer_ptrs();
		return 0;
	}
};

inline constexpr basic_streambuf_io_observer<> io_ref(basic_streambuf_output_binder)

}

namespace manipulators
{

template<std::integral ch_type,typename Traits = ::std::char_traits<typename stm::char_type>>
struct basic_streambuf_binder_ref
{
	using manip_tag_t = manip_tag;
	using reference_type = std::basic_streambuf<ch_type,Traits>*;
	reference_type ptr{};
};

inline constexpr ostream_binder

}

template<buffer_output_stream T>
inline auto bind_ostream(T&& t)
{
	using binder_type = ::std::remove_cvref_t<decltype(io_ref(t))>;
	return ::fast_io::manipulators::basic_streambuf_output_binder<binder_type>(io_ref(t));
}

}
