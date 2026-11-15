#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstddef>
#include <new>
#include <string>
#include <vector>

namespace
{

template <typename T, std::size_t N>
struct raw_storage
{
	alignas(T) std::byte bytes[N * sizeof(T)];

	T *begin() noexcept { return reinterpret_cast<T *>(bytes); }
	T *end() noexcept { return begin() + N; }
	std::size_t size() const noexcept { return N; }
};

struct counted
{
	static inline int live{};
	int value;
	counted() noexcept : value{7} { ++live; }
	~counted() noexcept { --live; }
};

constexpr bool constexpr_destroy_at_test()
{
	int value{5};
	::fast_io::algorithms::destroy_at(&value);
	::new (static_cast<void *>(&value)) int{6};
	return value == 6;
}

static_assert(constexpr_destroy_at_test());

constexpr bool constexpr_destroy_test()
{
	int a[4]{1, 2, 3, 4};
	auto const result{::fast_io::algorithms::destroy(a)};
	if (result != a + 4)
	{
		return false;
	}
	for (std::size_t i{}; i != 4; ++i)
	{
		::new (static_cast<void *>(a + i)) int{static_cast<int>(i)};
	}
	return a[0] == 0 && a[3] == 3;
}

static_assert(constexpr_destroy_test());

constexpr bool constexpr_destroy_n_test()
{
	int a[3]{1, 2, 3};
	auto const result{::fast_io::algorithms::destroy_n(a, 2)};
	if (result != a + 2)
	{
		return false;
	}
	::new (static_cast<void *>(a)) int{9};
	return a[0] == 9;
}

static_assert(constexpr_destroy_n_test());

constexpr bool constexpr_destroy_index_range_test()
{
	int a[5]{1, 2, 3, 4, 5};
	::fast_io::algorithms::destroy_index_range(a, 1, 4);
	for (int i{1}; i < 4; ++i)
	{
		::new (static_cast<void *>(a + i)) int{i};
	}
	return a[1] == 1 && a[3] == 3;
}

static_assert(constexpr_destroy_index_range_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::destroy_at(
	::std::declval<int *>())));
static_assert(noexcept(::fast_io::algorithms::destroy(
	::std::declval<int (&)[4]>())));
static_assert(noexcept(::fast_io::algorithms::destroy(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::destroy_n_iters(
	::std::declval<int *>(), std::size_t{})));
static_assert(noexcept(::fast_io::algorithms::destroy_index_range(
	::std::declval<std::vector<int> &>(), std::size_t{}, std::size_t{})));

int main()
{
	{
		int a[4]{};
		if (::fast_io::algorithms::destroy_iters(a, a + 4) != a + 4)
		{
			return 1;
		}
		if (::fast_io::algorithms::destroy_n_iters(a, 4) != a + 4)
		{
			return 2;
		}
	}

	{
		raw_storage<counted, 1> storage;
		::new (static_cast<void *>(storage.begin())) counted;
		if (counted::live != 1)
		{
			return 3;
		}
		::fast_io::algorithms::destroy_at(storage.begin());
		if (counted::live != 0)
		{
			return 4;
		}
	}

	{
		raw_storage<counted, 5> storage;
		::fast_io::algorithms::uninitialized_value_construct_n_iters(
			storage.begin(), 5);
		if (counted::live != 5)
		{
			return 5;
		}
		if (::fast_io::algorithms::destroy_iters(
				storage.begin(), storage.end()) != storage.end())
		{
			return 6;
		}
		if (counted::live != 0)
		{
			return 7;
		}

		::fast_io::algorithms::uninitialized_value_construct_n_iters(
			storage.begin(), 5);
		if (::fast_io::algorithms::destroy_n_iters(
				storage.begin(), 3) != storage.begin() + 3)
		{
			return 8;
		}
		if (counted::live != 2)
		{
			return 9;
		}
		::fast_io::algorithms::destroy_iters(
			storage.begin() + 3, storage.end());
		if (counted::live != 0)
		{
			return 10;
		}

		::fast_io::algorithms::uninitialized_value_construct_n_iters(
			storage.begin(), 5);
		if (::fast_io::algorithms::destroy(storage) != storage.end())
		{
			return 14;
		}
		if (counted::live != 0)
		{
			return 15;
		}
	}

	{
		raw_storage<std::string, 4> storage;
		::fast_io::algorithms::uninitialized_value_construct_n_iters(
			storage.begin(), 4);
		for (std::size_t i{}; i != 4; ++i)
		{
			storage.begin()[i] = "text";
		}
		if (::fast_io::algorithms::destroy_n(
				storage, 2) != storage.begin() + 2)
		{
			return 11;
		}
		::fast_io::algorithms::destroy_n_iters(storage.begin() + 2, 2);
	}

	{
		std::vector<int> values{1, 2, 3, 4};
		if (::fast_io::algorithms::destroy_iters(
				values.begin(), values.end()) != values.end())
		{
			return 12;
		}
	}

	{
		raw_storage<counted, 6> storage;
		::fast_io::algorithms::uninitialized_value_construct_index_range(
			storage, 0, 6);
		if (counted::live != 6)
		{
			return 16;
		}
		::fast_io::algorithms::destroy_index_range(storage, 1, 4);
		if (counted::live != 3)
		{
			return 17;
		}
		::fast_io::algorithms::destroy_index_range(storage, 0, 1);
		::fast_io::algorithms::destroy_index_range(storage, 4, 6);
		if (counted::live != 0)
		{
			return 18;
		}
	}
	return 0;
}
