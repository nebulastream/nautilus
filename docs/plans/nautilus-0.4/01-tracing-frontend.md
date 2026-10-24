# Component: Tracing Frontend

Part of the [Nautilus 0.4 design](README.md) · Milestone M2 · Builds on #300
(explicit control flow) and #561 (tracing statistics)

## 1. Purpose

The frontend turns a C++ function written with `val<T>` into a trace: NIR
at the pre-SSA level ([02-nir.md](02-nir.md)). Implicit tracing stays the
default and keeps its semantics. 0.4 makes it cheaper and adds explicit
control-flow constructs that trace in a single pass and can be mixed with
implicit control flow.

## 2. Today and what it costs

How tracing works in 0.3:

- `TraceContext::runScope` re-executes a scope body **once per explored
  path**. Already-recorded branch decisions are replayed in follow mode: the
  tracer advances a cursor without taking snapshots, except at `region()`
  call sites (§4.1).
- In record mode, every operation takes a **tag**: a frame-pointer walk
  appended into a trie of return addresses (`tag/Trie.hpp`). It combines the
  tag with the static-value hash into a `Snapshot` and looks that up in
  `globalTagMap` (`std::unordered_map<Snapshot, (block, index)>`). A hit is a
  control-flow merge.
- `processControlFlowMerge` splits the reference block, moves the tail into
  a new merge block and **rewrites the map entry of every moved operation**.
- **Record sizes:**
  - `TraceOperation` 56 B, plus 24 B per input (`InputVariant`, a variant
    of 8 alternatives);
  - `Snapshot` 16 B, inline in each record;
  - `BlockRef` 32 B plus a heap vector;
  - `FunctionCall` carries two `std::string`s;
  - blocks hold three heap `std::vector`s.

P0 measurements ([p0-baseline.md](p0-baseline.md)):

| Cost | Evidence |
|---|---|
| Replay is quadratic in independent branches | 300 `if`s: 301 iterations, **270,300 replayed ops**, 69–78% of the compile; with `region()`: 900 replayed ops |
| Tag creation scans trie children linearly | 1024 distinct call sites: `createReferenceTagBuildin` is **32% of all instructions**, ~3x tracing cost per op |
| The replay cursor is expensive | `getCurrentBlock` 12.6%, `follow` 9.2%, `getCurrentOperation` 7.6% self time on replay-heavy code |
| Value-ref churn | `allocateValRef` + `freeValRef` 11% on replay-heavy code |
| Allocation | 3–5 heap allocations per IR op on linear code, 18–25 on branchy code |
| Copies | 39% of recorded operations on TPC-H (#561) |

## 3. Implicit tracing: same semantics, cheaper

Unchanged: replay, tags, snapshots, merge detection at any operation,
`static_val`, `region()` and their diagnostics.

| Change | What it fixes |
|---|---|
| **Hashed trie children** above about 8 children: a small open-addressing table per node | O(children) tag creation for functions with many call sites |
| **Tag map in the trace arena,** open addressing, mapping snapshots to **global op ids** instead of `(block, index)` | One allocation per op; merge splits no longer rewrite map entries |
| **Index-based replay cursor** over the block's op-id list; the reconciliation skip in `follow()` compares `snapshot[id]` without re-fetching the block per step | ~30% self time in cursor code on replay-heavy functions |
| **Value-ref free list** in the trace arena | `allocateValRef`/`freeValRef` churn |
| **Arena-backed block lists;** call names resolved lazily from the callee identity (already the default via `engine.resolveFunctionNames`) | Heap allocations per op |
| **#561's statistics** (`tracing.iterations`, `tracing.replayedOperations`, `tracing.copies`, `tracing.firstRun.ms`/`rerun.ms`), plus a diagnostic that flags scopes whose replay exceeds a threshold and suggests `region()` or the explicit API; extended by R1 and C1 (§4) | Makes replay visible to users |

**Constant data.** `nautilus::constData(std::span<const T>)` (and a string
overload) copies a host array into a read-only data object of the module at
trace time and returns a `val<const T*>` to it
([13 §4.10](13-nir-spec.md)). It replaces tracing a pointer to host memory
for lookup tables, strings and seeds: the module no longer depends on that
host memory, loads from the table can fold, and the module stays
persistable.

The frame-pointer walk itself stays the main per-operation cost of record
mode. That cost belongs to implicit control flow. The explicit API (§5) is
how users avoid it.

## 4. Reducing re-run and copy cost

#561 measured TPC-H (nautilus-duck, 22 queries, 300 kernels):
- **61% of tracing is re-running traced bodies**: 4,687 re-runs replaying
  631,000 recorded operations, against 34% for the first runs;
- **39% of recorded operations are `val<T>` copies.**

§3 makes each operation cheaper. This section targets those two buckets
directly.

### 4.1 Anatomy of a re-run

A re-run (`runScope`, one per unexplored branch direction) goes through
three phases:

1. **Follow.** The body re-executes from the scope entry up to the branch
   to flip. Each traced operation advances the replay cursor (`follow()`).
   The C++ work of each `val` operation still runs: the runtime value, and
   `TypedValueRefHolder` construction and destruction through
   `allocateValRef`/`freeValRef`.
2. **Record.** The new branch direction is recorded until the pass reaches
   an operation whose snapshot is already in the tag map (a merge), or the
   scope's end.
3. **Paused tail.** After a merge the pass is finished, but **the body keeps
   running to its natural end**. Every `val` operation checks `paused_` and
   returns a dummy ref, and `traceBool` returns `false`.

   For chained branches the paused tail is about as long as the replayed
   prefix: flipping the k-th of N branches replays k branches and then
   runs the remaining N−k paused. That is roughly half of all re-run work.
   This estimate comes from the code structure; R1 in §4.2 adds the counter that
   measures it.

Two further costs:

- **Regions in follow mode take a full snapshot.** `traceRegion` calls
  `recordSnapshot()`, a frame walk, before it checks follow mode, only to
  look up the region memo.
- **Re-runs allocate.** On branchy corpus code there are 18–25 heap
  allocations per IR operation, against 3–5 on linear code.

### 4.2 Levers for re-runs, cheapest first

| # | Change | Effect | Risk |
|---|---|---|---|
| R1 | **Counters:** split `tracing.rerun.ms` into follow, record and paused time; add `tracing.pausedOperations` and per-scope re-run counts | Shows which lever pays on real workloads | none |
| R2 | **Cheaper follow:** index-based cursor and value-ref free list (§3); a region in follow mode looks up its memo by the recorded region-entry op id instead of a fresh snapshot | Removes the cursor overhead (~30% of a replay-heavy compile) and one frame walk per region per re-run | low |
| R3 | **Minimal paused path:** when `paused_` is set, `val` operations skip ref allocation and holder bookkeeping entirely (one flag check, then the runtime value only) | Paused tail costs close to native C++ execution | low |
| R4 | **End paused passes early:** when a pass becomes paused, unwind the body with a tracer-private exception caught in `runScope`, so a re-run costs one throw instead of the whole tail. User code that swallows it (`catch (...)`) is detected because the body returns normally, and that scope falls back to R3 | Removes the paused tail, about half of re-run work on chained branches | Medium: a `noexcept` frame between body and tracer would terminate. Ships behind `tracing.unwindPausedPasses`; the default is decided on the fuzzer, NebulaStream and TPC-H data before release |
| R5 | **Fewer re-runs:** the replay diagnostic names scopes with many re-runs and suggests `region()` (replay bounded to the region) or explicit control flow (§5); `nautilus-review` checks the same | Removes re-runs where users act on it | none |
| R6 | **Explicit control flow** (§5) | Zero re-runs for code written with it | API adoption |

### 4.3 Copies

Where copies come from:

- **Copy construction from an lvalue:** `val<T> r = x;`, by-value
  parameters, by-value captures, containers of `val`. Moves already record
  nothing (`val(val&&)` moves the state).
- **Assignment:** `x = expr` records `ASSIGN` (a variable update). These are
  needed: they are what makes `x` a variable for SSA construction.
- **Explicit fresh values:** `ptr + 0` (`ptrAddStatic`) copies so that
  assigning to the result does not reassign `ptr`.

Every copy in record mode costs a snapshot (frame walk), a tag-map lookup,
a record, and the reconciliation logic of `traceCopy`/`traceAssignment`
(issues #95, #382, #384). In follow mode it costs a cursor step. In the
conversion, copies are free: Braun-style SSA construction resolves them.

| # | Change | Effect | Risk |
|---|---|---|---|
| C1 | **Counters** per copy kind (construction, assignment, explicit) | Shows which kind dominates on TPC-H and NebulaStream | none |
| C2 | **Avoid unnecessary copies in library code:** take `const val<T>&` in the `std` plugin and in hot helpers; the `nautilus-review` skill flags by-value `val` parameters | Fewer copies with no semantic change | none |
| C3 | **Cheaper copy records:** the 16 B flat record (§6) plus the open-addressing tag map (§3) | Per-copy cost drops with every other operation | none |
| C4 | **Lazy copies (investigation, not committed for 0.4):** record a copy as an alias of its source and materialize the `ASSIGN` only when either side is later assigned | Could remove most copy records | High: copy sites take part in merge detection and in the alive-variable snapshot, so aliasing must not change which paths merge. Prototype behind a flag; adopt only if the fuzzer and every `tracing/` reference trace agree |

### 4.4 Targets

To confirm on TPC-H and NebulaStream with the R1/C1 counters:

- the re-run share of tracing time falls from 61% to at most 35% on
  unchanged (implicit) code;
- paused operations are close to zero with R4 enabled;
- recorded copies fall by at least a quarter through C2 and the frontend
  changes.

## 5. Explicit control flow, mixable with implicit

#300 adds `If`, `While` and `For`. They emit their blocks directly in a
single trace pass (230x faster on `chainedIf100`). #300 forbids mixing them
with native `if` in the same function, because replay would re-enter and
double-emit the explicit constructs. 0.4 removes that restriction by
building the constructs on **region scopes**.

```cpp
If(x > 0, [&] { r = r + 1; }, [&] { r = r - 1; });
auto v = If(c, [&] { return a; }, [&] { return b; });   // value form
While([&] { return i < n; }, [&] { sum = sum + data[i]; i = i + 1; });
For(val<int64_t>(0), n, [&](val<int64_t> i) {
	If(data[i] < 0, [&] { Break(); });
	sum = sum + data[i];
});
```

Semantics:

- **Each arm, loop header and loop body is a region scope** with its own
  exploration loop ([docs/region.md](../../region.md)). An implicit `if`
  *inside* an arm replays only that arm.
- **The whole construct is memoized like a region,** keyed on the
  enclosing state. When an implicit branch *outside* forces the enclosing
  function to replay, the construct is skipped and resumes at its exit
  block, as a memoized region does today. It is never emitted twice.
- **Blocks are emitted directly.** A loop gets a header, a body and a back
  edge. Loop-carried variables become block arguments through the existing
  SSA construction.
- **Recording is optimistic.** Inside an arm or body, operations are
  recorded without snapshots or tag-map lookups: with no implicit branch,
  nothing can merge. If an implicit branch is reached inside an arm, that
  arm's scope restarts its exploration with snapshots enabled. The restart
  is bounded to the arm, as a region replay is. The construct itself takes
  one snapshot at its entry, as the memo key.
- **Results flow out through captured `val`s,** as with `region()`; a value
  created inside a body must not outlive it. The value form of `If` is sugar
  over a captured variable.
- **Early exits:** `Break()` and `Continue()` inside `While`/`For`, and
  `Return(v)` to leave the function. Each is a terminator emitted directly.
- **Direct mode** (tracing disabled) runs the lambdas natively, like
  `select.hpp`.

A function written entirely with explicit constructs traces in one pass,
with one snapshot per construct and none per operation. Source-location
tags are recorded only when requested (debug, perf).

Port of #300: keep its public header (`nautilus/control_flow.hpp`), its
tests (`ExplicitControlFlowTest.cpp`) and its single-pass guarantees.
Replace its tag-free block emission with region-scope emission, and turn
its mixing guard into mixing tests.

## 6. The flat trace (NIR, pre-SSA level)

The trace is NIR with `level = PreSSA` ([02-nir.md](02-nir.md)). It shares
the instruction format, `extra`, `ConstPool`, the pointer table and the
extension registry with the SSA level.

```cpp
struct FlatTrace {                     // per function
	std::vector<Inst>       ops;        // trace op id == index; recording order
	std::vector<uint32_t>   extra;      // call args, block-ref args
	ConstPool               consts;
	std::vector<Snapshot>   snapshot;   // by op id; record mode only; never serialized
	std::vector<uint16_t>   region;     // by op id
	std::vector<TraceBlock> blocks;     // per block: arena list of op ids, predecessors
	FlatMap<Snapshot, uint32_t> tagMap; // trace arena; snapshot -> op id
};
```

- **Records are 16 B instead of 56 + 24·n B.** Operands are 4 B value refs.
  Constants, call descriptors, block refs, branch probabilities and alloca
  indices go into `consts`/`extra`/side tables.
- **Value refs stay what they are.** `TypedValueRef`, a `u32` plus the
  type, is what `val<T>` holds. Recording writes one record and the result's
  ref.
- **Merge splits move only the tail of one `u32` list.** Op ids are global
  and stable, so `tagMap` entries never change.
- **Plugins record their ops through one entry point:**
  `traceExtOp(op, type, inputs)` ([04-extensions.md](04-extensions.md)).
- **Pre-SSA images can be serialized** for debugging and reproducing
  tracing or conversion bugs (fuzzer, playground). They are never executed.
- **Implemented as a second tracer.** `val<T>` reaches the tracer only
  through the abstract `tracing::TracingInterface`, set per trace. The NIR
  tracer is a new implementation of that interface (`tracing/nir/`), built
  next to 0.3's `TraceContext` and tested on its own until the cut-over
  replaces the old one ([16 §2](16-implementation-plan.md)). It maps
  today's `Type` and `Op` values to NIR widths and flags at the interface,
  so the public headers do not change. The tag trie and symbolic execution
  are shared and kept.

## 7. Exit criteria (M2)

- `distinctSites1024` tracing at least 30% cheaper than 0.3.
- The re-run and copy targets of §4.4 met on TPC-H (nautilus-duck).
- Explicit `chainedIf` at every corpus size: one trace pass, zero replayed
  operations.
- Mixed implicit and explicit functions are correct under the differential
  fuzzer.
- No `malloc` per recorded operation in steady state.
- `tracing/` reference dumps regenerated once, in an isolated commit.

## 8. Risks

- **Region-based constructs inherit `region()`'s limits:** values escaping
  a body, and memo keys that differ when paths keep different values alive.
  Same diagnostics; dedicated fuzzer coverage.
- **Optimistic recording must restart correctly.** An arm that meets an
  implicit branch discards its optimistic records and re-explores. The
  fuzzer covers arms with and without implicit branches.
- **Unwinding paused passes (R4)** interacts with user code that catches
  everything or is `noexcept`. It stays opt-in until the data supports a
  default, with R3 as the safe fallback.
- **Lazy copies (C4)** may change which paths merge. They are an
  investigation and are adopted only if every reference trace agrees.
