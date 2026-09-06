#include <fast_io.h>
#include <fast_io_dsal/array.h>
#ifdef __HERBCEPTIONS__
#define FAST_IO_HERBCEPTIONS_THROWS throws
#define FAST_IO_HERBCEPTIONS_THROWS_IF(x) throws(x)
#else
#define FAST_IO_HERBCEPTIONS_THROWS
#define FAST_IO_HERBCEPTIONS_THROWS_IF(x) noexcept(!(x))
#endif

#if __has_cpp_attribute(indeterminate)
#define FAST_IO_INDETERMINATE [[indeterminate]]
#else
#define FAST_IO_INDETERMINATE
#endif


#if 1
#include "print_freestanding_cxx26.h"
#endif

int main()
{
	::fast_io::c_io_observer piob{stdout};
#if 1
	::fast_io::operations::decay::print_freestanding_decay2<true>(piob, ::fast_io::basic_io_scatter_t<char>{"Hello text2\n", 12},
#if 0
	::fast_io::parameter<::std::source_location const &>(::std::source_location::current()),
#endif
																  ::fast_io::basic_io_scatter_t<char>{"\n\n", 2}
#if 1
																  ,
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull),
																  ::fast_io::mnp::hex(0xFFFEull)
#endif
	);
#endif
}
