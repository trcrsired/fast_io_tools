#include<fast_io.h>
#include"sha256.h"
#include"md5_sha_hash_context.h"

int main()
{
	::fast_io::sha256_context ctx;
	print(fast_io::mnp::as_file(ctx),"Hello World\n");
}