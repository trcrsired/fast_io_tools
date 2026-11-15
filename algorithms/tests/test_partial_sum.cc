#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

namespace
{

constexpr bool constexpr_partial_sum_test()
{
	int a[]{1, 2, 3, 4};
	int d[4]{};
	int *const end{::fast_io::algorithms::partial_sum_iters(a, a + 4, d)};
	if (end != d + 4)
	{
		return false;
	}
	if (d[0] != 1 || d[1] != 3 || d[2] != 6 || d[3] != 10)
	{
		return false;
	}
	int product[4]{};
	::fast_io::algorithms::partial_sum(
		a, product, ::fast_io::algorithms::field_ops::multiply{});
	if (product[0] != 1 || product[1] != 2 || product[2] != 6 ||
		product[3] != 24)
	{
		return false;
	}
	// In-place is well-defined because the running value is cached before the
	// write and the write lags one element behind the read.
	int in_place[]{1, 2, 3, 4};
	::fast_io::algorithms::partial_sum_iters(
		in_place, in_place + 4, in_place);
	if (in_place[0] != 1 || in_place[1] != 3 || in_place[2] != 6 ||
		in_place[3] != 10)
	{
		return false;
	}
	int empty[1]{9};
	int *const empty_end{
		::fast_io::algorithms::partial_sum_iters(empty, empty, empty)};
	return empty_end == empty;
}

static_assert(constexpr_partial_sum_test());

static_assert(noexcept(::fast_io::algorithms::partial_sum(
	::std::declval<int (&)[4]>(), ::std::declval<int *>())));

} // namespace

int main()
{
	std::vector<int> v{1, 2, 3, 4, 5};
	int out[3]{};
	int *const end{::fast_io::algorithms::partial_sum_index_range(
		v, 1, 4, out)};
	if (end != out + 3)
	{
		return 1;
	}
	if (out[0] != 2 || out[1] != 5 || out[2] != 9)
	{
		return 2;
	}
	std::vector<int> dest(v.size());
	if (::fast_io::algorithms::partial_sum(v, dest.data()) != dest.data() + 5)
	{
		return 3;
	}
	if (dest[0] != 1 || dest[1] != 3 || dest[2] != 6 || dest[3] != 10 ||
		dest[4] != 15)
	{
		return 4;
	}
	return 0;
}
