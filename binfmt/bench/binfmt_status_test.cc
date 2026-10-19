// status-print indirector: values go through the shared binfmt interpreter
// (types in the cookie), unsupported shapes go through the custom thunk, and
// an all-span call degenerates to a plain scatter write.
#include <fast_io.h>

#include "binfmt_status.h"

int main() try
{
	auto out{::fast_io::mnp::binfmt(::fast_io::c_stdout())};
	::fast_io::io::print(out, "hello ", 42, 'A', true, static_cast<unsigned long>(7),
						 static_cast<long>(-5), ::fast_io::mnp::hex(255),
						 static_cast<long long>(-1234567890123ll),
						 static_cast<unsigned long long>(18446744073709551615ull),
						 ::fast_io::mnp::boolalpha(true), " end", "\n");
	::fast_io::io::println(out, "alpha", "beta", "gamma"); // all spans -> scatter write
	::fast_io::io::print(out, "no newline all spans");     // all spans, no line
	::fast_io::io::println(out);                           // zero args, just a newline
	return 0;
}
catch throws(::std::error e)
{
	::fast_io::io::perrln(e);
	return 1;
}
