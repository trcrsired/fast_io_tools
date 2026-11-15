#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

namespace
{

struct add_one
{
	inline constexpr int operator()(int value) const noexcept
	{
		return value + 1;
	}
};

struct add
{
	inline constexpr int operator()(int a, int b) const noexcept
	{
		return a + b;
	}
};

constexpr bool constexpr_transform_test()
{
	int a[]{1, 2, 3};
	int b[3]{};
	int c[3]{};
	::fast_io::algorithms::transform(a, b, add_one{});
	::fast_io::algorithms::transform_index(a, 1, 3, c + 1, add_one{});
	if (b[0] != 2 || b[1] != 3 || b[2] != 4)
	{
		return false;
	}
	if (c[0] != 0 || c[1] != 3 || c[2] != 4)
	{
		return false;
	}
	::fast_io::algorithms::transform_binary(a, b, c, add{});
	return c[0] == 3 && c[1] == 5 && c[2] == 7;
}

static_assert(constexpr_transform_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::transform(
	::std::declval<std::vector<int> &>(), ::std::declval<int *>(),
	add_one{})));
static_assert(noexcept(::fast_io::algorithms::transform_binary(
	::std::declval<std::vector<int> &>(),
	::std::declval<std::vector<int> &>(), ::std::declval<int *>(),
	add{})));

int main()
{
	std::vector<int> a{1, 2, 3, 4};
	std::vector<int> b{10, 20, 30, 40};
	std::vector<int> out(4);
	auto end{::fast_io::algorithms::transform_binary_index(
		a, 1, 4, b, 1, 4, out.begin(), add{})};
	if (end != out.begin() + 3)
	{
		return 1;
	}
	if (out[0] != 22 || out[1] != 33 || out[2] != 44)
	{
		return 2;
	}
	return 0;
}
