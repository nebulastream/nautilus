# Tracing: notes for agents

The tracer turns a C++ function written with `val<T>` into an
`ExecutionTrace`, then `phases/TraceToIRConversionPhase.cpp` converts it into
the SSA `IRGraph`. Read `docs/tracing.md` and `docs/region.md` first.

- **Entry point:** `TraceContext` (`TraceContext.hpp/.cpp`). `runScope`
  executes a scope body once per unexplored branch direction; recorded
  decisions are replayed in *follow* mode, new ones are *recorded*, and after
  a control-flow merge the rest of the pass runs *paused*.
- **Tags** (`tag/`): every recorded operation is identified by a frame-pointer
  walk (`TagRecorder`, a trie of return addresses) plus static values, as a
  `Snapshot`. Equal snapshots mean a control-flow merge. The build therefore
  uses `-fno-omit-frame-pointer`, and inlining differences change traces:
  tracing behaves differently at `-O0` (#450, the `debug` preset).
- **Changing what is recorded changes the reference dumps** in
  `nautilus/test/data/*/tracing` and the IR dumps after it. Those tests only
  build with Clang (`nautilus-tracing-tests`). Regenerate dumps in a separate
  commit (`regenerate-dumps` skill).
- **Copies:** `val<T>` copy construction records an `ASSIGN`;
  `dump.copySites=true` shows where copies come from.
- **Tests:** `nautilus-tracing-unit-tests` for internals, `nautilus-tracing-tests`
  for dumps, `nautilus-execution-tests` for behavior, the differential fuzzer for
  mixed control flow.
- **Statistics:** `TraceContext` counts iterations, replayed and paused operations, snapshots and copies by kind
  (`TracingStatistics.hpp`) and times the first pass and the re-runs, split into follow, record and paused time. The
  compilation pipeline reports them as `tracing.*` (`engine.logStatistics=true`).
- **NIR:** `nir/NirTrace.cpp` maps a finished trace to pre-SSA NIR. `tracing.verifyNir=true` runs the NIR verifier
  on it for every compilation (`NAUTILUS_OPTIONS="tracing.verifyNir=true" ctest --preset dev` checks the mapping on
  the whole suite); `nautilus-tracing-tests` compares the text with `nautilus/test/data/*/nir`.

## The direct NIR tracer (`tracing/nir/NirTracer`, `FlatTrace`)

Since the cut-over the engine traces with this tracer: it records pre-SSA NIR directly (16-byte records, tag map,
replay cursor), and `CompilationPipeline::traceToNir` verifies (under `tracing.verifyNir` or the
`NAUTILUS_IR_VERIFY=1` hook) and converts the module. `tracing.nirTracer=false` selects the 0.3 tracer described above
(`TraceContext`, `ExecutionTrace`) and the IR passes, kept as a fallback for one release; the `nir-tbc` test backends
and the dump tests (`nautilus-tracing-tests`, which call `TraceContext::Trace` directly) still use it. When the layout
puts an assigning block before the block that declared a variable (a constant reached again on the second path),
`emitFunction` hoists the declaration to the entry block and leaves an assign at the original site.
`tracing.nirTracer=true` records pre-SSA NIR directly (16-byte records, tag map, replay cursor) instead of the 0.3
`TraceContext` + `toPreSSA`; the module then goes through verification and conversion into an empty IR graph for the
`nirtbc` pseudo-backend. Tests run it as the `nir-direct` / `nir-direct-jit` backends, the fuzzer as peers. When the
layout puts an assigning block before the block that declared a variable (a constant reached again on the second
path), `emitFunction` hoists the declaration to the entry block and leaves an assign at the original site.

**Debug sections (D14).** With `debug`, `perf` or `perf.sample` the tracer writes the optional debug sections of the
module (13 §4.9) in `SessionState::emitDebugData`, once per function: the region table (`debugRegions`: name, call-site
location, parent; the flat trace already keeps it for `region()`), the location of the function (`debugFunctions`) and the
region of every instruction, which is that of its block (`debugInstRegions`); `debugInstLocations` is `NONE` (the C++
location of each operation is not recorded: 0.3 never put it in DWARF either, which points at the IR text). The
conversion copies the regions of the instructions it emits from their pre-SSA block (`FunctionConverter::emit`) and the
passes that rebuild a function must do the same or the backends lose the regions. Off, a trace pays one branch per
function.

The thread keeps the tables of the NIR tracer between traces (the flat trace, the symbolic execution context, the
write-out scratch of `emitFunction`), so a trace allocates only what it outgrows; anything new that is per trace belongs
there too, bounded so that one huge trace does not tax the small ones after it. Measure with `nautilus-compile-bench`
and callgrind on `nautilus-tracing-corpus` (the `perf-investigation` skill).

## Frontend features of M2 (NIR tracer)

- **Paused passes (R3, R4, R2).** A paused scope publishes `detail::pausedFast` (`syncAlive`), so the holders of `val<T>`
  skip the reference counting; any new code that relies on `allocateValRef` while paused must say so there.
  `tracing.unwindPausedPasses` makes `pauseAndUnwind` throw `PausedPass` (derived from nothing), which `runScope`
  catches; a body that returns after the throw swallowed it (`tracing.unwindFallbacks`). Every place that finishes a pass
  calls `pauseAndUnwind`, except `traceScopeExit`, which runs after the body. A region or construct in a replay is found
  by its entry block (`FlatTrace::seekBlock`, `regionMemo`), not by a snapshot.
- **Explicit constructs (`control_flow.hpp`).** `traceIf`/`traceLoop` run the arms, header, body and step as child scopes
  (`runConstructScope`, kinds in `ScopeKind`) between blocks the construct creates; the construct is memoized by its entry
  block like a region. A construct scope is *optimistic*: `recordSnapshot()` returns the tag-less snapshot (which
  `TagMap` ignores) until `traceBool` meets a native branch, then `requestRestart` -> `restartScope` rolls the
  `FlatTrace` back to the checkpoint taken in `runScope` (`FlatTrace::checkpoint/rollback`). **Any new append to a block
  that is older than the scope must go through `FlatTrace::addPred` (or be reproduced in `rollback`), and any new table
  that grows while recording must be truncated there**, or a restart leaves stale state. `tracing.forceConstructRestarts`
  runs every construct scope through a restart: use it in the test of any change to this code.
- **Non-local exits** record the destructor calls of the owned values above the loop frame's depth (`emitExitDestructors`),
  then a jump (`Break`, `Continue`) or `ret` (`Return`), and pause the pass. They never touch `activeDestructors`.
- **Diagnostics.** `TracingDiagnostics.*` defines `NAUT-T…` codes (`docs/diagnostics.md`); the tracer raises errors through
  `NirTracer::fail` and collects the warnings of a session in `NirTraceResult::diagnostics`. A new user-facing tracer
  error gets a code, a catalog entry and a test in `TracingDiagnosticsTest.cpp`.
- **Constant data.** `traceData` interns a data object; the conversion folds loads from `data.addr` plus constant adds
  (`FunctionConverter::simplify`, `Op::Load`).
