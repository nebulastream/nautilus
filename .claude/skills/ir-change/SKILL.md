---
name: ir-change
description: Checklist for adding or changing a Nautilus IR operation or trace operation, so that every stage and backend handles it. Use when adding an operation kind, changing an operation's operands, types or semantics, or adding a flag to an existing operation.
---

# Add or change an IR operation

An operation is handled in more than ten places. Missing one compiles fine and
fails only on one backend or one configuration. Find all places for a similar
existing operation first:

```bash
grep -rln "SelectOp\b\|SelectOperation\|Op::SELECT" nautilus/src nautilus/include
```

## Checklist

**Frontend**
- [ ] Trace opcode in `nautilus/include/nautilus/tracing/Operations.hpp`
      (and its `toString`), and the `val<T>` operator or function that records it.
- [ ] `nautilus/src/nautilus/tracing/phases/IRConversionSupport.hpp` and
      `TraceToIRConversionPhase.cpp`: trace op → IR operation.

**IR**
- [ ] Operation class in `compiler/ir/operations/` (+ its `CMakeLists.txt`),
      the `OperationType` entry in `operations/Operation.hpp`, and
      `operations/OperationProperties.hpp` (effects, commutativity, …).
- [ ] `compiler/ir/OperationDispatcher.hpp`.
- [ ] Printer (`compiler/ir/IRGraph.cpp`) and `util/GraphVizUtil.cpp`.
- [ ] `compiler/ir/passes/IRVerifier.cpp`: operand and type rules.
- [ ] Passes that must know it: `ConstantFoldingAndCopyPropagationPass`,
      `AlgebraicSimplificationPass`, `LocalCSEPass`, and the effect analyses
      (`OperationEffects.cpp`, `FunctionAttributeInferencePass`,
      `NoThrowInferencePass`) if it reads, writes or may throw.

**Backends** (each one, or the build or tests fail on it)
- [ ] `asmjit`: `X64LoweringProvider` and `A64LoweringProvider` (.hpp/.cpp).
- [ ] `tbc`: `TBCOpcodes.hpp`, `TBCLoweringProvider.cpp`, `TBCInterpreter.cpp`,
      `TBCCode.cpp`, and the copy-and-patch stencils in `tbc/jit/`
      (`TBCStencilSource.cpp`, then the `tbc-regen-stencils` target).
- [ ] `bc`: `ByteCode.hpp`, `BCLoweringProvider`.
- [ ] `mlir`: `MLIRLoweringProvider`.
- [ ] `cpp`: `CPPLoweringProvider`.

**Tests and tools**
- [ ] Execution test in `nautilus/test/execution-tests` running on every backend.
- [ ] A pass test in `nautilus/test/ir-pass-tests` for any folding rule.
- [ ] Reference dumps if traces change (`regenerate-dumps` skill).
- [ ] The differential fuzzer: generator and both evaluators in
      `nautilus/test/fuzz` (`Ast.hpp`, `EvalNative.hpp`, `EvalNautilus.hpp`),
      then `nautilus-fuzz-seedgen --check`.
- [ ] `docs/` for user-visible operations.

## Verify

Build `dev` and `full` (MLIR), run `nautilus-execution-tests` with
`NAUTILUS_IR_VERIFY=1`, `nautilus-ir-pass-tests` and the fuzzer replay; build
`nocompiler` if you touched headers that it compiles.

In Nautilus 0.4 the IR becomes NIR; the checklist there follows
`docs/plans/nautilus-0.4/13-nir-spec.md` on the `nautilus-0.4` branch.
