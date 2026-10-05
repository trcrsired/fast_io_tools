#pragma once
/*
 * io_uring support implemented entirely on top of fast_io's own Linux
 * system_call<> wrappers. No liburing, no libc io_uring helpers: the
 * submission/completion rings are mapped and driven directly, the same
 * way the kernel ABI (linux/io_uring.h) documents it.
 */

#if defined(__linux__)

#if !defined(__HERBCEPTIONS__)
#error "linux_io_uring.h requires a compiler with herbceptions support (-fherbceptions)"
#endif

#if __has_include(<chrono>)
#include <chrono>
#define FAST_IO_LINUX_IO_URING_HAS_CHRONO
#endif
#include <coroutine>

#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io
{

/*
 * Linux io_uring syscall numbers. <asm/unistd.h> provides __NR_io_uring_*
 * on reasonably recent headers; fall back to the assigned numbers when it
 * does not (alpha and mips are the exceptions to the common numbering).
 */
namespace details
{
inline constexpr ::std::uint_least64_t io_uring_setup_syscall_number{
#if defined(__NR_io_uring_setup)
	__NR_io_uring_setup
#elif defined(__alpha__)
	535
#elif defined(__mips__) && defined(__NR_Linux)
	__NR_Linux + 425
#else
	425
#endif
};
inline constexpr ::std::uint_least64_t io_uring_enter_syscall_number{
#if defined(__NR_io_uring_enter)
	__NR_io_uring_enter
#elif defined(__alpha__)
	536
#elif defined(__mips__) && defined(__NR_Linux)
	__NR_Linux + 426
#else
	426
#endif
};
inline constexpr ::std::uint_least64_t io_uring_register_syscall_number{
#if defined(__NR_io_uring_register)
	__NR_io_uring_register
#elif defined(__alpha__)
	537
#elif defined(__mips__) && defined(__NR_Linux)
	__NR_Linux + 427
#else
	427
#endif
};
} // namespace details

/*
 * Kernel UAPI (stable ABI, mirrored from linux/io_uring.h so neither
 * liburing nor kernel headers are required).
 */
struct io_uring_sqe
{
	::std::uint8_t opcode;  /* type of operation for this sqe */
	::std::uint8_t flags;   /* io_uring_sqe_flag */
	::std::uint16_t ioprio; /* ioprio for the request */
	::std::int32_t fd;      /* file descriptor to do IO on */
	union
	{
		::std::uint64_t off;   /* offset into file */
		::std::uint64_t addr2; /* literal pointer / second address */
		struct
		{
			::std::uint32_t cmd_op;
			::std::uint32_t pad1;
		} uring_cmd;
	};
	union
	{
		::std::uint64_t addr; /* pointer to buffer or iovecs */
		::std::uint64_t splice_off_in;
		struct
		{
			::std::uint32_t level;
			::std::uint32_t optname;
		} sockopt;
	};
	::std::uint32_t len; /* buffer size or number of iovecs */
	union
	{
		::std::uint32_t rw_flags;
		::std::uint32_t fsync_flags;
		::std::uint16_t poll_events;
		::std::uint32_t poll32_events;
		::std::uint32_t sync_range_flags;
		::std::uint32_t msg_flags;
		::std::uint32_t timeout_flags;
		::std::uint32_t accept_flags;
		::std::uint32_t cancel_flags;
		::std::uint32_t open_flags;
		::std::uint32_t statx_flags;
		::std::uint32_t fadvise_advice;
		::std::uint32_t splice_flags;
		::std::uint32_t rename_flags;
		::std::uint32_t unlink_flags;
		::std::uint32_t hardlink_flags;
		::std::uint32_t xattr_flags;
		::std::uint32_t msg_ring_flags;
		::std::uint32_t uring_cmd_flags;
		::std::uint32_t waitid_flags;
		::std::uint32_t futex_flags;
		::std::uint32_t install_fd_flags;
		::std::uint32_t nop_flags;
		::std::uint32_t pipe_flags;
	};
	::std::uint64_t user_data; /* data to be passed back at completion time */
	union
	{
		::std::uint16_t buf_index; /* index into fixed buffers, if used */
		::std::uint16_t buf_group; /* for grouped buffer selection */
	};
	::std::uint16_t personality; /* personality to use, if used */
	union
	{
		::std::int32_t splice_fd_in;
		::std::uint32_t file_index;
		::std::uint32_t optlen;
		struct
		{
			::std::uint16_t addr_len;
			::std::uint16_t pad3;
		} addrlen;
	};
	union
	{
		struct
		{
			::std::uint64_t addr3;
			::std::uint64_t pad2;
		} addr3_and_pad;
		struct
		{
			::std::uint64_t attr_ptr;       /* pointer to attribute information */
			::std::uint64_t attr_type_mask; /* bit mask of attributes */
		} attr;
		::std::uint64_t optval;
		/*
		 * If the ring is initialized with io_uring_setup_sqe128, then
		 * this field is used for arbitrary command data
		 */
		::std::uint8_t cmd[16];
	};
};

static_assert(sizeof(io_uring_sqe) == 64, "io_uring_sqe must match the kernel ABI");

struct io_uring_cqe
{
	::std::uint64_t user_data; /* sqe->user_data value passed back */
	::std::int32_t res;        /* result code for this event */
	::std::uint32_t flags;     /* io_uring_cqe_flag */
};

static_assert(sizeof(io_uring_cqe) == 16, "io_uring_cqe must match the kernel ABI");

/* Filled with the offsets for mmap(2) by io_uring_setup(2) */
struct io_sqring_offsets
{
	::std::uint32_t head;
	::std::uint32_t tail;
	::std::uint32_t ring_mask;
	::std::uint32_t ring_entries;
	::std::uint32_t flags;
	::std::uint32_t dropped;
	::std::uint32_t array;
	::std::uint32_t resv1;
	::std::uint64_t user_addr;
};

struct io_cqring_offsets
{
	::std::uint32_t head;
	::std::uint32_t tail;
	::std::uint32_t ring_mask;
	::std::uint32_t ring_entries;
	::std::uint32_t overflow;
	::std::uint32_t cqes;
	::std::uint32_t flags;
	::std::uint32_t resv1;
	::std::uint64_t user_addr;
};

/* Passed in for io_uring_setup(2). Copied back with updated info on success */
struct io_uring_params
{
	::std::uint32_t sq_entries;
	::std::uint32_t cq_entries;
	::std::uint32_t flags;
	::std::uint32_t sq_thread_cpu;
	::std::uint32_t sq_thread_idle;
	::std::uint32_t features;
	::std::uint32_t wq_fd;
	::std::uint32_t resv[3];
	io_sqring_offsets sq_off;
	io_cqring_offsets cq_off;
};

static_assert(sizeof(io_uring_params) == 120, "io_uring_params must match the kernel ABI");

/* Argument for io_uring_enter(2) with io_uring_enter_ext_arg */
struct io_uring_getevents_arg
{
	::std::uint64_t sigmask;
	::std::uint32_t sigmask_sz;
	::std::uint32_t min_wait_usec;
	::std::uint64_t ts;
};

/* __kernel_timespec equivalent */
struct io_uring_timespec
{
	::std::int64_t tv_sec;
	::std::int64_t tv_nsec;
};

/* io_uring_setup(2) flags */
enum io_uring_setup_flag : ::std::uint32_t
{
	io_uring_setup_iopoll = 1U << 0,              /* io_context is polled */
	io_uring_setup_sqpoll = 1U << 1,              /* SQ poll thread */
	io_uring_setup_sq_aff = 1U << 2,              /* sq_thread_cpu is valid */
	io_uring_setup_cqsize = 1U << 3,              /* app defines CQ size */
	io_uring_setup_clamp = 1U << 4,               /* clamp SQ/CQ ring sizes */
	io_uring_setup_attach_wq = 1U << 5,           /* attach to existing wq */
	io_uring_setup_r_disabled = 1U << 6,          /* start with ring disabled */
	io_uring_setup_submit_all = 1U << 7,          /* continue submit on error */
	io_uring_setup_coop_taskrun = 1U << 8,        /* cooperative task running */
	io_uring_setup_taskrun_flag = 1U << 9,        /* IORING_SQ_TASKRUN notification */
	io_uring_setup_sqe128 = 1U << 10,             /* SQEs are 128 byte */
	io_uring_setup_cqe32 = 1U << 11,              /* CQEs are 32 byte */
	io_uring_setup_single_issuer = 1U << 12,      /* only one task submits */
	io_uring_setup_defer_taskrun = 1U << 13,      /* defer task work to enter */
	io_uring_setup_no_mmap = 1U << 14,            /* no mmap for rings (unsupported) */
	io_uring_setup_registered_fd_only = 1U << 15, /* only registered ring fd */
	io_uring_setup_no_sqarray = 1U << 16,         /* no SQ array indirection */
	io_uring_setup_hybrid_iopoll = 1U << 17,      /* kernel decides to iopoll */
	io_uring_setup_cqe_mixed = 1U << 18,          /* mixed 16/32 byte CQEs */
	io_uring_setup_sqe_mixed = 1U << 19,          /* mixed 64/128 byte SQEs */
	io_uring_setup_sq_rewind = 1U << 20           /* rewind SQ on submit */
};

/* io_uring_enter(2) flags */
enum io_uring_enter_flag : ::std::uint32_t
{
	io_uring_enter_getevents = 1U << 0,
	io_uring_enter_sq_wakeup = 1U << 1,
	io_uring_enter_sq_wait = 1U << 2,
	io_uring_enter_ext_arg = 1U << 3,
	io_uring_enter_registered_ring = 1U << 4,
	io_uring_enter_abs_timer = 1U << 5,
	io_uring_enter_ext_arg_reg = 1U << 6,
	io_uring_enter_no_iowait = 1U << 7
};

/* io_uring_params.features flags */
enum io_uring_feat : ::std::uint32_t
{
	io_uring_feat_single_mmap = 1U << 0,
	io_uring_feat_nodrop = 1U << 1,
	io_uring_feat_submit_stable = 1U << 2,
	io_uring_feat_rw_cur_pos = 1U << 3,
	io_uring_feat_cur_personality = 1U << 4,
	io_uring_feat_fast_poll = 1U << 5,
	io_uring_feat_poll_32bits = 1U << 6,
	io_uring_feat_sqpoll_nonfixed = 1U << 7,
	io_uring_feat_ext_arg = 1U << 8,
	io_uring_feat_native_workers = 1U << 9,
	io_uring_feat_rsrc_tags = 1U << 10,
	io_uring_feat_cqe_skip = 1U << 11,
	io_uring_feat_linked_file = 1U << 12,
	io_uring_feat_reg_reg_ring = 1U << 13,
	io_uring_feat_recvsend_bundle = 1U << 14,
	io_uring_feat_min_timeout = 1U << 15,
	io_uring_feat_rw_attr = 1U << 16,
	io_uring_feat_no_iowait = 1U << 17
};

/* sq_ring->flags (kernel writes, application reads) */
enum io_uring_sq_flag : ::std::uint32_t
{
	io_uring_sq_need_wakeup = 1U << 0, /* needs io_uring_enter wakeup */
	io_uring_sq_cq_overflow = 1U << 1, /* CQ ring is overflown */
	io_uring_sq_taskrun = 1U << 2      /* task should enter the kernel */
};

/* cq_ring->flags */
enum io_uring_cq_flag : ::std::uint32_t
{
	io_uring_cq_eventfd_disabled = 1U << 0
};

/* cqe->flags */
enum io_uring_cqe_flag : ::std::uint32_t
{
	io_uring_cqe_f_buffer = 1U << 0, /* upper 16 bits are the buffer ID */
	io_uring_cqe_f_more = 1U << 1,   /* parent SQE will generate more CQEs */
	io_uring_cqe_f_sock_nonempty = 1U << 2,
	io_uring_cqe_f_notif = 1U << 3,
	io_uring_cqe_f_buf_more = 1U << 4,
	io_uring_cqe_f_skip = 1U << 5, /* padding CQE in a mixed ring, ignore */
	io_uring_cqe_f_32 = 1U << 15   /* 32-byte CQE in a mixed ring */
};

inline constexpr ::std::uint32_t io_uring_cqe_buffer_shift{16};

/* Magic offsets for the application to mmap the data it needs */
enum io_uring_mmap_offset : ::std::uint64_t
{
	io_uring_off_sq_ring = 0ULL,
	io_uring_off_cq_ring = 0x8000000ULL,
	io_uring_off_sqes = 0x10000000ULL,
	io_uring_off_pbuf_ring = 0x80000000ULL
};

/* sqe->opcode */
enum io_uring_op : ::std::uint8_t
{
	io_uring_op_nop,
	io_uring_op_readv,
	io_uring_op_writev,
	io_uring_op_fsync,
	io_uring_op_read_fixed,
	io_uring_op_write_fixed,
	io_uring_op_poll_add,
	io_uring_op_poll_remove,
	io_uring_op_sync_file_range,
	io_uring_op_sendmsg,
	io_uring_op_recvmsg,
	io_uring_op_timeout,
	io_uring_op_timeout_remove,
	io_uring_op_accept,
	io_uring_op_async_cancel,
	io_uring_op_link_timeout,
	io_uring_op_connect,
	io_uring_op_fallocate,
	io_uring_op_openat,
	io_uring_op_close,
	io_uring_op_files_update,
	io_uring_op_statx,
	io_uring_op_read,
	io_uring_op_write,
	io_uring_op_fadvise,
	io_uring_op_madvise,
	io_uring_op_send,
	io_uring_op_recv,
	io_uring_op_openat2,
	io_uring_op_epoll_ctl,
	io_uring_op_splice,
	io_uring_op_provide_buffers,
	io_uring_op_remove_buffers,
	io_uring_op_tee,
	io_uring_op_shutdown,
	io_uring_op_renameat,
	io_uring_op_unlinkat,
	io_uring_op_mkdirat,
	io_uring_op_symlinkat,
	io_uring_op_linkat,
	io_uring_op_msg_ring,
	io_uring_op_fsetxattr,
	io_uring_op_setxattr,
	io_uring_op_fgetxattr,
	io_uring_op_getxattr,
	io_uring_op_socket,
	io_uring_op_uring_cmd,
	io_uring_op_send_zc,
	io_uring_op_sendmsg_zc,
	io_uring_op_read_multishot,
	io_uring_op_waitid,
	io_uring_op_futex_wait,
	io_uring_op_futex_wake,
	io_uring_op_futex_waitv,
	io_uring_op_fixed_fd_install,
	io_uring_op_ftruncate,
	io_uring_op_bind,
	io_uring_op_listen,
	io_uring_op_recv_zc,
	io_uring_op_epoll_wait,
	io_uring_op_readv_fixed,
	io_uring_op_writev_fixed,
	io_uring_op_pipe,
	io_uring_op_nop128,
	io_uring_op_uring_cmd128,
	io_uring_op_last
};

/* sqe->flags */
enum io_uring_sqe_flag : ::std::uint8_t
{
	io_uring_sqe_fixed_file = 1U << 0,      /* use fixed fileset */
	io_uring_sqe_io_drain = 1U << 1,        /* issue after inflight IO */
	io_uring_sqe_io_link = 1U << 2,         /* links next sqe */
	io_uring_sqe_io_hardlink = 1U << 3,     /* like LINK, but stronger */
	io_uring_sqe_async = 1U << 4,           /* always go async */
	io_uring_sqe_buffer_select = 1U << 5,   /* select buffer from sqe->buf_group */
	io_uring_sqe_cqe_skip_success = 1U << 6 /* don't post CQE if success */
};

/* sqe->fsync_flags */
enum io_uring_fsync_flag : ::std::uint32_t
{
	io_uring_fsync_datasync = 1U << 0
};

/* sqe->timeout_flags */
enum io_uring_timeout_flag : ::std::uint32_t
{
	io_uring_timeout_abs = 1U << 0,
	io_uring_timeout_update = 1U << 1,
	io_uring_timeout_boottime = 1U << 2,
	io_uring_timeout_realtime = 1U << 3,
	io_uring_link_timeout_update = 1U << 4,
	io_uring_timeout_etime_success = 1U << 5,
	io_uring_timeout_multishot = 1U << 6,
	io_uring_timeout_clock_mask = io_uring_timeout_boottime | io_uring_timeout_realtime,
	io_uring_timeout_update_mask = io_uring_timeout_update | io_uring_link_timeout_update
};

/* sqe->cancel_flags */
enum io_uring_cancel_flag : ::std::uint32_t
{
	io_uring_async_cancel_all = 1U << 0,
	io_uring_async_cancel_fd = 1U << 1,
	io_uring_async_cancel_any = 1U << 2,
	io_uring_async_cancel_fd_fixed = 1U << 3,
	io_uring_async_cancel_userdata = 1U << 4,
	io_uring_async_cancel_op = 1U << 5
};

/* sqe->len for io_uring_op_poll_add */
enum io_uring_poll_flag : ::std::uint32_t
{
	io_uring_poll_add_multi = 1U << 0 /* multishot poll */
};

/* sqe->ioprio for io_uring_op_accept */
enum io_uring_accept_ioprio : ::std::uint16_t
{
	io_uring_accept_multishot = 1U << 0
};

/* sqe->msg_flags for io_uring_op_send/recv */
enum io_uring_recvsend_flag : ::std::uint32_t
{
	io_uring_recvsend_poll_first = 1U << 0,
	io_uring_recvsend_multishot = 1U << 1,
	io_uring_recvsend_fixed_buf = 1U << 2,
	io_uring_recvsend_bundle = 1U << 4
};

/* io_uring_register(2) opcodes */
enum io_uring_register_op : ::std::uint32_t
{
	io_uring_regop_buffers = 0,
	io_uring_regop_unregister_buffers = 1,
	io_uring_regop_files = 2,
	io_uring_regop_unregister_files = 3,
	io_uring_regop_eventfd = 4,
	io_uring_regop_unregister_eventfd = 5,
	io_uring_regop_files_update = 6,
	io_uring_regop_eventfd_async = 7,
	io_uring_regop_probe = 8,
	io_uring_regop_personality = 9,
	io_uring_regop_unregister_personality = 10,
	io_uring_regop_restrictions = 11,
	io_uring_regop_enable_rings = 12,
	io_uring_regop_files2 = 13,
	io_uring_regop_files_update2 = 14,
	io_uring_regop_buffers2 = 15,
	io_uring_regop_buffers_update = 16,
	io_uring_regop_iowq_aff = 17,
	io_uring_regop_unregister_iowq_aff = 18,
	io_uring_regop_iowq_max_workers = 19,
	io_uring_regop_ring_fds = 20,
	io_uring_regop_unregister_ring_fds = 21,
	io_uring_regop_pbuf_ring = 22,
	io_uring_regop_unregister_pbuf_ring = 23,
	io_uring_regop_sync_cancel = 24,
	io_uring_regop_file_alloc_range = 25,
	io_uring_regop_pbuf_status = 26,
	io_uring_regop_napi = 27,
	io_uring_regop_unregister_napi = 28,
	io_uring_regop_clock = 29,
	io_uring_regop_clone_buffers = 30,
	io_uring_regop_send_msg_ring = 31
};

/* Tell io_uring to allocate a direct (fixed) descriptor for ops that can
 * instantiate one (openat/accept/etc.) */
inline constexpr ::std::uint32_t io_uring_file_index_alloc{~0U};

/* user_data value used internally for wait timeouts on kernels without
 * io_uring_feat_ext_arg; applications must not use it */
inline constexpr ::std::uint64_t io_uring_internal_timeout_user_data{~0ULL};

namespace details
{

inline int io_uring_setup_impl(::std::uint32_t entries, io_uring_params *params) noexcept
{
	return system_call<io_uring_setup_syscall_number, int>(entries, params);
}

inline int io_uring_enter2_impl(::std::uint32_t fd, ::std::uint32_t to_submit, ::std::uint32_t min_complete,
								::std::uint32_t flags, void const *arg, ::std::size_t sz) noexcept
{
	return system_call<io_uring_enter_syscall_number, int>(fd, to_submit, min_complete, flags, arg, sz);
}

inline int io_uring_register_impl(::std::uint32_t fd, ::std::uint32_t opcode, void const *arg,
								  ::std::uint32_t nr_args) noexcept
{
	return system_call<io_uring_register_syscall_number, int>(fd, opcode, arg, nr_args);
}

/* userspace-side submission queue state (mmap'd, shared with the kernel) */
struct io_uring_sq_state
{
	::std::uint32_t *khead{};
	::std::uint32_t *ktail{};
	::std::uint32_t *kflags{};
	::std::uint32_t *kdropped{};
	::std::uint32_t *array{};
	io_uring_sqe *sqes{};
	::std::uint32_t sqe_head{};
	::std::uint32_t sqe_tail{};
	::std::size_t ring_sz{};
	void *ring_ptr{};
	::std::uint32_t ring_mask{};
	::std::uint32_t ring_entries{};
	::std::size_t sqes_sz{};
};

/* userspace-side completion queue state (mmap'd, shared with the kernel) */
struct io_uring_cq_state
{
	::std::uint32_t *khead{};
	::std::uint32_t *ktail{};
	::std::uint32_t *koverflow{};
	io_uring_cqe *cqes{};
	::std::size_t ring_sz{};
	void *ring_ptr{};
	::std::uint32_t ring_mask{};
	::std::uint32_t ring_entries{};
};

enum io_uring_int_flag : ::std::uint32_t
{
	io_uring_int_cq_enter = 1U << 0 /* IOPOLL (without SQPOLL) always needs enter */
};

} // namespace details

struct io_uring_ring_state
{
	::fast_io::details::io_uring_sq_state sq{};
	::fast_io::details::io_uring_cq_state cq{};
	int ring_fd{-1};
	::std::uint32_t flags{};
	::std::uint32_t features{};
	::std::uint32_t int_flags{};
};

class linux_io_uring_observer
{
public:
	using native_handle_type = io_uring_ring_state *;
	/* scheduler-provided allocator: a fast_io generic allocator adapter.
	 * Async defines allocate and free their completion storage through
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

inline io_uring_sqe *io_uring_get_sqe(linux_io_uring_observer ring) noexcept;

namespace details
{

/*
 * Memory ordering required by the ring protocol:
 * - SQ tail must be release-stored after the SQEs are written (SQPOLL).
 * - SQ head must be acquire-loaded before overwriting SQEs (SQPOLL).
 * - CQ tail must be acquire-loaded before reading CQEs.
 * - CQ head must be release-stored after the CQEs have been consumed.
 */
inline ::std::uint32_t io_uring_load_acquire(::std::uint32_t const *p) noexcept
{
	return __atomic_load_n(p, __ATOMIC_ACQUIRE);
}

inline void io_uring_store_release(::std::uint32_t *p, ::std::uint32_t v) noexcept
{
	__atomic_store_n(p, v, __ATOMIC_RELEASE);
}

inline void io_uring_smp_mb() noexcept
{
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}

inline unsigned io_uring_sqe_shift(io_uring_ring_state const &ring) noexcept
{
	return !!(ring.flags & io_uring_setup_sqe128);
}

inline unsigned io_uring_cqe_shift(io_uring_ring_state const &ring) noexcept
{
	return !!(ring.flags & io_uring_setup_cqe32);
}

inline void io_uring_unmap_rings(io_uring_ring_state &ring) noexcept
{
	if (ring.sq.ring_ptr != nullptr && ring.sq.ring_sz != 0)
	{
		sys_munmap_nothrow(ring.sq.ring_ptr, ring.sq.ring_sz);
	}
	if (ring.cq.ring_ptr != nullptr && ring.cq.ring_ptr != ring.sq.ring_ptr && ring.cq.ring_sz != 0)
	{
		sys_munmap_nothrow(ring.cq.ring_ptr, ring.cq.ring_sz);
	}
	ring.sq.ring_ptr = nullptr;
	ring.cq.ring_ptr = nullptr;
}

/*
 * Raw syscall mmap for the rings (PROT_READ|PROT_WRITE,
 * MAP_SHARED|MAP_POPULATE). Returns the raw syscall result so the caller
 * can distinguish the errno; check with linux_system_call_fails.
 */
inline ::std::ptrdiff_t io_uring_mmap_impl(::std::size_t len, int fd, ::std::uint64_t offset) noexcept
{
	constexpr int prot_read_write{3};        /* PROT_READ | PROT_WRITE */
	constexpr int map_shared_populate{0x8001}; /* MAP_SHARED | MAP_POPULATE */
#if defined(__s390__) || defined(__s390x__)
	// s390's __NR_mmap is the old single-argument entry; pass the kernel's
	// mmap argument block.
	struct s390_mmap_arg_struct
	{
		unsigned long addr;
		unsigned long len;
		unsigned long prot;
		unsigned long flags;
		unsigned long fd;
		unsigned long offset;
	};
	s390_mmap_arg_struct args{0, static_cast<unsigned long>(len),
							  static_cast<unsigned long>(prot_read_write),
							  static_cast<unsigned long>(map_shared_populate),
							  static_cast<unsigned long>(fd), static_cast<unsigned long>(offset)};
	return system_call<__NR_mmap, ::std::ptrdiff_t>(__builtin_addressof(args));
#elif defined(__NR_mmap)
	return system_call<__NR_mmap, ::std::ptrdiff_t>(nullptr, len, prot_read_write, map_shared_populate, fd,
												   offset);
#elif defined(__NR_mmap2)
	/* mmap2 takes the offset in 4KiB units; the io_uring magic offsets are
	 * all page aligned */
	return system_call<__NR_mmap2, ::std::ptrdiff_t>(nullptr, len, prot_read_write, map_shared_populate, fd,
													offset >> 12);
#else
	void *p{::fast_io::noexcept_call(::mmap, nullptr, len, prot_read_write, map_shared_populate, fd,
									 static_cast<::std::int64_t>(offset))};
	if (p == reinterpret_cast<void *>(-1))
	{
		return -errno;
	}
	return static_cast<::std::ptrdiff_t>(reinterpret_cast<::std::uintptr_t>(p));
#endif
}

/*
 * io_uring_setup(2) + mmap of the SQ ring, the CQ ring (shared when
 * io_uring_feat_single_mmap) and the SQE array. Ring pointers are then
 * derived from the offsets the kernel reported in io_uring_params.
 * io_uring_setup_no_sqarray is always tried first (like liburing >= 2.2);
 * on kernels that reject it we retry with the SQ array.
 */
inline void io_uring_queue_init_impl(io_uring_ring_state &ring, ::std::uint32_t entries, ::std::uint32_t flags)
	throws
{
	io_uring_params params{};
	params.flags = flags;
	if (!(flags & io_uring_setup_no_sqarray))
	{
		params.flags = flags | io_uring_setup_no_sqarray;
	}
	int fd{io_uring_setup_impl(entries, __builtin_addressof(params))};
	if (fd == -EINVAL && !(flags & io_uring_setup_no_sqarray))
	{
		params.flags = flags;
		fd = io_uring_setup_impl(entries, __builtin_addressof(params));
	}
	system_call_throw_error(fd);

	ring.sq.ring_sz = params.sq_off.array + params.sq_entries * sizeof(::std::uint32_t);
	::std::size_t cqes_bytes{static_cast<::std::size_t>(params.cq_entries)
							 << (((params.flags & io_uring_setup_cqe32) != 0) ? 5 : 4)};
	ring.cq.ring_sz = params.cq_off.cqes + cqes_bytes;

	if (params.features & io_uring_feat_single_mmap)
	{
		if (ring.cq.ring_sz > ring.sq.ring_sz)
		{
			ring.sq.ring_sz = ring.cq.ring_sz;
		}
		ring.cq.ring_sz = ring.sq.ring_sz;
	}

	int mmap_err{};
	::std::ptrdiff_t sq_ptr{io_uring_mmap_impl(ring.sq.ring_sz, fd, io_uring_off_sq_ring)};
	if (linux_system_call_fails(sq_ptr)) [[unlikely]]
	{
		mmap_err = static_cast<int>(-sq_ptr);
	}
	else
	{
		ring.sq.ring_ptr = reinterpret_cast<void *>(sq_ptr);
		if (params.features & io_uring_feat_single_mmap)
		{
			ring.cq.ring_ptr = ring.sq.ring_ptr;
		}
		else
		{
			::std::ptrdiff_t cq_ptr{io_uring_mmap_impl(ring.cq.ring_sz, fd, io_uring_off_cq_ring)};
			if (linux_system_call_fails(cq_ptr)) [[unlikely]]
			{
				mmap_err = static_cast<int>(-cq_ptr);
			}
			else
			{
				ring.cq.ring_ptr = reinterpret_cast<void *>(cq_ptr);
			}
		}
	}
	if (mmap_err == 0)
	{
		::std::size_t sqes_bytes{static_cast<::std::size_t>(params.sq_entries)
								 << (((params.flags & io_uring_setup_sqe128) != 0) ? 7 : 6)};
		::std::ptrdiff_t sqes_ptr{io_uring_mmap_impl(sqes_bytes, fd, io_uring_off_sqes)};
		if (linux_system_call_fails(sqes_ptr)) [[unlikely]]
		{
			mmap_err = static_cast<int>(-sqes_ptr);
		}
		else
		{
			ring.sq.sqes_sz = sqes_bytes;
			ring.sq.sqes = reinterpret_cast<io_uring_sqe *>(sqes_ptr);
		}
	}
	if (mmap_err != 0) [[unlikely]]
	{
		io_uring_unmap_rings(ring);
		if (ring.sq.sqes != nullptr)
		{
			sys_munmap_nothrow(ring.sq.sqes, ring.sq.sqes_sz);
			ring.sq.sqes = nullptr;
		}
		::fast_io::details::sys_close(fd);
		throw_posix_error(mmap_err);
	}

	::std::byte *sq_ring{reinterpret_cast<::std::byte *>(ring.sq.ring_ptr)};
	::std::byte *cq_ring{reinterpret_cast<::std::byte *>(ring.cq.ring_ptr)};

	ring.sq.khead = reinterpret_cast<::std::uint32_t *>(sq_ring + params.sq_off.head);
	ring.sq.ktail = reinterpret_cast<::std::uint32_t *>(sq_ring + params.sq_off.tail);
	ring.sq.kflags = reinterpret_cast<::std::uint32_t *>(sq_ring + params.sq_off.flags);
	ring.sq.kdropped = reinterpret_cast<::std::uint32_t *>(sq_ring + params.sq_off.dropped);
	if (!(params.flags & io_uring_setup_no_sqarray))
	{
		ring.sq.array = reinterpret_cast<::std::uint32_t *>(sq_ring + params.sq_off.array);
	}

	ring.cq.khead = reinterpret_cast<::std::uint32_t *>(cq_ring + params.cq_off.head);
	ring.cq.ktail = reinterpret_cast<::std::uint32_t *>(cq_ring + params.cq_off.tail);
	ring.cq.koverflow = reinterpret_cast<::std::uint32_t *>(cq_ring + params.cq_off.overflow);
	ring.cq.cqes = reinterpret_cast<io_uring_cqe *>(cq_ring + params.cq_off.cqes);

	ring.sq.ring_mask = *reinterpret_cast<::std::uint32_t *>(sq_ring + params.sq_off.ring_mask);
	ring.sq.ring_entries = *reinterpret_cast<::std::uint32_t *>(sq_ring + params.sq_off.ring_entries);
	ring.cq.ring_mask = *reinterpret_cast<::std::uint32_t *>(cq_ring + params.cq_off.ring_mask);
	ring.cq.ring_entries = *reinterpret_cast<::std::uint32_t *>(cq_ring + params.cq_off.ring_entries);

	if (ring.sq.array != nullptr)
	{
		/* map SQ slots directly to SQEs once, like liburing >= 2.0 does */
		for (::std::uint32_t index{}; index < ring.sq.ring_entries; ++index)
		{
			ring.sq.array[index] = index;
		}
	}

	ring.features = params.features;
	ring.flags = params.flags;
	ring.ring_fd = fd;

	/* IOPOLL without SQPOLL always needs io_uring_enter(2) to reap CQEs */
	if ((ring.flags & (io_uring_setup_iopoll | io_uring_setup_sqpoll)) == io_uring_setup_iopoll)
	{
		ring.int_flags |= io_uring_int_cq_enter;
	}
}

inline void io_uring_queue_exit_impl(io_uring_ring_state &ring) noexcept
{
	if (ring.sq.sqes != nullptr)
	{
		sys_munmap_nothrow(ring.sq.sqes, ring.sq.sqes_sz);
		ring.sq.sqes = nullptr;
	}
	io_uring_unmap_rings(ring);
	if (ring.ring_fd >= 0)
	{
		::fast_io::details::sys_close(ring.ring_fd);
		ring.ring_fd = -1;
	}
}

} // namespace details

/*
 * Owning io_uring object. The ring state is embedded (no heap allocation);
 * linux_io_uring_observer provides the non-owning view taken by all the
 * free functions below.
 */
class linux_io_uring : public linux_io_uring_observer
{
public:
	io_uring_ring_state storage{};

	inline linux_io_uring() noexcept
	{
		this->ring = __builtin_addressof(this->storage);
	}
	inline explicit linux_io_uring(native_interface_t, ::std::uint32_t entries, ::std::uint32_t flags)
		throws
		: linux_io_uring()
	{
		details::io_uring_queue_init_impl(this->storage, entries, flags);
	}
	inline explicit linux_io_uring(io_async_t) throws
		: linux_io_uring(native_interface, 64, 0)
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
		details::io_uring_queue_exit_impl(this->storage);
		this->storage = other.storage;
		other.storage = {};
		return *this;
	}
	inline ~linux_io_uring()
	{
		details::io_uring_queue_exit_impl(this->storage);
	}
};

namespace details
{

inline ::std::uint32_t io_uring_load_sq_head(io_uring_ring_state const &ring) noexcept
{
	/* without SQPOLL the kernel only advances khead as a hint */
	if (ring.flags & io_uring_setup_sqpoll)
	{
		return io_uring_load_acquire(ring.sq.khead);
	}
	return *ring.sq.khead;
}

inline void io_uring_initialize_sqe(io_uring_sqe *sqe) noexcept
{
	sqe->flags = 0;
	sqe->ioprio = 0;
	sqe->rw_flags = 0;
	sqe->buf_index = 0;
	sqe->personality = 0;
	sqe->file_index = 0;
	sqe->addr3_and_pad.addr3 = 0;
	sqe->addr3_and_pad.pad2 = 0;
}

inline bool io_uring_sq_ring_needs_enter(io_uring_ring_state const &ring, ::std::uint32_t submit,
										 ::std::uint32_t &flags) noexcept
{
	if (submit == 0)
	{
		return false;
	}
	if (!(ring.flags & io_uring_setup_sqpoll))
	{
		return true;
	}
	/* ensure the kernel can see the SQ tail store before we read the flags */
	io_uring_smp_mb();
	if (io_uring_load_acquire(ring.sq.kflags) & io_uring_sq_need_wakeup)
	{
		flags |= io_uring_enter_sq_wakeup;
		return true;
	}
	return false;
}

inline bool io_uring_cq_ring_needs_flush(io_uring_ring_state const &ring) noexcept
{
	return (io_uring_load_acquire(ring.sq.kflags) & (io_uring_sq_cq_overflow | io_uring_sq_taskrun)) != 0;
}

inline bool io_uring_cq_ring_needs_enter(io_uring_ring_state const &ring) noexcept
{
	return (ring.int_flags & io_uring_int_cq_enter) || io_uring_cq_ring_needs_flush(ring);
}

inline int io_uring_enter_impl(io_uring_ring_state const &ring, ::std::uint32_t to_submit,
							   ::std::uint32_t min_complete, ::std::uint32_t flags) noexcept
{
	return io_uring_enter2_impl(static_cast<::std::uint32_t>(ring.ring_fd), to_submit, min_complete, flags,
								nullptr, 0);
}

/*
 * Sync internal state with kernel ring state on the SQ side. Returns the
 * number of pending (unsubmitted) SQEs.
 */
inline ::std::uint32_t io_uring_flush_sq(io_uring_ring_state &ring) noexcept
{
	auto &sq{ring.sq};
	::std::uint32_t tail{sq.sqe_tail};
	if (ring.flags & io_uring_setup_sq_rewind)
	{
		sq.sqe_tail = 0;
		return tail;
	}
	if (sq.sqe_head != tail)
	{
		sq.sqe_head = tail;
		if (!(ring.flags & io_uring_setup_sqpoll))
		{
			*sq.ktail = tail;
		}
		else
		{
			io_uring_store_release(sq.ktail, tail);
		}
	}
	/* khead is written concurrently by the kernel for SQPOLL */
	return tail - io_uring_load_acquire(sq.khead);
}

/*
 * Advance the CQ head by the number of slots a CQE occupies (2 for
 * 32-byte CQEs in mixed rings, 1 otherwise).
 */
inline void io_uring_cq_advance_cqe(io_uring_ring_state &ring, io_uring_cqe const *cqe) noexcept
{
	io_uring_store_release(ring.cq.khead,
						   *ring.cq.khead + ((cqe->flags & io_uring_cqe_f_32) ? 2U : 1U));
}

/*
 * Whether this CQE is internal and must be consumed without being
 * reported: F_SKIP padding entries in mixed rings, and the internal
 * timeout SQE used by wait timeouts on kernels without
 * io_uring_feat_ext_arg. Errors from the internal timeout are propagated
 * through err.
 */
inline bool io_uring_skip_cqe(io_uring_ring_state &ring, io_uring_cqe *cqe, int &err) noexcept
{
	if (!(cqe->flags & io_uring_cqe_f_skip))
	{
		if (ring.features & io_uring_feat_ext_arg)
		{
			return false;
		}
		if (cqe->user_data != io_uring_internal_timeout_user_data)
		{
			return false;
		}
		if (cqe->res < 0)
		{
			err = cqe->res;
		}
	}
	io_uring_cq_advance_cqe(ring, cqe);
	return err == 0;
}

inline int io_uring_peek_cqe_impl(io_uring_ring_state &ring, io_uring_cqe **cqe_ptr,
								  ::std::uint32_t *nr_available) noexcept
{
	io_uring_cqe *cqe{};
	int err{};
	::std::uint32_t available{};
	::std::uint32_t mask{ring.cq.ring_mask};
	unsigned shift{io_uring_cqe_shift(ring)};
	do
	{
		/* acquire ordering pairs with the kernel publishing CQEs */
		::std::uint32_t tail{io_uring_load_acquire(ring.cq.ktail)};
		::std::uint32_t head{*ring.cq.khead};
		cqe = nullptr;
		available = tail - head;
		if (available == 0)
		{
			break;
		}
		cqe = ring.cq.cqes + ((head & mask) << shift);
		if (!io_uring_skip_cqe(ring, cqe, err))
		{
			if (err != 0)
			{
				cqe = nullptr;
			}
			break;
		}
		cqe = nullptr;
	} while (true);
	*cqe_ptr = cqe;
	if (nr_available != nullptr)
	{
		*nr_available = available;
	}
	return err;
}

struct io_uring_get_data
{
	::std::uint32_t submit;
	::std::uint32_t wait_nr;
	::std::uint32_t get_flags;
	::std::size_t sz;
	bool has_ts;
	void const *arg;
};

inline int io_uring_get_cqe_impl(io_uring_ring_state &ring, io_uring_cqe **cqe_ptr,
								 io_uring_get_data &data) noexcept
{
	io_uring_cqe *cqe{};
	bool looped{};
	int err{};
	do
	{
		bool need_enter{};
		::std::uint32_t flags{};
		::std::uint32_t nr_available{};
		int ret{io_uring_peek_cqe_impl(ring, __builtin_addressof(cqe), __builtin_addressof(nr_available))};
		if (ret != 0)
		{
			if (err == 0)
			{
				err = ret;
			}
			break;
		}
		if (cqe == nullptr && data.wait_nr == 0 && data.submit == 0)
		{
			/*
			 * If we already looped once, we already entered the kernel.
			 * Since there's nothing to submit or wait for, don't keep
			 * retrying.
			 */
			if (looped || !io_uring_cq_ring_needs_enter(ring))
			{
				if (err == 0)
				{
					err = -EAGAIN;
				}
				break;
			}
			need_enter = true;
		}
		if (data.wait_nr > nr_available || need_enter)
		{
			flags |= io_uring_enter_getevents | data.get_flags;
			need_enter = true;
		}
		if (io_uring_sq_ring_needs_enter(ring, data.submit, flags))
		{
			need_enter = true;
		}
		if (!need_enter)
		{
			break;
		}
		if (looped && data.has_ts)
		{
			if (cqe == nullptr && err == 0)
			{
				err = -ETIME;
			}
			break;
		}
		ret = io_uring_enter2_impl(static_cast<::std::uint32_t>(ring.ring_fd), data.submit, data.wait_nr,
								   flags, data.arg, data.sz);
		if (ret < 0)
		{
			if (err == 0)
			{
				err = ret;
			}
			break;
		}
		data.submit -= static_cast<::std::uint32_t>(ret);
		if (cqe != nullptr)
		{
			break;
		}
		if (!looped)
		{
			looped = true;
			err = ret;
		}
	} while (true);
	*cqe_ptr = cqe;
	return err;
}

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
			system_call_throw_error(ret);
		}
		else
		{
			::std::uint32_t submitted{io_uring_flush_sq(*ring.ring)};
			int ret{io_uring_enter_impl(*ring.ring, submitted, 0, 0)};
			system_call_throw_error(ret);
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
	::std::uint32_t head{details::io_uring_load_sq_head(*ring.ring)};
	::std::uint32_t tail{sq.sqe_tail};
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
inline ::std::uint32_t io_uring_sq_ready(linux_io_uring_observer ring) noexcept
{
	return ring.ring->sq.sqe_tail - details::io_uring_load_sq_head(*ring.ring);
}

/* Returns how much space is left in the SQ ring */
inline ::std::uint32_t io_uring_sq_space_left(linux_io_uring_observer ring) noexcept
{
	return ring.ring->sq.ring_entries - io_uring_sq_ready(ring);
}

/*
 * Publish pending SQEs to the kernel. Returns the number submitted.
 * With io_uring_setup_sqpoll this only wakes the SQ thread when needed.
 */
inline ::std::uint32_t io_uring_submit(linux_io_uring_observer ring) throws
{
	::std::uint32_t submitted{details::io_uring_flush_sq(*ring.ring)};
	bool cq_enter{details::io_uring_cq_ring_needs_enter(*ring.ring)};
	::std::uint32_t flags{};
	if (details::io_uring_sq_ring_needs_enter(*ring.ring, submitted, flags) || cq_enter)
	{
		if (cq_enter)
		{
			flags |= io_uring_enter_getevents;
		}
		int ret{details::io_uring_enter_impl(*ring.ring, submitted, 0, flags)};
		system_call_throw_error(ret);
		return static_cast<::std::uint32_t>(ret);
	}
	return submitted;
}

/* Submit pending SQEs and wait for at least wait_nr CQEs */
inline ::std::uint32_t io_uring_submit_and_wait(linux_io_uring_observer ring, ::std::uint32_t wait_nr)
	throws
{
	::std::uint32_t submitted{details::io_uring_flush_sq(*ring.ring)};
	bool cq_enter{wait_nr != 0 || details::io_uring_cq_ring_needs_enter(*ring.ring)};
	::std::uint32_t flags{};
	if (details::io_uring_sq_ring_needs_enter(*ring.ring, submitted, flags) || cq_enter)
	{
		if (cq_enter)
		{
			flags |= io_uring_enter_getevents;
		}
		int ret{details::io_uring_enter_impl(*ring.ring, submitted, wait_nr, flags)};
		system_call_throw_error(ret);
		return static_cast<::std::uint32_t>(ret);
	}
	return submitted;
}

/* Flush pending CQEs the kernel may be holding (IOPOLL/overflow/taskrun) */
inline void io_uring_get_events(linux_io_uring_observer ring) throws
{
	int ret{details::io_uring_enter_impl(*ring.ring, 0, 0, io_uring_enter_getevents)};
	system_call_throw_error(ret);
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
	system_call_throw_error(ret);
}

/* ======================= completion queue ======================= */

/* Returns how many unconsumed entries are ready in the CQ ring */
inline ::std::uint32_t io_uring_cq_ready(linux_io_uring_observer ring) noexcept
{
	return details::io_uring_load_acquire(ring.ring->cq.ktail) - *ring.ring->cq.khead;
}

/* Number of CQ slots a CQE occupies (2 for 32-byte CQEs in mixed rings) */
inline ::std::uint32_t io_uring_cqe_nr(io_uring_cqe const *cqe) noexcept
{
	return (cqe->flags & io_uring_cqe_f_32) ? 2U : 1U;
}

/*
 * Must be called after the application has consumed nr CQ slots, so the
 * kernel can reuse them.
 */
inline void io_uring_cq_advance(linux_io_uring_observer ring, ::std::uint32_t nr) noexcept
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
	io_uring_cqe *cqe{};
	int err{details::io_uring_peek_cqe_impl(*ring.ring, __builtin_addressof(cqe), nullptr)};
	if (err != 0)
	{
		system_call_throw_error(err);
	}
	if (cqe != nullptr)
	{
		return cqe;
	}
	if (!(ring.ring->flags & io_uring_setup_iopoll) &&
		!(details::io_uring_load_acquire(ring.ring->sq.kflags) & (io_uring_sq_cq_overflow | io_uring_sq_taskrun)))
	{
		return nullptr;
	}
	/* slow path: one kernel round trip with wait_nr = 0 */
	details::io_uring_get_data data{.submit = 0, .wait_nr = 0, .get_flags = 0, .sz = 0, .has_ts = false, .arg = nullptr};
	err = details::io_uring_get_cqe_impl(*ring.ring, __builtin_addressof(cqe), data);
	if (err < 0 && err != -EAGAIN)
	{
		system_call_throw_error(err);
	}
	return cqe;
}

/* Wait for (at least) wait_nr completions; returns the first CQE */
inline io_uring_cqe *io_uring_wait_cqes(linux_io_uring_observer ring, ::std::uint32_t wait_nr)
	throws
{
	io_uring_cqe *cqe{};
	details::io_uring_get_data data{.submit = 0, .wait_nr = wait_nr, .get_flags = 0, .sz = 0, .has_ts = false,
									.arg = nullptr};
	int err{details::io_uring_get_cqe_impl(*ring.ring, __builtin_addressof(cqe), data)};
	if (err < 0)
	{
		system_call_throw_error(err);
	}
	return cqe;
}

inline io_uring_cqe *io_uring_wait_cqe(linux_io_uring_observer ring) throws
{
	return io_uring_wait_cqes(ring, 1);
}

/* Submit pending SQEs, then wait for wait_nr completions */
inline io_uring_cqe *io_uring_submit_and_wait_cqes(linux_io_uring_observer ring, ::std::uint32_t wait_nr)
	throws
{
	io_uring_cqe *cqe{};
	details::io_uring_get_data data{.submit = details::io_uring_flush_sq(*ring.ring),
									.wait_nr = wait_nr,
									.get_flags = 0,
									.sz = 0,
									.has_ts = false,
									.arg = nullptr};
	int err{details::io_uring_get_cqe_impl(*ring.ring, __builtin_addressof(cqe), data)};
	if (err < 0)
	{
		system_call_throw_error(err);
	}
	return cqe;
}

/*
 * Wait for a completion with a timeout. On kernels with
 * io_uring_feat_ext_arg the deadline rides in io_uring_enter(2) directly;
 * on older kernels an internal io_uring_op_timeout SQE is queued instead.
 * Returns false on timeout (with *cqe_ptr set to nullptr).
 */
inline bool io_uring_wait_cqe_timeout(linux_io_uring_observer ring, io_uring_timespec const &ts,
									  io_uring_cqe **cqe_ptr) throws
{
	io_uring_cqe *cqe{};
	int err;
	if (ring.ring->features & io_uring_feat_ext_arg)
	{
		io_uring_getevents_arg arg{0, 0, 0, reinterpret_cast<::std::uint64_t>(__builtin_addressof(ts))};
		details::io_uring_get_data data{.submit = 0,
										.wait_nr = 1,
										.get_flags = io_uring_enter_ext_arg,
										.sz = sizeof(arg),
										.has_ts = true,
										.arg = __builtin_addressof(arg)};
		err = details::io_uring_get_cqe_impl(*ring.ring, __builtin_addressof(cqe), data);
	}
	else
	{
		/* queue an internal timeout SQE that the kernel completes when
		 * either the deadline expires or a CQE is posted */
		io_uring_sqe *sqe{details::ensure_io_uring_sqe(ring)};
		sqe->opcode = io_uring_op_timeout;
		sqe->fd = -1;
		sqe->addr = reinterpret_cast<::std::uint64_t>(__builtin_addressof(ts));
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
		err = details::io_uring_get_cqe_impl(*ring.ring, __builtin_addressof(cqe), data);
	}
	if (err == -ETIME)
	{
		*cqe_ptr = nullptr;
		return false;
	}
	if (err < 0)
	{
		system_call_throw_error(err);
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
inline ::std::uint32_t io_uring_peek_batch_cqe(linux_io_uring_observer ring, io_uring_cqe **cqes,
											   ::std::uint32_t count) throws
{
	::std::uint32_t ready{io_uring_cq_ready(ring)};
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
	::std::uint32_t head{*ring.ring->cq.khead};
	::std::uint32_t mask{ring.ring->cq.ring_mask};
	unsigned shift{details::io_uring_cqe_shift(*ring.ring)};
	::std::uint32_t nr{};
	::std::uint32_t last{head + ready};
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

/* user_data helpers */
inline void io_uring_sqe_set_data64(io_uring_sqe *sqe, ::std::uint64_t data) noexcept
{
	sqe->user_data = data;
}

inline void io_uring_sqe_set_data(io_uring_sqe *sqe, void const *data) noexcept
{
	sqe->user_data = reinterpret_cast<::std::uint64_t>(data);
}

inline ::std::uint64_t io_uring_cqe_get_data64(io_uring_cqe const *cqe) noexcept
{
	return cqe->user_data;
}

inline void *io_uring_cqe_get_data(io_uring_cqe const *cqe) noexcept
{
	return reinterpret_cast<void *>(static_cast<::std::uintptr_t>(cqe->user_data));
}

inline void io_uring_sqe_set_flags(io_uring_sqe *sqe, ::std::uint32_t flags) noexcept
{
	sqe->flags = static_cast<::std::uint8_t>(flags);
}

/* ======================= sqe preparation ======================= */

/*
 * Shared initializer for the rw-shaped opcodes: fd, one address/length
 * pair and a 64-bit offset. All the prep functions below reduce to it,
 * exactly like liburing's io_uring_prep_rw.
 */
inline void io_uring_prep_rw(io_uring_op op, io_uring_sqe *sqe, int fd, void const *addr,
							 ::std::uint32_t len, ::std::uint64_t offset) noexcept
{
	sqe->opcode = static_cast<::std::uint8_t>(op);
	sqe->fd = fd;
	sqe->off = offset;
	sqe->addr = reinterpret_cast<::std::uint64_t>(addr);
	sqe->len = len;
}

inline void io_uring_prep_nop(io_uring_sqe *sqe) noexcept
{
	io_uring_prep_rw(io_uring_op_nop, sqe, -1, nullptr, 0, 0);
}

inline void io_uring_prep_readv(io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								::std::uint32_t nr_vecs, ::std::uint64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_readv, sqe, fd, iovecs, nr_vecs, offset);
}

inline void io_uring_prep_writev(io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								 ::std::uint32_t nr_vecs, ::std::uint64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_writev, sqe, fd, iovecs, nr_vecs, offset);
}

inline void io_uring_prep_readv2(io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								 ::std::uint32_t nr_vecs, ::std::uint64_t offset, ::std::uint32_t flags) noexcept
{
	io_uring_prep_readv(sqe, fd, iovecs, nr_vecs, offset);
	sqe->rw_flags = flags;
}

inline void io_uring_prep_writev2(io_uring_sqe *sqe, int fd, io_scatter_t const *iovecs,
								  ::std::uint32_t nr_vecs, ::std::uint64_t offset, ::std::uint32_t flags) noexcept
{
	io_uring_prep_writev(sqe, fd, iovecs, nr_vecs, offset);
	sqe->rw_flags = flags;
}

inline void io_uring_prep_read(io_uring_sqe *sqe, int fd, void *buf, ::std::uint32_t nbytes,
							   ::std::uint64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_read, sqe, fd, buf, nbytes, offset);
}

inline void io_uring_prep_write(io_uring_sqe *sqe, int fd, void const *buf, ::std::uint32_t nbytes,
								::std::uint64_t offset) noexcept
{
	io_uring_prep_rw(io_uring_op_write, sqe, fd, buf, nbytes, offset);
}

inline void io_uring_prep_read_fixed(io_uring_sqe *sqe, int fd, void *buf, ::std::uint32_t nbytes,
									 ::std::uint64_t offset, ::std::uint16_t buf_index) noexcept
{
	io_uring_prep_rw(io_uring_op_read_fixed, sqe, fd, buf, nbytes, offset);
	sqe->buf_index = buf_index;
}

inline void io_uring_prep_write_fixed(io_uring_sqe *sqe, int fd, void const *buf, ::std::uint32_t nbytes,
									  ::std::uint64_t offset, ::std::uint16_t buf_index) noexcept
{
	io_uring_prep_rw(io_uring_op_write_fixed, sqe, fd, buf, nbytes, offset);
	sqe->buf_index = buf_index;
}

inline void io_uring_prep_fsync(io_uring_sqe *sqe, int fd, ::std::uint32_t fsync_flags) noexcept
{
	io_uring_prep_rw(io_uring_op_fsync, sqe, fd, nullptr, 0, 0);
	sqe->fsync_flags = fsync_flags;
}

inline void io_uring_prep_sync_file_range(io_uring_sqe *sqe, int fd, ::std::uint32_t len,
										  ::std::uint64_t offset, ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_sync_file_range, sqe, fd, nullptr, len, offset);
	sqe->sync_range_flags = flags;
}

inline void io_uring_prep_recvmsg(io_uring_sqe *sqe, int fd, void *msg, ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_recvmsg, sqe, fd, msg, 1, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_sendmsg(io_uring_sqe *sqe, int fd, void const *msg, ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_sendmsg, sqe, fd, msg, 1, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_recv(io_uring_sqe *sqe, int fd, void *buf, ::std::uint32_t len,
							   ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_recv, sqe, fd, buf, len, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_send(io_uring_sqe *sqe, int fd, void const *buf, ::std::uint32_t len,
							   ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_send, sqe, fd, buf, len, 0);
	sqe->msg_flags = flags;
}

inline void io_uring_prep_poll_add(io_uring_sqe *sqe, int fd, ::std::uint32_t poll_mask) noexcept
{
	io_uring_prep_rw(io_uring_op_poll_add, sqe, fd, nullptr, 0, 0);
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	/* poll32_events is word-reversed relative to poll_events on BE */
	poll_mask = (poll_mask >> 16) | (poll_mask << 16);
#endif
	sqe->poll32_events = poll_mask;
}

inline void io_uring_prep_poll_multishot(io_uring_sqe *sqe, int fd, ::std::uint32_t poll_mask) noexcept
{
	io_uring_prep_poll_add(sqe, fd, poll_mask);
	sqe->len = io_uring_poll_add_multi;
}

inline void io_uring_prep_poll_remove(io_uring_sqe *sqe, ::std::uint64_t user_data) noexcept
{
	io_uring_prep_rw(io_uring_op_poll_remove, sqe, -1, nullptr, 0, 0);
	sqe->addr = user_data;
}

inline void io_uring_prep_timeout(io_uring_sqe *sqe, io_uring_timespec const *ts, ::std::uint32_t count,
								  ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_timeout, sqe, -1, ts, 1, count);
	sqe->timeout_flags = flags;
}

inline void io_uring_prep_timeout_remove(io_uring_sqe *sqe, ::std::uint64_t user_data,
										 ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_timeout_remove, sqe, -1, nullptr, 0, 0);
	sqe->addr = user_data;
	sqe->timeout_flags = flags;
}

inline void io_uring_prep_timeout_update(io_uring_sqe *sqe, io_uring_timespec const *ts,
										 ::std::uint64_t user_data, ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_timeout_remove, sqe, -1, nullptr, 0,
					 reinterpret_cast<::std::uint64_t>(ts));
	sqe->addr = user_data;
	sqe->timeout_flags = flags | io_uring_timeout_update;
}

inline void io_uring_prep_link_timeout(io_uring_sqe *sqe, io_uring_timespec const *ts,
									   ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_link_timeout, sqe, -1, ts, 1, 0);
	sqe->timeout_flags = flags;
}

inline void io_uring_prep_accept(io_uring_sqe *sqe, int fd, void *addr, void *addrlen,
								 ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_accept, sqe, fd, addr, 0, reinterpret_cast<::std::uint64_t>(addrlen));
	sqe->accept_flags = flags;
}

inline void io_uring_prep_multishot_accept(io_uring_sqe *sqe, int fd, void *addr, void *addrlen,
										   ::std::uint32_t flags) noexcept
{
	io_uring_prep_accept(sqe, fd, addr, addrlen, flags);
	sqe->ioprio |= io_uring_accept_multishot;
}

inline void io_uring_prep_cancel64(io_uring_sqe *sqe, ::std::uint64_t user_data,
								   ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_async_cancel, sqe, -1, nullptr, 0, 0);
	sqe->addr = user_data;
	sqe->cancel_flags = flags;
}

inline void io_uring_prep_cancel_fd(io_uring_sqe *sqe, int fd, ::std::uint32_t flags) noexcept
{
	io_uring_prep_rw(io_uring_op_async_cancel, sqe, fd, nullptr, 0, 0);
	sqe->cancel_flags = flags | io_uring_async_cancel_fd;
}

inline void io_uring_prep_connect(io_uring_sqe *sqe, int fd, void const *addr,
								  ::std::uint32_t addrlen) noexcept
{
	io_uring_prep_rw(io_uring_op_connect, sqe, fd, addr, 0, addrlen);
}

inline void io_uring_prep_openat(io_uring_sqe *sqe, int dfd, char const *path, ::std::uint32_t flags,
								 ::std::uint32_t mode) noexcept
{
	io_uring_prep_rw(io_uring_op_openat, sqe, dfd, path, mode, 0);
	sqe->open_flags = flags;
}

inline void io_uring_prep_close(io_uring_sqe *sqe, int fd) noexcept
{
	io_uring_prep_rw(io_uring_op_close, sqe, fd, nullptr, 0, 0);
}

inline void io_uring_prep_close_direct(io_uring_sqe *sqe, ::std::uint32_t file_index) noexcept
{
	io_uring_prep_close(sqe, 0);
	/* indexes are encoded as "index + 1"; 0 means "no fixed file" */
	sqe->file_index = file_index + 1;
}

inline void io_uring_prep_splice(io_uring_sqe *sqe, int fd_in, ::std::int64_t off_in, int fd_out,
								 ::std::int64_t off_out, ::std::uint32_t nbytes,
								 ::std::uint32_t splice_flags) noexcept
{
	io_uring_prep_rw(io_uring_op_splice, sqe, fd_out, nullptr, nbytes, static_cast<::std::uint64_t>(off_out));
	sqe->splice_off_in = static_cast<::std::uint64_t>(off_in);
	sqe->splice_fd_in = fd_in;
	sqe->splice_flags = splice_flags;
}

inline void io_uring_prep_tee(io_uring_sqe *sqe, int fd_in, int fd_out, ::std::uint32_t nbytes,
							  ::std::uint32_t splice_flags) noexcept
{
	io_uring_prep_rw(io_uring_op_tee, sqe, fd_out, nullptr, nbytes, 0);
	sqe->splice_off_in = 0;
	sqe->splice_fd_in = fd_in;
	sqe->splice_flags = splice_flags;
}

inline void io_uring_prep_shutdown(io_uring_sqe *sqe, int fd, ::std::uint32_t how) noexcept
{
	io_uring_prep_rw(io_uring_op_shutdown, sqe, fd, nullptr, how, 0);
}

/* request a direct (fixed) descriptor as the result of ops like
 * openat/accept; pass io_uring_file_index_alloc - 1 to let the kernel
 * pick the slot */
inline void io_uring_sqe_set_target_fixed_file(io_uring_sqe *sqe, ::std::uint32_t file_index) noexcept
{
	sqe->file_index = file_index + 1;
}

/* ======================= io_uring_register(2) ======================= */

inline int io_uring_register(linux_io_uring_observer ring, ::std::uint32_t opcode, void const *arg,
							 ::std::uint32_t nr_args) throws
{
	int ret{details::io_uring_register_impl(static_cast<::std::uint32_t>(ring.ring->ring_fd), opcode, arg,
											nr_args)};
	system_call_throw_error(ret);
	return ret;
}

inline int io_uring_register_buffers(linux_io_uring_observer ring, io_scatter_t const *iovecs,
									 ::std::uint32_t nr) throws
{
	return io_uring_register(ring, io_uring_regop_buffers, iovecs, nr);
}

inline int io_uring_unregister_buffers(linux_io_uring_observer ring) throws
{
	return io_uring_register(ring, io_uring_regop_unregister_buffers, nullptr, 0);
}

inline int io_uring_register_files(linux_io_uring_observer ring, int const *files,
								   ::std::uint32_t nr) throws
{
	return io_uring_register(ring, io_uring_regop_files, files, nr);
}

inline int io_uring_unregister_files(linux_io_uring_observer ring) throws
{
	return io_uring_register(ring, io_uring_regop_unregister_files, nullptr, 0);
}

/* ======================= completion model ======================= */

namespace details
{
/*
 * Internal completion ABI: sqe->user_data points at one of these and the
 * event loop invokes it once per cqe. Not part of the public API —
 * applications only hand functors to the *_callback_define functions.
 * invoke() receives (bytes transferred, 0) on success and
 * (0, errno value) on failure.
 */
class io_uring_completion_base
{
public:
#if __cpp_constexpr >= 201907L
	constexpr
#endif
		virtual void
		invoke(::std::size_t transferred, int errn) noexcept = 0;
#if __cpp_constexpr >= 201907L
	constexpr
#endif
		virtual ~io_uring_completion_base() = default;
};

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

/* allocate + construct a T completion through the scheduler's allocator */
template <typename T, typename scheduler, typename... Args>
inline T *io_uring_new_completion(scheduler, Args &&...args) throws
{
	using typed_alloc =
		::fast_io::typed_generic_allocator_adapter<io_uring_scheduler_allocator_t<scheduler>, T>;
	T *p{typed_alloc::allocate(1)};
	try
	{
		new (p) T(::std::forward<Args>(args)...);
	}
	catch throws(::std::error e)
	{
		typed_alloc::deallocate(p);
		throw throws;
	}
	catch (...)
	{
		typed_alloc::deallocate(p);
		throw;
	}
	return p;
}

/* every self-owning completion exposes its allocator as allocator_type */
template <typename T>
inline void io_uring_delete_completion(T *p) noexcept
{
	p->~T();
	::fast_io::typed_generic_allocator_adapter<typename T::allocator_type, T>::deallocate(p);
}

inline void io_uring_dispatch_cqe(linux_io_uring_observer ring, io_uring_cqe *cqe) noexcept
{
	if (cqe == nullptr)
	{
		return;
	}
	void *data{io_uring_cqe_get_data(cqe)};
	::std::int_least32_t res{cqe->res};
	io_uring_cqe_seen(ring, cqe);
	auto *completion{static_cast<io_uring_completion_base *>(data)};
	if (completion == nullptr) [[unlikely]]
	{
		return;
	}
	if (res < 0)
	{
		completion->invoke(0, -res);
	}
	else
	{
		completion->invoke(static_cast<::std::size_t>(res), 0);
	}
}
} // namespace details

/*
 * Reap one completion and dispatch it to its completion object, blocking
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
#if defined(FAST_IO_LINUX_IO_URING_HAS_CHRONO)
template <typename Rep, typename Period>
inline bool io_async_wait_timeout(linux_io_uring_observer ring, ::std::chrono::duration<Rep, Period> duration)
	throws
{
	auto seconds{::std::chrono::duration_cast<::std::chrono::seconds>(duration)};
	auto nanos{::std::chrono::duration_cast<::std::chrono::nanoseconds>(duration - seconds)};
	io_uring_timespec ts{static_cast<::std::int64_t>(seconds.count()),
						 static_cast<::std::int64_t>(nanos.count())};
	io_uring_cqe *cqe{};
	if (!io_uring_wait_cqe_timeout(ring, ts, __builtin_addressof(cqe)))
	{
		return false;
	}
	details::io_uring_dispatch_cqe(ring, cqe);
	return true;
}
#endif

/* ======================= fd-based async operations ======================= */

template <::std::integral char_type>
inline constexpr io_type_t<linux_io_uring_observer>
async_scheduler_type(basic_posix_family_io_observer<posix_family::api, char_type>) noexcept
{
	return {};
}

namespace details
{
/*
 * sqe->off == ~0 tells the kernel to use (and advance) the file's current
 * position, i.e. read(2)/write(2) rather than pread(2)/pwrite(2).
 */
inline constexpr ::std::uint64_t io_uring_use_file_position{static_cast<::std::uint64_t>(-1)};

/*
 * Self-owning completion behind the *_callback_define functions:
 * allocated through the scheduler's allocator_type at submission and
 * freed at the end of invoke(), after the callback has run. The callback
 * is invoked once as callback(::std::cxx_std_error, ptr) noexcept where
 * ptr is one past the last byte transferred; a null domain is success.
 */
template <typename alloc_type, typename T>
class io_uring_bytes_callback_completion : public io_uring_completion_base
{
public:
	using allocator_type = alloc_type;
	::std::byte const *first{};
	T callback;
	template <typename... Args>
		requires ::std::constructible_from<T, Args...>
	inline constexpr io_uring_bytes_callback_completion(::std::byte const *f, Args &&...args)
		: first{f}, callback(::std::forward<Args>(args)...)
	{
	}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
		void
		invoke(::std::size_t transferred, int errn) noexcept override
	{
		::std::cxx_std_error err{};
		if (errn != 0)
		{
			err.domain = ::std::error_domain<::std::errc>::domain();
			err.code = static_cast<::std::size_t>(errn);
		}
		callback(err, first + transferred);
		io_uring_delete_completion(this);
	}
};
} // namespace details

/*
 * Async write_some_bytes. The operation is committed to the scheduler
 * before returning, matching cross-platform async APIs where no submit
 * step exists. callback is a functor invoked once as
 * callback(::std::cxx_std_error, writtenptr) noexcept on completion:
 * err.domain == nullptr means success, otherwise domain is the posix
 * domain and code the errno value; writtenptr is first + bytes written.
 * The functor is moved into the completion object, which frees itself
 * after the callback runs, so the functor need not outlive the submission.
 */
template <::std::integral char_type, typename func>
	requires ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::std::byte const *>
inline void
async_write_some_bytes_callback_define(linux_io_uring_observer ring,
									   basic_posix_family_io_observer<posix_family::api, char_type> piob,
									   ::std::byte const *first, ::std::byte const *last,
									   func &&callback) throws
{
	using completion_type = details::io_uring_bytes_callback_completion<
		details::io_uring_scheduler_allocator_t<linux_io_uring_observer>, ::std::remove_cvref_t<func>>;
	auto *completion{details::io_uring_new_completion<completion_type>(ring, first,
																	 ::std::forward<func>(callback))};
	io_uring_sqe *sqe{};
	try
	{
		sqe = details::ensure_io_uring_sqe(ring);
		io_uring_prep_write(sqe, piob.fd, first, static_cast<::std::uint32_t>(last - first),
							details::io_uring_use_file_position);
		io_uring_sqe_set_data(sqe, completion);
		io_uring_submit(ring);
	}
	catch throws(::std::error e)
	{
		if (sqe != nullptr)
		{
			/* the slot was already consumed; if it ever reaches the
			 * kernel it must not carry a dangling user_data */
			io_uring_prep_nop(sqe);
			io_uring_sqe_set_data(sqe, nullptr);
		}
		details::io_uring_delete_completion(completion);
		throw throws;
	}
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
	linux_io_uring_observer ring;
	basic_posix_family_io_observer<posix_family::api, char_type> piob;
	::std::byte const *last;
	T callback;
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
					io_uring_delete_completion(state);
					return;
				}
				io_uring_write_all_bytes_submit(state, written);
			});
	}
	catch throws(::std::error e)
	{
		state->callback(e.release());
		io_uring_delete_completion(state);
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
							basic_posix_family_io_observer<posix_family::api, char_type> piob,
							::std::byte const *first, ::std::byte const *last,
							func &&callback) throws
{
	using state_type =
		details::io_uring_write_all_bytes_state<char_type,
												details::io_uring_scheduler_allocator_t<linux_io_uring_observer>,
												::std::remove_cvref_t<func>>;
	auto *state{details::io_uring_new_completion<state_type>(ring, ring, piob, last,
														   ::std::forward<func>(callback))};
	details::io_uring_write_all_bytes_submit(state, first);
}

namespace details
{
template <::std::integral char_type>
struct io_uring_write_all_bytes_awaiter
{
	linux_io_uring_observer ring;
	basic_posix_family_io_observer<posix_family::api, char_type> piob;
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
	inline constexpr ::std::cxx_std_error await_resume() const noexcept
	{
		return result;
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
							basic_posix_family_io_observer<posix_family::api, char_type> piob,
							::std::byte const *first, ::std::byte const *last) noexcept
{
	return {ring, piob, first, last};
}

} // namespace fast_io

#include <fast_io_dsal/impl/misc/pop_macros.h>

#endif
