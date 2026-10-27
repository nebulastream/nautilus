#!/usr/bin/env python3
"""Extracts the per-function IR baseline of 0.3 from the dumps of a test run.

Run the execution tests of `main` with the dump options set and a scratch TMPDIR:

    TMPDIR=/path/to/scratch NAUTILUS_OPTIONS="dump.all=true" ctest ...
    extract_ir_baseline.py /path/to/scratch/dump out.json

Every compilation unit leaves `after_ir_passes.nautilus`, the IR after 0.3's passes. For each function in it this records
the attributes that FunctionAttributeInferencePass derived (`readnone`, `willreturn`, `nounwind`, ...) and the number of
operations, keyed by the function's name and registration site (docs/plans/nautilus-0.4/16-implementation-plan.md,
M1.10: the inputs of M3.6 and of the NirChain checks of M3.2 and M3.3).
"""
import json
import os
import posixpath
import re
import sys

HEADER = re.compile(r"^(?P<name>\S+)\((?P<args>[^)]*)\) :(?P<type>\S+)(?: \[(?P<attrs>[^\]]*)\])?(?:\s+; at (?P<loc>.*?))? \{$")
BLOCK = re.compile(r"^Block_\d+\(.*\):$")


def location(raw):
    if not raw:
        return ""
    # the path from the repository's `nautilus/` directory on, without the checkout prefix and with `..` resolved
    path = posixpath.normpath(raw)
    parts = path.split("/")
    for i, part in enumerate(parts):
        if part == "nautilus" and i + 1 < len(parts) and parts[i + 1] in ("test", "include", "src"):
            return "/".join(parts[i:])
    return "/".join(parts[-3:])


def functions(path):
    name = None
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.rstrip("\n")
            m = HEADER.match(line)
            if m:
                name = {
                    "function": m.group("name"),
                    "location": location(m.group("loc")),
                    "attributes": sorted(a.strip() for a in (m.group("attrs") or "").split(",") if a.strip()),
                    "operations": 0,
                }
                continue
            if name is None:
                continue
            if line == "}":
                yield name
                name = None
            elif line.startswith("\t") and not BLOCK.match(line.strip()):
                name["operations"] += 1


# 0.3's attribute names as guarantee bits (13 §4.6.2): no read = 1|4, no write = 2|8, nounwind 16, willreturn 32
BITS = {"readnone": 1 | 4 | 2 | 8, "readonly": 2 | 8, "writeonly": 1 | 4, "nounwind": 16, "willreturn": 32}


def bits(attributes):
    value = 0
    for a in attributes:
        value |= BITS.get(a, 0)
    return value


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    root, out = argv[1], argv[2]
    result = {}
    conflicts = 0
    units = 0
    for unit in sorted(os.listdir(root)):
        path = os.path.join(root, unit, "after_ir_passes.nautilus")
        if not os.path.exists(path):
            continue
        units += 1
        for fn in functions(path):
            key = f'{fn["function"]} @ {fn["location"]}'
            entry = {"attributes": fn["attributes"], "operations": fn["operations"]}
            if key in result and result[key] != entry:
                conflicts += 1
                # the same function compiled for another backend or option set: keep the smaller IR
                if entry["operations"] >= result[key]["operations"]:
                    continue
            result[key] = entry
    with open(out, "w", encoding="utf-8") as f:
        json.dump({"units": units, "conflicts": conflicts, "functions": dict(sorted(result.items()))}, f, indent=1)
        f.write("\n")
    # the same facts in the form the execution tests read: name, location, guarantee bits, operations
    with open(os.path.splitext(out)[0] + ".tsv", "w", encoding="utf-8") as f:
        for key, entry in sorted(result.items()):
            name, _, loc = key.partition(" @ ")
            f.write(f'{name}\t{loc}\t{bits(entry["attributes"])}\t{entry["operations"]}\n')
    print(f"{len(result)} functions from {units} compilation units ({conflicts} differing duplicates)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
