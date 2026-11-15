#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstddef>
#include <cstdint>
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
	counted(counted const &other) noexcept : value{other.value} { ++live; }
	~counted() noexcept { --live; }
};

struct throwing_default
{
	throwing_default() throws(true);
};

constexpr bool constexpr_value_construct_test()
{
	int a[4]{1, 2, 3, 4};
	auto const result{::fast_io::algorithms::uninitialized_value_construct(a)};
	if (result != a + 4)
	{
		return false;
	}
	return a[0] == 0 && a[1] == 0 && a[2] == 0 && a[3] == 0;
}

static_assert(constexpr_value_construct_test());

constexpr bool constexpr_value_construct_n_test()
{
	int value{};
	int *pointers[3]{&value, &value, &value};
	auto const result{
		::fast_io::algorithms::uninitialized_value_construct_n(pointers, 3)};
	if (result != pointers + 3)
	{
		return false;
	}
	return pointers[0] == nullptr && pointers[1] == nullptr &&
	       pointers[2] == nullptr;
}

static_assert(constexpr_value_construct_n_test());

constexpr bool constexpr_default_construct_test()
{
	int a[4]{1, 2, 3, 4};
	auto const result{::fast_io::algorithms::uninitialized_default_construct(a)};
	if (result != a + 4)
	{
		return false;
	}
	a[0] = 10;
	a[3] = 40;
	return a[0] == 10 && a[3] == 40;
}

static_assert(constexpr_default_construct_test());

constexpr bool constexpr_default_construct_n_test()
{
	int a[3]{1, 2, 3};
	auto const result{
		::fast_io::algorithms::uninitialized_default_construct_n(a, 3)};
	if (result != a + 3)
	{
		return false;
	}
	a[1] = 5;
	return a[1] == 5;
}

static_assert(constexpr_default_construct_n_test());

} // namespace

static_assert(noexcept(::fast_io::algorithms::uninitialized_value_construct(
	::std::declval<int (&)[4]>())));
static_assert(noexcept(::fast_io::algorithms::uninitialized_value_construct(
	::std::declval<std::vector<int> &>())));
static_assert(noexcept(::fast_io::algorithms::uninitialized_value_construct_n_iters(
	::std::declval<int *>(), std::size_t{})));
static_assert(noexcept(::fast_io::algorithms::uninitialized_default_construct(
	::std::declval<int (&)[4]>())));
static_assert(!noexcept(::fast_io::algorithms::uninitialized_default_construct(
	::std::declval<std::vector<throwing_default> &>())));
static_assert(!noexcept(::fast_io::algorithms::uninitialized_value_construct(
	::std::declval<std::vector<throwing_default> &>())));

int main()
{
	{
		raw_storage<int, 8> storage;
		auto const result{
			::fast_io::algorithms::uninitialized_value_construct(storage)};
		if (result != storage.end())
		{
			return 1;
		}
		for (std::size_t i{}; i != storage.size(); ++i)
		{
			if (storage.begin()[i] != 0)
			{
				return 2;
			}
		}

		::fast_io::algorithms::uninitialized_default_construct(storage);
		for (std::size_t i{}; i != storage.size(); ++i)
		{
			storage.begin()[i] = static_cast<int>(i + 1);
		}
		for (std::size_t i{}; i != storage.size(); ++i)
		{
			if (storage.begin()[i] != static_cast<int>(i + 1))
			{
				return 3;
			}
		}
	}

	{
		raw_storage<std::string, 4> storage;
		::fast_io::algorithms::uninitialized_default_construct(storage);
		for (std::size_t i{}; i != storage.size(); ++i)
		{
			if (!storage.begin()[i].empty())
			{
				return 4;
			}
			storage.begin()[i] = "value";
		}
		for (std::size_t i{}; i != storage.size(); ++i)
		{
			if (storage.begin()[i] != "value")
			{
				return 5;
			}
		}
		::fast_io::algorithms::destroy(storage);
	}

	{
		raw_storage<counted, 6> storage;
		auto const result{
			::fast_io::algorithms::uninitialized_default_construct_n_iters(
				storage.begin(), 4)};
		if (result != storage.begin() + 4)
		{
			return 6;
		}
		if (counted::live != 4)
		{
			return 7;
		}
		for (std::size_t i{}; i != 4; ++i)
		{
			if (storage.begin()[i].value != 7)
			{
				return 8;
			}
		}
		::fast_io::algorithms::destroy_n_iters(storage.begin(), 4);
		if (counted::live != 0)
		{
			return 9;
		}

		::fast_io::algorithms::uninitialized_value_construct_n_iters(
			storage.begin(), 3);
		if (counted::live != 3)
		{
			return 10;
		}
		::fast_io::algorithms::destroy_n_iters(storage.begin(), 3);
		if (counted::live != 0)
		{
			return 11;
		}
	}

	{
		raw_storage<int, 4> storage;
		::fast_io::algorithms::uninitialized_default_construct_n_iters(
			storage.begin(), 4);
		storage.begin()[0] = 1;
		storage.begin()[3] = 4;
		if (storage.begin()[0] != 1 || storage.begin()[3] != 4)
		{
			return 12;
		}
		::fast_io::algorithms::uninitialized_value_construct_n_iters(
			storage.begin(), 2);
		if (storage.begin()[0] != 0 || storage.begin()[1] != 0 ||
			storage.begin()[3] != 4)
		{
			return 13;
		}
	}
	return 0;
}
