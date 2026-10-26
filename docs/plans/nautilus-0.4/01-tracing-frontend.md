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
| **Cheaper alive-variable bookkeeping:** value refs are op ids (§6), so nothing is allocated or freed; `allocateValRef`/`freeValRef` become an inlined counter update on the tracer's alive-variable table instead of a virtual call per holder | `allocateValRef`/`freeValRef` churn |
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
| R2 | **Cheaper follow:** index-based cursor and cheaper alive-variable bookkeeping (§3); a region in follow mode looks up its memo by the recorded region-entry op id instead of a fresh snapshot | Removes the cursor overhead (~30% of a replay-heavy compile) and one frame walk per region per re-run | low |
| R3 | **Minimal paused path:** when `paused_` is set, `val` operations skip ref allocation and holder bookkeeping entirely (one flag check, then the runtime value only) | Paused tail costs close to native C++ execution | low |
| R4 | **End paused passes early:** when a pass becomes paused, unwind the body with a tracer-private exception caught in `runScope`, so a re-run costs one throw instead of the whole tail. User code that swallows it (`catch (...)`) is detected because the body returns normally, and that scope falls back to R3 | Removes the paused tail, about half of re-run work on chained branches | Medium: a `noexcept` frame between body and tracer would terminate. Ships behind `tracing.unwindPausedPasses`; the default is decided on the fuzzer, NebulaStream and TPC-H data before release |
| R5 | **Fewer re-runs:** the replay diagnostic names scopes with many re-runs and suggests `region()` (replay bounded to the region) or explicit control flow (§5); `nautilus-review` checks the same | Removes re-runs where users act on it | none |
| R6 | **Explicit control flow** (§5) | Zero re-runs for code written with it | API adoption |

### 4.3 Copies

Where copies come from:

- **Copy construction from an lvalue:** `val<T> r = x;`, by-value
  parameters, by-value captures, containers of `val`. Moves already record
  nothing (`val(val&&)` moves the state). In NIR a copy is a `var x` record
  (§6): it defines a new variable, so a later `r = y` does not reassign `x`.
- **Assignment:** `x = expr` records `ASSIGN` (a variable update; `assign`
  in NIR). These are needed: they are what makes `x` a variable for SSA construction.
- **Explicit fresh values:** `ptr + 0` (`ptrAddStatic`) copies so that
  assigning to the result does not reassign `ptr`.

Every copy in record mode costs a snapshot (frame walk), a tag-map lookup,
a record, and the reconciliation logic of `traceCopy`/`traceAssignment`
(issues #95, #382, #384). In follow mode it costs a cursor step. In the
conversion, copies are free: Braun-style SSA construction resolves them.

| # | Change | Effect | Risk |
|---|---|---|---|
| C1 | **Counters** per copy kind (construction, assignment, explicit); `var` records with a value operand count as construction copies, `var` of a constant is counted separately as constant initialization | Shows which kind dominates on TPC-H and NebulaStream | none |
| C2 | **Avoid unnecessary copies in library code:** take `const val<T>&` in the `std` plugin and in hot helpers; the `nautilus-review` skill flags by-value `val` parameters | Fewer copies with no semantic change | none |
| C3 | **Cheaper copy records:** the 16 B flat record (§6) plus the open-addressing tag map (§3) | Per-copy cost drops with every other operation | none |
| C4 | **Lazy copies (investigation, not committed for 0.4):** record a copy as an alias of its source and materialize the copy (`var` in NIR) only when either side is later assigned | Could remove most copy records | High: copy sites take part in merge detection and in the alive-variable snapshot, so aliasing must not change which paths merge. Prototype behind a flag; adopt only if the fuzzer and every `tracing/` reference trace agree |

### 4.4 Targets

To confirm on TPC-H and NebulaStream with the R1/C1 counters:

- the re-run share of tracing time falls from 61% to at most 35% on
  unchanged (implicit) code, with the shipped default of
  `tracing.unwindPausedPasses` (whatever M2.2 decides for R4);
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
  is bounded to the arm, as a region replay is, and rolls back what the arm
  recorded (§5.2). The construct itself takes one snapshot at its entry, as
  the memo key.
- **Results flow out through captured `val`s,** as with `region()`; a value
  created inside a body must not outlive it. The value form of `If` is sugar
  over a captured variable.
- **Early exits:** `Break()` and `Continue()` inside `While`/`For`, and
  `Return(v)` to leave the function. Each is a terminator emitted directly
  (§5.1).
- **Direct mode** (tracing disabled) runs the lambdas natively, like
  `select.hpp`.

A function written entirely with explicit constructs traces in one pass,
with one snapshot per construct and per operation outside constructs (at
function scope, between constructs), and none per operation inside arms and
bodies (§5.2). Source-location tags are recorded only when requested
(debug, perf).

Port of #300: keep its public header (`nautilus/control_flow.hpp`), its
tests (`ExplicitControlFlowTest.cpp`) and its single-pass guarantees.
Replace its tag-free block emission with region-scope emission, and turn
its mixing guard into mixing tests.

### 5.1 Non-local exits

`Break()`, `Continue()` and `Return(v)` are plain function calls: C++
`break` cannot leave a lambda, so the code after them in the same lambda
keeps running. 0.3's region scopes forbid what they need (a region body
cannot return, `traceReturnOperation`; a region whose exit has no
predecessor is an error, `traceScopeExit`). Construct scopes therefore
differ from `region()` scopes in four rules:

- **The rest of the pass runs paused.** `Break`/`Continue`/`Return` emit
  their terminator (`br` to the loop's exit block; `br` to the loop's
  continue target, which is the header for `While` and the increment
  block for `For`; `ret v`) and pause the scope that executes them for the
  rest of its current pass. The rest of that lambda runs on the minimal
  paused path (R3); nothing after the exit is recorded, and no dead block
  is created for it. Enclosing scopes are *not* paused by the exit
  itself: other passes of the inner scope may still reach its end (an
  implicit `if` with `Break()` in one direction only, or an
  `If(c, [&] { Break(); })` whose implicit else falls through), so an
  enclosing scope is paused only by the zero-predecessor rule below, when
  a construct ends with no live exit. The exit's target block is created
  when the target construct is entered, as a region's exit block is.
- **Exits do not cross `region()`.** A `Break`, `Continue` or `Return`
  whose target lies outside an enclosing `region()` body is rejected with
  a diagnostic: a region keeps its single exit and its "region body
  cannot return" rule (`traceReturnOperation`).
- **Owned values are destroyed on the exit path.** Before the terminator,
  the tracer emits, in reverse registration order, the destructor calls of
  the registered owned values (`registerDestructor`) on the current
  scope's destructor list above a depth: for `Break` and `Continue`, the
  list length at the target loop body's entry; for `Return`, depth 0, so
  every owned value of the function, including those of the function
  scope, as native `return` destroys them. The list itself is not
  changed: each scope's list is reseeded from its parent at every pass
  (`resume()`), the C++ destructors that run later while paused record
  nothing and remove their own entries as on any path, and the list a
  scope hands back to its parent at its end
  (`activeDestructors = std::move(child.activeDestructors)`) therefore
  still holds the enclosing scopes' live values even when the last pass
  ended in `Return`. Native `break` in 0.3 gets the destructor calls from
  C++ scoping; explicit exits get them from the tracer.
- **A construct scope's exit may have no predecessor.** When every path
  through an arm or body leaves non-locally (an arm that always `Break`s),
  this is not an error for explicit constructs. If the whole construct has
  no live exit, the code after it is unreachable, and the enclosing scope
  is paused up to its own end. `region()` keeps its diagnostic.
- **Escape-check exemptions.** `Return(v)` is allowed in any construct
  scope, and its value is exempt from the "created inside the body" check
  (the pass is paused, so `traceScopeExit` does not run its check). The
  `While` header's `val<bool>` is consumed by the header scope's own
  `condbr` and destroyed by the construct before the header scope's exit
  check. Any other value that outlives its scope is rejected as in
  `region()`.

### 5.2 Optimistic recording: checkpoint and restart

An arm or body records optimistically. When it meets an implicit branch it
restarts with snapshots enabled, and everything it recorded must disappear.

- **Checkpoint.** At entry, an optimistic scope saves the sizes of `ops`
  (which is also the next value ref, §6), `extra`, `blocks` (and of the
  per-op tables `snapshot`, `region`, `blockOf`), the alloca table, the
  data-object table, the region records, the callee queue
  (`functionsToTrace`), the destructor stack and the explicit-construct
  stack. Two appends do not fit a size: predecessor entries added to
  blocks older than the checkpoint (a nested `Break` adds one to the loop's
  exit block, a nested `Continue` to the header) and tag-map insertions
  (a nested construct's entry snapshot, the records of a nested `region()`
  body, which is not optimistic). Both go into a per-scope undo log while
  a checkpoint is open.
- **Restart.** The pass that meets the implicit branch ends there: it is
  paused for the rest of the lambda (R3, or unwound with R4) and does not
  count as an explored path. The scope then truncates every table to the
  checkpoint, replays the undo log backwards, drops region memo entries
  created after the checkpoint, resets its symbolic-execution state, and
  starts its first pass with snapshots enabled. `ConstPool`, `pointers` and
  callee table entries may stay: canonicalization drops unused ones
  ([13 §7](13-nir-spec.md)). As for a `region()` replay, C++ state the
  lambda changed is not rolled back.
- **Scope.** Only arms, loop headers and loop bodies are optimistic. The
  function scope records normally, with a snapshot per operation, because
  restarting a whole function at its first implicit branch would replay its
  prefix. An all-explicit function therefore takes one snapshot per
  construct plus one per operation outside constructs; `tracing.snapshots`
  ([10 §3](10-observability.md)) checks it.

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
	std::vector<uint32_t>   region;     // by op id; RegionRecord index
	std::vector<uint32_t>   blockOf;    // by op id; updated for the moved tail on a split
	std::vector<TraceBlock> blocks;     // per block: arena list of op ids, predecessors
	FlatMap<Snapshot, uint32_t> tagMap; // trace arena; snapshot -> op id
};
```

- **Records are 16 B instead of 56 + 24·n B.** Operands are 4 B value refs.
  Constants, call descriptors, block refs, branch probabilities and alloca
  indices go into `consts`/`extra`/side tables.
- **Value refs stay what they are.** `TypedValueRef`, a `u32` plus the
  type, is what `val<T>` holds. Recording writes one record and the result's
  ref, which is the record's op id (0.3's refs are already a plain
  counter, `getNextValueRef`). At the pre-SSA level that ref is always an
  instruction id, never a `ConstPool` ref, because any `val` may later be
  assigned:
  - `traceConstant` of an integer, float or `bool` records `var c` with
    the constant as its operand; of a pointer, `ptr.const` (below);
  - `traceCopy` records `var x`;
  - `traceAssignment` records `assign`, whose target is always such an id
    (or a parameter, which is a `blockarg`).

  `var` ([13 §5.9](13-nir-spec.md)) is a pre-SSA-only opcode that defines a
  new variable initialized to its operand. It is a 16 B record like any
  other and takes a snapshot like 0.3's `CONST` and copy `ASSIGN`. Like
  them it **never merges** on a tag hit: a `var` whose snapshot is already
  in the tag map comes from a different path through the same call site
  (the arms of `c ? a : b`, #95), so the tracer records
  `assign <original var>, <operand>` and returns the original id. That is
  one record where 0.3 records a pair (`CONST`/copy plus a reconciling
  `ASSIGN`), so the reconciliation skip in `follow()` (#384) is no longer
  needed for `var` sites; `follow()` accepts either record at such a site.
  A `ptr.const` at a known tag keeps 0.3's pair (`ptr.const`, then
  `assign` onto the original id). For example:

  ```text
  val<int64_t> i = 0;          %1 = var 0
  val<int64_t> r = i;          %2 = var %1
  while (i < n)                %3 = cmp.lt.s %1, %0   (block split here on the second iteration's merge)
    i = i + 1;                 %4 = var 1;  %5 = add %1, %4;  assign %1, %5
  ```

  The conversion forwards each `var` to its operand's current value and
  emits nothing for it ([03 §3](03-conversion-and-passes.md) step 1;
  `var` and `assign` also take extension-type values, so a `val` of an
  extension type is a variable like any other):
  `%2` becomes the constant 0, `%4` the constant 1, and only `%1` gets a
  header phi, because only `%1` is assigned in the loop
  ([03 §3.2](03-conversion-and-passes.md)); `r` keeps 0 after `i` changes.
  Constant operands that no `val` object holds (for example a
  `static_val` folded into an operation by the tracer itself) may reference
  `ConstPool` directly.
- **Merge splits move the tail of one `u32` list and update `blockOf` for
  it;** `tagMap` is never rewritten. Op ids are global and stable. The
  split still walks the moved tail (O(tail), as today); the saving is the
  hash-map write per moved op.
- **Plugins record their ops through one entry point:**
  `traceExtOp(op, type, inputs)` ([04-extensions.md](04-extensions.md)).
- **Integer `div`/`rem` always record a cleanup list** (the current
  destructor stack, [13 §3.4](13-nir-spec.md)), like a may-throw call:
  whether the divisor is safe is known only after folding, and a checked
  division that fails throws `nautilus::ArithmeticException` (D10,
  [06 §4](06-semantics.md)). The conversion drops the list when it proves
  the divisor safe.
- **Calls of registered intrinsics.** A traced `invoke` whose function
  address is registered as an intrinsic (`name@version`,
  [13 §4.6.1](13-nir-spec.md)) becomes an intrinsic callee at trace time,
  by an address → name lookup in the registry. Every other `invoke` is an
  external callee.
- **Traced constants and pointers.** A traced pointer is never a constant
  (13 has no `ptr` constants, [13 §4.2](13-nir-spec.md)): a traced host
  address becomes a `pointers` entry of kind 3 (*traced address*) with a
  provenance class, read by a `ptr.const #pN` instruction
  ([13 §4.7, §5.8](13-nir-spec.md), [07](07-serialization.md)); the null
  pointer is `ptr.const null` (opcode 38). The tracer applies the
  engine's `SymbolNamer` (`NautilusEngine::setSymbolNamer`, [07 §3](07-serialization.md),
  "Where naming happens") as it creates each `pointers` entry, callees,
  thunks and destructors included, so modes are final before the pre-SSA
  eligibility scan and every cache key. An entry that neither the namer
  names nor build ID + offset resolves ([07 §3](07-serialization.md)) stays process-local (mode 0,
  `ProcessLocal`), is `RawProcessValue` and makes the
  module process-local (`ModuleRecord.processToken`); an entry that
  resolves by name or build id (modes 1 and 2) is `Literal`, or
  `AuditedInvariant` when the user audited it (`cacheLiteral`/`cacheInvariant`, [07 §5](07-serialization.md); neither marker makes a mode-0 entry portable). `ptr.const` is an instruction, so a
  `val<T*>` holding it can be assigned like any other id. Integer, float
  and `bool` constants are `Literal` unless the user marks them.
- **Signature extension kinds.** For every parameter and result of an
  external, intrinsic or exported signature, the tracer records the ABI
  extension kind (none, `zext`, `sext`) from the `val<T>`'s C++ type:
  `bool` and the unsigned 8-, 16- and 32-bit types are `zext`, the signed
  ones `sext`, everything else none
  ([13 §4.5.1](13-nir-spec.md), [06 §4](06-semantics.md)).
- **Pre-SSA images can be serialized** for debugging and reproducing
  tracing or conversion bugs (fuzzer, playground). They are never executed.
- **Implemented as a second tracer.** `val<T>` reaches the tracer only
  through the abstract `tracing::TracingInterface`, set per trace. The NIR
  tracer is a new implementation of that interface (`tracing/nir/`), built
  next to 0.3's `TraceContext` and tested on its own until the cut-over
  replaces the old one ([16 §2](16-implementation-plan.md)). It maps
  today's `Type` and `Op` values to NIR widths, flags and signature
  extension kinds at the interface, so the user-facing headers of
  [README §5](README.md) do not change. `TracingInterface`
  (`include/nautilus/tracing/TracingInterface.hpp`), which is a public
  header but not user-facing, gains `traceExtOp`, `traceData`,
  `traceBinding` and the explicit-construct hooks; they are added in PR 3
  with no-op defaults in 0.3's `TraceContext`. The tag trie and symbolic
  execution are shared and kept.

## 7. Exit criteria (M2)

- `distinctSites1024` tracing at least 30% cheaper than 0.3.
- The re-run and copy targets of §4.4 met on TPC-H (nautilus-duck).
- Explicit `chainedIf` at every corpus size: one trace pass, zero replayed
  operations.
- Mixed implicit and explicit functions are correct under the differential
  fuzzer.
- No `malloc` per recorded operation in steady state.
- `tracing/` reference dumps regenerated in isolated commits, once per
  work package that changes traces (the cut-over, then C2, R2–R4 and
  explicit control flow), never together with the behavior change.
- Snapshots counted (`tracing.snapshots`): an all-explicit corpus function
  takes exactly one per construct plus one per operation outside
  constructs (§5.2).

## 8. Risks

- **Region-based constructs inherit `region()`'s limits:** values escaping
  a body, and memo keys that differ when paths keep different values alive.
  Same diagnostics; dedicated fuzzer coverage.
- **Optimistic recording must restart correctly.** An arm that meets an
  implicit branch rolls back to its checkpoint and undo log (§5.2) and
  re-explores. The
  fuzzer covers arms with and without implicit branches, and a mode that
  forces a restart in every arm.
- **Non-local exits** (§5.1) skip C++ code that users may expect to run.
  The tracer pauses it rather than recording it, and the destructor calls
  of owned values are emitted explicitly; both are covered by dedicated
  tests.
- **Unwinding paused passes (R4)** interacts with user code that catches
  everything or is `noexcept`. It stays opt-in until the data supports a
  default, with R3 as the safe fallback.
- **Lazy copies (C4)** may change which paths merge. They are an
  investigation and are adopted only if every reference trace agrees.
