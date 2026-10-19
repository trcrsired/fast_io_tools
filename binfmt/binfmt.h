#pragma once

// binfmt — the wire format: a compiled binary *program* representing what a
// format string would do. See spec.md in this directory.
//
// This header is the single definition of the format. It is deliberately
// just numeric codes + framing helpers — no containers, no fast_io deps —
// so both the compilers (binfmt_compile.h) and the runtime
// (binfmt_print.h) agree on one opcode space.
//
// Everything here is `fast_io::binfmt`. There is NO text format string at
// any point: fmt and C printf are only *front-ends* that lower to these
// nodes. Nodes are `[uleb128 tag][payload]`, tag = (code << 3) | kind.
// A program is a bare node sequence; the embedder's bound ends it.
//
// Herbceptions only: everything that reports an error throws
// std::errc via `throw throws`.

#if !defined(__HERBCEPTIONS__)
#error "fast_io::binfmt requires herbceptions (-fherbceptions); legacy C++ exceptions are not supported"
#endif

#include <cstddef>
#include <cstdint>

namespace fast_io::binfmt
{

// ---------------------------------------------------------------------------
// node framing — tag = (code << 3) | kind (kind is 3 bits; 5-7 reserved)
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t binfmt_kind_none{0};
inline constexpr ::std::uint_least32_t binfmt_kind_uleb{1};
inline constexpr ::std::uint_least32_t binfmt_kind_bytes{2};
inline constexpr ::std::uint_least32_t binfmt_kind_list{3};
inline constexpr ::std::uint_least32_t binfmt_kind_sleb{4};

// ---------------------------------------------------------------------------
// top-level ops (spec Part 2)
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t binfmt_op_literal{1};
inline constexpr ::std::uint_least32_t binfmt_op_field{2};
inline constexpr ::std::uint_least32_t binfmt_op_pct{3};
inline constexpr ::std::uint_least32_t binfmt_op_plural{0x40};
inline constexpr ::std::uint_least32_t binfmt_op_plural_count{0x41};
inline constexpr ::std::uint_least32_t binfmt_op_plural_form{0x42};

// ---------------------------------------------------------------------------
// field child params (spec Part 3)
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t binfmt_fp_arg{1};
inline constexpr ::std::uint_least32_t binfmt_fp_fill{2};
inline constexpr ::std::uint_least32_t binfmt_fp_align{3};
inline constexpr ::std::uint_least32_t binfmt_fp_sign{4};
inline constexpr ::std::uint_least32_t binfmt_fp_flag_alt{5};
inline constexpr ::std::uint_least32_t binfmt_fp_flag_zero{6};
inline constexpr ::std::uint_least32_t binfmt_fp_flag_group{7};
inline constexpr ::std::uint_least32_t binfmt_fp_flag_locale{8};
inline constexpr ::std::uint_least32_t binfmt_fp_flag_outdigits{9};
inline constexpr ::std::uint_least32_t binfmt_fp_width{10};
inline constexpr ::std::uint_least32_t binfmt_fp_prec{11};
inline constexpr ::std::uint_least32_t binfmt_fp_ctype{12};
inline constexpr ::std::uint_least32_t binfmt_fp_type{13};
inline constexpr ::std::uint_least32_t binfmt_fp_chrono{14};
inline constexpr ::std::uint_least32_t binfmt_fp_element{15};
inline constexpr ::std::uint_least32_t binfmt_fp_flag_upper{16};

// ---------------------------------------------------------------------------
// conv_type / type enum (spec Part 3)
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t binfmt_conv_d{1};
inline constexpr ::std::uint_least32_t binfmt_conv_u{2};
inline constexpr ::std::uint_least32_t binfmt_conv_o{3};
inline constexpr ::std::uint_least32_t binfmt_conv_x{4};
inline constexpr ::std::uint_least32_t binfmt_conv_b{5};
inline constexpr ::std::uint_least32_t binfmt_conv_c{6};
inline constexpr ::std::uint_least32_t binfmt_conv_s{7};
inline constexpr ::std::uint_least32_t binfmt_conv_debug{8};
inline constexpr ::std::uint_least32_t binfmt_conv_p{9};
inline constexpr ::std::uint_least32_t binfmt_conv_addr{10};
inline constexpr ::std::uint_least32_t binfmt_conv_mth{11};
inline constexpr ::std::uint_least32_t binfmt_conv_dec{12};
inline constexpr ::std::uint_least32_t binfmt_conv_decp{13};
inline constexpr ::std::uint_least32_t binfmt_conv_fix{14};
inline constexpr ::std::uint_least32_t binfmt_conv_fixp{15};
inline constexpr ::std::uint_least32_t binfmt_conv_sci{16};
inline constexpr ::std::uint_least32_t binfmt_conv_scip{17};
inline constexpr ::std::uint_least32_t binfmt_conv_gen{18};
inline constexpr ::std::uint_least32_t binfmt_conv_genp{19};
inline constexpr ::std::uint_least32_t binfmt_conv_hexf{20};
inline constexpr ::std::uint_least32_t binfmt_conv_hexfp{21};
inline constexpr ::std::uint_least32_t binfmt_conv_rng{22};
inline constexpr ::std::uint_least32_t binfmt_conv_rngn{23};
inline constexpr ::std::uint_least32_t binfmt_conv_rngm{24};

// ---------------------------------------------------------------------------
// ctype enum (spec Part 3) — declared C arg type, blob metadata
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t binfmt_ct_other{0};
inline constexpr ::std::uint_least32_t binfmt_ct_i32{1};
inline constexpr ::std::uint_least32_t binfmt_ct_i64{2};
inline constexpr ::std::uint_least32_t binfmt_ct_i128{3};
inline constexpr ::std::uint_least32_t binfmt_ct_u32{4};
inline constexpr ::std::uint_least32_t binfmt_ct_u64{5};
inline constexpr ::std::uint_least32_t binfmt_ct_u128{6};
inline constexpr ::std::uint_least32_t binfmt_ct_f16{7};
inline constexpr ::std::uint_least32_t binfmt_ct_bf16{8};
inline constexpr ::std::uint_least32_t binfmt_ct_f32{9};
inline constexpr ::std::uint_least32_t binfmt_ct_f64{10};
inline constexpr ::std::uint_least32_t binfmt_ct_f80{11};
inline constexpr ::std::uint_least32_t binfmt_ct_f128{12};
inline constexpr ::std::uint_least32_t binfmt_ct_cf16{13};
inline constexpr ::std::uint_least32_t binfmt_ct_cbf16{14};
inline constexpr ::std::uint_least32_t binfmt_ct_cf32{15};
inline constexpr ::std::uint_least32_t binfmt_ct_cf64{16};
inline constexpr ::std::uint_least32_t binfmt_ct_cf80{17};
inline constexpr ::std::uint_least32_t binfmt_ct_cf128{18};
inline constexpr ::std::uint_least32_t binfmt_ct_c8{19};
inline constexpr ::std::uint_least32_t binfmt_ct_c16{20};
inline constexpr ::std::uint_least32_t binfmt_ct_c32{21};
inline constexpr ::std::uint_least32_t binfmt_ct_cebc{22};
inline constexpr ::std::uint_least32_t binfmt_ct_c8ptr{23};
inline constexpr ::std::uint_least32_t binfmt_ct_c16ptr{24};
inline constexpr ::std::uint_least32_t binfmt_ct_c32ptr{25};
inline constexpr ::std::uint_least32_t binfmt_ct_ebcptr{26};
inline constexpr ::std::uint_least32_t binfmt_ct_gbptr{27};
inline constexpr ::std::uint_least32_t binfmt_ct_c8view{28};
inline constexpr ::std::uint_least32_t binfmt_ct_c16view{29};
inline constexpr ::std::uint_least32_t binfmt_ct_c32view{30};
inline constexpr ::std::uint_least32_t binfmt_ct_ebcview{31};
inline constexpr ::std::uint_least32_t binfmt_ct_gbview{32};
inline constexpr ::std::uint_least32_t binfmt_ct_ptr{33};
inline constexpr ::std::uint_least32_t binfmt_ct_fptr{34};
inline constexpr ::std::uint_least32_t binfmt_ct_fldptr_i{35};
inline constexpr ::std::uint_least32_t binfmt_ct_mthptr_i{36};
inline constexpr ::std::uint_least32_t binfmt_ct_fldptr_m1{37};
inline constexpr ::std::uint_least32_t binfmt_ct_fldptr_m2{38};
inline constexpr ::std::uint_least32_t binfmt_ct_mthptr_m1{39};
inline constexpr ::std::uint_least32_t binfmt_ct_mthptr_m2{40};
inline constexpr ::std::uint_least32_t binfmt_ct_mthptr_m3{41};
inline constexpr ::std::uint_least32_t binfmt_ct_mthptr_m4{42};
inline constexpr ::std::uint_least32_t binfmt_ct_error{43};

// ---------------------------------------------------------------------------
// cookie element — how the caller packs one type-erased argument
// ---------------------------------------------------------------------------
//
// The caller passes `elements_count` of these, densely, as one array:
//
//     struct binfmt_cookie_element
//     {
//         binfmt_typetag typetag;
//         ::std::size_t field1;
//         ::std::size_t field2;
//     };
//
// fmt's `{}` is type-erased at the call site, so the format *program* for
// it carries no type; the argument's type therefore rides in typetag. The
// value lives in field1/field2 — 16 contiguous bytes, so a 128-bit integer
// or a {ptr, len} view loads straight out of the pair:
//
//     typetag 0   use the type embedded in the format program (C printf)
//     typetag 1   caller-defined / opaque type
//     typetag >=2 a standard type, equal to its binfmt_ct_* code + 1
//
// So u64 is typetag 6 (binfmt_ct_u64 == 5). There are no per-element
// offsets: element i is simply cookie[i].

enum class binfmt_typetag : ::std::uint_least32_t
{
	from_format = 0,
	custom = 1,
	i32 = binfmt_ct_i32 + 1u,
	i64 = binfmt_ct_i64 + 1u,
	i128 = binfmt_ct_i128 + 1u,
	u32 = binfmt_ct_u32 + 1u,
	u64 = binfmt_ct_u64 + 1u,
	u128 = binfmt_ct_u128 + 1u,
	f16 = binfmt_ct_f16 + 1u,
	bf16 = binfmt_ct_bf16 + 1u,
	f32 = binfmt_ct_f32 + 1u,
	f64 = binfmt_ct_f64 + 1u,
	f80 = binfmt_ct_f80 + 1u,
	f128 = binfmt_ct_f128 + 1u,
	cf16 = binfmt_ct_cf16 + 1u,
	cbf16 = binfmt_ct_cbf16 + 1u,
	cf32 = binfmt_ct_cf32 + 1u,
	cf64 = binfmt_ct_cf64 + 1u,
	cf80 = binfmt_ct_cf80 + 1u,
	cf128 = binfmt_ct_cf128 + 1u,
	c8 = binfmt_ct_c8 + 1u,
	c16 = binfmt_ct_c16 + 1u,
	c32 = binfmt_ct_c32 + 1u,
	cebc = binfmt_ct_cebc + 1u,
	c8ptr = binfmt_ct_c8ptr + 1u,
	c16ptr = binfmt_ct_c16ptr + 1u,
	c32ptr = binfmt_ct_c32ptr + 1u,
	ebcptr = binfmt_ct_ebcptr + 1u,
	gbptr = binfmt_ct_gbptr + 1u,
	c8view = binfmt_ct_c8view + 1u,
	c16view = binfmt_ct_c16view + 1u,
	c32view = binfmt_ct_c32view + 1u,
	ebcview = binfmt_ct_ebcview + 1u,
	gbview = binfmt_ct_gbview + 1u,
	ptr = binfmt_ct_ptr + 1u,
	fptr = binfmt_ct_fptr + 1u,
	fldptr_i = binfmt_ct_fldptr_i + 1u,
	mthptr_i = binfmt_ct_mthptr_i + 1u,
	fldptr_m1 = binfmt_ct_fldptr_m1 + 1u,
	fldptr_m2 = binfmt_ct_fldptr_m2 + 1u,
	mthptr_m1 = binfmt_ct_mthptr_m1 + 1u,
	mthptr_m2 = binfmt_ct_mthptr_m2 + 1u,
	mthptr_m3 = binfmt_ct_mthptr_m3 + 1u,
	mthptr_m4 = binfmt_ct_mthptr_m4 + 1u,
	error = binfmt_ct_error + 1u,
};

struct binfmt_cookie_element
{
	binfmt_typetag typetag;
	::std::size_t field1;
	::std::size_t field2;
};

// ---------------------------------------------------------------------------
// align / sign operands
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t binfmt_align_left{1};
inline constexpr ::std::uint_least32_t binfmt_align_right{2};
inline constexpr ::std::uint_least32_t binfmt_align_center{3};

inline constexpr ::std::uint_least32_t binfmt_sign_plus{1};
inline constexpr ::std::uint_least32_t binfmt_sign_minus{2};
inline constexpr ::std::uint_least32_t binfmt_sign_space{3};

// ---------------------------------------------------------------------------
// pct child params + pct conv enum (spec Part 4) — locale/time directives
// ---------------------------------------------------------------------------

inline constexpr ::std::uint_least32_t binfmt_pctp_conv{20};
inline constexpr ::std::uint_least32_t binfmt_pctp_letter{21};
inline constexpr ::std::uint_least32_t binfmt_pctp_pad{22};	// bit0 '-' bit1 '_' bit2 '0'
inline constexpr ::std::uint_least32_t binfmt_pctp_case{23};	// bit0 '^' bit1 '#'
inline constexpr ::std::uint_least32_t binfmt_pctp_modifier{24}; // 1 E, 2 O
inline constexpr ::std::uint_least32_t binfmt_pctp_colons{25};

inline constexpr ::std::uint_least32_t binfmt_pct_modifier_era{1};
inline constexpr ::std::uint_least32_t binfmt_pct_modifier_alt{2};

inline constexpr ::std::uint_least32_t binfmt_pct_a{1};
inline constexpr ::std::uint_least32_t binfmt_pct_A{2};
inline constexpr ::std::uint_least32_t binfmt_pct_b{3};
inline constexpr ::std::uint_least32_t binfmt_pct_B{4};
inline constexpr ::std::uint_least32_t binfmt_pct_c{5};
inline constexpr ::std::uint_least32_t binfmt_pct_C{6};
inline constexpr ::std::uint_least32_t binfmt_pct_d{7};
inline constexpr ::std::uint_least32_t binfmt_pct_e{8};
inline constexpr ::std::uint_least32_t binfmt_pct_F{9};
inline constexpr ::std::uint_least32_t binfmt_pct_G{10};
inline constexpr ::std::uint_least32_t binfmt_pct_H{11};
inline constexpr ::std::uint_least32_t binfmt_pct_I{12};
inline constexpr ::std::uint_least32_t binfmt_pct_j{13};
inline constexpr ::std::uint_least32_t binfmt_pct_k{14};
inline constexpr ::std::uint_least32_t binfmt_pct_l{15};
inline constexpr ::std::uint_least32_t binfmt_pct_m{16};
inline constexpr ::std::uint_least32_t binfmt_pct_M{17};
inline constexpr ::std::uint_least32_t binfmt_pct_p{18};
inline constexpr ::std::uint_least32_t binfmt_pct_P{19};
inline constexpr ::std::uint_least32_t binfmt_pct_r{20};
inline constexpr ::std::uint_least32_t binfmt_pct_R{21};
inline constexpr ::std::uint_least32_t binfmt_pct_s{22};
inline constexpr ::std::uint_least32_t binfmt_pct_S{23};
inline constexpr ::std::uint_least32_t binfmt_pct_T{24};
inline constexpr ::std::uint_least32_t binfmt_pct_u{25};
inline constexpr ::std::uint_least32_t binfmt_pct_U{26};
inline constexpr ::std::uint_least32_t binfmt_pct_V{27};
inline constexpr ::std::uint_least32_t binfmt_pct_w{28};
inline constexpr ::std::uint_least32_t binfmt_pct_W{29};
inline constexpr ::std::uint_least32_t binfmt_pct_x{30};
inline constexpr ::std::uint_least32_t binfmt_pct_X{31};
inline constexpr ::std::uint_least32_t binfmt_pct_Y{32};
inline constexpr ::std::uint_least32_t binfmt_pct_z{33};
inline constexpr ::std::uint_least32_t binfmt_pct_Z{34};
inline constexpr ::std::uint_least32_t binfmt_pct_plus{35};
inline constexpr ::std::uint_least32_t binfmt_pct_iso8601{36};
inline constexpr ::std::uint_least32_t binfmt_pct_iso8601_utc{37};
inline constexpr ::std::uint_least32_t binfmt_pct_fracsec{38};

} // namespace fast_io::binfmt
