#!/usr/bin/env python3
"""Remove redundant constant-tracer trace-golden overrides.

After a code change that tightens the constant tracer (e.g. P0's
pre-merge-hook attempt, Stage 2's stratification, a new folding path),
some previously-divergent trace outputs start matching the baseline
again. The committed override files under test/data/constant-tracer/
then become dead weight that's identical to the sibling baseline under
test/data/.

This script walks test/data/constant-tracer/, and for every
<category>/<group>/<name>.trace file deletes it iff an identical sibling
exists at test/data/<category>/<group>/<name>.trace. After running, the
override tree contains only files whose folded trace actually differs.

Typical workflow:
  1. Make tracer changes.
  2. ctest --test-dir build/nautilus (first run auto-writes new overrides
     where trace diverges).
  3. ctest --test-dir build/nautilus (second run verifies everything
     passes).
  4. python3 scripts/prune_redundant_overrides.py
  5. git diff / git commit the net change.

Usage:
  scripts/prune_redundant_overrides.py [--data-dir DIR] [--dry-run]
"""

from __future__ import annotations

import argparse
import filecmp
import pathlib
import sys


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        "--data-dir",
        default="nautilus/test/data",
        help="Path to the test data root (default: nautilus/test/data)",
    )
    parser.add_argument("--dry-run", action="store_true", help="Print what would be deleted without deleting")
    args = parser.parse_args(argv)

    data_dir = pathlib.Path(args.data_dir).resolve()
    override_root = data_dir / "constant-tracer"
    if not override_root.is_dir():
        print(f"No override directory at {override_root}; nothing to do.", file=sys.stderr)
        return 0

    checked = 0
    pruned = 0
    orphan = 0
    for override in sorted(override_root.rglob("*.trace")):
        rel = override.relative_to(override_root)
        baseline = data_dir / rel
        checked += 1
        if not baseline.is_file():
            # Baseline is missing — the override is the only authoritative
            # reference for this test. Keep it.
            orphan += 1
            continue
        if filecmp.cmp(override, baseline, shallow=False):
            action = "would prune" if args.dry_run else "prune"
            print(f"[{action}] {override.relative_to(data_dir.parent)} == baseline")
            pruned += 1
            if not args.dry_run:
                override.unlink()

    print()
    print(f"checked={checked} pruned={pruned} orphan-no-baseline={orphan}")
    if args.dry_run:
        print("(dry run — no files were deleted)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
