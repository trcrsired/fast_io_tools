#include <fast_io.h>

#include "binfmt_print.h"

// The shared interpreter entry points. fast_io's status-print imbuer
// (binfmt_status.h) calls these instead of instantiating the interpreter in
// every translation unit, so a big program carries one implementation of the
// formatter — here — and only the argument packing at its call sites.
//
// One symbol per char_type flavour, plain C ABI: the device is already a
// plain {vptr, deviceptr} struct.
namespace fast_io::binfmt
{

#define FAST_IO_BINFMT_DEFINE_STATUS_VPRINT(NAME, CHAR_TYPE)                              \
	void NAME(basic_binfmt_device<CHAR_TYPE> dev, void const *base, ::std::size_t len,     \
			  ::std::size_t elements_count, void const *cookie) throws                     \
	{                                                                                      \
		details::binfmt_vprint_impl<binfmt_policy_default>(dev, base, len,                   \
														   elements_count, cookie);         \
	}

FAST_IO_BINFMT_DEFINE_STATUS_VPRINT(binfmt_status_vprint, char)
FAST_IO_BINFMT_DEFINE_STATUS_VPRINT(wbinfmt_status_vprint, wchar_t)
FAST_IO_BINFMT_DEFINE_STATUS_VPRINT(u8binfmt_status_vprint, char8_t)
FAST_IO_BINFMT_DEFINE_STATUS_VPRINT(u16binfmt_status_vprint, char16_t)
FAST_IO_BINFMT_DEFINE_STATUS_VPRINT(u32binfmt_status_vprint, char32_t)

#undef FAST_IO_BINFMT_DEFINE_STATUS_VPRINT

// legacy entry kept for existing callers
void binfmt_vprint_foo(binfmt_device dev, void const *base, ::std::size_t len,
					   ::std::size_t elements_count, void const *cookie) throws
{
	details::binfmt_vprint_impl<binfmt_policy_default>(dev, base, len, elements_count,
													   cookie);
}

} // namespace fast_io::binfmt
