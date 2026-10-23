# Component: Tracing Frontend

Part of the [Nautilus 0.4 design](README.md) · Milestone M2 · Builds on #300
(explicit control flow) and #561 (tracing statistics)

## 1. Purpose

The frontend turns a C++ function written with `val<T>` into a trace: NIR
at the trace level ([02-nir.md](02-nir.md)). Implicit tracing stays the
default and keeps its semantics. 0.4 makes it cheaper and adds explicit
control-flow constructs that trace in a single pass and can be mixed with
implicit control flow.

## 2. Today and what it costs

How tracing works in 0.3:

- `TraceContext::runScope` re-executes a scope body **once per explored
  path**. Already-recorded branch decisions are replayed in follow mode: the
  tracer advances a cursor without taking snapshots.
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
| **#561's statistics** (`tracing.iterations`, `tracing.replayedOperations`, `tracing.copies`, `tracing.firstRun.ms`/`rerun.ms`), plus a diagnostic that flags scopes whose replay exceeds a threshold and suggests `region()` or the explicit API | Makes replay visible to users |

The frame-pointer walk itself stays the main per-operation cost of record
mode. That cost belongs to implicit control flow. The explicit API (§4) is
how users avoid it.

## 4. Explicit control flow, mixable with implicit

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
  SSA construction. Explicit constructs take no snapshots and do no tag-map
  lookups.
- **Results flow out through captured `val`s,** as with `region()`; a value
  created inside a body must not outlive it. The value form of `If` is sugar
  over a captured variable.
- **Early exits:** `Break()` and `Continue()` inside `While`/`For`, and
  `Return(v)` to leave the function. Each is a terminator emitted directly.
- **Direct mode** (tracing disabled) runs the lambdas natively, like
  `select.hpp`.

A function written entirely with explicit constructs traces in one pass,
with tags needed only for source locations, which are recorded lazily.

Port of #300: keep its public header (`nautilus/control_flow.hpp`), its
tests (`ExplicitControlFlowTest.cpp`) and its single-pass guarantees.
Replace its tag-free block emission with region-scope emission, and turn
its mixing guard into mixing tests.

## 5. The flat trace (NIR, trace level)

The trace is NIR with `level = Trace` ([02-nir.md](02-nir.md)). It shares
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
- **Trace-level images can be serialized** for debugging and reproducing
  tracing or conversion bugs (fuzzer, playground). They are never executed.

## 6. Exit criteria (M2)

- `distinctSites1024` tracing at least 30% cheaper than 0.3.
- Explicit `chainedIf` at every corpus size: one trace pass, zero replayed
  operations.
- Mixed implicit and explicit functions are correct under the differential
  fuzzer.
- No `malloc` per recorded operation in steady state.
- `tracing/` reference dumps regenerated once, in an isolated commit.

## 7. Risks

- **Region-based constructs inherit `region()`'s limits:** values escaping
  a body, and memo keys that differ when paths keep different values alive.
  Same diagnostics; dedicated fuzzer coverage.
- **Copies remain about 40% of operations.** A lazier copy model is out of
  scope for 0.4, because copy sites take part in merge detection. It needs
  its own investigation.
