#include <fast_io.h>
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
}
