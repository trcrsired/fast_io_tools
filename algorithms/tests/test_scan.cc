#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

namespace
{

namespace ops = ::fast_io::algorithms::field_ops;

constexpr bool constexpr_scan_test()
{
	int a[]{3, 1, 4, 1, 5};

	int inclusive[5]{};
	int *const inclusive_end{
		::fast_io::algorithms::inclusive_scan_iters(a, a + 5, inclusive)};
	if (inclusive_end != inclusive + 5)
	{
		return false;
	}
	if (inclusive[0] != 3 || inclusive[1] != 4 || inclusive[2] != 8 ||
		inclusive[3] != 9 || inclusive[4] != 14)
	{
		return false;
	}

	// Inclusive scan and partial_sum compute the same values.
	int partial[5]{};
	::fast_io::algorithms::partial_sum_iters(a, a + 5, partial);
	for (int i{}; i != 5; ++i)
	{
		if (inclusive[i] != partial[i])
		{
			return false;
		}
	}

	int exclusive[5]{};
	int *const exclusive_end{::fast_io::algorithms::exclusive_scan_iters(
		a, a + 5, exclusive, 100)};
	if (exclusive_end != exclusive + 5)
	{
		return false;
	}
	if (exclusive[0] != 100 || exclusive[1] != 103 || exclusive[2] != 104 ||
		exclusive[3] != 108 || exclusive[4] != 109)
	{
		return false;
	}

	// A single element produces only the initial value.
	int one[]{3};
	int single[1]{};
	int *const single_end{
		::fast_io::algorithms::exclusive_scan_iters(one, one + 1, single, 100)};
	if (single_end != single + 1 || single[0] != 100)
	{
		return false;
	}

	// Empty input writes nothing.
	int empty_out[1]{9};
	int *const empty_end{
		::fast_io::algorithms::exclusive_scan_iters(one, one, empty_out, 100)};
	if (empty_end != empty_out || empty_out[0] != 9)
	{
		return false;
	}

	// A non-additive operation.
	int three[]{3, 4, 5};
	int product[3]{};
	::fast_io::algorithms::exclusive_scan_iters(
		three, three + 3, product, 2, ops::multiply{});
	return product[0] == 2 && product[1] == 6 && product[2] == 24;
}

static_assert(constexpr_scan_test());

static_assert(noexcept(::fast_io::algorithms::inclusive_scan(
	::std::declval<int (&)[3]>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::exclusive_scan(
	::std::declval<int (&)[3]>(), ::std::declval<int *>(), 0)));

} // namespace

int main()
{
	std::vector<int> v{3, 1, 4, 1, 5};

	int inclusive[3]{};
	int *const inclusive_end{
		::fast_io::algorithms::inclusive_scan_index_range(v, 1, 4, inclusive)};
	if (inclusive_end != inclusive + 3)
	{
		return 1;
	}
	if (inclusive[0] != 1 || inclusive[1] != 5 || inclusive[2] != 6)
	{
		return 2;
	}

	int exclusive[3]{};
	int *const exclusive_end{
		::fast_io::algorithms::exclusive_scan_index_range(
			v, 1, 4, exclusive, 10)};
	if (exclusive_end != exclusive + 3)
	{
		return 3;
	}
	if (exclusive[0] != 10 || exclusive[1] != 11 || exclusive[2] != 15)
	{
		return 4;
	}

	std::vector<int> dest(v.size());
	if (::fast_io::algorithms::inclusive_scan(v, dest.data()) !=
		dest.data() + 5)
	{
		return 5;
	}
	if (dest[0] != 3 || dest[1] != 4 || dest[2] != 8 || dest[3] != 9 ||
		dest[4] != 14)
	{
		return 6;
	}
	return 0;
}
