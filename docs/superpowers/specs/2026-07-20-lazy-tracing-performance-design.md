# Lazy-Tracing Performance Optimization Design

## Goal

Improve the average tracing latency of `LazyTraceContext` on the existing `Tracing Benchmark` suite while preserving
trace semantics and preventing material workload-specific regressions.

Success requires both conditions:

1. The geometric mean of candidate-to-baseline latency ratios across all 14 `completing_trace_*` workloads is below
   `1.0`, and the 95% upper bootstrap bound for that aggregate is also below `1.0`.
2. No individual workload has a candidate-to-baseline median latency ratio above `1.10`.

The work stops after local verification. It does not push, publish, deploy, or open a pull request.

## Scope

The optimization loop may change:

- `nautilus/src/nautilus/tracing/LazyTraceContext.cpp`
- `nautilus/src/nautilus/tracing/LazyTraceContext.hpp`
- shared trace-generation and representation code under `nautilus/src/nautilus/tracing/` when a lazy-tracing hot path
  requires it
- focused tracing tests needed to preserve semantics

Benchmark infrastructure and raw results live in the timestamped `autoresearch/` run directory rather than in the
production source tree.

The following are outside the scope:

- a new tracing mode
- platform-specific stack copying or execution-context switching
- broad compiler, IR-pass, or backend optimization
- changes that intentionally alter reference traces or generated IR
- exception-based tracer optimization unless a shared representation change makes a small corresponding adjustment
  unavoidable

## Architecture

`LazyTraceContext` keeps its current RECORD, FOLLOW, and passive execution behavior. Existing trace and IR fixtures are
the compatibility contract. Optimizations target implementation costs without changing externally observable trace
structure.

Each experiment addresses one hot-path concern, such as:

- repeated mode or state access
- snapshot construction and tag-map lookup
- FOLLOW cursor traversal
- alive-variable reference tracking
- avoidable container growth or clearing
- operation and block creation

Shared `ExecutionTrace` or symbolic-execution representation changes are acceptable only when they directly reduce a
lazy-tracing cost and remain compatible with existing consumers.

The loop starts with low-risk hot-path changes. It does not switch to the older replay-log or stack-checkpoint designs.
Compact representation ideas from those experiments may be adopted only when they fit the current lazy-tracing model
and retain its semantics.

## Benchmark Protocol

### Baseline

Build `nautilus-benchmarks` in the existing `build-perf` Release configuration, which uses Clang 21. Copy the resulting
executable into the timestamped orchestrator directory. This copied executable is the immutable baseline for the run.

Before optimization, compare the baseline executable against itself to prove that the parser, workload discovery,
process execution, and ratio calculation work.

### Working Signal

For each experiment:

1. Build the candidate `nautilus-benchmarks` executable.
2. Run the immutable baseline and current candidate in alternating order.
3. Use three independent A/B process rounds.
4. In each process, run the Catch2 `Tracing Benchmark` with five benchmark samples per workload and a fixed warmup.
5. Parse only the 14 `completing_trace_*` means and normalize all units to nanoseconds.
6. For every workload, use the median across its three independent process rounds.
7. Compute the geometric mean of the 14 candidate-to-baseline median ratios.

Alternating process order counters first-run and thermal drift. Independent launches prevent in-process sample
correlation from being mistaken for independent evidence.

### Experiment Acceptance

An experiment is kept only when:

- its geometric-mean ratio is strictly lower than the incumbent ratio;
- every per-workload median ratio is at most `1.10`;
- the tracing/reference guard passes; and
- the benchmark harness completes and parses all 14 workloads.

Otherwise, the experiment commit is reverted. A failed process or parser gets one retry. A second failure classifies
the experiment as a crash and reverts it.

### Final Held-Out Verification

After the loop converges, repeat the comparison with nine fresh A/B process rounds and ten Catch2 samples per workload.
The final run uses new Catch2 seeds and raw output files that were not used to choose experiments. It applies the same
geometric-mean and 10% per-workload rules. It also performs 20,000 paired bootstrap resamples of the nine A/B round
indices with seed `20260720`; each resample recomputes the per-workload medians and their geometric mean. The
95th-percentile resampled aggregate must be below `1.0`.

The pinned success predicate is:

```sh
python3 autoresearch/orchestrator-260720-1326/lazy_trace_bench.py compare \
  --baseline autoresearch/orchestrator-260720-1326/baseline/nautilus-benchmarks \
  --candidate build-perf/nautilus/test/benchmark/nautilus-benchmarks \
  --rounds 9 --samples 10 --max-regression-pct 10 \
  --require-ratio-below 1.0 --require-upper-ci-below 1.0
```

The comparison command rebuilds the candidate before measuring it. It succeeds only when it prints a numeric ratio
below `1.0`, the 95% upper bootstrap bound is below `1.0`, and every workload ratio is at most `1.10`.

## Autoresearch Loop

The goal archetype is `optimize-metric`, even though the generic keyword router classified the original wording as
`build-feature`. The presence of an existing numeric benchmark and a lower-is-better success predicate makes
`optimize-metric` the applicable route.

The loop runs at most 20 experiments and stops earlier after five consecutive experiments fail to improve the
incumbent. Every iteration:

1. Reviews the current results table and recent experiment commits.
2. Forms one performance hypothesis.
3. Makes one focused change.
4. Runs the CMake `format` target as required for every code change.
5. Commits it as `experiment: <description>`.
6. Runs the semantic guard.
7. Measures the working performance signal.
8. Keeps or reverts the commit and appends the result to the TSV ledger.

The run records raw benchmark output, parsed per-workload values, aggregate ratios, commit SHAs, guard status, and
keep/discard decisions under `autoresearch/`. These run artifacts are not staged with source commits.

## Correctness and Regression Testing

The per-iteration semantic guard runs the tracing/reference tests, including static, value, function-call,
path-explosion, runtime-call, regression, control-flow, loop, bool, select, cast, pointer, enum, and expression trace
cases. A semantic guard failure rejects an experiment before its performance result can be accepted.

The exact guard command is:

```sh
ctest --test-dir build-perf/nautilus -R 'Trace Test$|^Regressions$' --output-on-failure
```

At convergence:

1. Build the affected targets in Release with Clang 21.
2. Run the CMake `format` target.
3. Run `./format.sh` as a formatting check.
4. Run `ctest --test-dir build-perf/nautilus --output-on-failure`.
5. Run the nine-round held-out performance predicate.
6. Inspect the final source diff and the autoresearch evaluation summary.

No performance result overrides a correctness or formatting failure.

## Deliverables

- retained lazy-tracing performance changes as local commits
- focused tests for any new internal behavior
- `autoresearch/orchestrator-260720-1326/orchestrator-state.json`
- a loop TSV ledger and raw per-iteration benchmark evidence
- an evaluation summary describing kept and rejected hypotheses
- final correctness, formatting, and held-out performance results
