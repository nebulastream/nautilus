# Component: Validation

Part of the [Nautilus 0.4 design](README.md) · Every milestone · Builds on
the differential fuzzer (#544), the reference dumps in `test/data`, the
plugin suites, the sanitizer and NebulaStream CI jobs, and the latency
corpus ([10](10-observability.md))

## 1. Purpose

This document says how each component of 0.4 is tested: what proves it
correct, what proves it meets its performance and resource targets, and
which test infrastructure has to be built for it. The milestone exit
criteria in each component document refer to these tests.

## 2. Rules for every milestone

- **Correctness is checked behaviorally.** The differential fuzzer compares
  every backend and the interpreter with an independent C++ evaluation of
  the same program. Reference dumps check formats, not behavior.
- **Performance gates use deterministic counts** (D9):
  - per-op targets (µs/op budgets, "no case slower than 0.3") are checked
    on **instruction counts under callgrind**, which do not depend on the
    runner;
  - exact counts (replayed ops, trace passes, op counts, allocations,
    threads, pool bytes) are checked exactly;
  - wall-clock targets (the µs/op budgets themselves, warm-vs-cold, time to
    tier 1, `ExecutionBenchmark`) are measured on a **dedicated reference
    machine**. Shared CI runners only report them, with the 200% alert.
  - The reference machine is named in [10](10-observability.md) and fixed
    for the whole release.
- **Resource tests check counters, not RSS.** Thread counts, arena-pool
  bytes and queue lengths are exact; RSS on shared runners is not.
- **Every new option, API or format gets a negative test:** an invalid
  value is rejected with a clear error.
- **Deprecated surfaces keep a test** (D17): each shim (old intrinsic API,
  renamed options, `waitForPendingPromotions`) is exercised by a test that
  checks it still works and warns once.
- **The whole matrix runs on `nautilus-0.4`:** GCC 14, Clang 19/21, macOS,
  ARM, ASan/UBSan, TSan, `ENABLE_COMPILER=OFF`, plus NebulaStream from M1
  on (D16).

## 3. Tests per component

| Component | Correctness | Performance and resources | New infrastructure |
|---|---|---|---|
| **01 Tracing** | `tracing/` reference dumps (regenerated once); fuzzer with mixed implicit and explicit control flow; #300's tests, with its mixing guard turned into mixing tests; a fuzzer mode that forces optimistic arms to restart | R1/C1 counters on the corpus and TPC-H; explicit `chainedIf`: one pass and zero replayed ops, checked exactly; no `malloc` per recorded op (corpus allocation counter) | TPC-H tracing (nautilus-duck) as a CI job; fuzzer generator for explicit constructs |
| **02 NIR** | Verifier unit tests for both levels; round trip bridge → printer → serializer → loader, byte-identical, on every execution test; arena chunk-pool unit tests (budget, decay, `trimMemory`, page release) | Pool counters after a large compile return to the budget; the long-running engine test (40,000 modules) on counters | **A loader and verifier fuzzer** on malformed images: no crash, no undefined behavior, a clean rejection |
| **03 Conversion** | Fuzzer on every backend; the bridge comparison (0.3 pipeline vs. conversion on the same trace: same results, op counts no higher); merge-heavy corpora (regions, nested loops, `break`) | `irPasses.totalMs` ≈ 0; conversion instruction count per op; op counts ≤ 0.3 on the corpus and TPC-H kernels | An op-count comparison report (bridge vs. conversion) |
| **04 Extensions** | Every plugin suite on every backend; fuzzer with each handler on and off; a debug mode that ignores traits (A/B); late registration rejected with an error (D13) | No lock on the lookup path (TSan plus a lock-count check) | Fuzzer generation of extension ops (`simd.vec`, `builtin.i128`) |
| **05 Backends** | Execution tests and fuzzer per backend; for `asmjit`, an assembly diff against 0.3 on the corpus (code quality by construction); exception tests per backend; debug info and profiling tests (D14) | Backend instruction count per op; `calls` linear in N; `ExecutionBenchmark` and TPC-H run time per backend on the reference machine | A CI job that diffs `asmjit` assembly dumps against a recorded baseline |
| **06 Semantics** | Exception tests plus MLIR calls without cleanup; fuzzer across all widths and signedness; division by zero, `MIN / -1` and oversized shifts generated on purpose and checked against the defined semantics (D10); float edge cases: NaN, signed zero, infinities (D11) | Share of call sites with exception handling, from the statistics; cost of division checks and shift masks on `ExecutionBenchmark` | Tests with a wrongly declared `nothrow`/`pure` callee, which the debug-mode checks must catch |
| **07 Serialization** | Save → load on every backend, identical results, identical bytes on re-save; every negative test of #527, #528 and #532; provenance tests (a `RawProcessValue` makes a module ineligible) | Native hit in a fresh process with ASLR: zero tracing, conversion and codegen in the statistics | **Two processes produce the same structural hash** for the same module (cache-key determinism); 0.4.x compatibility fixtures (D12) |
| **08 Caching** | #528's storage, compatibility and concurrency tests; an L2 hit across different caller keys; a debug mode that re-traces on hit and compares | Warm NebulaStream suite ≥ 60% faster than cold, measured as in #529 | Cold / L1 / L2 cases in the corpus |
| **09 C API** | #531's strict C11 programs on every backend; the export test (`nautilus_*` only); C and C++ save paths byte-identical | — | A mechanical binding smoke test (Rust via bindgen) |
| **10 Observability** | Statistics keys present and consistent (unit tests) | The P0 numbers reproduced as the M1 reference | The corpus driver (M1) |
| **11 Tiered** | The tests of [11 §6](11-tiered-compilation.md): long-running engine, cancellation, waits, shutdown, failure, policy, cache hits; fuzzer with forced promotion | Thread and queue counters; hot-path cost of `calls:<N>`; time to tier 1 on the reference machine | Fault injection: a tier-1 backend that throws |

## 4. Platforms without full CI coverage

- **GPU (D15):** CI has no GPU hardware. CUDA is tested compile-only
  (kernel outlining, NIR → PTX generation, verifier). Metal runs on the
  macOS runner where the runner exposes a device, and compile-only
  otherwise. A manual run on GPU hardware is part of the M7 release
  checklist.
- **AArch64:** the ARM runner covers `asmjit`, `tbc` and MLIR.
- **Native payloads** are Linux x86-64 ELF in 0.4 ([07](07-serialization.md));
  other platforms test the NIR-image path.

## 5. Exit criteria

- **M1:** the loader/verifier fuzzer, the corpus driver and the NebulaStream
  job on `nautilus-0.4` are running; the reference machine is set up.
- **Each later milestone:** the rows of §3 for its components are green.
- **M7:** every row is green on x86-64 and AArch64, and the GPU manual run
  is recorded.
