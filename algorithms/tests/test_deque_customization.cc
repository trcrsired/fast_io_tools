#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <algorithm>
#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include <vector>

namespace
{

using int_deque =
	::fast_io::containers::deque<int, ::fast_io::native_global_allocator>;

struct nothrow_box
{
	int value{};
	nothrow_box() = default;
	nothrow_box(int v) : value(v) {}
	nothrow_box(nothrow_box const &) noexcept = default;
	nothrow_box(nothrow_box &&) noexcept = default;
	nothrow_box &operator=(nothrow_box const &) noexcept = default;
	nothrow_box &operator=(nothrow_box &&) noexcept = default;
	~nothrow_box() = default;
	bool operator==(nothrow_box const &) const noexcept = default;
};

template<typename Deque>
Deque make_deque(std::vector<int> const &values)
{
	return Deque(::fast_io::freestanding::from_range, values);
}

bool check_offsets(std::vector<int> const &values)
{
	int_deque d{make_deque<int_deque>(values)};
	std::deque<int> const reference(values.begin(), values.end());
	std::size_t const n{values.size()};
	std::size_t const block{
		::fast_io::containers::deque<int, ::fast_io::native_global_allocator>::
			size_per_segment()};
	std::vector<std::size_t> points{0, 1, 2};
	if (block > 2)
	{
		points.push_back(block - 2);
		points.push_back(block - 1);
	}
	points.push_back(block);
	points.push_back(block + 1);
	points.push_back(block * 2);
	points.push_back(block * 2 + 1);
	points.push_back(n);
	std::sort(points.begin(), points.end());
	points.erase(std::unique(points.begin(), points.end()), points.end());
	while (!points.empty() && points.back() > n)
	{
		points.pop_back();
	}
	for (std::size_t first : points)
	{
		for (std::size_t last : points)
		{
			if (last < first)
			{
				continue;
			}
			std::vector<int> out(n + 1, -1);
			auto end{::fast_io::algorithms::copy_iters(
				d.begin() + first, d.begin() + last, out.data())};
			if (end != out.data() + (last - first) ||
				!std::equal(reference.begin() + first, reference.begin() + last,
					out.begin()))
			{
				return false;
			}

			std::fill(out.begin(), out.end(), -1);
			end = ::fast_io::algorithms::copy_backward_iters(
				d.begin() + first, d.begin() + last, out.data() + (last - first));
			if (end != out.data() ||
				!std::equal(reference.begin() + first, reference.begin() + last,
					out.begin()))
			{
				return false;
			}

			std::fill(out.begin(), out.end(), -1);
			end = ::fast_io::algorithms::copy_n_iters(
				d.begin() + first, last - first, out.data());
			if (end != out.data() + (last - first) ||
				!std::equal(reference.begin() + first, reference.begin() + last,
					out.begin()))
			{
				return false;
			}

			std::fill(out.begin(), out.end(), -1);
			end = ::fast_io::algorithms::non_overlapped_copy_iters(
				d.begin() + first, d.begin() + last, out.data());
			if (end != out.data() + (last - first) ||
				!std::equal(reference.begin() + first, reference.begin() + last,
					out.begin()))
			{
				return false;
			}

			std::fill(out.begin(), out.end(), -1);
			end = ::fast_io::algorithms::move_iters(
				d.begin() + first, d.begin() + last, out.data());
			if (end != out.data() + (last - first) ||
				!std::equal(reference.begin() + first, reference.begin() + last,
					out.begin()))
			{
				return false;
			}

			std::fill(out.begin(), out.end(), -1);
			end = ::fast_io::algorithms::move_backward_iters(
				d.begin() + first, d.begin() + last, out.data() + (last - first));
			if (end != out.data() ||
				!std::equal(reference.begin() + first, reference.begin() + last,
					out.begin()))
			{
				return false;
			}
		}
	}

	std::vector<int> out(n + 1, -1);
	auto end{::fast_io::algorithms::reverse_copy(d, out.data())};
	if (end != out.data() + n ||
		!std::equal(reference.rbegin(), reference.rend(), out.begin()))
	{
		return false;
	}

	std::vector<int> expected(n + 1, -1);
	std::vector<std::size_t> middles{0, 1, n / 2, n};
	if (block < n)
	{
		middles.push_back(block - 1);
		middles.push_back(block);
		middles.push_back(block + 1);
	}
	if (n > 1)
	{
		middles.push_back(n - 1);
	}
	std::sort(middles.begin(), middles.end());
	middles.erase(std::unique(middles.begin(), middles.end()), middles.end());
	while (!middles.empty() && middles.back() > n)
	{
		middles.pop_back();
	}
	for (std::size_t middle : middles)
	{
		std::fill(out.begin(), out.end(), -1);
		end = ::fast_io::algorithms::rotate_copy(
			d, d.begin() + middle, out.data());
		std::rotate_copy(reference.begin(), reference.begin() + middle,
			reference.end(), expected.begin());
		if (end != out.data() + n ||
			!std::equal(expected.begin(), expected.begin() + n, out.begin()))
		{
			return false;
		}
	}
	return true;
}

bool check_int_deque()
{
	for (std::size_t n : {0u, 1u, 2u, 1023u, 1024u, 1025u, 2048u, 4096u, 5000u})
	{
		std::vector<int> values(n);
		for (std::size_t i{}; i != n; ++i)
		{
			values[i] = static_cast<int>((i * 37 + 11) % 101);
		}
		if (!check_offsets(values))
		{
			return false;
		}
	}
	return true;
}

bool check_uninitialized()
{
	constexpr std::size_t n{3000};
	std::vector<int> values(n);
	for (std::size_t i{}; i != n; ++i)
	{
		values[i] = static_cast<int>(i * 7 + 3);
	}
	int_deque d{make_deque<int_deque>(values)};
	alignas(int) unsigned char storage[sizeof(int) * n]{};
	int *const out{reinterpret_cast<int *>(storage)};
	auto end{::fast_io::algorithms::uninitialized_copy_iters(
		d.begin(), d.end(), out)};
	if (end != out + n || !std::equal(values.begin(), values.end(), out))
	{
		return false;
	}
	::fast_io::algorithms::destroy_iters(out, out + n);

	std::fill_n(reinterpret_cast<unsigned char *>(storage), sizeof(storage),
		static_cast<unsigned char>(0));
	end = ::fast_io::algorithms::uninitialized_copy_n_iters(
		d.begin(), n, out);
	if (end != out + n || !std::equal(values.begin(), values.end(), out))
	{
		return false;
	}
	::fast_io::algorithms::destroy_iters(out, out + n);

	int *const back_out{out + n};
	end = ::fast_io::algorithms::uninitialized_copy_backward_iters(
		d.begin(), d.end(), back_out);
	if (end != out || !std::equal(values.begin(), values.end(), out))
	{
		return false;
	}
	::fast_io::algorithms::destroy_iters(out, out + n);

	int_deque move_source{make_deque<int_deque>(values)};
	end = ::fast_io::algorithms::uninitialized_move_iters(
		move_source.begin(), move_source.end(), out);
	if (end != out + n || !std::equal(values.begin(), values.end(), out))
	{
		return false;
	}
	::fast_io::algorithms::destroy_iters(out, out + n);

	int_deque relocate_source{make_deque<int_deque>(values)};
	end = ::fast_io::algorithms::uninitialized_relocate_iters(
		relocate_source.begin(), relocate_source.end(), out);
	return end == out + n && std::equal(values.begin(), values.end(), out);
}

bool check_inplace()
{
	std::size_t const block{
		::fast_io::containers::deque<int, ::fast_io::native_global_allocator>::
			size_per_segment()};
	for (std::size_t n : {0u, 1u, 2u, 1023u, 1024u, 1025u, 2048u, 5000u})
	{
		std::vector<int> values(n);
		for (std::size_t i{}; i != n; ++i)
		{
			values[i] = static_cast<int>((i * 53 + 7) % 97);
		}
		{
			int_deque d{make_deque<int_deque>(values)};
			::fast_io::algorithms::reverse(d);
			auto expected{values};
			std::reverse(expected.begin(), expected.end());
			if (!std::equal(d.begin(), d.end(), expected.begin()))
			{
				return false;
			}
		}
		std::vector<std::size_t> middles{0, 1, n / 3, n / 2, n};
		if (block < n)
		{
			middles.push_back(block - 1);
			middles.push_back(block);
			middles.push_back(block + 1);
		}
		if (n > 1)
		{
			middles.push_back(n - 1);
		}
		std::sort(middles.begin(), middles.end());
		middles.erase(std::unique(middles.begin(), middles.end()), middles.end());
		while (!middles.empty() && middles.back() > n)
		{
			middles.pop_back();
		}
		for (std::size_t middle : middles)
		{
			int_deque d{make_deque<int_deque>(values)};
			auto const result{
				::fast_io::algorithms::rotate(d, d.begin() + middle)};
			auto expected{values};
			auto expected_result{
				std::rotate(expected.begin(), expected.begin() + middle,
					expected.end())};
			if (result != d.begin() + (n - middle) ||
				!std::equal(d.begin(), d.end(), expected.begin()) ||
				(result - d.begin()) != (expected_result - expected.begin()))
			{
				return false;
			}
		}
	}
	return true;
}

bool check_nontrivial()
{
	using box_deque =
		::fast_io::containers::deque<nothrow_box, ::fast_io::native_global_allocator>;
	std::vector<nothrow_box> source;
	for (int i{}; i != 5000; ++i)
	{
		source.emplace_back(i);
	}
	box_deque d(::fast_io::freestanding::from_range, source);
	std::vector<nothrow_box> out(5001);
	auto end{::fast_io::algorithms::copy(
		d, out.data())};
	if (end != out.data() + source.size() ||
		!std::equal(source.begin(), source.end(), out.begin()))
	{
		return false;
	}
	std::vector<nothrow_box> moved(5001);
	end = ::fast_io::algorithms::move(d, moved.data());
	return end == moved.data() + source.size() &&
		std::equal(source.begin(), source.end(), moved.begin());
}

bool check_move_only() throws(true)
{
	using ptr_deque = ::fast_io::containers::deque<
		std::unique_ptr<int>, ::fast_io::native_global_allocator>;
	ptr_deque d;
	for (int i{}; i != 100; ++i)
	{
		d.emplace_back(std::make_unique<int>(i));
	}
	std::vector<std::unique_ptr<int>> out(101);
	auto end{::fast_io::algorithms::move(d, out.data())};
	if (end != out.data() + 100)
	{
		return false;
	}
	for (int i{}; i != 100; ++i)
	{
		if (!out[i] || *out[i] != i)
		{
			return false;
		}
	}
	return true;
}

bool check_strings() throws(true)
{
	using string_deque = ::fast_io::containers::deque<
		std::string, ::fast_io::native_global_allocator>;
	std::vector<std::string> source;
	for (int i{}; i != 3000; ++i)
	{
		source.push_back(std::to_string(i));
	}
	string_deque d(::fast_io::freestanding::from_range, source);
	std::vector<std::string> out(3001);
	auto end{::fast_io::algorithms::copy(d, out.data())};
	return end == out.data() + source.size() &&
		std::equal(source.begin(), source.end(), out.begin());
}

} // namespace

int main()
{
	if (!check_int_deque() || !check_nontrivial() || !check_uninitialized() || !check_inplace())
	{
		return 1;
	}
	try
	{
		if (!check_move_only() || !check_strings())
		{
			return 2;
		}
	}
	catch throws(::std::error)
	{
		return 3;
	}
	return 0;
}
