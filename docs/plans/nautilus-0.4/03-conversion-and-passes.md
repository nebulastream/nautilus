# Component: Optimizing Conversion and Passes

Part of the [Nautilus 0.4 design](README.md) · Milestone M3

## 1. Purpose

The conversion turns pre-SSA NIR into SSA-level NIR. In 0.4 it also does
the cleanup that today's fixed-point pass pipeline does, so the pipeline
leaves the critical path. Legalization prepares SSA-level NIR for a specific
backend. Expensive optimizations remain available as optional passes.

## 2. Why

P0 ([p0-baseline.md](p0-baseline.md)):

| Code shape | IR passes' share of the compile |
|---|---|
| Linear code on `main` | **62–68%** |
| Linear code with #561 | **44–53%** (the largest stage) |
| Branchy code (`main` and #561) | 10–59% (`chainedIf` 23% / 10%, `chainedIfRegion` 59% / 35%) |

On `main`, building `FunctionRewriter` tables is 44% of all instructions,
and most pass runs change nothing (#561's pass statistics). The pipeline
runs up to `ir.maxPipelineIterations` (4) rounds of:
- constant folding and copy propagation;
- algebraic simplification;
- constant branch folding;
- empty-block elimination;
- block merging;
- DCE;
- argument pruning.

Most of this cleans up artifacts of tracing that a builder can avoid
creating.

### 2.1 Where each 0.3 pass goes

The default pipeline (`CompilationPipeline.cpp`, `addOptimizationPasses`
and `compile`) runs `FunctionAttributeInference`, then the fixed-point group
above (with `LocalCSE` and `StrengthReduction` opt-in), then `LICM`
(opt-in), `NoThrowInference`, `ExceptionRegionPreparation`, and
`IRLocationPass` when `debug` or `perf` is set. A backend can ask for
less through `IROptimizationLevel`: MLIR asks for `ArgumentPruning` (only
`BlockArgumentPruningPass`, because LLVM does the rest), and `None` runs no
cleanup. In 0.4 the conversion's cleanup is the same for every backend, so
the level disappears; what MLIR needed from pruning is the dead-value
sweep (§3.3). Every pass in `compiler/ir/passes/`, and the helpers next to
it in `compiler/ir/`, gets a home in 0.4:

| 0.3 pass or helper | 0.4 home |
|---|---|
| `ConstantFoldingAndCopyPropagationPass`, `AlgebraicSimplificationPass` | builder rules during emission (§3 step 2; M3.2) |
| `ConstantBranchFoldingPass`, `EmptyBlockEliminationPass`, `BlockMergingPass` | live-edge emission and threading (§3 steps 1 and 5, §3.1; M3.3) |
| `LocalCSEPass` | CSE during emission, block-local on by default (§3 step 3, §3.4; M3.4) |
| `DeadCodeEliminationPass`, `BlockArgumentPruningPass` | dead-value sweep at compaction (§3.3) and trivial-phi folding (§3 step 2) |
| `StrengthReductionPass`, `LoopInvariantCodeMotionPass` | optional copy passes (§5; M3.8) |
| `FunctionAttributeInferencePass`, `NoThrowInferencePass` | attribute pre-pass over all pre-SSA bodies (§3.5; M3.6) |
| `ExceptionRegionPreparationPass` | landing pads during emission (§3 step 7; M3.5) |
| `IRLocationPass` | the NIR printer's side output: with MLIR debug info on (`debug` or `perf`), printing returns an instruction → line map of the NIR dump (M4.5, M4.8) |
| `Dominators` | Cooper–Harvey–Kennedy over the trace CFG inside the conversion (§3 step 3) |
| `LoopInfo` | the derived loop table filled by the conversion (§3 step 6, §3.1) |
| `FunctionRewriter` | removed: passes are copy passes, real uses come from a CSR table built on demand ([02 §4](02-nir.md)) |
| `IRPassManager` | the runner of optional and plugin passes (§5) |
| `IRVerifier` | the NIR verifier ([13 §6.4](13-nir-spec.md); M1.2) |
| `IRStatistics` | NIR op and block counts in the statistics ([10 §3](10-observability.md)) |
| `IRPass` (interface) | the copy-pass interface of optional and plugin passes (§5) |
| `OperationEffects` (`isPureOp`, per-op effects) | the per-opcode effect table of 13 §5.0 and `OpInfo.effects` for extension ops, used by §3.3 and §3.4 |
| `Usages` | the CSR use table built on demand ([02 §4](02-nir.md)) |
| `IRLocationMap` | the instruction → line map that the printer returns (row `IRLocationPass`) |

## 3. The optimizing conversion

Today's `TraceToIRConversionPhase` already converts in one linear pass with
dense tables and Braun-style SSA construction on the complete trace CFG.
0.4's conversion is a new component (`conversion/`) that reads pre-SSA NIR
and keeps that approach, extended into an optimizing builder. It is tested
on its own until the cut-over ([16 §2](16-implementation-plan.md)):

1. **Visit blocks in a loop-nest-aware RPO** (§3.1), predecessors before
   successors except along back edges:
   - a block is *live* once a live edge enters it, and dead blocks are
     never emitted;
   - a `condbr` on a value that folded to a constant adds only the taken
     edge, so constant branch folding and unreachable-block removal happen
     during conversion;
   - back edges come from latches converted after the header, which is when
     Braun sealing closes the header; the sealing rule is §3.1, and reads in
     an unsealed header are limited by §3.2;
   - a `var` record ([13 §5.9](13-nir-spec.md)) defines a new variable and
     emits nothing: the conversion reads its operand (a constant, or the
     current value of the operand's variable in that block, resolved
     through `forward[]`) and writes that value as the new variable's
     current value in the block, exactly as an `assign` writes its
     target. A later `assign` to the source does not change it, which is
     what makes a copy a copy ([01 §6](01-tracing-frontend.md) walks
     through an example).
2. **Fold and simplify on emission.** The rules of today's
   `ConstantFoldingAndCopyPropagation` and `AlgebraicSimplification` become
   builder rules, along with the rules of extension ops
   ([04](04-extensions.md)). A block argument that every edge passes the same
   value for (or only itself) is folded into `forward[]`, as Braun's
   algorithm already does.
3. **CSE:**
   - an open-addressing table over `(op, type, a, b, c)` with an undo log;
   - **block-local first**, then scoped along the dominator tree (a dominator
     tree over the trace CFG is cheap: Cooper–Harvey–Kennedy);
   - loads and calls key their entries on the current effect id, so memory
     order is kept without alias analysis; §3.4 defines when the effect id
     changes;
   - a call is a CSE candidate only with `noArgWrite` and `noOtherWrite`
     (`readonly` or `pure`), `willReturn` and `noUnwind`
     ([13 §4.6.2](13-nir-spec.md), [06 §3.3](06-semantics.md)).
4. **Use counts while emitting.** `useCount` is maintained as instructions
   are emitted and forwarded. The dead-value sweep of §3.3 runs during the
   final compaction and removes dead values transitively, including dead
   loop-carried variables, so the fast path needs no DCE pass.
5. **Threading.** A trace block with a single predecessor and an
   unconditional jump is appended to its predecessor's NIR block (block
   merging and empty-block elimination). Jump-only trace blocks are already
   threaded today.
6. **Layout.** The conversion emits blocks in final layout order (the order
   of §3.1: each reducible loop's blocks contiguous) and fills a derived
   loop table (loop depth and loop end per block; `BlockRecord` holds
   neither, [02 §3.6](02-nir.md)). Loop depth is computed after emission
   from the live back edges, not from the trace CFG. The tracer already
   marks control-flow merge blocks (loop-header candidates) and knows the
   back edges.
7. **Exceptions.** The work of today's `ExceptionRegionPreparationPass`
   happens during emission: the pre-SSA level's cleanup lists
   ([13 §3.4](13-nir-spec.md)) become landing pads, keyed by their ordered
   destructor set and shared, and unwinding calls point at them
   ([02 §3.4](02-nir.md)). A pad holds one call per cleanup-list entry, in
   list order, of the destructor callee with the object's address; each
   has `NO_CLEANUP` (or `NO_UNWIND` for a `noUnwind` destructor). The pad
   ends in `resume`, or in a `br` to another pad that holds a shared tail
   of the list ([13 §6.3](13-nir-spec.md)). An empty cleanup list gives `NO_CLEANUP` and no pad.
   Integer `div`/`rem` carry cleanup lists like calls
   ([01 §6](01-tracing-frontend.md)); when folding proves the divisor safe,
   the check and the unwind entry are dropped
   ([13 §5.10](13-nir-spec.md)).
8. **Attributes.** The analyses of `NoThrowInferencePass` and
   `FunctionAttributeInferencePass` run as a pre-pass over the pre-SSA
   bodies of all functions of the module, before any function is converted
   (§3.5). From M6 on, declared effects and `nothrow` annotations
   ([06](06-semantics.md)) take precedence where present.

### 3.1 Order, sealing and irreducible regions

The trace CFG is complete before conversion starts, so the order and every
block's trace predecessors are known up front. Only liveness is discovered
during emission.

- **Order.** A loop-nest-aware RPO (Bourdoncle's weak topological order,
  computed iteratively) makes each reducible loop's blocks contiguous,
  which plain RPO does not guarantee. A *back edge* is an edge whose target
  dominates its source (dominators from Cooper–Harvey–Kennedy over the
  trace CFG).
- **Sealing rule.** A block is sealed when the traversal has passed the
  last of its trace predecessors in the order, whether that predecessor was
  emitted or skipped as dead. Only edges from live predecessors count; a
  predecessor that never became live contributes nothing. So:
  - a non-header block is sealed when it is visited, because all its
    forward predecessors precede it;
  - a loop header is sealed after the last block of its loop has been
    visited, with edges only from live latches;
  - if no latch is live, the header is no longer a header: its incomplete
    phis resolve to the preheader values, and the loop table does not
    count it (step 6).
- **Irreducible regions.** A cycle with a retreating edge whose target does
  not dominate its source (possible after merges at arbitrary operations
  and `region()` boundaries; 0.3's `LoopInfo` skips such loops) is an
  irreducible region. Its blocks keep the plain RPO order, get loop depth 0
  and no loop end, and are exempt from the contiguity rule
  ([13 §6.3](13-nir-spec.md)). Liveness inside the region is decided
  conservatively: once any entry of the region is live, every block of the
  region is treated as live, because a block can be entered through a
  retreating edge from a later entry. Blocks that turn out unreachable are
  dropped by the reachability step of the compaction (§3.3). Braun's
  construction needs no change: an entry block is sealed by the rule above,
  so reads in it before the retreating predecessor is visited create
  incomplete phis (for the variables of the region's assigned set, §3.2).
  A block that is treated as live but has no live incoming edge when it is
  sealed (possible only through this conservative rule) resolves every
  variable read to a zero constant of its type; such a block is
  unreachable and the compaction drops it with those placeholders.
- **No recursion.** 0.3's conversion recurses (`processBlock` →
  `processJump`/`processBranch` → `processBlock`), so its stack depth grows
  with the block count. The new traversal walks the precomputed order with
  an explicit worklist, and Braun's variable lookup across predecessors
  (`readVariableRecursive`) uses an explicit stack too.

### 3.2 Loop-carried variables and the refold sweep

In RPO a loop header is visited before its latches and cannot be sealed.
Under plain Braun, every read of a variable in the loop creates an
incomplete phi, and the users of that phi are folded and CSE'd with the phi
as an opaque operand. A phi that turns out trivial at sealing (the
variable was only copied or reassigned with the same value) would leave
those users unfolded. 0.3 recovers this with `removeTrivialPhis` and up to
four pass rounds; 0.4 does it in the conversion:

- **Loop-assigned sets.** Before emission, for each loop header (and each
  irreducible region), the conversion computes the set of variables that
  are the target of an `assign` in the loop's blocks (the natural loop of
  its back edges, or the region). One linear scan over the pre-SSA blocks
  does this for all loops.
- **Reads in an unsealed header** create an incomplete phi only for
  variables in that set. Every other variable is not loop-carried, and its
  read resolves through the forward predecessors (the preheaders), which
  are already sealed.
- **Refold sweep.** Phis that still become trivial at sealing are
  recorded. If there are any, a bounded refold sweep runs after the header
  is sealed. Only the loop's blocks can hold users that were emitted with
  the phi as an opaque operand (blocks after the loop are not emitted yet
  and see the forwarded value), and they are contiguous in layout (§3.1),
  so the sweep builds a CSR use table for that range only. It marks the
  users of the removed phis in a bitset over the range, scans the range
  once forward, reapplies the fold and simplify rules (not CSE: the scoped
  CSE entries of those blocks are gone) to each marked instruction, and
  marks the users of each instruction that changed. Since definitions
  precede their non-phi uses in layout, one scan sees every operand change
  of an instruction before it reaches it: each instruction is visited at
  most once per sweep, and the sweep never iterates to a fixed point (a
  phi that becomes trivial only through the sweep is folded by Braun's
  usual phi-user recursion, but its users are not revisited). The cost is
  O(loop size) per sealed header with trivial phis, O(n × loop depth) per
  function at worst.
- **Branches the sweep folds.** A `condbr` that folds to a constant becomes
  a `br`, and the dropped edge leaves the live-edge set. A block not yet
  visited (a loop exit) that loses its last live incoming edge is dead and
  never emitted; blocks inside the loop that become unreachable, and their
  incoming branch arguments, are dropped by the compaction (§3.3).
- **Statistics:** `conversion.refoldSweeps` and
  `conversion.refoldedInsts` per function. The corpus gets a case for a
  loop-invariant variable reassigned in the body.

### 3.3 Dead-value sweep at compaction

The final compaction ([02 §4.1](02-nir.md) item 5), which applies `forward[]` and
renumbers ids, also removes dead code. It is O(n) and runs in the copy the
conversion already makes:

1. **Reachability.** Blocks not reachable from the entry block through live
   edges (left by the refold sweep or by the conservative liveness of an
   irreducible region) are dropped, along with their incoming branch
   arguments.
2. **Mark from roots.** Use counts alone cannot remove a dead
   loop-carried variable: a counter assigned in the body and never read
   after the loop forms a phi → `add` → branch argument → phi cycle, and
   the phi has a real use (the `add`), so neither a use-count cascade nor
   a "used only as a branch argument" test removes it. The sweep therefore
   marks live values from roots, in one worklist:
   - **roots** are the instructions with an effect (store, load, call,
     checked division, extension op whose `OpInfo.effects` are not empty;
     as in 0.3's `isPureOp`, loads are kept), every terminator, and the
     condition of every `condbr` and the operand of every `ret`; a call
     with all of bits 0–5 ([13 §4.6.2](13-nir-spec.md)) is not a root;
   - marking an instruction marks its operands, except branch arguments;
   - marking a `blockarg` marks, for each live incoming edge, the branch
     argument at its position in the predecessor's terminator.
3. **Sweep.** Unmarked instructions are dropped; unmarked `blockarg`s are
   removed with the branch arguments at their positions. This removes dead
   chains (0.3's cascading DCE) and dead loop-carried cycles (0.3's
   `BlockArgumentPruningPass` rule 1) in one O(n) pass, before the
   renumbering.

The `.nir` tests check that dead loop-carried variables and chains of dead
values are absent from the output (M3.3).

### 3.4 Effect ids

CSE of memory-dependent values (loads, calls with read effects, extension
ops with read effects) is keyed on an **effect id**, a counter that
changes:

- at every instruction that may write memory: a store, a call without both
  `noArgWrite` and `noOtherWrite`, an extension op whose
  `OpInfo.effects` include a write;
- **at the entry of every block with more than one live predecessor, of
  every loop header** (sealed or not), **and of every entry of an
  irreducible region** (§3.1): its retreating predecessor is not visited
  yet when the entry is, so its live-predecessor count cannot be trusted
  at that point.

A block with a single live predecessor starts with the effect id at the
end of that predecessor.

Memory-dependent entries match only within the same effect id, so they
never cross a merge or a back edge: a load after an if/else whose one arm
stores, or a load in a header whose body stores, is not merged with an
earlier load. Values that read no memory are scoped along the dominator
tree without this restriction. Dominators of the trace CFG are safe for the
live CFG, because removing dead edges only adds dominance. M3.4 adds a
`.nir` negative test for both cases.

### 3.5 Attribute pre-pass

The conversion emits unwind entries and landing pads (step 7) and merges
or hoists calls (step 3) while it converts a caller, so a callee's
`noUnwind` and effect bits must be final before any caller is converted.
`NoThrowInferencePass` is a whole-module optimistic fixed point, and
`FunctionAttributeInferencePass` reads callee bodies (loops, loads, stores,
calls) and pessimizes cycles. Neither fits "inside the conversion, per
function". Instead:

- **When.** The whole module is traced before conversion (as today:
  `TraceContext` drains `functionsToTrace` first), so one analysis runs
  over the pre-SSA bodies of all its functions before any of them is
  converted.
- **Per function:** may-read, may-write and may-throw from its
  instructions and its callees' records, and may-loop from cycles in the
  trace CFG (which clears `willReturn`). At the pre-SSA level every integer
  `div`/`rem` is checked (13 §5.10), which would make every function that
  divides by a literal may-throw, unlike 0.3. The pre-pass therefore
  treats a `div`/`rem` as non-throwing when its divisor is a safe constant
  (13 §5.10) or a `var` of one that is never the target of an `assign` in
  the function (one scan for the assigned set); the conversion clears the
  checked bit of exactly those, and possibly more after folding.
- **Across functions:** a fixed point over the strongly connected
  components of the call graph, optimistic for `noUnwind` (as 0.3's
  `NoThrowInferencePass`) and pessimistic for memory effects and
  `willReturn` on cycles (as 0.3's `FunctionAttributeInferencePass`).
- **Results** go into the internal callee records. Declarations (M6.2)
  override inference; imported callees take their bits from the
  `ImportRecord`.
- **Soundness.** The facts come from the unfolded body, so they may be more
  pessimistic than the folded body, never less. That costs optimization,
  not correctness, as in 0.3.

## 4. Legalization (NIR → NIR)

Before a backend runs, one copy pass prepares SSA-level NIR for it:

- extension ops without a handler on that backend expand to their
  fallback (a native call or an expansion into core ops);
- extension types without support become their memory fallback (stack
  slots);
- calls are lowered to the target ABI where the backend asks for it.

There is no separate low-level IR in core. Each backend keeps its own
internal form below this point ([05](05-backends.md)).

## 5. Optional and plugin passes

- **Optional passes:** LICM, strength reduction and **inlining**, rewritten
  as copy passes ([02 §4](02-nir.md)). They are off on latency-critical
  compiles and enabled per module. Inlining works on internal and imported
  callees, across modules ([13 §4.4.3](13-nir-spec.md)); inlining of
  imported callees lands with linking in M4.
- **Plugin passes** register at named points (`afterConversion`,
  `beforeLowering`), each marked *required* (correctness, for example the GPU
  partition pass of [17 §4.5](17-gpu.md)) or *optional*. Only required passes run on
  latency-critical compiles, so a plugin cannot silently put a fixed-point
  pass back on the critical path.
- **Analyses** come from `BlockRecord`, the derived loop table and
  `useCount`; plugins do not recompute them.
- The options `ir.runOptimizationPasses` and `ir.disable*` keep their meaning
  for the optional passes. Options for passes that no longer exist are
  accepted with a deprecation warning.

## 6. Exit criteria (M3)

- No IR pass runs on a default compile (`irPasses.pipelineIterations` = 0,
  checked exactly), and the conversion itself costs at most 0.25 µs/op on
  linear corpus code (0.3: IR generation 0.14–0.19 µs/op plus passes
  0.6–1.7 µs/op on `straight` and `loopBody`, [p0](p0-baseline.md)).
- IR op counts at most those of 0.3 after its full pipeline, on every
  corpus function and on TPC-H (nautilus-duck) kernels.
- Differential fuzzer clean, including merge-heavy corpora: regions, nested
  loops, `break`, irreducible regions from merges at arbitrary operations.
- Dead loop-carried variables and chains of dead values absent from the
  output; loop-invariant variables reassigned in a loop body fold as in
  0.3 (§3.2, §3.3).

## 7. Risks

- **CSE correctness.** Block-local CSE ships first. Dominator-scoped CSE
  ships only after dedicated fuzzing. CSE is off in debug builds that need
  per-call-site provenance.
- **Folding changes which blocks exist.** Braun sealing must not wait on
  predecessors that were folded away. The sealing rule of §3.1 counts
  trace predecessors by position, not by arrival, so a dead predecessor
  cannot block it; merge-heavy tests cover it.
- **Irreducible regions** are rare and get conservative liveness and no
  loop table entry (§3.1). If corpus or NebulaStream code produces many,
  their code quality is measured before anything more is built.
- **Refold sweeps** are bounded per sweep but can run once per sealed
  header. `conversion.refoldSweeps` shows whether they matter on real
  code.
- **Code quality on backends that relied on the pipeline** (`tbc`
  dispatch counts). Compare executed instruction counts in
  `ExecutionBenchmark` and keep an optional per-backend pass list where it
  pays.
