#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_replace_test()
{
	int a[]{1, 2, 1, 3};
	int b[4]{};
	::fast_io::algorithms::replace(a, 1, 5);
	if (a[0] != 5 || a[1] != 2 || a[2] != 5 || a[3] != 3)
	{
		return false;
	}
	::fast_io::algorithms::replace_if(a, [](int value) noexcept { return value > 3; }, 9);
	if (a[0] != 9 || a[1] != 2 || a[2] != 9 || a[3] != 3)
	{
		return false;
	}
	::fast_io::algorithms::replace_copy(a, b, 9, 0);
	if (b[0] != 0 || b[1] != 2 || b[2] != 0 || b[3] != 3)
	{
		return false;
	}
	int c[4]{};
	::fast_io::algorithms::replace_copy_if(b, c, [](int value) noexcept { return value == 2; }, 4);
	return c[1] == 4;
}

static_assert(constexpr_replace_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::replace(
	::std::declval<std::vector<int> &>(), 1, 2)));
static_assert(noexcept(::fast_io::algorithms::replace_if(
	::std::declval<std::vector<int> &>(),
	[](int value) noexcept { return value == 1; }, 2)));
static_assert(noexcept(::fast_io::algorithms::replace_copy(
	::std::declval<std::vector<int> &>(),
	::std::declval<int *>(), 1, 2)));

int main()
{
	std::vector<std::string> values{"a", "b", "a"};
	::fast_io::algorithms::replace_index_range(values, 0, 3, std::string{"a"}, std::string{"z"});
	if (values[0] != "z" || values[1] != "b" || values[2] != "z")
	{
		return 1;
	}
	std::vector<std::string> out(3);
	auto end{::fast_io::algorithms::replace_copy(values, out.begin(),
		std::string{"z"}, std::string{"a"})};
	if (end != out.end() || out[0] != "a" || out[2] != "a")
	{
		return 2;
	}
	return 0;
}
