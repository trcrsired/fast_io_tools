#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <string>
#include <vector>

namespace
{

constexpr bool constexpr_index_of_test()
{
	int sorted[]{1, 2, 2, 2, 3, 5, 8};
	if (::fast_io::algorithms::lower_bound_index_of(sorted, 2) != 1 ||
		::fast_io::algorithms::upper_bound_index_of(sorted, 2) != 4 ||
		::fast_io::algorithms::lower_bound_index_of(sorted, 9) != 7 ||
		::fast_io::algorithms::upper_bound_index_of(sorted, 0) != 0)
	{
		return false;
	}
	auto const same{::fast_io::algorithms::equal_range_index_of(sorted, 2)};
	if (same.first != 1 || same.second != 4)
	{
		return false;
	}
	auto const absent{::fast_io::algorithms::equal_range_index_of(sorted, 4)};
	if (absent.first != 5 || absent.second != 5)
	{
		return false;
	}

	int text[]{3, 1, 4, 1, 5, 9, 2, 6};
	if (::fast_io::algorithms::find_index_of(text, 5) != 4 ||
		::fast_io::algorithms::find_index_of(text, 7) != 8 ||
		::fast_io::algorithms::find_if_index_of(
			text, [](int value) constexpr noexcept { return value > 4; }) != 4 ||
		::fast_io::algorithms::find_if_not_index_of(
			text, [](int value) constexpr noexcept { return value <= 4; }) != 4 ||
		::fast_io::algorithms::find_if_index_of(
			text, [](int value) constexpr noexcept { return value > 9; }) != 8)
	{
		return false;
	}

	int hay[]{1, 2, 3, 4, 3, 4, 5};
	int needle[]{3, 4};
	int single[]{4};
	if (::fast_io::algorithms::search_index_of(hay, needle) != 2 ||
		::fast_io::algorithms::find_end_index_of(hay, needle) != 4 ||
		::fast_io::algorithms::find_first_of_index_of(hay, needle) != 2 ||
		::fast_io::algorithms::find_first_of_index_of(hay, single) != 3)
	{
		return false;
	}
	int duplicate[]{1, 2, 2, 3};
	if (::fast_io::algorithms::adjacent_find_index_of(duplicate) != 1 ||
		::fast_io::algorithms::adjacent_find_index_of(hay) != 7)
	{
		return false;
	}
	int runs[]{1, 7, 7, 7, 3};
	if (::fast_io::algorithms::search_n_index_of(runs, 3, 7) != 1 ||
		::fast_io::algorithms::search_n_index_of(runs, 4, 7) != 5)
	{
		return false;
	}

	int vals[]{4, 1, 7, 1, 7, 2};
	if (::fast_io::algorithms::min_element_index_of(vals) != 1 ||
		::fast_io::algorithms::max_element_index_of(vals) != 2)
	{
		return false;
	}
	auto const minmax{::fast_io::algorithms::minmax_element_index_of(vals)};
	if (minmax.first != 1 || minmax.second != 4)
	{
		return false;
	}

	int broken_heap[]{9, 7, 5, 8};
	int good_heap[]{9, 7, 5};
	if (::fast_io::algorithms::is_heap_until_index_of(broken_heap) != 3 ||
		::fast_io::algorithms::is_heap_until_index_of(good_heap) != 3)
	{
		return false;
	}

	int sortedish[]{1, 2, 2, 3, 1};
	if (::fast_io::algorithms::is_sorted_until_index_of(sortedish) != 4 ||
		::fast_io::algorithms::is_sorted_until_index_of(sorted) != 7)
	{
		return false;
	}

	auto const even{[](int value) constexpr noexcept { return value % 2 == 0; }};
	int part[]{1, 2, 3, 4, 5, 6};
	if (::fast_io::algorithms::partition_index_of(part, even) != 3)
	{
		return false;
	}
	int stable[]{1, 2, 3, 4, 5, 6};
	if (::fast_io::algorithms::stable_partition_index_of(stable, even) != 3 ||
		stable[0] != 2 || stable[1] != 4 || stable[2] != 6)
	{
		return false;
	}
	int partitioned[]{2, 4, 6, 1, 3, 5};
	if (::fast_io::algorithms::partition_point_index_of(partitioned, even) != 3)
	{
		return false;
	}

	int removed[]{1, 2, 1, 3};
	if (::fast_io::algorithms::remove_index_of(removed, 1) != 2)
	{
		return false;
	}
	int removed_if[]{1, 2, 3, 4};
	if (::fast_io::algorithms::remove_if_index_of(removed_if, even) != 2)
	{
		return false;
	}

	int duplicates[]{1, 1, 2, 2, 2, 3};
	if (::fast_io::algorithms::unique_index_of(duplicates) != 3)
	{
		return false;
	}

	int rotated[]{1, 2, 3, 4, 5};
	if (::fast_io::algorithms::rotate_index_of(rotated, rotated + 2) != 3 ||
		rotated[0] != 3 || rotated[3] != 1)
	{
		return false;
	}

	int shifted[]{1, 2, 3, 4, 5};
	if (::fast_io::algorithms::shift_left_index_of(shifted, 2) != 3 ||
		shifted[0] != 3)
	{
		return false;
	}
	int shifted_right[]{1, 2, 3, 4, 5};
	if (::fast_io::algorithms::shift_right_index_of(shifted_right, 2) != 2 ||
		shifted_right[2] != 1)
	{
		return false;
	}
	return true;
}

static_assert(constexpr_index_of_test());

static_assert(noexcept(::fast_io::algorithms::find_index_of(
	::std::declval<std::vector<int> &>(), 1)));
static_assert(noexcept(::fast_io::algorithms::lower_bound_index_of(
	::std::declval<std::vector<int> &>(), 1)));

} // namespace

int main()
{
	std::vector<int> empty;
	if (::fast_io::algorithms::find_index_of(empty, 1) != 0 ||
		::fast_io::algorithms::min_element_index_of(empty) != 0 ||
		::fast_io::algorithms::search_n_index_of(empty, 1, 2) != 0)
	{
		return 1;
	}

	std::vector<int> sorted_values{1, 3, 3, 3, 7};
	if (::fast_io::algorithms::lower_bound_index_of(sorted_values, 3) != 1 ||
		::fast_io::algorithms::upper_bound_index_of(sorted_values, 3) != 4 ||
		::fast_io::algorithms::find_index_of(sorted_values, 7) != 4)
	{
		return 2;
	}

	std::vector<std::string> words{"pear", "apple", "zebra"};
	if (::fast_io::algorithms::find_index_of(words, std::string{"zebra"}) != 2 ||
		::fast_io::algorithms::find_if_index_of(
			words, [](std::string const &value) noexcept {
				return value == std::string{"apple"};
			}) != 1)
	{
		return 3;
	}

	return 0;
}
