#!/usr/bin/env python3
"""Turns NebulaStream systest benchmark runs into github-action-benchmark input.

Reads, for each backend the run used,
  * the BenchmarkResults.json that `systest -b` writes (best execution time per query), and
  * the JSON-lines file nautilus appends one compilation's statistics to when
    NAUTILUS_STATISTICS_FILE is set (one line per compiled pipeline),
and writes a `customSmallerIsBetter` list of metrics, each named after its
backend, plus a Markdown summary.

Usage: benchmark-report.py <metrics.json> <summary.md> <backend>=<BenchmarkResults.json>,<statistics.jsonl>...
"""
import json
import math
import statistics
import sys


def is_duration(key):
    return key.endswith(".ms") or key.endswith("Ms")


def percentile(values, fraction):
    ordered = sorted(values)
    return ordered[min(len(ordered) - 1, int(fraction * len(ordered)))]


def report(backend, results_path, statistics_path, metrics, summary):
    with open(results_path) as f:
        queries = json.load(f)
    with open(statistics_path) as f:
        compilations = [json.loads(line) for line in f if line.strip()]
    if not queries or not compilations:
        sys.exit(f"empty {backend} benchmark run: {len(queries)} queries, {len(compilations)} compilations")
    used = {c.get("backend.name") for c in compilations}
    if used != {backend}:
        sys.exit(f"the {backend} run compiled with {sorted(map(str, used))}")

    first = len(metrics)

    def add(name, unit, value, extra=""):
        metrics.append({"name": f"{backend} {name}", "unit": unit, "value": round(value, 3), "extra": extra})

    # Compilation: each phase summed over every pipeline the run compiled. Keys are
    # taken from the statistics themselves, so new nautilus phases show up on their own.
    phases = {}
    for compilation in compilations:
        for key, value in compilation.items():
            if is_duration(key) and isinstance(value, (int, float)):
                phases[key] = phases.get(key, 0.0) + value
    totals = [c["compilation.totalMs"] for c in compilations if "compilation.totalMs" in c]
    pipelines = f"{len(compilations)} pipelines"
    add("compilation: total", "ms", sum(totals), pipelines)
    add("compilation: median per pipeline", "ms", statistics.median(totals), pipelines)
    add("compilation: p90 per pipeline", "ms", percentile(totals, 0.9), pipelines)
    for key in sorted(phases):
        if key != "compilation.totalMs":
            add(f"compilation phase: {key}", "ms", phases[key], pipelines)
    for key in ("ir.operations", "mlir.module.ops"):
        counts = [c[key] for c in compilations if isinstance(c.get(key), int)]
        if counts:
            add(f"generated code: {key}", "ops", sum(counts), pipelines)

    # Execution: the worker's running-to-stopped time of each query, best of its rounds.
    times = [q["time"] * 1000 for q in queries]
    measured = f"{len(times)} queries"
    add("execution: total", "ms", sum(times), measured)
    add("execution: geometric mean per query", "ms", math.exp(statistics.fmean(math.log(max(t, 1e-3)) for t in times)), measured)
    for query in sorted(queries, key=lambda q: q["query name"]):
        add(f"execution: {query['query name']}", "ms", query["time"] * 1000)

    summary.write(f"### NebulaStream systests on {backend}\n\n{len(compilations)} pipelines compiled, {len(times)} queries measured.\n\n")
    summary.write("| Metric | Value |\n|---|---|\n")
    for metric in metrics[first:]:
        if not metric["name"].startswith(f"{backend} execution: ") or metric["extra"]:
            summary.write(f"| {metric['name']} | {metric['value']:,} {metric['unit']} |\n")
    slowest = sorted(queries, key=lambda q: q["time"], reverse=True)[:10]
    summary.write("\nSlowest queries:\n\n| Query | Execution |\n|---|---|\n")
    for query in slowest:
        summary.write(f"| {query['query name']} | {query['time'] * 1000:,.1f} ms |\n")
    summary.write("\n")


def main(metrics_path, summary_path, *runs):
    metrics = []
    with open(summary_path, "w") as summary:
        for run in runs:
            backend, paths = run.split("=", 1)
            results_path, statistics_path = paths.split(",", 1)
            report(backend, results_path, statistics_path, metrics, summary)
    with open(metrics_path, "w") as f:
        json.dump(metrics, f, indent=1)


if __name__ == "__main__":
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
