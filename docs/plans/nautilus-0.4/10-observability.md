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
- **explicit control-flow twins** of `chainedIf` and `loopBody`;
- **an extension-op case** (SIMD);
- **a cache case:** cold, warm L1, warm L2, and L0 canonicalize + hash
  cost ([08 §5](08-caching.md));
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
  execution-test function, `ExecutionBenchmark`, TPC-H), taken at the
  branch point.
- **The `baselines-0.3` job runs on `nautilus-0.4`:** it checks out `main`,
  builds the same corpus driver against it in the same pinned container,
  and records the results ([16 §2](16-implementation-plan.md)). No job or
  driver is added to `main`, which takes only fixes.

## 3. Statistics

`CompilationStatistics` keys, extended per component:

| Area | Keys |
|---|---|
| Tracing | from #561: `tracing.ms`, `tracing.firstRun.ms`, `tracing.rerun.ms`, `tracing.iterations`, `tracing.replayedOperations`, `tracing.snapshots`, `tracing.copies`, `tracing.operations`; new ([01 §4](01-tracing-frontend.md)): `tracing.rerun.{follow,record,paused}.ms`, `tracing.pausedOperations`, per-scope re-run counts, `tracing.copies.{construct,assign,explicit}` |
| Conversion | `conversion.ms`, `conversion.folded`, `conversion.cse`, `conversion.deadBlocks` |
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
  playground.
- **The backend explorer (#524)** reads NIR and the statistics to compare
  compile time and code quality per backend.
- **The differential fuzzer** gains:
  - mixed explicit and implicit control flow;
  - extension ops with handlers on and off;
  - serialization round trips;
  - L2 hits.

## 5. Exit criteria

- **M1:** the corpus runs in CI on `nautilus-0.4` in the pinned callgrind
  container; the P0 baseline numbers are reproduced as the reference; the
  0.3 baselines are recorded in the same container; two runs of the same
  image give identical instruction counts.
- **Each later milestone:** its exit criterion is met on the corpus, and no
  other corpus case regresses by more than noise.
