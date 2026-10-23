# P0: Compile-Latency Baseline

Status: measured on 2026-10-23 · Plan step P0 of
[low-latency-plan.md](low-latency-plan.md) · Feeds decision D1 and the
Nautilus 2 design ([nautilus-2.md](nautilus-2.md)).

## 1. Setup

- **Driver:** `nautilus-latency-corpus` (`nautilus/test/benchmark/LatencyCorpus.cpp`,
  built with `ENABLE_BENCHMARKS=ON`). For each (function, size, backend) it
  compiles through the public engine API with `engine.backend` set
  (single-tier) and reports:
  - the medians of 5 repetitions of the per-stage timings the engine
    already records (`tracing.ms`, `irGeneration.ms`, `irPasses.totalMs`,
    `backend.totalMs`);
  - the IR operations after conversion;
  - the heap allocations during the compile, counted by replacing the global
    `operator new`;
  - the full statistics object as JSON.

  Usage: `nautilus-latency-corpus [reps [function [N [backend]]]]`.
- **Corpus:**

  | Function | Shape | What it stresses |
  |---|---|---|
  | `straight` | `r = r * 3 + i`, N times, unrolled by `static_val` | linear code; passes |
  | `chainedIf` | N independent `if (x > i) r = r + 1` | symbolic replay |
  | `chainedIfRegion` | the same, each `if` inside `region()` | bounded replay |
  | `loopBody` | a `val` loop whose body is N unrolled ops | loops, block arguments |
  | `calls` | N `invoke` runtime calls | calls |
  | `distinctSites1024` | 1024 textually distinct `r = r * 3 + 1` | tag creation, the tag trie |

- **Machine and build:** a 4-vCPU cloud VM (shared, so run-to-run noise
  reaches about ±30% on mid-size cases), GCC 14.2, Release,
  `ENABLE_MLIR_BACKEND=OFF`. MLIR numbers come from CI and #561 instead.
- **Code versions:** `main` at `4bb097e`, and #561 at `b9f7190`: per-thread
  MLIR/LLVM state reuse, open-addressing pass tables, one `FunctionRewriter`
  shared per pass group, and the new tracing counters.
- **Profiles:** `valgrind --tool=callgrind` (instruction counts) on `main`
  for three cases.

## 2. Where the time goes

`ms (share of total)`. `ops` = IR operations after conversion.
`allocs/op` = heap allocations per IR operation. `µs/op` = total
compile time per IR operation.

### `main`

| Case | Backend | Total ms | Tracing | IR gen | IR passes | Backend | ops | allocs/op | µs/op |
|---|---|---|---|---|---|---|---|---|---|
| straight 10000 | bc | 125.2 | 15.6 (12%) | 9.4 (8%) | **84.3 (67%)** | 13.9 (11%) | 50001 | 4.8 | 2.50 |
| straight 10000 | asmjit | 115.1 | 15.0 (13%) | 8.4 (7%) | **71.7 (62%)** | 22.9 (20%) | 50001 | 3.2 | 2.30 |
| loopBody 10000 | bc | 85.9 | 11.6 (14%) | 5.8 (7%) | **58.3 (68%)** | 11.8 (14%) | 40009 | 4.8 | 2.15 |
| chainedIf 300 | bc | 36.6 | **25.2 (69%)** | 1.0 (3%) | 8.3 (23%) | 2.1 (6%) | 2701 | 25.3 | 13.56 |
| chainedIfRegion 300 | bc | 5.9 | 0.9 (15%) | 0.4 (7%) | 3.5 (59%) | 0.9 (16%) | 2102 | 24.7 | 2.83 |
| calls 5000 | asmjit | 29.9 | 3.1 (10%) | 1.5 (5%) | 3.9 (13%) | **20.9 (70%)** | 10001 | 5.5 | 2.99 |
| distinctSites1024 | bc | 6.7 | **3.6 (53%)** | 0.3 (4%) | 1.7 (26%) | 0.9 (14%) | 4097 | 5.4 | 1.63 |

### #561

| Case | Backend | Total ms | Tracing | IR gen | IR passes | Backend | ops | allocs/op | µs/op |
|---|---|---|---|---|---|---|---|---|---|
| straight 10000 | bc | 63.9 | 14.9 (23%) | 7.8 (12%) | **31.0 (49%)** | 11.6 (18%) | 50001 | 4.8 | 1.28 |
| straight 10000 | asmjit | 82.7 | 14.2 (17%) | 7.2 (9%) | **36.4 (44%)** | 21.3 (26%) | 50001 | 3.2 | 1.65 |
| loopBody 10000 | bc | 74.3 | 16.9 (23%) | 7.1 (10%) | **39.6 (53%)** | 10.2 (14%) | 40009 | 4.8 | 1.86 |
| chainedIf 300 | bc | 18.2 | **14.3 (78%)** | 0.5 (3%) | 1.9 (10%) | 1.3 (7%) | 2701 | 21.3 | 6.73 |
| chainedIfRegion 300 | asmjit | 4.5 | 0.9 (21%) | 0.3 (7%) | 1.6 (35%) | 1.3 (30%) | 2102 | 17.8 | 2.15 |
| calls 5000 | asmjit | 27.2 | 2.9 (11%) | 1.5 (5%) | 1.5 (6%) | **20.8 (76%)** | 10001 | 5.6 | 2.72 |
| distinctSites1024 | bc | 6.1 | **3.8 (62%)** | 0.4 (7%) | 0.6 (10%) | 1.0 (17%) | 4097 | 5.4 | 1.49 |

### Tracing counters (#561)

| Case | Iterations | Replayed ops | Copies / trace ops |
|---|---|---|---|
| chainedIf 10 / 100 / 300 | 11 / 101 / 301 | 310 / 30,100 / **270,300** | 599 / 3,299 at 300 |
| chainedIfRegion 300 | 601 | **900** | 300 / 2,702 |
| straight 10000 | 1 | 0 | 10,001 / 60,002 |

Replay grows as N² (about 3·N² replayed ops). `region()` makes it linear.
The external TPC-H data in #561 agrees: **61% of tracing time is re-runs**
and 39% of recorded operations are copies.

### Scaling, `straight` on `main`, 1k → 10k (10x ops)

| Stage | Growth |
|---|---|
| Tracing | ~14x |
| IR gen | ~19x |
| IR passes | ~14x |
| asmjit (calls 1000 → 5000, 5x) | **~10–17x** |

All stages are mildly superlinear. AsmJit on call-heavy code is roughly
quadratic.

### Profiles (`main`, share of all instructions)

**`straight 10000`, asmjit:**

| Item | Share |
|---|---|
| `IRPassManager::run` | 61.9% |
| `FunctionRewriter` construction | **43.6%** |
| Hash-map code (`_Map_base`, `_Hashtable`, rehash, `std::hash`) | ~25% self |
| AsmJit lowering | 23.8%, of which `x86::Compiler::finalize` + RA passes 9.9% |
| Tracing | 10.0% |
| `malloc`/`free` | ~8% |

**`chainedIf 300`, bc:**

| Item | Share |
|---|---|
| Tracing | 72% |
| Replay cursor: `getCurrentBlock` 12.6%, `follow` 9.2%, `getCurrentOperation` 7.6% | ~30% self |
| `allocateValRef`/`freeValRef` | 11% |
| IR passes | 20% |

**`distinctSites1024`, bc:**

| Item | Share |
|---|---|
| `TagRecorder::createReferenceTagBuildin` (frame walk + linear trie child scan) | **31.8%**, i.e. 80% of tracing |
| IR passes | 33% |
| bc lowering | 18%, of which `countAllUsages` 6.7% |

### MLIR (not built here)

- CI tracing benchmark on `main`: `comp_mlir_*` takes 3.3–5 ms for trivial
  functions and 47 ms for `chainedIf100`, against 12–257 µs for `bc`.
- #561 on TPC-H: the fixed per-kernel cost drops from 10.2 ms to 4.9 ms. The
  median kernel is 266 MLIR ops.

## 3. Verdicts on the design hypotheses

| # | Hypothesis | Verdict | Evidence |
|---|---|---|---|
| H1 | Hash maps on the backend per-operand path dominate lowering | **Partly.** Real, but the backend is 11–30% of a compile; the dominant hash-map cost is in the *passes* | bc `countAllUsages` 6.7%; most hash time sits under `FunctionRewriter` |
| H2 | Rebuilding pass-local use tables dominates the passes | **Confirmed.** 44% of all instructions on `main` | `FunctionRewriter::FunctionRewriter`; #561 halves pass time, yet passes remain the **largest stage (44–53%)** on linear code |
| H3 | Per-op heap allocations in tracing | **Confirmed, moderate:** 3–5 allocations per IR op on linear code and 18–25 on branchy code (replay allocates); `malloc`/`free` about 8% of instructions | allocs/op column |
| H4 | Two materializations (trace + IR) | **Confirmed, moderate:** IR gen 7–12%, tracing 12–23% on linear code | stage tables |
| H5 | Redundant per-backend analyses | Minor (≤ 7%) | `countAllUsages` |
| T1 | Replay cost is paths × length | **Confirmed, dominant on branchy code:** N² replayed ops, 69–78% of the compile | 270,300 replayed ops for 300 ifs |
| T2 | Tag trie fan-out is linear | **Confirmed:** 3x tracing cost per op with many distinct call sites | 0.74 vs 0.25 µs per trace op; 32% in tag creation |
| T3 | Follow mode is cheap | **Partly.** No snapshots, but the cursor walk (`getCurrentBlock`/`getCurrentOperation`/`follow`) is about 30% of a replay-heavy compile | `chainedIf` profile |
| B1 | AsmJit's `Compiler`/RA is multi-pass and costly | **Confirmed, plus a new finding:** about 0.4 µs/op normally, **~4 µs per call** at 5000 calls (superlinear) | `calls` rows |
| B2 | MLIR is dominated by fixed setup | Confirmed (CI, #561); #561 addresses it | MLIR section above |

## 4. Consequences (decision D1)

1. **IR passes are still the largest stage on linear code after #561** (44–53%).
   Taking the fixed-point pipeline off the critical path moves *up*:
   - a single-round "cheap" pass level on today's IR as a quick win;
   - folding and CSE inside the conversion (plan P4) ahead of the flat
     trace (P2-d).
2. **Replay dominates branchy code** (70–78%). Three changes follow:
   - the explicit control-flow API (#300) moves from "deferred" into the
     release;
   - the replay cursor gets a dedicated fix (index-based walk; no
     reconciliation loop calling `getCurrentBlock`);
   - `region()` guidance gets automated statistics.
3. **Tag-trie fan-out is cheap to fix and worth about 3x tracing** on code
   with many call sites (real operators). P1-b stays first in line.
4. **AsmJit's call handling is superlinear.** Investigate and fix as a quick
   win: it is 70–76% of compile time on call-heavy code.
5. **The backend share is 11–30%.** The single-pass backend remains
   important, but behind items 1–3 for latency on large functions. For small
   functions on MLIR, the fixed cost (#561) matters most.
6. **3–25 allocations per op and two materializations** justify the flat
   representation as the strategic change. It also carries serialization
   and caching (see [nautilus-2.md](nautilus-2.md)).

Raw results (JSON lines, one object per case, with the full statistics) are
reproducible with the driver. These results were produced from the commits
above.
