#pragma once

// Batched status path — a side-by-side copy of binfmt_status.h's status-print
// machinery that feeds the single-device-interaction interpreter
// (binfmt_batch.h) instead of the stock one. Nothing here replaces the
// original: mnp::binfmt / binfmt_status_vprint still exist unchanged, and the
// argument mapping, program generation and cookie packing are shared verbatim
// from binfmt_status.h. Only the interpreter entry differs.

#include "binfmt_batch.h"
#include "binfmt_status.h"

#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io::binfmt
{

// shared batched interpreter entries, defined once in binfmt_batch.cc. Like the
// stock entries these are out of line, so the interpreter body stays in one
// translation unit and call sites carry only the packing.
void binfmt_status_batch_vprint(binfmt_device dev, void const *base, ::std::size_t len,
								::std::size_t elements_count, void const *cookie) throws;
void wbinfmt_status_batch_vprint(wbinfmt_device dev, void const *base, ::std::size_t len,
								 ::std::size_t elements_count, void const *cookie) throws;
void u8binfmt_status_batch_vprint(u8binfmt_device dev, void const *base, ::std::size_t len,
								  ::std::size_t elements_count, void const *cookie) throws;
void u16binfmt_status_batch_vprint(u16binfmt_device dev, void const *base, ::std::size_t len,
								   ::std::size_t elements_count, void const *cookie) throws;
void u32binfmt_status_batch_vprint(u32binfmt_device dev, void const *base, ::std::size_t len,
								   ::std::size_t elements_count, void const *cookie) throws;

// the batched indirector — same shape as binfmt_indirector, distinct type so it
// gets its own status_print_define hook.
template <typename stm>
struct binfmt_batch_indirector
{
	using handle_type = stm;
	using output_char_type = typename handle_type::output_char_type;
	handle_type handle{};
};

namespace details
{

template <::std::integral char_type>
inline void binfmt_run_batch(basic_binfmt_device<char_type> dev, char8_t const *prog,
							 ::std::size_t prog_len, ::std::size_t elements_count,
							 void const *cookie) throws
{
	if constexpr (::std::same_as<char_type, char>)
	{
		binfmt_status_batch_vprint(dev, prog, prog_len, elements_count, cookie);
	}
	else if constexpr (::std::same_as<char_type, wchar_t>)
	{
		wbinfmt_status_batch_vprint(dev, prog, prog_len, elements_count, cookie);
	}
	else if constexpr (::std::same_as<char_type, char8_t>)
	{
		u8binfmt_status_batch_vprint(dev, prog, prog_len, elements_count, cookie);
	}
	else if constexpr (::std::same_as<char_type, char16_t>)
	{
		u16binfmt_status_batch_vprint(dev, prog, prog_len, elements_count, cookie);
	}
	else if constexpr (::std::same_as<char_type, char32_t>)
	{
		u32binfmt_status_batch_vprint(dev, prog, prog_len, elements_count, cookie);
	}
	else
	{
		binfmt_vprint_batched<binfmt_policy_default>(dev, prog, prog_len, elements_count, cookie);
	}
}

template <bool line, ::std::integral char_type, typename... Args>
inline void binfmt_emit_batch(basic_binfmt_device<char_type> dev, Args... args) throws
{
	constexpr ::std::size_t k{sizeof...(Args)};
	constexpr bool all_scatter{
		k != 0u && (binfmt_is_scatter_of<char_type, ::std::remove_cvref_t<Args>> && ...)};
	if constexpr (all_scatter)
	{
		constexpr ::std::size_t n{k + (line ? 1u : 0u)};
		::fast_io::array<::fast_io::basic_io_scatter_t<char_type>, n> sc FAST_IO_INDETERMINATE;
		template for (constexpr auto i : ::fast_io::details::index_array_range<0, k>)
		{
			sc.index_unchecked(i) = {args...[i].base, args...[i].len};
		}
		if constexpr (line)
		{
			sc.index_unchecked(k) = {__builtin_addressof(binfmt_newline<char_type>), 1u};
		}
		scatter_write_all(dev, sc.data(), n);
	}
	else if constexpr (k != 0u || line)
	{
		::fast_io::array<binfmt_cookie_element, k> cookie FAST_IO_INDETERMINATE;
		template for (constexpr auto i : ::fast_io::details::index_array_range<0, k>)
		{
			binfmt_pack_one<char_type>(cookie.index_unchecked(i), args...[i]);
		}
		binfmt_run_batch(dev, binfmt_program<line, char_type, Args...>.data(),
						 binfmt_program<line, char_type, Args...>.size(), k, cookie.data());
	}
}

template <bool line, typename output, typename... Args>
inline void binfmt_print_all_batch(binfmt_batch_indirector<output> ind, Args... args) throws
{
	using stream_type = ::std::remove_cvref_t<output>;
	if constexpr (sizeof...(Args) == 0u && !line)
	{
		return;
	}
	else if constexpr (::fast_io::operations::decay::defines::
						   has_output_or_io_stream_mutex_ref_define<stream_type>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(ind.handle)};
		auto unlocked{::fast_io::operations::decay::output_stream_unlocked_ref_decay(ind.handle)};
		binfmt_emit_batch<line>(binfmt_device_of(unlocked), args...);
	}
	else
	{
		binfmt_emit_batch<line>(binfmt_device_of(ind.handle), args...);
	}
}

} // namespace details

template <typename stm>
inline constexpr binfmt_batch_indirector<stm>
output_stream_ref_define(binfmt_batch_indirector<stm> t) noexcept
{
	return t;
}

template <typename stm>
	requires(::std::is_lvalue_reference_v<stm> || ::std::is_trivially_copyable_v<stm>)
inline constexpr auto indirect_batch(stm &&out) throws
{
	using reftype = decltype(::fast_io::operations::output_stream_ref(out));
	return binfmt_batch_indirector<reftype>{::fast_io::operations::output_stream_ref(out)};
}

template <bool line, typename output, typename... Args>
inline constexpr void status_print_define(binfmt_batch_indirector<output> ind, Args... args) throws
{
	details::binfmt_print_all_batch<line>(ind, args...);
}

} // namespace fast_io::binfmt

namespace fast_io::manipulators
{

template <typename stm>
	requires(::std::is_lvalue_reference_v<stm> || ::std::is_trivially_copyable_v<stm>)
inline constexpr auto binfmt_batch(stm &&out) throws
{
	return ::fast_io::binfmt::indirect_batch(static_cast<stm &&>(out));
}

} // namespace fast_io::manipulators

#include <fast_io_dsal/impl/misc/pop_macros.h>
