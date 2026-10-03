// locale_dump — decode + disassemble a .bin locale blob for verification.
//
// usage: locale_dump <file.bin>

#include <herbceptions/error>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include "localedef.h"

namespace
{
using namespace ::fast_io_i18n;

void dump_program(::fast_io::u8string_view prog, unsigned indent) throws
{
	using namespace ::fast_io_i18n::binfmt;
	reader r{prog};
	auto pad{[](unsigned n) throws {
		for (unsigned i{}; i < n; ++i)
		{
			::fast_io::print(" ");
		}
	}};
	while (!r.empty())
	{
		node_head h;
		next_tag(r, h);
		pad(indent);
		switch (h.code)
		{
		case op_literal:
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			::fast_io::println("literal [", n, "]");
			break;
		}
		case op_field:
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::println("field children=", n);
			for (::std::uint_least64_t k{}; k < n; ++k)
			{
				node_head c;
				next_tag(r, c);
				pad(indent + 2);
				::fast_io::print("param ", c.code, " kind=", (int)c.kind);
				if (c.kind == kind_uleb)
				{
					::std::uint_least64_t v{};
					r.get_leb(v);
					::fast_io::print(" = ", v);
				}
				else if (c.kind == kind_sleb)
				{
					::std::int_least64_t v{};
					r.get_leb(v);
					::fast_io::print(" = ", v);
				}
				else if (c.kind == kind_bytes)
				{
					::std::uint_least64_t v{};
					r.get_leb(v);
					::fast_io::u8string_view bytes;
					r.get_bytes(v, bytes);
					::fast_io::print(" [", v, "]");
				}
				::fast_io::print("\n");
			}
			break;
		}
		case op_pct:
			if (h.kind == kind_uleb)
			{
				::std::uint_least64_t v{};
				r.get_leb(v);
				::fast_io::println("pct conv=", v);
			}
			else
			{
				::std::uint_least64_t n{};
				r.get_leb(n);
				::fast_io::println("pct children=", n);
				for (::std::uint_least64_t k{}; k < n; ++k)
				{
					node_head c;
					next_tag(r, c);
					pad(indent + 2);
					::std::uint_least64_t v{};
					r.get_leb(v);
					::fast_io::println("param ", c.code, " = ", v);
				}
			}
			break;
		default:
			::fast_io::println("node code=", h.code, " kind=", (int)h.kind);
			skip_payload(r, h);
			break;
		}
	}
}

} // namespace

int main(int argc, char **argv) try
{
	if (argc != 2)
	{
		::fast_io::perrln("usage: locale_dump <file.bin>");
		return 1;
	}
	::fast_io::native_file_loader fmp(::fast_io::mnp::os_c_str(argv[1]));
	::fast_io::u8string_view blob{
		reinterpret_cast<char8_t const *>(fmp.data()), fmp.size()};
	auto h{lcblob::read_header(blob)};
	::fast_io::println("total=", h.total_size, " (file ", blob.size(), ")");
	::fast_io::println("name_len=", h.name.size(), " enc_len=",
			   h.encoding.size());
	for (::std::size_t c{}; c < lcblob::cat_count; ++c)
	{
		auto cat_rva{lcblob::read_u32(h.cat_dir + c * 4)};
		if (!cat_rva)
		{
			continue;
		}
		auto const &sch{lcblob::cat_schemas[c]};
		::fast_io::println(::fast_io::mnp::os_c_str(sch.glibc_name), " @",
				   cat_rva);
		for (::std::size_t f{}; f < sch.fields.size(); ++f)
		{
			auto slot_rva{lcblob::read_u32(
				blob.data() + cat_rva + f * 4)};
			if (!slot_rva)
			{
				continue;
			}
			binfmt::reader r{{blob.data() + slot_rva,
					  blob.size() - slot_rva}};
			::std::uint_least64_t tag{};
			r.get_leb(tag);
			::fast_io::print("  ", ::fast_io::mnp::os_c_str(
							  sch.fields[f].name),
					 " tag=", tag);
			switch (static_cast<lcblob::slot_tag>(tag))
			{
			case lcblob::slot_tag::string:
			case lcblob::slot_tag::program:
			{
				::std::uint_least64_t rva{}, len{};
				r.get_leb(rva);
				r.get_leb(len);
				::fast_io::println(" rva=", rva, " len=", len);
				if (tag == static_cast<::std::uint_least64_t>(
						    lcblob::slot_tag::program))
				{
					dump_program(
						::fast_io::u8string_view{blob.data() + rva, len}, 4);
				}
				break;
			}
			case lcblob::slot_tag::bytes:
			{
				::std::uint_least64_t rva{}, len{};
				r.get_leb(rva);
				r.get_leb(len);
				::fast_io::println(" rva=", rva, " len=", len);
				break;
			}
			case lcblob::slot_tag::strlist:
			case lcblob::slot_tag::eralist:
			{
				::std::uint_least64_t rva{};
				r.get_leb(rva);
				::fast_io::println(" rva=", rva);
				break;
			}
			case lcblob::slot_tag::integer:
			{
				::std::int_least64_t v{};
				r.get_leb(v);
				::fast_io::println(" = ", v);
				break;
			}
			case lcblob::slot_tag::int3:
			{
				::std::int_least64_t a{}, b{}, cc{};
				r.get_leb(a);
				r.get_leb(b);
				r.get_leb(cc);
				::fast_io::println(" = ", a, " ", b, " ", cc);
				break;
			}
			default:
				::fast_io::println(" ?");
				break;
			}
		}
	}
	return 0;
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
