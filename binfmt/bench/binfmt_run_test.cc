#include <fast_io.h>
#include "binfmt_print.h"

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string_view>
#include <unistd.h>

namespace binfmt = ::fast_io::binfmt;

static void put_uleb(std::vector<std::uint8_t> &b, std::uint64_t v)
{
	do
	{
		auto byte = static_cast<std::uint8_t>(v & 0x7Fu);
		v >>= 7;
		if (v)
			byte |= 0x80u;
		b.push_back(byte);
	} while (v);
}

static void put_node(std::vector<std::uint8_t> &b, std::uint32_t code,
					 std::uint32_t kind)
{
	put_uleb(b, (static_cast<std::uint64_t>(code) << 3) | kind);
}

static void put_uleb_param(std::vector<std::uint8_t> &b, std::uint32_t code,
						   std::uint64_t v)
{
	put_node(b, code, 1);
	put_uleb(b, v);
}

static void put_literal(std::vector<std::uint8_t> &b, char const *s,
						std::size_t n)
{
	put_node(b, 1 /*literal*/, 2 /*bytes*/);
	put_uleb(b, n);
	for (std::size_t i = 0; i != n; ++i)
		b.push_back(static_cast<std::uint8_t>(s[i]));
}

static void put_field(std::vector<std::uint8_t> &b,
					  std::vector<std::pair<std::uint32_t, std::uint64_t>> const &params)
{
	put_node(b, 2 /*field*/, 3 /*list*/);
	put_uleb(b, params.size());
	for (auto const &[code, val] : params)
	{
		// flag params are bare markers (kind none, no payload)
		if (code == 5 || code == 6 || code == 7 || code == 8 || code == 9 ||
			code == 16)
			put_node(b, code, 0 /*none*/);
		else
			put_uleb_param(b, code, val);
	}
}

struct view_t
{
	void const *ptr;
	std::size_t len;
};

static bool run(std::vector<std::uint8_t> const &prog, std::size_t count,
				void const *cookie, char const *expect) throws
{
	char out[256]{};
	::fast_io::basic_obuffer_view<char> ob{out, out + sizeof(out)};
	auto dev{binfmt::binfmt_device_of(ob)};
	binfmt::binfmt_vprint<binfmt::binfmt_policy_c_style>(dev, prog.data(), prog.size(), count, cookie);
	auto p{binfmt::get_output_buffer_ptrs(dev)};
	return std::string_view{out, static_cast<std::size_t>(p.curr_ptr - out)} == expect;
}

int main()
{
	int failures{};
	try
	{
		// ---- case 1: literal "x = " %d(arg0) literal "\n" ----------------
		{
			std::vector<std::uint8_t> prog;
			put_literal(prog, "x = ", 4);
			put_field(prog, {{13 /*type*/, 1 /*d*/}, {12 /*ctype*/, 1 /*i32*/}, {1 /*arg*/, 0}});
			put_literal(prog, "\n", 1);

			binfmt::binfmt_cookie_element cookie{};
			cookie.field1 = static_cast<std::uint32_t>(-42);

			failures += run(prog, 1, &cookie, "x = -42\n") ? 0 : 1;
		}

		// ---- case 2: "n=" %#012x(arg0) ", s=" %s(arg1) "\n" -------------
		{
			std::vector<std::uint8_t> prog;
			put_literal(prog, "n=", 2);
			put_field(prog, {{13 /*type*/, 4 /*x*/},
							 {12 /*ctype*/, 4 /*u32*/},
							 {1 /*arg*/, 0},
							 {5 /*flag_alt*/, 1},
							 {6 /*flag_zero*/, 1},
							 {10 /*width*/, 12}});
			put_literal(prog, ", s=", 4);
			put_field(prog, {{13 /*type*/, 7 /*s*/}, {12 /*ctype*/, 28 /*c8view*/}, {1 /*arg*/, 1}});
			put_literal(prog, "\n", 1);

			binfmt::binfmt_cookie_element cookie[2]{};
			cookie[0].field1 = 0x2au;
			cookie[1].field1 = reinterpret_cast<std::size_t>("hi");
			cookie[1].field2 = 2;

			failures += run(prog, 2, cookie, "n=0x000000002a, s=hi\n") ? 0 : 1;
		}

		// ---- case 3: literal "[" %5s(arg0) "]" left-align via fill ------
		{
			std::vector<std::uint8_t> prog;
			put_literal(prog, "[", 1);
			put_field(prog, {{13 /*type*/, 7 /*s*/},
							 {12 /*ctype*/, 28 /*c8view*/},
							 {1 /*arg*/, 0},
							 {3 /*align*/, 1 /*left*/},
							 {10 /*width*/, 5}});
			put_literal(prog, "]\n", 2);

			binfmt::binfmt_cookie_element cookie{};
			cookie.field1 = reinterpret_cast<std::size_t>("ab");
			cookie.field2 = 2;

			failures += run(prog, 1, &cookie, "[ab   ]\n") ? 0 : 1;
		}

		// ---- case 5: fmt `{}` — the program carries no ctype; the cookie's
		//              typetag supplies it (u64) --------------------------
		{
			std::vector<std::uint8_t> prog;
			put_field(prog, {{13 /*type*/, 2 /*u*/}, {1 /*arg*/, 0}});

			binfmt::binfmt_cookie_element cookie{};
			cookie.typetag = binfmt::binfmt_typetag::u64;
			cookie.field1 = 123456789;

			failures += run(prog, 1, &cookie, "123456789") ? 0 : 1;
		}

		// ---- case 4: out-of-range arg must throw ------------------------
		{
			std::vector<std::uint8_t> prog;
			put_field(prog, {{13 /*type*/, 1 /*d*/}, {12 /*ctype*/, 1 /*i32*/}, {1 /*arg*/, 9}});
			char out[16]{};
			::fast_io::basic_obuffer_view<char> ob{out, out + sizeof(out)};
			auto dev{binfmt::binfmt_device_of(ob)};
			bool threw{};
			try
			{
				binfmt::binfmt_vprint<binfmt::binfmt_policy_c_style>(dev, prog.data(), prog.size(), 1, nullptr);
			}
			catch throws(::std::error e)
			{
				threw = true;
			}
			failures += threw ? 0 : 1;
		}
	}
	catch throws(::std::error e)
	{
		failures += 1;
	}

	char const *msg{failures == 0 ? "all ok\n" : "FAILURES\n"};
	::write(1, msg, failures == 0 ? 7 : 9);
	return failures;
}
