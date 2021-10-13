#pragma once

namespace fast_io::win32::nt
{

struct ioring_structv1
{
	std::uint32_t IoRingVersion;
	std::uint32_t SubmissionQueueSize;
	std::uint32_t CompletionQueueSize;
	std::uint32_t RequiredFlags;
	std::uint32_t AdvisoryFlags;
};

struct ioring_queue_head
{
	std::uint32_t QueueIndex;
	std::uint32_t QueueCount;
	std::uint64_t Alignment;
};

struct nt_ioring_info
{
	std::uint32_t Version;
	IORING_CREATE_FLAGS Flags;
	std::uint32_t SubmissionQueueSize;
	std::uint32_t SubQueueSizeMask;
	std::uint32_t CompletionQueueSize;
	std::uint32_t CompQueueSizeMask;
	ioring_queue_head* SubQueueBase;
	void* CompQueueBase;
};

struct large_integer
{
	std::uint32_t low,high;
};

struct nt_ioring_sqe
{
	std::uint32_t Opcode;
	std::uint32_t Flags;
	void* FileRef;
	large_integer FileOffset;
	void* Buffer;
	std::uint32_t BufferSize;
	std::uint32_t BufferOffset;
	std::uint32_t Key;
	void* Unknown;
	void* UserData;
	void* stuff1;
	void* stuff2;
	void* stuff3;
	void* stuff4;
};

class nt_ioring_handle
{
	void* handle{};
	nt_ioring_info ring_info;
	ioring_structv1 ring_struct;
};

template<bool zw>
inline std::uint32_t nt_close_io_ring(nt_ioring_handle* h) noexcept
{
	if(h==nullptr)
		return 0;
	auto hd{h->handle};
	std::uint32_t ret{};
	if(hd)
	{
		ret=nt_close<zw>(hd);
	}
	std::free(h);
	return ret;
}

inline nt_ioring_handle* nt_create_io_ring()
{
	nt_ioring_handle
}

}
