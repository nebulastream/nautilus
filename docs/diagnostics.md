# Diagnostics

Tracing problems are reported with a stable code, the source location in the user's code, a message and a hint, for
example:

```text
NAUT-T001 constant loop at Query.cpp:42: the loop condition does not depend on a val; use a val loop variable, or static_val to unroll
```

Errors (`NAUT-T001`, `NAUT-T004`, `NAUT-T005`) fail the compilation with the text above as the message. Warnings
(`NAUT-T002`, `NAUT-T003`) do not: they are collected in the statistics of the compilation (`diagnostics.count`,
`diagnostics.<n>.code`, `.message`, `.hint`, `.location`).

`engine.diagnostics` selects how they are also written to standard error as they are raised: `off` (the default), `text`
(the line above) or `json` (one JSON object per line with the fields `code`, `title`, `message`, `hint`, `file`, `line`
and `function`, for tools). The location is the innermost frame outside Nautilus when the binaries carry debug
information (`ENABLE_STACKTRACE`), and otherwise the place the traced function was registered at.

Every code has a test that triggers it (`nautilus/test/execution-tests/TracingDiagnosticsTest.cpp`).

| Code | Kind | Meaning | Fix |
|---|---|---|---|
| `NAUT-T001` | error | **Constant loop.** An operation was reached again with the same call stack and the same static state, so the tracer took it for a loop whose condition never changes. Typically a plain C++ `for (int i = 0; i < 3; i++)` that repeats a `val` operation. | Loop over a `val` (the condition must depend on one), or use a `static_val` counter to unroll. |
| `NAUT-T002` | warning | **Path explosion.** A scope was run more than `tracing.diagnostics.maxPasses` times (default 256). | Wrap independent branches in `region()`, write them with `If`/`While`/`For` (`nautilus/control_flow.hpp`), or use `select`. |
| `NAUT-T003` | warning | **Replay share.** Re-runs replayed more than `tracing.diagnostics.replayedOperations` operations (default 100000) and took more than half of the pass time. | The same as `NAUT-T002`: bound the replay with `region()` or the explicit constructs. |
| `NAUT-T004` | error | **Value escapes a region.** A `val` created inside a `region()` body or an arm or body of an explicit construct is still alive at its end (or, rarer, the state alive at the end differs between the paths through it). | Declare the `val` outside, and assign to it inside. |
| `NAUT-T005` | error | **Invalid exit.** `Break()`/`Continue()` outside an explicit loop or in a loop's condition, an exit that would leave a `region()` body, or `Return()` inside one. | Move the exit out of the region, or use a `val<bool>` flag. |
