#include <fast_io.h>
#include <fast_io_device.h>
#include "linux_io_uring.h"
#include "task.h"

namespace
{
::fast_io::details::io_uring_task<::fast_io::native_global_allocator> test_async_write_all(fast_io::linux_io_uring_observer sched, fast_io::posix_io_observer ob,
										  ::std::byte const *first, ::std::byte const *last) throws
{
	co_await ::fast_io::liburing::async_write_all_bytes_decay(sched, ob, first, last);
}
} // namespace

int main()
{
	namespace fi = fast_io;
	try
	{
		fi::linux_io_uring sched{fi::io_async};

		fi::posix_file file{"hello_async.txt", fi::open_mode::out};

		char const msg[] = "hello async world\n";
		auto const *first{reinterpret_cast<::std::byte const *>(msg)};

		auto task{test_async_write_all(sched, fi::posix_io_observer{file.native_handle()}, first,
							first + sizeof(msg) - 1)};
		auto h{task.native_handle()};
		h.resume();
		while (!h.done())
		{
			fi::liburing::io_async_wait(sched);
		}

		task.rethrow_if_error();
	}
	catch throws(::std::error e)
	{
		return 1;
	}
	return 0;
}
