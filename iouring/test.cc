#include <fast_io.h>
#include <fast_io_device.h>
#include "linux_io_uring.h"
#include "task.h"
#include <cerrno>
#include <cstring>
#include <vector>
#include <unistd.h>

namespace
{
/* coroutine bodies must catch their own herbceptions; the error is handed
 * to promise.unhandled_herbception so an awaiting caller gets it back
 * through the channel */
fast_io::details::io_uring_task coro_write_all(fast_io::linux_io_uring_observer sched,
											   fast_io::posix_io_observer ob)
{
	auto &promise{co_await fast_io::details::io_uring_task_promise_access{}};
	try
	{
		char const msg[] = "coroutine write_all\n";
		auto const *f{reinterpret_cast<::std::byte const *>(msg)};
		auto e{co_await fast_io::async_write_all_bytes_decay(sched, ob, f, f + sizeof(msg) - 1)};
		if (e.domain != nullptr)
		{
			throw throws e;
		}
	}
	catch throws(::std::error e)
	{
		promise.unhandled_herbception(e.release());
	}
}

fast_io::details::io_uring_task coro_outer(fast_io::linux_io_uring_observer sched,
										   fast_io::posix_io_observer ob)
{
	auto &promise{co_await fast_io::details::io_uring_task_promise_access{}};
	try
	{
		co_await coro_write_all(sched, ob);
	}
	catch throws(::std::error e)
	{
		promise.unhandled_herbception(e.release());
	}
}
} // namespace

int main()
{
	namespace fi = fast_io;
	fi::linux_io_uring ring{fi::native_interface, 16, 0};

	// raw path: nop
	{
		auto *sqe{fi::io_uring_get_sqe(ring)};
		fi::io_uring_prep_nop(sqe);
		fi::io_uring_sqe_set_data64(sqe, 0xdead);
		fi::io_uring_submit(ring);
		auto *cqe{fi::io_uring_wait_cqe(ring)};
		fi::println("nop res=", cqe->res);
		fi::io_uring_cqe_seen(ring, cqe);
	}

	// batch drain (raw)
	for (::std::uint64_t i{}; i < 8; ++i)
	{
		auto *sqe{fi::io_uring_get_sqe(ring)};
		fi::io_uring_prep_nop(sqe);
		fi::io_uring_sqe_set_data64(sqe, i);
	}
	fi::io_uring_submit(ring);
	fi::io_uring_wait_cqes(ring, 8);
	fi::io_uring_cqe *cqes[8];
	auto n{fi::io_uring_peek_batch_cqe(ring, cqes, 8)};
	fi::println("batch: ", n, " cqes");
	::std::uint32_t slots{};
	for (::std::uint32_t i{}; i < n; ++i)
	{
		slots += fi::io_uring_cqe_nr(cqes[i]);
	}
	fi::io_uring_cq_advance(ring, slots);

	fi::posix_file file{"/tmp/iouring_test.bin",
						fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat | fi::open_mode::trunc};

	// functor callback: (cxx_std_error, writtenptr)
	{
		char const cb_msg[] = "callback-write";
		auto const *cb{reinterpret_cast<::std::byte const *>(cb_msg)};
		::std::byte const *written{};
		::std::cxx_std_error cb_err{};
		bool fired{};
		fi::async_write_some_bytes_callback_define(ring, fi::posix_io_observer{file.native_handle()}, cb,
												   cb + sizeof(cb_msg) - 1,
												   [&](::std::cxx_std_error e, ::std::byte const *p) noexcept {
													   cb_err = e;
													   written = p;
													   fired = true;
												   });
		fi::io_async_wait(ring);
		fi::println("callback: fired=", fired, " domain=", reinterpret_cast<::std::uintptr_t>(cb_err.domain),
					" code=", cb_err.code, " written=", written - cb);
	}

	// write_all: chained resubmission, callback takes only cxx_std_error
	{
		fi::posix_file file2{"/tmp/iouring_test2.bin",
							 fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat | fi::open_mode::trunc};
		::std::vector<char> big(1u << 20);
		for (::std::size_t i{}; i < big.size(); ++i)
		{
			big[i] = static_cast<char>(i * 31 + 7);
		}
		auto const *wb2{reinterpret_cast<::std::byte const *>(big.data())};
		::std::cxx_std_error all_err{};
		bool all_fired{};
		fi::async_write_all_bytes_decay_callback(ring, fi::posix_io_observer{file2.native_handle()}, wb2,
												 wb2 + big.size(), [&](::std::cxx_std_error e) noexcept {
													 all_err = e;
													 all_fired = true;
												 });
		while (!all_fired)
		{
			fi::io_async_wait(ring);
		}
		fi::println("write_all: fired=", all_fired, " domain=", reinterpret_cast<::std::uintptr_t>(all_err.domain),
					" code=", all_err.code);

		// verify tail bytes with a plain positioned read
		char tail_buf[16]{};
		auto got{::pread(file2.native_handle(), tail_buf, sizeof(tail_buf),
						 static_cast<long>(big.size() - sizeof(tail_buf)))};
		bool tail_ok{got == static_cast<long>(sizeof(tail_buf))};
		for (::std::size_t i{}; tail_ok && i < sizeof(tail_buf); ++i)
		{
			tail_ok = tail_buf[i] == big[big.size() - sizeof(tail_buf) + i];
		}
		fi::println("write_all tail verify: ", tail_ok);

		// error path: bad fd -> single callback with posix domain + EBADF
		::std::cxx_std_error bad_err{};
		bool bad_fired{};
		fi::async_write_all_bytes_decay_callback(ring, fi::posix_io_observer{-1}, wb2, wb2 + 16,
												 [&](::std::cxx_std_error e) noexcept {
													 bad_err = e;
													 bad_fired = true;
												 });
		fi::io_async_wait(ring);
		fi::println("write_all badfd: fired=", bad_fired, " code=", bad_err.code, " (EBADF=", EBADF, ")");
	}

	// coroutine: outer co_awaits inner; errors propagate promise->channel->promise
	{
		auto t{coro_outer(ring, fi::posix_io_observer{file.native_handle()})};
		auto h{t.native_handle()};
		h.resume();
		while (!h.done())
		{
			fi::io_async_wait(ring);
		}
		auto const &err{h.promise().error};
		fi::println("coro write_all: domain=", reinterpret_cast<::std::uintptr_t>(err.domain),
					" code=", err.code);
		h.destroy();
	}

	{
		auto t{coro_outer(ring, fi::posix_io_observer{-1})};
		auto h{t.native_handle()};
		h.resume();
		fi::io_async_wait(ring);
		auto const &err{h.promise().error};
		fi::println("coro badfd: domain!=0: ", err.domain != nullptr, " code=", err.code, " (EBADF=", EBADF,
					")");
		h.destroy();
	}

	// non-blocking peek + timed wait
	fi::println("peek (empty): ", fi::io_async_peek(ring));
	fi::println("wait 50ms: ", fi::io_async_wait_timeout(ring, ::std::chrono::milliseconds{50}));

	return 0;
}
