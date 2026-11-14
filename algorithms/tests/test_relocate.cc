#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstdint>
#include <vector>

struct counted
{
	int value;
	static inline int destroyed{};
	counted(int v) : value{v} {}
	counted(counted const &o) : value{o.value} {}
	counted(counted &&o) : value{o.value} {}
	counted &operator=(counted const &) = default;
	counted &operator=(counted &&) = default;
	~counted() { ++destroyed; }
};

constexpr bool constexpr_relocate_test()
{
	int src[4]{1, 2, 3, 4};
	union alignas(int) buffer
	{
		int data[4];
	};
	buffer b{};
	auto const r{::fast_io::algorithms::uninitialized_relocate_n_iters(src, 4, b.data)};
	return r == b.data + 4 && b.data[0] == 1 && b.data[3] == 4;
}

static_assert(constexpr_relocate_test());

constexpr bool constexpr_move_test()
{
	union alignas(int) buffer
	{
		int data[8];
	};
	buffer b{};
	b.data[0] = 1;
	b.data[1] = 2;
	auto const r{::fast_io::algorithms::uninitialized_move_n_iters(b.data, 2, b.data + 4)};
	return r == b.data + 6 && b.data[4] == 1 && b.data[5] == 2;
}

static_assert(constexpr_move_test());

int main()
{
	// trivial relocate forward
	int a[4]{1, 2, 3, 4};
	int b[4]{};
	auto const r{::fast_io::algorithms::uninitialized_relocate(a, b)};
	if (r != b + 4 || b[0] != 1 || b[3] != 4)
	{
		return 1;
	}

	// backward relocate within a buffer (overlap)
	int c[8]{1, 2, 3, 4, 0, 0, 0, 0};
	auto const rb{::fast_io::algorithms::uninitialized_relocate_backward_iters(c, c + 4, c + 6)};
	if (rb != c + 2 || c[2] != 1 || c[5] != 4)
	{
		return 2;
	}

	// non-trivial relocate destroys source
	alignas(counted) unsigned char src_storage[sizeof(counted) * 4]{};
	alignas(counted) unsigned char dst_storage[sizeof(counted) * 4]{};
	auto *src{reinterpret_cast<counted *>(src_storage)};
	auto *dst{reinterpret_cast<counted *>(dst_storage)};
	for (int i{}; i != 4; ++i)
	{
		::new (static_cast<void *>(src + i)) counted(i);
	}
	counted::destroyed = 0;
	auto const rr{::fast_io::algorithms::uninitialized_relocate_iters(src, src + 4, dst)};
	if (rr != dst + 4 || dst[0].value != 0 || dst[3].value != 3 || counted::destroyed != 4)
	{
		return 3;
	}
	for (int i{}; i != 4; ++i)
	{
		dst[i].~counted();
	}

	// uninitialized_move does not destroy source
	alignas(counted) unsigned char src2_storage[sizeof(counted) * 4]{};
	alignas(counted) unsigned char dst2_storage[sizeof(counted) * 4]{};
	auto *src2{reinterpret_cast<counted *>(src2_storage)};
	auto *dst2{reinterpret_cast<counted *>(dst2_storage)};
	for (int i{}; i != 4; ++i)
	{
		::new (static_cast<void *>(src2 + i)) counted(i + 10);
	}
	counted::destroyed = 0;
	auto const mr{::fast_io::algorithms::uninitialized_move_iters(src2, src2 + 4, dst2)};
	if (mr != dst2 + 4 || dst2[0].value != 10 || dst2[3].value != 13 || counted::destroyed != 0)
	{
		return 4;
	}
	for (int i{}; i != 4; ++i)
	{
		src2[i].~counted();
		dst2[i].~counted();
	}

	// uninitialized_move_backward
	alignas(counted) unsigned char src3_storage[sizeof(counted) * 4]{};
	alignas(counted) unsigned char dst3_storage[sizeof(counted) * 6]{};
	auto *src3{reinterpret_cast<counted *>(src3_storage)};
	auto *dst3{reinterpret_cast<counted *>(dst3_storage)};
	for (int i{}; i != 4; ++i)
	{
		::new (static_cast<void *>(src3 + i)) counted(i + 20);
	}
	auto const mbr{::fast_io::algorithms::uninitialized_move_backward_iters(src3, src3 + 4, dst3 + 6)};
	if (mbr != dst3 + 2 || dst3[2].value != 20 || dst3[5].value != 23)
	{
		return 5;
	}
	for (int i{}; i != 4; ++i)
	{
		src3[i].~counted();
	}
	for (int i{}; i != 4; ++i)
	{
		dst3[i + 2].~counted();
	}

	// relocate backward non-trivial
	alignas(counted) unsigned char src4_storage[sizeof(counted) * 4]{};
	alignas(counted) unsigned char dst4_storage[sizeof(counted) * 6]{};
	auto *src4{reinterpret_cast<counted *>(src4_storage)};
	auto *dst4{reinterpret_cast<counted *>(dst4_storage)};
	for (int i{}; i != 4; ++i)
	{
		::new (static_cast<void *>(src4 + i)) counted(i + 30);
	}
	counted::destroyed = 0;
	auto const rbr{::fast_io::algorithms::uninitialized_relocate_backward_iters(src4, src4 + 4, dst4 + 6)};
	if (rbr != dst4 + 2 || dst4[2].value != 30 || dst4[5].value != 33 || counted::destroyed != 4)
	{
		return 6;
	}
	for (int i{}; i != 4; ++i)
	{
		dst4[i + 2].~counted();
	}

	// vector relocate
	std::vector<int> v{1, 2, 3, 4};
	alignas(int) unsigned char vbuf[sizeof(int) * 4]{};
	auto *vp{reinterpret_cast<int *>(vbuf)};
	::fast_io::algorithms::uninitialized_relocate(v, vp);
	if (vp[0] != 1 || vp[3] != 4)
	{
		return 7;
	}

	// n variants
	int nsrc[4]{5, 6, 7, 8};
	int ndst[4]{};
	::fast_io::algorithms::uninitialized_relocate_n(nsrc, 2, ndst);
	if (ndst[0] != 5 || ndst[1] != 6)
	{
		return 8;
	}
	int msrc[4]{5, 6, 7, 8};
	int mdst[4]{};
	::fast_io::algorithms::uninitialized_move_n(msrc, 2, mdst);
	if (mdst[0] != 5 || mdst[1] != 6)
	{
		return 9;
	}
	return 0;
}

static_assert(noexcept(::fast_io::algorithms::uninitialized_relocate(
	::std::declval<int (&)[4]>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::uninitialized_move(
	::std::declval<int (&)[4]>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::uninitialized_relocate_n_iters(
	::std::declval<int *>(), ::std::size_t{}, ::std::declval<int *>())));

struct may_throw
{
	may_throw() = default;
	may_throw(may_throw const &) {}
	may_throw(may_throw &&) {}
	may_throw &operator=(may_throw const &) = default;
	may_throw &operator=(may_throw &&) = default;
};

static_assert(!noexcept(::fast_io::algorithms::uninitialized_relocate_iters(
	::std::declval<may_throw *>(), ::std::declval<may_throw *>(), ::std::declval<may_throw *>())));
static_assert(!noexcept(::fast_io::algorithms::uninitialized_move_iters(
	::std::declval<may_throw *>(), ::std::declval<may_throw *>(), ::std::declval<may_throw *>())));
static_assert(noexcept(::fast_io::algorithms::copy_iters(
	::std::declval<int *>(), ::std::declval<int *>(), ::std::declval<int *>())));
