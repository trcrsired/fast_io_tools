// code-size benchmark (native side): the same forty call sites as
// status_size.cc, but through ordinary fast_io. Compare emit() sizes.
#include <fast_io.h>
#include <fast_io_dsal/string.h>

void emit(::fast_io::string &bm, ::std::size_t i) throws
{
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, bm)};
	auto &out{oref};
	::fast_io::io::println(out, "msg", 1u, " v=", i + 1, " done");
	::fast_io::io::println(out, "msg", 2u, " v=", i + 2, " done");
	::fast_io::io::println(out, "msg", 3u, " v=", i + 3, " done");
	::fast_io::io::println(out, "msg", 4u, " v=", i + 4, " done");
	::fast_io::io::println(out, "msg", 5u, " v=", i + 5, " done");
	::fast_io::io::println(out, "msg", 6u, " v=", i + 6, " done");
	::fast_io::io::println(out, "msg", 7u, " v=", i + 7, " done");
	::fast_io::io::println(out, "msg", 8u, " v=", i + 8, " done");
	::fast_io::io::println(out, "msg", 9u, " v=", i + 9, " done");
	::fast_io::io::println(out, "msg", 10u, " v=", i + 10, " done");
	::fast_io::io::println(out, "msg", 11u, " v=", i + 11, " done");
	::fast_io::io::println(out, "msg", 12u, " v=", i + 12, " done");
	::fast_io::io::println(out, "msg", 13u, " v=", i + 13, " done");
	::fast_io::io::println(out, "msg", 14u, " v=", i + 14, " done");
	::fast_io::io::println(out, "msg", 15u, " v=", i + 15, " done");
	::fast_io::io::println(out, "msg", 16u, " v=", i + 16, " done");
	::fast_io::io::println(out, "msg", 17u, " v=", i + 17, " done");
	::fast_io::io::println(out, "msg", 18u, " v=", i + 18, " done");
	::fast_io::io::println(out, "msg", 19u, " v=", i + 19, " done");
	::fast_io::io::println(out, "msg", 20u, " v=", i + 20, " done");
	::fast_io::io::println(out, "msg", 21u, " v=", i + 21, " done");
	::fast_io::io::println(out, "msg", 22u, " v=", i + 22, " done");
	::fast_io::io::println(out, "msg", 23u, " v=", i + 23, " done");
	::fast_io::io::println(out, "msg", 24u, " v=", i + 24, " done");
	::fast_io::io::println(out, "msg", 25u, " v=", i + 25, " done");
	::fast_io::io::println(out, "msg", 26u, " v=", i + 26, " done");
	::fast_io::io::println(out, "msg", 27u, " v=", i + 27, " done");
	::fast_io::io::println(out, "msg", 28u, " v=", i + 28, " done");
	::fast_io::io::println(out, "msg", 29u, " v=", i + 29, " done");
	::fast_io::io::println(out, "msg", 30u, " v=", i + 30, " done");
	::fast_io::io::println(out, "msg", 31u, " v=", i + 31, " done");
	::fast_io::io::println(out, "msg", 32u, " v=", i + 32, " done");
	::fast_io::io::println(out, "msg", 33u, " v=", i + 33, " done");
	::fast_io::io::println(out, "msg", 34u, " v=", i + 34, " done");
	::fast_io::io::println(out, "msg", 35u, " v=", i + 35, " done");
	::fast_io::io::println(out, "msg", 36u, " v=", i + 36, " done");
	::fast_io::io::println(out, "msg", 37u, " v=", i + 37, " done");
	::fast_io::io::println(out, "msg", 38u, " v=", i + 38, " done");
	::fast_io::io::println(out, "msg", 39u, " v=", i + 39, " done");
	::fast_io::io::println(out, "msg", 40u, " v=", i + 40, " done");
}
