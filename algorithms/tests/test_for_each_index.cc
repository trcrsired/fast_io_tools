#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <deque>
#include <vector>

struct throwing_deque
{
	static inline constexpr ::fast_io::allocator_adapter_flags allocator_flags{
		::fast_io::allocator_adapter_flags::throws_on_violations};
	::std::deque<int> data;

	auto begin() noexcept { return data.begin(); }
	auto end() noexcept { return data.end(); }
	auto begin() const noexcept { return data.begin(); }
	auto end() const noexcept { return data.end(); }
	::std::size_t size() const noexcept { return data.size(); }
};

int main()
{
	int a[8]{};
	::fast_io::algorithms::for_each_index(a, 0, 8, [](int &x) { ++x; });
	::fast_io::algorithms::for_each_index(a, 1, 3, [](int &x) { ++x; });
	::fast_io::algorithms::for_each_index(a, 4, 4, [](int &x) { ++x; });

	::std::vector<int> v(8);
	::fast_io::algorithms::for_each_index(v, 0, 8, [](int &x) { ++x; });
	::fast_io::algorithms::for_each_index(v, 2, 5, [](int &x) { ++x; });

	throwing_deque d{.data = {1, 2, 3, 4}};
	::fast_io::algorithms::for_each_index(d, 1, 3, [](int &x) { ++x; });
}

static_assert(noexcept(::fast_io::algorithms::for_each_index(
	::std::declval<int (&)[8]>(), ::std::size_t{}, ::std::size_t{},
	[](int &) noexcept {})));
static_assert(!noexcept(::fast_io::algorithms::for_each_index(
	::std::declval<int (&)[8]>(), ::std::size_t{}, ::std::size_t{},
	[](int &) {})));
static_assert(!noexcept(::fast_io::algorithms::for_each_index(
	::std::declval<throwing_deque &>(), ::std::size_t{}, ::std::size_t{},
	[](int &) noexcept {})));
