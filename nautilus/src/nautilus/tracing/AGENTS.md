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
