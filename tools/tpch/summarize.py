#!/usr/bin/env python3
"""Summarizes the compilation statistics of a TPC-H run (nautilus-duck) for the M2 gates.

Input: the JSON lines that NAUTILUS_STATISTICS_FILE collects (one flat object per compilation, see
CompilationStatistics::toJson). Output: one JSON object with, over all compilations,

  compilations   number of statistics records
  tracing_ms     total tracing time
  rerun_share    re-run time over (first run + re-run) time (0.3 and 0.4 report tracing.firstRun.ms/tracing.rerun.ms)
  copies         recorded copies (tracing.copies)
  operations     recorded operations (tracing.operations)
  ir_operations  operations after the conversion and passes (ir.operations for 0.3, conversion.instructions for 0.4)
  replayed, paused, snapshots   where the tracer reports them (0.4 only; the keys of 0.3 are absent)

Usage: summarize.py STATS.jsonl [--baseline BASELINE.json] [--gate]
  --baseline   a summary of 0.3 (the same script on a run against `main`); adds the ratios to it
  --gate       exit 1 unless the gates of the plan hold (docs/plans/nautilus-0.4/16-implementation-plan.md M2.3, M2.8):
               recorded copies at least a quarter below the baseline and a re-run share of at most 35% of tracing
"""
import argparse
import json
import sys


def number(record, *keys):
    for key in keys:
        value = record.get(key)
        if isinstance(value, (int, float)):
            return value
    return None


def summarize(path):
    totals = {
        "compilations": 0,
        "tracing_ms": 0.0,
        "first_run_ms": 0.0,
        "rerun_ms": 0.0,
        "copies": 0,
        "operations": 0,
        "ir_operations": 0,
        "replayed": 0,
        "paused": 0,
        "snapshots": 0,
    }
    with open(path) as lines:
        for line in lines:
            line = line.strip()
            if not line:
                continue
            record = json.loads(line)
            totals["compilations"] += 1
            for name, keys in (
                ("tracing_ms", ("tracing.ms",)),
                ("first_run_ms", ("tracing.firstRun.ms",)),
                ("rerun_ms", ("tracing.rerun.ms",)),
                ("copies", ("tracing.copies",)),
                ("operations", ("tracing.operations",)),
                ("ir_operations", ("conversion.instructions", "ir.operations")),
                ("replayed", ("tracing.replayedOperations",)),
                ("paused", ("tracing.pausedOperations",)),
                ("snapshots", ("tracing.snapshots",)),
            ):
                value = number(record, *keys)
                if value is not None:
                    totals[name] += value
    passes = totals["first_run_ms"] + totals["rerun_ms"]
    totals["rerun_share"] = totals["rerun_ms"] / passes if passes > 0 else 0.0
    return totals


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("stats")
    parser.add_argument("--baseline")
    parser.add_argument("--gate", action="store_true")
    args = parser.parse_args()

    summary = summarize(args.stats)
    failures = []
    if args.baseline:
        with open(args.baseline) as f:
            base = json.load(f)
        ratios = {}
        for key in ("tracing_ms", "copies", "operations", "ir_operations"):
            if base.get(key):
                ratios[key] = summary[key] / base[key]
        summary["ratio_to_baseline"] = ratios
        if args.gate:
            if "copies" in ratios and ratios["copies"] > 0.75:
                failures.append("recorded copies are %.0f%% of the 0.3 baseline; the gate is 75%%" % (100 * ratios["copies"]))
    if args.gate and summary["rerun_share"] > 0.35:
        failures.append("the re-run share of tracing is %.0f%%; the gate is 35%%" % (100 * summary["rerun_share"]))
    summary["failures"] = failures
    json.dump(summary, sys.stdout, indent=2, sort_keys=True)
    sys.stdout.write("\n")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
