#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <deque>

struct throwing_deque
{
	static inline constexpr ::fast_io::allocator_adapter_flags allocator_flags{
		::fast_io::allocator_adapter_flags::throws_on_violations};
	::std::deque<int> data;

	auto begin() noexcept { return data.begin(); }
	auto end() noexcept { return data.end(); }
	::std::size_t size() const noexcept { return data.size(); }
};

int main()
{
	throwing_deque d{};
	bool caught{};

	try
	{
		::fast_io::algorithms::for_each_index(d, 0, 1, [](int &) noexcept {});
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 2;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::reverse_index(d, 0, 2);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	return caught ? 0 : 3;
}
