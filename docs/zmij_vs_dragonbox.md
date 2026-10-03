# Shortest round-trip float printing: dragonbox vs zmij

Notes on the two Schubfach-family `double → shortest decimal` cores, why zmij is
~2x faster, and what we are porting into `fast_io.floating`.

**Schubfach** (Raffaello Giulietti, ~2020) is the algorithm family these all
belong to — German for "pigeonhole/drawer", after the *Schubfachprinzip*
(pigeonhole principle), which is the grid argument in §2. It powers Java's
`Double.toString` since JDK 19. Lineage:

```
Dragon4 (1990, bignum)
 └─ Grisu (2010)
     ├─ Ryū (2018)
     ├─ Schubfach (2020) — pigeonhole + Nadezhin, non-iterative
     │    ├─ dragonbox (2021) ──► our roundtrip.h
     │    └─ yy / yyjson ──► zmij (2025)
     │         + xjb's 10^{-(k+1)} scaling + Dougall Johnson's pow10
     │           compression + Lean proofs
     └─ MacroModel's "DA" — another yy/Schubfach derivative
```

Measured on this machine (random f64 bit patterns, clang trunk, `-O3 -march=native`):

| implementation | insns (core) | ns/value |
|---|---|---|
| zmij `::to_decimal` (zmij.cc:1243) | ~186 | **6.5** |
| zmij header constexpr `to_decimal` (zmij.h:394) | ~190 | 14.7 |
| upstream dragonbox `compute_nearest` | ~338 | 12.8 |
| zmij full `write` (conv + SIMD digits + layout) | ~318 | 14.9 |

50M random f64 (incl. subnormals): zmij `to_decimal`+trim matches the current
implementation on every input. zmij is also Lean-verified (`test/*.lean`)
and MIT/Boost licensed.

---

## 1. The problem

A finite double is `v = m × 2^e` with `m` a 53-bit integer (implicit leading 1
for normals) and `e ∈ [-1074, 971]`. The function we want is `(m, e) →
{m10, e10}` — two integers in, two integers out — where `m10 × 10^e10` is
the *shortest* decimal that round-trips.

**Doubles can't represent most reals** — there are only 2⁶⁴ of them, so
consecutive doubles have a gap (`2^e`). Parsing takes the *nearest* double,
which means `v` is not a point — it *owns* a rounding interval
`R = [v−δ, v+δ]`, `δ = 2^e / 2` (the midpoints to its neighbors are the
borders):

```
     ◄──────────►◄──────────►
prev ──●────────│────●────────│────●──── next
       │◄─ interval owned by v ─►│
       [v − gap/2,  v + gap/2]
```

**Any real inside R parses back to v** — so `v` has many correct spellings.
`1.0` owns `[1 − 1.1e-16, 1 + 1.1e-16]`; `1`, `1.0`,
`0.99999999999999999`, `1.00000000000000011` all round-trip. The exact binary
value of e.g. `0.1` is a ~55-digit decimal — the interval is what lets the
printer answer `1` instead. The printer's job:

> find the shortest decimal `d = c × 10^k` that lands inside R.

Wider strip (larger `e`) → bigger target → fewer digits needed; that's why
`1e300` prints as `1e300`.

Tie rule (round-to-nearest-even): if `m` is **even**, both endpoints `v±δ`
belong to `v` (closed interval); if **odd**, they belong to the neighbors
(open interval). A decimal sitting *exactly* on a midpoint round-trips to `v`
only when `m` is even — this decides ties.

Wrinkle: at powers of two the gap *below* is half the gap above — the
"irregular" interval (`δ− = 2^e/4`). Subnormals have uniform spacing but tiny
significands (extra fixup needed).

## 2. The grid argument (pigeonhole)

Decimals with D significant digits form a ruler: ticks at `c × 10^k`,
spacing `10^k`.

- `spacing ≤ width(R)` → ≥1 tick *must* fall inside R.
- `spacing > width(R)` → ≤1 tick *can* be inside.

Pick `k` with `10^k ≤ width(R) < 10^{k+1}`. Then only two scales matter:

- **coarse candidate** `s × 10^{k+1}` — one tick may be inside by luck →
  shortest answer (~16 digits for f64).
- **fine candidate** `t × 10^k` — at least one tick is guaranteed → used when
  the coarse grid misses (~17 digits).

**What `k` is concretely**: since `v = m × 2^e` and `m` is already a 16-digit
integer, `v`'s decimal magnitude is almost entirely `2^e`, so
`k ≈ floor(e · log10 2) = e · 0.30103…` — one fixed-point multiply, no table.
Choosing `k` with `10^{k+1} ≈ 2^e` means `v / 10^{k+1} ≈ m`: the division
undoes the binary exponent and leaves a significand-sized integer. `k` is
both the scaling input and the skeleton of the output exponent `e10`.

**The interval `R` itself is never computed.** Only three derived quantities
are used:

- `k` — comes from `e` alone, not from an actual width computation;
- `h = δ × 10^{-(k+1)}`, the half-width in scaled units — free, because `δ`
  is a power of two (no mantissa), so it's just a *shift* of the same pow10
  table entry used for `v`;
- `even = m & 1` — endpoint inclusion, folded into `h` as ±1 ulp.

The algorithm only answers two questions *about* R — "does a coarse tick
fall inside it?" and "which fine tick is nearest v?" — never builds it.

## 3. The scaling step (shared machinery)

To locate `v` on the ruler compute `c = v × 10^{-(k+1)}` = `m × 10^{-(k+1)} ×
2^e` as fixed-point.

`10^{-k}` is an uncomputable fraction, so a table stores
`P ≈ 10^{-k} × 2^{127}` — a normalized 128-bit significand (top bit set).
One 64×128→192-bit multiply `m × P` yields `c` as `integer.fraction`.

**Why is an approximate P safe?** (Nadezhin) Interval edges are never
*extremely* close to a tick unless exactly on it — a provable gap exists, so
a 1-ulp-off `P` makes the identical rounding decision as exact math.

Both algorithms do exactly this much. They differ in **which scale they
multiply at** — and that choice decides how much work is left afterwards.

## 4. dragonbox: scale big, divide down, recover what division destroyed

`dragonbox_main` (roundtrip.h) scales the interval's right edge to a fine
grid, then divides:

```cpp
two_fr = 2*m2 + 1;                          // right edge of R (×2 for half-ulp)
zi     = umulh(two_fr << β, pow10[-k]);     // ~18-19 digit scaled edge
q      = zi / 1000;                         // coarse-grid candidate
r      = zi % 1000;                         // edge's position inside the cell
delta  = scaled interval width;
```

Decision tree (~30 branches):

- `r < delta` → `q` may be inside R. But `zi` came from an *approximate*
  power of 10, so `r` cannot distinguish "edge exactly on a tick" from "a
  hair inside" — that information was destroyed by `/1000`. Recovery:
  `mul_parity(two_fl)` / `is_integral_end_point(two_fr)` — **extra 128-bit
  multiplies** testing whether an edge is exactly divisible by `2^β·5^κ`
  (i.e. exactly a decimal — the tie case). Needed up to 3× on different
  boundaries.
- `r == delta` → edge on a tick → another parity multiply.
- else `q` overshoots → rebuild the fine candidate `q×10 + dist/100`, then
  maybe `q−1` via a third parity check.

Then `dragonbox_impl` trims trailing zeros (`rtz_iec559`: ctz + pow5 table).

Total: **1 main multiply + up to 3 recovery multiplies + magic-divides by
1000/100/10 + ~30 branches**. Every divide throws away remainder bits that
must then be reconstructed by multiplying again.

## 5. zmij: scale one notch finer, read everything off one product

Framed differently: the whole algorithm is **one division**
`c = v / 10^{k+1}` computed in fixed point, plus `h` in the same units.
In cell units **integers are the coarse ticks and tenths are the fine
ticks**, so "is there a coarse tick inside R?" reduces to "is there an
integer inside `[c−h, c+h]`?" — two compares. If not, `fractional` literally
*is* the next decimal digit.

`::to_decimal` (zmij.cc:1243, the production path) chooses the grid **one
level finer than the coarse candidate** (`10^{-dec_exp-1}` — Xiang JunBo's
scaling) so a single product splits into both answers:

```cpp
p          = umul192_hi128(pow10.hi, pow10.lo, m2 << shift);  // ONE multiply
integral   = p.hi >> 9;                  // the 16-digit (coarse) candidate
fractional = p.hi << 55 | p.lo >> 9;     // v's position in the cell, ∈[0,2⁶⁴)
```

zmij's own example, `v = 5.0507837461e-27`:

```
c = 5050783746100000.3153987           (units of the coarse grid)
    └──── integral ────┘└─fractional─┘

tick              v           tick
 0  [.315-.359, .315+.359]     1
 ─●────[──────●──────]──────────●───
 h = 0.359 > frac = 0.315 → bottom tick inside → answer = integral
```

`fractional` *is* all the boundary information, nothing was divided away:

```cpp
half_ulp   = (pow10.hi >> (10 - shift)) + even;   // interval half-width, ±1ulp tie rule
round_up   = fractional + half_ulp < fractional;  // interval reaches top tick
round_down = half_ulp > fractional;               // interval covers bottom tick
integral  += round_up;

digit = umul128_add_hi64(fractional, 10, biased_half);  // next decimal digit
if (fractional == 1<<62) digit = 2;                     // "2.5 → 2" tie fix
```

- `round_up` → top tick `integral+1` inside R → answer `integral+1` (16 digits).
- `round_down` → bottom tick `integral` inside R → answer `integral`.
- neither → no coarse tick inside → `has_last_digit` → answer is the
  17-digit `integral×10 + digit` (where `digit = round(fractional × 10)`).

The result is encoded `sig*10 + (has_last_digit ? digit : 0)` — so an
ordinary trailing-zero strip naturally recovers the shorter form
(`sig*10+0 → sig`). Our `rtz_iec559` does exactly this — drop-in fit.

**The endpoint-inclusion problem** (open vs closed R for odd/even `m`) becomes

```cpp
even = 1 - (m2 & 1);
half_ulp += even;      // even m → closed interval → 1 ulp wider
```

±1 ulp only flips the decision when an edge is *exactly* on a tick — which
is exactly the tie case. Dragonbox spends whole parity multiplies on this.

Two table lookups replace the remaining arithmetic:

- `exp_shifts[raw_e2]` — 2048-byte i8 table indexed directly by the biased
  exponent (values = `compute_exp_shift(bin_exp, dec_exp+1) + 9`). Floats
  index it at `raw_exp + 925` — same bin_exp range, one table serves both.
- `pow10_significands[-dec_exp-1]` — uncompressed 649×16B table
  (dec_exp ∈ [-307, 341]), `alignas(64)`, one load.

**Total: 1 main multiply + 1 digit multiply + 0 divides + ~10 branches.**

## 6. The one-sentence difference

Dragonbox scales to a big integer and **divides** — and every divide throws
away remainder information it then reconstructs with extra multiplies. zmij
picks the grid so that the short candidate (`integral`), the extra digit, and
both distances-to-ticks coexist in one product's integer and fractional
parts — nothing is divided away, so nothing has to be recovered.

## 7. Subnormals: DA needs no fixup at all

zmij's `write` pads short subnormal significands (`clz` deficit + `pow10s[]`
multiply, zmij.cc:1709-1726) — but that's for its writer's digit-block
layout invariants, **not correctness**.

DA passes subnormals straight into the regular formula with
`effective_raw_exp = 1` and no implicit bit — and it's *provably* fine:
for a subnormal, `δ` is fixed at `2^-1075` (never varies with `m`) and the
grid is fixed at `10^{-323}`, so the scaled half-width is always
`h = 2^-1075/10^-323 ≈ 0.247 < 0.5` — the cell can never hold more than one
coarse tick, exactly what the two-grid proof needs. No normalization, no pad.

Verified: all `m ∈ [1, 2²¹]` plus 3M random 52-bit subnormals — 5,097,152
cases, 0 mismatches vs the canonical implementation.

(The trap to avoid: thinking `h ≈ 2.47` — that's off by 10×, confusing
`2^-1075` with `2^-1074`.)

## 8. Float (binary32) path — even simpler

```cpp
p_hi       = umul128_hi64(pow10.hi + 1, (u64)bin_sig << shift);   // ONE 64×64→128
integral   = p_hi >> 34;
fractional = p_hi & ((1<<34) - 1);
half_ulp   = (pow10.hi >> (65 - shift)) + even;
// same round_up/round_down, digit = (fractional*10 + 2^33) >> 34
// tie fix: fractional == 2^32 → digit = 2
```

Only the pow10 *hi* word (+1 ulp) is needed — no umul192 at all.

## 9. Static data actually needed for the port

From `struct data` (zmij.cc:733), only:

| table | size | content |
|---|---|---|
| `exp_shifts` | 2048 B | `u8[raw_exp]` = `compute_exp_shift(e, d+1) + 9`, `e = raw − 1075` (raw 0 → e = −1074), `d = floor(e·log10 2)` |
| `pow10_significands` | 649 × 16 B | `10^e` normalized 128-bit floor, e ∈ [−307, 341] |
| `biased_half` | const | `(1<<63) + 6` |
| `pow10s` | 20 × 8 B | `10^0..10^19` literals (subnormal pad) |

Formulas (must match zmij *exactly*):

```cpp
compute_dec_exp(e)          = (e * 315653) >> 20            // floor(e·log10 2)
compute_exp_shift(e, q)     = e + ((-q * 217707) >> 16) + 1 // floor(e·log2(10^-q)) etc
exp_shifts[raw]             = compute_exp_shift(e, d+1) + 9
pow10_significands[e]       = floor(10^e / 2^{floor(log2 10^e) - 127})
```

`umul192_hi128(x_hi, x_lo, y)` = high 128 bits of the 192-bit product
`(x_hi·2⁶⁴ + x_lo) × y` — we have `::fast_io::intrinsics::umul`/`umulh`.
`umul128_add_hi64(x, y, c)` = `(x·y + c) >> 64`.

`log10_2` alternative for `dec_exp` in the fast path:
`umul128_hi64(bin_exp, 78913 << (64-18))` — same value, keeps dec_exp in a
32-bit register for the table index.

## 10. Correctness constraints of the port

- `m2` must be **odd-aware**: `even` folds into `half_ulp` — do not drop it.
- `bin_sig << shift` must fit u64: `extra_shift = 9` keeps `shift ≤ 12`-ish;
  the irregular `2^52 << 12` is the bound (zmij comment: 11 would overflow).
- `pow10.hi + 1` in the f32 path is deliberate (overestimate by 1 ulp).
- The `2.5 → 2` fixes (`fractional == 1<<62` → digit=2; `1<<32` for f32) are
  load-bearing ties-to-even cases.
- Irregular interval (`m2 == implicit_bit`, i.e. powers of two): separate
  cold path, `compute_dec_exp(bin_exp, regular=false)` = subtract the
  `log10(3/4)` term (`131072 >> 20` in fixed point) — do NOT route it through
  the regular table lookup.
- `has_extra_digit`/`threshold` (1e15) and layout/shuffle tables belong to
  zmij's `write` — not needed; our emit layer handles digits.

## 11. Interface mapping

```
zmij ::to_decimal(m2_with_implicit, raw_e2, regular=m2!=0, data)
  → {sig, exp, last_digit, has_last_digit}
  → our m10_result:  s = sig*10 + (-has_last_digit & last_digit)
                     {m10, e10} = rtz_iec559(s), e10 = exp + zeros
```

Same call contract as `dragonbox_impl(m2, raw_e2)` — drop-in at
roundtrip.h:715 and lc_roundtrip.h:455. `m2` is the raw mantissa **without**
implicit bit, `raw_e2` the biased field — identical to zmij's `get_sig`/
`get_exp` split.

## 12. Where DA is wrong: the irregular lower boundary

DA's asymmetric interval test uses `h/2 > F` — an **open** lower boundary
computed under the assumption the boundary case never materializes for its
formats. For binary16, reusing the same machinery hits it: v = 2^13 has
sig 2^11 (even, so the lower edge is **closed**) and R = [8190, 8196].

At scale 10, the cell below v contains exactly one member: 8190 — and
fractional − half_ulp lands exactly on it (frac == h/2). DA's strict `>`
rejects the boundary member and keeps the 17th-digit-form 8192; canonical
is **8190** (one digit shorter). The same goes for 2^14 → 16380 and
2^15 → 32760.

Counterexample table (f16 powers of two, exhaustive — 3 of 255 finite
irregular cases):

| v | DA strict `>` | canonical |
|---|---|---|
| 2^13 = 8192 | `{8192,0}` → "8192" | `{819,1}` → "8190" |
| 2^14 = 16384 | `{1639,1}` → "16390" | `{1638,1}` → "16380" |
| 2^15 = 32768 | `{3277,1}` → "32770" | `{3276,1}` → "32760" |

Why binary16 alone bites: the boundary member exists only when the
mantissa odd-part divides a power of ten, i.e. 2^(p+2)−1 carries a factor
of 5 — f16's 4095 = 3^2·5·7·13 does; bf16's 511, f32's 2^25−1, f64's
2^54−1, and both wide formats' masks do not. The fix: for a closed lower
edge the member test must be inclusive (`>=`), or (simpler, as
implemented) — in the narrow irregular path, solve the whole interval
exactly in integers: search the coarsest scale containing a member, then
take the member nearest v with ties to even.

## 13. Sources


- zmij repo (vitaut/zmij): `zmij.cc:1243` core, `:1663` wrapper, `:1709`
  subnormal fixup; `zmij.h:294` seed tables, `:347` `compute_pow10`, `:394`
  constexpr reference `to_decimal`; Lean proofs in `zmij/test/*.lean`
- upstream dragonbox repo (jk-jeon/dragonbox)
- fast_io: `include/fast_io_unit/floating/roundtrip.h` (`dragonbox_main`:281,
  `schubfach_asymmetric_interval`:214, `dragonbox_impl`:418)
- algorithm lineage: Schubfach (Giulietti) → yyjson/yy (ibireme) → xjb
  10^{-k-1} scaling (xjb714) → Dougall Johnson pow10 compression → zmij
  (Zverovich). Formal proofs: `zmij/test/*.lean`.
- blog: vitaut.net/posts/2025/smallest-dtoa (the 200-line reference version —
  note it is the *simple* Schubfach, ~70% slower than dragonbox; zmij is the
  optimized evolution of it)
