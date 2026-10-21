/* Does io_uring poll-arm IORING_OP_SPLICE pipe->nonblocking socket on EAGAIN,
 * or does it complete the op with -EAGAIN right away? */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include "liburing.h"

static int peer_fd;
static long now_ms(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

static void *reader(void *arg)
{
	char buf[65536];
	(void)arg;
	/* let the splice sit in EAGAIN for a while, then start draining */
	usleep(1500 * 1000);
	for (;;)
	{
		ssize_t n = read(peer_fd, buf, sizeof(buf));
		if (n <= 0)
			break;
	}
	return NULL;
}

int main(void)
{
	int sp[2], pp[2];
	if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sp) < 0)
		return 1;
	peer_fd = sp[1];
	int sndbuf = 32768;
	setsockopt(sp[0], SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
	if (pipe(pp) < 0)
		return 1;
	fcntl(pp[1], F_SETPIPE_SZ, 1048576);

	/* fill the socket send buffer until EAGAIN */
	char junk[4096];
	memset(junk, 'x', sizeof(junk));
	long filled = 0;
	for (;;)
	{
		ssize_t n = write(sp[0], junk, sizeof(junk));
		if (n < 0)
			break;
		filled += n;
	}
	printf("socket send buffer filled: %ld bytes (errno %d)\n", filled, errno);

	/* fill the pipe with 1 MiB */
	long piped = 0;
	while (piped < 1048576)
	{
		ssize_t n = write(pp[1], junk, sizeof(junk));
		if (n <= 0)
			break;
		piped += n;
	}
	printf("pipe filled: %ld bytes\n", piped);

	struct io_uring ring;
	if (io_uring_queue_init(8, &ring, 0) < 0)
		return 1;
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_splice(sqe, pp[0], -1, sp[0], -1, 65536, 0);
	long t0 = now_ms();
	io_uring_submit(&ring);

	struct io_uring_cqe *cqe;
	/* peek without blocking: is it completed immediately? */
	usleep(200 * 1000);
	int peek = io_uring_peek_cqe(&ring, &cqe);
	if (peek == 0)
	{
		printf("t=%ldms: completed immediately: res=%d\n", now_ms() - t0, cqe->res);
		io_uring_cqe_seen(&ring, cqe);
	}
	else
	{
		printf("t=%ldms: still pending, starting reader and waiting\n", now_ms() - t0);
		pthread_t th;
		pthread_create(&th, NULL, reader, NULL);
		io_uring_wait_cqe(&ring, &cqe);
		printf("t=%ldms: completed after reader: res=%d\n", now_ms() - t0, cqe->res);
		io_uring_cqe_seen(&ring, cqe);
		pthread_join(th, NULL);
	}
	io_uring_queue_exit(&ring);
	return 0;
}
