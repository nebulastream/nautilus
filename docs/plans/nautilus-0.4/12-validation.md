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
- **Performance gates use deterministic counts** (D9,
  [10 §2](10-observability.md)):
  - CI gates are **callgrind instruction counts in a pinned CI container
    image**, expressed as ratios against the recorded 0.3 baseline of the
    same function in the same container: a per-op budget that is a
    fraction *f* of 0.3's time is gated at *f* times 0.3's instruction
    count, and "no case slower than 0.3" at a ratio of at most 1. Counts
    are compared only within one container image, with CPU dispatch pinned;
  - exact counts (replayed ops, trace passes, op counts, allocations,
    threads, pool bytes) are checked exactly;
  - the absolute µs/op budgets, warm-vs-cold, time to tier 1 and
    `ExecutionBenchmark` are checked on a **dedicated reference machine** at
    milestone exits only. Shared CI runners only report them, with the
    200% alert.
  - The reference machine is procured and recorded in M1.10 (owner per
    [16 §16](16-implementation-plan.md)); until then wall-clock targets are
    reported only.
- **Resource tests check counters, not RSS.** Thread counts, arena-pool
  bytes and queue lengths are exact; RSS on shared runners is not.
- **Every new option, API or format gets a negative test:** an invalid
  value is rejected with a clear error.
- **Deprecated surfaces keep a test** (D17): each deprecated alias of the
  user-facing API (renamed options) is exercised by a test that checks it
  still works and warns once.
- **The user-facing API is checked by `test/api-compat/`,** frozen at the
  branch point, from the cut-over on ([16 §2](16-implementation-plan.md)).
  It holds only those 0.3 user-API tests and examples that include public
  headers only and neither name a backend string nor parse IR or dump text;
  the others are ported with their components.
- **The whole matrix runs on `nautilus-0.4`:** GCC 14, Clang 19/21, macOS,
  ARM, ASan, UBSan and TSan (separate legs: the sanitizer options are
  mutually exclusive in `CMakeLists.txt`; M1 adds the UBSan preset and
  leg, which `pr.yml` lacks today), `ENABLE_COMPILER=OFF`, plus
  NebulaStream from M1 on, gating from the M4 exit (D16). Today `pr.yml`,
  `benchmark.yml` and `nebulastream.yml` trigger only for `main`; M1.11
  adds `nautilus-0.4`.
- **Comparisons with 0.3 use recorded baselines,** recorded by a job on
  `nautilus-0.4` that checks out `main` and runs the same corpus driver in
  the same container ([10 §2](10-observability.md),
  [16 §2](16-implementation-plan.md)).

## 3. Tests per component

| Component | Correctness | Performance and resources | New infrastructure |
|---|---|---|---|
| **01 Tracing** | `tracing/` reference dumps (regenerated in isolated commits, once per behavior-changing WP); fuzzer with mixed implicit and explicit control flow; #300's tests, with its mixing guard turned into mixing tests; a fuzzer mode that forces optimistic arms to restart | R1/C1 counters on the corpus and TPC-H; explicit `chainedIf`: one pass and zero replayed ops, checked exactly; no `malloc` per recorded op (corpus allocation counter) | TPC-H tracing (nautilus-duck) as a CI job; fuzzer generator for explicit constructs |
| **02 NIR, 13 spec** | Verifier unit tests for both levels, one positive and one negative test per diagnostic code of [13](13-nir-spec.md) (each atomic verifier rule of §6 and each loader rejection of §8) and per opcode of §5; round trip builder → printer → parser → serializer → loader, byte-identical, on every opcode and golden image; arena chunk-pool unit tests (budget, decay, `trimMemory`, page release) | Pool counters after a large compile return to the budget; the long-running engine test (40,000 modules) on counters | **A loader and verifier fuzzer** on malformed images: no crash, no undefined behavior, a clean rejection; run by the weekly libFuzzer sweep (`fuzz-weekly.yml`), with its seeds in the PR jobs (`fuzz-libfuzzer-smoke` runs seeds only, `-runs=0`; `fuzz-replay-smoke`); `nautilus-nir-opt` (`.nir` text tests) and `nautilus-nir-run` (a `.nir` file on one backend) |
| **03 Conversion** | Fuzzer on every backend; every execution-test function through new tracer → conversion → `tbc` before the cut-over (`NirChain`); merge-heavy corpora (regions, nested loops, `break`) | no optional pass runs on a default compile (`irPasses.pipelineIterations` = 0, checked exactly); conversion instruction count per op; op counts ≤ the recorded 0.3 baseline on the corpus and TPC-H kernels | An op-count report against the recorded baseline |
| **04 Extensions** | Every plugin suite on every backend; fuzzer with each handler on and off; a debug mode that ignores traits (A/B); late registration rejected with an error (D13) | No lock on the lookup path (TSan plus a lock-count check) | Fuzzer generation of extension ops (`simd.vec`, `builtin.i128`) |
| **05 Backends** | Execution tests and fuzzer per backend; the D10/D11 edge cases on each backend from its M4 port ([06 §4](06-semantics.md)); for `asmjit`, an assembly diff against the recorded 0.3 assembly of the corpus (code quality by construction); before the cut-over, every opcode on every port through `nautilus-nir-run`; exception tests per backend; debug info and profiling tests (D14) | Backend instruction count per op; `calls` linear in N; `ExecutionBenchmark` and TPC-H run time per backend on the reference machine | A CI job that diffs `asmjit` assembly dumps against a recorded baseline |
| **06 Semantics** | Exception tests plus MLIR calls without cleanup; fuzzer across all widths and signedness; division by zero, `MIN / -1` and oversized shifts generated on purpose and checked against the defined semantics (D10); float edge cases: NaN, signed zero, infinities (D11) | Share of call sites with exception handling, from the statistics; cost of division checks and shift masks on `ExecutionBenchmark` | Tests with a wrongly declared `nothrow`/`pure` callee, which the debug-mode checks must catch |
| **07 Serialization** | Save → load on every backend, identical results, identical bytes on re-save; every negative test of #527, #528 and #532; provenance tests (an unnamed traced pointer makes a module ineligible and its image process-local; folding keeps the least trusted class); signature checks on width and extension kind; payload rejection on a CPU or feature mismatch, an unaccepted relocation and an unlisted symbol ([07 §2.1–§2.2](07-serialization.md)); `NAUTILUS_INLINE` modules ineligible or reloading correctly | Native hit in a fresh process with ASLR: zero tracing, conversion and codegen in the statistics | **Two processes produce the same structural hash** for the same module (cache-key determinism), equal to the hashes recorded for the golden images; 0.4.x compatibility fixtures (D12) for images and for artifacts with payloads |
| **08 Caching** | #528's storage, compatibility and concurrency tests; an L2 hit across different caller keys; an L1 hit with `manual` promotion skips tracing; corrupt, stale, full-disk and size-bound tests and single flight ([08 §5](08-caching.md)); a debug mode that re-traces on hit and compares | Warm wall time ≤ 40% of cold on the NebulaStream suite ([08 §5.1](08-caching.md)); canonicalize + hash ≤ 0.05 µs/op | Cold / L1 / L2 / L0-hash cases in the corpus; the `nebulastream-cache` workflow and NebulaStream's per-query key patch (M5.7) |
| **09 C API** | #531's strict C11 programs on every backend; the export test (`nautilus_*` only); C and C++ save paths byte-identical; exceptions and division by zero returned as a status; stale handles rejected; a namer/resolver round trip in a fresh process ([09 §4](09-c-api.md)) | — | A mechanical binding smoke test (Rust via bindgen) |
| **10 Observability** | Statistics keys present and consistent (unit tests) | The P0 numbers reproduced as the M1 reference; identical instruction counts on two runs of the pinned container | The corpus driver (M1.10); the pinned callgrind container image |
| **11 Tiered** | The tests of [11 §6](11-tiered-compilation.md): long-running engine, cancellation, waits, shutdown, failure, policy, cache hits; fuzzer with forced promotion | Thread and queue counters; time to tier 1 on the reference machine | Fault injection: a tier-1 backend that throws |

## 4. Platforms without full CI coverage

- **GPU (D15, D21):** there is no GPU runner. Metal kernels execute on the
  hosted macOS runner (Apple silicon), with Metal API and shader validation
  from M7 on. CUDA is checked in CI by the CPU grid emulator and by compiling
  every kernel with NVRTC and `ptxas`; with no NVIDIA hardware, CUDA ships as
  experimental ([17 §5](17-gpu.md)). These legs rely on the plugin loading
  `libcuda` and `libnvrtc` with `dlopen` at the first device query and
  linking neither ([17 §4.1](17-gpu.md)), since the runners have no
  `libcuda.so.1`. A manual Metal run on an Apple-silicon Mac stays on the M8 release
  checklist. From the cut-over until M7 the GPU plugin is off and the GPU
  legs build without it ([17 §3](17-gpu.md)).
- **AArch64:** the ARM runner covers `asmjit`, `tbc` and MLIR.
- **Native payloads** are Linux x86-64 ELF in 0.4 ([07](07-serialization.md));
  other platforms test the NIR-image path.

## 5. Exit criteria

- **M1:** `pr.yml`, `benchmark.yml` and `nebulastream.yml` run for
  `nautilus-0.4`, with the UBSan leg; the loader/verifier fuzzer, the
  corpus driver in the pinned callgrind container and the NebulaStream job
  on `nautilus-0.4` are running; the reference machine is set up or its
  owner named; the 0.3 baselines and `test/api-compat/` are recorded.
- **The cut-over:** every execution-test function passes through the new
  chain (`NirChain`) before it, and every test plus `test/api-compat/`
  passes on `tbc` after it.
- **Each later milestone:** the rows of §3 for its components are green.
- **M7:** [17 §6](17-gpu.md).
- **M8:** every row is green on x86-64 and AArch64, and the manual Metal run
  is recorded.
