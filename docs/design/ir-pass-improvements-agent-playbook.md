# Agent Playbook: Implementing the IR Pass Improvements

Instructions for an implementation agent (Sonnet-class) executing the design
in [ir-pass-improvements.md](ir-pass-improvements.md). Read that document
first; this one tells you *how to work*, in what order, and how to verify each
step. Do not start a milestone before its predecessor is merged or explicitly
approved in-branch.

## 0. Ground rules

- **One milestone per branch/PR.** Keep diffs reviewable. Branch names:
  `claude/ir-pass-<milestone>-<session-id>`.
- **Read before writing.** Before touching a file, read it fully. Before
  writing a pass, read all three existing passes
  (`ConstantFoldingAndCopyPropagationPass.cpp`, `EmptyBlockEliminationPass.cpp`,
  `StrengthReductionPass.cpp`) — they encode the house style for traversal,
  rewiring, and fixed-point loops.
- **Never hand-roll purity or rewiring.** Side-effect queries go through
  `OperationProperties.hpp` (`isPureOp`, `mayReadMemory`, `mayWriteMemory`).
  Use-rewiring goes through `RewriteUtil::replaceAllUses` (created in M0).
  CFG edits go through `BasicBlock::replaceSuccessor`,
  `replaceTerminatorOperation`, `BasicBlockInvocation::setBlock`, and the
  predecessor helpers — never mutate `predecessors` semantics by hand.
- **Style**: tabs, 120 cols, `CamelCase` classes, `lower_case` locals,
  member `trailing_underscore_` only where the file you're editing already
  does so — match the surrounding file. Doxygen-style class comment on every
  pass header describing exact scope and guards (copy the tone of
  `EmptyBlockEliminationPass.hpp`).
- **Format before every commit**: `cmake --build build --target format`,
  then confirm `./format.sh` is clean.
- **Compiler**: Clang 21 (`-DCMAKE_CXX_COMPILER=clang++-21`).
- If reality contradicts the design doc (an API is missing, an invariant
  doesn't hold), stop and report the discrepancy in the PR/summary instead of
  improvising a workaround.

## 1. Build & test commands (use these exactly)

```bash
# One-time configure
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++-21 \
      -DENABLE_TESTS=ON -DENABLE_FUZZER_REPLAY=ON ..

# Build + full test suite
cmake --build . -j && ctest --test-dir nautilus --output-on-failure

# Just the IR pass tests (fast inner loop — run the Catch2 binary directly;
# ctest registers individual TEST_CASE names, not the suite name)
./nautilus/test/ir-pass-tests/nautilus-ir-pass-tests

# Differential fuzzer smoke corpus (must be clean before every push)
./nautilus/test/fuzz/nautilus-fuzz-replay

# Benchmarks (separate Release build; needed for measurement milestones)
cmake -B build-rel -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-21 \
      -DENABLE_BENCHMARKS=ON .
cmake --build build-rel -j --target ExecutionBenchmark
```

(If a target/test name differs, discover the real one via
`ctest --test-dir nautilus -N` and the `CMakeLists.txt` of the directory —
do not guess.)

Every new pass test must set, on the `engine::Options` given to
`IRPassManager`:

```cpp
opts.setOption("ir.verifyAfterEachPass", true);
opts.setOption("ir.failOnVerifyError", true);
```

## 2. Milestones

### M0 — Infrastructure (PR-0)

Files: `ir/passes/IRPass.hpp`, `IRPassManager.{hpp,cpp}`, new
`RewriteUtil.{hpp,cpp}`, new `Dominators.{hpp,cpp}`, `IRVerifier.cpp`,
`ir/passes/CMakeLists.txt`, plus the three existing passes and
`LegacyCompiler.cpp`.

1. Change `IRPass::apply` to return `bool` (true iff the graph changed).
   Update the three existing passes — each already tracks a `changed` flag
   internally; surface it. Update `IRPassManager::run` to skip the
   `dumpAfterEachPass` dump when a pass reports no change (keep verification
   behavior as-is).
2. Add fixed-point groups to `IRPassManager`. Simplest workable API:
   `void addFixedPointGroup(std::vector<std::unique_ptr<IRPass>> passes, size_t maxIterations)`
   where a plain `addPass` is a group of one with one iteration. Read
   `ir.maxPipelineIterations` (default 4) in `LegacyCompiler.cpp` when
   constructing the group. Record `irPasses.pipelineIterations` in
   statistics. Per-pass timing/delta statistics must aggregate across
   iterations (sum the ms, sum the deltas).
3. Extract `replaceAllUses(FunctionOperation&, Operation* from, Operation* to)`
   into `RewriteUtil` from the copy-propagation logic in
   `ConstantFoldingAndCopyPropagationPass.cpp`; make that pass call it. It
   must cover: every operation's `getInputs()` span (rewire via `setInput`)
   and every terminator's `BasicBlockInvocation` arguments, in every block of
   the function.
4. Add `Dominators` (iterative Cooper–Harvey–Kennedy over RPO from the entry
   block). API: constructed from a `FunctionOperation&` (after predecessor
   lists are valid), `bool dominates(const BasicBlock*, const BasicBlock*)`.
5. Verifier: add the invocation-arity check — for every terminator
   invocation, `invocation.getArguments().size() == target->getArguments().size()`
   (use the real accessor names from `BasicBlockInvocation.hpp`).
6. Tests: `IRPassManagerTest.cpp` gains fixed-point-group cases (group
   converges; maxIterations bound respected; no-change passes skipped in
   dumps). `IRVerifierTest.cpp` gains arity-violation cases (construct a
   deliberately broken graph). Add `DominatorsTest.cpp` (diamond, loop,
   chain).
7. **Behavioral gate**: with the pipeline in `LegacyCompiler.cpp` expressed
   as a maxIterations=1 group of the existing passes, all test-data dumps and
   the whole suite must be unchanged. Run the full suite + fuzz smoke.

### M1 — DeadCodeEliminationPass (PR-1)

Files: new `ir/passes/DeadCodeEliminationPass.{hpp,cpp}`, `LegacyCompiler.cpp`,
new `test/ir-pass-tests/DeadCodeEliminationTest.cpp`, `docs/options.md`.

1. Implement per design §4.3-A. Reuse `Usages::countUsages` for the initial
   counts; maintain counts incrementally while erasing (`removeOperation`),
   loop to fixed point within the function. Only ops with
   `isPureOp(op->getOperationType())`, not terminators, not block arguments.
2. Register default-on behind `ir.disableDeadCodeElimination`, positioned per
   design §4.4 (for now: after EmptyBlockElimination inside the group).
3. Tests: dead const chain removed; dead cast removed; op used only by a
   dead op removed (transitivity); store/call/load NOT removed even when
   unused; op used only as invocation argument NOT removed; idempotence.
4. Full suite + fuzz smoke. Capture `ir.operations` before/after on two
   test-data categories (`engine.logStatistics` or the dump files) and put
   the numbers in the PR description.

### M2 — AlgebraicSimplificationPass (PR-2)

Files: new pass + test, `LegacyCompiler.cpp`, `docs/options.md`.

1. Implement the rule set of design §4.3-B **exactly** — the float and
   signedness carve-outs are the specification, not suggestions. Structure:
   one `simplify<OpKind>` helper per op family returning
   `Operation*` replacement or `nullptr`; the driver walks each block,
   applies, uses `replaceAllUses`, and lets the op die (DCE cleans up —
   do not duplicate removal logic).
2. Canonicalization (const-to-right) must use a strict test
   (`left is const && right is not const`) so repeated application is a
   no-op — this is what makes the fixed-point group terminate.
3. Tests: every rule, positive and negative (at minimum: float `x*0` NOT
   folded, float `x+0` NOT folded, `x-x` float NOT folded, signed vs unsigned
   shift behavior preserved, cast-of-cast only in the lossless cases,
   comparator swap correctness for `<`/`<=` when canonicalizing).
   Idempotence test.
4. Full suite + fuzz smoke, plus a longer replay/survey run
   (`nautilus-fuzz-replay --survey 200000` if available) because this pass
   has the widest semantic surface. Any finding: fix or revert the rule —
   never ship a rule with a known finding.

### M3 — ConstantBranchFoldingPass (PR-3)

Per design §4.3-C. New pass + tests; runs inside the group after
simplification. Key test cases: if(true)/if(false) fold with argument lists
preserved; both-arms-same-target-same-args fold; unreachable diamond arm
removed; unreachable *loop* (cycle not reachable from entry) removed; entry
block never removed; predecessor lists correct afterwards (verifier enforces).
After this lands, check the interaction test: constant compare → (existing
folding) → constant bool → (this pass) → straight line, in one pipeline run.

### M4 — BlockMergingPass (PR-4)

Per design §4.3-D. Merge only when: terminator is unconditional branch,
target has exactly one predecessor, target ≠ source, target is not the entry
block. Rewrite target's block-argument uses to the invocation's actual
arguments via `replaceAllUses` **before** splicing. Tests: chain of
single-pred blocks collapses to one; block with two preds NOT merged;
self-loop NOT merged; loop latch→header NOT merged (header has 2 preds);
argument values correctly substituted (build a graph where the argument is
used by an op and by a nested invocation).

### M5 — BlockArgumentPruningPass (PR-5)

Per design §4.3-E. This is the invariant-heaviest pass:

- Remove an argument slot and every corresponding invocation argument in the
  same mutation step; run the verifier in every test (arity check from M0
  will catch drift).
- Same-value pass-through: ignore self-referential edges (latch passing the
  header's own argument back) when checking all-preds-agree; require the
  agreed value's defining block to dominate the target (`Dominators`).
- Entry block: never touched.
- Tests: unused arg pruned everywhere; loop-invariant carried arg replaced by
  preheader value (the marquee case — assert the loop body now reads the
  outer value directly); arg used only by dead code (order with DCE: run
  group, expect both gone); args where preds disagree NOT pruned; dominance
  failure NOT pruned.

### M6 — StrengthReduction default flip (PR-6, measurement-only)

1. Release build, run `ExecutionBenchmark` on `tbc`, `bc`, `asmjit` with
   `ir.enableStrengthReduction` off vs on (now that A+B clean its residue).
   ≥ 5 runs, report medians.
2. If no interpreter kernel regresses > 2%: flip the default (design §4.3),
   update `StrengthReductionPass.hpp`'s comment and `docs/options.md`, keep a
   disable option. If it still regresses: do NOT flip; update the header
   comment with the fresh numbers and close the milestone with the report.

### M7 — LocalCSEPass (PR-7) and M8 — LICM (PR-8)

Per design §4.3-F/G. LICM ships opt-in (`ir.enableLICM`); extract the
natural-loop matcher from `StrengthReductionPass.cpp` into a shared helper
(`passes/LoopInfo.{hpp,cpp}`) as part of M8 and make StrengthReduction use it
(behavior-preserving refactor, verified by its existing tests). LICM tests
must include: div/mod NOT hoisted (non-const divisor), load NOT hoisted,
chain-hoisting (op depending on hoisted op hoists too), nested-loop-shaped
graph (only the matched natural loop is transformed).

## 3. Definition of done — every milestone

1. `cmake --build build -j` clean, zero warnings introduced.
2. `ctest --test-dir nautilus --output-on-failure` fully green.
3. `./nautilus/test/fuzz/nautilus-fuzz-replay` (smoke corpus) clean.
4. New pass: unit tests exist, include an idempotence case and at least three
   negative cases; test options enable verify-after-each-pass + fail-on-error.
5. `cmake --build build --target format` run; `./format.sh` clean.
6. `docs/options.md` updated for any new option.
7. PR description: what/why (link the design doc section), test evidence,
   and for measurement-relevant milestones the benchmark/op-count numbers.
8. Commit messages: imperative mood, one logical change per commit.

## 4. Known traps (learned from this codebase's history)

- **#327**: replacing an op without rewiring *invocation arguments* leaves
  stale pointers that the verifier now catches — but only if your test turns
  the verifier on. Always route through `replaceAllUses`.
- **#321**: block-argument identifier collisions corrupted asmjit merged
  values. When you create or substitute block arguments, never reuse
  identifiers; when pruning, don't renumber survivors.
- `Operation` has a **non-virtual destructor** and lives in an arena: never
  `delete` an operation, never keep an op alive outside its arena's lifetime,
  and "removing" means detaching from the block's vector only.
- `BasicBlock::getPredecessors()` is a maintained cache, valid only if every
  CFG edit went through the wiring helpers. If you bypass them, the verifier's
  pred/succ symmetry check will fail far from your bug.
- A block may list the same predecessor **twice** (both arms of an `if`
  targeting one block). `removePredecessor` removes one occurrence — call it
  once per edge, not once per block.
- The interpreters' cost model is dispatch-bound: a transformation that adds
  ops to remove "work" (the StrengthReduction lesson) can be a net loss.
  When in doubt, measure on `tbc` before assuming.
- `mlir` and `cpp` run the same pipeline: your pass must be
  semantics-preserving universally, not just "good enough for interpreters".
