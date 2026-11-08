# Component: Observability and Tooling

Part of the [Nautilus 0.4 design](README.md) · Milestones M1 (corpus in CI),
then every milestone · Builds on #561 (statistics), #524 (backend explorer)
and the latency corpus of the design study (closed #562)

## 1. Purpose

Every 0.4 milestone is gated on measurements. This component provides them
and keeps them in CI.

## 2. The latency corpus

`nautilus-latency-corpus` (written for the design study in the closed #562;
added to `nautilus-0.4` in M1.10 at `nautilus/test/benchmark/latency-corpus/`)
compiles scalable functions through the public engine API only,
single-tier per backend, so the same driver also builds against 0.3. Per (function, size, backend) it reports:
- the medians of the per-stage timings;
- IR operations after conversion;
- heap allocations during the compile (by replacing the global
  `operator new`);
- the full statistics object as JSON.

| Function | Stresses |
|---|---|
| `straight` (N unrolled ops) | linear code; passes |
| `chainedIf` (N independent `if`s) | symbolic replay |
| `chainedIfRegion` | bounded replay |
| `loopBody` (`val` loop, N-op body) | loops; block arguments |
| `calls` (N `invoke`s) | calls |
| `distinctSites1024` | tag creation |

Additions in 0.4:
- **explicit control-flow twins** of `chainedIf` and `loopBody` (M2.5);
- **an extension-op case** (SIMD, M6.5);
- **a cache case:** cold, warm L1, warm L2, and L0 canonicalize + hash
  cost ([08 §5](08-caching.md), M5.7);
- **real kernels:** NebulaStream queries via the `nebulastream-0.4` job,
  and TPC-H kernels from nautilus-duck via the `tpch-duck` job
  ([16 §3](16-implementation-plan.md)).

The CI jobs that run it ([16 §3](16-implementation-plan.md), rule 5):
`latency-corpus` (callgrind, gated), `baselines-0.3` (records the 0.3
side), `asm-diff` (`asmjit` assembly) and `nebulastream-cache` (warm vs.
cold, [08 §5.1](08-caching.md)).

How it is measured (D9):
- **Gates use instruction counts under callgrind in a pinned CI container
  image**, expressed as ratios against the recorded 0.3 baseline of the
  same function, measured in the same container. Each per-op target becomes
  a ratio: a case whose budget is a fraction *f* of its 0.3 time is gated
  at *f* times 0.3's instruction count; "no slower than 0.3" is a ratio of
  at most 1. This assumes 0.3 and 0.4 run at a similar IPC; the reference
  machine checks the µs/op budget itself at the milestone exit.
- **Costs with no 0.3 counterpart** (L0 canonicalize + hash, the cache
  cases, extension ops) have no ratio. Their instruction counts are
  recorded by `latency-corpus` from the WP that adds them and gated
  against that first recording (ratio ≤ 1 plus the noise margin); their
  absolute budgets (such as 0.05 µs/op for L0 hashing, [08 §5](08-caching.md))
  are checked on the reference machine.
- **Counts are reproducible only with dispatch pinned.** Under valgrind,
  glibc's ifunc selection (`memcpy`, `memset`) and the runtime dispatch of
  XXH3 and CRC-32C still follow the host's CPUID, so the container fixes
  them: `GLIBC_TUNABLES` disables the hwcaps above the x86-64 baseline, and
  the callgrind build selects the baseline XXH3 and CRC-32C code paths.
  The corpus runs single-tier (no background promotion thread) with ASLR
  off (`setarch -R`), because hash tables keyed by addresses change their
  probe counts with the layout. Counts are compared only between runs of
  the same image.
- **Wall-clock targets** (the µs/op budgets, warm vs. cold, time to tier 1,
  `ExecutionBenchmark`) are measured on **one dedicated reference
  machine**, at milestone exits only. It is procured and recorded in M1.10
  (with the owner named in [16 §16](16-implementation-plan.md)): CPU model,
  frequency policy, kernel and compiler go into the corpus output, and the
  machine stays fixed for the whole release. Until it exists, wall-clock
  targets are reported only.
- **Shared CI runners** run the corpus in the benchmark workflow with the
  existing 200% alert. Their wall-clock numbers are reported, not gated; a
  regression there is confirmed only after a rerun. Today `pr.yml`,
  `benchmark.yml` and `nebulastream.yml` trigger only for `main`; M1.11
  adds `nautilus-0.4` to them.
- The release criteria ([README §6](README.md)) are checked on both the
  instruction counts and the reference machine.
- **0.3 is not run beside 0.4 on every PR.** "No slower than 0.3" and "op
  counts ≤ 0.3" compare with **recorded 0.3 baselines** (corpus instruction
  counts and stage timings, IR op counts after 0.3's passes, `asmjit`
  assembly of the corpus, the inferred `FunctionAttributes` of every
  execution-test function, `ExecutionBenchmark`), taken at the branch
  point; the TPC-H numbers (tracing time, re-run share, copies, op counts,
  execution time) are added when the `tpch-duck` job exists (M2.3), by
  building nautilus-duck against `main` in the same job.
- **The `baselines-0.3` job runs on `nautilus-0.4`:** it checks out `main`,
  builds the same corpus driver against it in the same pinned container,
  and records the results ([16 §2](16-implementation-plan.md)). No job or
  driver is added to `main`, which takes only fixes.

### 2.1 The callgrind gate, as built (M1.10, M1.11)

The `callgrind-gate` job of `pr.yml` is the instruction-count gate of D9; `tools/latency/run-callgrind-local.sh` runs the
same measurements without the image.

- **What is counted.** For every (function, N, backend) case of the corpus on `tbc` and `asmjit`, the instructions (Ir)
  of the one call that compiles, `registerFunction` through tracing, conversion, passes and the backend: the driver's
  `nautilusCorpusCompile`, selected with `--toggle-collect`. Process start-up, the run of the compiled code and the
  reporting are not counted. Besides the total, the inclusive cost of the tracer, of the rest of the front end
  (conversion and passes) and of the backend (`compileIR`) are taken from the same profile, so that the stage budgets
  of the plan have a ratio of their own. `mlir` and `cpp` are not gated: their cost is LLVM's and the system compiler's.
- **The reference.** The same driver built from `main` in the same job and image and run under the same setup. The
  in-binary 0.3 pipeline (`tracing.nirTracer=false`) is no substitute: it shares the compiler and library code this
  branch has since made cheaper and counts 1% to 30% fewer instructions than `main` on the corpus.
- **Pinned dispatch.** `env -i`, `GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX...` (glibc's ifunc selection of the string
  functions then takes the x86-64 baseline variants whatever the host's CPUID, which valgrind passes through),
  `LD_BIND_NOW=1` (lazy binding is CPU-dependent), `NAUTILUS_DETERMINISTIC=1` (below), single tier (the driver pins the
  backend, so no promotion thread), ASLR off when the container allows `setarch -R`. XXH3 (`XXH_INLINE_ALL`) and the
  CRC-32C of NIR images (a table) choose their code when the library is built, not by CPUID, so the library has no
  dispatch to pin.
- **Sources of non-determinism found and fixed.** The compilation unit id read the clock and the entropy source: the
  per-second cache of the formatted date made the count depend on when a second ended (a few thousand instructions in a
  0.2 s profile), `std::random_device` costs differ with RDRAND, and the process token of NIR modules is random too.
  `NAUTILUS_DETERMINISTIC=1` replaces them by counters and a constant (`common/Deterministic.hpp`, `docs/options.md`); the
  clock and random reads of the normal path are unchanged. What is left of it on the 0.3 side is the unit id's clock
  (about 0.004% of a count), far below the margin. The size of the environment moved the stack and with it a few dozen
  instructions of address-dependent paths: `env -i` removes that.
- **The gate.** Ratios (NIR over 0.3) per case and metric against `nautilus/test/baselines/callgrind-gates.json`. Each
  entry has a `threshold`, a ratchet at the recorded ratio plus 3% for build differences, which fails the job when
  exceeded and only moves down (`callgrind-gate.py update`; raising it needs `--allow-raise` and a reason in the PR),
  and a `target`, the plan's budget as a fraction of 0.3's cost: 0.75 µs/op end to end and 0.25 µs/op for tracing,
  conversion and `asmjit`'s backend on linear code (`straight` and `loopBody` from N = 1000, the fraction taken from the
  recorded 0.3 µs/op), at least 30% cheaper tracing on `distinctSites1024`, and 1.0 everywhere else ("no case slower
  than 0.3"). A ratio above its target is a `WARN` in every run until it is met; targets are never relaxed to what is
  measured. A target that is met is enforced through the threshold.
- **The identity check.** The job runs this branch's corpus twice in the same container and requires every count of
  every metric to be equal; any difference names the cases and fails the job. A changed environment fingerprint
  (valgrind, libc, libstdc++, the compiler, the pinned tunables) fails it too, with the instruction to re-record the
  thresholds in the new image.
- **The image.** The job builds `.github/docker/ci-0.4` itself (`docker build`, cached by GitHub): the base image by
  digest, Ubuntu packages from a dated snapshot of the archive, Clang 21 by exact version. Publishing and naming a digest
  is the owners' follow-up (`.github/docker/README.md`).

Status of the targets at the first recording: end to end, every case is far inside its budget (ratios 0.13 to 0.66,
`calls` the highest); the unmet targets are the tracer on linear code (0.63 to 0.78 of 0.3's cost against targets of 0.58 and 0.67),
`asmjit`'s backend on `loopBody` (0.52 against 0.43) and the tracer on `calls`, which is 13% to 19% more expensive than
0.3's (target: not slower). They are reported by every run and ratcheted at today's value.

## 3. Statistics

`CompilationStatistics` keys, extended per component:

| Area | Keys |
|---|---|
| Tracing | from #561: `tracing.ms`, `tracing.firstRun.ms`, `tracing.rerun.ms`, `tracing.iterations`, `tracing.replayedOperations`, `tracing.snapshots`, `tracing.copies`, `tracing.operations`; new ([01 §4](01-tracing-frontend.md)): `tracing.rerun.{follow,record,paused}.ms`, `tracing.pausedOperations`, per-scope re-run counts, `tracing.copies.{construct,assign,explicit}` |
| Conversion | `conversion.ms`, `conversion.folded`, `conversion.cse`, `conversion.deadBlocks`, `conversion.refoldSweeps`, `conversion.refoldedInsts` ([03 §3.2](03-conversion-and-passes.md)) |
| Passes | `irPasses.<pass>.ms`, `.runs`, `.changes` (#561); `irPasses.pipelineIterations` (0 when no optional pass runs, [03](03-conversion-and-passes.md)) |
| Backend | `backend.totalMs`, per-backend sub-stages, code size |
| Reuse | `cache.level` (L0/L1/L2/miss), `cache.reason` ([08 §4](08-caching.md)), `cache.hashMs` (canonicalize + hash), `cache.payloadRejected` (which descriptor check failed, [07 §2.2](07-serialization.md)), `artifact.eligible`, `artifact.ineligibleReason` |
| Memory | trace and NIR arena bytes, bytes held by the arena pools, allocations (debug builds) |

Statistics stay opt-in per compile. When they are not collected, the cost is
an increment per counter and no walks.

## 4. Dumps and tools

- **One printer** for both NIR levels. `dump.*` options print the trace,
  the SSA level after conversion, after legalization and after optional
  passes.
- **Pre-SSA images** ([01 §6](01-tracing-frontend.md)) as reproducers,
  attached to fuzzer findings and bug reports, and loadable by the
  playground (M5.10).
- **The backend explorer (#524)** reads NIR and the statistics to compare
  compile time and code quality per backend.
- **The differential fuzzer** gains ([16](16-implementation-plan.md)):
  - a `NirChain` mode that runs the new chain before the cut-over (M3.1);
  - mixed explicit and implicit control flow (M2.5);
  - extension ops with handlers on and off (M6.6);
  - serialization round trips (M5.5);
  - L2 hits (M5.7).

## 5. Exit criteria

- **M1:** the corpus runs in CI on `nautilus-0.4` in the pinned callgrind
  container; the P0 baseline numbers are reproduced as the reference; the
  0.3 baselines are recorded in the same container; two runs of the same
  image give identical instruction counts.
- **Each later milestone:** its exit criterion is met on the corpus, and no
  other corpus case regresses by more than noise.
