// smoke test: the batched status path must produce byte-identical output to
// the stock status path, end to end through println/print.

#include <cstring>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_status.h"
#include "binfmt_status_batch.h"

namespace
{

template <typename... Args>
void cmp(char const *what, Args &&...args) throws
{
	::fast_io::string a;
	::fast_io::string b;
	{
		auto oa{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, a)};
		::fast_io::io::println(::fast_io::mnp::binfmt(oa), args...);
	}
	{
		auto ob{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, b)};
		::fast_io::io::println(::fast_io::mnp::binfmt_batch(ob), args...);
	}
	bool const ok{a.size() == b.size() && ::std::memcmp(a.data(), b.data(), a.size()) == 0};
	::fast_io::io::println(ok ? "ok   " : "FAIL ", ::fast_io::mnp::os_c_str(what));
	if (!ok)
	{
		::fast_io::io::println("  a=[", a, "]");
		::fast_io::io::println("  b=[", b, "]");
	}
}

} // namespace

int main()
{
	try
	{
		cmp("int", 42);
		cmp("mixed", "hello ", ::std::size_t{12345}, " world ", -7, " end");
		cmp("hex", ::fast_io::mnp::hex(0xdeadbeefu));
		cmp("oct", ::fast_io::mnp::oct(0755u));
		cmp("bin", ::fast_io::mnp::bin(0b1011u));
		cmp("hexsigned", ::fast_io::mnp::hex(-5));
		cmp("spans", "alpha", "beta", "gamma");
		cmp("chars", "a", 'b', "c");
		cmp("u64", ::std::uint_least64_t{0x1122334455667788ull});
		cmp("i128", static_cast<__int128>(-123456789012345678LL));
		cmp("view", ::std::size_t{7});
		cmp("none");
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln("caught: ", e);
		return 1;
	}
}
