#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <list>
#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_move_test()
{
	int a[]{1, 2, 3, 4};
	int b[4]{};
	int c[4]{};
	::fast_io::algorithms::move_iters(a + 1, a + 3, b);
	if (b[0] != 2 || b[1] != 3)
	{
		return false;
	}
	::fast_io::algorithms::move_backward_iters(a, a + 4, c + 4);
	return c[0] == 1 && c[1] == 2 && c[2] == 3 && c[3] == 4;
}

static_assert(constexpr_move_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::move(
	::std::declval<std::vector<int> &>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::move_backward(
	::std::declval<std::vector<int> &>(), ::std::declval<int *>())));

int main()
{
	std::vector<std::string> source{"alpha", "beta", "gamma"};
	std::vector<std::string> dest(3);
	::fast_io::algorithms::move(source, dest.begin());
	if (dest[0] != "alpha" || dest[1] != "beta" || dest[2] != "gamma")
	{
		return 1;
	}
	if (!source[0].empty() || !source[1].empty() || !source[2].empty())
	{
		return 2;
	}
	std::vector<std::string> backward(3);
	::fast_io::algorithms::move_backward(dest, backward.end());
	if (backward[0] != "alpha" || backward[1] != "beta" || backward[2] != "gamma")
	{
		return 3;
	}
	return 0;
}
