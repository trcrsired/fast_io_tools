// Compile float programs through the fmt and printf front-ends and run them
// on the float-enabled interpreter, comparing against the ordinary fast_io
// print pipeline.
#include <cstddef>
#include <cstdio>

#include <fast_io.h>
#include <fast_io_dsal/string.h>

#include "binfmt_compile.h"
#include "binfmt_print.h"
#include "binfmt_status.h"

namespace
{

int failures{};

void run(char const *label, char const *fmt, bool printf_style, double value,
		 ::fast_io::string const &ref) throws
{
	::fast_io::string prog;
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, prog)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	if (printf_style)
	{
		::fast_io::binfmt::compile_c_printf(dev, fmt, ::fast_io::cstr_len(fmt));
	}
	else
	{
		::fast_io::binfmt::compile_fmt(dev, fmt, ::fast_io::cstr_len(fmt));
	}

	::fast_io::string out;
	auto oref2{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev2{::fast_io::binfmt::binfmt_device_of(oref2)};
	::fast_io::binfmt::binfmt_cookie_element ck[1];
	ck[0].typetag = ::fast_io::binfmt::binfmt_typetag::f64;
	::std::memcpy(__builtin_addressof(ck[0].field1), __builtin_addressof(value), sizeof(value));
	::fast_io::binfmt::binfmt_status_vprint_float(dev2, prog.data(), prog.size(), 1, ck);

	bool const ok{out.size() == ref.size() &&
				  __builtin_memcmp(out.data(), ref.data(), out.size()) == 0};
	if (!ok)
	{
		++failures;
	}
	::std::printf("%-12s %-8s %s  [%.*s]\n", label, fmt, ok ? "ok  " : "FAIL",
				  static_cast<int>(out.size()), out.data());
}

::fast_io::string native_of(auto const &v) throws
{
	::fast_io::string s;
	auto r{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, s)};
	::fast_io::io::print(r, v);
	return s;
}

// feed a raw cookie (the types the status path packs) through a compiled fmt
// program, and compare with a literal expectation.
void run_raw(char const *label, ::fast_io::binfmt::binfmt_typetag tag,
			 ::std::size_t f1, ::std::size_t f2, char const *fmt, char const *expect) throws
{
	::fast_io::string prog;
	auto oref{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, prog)};
	auto dev{::fast_io::binfmt::binfmt_device_of(oref)};
	::fast_io::binfmt::compile_fmt(dev, fmt, ::fast_io::cstr_len(fmt));

	::fast_io::string out;
	auto oref2{::fast_io::containers::io_strlike_ref(::fast_io::io_alias, out)};
	auto dev2{::fast_io::binfmt::binfmt_device_of(oref2)};
	::fast_io::binfmt::binfmt_cookie_element ck[1];
	ck[0].typetag = tag;
	ck[0].field1 = f1;
	ck[0].field2 = f2;
	::fast_io::binfmt::binfmt_status_vprint_float(dev2, prog.data(), prog.size(), 1, ck);

	auto const expect_len{::fast_io::cstr_len(expect)};
	bool const ok{out.size() == expect_len &&
				  __builtin_memcmp(out.data(), expect, expect_len) == 0};
	if (!ok)
	{
		++failures;
	}
	::std::printf("%-12s %-8s %s  [%.*s] expect [%s]\n", label, fmt, ok ? "ok  " : "FAIL",
				  static_cast<int>(out.size()), out.data(), expect);
}

template <typename T>
void run_raw_bits(char const *label, ::fast_io::binfmt::binfmt_typetag tag, T v) throws
{
	::std::size_t f1{}, f2{};
	::std::memcpy(__builtin_addressof(f1), __builtin_addressof(v), sizeof(v) < 8u ? sizeof(v) : 8u);
	if constexpr (sizeof(v) > 8u)
	{
		::std::memcpy(__builtin_addressof(f2), reinterpret_cast<char const *>(__builtin_addressof(v)) + 8u,
					  sizeof(v) - 8u);
	}
	run_raw(label, tag, f1, f2, "{}", "1.5");
	run_raw(label, tag, f1, f2, "{:.2f}", "1.50");
	run_raw(label, tag, f1, f2, "{:e}", "1.500000e+00");
	run_raw(label, tag, f1, f2, "{:a}", "0x1.8p+0");
	run_raw(label, tag, f1, f2, "{:A}", "0X1.8P+0");
}

} // namespace

int main() try
{
	double const v{1.0};
	run("fmt", "{}", false, v, native_of(v));
	run("fmt", "{:f}", false, v, native_of(::fast_io::mnp::fixed(v, 6)));
	run("fmt", "{:.2f}", false, v, native_of(::fast_io::mnp::fixed(v, 2)));
	run("fmt", "{:e}", false, v, native_of(::fast_io::mnp::scientific(v, 6)));
	run("fmt", "{:a}", false, v, native_of(::fast_io::mnp::hexfloat(v)));
	run("fmt", "{:A}", false, v, native_of(::fast_io::mnp::hexfloat<true>(v)));

	double const w{0.1};
	run("fmt", "{}", false, w, native_of(w));
	run("fmt", "{:f}", false, w, native_of(::fast_io::mnp::fixed(w, 6)));
	run("fmt", "{:.10f}", false, w, native_of(::fast_io::mnp::fixed(w, 10)));
	run("fmt", "{:e}", false, w, native_of(::fast_io::mnp::scientific(w, 6)));
	run("fmt", "{:g}", false, w, native_of(::fast_io::mnp::general(w, 6)));
	run("fmt", "{:a}", false, w, native_of(::fast_io::mnp::hexfloat(w)));
	run("fmt", "{:A}", false, w, native_of(::fast_io::mnp::hexfloat<true>(w)));

	run("printf", "%f", true, v, native_of(::fast_io::mnp::fixed(v, 6)));
	run("printf", "%.2f", true, v, native_of(::fast_io::mnp::fixed(v, 2)));
	run("printf", "%e", true, v, native_of(::fast_io::mnp::scientific(v, 6)));
	run("printf", "%g", true, v, native_of(::fast_io::mnp::general(v, 6)));
	run("printf", "%a", true, v, native_of(::fast_io::mnp::hexfloat(v)));
	run("printf", "%.3a", true, w, native_of(::fast_io::mnp::hexfloat(w, 3)));

	// raw packed values for the ctypes with no language type here
	using ::fast_io::binfmt::binfmt_typetag;
	run_raw("f16", binfmt_typetag::f16, 0x3e00u, 0u, "{}", "1.5");
	run_raw("f16", binfmt_typetag::f16, 0x3e00u, 0u, "{:.2f}", "1.50");
	run_raw("f16", binfmt_typetag::f16, 0x3e00u, 0u, "{:e}", "1.500000e+00");
	run_raw("f16", binfmt_typetag::f16, 0x3e00u, 0u, "{:a}", "0x1.8p+0");
	run_raw("f16", binfmt_typetag::f16, 0x3e00u, 0u, "{:A}", "0X1.8P+0");
	run_raw("bf16", binfmt_typetag::bf16, 0x3fc0u, 0u, "{}", "1.5");
	run_raw("bf16", binfmt_typetag::bf16, 0x3fc0u, 0u, "{:.2f}", "1.50");
	run_raw("bf16", binfmt_typetag::bf16, 0x3fc0u, 0u, "{:e}", "1.500000e+00");
	run_raw("bf16", binfmt_typetag::bf16, 0x3fc0u, 0u, "{:a}", "0x1.8p+0");
	run_raw_bits("f32", binfmt_typetag::f32, 1.5f);
	run_raw_bits("f64", binfmt_typetag::f64, 1.5);
	run_raw_bits("f80", binfmt_typetag::f80, 1.5L);
#if defined(__SIZEOF_INT128__) && (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__))
	run_raw_bits("f128", binfmt_typetag::f128, static_cast<__float128>(1.5L));
#endif

	::std::printf("%s (%d failures)\n", failures == 0 ? "PASS" : "FAIL", failures);
	return failures == 0 ? 0 : 1;
}
catch throws(::std::error e)
{
	::fast_io::io::perrln(e);
	return 2;
}
