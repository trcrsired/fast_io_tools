#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

namespace
{

struct counter
{
	int value{};
	inline constexpr int operator()() noexcept
	{
		return value++;
	}
};

constexpr bool constexpr_generate_test()
{
	int a[5]{};
	counter c{};
	::fast_io::algorithms::generate(a, c);
	if (a[0] != 0 || a[4] != 4)
	{
		return false;
	}
	counter d{10};
	auto end{::fast_io::algorithms::generate_n_iters(a + 1, 2, d)};
	return end == a + 3 && a[1] == 10 && a[2] == 11 && a[3] == 3;
}

static_assert(constexpr_generate_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::generate(
	::std::declval<std::vector<int> &>(), counter{})));
static_assert(noexcept(::fast_io::algorithms::generate_n_iters(
	::std::declval<int *>(), std::size_t{}, counter{})));

int main()
{
	std::vector<int> values(4);
	counter c{7};
	::fast_io::algorithms::generate_index_range(values, 1, 3, c);
	if (values[0] != 0 || values[1] != 7 || values[2] != 8 || values[3] != 0)
	{
		return 1;
	}
	auto end{::fast_io::algorithms::generate_n(values, 2, counter{3})};
	if (end != values.begin() + 2 || values[0] != 3 || values[1] != 4)
	{
		return 2;
	}
	return 0;
}
