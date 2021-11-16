#include<fast_io.h>
#include"sha256.h"
#include"md5_sha_hash_context.h"
#include"hash_print_reserve_define.h"

int main()
{
	::fast_io::sha256_context ctx;
	print(fast_io::mnp::u8as_file(ctx),u8"Hello WorldHello WorldHello WorldHello WorldHello World1124124124");
	ctx.do_final();
	println(ctx.hash());
#if 0
	ctx.reset();
	print(fast_io::mnp::u8as_file(ctx),u8"Hello World");
	ctx.do_final();
	println(ctx.hash());
#endif
}
