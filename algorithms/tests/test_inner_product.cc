#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

namespace
{

struct max_op
{
	inline constexpr int compute(int a, int b) const noexcept
	{
		return a < b ? b : a;
	}
};

struct plus_one_multiply
{
	inline constexpr int compute(int a, int b) const noexcept
	{
		return (a + 1) * b;
	}
};

constexpr bool constexpr_inner_product_test()
{
	int a[]{1, 2, 3};
	int b[]{4, 5, 6};
	if (::fast_io::algorithms::inner_product(a, b, 0) != 32)
	{
		return false;
	}
	if (::fast_io::algorithms::inner_product_iters(a, a + 3, b, b + 3, 1) !=
		33)
	{
		return false;
	}
	// The scan stops at whichever range ends first.
	int short_b[]{4, 5};
	if (::fast_io::algorithms::inner_product_iters(
			a, a + 3, short_b, short_b + 2, 0) != 14)
	{
		return false;
	}
	// Custom combine and multiply operations.
	if (::fast_io::algorithms::inner_product(
			a, b, 0, max_op{},
			::fast_io::algorithms::field_ops::multiply{}) != 18)
	{
		return false;
	}
	if (::fast_io::algorithms::inner_product(
			a, b, 0, ::fast_io::algorithms::field_ops::add{},
			plus_one_multiply{}) != 8 + 15 + 24)
	{
		return false;
	}
	int const indices{::fast_io::algorithms::inner_product_index_range(
		a, 1, 3, b, 1, 3, 0)};
	return indices == 2 * 5 + 3 * 6;
}

static_assert(constexpr_inner_product_test());

} // namespace

int main()
{
	std::vector<int> a{1, 2, 3};
	std::vector<int> b{4, 5, 6};
	if (::fast_io::algorithms::inner_product(a, b, 0) != 32)
	{
		return 1;
	}
	if (::fast_io::algorithms::inner_product(
			a, b, 0, max_op{},
			::fast_io::algorithms::field_ops::multiply{}) != 18)
	{
		return 2;
	}
	if (::fast_io::algorithms::inner_product_index_range(
			a, 0, 2, b, 0, 2, 0) != 14)
	{
		return 3;
	}
	std::vector<int> empty;
	if (::fast_io::algorithms::inner_product(empty, b, 9) != 9)
	{
		return 4;
	}
	return 0;
}
