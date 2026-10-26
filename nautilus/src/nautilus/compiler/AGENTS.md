# Compiler: notes for agents

- **Pipeline** (`CompilationPipeline.cpp`): trace → `TraceToIRConversionPhase`
  → IR passes (`ir/passes/`, run by `IRPassManager` up to
  `ir.maxPipelineIterations` rounds) → a backend (`backends/`).
  `irOptimizationLevel` lets a backend ask for fewer passes (MLIR takes only
  block-argument pruning).
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
