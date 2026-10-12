#!/usr/bin/env python3
"""Benchmarks NebulaStream's systests against nautilus configurations.

  nes-bench.py run      runs the queries of a benchmark.toml under each of its
                        configurations and writes the measurements as JSON
  nes-bench.py report   turns those measurements into github-action-benchmark
                        metrics and a Markdown summary
  nes-bench.py compare  prints two measurement files side by side, e.g. a branch
                        against main

Each (round, query, configuration) is its own `systest -b` process on a single
NebulaStream node with `worker_threads` worker threads. NAUTILUS_OPTIONS carries
the configuration's options into every nautilus engine NebulaStream creates and
NAUTILUS_STATISTICS_FILE collects the statistics of every pipeline it compiles,
so one NebulaStream build serves every configuration. Configurations are
interleaved within a round, so drift on the machine spreads evenly over them.

The systest command line is the `systest_benchmark_small` ctest entry's and the
topology NebulaStream's single-node.yaml, so both follow NebulaStream's own
benchmark setup apart from the worker thread count.

Run `nes-bench.py <command> --help` for the arguments.
"""
import argparse
import json
import math
import os
import re
import shutil
import statistics
import subprocess
import sys
import time
import tomllib


# --- run --------------------------------------------------------------------------------------------------------------


def systest_queries(path):
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


def option_value(value):
    if isinstance(value, bool):
        return "true" if value else "false"
    return str(value)


class Systest:
    """Runs systest, inside the NebulaStream container when one is given."""

    def __init__(self, args, worker_threads, out_dir):
        self.container = args.container
        self.systest_dir = os.path.abspath(args.systest_dir)
        listing = self.execute(["ctest", "--test-dir", os.path.abspath(args.build_dir), "--show-only=json-v1",
                                "-R", "^systest_benchmark_small$"], {}, check=True).stdout
        (test,) = json.loads(listing)["tests"]
        command = [arg for arg in test["command"] if not arg.startswith("--workingDir=")]
        split = command.index("--")
        self.worker = [re.sub(r"^(--worker\.query_engine\.number_of_worker_threads=)\d+$", rf"\g<1>{worker_threads}", arg)
                       for arg in command[split:]]
        source = os.path.join(self.systest_dir, "configs", "topologies", "single-node.yaml")
        topology, replaced = re.subn(r"number_of_worker_threads:\s*\d+", f"number_of_worker_threads: {worker_threads}",
                                     open(source).read())
        if replaced != 1:
            sys.exit(f"{source} no longer sets number_of_worker_threads once; update {__file__}")
        topology_path = os.path.join(out_dir, "single-node.yaml")
        with open(topology_path, "w") as f:
            f.write(topology)
        self.systest = command[:split] + ["-c", topology_path]

    def execute(self, command, environment, check=False):
        if self.container:
            env_args = [arg for key, value in environment.items() for arg in ("-e", f"{key}={value}")]
            command = ["docker", "exec", *env_args, self.container, *command]
            environment = {}
        return subprocess.run(command, capture_output=True, text=True, check=check, env={**os.environ, **environment})

    def run(self, options, work_dir, statistics_file, locations=()):
        """Runs the given locations, or the whole suite, and returns
        (process, measured queries or None, compilations)."""
        shutil.rmtree(work_dir, ignore_errors=True)
        if os.path.exists(statistics_file):
            os.remove(statistics_file)
        environment = {
            "NAUTILUS_OPTIONS": ";".join(f"{key}={option_value(value)}" for key, value in options.items()),
            "NAUTILUS_STATISTICS_FILE": statistics_file,
        }
        process = self.execute([*self.systest, f"--workingDir={work_dir}",
                                *(["-t", *locations] if locations else []), *self.worker], environment)
        results = os.path.join(work_dir, "BenchmarkResults.json")
        measured = json.load(open(results)) if process.returncode == 0 and os.path.exists(results) else None
        compilations = [json.loads(l) for l in open(statistics_file) if l.strip()] if os.path.exists(statistics_file) else []
        return process, measured, compilations


def phase_sums(compilations):
    sums = {}
    for compilation in compilations:
        for key, value in compilation.items():
            if (key.endswith(".ms") or key.endswith("Ms")) and isinstance(value, (int, float)):
                sums[key] = sums.get(key, 0.0) + value
    return sums


def check_backend(configuration, compilations):
    """Every compilation must use a backend the configuration names, so a broken
    override cannot record another backend's numbers under this configuration."""
    options = configuration["options"]
    if options.get("engine.backend"):
        expected = {options["engine.backend"]}
    else:
        expected = {options[k] for k in ("engine.tier0.backend", "engine.tier1.backend") if options.get(k)}
    return not expected or {c.get("backend.name") for c in compilations} <= expected


def run(args):
    config = tomllib.load(open(args.config, "rb"))
    settings = config.get("run", {})
    rounds = args.rounds or settings.get("rounds", 5)
    configurations = [c for c in config["configuration"] if not args.configurations or c["name"] in args.configurations]
    queries = [q for q in config["query"] if not args.queries or re.search(args.queries, q["label"])]
    if args.configurations and len(configurations) != len(args.configurations):
        sys.exit(f"unknown configuration among {args.configurations}")
    for configuration in configurations:
        configuration.setdefault("options", {})

    out_dir = os.path.abspath(os.path.dirname(args.out) or ".")
    scratch = os.path.join(out_dir, "nes-bench")
    os.makedirs(scratch, exist_ok=True)
    systest = Systest(args, settings.get("worker_threads", 1), scratch)
    work_dir = os.path.join(scratch, "systest")
    statistics_file = os.path.join(scratch, "statistics.jsonl")
    result = {"rounds": rounds, "worker_threads": settings.get("worker_threads", 1),
              "configurations": {c["name"]: c["options"] for c in configurations}, "queries": [], "suites": []}
    failures = []

    # Resolve each query to its current number once; a query whose SQL changed in
    # NebulaStream is skipped with a warning rather than failing the build.
    located = []
    for query in queries:
        numbers = [n for n, sql in enumerate(systest_queries(os.path.join(systest.systest_dir, query["file"])), 1)
                   if sql.startswith(query["sql"])]
        if len(numbers) != 1:
            print(f"::warning::query '{query['label']}': {len(numbers)} queries in {query['file']} start with '{query['sql']}'")
            continue
        located.append((query, f"{query['file'].removesuffix('.test')}:{numbers[0]}",
                        f"{os.path.join(systest.systest_dir, query['file'])}:{numbers[0]}"))

    started = time.monotonic()
    for round_number in range(1, rounds + 1):
        for query, name, location in located:
            if round_number > query.get("rounds", rounds):
                continue
            for configuration in configurations:
                if (configuration["name"], query["label"]) in failures:
                    continue
                process, measured, compilations = systest.run(configuration["options"], work_dir, statistics_file, [location])
                if measured is None or [m["query name"] for m in measured] != [name] or not check_backend(configuration, compilations):
                    print(process.stdout[-3000:], process.stderr[-3000:], sep="\n")
                    level = "warning" if configuration.get("optional") else "error"
                    print(f"::{level}::query '{query['label']}' ({name}) failed under '{configuration['name']}' in round {round_number}")
                    failures.append((configuration["name"], query["label"]))
                    continue
                phases = phase_sums(compilations)
                result["queries"].append({
                    "configuration": configuration["name"], "label": query["label"], "query": name, "round": round_number,
                    "execution_ms": measured[0]["time"] * 1000, "compilation_ms": phases.get("compilation.totalMs", 0.0),
                    "pipelines": len(compilations), "phases_ms": phases,
                    "ir_operations": sum(c.get("ir.operations", 0) for c in compilations if isinstance(c.get("ir.operations"), int)),
                })
        print(f"round {round_number}/{rounds} done after {time.monotonic() - started:.0f} s", flush=True)

    # The whole suite, once per configuration that asks for it.
    for configuration in configurations:
        if not configuration.get("suite") or args.no_suite:
            continue
        process, measured, compilations = systest.run(configuration["options"], work_dir, statistics_file)
        if measured is None or not check_backend(configuration, compilations):
            print(process.stdout[-8000:], process.stderr[-8000:], sep="\n")
            level = "warning" if configuration.get("optional") else "error"
            print(f"::{level}::the systest suite failed under '{configuration['name']}'")
            failures.append((configuration["name"], "suite"))
            continue
        result["suites"].append({
            "configuration": configuration["name"],
            "queries": [{"query": m["query name"], "execution_ms": m["time"] * 1000} for m in measured],
            "pipelines": len(compilations), "phases_ms": phase_sums(compilations),
            "pipeline_compilation_ms": [c.get("compilation.totalMs", 0.0) for c in compilations],
            "ir_operations": sum(c.get("ir.operations", 0) for c in compilations if isinstance(c.get("ir.operations"), int)),
        })
        print(f"suite under {configuration['name']}: {len(measured)} queries after {time.monotonic() - started:.0f} s", flush=True)

    result["failures"] = [{"configuration": c, "query": q} for c, q in failures]
    with open(args.out, "w") as f:
        json.dump(result, f, indent=1)
    optional = {c["name"] for c in configurations if c.get("optional")}
    required = [(c, q) for c, q in failures if c not in optional]
    if required:
        sys.exit(f"{len(required)} runs failed: {', '.join(f'{c}/{q}' for c, q in required)}")


# --- report -----------------------------------------------------------------------------------------------------------


def geometric_mean(values):
    return math.exp(statistics.fmean(math.log(max(v, 1e-3)) for v in values))


def per_query(result):
    """{(configuration, label): (query, compilation median, execution best, pipelines, rounds)}"""
    grouped = {}
    for run in result["queries"]:
        grouped.setdefault((run["configuration"], run["label"]), []).append(run)
    return {key: (runs[0]["query"], statistics.median(r["compilation_ms"] for r in runs),
                  min(r["execution_ms"] for r in runs), runs[0]["pipelines"], len(runs))
            for key, runs in grouped.items()}


def report(args):
    result = json.load(open(args.results))
    metrics = []

    def add(name, unit, value, extra=""):
        metrics.append({"name": name, "unit": unit, "value": round(value, 3), "extra": extra})

    queries = per_query(result)
    configurations = list(result["configurations"])
    labels = list(dict.fromkeys(label for _, label in queries))
    for configuration in configurations:
        for label in labels:
            if (configuration, label) in queries:
                query, compilation, execution, pipelines, rounds = queries[(configuration, label)]
                extra = f"{query}, {pipelines} pipelines, {rounds} rounds"
                add(f"{configuration} query {label}: compilation", "ms", compilation, extra)
                add(f"{configuration} query {label}: execution", "ms", execution, extra)
    for suite in result["suites"]:
        name, extra = suite["configuration"], f"{suite['pipelines']} pipelines"
        add(f"{name} suite compilation: total", "ms", suite["phases_ms"].get("compilation.totalMs", 0.0), extra)
        add(f"{name} suite compilation: median per pipeline", "ms", statistics.median(suite["pipeline_compilation_ms"]), extra)
        for phase, value in sorted(suite["phases_ms"].items()):
            if phase != "compilation.totalMs":
                add(f"{name} suite compilation phase: {phase}", "ms", value, extra)
        add(f"{name} suite generated code: ir.operations", "ops", suite["ir_operations"], extra)
        times = [q["execution_ms"] for q in suite["queries"]]
        add(f"{name} suite execution: geometric mean per query", "ms", geometric_mean(times), f"{len(times)} queries")
        groups = {}
        for q in suite["queries"]:
            groups.setdefault(q["query"].split("/")[0], []).append(q["execution_ms"])
        for group, values in sorted(groups.items()):
            add(f"{name} suite execution: {group}/ geometric mean", "ms", geometric_mean(values), f"{len(values)} queries")

    with open(args.metrics, "w") as f:
        json.dump(metrics, f, indent=1)
    with open(args.summary, "w") as f:
        f.write(f"### NebulaStream systests per nautilus configuration\n\n"
                f"Single node, {result['worker_threads']} worker thread(s); compilation is the median of the rounds, "
                f"execution the best round.\n\n")
        for title, column in (("Compilation (ms)", 1), ("Execution (ms)", 2)):
            f.write(f"| {title} | " + " | ".join(configurations) + " |\n|---|" + "---|" * len(configurations) + "\n")
            for label in labels:
                cells = [f"{queries[(c, label)][column]:,.1f}" if (c, label) in queries else "–" for c in configurations]
                f.write(f"| {label} | " + " | ".join(cells) + " |\n")
            f.write("\n")
        if result["suites"]:
            f.write("| Whole suite | Pipelines | Compilation | Execution, geometric mean |\n|---|---|---|---|\n")
            for suite in result["suites"]:
                times = [q["execution_ms"] for q in suite["queries"]]
                f.write(f"| {suite['configuration']} | {suite['pipelines']} | "
                        f"{suite['phases_ms'].get('compilation.totalMs', 0.0) / 1000:,.1f} s | {geometric_mean(times):,.2f} ms |\n")


# --- compare ----------------------------------------------------------------------------------------------------------


def compare(args):
    base, head = per_query(json.load(open(args.base))), per_query(json.load(open(args.head)))
    print(f"{'configuration':20} {'query':40} {'compile base':>12} {'head':>9} {'ratio':>6}   {'exec base':>9} {'head':>9} {'ratio':>6}")
    for key in sorted(base.keys() & head.keys()):
        b, h = base[key], head[key]
        print(f"{key[0]:20} {key[1][:40]:40} {b[1]:12.1f} {h[1]:9.1f} {h[1] / max(b[1], 1e-3):6.2f}   "
              f"{b[2]:9.2f} {h[2]:9.2f} {h[2] / max(b[2], 1e-3):6.2f}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    commands = parser.add_subparsers(dest="command", required=True)
    run_parser = commands.add_parser("run", help="run the benchmark")
    run_parser.add_argument("--config", required=True, help="benchmark.toml")
    run_parser.add_argument("--build-dir", required=True, help="NebulaStream build directory with the systest binary")
    run_parser.add_argument("--systest-dir", required=True, help="NebulaStream's nes-systests directory")
    run_parser.add_argument("--container", help="run systest in this Docker container (paths must match inside it)")
    run_parser.add_argument("--configurations", nargs="+", help="only these configurations")
    run_parser.add_argument("--queries", help="only queries whose label matches this regex")
    run_parser.add_argument("--rounds", type=int, help="override the rounds of benchmark.toml")
    run_parser.add_argument("--no-suite", action="store_true", help="skip the whole-suite runs")
    run_parser.add_argument("--out", required=True, help="where to write the measurements (JSON)")
    run_parser.set_defaults(handler=run)
    report_parser = commands.add_parser("report", help="turn measurements into metrics and a summary")
    report_parser.add_argument("results")
    report_parser.add_argument("--metrics", required=True, help="github-action-benchmark customSmallerIsBetter JSON")
    report_parser.add_argument("--summary", required=True, help="Markdown summary")
    report_parser.set_defaults(handler=report)
    compare_parser = commands.add_parser("compare", help="compare two measurement files")
    compare_parser.add_argument("base")
    compare_parser.add_argument("head")
    compare_parser.set_defaults(handler=compare)
    args = parser.parse_args()
    args.handler(args)


if __name__ == "__main__":
    main()
