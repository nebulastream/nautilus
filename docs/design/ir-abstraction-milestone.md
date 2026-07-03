# Milestone IR-0: Hardening the Nautilus IR Abstraction

**Status:** Proposed
**Parent design:** [ir-pass-improvements.md](ir-pass-improvements.md) — this
milestone replaces and expands that document's "Phase 0 infrastructure"
(§4.2) with concrete, testable requirements. The pass catalog (DCE,
simplification, branch folding, …) builds strictly on top of what is
specified here.
**Implementation guide:** [agent playbook](ir-pass-improvements-agent-playbook.md), milestone M0.

---

## 1. Why an abstraction milestone first

Every planned pass performs the same five mutations: *replace uses*, *erase*,
*create/insert*, *change block-argument arity*, *rewire CFG edges*. Today each
of these is either missing, unsafe, or reinvented per pass. The evidence is
in the tree:

1. **Def→use direction does not exist.** `Operation::getInputs()` gives
   use→def edges only; `Usages::countUsages` gives counts, not users. So
   `ConstantFoldingAndCopyPropagationPass` rewires consumers by rescanning
   the whole function per fold round — a fixed-point loop that is
   O(ops × folds). Every new pass would inherit that shape.
2. **Invocation operands are invisible to generic traversal.**
   `BasicBlockInvocation` stores its arguments in the base
   `Operation::inputs` span (good), but the invocation itself is *embedded in
   the terminator* — it is not in `BasicBlock::getOperations()`. Any code
   that walks "all ops in all blocks" silently misses every block-edge
   operand unless it also calls `getSuccessorInvocations()`. Issue **#327**
   (stale consumer pointers after constant folding) was exactly this trap,
   and `countUsages`, the verifier, and the folding pass each carry their own
   special case for it today.
3. **There is no fresh-identifier authority.**
   `StrengthReductionPass.cpp` hand-rolls `computeNextId(ir)` (a max-scan)
   and threads a `uint32_t& nextId` through its helpers. Issue **#321**
   (asmjit merged-value corruption) was an identifier collision. Nothing
   prevents the next pass from re-creating it.
4. **Block-argument arity is a two-sided invariant with no owner.**
   `BasicBlock::addArgument` documents: *"callers are responsible for the
   invocation side"*. There is no `removeArgument` on either side at all —
   the planned `BlockArgumentPruningPass` has no primitive to build on.
5. **Erase has no safety rail.** `BasicBlock::removeOperation` detaches an
   op regardless of remaining uses; a #327-style dangling pointer is one
   forgotten rewire away, discovered only if a test happens to run the
   verifier.
6. **The verifier cannot see the bugs the new passes will write.** It checks
   structure (terminators, ownership, stale pointers, pred *membership*) but
   not: identifier uniqueness (would have caught #321 statically),
   invocation/block-arg arity, SSA dominance of operands, stamp agreement on
   block edges, or predecessor *multiplicity* (a block targeted by both arms
   of one `if` must list that predecessor twice).

This milestone fixes the write path (a `FunctionRewriter` mutation facade)
and the read-back path (verifier completeness) before any new pass lands, so
that pass authors — human or agent — get invariants by construction and
violations at the point of the bug, not three passes later in a backend
miscompile.

## 2. Scope

**In scope**

- `Dominators` analysis helper (needed by both the verifier and later passes).
- `FunctionRewriter`: a per-function mutation session that owns use tracking,
  identifier minting, safe erase, insertion, atomic block-argument mutation,
  and CFG rewiring.
- Verifier completion: six new invariant checks with precise error messages.
- Shared test fixtures (`IRGraphFixtures` extensions) for all future pass
  tests.
- Retrofit of the three existing passes onto the new primitives as the
  proof-of-fit gate (behavior-preserving).

**Out of scope**

- Any change to `Operation`'s 32-byte layout (no intrusive use-lists — use
  tracking is an external side table owned by the rewriter session).
- Any new pass, any pipeline/default change (that is PR-1+ of the parent
  design).
- `IRPassManager` fixed-point groups and the `apply()`→`bool` change (they
  remain in the parent design's rollout as the step after this milestone;
  they depend on nothing here and nothing here depends on them).

## 3. Design

### 3.1 `Dominators` (`ir/passes/Dominators.{hpp,cpp}`)

Iterative dominator computation (Cooper–Harvey–Kennedy) over reverse
post-order from the entry block. Requires valid predecessor lists
(`IRPassManager` already calls `rebuildPredecessorLists`; standalone users
call it themselves).

```cpp
class Dominators {
public:
	explicit Dominators(const FunctionOperation& fn); // preds must be valid
	/// True iff a dominates b. a dominates itself.
	[[nodiscard]] bool dominates(const BasicBlock* a, const BasicBlock* b) const;
	/// True iff block is reachable from the entry block.
	[[nodiscard]] bool isReachable(const BasicBlock* block) const;
	/// Blocks in reverse post-order (reachable blocks only).
	[[nodiscard]] const std::vector<const BasicBlock*>& reversePostOrder() const;
};
```

Semantics for unreachable blocks (they exist transiently, e.g. after branch
folding and before the unreachable sweep): `isReachable` is false,
`dominates(x, unreachable)` is false for all `x`, and unreachable blocks are
absent from RPO. Consumers must not ask for dominance *of* an unreachable
block's contents; the verifier (V3) restricts its dominance checking to
reachable blocks accordingly.

### 3.2 `FunctionRewriter` (`ir/passes/FunctionRewriter.{hpp,cpp}`)

A mutation session over one function. Construction is O(function); every
primitive is O(affected entities) — **no primitive may rescan the function**.
This is what turns the folding pass's O(ops × folds) loop into O(ops + edges).

```cpp
class FunctionRewriter {
public:
	FunctionRewriter(FunctionOperation& fn, common::Arena& arena);

	// ── Use queries (uniform over operand edges AND invocation arguments) ──
	struct Use { Operation* user; uint32_t operandIndex; };   // user may be a BasicBlockInvocation
	[[nodiscard]] std::span<const Use> usesOf(const Operation* op) const;
	[[nodiscard]] size_t useCount(const Operation* op) const;
	[[nodiscard]] bool hasUses(const Operation* op) const;
	[[nodiscard]] BasicBlock* definingBlock(const Operation* op) const; // block args included

	// ── Value rewiring ──
	/// Rewires every use of from (operand slots and invocation arguments)
	/// to to. O(uses(from)).
	void replaceAllUses(Operation* from, Operation* to);

	// ── Identifiers ──
	/// Strictly greater than every identifier in the function at session
	/// start and every identifier previously minted by this session.
	[[nodiscard]] OperationIdentifier freshId();

	// ── Creation & insertion ──
	/// Arena-allocates T (injecting the arena per the uniform ctor
	/// convention), registers its operand uses, and inserts it.
	template <typename T, typename... Args>
	T* createBeforeTerminator(BasicBlock* block, Args&&... args);
	template <typename T, typename... Args>
	T* createBefore(Operation* anchor, Args&&... args);

	// ── Erasure ──
	/// Precondition: useCount(op) == 0 and op is not a terminator.
	/// Detaches op from its block and unregisters its operand uses.
	/// Throws RuntimeException on a live op (testable safety rail).
	void erase(Operation* op);
	/// erase(op) if op is pure and use-free, then cascade into operands
	/// that became use-free and pure. Returns number of ops erased.
	size_t eraseIfDead(Operation* op);

	// ── Block arguments (both sides of the arity invariant, atomically) ──
	/// Precondition: the argument has no uses. Removes argument slot i of
	/// block AND the i-th argument of every invocation targeting block
	/// (including multiple invocations from one predecessor).
	void removeBlockArgument(BasicBlock* block, size_t index);
	/// Appends an argument of the given stamp; valueForEdge is called once
	/// per invocation targeting block to supply that edge's value.
	BasicBlockArgument* addBlockArgument(BasicBlock* block, Type stamp,
	                                     const std::function<Operation*(BasicBlockInvocation&)>& valueForEdge);

	// ── CFG ──
	/// Retargets an invocation; predecessor lists stay consistent
	/// (delegates to BasicBlockInvocation::setBlock).
	void setInvocationTarget(BasicBlockInvocation& inv, BasicBlock* newTarget);
	/// Replaces the terminator; predecessor lists of old and new
	/// successors stay consistent; use table updated for both terminators'
	/// operands and invocation arguments.
	void replaceTerminator(BasicBlock* block, Operation* newTerminator);
	/// Precondition: block has no predecessors and is not the entry block.
	/// Detaches the block (FunctionOperation::detachBasicBlock) and
	/// unregisters all uses originating in it. Throws otherwise.
	void eraseBlock(BasicBlock* block);
};
```

Design decisions, with rationale:

- **External use table, not intrusive use-lists.** `Operation` is a packed
  32-byte arena object with a `static_assert` on its size; growing it for a
  users pointer taxes every compile to benefit only the pass pipeline. The
  side table (`unordered_map<const Operation*, small vector<Use>>`) is built
  once per session in O(n) and maintained incrementally. Because invocation
  arguments already live in `Operation::inputs`, a `Use` whose `user` is the
  invocation is representable and rewireable via the same
  `Operation::setInput` path — **the invocation special case exists in
  exactly one place** (the table builder), instead of in every pass.
- **Session discipline.** While a rewriter is live, all mutation of that
  function goes through it; bypassing it (raw `removeOperation`,
  `setInput`, …) leaves the table stale. This is a documented contract, the
  same one `getPredecessors()` already has. Enforcement is the verifier run
  after every pass in tests, plus debug-build assertions inside the rewriter
  where cheap (e.g. `erase` re-checking membership).
- **Throwing preconditions, not UB.** `erase`, `removeBlockArgument`,
  `eraseBlock` throw `RuntimeException` on contract violation. Throwing is
  chosen over `assert` because it is testable with Catch2
  (`REQUIRE_THROWS_AS`) and active in Release test runs.
- **`eraseIfDead` lives here, not in the DCE pass.** The cascade ("erasing a
  consumer may free its producers") is the reusable core; the future
  `DeadCodeEliminationPass` becomes a ~20-line driver that seeds the cascade
  from all ops. Simplification/CSE/branch-folding get correct cleanup for
  free by calling `eraseIfDead` on whatever they orphan.
- **`freshId()` subsumes `computeNextId`.** The max-scan moves into the
  session constructor; `StrengthReductionPass`'s local copy is deleted in the
  retrofit step. Uniqueness is then verifier-enforced (V2), closing the #321
  class at both ends.

### 3.3 Verifier completion (`ir/passes/IRVerifier.cpp`)

Six new checks, added to the existing ones. All run unconditionally in
`verify()` (the verifier is already opt-in per run via
`ir.verifyBeforePipeline` / `ir.verifyAfterEachPass`); all must stay
O(function) or O(function × dom-depth). Every error message must name the
function, block, and offending operation identifier(s), and state expected
vs. actual — the existing message style.

| ID | Invariant |
|----|-----------|
| **V1** | Every `BasicBlockInvocation`'s argument count equals its target block's argument count. |
| **V2** | Operation identifiers (block ops **and** block arguments) are unique within a function. |
| **V3** | SSA dominance: for every operand edge in a *reachable* block, the definition dominates the use. Same block: definition at a lower operation index (block arguments precede all operations; a terminator's invocation arguments are uses at the terminator's position). Cross-block: `Dominators::dominates(defBlock, useBlock)`, with block arguments defined at their block's head. Unreachable blocks keep only the existing structural checks. |
| **V4** | Predecessor lists match incoming edges as a **multiset**: for each block, the number of times block `p` appears in `getPredecessors()` equals the number of invocations in `p`'s terminator targeting it. Replaces the current one-directional membership check. |
| **V5** | The entry block has no predecessors, and its argument count and stamps match `FunctionOperation::getInputArgs()`. |
| **V6** | Stamp agreement on block edges: each invocation argument's stamp equals the corresponding block argument's stamp. *Calibration step:* before enabling, run the full test corpus with V6 counted-but-not-failing; if legitimate mismatches exist in today's IR (e.g. deliberate ptr/int punning from tracing), narrow V6 to the mismatch classes that are genuinely illegal and document the exemptions in the check itself.* |

### 3.4 Shared fixtures (`test/ir-pass-tests/IRGraphFixtures.hpp`)

New builders, used by this milestone's tests and by every subsequent pass
test:

- `makeDiamondGraph()` — entry → if → (then, else) → merge(arg) → ret; the
  merge block takes one argument fed differently by each arm.
- `makeSharedTargetIfGraph()` — an `if` whose **both** invocations target the
  same block (the duplicate-predecessor case).
- `makeNaturalLoopGraph()` — preheader → header(args: iv, acc) ⇄ latch, header
  → exit; loop-carried and loop-invariant arguments.
- `makeDeadChainGraph()` — a pure chain (const → cast → add) with zero uses
  next to a live result, plus a use-free `StoreOperation` (must never count
  as dead).
- Each builder returns a verifier-clean graph (asserted in a fixture
  self-test), so pass tests start from a known-good baseline.

## 4. Requirements and acceptance tests

Conventions for every test in this milestone: construct via
`IRGraphFixtures`, run mutations, then `REQUIRE(IRVerifier::verify(*ir).ok())`
(with the V1–V6 checks active) unless the test *is* a verifier-negative test.
Test files: `DominatorsTest.cpp`, `FunctionRewriterTest.cpp`, extended
`IRVerifierTest.cpp`, `IRGraphFixturesTest.cpp` (self-checks).

### D — Dominators

| Req | Requirement | Acceptance tests |
|-----|-------------|------------------|
| D1 | Correct dominance on the standard shapes | Diamond: entry dominates all; neither arm dominates merge; merge does not dominate arms. Loop: preheader dominates header/latch/exit; header dominates latch; latch does not dominate exit. Chain: transitive. Self: `dominates(b,b)` true. |
| D2 | Unreachable handling per §3.1 | Graph with a detached cycle: `isReachable` false for its blocks, absent from RPO, `dominates(entry, unreachableBlock)` false; construction does not throw. |
| D3 | Determinism | RPO identical across two constructions of the same graph. |

### M — FunctionRewriter mutation primitives

| Req | Requirement | Acceptance tests |
|-----|-------------|------------------|
| M1 | **Uniform use model.** `usesOf/useCount/hasUses` count operand edges and invocation-argument edges identically; `definingBlock` covers ops and block arguments. | Value used once by an `AddOperation` and once as an invocation argument → `useCount == 2` and `usesOf` names both users (one of them a `BasicBlockInvocation`). Unused const → `hasUses` false. Block argument's defining block reported correctly. |
| M2 | **`replaceAllUses` is complete and incremental.** After the call, `useCount(from) == 0`, `to` gained exactly the moved uses, and *no other* counts changed. Must cover invocation arguments (the #327 shape). | (a) Diamond fixture: replace the value one arm passes to the merge block; the invocation now passes the replacement; `erase(from)` succeeds; verifier green (stale-pointer + V3 would catch omissions). (b) Value with mixed uses (op operand + invocation arg + return value) — all three rewired. (c) Counts on an unrelated op unchanged. |
| M3 | **Safe erase.** `erase` on a live op throws `RuntimeException` and mutates nothing; successful `erase` detaches and decrements its operands' counts. | `REQUIRE_THROWS_AS` on a used op, then verify the graph is byte-identical (`toString()` unchanged). Erase a use-free op: gone from `getOperations()`, operand counts decremented, verifier green. Erasing a terminator throws. |
| M4 | **`eraseIfDead` cascade with purity boundary.** Erases the maximal dead pure chain; never erases non-pure ops (`isPureOp` is the single oracle); returns the count. | Dead-chain fixture: `eraseIfDead(add)` returns 3 (add, cast, const), live sibling untouched. Use-free `StoreOperation`: returns 0, store remains. Chain feeding both a dead and a live consumer: shared producer survives. Second call returns 0 (idempotence). |
| M5 | **Fresh-identifier authority.** `freshId()` values are strictly greater than all pre-existing ids and never repeat within a session, including after erases. | Mint two ids on a fixture: both unique vs. every existing op/arg id (checked by V2 after creating ops with them) and unequal. Erase an op, mint again: still no reuse. |
| M6 | **Creation & insertion.** `createBefore*` inserts at the documented position, registers the new op's operand uses, and assigns a fresh id automatically. | `createBeforeTerminator<AddOperation>` lands at index `size-2`; its operands' use counts incremented; V2/V3 green. `createBefore(anchor)` lands immediately before anchor. |
| M7 | **Atomic block-argument removal.** `removeBlockArgument` updates the block and *every* invocation targeting it in one call; throws (mutating nothing) if the argument has uses. | Diamond fixture: remove the merge block's unused second argument (add one in the fixture) → both arm invocations shrink; V1 green. **Shared-target-if fixture: both invocations of the single predecessor shrink.** Natural-loop fixture: removing a header argument updates preheader and latch edges. Live argument → throws, graph unchanged. |
| M8 | **Atomic block-argument addition.** `addBlockArgument` appends the argument and exactly one value per incoming invocation (callback per edge); fresh id; stamp as given. | Loop fixture: add a pointer argument with distinct preheader/latch values (the strength-reduction shape) → V1/V2/V3/V6 green; callback invoked exactly twice. Shared-target-if: callback invoked once per *invocation* (twice), not per predecessor block. |
| M9 | **CFG primitives preserve the predecessor multiset.** `setInvocationTarget`, `replaceTerminator`, `eraseBlock` leave `getPredecessors()` exactly matching edges (V4-checkable at any point). `eraseBlock` throws on the entry block or on a block with predecessors. | Retarget one arm of a diamond to a new block → old target loses one pred entry (still keeping others), new target gains one; V4 green. Replace an `IfOperation` with a `BranchOperation` to one of its targets → the other target's pred entry removed. Detached-cycle fixture: `eraseBlock` succeeds on pred-free block, throws on entry and on a block with preds. |
| M10 | **No full-function rescans after construction.** Contract: every primitive is O(affected uses/args/edges). | Code-review contract (documented per method in the header) + a large-graph smoke test: a generated straight-line function with 5,000 ops; perform 1,000 `replaceAllUses` + `eraseIfDead` operations; test asserts completion and verifier-green only (no timing assertions — wall-clock flake is worse than no check; the complexity gate is review). |

### V — Verifier completion

Each check needs at least one *catches-the-violation* test (build the broken
graph with raw IR APIs, bypassing the rewriter) and one *accepts-legal-IR*
test (the fixtures). Error-message tests assert the message names the
offending identifiers.

| Req | Requirement | Negative test (must flag) | Positive test (must stay silent) |
|-----|-------------|---------------------------|----------------------------------|
| V1 | Invocation arity == target arity | Invocation with one argument too few (built via `clearArguments` + partial re-add) | All fixtures |
| V2 | Unique op/arg identifiers per function | Two ops sharing an id (the #321 shape); duplicated between an op and a block argument | All fixtures; ids minted via `freshId` |
| V3 | Def dominates use (reachable blocks) | (a) use before def in the same block; (b) merge block using a value defined in only one diamond arm; (c) value defined in a loop body used in the preheader | Diamond + loop fixtures (incl. loop-carried args — the legal cyclic case); detached-cycle graph is not dominance-checked |
| V4 | Predecessor multiset == incoming edges | Shared-target-if graph with the duplicate pred entry manually removed once; a stale extra pred entry | Shared-target-if fixture (predecessor listed exactly twice) |
| V5 | Entry: no preds; args match signature | Branch retargeted to the entry block; entry arg count != `getInputArgs().size()`; stamp mismatch in slot 0 | All fixtures |
| V6 | Edge stamp agreement (post-calibration; see §3.3) | Invocation passing an `i64` into a `ptr` block argument | All fixtures; whatever exemptions calibration documented |
| V7 | **Corpus gate:** the whole execution-test suite runs verifier-clean. Add an env-var hook (e.g. `NAUTILUS_IR_VERIFY=1`) that makes the shared test engine options enable `ir.verifyAfterEachPass` + `ir.failOnVerifyError` (wire it where `test/common/ExecutionTest.hpp` builds engine options). | — | One full `ctest` run and one fuzz-replay smoke run with the hook enabled must be green before the milestone merges; the hook stays for future CI use. |

### F — Retrofit (proof-of-fit gate)

| Req | Requirement | Acceptance |
|-----|-------------|------------|
| F1 | `ConstantFoldingAndCopyPropagationPass` reimplemented on `FunctionRewriter` (`replaceAllUses`, `eraseIfDead`, worklist instead of whole-function rescan per round). | All existing `ConstantFoldingAndCopyPropagationTest` cases pass unchanged; `test/data/**` dumps unchanged (full suite green); folding leaves no dead feeding constants behind on a new chained-fold unit test (`(5+3)*2`: the `5`, `3`, and `8` constants are gone, only `16` remains). |
| F2 | `EmptyBlockEliminationPass` uses rewriter CFG/argument primitives instead of hand-rolled rewiring. | Existing `EmptyBlockEliminationTest` cases pass unchanged; full suite green. |
| F3 | `StrengthReductionPass` uses `freshId()`/creation primitives; local `computeNextId` and `nextId` threading deleted. | Existing behavior preserved (its tests + `ir.enableStrengthReduction` execution runs); no remaining `computeNextId` in the tree. |
| F4 | Whole-milestone regression gate. | Full `ctest` suite, fuzz-replay smoke corpus, and the V7 verifier-enabled run all green; `./format.sh` clean. |

If a retrofit reveals the rewriter API cannot express something a pass needs,
**change the rewriter in this milestone** (and its tests) rather than letting
the pass bypass it — the entire point is that PR-1+ never touches raw
rewiring again.

## 5. Sequencing within the milestone

Three PRs, each independently green (tracking issues: #338, #339, #340):

1. **IR-0a — Analysis & verification:** `Dominators` (D1–D3), fixtures
   (§3.4 + self-tests), verifier checks V1–V6 with tests, V6 calibration,
   V7 env hook. No mutation-API changes; existing passes must already
   satisfy V1–V6 (if one does not, fixing that latent bug is in-scope for
   IR-0a and gets its own regression test).
2. **IR-0b — FunctionRewriter:** the class + M1–M10 tests. No pass changes.
3. **IR-0c — Retrofit:** F1–F4.

Order rationale: the verifier lands first so the rewriter is developed
against a net that already catches arity/dominance/id bugs; the retrofit
lands last as the proof that the abstraction fits real passes.

## 6. Definition of done (milestone)

1. All D/M/V/F requirement tests exist and pass; every mutation test runs the
   full verifier at the end.
2. `ctest --test-dir nautilus` green; fuzz-replay smoke green; V7 run green.
3. No pass or utility outside `FunctionRewriter` mints `OperationIdentifier`s
   or hand-rolls consumer rewiring (grep gate: `computeNextId` gone;
   `replaceArgument`/raw `setInput` uses outside the rewriter and the
   IR-construction phases are reviewed and justified).
4. Header docs on `FunctionRewriter`, `Dominators`, and each new verifier
   check follow the house documentation style (scope, guards, complexity).
5. `cmake --build build --target format` applied; zero new warnings.
6. Parent design doc's §4.2 stays accurate (it references this file).
