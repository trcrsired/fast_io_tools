#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstdint>
#include <vector>
#include <list>

struct pair16
{
	std::uint64_t a;
	std::uint64_t b;
	auto operator==(pair16 const &) const noexcept -> bool = default;
};

struct counted
{
	int value;
	static inline int constructed{};
	static inline int destroyed{};
	counted(int v) : value{v} { ++constructed; }
	counted(counted const &o) : value{o.value} { ++constructed; }
	counted(counted &&o) : value{o.value} { ++constructed; }
	counted &operator=(counted const &) = default;
	counted &operator=(counted &&) = default;
	~counted() { ++destroyed; }
};

constexpr bool constexpr_copy_test()
{
	int src[5]{1, 2, 3, 4, 5};
	int dst[5]{};
	auto const r{::fast_io::algorithms::copy(src, dst)};
	if (r != dst + 5 || dst[0] != 1 || dst[4] != 5)
	{
		return false;
	}
	int dst2[5]{};
	auto const r2{::fast_io::algorithms::copy_backward(src, dst2 + 5)};
	return r2 == dst2 && dst2[0] == 1 && dst2[4] == 5;
}

static_assert(constexpr_copy_test());

constexpr bool constexpr_uninitialized_copy_test()
{
	union alignas(int) buffer
	{
		int data[4];
	};
	buffer b{};
	auto const r{::fast_io::algorithms::uninitialized_copy_n_iters(b.data, 0, b.data)};
	return r == b.data;
}

static_assert(constexpr_uninitialized_copy_test());


static_assert(noexcept(::fast_io::algorithms::copy_if(
	::std::declval<std::vector<int> &>(), ::std::declval<int *>(),
	[](int value) noexcept { return value == 1; })));

int main()
{
	{
		int const source[]{1, 2, 1, 3};
		int out[4]{};
		auto const end{::fast_io::algorithms::copy_if_iters(
			source, source + 4, out,
			[](int value) noexcept { return value == 1; })};
		if (end != out + 2 || out[0] != 1 || out[1] != 1)
		{
			return 9;
		}
	}
	// copy + overlap via copy (memmove semantics)
	int a[8]{0, 1, 2, 3, 4, 5, 6, 7};
	::fast_io::algorithms::copy_n_iters(a, 4, a + 2);
	if (a[2] != 0 || a[3] != 1 || a[4] != 2 || a[5] != 3)
	{
		return 1;
	}

	int b[8]{0, 1, 2, 3, 4, 5, 6, 7};
	::fast_io::algorithms::copy_backward_iters(b + 2, b + 8, b + 6);
	if (b[0] != 2 || b[5] != 7)
	{
		return 2;
	}

	// non_overlapped_copy
	int c[4];
	int const src[4]{10, 20, 30, 40};
	::fast_io::algorithms::non_overlapped_copy(src, c);
	if (c[0] != 10 || c[3] != 40)
	{
		return 3;
	}

	// copy_n on range
	int d[4]{};
	::fast_io::algorithms::copy_n(src, 2, d);
	if (d[0] != 10 || d[1] != 20)
	{
		return 4;
	}

	// uninitialized copy of non-trivial type
	alignas(counted) unsigned char storage[sizeof(counted) * 4]{};
	auto *base{reinterpret_cast<counted *>(storage)};
	counted csrc[4]{1, 2, 3, 4};
	auto const ur{::fast_io::algorithms::uninitialized_copy(csrc, base)};
	if (ur != base + 4 || base[0].value != 1 || base[3].value != 4)
	{
		return 5;
	}
	for (int i{}; i != 4; ++i)
	{
		base[i].~counted();
	}

	// backward uninitialized copy into buffer
	alignas(counted) unsigned char storage2[sizeof(counted) * 4]{};
	auto *base2{reinterpret_cast<counted *>(storage2)};
	auto const ubr{::fast_io::algorithms::uninitialized_copy_backward(csrc, base2 + 4)};
	if (ubr != base2 || base2[0].value != 1 || base2[3].value != 4)
	{
		return 6;
	}
	for (int i{}; i != 4; ++i)
	{
		base2[i].~counted();
	}

	// vector
	std::vector<int> v{1, 2, 3, 4, 5};
	std::vector<int> w(5);
	::fast_io::algorithms::copy(v, w.begin());
	if (w[0] != 1 || w[4] != 5)
	{
		return 7;
	}
	std::vector<int> x(5);
	::fast_io::algorithms::copy_backward(v, x.end());
	if (x[0] != 1 || x[4] != 5)
	{
		return 8;
	}

	// non-contiguous bidirectional iterators
	std::list<int> ls{1, 2, 3, 4};
	std::list<int> ld{0, 0, 0, 0};
	::fast_io::algorithms::copy_backward_iters(ls.begin(), ls.end(), ld.end());
	if (ld.front() != 1 || ld.back() != 4)
	{
		return 9;
	}
	std::list<int> lf{0, 0, 0, 0};
	::fast_io::algorithms::copy_iters(ls.begin(), ls.end(), lf.begin());
	if (lf.front() != 1 || lf.back() != 4)
	{
		return 10;
	}

	// non-pointer contiguous iterator source for uninitialized_copy
	std::vector<counted> cv;
	cv.reserve(4);
	cv.emplace_back(7);
	cv.emplace_back(8);
	cv.emplace_back(9);
	cv.emplace_back(10);
	alignas(counted) unsigned char cv_storage[sizeof(counted) * 4]{};
	auto *cv_base{reinterpret_cast<counted *>(cv_storage)};
	auto const cvr{::fast_io::algorithms::uninitialized_copy(cv, cv_base)};
	if (cvr != cv_base + 4 || cv_base[0].value != 7 || cv_base[3].value != 10)
	{
		return 11;
	}
	for (int i{}; i != 4; ++i)
	{
		cv_base[i].~counted();
	}

	// non_overlapped_copy with vector
	std::vector<int> nsrc{4, 5, 6};
	std::vector<int> ndst(3);
	::fast_io::algorithms::non_overlapped_copy(nsrc, ndst.begin());
	if (ndst[0] != 4 || ndst[2] != 6)
	{
		return 12;
	}

	// uninitialized_copy_backward with non-pointer iterators
	std::vector<counted> bv;
	bv.reserve(4);
	bv.emplace_back(21);
	bv.emplace_back(22);
	bv.emplace_back(23);
	bv.emplace_back(24);
	alignas(counted) unsigned char bv_storage[sizeof(counted) * 4]{};
	auto *bv_base{reinterpret_cast<counted *>(bv_storage)};
	auto const bvr{::fast_io::algorithms::uninitialized_copy_backward(bv, bv_base + 4)};
	if (bvr != bv_base || bv_base[0].value != 21 || bv_base[3].value != 24)
	{
		return 13;
	}
	for (int i{}; i != 4; ++i)
	{
		bv_base[i].~counted();
	}
	return 0;
}
