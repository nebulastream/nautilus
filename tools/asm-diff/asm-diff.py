#!/usr/bin/env python3
"""asm-diff: the code `asmjit` generates for the latency corpus, against the recorded 0.3 baseline.

The plan (docs/plans/nautilus-0.4/05-backends.md §4.3) requires no code-quality regression of the NIR lowering against
0.3's. The corpus driver records the size of the generated machine code (`asmjit.codeSize.bytes`) for every function
and size; this script compares it with the baseline of `nautilus/test/baselines/0.3/corpus.jsonl`. A function fails when
the total over its sizes grew by more than the tolerance (a few bytes of fixed difference at the smallest size are not a
regression of the lowering); single rows that grew are marked. Look at the instructions themselves with `dump.after_asmjit_assembly` (see the
`asm-diff` skill).

usage: asm-diff.py CURRENT.jsonl [--baseline FILE] [--tolerance FRACTION]
"""
import argparse
import json
import sys


def rows(path):
    out = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            record = json.loads(line)
            if record.get("backend") != "asmjit":
                continue
            size = record.get("stats", {}).get("asmjit.codeSize.bytes")
            if size is not None:
                out[(record["function"], record["n"])] = int(size)
    return out


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("current")
    parser.add_argument("--baseline", default="nautilus/test/baselines/0.3/corpus.jsonl")
    parser.add_argument("--tolerance", type=float, default=0.02, help="allowed growth of a function total, as a fraction of the baseline")
    args = parser.parse_args()

    baseline, current = rows(args.baseline), rows(args.current)
    if not current:
        print("asm-diff: no asmjit rows with a code size in", args.current)
        return 2
    failed = False
    totals = {}
    print(f"{'function':<20}{'n':>8}{'0.3 bytes':>12}{'now bytes':>12}{'ratio':>8}")
    for key in sorted(current):
        now = current[key]
        before = baseline.get(key)
        if before is None:
            print(f"{key[0]:<20}{key[1]:>8}{'-':>12}{now:>12}{'-':>8}  (not in the baseline)")
            continue
        total = totals.setdefault(key[0], [0, 0])
        total[0] += before
        total[1] += now
        print(f"{key[0]:<20}{key[1]:>8}{before:>12}{now:>12}{now / before:>8.3f}{'  larger' if now > before else ''}")
    for function, (before, now) in totals.items():
        bad = now > before * (1.0 + args.tolerance)
        failed = failed or bad
        print(f"total {function:<14}{before:>12}{now:>12}{now / before:>8.3f}{'  REGRESSION' if bad else ''}")
    if failed:
        print(f"asm-diff: the generated code of a function grew by more than {args.tolerance:.1%} against the 0.3 baseline")
        return 1
    print("asm-diff: no code-size regression against the 0.3 baseline")
    return 0


if __name__ == "__main__":
    sys.exit(main())
