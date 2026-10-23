# Component: Observability and Tooling

Part of the [Nautilus 0.4 design](README.md) · Milestones M1 (corpus in CI),
then every milestone · Builds on #561 (statistics), #562 (latency corpus),
#524 (backend explorer)

## 1. Purpose

Every 0.4 milestone is gated on measurements. This component provides them
and keeps them in CI.

## 2. The latency corpus

`nautilus-latency-corpus` (introduced in #562; moves to `nautilus-0.4` in
M1) compiles scalable functions through the public engine API, single-tier
per backend. Per (function, size, backend) it reports:
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

CI:
- the corpus runs in the benchmark workflow on `nautilus-0.4`, with the
  existing 200% alert;
- the release criteria ([README §6](README.md)) are checked against it;
- because the runners are shared, a regression is confirmed only after a
  rerun.

## 3. Statistics

`CompilationStatistics` keys, extended per component:

| Area | Keys |
|---|---|
| Tracing | `tracing.ms`, `tracing.firstRun.ms`, `tracing.rerun.ms`, `tracing.iterations`, `tracing.replayedOperations`, `tracing.snapshots`, `tracing.copies`, `tracing.operations` (#561) |
| Conversion | `conversion.ms`, `conversion.folded`, `conversion.cse`, `conversion.deadBlocks` |
| Passes | `irPasses.<pass>.ms`, `.runs`, `.changes` (#561) |
| Backend | `backend.totalMs`, per-backend sub-stages, code size |
| Reuse | `cache.level` (L0/L1/L2/miss), `cache.reason`, `artifact.eligible`, `artifact.ineligibleReason` |
| Memory | trace and NIR arena bytes, allocations (debug builds) |

Statistics stay opt-in per compile. When they are not collected, the cost is
an increment per counter and no walks.

## 4. Dumps and tools

- **One printer** for both NIR levels. `dump.*` options print the trace,
  the SSA level after conversion, after legalization and after optional
  passes.
- **Trace-level images** ([01 §5](01-tracing-frontend.md)) as reproducers,
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
  are reproduced as the reference.
- **Each later milestone:** its exit criterion is met on the corpus, and no
  other corpus case regresses by more than noise.
