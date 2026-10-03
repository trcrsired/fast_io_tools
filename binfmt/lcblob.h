#pragma once

// lcblob — flat position-independent locale container (spec Part 7).
// Every reference is an RVA uleb/u32 from the blob base; mmap-friendly,
// no relocations.
//
// Layout: [header][cat_dir u32*12][pool][slot tables][records]
//
//   header:   u32 magic 'FCL1' | uleb version | uleb total_size | uleb flags
//             | strref name | strref encoding | uleb cat_dir_rva
//   cat_dir:  u32 rva per category (0 = category absent)
//   pool:     string / program / list-body bytes (strref targets)
//   slot tbl: u32 rva per field (0 = slot absent)
//   records:  uleb tag | payload
//
// Pool-first ordering matters: strrefs live inside pool bodies
// (STRLIST/ERALIST) and inside records, so pool content depends only on
// pool_base — one fixpoint loop covers header-size feedback.

#include "compile.h"

namespace fast_io_i18n
{
namespace lcblob
{

inline constexpr ::std::uint_least32_t magic{0x314C4346}; // 'FCL1'
inline constexpr ::std::uint_least64_t blob_version{1};   // reader rejects files with version > this

// categories — fixed ids
enum lc_cat : ::std::uint_least32_t
{
	cat_identification = 0,
	cat_ctype = 1,
	cat_collate = 2,
	cat_time = 3,
	cat_numeric = 4,
	cat_monetary = 5,
	cat_messages = 6,
	cat_paper = 7,
	cat_name = 8,
	cat_address = 9,
	cat_telephone = 10,
	cat_measurement = 11,
	cat_count = 12,
};

enum class slot_tag : ::std::uint_least8_t
{
	absent = 0,
	string = 1,
	strlist = 2,
	integer = 3,
	bytes = 4,
	program = 5,
	int3 = 6,
	eralist = 7,
};

// front-end used for program slots
enum class src_lang : ::std::uint_least8_t
{
	none = 0,
	pct_time = 1,	 // strict strftime directives
	pct_generic = 2, // generic %-letters (name_fmt, postal_fmt, tel_*)
};

struct field_def
{
	char const *name;
	slot_tag tag;
	src_lang lang{src_lang::none};
};

// ---------------------------------------------------------------------------
// slot schemas — field order matches fast_io basic_lc_* member order
// ---------------------------------------------------------------------------

inline constexpr ::std::size_t szf{0};

#define LC_S(name_)                                                                                                    \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::string, {}                                                            \
	}
#define LC_SL(name_)                                                                                                   \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::strlist, {}                                                           \
	}
#define LC_I(name_)                                                                                                    \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::integer, {}                                                           \
	}
#define LC_B(name_)                                                                                                    \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::bytes, {}                                                             \
	}
#define LC_PT(name_)                                                                                                   \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::program,                                                              \
			::fast_io_i18n::lcblob::src_lang::pct_time                                                                 \
	}
#define LC_PG(name_)                                                                                                   \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::program,                                                              \
			::fast_io_i18n::lcblob::src_lang::pct_generic                                                              \
	}
#define LC_I3(name_)                                                                                                   \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::int3, {}                                                              \
	}
#define LC_EL(name_)                                                                                                   \
	{                                                                                                                  \
		name_, ::fast_io_i18n::lcblob::slot_tag::eralist, {}                                                           \
	}

inline constexpr ::fast_io_i18n::lcblob::field_def identification_fields[]{
	LC_S("title"), LC_S("source"), LC_S("address"), LC_S("contact"),
	LC_S("email"), LC_S("tel"), LC_S("fax"), LC_S("language"),
	LC_S("territory"), LC_S("audience"), LC_S("application"),
	LC_S("abbreviation"), LC_S("revision"), LC_S("date"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def ctype_fields[]{
	LC_S("codeset"), // v1: charset name only
};

inline constexpr ::fast_io_i18n::lcblob::field_def collate_fields[]{
	LC_I("collation"), // 1 = codepoint
};

inline constexpr ::fast_io_i18n::lcblob::field_def time_fields[]{
	LC_SL("abday"), LC_SL("day"), LC_SL("abmon"), LC_SL("ab_alt_mon"),
	LC_SL("mon"), LC_SL("alt_mon"), LC_PT("d_t_fmt"), LC_PT("d_fmt"), LC_PT("t_fmt"),
	LC_PT("t_fmt_ampm"), LC_PT("date_fmt"), LC_SL("am_pm"),
	LC_EL("era"), LC_PT("era_d_fmt"), LC_PT("era_d_t_fmt"),
	LC_PT("era_t_fmt"), LC_SL("alt_digits"), LC_I3("week"),
	LC_I("first_weekday"), LC_I("first_workday"), LC_I("cal_direction"),
	LC_SL("timezone"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def numeric_fields[]{
	LC_S("decimal_point"), LC_S("thousands_sep"), LC_B("grouping"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def monetary_fields[]{
	LC_S("int_curr_symbol"), LC_S("currency_symbol"), LC_S("mon_decimal_point"),
	LC_S("mon_thousands_sep"), LC_B("mon_grouping"), LC_S("positive_sign"),
	LC_S("negative_sign"), LC_I("int_frac_digits"), LC_I("frac_digits"),
	LC_I("p_cs_precedes"), LC_I("p_sep_by_space"), LC_I("n_cs_precedes"),
	LC_I("n_sep_by_space"), LC_I("int_p_cs_precedes"), LC_I("int_p_sep_by_space"),
	LC_I("int_n_cs_precedes"), LC_I("int_n_sep_by_space"), LC_I("p_sign_posn"),
	LC_I("n_sign_posn"), LC_I("int_p_sign_posn"), LC_I("int_n_sign_posn"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def messages_fields[]{
	LC_S("yesexpr"), LC_S("noexpr"), LC_S("yesstr"), LC_S("nostr"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def paper_fields[]{
	LC_I("height"), LC_I("width"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def name_fields[]{
	LC_PG("name_fmt"), LC_S("name_gen"), LC_S("name_miss"),
	LC_S("name_mr"), LC_S("name_mrs"), LC_S("name_ms"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def address_fields[]{
	LC_PG("postal_fmt"), LC_S("country_name"), LC_S("country_post"),
	LC_S("country_ab2"), LC_S("country_ab3"), LC_I("country_num"),
	LC_S("country_car"), LC_S("country_isbn"), LC_S("lang_name"),
	LC_S("lang_ab"), LC_S("lang_term"), LC_S("lang_lib"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def telephone_fields[]{
	LC_PG("tel_int_fmt"), LC_PG("tel_dom_fmt"), LC_S("int_select"),
	LC_S("int_prefix"),
};

inline constexpr ::fast_io_i18n::lcblob::field_def measurement_fields[]{
	LC_I("measurement"),
};

struct cat_schema
{
	::std::span<::fast_io_i18n::lcblob::field_def const> fields;
	char const *glibc_name;
};

inline constexpr ::fast_io_i18n::lcblob::cat_schema cat_schemas[cat_count]{
	{identification_fields, "LC_IDENTIFICATION"}, {ctype_fields, "LC_CTYPE"},
	{collate_fields, "LC_COLLATE"},		 {time_fields, "LC_TIME"},
	{numeric_fields, "LC_NUMERIC"},		 {monetary_fields, "LC_MONETARY"},
	{messages_fields, "LC_MESSAGES"},	 {paper_fields, "LC_PAPER"},
	{name_fields, "LC_NAME"},		 {address_fields, "LC_ADDRESS"},
	{telephone_fields, "LC_TELEPHONE"},	 {measurement_fields, "LC_MEASUREMENT"},
};

#undef LC_S
#undef LC_SL
#undef LC_I
#undef LC_B
#undef LC_PT
#undef LC_PG
#undef LC_I3
#undef LC_EL

// ---------------------------------------------------------------------------
// charset of the container's payload bytes
// ---------------------------------------------------------------------------

// payload encoding of one blob section — not to be confused with the
// section selector in the consumer header
enum class blob_charset : ::std::uint_least8_t
{
	utf8 = 0,
	utf16 = 1, // LE
	utf32 = 2, // LE
	gb18030 = 3,
	utf_ebcdic = 4,
};

inline constexpr ::fast_io::u8string_view blob_charset_name(blob_charset cs) noexcept
{
	switch (cs)
	{
	case blob_charset::utf8:
		return u8"UTF-8";
	case blob_charset::utf16:
		return u8"UTF-16";
	case blob_charset::utf32:
		return u8"UTF-32";
	case blob_charset::gb18030:
		return u8"GB18030";
	default:
		return u8"UTF-EBCDIC";
	}
}

// u8 source -> section payload bytes
inline ::fast_io::string transcode(::std::u8string_view sv, blob_charset cs) throws
{
	::fast_io::string out;
	switch (cs)
	{
	case blob_charset::utf8:
		out.append(reinterpret_cast<char const *>(sv.data()), sv.size());
		return out;
	case blob_charset::utf16:
	{
		::fast_io::u16string u16;
		::fast_io::u16ostring_ref_fast_io ref{__builtin_addressof(u16)};
		::fast_io::print(
			ref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
						      ::fast_io::encoding_scheme::utf_le>(
				 ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		out.append(reinterpret_cast<char const *>(u16.data()), u16.size() * 2);
		return out;
	}
	case blob_charset::utf32:
	{
		::fast_io::u32string u32;
		::fast_io::u32ostring_ref_fast_io ref{__builtin_addressof(u32)};
		::fast_io::print(
			ref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
						      ::fast_io::encoding_scheme::utf_le>(
				 ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		out.append(reinterpret_cast<char const *>(u32.data()), u32.size() * 4);
		return out;
	}
	case blob_charset::gb18030:
	{
		::fast_io::ostring_ref_fast_io oref{__builtin_addressof(out)};
		::fast_io::print(
			oref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
					       ::fast_io::encoding_scheme::gb18030>(
				  ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		return out;
	}
	default: // utf_ebcdic — a byte transform of the utf8 form
	{
		::fast_io::ostring_ref_fast_io oref{__builtin_addressof(out)};
		::fast_io::print(
			oref, ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
					       ::fast_io::encoding_scheme::utf_ebcdic>(
				  ::fast_io::basic_io_scatter_t<char8_t>{sv.data(), sv.size()}));
		return out;
	}
	}
}

// ---------------------------------------------------------------------------
// program transcode — walk a compiled UTF-8 program and re-emit with all
// text payloads (literal, fill, chrono/element bodies) in charset cs
// ---------------------------------------------------------------------------

namespace details
{

inline void transcode_nodes(::fast_io_i18n::binfmt::reader &r,
			    ::fast_io_i18n::binfmt::encoder &out, blob_charset cs,
			    unsigned depth) throws;

// copy a node whose tag was just read, byte-identical [start, cur)
inline void copy_node_tail(::fast_io_i18n::binfmt::reader &r, char8_t const *start,
			   ::fast_io_i18n::binfmt::node_head h,
			   ::fast_io_i18n::binfmt::encoder &out) throws
{
	::fast_io_i18n::binfmt::skip_payload(r, h);
	out.buf.append(reinterpret_cast<char const *>(start),
		       static_cast<::std::size_t>(r.cur() - start));
}

inline void transcode_field_children(::std::uint_least64_t nchildren,
				     ::fast_io_i18n::binfmt::reader &r,
				     ::fast_io_i18n::binfmt::encoder &out,
				     blob_charset cs, unsigned depth) throws
{
	using namespace ::fast_io_i18n::binfmt;
	for (::std::uint_least64_t k{}; k < nchildren; ++k)
	{
		char8_t const *start{r.cur()};
		node_head h;
		next_tag(r, h);
		if (h.kind == kind_bytes &&
		    h.code == static_cast<::std::uint_least32_t>(field_param::fill))
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			auto tb{transcode({bytes.data(), bytes.size()}, cs)};
			out.node_bytes(h.code, {tb.data(), tb.size()});
		}
		else if (h.kind == kind_bytes &&
			 (h.code == static_cast<::std::uint_least32_t>(field_param::chrono) ||
			  h.code == static_cast<::std::uint_least32_t>(field_param::element)))
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			reader sub{{bytes.data(), bytes.size()}};
			encoder subout;
			transcode_nodes(sub, subout, cs, depth + 1);
			out.node_bytes(h.code,
				       ::std::string_view{subout.buf.data(), subout.buf.size()});
		}
		else
		{
			copy_node_tail(r, start, h, out);
		}
	}
}

inline void transcode_nodes(::fast_io_i18n::binfmt::reader &r,
			    ::fast_io_i18n::binfmt::encoder &out, blob_charset cs,
			    unsigned depth) throws
{
	using namespace ::fast_io_i18n::binfmt;
	if (depth > 8)
	{
		throw throws ::std::errc::invalid_argument;
	}
	while (!r.empty())
	{
		char8_t const *start{r.cur()};
		node_head h;
		next_tag(r, h);
		if (h.code == op_literal && h.kind == kind_bytes)
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			::fast_io::u8string_view bytes;
			r.get_bytes(n, bytes);
			auto tb{transcode({bytes.data(), bytes.size()}, cs)};
			out.literal({tb.data(), tb.size()});
		}
		else if (h.code == op_field && h.kind == kind_list)
		{
			::std::uint_least64_t n{};
			r.get_leb(n);
			// emit children into a side buffer: the out list node
			// needs the child count first
			encoder sub;
			transcode_field_children(n, r, sub, cs, depth);
			out.node_list_begin(op_field, n);
			out.buf.append(sub.buf.data(), sub.buf.size());
		}
		else
		{
			copy_node_tail(r, start, h, out);
		}
	}
}

} // namespace details

// transcode every text payload of a compiled program into charset cs
inline ::fast_io::string transcode_program(::std::string_view prog,
					   blob_charset cs) throws
{
	if (cs == blob_charset::utf8)
	{
		::fast_io::string r;
		r.append(prog.data(), prog.size());
		return r;
	}
	::fast_io_i18n::binfmt::reader r{{prog.data(), prog.size()}};
	::fast_io_i18n::binfmt::encoder out;
	details::transcode_nodes(r, out, cs, 0);
	return ::std::move(out.buf);
}

// ---------------------------------------------------------------------------
// builder
// ---------------------------------------------------------------------------

struct era_src
{
	::std::int_least64_t direction{};
	::std::int_least64_t offset{};
	::std::int_least64_t start_year{};
	::std::int_least32_t start_month{};
	::std::int_least32_t start_day{};
	::std::int_least64_t end_year{};
	::std::int_least32_t end_month{};
	::std::int_least32_t end_day{};
	::std::u8string_view name{};
	::std::u8string_view fmt{}; // pct program source text
};

struct slot_src
{
	// STRING: strs[0]; STRLIST: all entries
	::fast_io::vector<::std::u8string_view> strs{};
	// INT ints[0]; INT3 ints[0..2]; BYTES ints as i8 values
	::fast_io::vector<::std::int_least64_t> ints{};
	// PROGRAM source text
	::std::u8string_view fmt_src{};
	// ERALIST
	::fast_io::vector<era_src> eras{};
};

struct cat_src
{
	bool present{false};
	::fast_io::vector<slot_src> slots{}; // sized to schema fields
};

namespace details
{

inline ::std::size_t uleb_len(::std::uint_least64_t v) noexcept
{
	::std::size_t n{1};
	for (; v >>= 7; ++n)
	{
	}
	return n;
}

inline void put_u32(::fast_io::string &buf, ::std::uint_least32_t v) noexcept
{
	char b[4];
	::fast_io::details::my_memcpy(b, __builtin_addressof(v), 4);
	buf.append(b, 4);
}

// pool builder — payloads + rva bookkeeping for one fixpoint pass
struct pool_builder
{
	::fast_io::string pool{};
	::std::uint_least32_t base{}; // rva where pool[0] lands in the blob

	::std::uint_least32_t here() const noexcept
	{
		return base + static_cast<::std::uint_least32_t>(pool.size());
	}
	::std::uint_least32_t intern(::fast_io::string const &bytes) noexcept
	{
		auto rva{here()};
		pool.append(bytes.data(), bytes.size());
		return rva;
	}
	void strref(::std::uint_least32_t rva, ::std::size_t len) noexcept
	{
		::fast_io_i18n::binfmt::put_leb128_to(pool, rva);
		::fast_io_i18n::binfmt::put_leb128_to(pool, len);
	}
};

// build one slot's pool payload; returns the pool rva where the payload
// starts (0 for inline-tag slots that carry no pool data)
// record bytes for one slot; payload_rva valid for
// string/strlist/bytes/program/eralist slots
inline ::fast_io::string emit_record(field_def def, slot_src const &s,
				     ::std::uint_least32_t payload_rva,
				     ::std::size_t payload_len) noexcept
{
	using ::fast_io_i18n::binfmt::put_leb128_to;
	::fast_io::string rec;
	put_leb128_to(rec, static_cast<::std::uint_least64_t>(def.tag));
	switch (def.tag)
	{
	case slot_tag::string:
	case slot_tag::bytes:
	case slot_tag::program:
		put_leb128_to(rec, payload_rva);
		put_leb128_to(rec, payload_len);
		break;
	case slot_tag::strlist:
	case slot_tag::eralist:
		put_leb128_to(rec, payload_rva);
		break;
	case slot_tag::integer:
		put_leb128_to(rec, s.ints[0]);
		break;
	case slot_tag::int3:
		put_leb128_to(rec, s.ints[0]);
		put_leb128_to(rec, s.ints[1]);
		put_leb128_to(rec, s.ints[2]);
		break;
	default:
		break;
	}
	return rec;
}

inline bool slot_has(slot_tag tag, slot_src const &s) noexcept
{
	switch (tag)
	{
	case slot_tag::string:
	case slot_tag::strlist:
		return !s.strs.empty();
	case slot_tag::bytes:
	case slot_tag::integer:
	case slot_tag::int3:
		return !s.ints.empty();
	case slot_tag::program:
		return !s.fmt_src.empty();
	case slot_tag::eralist:
		return !s.eras.empty();
	default:
		return false;
	}
}

// emit one slot's pool payload; returns pool rva (inline-tag slots return 0).
// For string/program the transcoded bytes are returned in `tc` so the record
// can carry the length.
inline ::std::uint_least32_t emit_slot_payload(
	pool_builder &p, field_def def, slot_src const &s, blob_charset cs,
	::fast_io::string &tc, ::fast_io_i18n::binfmt::src_ctx ctx) throws
{
	using ::fast_io_i18n::binfmt::put_leb128_to;
	switch (def.tag)
	{
	case slot_tag::string:
		tc = transcode(s.strs[0], cs);
		return p.intern(tc);
	case slot_tag::bytes:
	{
		auto rva{p.here()};
		for (auto v : s.ints)
		{
			p.pool.push_back(static_cast<char>(v & 0xFF));
		}
		return rva;
	}
	case slot_tag::program:
	{
		// a rejected directive drops the slot, not the locale
		try
		{
			auto prog{def.lang == src_lang::pct_generic
					  ? ::fast_io_i18n::binfmt::compile_generic_pct(
						    s.fmt_src, ctx)
					  : ::fast_io_i18n::binfmt::compile_time(
						    s.fmt_src, ctx)};
			tc = transcode_program({prog.data(), prog.size()}, cs);
			return p.intern(tc);
		}
		catch throws(::std::error e)
		{
			::fast_io::perrln("  skip program `", ::std::string_view{def.name}, "' in ",
					  ctx.where);
			return 0;
		}
	}
	case slot_tag::strlist:
	{
		// element strings first (collect rvas + transcoded lens),
		// then count|strref*
		::fast_io::vector<::std::uint_least32_t> rvas;
		::fast_io::vector<::std::size_t> lens;
		for (auto e : s.strs)
		{
			auto t{transcode(e, cs)};
			lens.push_back(t.size());
			rvas.push_back(p.intern(::std::move(t)));
		}
		auto rva{p.here()};
		put_leb128_to(p.pool, rvas.size());
		auto it{rvas.cbegin()};
		for (auto l : lens)
		{
			p.strref(*it++, l);
		}
		return rva;
	}
	case slot_tag::eralist:
	{
		// era names + fmts first (strrefs point at them)
		::fast_io::vector<::std::uint_least32_t> name_rvas;
		::fast_io::vector<::std::size_t> name_lens;
		::fast_io::vector<::std::uint_least32_t> fmt_rvas;
		::fast_io::vector<::fast_io::string> fmts;
		try
		{
			for (auto const &e : s.eras)
			{
				auto t{transcode(e.name, cs)};
				name_lens.push_back(t.size());
				name_rvas.push_back(p.intern(::std::move(t)));
				auto prog{::fast_io_i18n::binfmt::compile_time(e.fmt, ctx)};
				fmts.emplace_back(transcode_program(
					{prog.data(), prog.size()}, cs));
				fmt_rvas.push_back(p.intern(fmts.back()));
			}
		}
		catch throws(::std::error e)
		{
			::fast_io::perrln("  skip `era' in ", ctx.where);
			return 0;
		}
		auto rva{p.here()};
		put_leb128_to(p.pool, s.eras.size());
		auto ni{name_rvas.cbegin()};
		auto nl{name_lens.cbegin()};
		auto fi{fmt_rvas.cbegin()};
		auto ti{fmts.cbegin()};
		for (auto const &e : s.eras)
		{
			put_leb128_to(p.pool, e.direction);
			put_leb128_to(p.pool, e.offset);
			put_leb128_to(p.pool, e.start_year);
			put_leb128_to(p.pool, static_cast<::std::uint_least32_t>(e.start_month));
			put_leb128_to(p.pool, static_cast<::std::uint_least32_t>(e.start_day));
			put_leb128_to(p.pool, e.end_year);
			put_leb128_to(p.pool, static_cast<::std::uint_least32_t>(e.end_month));
			put_leb128_to(p.pool, static_cast<::std::uint_least32_t>(e.end_day));
			p.strref(*ni++, *nl++);
			p.strref(*fi++, ti->size());
			++ti;
		}
		return rva;
	}
	default:
		return 0;
	}
}

} // namespace details

// ---------------------------------------------------------------------------
// emit — fixpoint over hdr_size: pool strrefs and record RVAs all depend
// on where the pool lands, which depends on the header's own size
// ---------------------------------------------------------------------------

inline ::fast_io::string build_blob(cat_src const cats[cat_count],
				    ::std::u8string_view name, blob_charset cs,
				    ::std::string_view file_ctx) throws
{
	using ::fast_io_i18n::binfmt::put_leb128_to;
	auto enc_name{blob_charset_name(cs)};

	::fast_io::string name_tc{transcode(name, cs)};
	::fast_io::string enc_tc{transcode(
		{reinterpret_cast<char8_t const *>(enc_name.data()), enc_name.size()}, cs)};

	struct slot_info
	{
		::std::uint_least32_t rva{};
		::std::size_t len{};
	};

	::std::uint_least32_t hdr_size{24}; // seed
	for (unsigned iter{};; ++iter)
	{
		if (iter > 8)
		{
			throw throws ::std::errc::invalid_argument; // fixpoint diverged
		}
		::std::uint_least32_t const pool_base{hdr_size + 48};
		details::pool_builder p;
		p.base = pool_base;

		// header strrefs are pool entries too — name first, encoding second
		auto const name_rva{p.intern(name_tc)};
		auto const enc_rva{p.intern(enc_tc)};

		::fast_io::vector<slot_info> infos[cat_count];
		for (::std::size_t c{}; c < cat_count; ++c)
		{
			if (!cats[c].present)
			{
				continue;
			}
			auto const &schema{cat_schemas[c]};
			for (::std::size_t f{}; f < schema.fields.size(); ++f)
			{
				auto const &def{schema.fields[f]};
				auto const &s{cats[c].slots[f]};
				slot_info si;
				if (details::slot_has(def.tag, s))
				{
					::fast_io::string tc;
					si.rva = details::emit_slot_payload(
						p, def, s, cs, tc, {file_ctx});
					si.len = p.here() - si.rva;
				}
				infos[c].push_back(si);
			}
		}

		// records — keep offsets only; the records base is patched at emit
		::fast_io::string records;
		::fast_io::vector<::std::uint_least32_t> rec_off[cat_count];
		for (::std::size_t c{}; c < cat_count; ++c)
		{
			if (!cats[c].present)
			{
				continue;
			}
			auto const &schema{cat_schemas[c]};
			for (::std::size_t f{}; f < schema.fields.size(); ++f)
			{
				auto const &si{infos[c][f]};
				bool inline_int{schema.fields[f].tag == slot_tag::integer ||
						schema.fields[f].tag == slot_tag::int3};
				if (!si.rva && !inline_int)
				{
					rec_off[c].push_back(0);
					continue;
				}
				if (!details::slot_has(schema.fields[f].tag, cats[c].slots[f]))
				{
					rec_off[c].push_back(0);
					continue;
				}
				rec_off[c].push_back(
					static_cast<::std::uint_least32_t>(records.size()));
				auto rec{details::emit_record(schema.fields[f], cats[c].slots[f],
							      si.rva, si.len)};
				records.append(rec.data(), rec.size());
			}
		}

		::std::uint_least32_t slots_size{};
		for (::std::size_t c{}; c < cat_count; ++c)
		{
			if (cats[c].present)
			{
				slots_size += static_cast<::std::uint_least32_t>(
					cat_schemas[c].fields.size() * 4);
			}
		}

		// hdr_size = 4 + uleb(version) + uleb(total) + 1(flags)
		//          + uleb(name_rva) + uleb(name_len)
		//          + uleb(enc_rva) + uleb(enc_len) + uleb(cat_dir_rva)
		// cat_dir_rva = hdr_size; pool rvas = hdr_size + 48 + off.
		// Self-referential — inner fixpoint.
		::std::uint_least32_t h{};
		for (;;)
		{
			::std::uint_least64_t total{static_cast<::std::uint_least64_t>(h) + 48 +
						    p.pool.size() + slots_size + records.size()};
			::std::uint_least32_t nh{
				4 +
				static_cast<::std::uint_least32_t>(details::uleb_len(blob_version)) +
				static_cast<::std::uint_least32_t>(details::uleb_len(total)) +
				1 +
				static_cast<::std::uint_least32_t>(details::uleb_len(name_rva)) +
				static_cast<::std::uint_least32_t>(details::uleb_len(name_tc.size())) +
				static_cast<::std::uint_least32_t>(details::uleb_len(enc_rva)) +
				static_cast<::std::uint_least32_t>(details::uleb_len(enc_tc.size())) +
				static_cast<::std::uint_least32_t>(details::uleb_len(h))};
			if (nh == h)
			{
				break;
			}
			h = nh;
		}
		if (h != hdr_size)
		{
			hdr_size = h;
			continue; // pool_base moved — rebuild
		}

		// stable — emit
		::std::uint_least64_t const total{static_cast<::std::uint_least64_t>(hdr_size) +
						  48 + p.pool.size() + slots_size +
						  records.size()};
		::fast_io::string blob;
		details::put_u32(blob, magic);
		put_leb128_to(blob, blob_version);
		put_leb128_to(blob, total);
		put_leb128_to(blob, 0); // flags
		put_leb128_to(blob, name_rva);
		put_leb128_to(blob, name_tc.size());
		put_leb128_to(blob, enc_rva);
		put_leb128_to(blob, enc_tc.size());
		put_leb128_to(blob, hdr_size); // cat_dir_rva
		::std::uint_least32_t const tables_base{hdr_size + 48 +
							static_cast<::std::uint_least32_t>(
								p.pool.size())};
		{
			::std::uint_least32_t off{tables_base};
			for (::std::size_t c{}; c < cat_count; ++c)
			{
				details::put_u32(blob, cats[c].present ? off : 0);
				if (cats[c].present)
				{
					off += static_cast<::std::uint_least32_t>(
						cat_schemas[c].fields.size() * 4);
				}
			}
		}
		blob.append(p.pool.data(), p.pool.size());
		::std::uint_least32_t const records_base{tables_base + slots_size};
		for (::std::size_t c{}; c < cat_count; ++c)
		{
			if (!cats[c].present)
			{
				continue;
			}
			for (::std::size_t f{}; f < cat_schemas[c].fields.size(); ++f)
			{
				auto const ro{rec_off[c][f]};
				details::put_u32(blob, ro ? records_base + ro : 0);
			}
		}
		blob.append(records.data(), records.size());
		return blob;
	}
}
// ---------------------------------------------------------------------------
// header decode (dump/accessor side)
// ---------------------------------------------------------------------------

struct blob_header
{
	::std::uint_least64_t version{};
	::std::uint_least64_t total_size{};
	::std::uint_least64_t flags{};
	::fast_io::u8string_view name{};
	::fast_io::u8string_view encoding{};
	::std::uint_least64_t cat_dir_rva{};
	char8_t const *cat_dir{}; // -> u32[cat_count]
};

inline blob_header read_header(::fast_io::u8string_view blob) throws
{
	if (blob.size() < 4)
	{
		throw throws ::std::errc::invalid_argument;
	}
	::std::uint_least32_t m{};
	::fast_io::details::my_memcpy(__builtin_addressof(m), blob.data(), 4);
	if (m != magic)
	{
		throw throws ::std::errc::invalid_argument;
	}
	::fast_io_i18n::binfmt::reader r{{blob.data() + 4, blob.size() - 4}};
	blob_header h;
	r.get_leb(h.version);
	if (h.version > blob_version)
	{
		throw throws ::std::errc::invalid_argument;
	}
	r.get_leb(h.total_size);
	r.get_leb(h.flags);
	::std::uint_least64_t rva{}, len{};
	r.get_leb(rva);
	r.get_leb(len);
	if (rva + len > blob.size())
	{
		throw throws ::std::errc::invalid_argument;
	}
	h.name = ::fast_io::u8string_view{blob.data() + rva, len};
	r.get_leb(rva);
	r.get_leb(len);
	if (rva + len > blob.size())
	{
		throw throws ::std::errc::invalid_argument;
	}
	h.encoding = ::fast_io::u8string_view{blob.data() + rva, len};
	r.get_leb(h.cat_dir_rva);
	if (h.cat_dir_rva + cat_count * 4 > blob.size())
	{
		throw throws ::std::errc::invalid_argument;
	}
	h.cat_dir = blob.data() + h.cat_dir_rva;
	return h;
}

inline ::std::uint_least32_t read_u32(char8_t const *p) noexcept
{
	::std::uint_least32_t v{};
	::fast_io::details::my_memcpy(__builtin_addressof(v), p, 4);
	return v;
}

// ---------------------------------------------------------------------------
// container — one file per locale carrying all three charset sections.
//   u32 magic 'FCL1' | uleb version | uleb total_size | uleb flags
//   | strref name | strref encoding            (utf8; targets live in
//   |                                          the utf8 section's pool)
//   | (uleb rva | uleb size) * blob_charset_count
// Each section is a complete standalone v1 blob — the same read_header
// validates it and all its RVAs are section-relative.
// ---------------------------------------------------------------------------

// Build the outer container. sections are the charset, utf8, utf16 and
// utf32 blobs in that order; an EMPTY charset section aliases the utf8
// one (the codeset IS utf8). name/encoding are stored as utf8 in a
// small pool right after the section directory.
inline ::fast_io::string build_container(::fast_io::string const (&sections)[4],
					 ::std::u8string_view name,
					 ::std::u8string_view encoding) throws
{
	using ::fast_io_i18n::binfmt::put_leb128_to;
	::std::size_t const nsec{sections[0].empty() ? 3zu : 4zu};
	::std::uint_least64_t secsz[4]{sections[0].empty() ? sections[1].size() : sections[0].size(),
								   sections[1].size(), sections[2].size(), sections[3].size()};
	::std::uint_least64_t const name_len{name.size()};
	::std::uint_least64_t const enc_len{encoding.size()};

	::std::uint_least32_t hdr_size{32}; // seed
	for (unsigned iter{};; ++iter)
	{
		if (iter > 8)
		{
			throw throws ::std::errc::invalid_argument;
		}
		::std::uint_least64_t const pool_rva{hdr_size};
		::std::uint_least64_t const name_rva{pool_rva};
		::std::uint_least64_t const enc_rva{pool_rva + name_len};
		::std::uint_least64_t sec_rva[4];
		::std::uint_least64_t off{pool_rva + name_len + enc_len};
		::std::uint_least64_t sidx{nsec == 3 ? 1zu : 0zu};
		::std::uint_least64_t off0{off};
		sec_rva[0] = nsec == 3 ? off0 : off; // charset slot aliases utf8 when nsec==3
		if (nsec == 4)
		{
			sec_rva[0] = off;
			off += secsz[0];
		}
		for (::std::size_t c{1}; c < 4; ++c)
		{
			sec_rva[c] = off;
			off += secsz[c];
		}
		if (nsec == 3)
		{
			sec_rva[0] = sec_rva[1];
		}
		::std::uint_least64_t const total{off};
		::std::uint_least32_t nh{
			4 + static_cast<::std::uint_least32_t>(details::uleb_len(blob_version)) +
			static_cast<::std::uint_least32_t>(details::uleb_len(total)) + 1 +
			static_cast<::std::uint_least32_t>(details::uleb_len(name_rva)) +
			static_cast<::std::uint_least32_t>(details::uleb_len(name_len)) +
			static_cast<::std::uint_least32_t>(details::uleb_len(enc_rva)) +
			static_cast<::std::uint_least32_t>(details::uleb_len(enc_len))};
		for (::std::size_t c{}; c < 4; ++c)
		{
			nh += static_cast<::std::uint_least32_t>(
				details::uleb_len(sec_rva[c]) + details::uleb_len(secsz[c]));
		}
		if (nh == hdr_size)
		{
			// emit
			::fast_io::string blob;
			details::put_u32(blob, magic);
			put_leb128_to(blob, blob_version);
			put_leb128_to(blob, total);
			put_leb128_to(blob, 0); // flags
			put_leb128_to(blob, name_rva);
			put_leb128_to(blob, name_len);
			put_leb128_to(blob, enc_rva);
			put_leb128_to(blob, enc_len);
			for (::std::size_t c{}; c < 4; ++c)
			{
				put_leb128_to(blob, sec_rva[c]);
				put_leb128_to(blob, secsz[c]);
			}
			blob.append(reinterpret_cast<char const *>(name.data()), name.size());
			blob.append(reinterpret_cast<char const *>(encoding.data()), encoding.size());
			for (::std::size_t c{sidx}; c < 4; ++c)
			{
				blob.append(sections[c].data(), sections[c].size());
			}
			return blob;
		}
		hdr_size = nh;
	}
}


} // namespace lcblob
} // namespace fast_io_i18n
