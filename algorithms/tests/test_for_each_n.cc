#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstdint>
#include <vector>

namespace
{

constexpr bool constexpr_test()
{
	int a[6]{1, 2, 3, 4, 5, 6};
	auto const result{::fast_io::algorithms::for_each_n_iters(
		a, 4, [](int &value) constexpr noexcept { value *= 2; })};
	if (result != a + 4)
	{
		return false;
	}
	int const expected[]{2, 4, 6, 8, 5, 6};
	for (int i{}; i != 6; ++i)
	{
		if (a[i] != expected[i])
		{
			return false;
		}
	}
	auto const empty{::fast_io::algorithms::for_each_n_iters(
		a, 0, [](int &) constexpr noexcept {})};
	return empty == a;
}

static_assert(constexpr_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::for_each_n(
	::std::declval<std::vector<int> &>(), std::size_t{},
	[](int &) noexcept {})));
static_assert(noexcept(::fast_io::algorithms::for_each_n_iters(
	::std::declval<int *>(), std::size_t{}, [](int &) noexcept {})));

int main()
{
	std::vector<int> values(8);
	auto const result{::fast_io::algorithms::for_each_n(
		values, 5, [](int &value) noexcept { value = 7; })};
	if (result != values.begin() + 5)
	{
		return 1;
	}
	for (std::size_t i{}; i != values.size(); ++i)
	{
		if (values[i] != (i < 5 ? 7 : 0))
		{
			return 2;
		}
	}

	auto const pointer_result{::fast_io::algorithms::for_each_n_iters(
		values.data(), 8, [](int &value) noexcept { ++value; })};
	if (pointer_result != values.data() + 8)
	{
		return 3;
	}
	for (std::size_t i{}; i != values.size(); ++i)
	{
		if (values[i] != (i < 5 ? 8 : 1))
		{
			return 4;
		}
	}
	return 0;
}
