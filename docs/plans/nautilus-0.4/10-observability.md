# Component: Observability and Tooling

Part of the [Nautilus 0.4 design](README.md) · Milestones M1 (corpus in CI),
then every milestone · Builds on #561 (statistics), #524 (backend explorer)
and the latency corpus of the design study (closed #562)

## 1. Purpose

Every 0.4 milestone is gated on measurements. This component provides them
and keeps them in CI.

## 2. The latency corpus

`nautilus-latency-corpus` (written for the design study in the closed #562;
added to `nautilus-0.4` in M1) compiles scalable functions through the
public engine API, single-tier per backend. Per (function, size, backend) it reports:
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
- **a cache case:** cold, warm L1, warm L2;
- **real kernels:** NebulaStream queries via the existing CI job, and TPC-H
  kernels from nautilus-duck.

How it is measured (D9):
- **Gates use instruction counts under callgrind** for every per-op target
  and every "no slower than 0.3" check. They are deterministic and do not
  depend on the runner, so CI can enforce them.
- **Wall-clock targets** (the µs/op budgets, warm vs. cold, time to tier 1,
  `ExecutionBenchmark`) are measured on **one dedicated reference
  machine**, fixed for the whole release and recorded in the corpus output
  (CPU model, frequency policy, kernel, compiler).
- **Shared CI runners** run the corpus in the benchmark workflow on
  `nautilus-0.4` with the existing 200% alert. Their wall-clock numbers are
  reported, not gated; a regression there is confirmed only after a rerun.
- The release criteria ([README §6](README.md)) are checked on both the
  instruction counts and the reference machine.
- **0.3 is not run beside 0.4.** "No slower than 0.3" and "op counts ≤ 0.3"
  compare with **recorded 0.3 baselines** (corpus instruction counts and
  stage timings, IR op counts after 0.3's passes, `asmjit` assembly of the
  corpus, `ExecutionBenchmark`, TPC-H), taken at the branch point and
  refreshed from `main` by the `baselines-0.3` job
  ([16 §2](16-implementation-plan.md)).

## 3. Statistics

`CompilationStatistics` keys, extended per component:

| Area | Keys |
|---|---|
| Tracing | from #561: `tracing.ms`, `tracing.firstRun.ms`, `tracing.rerun.ms`, `tracing.iterations`, `tracing.replayedOperations`, `tracing.snapshots`, `tracing.copies`, `tracing.operations`; new ([01 §4](01-tracing-frontend.md)): `tracing.rerun.{follow,record,paused}.ms`, `tracing.pausedOperations`, per-scope re-run counts, `tracing.copies.{construct,assign,explicit}` |
| Conversion | `conversion.ms`, `conversion.folded`, `conversion.cse`, `conversion.deadBlocks` |
| Passes | `irPasses.<pass>.ms`, `.runs`, `.changes` (#561) |
| Backend | `backend.totalMs`, per-backend sub-stages, code size |
| Reuse | `cache.level` (L0/L1/L2/miss), `cache.reason`, `artifact.eligible`, `artifact.ineligibleReason` |
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

- **M1:** the corpus runs in CI on `nautilus-0.4`; the P0 baseline numbers
  are reproduced as the reference; the 0.3 baselines are recorded.
- **Each later milestone:** its exit criterion is met on the corpus, and no
  other corpus case regresses by more than noise.
