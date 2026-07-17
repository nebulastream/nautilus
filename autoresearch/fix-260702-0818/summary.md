# Fuzz coverage campaign — control-flow shapes

**Task:** improve the fuzzer further — more control-flow: ifs, different loop kinds, etc.

**Result: 4 new control-flow constructs added to the differential fuzzer; no new miscompiles found** (the previous session's fixes hold under substantially harder control-flow stress, which this coverage now permanently guards).

## Extensions (branch `claude/fuzzer-controlflow-XqJnkq`, based on the bug-fix PR branch)

| Construct | Commit | What it stresses |
|---|---|---|
| `Loop2` — two loop-carried accumulators, parallel update (fibonacci shape, real back-edge swaps) | ce22ad8b | parallel-copy sequencing, multi-argument block-invocation passing — where the asmjit #321 and BC merge bugs lived |
| `LoopBreak` — traced `if (...) break;` mid-body on a fixed accumulator predicate | ce22ad8b | mid-body early-exit CFG split, a shape no other loop kind produces |
| `WhileLoop` — compound header condition `i < trips && pred(acc)` re-evaluated per iteration | ce22ad8b | `val<bool>` conjunctions in loop headers, data-dependent exit |
| `LoopIndexOuter` — doubly-nested body reads the enclosing loop's index | 1ef02487 | outer-frame values living across inner back edges as extra block args |

Infrastructure: `Node` gained a 4th kid slot; loop frames carry a second accumulator (`acc2 == acc` for single-acc kinds, so the `lb` leaf is legal in any loop body); all predicates fixed and mirrored exactly between oracle and traced kernel (NaN-safe).

## Verification

- Per-extension surveys: 1500 and 2500 inputs, 0 findings.
- Final sweep: 5000-input survey + 2000-input built-in corpus across interpreter + MLIR + C++ + BC + AsmJit, plus full ctest suite — results recorded in `results.tsv`.
