#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstddef>
#include <iterator>

namespace user
{

struct iter
{
	using value_type = int;
	using difference_type = std::ptrdiff_t;
	using iterator_concept = std::random_access_iterator_tag;
	using iterator_category = std::random_access_iterator_tag;
	int *ptr{};

	inline constexpr int &operator*() const noexcept { return *ptr; }
	inline constexpr int *operator->() const noexcept { return ptr; }
	inline constexpr int &operator[](difference_type n) const noexcept
	{
		return ptr[n];
	}
	inline constexpr iter &operator++() noexcept
	{
		++ptr;
		return *this;
	}
	inline constexpr iter operator++(int) noexcept
	{
		auto copy{*this};
		++*this;
		return copy;
	}
	inline constexpr iter &operator--() noexcept
	{
		--ptr;
		return *this;
	}
	inline constexpr iter operator--(int) noexcept
	{
		auto copy{*this};
		--*this;
		return copy;
	}
	inline constexpr iter &operator+=(difference_type n) noexcept
	{
		ptr += n;
		return *this;
	}
	inline constexpr iter &operator-=(difference_type n) noexcept
	{
		ptr -= n;
		return *this;
	}
	inline constexpr bool operator==(iter const &) const noexcept = default;
	inline constexpr auto operator<=>(iter const &) const noexcept = default;
};

inline constexpr iter operator+(iter a, iter::difference_type n) noexcept
{
	a += n;
	return a;
}
inline constexpr iter operator+(iter::difference_type n, iter a) noexcept
{
	a += n;
	return a;
}
inline constexpr iter operator-(iter a, iter::difference_type n) noexcept
{
	a -= n;
	return a;
}
inline constexpr iter::difference_type operator-(
	iter a, iter b) noexcept
{
	return a.ptr - b.ptr;
}

struct range
{
	iter first;
	iter last;
	inline constexpr iter begin() const noexcept { return first; }
	inline constexpr iter end() const noexcept { return last; }
};

inline int copy_calls{};
inline int find_calls{};
inline int reduce_calls{};
inline int sort_calls{};
inline int nth_calls{};

// Customizations are ordinary overloads named after the public entry. The
// first parameter is the shared io_algorithm_t tag; the remaining parameters
// identify the iterator types, so ADL finds these in namespace user.

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter copy_iters_define(
	::fast_io::io_algorithm_t,
	Iter first, Snt last, OIter dest) noexcept
{
	++copy_calls;
	for (; first != last; ++first, ++dest)
	{
		*dest = *first + 100;
	}
	return dest;
}

template<typename Iter, typename Snt, typename T>
inline constexpr Iter find_iters_define(
	::fast_io::io_algorithm_t,
	Iter first, Snt last, T const &value) noexcept
{
	++find_calls;
	for (; first != last; ++first)
	{
		if (*first == value)
		{
			return first;
		}
	}
	return first;
}

template<typename Iter, typename Snt, typename T, typename Op>
inline constexpr T reduce_iters_define(
	::fast_io::io_algorithm_t,
	Iter first, Snt last, T init, Op op) noexcept
{
	++reduce_calls;
	return ::fast_io::details::algorithms::reduce_iters_impl(
		first.ptr, last.ptr, init, op);
}

template<typename Iter, typename Cmp>
inline constexpr void sort_iters_define(
	::fast_io::io_algorithm_t,
	Iter first, Iter last, Cmp cmp) noexcept
{
	++sort_calls;
	::fast_io::details::algorithms::sort_iters_impl(
		first.ptr, last.ptr, cmp);
}

template<typename Iter, typename Cmp>
inline constexpr void nth_element_iters_define(
	::fast_io::io_algorithm_t,
	Iter first, Iter nth, Iter last, Cmp cmp) noexcept
{
	++nth_calls;
	::fast_io::details::algorithms::nth_element_iters_impl(
		first.ptr, nth.ptr, last.ptr, cmp);
}

} // namespace user

int main()
{
	int source[]{3, 1, 4, 2};
	user::range r{user::iter{source}, user::iter{source + 4}};

	int out[4]{};
	auto end{::fast_io::algorithms::copy(r, out)};
	if (user::copy_calls != 1 || end != out + 4 ||
		out[0] != 103 || out[1] != 101 || out[2] != 104 || out[3] != 102)
	{
		return 1;
	}

	auto found{::fast_io::algorithms::find(r, 4)};
	if (user::find_calls != 1 || found.ptr != source + 2)
	{
		return 2;
	}

	::fast_io::algorithms::sort(r);
	if (user::sort_calls != 1 ||
		!(source[0] == 1 && source[1] == 2 && source[2] == 3 && source[3] == 4))
	{
		return 3;
	}

	int nth_source[]{5, 2, 4, 1, 3};
	::fast_io::algorithms::nth_element_iters(
		user::iter{nth_source}, user::iter{nth_source + 2},
		user::iter{nth_source + 5});
	if (user::nth_calls != 1 || nth_source[2] != 3)
	{
		return 4;
	}

	int reduce_source[]{1, 2, 3, 4};
	user::range rr{user::iter{reduce_source}, user::iter{reduce_source + 4}};
	if (::fast_io::algorithms::reduce(rr, 0) != 10 || user::reduce_calls != 1)
	{
		return 5;
	}
	return 0;
}
