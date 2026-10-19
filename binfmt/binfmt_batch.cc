#include <fast_io.h>

#include "binfmt_batch.h"

// The shared batched interpreter entry points, one per char_type flavour, plain
// C ABI. binfmt_status_batch.h calls these so a big program carries a single
// copy of the batched interpreter instead of instantiating it per call site.
namespace fast_io::binfmt
{

#define FAST_IO_BINFMT_DEFINE_STATUS_BATCH_VPRINT(NAME, CHAR_TYPE)                    \
	void NAME(basic_binfmt_device<CHAR_TYPE> dev, void const *base, ::std::size_t len, \
			  ::std::size_t elements_count, void const *cookie) throws                  \
	{                                                                                 \
		details::binfmt_vprint_batched_impl<binfmt_policy_default>(dev, base, len,      \
																  elements_count, cookie); \
	}

FAST_IO_BINFMT_DEFINE_STATUS_BATCH_VPRINT(binfmt_status_batch_vprint, char)
FAST_IO_BINFMT_DEFINE_STATUS_BATCH_VPRINT(wbinfmt_status_batch_vprint, wchar_t)
FAST_IO_BINFMT_DEFINE_STATUS_BATCH_VPRINT(u8binfmt_status_batch_vprint, char8_t)
FAST_IO_BINFMT_DEFINE_STATUS_BATCH_VPRINT(u16binfmt_status_batch_vprint, char16_t)
FAST_IO_BINFMT_DEFINE_STATUS_BATCH_VPRINT(u32binfmt_status_batch_vprint, char32_t)

#undef FAST_IO_BINFMT_DEFINE_STATUS_BATCH_VPRINT

} // namespace fast_io::binfmt
