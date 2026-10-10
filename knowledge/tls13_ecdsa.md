# fast_io TLS 1.3 + ECDSA/bignum — session handoff knowledge

Continuation of `async_tls_herbceptions.md` (read that first for toolchain +
herbceptions rules). This covers the visible, self-contained TLS 1.3 stack and
the ECC/bignum work behind ECDSA P-256/P-384/P-521 + RFC 6979.

## What exists (all verified vs openssl, committed)

- TLS 1.3 only (no 1.2), X25519 key exchange, AES-128/256-GCM +
  ChaCha20-Poly1305, RSA-PSS, Ed25519, ECDSA on all three NIST curves,
  RFC 6979 deterministic nonces, X.509/DER + chain validation, SAN hostname
  check, system trust store, mTLS, session tickets + PSK resumption, encrypted
  alerts, kTLS offload on Linux with userspace fallback, async
  handshake/read/write/close on io_uring + posix thread pool (works on macOS).

## File map (repo: ~/libraries/fast_io_kilo/fast_io, branch herbceptions)

| File | Holds |
|---|---|
| `tls/client_impl.h` | sync client handshake driver + record layer + kTLS |
| `tls/server.h` | server driver, CH parse, scheme pick, tickets, CR, post-handshake |
| `tls/handshake.h` | CH builder + `supported_signature_schemes` list |
| `tls/x509.h` | DER parse (`der_read_tlv` long-form OK), chain verify, `tls_certificate_verify` (CV sig verify), hostname match, cert file loading |
| `tls/pkey.h` | private key parse (PKCS#8/SEC1/PKCS#1/raw ed25519) + `tls_cv_sign` |
| `tls/crypto_backend.h` + `ossl_backend.h` + `gnutls_backend.h` | backend customization point; fast_io userspace backend is default when `FAST_IO_TLS_FORCE_FAST_IO` or no openssl |
| `tls/impl.h` | digest/HKDF/AEAD/param wiring, `tls_generic_sched` event helpers |
| `tls/async.h` | async TLS op defines — `__linux__` gate covers only io_uring/kTLS pieces; posix-pool defines are portable |
| `tls/pem.h` | PEM/base64 block decode |
| `tls/defs.h` | enums: cipher_suite, signature_scheme, alert_description, groups |
| `ecc/secp256r1.h` | ALL NIST curves: `ec_mont_ctx`/`ec_curve`, Jacobian point math, `ecdsa_verify_to_ptr`, `ecdsa_sign_to_ptr`, `ecdsa_sign_rfc6979_to_ptr`, `ecdsa_rfc6979_drbg` |
| `rsa/number.h` | limb arrays: mul/add/sub/compare, `limbs_to_bytes_be`, `limbs_pow2_mod`, `limbs_mod_double` |
| `rsa/montgomery.h` | `montgomery_reduction` (SOS), `montgomery_multiplication`, `montgomery_pow`, `montgomery_r2_setup`, `montgomery_n0_inverse` |
| `rsa/impl.h` + `rsa/emsa.h` | RSA-PSS, EMSA; ECDSA code reuses details::rsa bignum |
| `curve25519/*` | X25519 + ed25519 — reference style for the ECC code |
| `share/fast_io/fast_io_inc/crypto.inc` | module export surface — new public names must be added here |

## Architecture rules (user-enforced)

- FREE FUNCTIONS over C++ OOP. State lives in public structs
  (`ec_mont_ctx`, `tls_pkey`, `x509_certificate`, `tls_server_config`).
- Customization points via `operations::` decay/`handshake_define`/`async_*_define`.
- `constexpr` where feasible — ECC contexts and point ops are constexpr.
- Header-only; never assume a dep — check includes. Exceptions = herbceptions
  (`details::tls_fail(sock, alert, false)` throws inside the handshake).
- clang-format is enforced (`.clang-format`: Microsoft base, tabs, `ColumnLimit: 0`,
  `QualifierAlignment: Right`, `InsertBraces`). Format touched files before commit.
  `share/fast_io/fast_io_inc/crypto.inc` is space-indented — clang-format would
  churn it; leave its style alone.

## ECC internals (ecc/secp256r1.h)

- `ec_curve_params` = {p, n, b, Gx, Gy, pm2, nm2, nl, nbytes, nbits}; `ec_curve`
  precomputes field+order Montgomery ctxs + Montgomery-domain generator.
- `ec_max_limbs = 9` (P-521 needs it). Any buffer touching scalars/points must
  be `ec_max_limbs`-sized or `nbits`-derived — **never 48/64 literal**.
- Point math is Jacobian projective (double/add formulas commented in source);
  `ec_scalar_mul` is MSB-first double-and-add (variable-time — private-key
  operations are documented non-constant-time).
- Inversion = Fermat `a^(p-2)` via fixed-exponent `montgomery_pow` (Euler
  theorem). `pm2`/`nm2` are byte arrays `p-2`, `n-2`.
- `ec_point_load` Montgomery-converts affine (x,y,1); `ec_affine_x` converts
  back via one field inversion.
- ECDSA: `ecdsa_verify_to_ptr` (pubkey SEC1 uncompressed, r/s DER-integers,
  digest) and `ecdsa_sign_to_ptr(privkey, k, digest)` producing DER
  SEQUENCE{INTEGER r, INTEGER s} with long-form lengths. `ecdsa_sign_rfc6979_to_ptr
  <hash_ctx>` derives k deterministically.

## RFC 6979 (ecdsa_rfc6979_drbg<hash_ctx>)

- HMAC_DRBG per RFC 6979 §3.2: K=V=init, `V||0x00||int2octets(x)||bits2octets(h1)`
  then `V||0x01||...`. Rejected candidates reseed `K = HMAC(K, V||0x00)`.
- `bits2octets`: digest >> (hlen*8 - qlen) then mod n.
- `next()` generates T = V||V||... up to `nbytes` — P-521 (nbytes=66 > 64-byte
  sha512 output) takes TWO V blocks, then `bits2int` drops
  `nbytes*8 - nbits` (528-521=7) high bits.
- Candidates are `1 <= k < n`; rejection probability for P-521 is ~0.4%
  (candidate < 2^521, n ≈ 0.996·2^521).
- Test vectors: RFC 6979 A.2.5 (P-256/SHA256), A.2.6 (P-384/SHA384),
  A.2.7 (P-521/SHA512) — all match exactly (k, r, s).

## TLS wiring facts

- Signature schemes live in `signature_scheme` enum (defs.h): 0x0403 P-256,
  0x0503 P-384, 0x0603 P-521. Advertise in `supported_signature_schemes`
  (handshake.h) AND `server_cr_sigalgs` (server.h) — keep
  `certificate_request_write`'s byte reservation in sync (2+2+2·count).
- `tls_server_scheme_pick` chooses from leaf's SPKI params OID (curve), not
  the cert's signature_algorithm.
- `tls_cv_sign` dispatches on scheme → hash; RFC 6979 path for ECDSA,
  `tls_fill_random` salt for RSA-PSS.
- `x509_verify_signature`: hash from `cert.signature_algorithm_oid`, curve from
  `issuer_alg.params` OID — X.509 mixes hash/curve freely (openssl signs P-384
  certs with SHA-256), do NOT pin pairs.
- CV covered content: `certificate_verify_content_write` over transcript digest.
- Server CV sig buffer: `signature[1024]` — fits P-521's ~140B DER.

## Hard-won pitfalls (all burned time)

- **Stale debug binaries caused several false diagnoses.** Rebuild every
  reproducer before trusting output. `limbs_pow2_mod` was "broken" twice —
  both times it was a stale binary or a malformed Python constant.
- **Never hand-transcribe hex constants.** Generate them (`openssl ec -text`,
  python `hex(v)`). Failures traced to: a P-521 order with an extra `ff`
  (529 bits), a 65-byte `s` array, `b(0x3)` vs `b(0xd3)`, `7f a0` vs `77 fa`.
- **`limbs_to_bytes_be` partial top limb was little-endian** — fixed
  (bd5282ea). RSA never noticed (plen always == nl*8); P-521's 66-byte
  values were the first `rem != 0` caller.
- **DER long-form lengths**: a ~138-byte P-521 signature needs `30 81 LL`,
  not `30 87` (0x87 = "long form, 7 length bytes" to a parser). Both SEQ and
  INTEGER writers must emit it. `der_read_tlv` already parses it.
- **Stack-scratch sizing**: `limbs_multiplication` writes 2nl limbs;
  `montgomery_reduction` scratch needs 2nl+1; `montgomery_pow` scratch
  5·nl+2. Buffers hardcoded for P-384 (48B/64B/6 limbs) silently clobbered
  neighbors at nl=9 — symptom was plausible-but-wrong values, not crashes
  (ASAN-clean since all writes landed in adjacent in-frame arrays).
- **openssl s_client `-sigalgs`**: names are `ecdsa_secp521r1_sha512` etc.
  `-groups "P-521"` alone fails handshakes — no X25519 in common.
- `ec_limb` is `details::rsa::value_type` (u64); prints need limb dumps or
  padded u8 hex — minimal-digit `hex(u8)` byte dumps misalign everything.
- `main` cannot be `throws`; TLS API symbols live under `tls::details`.

## Verification recipes

- RFC vector check: drive `ecdsa_sign_rfc6979_to_ptr` + drbg.next against
  spec numbers; also verify `x*G == U` and `affine_x(k*G) == r`.
- Live server test: `openssl ecparam -genkey -name secp521r1`, `req -x509
  -sha512 -addext subjectAltName=DNS:localhost` (CN alone is rejected —
  no SAN means bad_certificate), `openssl pkcs8 -topk8 -nocrypt`, then
  `openssl s_client -connect 127.0.0.1:4433 -sigalgs ecdsa_secp521r1_sha512
  -tls1_3 -verify_return_error -verifyCAfile cert.pem`.
- Self-verify a chain: `x509_verify_signature(cert, cert.spki_algorithm,
  cert.public_key, cert.public_key_size)` on a self-signed cert.
- Cross-verify signatures: `openssl dgst -sha512 -verify pubkey.pem
  -signature sig.der msg`.

## TODO / open items

- **KeyUpdate** (RFC 8446 4.6.3) — post-handshake traffic-key rotation,
  update_requested handling, next-gen key derivation.
- **Post-handshake client auth** — CertificateRequest after handshake;
  builds on the mTLS path, needs post-handshake message framing.
- **kTLS sendfile/splice** — record-layer offload exists; wire a file→socket
  transmit path through it.
- **`MSG_NOSIGNAL`/`poll`-based writers in posix_netmode.h** — send() on a
  dead socket still raises SIGPIPE; library should pass MSG_NOSIGNAL or
  SO_NOSIGPIPE by default.
- **Perf**: Montgomery path is ~4x behind openssl ADX asm; consider CIOS
  (fused multiply-reduce per limb) instead of separate mul+reduce, plus a
  comba for limbs_multiplication. Binary-size review of header bloat.
- **Constant-time audit**: ec_scalar_mul + montgomery_pow are variable-time
  (documented); safe for verify, questionable for sign — decide policy.
- **Windows schannel + win32-pool TLS**: compiles, untested live.
- **0-RTT** — early data (psk mode psk_dhe + early_data ext) untouched.
- Broader async coverage: async accept/mTLS paths exercised only on posix
  pool + io_uring.
