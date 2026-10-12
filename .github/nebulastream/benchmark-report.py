#!/usr/bin/env python3
"""Turns one NebulaStream systest benchmark run into github-action-benchmark input.

Reads
  * the BenchmarkResults.json that `systest -b` writes (best execution time per query), and
  * the JSON-lines file nautilus appends one compilation's statistics to when
    NAUTILUS_STATISTICS_FILE is set (one line per compiled pipeline),
and writes a `customSmallerIsBetter` list of metrics plus a Markdown summary.

Usage: benchmark-report.py <BenchmarkResults.json> <statistics.jsonl> <metrics.json> <summary.md>
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


def main(results_path, statistics_path, metrics_path, summary_path):
    with open(results_path) as f:
        queries = json.load(f)
    with open(statistics_path) as f:
        compilations = [json.loads(line) for line in f if line.strip()]
    if not queries or not compilations:
        sys.exit(f"empty benchmark run: {len(queries)} queries, {len(compilations)} compilations")

    metrics = []

    def add(name, unit, value, extra=""):
        metrics.append({"name": name, "unit": unit, "value": round(value, 3), "extra": extra})

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

    with open(metrics_path, "w") as f:
        json.dump(metrics, f, indent=1)

    with open(summary_path, "w") as f:
        f.write(f"### NebulaStream systest benchmark\n\n{len(compilations)} pipelines compiled, {len(times)} queries measured.\n\n")
        f.write("| Metric | Value |\n|---|---|\n")
        for metric in metrics:
            if not metric["name"].startswith("execution: ") or metric["extra"]:
                f.write(f"| {metric['name']} | {metric['value']:,} {metric['unit']} |\n")
        slowest = sorted(queries, key=lambda q: q["time"], reverse=True)[:10]
        f.write("\nSlowest queries:\n\n| Query | Execution |\n|---|---|\n")
        for query in slowest:
            f.write(f"| {query['query name']} | {query['time'] * 1000:,.1f} ms |\n")


if __name__ == "__main__":
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    main(*sys.argv[1:])
