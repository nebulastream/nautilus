#!/usr/bin/env python3
"""The op-count gate of M3 (docs/plans/nautilus-0.4/03-conversion-and-passes.md §6, plan M3.10).

    opcount-report.py corpus.jsonl [--baseline nautilus/test/baselines/0.3/corpus.jsonl]... [--markdown FILE]

`corpus.jsonl` is the output of `nautilus-latency-corpus`. For every (function, N, backend) the NIR instructions after
the conversion (`nir_ops`) are compared with the operations 0.3 had after its full pass pipeline (`ir_ops` of the
recorded baseline). The report also shows the refold sweeps of the conversion (03 §3.2) and checks that no optional
pass ran: `pipeline_iterations` must be exactly 0.

Exit status 1 if a case has more operations than the baseline, or if a pass ran on a default compile. A case without a
baseline is listed and does not fail unless --require-baseline is given. Several --baseline files are merged; the
later one wins.
"""
import argparse
import json
import sys


def load(path):
    rows = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                r = json.loads(line)
                rows[(r["function"], r["n"], r["backend"])] = r
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("corpus")
    parser.add_argument("--baseline", action="append", default=[])
    parser.add_argument("--markdown")
    parser.add_argument("--require-baseline", action="store_true")
    args = parser.parse_args()

    corpus = load(args.corpus)
    baseline = {}
    for path in args.baseline:
        baseline.update(load(path))

    lines = ["| case | backend | NIR ops | 0.3 ops | ratio | refold sweeps | refolded | pass rounds |",
             "|---|---|---:|---:|---:|---:|---:|---:|"]
    failures = []
    for key in sorted(corpus):
        fn, n, backend = key
        row = corpus[key]
        ops = row.get("nir_ops", -1)
        if ops < 0:
            failures.append(f"{fn}/{n}/{backend}: the corpus output has no nir_ops (not a NIR build?)")
            continue
        base = baseline.get(key)
        base_ops = base["ir_ops"] if base else None
        ratio = f"{ops / base_ops:.3f}" if base_ops else "-"
        rounds = row.get("pipeline_iterations", -1)
        lines.append(f"| {fn}/{n} | {backend} | {ops} | {base_ops if base_ops else '-'} | {ratio} | "
                     f"{row.get('refold_sweeps', '-')} | {row.get('refolded_insts', '-')} | {rounds} |")
        if base_ops is None:
            if args.require_baseline:
                failures.append(f"{fn}/{n}/{backend}: no 0.3 baseline")
        elif ops > base_ops:
            failures.append(f"{fn}/{n}/{backend}: {ops} NIR instructions, 0.3 had {base_ops}")
        if rounds != 0:
            failures.append(f"{fn}/{n}/{backend}: irPasses.pipelineIterations is {rounds} on a default compile")
    report = "\n".join(lines)
    print(report)
    if args.markdown:
        with open(args.markdown, "a", encoding="utf-8") as f:
            f.write("### Op counts against the recorded 0.3 baseline\n\n" + report + "\n")
    if failures:
        print("\nFAILED:\n  " + "\n  ".join(failures), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
