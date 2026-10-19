#include<fast_io.h>
#include"binfmt_print.h"

void binfmt_vprint_foo(::fast_io::binfmt::binfmt_device dev, void const *base, ::std::size_t len,
						  ::std::size_t elements_count, void const *cookie) throws
{
	::fast_io::binfmt::details::binfmt_vprint_impl<::fast_io::binfmt::binfmt_policy_default>(
		dev, base, len, elements_count, cookie);
}

