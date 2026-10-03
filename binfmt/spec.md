# binfmt + lcblob — design spec (v1, for review)

Binary program representation of format strings, plus the flat
position-independent locale container that carries them.

Two pieces:

1. **binfmt** — a compiled *program* representing what a format string
   would do. Text is parsed **once**, offline (localedef/codegen time).
   fast_io never parses a format string at runtime — the only thing that
   executes is a bounds-checked byte program.
2. **lcblob** — flat locale file where every internal reference is an
   **RVA-style u32 offset from the blob base**. No pointers, no
   relocations: mmap'd read-only and shared byte-identical across all
   processes, embeddable in `.rodata`, loadable on targets with no
   DLL support at all. Replaces the old `.cpp`→DLL codegen — pointer
   data in a DLL relocates per-process and every process gets private
   copies of the pages.

## Security model — structural, not enforced

The hazard in format strings is that data becomes code (`%n` writes,
`format(str, str)` double-format, `*`-driven arg consumption, macro/codegen
injection around "compile-time" formats). binfmt removes the class, not
the instances:

- there is no format-string parser at runtime, period;
- the op set cannot express writes (`%n`), lookups (`{name}`), or
  dynamic specs (`*`, `{:{}}`, `!r`) — no codes exist for them;
- an argref is only an index — **arg content can never become format
  code**;
- blobs are trusted compiled artifacts (same trust level as generated
  tables); the decoder bounds-checks structure only — it does not police
  sizes or counts, since nothing encodable can do anything dangerous.

---

## Part 1 — wire encoding

### Integers and text

- Node stream: **LEB128** (`uleb128`/`sleb128`) — tags, lengths, scalar
  values. Locale data is mostly small numbers and markers; fixed u32
  would spend most of its bytes on zeros.
- The **container** (lcblob tables, RVAs, slot records) stays fixed-width
  little-endian u32/i32 — those are random-access tables, not a walked
  stream; decode is a memcpy.
- Text payloads (literals, names, fill chars) are byte strings in the
  container's declared charset (UTF-8 default).
- Structural codes are numeric enums — never character codes. A compiler
  on an EBCDIC host maps its charset at compile time; the blob is
  charset-independent.
- No alignment requirements anywhere.

### Node framing — lisp-like tagged nodes

Every node is `[uleb128 tag][payload]`:

```
uleb128 tag = (code << 2) | payload_kind

kind 0 none    bare marker                    next node at +tagbytes
kind 1 uleb    [uleb128 value]                unsigned scalar
kind 2 bytes   [uleb128 len][data]            a code, a length, a payload
kind 3 list    [uleb128 len][child nodes]     children bounded by len
kind 4 sleb    [sleb128 value]                signed scalar
(kind 5-7 reserved)
```

Every node is self-delimiting: the walker reads the tag varint and hops
— it never inspects payload to find the next node. Unknown codes skip
cleanly via kind, so new codes are forward-compatible. Common codes are
1-byte tags; the encoding spends bytes on data, not on zeros.

### Code space

`code` is unbounded (it lives in the varint tag). Ranges by value:

| range | meaning |
|-------|---------|
| `0x00`–`0x3F` | core codes (this spec) — 1-byte tags |
| `0x40`–`0xFF` | plural expression codes — still 1-byte tags |
| `0x100`–`0x3FFFFF` | reserved — future standard params (2-3-byte tags) |
| `0x400000`+ | reserved — vendor/experimental |

### Program header

```
uleb128 family        1=fmt 2=pct 3=stdio 4=gettext
uleb128 content_size  bytes of the node region that follows
nodes*                until content_size is exhausted — size bounds it,
                      no terminator needed
```

No version field. Forward compat is handled by the code-space partition
and by unknown-code skipping (every node is self-delimiting).

`family` records which source grammar produced the program — it only
matters for decompiling back to text. The node set itself is grammar-
agnostic: **there is no stdio-specific field encoding** — a printf
front-end compiles `%08.3f` straight into the `field` op, which is a
superset of the fmt spec. Families:

| id | source grammar | emits |
|----|---------------|-------|
| 1 | fmt / `std::format` | `literal` + `field` |
| 2 | strftime / generic `%` slots | `literal` + `pct` (+ `field` for nested) |
| 3 | printf | `literal` + `field` (same ops as fmt) |
| 4 | gettext plural entry | one `plural` node |

`family` only matters for decompiling back to text.

---

## Part 2 — top-level ops

| code | node | kind | payload |
|------|------|------|---------|
| 1 | `literal` | bytes | already-unescaped text |
| 2 | `field` | list | **the** format field — fmt-spec superset; printf compiles into this too (children below) |
| 3 | `pct` | list | `%`-directive (children below) |
| 4 | `plural` | list | gettext plural selection (below) |

`field` may appear inside a `pct` program (chrono/generic specs allow
nested `{...}`). Otherwise families don't mix.

`field` takes a call arg and formats it. `pct` indexes locale/time-struct
fields — `%H` is "hour", not an arg. Different domains, different nodes.

The `field` node is deliberately a **superset of fmt's format spec**:
fmt needs fill/align/sign/`#`/`0`/width/prec/`L`/type/chrono — printf
adds `'`-grouping, `I`-outdigits, and `hh/l/ll/j/z/t/L/q/w/wf` length —
both are the same node. A printf compiler is just a front-end option
that parses `%`-syntax into `field` ops.

---

## Part 3 — `field` children (printf ≡ fmt unified)

| code | param | kind | value | printf | fmt |
|------|-------|------|-------|--------|-----|
| 1 | arg | uleb | arg index | `%2$`→1 | `{1}` |
| 2 | fill | bytes 1–4 | one code point | — | `{:*>8}` |
| 3 | align | uleb | 1`<` 2`>` 3`^` | `-`→1 | `<` `>` `^` |
| 4 | sign | uleb | 1`+` 2`-` 3` ` | `+` ` ` | `+` `-` ` ` |
| 5 | flag-alt | none | `#` | `#` | `#` |
| 6 | flag-zero | none | `0` | `0` | `0` |
| 7 | flag-group | none | `'` | `'` | — |
| 8 | flag-locale | none | `L` | — | `L` |
| 9 | flag-outdigits | none | `I` | `I` | — |
| 10 | width | uleb | value | `%5` | `{:5}` |
| 11 | prec | uleb | value (`.` alone → 0) | `%.3` | `{:.3}` |
| 12 | length | uleb | enum below | `ll` `L` `w` | — |
| 13 | length-bits | uleb | 8/16/32/64 | `%w64` | — |
| 14 | type | uleb | enum below | `%d` | `{:d}` |
| 15 | chrono | bytes | nested pct program | — | `{:%H:%M}` |

Absent `arg` = AUTO (next arg). printf `%2$` is normalized to 0-based
index at compile. fmt `-` align is printf `-`; fmt `-` *sign* is sign=2.

**type enum** (semantic codes — `%i` canonicalizes to `d`; a blob
decompiled to printf always emits `d`):

| code | letter | notes |
|------|--------|-------|
| 1 | `d` | signed dec — absorbs printf `%i` |
| 2 | `u` | unsigned dec |
| 3 | `o` | octal |
| 4 | `x` | hex lower |
| 5 | `X` | hex upper |
| 6 | `b` | binary lower (C23 + fmt) |
| 7 | `B` | binary upper |
| 8 | `e` | sci lower |
| 9 | `E` | sci upper |
| 10 | `f` | fixed lower |
| 11 | `F` | fixed upper |
| 12 | `g` | general lower |
| 13 | `G` | general upper |
| 14 | `a` | hexfloat lower |
| 15 | `A` | hexfloat upper |
| 16 | `c` | character |
| 17 | `s` | string |
| 18 | `?` | debug-escaped string (fmt only) |
| 19 | `p` | pointer |
| 20 | `P` | pointer upper (fmt only) |
| 21 | `C` | wide char (glibc `%C`) |
| 22 | `S` | wide string (glibc `%S`) |

**length enum**: `0` none · `1` hh · `2` h · `3` l · `4` ll · `5` j ·
`6` z · `7` t · `8` L · `9` q · `10` w · `11` wf (+ length-bits 8/16/32/64).

Example — `"%08.3f"` and `"{:08.3f}"` compile to the same node:

```
(field (flag-zero) (width 8) (prec 3) (type f))
```

---

## Part 4 — `pct` children (%-directives)

| code | param | kind | value |
|------|-------|------|-------|
| 20 | conv | uleb | enum below — required |
| 21 | letter | uleb | raw ASCII conv — generic slots only (name_fmt, postal_fmt, tel_*_fmt); meaning is slot-defined |
| 22 | pad | uleb | bit0 `-` bit1 `_` bit2 `0` |
| 23 | case | uleb | bit0 `^` bit1 `#` |
| 24 | modifier | uleb | 1 `E` era · 2 `O` alternative |
| 25 | colons | uleb | 1–3 (`%:z` `%::z` `%:::z`, conv `z` only) |
| 10 | width | uleb | shared code (`%5Y`) |
| 11 | prec | uleb | fractional-second digits etc. |

`conv` is its **own enum** — pct codes are not ASCII. strftime `%c`
(locale date-time composite) and printf `%c` (a character) are different
semantics → different numbers.

### conv enum

| code | directive | emits |
|------|-----------|-------|
| 1 | `%a` | weekday abbr |
| 2 | `%A` | weekday full |
| 3 | `%b` | month abbr — **`%h` canonicalizes here** |
| 4 | `%B` | month full |
| 5 | `%c` | composite → `d_t_fmt` |
| 6 | `%C` | century (year/100) |
| 7 | `%d` | day of month |
| 8 | `%e` | day of month, space-padded |
| 9 | `%F` | composite ISO date → `%Y-%m-%d` |
| 10 | `%G` | ISO week-numbering year (4-digit) |
| 11 | `%H` | hour 00–23 |
| 12 | `%I` | hour 01–12 |
| 13 | `%j` | day of year |
| 14 | `%k` | hour 0–23 space-pad |
| 15 | `%l` | hour 1–12 space-pad |
| 16 | `%m` | month |
| 17 | `%M` | minute |
| 18 | `%p` | AM/PM |
| 19 | `%P` | am/pm lowercase |
| 20 | `%r` | composite → `t_fmt_ampm` |
| 21 | `%R` | composite → `%H:%M` |
| 22 | `%s` | seconds since epoch |
| 23 | `%S` | second |
| 24 | `%T` | composite → `%H:%M:%S` |
| 25 | `%u` | weekday Mon=1..Sun=7 |
| 26 | `%U` | week number (Sunday) |
| 27 | `%V` | ISO week number |
| 28 | `%w` | weekday Sun=0..Sat=6 |
| 29 | `%W` | week number (Monday) |
| 30 | `%x` | composite → `d_fmt` |
| 31 | `%X` | composite → `t_fmt` |
| 32 | `%Y` | year (full — only form) |
| 33 | `%z` | tz offset ±hhmm; colons param for `±hh:mm` |
| 34 | `%Z` | tz name |
| 35 | `%+` | composite → `date_fmt` |
| 36 | `iso8601` | **fast_io's own iso8601 timestamp** — one code; emitter is `iso8601_timestamp` in `iso/iso8601.h` |
| 37 | `iso8601-utc` | utc variant (emits `Z` / zero offset form) |
| 38 | `fracsec` | fractional seconds; `prec` param = digits |

Composite convs stay codes: the interpreter resolves the referenced
locale slot (itself a blob) with a bounded recursion depth.

### Modifier matrix — `E`/`O` (from real localedata usage)

```
%E:  c C x X Y y
     %Ec → era_d_t_fmt   %Ex → era_d_fmt   %EX → era_t_fmt
     %EC → era name      %EY → era full (name+year "令和5年")
     %Ey → era year (year within era — NOT a 2-digit year; eras
           number years from their own offset: 令和5年 → "5")

%O:  d e H I k l m M p S u U V w W B C
     alternative-digit/alt-form numerics; %Op alt AM/PM;
     %OB/%OC → ab_alt_mon / alt forms (ru_RU etc. use these)
```

`%Ey` survives because it is an *era* year, not a truncated AD year.
`%Oy` does not — it's year-mod-100 in alternative digits.

### Dropped — no 2-digit years

`%y`, `%Oy`, `%g`, `%D` — **not encodable.** Two-digit years are
ambiguity-shaped bugs; nothing in this format emits one.

Impact: ~108 locale files use `%y` (mostly in `d_fmt`/`date_fmt`; the C
locale itself has `d_fmt "%m//%d//%y"`). Decision needed: compiler rejects
those slots outright, or data gets fixed upstream.

Also folded to literal bytes by the compiler (never nodes): `%%`→`%`,
`%n`→`\n`, `%t`→`\t`, `{{`→`{`, `}}`→`}`.

---

## Part 5 — plural (gettext)

A gettext catalog entry is `msgid` / `msgid_plural` / `msgstr[0..N-1]`
plus a plural rule. **The rule does not live in the blob** — it is a
fixed function of `n` per locale, and this format is not a programming
language. The caller evaluates the rule (a native per-locale function,
selected by a rule id stored in the lcblob catalog metadata) and passes
the chosen **form index**; the `plural` node is just an ordered list of
programs:

```
(plural (count <arg-idx>)  (form <prog>)  (form <prog>) ...)

1st child: count — uleb, which call arg holds n
rest:      form×N — bytes nodes; each payload is a complete binfmt
                    program (usually family=stdio, compiled from msgstr[N])
```

Interpretation: the caller reads `count` to know which arg is `n`,
evaluates its own plural rule, passes the chosen index → `form[i]` runs.
`form[i]` is whatever the locale's rule says — `form[0]` is conventionally
the singular but the rule decides: English `n != 1` sends `n=0` to the
plural ("0 apples"), French `n <= 1` sends it to `form[0]`. The blob only
carries the forms; which n maps where is the rule's business. The forms
are ordinary programs — gettext printf-isms (`%1$s`, `%d`…) compile
through the stdio front-end into `field` nodes, so a form can consume
the count arg like any other (`"%d apples"` just prints it).

The lcblob side carries `plural_rule` as an integer slot — an index into
fast_io's own plural-rule table (CLDR rules implemented as native code),
*not* an encoded expression.

| code | node | kind | payload |
|------|------|------|---------|
| 0x40 | `plural` | list | children: `count` + `form`×N |
| 0x41 | `count` | uleb | arg index of `n` |
| 0x42 | `form` | bytes | complete program blob |

### Example — `"an apple"` / `"%d apples"`

```
(plural (count 0) (form "an apple") (form "%d apples"))
```

`form[0]` is literal-only — "an apple" has no field at all; `form[1]`
consumes the count arg through an ordinary `%d` field.

---

## Byte examples

`{0:2147483647}` (family=fmt) — 12 bytes total:

```
01               family=1 (fmt)
0A               content_size = 10
0B               tag (2<<2)|3   field, list
  08             list len = 8
  05 00          tag (1<<2)|1   arg  = 0
  29 FF*4 07     tag (10<<2)|1  width = 2147483647  (uleb: 5 bytes)
```

`"%Y年%m月%d日 %H時%M分%S秒"` (ja_JP `d_t_fmt`, family=pct) = 57 bytes —
each `%X` directive is 4 bytes, e.g. `%Y`:

```
0F               tag (3<<2)|3   pct, list
  02             len = 2
  51 20          tag (20<<2)|1  conv = 32 ('%Y')
```

Plural, `count=arg0` + `"an apple"` / `"%d apples"` — the caller picks
the index = 41 bytes:

```
04               family=4 (gettext)
27               content_size = 39
83 02            tag (0x40<<2)|3 plural, list
  24             len = 36
  85 02 00       tag (0x41<<2)|1 count = 0
  8A 02 0C       tag (0x42<<2)|2 form, len=12
    <blob for "an apple"   = family3: (literal "an apple")>
  8A 02 0F       tag (0x42<<2)|2 form, len=15
    <blob for "%d apples"  = family3: (field (type d))(literal " apples")>
```

---

## Part 6 — rejected constructs

No encoding exists; compilers error out:

- `%n`, `%m`, `%[` (stdio)
- `*`, `*n$` — runtime-driven width/precision
- `{name}` — named args
- `!r` `!a` `!s` — repr dispatch
- `{:{}}` — nested `{}` inside width/precision
- `,`/`_` digit-group specifiers
- `%y` `%Oy` `%g` `%D` — 2-digit years (era `%Ey`/`%EY`/`%EC` are kept)
- unknown convs; colons on non-`z`; unterminated fields; lone `}`

---

## Part 7 — lcblob container

```
header:
  u32 magic          'FCL1' = 0x314C4346  (trailing digit = rev)
  u32 total_size
  u32 flags          reserved, 0
  u32 name_rva       strref — "de_DE"
  u32 encoding_rva   strref — charset of all string payloads
  u32 cat_dir_rva    -> u32 table_rva[LC_CAT_COUNT]   (0 = absent)

strref := u32 rva | u32 len
slot   := u32 tag | u32 rva | u32 len | u32 aux          (16 bytes)
  tag 0 ABSENT
  tag 1 STRING      rva+len -> bytes
  tag 2 STRLIST     rva -> strref[len]      (abday[7], mon[12], am_pm[2]…)
  tag 3 INT         aux = i32 value
  tag 4 BYTES       rva+len -> i8 list      (grouping, mon_grouping)
  tag 5 PROGRAM     rva+len -> binfmt blob  (d_t_fmt, name_fmt, …)
  tag 6 INT3        rva -> i32[3]           (week: ndays;first_date;first_week)
  tag 7 ERALIST     rva -> era_rec[len]
```

Each category is a fixed-order slot table indexed by field id; the schema
(`lc_field_def{name,kind}` arrays) lives in `lcblob.h`. Lookup is
`cat_dir[cat] + field*16` — no key strings at runtime.

```
era_rec (40 bytes):
  i32 direction      +1 | -1
  i32 offset
  i32 start_year     i32_MIN = "-*"
  u8  start_month, start_day
  i32 end_year       i32_MAX = "+*"
  u8  end_month, end_day
  u32 name_rva, name_len
  u32 fmt_rva,  fmt_len      binfmt pct program
```

Categories (fixed ids 0–11): identification, ctype, collate, time,
numeric, monetary, messages, paper, name, address, telephone,
measurement.

- `ctype` v1: encoding string only (translit/charclass tables TBD)
- `collate` v1: flag slot only (codepoint vs table)
- `messages.yesexpr/noexpr` stay **strings** — they're regex sources for
  matching, not formatting programs

## Open items

1. `%y` fallout: ~108 locales — reject slot / fix data / keep compat code?
2. era `fmt` inside `era_rec` is a pct program — era fmts can themselves
   contain `%E` directives (`%EC%Ey年`); recursion is depth-capped ≤ 8.
3. `field` inside `pct` (chrono nested `{}`) — allowed; depth-capped.
4. iso8601 composite granularity: single `iso8601` code + utc variant +
   `fracsec` — enough, or do we want week-date/ordinal-date codes too?
