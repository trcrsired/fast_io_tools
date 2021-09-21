#pragma once

namespace fast_io
{

namespace details::decay
{

template<std::integral char_type,typename T>
inline constexpr void scatter_print_recursive(io_scatter_t* arr,T t)
{
	*arr=print_scatter_define(print_scatter_type<char_type>,t);
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr void scatter_print_recursive(io_scatter_t* arr,T t, Args ...args)
{
	*arr=print_scatter_define(print_scatter_type<char_type>,t);
	scatter_print_recursive<char_type>(arr+1,args...);
}

template<std::integral char_type,typename T>
inline constexpr std::size_t calculate_scatter_reserve_size_unit()
{
	using real_type = std::remove_cvref_t<T>;
	if constexpr(reserve_printable<char_type,real_type>)
	{
		constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,real_type>)};
		return sz;
	}
	else
		return 0;
}

template<std::integral char_type,typename T>
inline constexpr std::size_t calculate_scatter_reserve_size()
{
	return calculate_scatter_reserve_size_unit<char_type,T>();
}

template<std::integral char_type,typename T,typename... Args>
requires (sizeof...(Args)!=0)
inline constexpr std::size_t calculate_scatter_reserve_size()
{
	return ::fast_io::details::intrinsics::add_or_overflow_die(calculate_scatter_reserve_size_unit<char_type,T>(),
		calculate_scatter_reserve_size<char_type,Args...>());
}



template<std::integral char_type,typename T,typename... Args>
inline constexpr std::size_t calculate_scatter_dynamic_reserve_size([[maybe_unused]] T t,Args... args)
{
	if constexpr(!reserve_printable<char_type,T>&&
		dynamic_reserve_printable<char_type,T>)
	{
		std::size_t res{print_reserve_size(io_reserve_type<char_type,T>,t)};
		if constexpr(sizeof...(Args)==0)
			return res;
		else
			return ::fast_io::details::intrinsics::add_or_overflow_die(res,calculate_scatter_dynamic_reserve_size<char_type>(args...));
	}
	else
	{
		if constexpr(sizeof...(Args)==0)
			return 0;
		else
			return calculate_scatter_dynamic_reserve_size<char_type>(args...);
	}
}


template<std::integral char_type,typename T>
inline constexpr void scatter_print_with_reserve_recursive_unit(char_type*& start_ptr,
		io_scatter_t* arr,T t)
{
	using real_type = std::remove_cvref_t<T>;
	if constexpr(reserve_printable<char_type,T>)
	{
#if defined(__SANITIZE_ADDRESS__) && !defined(__OPTIMIZE__)
		constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,real_type>)};
		char_type sanitize_buffer[sz];
		auto dt{print_reserve_define(io_reserve_type<char_type,real_type>,sanitize_buffer,t)};
		auto end_ptr{non_overlapped_copy_n(sanitize_buffer,static_cast<std::size_t>(dt-sanitize_buffer),start_ptr)};
		*arr={start_ptr,static_cast<std::size_t>(end_ptr-start_ptr)*sizeof(*start_ptr)};
		start_ptr=end_ptr;
#else
		auto end_ptr = print_reserve_define(io_reserve_type<char_type,real_type>,start_ptr,t);
		*arr={start_ptr,static_cast<std::size_t>(end_ptr-start_ptr)*sizeof(*start_ptr)};
		start_ptr=end_ptr;
#endif
	}
	else
	{
		*arr=print_scatter_define(print_scatter_type<char_type>,t);
	}
}

template<std::integral char_type,typename T>
inline constexpr void scatter_print_with_reserve_recursive(char_type* ptr,
		io_scatter_t* arr,T t)
{
	scatter_print_with_reserve_recursive_unit(ptr,arr,t);
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr void scatter_print_with_reserve_recursive(char_type* ptr,
	io_scatter_t* arr,T t, Args ...args)
{
	scatter_print_with_reserve_recursive_unit(ptr,arr,t);
	scatter_print_with_reserve_recursive(ptr,arr+1,args...);
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr void scatter_print_with_dynamic_reserve_recursive(io_scatter_t* __restrict arr,
	[[maybe_unused]] char_type* __restrict ptr,
	[[maybe_unused]] char_type* __restrict dynamic_buffer_ptr,[[maybe_unused]] T t, Args ...args)
{
	
	if constexpr(scatter_type_printable<char_type,T>)
		*arr=print_scatter_define(print_scatter_type<char_type>,t);
	else if constexpr(reserve_printable<char_type,T>)
	{
		auto end_ptr = print_reserve_define(io_reserve_type<char_type,T>,ptr,t);
		*arr={ptr,(end_ptr-ptr)*sizeof(*ptr)};
		if constexpr(sizeof...(Args)!=0)
			ptr=end_ptr;
	}
	else if constexpr(dynamic_reserve_printable<char_type,T>)
	{
		auto end_ptr = print_reserve_define(io_reserve_type<char_type,T>,dynamic_buffer_ptr,t);
		*arr={dynamic_buffer_ptr,(end_ptr-dynamic_buffer_ptr)*sizeof(*dynamic_buffer_ptr)};
		if constexpr(sizeof...(Args)!=0)
			dynamic_buffer_ptr = end_ptr;
	}
	if constexpr(sizeof...(Args)!=0)
		scatter_print_with_dynamic_reserve_recursive(arr+1,ptr,dynamic_buffer_ptr,args...);
}


template<std::integral char_type,typename T>
requires scatter_type_printable<char_type,T>
inline constexpr auto extract_one_scatter(T t)
{
	return print_scatter_define(print_scatter_type<char_type>,t);
}


template<bool line,typename value_type,output_stream output>
#if __has_cpp_attribute(gnu::cold)
[[gnu::cold]]
#endif
inline constexpr void print_control_dynamic_reserve_bad_path(output out,value_type t,std::size_t sizep1)
{
	using char_type = typename output::char_type;
	local_operator_new_array_ptr<char_type> ptr(sizep1);
	auto it{print_reserve_define(io_reserve_type<char_type,value_type>,ptr.ptr,t)};
	if constexpr(line)
	{
		*it=char_literal_v<u8'\n',char_type>;
		++it;
	}
	write(out,ptr.ptr,it);
}

template<bool line=false,output_stream output,typename T>
requires (std::is_trivially_copyable_v<output>&&std::is_trivially_copyable_v<T>)
inline constexpr void print_control(output out,T t)
{
	using char_type = typename output::char_type;
	using value_type = std::remove_cvref_t<T>;
	constexpr auto lfch{char_literal_v<u8'\n',char_type>};
	if constexpr(scatter_type_printable<char_type,value_type>)
	{
		basic_io_scatter_t<char_type> scatter{print_scatter_define(print_scatter_type<char_type>,t)};
		if constexpr(line)
		{
			if constexpr(contiguous_output_stream<output>)
			{
				auto curr=obuffer_curr(out);
				auto end=obuffer_end(out);
				std::ptrdiff_t sz(end-curr-1);
				std::size_t const len{scatter.len};
				if(sz<static_cast<std::ptrdiff_t>(len))
					fast_terminate();
				curr=non_overlapped_copy_n(scatter.base,scatter.len,curr);
				*curr=lfch;
				++curr;
				obuffer_set_curr(out,curr);
			}
			else if constexpr(buffer_output_stream<output>)
			{
				auto curr=obuffer_curr(out);
				auto end=obuffer_end(out);
				std::size_t const len{scatter.len};
				std::ptrdiff_t sz(end-curr-1);
				if(static_cast<std::ptrdiff_t>(len)<sz)[[likely]]
				{
					curr=details::non_overlapped_copy_n(scatter.base,len,curr);
					*curr=lfch;
					++curr;
					obuffer_set_curr(out,curr);
				}
				else
				{
					write(out,scatter.base,scatter.base+scatter.len);
					put(out,lfch);
				}
			}
			else
			{
				write(out,scatter.base,scatter.base+scatter.len);
				write(out,__builtin_addressof(lfch),
				__builtin_addressof(lfch)+1);
			}
		}
		else
		{
			write(out,scatter.base,scatter.base+scatter.len);
		}
	}
	else if constexpr(reserve_printable<char_type,value_type>)
	{
		constexpr std::size_t real_size{print_reserve_size(io_reserve_type<char_type,value_type>)};
		constexpr std::size_t size{real_size+static_cast<std::size_t>(line)};
		static_assert(real_size!=SIZE_MAX);
		if constexpr(contiguous_output_stream<output>)
		{
			auto bcurr{obuffer_curr(out)};
			auto bend{obuffer_end(out)};
			std::size_t diff{static_cast<std::size_t>(bend-bcurr)};
			if(diff<size)[[unlikely]]
				fast_terminate();
			auto it{print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t)};
			if constexpr(line)
			{
				*it=lfch;
				++it;
			}
			obuffer_set_curr(out,it);
		}
		else
		{
			char_type buffer[size];
			if constexpr(buffer_output_stream<output>)
			{
				char_type* bcurr{obuffer_curr(out)};
				char_type* bend{obuffer_end(out)};
				std::ptrdiff_t const diff(bend-bcurr);
				bool smaller{static_cast<std::ptrdiff_t>(size)<diff};
				if(!smaller)[[unlikely]]
					bcurr=buffer;
				bcurr=print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t);
				if constexpr(line)
				{
					*bcurr=lfch;
					++bcurr;
				}
				if(smaller)[[likely]]
					obuffer_set_curr(out,bcurr);
				else[[unlikely]]
					write(out,buffer,bcurr);
			}
			else
			{
				char_type* i{print_reserve_define(io_reserve_type<char_type,value_type>,buffer,t)};
				if constexpr(line)
				{
					*i=lfch;
					++i;
				}
				write(out,buffer,i);
			}
		}
	}
	else if constexpr(dynamic_reserve_printable<char_type,value_type>)
	{
		std::size_t size{print_reserve_size(io_reserve_type<char_type,value_type>,t)};
		if constexpr(line)
		{
			constexpr std::size_t mx{std::numeric_limits<std::ptrdiff_t>::max()-1};
			if(size>=mx)
				fast_terminate();
			++size;
		}
		else
		{
			constexpr std::size_t mx{std::numeric_limits<std::ptrdiff_t>::max()};
			if(mx<size)
				fast_terminate();
		}
		if constexpr(contiguous_output_stream<output>)
		{
			auto bcurr{obuffer_curr(out)};
			auto bend{obuffer_end(out)};
			auto it{print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t,size)};
			std::size_t diff{static_cast<std::size_t>(bend-bcurr)};
			if(diff<size)[[unlikely]]
				fast_terminate();
			if constexpr(line)
			{
				*it=lfch;
				++it;
			}
			obuffer_set_curr(out,it);
		}
		else
		{
#if !defined(__SANITIZE_ADDRESS__) || defined(__OPTIMIZE__)
			if constexpr(buffer_output_stream<output>)
			{
				auto curr{obuffer_curr(out)};
				auto ed{obuffer_end(out)};
				std::ptrdiff_t diff(ed-curr);
				if(static_cast<std::ptrdiff_t>(size)<diff)[[likely]]
				{
					auto it{print_reserve_define(io_reserve_type<char_type,value_type>,curr,t)};
					if constexpr(line)
					{
						*it=lfch;
						++it;
					}
					obuffer_set_curr(out,it);
					return;
				}
			}
#endif
			print_control_dynamic_reserve_bad_path<line,value_type>(out,t,size);
		}
	}
	else if constexpr(printable<output,value_type>)
	{
			print_define(out,t);
		if constexpr(line)
		{
			if constexpr(buffer_output_stream<output>)
				put(out,lfch);
			else
				write(out,__builtin_addressof(lfch),
				__builtin_addressof(lfch)+1);
		}
	}
	else
	{
		constexpr bool no{printable<output,value_type>};
		static_assert(no,"type not printable");
	}
}


struct para_single_arg
{
	std::uintptr_t reg1;
#if defined(__ELF__) || UINTPTR_MAX == UINT_LEAST32_MAX
	std::uintptr_t reg2;
#endif
};

template<bool line,typename output,typename T>
inline void para_output_single(para_single_arg reg01,para_single_arg reg23)
{
	if constexpr(std::is_trivially_copyable_v<output>&&sizeof(output)<=sizeof(para_single_arg))
	{
		output out;
		__builtin_memcpy(__builtin_addressof(out),__builtin_addressof(reg01),sizeof(output));
		if constexpr(std::is_trivially_copyable_v<T>&&sizeof(T)<=sizeof(para_single_arg))
		{
			::fast_io::freestanding::array<char,sizeof(T)> t;
			__builtin_memcpy(__builtin_addressof(t),__builtin_addressof(reg23),sizeof(T));
			print_control<line>(out,__builtin_bit_cast(T,t));
		}
		else
		{
			T* pt;
			__builtin_memcpy(__builtin_addressof(pt),__builtin_addressof(reg23),sizeof(std::uintptr_t));
			print_control<line>(out,*pt);
		}
	}
	else
	{
		output* pout;
		__builtin_memcpy(__builtin_addressof(pout),__builtin_addressof(reg01),sizeof(uintptr_t));
		if constexpr(std::is_trivially_copyable_v<T>&&sizeof(T)<=sizeof(para_single_arg))
		{
			::fast_io::freestanding::array<char,sizeof(T)> t;
			__builtin_memcpy(__builtin_addressof(t),__builtin_addressof(reg23),sizeof(T));
			print_control<line>(*pout,__builtin_bit_cast(T,t));
		}
		else
		{
			T* pt;
			__builtin_memcpy(__builtin_addressof(pt),__builtin_addressof(reg23),sizeof(std::uintptr_t));
			print_control<line>(*pout,*pt);
		}
	}
}

using para_function_pointer = void (*) (para_single_arg,para_single_arg);

template<bool line,typename output,typename T,typename... Args>
inline constexpr void generate_internal_single_para(para_function_pointer* p)
{
	if constexpr(sizeof...(Args)==0)
		*p=&para_output_single<line,output,T>;
	else
	{
		*p=&para_output_single<false,output,T>;
		generate_internal_single_para<line,output,Args...>(p+1);
	}
}

template<bool line,typename output,typename... Args>
inline constexpr auto generate_internal_paras()
{
	::fast_io::freestanding::array<para_function_pointer,sizeof...(Args)> para_fptr_array{};
	generate_internal_single_para<line,output,Args...>(para_fptr_array.data());
	return para_fptr_array;
}

template<bool line,typename output,typename... Args>
inline constexpr auto internal_parameters{generate_internal_paras<line,output,Args...>()};


struct va_list_raii
{
	std::va_list args;
	explicit va_list_raii() noexcept=default;
	va_list_raii(va_list_raii const&);
	va_list_raii& operator=(va_list_raii const&)=delete;
	~va_list_raii()
	{
		va_end(args);
	}
};

inline void print_controls_va_args(para_single_arg output,para_function_pointer const* first,std::size_t n,...)
{
	va_list_raii args;
	va_start(args.args, n);
	for(auto i{first},e{first+n};i!=e;++i)
		(*i)(output,va_arg(args.args,para_single_arg));
}

template<bool line,output_stream output,typename... Args>
inline void print_controls_line_paras(output out,Args... args)
{
	constexpr auto& paramater_list{internal_parameters<line,output,Args...>};
	if constexpr(std::is_trivially_copyable_v<output>&&sizeof(output)<=sizeof(para_single_arg))
	{
		para_single_arg out_arg;
		__builtin_memcpy(__builtin_addressof(out_arg),__builtin_addressof(out),sizeof(output));
		print_controls_va_args(out_arg,paramater_list.data(),paramater_list.size(),args...);
	}
	else
	{
		para_single_arg out_arg;
		auto pout{__builtin_addressof(out_arg)};
		__builtin_memcpy(__builtin_addressof(out),__builtin_addressof(pout),sizeof(output));
		print_controls_va_args(out_arg,paramater_list.data(),paramater_list.size(),args...);
	}
}

template<bool line,output_stream output,typename T,typename... Args>
inline void print_controls_line_paras2(output out,T t,Args... args)
{
	print_controls_line_paras<line>(out,t,args...);
}

template<bool ln,output_stream output,typename T,typename... Args>
inline constexpr void print_controls_line(output out,T t,Args... args)
{
	if constexpr(sizeof...(Args)==0)
	{
		print_control<ln>(out,t);
	}
	else
	{
			print_control<false>(out,t);
			print_controls_line<ln>(out,args...);
#if 0
		if(std::is_constant_evaluated())
		{
			print_control<false>(out,t);
			print_controls_line<ln>(out,args...);
		}
		else
		{
			print_controls_line_paras2<ln,output,T,Args...>(out,t,args...);
		}
#endif
	}
}


template<std::integral char_type,bool ln,typename T,typename... Args>
inline constexpr std::size_t calculate_print_normal_maxium_size_main(std::size_t mx_value) noexcept
{
	std::size_t val{};
	if constexpr(ln&&(sizeof...(Args)==0))
		++val;
	if constexpr(reserve_printable<char_type,T>)
	{
		constexpr std::size_t size{print_reserve_size(io_reserve_type<char_type,T>)};
		static_assert(size!=SIZE_MAX,"overflow");
		val+=size;
	}
	if(mx_value<val)
		mx_value=val;
	if constexpr((sizeof...(Args)==0))
		return mx_value;
	else
		return calculate_print_normal_maxium_size_main<char_type,ln,Args...>(mx_value);
}

template<std::integral char_type,bool ln,typename... Args>
inline constexpr std::size_t calculate_print_normal_maxium_size() noexcept
{
	return calculate_print_normal_maxium_size_main<char_type,ln,Args...>(0);
}

template<std::integral char_type,bool ln,typename T,typename... Args>
inline constexpr std::size_t calculate_print_normal_dynamic_maxium_main(std::size_t mx_value,T t,Args... args) noexcept
{
	if constexpr(dynamic_reserve_printable<char_type,T>)
	{
		std::size_t size{print_reserve_size(io_reserve_type<char_type,T>,t)};
		if constexpr(ln&&(sizeof...(Args)==0))
		{
			if(size==SIZE_MAX)
				fast_terminate();
			++size;
		}
		if(mx_value<size)
			mx_value=size;
	}
	if constexpr((sizeof...(Args)==0))
		return mx_value;
	else
		return calculate_print_normal_dynamic_maxium_main<char_type,ln>(mx_value,args...);
}

template<bool line,typename output,std::integral char_type,typename T,typename... Args>
inline constexpr void normal_print_with_dynamic_reserve_recursive(output out,char_type* __restrict ptr,T t, Args ...args)
{
	if constexpr((scatter_type_printable<char_type,T>&&(scatter_type_printable<char_type,Args>&&...)))
	{
		print_controls_line<line>(out,t,args...);
	}
	else if constexpr(scatter_type_printable<char_type,T>)
	{
		static_assert(sizeof...(Args)!=0);
		print_control<false>(out,t);
	}
	else if constexpr(reserve_printable<char_type,T>||dynamic_reserve_printable<char_type,T>)
	{
		auto end_ptr = print_reserve_define(io_reserve_type<char_type,T>,ptr,t);
		if constexpr(line&&(sizeof...(Args)==0))
		{
			*end_ptr=char_literal_v<u8'\n',char_type>;
			++end_ptr;
		}
		write(out,ptr,end_ptr);
	}
	if constexpr(sizeof...(Args)!=0)
		normal_print_with_dynamic_reserve_recursive<line>(out,ptr,args...);
}

template<bool line,output_stream output,typename ...Args>
inline constexpr void print_fallback(output out,Args ...args)
{
	using char_type = typename output::char_type;
	constexpr auto lfch{char_literal_v<u8'\n',char_type>};
	if constexpr(line&&sizeof...(Args)==0)
	{
		char_type ch{lfch};
		write(out,__builtin_addressof(lfch),__builtin_addressof(lfch)+1);
	}
	else if constexpr(((scatter_printable<typename output::char_type,Args>||reserve_printable<typename output::char_type,Args>||dynamic_reserve_printable<typename output::char_type,Args>)&&...))
	{
		if constexpr(scatter_output_stream<output>)
		{
			constexpr std::size_t n{(sizeof...(Args))+static_cast<std::size_t>(line)};
			io_scatter_t scatters[n];
			if constexpr((scatter_printable<typename output::char_type,Args>&&...))
			{
				scatter_print_recursive<typename output::char_type>(scatters,args...);
				if constexpr(line)
				{
					char_type ch{lfch};
					scatters[n-1]={__builtin_addressof(ch),sizeof(ch)};
					scatter_write(out,{scatters,n});
				}
				else
					scatter_write(out,{scatters,n});
			}
			else if constexpr(((scatter_printable<char_type,Args>||
				reserve_printable<char_type,Args>)&&...))
			{
				constexpr std::size_t total_size{calculate_scatter_reserve_size<char_type,Args...>()};
				char_type buffer[total_size];
				scatter_print_with_reserve_recursive(buffer,scatters,args...);
				if constexpr(line)
				{
					char_type ch{lfch};
					scatters[n-1]={__builtin_addressof(ch),sizeof(ch)};
					scatter_write(out,{scatters,n});
				}
				else
					scatter_write(out,{scatters,n});
			}
			else
			{
				constexpr std::size_t total_size{calculate_scatter_reserve_size<char_type,Args...>()};
				char_type buffer[total_size];
				local_operator_new_array_ptr<char_type> new_ptr(calculate_scatter_dynamic_reserve_size<char_type>(args...));
				scatter_print_with_dynamic_reserve_recursive(scatters,buffer,new_ptr.ptr,args...);
				if constexpr(line)
				{
					char_type ch{lfch};
					scatters[n-1]={__builtin_addressof(ch),sizeof(ch)};
					scatter_write(out,{scatters,n});
				}
				else
					scatter_write(out,{scatters,n});
			}
		}
		else
		{
			if constexpr((scatter_printable<typename output::char_type,Args>&&...))
			{
				print_controls_line<line>(out,args...);
			}
			else if constexpr(((scatter_printable<char_type,Args>||
				reserve_printable<char_type,Args>)&&...))
			{
				constexpr std::size_t array_size{calculate_print_normal_maxium_size<char_type,line,Args...>()};
				char_type buffer[array_size];
				normal_print_with_dynamic_reserve_recursive<line>(out,buffer,args...);
			}
			else
			{
				constexpr std::size_t array_size{calculate_print_normal_maxium_size<char_type,line,Args...>()};
				std::size_t dynamic_array_size{calculate_print_normal_dynamic_maxium_main<char_type,line,Args...>(array_size,args...)};
				local_operator_new_array_ptr<char_type> buffer(dynamic_array_size);
				normal_print_with_dynamic_reserve_recursive<line>(out,buffer.ptr,args...);
			}
		}
	}
	else
	{
		dynamic_io_buffer<typename output::char_type> buffer;
		auto ref{io_ref(buffer)};
		print_controls_line<line>(ref,args...);
		write(out,buffer.buffer_begin,buffer.buffer_curr);
	}
}

template<bool line,output_stream output,typename ...Args>
inline constexpr void print_freestanding_decay_normal(output out,Args ...args)
{
	using char_type = typename output::char_type;
	if constexpr(sizeof...(Args)==0&&!line)
		return;
	else if constexpr(mutex_stream<output>)
	{
		lock_guard lg{out};
		decltype(auto) dout{out.unlocked_handle()};
		print_freestanding_decay_normal<line>(io_ref(dout),args...);
	}
	else if constexpr(buffer_output_stream<output>)
	{
		if constexpr(sizeof...(Args)==0&&line)
			put(out,char_literal_v<u8'\n',char_type>);
		else
		{
			if constexpr(line)
				print_controls_line<line>(out,args...);
			else
				(print_control<line>(out,args),...);
		}
	}
	else if constexpr(sizeof...(Args)==1&&
		(((!line&&((printable<output,Args>||scatter_type_printable<char_type,Args>)&&...))||
		((reserve_printable<char_type,Args>||dynamic_reserve_printable<char_type,Args>)&&...))))
	{
		(print_control<line>(out,args),...);
	}
	else
	{
		print_fallback<line>(out,args...);
	}
}

}

template<typename output,typename ...Args>
requires (output_stream<output>||status_output_stream<output>)&&(std::is_trivially_copyable_v<output>&&(std::is_trivially_copyable_v<Args>&&...))
inline constexpr void print_freestanding_decay(output out,Args ...args)
{
	if constexpr(status_output_stream<output>)
		print_status_define(out,args...);
	else if constexpr(output_stream<output>)
		details::decay::print_freestanding_decay_normal<false>(out,args...);
}

template<typename output,typename ...Args>
requires (output_stream<output>||status_output_stream<output>)
inline constexpr void print_freestanding(output&& out,Args&& ...args)
{
	print_freestanding_decay(io_ref(out),io_print_forward<typename std::remove_cvref_t<output>::char_type>(io_print_alias(args))...);
}

namespace details
{

template<bool line,typename output,typename ...Args>
requires (output_stream<output>||status_output_stream<output>)
inline constexpr void print_freestanding_decay_cold_impl(output out,Args ...args)
{
	if constexpr(status_output_stream<output>)
	{
		if constexpr(line)
			println_status_define(out,args...);
		else
			print_status_define(out,args...);
	}
	else
		details::decay::print_freestanding_decay_normal<line>(out,args...);
}

}


template<typename output,typename ...Args>
requires (output_stream<output>||status_output_stream<output>)
inline constexpr void println_freestanding_decay(output out,Args ...args)
{
	if constexpr(status_output_stream<output>)
	{
		println_status_define(out,args...);
	}
	else
		details::decay::print_freestanding_decay_normal<true>(out,args...);
}

template<output_stream output,typename ...Args>
inline constexpr void println_freestanding(output&& out,Args&& ...args)
{
	println_freestanding_decay(io_ref(out),io_print_forward<typename std::remove_cvref_t<output>::char_type>(io_print_alias(args))...);
}


}