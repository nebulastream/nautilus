---
name: perf-investigation
description: Measure and explain compile-time or generated-code performance in Nautilus, before and after a change. Use when asked why compilation is slow, when a change may affect tracing, IR passes or backend time, when the benchmark bot reports a ratio, or when comparing generated code quality.
---

# Investigate performance

## 1. Decide what to measure

| Question | Measure |
|---|---|
| where does compile time go? | per-stage statistics, then a callgrind profile |
| did my change slow compilation? | instruction counts before and after (callgrind), not wall time |
| did generated code change? | assembly or IR dumps before and after |
| is a benchmark-bot ratio real? | compare with benchmarks your change cannot affect |

**Instruction counts beat wall time** in shared environments: wall time
varies by ±30% between runs and runners; callgrind counts are stable.

## 2. Per-stage statistics

```bash
NAUTILUS_STATISTICS_FILE=/tmp/stats.jsonl ./your-binary   # one JSON line per compile
NAUTILUS_OPTIONS="engine.logStatistics=true" ./your-binary # human-readable report
```

Keys include tracing, IR generation, each pass (`irPasses.<pass>.ms`, runs,
changes) and backend sub-stages.

The ready-made harness (`-DENABLE_BENCHMARKS=ON`) is `nautilus-compile-bench [reps [kernel [backend [mode]]]]`
(`nautilus/test/benchmark/CompileTimeBench.cpp`): per kernel and size, tracing, conversion (or IR generation plus
passes) and backend time and microseconds per traced operation, for the 0.3 pipeline and the NIR tracer on one binary.
`nautilus-tracing-corpus function[:N] [reps [nir]]` runs the tracer alone for callgrind; the same kernels.
`nautilus-fixed-cost-bench [reps [kernel [backend]]]` is the harness for the cost that does not grow with the function:
one engine, one tiny kernel registered over and over, the median wall time of `registerFunction` next to every duration
statistic and the time no stage accounts for (`FIXED_COST_CALL=1` adds the first call). Compare two builds by running
their binaries alternately, and use callgrind (`NautilusModule::compile()` inclusive) for the numbers to quote.

## 3. A probe for one function shape

For a controlled measurement, write a temporary executable that compiles one
scalable function N times through the public API (`NautilusEngine`,
`registerFunction` / modules), and add it temporarily to
`nautilus/test/execution-tests/CMakeLists.txt`:

```cmake
add_executable(perf-probe PerfProbe.cpp)
target_link_libraries(perf-probe PRIVATE nautilus)
```

- Make operations come from **distinct call sites** (for example template
  recursion `step<N>` calling `step<N-1>`); repeating one call site in a C++
  loop is detected as a constant loop.
- Pin the backend with `engine.backend=<x>`.
- **Remove the probe and the CMake lines before committing.**

## 4. Profile with callgrind

```bash
valgrind --tool=callgrind --callgrind-out-file=/tmp/cg.out ./perf-probe
callgrind_annotate --inclusive=yes /tmp/cg.out | head -40
```

- Compare the total and the top inclusive functions before and after.
- A cost that grows faster than the input (2x input → 4x cost) is a
  superlinear path; find the loop in the top function (#565 was one).

## 4b. The instruction-count gate

CI gates the compile cost of the latency corpus as callgrind instruction counts against `main`'s
(`callgrind-gate` job, `nautilus/test/baselines/callgrind-gates.json`, plan `10-observability.md` §2.1). To run it
yourself, with valgrind and Clang installed (no container; about 15 minutes cold, a minute with a warm ccache):

```bash
tools/latency/run-callgrind-local.sh --out /tmp/cg-gate               # builds main's driver and yours, measures, compares
tools/latency/run-callgrind-local.sh --out /tmp/cg-gate --update      # after an intended change: lowers the thresholds
python3 tools/latency/callgrind-gate.py --self-test
```

The table lists, per case and metric (`total`, `tracing`, `ir` = conversion and passes, `backend`), the counts, the
ratio against 0.3, the threshold that fails and the plan's target that warns. Outside the pinned image the library
counts differ from the recorded ones, so a local run warns about the fingerprint and only the ratios are meaningful;
`--update` belongs to the image (use the counts the CI job uploads: `callgrind-gate.py update --ref 0.3.tsv --new
nir.tsv`). A real regression is fixed or, rarely, accepted with `--allow-raise` and a reason in the PR. One case:
`tools/latency/callgrind-corpus.sh -c 'straight 1000 tbc' <driver> out.tsv`.

## 5. Compare generated code

Dumps go to `$TMPDIR/dump/<compilation unit id>/` (`dump.file=true` is the
default; `dump.console=true` also prints them):

```bash
rm -rf /tmp/dump
NAUTILUS_OPTIONS="engine.backend=asmjit;dump.after_asmjit_assembly=true" ./perf-probe
mkdir -p before && cp -r /tmp/dump/* before/
# rebuild with the change, run again into after/, then
diff -r before after
```

Unit ids contain timestamps, so compare files by their order or name inside
each unit directory, not by directory name.

Use `dump.after_mlir_generation` / `dump.after_llvm_generation` for MLIR,
`dump.after_ir_creation` for the IR. Identical output is the strongest
evidence that a compile-time change is safe.

## 6. Benchmarks and the bot

- `ENABLE_BENCHMARKS=ON` (`bench` preset) builds `nautilus-benchmarks`
  (tracing, IR passes, backends, tiered compilation, execution).
- The PR benchmark bot reports ratios against `main`. Only the 200% alert is a
  signal. If many unrelated benchmarks move together, the runner changed, not
  the code.

## 7. Report

State: the function shape and size, the backend, before/after numbers
(instructions or medians of several runs), and whether generated code is
identical.
