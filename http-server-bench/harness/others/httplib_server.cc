#include "httplib.h"
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		return 2;
	}
	httplib::Server svr;
	svr.set_tcp_nodelay(true);
	svr.set_keep_alive_max_count(0);
	if (!svr.set_mount_point("/", argv[2]))
	{
		return 1;
	}
	svr.listen("127.0.0.1", std::atoi(argv[1]));
	return 0;
}
