# Regions v2 — "a region is a trace scope with a parent"

> **Status: implemented.** Stages 0-5 are done and green (the full restored region suite,
> both trace modes, all backends, plus two new white-box tests). What changed against the
> plan below, and what was learned doing it, is recorded in [§9](#9-implementation-notes).

Implementation plan for a second, simpler `nautilus::region()` implementation on branch
`nautilus_regions`. Written to be executed by an implementation session (Sonnet); each stage
is small, independently verifiable, and has its own tests.

Reference for intended semantics: `docs/region.md` (restore from `af5fcb20`).
Reference for what *not* to do: the v1 implementation in `af5fcb20`'s
`LazyTraceContext.{hpp,cpp}` (`RegionFrame`, `traceRegionBegin/Continue/End`).

---

## 1. The idea

v1 made every `trace*` method region-aware: each operation had to decide *which* scope's
`paused` flag, `SymbolicExecutionContext`, `staticVars`, `aliveVars` and `TagRecorder` were
"current" (`currentPaused()`, `exploringFrame()`, `currentEnv()`, …). That cross-cutting
conditional is where the fragility lived — every new operation had to remember to be
region-aware, and three separate bugs came from one scope's state being consulted while
another scope's tag space was in effect.

v2 removes the conditional entirely by reusing the mechanism Nautilus already has for
scoping tracing state: **the trace context object itself**.

Compare what `LazyTraceContext::startTrace` does per function with what a region needs:

```cpp
// per function, today
TagRecorder tr(startTraceReturnAddress, arena);
SymbolicExecutionContext sec;
state.emplace(tr, executionTrace, sec, options);
while (sec.shouldContinue()) { sec.next(); trace.resetExecution(); resume(); body(); }

// per region, wanted
TagRecorder tr(traceRegionReturnAddress, arena);
SymbolicExecutionContext sec;
state.emplace(tr, parentTrace, sec, options);
while (sec.shouldContinue()) { sec.next(); trace.setCurrentBlock(entry); resume(); body(); exitMarker(); }
```

These are the same loop. `ExecutionTrace::resetExecution()` is literally
`currentBlockIndex = 0; currentOperationIndex = 0;` — i.e. `setCurrentBlock(entryBlock)`
with `entryBlock == 0`. So:

> **Tracing a function is tracing a scope with no parent, whose entry block is block 0 and
> whose per-pass terminator is `RETURN`. A region is a scope *with* a parent, whose entry
> block is freshly created inside the parent's trace and whose per-pass terminator is a
> jump to the region's exit block.**

Everything the feature promises falls out of that:

* Every `trace*` method stays exactly as it is today. No region-awareness anywhere.
* Tag isolation: the scope's `TagRecorder` is rooted at its own entry frame, so tags are
  the call path *from the scope entry* and can never collide with the parent's.
* Liveness/static isolation: the scope has its own `staticVars`/`aliveVars`, so the
  snapshot hash inside it is a delta over the scope only.
* Bounded exploration: the scope's `SymbolicExecutionContext` re-runs *its own body*,
  never the enclosing function.
* Memoized replay: a region's exploration loop runs **to completion inside one
  `traceRegion()` call**. Once it returns, the body is fully recorded, and every later
  parent pass skips it by jumping the trace cursor to the region's exit block.
* No region-specific operation in the trace, the IR or any backend: the region's blocks are
  ordinary blocks, entry and exit are ordinary `JMP`s.

Because the parent scope is *suspended* for the whole duration of `traceRegion()`, the
region owns a contiguous block range in the shared trace while it runs. That single
property replaces most of v1's bookkeeping.

### Why share the parent's trace instead of splicing one in

The alternative (trace the region into a fresh `ExecutionTrace`, then splice) needs
remapping of block ids, `BlockRef` targets, `predecessors`, `allocaSpecs` indices and value
refs. Sharing the trace needs none of it — and the one thing it does not give for free
("rewind to the scope entry between passes") is the `setCurrentBlock(entryBlock)` that
already unifies with `resetExecution()`. Share the trace.

---

## 2. The scope primitive

One primitive, used by both function tracing and region tracing:

```cpp
class LazyTraceContext final : public TraceContextBase {
	// ... unchanged TracingInterface overrides ...
private:
	void runScope(std::function<void()>& body);

	TraceSession* session_ = nullptr;   // worklist + name caches, shared by all scopes
	LazyTraceContext* parent_ = nullptr; // null => function scope
	uint32_t entryBlock_ = 0;
	uint32_t exitBlock_ = 0;             // region scopes only
	std::optional<Snapshot> exitSnapshot_;  // §4.4
	std::unordered_map<Snapshot, RegionRecord> regionMemo_;   // regions opened *by* this scope
	std::deque<TagRecorder> regionTagRecorders_;              // §4.1
	// existing: state, staticVars, aliveVars, paused_
};

void LazyTraceContext::runScope(std::function<void()>& body) {
	auto& sec = state->symbolicExecutionContext;
	while (sec.shouldContinue()) {
		sec.next();
		state->executionTrace.setCurrentBlock(entryBlock_);
		resume();
		body();
		if (parent_ != nullptr) {
			traceScopeExit();          // §3, idea 1
		}
		assert(staticVars.empty() && "static variable stack not empty after tracing iteration");
	}
}
```

`startTrace` calls `runScope` per function with `parent_ == nullptr`, `entryBlock_ == 0`.
`traceRegion` calls `runScope` on a child scope with `parent_ == this` and a fresh entry
block. The remaining differences, in full:

| | function scope | region scope |
| --- | --- | --- |
| `ExecutionTrace` | fresh, one per function | the parent's |
| entry block | 0 (created by the trace ctor) | fresh block; parent `JMP`s into it |
| tag root address | `startTrace`'s return address | `traceRegion`'s return address |
| value refs | fresh counter | shared with parent (automatic) |
| per-pass terminator | the body's own `RETURN` | `traceScopeExit()` |
| after the loop | the trace is done | parent resumes in the exit block |
| worklist / name caches | `session_` | `session_` (identical) |
| `registerFunctionArgument` | yes | never — throw if called |
| memoization | n/a | `regionMemo_` on the *parent* scope |

Everything not in that table is shared code.

---

## 3. `traceRegion`

```
void LazyTraceContext::traceRegion(std::function<void()>& regionFunction) {
    if (paused_) return;                        // (a) parent in passive mode: skip body

    auto key = recordSnapshot();                // (b) region identity at this call site

    if (isFollowing()) {                        // (c) memoized replay
        auto it = regionMemo_.find(key);
        if (it == regionMemo_.end()) throw <diagnostic: region replay diverged>;
        state->executionTrace.setCurrentBlock(it->second.exitBlock);
        return;
    }

    if (!state->executionTrace.checkTag(key)) { // (d) repeated key while recording
        paused_ = true;                         //     -> control-flow merge, pause parent
        return;
    }

    // (e) open the region: entry block, exit block, tagged JMP into the entry
    auto& trace = state->executionTrace;
    auto entry = trace.createBlock();
    auto exit  = trace.createBlock();
    trace.addJumpOperation(key, entry);                  // tagged: see §4.2
    trace.getBlock(entry).predecessors.push_back(trace.getCurrentBlockIndex());
    trace.setCurrentBlock(entry);

    // (f) child scope, installed as the active tracer for the body
    auto& child = session_->acquireScope();             // pooled, §4.6
    child.initRegionScope(this, entry, exit,
                          regionTagRecorders_.emplace_back(__builtin_return_address(0),
                                                           trace.getArena()));
    ActiveTracerSwap swap(&child);                      // RAII, restores `this` on any exit

    child.runScope(regionFunction);                     // §2 — the shared loop

    // (g) hand control back to the parent
    child.transferEscapesTo(*this);                     // §4.4
    trace.setCurrentBlock(exit);
    regionMemo_[key] = { entry, exit };
}
```

Steps (b)–(d) are deliberately the same shape as every other traced operation
(`recordSnapshot` / `isFollowing` / `checkTag`) — a region behaves like an operation to its
parent and like a function to its body.

### `traceScopeExit` — the explicit end of every pass

```cpp
void LazyTraceContext::traceScopeExit() {
	if (paused_ || isFollowing()) {
		return;   // see below
	}
	auto snapshot = recordSnapshot();
	if (!exitSnapshot_) {
		exitSnapshot_ = snapshot;
	} else if (!(*exitSnapshot_ == snapshot)) {
		throw <diagnostic: region exit state depends on the path>;   // §4.4
	}
	if (!state->executionTrace.checkTag(snapshot)) {
		paused_ = true;   // merged with an earlier pass's exit — the pass is over anyway
		return;
	}
	state->executionTrace.addJumpOperation(snapshot, exitBlock_);
}
```

This is idea 1, and it replaces the "scan `[entry, end)` for blocks without a terminator and
wire them up by hand" that a first draft of this plan proposed. Why it is better:

* **It reuses the merge machinery instead of duplicating it.** When pass 2 ends in a
  different block, the same tag is hit, `checkTag` calls `processControlFlowMerge`, and the
  two tails are merged exactly the way the tracer merges the arms of an ordinary `if`.
  Pass 3 merges into that chain, and so on. The region's block shapes therefore match what
  the *unregioned* equivalent produces, which is the central claim of `docs/region.md`.
* **It makes the escape check a one-line comparison** (§4.4) instead of a bespoke
  ref-set diff, and that comparison also covers static-var divergence, which a ref-set diff
  misses.
* **It removes a whole helper** (`openTailsSince`) and the "is this block terminated?"
  predicate it needed.
* **The exit block ends up with exactly one predecessor** in every case, because each extra
  completed pass creates a merge block that repoints the edge. Nothing fans in by hand.

The three guards at the top are exhaustive, and each is a case that has *already* been
terminated by other machinery:

| pass ended … | why no marker |
| --- | --- |
| paused by a control-flow merge | `processControlFlowMerge` already added the `JMP` from the current block |
| paused by a `SecondVisit` termination | the block ends with the `CMP` that paused it |
| entirely in FOLLOW mode | the marker is already recorded at that position |
| normally, in RECORD mode | **the marker is emitted — this is the only case that needs it** |

Note that a function scope needs no equivalent hook: its body already emits a tagged
`RETURN` per completed pass (`ExecutionTrace::addReturn`), and `SSACreationPhase` builds a
unified return block from `returnRefs` (`SSACreationPhase.cpp:107`). A region's exit block
is to a region what that unified return block is to a function.

---

## 4. Design points that must be got right

### 4.1 Scope `TagRecorder` lifetime and root address

`TagRecorder::createReferenceTagBuildin()` walks the frame-pointer chain outwards from the
operation and stops at the frame whose return address equals `startAddress`. Taking
`__builtin_return_address(0)` **inside `LazyTraceContext::traceRegion`** yields the return
address into `tracing::traceRegion` — the same constant for every region call site, and the
first match walking outward is always the innermost enclosing `traceRegion` frame. That is
exactly the scoping we want, and it nests correctly.

**Lifetime hazard:** `TagRecorder::rootTagThreeNode` is a *member* `Tag`, and child trie
nodes carry parent pointers to it. `Tag*` values end up in the shared trace's
`globalTagMap` and in `TraceOperation::tag`, both of which outlive the region. A
stack-local child `TagRecorder` would leave dangling `Tag*` behind. Own the recorders in
the scope that opens the region (`std::deque` — node-stable, and `TagRecorder` holds an
`Arena&`, so existing entries must not move). Allocate the tags from
`trace.getArena()`, not from a scratch arena.

### 4.2 The parent-visible region marker

The `JMP` from the parent's current block into the region entry block must be **tagged with
`key` and registered in `globalTagMap`**. Add
`ExecutionTrace::addJumpOperation(Snapshot&, uint32_t target)` alongside
`addAssignmentOperation` (the existing untagged `makeTraceOp(arena, JMP, blockRef)` form
stays for merge-generated jumps). Two reasons:

* `checkTag(key)` in step (d) needs a recorded operation at that tag so that re-entering the
  same region call site under an unchanged enclosing state (a native loop wrapping the
  region) triggers `processControlFlowMerge` instead of re-recording the whole body.
* `processControlFlowMerge` moves that tagged `JMP` (and everything after it) into the merge
  block, which leaves the region's blocks correctly reachable from the merge block. That is
  structurally right and needs no region-specific handling.

The tagged `JMP` is invisible to `follow()` (both `getCurrentOperation()` and
`nextOperation()` transparently traverse `JMP`), which is why the FOLLOW path in step (c)
uses the memo map instead of the cursor.

### 4.3 Memo lookup key and validity

Key is the full `Snapshot` (parent `Tag*` ^ static/alive hash), same as every other
operation. On a parent FOLLOW pass, the C++ code up to the region re-executes identically
and `follow()` hands back the recorded refs, so the hash matches — the same invariant the
rest of the tracer already depends on.

The memo stores **block indices**, not `(block, operationIndex)` positions. Blocks are never
removed from `ExecutionTrace::blocks`, so an index stays valid; a stored operation index
would not, because a later `processControlFlowMerge` (triggered by the parent's own control
flow after the region) can move operations out of a block. Entering the exit block at index
0 is always correct: if its contents were moved to a merge block, index 0 now holds the
`JMP` that `getCurrentOperation()` traverses transparently.

If the lookup misses in FOLLOW mode, **throw a clear diagnostic** naming `region()` and
suggesting `engine.traceMode = "exceptionBasedTracing"` rather than silently re-tracing.
Silently falling back re-desynchronizes the cursor and produces the class of miscompilation
v1 suffered from.

### 4.4 Escapes: one snapshot comparison

`val::operator=` traces an `ASSIGN` into the **target's existing** `state` ref
(`val_arith.hpp:112`), so the common escape — writing to a captured outer `val` — never
moves ref ownership across the boundary. A region scope's `aliveVars` is therefore normally
empty at the end of each pass, and its exit `Snapshot` is `{stableTag, 0 ^ 0}`.

That gives the escape check for free: **every completed pass must produce the same exit
`Snapshot`** (`traceScopeExit` above). A differing hash means the region's static/alive
state at the exit depends on which path was explored — exactly `docs/region.md`'s
"a value that escapes a region must be built on every path through it". Throw a diagnostic
naming `region()` and `exceptionBasedTracing`; the existing test
`Region Live Escape From One Branch Arm` asserts those two substrings.

A legitimate residue is still possible when a `val` whose ref was allocated inside the
region outlives it (assigned into a `std::optional<val<T>>` or a container declared
outside). For those, at region close `increment()` each residual ref into the parent's
`aliveVars` with the child's count, so the later `freeValRef` from the parent scope does not
trip `AliveVariableHash::decrement`'s "not alive" assert. `AliveVariableHash` needs one
addition for this — a `forEachAlive(F&&) const` over its dense `counts` vector, kept out of
the increment/decrement hot path.

**Symmetric hazard:** a `freeValRef` inside the region for a ref allocated *outside* it
(exotic, e.g. moving an outer `val` into a region-local variable) would assert in the child.
Make a decrement of an id with zero count in the child forward to `parent_` instead of
asserting.

### 4.5 Block-count cost, and where to verify IR equivalence

A region adds an entry block and an exit block that the unregioned equivalent would not
have. Both are single-predecessor / single-successor seams, and the default IR pipeline
already contains `EmptyBlockEliminationPass` and `BlockMergingPass` ("collapses the
single-predecessor seams trace-generated IR is full of",
`CompilationPipeline.cpp`). So: **assert regioned ≡ unregioned equivalence after the IR
pass pipeline, not on the raw trace**, and do not add tracing-time peephole surgery to
close the gap. If the passes turn out not to fold these seams, fixing them there is the
right place — it benefits non-region code too.

Related finding: `Block::Type::ControlFlowMerge` is set only by `processControlFlowMerge`
and read only by the trace formatter. `SSACreationPhase` computes block arguments from
`predecessors` alone. So a hand-wired exit block would not have been *incorrect* for SSA —
the argument for `traceScopeExit` is IR-shape equivalence and reuse of trusted machinery,
not SSA correctness. Do not let a reviewer's "but SSA needs the merge flag" claim redirect
this.

### 4.6 Scope pooling — do not regress tracing time

One context object per scope means one `SymbolicExecutionContext`, `staticVars` vector,
`aliveVars` vector and `mangledNameCache` per region *entry*. `SymbolicExecutionContext`'s
constructor does `tagMap.reserve(128)`, which at one-region-per-branch is paid N times per
trace — enough to eat the tracing win the feature exists to deliver. v1 pooled its
`RegionFrame`s by depth for exactly this reason; do the same for scopes:

* `TraceSession` owns `std::deque<LazyTraceContext> scopePool_`, handed out by nesting
  depth and `initRegionScope()`d rather than constructed.
* Add `SymbolicExecutionContext::reset()` (clear `tagMap`, `inflightExecutionPaths`,
  `currentExecutionPath`, `currentMode = RECORD`, counters) — it does not exist today and
  pooling needs it.
* Add a capacity hint to the `SymbolicExecutionContext` constructor; regions want ~8, not
  128 (v1's `kExpectedRegionTags`).

Measure this: the `chainedIf100Region` tracing benchmark is the guard (v1's figure was
0.310–0.325 ms/iter).

### 4.7 Things that must be routed to the session, not the scope

* `functionsToTrace` / `registeredFunctions` — a `nautilus::function` called inside a region
  must land on the session's worklist, or the callee is never traced.
* `mangledNameCache` (on `TraceContextBase`) and `state->normalizedFunctionNameCache`
  (`ExceptionBasedTraceContext.cpp:522`) — route through the session so a call inside a
  region and the same call outside one normalize to the same name.

### 4.8 Invariants worth asserting

* `traceReturnOperation` and `registerFunctionArgument` are never called on a region scope
  (a region body returns `void` and has no arguments) — throw a clear message if they are.
* `staticVars.empty()` after each pass, for both scope kinds (already asserted for
  functions; the shared loop gives regions the same check).
* A region scope never appends to `returnRefs`.

---

## 5. Public API

`nautilus/include/nautilus/region.hpp` (currently a non-compiling stub — replace):

```cpp
template <typename F>
NAUTILUS_REGION_NOINLINE void region(const char*, F&& fn) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		std::function<void()> wrapper = [&fn]() { fn(); };
		tracing::traceRegion(wrapper);
		return;
	}
#endif
	fn();
}
```

* `createFunctionWrapper` (from `Engine.hpp`) is for `val<T>`-argument functions and is
  **not** what a region needs — a region body is already `void()`. Build the
  `std::function<void()>` directly over a reference to `fn` (captures one pointer, fits in
  `std::function`'s small buffer, no allocation).
* Keep `NAUTILUS_REGION_NOINLINE` on both overloads: `region()` must be a real frame.
* Keep the `const char* name` overload as a debug-only label (unused, as documented).

`TracingUtil.hpp` gains `void traceRegion(std::function<void()>&)`, forwarding to
`getActiveTracer()` (guarded like the other wrappers).
`TracingInterface` gains `virtual void traceRegion(std::function<void()>&) = 0;`.
`ExceptionBasedTraceContext::traceRegion` is a pass-through — that tracer restarts the whole
enclosing function on every unresolved branch, so there is nothing for a region to bound.

---

## 6. Staged implementation

Each stage ends with: `cmake --build . --target format`, then `./format.sh`, then the named
tests. Build with Clang 21, `-DENABLE_TESTS=ON`.

### Stage 0 — restore the corpus (no implementation)

* `git show af5fcb20:docs/region.md > docs/region.md`
* `git show af5fcb20:nautilus/test/execution-tests/RegionTest.cpp > nautilus/test/execution-tests/RegionTest.cpp`
* Add `RegionTest.cpp` to `nautilus/test/execution-tests/CMakeLists.txt`.

That file is 33 traced fixtures / ~35 sections covering basics, static unrolling, replay,
escapes (nested, triple-nested, multiple, with copies, in loops, under static unroll),
internal branches and loops, chained ifs, nesting with control flow before/after/between,
runtime calls, `nautilus::function` calls, allocas, and the diagnosed
escape-from-one-branch-arm case. **It is the acceptance criterion for this plan** — do not
weaken a test to make an implementation pass; if a test is wrong, say so explicitly and
justify it.

Expected: does not compile (the stub `region.hpp` references non-existent symbols). That is
the starting red.

### Stage A — unify function tracing onto `runScope` (no regions, no behaviour change)

Extract `TraceSession` (worklist, `registeredFunctions`, name caches) and `runScope`
(§2), and rewrite `startTrace`'s per-function loop in terms of them. `parent_ == nullptr`
everywhere; no `traceScopeExit`; no region code yet. Add `SymbolicExecutionContext::reset()`
and the capacity hint (§4.6) here, since function tracing exercises them.

Green: the **entire existing test suite**, unchanged, plus no movement in the LLVM-IR
reference test and no regression in `TracingBenchmark`. This stage is a pure refactor and
must be reviewable as one — do not let a single region concept leak into it. Landing it
separately is what keeps the risk of touching the hot, well-tested function path away from
the feature work.

### Stage 1 — plumbing, exception-based tracer only

`TracingInterface::traceRegion`, the `TracingUtil` free function, the new `region.hpp`,
`ExceptionBasedTraceContext::traceRegion` = pass-through, and a `LazyTraceContext::traceRegion`
that is *also* a pass-through for now.

Green: the whole `[region]` suite under **both** trace modes (regions inlined everywhere).
This pins the semantics before any region machinery exists. The one expected failure is
`Region Live Escape From One Branch Arm`'s lazy half, which needs Stage 3's diagnostic.

### Stage 2 — region scopes, branch-free bodies

`traceRegion` (§3) minus the exploration loop: run `runScope` with a
`SymbolicExecutionContext` that completes in one pass. Adds `addJumpOperation`,
`traceScopeExit`, scope pooling, escape transfer, `regionTagRecorders_`, session routing.

Green: `region basic`, `region preserves static unrolling`, `region escaped value
lifecycle`, `region nesting`, all `nested escape` sections, `region empty and unnamed`.

Also add the trace-shape test: a region'd function and the same function without `region()`
must produce the same IR **after the IR pass pipeline** (§4.5). That equivalence is the core
claim of `docs/region.md` and should be pinned early.

### Stage 3 — region-local exploration

Nothing new in `traceRegion`: the loop is already `runScope`. What lands here is the
multi-pass behaviour of `traceScopeExit` — the merge path, the `exitSnapshot_` agreement
check, and the diagnostic (§4.4).

Green: `region internal branch`, `region chained internal branches`, `region internal loop`,
`region operation after internal branch`, `region branches write different captured vars`,
`region live escape with internal branch`, and `Region Live Escape From One Branch Arm`
(both halves).

### Stage 4 — memoized replay in the parent

The FOLLOW path (step (c)), `checkTag` on the region key (step (d)), memo lifetime.

Green: `region replay skips closed body`, `region branch after outer region`, `region branch
after inner region`, `region loop after inner region`, `region branch between inner
regions`, `region branch before inner region`, `region nested escape re-entered in loop`,
`region outer dynamic inner static`.

Add a **counting test**: a region containing N independent `if`s, with the body's invocation
count observable, asserting O(N) body invocations rather than O(N²). That is the whole point
of the feature and nothing in the restored suite pins it.

### Stage 5 — calls and allocas inside regions

With a shared trace these mostly work for free (`allocaSpecs` lives on the trace; the
worklist is routed to the session in Stage 2). Verify rather than build.

Green: `region runtime call alone`, `region runtime call before internal branch`, `region
runtime call inside branch arm`, `region nautilus function call before internal branch`.
Re-verify af5fcb20's claim that an `invoke` target is never executed at trace time, by
counting calls inside a re-explored region.

### Stage 6 — finish

* Full suite: `ctest --test-dir nautilus --output-on-failure` (both trace modes, all
  backends) plus an ASAN build (the `Tag*` lifetime in §4.1 is exactly what ASAN catches).
* Update `docs/region.md`: rewrite "How regions trace branches" around the scope model;
  keep the limitations list, re-verifying each claim.
* Restore the `chainedIf100Region` tracing benchmark; report lazy-path numbers against
  `main` (v1: 0.310–0.325 ms/iter).
* LLVM-IR reference test (`llvm-diff-21`) must show no reference churn for non-region
  functions.

---

## 7. Risks, called out up front

| Risk | Where it bites | Mitigation |
| --- | --- | --- |
| Dangling `Tag*` from a stack-local scope `TagRecorder` | Silent, late, ASAN-visible | §4.1 deque owned by the opening scope |
| Memo key mismatch on parent FOLLOW | Cursor desync → miscompilation | §4.3 throw, never silently re-trace |
| Exit state differs across passes | Trace depends on which pass ran last | §4.4 exit-`Snapshot` equality + diagnostic |
| `aliveVars` decrement across the boundary | Assert in debug, wrong hash in release | §4.4 transfer + forwarding decrement |
| Callee inside a region never traced | Failure at the backend | §4.7 route worklist to the session |
| Region inside a native loop re-recorded per iteration | Trace blow-up | §4.2 tagged entry `JMP` + `checkTag` |
| Per-region allocation eats the tracing win | Benchmark, not tests | §4.6 pooling + SEC capacity hint |
| Stage A refactor destabilises function tracing | Everything | Land Stage A alone, green, before Stage 1 |

## 8. Explicitly out of scope

* Regions under `exceptionBasedTracing` stay inlined pass-throughs.
* Regions returning values (`region()` bodies remain `void`).
* Writes to a captured `static_val` from inside a region (invisible to the tracer in
  general, not a region-specific limitation).
* Any region-specific operation in the trace, the IR, or a backend. If a stage seems to need
  one, stop and re-open the design instead.


---

## 9. Implementation notes

What actually happened, against what this plan predicted.

### Stages 2-5 landed as one change

The plan already noted that `traceRegion` contains no exploration loop of its own -- it is
`runScope` -- so Stage 3 "lands nothing in traceRegion". That turned out to understate it:
once the child scope and `traceScopeExit` existed, internal branches, internal loops,
memoized replay, nesting, calls and allocas all worked without further code. The staging
was still worth having as a verification order, but Stages 2-5 are one commit's worth of
mechanism, not four.

### The exit-`Snapshot` check subsumed the escape check exactly as predicted

`Region Live Escape From One Branch Arm` -- restored from af5fcb20, and the one test that
demands a *diagnostic* rather than a result -- passes off the equality comparison in
`traceScopeExit` alone. No ref-set diffing was written.

### Two defensive checks added beyond the plan

* **A region whose body never completes a pass** would leave the enclosing scope resuming
  in a block with no predecessors. `traceRegion` now throws there rather than letting a
  later phase fail on an unreachable block.
* **A value allocated inside a region that outlives it, in a function that replays that
  call site**, cannot be replayed correctly at all: replay skips the body, so the C++
  object is never created and the live-value state after the region no longer matches the
  recorded pass. §4.4's transfer keeps the accounting sound but cannot fix that. The
  `RegionRecord` now carries a `hasEscapes` flag and the FOLLOW path throws on it. No
  fixture in the restored suite trips either check.

### `SymbolicExecutionContext.hpp` and `SymbolicExecutionPath.hpp` had no include guards

They had only ever been included once per TU. Including the former from
`LazyTraceContext.hpp` (for the owned per-scope context) surfaced it as a redefinition
error. Both now have `#pragma once`.

### The IR-equivalence claim in §4.5 was too strong, in an interesting direction

The plan said to assert regioned == unregioned IR after the pass pipeline. That is false,
and the reason is worth keeping: exploring a region independently records the code after
an internal branch **once**, in the merge block, where the unregioned tracer re-records it
on every incoming path. For three chained ifs the unregioned form materializes five `0`
constants and threads one through a block argument; the regioned form has three and no
such argument. Same CFG, fewer operations.

`Region IR Matches Unregioned IR` therefore asserts equal function and block counts and
`regionedOps <= unregionedOps`, via `ir::computeStatistics`. `docs/region.md`'s
"equivalent to the unregioned version" sentence was rewritten to say this.

### Verified

* `[region]`: 630 assertions, 4 test cases, all passing -- the 35 restored sections across
  both trace modes and every backend, plus the two new white-box tests.
* `Region Bounds Branch Exploration To The Region` pins the mechanism directly: with all
  three branches inside regions the enclosing function is traced in **exactly one pass**,
  against more than one for the unregioned equivalent, with the region bodies costing a
  bounded number of local passes. A `region()` that silently degraded to inline tracing
  fails this test.
* Full suite: 539/547. The 8 failures are `llvm-ir:LLVM IR Test` loop cases that fail
  identically with these changes stashed -- verified against a rebuilt baseline, not
  assumed.

### ASAN

The full `[region]` suite runs clean under `-DENABLE_ADDRESS_SANITIZER=ON` (Debug), 630
assertions, no sanitizer reports. That is the check that exercises §4.1's `Tag*` lifetime
reasoning: the session-owned recorder storage is what keeps the trie roots alive after a
region ends.

### Benchmarks (Stage 6) -- and a caveat about how they were measured

Single-run A/B on this machine is worthless: consecutive runs of the *same* binary vary by
up to 50%. Everything below is min-of-N over **interleaved** runs of binaries kept side by
side (`base` = clean HEAD, `v1` = af5fcb20, `branch` = this work), which brings run-to-run
spread down to a few percent.

Lazy path (`completing_trace`), median of 3 interleaved runs on an idle machine, µs:

| | main | v1 | v2 |
| --- | --- | --- | --- |
| `chainedIf100` (no regions) | 662.8 | 701.0 | 700.5 |
| `chainedIf100Region` | n/a | 83.4 | 100.8 |

* **Regions work, and by a wide margin.** 100.8 µs against 700.5 µs for the same function
  without regions -- 7x less tracing work, and the O(N²)→O(N) scaling shows up directly:
  `chainedIf10` → `chainedIf100` costs 55x unregioned against 16x regioned.
* **Supporting regions costs ~6% on functions that use none**, and that cost is *not*
  specific to this implementation -- v1 and v2 are within noise of each other there
  (+5.8% and +5.7% over main). Moving the region-only state behind `RegionScopeState`
  (see the struct's comment) recovered part of it; the rest is the `parent_` test on the
  `freeValRef` hot path and the slightly larger context object.
* **v2's region path is 1.21x v1's** (100.8 µs vs 83.4 µs), down from 1.54x before the two
  fixes below. What remains is structural: v2 records an entry block, an exit block and
  two tagged jumps per region, and `Block`'s three `std::vector`s malloc on first push, so
  a region costs a handful of allocations that v1 -- which recorded the body inline into
  the enclosing block -- did not pay. Removing them means reintroducing the special-casing
  v2 exists to avoid, so it is left as a deliberate trade.

#### What the profiler said

`sample` on a build trimmed to just `chainedIf100Region` named the cost immediately, and
it was not where the guesses above pointed. The single largest Nautilus symbol was
`transferEscapesTo` -- §4.4's escape hand-off -- because `forEachAlive` scanned the whole
`counts` vector, which is indexed by a *global* value ref and therefore grows with the
trace. Paid once per region, that is quadratic in a function built out of many regions.
`__bzero`/`memset` came next, from `AliveVariableHash::reset()`'s unconditional
`std::fill` over the same vector, paid once per pass of every scope.

Both had the same O(1) fix, resting on an invariant the class already maintains:
`aliveCount` *is* the number of non-zero entries, so when it is zero every slot is already
zero. `reset()` skips the fill and `forEachAlive` returns immediately; `forEachAlive` also
counts down `aliveCount` so it stops as soon as it has seen every live entry, and
`traceRegion` skips the transfer call entirely when nothing escaped. `transferEscapesTo`
disappeared from the profile and the region benchmark went 128.2 → 100.8 µs.

Neither fix is region-specific: `reset()` runs once per pass of *every* scope, so
unregioned tracing benefits too. The invariant is load-bearing, which is why the ASAN
build matters here -- it is a Debug build, so `decrement`'s assert is live and would fire
if a count ever went negative and desynchronised `aliveCount`. It does not.

An earlier version of this note claimed v2 was faster than v1, by comparing a measurement
here against the 0.310-0.325 ms/iter figure in af5fcb20's commit message. That comparison
was invalid -- v1 measures 83.4 µs on this machine, not 310 µs -- and is retracted.

#### Measurement method, because it bit twice

Single-run A/B on this machine is worthless: consecutive runs of the *same* binary varied
by up to 50%, which first hid the ~6% unregioned regression and then made it look like
10-13%. Two things fixed that: interleaving runs of binaries kept side by side rather than
rebuilding between measurements, and noticing that a runaway benchmark process left over
from a profiling run had been competing for CPU the whole time. On an idle machine with
interleaved runs the spread is ~2%. Any future number here should be collected the same
way.


---

## 10. Escapes: why the exit-block copy cannot work, and what replaced it

The proposal was: instead of `transferEscapesTo` moving raw alive-counts into the parent,
emit an explicit copy in the region's end block that assigns each escaping value into a
parent-owned ref, so that a FOLLOW replay could re-initialise it and `hasEscapes` would
not have to be a hard error.

It cannot be made to work, and the reason is not in the trace -- it is in the C++ object
graph.

`TypedValueRefHolder`'s constructor and destructor are what maintain a ref's alive count,
so a ref's liveness is owned by a live `val<T>` object. `val::operator=` assigns *into* the
holder's existing ref (`val_arith.hpp:112`) without changing it. That is exactly why the
supported pattern is safe: a `val<T>` declared outside the region has a ref allocated
before the region and stable across it, so skipping the body on a replay pass changes
nothing the parent can observe. An escape is the opposite shape -- a `val<T>` object
*constructed inside* the region that outlives it -- so its ref cannot be stable, because it
did not exist before the region ran.

On a replay pass the body does not run, so that object is never constructed. Restoring the
tracer's *counts* from the memo is easy; restoring the object is not, and the object is
what matters:

* Once the enclosing scope's exploration flips from FOLLOW to RECORD to explore a new
  branch, every newly recorded operation takes its input refs from the live `val<T>`
  objects. The escaping object does not exist, so the operation would be recorded against
  whatever the user's code yields instead -- an empty `std::optional` is undefined
  behaviour, a default-constructed `val` silently records `$0`.
* The object's destructor never runs on the replay pass either, so any count restored from
  the memo would stay elevated exactly where the recording pass released it, diverging
  again at the next snapshot.

A copy in the end block changes neither: the tracer knows ref numbers, not objects, and
cannot retroactively make a not-yet-constructed object hold a ref.

### So escapes are rejected outright

Rather than keep partial support with a replay-time trap, `traceScopeExit` now rejects
*any* value created inside a region body that is still alive when the body returns, naming
the refs involved. That makes the limitation a single sentence -- nothing created inside a
region may outlive it, carry values out through a `val<T>` declared outside -- instead of a
rule about which escapes happen to survive which exploration order.

It also deletes machinery: `transferEscapesTo`, the `hasEscapes` flag on `RegionRecord`,
the replay-time trap that consulted it, and the cross-pass escape-set comparison are all
gone, because nothing can escape any more. The exit-`Snapshot` comparison stays, where it
now means "a captured `static_val` was written inside the body".

Nine fixtures in the restored suite exercised the old escape-transfer machinery
(`regionMultipleEscapes`, `regionEscapeWithMultipleCopies`, the nested-escape family,
`regionLiveEscapeWithInternalBranch`). They were not deleted: they moved into
`Region Rejects Values Outliving The Body`, which pins both halves -- each is diagnosed
under `lazyTracing`, and each still traces to its original expected value under
`exceptionBasedTracing`, which inlines region bodies. No coverage was lost, and the
expected values from the removed sections are preserved verbatim.

The one relaxation that *would* restore support is to re-run the body under the enclosing
scope's FOLLOW cursor instead of skipping it -- each operation would follow its recorded
counterpart, handing the re-created objects their recorded refs. That is sound only for a
region with no internal branches (a region-internal `CMP` would consult the enclosing
scope's execution path, which knows nothing about it), and it gives up memoised replay for
that region. Worth doing only if escaping regions turn out to matter in practice.

One trap to note for anyone writing tests here: `makeEngine("interpreter")` sets
`engine.Compilation = false`, so it never traces at all. A rejection test written against
it passes vacuously.

### A latent underflow this uncovered

`regionEscapeAcrossBranch` crashed the moment the explicit check went in, and the cause
turned out to predate all of this work. Its `std::optional<val<int64_t>>` is declared
outside the region and emplaced inside, so it survives *across exploration passes*: pass 2's
`emplace` destroys pass 1's value, calling `freeValRef` for a ref whose count `resume()`
had already reset to zero. `AliveVariableHash::decrement` underflowed it to `UINT32_MAX`
while leaving `aliveCount` at 0.

That was invisible while `reset()` unconditionally refilled the vector every pass. Making
`reset()` O(1) (§9) meant the corrupted count survived instead, poisoning every later
snapshot hash and running tracing away until it crashed. `decrement` now returns without
touching anything when the count is already zero, which is the correct semantics -- a scope
whose environment was reset between passes has nothing to give back for a ref allocated in
an earlier pass -- and it restores the `aliveCount == number of non-zero entries` invariant
that the O(1) `reset()` and `forEachAlive` depend on.

The assert that used to sit there could not have caught this: the Debug/ASAN build is where
it is live, and the shape only arises under `lazyTracing`'s per-pass environment reset,
which the assert's author had no reason to expect. Restoring the invariant in `decrement`
is what makes it unnecessary.
