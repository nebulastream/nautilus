# Design: ReplayTraceContext — a log-based, exception-free tracing context

**Status:** Proposal (design only, no implementation yet)

This document proposes `ReplayTraceContext`, a third implementation of the virtual `TracingInterface`
(`nautilus/include/nautilus/tracing/TracingInterface.hpp`), selectable via `engine.traceMode = "replayTracing"`.
It redesigns how Nautilus explores the control flow of a traced function so that **each branch of a traced
program is recorded exactly once**, replay of already-explored path prefixes is a validated O(1)-per-call
cursor walk, and **branches whose condition is a trace-time constant are pruned at trace time** — the
statically impossible side is never executed at all. The two existing contexts
(`ExceptionBasedTraceContext`, `LazyTraceContext`) remain untouched.

For background on the overall tracing pipeline, see [tracing.md](tracing.md).

## 1. Motivation: problems with the current design

Nautilus discovers all control-flow paths by re-executing the traced function once per discovered branch.
Both existing contexts share the same machinery (`SymbolicExecutionContext`, `Snapshot` tags, structural
trace replay) and therefore the same weaknesses:

1. **Fragile replay.** When a re-execution traverses an already-recorded prefix (FOLLOW mode), every trace
   call is answered by walking the *mutable* trace block structure: `follow()` returns
   `executionTrace.getCurrentOperation().resultRef` and advances a structural cursor that chases `JMP`
   operations inserted by control-flow merging
   (`nautilus/src/nautilus/tracing/ExceptionBasedTraceContext.cpp:64-73`,
   `nautilus/src/nautilus/tracing/ExecutionTrace.cpp:234-260`). Correctness rests on the replayed call
   sequence exactly matching a block structure that `processControlFlowMerge` keeps reshuffling. The only
   validation is `assert(currentOperation.op == op)` (ExceptionBasedTraceContext.cpp:71) — compiled out in
   release builds. On desync, a generic `RuntimeException("Operation index out of bounds")` is thrown *in
   the middle of user code* (ExecutionTrace.cpp:238), or worse, a wrong `TypedValueRef` is silently
   returned and the trace is corrupted.

2. **Hash-based branch identity.** Path scheduling identifies branches by `Snapshot` — a stack-walk tag
   combined with an FNV hash of live `static_val` bytes XORed with an alive-variable hash
   (ExceptionBasedTraceContext.cpp:478-480) — stored in a
   `tagMap<Snapshot, FirstVisit|SecondVisit>` plus a queue of boolean decision paths
   (`nautilus/src/nautilus/tracing/symbolic_execution/SymbolicExecutionContext.cpp:8-43`). A hash collision
   mis-merges or mis-terminates exploration with no diagnostic.

3. **Statically impossible paths are executed.** `if (x == 42)` where `x` was built from literals still
   records a `CMP`, spawns an exploration iteration, and *executes* the impossible side. Beyond the wasted
   re-execution, running an infeasible path can violate user-side invariants the feasible path established,
   crash, or record garbage into the trace. (Downstream IR constant folding removes the dead branch later,
   but by then tracing has already paid for — and risked — exploring it.)

4. **Exceptions as control flow.** `ExceptionBasedTraceContext` terminates every explored path by throwing
   `TraceTerminationException` through user stack frames (ExceptionBasedTraceContext.cpp:94-104, 260-289).
   `LazyTraceContext` fixed this with passive mode but kept everything else.

5. **Silently disabled and missing safety checks.** The `MAX_ITERATIONS` guard is commented out
   (SymbolicExecutionContext.cpp:134-138): pathological traces loop forever. `LazyTraceContext`'s passive
   `traceBool()` always returns `false` (LazyTraceContext.cpp:255-259), which hangs forever on loops that
   only exit via `break` on a traced condition (`for (;;) { if (c) break; }`). Non-deterministic traced
   functions (branching on `invoke()` results, time, pointer addresses) are not detected at all.

6. **RECORD-side per-operation cost.** Every newly recorded operation computes a full `Snapshot`: a
   backtrace of up to 256 frames plus a trie walk (`nautilus/src/nautilus/tracing/tag/TagRecorder.cpp:65-98`)
   plus static-value hashing, followed by one or two hash-map lookups (`ExecutionTrace::checkTag`,
   ExecutionTrace.cpp:134-152). This machinery is genuinely needed to detect merges and loop closure — but
   today it is also entangled with *scheduling*, which does not need it.

`ReplayTraceContext` replaces the replay and scheduling layers with a flat per-path **call log** and an
explicit DFS **worklist**, keeps the RECORD-side snapshot/merge machinery unchanged (it is what builds the
trace), adds trace-time constant tracking to prune infeasible branches, and turns every fragile spot above
into a defined, diagnosable error.

## 2. Goals and non-goals

**Goals**

1. A third `TracingInterface` implementation; the existing two stay untouched and selectable.
2. Keep the re-execution model. Fibers/stack snapshots and `fork()`-based exploration were considered and
   ruled out: the stack state of arbitrary traced C++ functions cannot be guaranteed copyable, and `fork()`
   is unsafe inside multithreaded host processes.
3. Each dynamic branch instance is recorded exactly once and spawns at most one pending exploration entry;
   replay of recorded prefixes is O(1) per call, structure-independent, and validated.
4. Trace-time constant branches record no `CMP` and never execute the infeasible side.
5. No exceptions through user code on any normal path; exceptions only as deliberate, documented aborts for
   fatal conditions.
6. Replay divergence (non-determinism), iteration-limit overflow, and passive-mode non-termination produce
   hard, descriptive errors instead of silence.

**Non-goals**

- Changing `ExecutionTrace`, the trace text format, `SSACreationPhase`, or `TraceToIRConversionPhase`
  (traces will merely have fewer blocks where pruning applies).
- Reducing the number of user-function executions below 1 + #recorded-branches — that would require
  resuming execution mid-function, which the re-execution decision rules out.
- Constant folding in the IR (already exists as a separate pass); folding here exists only to prune control
  flow at trace time, recorded operations are kept as-is.
- Cross-compilation trace caching.

## 3. Architecture overview: the mode machine

Per traced function, the context runs an outer exploration loop. Each loop iteration executes the user
function once, in one of three modes:

```
                     iteration 0 starts here
                              |
            +-----------------v------------------+
            |               RECORD               |
            |  snapshot/tag machinery as today,  |
            |  appends one LogEntry per call,    |
            |  pushes PendingBranch per new CMP  |
            +------+--------------+--------------+
   merge detected  |              | user `return`
 (checkTag false)  |              | (traceReturnOperation)
            +------v------+       |
            |   PASSIVE   |<------+
            | all no-ops, |
            | bool=false  |
            +------+------+
                   | user function returns naturally
                   v
        outer loop: pop PendingBranch (LIFO),
        truncate log to its position, flip decision
                   |
            +------v------+
            |   FOLLOW    |  cursor over pathLog, O(1)/call,
            |             |  validates op kind + operand hash
            +------+------+
                   | cursor reaches flip position (a CMP entry)
                   v
            switch to RECORD, trace.setCurrentBlock(falseBlock)
```

Mode transitions:

- **RECORD → PASSIVE**: `ExecutionTrace::checkTag` returns `false` — the current operation already exists
  (control-flow merge or loop closure), `processControlFlowMerge` has wired the merge block, the path is
  complete. Instead of throwing, the context goes passive and lets the function return naturally
  (`LazyTraceContext`'s idea, LazyTraceContext.cpp:81-98).
- **FOLLOW → RECORD**: exactly when the replay cursor reaches the pending branch's log position. No
  final-tag stack walk, no tag-map rendezvous (contrast `SymbolicExecutionContext::follow`,
  SymbolicExecutionContext.cpp:93-103) — the position in the log *is* the branch identity.
- **FOLLOW → PASSIVE**: validation mismatch (divergence). The error is latched and reported by the driver
  loop after the function returns; nothing is thrown through user frames.
- **PASSIVE** is absorbing within an iteration.

## 4. Core data structures

All members live on the thread-local `ReplayTraceContext`. The log, worklist, and constant tracker are
per-function and reset in the multi-function `startTrace` loop.

```cpp
// One entry per TracingInterface call made by user code while in RECORD mode.
// Stored in *call order* — the order user code observes — which is invariant
// under processControlFlowMerge restructuring: the log stores the returned
// value, not a position in the block graph.
struct LogEntry {
	Op op;                 // op kind as seen at the interface (CMP for traceBool)
	uint8_t flags;         // bit0 DECISION (CMP direction), bit1 PRUNED (CMP), bit2 HAS_RESULT
	Type resultType;
	TypedValueRef result;  // the ref returned to user code
	uint64_t operandHash;  // determinism checksum, see section 9
	bool decision() const;
};

// One per recorded (non-pruned) CMP: the not-yet-explored side.
struct PendingBranch {
	uint32_t logPos;       // index of the CMP's LogEntry in pathLog
	uint32_t falseBlockId; // block pre-created empty by addCmpOperation; block ids are
	                       // stable: merges move operations, blocks are never renumbered
};

// Trace-time constness lattice, monotone toward Runtime (never back).
// Indexed by ValueRef (dense small integers from ExecutionTrace::lastValueRef).
// Persistent across iterations; see section 6 for the soundness argument.
class ConstantTracker {
	enum class State : uint8_t { Unknown, Runtime, Const };
	struct Entry { State state = State::Unknown; ConstantLiteral lit; };
	std::vector<Entry> entries; // resized on demand
public:
	void setConst(ValueRef r, const ConstantLiteral& l);
	void setRuntime(ValueRef r); // kill — never undone
	std::optional<bool> tryFoldBool(ValueRef r) const;
	// fold(op, resultType, lits...) -> nullopt for unfoldable ops or trapping folds
	std::optional<ConstantLiteral> fold(Op op, Type resultType, /* literals */);
};

class ReplayTraceContext final : public TraceContextBase {
	enum class Mode : uint8_t { Record, Follow, Passive };

	// --- per-function exploration state ---
	std::vector<LogEntry> pathLog;       // log of the current DFS path
	std::vector<PendingBranch> worklist; // LIFO => depth-first exploration
	ConstantTracker constants;

	// --- per-iteration state ---
	Mode mode = Mode::Record;
	uint32_t cursor = 0;                 // FOLLOW position in pathLog
	uint32_t flipPos = kNoFlip;          // log index of the pending CMP
	uint32_t pendingFalseBlock = 0;
	uint64_t passiveSteps = 0;
	std::optional<DivergenceInfo> divergence; // latched error, see section 9

	// --- shared with the existing contexts ---
	std::vector<StaticVarHolder> staticVars; // as today
	AliveVariableHash aliveVars;             // as today
	TypedValueRef dummyRef_ {0, Type::v};    // passive-mode return, like Lazy
	std::list<compiler::CompilableFunction> functionsToTrace;
	std::unordered_set<std::string> registeredFunctions;
};
```

Everything on the `ExecutionTrace` side is **reused unchanged** and only ever invoked while
`mode == Record`: `Snapshot` creation, `globalTagMap`/`localTagMap`, `checkTag`,
`processControlFlowMerge`, the `CONST` global-dedup special case
(ExceptionBasedTraceContext.cpp:81-92, mirrored in LazyTraceContext.cpp:68-78), and `resetExecution`'s
local-to-global tag-map merge (ExecutionTrace.cpp:360-365).

## 5. The replay log and FOLLOW mode

This is the heart of the design.

### What is logged

Every interface call user code makes while in RECORD mode appends one `LogEntry`:

| Interface method | logged? | entry contents |
|---|---|---|
| `registerFunctionArgument`, `traceConstant`, `traceAlloca`, `traceCopy`, `traceBinaryOp`, `traceUnaryOp`, `traceTernaryOp`, `traceCall`, `traceIndirectCall`, `traceNautilusCall`, `traceNautilusFunctionPtr` | yes | op kind + result ref + operand hash |
| `traceAssignment`, `traceReturnOperation` | yes (void entry) | op kind + operand hash, `HAS_RESULT` clear — logged purely so the cursor stays in lockstep and divergence is caught at the exact call |
| `traceBool` | yes | `CMP` kind, `DECISION` bit, `PRUNED` bit if folded (section 6) |
| `allocateValRef`, `freeValRef`, `pushStaticVal`, `popStaticVal` | **no** | state maintenance — executed for real in both RECORD *and* FOLLOW (see below) |

### FOLLOW semantics

Each call in FOLLOW mode does exactly this: bounds-check the cursor, compare the entry's op kind and
operand hash against the actual call (mismatch ⇒ divergence, section 9), advance the cursor, and return
`entry.result`. Returning a reference into `pathLog` is safe for the same reason `LazyTraceContext` returns
`dummyRef_`: callers (`val<T>` constructors) copy the `TypedValueRef` immediately and never hold the
reference across calls (LazyTraceContext.hpp:124-126). `traceBool` returns `entry.decision()`.

FOLLOW performs **no snapshot creation, no tag lookups, and no `ExecutionTrace` access at all** — in
particular, no structural cursor walking the block graph.

### The flip

Each `PendingBranch` stores its `CMP`'s log position. When `cursor == flipPos` at a `traceBool` call, the
context returns the (pre-flipped, see section 7) `false` decision, sets `mode = Record`, and calls
`executionTrace.setCurrentBlock(pendingFalseBlock)`. The false block was created empty by
`addCmpOperation` (ExecutionTrace.cpp:214-232) and — by the once-per-branch invariant — is still empty.

### Why tags can be skipped entirely during FOLLOW

The prefix `pathLog[0..flipPos]` was fully processed by the iteration that recorded it: its operations are
in the block graph and its tags were merged into `globalTagMap` by `resetExecution`
(ExecutionTrace.cpp:360-365). Replay's only job is to hand user code the same `TypedValueRef`s so that the
post-flip segment sees identical operands. Loop closures inside the prefix need no re-detection. Crucially,
the log is immune to merge-time restructuring: `processControlFlowMerge` moves operation *pointers* between
blocks but never changes an operation's `resultRef` (ExecutionTrace.cpp:284-308), and the log stores those
result refs by value, not block positions. This removes the entire class of structural-cursor desyncs that
plagues the current `follow()`.

### State that must still be maintained in FOLLOW

So that the first post-flip `recordSnapshot()` is consistent with the snapshots stored in `globalTagMap`,
the non-logged maintenance calls run for real in FOLLOW:

- `allocateValRef` / `freeValRef` update `aliveVars` (replay is deterministic, so the live-set hash at the
  flip equals what it was at record time).
- `pushStaticVal` / `popStaticVal` maintain `staticVars` (the static values themselves evolve inside the
  re-executed user code; the tracer only tracks the stack of pointers).
- `traceNautilusCall` / `traceNautilusFunctionPtr` in FOLLOW do **not** re-register the callee — the name
  entered `registeredFunctions` when the call was recorded.
- `ExecutionTrace::lastValueRef` is untouched in FOLLOW (all replayed refs already exist).

## 6. Constness tracking and branch pruning

During tracing, `val<T>` raw values are **not** computed: a `val<bool>` constructed from a traced operation
carries `value(false)` (`nautilus/include/nautilus/val_bool.hpp:265`), and `operator bool()` returns
`traceBool(...)`'s result directly (val_bool.hpp:338-346). The tracer therefore cannot observe "the real
runtime value" of a condition — to prune, it must fold constants itself. `ConstantTracker` does this on
`ConstantLiteral`s. All rules apply in RECORD mode only; FOLLOW never consults or updates the tracker.

**Sources.**

- `traceConstant(type, lit)` ⇒ `Const(lit)`. This covers `val<int> x = 5;`, `val<bool> b = true;`, etc.
- `registerFunctionArgument`, `traceCall`, `traceIndirectCall`, `traceNautilusCall`,
  `traceNautilusFunctionPtr`, `traceAlloca`, `traceUnaryOp(LOAD)`, `traceTernaryOp(SELECT)` ⇒ `Runtime`.

**Propagation.**

- `traceCopy(src)` ⇒ the result inherits `src`'s state. Copy construction is pervasive in `val<T>` code;
  without this rule almost nothing would stay constant.
- `traceBinaryOp` / `traceUnaryOp` with all-`Const` inputs and a foldable op
  (`EQ NEQ LT LTE GT GTE ADD SUB MUL DIV MOD AND OR NOT NEGATE BAND BOR BXOR LSH RSH CAST`) ⇒
  `Const(fold(...))`. Folding operates on the literals at the already-promoted types — `val_arith` applies
  C++ integer promotion before calling `traceBinaryOp` (val_arith.hpp:243-265) — so the folded result
  matches what compiled code would compute.
- **Trapping or UB-prone folds produce `Runtime`, never a pruned branch**: `DIV`/`MOD` with a zero divisor,
  shifts ≥ bit width, and any literal-type combination the folder does not handle exactly.
- Any input that is not `Const` ⇒ result is `Runtime`.

**Kill rule.** `traceAssignment(target, source)` **always sets `target` to `Runtime`**, even when `source`
is `Const`. This single conservative rule carries the soundness of the whole scheme:

- *Soundness across merges.* A constness fact must hold on every path on which the ref is later used. Fresh
  result refs have exactly one defining operation with one literal, so they are trivially path-independent.
  The only refs whose value can differ per path are ASSIGN targets (mutable `val`s) — exactly the refs that
  cross control-flow merges as block arguments. Killing every ASSIGN target makes all remaining `Const`
  facts path-independent, so the tracker may safely persist across iterations. Monotonicity
  (`Unknown → {Const | Runtime} → Runtime`, never back) guarantees a stale entry can only be *more*
  conservative than the truth.
- *Bounded unrolling.* In `for (val<int> i = 0; i < 10; ++i)`, the increment is an ADD followed by an
  ASSIGN to `i`, which kills `i`. Only the *first* `i < 10` check folds (provably true ⇒ pruned, sound: the
  trace becomes do-while-shaped); the second check records a real `CMP` and loop closure proceeds via
  `localTagMap` exactly as today. Constness tracking therefore cannot cause unbounded trace-time loop
  unrolling. Users who *want* full unrolling already have `static_val`, which is unaffected by this design
  (its conditions are raw C++ bools that never reach `traceBool`).

**Pruning in `traceBool` (RECORD mode).** If `constants.tryFoldBool(cond.ref)` yields `b`: append a
`LogEntry{CMP, PRUNED, decision=b}`, record **nothing** in the trace (no `CMP` operation, no blocks, no
snapshot, no pending branch), and return `b`. The infeasible side is never executed in any iteration —
neither recorded nor replayed nor passively walked. In FOLLOW, the pruned entry replays `b` from the log,
so the tracker is not needed during replay.

**Interaction with merges.** Pruned branches create no blocks and no merge, so nothing can merge "across" a
pruned branch. Recorded `CMP`s keep both sides exactly as today. Refs flowing into merge blocks as block
arguments are ASSIGN targets and therefore already `Runtime`. No additional invalidation is needed.

## 7. Branch scheduling: explicit DFS over a shared-prefix log

This layer fully replaces `SymbolicExecutionContext` (`tagMap<Snapshot, FirstVisit|SecondVisit>` plus the
queue of boolean paths).

**Branch identity = log position.** The traced function is required to be deterministic (and section 9
enforces it), and FOLLOW replays the prefix exactly. Therefore "the i-th interface call of an execution
that took decisions d0..dk" identifies a unique dynamic branch instance. The same source-level `if` in two
unrolled loop iterations occupies two different log positions ⇒ two distinct branch instances — matching
the semantics today's snapshot-plus-static-hash identity provides, but exact instead of hash-based. Tags
remain only for what they are genuinely needed for: RECORD-mode operation dedup, merge detection, and loop
closure.

**LIFO worklist with log truncation.** Recording a `CMP` pushes `PendingBranch{logPos, falseBlockId}`. The
outer loop pops LIFO (depth-first):

```
pop p from worklist
pathLog.resize(p.logPos + 1)              // discard the explored side's suffix
pathLog[p.logPos].setDecision(false)      // pre-flip the stored decision
flipPos = p.logPos
pendingFalseBlock = p.falseBlockId
```

*DFS invariant:* every entry remaining in the worklist was pushed before `p` on the current path, hence has
`logPos <= p.logPos` — entries pushed while exploring `p`'s subtree are popped first. Truncation therefore
never destroys a prefix some pending branch still needs. The log is a single flat vector that always equals
the current DFS path: no copy-on-write, no path tree, O(longest-path) memory. (Today's FIFO scheduling
would require a full decision-path copy per pending branch, which is exactly what
`SymbolicExecutionPath` does. Exploration *order* differs from today, which is acceptable because the new
context gets its own golden files, section 11.)

**Once-per-branch invariant.** A `CMP` is recorded only in RECORD mode at a fresh log position; recording
pushes exactly one pending entry; the flip consumes that entry and records into the pre-created,
still-empty false block; FOLLOW never records anything. Hence each dynamic branch is recorded exactly once
and executed at most twice (true side during its recording run, false side in its own iteration); pruned
branches contribute zero extra iterations. The `FirstVisit`/`SecondVisit` bookkeeping disappears entirely.

**Path termination and passive mode.** In RECORD, `checkTag(tag) == false` means
`processControlFlowMerge` has already wired the merge — the path is complete: enter PASSIVE. Passive
behavior mirrors `LazyTraceContext` (LazyTraceContext.cpp:255-301):

- all value-producing methods return `dummyRef_`; assignment/return are no-ops; `pushStaticVal` /
  `popStaticVal` keep the static stack balanced; `allocateValRef` / `freeValRef` are skipped.
- `traceBool` returns **`false`**, now guarded by a **passive step limit** (option
  `engine.tracePassiveStepLimit`, default 1,000,000): exceeding it raises `TraceIncompleteException`.
  Rationale for `false`: it exits every loop whose continuation requires a true condition (`while (c)`,
  `do { } while (c)`; for `while (!c)` the NOT is itself a traced operation, so the `CMP` input is the
  negation result and `false` still exits). The pathological case is a loop that exits only via `break` on
  a traced condition (`for (;;) { if (c) break; }`): `false` never breaks, and `LazyTraceContext` hangs
  forever there today. The step limit converts that hang into a hard diagnostic ("passive replay cannot
  exit this loop; the loop exits only via break on a runtime condition"). Returning the real runtime value
  is impossible (raw values are not computed during tracing, section 6), and constness-aware passive
  decisions were rejected: they would turn the common `while (true) { ... break; }` pattern into a
  guaranteed limit error.
- Exception discipline: PASSIVE is the no-exception path for all *normal* terminations. Exactly two fatal
  conditions abort mid-execution by throwing — `TraceIncompleteException` (passive step limit) and
  `TagCreationException` (stack too deep, TagRecorder.cpp:76-78, unchanged) — both unrecoverable program
  errors, documented as the deliberate exception to goal 5. Unwinding through user frames is safe in
  exactly the way today's `TraceTerminationException` unwind is: `static_val` and `val<T>` destructors fire
  normally (ExceptionBasedTraceContext.cpp:387-398 asserts the static stack is empty afterwards).

**Outer driver loop (per function):**

```
iterations = 0; mode = Record
pathLog.clear(); worklist.clear(); constants.clear()
run user function                          // iteration 0: records the spine
while worklist not empty:
    if ++iterations > maxIterations: throw TraceLimitException(name, iterations)
    pop & prepare pending branch           // truncate log, pre-flip decision
    executionTrace.resetExecution()        // also merges localTagMap -> globalTagMap
    staticVars.clear(); aliveVars.reset()
    mode = Follow; cursor = 0; passiveSteps = 0; divergence.reset()
    run user function                      // FOLLOW -> flip -> RECORD -> PASSIVE -> return
    if divergence: throw TraceDivergenceException(*divergence)
    if staticVars not empty: throw TraceDivergenceException("static stack imbalance ...")
```

`maxIterations` comes from option `engine.traceMaxIterations` (default 100000) and is **enforced** — unlike
the commented-out check at SymbolicExecutionContext.cpp:134-138.

## 8. Algorithms (pseudocode)

```cpp
// Generic value-producing operation (binary/unary/ternary/copy/call/alloca/const...).
TypedValueRef& traceOperation(Op op, OperandSpan operands, OnCreation onCreate) {
	switch (mode) {
	case Passive:
		return dummyRef_;
	case Follow: {
		if (cursor >= pathLog.size())
			return diverge(op, operands);                 // latches info, -> Passive
		auto& e = pathLog[cursor];
		if (e.op != op || (validateReplay && e.operandHash != hash(op, operands)))
			return diverge(op, operands);
		++cursor;
		return e.result;                                  // no snapshot, no trace access
	}
	case Record: {
		auto tag = recordSnapshot();                      // unchanged machinery
		if (!executionTrace.checkTag(tag)) {              // merge already wired
			mode = Passive;
			return dummyRef_;
		}
		TypedValueRef& r = onCreate(tag);                 // addOperationWithResult etc.
		constants.onResult(op, r, operands);              // fold or mark Runtime (section 6)
		pathLog.push_back({op, HAS_RESULT, r.type, r, hash(op, operands)});
		return r;
	}
	}
}

bool traceBool(const TypedValueRef& cond, double probability) {
	if (mode == Passive) {
		if (++passiveSteps > passiveStepLimit)
			throw TraceIncompleteException(...);          // documented fatal abort
		return false;
	}
	if (mode == Follow) {
		// bounds + kind + hash checked as above; mismatch -> divergeBool() -> Passive, false
		auto& e = pathLog[cursor];
		bool d = e.decision();
		if (cursor == flipPos) {                          // the pending branch
			mode = Record;                                // d is already false (pre-flipped)
			executionTrace.setCurrentBlock(pendingFalseBlock);
		}
		++cursor;
		return d;
	}
	// Record:
	if (auto c = constants.tryFoldBool(cond.ref)) {       // section 6: prune
		pathLog.push_back(prunedCmpEntry(*c, hash(CMP, cond)));
		return *c;                                        // nothing recorded, no exploration
	}
	auto tag = recordSnapshot();
	if (!executionTrace.checkTag(tag)) {                  // loop closure / merge at this CMP
		mode = Passive;
		return false;
	}
	executionTrace.addCmpOperation(tag, cond, probability);
	auto& cmp = executionTrace.getCurrentOperation();     // the CMP just added
	uint32_t trueBlock  = blockOf(cmp.input[1]);
	uint32_t falseBlock = blockOf(cmp.input[2]);
	worklist.push_back({(uint32_t) pathLog.size(), falseBlock});
	pathLog.push_back(cmpEntry(/*decision=*/true, hash(CMP, cond)));
	executionTrace.setCurrentBlock(trueBlock);            // explore the true side first, as today
	return true;
}
```

Notes:

- `traceConstant` keeps the global-dedup special case in RECORD mode (a repeated constant site emits a
  fresh `CONST` plus an `ASSIGN` to the original ref and returns the original ref,
  ExceptionBasedTraceContext.cpp:81-92). This is required so refs defined in one branch arm are also
  defined on the other arm — visible as the `CONST $13` / `ASSIGN $8 $13` pair in
  `nautilus/test/data/control-flow-tests/tracing/ifThenElseCondition.trace`.
- `traceAssignment` logs a void entry; in RECORD it additionally calls `constants.setRuntime(target.ref)`.
- `traceReturnOperation`: RECORD ⇒ `addReturn` + void log entry; FOLLOW ⇒ validate + advance; PASSIVE ⇒
  no-op. After a real `return`, the iteration ends naturally.
- `traceAlloca`'s `addAllocaSpec` call stays inside the tag-checked creation lambda, preserving the
  one-spec-per-site invariant documented at `nautilus/src/nautilus/tracing/ExecutionTrace.hpp:308-319` —
  in the new context that lambda only runs in RECORD mode.

## 9. Non-determinism detection

Each `LogEntry` carries `operandHash`: 64-bit FNV-1a over (op kind, result type, each operand's ref id and
type), plus the literal bytes for `CONST`, the callee pointer and argument refs for `CALL`, and the
condition ref for `CMP`. It is computed once at record time and recomputed in FOLLOW from the actual call
arguments — a handful of integer mixes, no allocation, no stack walk.

On a kind/hash mismatch, a cursor overrun (the function makes more calls than logged before the flip), or a
premature return (the function returns while `cursor < flipPos`):

1. latch `DivergenceInfo{ functionName, iteration, callIndex = cursor, expectedOp, expectedHash, actualOp,
   actualHash }`,
2. enter PASSIVE so the function returns naturally (nothing thrown through user frames),
3. the driver loop throws `TraceDivergenceException` (subclass of the existing nautilus
   `RuntimeException`) with a message like:

```
Trace replay diverged in function 'execute' (iteration 7, call #142):
expected ADD(i32 $17, $23), observed MUL(i32 $17, $24).
The traced function must be deterministic across re-executions. Common causes:
branching on invoke() results, time/random sources, pointer addresses, or
mutated external state.
```

This subsumes today's only divergence "detection" — the debug-only assert
(ExceptionBasedTraceContext.cpp:71) and the release-mode generic `RuntimeException` thrown mid-user-code
(ExecutionTrace.cpp:238). An option `engine.traceValidateReplay` (default `true`) can disable the hash
computation for maximum replay speed on trusted workloads; the op-kind and bounds checks are free and
always on.

## 10. Class structure and integration

- **Name and files:** `ReplayTraceContext` (final), in
  `nautilus/src/nautilus/tracing/ReplayTraceContext.{hpp,cpp}`, with
  `nautilus/src/nautilus/tracing/replay/{ReplayLog.hpp, ConstantTracker.hpp, ConstantTracker.cpp}` and new
  exception types `TraceDivergenceException`, `TraceLimitException`, `TraceIncompleteException` under
  `nautilus/src/nautilus/tracing/exceptions/`. CMake entries mirror the existing
  `symbolic_execution/` setup.
- **Base class:** subclass `TraceContextBase` (ExceptionBasedTraceContext.hpp:158-171) to reuse
  `getMangledName` / `getFunctionName` and the injected `TraceState`. `TraceState` currently holds a
  `SymbolicExecutionContext&` (ExceptionBasedTraceContext.hpp:140-149); the new context does not use one,
  so either the field becomes a nullable pointer with a second constructor (a mechanical change that does
  not alter the existing contexts' behavior), or — if zero churn in the shared header is preferred — the
  new context stack-allocates an unused dummy instance exactly where the existing contexts allocate a real
  one. Recorded as an open question (section 13).
- **Entry points:** static `Trace(std::list<CompilableFunction>&, const Options&, Arena&) ->
  std::unique_ptr<TraceModule>` plus member `startTrace`, with the identical multi-function work-list loop
  as the other two contexts (ExceptionBasedTraceContext.cpp:346-408): pop a function, `addNewFunction`, tag
  the first as `entry`, fresh `TagRecorder`, per-function reset of `pathLog` / `worklist` / `constants`,
  run the section-7 driver loop, `ActiveTracerGuard` for cleanup. Thread-local instance registered via
  `setActiveTracer(this)`.
- **Dispatch:** the two-way ternary in `nautilus/src/nautilus/compiler/LegacyCompiler.cpp:124-131` becomes
  a three-way selection on `engine.traceMode`: `"lazyTracing"` (default, unchanged),
  `"exceptionBasedTracing"`, `"replayTracing"`.
- **New options:** `engine.traceMaxIterations` (uint, default 100000), `engine.tracePassiveStepLimit`
  (uint, default 1000000), `engine.traceValidateReplay` (bool, default true). To be documented in
  [options.md](options.md) when implemented.

## 11. Testing and migration strategy

1. **Execution equivalence (primary oracle, no goldens needed):** add `"replayTracing"` to the trace-mode
   list in `forEachBackendWithTraceMode` (`nautilus/test/common/ExecutionTest.hpp:78`) and the per-test
   loops that enumerate trace modes locally (e.g.
   `nautilus/test/execution-tests/BoolExecutionTest.cpp:303`). Every execution test then verifies that
   replay-traced and compiled code produces identical runtime results across all backends.
2. **Golden traces:** extend the trace-test harness (`nautilus/test/execution-tests/TracingTest.cpp`) so
   each context entry carries a reference-dump directory suffix; `ReplayTraceContext` reads/writes
   `tracing-replay/`, `after_ssa-replay/`, `ir-replay/` sibling directories under
   `nautilus/test/data/<category>/`. The existing golden files for the two old contexts are untouched —
   pruning legitimately changes traces (fewer blocks wherever conditions are literal-derived), and
   DFS exploration changes block numbering. The first generated set is reviewed by hand, with pruning
   diffs spot-checked against expectations.
3. **New unit tests:** divergence (a function branching on a mutated global ⇒ expect
   `TraceDivergenceException` naming the call index); iteration limit (pathological branch generator with a
   tiny `engine.traceMaxIterations`); passive step limit (`for (;;) { if (c) break; }`); pruning (literal
   conditions produce zero `CMP` operations); static-stack imbalance diagnostic.
4. **Benchmarks:** add a `replayTracing` variant in `nautilus/test/benchmark/ExecutionBenchmark.cpp` to
   quantify tracing-time wins (the `tracing.ms` statistic is already recorded, LegacyCompiler.cpp:133),
   especially on branch-heavy and loop-heavy workloads.
5. **Migration:** ship opt-in behind `engine.traceMode = "replayTracing"`. After a soak period, consider
   flipping the default in `LegacyCompiler.cpp`; the old contexts remain selectable indefinitely.

## 12. Worked examples

### 12.1 If/else

`execute(x): r = 1; if (x == 42) { r += 1; } else { r += 42; } return r + 42;` — the shape behind
`nautilus/test/data/control-flow-tests/tracing/ifThenElseCondition.trace`.

**Iteration 0 (RECORD).** Calls and log entries:

```
#0  arg(i32)            -> $1        (Runtime)
#1  CONST 1             -> $2        (Const 1)
#2  CONST 42            -> $3        (Const 42)
#3  EQ($1, $3)          -> $4        ($1 Runtime => $4 Runtime, no prune)
#4  CMP($4)             decision=true   push Pending{logPos=4, falseBlock=B2}
        ... continue in B1 (true side):
#5  CONST 1             -> $6
#6  ADD($2, $6)         -> $7
#7  ASSIGN $2 <- $7                  (kills $2)
#8  CONST 42            -> $8
#9  ADD($2, $8)         -> $9
#10 RETURN $9
```

The function returns; the log has 11 entries.

**Iteration 1.** Pop `Pending{4, B2}`; truncate the log to 5 entries (#0..#4); pre-flip #4 to `false`.
FOLLOW replays #0..#3, returning $1, $2, $3, $4 with zero trace access; at #4, `cursor == flipPos` ⇒
RECORD, current block = B2, return `false`. Record on the false side: `CONST 42 -> $11`,
`ADD -> $12`, `ASSIGN $2 <- $12`; the next `CONST 42` hits the global CONST dedup ⇒ emits
`CONST $13` plus `ASSIGN $8 <- $13` and returns $8; the following `ADD($2, $8)` produces a snapshot equal
to iteration 0's entry #9 in `globalTagMap` ⇒ `checkTag` is false ⇒ merge block B3 is created, the
ADD/RETURN move there, JMPs are wired ⇒ PASSIVE; the user-level `return` is a no-op; the function exits.
The worklist is empty ⇒ done. **Two executions, zero exceptions**, and the resulting trace matches today's
golden file modulo block numbering.

**Variant with a literal condition** (`val<int> x = 42;` instead of an argument): `EQ(Const 42, Const 42)`
folds to `Const(true)`; `traceBool` prunes — no CMP, no B1/B2, no iteration 1. The final trace is one
straight-line block, produced by a **single execution**. Today this costs an extra iteration and executes
the impossible `r += 42` arm.

### 12.2 Loop

`sumLoop(n): agg = 1; for (i = 0; i < n; ++i) { agg += 10; } return agg;` —
`nautilus/test/data/loop-tests/tracing/sumLoop.trace`.

**Iteration 0 (RECORD).**

```
#0  arg(i32)            -> $1
#1  CONST 1             -> $2        (agg)
#2  CONST 0             -> $3        (i)
#3  LT($3, $1)          -> $4        ($1 Runtime => no prune)
#4  CMP($4)             decision=true   push Pending{logPos=4, falseBlock=B2}
        ... loop body:
#5  CONST 10 -> $6   #6 ADD -> $7   #7 ASSIGN $2<-$7 (kill agg)
#8  CONST 1  -> $8   #9 ADD -> $9   #10 ASSIGN $3<-$9 (kill i)
        ... back edge: second LT
```

The second `LT`'s snapshot equals the first's (same tag, same static/alive hashes) ⇒ `localTagMap` hit ⇒
`processControlFlowMerge` moves the LT and CMP into merge block B3 ⇒ PASSIVE. Passive `traceBool` returns
`false` ⇒ the loop exits; the `RETURN` is a passive no-op; the function returns. The log has 11 entries
(the second LT appended nothing — it terminated the path).

**Iteration 1.** Pop `Pending{4, B2}`; truncate to 5 entries; FOLLOW #0..#3 — note the replay never looks
at B3 or any block: entry #3 stores `$4` by value, so the fact that the LT operation *moved* into the merge
block is invisible to replay (this exact situation is what desyncs a structural cursor). Flip at #4 ⇒
RECORD in B2: `RETURN $2`. Done. The final trace is the same shape as today's golden file
(`B0 -> B3{LT, CMP} -> B1 body -> B3`, `B2` return).

## 13. Edge cases

- **Nested loops:** inner-loop closure is handled in RECORD by `localTagMap` exactly as today. A prefix
  containing closed inner loops replays as a linear scan of the log (one entry per call the user code made
  while recording). Truncation discards post-closure entries when the enclosing pending branch is popped.
- **Short-circuit `&&` / `||`** (`ENABLE_SHORT_CIRCUIT_BOOL`): each operand reaches its own `traceBool` ⇒
  its own log entry and pending branch; pruning applies per operand (a literal `true` right-hand side
  disappears entirely). In the non-short-circuit default, `AND`/`OR` are binary ops and fold when both
  operands are `Const`.
- **`return` inside a branch:** RETURN entries are logged; the iteration ends at the user `return` with a
  non-empty worklist; later pending branches replay shorter prefixes — truncation already handles this.
  Multiple returns accumulate in `returnRefs` as today (ExecutionTrace.cpp:154-171).
- **Nested Nautilus calls:** `traceNautilusCall` records one CALL operation and queues the callee for
  separate tracing (RECORD mode only); the callee body never executes during the caller's trace, so it
  contributes exactly one log entry. Each queued function gets its own log/worklist/constants in the
  `startTrace` loop.
- **Functions whose only branches are pruned:** the worklist stays empty ⇒ a single execution and a
  straight-line trace.
- **Branch on `val<bool>` literals / default-constructed values:** all pruned.
- **`TagCreationException` (stack too deep):** can now only fire in RECORD mode; deep prefixes replay
  without stack walks — a strict reliability improvement for deeply nested traced code.
- **Divergence on a pruned entry during FOLLOW:** folding is deterministic, so a mismatch there indicates
  real non-determinism and takes the same diagnostic path as any other divergence.

## 14. Complexity comparison

| Dimension | Current (both contexts) | ReplayTraceContext |
|---|---|---|
| RECORD, per operation | backtrace (≤256 frames) + trie walk + static-val FNV hash + 1–2 map lookups | same (unchanged; needed for merges/loops) + O(#operands) hash and fold + one vector append |
| FOLLOW, per operation | structural cursor over the block graph (JMP chasing); debug-only assert; throws through user code on desync | `pathLog[cursor++]` + integer hash compare; structure-independent; validated |
| FOLLOW, per branch | decision-path lookup; at path end: final-tag stack walk + `record(finalTag)` tag-map rendezvous | one log entry; flip = a single `cursor == flipPos` comparison |
| Branch identity | `Snapshot` (stack walk + XOR hashes), collision-prone | log position — exact under determinism, which is enforced |
| User-function executions | 1 + #branches | 1 + #recorded branches − #pruned branches |
| Statically impossible paths | executed | never executed |
| Exceptions through user code | 1 per iteration (exception-based); 0 (lazy) but silent infinite loops possible | 0 on all normal paths; throws only as documented fatal aborts; divergence reported post-return |
| Scheduling memory | tag map over all snapshots + one full decision path per pending branch | flat log (≈24 B × longest path) + worklist (≤ open branches) |
| Failure modes | silent wrong trace / infinite loop / generic `RuntimeException` | `TraceDivergenceException` / `TraceLimitException` / `TraceIncompleteException`, each with operation-level context |

## 15. Open questions

- **ASSIGN constness refinement.** The always-kill rule forfeits pruning of patterns like
  `val<bool> flag = true; ...; if (flag)`. A future refinement could propagate `Const` through ASSIGN with
  a per-branch prune budget to keep trace-time loop unrolling bounded; this needs a policy for budget
  exhaustion and is deliberately deferred.
- **`TraceState` refactor vs. dummy `SymbolicExecutionContext`.** The nullable-pointer constructor touches
  a shared header; the dummy-instance fallback is behaviorally identical with zero shared churn.
- **Replay-validation depth.** Is op kind + operand hash enough, or should CALL entries additionally verify
  the callee identity in full under `engine.traceValidateReplay`?
- **Pruned-branch observability.** Optionally annotate trace dumps with a per-function pruned-branch count,
  so unexpectedly missing control flow is easy to diagnose.
- **Probability-aware worklist ordering.** LIFO ignores the `traceBool` probability hint; exploring
  high-probability sides first could improve downstream block layout. Worth measuring, not blocking.
