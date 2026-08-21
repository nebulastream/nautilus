# Design: `nautilus::region` — Inline Isolated Scopes in the Execution Trace

**Status:** Proposed
**Audience:** Nautilus contributors; implementation agents
**Scope:** `nautilus/include/nautilus/tracing/`, `nautilus/src/nautilus/tracing/`,
`nautilus/include/nautilus/region.hpp` (new), `nautilus/test/execution-tests/RegionTest.cpp` (new)

---

## 1. Motivation

A `nautilus::function` creates a new tracing instance: its own `TagRecorder`,
`ExecutionTrace`, and `SymbolicExecutionContext`. This isolates tags, values, and
control flow, but also creates a separate compiled function the caller must
`invoke`. For lightweight inline sub-scopes — bounded traced logic recorded
*into the enclosing function's trace*, with a fresh tag/liveness domain and a
value scope — a function boundary is too heavy.

`nautilus::region` provides such an inline scope: fresh tag space and liveness
domain, values do not leak into the enclosing scope, yet region blocks are
ordinary basic blocks and region jumps ordinary JMPs. No `RegionOperation` exists
in the trace, IR, or any backend.

### Use cases

- **Liveness isolation**: limit intermediate-value lifetimes to a sub-scope.
- **Tag/liveness domain isolation**: fresh recorder so region tags never alias
  the enclosing context.
- **Memoized replay**: once a region is fully traced for a given outer state, it
  is never traced again unless that state changes (§4.4).

## 2. Goals / Non-Goals

### Goals

- API: `nautilus::region("name", [&]() { ... })` or `nautilus::region([&]() { ... })`
  (name optional, debug label only). **The lambda returns void.**
- Region ops record into the enclosing function's single `ExecutionTrace`; fresh
  entry block reached by a JMP from the enclosing block.
- Fresh `TagRecorder` per region call-site (identical tags across executions).
- Hash-combine environment (§4.2): entering a region never mutates the parent env.
- **Memoized replay: on a memo hit the region lambda is NOT executed** (§4.4).
- Nesting; `static_val` unrolling works across region boundaries; transparent in IR.

### Non-Goals

- No region attributes (name not persisted in trace/IR).
- No explicit exit/merge block.
- Non-void region lambdas (future work).
- Assigning to an *enclosing* `static_val` from inside a region is unsupported
  and not detected (static_val writes are invisible to the tracer; §4.8).

## 3. User-Facing API

`include/nautilus/region.hpp`:

```cpp
namespace nautilus {
// name is an optional debug label; not recorded in the trace.
template <typename F> void region(const char* name, F&& fn);
template <typename F> void region(F&& fn);
}
```

Captures are by reference (`[&]`); captured `val<T>`/`static_val<T>` are
read/written through the enclosing scope's normal tracing.

```cpp
val<int32_t> sum = 0;
nautilus::region([&]() {
    val<int32_t> tmp = sum + 42;   // traced; lives only inside the region
    sum = tmp;                     // escapes through the captured reference
});
```

## 4. Architecture

### 4.0 Unified region model: a function is its root region

Tracing a function = tracing its **root region**, created implicitly at trace
start and closed at function return. Every `region(...)` call creates a child
region whose parent is the enclosing context; `activeTracer` points at the
innermost region. All blocks/ops from any depth record into the root's single
`ExecutionTrace`.

```
activeTracer → RegionContext (innermost)
                 parent ↑ RegionContext
                 parent ↑ RootRegionContext   ← owns ExecutionTrace, SymbolicExecutionContext,
                                                recorder + region-memo caches; mode-specialized
                                                (ExceptionBased / Lazy)
```

Entry/exit machinery is uniform at every level; the root starts with an empty env
and no parent.

### 4.1 Region context and recorder

Each `region(...)` call creates a `RegionTraceContext` whose `TagRecorder` is
cached per call-site address (`__builtin_return_address(0)`, wrapper kept
`noinline`). Repeated execution of the same call site (loop iterations, symbolic
re-runs) reuses the same recorder → identical tags → stable snapshots. The cache
is cleared when a function trace starts.

### 4.2 Hash-combine environment

The region keeps only its **own delta** env: `region.staticVars` and
`region.aliveVars`, empty at entry, holding only statics pushed / refs
incremented inside the region. Region snapshots:

```
S = hashStaticVector(region.staticVars) ^ region.aliveVars.hash() ^ P
P = hashStaticVector(parent.staticVars) ^ parent.aliveVars.hash()   // parent's staticValueHash
```

`P` is computed once at region entry (O(#parent statics); `aliveVars.hash()` is a
cached O(1)). The parent env is never mutated while a region runs. XOR with the
constant `P` preserves all within-region merge decisions; `P` carries the parent
static counter values so distinct parent iterations yield distinct region
snapshots (the +0,+1,+2 case). Nested regions compose: an inner region's `P` is
the outer region's full combined hash.

### 4.3 Entry block and predecessor wiring

RECORD mode: the enclosing block appends a JMP to a freshly created block
(`ExecutionTrace::createBlock`), which becomes the current block; the new block
records the enclosing block as a **predecessor** (required — `SSACreationPhase`
walks `getPredecessors()`, `SSACreationPhase.cpp:174-228`). FOLLOW mode: no-op.

### 4.4 Memoized replay — "once traced, don't trace again"

Each region call site maintains a memo:

```
memo (per call-site): P → RegionExec { continuationOperationIndex, continuationBlockIndex }
```

On every region entry:

```
P = parent staticValueHash at entry
exec = memo[P]
if miss:
    RECORD → create entry block (predecessor-wired), record body ops into the
             shared trace; on exit, if the execution produced NO escaped refs
             (empty region delta aliveVars), cache RegionExec under P (the
             continuation position after the region's trailing JMP).
else:
    REPLAY → do NOT execute the lambda.
      RECORD-mode parent: emit nothing. The parent's first post-region traced op
        has the same recorder + P + structural position as the recorded
        continuation's first op → identical snapshot → it merges into the
        recorded continuation via the existing checkTag/processControlFlowMerge
        machinery (Lazy: paused_; Exception: unwind).
      FOLLOW-mode parent: advance the shared cursor to the recorded continuation
        (RegionExec.continuation*), then resume following.
```

**Why only closed executions are cached.** Escaping executions (region-local
refs alive at exit) change the parent's `aliveVars`, hence `P`, so a later entry
under the same `P` can never occur; caching them is pointless and would be
unsound (a skip would leave captured vals stale). Therefore every memo hit is a
*closed* execution — the region's only possible observable effect on the parent
is none — so skipping the lambda reproduces the recorded trace exactly. No
SSA-dominance concern exists because no interior-produced refs flow out (void
lambda, no escapes).

**Static-loop unrolling is preserved.** In `for (static_val<int> j = 0; j < 3;
j++) region(...) { sum += val<int32_t>(j); }`, each entry has a different `P`
(j advances) → memo miss → re-trace each iteration → +0,+1,+2, never +0,+0,+0.

### 4.5 Escape transfer (RECORD executions only)

For a region that escapes refs, on exit call `parent->allocateValRef(ref)` for
each ref still alive in the region delta, so enclosing destructors decrement
counted refs (parent `aliveVars` hash stays balanced). Escaping executions are
not cached (§4.4). No transfer happens on replay (no refs created).

### 4.6 Nesting

Each `RegionTraceContext` holds a parent pointer; inner regions combine the outer
region's full hash, maintain their own delta + memo, and perform their own escape
transfer. Replay of an outer region skips inner regions too.

### 4.7 Delegation

`RegionTraceContext` implements `TracingInterface` like the roots, recording into
the shared root `ExecutionTrace` and hashing against its own delta + `P`.

### 4.8 Unsupported: mutating an enclosing `static_val`

Assigning to an enclosing `static_val` from inside a region is unsupported and
not detected: `static_val` assignment is a plain memory write
(`static.hpp:93-118`) invisible to the tracer; detection would require write hooks
in the `static_val` hot path. Analysis shows it is not a correctness bug (region
`P` is captured from live memory at entry; regions are deterministic), so this is
a model-cleanliness limitation.

## 5. Interface and Structural Changes

- `TracingInterface` — new pure virtuals: `recordSnapshot()`, `getEnv()`,
  `getRootContext()`, `traceRegionBegin(TagAddress)`, `traceRegionEnd()`; ~6
  `recordSnapshot()` call sites (`ExceptionBasedTraceContext.cpp` ~105, 123, 147,
  257, 294, 352) retarget to `getActiveTracer()->recordSnapshot()`.
- `TraceContextBase` (new) — delta env (`staticVars`+`aliveVars`), parent pointer,
  region entry/exit helpers, escape transfer, recorder + memo caches.
- Root region (mode-specialized `ExceptionBased` / `Lazy`) — owns trace + SEC +
  caches.
- `RegionTraceContext` (new: `src/nautilus/tracing/RegionTraceContext.{hpp,cpp}`)
  — delta env, `P`, memo, RECORD/REPLAY dispatch.
- `include/nautilus/region.hpp` (new) — API wrapper.
- `ExecutionTrace.{hpp,cpp}` — entry-block + predecessor helpers.

## 6. Error Handling and Edge Cases

- **Empty region**: entry JMP + immediate merge; DCE removes it.
- **Region reads enclosing static**: value read through live memory; cross-
  iteration distinction via `P` (regression test 2).
- **Region escapes a value**: escape transfer (§4.5); the escape changes `P`, so
  the next entry re-traces (no skip) — correct.
- **Closed region in a dynamic loop**: traced once, skipped on later iterations.
- **Branching inside a region**: ordinary blocks/JMPs; snapshot/merge runs on the
  region delta + `P`.
- **Early return out of a region**: unsupported (inline scope); documented.
- **Tracing disabled**: `region` degrades to a plain lambda call.

## 7. Testing

`test/execution-tests/RegionTest.cpp` (auto-covered over both trace modes and all
backends via `test/common/ExecutionTest.hpp` `forEachBackendWithTraceMode`):

1. Basic: region increments an enclosing `val<int32_t>`; correct in all backends.
2. **Regression (static_val unrolling)**: `for (static_val<int> j = 0; j < 3;
   j++) region(...) { sum += val<int32_t>(j); }` must yield +0,+1,+2.
3. **Replay skip**: a closed region inside a dynamic loop is traced once, skipped
   on later iterations (verify via execution result correctness).
4. Value isolation: a `val<T>` created inside a region is not alive outside it.
5. Escape: a value created in a region and captured out stays alive, destroyed
   once by the enclosing scope (hash balance); region re-traced on next entry.
6. Branching inside a region; control flow rejoins the enclosing scope.
7. Nesting: two levels, inner reads an enclosing-loop static.
8. Empty region; region without a name.
9. Goldens: `test/data/region-tests/` if required.

## 8. Risks and Tradeoffs

- **Skip soundness relies on the `P`-proxy**: memo hits imply parent env identity;
  hash collisions are the residual risk (same trust as existing snapshot hashing).
- **Replay emits nothing in a RECORD-mode parent** and relies on the parent's
  post-region op snapshot matching the recorded continuation — must be verified
  before generalizing (test 3).
- **Predecessor wiring** is new; missed wiring silently breaks SSA (test 2).
- **New pure virtuals on `TracingInterface`**: only two tracers exist.

## 9. Files Touched

- `nautilus/include/nautilus/tracing/TracingInterface.hpp`
- `nautilus/src/nautilus/tracing/TraceContextBase.{hpp,cpp}` (new)
- `nautilus/src/nautilus/tracing/ExceptionBasedTraceContext.{hpp,cpp}`
- `nautilus/src/nautilus/tracing/LazyTraceContext.{hpp,cpp}`
- `nautilus/src/nautilus/tracing/RegionTraceContext.{hpp,cpp}` (new)
- `nautilus/src/nautilus/tracing/ExecutionTrace.{hpp,cpp}`
- `nautilus/include/nautilus/region.hpp` (new)
- `nautilus/test/execution-tests/RegionTest.cpp` (new)
- `nautilus/test/data/region-tests/` (new, optional goldens)
