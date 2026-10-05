#pragma once
/*
 * io_uring support implemented entirely on top of fast_io's own Linux
 * system_call<> wrappers. The liburing-equivalent ring machinery lives
 * in linux_liburing.h; this header provides the fast_io-facing
 * observer, queue functions and the async defines.
 */

#if defined(__linux__)

#include <coroutine>

#include "linux_liburing.h"

#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io
{

class linux_io_uring_observer
{
public:
	using native_handle_type = liburing::io_uring_ring_state *;
	/* scheduler-provided allocator: a fast_io generic allocator adapter.
	 * Async defines allocate and free their operation state through
	 * it; schedulers without one fall back to native_global_allocator. */
	using allocator_type = ::fast_io::native_global_allocator;
	native_handle_type ring{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return ring;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{ring};
		ring = nullptr;
		return temp;
	}
	inline constexpr explicit operator bool() const noexcept
	{
		return ring != nullptr;
	}
};

} // namespace fast_io


namespace fast_io
{

/*
 * Owning io_uring object. The ring state is embedded (no heap allocation);
 * linux_io_uring_observer provides the non-owning view taken by all the
 * free functions below.
 */

/*
 * Owning io_uring object. The ring state is embedded (no heap allocation);
 * linux_io_uring_observer provides the non-owning view taken by all the
 * free functions below.
 */
class linux_io_uring : public linux_io_uring_observer
{
public:
	liburing::io_uring_ring_state storage{};

	inline linux_io_uring() noexcept
	{
		this->ring = __builtin_addressof(this->storage);
	}
	inline explicit linux_io_uring(::fast_io::native_interface_t, ::std::uint_least32_t entries, ::std::uint_least32_t flags)
		throws
		: linux_io_uring()
	{
		liburing::details::io_uring_queue_init_impl(this->storage, entries, flags);
	}
	inline explicit linux_io_uring(::fast_io::io_async_t) throws
		: linux_io_uring(::fast_io::native_interface, 64, 0)
	{
	}
	linux_io_uring(linux_io_uring const &) = delete;
	linux_io_uring &operator=(linux_io_uring const &) = delete;
	inline linux_io_uring(linux_io_uring &&other) noexcept
		: linux_io_uring()
	{
		this->storage = other.storage;
		other.storage = {};
	}
	inline linux_io_uring &operator=(linux_io_uring &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		liburing::details::io_uring_queue_exit_impl(this->storage);
		this->storage = other.storage;
		other.storage = {};
		return *this;
	}
	inline ~linux_io_uring()
	{
		liburing::details::io_uring_queue_exit_impl(this->storage);
	}
};

template <::std::integral char_type>
inline constexpr ::fast_io::io_type_t<linux_io_uring_observer>
async_scheduler_type(::fast_io::basic_posix_family_io_observer<::fast_io::posix_family::api, char_type>) noexcept
{
	return {};
}

} // namespace fast_io

namespace fast_io::liburing
{


/* ======================= dispatch model ======================= */

namespace details
{
/*
 * Internal user_data ABI: sqe->user_data points at a state object whose
 * FIRST member is an io_uring_invoke_func; the event loop calls it once
 * per cqe. Not part of the public API — applications only hand functors
 * to the *_callback_define functions.
 * The function receives (bytes transferred, 0) on success and
 * (0, errno value) on failure.
 */
using io_uring_invoke_func = void (*)(void *, ::std::size_t, int) noexcept;

/*
 * Scheduler-provided allocator: a member type allocator_type that is a
 * fast_io generic allocator adapter (e.g. native_global_allocator).
 * Schedulers without one fall back to native_global_allocator.
 */
template <typename scheduler>
concept io_uring_scheduler_has_allocator = requires { typename scheduler::allocator_type; };

template <typename scheduler>
using io_uring_scheduler_allocator_t =
	::std::conditional_t<io_uring_scheduler_has_allocator<scheduler>, typename scheduler::allocator_type,
					   ::fast_io::native_global_allocator>;

/* allocate + construct a T state object through the scheduler's
 * allocator. Allocation uses the _try entry point so failure is a
 * herbception, not fast_terminate. A scheduler whose allocator_type has
 * status supplies the allocator handle as an alloc_handle member; the
 * object copies it into itself so io_uring_delete_state can free without
 * the scheduler. */
template <typename T, typename scheduler, typename... Args>
inline T *io_uring_new_state(scheduler sched, Args &&...args) throws
{
	using typed_alloc =
		::fast_io::typed_generic_allocator_adapter<io_uring_scheduler_allocator_t<scheduler>, T>;
	T *p;
	if constexpr (typed_alloc::has_status)
	{
		p = typed_alloc::handle_allocate_try(sched.alloc_handle, 1);
	}
	else if constexpr (typed_alloc::has_allocate_try)
	{
		p = typed_alloc::allocate_try(1);
	}
	else
	{
		p = typed_alloc::allocate(1);
	}
	try
	{
		new (p) T(::std::forward<Args>(args)...);
	}
	catch throws(::std::error e)
	{
		if constexpr (typed_alloc::has_status)
		{
			typed_alloc::handle_deallocate_n(sched.alloc_handle, p, 1);
		}
		else
		{
			typed_alloc::deallocate(p);
		}
		throw throws;
	}
	if constexpr (typed_alloc::has_status)
	{
		p->alloc_handle = sched.alloc_handle;
	}
	return p;
}

/* every self-owning state object exposes its allocator as
 * allocator_type; status allocators additionally carry the handle in
 * alloc_handle */
template <typename T>
inline void io_uring_delete_state(T *p) noexcept
{
	using typed_alloc = ::fast_io::typed_generic_allocator_adapter<typename T::allocator_type, T>;
	if constexpr (typed_alloc::has_status)
	{
		auto handle{p->alloc_handle};
		p->~T();
		typed_alloc::handle_deallocate_n(handle, p, 1);
	}
	else
	{
		p->~T();
		typed_alloc::deallocate(p);
	}
}

/* RAII guard for one pending submission: while alive, destruction
 * neuters the sqe (if one was taken) and frees the cookie. release()
 * hands ownership to the kernel once the sqe is submitted. */
template <typename T>
class io_uring_submit_guard
{
public:
	T *cookie;
	io_uring_sqe *sqe{};
	inline explicit io_uring_submit_guard(T *c) noexcept
		: cookie{c}
	{
	}
	io_uring_submit_guard(io_uring_submit_guard const &) = delete;
	io_uring_submit_guard &operator=(io_uring_submit_guard const &) = delete;
	inline ~io_uring_submit_guard()
	{
		if (sqe != nullptr)
		{
			/* the slot was already consumed; if it ever reaches the
			 * kernel it must not carry a dangling user_data */
			io_uring_prep_nop(sqe);
			io_uring_sqe_set_data(sqe, nullptr);
		}
		if (cookie != nullptr)
		{
			io_uring_delete_state(cookie);
		}
	}
	inline constexpr void release() noexcept
	{
		sqe = nullptr;
		cookie = nullptr;
	}
};

inline void io_uring_dispatch_cqe(linux_io_uring_observer ring, io_uring_cqe *cqe) noexcept
{
	if (cqe == nullptr)
	{
		return;
	}
	void *data{io_uring_cqe_get_data(cqe)};
	::std::int_least32_t res{cqe->res};
	io_uring_cqe_seen(*ring.ring, cqe);
	if (data == nullptr) [[unlikely]]
	{
		return;
	}
	auto invoke{*static_cast<io_uring_invoke_func *>(data)};
	if (res < 0)
	{
		invoke(data, 0, -res);
	}
	else
	{
		invoke(data, static_cast<::std::size_t>(res), 0);
	}
}
} // namespace details

/*
 * Reap one completion and dispatch it to its cookie, blocking
 * until one is available.
 */
inline void io_async_wait(linux_io_uring_observer ring) throws
{
	io_uring_cqe *cqe{io_uring_wait_cqe(*ring.ring)};
	details::io_uring_dispatch_cqe(ring, cqe);
}

/* Non-blocking variant: dispatch one completion if one is ready */
inline bool io_async_peek(linux_io_uring_observer ring) throws
{
	io_uring_cqe *cqe{io_uring_peek_cqe(*ring.ring)};
	if (cqe == nullptr)
	{
		return false;
	}
	details::io_uring_dispatch_cqe(ring, cqe);
	return true;
}

/* Timed variant; returns false when the deadline elapsed with no CQE */
inline bool io_async_wait_timeout(linux_io_uring_observer ring, ::fast_io::posix_statx_timestamp64 timestamp)
	throws
{
	io_uring_cqe *cqe{io_uring_wait_cqe_timeout(*ring.ring, timestamp)};
	if (cqe == nullptr)
	{
		return false;
	}
	details::io_uring_dispatch_cqe(ring, cqe);
	return true;
}

/* ======================= fd-based async operations ======================= */


namespace details
{
/*
 * sqe->off == ~0 tells the kernel to use (and advance) the file's current
 * position, i.e. read(2)/write(2) rather than pread(2)/pwrite(2).
 */
inline constexpr ::std::uint_least64_t io_uring_use_file_position{static_cast<::std::uint_least64_t>(-1)};

/*
 * Cookie for one pending write_some: a heap block handed to the kernel
 * through sqe->user_data and freed after the callback runs. Plain C
 * layout — invoke must stay the first member because
 * io_uring_dispatch_cqe dereferences user_data as a pointer to it.
 * The callback is invoked once as callback(::std::cxx_std_error, ptr)
 * noexcept where ptr is one past the last byte transferred; a null
 * domain is success.
 */
template <typename alloc_type, typename T>
struct io_uring_write_some_bytes_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	using handle_or_empty =
		::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
							 ::fast_io::details::empty>;
	io_uring_invoke_func invoke;
	/* status allocator handle copied from the scheduler at submission so
	 * the cookie can be freed without it */
	[[no_unique_address]] handle_or_empty alloc_handle{};
	::std::byte const *first{};
	T callback;
};

template <typename alloc_type, typename T>
inline void io_uring_write_some_bytes_invoke(void *self, ::std::size_t transferred, int errn) noexcept
{
	auto *cookie{static_cast<io_uring_write_some_bytes_cookie<alloc_type, T> *>(self)};
	::std::cxx_std_error err{};
	if (errn != 0)
	{
		err.domain = ::std::error_domain<::std::errc>::domain();
		err.code = static_cast<::std::size_t>(errn);
	}
	cookie->callback(err, cookie->first + transferred);
	io_uring_delete_state(cookie);
}
} // namespace details

/*
 * Async write_some_bytes. The operation is committed to the scheduler
 * before returning, matching cross-platform async APIs where no submit
 * step exists. callback is a functor invoked once as
 * callback(::std::cxx_std_error, writtenptr) noexcept on completion:
 * err.domain == nullptr means success, otherwise domain is the posix
 * domain and code the errno value; writtenptr is first + bytes written.
 * The functor is moved into the state object, which frees itself
 * after the callback runs, so the functor need not outlive the submission.
 */
template <::std::integral char_type, typename func>
	requires ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::std::byte const *>
inline void
async_write_some_bytes_callback_define(linux_io_uring_observer ring,
									   ::fast_io::basic_posix_family_io_observer<::fast_io::posix_family::api, char_type> piob,
									   ::std::byte const *first, ::std::byte const *last,
									   func &&callback) throws
{
	using alloc_type = details::io_uring_scheduler_allocator_t<linux_io_uring_observer>;
	using cookie_type = details::io_uring_write_some_bytes_cookie<alloc_type, ::std::remove_cvref_t<func>>;
	details::io_uring_submit_guard<cookie_type> guard{
		details::io_uring_new_state<cookie_type>(
			ring, &details::io_uring_write_some_bytes_invoke<alloc_type, ::std::remove_cvref_t<func>>,
			typename cookie_type::handle_or_empty{}, first, ::std::forward<func>(callback))};
	guard.sqe = details::ensure_io_uring_sqe(*ring.ring);
	io_uring_prep_write(guard.sqe, piob.fd, first, static_cast<::std::uint_least32_t>(last - first),
						details::io_uring_use_file_position);
	io_uring_sqe_set_data(guard.sqe, guard.cookie);
	io_uring_submit(*ring.ring);
	guard.release();
}

namespace details
{
/* Shared state for the async_write_all_bytes chain: one allocation lives
 * across every partial-write resubmission and is freed when the user's
 * callback is finally invoked. */
template <::std::integral char_type, typename alloc_type, typename T>
struct io_uring_write_all_bytes_state
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	linux_io_uring_observer ring;
	::fast_io::basic_posix_family_io_observer<::fast_io::posix_family::api, char_type> piob;
	::std::byte const *last;
	T callback;
	/* status allocator handle copied from the scheduler at submission so
	 * the state can be freed without it; kept last to preserve the
	 * aggregate initialization order */
	[[no_unique_address]] ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											 ::fast_io::details::empty>
		alloc_handle{};
};

template <::std::integral char_type, typename alloc_type, typename T>
inline void io_uring_write_all_bytes_submit(io_uring_write_all_bytes_state<char_type, alloc_type, T> *state,
											::std::byte const *first) noexcept
{
	try
	{
		async_write_some_bytes_callback_define(
			state->ring, state->piob, first, state->last,
			[state](::std::cxx_std_error err, ::std::byte const *written) noexcept {
				if (err.domain != nullptr || written == state->last)
				{
					state->callback(err);
					io_uring_delete_state(state);
					return;
				}
				io_uring_write_all_bytes_submit(state, written);
			});
	}
	catch throws(::std::error e)
	{
		state->callback(e.release());
		io_uring_delete_state(state);
	}
}
} // namespace details

/*
 * write_all built on async_write_some_bytes_callback_define: partial
 * writes are resubmitted until last is reached. The functor is invoked
 * once as callback(::std::cxx_std_error) noexcept: domain == nullptr
 * means every byte was written, otherwise domain is the posix domain and
 * code the errno value (a submission failure is delivered the same way,
 * released from the caught std::error).
 */
template <::std::integral char_type, typename func>
	requires ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error>
inline void
async_write_all_bytes_decay_callback(linux_io_uring_observer ring,
							::fast_io::basic_posix_family_io_observer<::fast_io::posix_family::api, char_type> piob,
							::std::byte const *first, ::std::byte const *last,
							func &&callback) throws
{
	using state_type =
		details::io_uring_write_all_bytes_state<char_type,
												details::io_uring_scheduler_allocator_t<linux_io_uring_observer>,
												::std::remove_cvref_t<func>>;
	auto *state{details::io_uring_new_state<state_type>(ring, ring, piob, last,
														   ::std::forward<func>(callback))};
	details::io_uring_write_all_bytes_submit(state, first);
}

namespace details
{
template <::std::integral char_type>
struct io_uring_write_all_bytes_awaiter
{
	linux_io_uring_observer ring;
	::fast_io::basic_posix_family_io_observer<::fast_io::posix_family::api, char_type> piob;
	::std::byte const *first;
	::std::byte const *last;
	::std::cxx_std_error result{};
	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		try
		{
			async_write_all_bytes_decay_callback(ring, piob, first, last,
												 [this, h](::std::cxx_std_error e) noexcept {
													 result = e;
													 h.resume();
												 });
		}
		catch throws(::std::error e)
		{
			/* submission failed before suspending: report the error
			 * through await_resume instead of resuming later */
			result = e.release();
			return false;
		}
		return true;
	}
	inline constexpr void await_resume() throws
	{
		if (result.domain)
		{
			throw throws result;
		}
	}
};
} // namespace details

/*
 * Coroutine form of async_write_all_bytes_decay_callback: co_await it to
 * submit the whole write and suspend until completion. await_resume()
 * yields the cxx_std_error: domain == nullptr means every byte was
 * written, otherwise domain is the posix domain and code the errno value.
 * A submission failure surfaces the same way without ever suspending.
 */
template <::std::integral char_type>
inline details::io_uring_write_all_bytes_awaiter<char_type>
async_write_all_bytes_decay(linux_io_uring_observer ring,
							::fast_io::basic_posix_family_io_observer<::fast_io::posix_family::api, char_type> piob,
							::std::byte const *first, ::std::byte const *last) noexcept
{
	return {ring, piob, first, last};
}

} // namespace fast_io::liburing

#include <fast_io_dsal/impl/misc/pop_macros.h>

#endif
