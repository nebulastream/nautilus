#!/usr/bin/env python3
"""Runs each query of query-shapes.txt on its own, so its compilation and execution
time can be attributed to it, and writes one JSON record per query.

Every query runs `rounds` times as its own `systest -b` process inside the NebulaStream
container, with NAUTILUS_BACKEND selecting the backend and NAUTILUS_STATISTICS_FILE
collecting the statistics of each pipeline nautilus compiles for it. The systest command
line is taken from the `systest_benchmark_small` ctest entry, so it follows whatever
worker configuration NebulaStream's own benchmark uses.

Usage: run-query-shapes.py <container> <build-dir> <systest-dir> <shapes.txt> <backend> <rounds> <out.json>
  build-dir and systest-dir are absolute paths, the same inside and outside the container.
"""
import json
import os
import re
import subprocess
import sys


def queries(path):
    """The file's queries in systest's numbering: every SELECT or EXPLAIN statement,
    plus one for the second half of each differential (`====`) block."""
    lines = open(path).read().split("\n")
    found = []
    i = 0
    while i < len(lines):
        if lines[i].strip() == "====":
            found.append("")
            i += 1
        elif re.match(r"^\s*(SELECT|EXPLAIN)\b", lines[i], re.I):
            statement = []
            while i < len(lines) and lines[i].strip() not in ("----", "===="):
                statement.append(lines[i])
                i += 1
            found.append(" ".join(" ".join(statement).split()))
        else:
            i += 1
    return found


def resolve(systest_dir, test_file, sql_prefix):
    matches = [n for n, sql in enumerate(queries(os.path.join(systest_dir, test_file)), 1) if sql.startswith(sql_prefix)]
    return matches[0] if len(matches) == 1 else None


def systest_command(container, build_dir):
    listing = subprocess.run(
        ["docker", "exec", container, "ctest", "--test-dir", build_dir, "--show-only=json-v1", "-R", "^systest_benchmark_small$"],
        check=True, capture_output=True, text=True).stdout
    (test,) = json.loads(listing)["tests"]
    command = [arg for arg in test["command"] if not arg.startswith("--workingDir=")]
    split = command.index("--")
    return command[:split], command[split:]


def main(container, build_dir, systest_dir, shapes_path, backend, rounds, out_path):
    before, worker_args = systest_command(container, build_dir)
    work_dir = os.path.join(os.path.dirname(os.path.abspath(out_path)), f"query-shapes-{backend}")
    os.makedirs(work_dir, exist_ok=True)
    records, failed = [], []
    for line in open(shapes_path):
        if not line.strip() or line.startswith("#"):
            continue
        label, test_file, sql_prefix = (part.strip() for part in line.split("|", 2))
        number = resolve(systest_dir, test_file, sql_prefix)
        if number is None:
            # NebulaStream changed the query; the label needs updating, but that is no
            # reason to fail the build.
            print(f"::warning::query shape '{label}': no unique query in {test_file} starts with '{sql_prefix}'")
            continue
        name = f"{test_file.removesuffix('.test')}:{number}"
        runs = []
        for round_number in range(int(rounds)):
            statistics_file = os.path.join(work_dir, "statistics.jsonl")
            results_file = os.path.join(work_dir, "BenchmarkResults.json")
            for stale in (statistics_file, results_file):
                if os.path.exists(stale):
                    os.remove(stale)
            run = subprocess.run(
                ["docker", "exec", "-e", f"NAUTILUS_BACKEND={backend}", "-e", f"NAUTILUS_STATISTICS_FILE={statistics_file}",
                 container, *before, f"--workingDir={work_dir}", "-t", f"{os.path.join(systest_dir, test_file)}:{number}",
                 *worker_args],
                capture_output=True, text=True)
            measured = json.load(open(results_file)) if os.path.exists(results_file) else []
            if run.returncode != 0 or [m["query name"] for m in measured] != [name]:
                print(run.stdout[-4000:], run.stderr[-4000:], sep="\n")
                print(f"::error::query shape '{label}' ({name}) failed on {backend} in round {round_number + 1}")
                failed.append(label)
                break
            compilations = [json.loads(l) for l in open(statistics_file) if l.strip()]
            phases = {}
            for compilation in compilations:
                for key, value in compilation.items():
                    if (key.endswith(".ms") or key.endswith("Ms")) and isinstance(value, (int, float)):
                        phases[key] = phases.get(key, 0.0) + value
            runs.append({
                "execution_ms": measured[0]["time"] * 1000,
                "compilation_ms": phases.get("compilation.totalMs", 0.0),
                "pipelines": len(compilations),
                "phases_ms": phases,
            })
        if runs and len(runs) == int(rounds):
            records.append({"label": label, "query": name, "backend": backend, "runs": runs})
            print(f"{label:45} {name:60} pipelines {runs[0]['pipelines']:2}  "
                  f"compile {min(r['compilation_ms'] for r in runs):8.1f} ms  execute {min(r['execution_ms'] for r in runs):8.1f} ms")
    with open(out_path, "w") as f:
        json.dump(records, f, indent=1)
    if failed:
        sys.exit(f"{len(failed)} query shapes failed on {backend}: {', '.join(failed)}")


if __name__ == "__main__":
    if len(sys.argv) != 8:
        sys.exit(__doc__)
    main(*sys.argv[1:])
