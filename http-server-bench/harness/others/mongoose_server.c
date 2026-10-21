#include "mongoose.h"

static const char *s_root;

static void fn(struct mg_connection *c, int ev, void *ev_data)
{
	if (ev == MG_EV_HTTP_MSG)
	{
		struct mg_http_serve_opts opts = {0};
		opts.root_dir = s_root;
		mg_http_serve_dir(c, (struct mg_http_message *)ev_data, &opts);
	}
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		return 2;
	}
	s_root = argv[2];
	struct mg_mgr mgr;
	mg_mgr_init(&mgr);
	char url[128];
	mg_snprintf(url, sizeof(url), "http://127.0.0.1:%d", atoi(argv[1]));
	if (mg_http_listen(&mgr, url, fn, NULL) == NULL)
	{
		return 1;
	}
	for (;;)
	{
		mg_mgr_poll(&mgr, 1000);
	}
	return 0;
}
