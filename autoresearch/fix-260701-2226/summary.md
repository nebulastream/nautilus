# Fuzz-and-fix campaign — Nautilus differential fuzzer

**Task:** run and extend the Nautilus fuzzer to cover a wider range of features, fixing all bugs found along the way.

**Result: 4 real miscompiles found and fixed, 3 fuzzer feature extensions, 5 regression tests, 8 commits.**

## Coverage extensions (fuzzer)

| Extension | Commit | What it exercises |
|---|---|---|
| Backend widening (build config) | — | AsmJit + C++ backends added to the differential net (previously interpreter+MLIR+BC only) |
| `val<bool>` logical ops (`LAnd`/`LOr`/`LNot`) | fbe1c56a | real `&&`/`||`/`!` on `val<bool>`, both domains |
| `invoke()` runtime calls (`Call`, `Callees.hpp`) | 28020ed4 | ProxyCall lowering: arg/return marshalling, narrow-int ABI; + survey saves `fuzz-finding-<n>.bin` |
| Trace-time static loops (`StaticLoop`) | 14724157 | `static_val` snapshot machinery, trace-time unrolling |
| Triage infra | e0bb4c72 (part) | `NAUTILUS_FUZZ_DUMP=1` dumps all compilation stages during replay |

## Bugs found & fixed (all verified by survey + full ctest suite, each pinned by a regression test)

1. **AsmJit A64: narrow-int arithmetic not re-normalized** (56730dbf) — wrapped u32/u16/u8 results kept stale high register bits; 64-bit compare/div/convert consumers mis-evaluated. Added `narrowToStamp` after add/sub/mul/shift/mvn + returns (mirrors x64). Pinned: `u32WrapSubThenCompare`.
2. **AsmJit x64+A64: narrow call returns not re-extended** (919e8925) — ABI leaves upper bits of sub-register-width returns unspecified; `ProxyCall`/`IndirectCall` results bound raw. Pinned: `i16NarrowCallReturnCompare`.
3. **MLIR: narrow call args missing `llvm.signext`/`zeroext`** (e0bb4c72) — Darwin AArch64 / x86-64 SysV require caller extension of sub-32-bit args; LLVM folded truncations away and passed garbage high bits (address-dependent, ~5/6 repro). Pinned: `i16NarrowCallArgCompare`.
4. **BC: merged-value corruption from block-arg identifier collision** (d4cb9632) — the BC variant of AsmJit issue #321/#322; emplace-only frame binding dropped an arm's re-definition of a merge-parameter identifier. Minimized via input truncation to a 3-block program. Pinned: `zeroTripLoopMergeThenAddConstant` (verified to fail pre-fix).

Bugs 1–3 are one invariant violated in three places: *narrow integers must stay sign/zero-extended in full-width registers, including across ABI boundaries*.

## Final state

- Survey runs of 1500–4000 inputs across interpreter + MLIR + C++ + BC + AsmJit: **0 findings**.
- Full ctest suite: **209/209 pass**.
- Results log: `results.tsv` in this directory.
