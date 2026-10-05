#include <fast_io.h>
#include <fast_io_device.h>
#include "linux_io_uring.h"
#include "task.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>

namespace
{
/* a throws coroutine: herbception errors escaping the body are delivered
 * to promise.unhandled_herbception automatically; an awaiting caller gets
 * them back through the channel at co_await */
fast_io::details::io_uring_task<fast_io::native_global_allocator>
coro_write_all(fast_io::linux_io_uring_observer sched, fast_io::posix_io_observer ob) throws
{
	char const msg[] = "coroutine write_all\n";
	auto const *f{reinterpret_cast<::std::byte const *>(msg)};
	co_await fast_io::liburing::async_write_all_bytes_decay(sched, ob, f, f + sizeof(msg) - 1);
}

fast_io::details::io_uring_task<fast_io::native_global_allocator>
coro_outer(fast_io::linux_io_uring_observer sched, fast_io::posix_io_observer ob) throws
{
	co_await coro_write_all(sched, ob);
}

/* status allocator shaped like linux_kmalloc_allocator: every operation
 * takes the handle by value; _try throws a herbception on failure.
 * handle < 0 simulates an always-failing allocator. */
struct test_status_allocator
{
	using handle_type = int;
	static inline int allocs{};
	static inline int frees{};
	static inline int last_free_handle{-1};
	static inline void *handle_allocate_try(handle_type h, ::std::size_t n) throws
	{
		++allocs;
		void *p{h < 0 ? nullptr : ::std::malloc(n == 0 ? 1 : n)};
		if (p == nullptr)
		{
			throw throws ::std::errc::not_enough_memory;
		}
		return p;
	}
	static inline void handle_deallocate(handle_type h, void *p) noexcept
	{
		++frees;
		last_free_handle = h;
		::std::free(p);
	}
};

/* status-allocator coroutine: the handle is the first parameter and
 * reaches promise_type::operator new through the coroutine arguments.
 * noinline keeps the ramp out of main so HALO cannot elide the frame
 * allocation and the allocator counters stay observable under LTO. */
__attribute__((noinline)) fast_io::details::io_uring_task<test_status_allocator>
coro_status(test_status_allocator::handle_type h, fast_io::linux_io_uring_observer sched,
			fast_io::posix_io_observer ob) throws
{
	co_await coro_write_all(sched, ob);
}
} // namespace

int main()
{
	namespace fi = fast_io;
	fi::linux_io_uring ring{fi::native_interface, 16, 0};

	// raw path: nop
	{
		auto *sqe{fi::liburing::io_uring_get_sqe(*ring.native_handle())};
		fi::liburing::io_uring_prep_nop(sqe);
		fi::liburing::io_uring_sqe_set_data64(sqe, 0xdead);
		fi::liburing::io_uring_submit(*ring.native_handle());
		auto *cqe{fi::liburing::io_uring_wait_cqe(*ring.native_handle())};
		fi::println("nop res=", cqe->res);
		fi::liburing::io_uring_cqe_seen(*ring.native_handle(), cqe);
	}

	// batch drain (raw)
	for (::std::uint64_t i{}; i < 8; ++i)
	{
		auto *sqe{fi::liburing::io_uring_get_sqe(*ring.native_handle())};
		fi::liburing::io_uring_prep_nop(sqe);
		fi::liburing::io_uring_sqe_set_data64(sqe, i);
	}
	fi::liburing::io_uring_submit(*ring.native_handle());
	fi::liburing::io_uring_wait_cqes(*ring.native_handle(), 8);
	fi::liburing::io_uring_cqe *cqes[8];
	auto n{fi::liburing::io_uring_peek_batch_cqe(*ring.native_handle(), cqes, 8)};
	fi::println("batch: ", n, " cqes");
	::std::uint32_t slots{};
	for (::std::uint32_t i{}; i < n; ++i)
	{
		slots += fi::liburing::io_uring_cqe_nr(cqes[i]);
	}
	fi::liburing::io_uring_cq_advance(*ring.native_handle(), slots);

	fi::posix_file file{"/tmp/iouring_test.bin",
						fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat | fi::open_mode::trunc};

	// functor callback: (cxx_std_error, writtenptr)
	{
		char const cb_msg[] = "callback-write";
		auto const *cb{reinterpret_cast<::std::byte const *>(cb_msg)};
		::std::byte const *written{};
		::std::cxx_std_error cb_err{};
		bool fired{};
		fi::liburing::async_write_some_bytes_callback_define(ring, fi::posix_io_observer{file.native_handle()}, cb,
												   cb + sizeof(cb_msg) - 1,
												   [&](::std::cxx_std_error e, ::std::byte const *p) noexcept {
													   cb_err = e;
													   written = p;
													   fired = true;
												   });
		fi::liburing::io_async_wait(ring);
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
		fi::liburing::async_write_all_bytes_decay_callback(ring, fi::posix_io_observer{file2.native_handle()}, wb2,
												 wb2 + big.size(), [&](::std::cxx_std_error e) noexcept {
													 all_err = e;
													 all_fired = true;
												 });
		while (!all_fired)
		{
			fi::liburing::io_async_wait(ring);
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
		fi::liburing::async_write_all_bytes_decay_callback(ring, fi::posix_io_observer{-1}, wb2, wb2 + 16,
												 [&](::std::cxx_std_error e) noexcept {
													 bad_err = e;
													 bad_fired = true;
												 });
		fi::liburing::io_async_wait(ring);
		fi::println("write_all badfd: fired=", bad_fired, " code=", bad_err.code, " (EBADF=", EBADF, ")");
	}

	// coroutine: outer co_awaits inner; errors propagate promise->channel->promise
	{
		auto t{coro_outer(ring, fi::posix_io_observer{file.native_handle()})};
		auto h{t.native_handle()};
		h.resume();
		while (!h.done())
		{
			fi::liburing::io_async_wait(ring);
		}
		auto const &err{h.promise().error};
		fi::println("coro write_all: domain=", reinterpret_cast<::std::uintptr_t>(err.domain),
					" code=", err.code);
	}

	{
		auto t{coro_outer(ring, fi::posix_io_observer{-1})};
		auto h{t.native_handle()};
		h.resume();
		fi::liburing::io_async_wait(ring);
		auto const &err{h.promise().error};
		fi::println("coro badfd: domain!=0: ", err.domain != nullptr, " code=", err.code, " (EBADF=", EBADF,
					")");
	}

	// status allocator: frame goes through handle_allocate_try, freed via
	// handle_deallocate with the same handle recovered from the prefix
	{
		int const hdl{7};
		::std::cxx_std_error err{};
		{
			auto t{coro_status(hdl, ring, fi::posix_io_observer{file.native_handle()})};
			auto h{t.native_handle()};
			h.resume();
			while (!h.done())
			{
				fi::liburing::io_async_wait(ring);
			}
			err = h.promise().error;
		} // task dtor -> operator delete -> handle_deallocate
		fi::println("status coro: code=", err.code, " allocs=", test_status_allocator::allocs,
					" frees=", test_status_allocator::frees,
					" free_handle=", test_status_allocator::last_free_handle);
	}

	// status allocator failure: operator new throws -> caller catches it
	// through the channel, no frame is created
	{
		int const before{test_status_allocator::allocs};
		try
		{
			auto t{coro_status(-1, ring, fi::posix_io_observer{file.native_handle()})};
			fi::println("BUG: status alloc failure not propagated");
		}
		catch throws(::std::error e)
		{
			fi::println("status alloc fail caught: code=", e.code(), " (ENOMEM=", ENOMEM,
						") allocs_delta=", test_status_allocator::allocs - before);
		}
	}

	// non-blocking peek + timed wait
	fi::println("peek (empty): ", fi::liburing::io_async_peek(ring));
	fi::println("wait 50ms: ", fi::liburing::io_async_wait_timeout(ring, fi::posix_statx_timestamp64{0, 50000000}));

	return 0;
}
