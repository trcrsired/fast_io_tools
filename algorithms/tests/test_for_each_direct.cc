#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../for_each.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <vector>

int main()
{
	int a[8]{};
	::fast_io::algorithms::for_each(a, [](int &x) { ++x; });
	::fast_io::algorithms::for_each_iters(a, a + 8, [](int &x) { ++x; });
	std::vector<int> v(8);
	::fast_io::algorithms::for_each(v, [](int &x) { ++x; });
	::fast_io::algorithms::for_each_iters(v.begin(), v.end(), [](int &x) { ++x; });
	::fast_io::algorithms::for_each_iters(v.data(), v.data() + v.size(), [](int &x) { ++x; });
}
