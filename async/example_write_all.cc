#include <fast_io.h>
#include <fast_io_device.h>
#include "linux_io_uring.h"

int main()
{
	namespace fi = fast_io;
	try
	{
		fi::linux_io_uring sched{fi::io_async};

		fi::posix_file file{"/tmp/async_write_all.bin",
							fi::open_mode::out | fi::open_mode::creat | fi::open_mode::trunc};

		char const msg[] = "hello async world\n";
		auto const *first{reinterpret_cast<::std::byte const *>(msg)};

		bool done{};
		::std::cxx_std_error result{};
		fi::liburing::async_write_all_bytes_decay_callback(sched, fi::posix_io_observer{file.native_handle()}, first,
										first + sizeof(msg) - 1, [&](::std::cxx_std_error e) noexcept {
											result = e;
											done = true;
										});

		while (!done)
		{
			fi::liburing::io_async_wait(sched);
		}

		if (result.domain != nullptr)
		{
			throw throws result;
		}
	}
	catch throws(::std::error e)
	{
		return 1;
	}
	return 0;
}
