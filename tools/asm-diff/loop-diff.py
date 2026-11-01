#!/usr/bin/env python3
"""loop-diff: the instructions in the loops of the kernels, NIR lowering against the 0.3 lowering.

usage: loop-diff.py ASM_KERNELS_BINARY [--tolerance N]

Runs `nautilus-asm-kernels dump` (tools/asm-diff/kernels.cpp), which prints the assembly of every kernel on both
pipelines, finds the loops (a backward jump to a label of the same function) and counts the instructions from the label
to the jump. The lowering of NIR must not put more instructions into a loop than the 0.3 lowering did; this is what
catches a redundant move in a loop-carried chain, which costs run time that the code size does not show.
"""
import argparse
import re
import subprocess
import sys

JUMP = re.compile(r"^(?:short\s+)?j[a-z]+\s+(L\d+)$")
LABEL = re.compile(r"^(L\d+):$")
SKIP = re.compile(r"^(\.|after_asmjit_assembly)")


def loops(text):
    """The instruction count of every loop in one listing."""
    lines = [line.strip() for line in text.splitlines()]
    counts = []
    seen = {}  # label -> number of instructions before it
    n = 0
    for line in lines:
        if not line or SKIP.match(line):
            continue
        m = LABEL.match(line)
        if m:
            seen[m.group(1)] = n
            continue
        n += 1
        j = JUMP.match(line)
        if j and j.group(1) in seen:
            counts.append(n - seen[j.group(1)])
    return counts


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("binary")
    parser.add_argument("--tolerance", type=int, default=0, help="instructions a loop may grow by")
    args = parser.parse_args()
    out = subprocess.run([args.binary, "dump"], capture_output=True, text=True, check=True).stdout
    sections = {}
    key = None
    for line in out.splitlines():
        if line.startswith("== "):
            _, pipeline, kernel = line.split()
            key = (kernel, pipeline)
            sections[key] = []
        elif key is not None:
            sections[key].append(line)
    failed = False
    print(f"{'kernel':<22}{'0.3 loops':>16}{'NIR loops':>16}")
    for kernel in sorted({k for k, _ in sections}):
        old = loops("\n".join(sections.get((kernel, "asmjit-0.3"), [])))
        new = loops("\n".join(sections.get((kernel, "asmjit-nir"), [])))
        bad = sum(new) > sum(old) + args.tolerance
        failed = failed or bad
        print(f"{kernel:<22}{str(old):>16}{str(new):>16}{'  REGRESSION' if bad else ''}")
    if failed:
        print("loop-diff: a loop of the NIR lowering has more instructions than the 0.3 lowering's")
        return 1
    print("loop-diff: no loop has more instructions than in the 0.3 lowering")
    return 0


if __name__ == "__main__":
    sys.exit(main())
