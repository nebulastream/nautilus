---
name: debug-trace
description: Find out why a function written with `val<T>` traces slowly, fails to trace or produces a huge trace, by diagnostic code and statistics. Use when a compilation fails with a NAUT-T code, when tracing takes long, when tracing.replayedOperations or tracing.pausedOperations is large, or when a constant loop, path explosion or a value escaping a region() is suspected.
---

# Debugging a trace

1. **Get the diagnostics.** Run with `NAUTILUS_OPTIONS="engine.diagnostics=text;engine.logStatistics=true"` (or
   `engine.diagnostics=json` for a tool). Every code is in `docs/diagnostics.md` with its fix.
2. **Read the tracing statistics** (`engine.logStatistics=true`):
   - `tracing.iterations`, `tracing.maxScopeIterations`: passes of the traced body. Many passes for one scope mean one
     pass per explored path.
   - `tracing.replayedOperations`, `tracing.rerun.share`: the re-run cost. Above about a third of the tracing time, bound the
     replay: `region()` (`docs/region.md`) or the explicit constructs (`docs/control-flow.md`).
   - `tracing.pausedOperations`: the cost of the tail of finished passes; `tracing.unwindPausedPasses=true` removes it.
   - `tracing.copies.construct`: `val` copies by construction; take `const val<T>&` parameters.
   - `tracing.snapshots`: frame walks; explicit constructs take none inside their arms.
3. **By code:**
   - `NAUT-T001` constant loop: a plain C++ loop repeating `val` operations; use a `val` loop variable or a `static_val`.
   - `NAUT-T002`/`NAUT-T003` path explosion, replay share: region() or `If`/`While`/`For`.
   - `NAUT-T004` a value escaping a region or construct body: declare it outside, assign inside.
   - `NAUT-T005` an exit that is not inside an explicit loop, or that leaves a region().
4. **Look at the trace** when the codes do not explain it: `dump.after_nir_tracing=true;dump.console=true` prints the
   pre-SSA NIR; `docs/tracing.md` explains how to read it.
5. **Build the check into a test** under `nautilus/test/execution-tests/` (`TracingDiagnosticsTest.cpp` shows how to read
   the diagnostics of a compilation).
