/* Closed-loop HTTP/1.1 keep-alive load generator.
 * usage: loadgen <port> <path> <connections> <seconds>
 * Prints one line of key=value stats.
 */
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MAX_CONNS 4096
#define READ_CHUNK 262144
#define MAX_SAMPLES (1u << 23)

struct conn
{
	int fd;
	int connecting;
	size_t blen;
	int64_t body_remaining;
	size_t header_end;
	int header_done;
	size_t scan_from;
	uint64_t t0_ns;
	uint64_t reqs;
};

static struct conn conns[MAX_CONNS];
static uint8_t *bufs[MAX_CONNS];
static uint32_t *lat_us;
static size_t lat_n, lat_cap = MAX_SAMPLES;
static uint64_t total_bytes, total_reqs, total_errors, total_connects;
static uint64_t now_ns(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static int send_request(int i, const char *req, size_t reqlen)
{
	struct conn *c = &conns[i];
	for (;;)
	{
		ssize_t n = send(c->fd, req, reqlen, MSG_NOSIGNAL);
		if (n == (ssize_t)reqlen)
		{
			c->t0_ns = now_ns();
			return 0;
		}
		if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
			return 0; /* let epoll wake us; rare for small requests */
		if (n < 0 && errno == EINTR)
			continue;
		return -1;
	}
}

static void note_latency(uint64_t t0)
{
	uint64_t d = now_ns() - t0;
	uint32_t us = (uint32_t)(d / 1000);
	if (lat_n < lat_cap)
		lat_us[lat_n++] = us;
}

static int cmp_u32(const void *a, const void *b)
{
	uint32_t x = *(const uint32_t *)a, y = *(const uint32_t *)b;
	return (x > y) - (x < y);
}

static int connect_one(int i, struct sockaddr_in *sa)
{
	if (conns[i].fd >= 0)
		close(conns[i].fd);
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (fd < 0)
		return -1;
	int one = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
	conns[i].fd = fd;
	conns[i].connecting = 1;
	conns[i].blen = 0;
	conns[i].header_done = 0;
	conns[i].body_remaining = -1;
	conns[i].scan_from = 0;
	int r = connect(fd, (struct sockaddr *)sa, sizeof(*sa));
	if (r == 0)
	{
		conns[i].connecting = 0;
		++total_connects;
		return 0;
	}
	if (errno == EINPROGRESS)
		return 0;
	return -1;
}

/* a reconnect closes the old fd, which drops its epoll registration; the
 * new socket may reuse the fd number, so MOD returns ENOENT and must fall
 * back to ADD */
static void arm_conn(int ep, int i, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.u32 = (uint32_t)i;
	if (epoll_ctl(ep, EPOLL_CTL_MOD, conns[i].fd, &ev) != 0 && errno == ENOENT)
		epoll_ctl(ep, EPOLL_CTL_ADD, conns[i].fd, &ev);
}

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		fprintf(stderr, "usage: %s <port> <path> <connections> <seconds>\n", argv[0]);
		return 2;
	}
	int port = atoi(argv[1]);
	const char *path = argv[2];
	int nconn = atoi(argv[3]);
	double seconds = atof(argv[4]);
	if (port <= 0 || port > 65535 || nconn <= 0 || nconn > MAX_CONNS || seconds <= 0)
		return 2;

	char req[512];
	int reqlen = snprintf(req, sizeof(req),
						  "GET %s HTTP/1.1\r\nHost: 127.0.0.1:%d\r\n"
						  "User-Agent: loadgen\r\nAccept: */*\r\n"
						  "Accept-Encoding: identity\r\nConnection: keep-alive\r\n\r\n",
						  path, port);

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons((uint16_t)port);
	inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);

	lat_us = malloc((size_t)lat_cap * sizeof(uint32_t));
	for (int i = 0; i < nconn; ++i)
	{
		bufs[i] = malloc(READ_CHUNK);
		conns[i].fd = -1;
		if (connect_one(i, &sa) != 0)
		{
			fprintf(stderr, "connect setup failed: %s\n", strerror(errno));
			return 1;
		}
	}

	int ep = epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event ev, events[MAX_CONNS];
	for (int i = 0; i < nconn; ++i)
	{
		ev.events = conns[i].connecting ? EPOLLOUT : EPOLLOUT;
		ev.data.u32 = (uint32_t)i;
		epoll_ctl(ep, EPOLL_CTL_ADD, conns[i].fd, &ev);
	}

	uint64_t start = now_ns();
	uint64_t deadline = start + (uint64_t)(seconds * 1e9);
	struct rusage ru0, ru1;
	getrusage(RUSAGE_SELF, &ru0);

	for (;;)
	{
		uint64_t now = now_ns();
		if (now >= deadline)
			break;
		int64_t wait_ms = (int64_t)((deadline - now) / 1000000ull);
		if (wait_ms > 200)
			wait_ms = 200;
		if (wait_ms < 0)
			wait_ms = 0;
		int n = epoll_wait(ep, events, nconn, (int)wait_ms);
		for (int e = 0; e < n; ++e)
		{
			int i = (int)events[e].data.u32;
			struct conn *c = &conns[i];
			uint32_t mask = events[e].events;
			if (c->connecting)
			{
				int err = 0;
				socklen_t elen = sizeof(err);
				if (getsockopt(c->fd, SOL_SOCKET, SO_ERROR, &err, &elen) != 0 || err != 0)
				{
					++total_errors;
					if (connect_one(i, &sa) != 0)
						return 1;
				}
				else
				{
					c->connecting = 0;
					++total_connects;
					if (send_request(i, req, (size_t)reqlen) != 0)
					{
						++total_errors;
						if (connect_one(i, &sa) != 0)
							return 1;
						continue;
					}
				}
				ev.events = EPOLLIN | (c->connecting ? EPOLLOUT : 0);
				ev.data.u32 = (uint32_t)i;
				arm_conn(ep, i, ev.events);
				continue;
			}
			if (mask & (EPOLLHUP | EPOLLERR))
			{
				++total_errors;
				if (connect_one(i, &sa) != 0)
					return 1;
				ev.events = c->connecting ? EPOLLOUT : (EPOLLIN | EPOLLOUT);
				ev.data.u32 = (uint32_t)i;
				arm_conn(ep, i, ev.events);
				continue;
			}
			/* read whatever is available */
			for (;;)
			{
				ssize_t r = recv(c->fd, bufs[i] + c->blen, READ_CHUNK - c->blen, 0);
				if (r > 0)
				{
					c->blen += (size_t)r;
					total_bytes += (uint64_t)r;
					/* consume as many complete responses as present */
					for (;;)
					{
						if (!c->header_done)
						{
							uint8_t *h = memmem(bufs[i] + c->scan_from, c->blen - c->scan_from,
												"\r\n\r\n", 4);
							if (!h)
							{
								if (c->blen > c->scan_from)
									c->scan_from = c->blen > 3 ? c->blen - 3 : 0;
								break;
							}
							c->header_end = (size_t)(h - bufs[i]) + 4;
							/* parse content-length (case-insensitive) */
							long long cl = -1;
							for (size_t p = 0; p + 15 <= c->header_end; ++p)
							{
								if ((bufs[i][p] | 0x20) == 'c' &&
									strncasecmp((char *)bufs[i] + p, "content-length:", 15) == 0)
								{
									cl = strtoll((char *)bufs[i] + p + 15, NULL, 10);
									break;
								}
							}
							if (cl < 0)
							{
								fprintf(stderr, "no content-length in response\n");
								return 1;
							}
							c->body_remaining = cl;
							c->header_done = 1;
							c->scan_from = 0;
						}
						size_t avail = c->blen - c->header_end;
						if ((int64_t)avail < c->body_remaining)
						{
							c->body_remaining -= (int64_t)avail;
							c->blen = c->header_end;
							break;
						}
						size_t consumed = c->header_end + (size_t)c->body_remaining;
						note_latency(c->t0_ns);
						++c->reqs;
						++total_reqs;
						c->blen -= consumed;
						if (c->blen)
							memmove(bufs[i], bufs[i] + consumed, c->blen);
						c->header_done = 0;
						c->body_remaining = -1;
						c->scan_from = 0;
						if (now_ns() >= deadline)
							break;
						if (send_request(i, req, (size_t)reqlen) != 0)
						{
							++total_errors;
							if (connect_one(i, &sa) != 0)
								return 1;
							ev.events = EPOLLOUT;
							ev.data.u32 = (uint32_t)i;
							arm_conn(ep, i, ev.events);
							break;
						}
					}
					continue;
				}
				if (r == 0)
				{
					++total_errors;
					if (connect_one(i, &sa) != 0)
						return 1;
					ev.events = EPOLLOUT;
					ev.data.u32 = (uint32_t)i;
					arm_conn(ep, i, ev.events);
					break;
				}
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				if (errno == EINTR)
					continue;
				++total_errors;
				if (connect_one(i, &sa) != 0)
					return 1;
				ev.events = EPOLLOUT;
				ev.data.u32 = (uint32_t)i;
				arm_conn(ep, i, ev.events);
				break;
			}
		}
		if (now_ns() >= deadline)
			break;
	}

	uint64_t end = now_ns();
	getrusage(RUSAGE_SELF, &ru1);
	double wall = (double)(end - start) / 1e9;
	double cpu = (double)((ru1.ru_utime.tv_sec - ru0.ru_utime.tv_sec) +
						  (ru1.ru_stime.tv_sec - ru0.ru_stime.tv_sec)) +
				 (double)((ru1.ru_utime.tv_usec - ru0.ru_utime.tv_usec) +
						  (ru1.ru_stime.tv_usec - ru0.ru_stime.tv_usec)) /
					 1e6;

	qsort(lat_us, lat_n, sizeof(uint32_t), cmp_u32);
#define PCT(p) (lat_n ? lat_us[(size_t)((double)(lat_n - 1) * (p) / 100.0)] / 1000.0 : 0.0)
	double mean = 0;
	for (size_t i = 0; i < lat_n; ++i)
		mean += lat_us[i];
	mean = lat_n ? mean / (double)lat_n / 1000.0 : 0.0;
	printf("requests=%llu duration_s=%.3f rps=%.1f bytes=%llu mbps=%.2f "
		   "mean_ms=%.3f p50_ms=%.3f p90_ms=%.3f p99_ms=%.3f p999_ms=%.3f max_ms=%.3f "
		   "errors=%llu connects=%llu client_cpu_s=%.3f samples=%zu\n",
		   (unsigned long long)total_reqs, wall, (double)total_reqs / wall,
		   (unsigned long long)total_bytes, (double)total_bytes / 1e6 / wall, mean,
		   PCT(50), PCT(90), PCT(99), PCT(99.9),
		   lat_n ? lat_us[lat_n - 1] / 1000.0 : 0.0,
		   (unsigned long long)total_errors, (unsigned long long)total_connects, cpu, lat_n);
	return 0;
}
