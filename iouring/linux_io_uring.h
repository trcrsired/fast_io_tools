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

namespace fast_io::liburing
{

inline io_uring_sqe *io_uring_get_sqe(::fast_io::linux_io_uring_observer ring) noexcept;

} // namespace fast_io::liburing

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

namespace details
{

inline io_uring_sqe *ensure_io_uring_sqe(linux_io_uring_observer ring) throws
{
	io_uring_sqe *sqe{io_uring_get_sqe(ring)};
	while (sqe == nullptr)
	{
		/*
		 * SQ is full. Without SQPOLL, submitting publishes the pending
		 * SQEs and the kernel advances the head as it consumes them.
		 * With SQPOLL the kernel thread drains the SQ by itself; wait
		 * for space instead.
		 */
		if (ring.ring->flags & io_uring_setup_sqpoll)
		{
			int ret{io_uring_enter_impl(*ring.ring, 0, 0, io_uring_enter_sq_wait)};
			::fast_io::system_call_throw_error(ret);
		}
		else
		{
			::std::uint_least32_t submitted{io_uring_flush_sq(*ring.ring)};
			int ret{io_uring_enter_impl(*ring.ring, submitted, 0, 0)};
			::fast_io::system_call_throw_error(ret);
		}
		sqe = io_uring_get_sqe(ring);
	}
	return sqe;
}

} // namespace details

/* ======================= submission queue ======================= */

/*
 * Return an sqe to fill, or nullptr if the SQ is full. Submission only
 * happens when the tail is published by io_uring_submit() (or a wait
 * function that submits on the caller's behalf).
 */
inline io_uring_sqe *io_uring_get_sqe(linux_io_uring_observer ring) noexcept
{
	auto &sq{ring.ring->sq};
	::std::uint_least32_t head{details::io_uring_load_sq_head(*ring.ring)};
	::std::uint_least32_t tail{sq.sqe_tail};
	if (tail - head >= sq.ring_entries)
	{
		return nullptr;
	}
	io_uring_sqe *sqe{sq.sqes + ((tail & sq.ring_mask) << details::io_uring_sqe_shift(*ring.ring))};
	sq.sqe_tail = tail + 1;
	details::io_uring_initialize_sqe(sqe);
	return sqe;
}

/* Returns how many unsubmitted entries are pending in the SQ */
inline ::std::uint_least32_t io_uring_sq_ready(linux_io_uring_observer ring) noexcept
{
	return ring.ring->sq.sqe_tail - details::io_uring_load_sq_head(*ring.ring);
}

/* Returns how much space is left in the SQ ring */
inline ::std::uint_least32_t io_uring_sq_space_left(linux_io_uring_observer ring) noexcept
{
	return ring.ring->sq.ring_entries - io_uring_sq_ready(ring);
}

/*
 * Publish pending SQEs to the kernel. Returns the number submitted.
 * With io_uring_setup_sqpoll this only wakes the SQ thread when needed.
 */
inline ::std::uint_least32_t io_uring_submit(linux_io_uring_observer ring) throws
{
	::std::uint_least32_t submitted{details::io_uring_flush_sq(*ring.ring)};
	bool cq_enter{details::io_uring_cq_ring_needs_enter(*ring.ring)};
	::std::uint_least32_t flags{};
	if (details::io_uring_sq_ring_needs_enter(*ring.ring, submitted, flags) || cq_enter)
	{
		if (cq_enter)
		{
			flags |= io_uring_enter_getevents;
		}
		int ret{details::io_uring_enter_impl(*ring.ring, submitted, 0, flags)};
		::fast_io::system_call_throw_error(ret);
		return static_cast<::std::uint_least32_t>(ret);
	}
	return submitted;
}

/* Submit pending SQEs and wait for at least wait_nr CQEs */
inline ::std::uint_least32_t io_uring_submit_and_wait(linux_io_uring_observer ring, ::std::uint_least32_t wait_nr)
	throws
{
	::std::uint_least32_t submitted{details::io_uring_flush_sq(*ring.ring)};
	bool cq_enter{wait_nr != 0 || details::io_uring_cq_ring_needs_enter(*ring.ring)};
	::std::uint_least32_t flags{};
	if (details::io_uring_sq_ring_needs_enter(*ring.ring, submitted, flags) || cq_enter)
	{
		if (cq_enter)
		{
			flags |= io_uring_enter_getevents;
		}
		int ret{details::io_uring_enter_impl(*ring.ring, submitted, wait_nr, flags)};
		::fast_io::system_call_throw_error(ret);
		return static_cast<::std::uint_least32_t>(ret);
	}
	return submitted;
}

/* Flush pending CQEs the kernel may be holding (IOPOLL/overflow/taskrun) */
inline void io_uring_get_events(linux_io_uring_observer ring) throws
{
	int ret{details::io_uring_enter_impl(*ring.ring, 0, 0, io_uring_enter_getevents)};
	::fast_io::system_call_throw_error(ret);
}

/*
 * SQPOLL only: wait for space to free up in the SQ ring. No-op for
 * non-SQPOLL rings or when space is already available.
 */
inline void io_uring_sqring_wait(linux_io_uring_observer ring) throws
{
	if (!(ring.ring->flags & io_uring_setup_sqpoll) || io_uring_sq_space_left(ring) != 0)
	{
		return;
	}
	int ret{details::io_uring_enter_impl(*ring.ring, 0, 0, io_uring_enter_sq_wait)};
	::fast_io::system_call_throw_error(ret);
}

/* ======================= completion queue ======================= */

/* Returns how many unconsumed entries are ready in the CQ ring */
inline ::std::uint_least32_t io_uring_cq_ready(linux_io_uring_observer ring) noexcept
{
	return details::io_uring_load_acquire(ring.ring->cq.ktail) - *ring.ring->cq.khead;
}

/*
 * Must be called after the application has consumed nr CQ slots, so the
 * kernel can reuse them.
 */
inline void io_uring_cq_advance(linux_io_uring_observer ring, ::std::uint_least32_t nr) noexcept
{
	if (nr != 0)
	{
		details::io_uring_store_release(ring.ring->cq.khead, *ring.ring->cq.khead + nr);
	}
}

/* Mark one CQE as consumed */
inline void io_uring_cqe_seen(linux_io_uring_observer ring, io_uring_cqe const *cqe) noexcept
{
	if (cqe != nullptr)
	{
		io_uring_cq_advance(ring, io_uring_cqe_nr(cqe));
	}
}

/*
 * Peek at the next completion without entering the kernel unless CQEs
 * might be pending a flush (IOPOLL / overflow / taskrun). Returns nullptr
 * when no completion is currently available.
 */
inline io_uring_cqe *io_uring_peek_cqe(linux_io_uring_observer ring) throws
{
	io_uring_cqe *cqe{details::io_uring_peek_cqe_impl(*ring.ring).cqe_ptr};
	if (cqe != nullptr)
	{
		return cqe;
	}
	if (!(ring.ring->flags & io_uring_setup_iopoll) &&
		!(details::io_uring_load_acquire(ring.ring->sq.kflags) & (io_uring_sq_cq_overflow | io_uring_sq_taskrun)))
	{
		return nullptr;
	}
	/* slow path: one kernel round trip with wait_nr = 0; when even that
	 * comes back empty the result is EAGAIN, delivered like any error */
	details::io_uring_get_data data{.submit = 0, .wait_nr = 0, .get_flags = 0, .sz = 0, .has_ts = false, .arg = nullptr};
	return details::io_uring_get_cqe_impl(*ring.ring, data);
}

/* Wait for (at least) wait_nr completions; returns the first CQE */
inline io_uring_cqe *io_uring_wait_cqes(linux_io_uring_observer ring, ::std::uint_least32_t wait_nr)
	throws
{
	details::io_uring_get_data data{.submit = 0, .wait_nr = wait_nr, .get_flags = 0, .sz = 0, .has_ts = false,
									.arg = nullptr};
	return details::io_uring_get_cqe_impl(*ring.ring, data);
}

inline io_uring_cqe *io_uring_wait_cqe(linux_io_uring_observer ring) throws
{
	return io_uring_wait_cqes(ring, 1);
}

/* Submit pending SQEs, then wait for wait_nr completions */
inline io_uring_cqe *io_uring_submit_and_wait_cqes(linux_io_uring_observer ring,
													 ::std::uint_least32_t wait_nr) throws
{
	details::io_uring_get_data data{.submit = details::io_uring_flush_sq(*ring.ring),
									.wait_nr = wait_nr,
									.get_flags = 0,
									.sz = 0,
									.has_ts = false,
									.arg = nullptr};
	return details::io_uring_get_cqe_impl(*ring.ring, data);
}

/*
 * Wait for a completion with a timeout. On kernels with
 * io_uring_feat_ext_arg the deadline rides in io_uring_enter(2) directly;
 * on older kernels an internal io_uring_op_timeout SQE is queued instead.
 * Returns false on timeout (with *cqe_ptr set to nullptr).
 */
inline bool io_uring_wait_cqe_timeout(linux_io_uring_observer ring,
									  ::fast_io::posix_statx_timestamp64 timestamp,
									  io_uring_cqe **cqe_ptr) throws
{
	/* the kernel reads the full 64-bit nsec field out of the address we
	 * hand it; convert so the statx layout's 4-byte tail padding can
	 * never leak garbage into it */
	io_uring_timespec ts{static_cast<::std::int64_t>(timestamp.tv_sec),
						 static_cast<::std::int64_t>(timestamp.tv_nsec)};
	io_uring_cqe *cqe{};
	if (ring.ring->features & io_uring_feat_ext_arg)
	{
		io_uring_getevents_arg arg{0, 0, 0, reinterpret_cast<::std::uint_least64_t>(__builtin_addressof(ts))};
		details::io_uring_get_data data{.submit = 0,
										.wait_nr = 1,
										.get_flags = io_uring_enter_ext_arg,
										.sz = sizeof(arg),
										.has_ts = true,
										.arg = __builtin_addressof(arg)};
		cqe = details::io_uring_get_cqe_impl(*ring.ring, data);
	}
	else
	{
		/* queue an internal timeout SQE that the kernel completes when
		 * either the deadline expires or a CQE is posted */
		io_uring_sqe *sqe{details::ensure_io_uring_sqe(ring)};
		sqe->opcode = io_uring_op_timeout;
		sqe->fd = -1;
		sqe->addr = reinterpret_cast<::std::uint_least64_t>(__builtin_addressof(ts));
		sqe->len = 1;
		sqe->off = 1;
		sqe->timeout_flags = 0;
		sqe->user_data = io_uring_internal_timeout_user_data;
		details::io_uring_get_data data{.submit = details::io_uring_flush_sq(*ring.ring),
										.wait_nr = 1,
										.get_flags = 0,
										.sz = 0,
										.has_ts = false,
										.arg = nullptr};
		try
		{
			cqe = details::io_uring_get_cqe_impl(*ring.ring, data);
		}
		catch throws(::std::error e)
		{
			/* the internal timeout sqe completed: deadline expired */
			if (e != static_cast<::std::errc>(ETIME))
			{
				throw throws;
			}
		}
	}
	*cqe_ptr = cqe;
	return cqe != nullptr;
}

/*
 * Fill an array of CQE pointers for the currently available completions.
 * Returns the number filled; nothing is consumed — call
 * io_uring_cq_advance afterwards with the total slot count
 * (sum of io_uring_cqe_nr over the returned CQEs).
 */
inline ::std::uint_least32_t io_uring_peek_batch_cqe(linux_io_uring_observer ring, io_uring_cqe **cqes,
											   ::std::uint_least32_t count) throws
{
	::std::uint_least32_t ready{io_uring_cq_ready(ring)};
	if (ready == 0)
	{
		if (!details::io_uring_cq_ring_needs_flush(*ring.ring))
		{
			return 0;
		}
		io_uring_get_events(ring);
		ready = io_uring_cq_ready(ring);
		if (ready == 0)
		{
			return 0;
		}
	}
	::std::uint_least32_t head{*ring.ring->cq.khead};
	::std::uint_least32_t mask{ring.ring->cq.ring_mask};
	::std::uint_least32_t shift{details::io_uring_cqe_shift(*ring.ring)};
	::std::uint_least32_t nr{};
	::std::uint_least32_t last{head + ready};
	while (head != last && nr < count)
	{
		io_uring_cqe *cqe{ring.ring->cq.cqes + ((head & mask) << shift)};
		if (cqe->flags & io_uring_cqe_f_skip)
		{
			/* a skip entry can only be consumed at the CQ head */
			if (nr != 0)
			{
				break;
			}
			io_uring_cq_advance(ring, 1);
			++head;
			continue;
		}
		head += io_uring_cqe_nr(cqe);
		cqes[nr++] = cqe;
	}
	return nr;
}

/* ======================= io_uring_register(2) ======================= */

inline int io_uring_register(linux_io_uring_observer ring, ::std::uint_least32_t opcode, void const *arg,
							 ::std::uint_least32_t nr_args) throws
{
	int ret{details::io_uring_register_impl(static_cast<::std::uint_least32_t>(ring.ring->ring_fd), opcode, arg,
											nr_args)};
	::fast_io::system_call_throw_error(ret);
	return ret;
}

inline int io_uring_register_buffers(linux_io_uring_observer ring, io_scatter_t const *iovecs,
									 ::std::uint_least32_t nr) throws
{
	return io_uring_register(ring, io_uring_regop_buffers, iovecs, nr);
}

inline int io_uring_unregister_buffers(linux_io_uring_observer ring) throws
{
	return io_uring_register(ring, io_uring_regop_unregister_buffers, nullptr, 0);
}

inline int io_uring_register_files(linux_io_uring_observer ring, int const *files,
								   ::std::uint_least32_t nr) throws
{
	return io_uring_register(ring, io_uring_regop_files, files, nr);
}

inline int io_uring_unregister_files(linux_io_uring_observer ring) throws
{
	return io_uring_register(ring, io_uring_regop_unregister_files, nullptr, 0);
}

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
	io_uring_cqe_seen(ring, cqe);
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
	io_uring_cqe *cqe{io_uring_wait_cqe(ring)};
	details::io_uring_dispatch_cqe(ring, cqe);
}

/* Non-blocking variant: dispatch one completion if one is ready */
inline bool io_async_peek(linux_io_uring_observer ring) throws
{
	io_uring_cqe *cqe{io_uring_peek_cqe(ring)};
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
	io_uring_cqe *cqe{};
	if (!io_uring_wait_cqe_timeout(ring, timestamp, __builtin_addressof(cqe)))
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
	guard.sqe = details::ensure_io_uring_sqe(ring);
	io_uring_prep_write(guard.sqe, piob.fd, first, static_cast<::std::uint_least32_t>(last - first),
						details::io_uring_use_file_position);
	io_uring_sqe_set_data(guard.sqe, guard.cookie);
	io_uring_submit(ring);
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
