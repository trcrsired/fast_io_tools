#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

constexpr bool constexpr_index_test()
{
	int a[8]{1, 2, 3, 4, 5, 6, 7, 8};
	int sum{};
	::fast_io::algorithms::for_each_index(
		a, 1, 8, [&](int &x) noexcept { sum += x; });
	return sum == 2 + 3 + 4 + 5 + 6 + 7 + 8;
}

static_assert(constexpr_index_test());

static_assert(noexcept(::fast_io::algorithms::for_each(
	::std::declval<::std::vector<int> &>(), [](int &) noexcept {})));
static_assert(!noexcept(::fast_io::algorithms::for_each(
	::std::declval<::std::vector<int> &>(), [](int &) {})));
static_assert(noexcept(::fast_io::algorithms::for_each_index(
	::std::declval<::std::vector<int> &>(), ::std::size_t{}, ::std::size_t{},
	[](int &) noexcept {})));
static_assert(!noexcept(::fast_io::algorithms::for_each_index(
	::std::declval<::std::vector<int> &>(), ::std::size_t{}, ::std::size_t{},
	[](int &) {})));

int main() {}
