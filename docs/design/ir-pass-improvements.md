# Design: IR Optimization Passes for Direct-Lowering Backends

**Status:** Proposed
**Audience:** Nautilus contributors; implementation agents (see the companion
[agent playbook](ir-pass-improvements-agent-playbook.md))
**Scope:** `nautilus/src/nautilus/compiler/ir/passes/` and the pass pipeline in
`LegacyCompiler::compileToIR`

---

## 1. Motivation

Nautilus has five backends with two fundamentally different relationships to
the Nautilus IR:

| Backend | Downstream optimizer | Consequence of a redundant IR op |
|---|---|---|
| `mlir` | Full MLIR + LLVM pass pipeline | Usually optimized away for free |
| `cpp` | Host C++ compiler (`-O2`) | Usually optimized away for free |
| `asmjit` (x64/a64) | Only a small post-RA peephole | One or more machine instructions + a vreg, forever |
| `bc` | None | One interpreter dispatch **per execution** |
| `tbc` | None | One interpreter dispatch **per execution** |

For `asmjit`, `bc`, and `tbc`, the IR pass pipeline **is the entire
optimizer**. Every operation that survives `compileToIR` is lowered 1:1:

- `bc`/`tbc` lowering walks `block->getOperations()` and emits an instruction
  per op (`TBCLoweringProvider::processBlock`, `BCLoweringProvider`
  equivalents). A dead pure op is not just wasted code size — it is a wasted
  dispatch on every call and every loop iteration.
- In `tbc`, every `Const*Operation` occupies a slot in the function's
  *constant image*, which is `memcpy`'d into the frame **on every call**
  (`TBCCode.hpp`). Dead constants make every invocation slower.
- Block-argument edges lower to parallel-copy `MOV`s at every block
  transition (`bc`/`tbc` coalescing minimizes but cannot eliminate them;
  asmjit materializes merged values similarly). A redundant block argument is
  a redundant `MOV` per loop iteration.
- `asmjit` has no mid-end: IR op count maps directly to instruction count and
  register pressure; its free-list allocator spills when pressure grows.

The cost model for the interpreters is **dispatch-bound**: dynamic executed-op
count dominates wall clock (see `backends/tbc/README.md`). The cost model for
`asmjit` is conventional (instructions + register pressure). Both are served
by the same lever: **fewer, more canonical IR operations reaching the
backend.**

### Evidence this is the current bottleneck

- `StrengthReductionPass` is *correct* but ships **default-off** because it
  leaves behind an unfolded `ptr + 0` and a dead cast, which regress the
  dispatch-bound interpreters (`StrengthReductionPass.hpp`,
  `LegacyCompiler.cpp:171-177`). The pass is blocked on cleanup passes that
  don't exist yet.
- `ConstantFoldingAndCopyPropagationPass` replaces the folded op in place, but
  the *feeding* constants frequently become dead and are still lowered (and,
  in `tbc`, still copied per call).
- The `tbc` README's follow-up list ("dead-MOV elimination after immediate
  folding") is a backend-local symptom of a missing IR-level DCE.
- Tracing produces structurally noisy IR by construction: one block per
  control-flow merge, pass-through block arguments for every live value, and
  repeated subexpressions the trace could not deduplicate across blocks.

## 2. Current state (inventory)

Infrastructure that already exists and that new passes must build on:

- **Pass framework** — `IRPass` (`apply(IRGraph&)`, `getName()`),
  `IRPassManager` (registration order, option-driven verification/dumping,
  per-pass timing + block/op deltas via `CompilationStatistics`), predecessor
  lists rebuilt before each run (`rebuildPredecessorLists`).
- **Passes** — `ConstantFoldingAndCopyPropagationPass` (default on),
  `EmptyBlockEliminationPass` (default on), `StrengthReductionPass`
  (opt-in via `ir.enableStrengthReduction`).
- **Analysis** — `IRVerifier` (structural invariants incl. stale-pointer
  detection, #327), `IRStatistics` (block/op counts), `Usages::countUsages`
  (per-function static use counts; already consumed by the asmjit branch
  fusion).
- **Op metadata** — `OperationProperties.hpp`: constant-time per-optype flags
  `Constant / Terminator / MayReadMem / MayWriteMem / Pure`. This is the
  purity oracle for DCE/CSE/LICM.
- **Generic rewiring** — `Operation::getInputs()` + `Operation::setInput()`
  span-based operand access across all op kinds; `BasicBlock::replaceSuccessor`
  and invocation helpers keep the predecessor invariant.
- **Test surface** — `test/ir-pass-tests/` (Catch2 unit tests over
  programmatically built graphs via `IRGraphFixtures.hpp`), the whole
  execution/val suite parameterized over all backends
  (`test/common/ExecutionTest.hpp::availableBackends()`), the differential
  AST fuzzer (`test/fuzz/`, behavioral cross-backend equivalence), and
  `test/benchmark/ExecutionBenchmark.cpp`.

### Gaps

1. No **dead code elimination** — the single biggest omission; everything
   else produces garbage that currently cannot be swept.
2. No **algebraic simplification / canonicalization** (`x+0`, `x*1`, `x*0`,
   same-type casts, `not(not x)`, constant-to-the-right canonicalization…).
3. No **constant branch folding** (`if(constBool)` survives as a conditional
   branch even after folding proves the condition) and no **unreachable block
   elimination**.
4. No **block merging** — `EmptyBlockElimination` only removes *fully empty*
   blocks; a single-predecessor block reached by an unconditional branch still
   costs a `JMP` dispatch + edge copies per execution.
5. No **block-argument pruning** — unused and pass-through-invariant
   arguments survive as per-iteration `MOV`s.
6. No **CSE**, no **LICM**.
7. `IRPass::apply` returns `void`, so the manager cannot iterate pass groups
   to a fixed point or skip work when nothing changed.
8. `IRVerifier` does not check invocation-argument arity against target-block
   argument arity — an invariant the new block-arg passes will stress.

## 3. Goals / non-goals

**Goals**

- Reduce the *dynamic* op count reaching `bc`/`tbc` and the static op count /
  register pressure reaching `asmjit`, measurably, on the existing benchmark
  kernels and on trace-shaped IR generally.
- Unblock `StrengthReductionPass` becoming default-on.
- Keep every pass idempotent, composable, verifier-clean, and option-gated,
  matching the existing framework contract (`IRPass.hpp`).
- Keep `mlir`/`cpp` correctness unaffected (they run the same pipeline; the
  passes must be semantics-preserving everywhere).

**Non-goals**

- No new IR operation kinds (e.g. a GEP/load-with-offset op). Backend-local
  superinstruction fusion (tbc `LOAD_off`/`STORE_off`) stays in the backends.
- No changes to backend register allocators or dispatch machinery.
- No inlining (ProxyCall inlining is a separate, larger design).
- No full sparse-conditional constant propagation; the folding pass stays
  syntactic.

## 4. Design

### 4.1 Guiding principles

1. **Purity comes from one place**: `OperationProperties.hpp`
   (`isPureOp` / `mayReadMemory` / `mayWriteMemory`). No pass hand-rolls an
   op-type switch to decide side effects.
2. **Rewiring comes from one place**: a new shared utility (§4.2) built on
   `Operation::setInput` + terminator invocation arguments, extracted from the
   logic that already exists inside the constant-folding pass. Bug #327
   (stale consumer pointers) must be impossible to reintroduce per-pass.
3. **CFG invariants are maintained, not rebuilt**: passes use
   `replaceSuccessor` / `BasicBlockInvocation::setBlock` /
   `add|removePredecessor` so `getPredecessors()` stays truthful mid-pipeline.
   The verifier (run with `ir.verifyAfterEachPass` + `ir.failOnVerifyError`
   in all pass tests) is the enforcement mechanism.
4. **Every transformation is measured before it defaults on**, on all three
   target backends — the StrengthReduction lesson: a locally "obvious" win can
   regress a dispatch-bound cost model. Benchmarks gate defaults; options gate
   rollback.

### 4.2 Infrastructure changes (Phase 0)

> **Expanded into its own milestone.** Items (c)–(e) below, plus safe
> mutation primitives the pass catalog assumes (use tracking, safe erase,
> fresh-identifier minting, atomic block-argument mutation, CFG rewiring)
> and a fuller set of verifier invariants, are specified with concrete
> requirements and acceptance tests in
> [ir-abstraction-milestone.md](ir-abstraction-milestone.md) (Milestone
> IR-0). That milestone lands **first**; items (a)–(b) below follow it as a
> separate small PR. Where this section and the milestone doc overlap, the
> milestone doc is authoritative.

**(a) `IRPass::apply` returns `bool` (changed).**
All three existing passes already track an internal `changed` flag for their
fixed-point loops; surface it. The manager uses it to skip per-pass dumps and
verification when nothing changed, and to drive (b).

**(b) Fixed-point pass groups in `IRPassManager`.**
Add `beginFixedPointGroup(maxIterations = 4)` / `endFixedPointGroup()` (or an
`addFixedPointGroup(std::vector<std::unique_ptr<IRPass>>, size_t)` — pick the
simpler to implement). A group re-runs while any member reports a change,
bounded by `maxIterations` (option `ir.maxPipelineIterations`, default 4).
Rationale: the cleanup passes enable each other (branch folding exposes
unreachable blocks; block merging exposes dead args; DCE exposes empty
blocks). Statistics keep per-pass attribution; add a
`irPasses.pipelineIterations` counter.

**(c) Shared mutation facade** — `passes/FunctionRewriter.{hpp,cpp}`: a
per-function mutation session owning use tracking, `replaceAllUses`
(covering operand spans **and** `BasicBlockInvocation` arguments), safe
erase with a dead-chain cascade, fresh-identifier minting, atomic
block-argument mutation, and CFG rewiring. Fully specified in the milestone
doc (§3.2 there); the pass catalog below refers to its primitives by name.

**(d) Dominance helper** — `passes/Dominators.{hpp,cpp}`: iterative
dominator-tree computation (Cooper–Harvey–Kennedy over reverse post order).
Functions here are small (tens of blocks); simplicity beats asymptotics.
Needed by block-argument pruning and LICM. Exposes
`bool dominates(const BasicBlock* a, const BasicBlock* b)` plus RPO order.

**(e) Verifier extension**: every `BasicBlockInvocation`'s argument count must
equal its target block's argument count. Cheap, and it turns the classic
block-arg-pruning bug into an immediate, pinpointed failure instead of a
backend miscompile.

### 4.3 Pass catalog

Priority key: **P0** = highest leverage / lowest risk, do first.
Each pass: default state, algorithm sketch, who benefits, risks.

---

#### Pass A (P0): `DeadCodeEliminationPass` — default **on** (`ir.disableDeadCodeElimination`)

Remove operations that are pure (`isPureOp`, includes `Constant` ops) and have
zero uses. Iterate to a fixed point within the pass (removing a consumer can
kill its producers).

Algorithm (per function): a thin driver over
`FunctionRewriter::eraseIfDead` (milestone requirement M4 — the cascade,
purity oracle, and incremental use tracking live in the rewriter): open a
session, seed the cascade from every operation. Block arguments are **not**
removed here (Pass E owns the arity invariant).

Benefits: sweeps constant-folding residue (dead feeding constants → smaller
`tbc` constant image → faster every call), strength-reduction residue (the
dead cast), and trace noise. Reduces `asmjit` vreg pressure. This is the
enabling pass for everything else.

Risks: none structural — removal of a zero-use pure op cannot invalidate any
consumer by definition; the #327-style hazard (dangling pointers) is exactly
what the verifier's stale-pointer check catches.

---

#### Pass B (P0): `AlgebraicSimplificationPass` — default **on** (`ir.disableAlgebraicSimplification`)

Local, pattern-per-op rewrites; each either replaces the op's uses with an
existing operand (then the op dies in DCE) or folds it to a constant:

- Additive/multiplicative identities: `x+0`, `0+x`, `x-0`, `x*1`, `1*x`,
  `x/1` → `x`; `x*0`, `0*x` → `0` (integers only; float `0*x` is **not**
  folded — NaN/−0.0); `x-x` → `0` (integers only).
- Bitwise: `x&x`, `x|x` → `x`; `x^x` → `0`; `x&0` → `0`; `x|0`, `x^0`,
  `x<<0`, `x>>0` → `x`.
- Boolean: `and(x,true)` → `x`, `and(x,false)` → `false`, `or` duals,
  `not(not(x))` → `x`, `not(compare)` → inverted comparator.
- Casts: cast to the identical stamp → operand; cast-of-cast collapse only
  when provably lossless (widening chain of same signedness, or inner width ≥
  outer width for truncations); anything else left alone.
- `select(constBool, a, b)` → `a`/`b`; `select(c, x, x)` → `x`.
- **Canonicalization: constant operand to the right** for commutative ops
  (`add`, `mul`, `and`, `or`, `xor`, `eq`, `ne` — with comparator swap for
  ordered compares). This directly widens the reach of the existing backend
  immediate-folding optimizations, which only inspect the **right** operand
  (`tbc.immediates`; asmjit x64 immediate folding).

`ptr + 0` (any pointer-typed add with integer constant 0) → base pointer —
this specifically removes the strength-reduction residue.

Benefits: all three backends, 1 dispatch/instruction per hit per execution;
canonicalization multiplies the value of existing backend optimizations.

Risks: float semantics (be strictly conservative: only identity folds that
are IEEE-exact — `x+0.0` is *not* safe for −0.0, so skip float additive
identities entirely in v1); signedness of shifts and compares. Every rule
gets a differential-fuzzer soak (§5).

---

#### Pass C (P0): `ConstantBranchFoldingPass` + unreachable-block removal — default **on** (`ir.disableConstantBranchFolding`)

Two coupled steps, one pass:

1. Every `IfOperation` whose condition is a `ConstBooleanOperation` is
   replaced (via `replaceTerminatorOperation`) with a `BranchOperation` that
   adopts the taken side's `BasicBlockInvocation` (target + arguments). The
   not-taken edge's predecessor entry is removed. Also: `IfOperation` whose
   true and false invocations target the same block **with identical argument
   lists** → `BranchOperation`.
2. Unreachable-block sweep: DFS from the entry block; any unreached block is
   detached from `FunctionOperation`'s block list (arena reclaims memory at
   compile end; predecessor lists of reachable blocks are already correct
   because step 1 removed the edges).

Closes the loop that constant folding opens: today `compare(c1,c2)` folds to
a constant that still drives a *conditional* branch at runtime. After this
pass the branch is unconditional, the dead arm disappears, and
`EmptyBlockElimination`/block merging collapse the rest.

Benefits: interpreters lose a conditional-dispatch per execution and whole
dead regions; asmjit loses dead code and the branch. Especially valuable for
traced code with configuration-like constants.

Risks: predecessor-list bookkeeping (mitigated by 4.1(3) + verifier); entry
block must never be removed.

---

#### Pass D (P1): `BlockMergingPass` — default **on** (`ir.disableBlockMerging`)

If block `p` ends in an unconditional `BranchOperation` targeting `s`, and `s`
has exactly one predecessor (`p`) and `s != p`: rewrite every use of each of
`s`'s block arguments to the corresponding invocation argument (via
`replaceAllUses`), splice `s`'s non-terminator operations onto the end of `p`
(replacing `p`'s branch), adopt `s`'s terminator, fix successor predecessor
lists, delete `s`. Iterate to fixed point (chains collapse).

This is the generalization of `EmptyBlockEliminationPass` for the
single-predecessor case; the empty-block pass remains for the
multi-predecessor case it uniquely handles. Long term the two could unify;
not required now.

Benefits: one `JMP` dispatch + one full set of edge-copy `MOV`s per execution
per merged block — trace-generated IR is full of these seams. asmjit: shorter
code, fewer merged-value shuffles (#321 territory).

Risks: use-rewriting must cover invocation arguments (use `replaceAllUses`,
nothing hand-rolled); ordering with Pass C (run after, so unreachable blocks
don't get merged pointlessly).

---

#### Pass E (P1): `BlockArgumentPruningPass` — default **on** (`ir.disableBlockArgumentPruning`)

Two prunes per block (skip the entry block — its arguments are the function
ABI):

1. **Unused argument**: a block argument with zero uses in its block
   (including transitively via invocation args — use counts again). Remove
   the argument *and* the corresponding argument slot from **every**
   predecessor invocation targeting the block.
2. **Same-value pass-through**: every predecessor passes the *identical*
   `Operation*` `v` for the slot, and `v`'s defining block dominates the
   target block (Dominators helper; block arguments count as defined in their
   block). Replace all uses of the argument with `v`, then remove the slot as
   in (1). This is the SSA-phi-elimination analog and it is what deletes
   loop-invariant loop-carried arguments (preheader and latch both pass `v`).

Benefits: each pruned slot is a per-edge-execution `MOV` in `bc`/`tbc` (per
**iteration** for loop headers) and a register freed in all three backends.
Also shrinks `tbc` frame size.

Risks: the arity invariant — argument list and every predecessor invocation
must change atomically; guarded by the new verifier arity check (4.2(e)).
Dominance check must be correct for the self-referential case (a loop header
argument passed back to itself through the latch counts as "same value `v`"
only if the *other* predecessors agree on `v`; the standard rule: ignore
self-references when checking agreement).

---

#### Pass F (P2): `LocalCSEPass` — default **on** after measurement (`ir.disableLocalCSE`)

Per-block value numbering over pure ops: key =
`(opType, stamp, canonicalized operand identities, op-specific payload —
comparator, cast target, shift kind…)`. On a hit, `replaceAllUses(dup, first)`
and let DCE remove it. Commutative ops normalize operand order (Pass B's
canonicalization already helps). Memory ops excluded in v1 (no alias
analysis); do **not** CSE loads.

Scope note: block-local only. A dominator-scoped CSE is a straightforward
extension using 4.2(d) but v1 stays local — trace IR has long straight-line
blocks after Pass D, which is where the duplicates live.

---

#### Pass G (P2): `LoopInvariantCodeMotionPass` — default **off** initially (`ir.enableLICM`)

Reuse and extract the natural-loop recognizer from `StrengthReductionPass`
(single preheader + single latch) into a shared helper. Hoist a pure op into
the preheader when all its operands are defined outside the loop (or already
hoisted). **Never** hoist `DivOp`/`ModOp` (they trap on zero — speculation
across the loop-taken check is unsound when the loop body would not have
executed) unless the divisor is a non-zero constant. Loads are not hoisted in
v1 (no alias analysis).

Benefits: interpreters save `(hoisted ops) × (iterations − 1)` dispatches —
the highest per-hit payoff of any pass here; asmjit saves re-computation.

Risks: highest of the set — correctness depends on the loop matcher and the
speculation policy; hence opt-in first, promote to default after a fuzz soak
and benchmark sweep.

---

#### Enablement: `StrengthReductionPass` default **on**

After A + B land (they remove exactly the residue documented in
`StrengthReductionPass.hpp`), re-benchmark on `bc`, `tbc`, and `asmjit`. If
the regression is gone, flip the default (`ir.enableStrengthReduction` →
`ir.disableStrengthReduction`, keeping the old option recognized or migrating
it — grep consumers). If it still regresses the interpreters, keep it opt-in
and record the numbers in the pass header.

---

#### Explicitly deferred (documented, not designed here)

- **If-to-Select** for tiny diamonds (backends have `SelectOperation`
  support; `tbc` has a fused `SELECT`): promising but needs a cost heuristic;
  revisit after P0–P2 numbers exist.
- Dominator-scoped CSE, load CSE / redundant-load elimination (needs an
  aliasing story), ProxyCall inlining, sparse conditional constant
  propagation.

### 4.4 Pipeline

```
compileToIR:
  … existing tracing / SSA / IR generation …
  if ir.runPasses (default true):
    FixedPointGroup (maxIterations = ir.maxPipelineIterations, default 4):
      1. ConstantFoldingAndCopyPropagation   (existing)
      2. AlgebraicSimplification             (B)
      3. ConstantBranchFolding + unreachable (C)
      4. EmptyBlockElimination               (existing)
      5. BlockMerging                        (D)
      6. LocalCSE                            (F, once defaulted)
      7. DeadCodeElimination                 (A)
      8. BlockArgumentPruning                (E)
    Then (outside the group, single-shot):
      9. LICM                                (G, opt-in)
     10. StrengthReduction (+ one more Simplify/DCE round if it changed)
```

Ordering rationale: folding creates constants → simplification canonicalizes
and exposes constant conditions → branch folding kills edges → CFG cleanup
merges what remains → CSE dedupes the longer straight-line blocks → DCE
sweeps everything the previous passes orphaned → arg pruning last because
every CFG change can strand arguments. The group loops because each pass
feeds the ones "above" it on the next iteration; 4 iterations is empirically
generous for trace-shaped IR (validate via the new
`irPasses.pipelineIterations` statistic; the group is convergent because every
pass strictly shrinks blocks/ops/args or is a no-op — canonicalization must
use a strict order test so it cannot ping-pong).

All new default-on passes follow the existing option convention
(`ir.disableX`), opt-in passes use `ir.enableX` (see
`LegacyCompiler.cpp:163-180`). Document all new options in `docs/options.md`.

## 5. Testing & validation strategy

Layered, matching what the repo already has:

1. **Unit tests** — one `test/ir-pass-tests/<Pass>Test.cpp` per pass, built on
   `IRGraphFixtures.hpp` (extend fixtures as needed: loop graph, diamond
   graph, dead-op graph). Every test runs the manager with
   `ir.verifyAfterEachPass = true` and `ir.failOnVerifyError = true`.
   Idempotence test per pass: run twice, assert the second run reports
   no change. Negative tests matter as much as positive ones (float `0*x`
   NOT folded; div NOT hoisted; entry block args NOT pruned; self-loop
   handling).
2. **Verifier tests** — the new arity check gets its own
   `IRVerifierTest.cpp` cases.
3. **Whole-suite execution** — the full execution/val/tracing suites already
   run across every backend (`availableBackends()`); they must pass with the
   new defaults on. This is the primary miscompile net for `mlir`/`cpp` too.
4. **Differential fuzzer** — `nautilus-fuzz-replay` smoke corpus in CI-like
   runs during development; a longer `nautilus-fuzz` soak (≥ an hour) before
   flipping any pass default. The fuzzer generates loops, casts, selects and
   all ten numeric domains — precisely the risk surface of passes B/E/G. Add
   a fuzz configuration that runs with each new pass individually disabled to
   bisect any finding quickly.
5. **Benchmarks** — `test/benchmark/ExecutionBenchmark.cpp` on `tbc`, `bc`,
   `asmjit` (Release, Clang 21), before/after per pass and for the final
   pipeline. Record numbers in the PR description. Acceptance gate for
   default-on: no kernel regresses > 2% on any of the three backends, and the
   targeted kernels improve.
6. **Statistics as regression tripwire** — `ir.operations` /
   per-pass deltas via `engine.logStatistics` provide static-count evidence
   (e.g. "DCE removes N ops on the fibonacci kernel") that is stable across
   machines, unlike wall clock.

## 6. Rollout plan

Land in small PRs, in dependency order; every PR leaves `main` green with
defaults unchanged until the measurement gate passes:

Tracking issues group the PRs into four milestones: #338 (IR-0 +
pass-manager infrastructure), #342 (P0), #345 (P1), #347 (P2).

1. **PR-0a/0b/0c** Milestone IR-0 (see
   [ir-abstraction-milestone.md](ir-abstraction-milestone.md)):
   `Dominators` + verifier completion (V1–V7), `FunctionRewriter` mutation
   facade, retrofit of the three existing passes onto it. Behavior-
   preserving throughout. (#338)
2. **PR-0d** Pass-manager mechanics: `apply()`→`bool`, fixed-point groups.
   No behavior change (group of existing passes with maxIter 1 reproduces
   today's pipeline bit-for-bit). (#338)
3. **PR-1** DCE (A) — default on immediately (risk is minimal, effect is
   large and strictly shrinking). (#342)
4. **PR-2** AlgebraicSimplification (B) — default on after fuzz soak. (#342)
5. **PR-3** ConstantBranchFolding + unreachable removal (C). (#342)
6. **PR-4** BlockMerging (D). (#345)
7. **PR-5** BlockArgumentPruning (E). (#345)
8. **PR-6** StrengthReduction default flip (measurement PR; may be a no-op).
   (#347)
9. **PR-7** LocalCSE (F). (#347)
10. **PR-8** LICM (G) — opt-in; separate default-flip PR after soak. (#347)

Each pass PR: pass + unit tests + options.md entry + benchmark numbers in the
description. The companion playbook encodes this as agent milestones.

## 7. Risks

| Risk | Mitigation |
|---|---|
| Stale consumer pointers after replacement (repeat of #327) | Single `replaceAllUses` utility; verifier stale-pointer check runs after every pass in all tests |
| Block-arg / invocation arity drift | New verifier arity check (4.2(e)); pruning is the only pass allowed to change arity |
| Float semantics broken by simplification | Conservative rule set (no float additive identities, no float `x*0`); fuzzer float domains |
| Dispatch-bound cost-model regressions (StrengthReduction lesson) | Per-backend benchmark gate before any default flips; every pass individually disableable |
| Pass-group non-termination / ping-ponging | Strict-decrease or strict-canonical-order rules per rewrite; hard `maxIterations` bound; iteration-count statistic |
| Predecessor-list cache drift mid-pipeline | Passes must use the wiring helpers; verifier checks pred/succ symmetry |
| Compile-time growth (Nautilus compiles per query) | Per-pass `ms` statistics already recorded; budget: full pipeline ≤ 15% of `frontend.totalMs` on the benchmark suite |

## 8. Success metrics

- `exec_tbc_*` / `exec_bc_*` / asmjit benchmark kernels: measurable wall-clock
  improvement on loop kernels (target: ≥ 10% on at least the pointer/loop
  kernels once D+E+G land; A–C alone may be small on the existing kernels —
  their payoff is on trace-noise-heavy real workloads, tracked via op-count
  deltas).
- `ir.operations` after passes reduced vs. baseline on the test-data corpus.
- `StrengthReductionPass` default-on with no interpreter regression.
- Zero new fuzzer findings attributable to the passes after soak.
