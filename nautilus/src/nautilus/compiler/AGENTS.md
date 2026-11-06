# Compiler: notes for agents

- **Pipeline** (`CompilationPipeline.cpp`), default: the direct NIR tracer
  (`tracing/nir/`) → `conversion/` (SSA NIR, `dump.after_nir_conversion`) → a
  backend. A graph leaves `compileToIR` carrying only the NIR module
  (`IRGraph::getNirModule`). A backend with `compilesNir()` (`tbc`) compiles it
  as it is; for the others (`asmjit`, `mlir`, `cpp`) `prepareFor` first builds
  the 0.3 IR from it, in `NirToIR.cpp` (the temporary bridge: it infers integer
  signedness, turns landing pads into the destructors of their calls, and
  does not apply D10/D11), and runs the exception-region preparation. The
  optimization passes run on bridged IR only under `ir.runOptimizationPasses=true`.
- **0.3 pipeline** (fallback, `tracing.nirTracer=false`): trace →
  `TraceToIRConversionPhase` → IR passes (`ir/passes/`, run by `IRPassManager`
  up to `ir.maxPipelineIterations` rounds) → a backend. `irOptimizationLevel`
  lets a backend ask for fewer passes (MLIR takes only block-argument pruning).
  The `ir.*` options configure this pipeline. Both go when the last backend has
  its NIR port (PRs 7 and 8).
- **Tiered compilation** (`TieredCompiler.cpp`): `engine.backend=<x>` pins one
  backend. Otherwise tier 0 (`asmjit`, else `tbc`, else the interpreter) is
  compiled synchronously and tier 1 (MLIR) on a background thread, which swaps
  the module's executable. Finished promotion threads are reaped when the next
  promotion starts (#566).
- **IR** (`ir/`): `IRGraph`, `operations/` (one class per operation kind),
  `blocks/`. Adding or changing an operation touches many places: use the
  `ir-change` skill.
- **Passes** must keep the IR valid: run the tests with `NAUTILUS_IR_VERIFY=1`.
  Each pass has a test in `nautilus/test/ir-pass-tests`. `FunctionRewriter`
  rebuilds use tables per pass, which is expensive on large functions; prefer
  one linear walk.
- **Dumps and statistics:** `DumpHandler` writes the `dump.*` options;
  `CompilationStatistics` collects per-stage timings (`engine.logStatistics`,
  `NAUTILUS_STATISTICS_FILE`).
- **Options:** documented in `docs/options.md`; add new options there.
