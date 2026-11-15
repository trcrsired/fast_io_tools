#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

namespace
{

constexpr bool constexpr_adjacent_difference_test()
{
	int a[]{1, 4, 9, 16};
	int d[4]{};
	int *const end{::fast_io::algorithms::adjacent_difference_iters(a, a + 4, d)};
	if (end != d + 4)
	{
		return false;
	}
	if (d[0] != 1 || d[1] != 3 || d[2] != 5 || d[3] != 7)
	{
		return false;
	}
	int sum[4]{};
	::fast_io::algorithms::adjacent_difference(
		a, sum, ::fast_io::algorithms::field_ops::add{});
	if (sum[0] != 1 || sum[1] != 5 || sum[2] != 13 || sum[3] != 25)
	{
		return false;
	}
	// In-place is well-defined because the previous value is cached.
	int in_place[]{1, 4, 9, 16};
	::fast_io::algorithms::adjacent_difference_iters(
		in_place, in_place + 4, in_place);
	if (in_place[0] != 1 || in_place[1] != 3 || in_place[2] != 5 ||
		in_place[3] != 7)
	{
		return false;
	}
	int empty[1]{9};
	int *const empty_end{
		::fast_io::algorithms::adjacent_difference_iters(empty, empty, empty)};
	return empty_end == empty;
}

static_assert(constexpr_adjacent_difference_test());

static_assert(noexcept(::fast_io::algorithms::adjacent_difference(
	::std::declval<int (&)[4]>(), ::std::declval<int *>())));

} // namespace

int main()
{
	std::vector<int> v{1, 4, 9, 16, 25};
	int out[3]{};
	int *const end{::fast_io::algorithms::adjacent_difference_index_range(
		v, 1, 4, out)};
	if (end != out + 3)
	{
		return 1;
	}
	if (out[0] != 4 || out[1] != 5 || out[2] != 7)
	{
		return 2;
	}
	std::vector<int> dest(v.size());
	if (::fast_io::algorithms::adjacent_difference(v, dest.data()) !=
		dest.data() + 5)
	{
		return 3;
	}
	if (dest[0] != 1 || dest[1] != 3 || dest[2] != 5 || dest[3] != 7 ||
		dest[4] != 9)
	{
		return 4;
	}
	return 0;
}
