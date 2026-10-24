# Component: Tiered Compilation

Part of the [Nautilus 0.4 design](README.md) · Milestone M4 (with the
backends) · Builds on
`TieredJITCompiler` (0.3), #561 (MLIR state reuse), [08](08-caching.md)

## 1. Purpose

`TieredJITCompiler` returns a fast tier-0 executable (`asmjit`, `tbc` or the
interpreter) and promotes the module to a tier-1 executable (MLIR) in the
background. This document reviews how 0.3 does that, records what is wrong
with it, and proposes the 0.4 design. Per-tier IR levels, more than two
tiers and on-stack replacement stay out of scope (§9).

## 2. How 0.3 works

- **Configuration** (`TieredCompiler.cpp`, constructor):
  - `engine.backend=<x>` pins single-tier compilation with `<x>`;
  - otherwise tier 1 is `engine.tier1.backend` (default MLIR, or the
    registry's default backend if MLIR is not built);
  - tier 0 is `engine.tier0.backend`, or the first available of `asmjit`,
    `bc`, interpreter.
- **`compileModule`:**
  1. traces and converts once;
  2. compiles tier 0 on the caller's thread and installs it in
     `ModuleState`;
  3. calls `promoteAsync`, which starts **one `std::thread` per module**
     (finished ones are joined when the next promotion starts). That thread
     compiles the same IR with the tier-1 backend, then swaps
     `ModuleState::executable` under the module's exclusive lock and bumps
     its version.
- **Handles** (`ModuleFunction`) see the new version on their next call and
  re-resolve. Superseded executables stay alive for as long as the handle
  that resolved them (issue #506).
- **Waiting:** `waitForPendingPromotions()` joins every promotion thread.
  The compiler's destructor calls it.

## 3. Findings

| # | Finding | Evidence | Effect |
|---|---|---|---|
| F1 | **No bound on concurrent promotions:** one thread per module in flight | Code | N concurrent query compiles start N MLIR compiles (3–5 ms each, more for real kernels) that compete with query execution |
| F2 | **Thread per promotion defeats MLIR state reuse.** #561 reuses `DialectRegistry`/`MLIRContext` per thread | Code: every promotion runs on a fresh thread | Tier 1 pays MLIR's full fixed cost (10.2 ms per TPC-H kernel without #561, 4.9 ms with it) on every promotion |
| F3 | **No cancellation.** The promotion checks its `weak_ptr` to the module only when it swaps | Code | A module dropped right after tier 0 is still compiled to the end. Short NebulaStream queries pay a full tier-1 compile for nothing |
| F4 | **Blocking waits.** The destructor waits for every promotion, including those of dead modules. `waitForPendingPromotions()` holds `threadsMutex_` while it joins, and `promoteAsync` takes the same mutex | Code | Engine shutdown waits for all queued MLIR work. Every `compileModule` on another thread stalls behind a wait, *after* its tier-0 compile, before it returns |
| F5 | **Silent failure.** A failure is logged and the module stays on tier 0 | Code | Callers cannot see which tier runs, whether promotion failed, or wait for one module |
| F6 | **Promotion duplicates `CompilationPipeline::compileIR`** (dump handler, statistics, unit ID) | Code | Its unit ID has one-second resolution (`<date>_<time>_tier1`), so promotions within the same second share a dump directory. The tier-1 report is not linked to the tier-0 unit ID |
| F7 | `engine.tier0.backend`/`engine.tier1.backend` take effect only when **both** are set | Code, `docs/options.md` | Setting only `engine.tier1.backend=cpp` is silently ignored |
| F8 | **Always eager.** Every module is promoted immediately, however long it will live | Code | The same waste as F3, even for modules that stay alive but run only briefly |
| F9 | Tier 1 starts only after tier-0 code generation returns, though the IR is ready before it | Code | Time to tier 1 includes the tier-0 backend time (small next to MLIR, but free to remove) |
| F10 | A two-tier compile runs the IR passes of `max(tier 0, tier 1)` | Code (`irOptimizationLevel`) | `asmjit` asks for the full pipeline on the critical path; solved in 0.4 by the optimizing conversion ([03](03-conversion-and-passes.md)) |

Measured on `main` (GCC 14, Release, tier 0 `tbc`, tier 1 `asmjit`, a
200-op function): time to tier 1 is 0.38 ms on average. Superseded tier-0
executables stay resident for as long as the handles that resolved them
(#506). This is intended, and 0.4 keeps it (§4.8).

## 4. Design for 0.4

### 4.1 A promotion service with a fixed worker pool (F1, F2)

- **One `PromotionService` per engine,** owning a job queue and up to
  `engine.tiered.promotionThreads` workers (default
  `max(1, hardware_concurrency / 4)`), created lazily and kept for the
  engine's lifetime.
- **Bounded thread count** whatever the number of modules compiled or in
  flight (F1).
- **Long-lived workers make #561's per-thread MLIR state reuse effective**
  for every promotion after the first one on each worker.
- **Admission:** `engine.tiered.maxQueuedPromotions` (default unbounded). A
  job beyond the cap is not queued; its module stays on tier 0 with the
  status `skipped` (§4.3).
- **Workers run at lower OS priority** where the platform allows it
  (`nice` on Linux, a background QoS class on macOS), so promotions do not
  slow the caller's tier-0 compile or the running query.

### 4.2 Job lifecycle and cancellation (F3, F4)

- A job holds the module's `weak_ptr`, the immutable NIR module and the
  options. It records the states `queued → running → done | failed |
  cancelled | skipped`.
- **Cancellation:**
  - a job whose module is gone is dropped when dequeued;
  - while running, the backend checks a cancellation token at stage
    boundaries (MLIR: after MLIR generation and after LLVM optimization).
    That costs one relaxed load per stage.
- **Shutdown:** the engine cancels queued jobs and waits only for running
  ones, at most one compile per worker. No mutex is held while waiting, so
  compiles on other threads never block on a wait.

### 4.3 Status and API (F5)

| API | Meaning |
|---|---|
| `CompiledModule::tier()` | `interpreted`, `tier0` or `tier1` |
| `CompiledModule::promotionStatus()` | `none`, `queued`, `running`, `done`, `failed`, `cancelled` or `skipped`, plus the error message on failure |
| `CompiledModule::waitForPromotion(timeout)` | Waits for this module only |
| `CompiledModule::promote()` | Requests promotion now (policy `manual`, §4.4) |
| `NautilusEngine::waitForPromotions()` | Replaces the compiler-level `waitForPendingPromotions()` |

- **Failures** are caught with `catch (...)`, recorded in the status and in
  the statistics (`promotion.status`, `promotion.error`), and logged once.
  A failed promotion is not retried; the module keeps tier 0.
- **Statistics:**
  - `promotion.queueMs`, `promotion.compileMs` and
    `promotion.timeToTier1Ms`;
  - the tier-1 report links the tier-0 unit ID.

### 4.4 Promotion policy (F8)

`engine.tiered.promotion` selects when a module is promoted (D8, decided):

| Value | Behavior |
|---|---|
| `eager` (default) | As in 0.3: queued right after conversion (§4.5) |
| `manual` | Only on `CompiledModule::promote()` |

- **The default stays `eager`,** so behavior is unchanged unless a caller
  opts in.
- **`manual` lets the embedder decide.** For example, NebulaStream can
  promote a query once it is known to be long-running, and never promote
  one-shot queries.
- **No call-count policy in 0.4.** A `calls:<N>` policy would be the only
  change to the call hot path (the header-only `ModuleFunction`) and would
  need a threshold tuned per workload. Its benefit is small once
  cancellation (§4.2) stops promotions of dropped modules and a tier-1
  cache hit (§4.6) starts repeated queries on optimized code. It can be
  added later without a format or API break if NebulaStream data shows a
  need.

### 4.5 Start tier 1 as soon as NIR exists (F9)

With `eager`, the job is queued right after conversion, *before* tier-0 code
generation. SSA-level NIR is immutable after conversion
([02](02-nir.md)), so the worker and the caller's tier-0 backend read it
concurrently without copying. A tier 1 that finishes first is installed,
and the tier-0 executable is then discarded unused.

### 4.6 Caches and tiers ([08](08-caching.md))

- **A cache hit for the tier-1 backend skips tier 0 entirely.** Before
  tier-0 code generation, the engine looks up L0, L1 or L2 for the tier-1
  backend. On a hit it installs the tier-1 executable synchronously, with no
  tier-0 compile and no promotion. This is what makes warm NebulaStream
  queries start on optimized code.
- **Promotion results feed the caches:** a finished tier 1 is stored in L0
  and, with a persisted native payload, in L1/L2.
- **Tier-0 executables are cached only in L0.** They are cheap to rebuild
  from NIR.

### 4.7 Configuration (F7)

- `engine.tier0.backend` and `engine.tier1.backend` can each be set alone.
  The other keeps its default.
- An unknown or unavailable backend fails at engine construction, not at
  the first compile.
- The tier-0 fallback order is `asmjit` → `tbc` → interpreter, since `bc`
  is removed ([05 §6.1](05-backends.md)).
- `engine.backend=<x>` keeps pinning single-tier compilation.

### 4.8 One compile path (F6)

- **Promotion calls `CompilationPipeline::compileIR`,** so dumps,
  statistics and unit IDs come from one place. The tier-1 unit ID is the
  tier-0 unit ID plus `_tier1`, so the dumps of one module sit side by
  side.
- **Retention is unchanged:** superseded executables stay resident for as
  long as the handles that resolved them (#506). Releasing them earlier
  needs in-flight call tracking on the hot path and is not in 0.4.

## 5. Where each change lands

| Change | Target | Why |
|---|---|---|
| Promotion service, cancellation, shutdown (§4.1, §4.2) | 0.4, M4 | Builds on NIR's immutability and #561 |
| Status API, statistics, one compile path (§4.3, §4.8) | 0.4, M4 | New public API |
| Promotion policy (§4.4) | 0.4, M4, opt-in `manual` (D8) | Default unchanged |
| Early start (§4.5), cache interplay (§4.6) | 0.4, M4/M5 | Needs NIR and the caches |
| Configuration cleanup (§4.7) | 0.4, M4 | Behavior change for half-set options |

## 6. Tests

- **Long-running engine:** 40,000 short-lived modules on one engine. It
  completes, the thread count stays at most `promotionThreads + 1`, and the
  virtual memory stays flat.
- **Cancellation:** dropping a module while its job is queued or running
  ends that job without installing anything. A running job ends at the next
  stage boundary.
- **Concurrent compiles with a wait in progress:** `compileModule` on other
  threads does not block (F4).
- **Shutdown:** engine destruction with 100 queued jobs returns after at most
  one compile per worker.
- **Failure:** a tier-1 backend that throws a non-`std::exception` value
  leaves the module on tier 0 with the status `failed`.
- **Policy:**
  - `manual` never promotes without `promote()`, and `promote()` from
    several threads at once queues one job;
  - `eager` queues exactly one job per module.
- **Caches:** a warm L1/L2 hit runs tier 1 from the first call, with no tier-0
  compile recorded in the statistics.
- **The differential fuzzer** runs with promotion forced (`eager`, one
  worker) and checks results across the swap.

## 7. Exit criteria (M4)

- No unbounded resource growth with the number of modules (the
  long-running test of §6).
- With 0.3's defaults, `ExecutionBenchmark` and TPC-H are no slower.
- With one worker per four cores, mean time to tier 1 on the NebulaStream
  suite is no worse than 0.3. The MLIR fixed cost per promotion after
  warm-up is at #561's level.
- The status API, statistics and options are documented in
  `docs/options.md` and `docs/engine.md`.

## 8. Risks

| Risk | Mitigation |
|---|---|
| A small pool delays tier 1 under load | `promotion.queueMs` makes it visible; `engine.tiered.promotionThreads` tunes it |
| Cancellation points inside MLIR/LLVM are coarse | Stage boundaries are enough: a running job costs at most one compile per worker |
| Early start (§4.5) runs tier 0 and tier 1 at once | Workers run at lower OS priority (§4.1), so the caller's tier 0 keeps its core |
| Callers relying on `waitForPendingPromotions()` | Kept as a deprecated alias of `NautilusEngine::waitForPromotions()` through 0.4.x |

## 9. Not in 0.4

- Per-tier IR levels and more than two tiers.
- On-stack replacement: a single long-running call stays in the tier it
  started in until it returns. NebulaStream calls a pipeline once per
  buffer, so a promotion takes effect at the next buffer.
- Releasing superseded executables before their handles are destroyed.
- Profile-guided tier-1 compilation (feeding tier-0 counters into MLIR).
- A call-count promotion policy (`calls:<N>`, §4.4).
