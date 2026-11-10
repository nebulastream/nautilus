# Compiler: notes for agents

- **Pipeline** (`CompilationPipeline.cpp`), default: the direct NIR tracer
  (`tracing/nir/`) → `conversion/` (SSA NIR, `dump.after_nir_conversion`) → a
  backend. A graph leaves `compileToIR` carrying only the NIR module
  (`IRGraph::getNirModule`). A backend with `compilesNir()` (`tbc`, `cpp`, `asmjit`, `mlir`) compiles it
  as it is; for any other backend `prepareFor` first builds
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
- **Tiered compilation** (`TieredCompiler.cpp`, `PromotionService.cpp`): `engine.backend=<x>` pins one
  backend. Otherwise tier 0 (`asmjit`, else `tbc`, else the interpreter) is compiled synchronously and tier 1
  (MLIR) by the engine's `PromotionService`: a fixed pool of `engine.tiered.promotionThreads` workers (lower OS
  priority) that run `PromotionJob`s (`queued -> running -> done | failed | cancelled`, or `skipped` past
  `engine.tiered.maxQueuedPromotions`) and swap the module's executable. A job holds its module weakly; dropping
  the module cancels the job, and the backends check the token at stage boundaries. With the policy `eager` the job
  is queued right after conversion, before the tier-0 code generation, but only if both backends compile the
  immutable NIR (`canStartEarly`); tier 0 is installed only while `ModuleState::version` is 0. The service mutex is
  never held while compiling, publishing a status, joining or waiting. Promotions turn
  `mlir.enableMultithreading` off: workers own no thread pool (11 §4.1).
- **Tiered compilation** (`TieredCompiler.cpp`): `engine.backend=<x>` pins one
  backend. Otherwise tier 0 (`asmjit`, else `tbc`, else the interpreter) is
  compiled synchronously and tier 1 (MLIR) on a background thread, which swaps
  the module's executable. Finished promotion threads are reaped when the next
  promotion starts (#566).
- **Modules and linking** (`../link/`, 13 §4.4): `createModule(name)` registers a module with the engine's
  `ModuleRegistry` when it is compiled; `NautilusModule::importFunction` declares a call into another compiled module.
  `NautilusModule::compile` opens a `LinkSession` (the thread's current one), `CompilationPipeline::traceToNir` hands the
  converted module to it (`link::prepareModule`: module name, declared export attributes, link), and the resulting
  `ImportLink` rides on the `IRGraph` to the backends as `HostEnvironment::imports`. `ModuleState::link` owns the handle,
  so releasing a module releases its name and pins; `ModuleState::version.fetch_add` is the one point where a new
  executable is announced, and the registry's observer there republishes the slots of the importers. Anything that
  installs an executable in a `ModuleState` must bump the version while holding `ModuleState::mutex` (the tiered compiler
  does). Rules to keep: lock order is the state's mutex, then a handle's `dependentsMutex_`, then an `ImportLink`'s; the
  registry's mutex is never held while taking the state's.
- **IR** (`ir/`): `IRGraph`, `operations/` (one class per operation kind),
  `blocks/`. Adding or changing an operation touches many places: use the
  `ir-change` skill.
- **Passes** must keep the IR valid: run the tests with `NAUTILUS_IR_VERIFY=1`.
  Each pass has a test in `nautilus/test/ir-pass-tests`. `FunctionRewriter`
  rebuilds use tables per pass, which is expensive on large functions; prefer
  one linear walk.
- **Dumps and statistics:** `DumpHandler` writes the `dump.*` options;
  `CompilationStatistics` collects per-stage timings (`engine.logStatistics`,
  `NAUTILUS_STATISTICS_FILE`). Both sit on the path of every compile, so they must stay free when unused: see
  "Fixed cost per compile" in `backends/AGENTS.md` before adding a lookup or a `set` to the pipeline.
- **Options:** documented in `docs/options.md`; add new options there.
- **`explain()`:** `CompiledModule::explain()` (`nautilus/src/nautilus/link/ModuleExplain.cpp`) reads what a compile has
  already recorded: the statistics, the registry's NIR and the promotion's tier backends. A new thing worth showing to
  a user goes into the statistics or the module, then into the report, never into extra work on the compile path.
  `persistability` and `inlining` stay `not available yet` until M5.3 and M3.8 fill them.
