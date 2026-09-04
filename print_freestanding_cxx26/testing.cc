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
	::fast_io::posix_io_observer piob{2};
	::fast_io::operations::decay::print_freestanding_decay2<true>(piob, ::fast_io::mnp::hex(0xFFFEull));
}
