#include"../../fast_io/include/fast_io.h"
#include"token.h"

int main()
{
	constexpr auto tok(parse_token(u8"#define PF_UNSPEC	0	/* Unspecified.  */"));
	print("first: [",tok.first.start,",",tok.first.last,")\n"
	"second: [",tok.second.start,",",tok.second.last,")\n"
	"third: [",tok.third.start,",",tok.third.last,")\n"
	"fourth: [",tok.fourth.start,",",tok.fourth.last,")\n");
}