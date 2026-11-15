# AGENTS.md: optional passes and plugin pass hooks (`nautilus/src/nautilus/passes/`)

The design is `docs/plans/nautilus-0.4/03-conversion-and-passes.md` §5. The default pipeline runs **no pass** after the
conversion (`irPasses.pipelineIterations` is 0, checked exactly by `OptionalPassesTest`); everything here is opt-in and
must cost nothing when it is off. The library is part of `nautilus-nir`, so `nautilus-nir-tests` and `nautilus-nir-run`
use it without the engine.

| File | Content |
|---|---|
| `Pass.hpp`, `PassManager.cpp` | the copy-pass interface (`run(in) -> optional<Module>`: nothing means unchanged), `PassContext`, `PassManager` (groups, rounds until a fixed point, `PipelineReport`) |
| `MutableFunction.*` | `decode`/`encode`: a function as blocks of instructions with stable uids, written back with dense ids, landing pads last in the order of their first reference, unreachable blocks dropped |
| `Cfg.*` | CFG over normal edges, dominators, natural loops (innermost first), `ensurePreheader` |
| `Licm.cpp`, `StrengthReduction.cpp`, `Inline.cpp` | the three core optional passes; `Passes.*` builds them from the options |

## Rules that are easy to break

- **Passes are copy passes.** The input module is immutable and shared by the tiers of a module. Start the output with
  `copyForRewrite(in)` (`copyTables` alone leaves the signatures' parameter types pointing into a body section that is
  not copied), then `encode` every function in order.
- **Edit by uid, not by index.** Inserting a block or an instruction shifts indices; `MInst::uid` and `MBlock::uid` do not
  change. A pass that inserts a block (`ensurePreheader`, inlining) recomputes the `Cfg` afterwards.
- **`encode` is the layout.** It orders non-pad blocks as given and pads by first reference, and throws if a reachable
  instruction uses a value with no definition. A pass that breaks dominance is found by the verifier
  (`ir.verifyAfterEachPass` verifies after every pass that changed the module), not by `encode`.
- **Unwind entries when inlining.** `NO_CLEANUP` in a callee's normal block becomes the call's own entry; a `resume` in a
  callee pad becomes a `br` to the call's pad. Pads are never inlined into.
- **`bind.addr` stays with its module.** Across modules only a closure without `bind.addr`, extension operations and imports
  is copied (`Transplant` in `Inline.cpp`); the importer records the exporter hash for the cache key (13 §7.3).
- **Pipeline wiring is in `compiler/CompilationPipeline.cpp`** (`runLoweringPasses`, `runRequiredPluginPasses`). Required
  `afterConversion` plugin passes run once in `traceToNir`; everything else runs per compile in `compileIR`, on a copy,
  so tier 1 can run what tier 0 (latency-critical) skipped.
- **Tests.** `nautilus/test/nir-tests/PassTest.cpp` (structure, `NIR_PASS_PRINT=1 ctest -V -R <name>` prints the result),
  `nautilus/test/data/nir-run/*.nir` (every file also runs with all three passes on, `nir-run-*-optional-*`),
  `nautilus/test/pass-tests/` (plugin hooks, a binary of its own because the registry is process-wide).
