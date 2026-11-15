#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

namespace
{

struct max_abs
{
	inline constexpr int compute(int a, int b) const noexcept
	{
		int const abs_a{a < 0 ? -a : a};
		int const abs_b{b < 0 ? -b : b};
		return abs_a < abs_b ? b : a;
	}
};

constexpr bool constexpr_reduce_test()
{
	int a[]{1, 2, 3, 4};
	if (::fast_io::algorithms::reduce(a, 0) != 10)
	{
		return false;
	}
	if (::fast_io::algorithms::reduce(
			a, 1, ::fast_io::algorithms::field_ops::multiply{}) != 24)
	{
		return false;
	}
	if (::fast_io::algorithms::reduce_iters(a, a + 4, 0) != 10)
	{
		return false;
	}
	if (::fast_io::algorithms::accumulate(a, 0) != 10)
	{
		return false;
	}
	if (::fast_io::algorithms::accumulate_iters(
			a, a + 4, 1, ::fast_io::algorithms::field_ops::multiply{}) != 24)
	{
		return false;
	}
	if (::fast_io::algorithms::reduce_index_range(a, 1, 3, 0) != 5)
	{
		return false;
	}
	if (::fast_io::algorithms::accumulate_index_range(a, 1, 3, 0) != 5)
	{
		return false;
	}
	// An empty range returns the initial value.
	if (::fast_io::algorithms::reduce_iters(a, a, 7) != 7)
	{
		return false;
	}
	int b[]{-5, 3, -9, 2};
	return ::fast_io::algorithms::reduce(b, 0, max_abs{}) == -9;
}

static_assert(constexpr_reduce_test());

static_assert(noexcept(::fast_io::algorithms::reduce(
	::std::declval<int (&)[4]>(), 0)));
static_assert(noexcept(::fast_io::algorithms::accumulate(
	::std::declval<int (&)[4]>(), 0,
	::fast_io::algorithms::field_ops::multiply{})));

} // namespace

int main()
{
	std::vector<int> v{1, 2, 3, 4, 5};
	if (::fast_io::algorithms::reduce(v, 0) != 15)
	{
		return 1;
	}
	if (::fast_io::algorithms::accumulate(v, 0) != 15)
	{
		return 2;
	}
	if (::fast_io::algorithms::reduce(
			v, 1, ::fast_io::algorithms::field_ops::multiply{}) != 120)
	{
		return 3;
	}
	if (::fast_io::algorithms::reduce_index_range(v, 1, 4, 0) != 9)
	{
		return 4;
	}
	if (::fast_io::algorithms::accumulate_index_range(v, 1, 4, 0) != 9)
	{
		return 5;
	}
	if (::fast_io::algorithms::reduce(v, 0, max_abs{}) != 5)
	{
		return 6;
	}
	std::vector<int> empty;
	if (::fast_io::algorithms::reduce(empty, 42) != 42)
	{
		return 7;
	}
	return 0;
}
