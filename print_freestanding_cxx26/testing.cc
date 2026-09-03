#include<fast_io.h>
#ifdef __HERBCEPTIONS__
#define FAST_IO_HERBCEPTIONS_THROWS throws
#define FAST_IO_HERBCEPTIONS_THROWS_IF(x) throws(x)
#else
#define FAST_IO_HERBCEPTIONS_THROWS
#define FAST_IO_HERBCEPTIONS_THROWS_IF(x) noexcept(!(x))
#endif

#if 1
#include"print_freestanding_cxx26.h"
#endif

template<typename... Args>
inline void foo(Args... args)
{
	if constexpr(!sizeof...(args))
	{
		return;
	}
	else if constexpr(sizeof...(args)<2)
	{
		return;
	}
	else
	{
		constexpr ::std::size_t mid{sizeof...(args)>>1};
	   [&]<std::size_t... Is>(std::index_sequence<Is...>) {
			foo(args...[mid + Is]...);
		}(::std::make_index_sequence<sizeof...(args) - mid>{});
	}
}

int main()
{
    foo(0,1,2,3,4,5,6,7,8);
}