#pragma once
/*
 * Minimal lazy coroutine task used by the async defines. A coroutine
 * declared throws delivers herbception errors escaping its body to
 * promise.unhandled_herbception(cxx_std_error) automatically; an awaiting
 * caller receives the error back as a real herbception: await_resume()
 * throws it through the channel like any throws function. Without an
 * awaiting caller the error stays observable through
 * handle.promise().error.
 *
 * The coroutine frame is allocated through a fast_io allocator — a
 * template parameter, not a global default — so the task also works in
 * freestanding environments where there is no global operator new.
 *
 * Statusless allocators (generic_allocator_adapter<alloc>::has_status
 * == false) need nothing extra:
 *     io_uring_task<alloc> f(args...) throws { ... }
 *
 * Status (handle-based) allocators take the allocator handle as the
 * coroutine's first parameter; the compiler passes the coroutine
 * arguments to promise_type::operator new:
 *     io_uring_task<alloc> f(alloc::handle_type handle, args...) throws
 * A status coroutine without the leading handle is a hard error rather
 * than a silent fall back to global operator new.
 *
 * Frame allocation failures are herbceptions: operator new uses the
 * *_try entry points and is itself throws, so a throws coroutine
 * propagates the failure to the caller through the channel like any
 * other ramp-phase failure.
 */

#include <coroutine>

#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io::details
{

template <typename allocator>
struct io_uring_task
{
	using allocator_type = allocator;
	using untyped_allocator_type = ::fast_io::generic_allocator_adapter<allocator_type>;
	static inline constexpr bool alloc_with_status{untyped_allocator_type::has_status};
	using alloc_handle_type =
		::std::conditional_t<alloc_with_status, typename untyped_allocator_type::handle_type, allocator_type>;
	/* status handles ride in a header ahead of the coroutine frame inside
	 * the same allocation so operator delete — which receives no
	 * coroutine parameters — can recover the handle to deallocate with */
	static inline constexpr ::std::size_t status_prefix_size{
		((sizeof(alloc_handle_type) + untyped_allocator_type::default_alignment - 1) /
		 untyped_allocator_type::default_alignment) *
		untyped_allocator_type::default_alignment};

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

		static inline void *operator new(::std::size_t n, auto &&...) throws
			requires(!alloc_with_status)
		{
			if constexpr (untyped_allocator_type::has_native_allocate_try)
			{
				return untyped_allocator_type::allocate_try(n);
			}
			else
			{
				return untyped_allocator_type::allocate_die(n);
			}
		}
		static inline void *operator new(::std::size_t n, alloc_handle_type handle, auto &&...) throws
			requires(alloc_with_status)
		{
			void *base;
			if constexpr (untyped_allocator_type::has_native_handle_allocate_try)
			{
				base = untyped_allocator_type::handle_allocate_try(handle, status_prefix_size + n);
			}
			else
			{
				base = untyped_allocator_type::handle_allocate_die(handle, status_prefix_size + n);
			}
			*static_cast<alloc_handle_type *>(base) = handle;
			return static_cast<char unsigned *>(base) + status_prefix_size;
		}
		/* a status-allocator coroutine without the handle parameter must
		 * not silently fall back to global operator new */
		static void *operator new(::std::size_t) noexcept
			requires(alloc_with_status) = delete;

		static inline void operator delete(void *p, ::std::size_t n) noexcept
		{
			if constexpr (alloc_with_status)
			{
				auto *base{static_cast<char unsigned *>(p) - status_prefix_size};
				auto handle{*reinterpret_cast<alloc_handle_type const *>(base)};
				untyped_allocator_type::handle_deallocate_n(handle, base, status_prefix_size + n);
			}
			else
			{
				untyped_allocator_type::deallocate_n(p, n);
			}
		}
	};
	::std::coroutine_handle<promise_type> handle{};

	io_uring_task() = default;
	inline constexpr io_uring_task(::std::coroutine_handle<promise_type> h) noexcept
		: handle{h}
	{
	}
	io_uring_task(io_uring_task const &) = delete;
	io_uring_task &operator=(io_uring_task const &) = delete;
	inline io_uring_task(io_uring_task &&other) noexcept
		: handle{other.handle}
	{
		other.handle = nullptr;
	}
	inline io_uring_task &operator=(io_uring_task &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		if (handle != nullptr)
		{
			handle.destroy();
		}
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}
	/* owns the coroutine frame: destroys it when the task object dies.
	 * An awaited temporary task frees its (already completed) frame when
	 * the await expression ends, so child tasks don't leak either. */
	inline ~io_uring_task()
	{
		if (handle != nullptr)
		{
			handle.destroy();
		}
	}

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
		rethrow_if_error();
		return handle.promise().error;
	}
	/* synchronous error check for non-coroutine callers: rethrows a
	 * stored herbception error through the channel */
	inline void rethrow_if_error() throws
	{
		auto e{handle.promise().error};
		if (e.domain != nullptr)
		{
			throw throws e;
		}
	}
};

/* co_await this inside an io_uring_task coroutine to reach its promise */
template <typename allocator>
struct io_uring_task_promise_access
{
	using promise_type = typename io_uring_task<allocator>::promise_type;
	promise_type *promise{};
	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline bool await_suspend(::std::coroutine_handle<promise_type> h) noexcept
	{
		promise = __builtin_addressof(h.promise());
		return false;
	}
	inline promise_type &await_resume() noexcept
	{
		return *promise;
	}
};

} // namespace fast_io::details

#include <fast_io_dsal/impl/misc/pop_macros.h>
