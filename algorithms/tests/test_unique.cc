#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_unique_test()
{
	int a[]{1, 1, 2, 2, 2, 3, 1};
	auto end{::fast_io::algorithms::unique(a)};
	if (end != a + 4 || a[0] != 1 || a[1] != 2 || a[2] != 3 || a[3] != 1)
	{
		return false;
	}
	int b[7]{};
	auto copy_end{::fast_io::algorithms::unique_copy_iters(a, end, b)};
	return copy_end == b + 4 && b[3] == 1;
}

static_assert(constexpr_unique_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::unique(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::unique_copy(
	::std::declval<std::vector<int> &>(),
	::std::declval<int *>())));

int main()
{
	std::vector<std::string> values{"a", "a", "b", "b", "c", "a"};
	auto end{::fast_io::algorithms::unique_index(values, 0, 6)};
	if (end != 4)
	{
		return 1;
	}
	if (values[0] != "a" || values[1] != "b" || values[2] != "c" ||
		values[3] != "a")
	{
		return 2;
	}
	std::vector<std::string> out(6);
	auto copy_end{::fast_io::algorithms::unique_copy_iters(values.begin(), values.begin() + static_cast<std::ptrdiff_t>(end), out.begin())};
	if (copy_end != out.begin() + 4 || out[3] != "a")
	{
		return 3;
	}
	return 0;
}
