# Loop & Control-Flow Identification for Better Backend Code

**Status:** Proposed
**Parent design:** [ir-pass-improvements.md](ir-pass-improvements.md) — this
document specifies the shared loop/control-flow *analysis* layer that §4.3-G
(LICM, issue #347) assumes ("extract the natural-loop recognizer from
`StrengthReductionPass` into a shared helper"), and extends it beyond pass
consumers to the four code-generation backends.
**Infrastructure baseline:** [ir-abstraction-milestone.md](ir-abstraction-milestone.md)
(`FunctionRewriter`, `Dominators`, verifier) — all prerequisites are merged.

---

## 1. Why this exists

Nautilus IR is deliberately unstructured: tracing emits plain basic blocks
wired by `br`/`if` terminators, and every trace of structured C++ control flow
(`for`, `while`, `break`, `continue`, `if/else` — see `docs/loops.md`,
`docs/control-flow.md`) is flattened into that CFG. The *structure* the user
wrote is known at trace time and then thrown away. Nothing downstream
rediscovers it:

1. **The one pass that needs loops hand-rolls its own matcher.**
   `StrengthReductionPass.cpp` implements `findSimpleLoopEdges`,
   `computeLoopBody`, `computeIvAliases` and `isLoopInvariant` privately
   (~250 lines), restricted to single-preheader/single-latch loops. Issue
   #347 already requires extracting this for LICM; any future unroller,
   vectorizer or loop-aware layout would re-invent it again.

2. **No backend knows where the loops are.** A grep for RPO/layout logic in
   `src/nautilus/compiler/backends/` comes back empty: every backend lowers
   blocks in discovery order (a DFS of the trace-shaped CFG). Loop bodies are
   not laid out contiguously, exit paths are not sunk, back edges get no
   special treatment.

3. **The only frequency hint that survives to a backend is the user's
   `val<bool>` probability.** `MLIRLoweringProvider.cpp:1083-1091` turns
   `IfOperation::getProbability()` into `cf.cond_br` branch weights — and
   that is the entire story. The far stronger *structural* signal ("this edge
   is a loop back edge, it is taken ~N times per entry") reaches no backend,
   including MLIR. bc/tbc/asmjit/cpp ignore probability entirely.

4. **Trace-time structure fields are already dead.** `IfOperation::mergeBlock`
   is never written by the compiler (`setMergeBlock` has zero call sites) and
   would be stale after `BlockMergingPass`/`EmptyBlockEliminationPass` anyway.
   Structure must be *recomputed* from the CFG, not trusted from tracing.

5. **The C++ backend emits `goto` soup.** `CPPLoweringProvider.cpp` lowers
   every block to a `Block_N:` label and every edge to `goto` — correct, but
   it forecloses `[[likely]]`/`[[unlikely]]` and `#pragma clang loop`
   annotations (which need structured statements to attach to) and makes the
   dumped sources near-unreadable for debugging.

The proposal: one reusable **analysis layer** (loops + acyclic regions +
static block frequencies + a layout order), one small **normalization pass**,
and then per-backend consumption. Analyses are read-only and recomputed on
demand, in the mold of the existing `Dominators` class; nothing is cached on
the `IRGraph`, so there is no invalidation protocol to get wrong.

## 2. Non-goals

- **No structured IR / region operations.** The IR stays a flat CFG; MLIR-style
  `scf.for` regions are out of scope (noted as a possible follow-up in §6.3).
- **No loop transformations beyond normalization.** LICM (#347), unrolling,
  vectorization are consumers of this layer, not part of it.
- **No profile feedback.** Frequencies are static heuristics seeded by the
  existing `val<bool>` probability hints.

## 3. The analysis layer

All new files live in `src/nautilus/compiler/ir/analysis/` (the existing
`Dominators.{hpp,cpp}` moves there in M1; `passes/` keeps re-exporting via an
include for one release if anything external includes it). Each analysis is a
value object constructed from a `FunctionOperation` with valid predecessor
lists (same contract as `Dominators`), and each must never mutate the graph.

### 3.1 `PostDominators`

Mirror of `Dominators` run on the reversed CFG. One wrinkle: Nautilus
functions can have multiple `ReturnOp` blocks, so the reverse CFG has a
virtual exit node that all return blocks (and, defensively, blocks with no
successors) connect to. Same iterative Cooper-Harvey-Kennedy algorithm, same
unreachable-block semantics as `Dominators` documents.

### 3.2 `LoopInfo`

Natural-loop forest over the dominator tree:

```cpp
class LoopInfo {
public:
	struct Loop {
		const BasicBlock* header;
		std::vector<const BasicBlock*> latches;   // sources of back edges into header
		const BasicBlock* preheader;              // unique out-of-loop pred outside header, or nullptr
		std::vector<const BasicBlock*> blocks;    // body incl. header, in RPO
		std::vector<std::pair<const BasicBlock*, const BasicBlock*>> exitEdges;
		Loop* parent;                             // nullptr for top-level loops
		std::vector<Loop*> subLoops;
		size_t depth;                             // 1 for top-level
	};

	LoopInfo(const FunctionOperation& fn, const Dominators& dom);

	[[nodiscard]] std::span<Loop* const> topLevelLoops() const;
	[[nodiscard]] const Loop* innermostLoopOf(const BasicBlock* b) const; // nullptr if not in a loop
	[[nodiscard]] size_t loopDepth(const BasicBlock* b) const;            // 0 if not in a loop
	[[nodiscard]] bool isBackEdge(const BasicBlock* from, const BasicBlock* to) const;
	[[nodiscard]] bool hasIrreducibleControlFlow() const;
};
```

Algorithm: a CFG edge `u → h` is a back edge iff `dom.dominates(h, u)`; the
natural loop of `h` is `h` plus everything that reaches any latch without
passing through `h` (backward walk over predecessors). Loops sharing a header
are merged (one `Loop`, several latches — this is exactly what `continue`
produces, see #365's latch-from-mid-body shape). Nesting falls out of body
containment; build inner-to-outer by processing headers in reverse RPO.

**Irreducibility:** trace-generated IR is reducible by construction (it comes
from structured C++), and no current pass can make it irreducible. Still,
detect it (a retreating edge in RPO whose target does not dominate its
source), set `hasIrreducibleControlFlow()`, exclude the offending SCC from
the forest, and let consumers degrade gracefully — same defensive posture
`Dominators` takes for unreachable blocks. The verifier gains an opt-in
`ir.verifyReducibility` check so the fuzzer would catch a pass ever
introducing this.

**First consumer (same milestone):** `StrengthReductionPass` drops its
private matcher and queries `LoopInfo` (keeping its extra "simple shape"
gates: unique preheader, single latch). Its existing tests gate the refactor
— behavior must be identical. This also discharges the first checkbox of
#347's LICM item.

### 3.3 `ControlFlowInfo` (acyclic regions + frequencies + layout)

Three orthogonal pieces, one constructor pass over the CFG, all consuming
`Dominators`/`PostDominators`/`LoopInfo`:

**(a) If-region classification.** For every `IfOperation` block `b`, the merge
point is the immediate post-dominator of `b`. Classify:

- `Diamond` — both arms are single-entry subgraphs that rejoin at the merge,
- `Triangle` — one arm branches straight to the merge (`if` without `else`),
- `Open` — everything else (an arm returns, breaks out of a loop, …).

This *recomputes* what `IfOperation::mergeBlock` was meant to be; that field
and its accessors are removed in the same PR (M2) — one source of truth.

**(b) Static block frequencies.** Entry block = 1.0. Propagate in loop-aware
RPO: conditional edges split by `IfOperation::getProbability()` (0.5 when the
hint is neutral), loop bodies are scaled by `kAssumedTripCount` (default 10,
option `ir.assumedLoopTripCount`) per nesting depth, exit-edge mass is what
leaves the loop. Deliberately a one-pass heuristic (no cyclic fixed point);
the point is ordering blocks and weighting spills, not exact counts.

**(c) Layout order.** `layoutOrder()` returns all reachable blocks in a
loop-aware RPO with cold sinking: bodies of each loop contiguous (header
first, latch last so the back edge is the short/adjacent jump), and
low-frequency blocks (loop exits, unlikely arms) placed after the hot spine.
This is *advice* — each backend decides what to do with it.

### 3.4 Where analyses run

Passes construct what they need inside `apply()` (as `StrengthReductionPass`
already does with `Dominators`). Backends construct them at the top of their
lowering provider — by the time `CompilationBackend::compile` runs, the IR is
final, and because the tiered compiler re-submits the *same cached* `IRGraph`
for tier-1 promotion (`IRGraph.hpp:38-41`), recomputing in the backend is
also the only staleness-proof option (stamped metadata would have to survive
arbitrary time between tiers). Construction is O(V+E) over graphs that are
typically tens of blocks; cost is noise next to any backend's own work
(measured guard: `irPasses.*`-style timing counters, §7).

## 4. `LoopNormalizationPass` (transform, opt-in `ir.enableLoopNormalization`)

Establishes the canonical shape most consumers want, using `FunctionRewriter`
primitives so block-argument/invocation arity invariants hold by construction:

1. **Dedicated preheader** — if the header has more than one out-of-loop
   predecessor (or the unique one is a conditional branch), insert a fresh
   block so the loop is entered by exactly one unconditional edge.
2. **Single latch** — if the header has several back edges (`continue`),
   merge them through one latch block that forwards the loop-carried
   arguments.
3. **Dedicated exits** — split any exit block that also has predecessors from
   outside the loop, so every exit block's predecessors are all inside.

Runs as its own `addPass` group *after* the existing fixed-point group in
`CompilationPipeline::compileToIR` (it wants the merged, folded CFG; running
it inside the group would fight `BlockMergingPass`, which would immediately
re-merge a preheader that ends in an unconditional branch — hence also: block
merging learns to skip blocks marked as loop preheaders is **not** done;
instead normalization runs last, and idempotence tests pin that a second run
changes nothing).

Cost note: normalization inserts blocks and unconditional branches. For the
dispatch-bound interpreters an extra `BR` per loop entry is one dispatch per
*entry* (not per iteration) — negligible — but the pass still ships opt-in
and is enabled by its consumers (LICM) or by measurement, matching the
`StrengthReductionPass` precedent.

## 5. Diagnostics (part of M2)

- `GraphVizUtil` gets loop clusters: blocks of a loop rendered as a subgraph,
  back edges in the already-reserved `loop` color (`GraphVizUtil.cpp:45`),
  headers/preheaders/latches labeled, block frequency in tooltips.
- `IRPrintOptions.showLoopInfo`: annotate block headers in text dumps with
  `// loop L1 depth=2 header` — flows into `dumpAfterEachPass` output.
- `CompilationStatistics`: `ir.loops`, `ir.maxLoopDepth`,
  `ir.irreducibleCfg`, and analysis-construction timings.

## 6. Backend consumption — what each backend gains

### 6.1 AsmJit (`backends/amsjit/`) — biggest winner

The only native backend with no optimizing middle-end behind it: whatever the
lowering emits is what executes.

- **Block layout** (`X64/A64LoweringProvider`): emit blocks in
  `layoutOrder()`. Loop bodies become I-cache-contiguous, the back edge
  becomes a short backward jump (statically predicted taken on x86), cold
  exits and unlikely arms move out of the hot path, and unconditional jumps
  to the next emitted block disappear.
- **Branch polarity**: with frequency info, invert `if` conditions so the
  likely successor is the fall-through — today polarity is whatever tracing
  produced.
- **Loop-header alignment**: 16-byte align headers of innermost hot loops
  (cheap, standard, currently impossible for lack of the "is a header" bit).
- **Spill weights**: scale the register allocator's spill cost by
  `loopDepth(block)` (or frequency) so loop-carried values keep registers
  and spill code lands outside loops. The diamond/loop merge handling that
  produced #321 gets a principled priority signal instead of block order.

### 6.2 Bytecode interpreter (`backends/bc/`)

Dispatch-bound: every executed instruction costs an indirect-branch dispatch
(`BCInterpreter.cpp:843`), so the wins are about *removing instructions from
loop bodies* and helping the host CPU's predictors.

- **Code layout + fall-through elision**: order blocks by `layoutOrder()` and
  skip the `BR` when the target is the next block in the stream — today block
  order is discovery order and every edge pays a dispatch. Eliding one `BR`
  in a loop body saves one dispatch *per iteration*.
- **Back-edge fusion guarantee**: the fused compare-and-branch
  superinstructions (`ByteCode.hpp:17`) exist precisely for the loop
  back-edge; loop info lets the lowering *verify* the latch terminator fuses
  and prioritize fusion there.
- **Register-move scheduling on back edges**: `BCLoweringProvider` already
  special-cases loop-carried argument moves (`BCLoweringProvider.hpp:22`);
  loop-carried-ness becomes a queryable fact instead of a lowering-order
  accident, and register assignment can prefer keeping loop-carried values
  in stable registers so back-edge `REG_MOV`s cancel.

### 6.3 MLIR / LLVM (`backends/mlir/`)

LLVM rebuilds dominators and loops itself, so structural info is *not* the
gift here — frequency and metadata are:

- **Back-edge branch weights**: extend the existing weight emission
  (`MLIRLoweringProvider.cpp:1083`) from user `if`-hints to structural
  weights on the latch's `cf.cond_br` ("stay in loop" ≫ "exit"). LLVM's
  BlockFrequency then drives its own layout, spill placement and
  if-conversion with real information instead of 50/50 defaults.
- **`llvm.loop` metadata hook**: identifying the latch branch is the missing
  prerequisite for attaching `llvm.loop.unroll.*` / vectorization hints; this
  design adds the identification and a small internal API, and a follow-up
  can surface user-facing hints (e.g. a `nautilus::loop_hint` on the tracing
  side) without new analysis work.
- **Deterministic emission order**: emit MLIR blocks in `layoutOrder()`
  instead of recursion order — no correctness change, but stable, readable
  dumps and fewer spurious diffs in `test/llvm-ir-test/` going forward.
- *(Follow-up, explicitly out of scope: raising counted loops to `scf.for`,
  which would unlock MLIR-level unroll/vectorize passes.)*

### 6.4 C++ backend (`backends/cpp/`)

The host compiler rebuilds the CFG from gotos, so runtime wins come only from
annotations — but those need structure to attach to:

- **Structured re-emission**: where `ControlFlowInfo` proves a region is a
  `Diamond`/`Triangle` and `LoopInfo` gives a single-entry loop, emit real
  `while (true) { ... }` / `if/else` statements; fall back to label+`goto`
  for `Open` regions. (Every traced program is reducible, so coverage is
  near-total in practice.)
- **Hints**: `[[likely]]`/`[[unlikely]]` from probabilities and
  `#pragma clang loop` on emitted loops — syntactically impossible today.
- **Debuggability**: the generated source (the backend's stated purpose is
  debugging/reference) becomes structured and diffable; loop comments carry
  header/depth info.

### 6.5 TBC (`backends/tbc/`)

Same wins as bc (layout, fall-through elision, back-edge fusion) with one
extra: TBC executes a single flat instruction stream, so `layoutOrder()`
directly controls the memory layout the dispatch loop walks — its README
already lists "instruction-stream prefetch hints / handler layout
experiments" as blocked future work; this unblocks it.

### 6.6 IR passes (cross-cutting)

`LoopInfo` is the stated prerequisite for LICM (#347) and lets
`StrengthReductionPass` later generalize past single-latch loops;
`ControlFlowInfo` frequencies give any future pass (e.g. select-vs-branch
conversion) a cost model.

## 7. Delivery plan — one PR per milestone

| # | Deliverable | Gate |
|---|---|---|
| M1 | `analysis/` directory; `PostDominators`; `LoopInfo`; `StrengthReductionPass` refactored onto it | new unit tests; StrengthReduction suite byte-identical; fuzz smoke |
| M2 | `ControlFlowInfo` (regions, frequencies, layout order); remove `IfOperation::mergeBlock`; GraphViz/dump/statistics integration | unit tests incl. break/continue/nested fixtures; dump goldens |
| M3 | `LoopNormalizationPass` (opt-in) | verifier clean with `ir.verifyAfterEachPass`; idempotence; fuzz soak with flag on |
| M4a | AsmJit: layout + alignment + polarity + spill weights | differential fuzz; ExecutionBenchmark loop kernels — target ≥ 5% on loop/pointer kernels, no kernel −2% |
| M4b | bc + tbc: layout + fall-through elision + back-edge fusion check | same harness; no interpreter kernel −2% |
| M4c | MLIR: back-edge weights, emission order, `llvm.loop` hook | `llvm-ir-test` references updated once, with rationale in the PR |
| M4d | C++: structured emission + hints (fallback to goto) | differential fuzz across all cpp kernels; goldens for dumped sources |
| M5 | Measurement write-up; default-on decision for M3/M4 flags | benchmark sweep, ≥ 5 runs, medians, recorded in pass headers |

Every M4 backend change ships behind its own option (default on for pure
layout/weights, which cannot change semantics; the cpp structured emitter
starts opt-in) and is fuzzed with the differential harness — which, after
#365, generates exactly the multi-exit/multi-latch loop shapes these analyses
must classify.

## 8. Testing strategy

- **Fixtures**: extend the IR fixtures with hand-built graphs: counted loop;
  `while` with `break` (two exit edges); `continue` (two latches pre-merge);
  triply nested loops; diamond inside a loop; loop whose exit returns
  directly; self-loop (single-block); unreachable loop remnant. Assert the
  full forest (headers, latches, preheaders, depths, exits) and region
  classifications.
- **Property tests via fuzzer**: with `ir.verifyAfterEachPass` +
  reducibility check on, soak the AST fuzzer; add an invariant check that
  `layoutOrder()` is a permutation of reachable blocks and every loop body is
  contiguous in it.
- **Differential correctness**: all M4 changes run the existing
  all-backends-vs-native-oracle harness; any disagreement is a pinned
  regression test (house rule from #321/#358).
- **Performance**: `ExecutionBenchmark` loop kernels on bc/tbc/asmjit,
  Release, Clang 21, ≥ 5 runs, medians — the same protocol #347 specifies.

## 9. Risks

- **Layout is a semantic no-op but a reference-file churn.** MLIR emission
  order changes rewrite `test/llvm-ir-test/reference-ir/` wholesale — done
  once in M4c, isolated in that PR.
- **Frequency heuristic misfires** (e.g. rarely-taken loop assumed hot):
  mitigated by conservative `kAssumedTripCount`, user probability hints
  taking precedence, and layout only reordering — never duplicating — code.
- **Normalization vs. BlockMerging tension** (§4): resolved by pipeline
  position + idempotence tests, not by cross-pass special cases.
- **cpp structured emitter correctness**: the riskiest M4 item (it re-derives
  syntax from the CFG); hence opt-in, goto fallback for anything not
  provably structured, and differential fuzz before any default flip.
