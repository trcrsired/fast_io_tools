#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <list>
#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_equal_test()
{
	int a[]{1, 2, 3};
	int b[]{1, 2, 3};
	int c[]{1, 2, 4};
	if (!::fast_io::algorithms::equal(a, b))
	{
		return false;
	}
	if (::fast_io::algorithms::equal(a, c))
	{
		return false;
	}
	return ::fast_io::algorithms::equal_index(a, 0, 3, b, 0, 3);
}

static_assert(constexpr_equal_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::equal(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::equal(
	::std::declval<std::vector<std::string> &>(),
	::std::declval<std::vector<std::string> &>())));

int main()
{
	std::vector<std::string> a{"a", "b", "c"};
	std::vector<std::string> b{"a", "b", "d"};
	if (::fast_io::algorithms::equal(a, b))
	{
		return 1;
	}
	if (::fast_io::algorithms::equal_index(a, 0, 2, b, 0, 2) != true)
	{
		return 2;
	}
	if (::fast_io::algorithms::equal_index(a, 0, 3, b, 0, 3) != false)
	{
		return 3;
	}
	return 0;
}
