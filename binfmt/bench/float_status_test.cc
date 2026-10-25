// Compare the float-enabled status paths (stock and batched) byte-for-byte
// with the ordinary fast_io print pipeline for the same manipulator.
#include <cstdio>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_status.h"
#include "binfmt_status_batch.h"

namespace
{

int failures{};

bool same(::fast_io::string const &a, ::fast_io::string const &b) noexcept
{
	return a.size() == b.size() &&
		   __builtin_memcmp(a.data(), b.data(), a.size()) == 0;
}

template <typename M>
void check(char const *label, M m) throws
{
	::fast_io::string ref;
	auto rr{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, ref)};
	::fast_io::io::print(rr, m);

	::fast_io::string stock;
	auto sr{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, stock)};
	::fast_io::io::print(::fast_io::mnp::binfmt_float(sr), m);

	::fast_io::string batch;
	auto br{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, batch)};
	::fast_io::io::print(::fast_io::mnp::binfmt_batch_float(br), m);

	bool const ok{same(ref, stock) && same(ref, batch)};
	if (!ok)
	{
		++failures;
		::std::printf("%-28s MISMATCH\n  ref  [%.*s]\n  stock[%.*s]\n  batch[%.*s]\n", label,
					  static_cast<int>(ref.size()), ref.data(), static_cast<int>(stock.size()),
					  stock.data(), static_cast<int>(batch.size()), batch.data());
	}
	else
	{
		::std::printf("%-28s ok  [%.*s]\n", label, static_cast<int>(ref.size()), ref.data());
	}
}

template <typename T>
void probe_type(char const *tag, T v) throws
{
	check(tag, v);
	check(tag, static_cast<T>(-v));
	check(tag, ::fast_io::mnp::fixed(v));
	check(tag, ::fast_io::mnp::fixed(v, 0));
	check(tag, ::fast_io::mnp::fixed(v, 6));
	check(tag, ::fast_io::mnp::scientific(v));
	check(tag, ::fast_io::mnp::scientific(v, 3));
	check(tag, ::fast_io::mnp::general(v));
	check(tag, ::fast_io::mnp::general(v, 12));
	check(tag, ::fast_io::mnp::decimal(v));
	check(tag, ::fast_io::mnp::decimal(v, 5));
	check(tag, ::fast_io::mnp::hexfloat(v));
	check(tag, ::fast_io::mnp::hexfloat(v, 3));
	check(tag, ::fast_io::mnp::hexfloat<true>(v));
	check(tag, ::fast_io::mnp::fixed<true>(v, 4));
	check(tag, ::fast_io::mnp::scientific<true>(v, 2));
}

// the same equality check for the other character flavours: the helper writes
// ASCII once, so every non-char stream is widened on the fly and must still
// match the ordinary fast_io pipeline.
template <typename C>
using str_t = ::fast_io::containers::basic_string<C, ::fast_io::native_global_allocator>;

template <typename C>
bool same_c(str_t<C> const &a, str_t<C> const &b) noexcept
{
	return a.size() == b.size() &&
		   __builtin_memcmp(a.data(), b.data(), a.size() * sizeof(C)) == 0;
}

template <typename C, typename M>
void check_c(char const *label, M m) throws
{
	str_t<C> ref;
	auto rr{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, ref)};
	::fast_io::io::print(rr, m);

	str_t<C> stock;
	auto sr{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, stock)};
	::fast_io::io::print(::fast_io::mnp::binfmt_float(sr), m);

	str_t<C> batch;
	auto br{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, batch)};
	::fast_io::io::print(::fast_io::mnp::binfmt_batch_float(br), m);

	bool const ok{same_c(ref, stock) && same_c(ref, batch)};
	if (!ok)
	{
		++failures;
		::std::printf("%-28s MISMATCH ref=%zu stock=%zu batch=%zu\n", label, ref.size(),
					  stock.size(), batch.size());
	}
	else
	{
		::std::printf("%-28s ok\n", label);
	}
}

template <typename C>
void probe_char_type(char const *tag) throws
{
	check_c<C>("w 1.5", static_cast<double>(1.5));
	check_c<C>("w fixed", ::fast_io::mnp::fixed(static_cast<double>(0.1), 5));
	check_c<C>("w scientific", ::fast_io::mnp::scientific(static_cast<double>(1e300), 3));
	check_c<C>("w hexfloat upper", ::fast_io::mnp::hexfloat<true>(static_cast<double>(1.0 / 3.0)));
	check_c<C>("w long double", static_cast<long double>(0.1L));
}

} // namespace

int main() try
{
	probe_type("float", 1.5f);
	probe_type("float", 0.1f);
	probe_type("double", 1.5);
	probe_type("double", 3.141592653589793);
	probe_type("double", 1.0 / 3.0);
	probe_type("long double", 1.5L);
	probe_type("long double", 0.1L);

	// non-finite values: zero padding must stay spaces, sign still applies
	check("inf", ::fast_io::mnp::general(__builtin_inf()));
	check("inf s", ::fast_io::mnp::scientific(__builtin_inf()));
	check("nan", ::fast_io::mnp::fixed(__builtin_nan("")));

	// the helper's fixed-size buffer must hold the widest precision it accepts
	check("max prec fixed", ::fast_io::mnp::fixed(1.5L, 11566));
	check("max prec sci", ::fast_io::mnp::scientific(1.5L, 11566));
	check("max prec hex", ::fast_io::mnp::hexfloat(1.5L, 11566));

	probe_char_type<wchar_t>("wchar_t");
	probe_char_type<char8_t>("char8_t");
	probe_char_type<char16_t>("char16_t");
	probe_char_type<char32_t>("char32_t");

	::std::printf("%s (%d failures)\n", failures == 0 ? "PASS" : "FAIL", failures);
	return failures == 0 ? 0 : 1;
}
catch throws(::std::error e)
{
	::fast_io::io::perrln(e);
	return 2;
}
