#!/usr/bin/env python3
"""Runs NebulaStream's systests as a benchmark on one nautilus backend.

All runs use a single worker node with a single worker thread. With the default
two-node topology a query's measured execution includes the source node's channel
registration backoff (up to 500 ms), and with several worker threads idle threads
add jumps of about 30 ms; neither has anything to do with the code nautilus
generated. The topology is NebulaStream's own single-node.yaml with its thread
count set to 1, and the systest command line is the `systest_benchmark_small`
ctest entry's, so both follow NebulaStream's benchmark configuration otherwise.

Writes into <out-dir>:
  suite-<backend>.json             BenchmarkResults.json of the whole suite, every query once
  suite-statistics-<backend>.jsonl nautilus' statistics of every pipeline the suite compiled
  query-shapes-<backend>.json      each query of the shapes file run on its own `rounds` times

Usage: run-benchmark.py <container> <build-dir> <systest-dir> <shapes.txt> <backend> <rounds> <out-dir>
  build-dir, systest-dir and out-dir are absolute paths, the same inside and outside the container.
"""
import json
import os
import re
import shutil
import subprocess
import sys

WORKER_THREADS = 1


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


def single_node_topology(systest_dir, out_dir):
    source = os.path.join(systest_dir, "configs", "topologies", "single-node.yaml")
    topology, replaced = re.subn(r"number_of_worker_threads:\s*\d+", f"number_of_worker_threads: {WORKER_THREADS}", open(source).read())
    if replaced != 1:
        sys.exit(f"{source} no longer sets number_of_worker_threads once; update {__file__}")
    path = os.path.join(out_dir, "single-node.yaml")
    with open(path, "w") as f:
        f.write(topology)
    return path


def systest_command(container, build_dir, topology):
    """The benchmark ctest entry's command, split around `--` into systest's own
    arguments (without its working directory) and the worker's."""
    listing = subprocess.run(
        ["docker", "exec", container, "ctest", "--test-dir", build_dir, "--show-only=json-v1", "-R", "^systest_benchmark_small$"],
        check=True, capture_output=True, text=True).stdout
    (test,) = json.loads(listing)["tests"]
    command = [arg for arg in test["command"] if not arg.startswith("--workingDir=")]
    split = command.index("--")
    worker = [re.sub(r"^(--worker\.query_engine\.number_of_worker_threads=)\d+$", rf"\g<1>{WORKER_THREADS}", arg)
              for arg in command[split:]]
    return command[:split] + ["-c", topology], worker


def run_systest(container, backend, command, work_dir, statistics_file, locations=()):
    before, worker = command
    shutil.rmtree(work_dir, ignore_errors=True)
    if os.path.exists(statistics_file):
        os.remove(statistics_file)
    run = subprocess.run(
        ["docker", "exec", "-e", f"NAUTILUS_BACKEND={backend}", "-e", f"NAUTILUS_STATISTICS_FILE={statistics_file}",
         container, *before, f"--workingDir={work_dir}", *(["-t", *locations] if locations else []), *worker],
        capture_output=True, text=True)
    results_file = os.path.join(work_dir, "BenchmarkResults.json")
    measured = json.load(open(results_file)) if run.returncode == 0 and os.path.exists(results_file) else None
    compilations = [json.loads(l) for l in open(statistics_file) if l.strip()] if os.path.exists(statistics_file) else []
    return run, measured, compilations


def main(container, build_dir, systest_dir, shapes_path, backend, rounds, out_dir):
    os.makedirs(out_dir, exist_ok=True)
    command = systest_command(container, build_dir, single_node_topology(systest_dir, out_dir))
    work_dir = os.path.join(out_dir, f"systest-{backend}")
    statistics_file = os.path.join(out_dir, f"suite-statistics-{backend}.jsonl")

    run, measured, _ = run_systest(container, backend, command, work_dir, statistics_file)
    if measured is None:
        print(run.stdout[-8000:], run.stderr[-8000:], sep="\n")
        sys.exit(f"the systest suite failed on {backend}")
    shutil.copy(os.path.join(work_dir, "BenchmarkResults.json"), os.path.join(out_dir, f"suite-{backend}.json"))
    print(f"suite on {backend}: {len(measured)} queries measured")

    shape_statistics = os.path.join(out_dir, f"shape-statistics-{backend}.jsonl")
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
            run, measured, compilations = run_systest(
                container, backend, command, work_dir, shape_statistics, [f"{os.path.join(systest_dir, test_file)}:{number}"])
            if measured is None or [m["query name"] for m in measured] != [name]:
                print(run.stdout[-4000:], run.stderr[-4000:], sep="\n")
                print(f"::error::query shape '{label}' ({name}) failed on {backend} in round {round_number + 1}")
                failed.append(label)
                break
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
        if len(runs) == int(rounds):
            records.append({"label": label, "query": name, "backend": backend, "runs": runs})
            print(f"{label:45} {name:60} pipelines {runs[0]['pipelines']:2}  "
                  f"compile {sorted(r['compilation_ms'] for r in runs)[len(runs) // 2]:8.1f} ms  "
                  f"execute {min(r['execution_ms'] for r in runs):8.1f} ms")
    with open(os.path.join(out_dir, f"query-shapes-{backend}.json"), "w") as f:
        json.dump(records, f, indent=1)
    if failed:
        sys.exit(f"{len(failed)} query shapes failed on {backend}: {', '.join(failed)}")


if __name__ == "__main__":
    if len(sys.argv) != 8:
        sys.exit(__doc__)
    main(*sys.argv[1:])
