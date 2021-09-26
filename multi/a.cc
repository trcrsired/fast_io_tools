#include<fast_io.h>

namespace fast_io::details::decay
{
struct print_size_struct
{
	std::size_t n{};
	std::size_t total_size{};
};

inline constexpr std::size_t max_reserve_multiple_parameter_size{512u};

template<std::integral char_type,typename T,typename... Args>
inline constexpr print_size_struct print_reserve_total_size_para()
{
	if constexpr(reserve_printable<char_type,T>)
	{
		constexpr std::size_t rsvsize{print_reserve_size(io_reserve_type<char_type,T>)};
		if constexpr(rsvsize>=max_reserve_multiple_parameter_size)
			return {};
		else if constexpr(sizeof...(Args)==0)
		{
			return {1,rsvsize};
		}
		else
		{
			auto next{print_reserve_total_size_para<char_type,Args...>()};
			std::size_t mx_size{static_cast<std::size_t>(SIZE_MAX>>1u)-rsvsize};
			if(next.total_size>=mx_size)
				return next;
			return {next.n+1,next.total_size+rsvsize};
		}
	}
	else
	{
		return {};
	}
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr print_size_struct print_reserve_multiple_parameters_total_size()
{
	constexpr std::size_t t_reserve_size{print_reserve_size(io_reserve_type<char_type,T>)};
	static_assert(t_reserve_size<(SIZE_MAX>>1u),"overflow");
	if constexpr((t_reserve_size>=max_reserve_multiple_parameter_size)||(sizeof...(Args)==0))
	{
		return {0,t_reserve_size};
	}
	else if constexpr(sizeof...(Args)!=0)
	{
		constexpr print_size_struct sizestrct{print_reserve_total_size_para<char_type,Args...>()};
		constexpr std::size_t overflow_max_size{max_reserve_multiple_parameter_size-t_reserve_size};
		if constexpr(sizestrct.total_size>=overflow_max_size)
			return {0,t_reserve_size};
		else
			return {sizestrct.n,t_reserve_size+sizestrct.total_size};
	}
}

template<std::size_t n,
std::integral char_type,
typename T,typename... Args>
requires(n!=0)
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* partition_reserve_impl(char_type* ptr,T t,Args ...args)
{
	ptr=print_reserve_define(io_reserve_type<char_type,T>,ptr,t);
	if constexpr(n==1)
	{
		return ptr;
	}
	else
	{
		return partition_reserve_impl<n-1>(ptr,args...);
	}
}

template<bool line,
std::size_t n,
typename output,
typename T,typename... Args>
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_controls_line_multi_impl(output out,T t,Args ...args)
{
	if constexpr(n==0)
	{
		using char_type = typename output::char_type;
		if constexpr(reserve_printable<char_type,T>)
		{
			constexpr auto reserve_paras_info{print_reserve_multiple_parameters_total_size<char_type,T,Args...>()};
			constexpr std::size_t reserve_paras_n{reserve_paras_info.n};
			if constexpr(reserve_paras_n!=0)
			{
				static_assert(reserve_paras_info.total_size<static_cast<std::size_t>(PTRDIFF_MAX),"reserve size cannot be zero");
				{
					constexpr bool need_output_lf{sizeof...(Args)==reserve_paras_n&&line};
					constexpr std::size_t buffer_size{reserve_paras_info.total_size+static_cast<std::size_t>(need_output_lf)};
					if constexpr(buffer_output_stream<output>)
					{
						auto curr_ptr{obuffer_curr(out)};
						auto end_ptr{obuffer_end(out)};
						std::ptrdiff_t diff{end_ptr-curr_ptr};
						char_type stack_buffer[buffer_size];
						char_type *ptr{curr_ptr};
						bool const on_io_buffer{static_cast<std::ptrdiff_t>(buffer_size)<diff};
						if(!on_io_buffer)[[unlikely]]
							ptr=stack_buffer;
						ptr=print_reserve_define(io_reserve_type<char_type,T>,ptr,t);
						ptr=partition_reserve_impl<reserve_paras_n>(ptr,args...);
						if constexpr(need_output_lf)
						{
							*ptr=char_literal_v<u8'\n',char_type>;
							++ptr;
						}
						if(on_io_buffer)[[likely]]
						{
							obuffer_set_curr(out,ptr);
						}
						else[[unlikely]]
						{
							write(out,stack_buffer,ptr);
						}
					}
					else
					{
						char_type stack_buffer[buffer_size];
						auto start{stack_buffer};
						auto ptr{print_reserve_define(io_reserve_type<char_type,T>,start,t)};
						ptr=partition_reserve_impl<reserve_paras_n>(ptr,args...);
						if constexpr(need_output_lf)
						{
							*ptr=char_literal_v<u8'\n',char_type>;
							++ptr;
						}
						write(out,start,ptr);
					}
				}
				if constexpr(reserve_paras_info.n!=sizeof...(Args))
					print_controls_line_multi_impl<line,reserve_paras_info.n>(out,args...);
			}
			else
			{
				if constexpr(sizeof...(Args)==0)
					print_control<line>(out,t);
				else
				{
					print_control<false>(out,t);
					print_controls_line_multi_impl<line,0>(out,args...);
				}
			}
		}
		else
		{
			if constexpr(sizeof...(Args)==0)
				print_control<line>(out,t);
			else
			{
				print_control<false>(out,t);
				print_controls_line_multi_impl<line,0>(out,args...);
			}
		}
	}
	else
	{
		print_controls_line_multi_impl<line,n-1>(out,args...);
	}
}
}
int main()
{
	println(4235235,6,6,100,200);
//	partition_impl<3>(3,4,5,6,7);
}