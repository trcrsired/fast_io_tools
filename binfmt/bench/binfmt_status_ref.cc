// reference: the same values through ordinary fast_io (no binfmt)
#include <fast_io.h>

int main() try
{
	::fast_io::io::print(::fast_io::c_stdout(), "hello ", 42, 'A', true,
						 static_cast<unsigned long>(7), static_cast<long>(-5),
						 ::fast_io::mnp::hex(255),
						 static_cast<long long>(-1234567890123ll),
						 static_cast<unsigned long long>(18446744073709551615ull),
						 ::fast_io::mnp::boolalpha(true), " end", "\n");
	return 0;
}
catch throws(::std::error e)
{
	::fast_io::io::perrln(e);
	return 1;
}
