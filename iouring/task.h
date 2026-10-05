#pragma once
/*
 * Minimal lazy coroutine task used by the async defines. Coroutines
 * cannot declare throws: the body catches its own herbceptions and hands
 * them to promise.unhandled_herbception(cxx_std_error) — the same shape
 * as the planned compiler hook of that name. An awaiting caller receives
 * the error back as a real herbception: await_resume() throws it through
 * the channel like any throws function. Without an awaiting caller the
 * error stays observable through handle.promise().error.
 */

#if !defined(__HERBCEPTIONS__)
#error "task.h requires a compiler with herbceptions support (-fherbceptions)"
#endif

#include <coroutine>

#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io::details
{

struct io_uring_task
{
	struct promise_type
	{
		::std::cxx_std_error error{};
		::std::coroutine_handle<> continuation{};

		io_uring_task get_return_object() noexcept
		{
			return {::std::coroutine_handle<promise_type>::from_promise(*this)};
		}
		static constexpr ::std::suspend_always initial_suspend() noexcept
		{
			return {};
		}
		struct final_awaiter
		{
			static constexpr bool await_ready() noexcept
			{
				return false;
			}
			static inline ::std::coroutine_handle<> await_suspend(::std::coroutine_handle<promise_type> h) noexcept
			{
				auto continuation{h.promise().continuation};
				if (continuation != nullptr)
				{
					return continuation;
				}
				return ::std::noop_coroutine();
			}
			static constexpr void await_resume() noexcept
			{
			}
		};
		static constexpr final_awaiter final_suspend() noexcept
		{
			return {};
		}
		void unhandled_herbception(::std::cxx_std_error e) noexcept
		{
			error = e;
		}
		void unhandled_exception() noexcept
		{
			::fast_io::fast_terminate();
		}
		static constexpr void return_void() noexcept
		{
		}
	};
	::std::coroutine_handle<promise_type> handle{};

	inline ::std::coroutine_handle<promise_type> native_handle() const noexcept
	{
		return handle;
	}
	inline bool await_ready() const noexcept
	{
		return false;
	}
	inline ::std::coroutine_handle<> await_suspend(::std::coroutine_handle<> h) noexcept
	{
		handle.promise().continuation = h;
		return handle;
	}
	inline ::std::cxx_std_error await_resume() throws
	{
		auto e{handle.promise().error};
		if (e.domain != nullptr)
		{
			throw throws e;
		}
		return e;
	}
};

/* co_await this inside an io_uring_task coroutine to reach its promise */
struct io_uring_task_promise_access
{
	io_uring_task::promise_type *promise{};
	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline bool await_suspend(::std::coroutine_handle<io_uring_task::promise_type> h) noexcept
	{
		promise = __builtin_addressof(h.promise());
		return false;
	}
	inline io_uring_task::promise_type &await_resume() noexcept
	{
		return *promise;
	}
};

} // namespace fast_io::details

#include <fast_io_dsal/impl/misc/pop_macros.h>
