---
name: ir-change
description: Checklist for adding or changing a Nautilus IR operation or trace operation, so that every stage and backend handles it. Use when adding an operation kind, changing an operation's operands, types or semantics, or adding a flag to an existing operation.
---

# NIR operations (0.4, `nautilus/src/nautilus/nir/`)

NIR is not called by the engine until the cut-over (plan PR 6); the 0.3 checklist below still applies to the engine
until then. A NIR operation is a line in the one opcode table plus a case in each `switch` over `Op`. Change the spec
first (`docs/plans/nautilus-0.4/13-nir-spec.md` §5, and §6 for new verifier rules, §11 for change control).

- [ ] `nautilus/src/nautilus/nir/Opcodes.def`: code, text name, operand kinds of a/b/c, flags. Codes are never reused.
- [ ] Verifier: `VerifierFunction.cpp` (`FunctionChecker::instruction`: operand and result types, flags); a new
      rule gets a code in the spec's table in §6.6 and in `Diagnostics.def`, with a positive and a negative test in
      `nautilus/test/nir-tests/Verifier*Test.cpp`.
- [ ] Printer and parser: `Printer.cpp` (`instruction`), `Parser.cpp` (`op`).
- [ ] If it has an `extra` list or a table operand: `Operands.hpp` (`forEachRef`), `Rewriter.cpp` (`rewriteList`),
      `Canonicalize.cpp` (`list`, `field`), `Builder.cpp` (use counts), `VerifierFunction.cpp` (`listLength`).
- [ ] Tests: `RecordTest.cpp` counts the opcode table; `TestModules.hpp` builds a module that uses every opcode, so
      add the new one there; add a `.nir` file under `nautilus/test/data/nir-conformance/text/` (canonical form via
      `nautilus-nir-opt`) and, if the format changes, regenerate the golden images in a commit of their own.
- [ ] Docs: the table in spec §5, and `nautilus/src/nautilus/nir/AGENTS.md` if a rule changed.

# 0.3 IR: add or change an operation

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
