// Isolate parser cost from write cost.
//
// A "literal, len 0" node is `0a 00`:
//   top-level loop turn: get_uleb(tag)=0x0a, get_bytes_payload -> get_uleb(len)=0,
//   bounds check, then binfmt_write_units(.., 0) returns immediately (no write).
//
// So timing a program of K such nodes and subtracting K=0 gives the pure
// per-node PARSER cost, with zero bytes written.
//
// Compare with K nodes of "literal, len 1" (`0a 01 0a`) to add the write cost.

#include <cstddef>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_print.h"

namespace
{

constexpr ::std::size_t N{10000000};

// 16 zero-length literal nodes
inline constexpr char zero_nodes[]{
	"\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00"
	"\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00\x0a\x00"};

// 16 one-byte literal nodes ("\n")
inline constexpr char one_nodes[]{
	"\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a"
	"\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a\x0a\x01\x0a"};

::fast_io::string sink;

void run(char const *prog, ::std::size_t plen, ::std::size_t count) throws
{
	using cookie_t = ::fast_io::binfmt::binfmt_cookie_element;
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, sink)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	cookie_t ck{};
	auto const t0{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	for (::std::size_t i{}; i != N; ++i)
	{
		ck.field1 = i;
		::fast_io::binfmt::binfmt_vprint(dev, prog, plen, 1, &ck);
	}
	auto const t1{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	double const sec{t1 - t0};
	::fast_io::io::println("K=", count, "  ", sec, "s   per-call=", sec / N * 1e9, "ns");
}

} // namespace

int main()
{
	sink.reserve(1u << 20);
	try
	{
		::fast_io::io::println("--- zero-length literals (parser only) ---");
		for (::std::size_t k : {0u, 1u, 2u, 4u, 8u, 16u})
		{
			run(zero_nodes, k * 2, k);
		}
		::fast_io::io::println("--- one-byte literals (parser + write) ---");
		for (::std::size_t k : {0u, 1u, 2u, 4u, 8u, 16u})
		{
			sink.clear();
			run(one_nodes, k * 3, k);
		}
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
