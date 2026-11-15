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
	if (!caught)
	{
		return 3;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::fill_index(d, 0, 2, 7);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 4;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::sort_index(d, 0, 2);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 5;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::sort_heap_index(d, 0, 2);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 6;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::destroy_index(d, 0, 2);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 7;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::uninitialized_fill_index(d, 0, 2, 7);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 8;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::uninitialized_value_construct_index(d, 0, 2);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 9;
	}

	throwing_deque populated{};
	for (int i{}; i != 5; ++i)
	{
		populated.data.push_back(i);
	}

	// middle before first
	caught = false;
	try
	{
		::fast_io::algorithms::rotate_index(populated, 1, 0, 4);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 10;
	}

	// middle after last
	caught = false;
	try
	{
		::fast_io::algorithms::rotate_index(populated, 1, 5, 4);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 11;
	}

	// A valid rotation must not report a violation.
	caught = false;
	try
	{
		auto const result{
			::fast_io::algorithms::rotate_index(populated, 1, 3, 4)};
		if (result != 2)
		{
			return 12;
		}
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (caught)
	{
		return 13;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::remove_index(d, 0, 2, 7);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 14;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::lexicographical_compare_index(
			d, 0, 2, populated, 0, 1);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 15;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::lexicographical_compare_index(
			populated, 0, 1, d, 0, 2);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 16;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::all_of_index(
			d, 0, 2, [](int) noexcept { return true; });
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 17;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::any_of_index(
			d, 0, 2, [](int) noexcept { return false; });
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 18;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::none_of_index(
			d, 0, 2, [](int) noexcept { return false; });
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 19;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::count_index(d, 0, 2, 1);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 20;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::count_if_index(
			d, 0, 2, [](int) noexcept { return true; });
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 21;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::mismatch_index(
			d, 0, 2, populated, 0, 1);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 22;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::find_if_index(
			d, 0, 2, [](int) noexcept { return false; });
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 23;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::find_if_not_index(
			d, 0, 2, [](int) noexcept { return true; });
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 24;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::adjacent_find_index(d, 0, 2);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 25;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::find_end_index(
			d, 0, 2, populated, 0, 1);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 26;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::find_first_of_index(
			d, 0, 2, populated, 0, 1);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 27;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::search_index(
			d, 0, 2, populated, 0, 1);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 28;
	}

	caught = false;
	try
	{
		::fast_io::algorithms::search_n_index(d, 0, 2, 1, 1);
	}
	catch throws(::std::error ec)
	{
		(void)ec;
		caught = true;
	}
	if (!caught)
	{
		return 29;
	}

	return 0;
}
