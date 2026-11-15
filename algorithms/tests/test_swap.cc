#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstdint>
#include <string>
#include <vector>

// Kept outside the anonymous namespace: a friend function defined in an
// internal-linkage class is itself internal and would be flagged as unused.
struct throwing_swap
{
	int value;

	friend void swap(throwing_swap &a, throwing_swap &b) throws(true)
	{
		int const temp{a.value};
		a.value = b.value;
		b.value = temp;
	}
};

namespace
{

// Movable and assignable, so the move-based exchange would compile, but the
// ADL swap must be preferred.
struct tracked
{
	int value;
	int *adl_calls;

	explicit tracked(int v, int *calls = nullptr) : value{v}, adl_calls{calls} {}

	friend constexpr void swap(tracked &a, tracked &b) throws(false)
	{
		int const temp{a.value};
		a.value = b.value;
		b.value = temp;
		if (a.adl_calls != nullptr)
		{
			++*a.adl_calls;
		}
		if (b.adl_calls != nullptr)
		{
			++*b.adl_calls;
		}
	}
};

// Only the ADL swap can exchange this type: the move operations are deleted.
struct adl_only
{
	int value;

	explicit constexpr adl_only(int v) : value{v} {}

	adl_only(adl_only const &) = delete;
	adl_only &operator=(adl_only const &) = delete;
	adl_only(adl_only &&) = delete;
	adl_only &operator=(adl_only &&) = delete;

	friend constexpr void swap(adl_only &a, adl_only &b) throws(false)
	{
		int const temp{a.value};
		a.value = b.value;
		b.value = temp;
	}
};

struct left
{
	int value;
};

struct right
{
	int value;
};

constexpr void swap(left &a, right &b) throws(false)
{
	int const temp{a.value};
	a.value = b.value;
	b.value = temp;
}

struct throwing_move
{
	int value{};

	throwing_move() = default;
	throwing_move(throwing_move &&) throws(true) {}
	throwing_move &operator=(throwing_move &&) throws(true)
	{
		return *this;
	}
};

constexpr bool constexpr_swap_test()
{
	int a{1};
	int b{2};
	::fast_io::algorithms::swap(a, b);
	if (a != 2 || b != 1)
	{
		return false;
	}

	int x[3]{1, 2, 3};
	int y[3]{4, 5, 6};
	::fast_io::algorithms::swap(x, y);
	if (x[0] != 4 || x[2] != 6 || y[0] != 1 || y[2] != 3)
	{
		return false;
	}

	int p[2][2]{{1, 2}, {3, 4}};
	int q[2][2]{{5, 6}, {7, 8}};
	::fast_io::algorithms::swap(p, q);
	if (p[0][0] != 5 || p[1][1] != 8 || q[0][0] != 1 || q[1][1] != 4)
	{
		return false;
	}

	adl_only c{3};
	adl_only d{4};
	::fast_io::algorithms::swap(c, d);
	if (c.value != 4 || d.value != 3)
	{
		return false;
	}

	left e{5};
	right f{6};
	::fast_io::algorithms::swap(e, f);
	if (e.value != 6 || f.value != 5)
	{
		return false;
	}

	int numbers[2]{10, 20};
	int *first{numbers};
	int *last{numbers + 1};
	::fast_io::algorithms::iter_swap(first, last);
	if (numbers[0] != 20 || numbers[1] != 10)
	{
		return false;
	}

	return true;
}

static_assert(constexpr_swap_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::swap(
	::std::declval<int &>(), ::std::declval<int &>())));
static_assert(noexcept(::fast_io::algorithms::swap(
	::std::declval<int (&)[4]>(), ::std::declval<int (&)[4]>())));
static_assert(noexcept(::fast_io::algorithms::swap(
	::std::declval<std::string &>(), ::std::declval<std::string &>())));
static_assert(noexcept(::fast_io::algorithms::swap(
	::std::declval<std::vector<int> &>(), ::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::swap(
	::std::declval<tracked &>(), ::std::declval<tracked &>())));
static_assert(noexcept(::fast_io::algorithms::swap(
	::std::declval<adl_only &>(), ::std::declval<adl_only &>())));
static_assert(noexcept(::fast_io::algorithms::swap(
	::std::declval<left &>(), ::std::declval<right &>())));
static_assert(!noexcept(::fast_io::algorithms::swap(
	::std::declval<throwing_swap &>(), ::std::declval<throwing_swap &>())));
static_assert(!noexcept(::fast_io::algorithms::swap(
	::std::declval<throwing_move &>(), ::std::declval<throwing_move &>())));

static_assert(noexcept(::fast_io::algorithms::iter_swap(
	::std::declval<int *>(), ::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::iter_swap(
	::std::declval<tracked *>(), ::std::declval<tracked *>())));
static_assert(!noexcept(::fast_io::algorithms::iter_swap(
	::std::declval<throwing_swap *>(), ::std::declval<throwing_swap *>())));

// The move-based exchange must reject rvalues, as `std::ranges::swap` does.
template <typename T>
concept rvalue_swappable = requires(T &&a, T &&b) {
	::fast_io::algorithms::swap(static_cast<T &&>(a), static_cast<T &&>(b));
};
static_assert(!rvalue_swappable<int>);

int main()
{
	// The ADL swap must win even though `tracked` is movable.
	{
		int calls{};
		tracked a{1, &calls};
		tracked b{2, &calls};
		::fast_io::algorithms::swap(a, b);
		if (a.value != 2 || b.value != 1 || calls != 2)
		{
			return 1;
		}
	}

	// iter_swap goes through the element type's own swap.
	{
		int calls{};
		tracked values[2]{tracked{1, &calls}, tracked{2, &calls}};
		::fast_io::algorithms::iter_swap(values, values + 1);
		if (values[0].value != 2 || values[1].value != 1 || calls != 2)
		{
			return 2;
		}
	}

	// Standard library types use std::swap through ADL.
	{
		std::string a{"abc"};
		std::string b{"de"};
		::fast_io::algorithms::swap(a, b);
		if (a != "de" || b != "abc")
		{
			return 3;
		}
	}
	{
		std::vector<int> a{1, 2, 3};
		std::vector<int> b{4, 5};
		::fast_io::algorithms::swap(a, b);
		if (a.size() != 2 || b.size() != 3 || a[0] != 4 || b[2] != 3)
		{
			return 4;
		}
	}

	return 0;
}
