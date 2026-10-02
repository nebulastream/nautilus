#!/usr/bin/env python3
"""Backend explorer: measure how Nautilus' backends and their options trade compilation latency for code quality.

Sweeps backend configurations over the benchmark kernels (runner/BenchmarkKernels.hpp), runs each
configuration in its own `nautilus-backend-explorer` process, and writes the results as JSON plus a self-contained
HTML report with a compile-time vs. execution-time frontier.

    # everything (backends, option toggles, MLIR/LLVM levels, LLVM pass ablation), then the report
    ./explore.py run --out results.json
    ./explore.py report results.json --out report.html

    # also search a pass pipeline bottom-up (greedy forward selection over LLVM passes)
    ./explore.py run --out results.json --sweep all --sweep llvm-greedy

    # a quick look: fewer repetitions and kernels
    ./explore.py run --quick --out quick.json --report quick.html

`run` appends to an existing results file and skips configurations it already holds, so an interrupted sweep
resumes where it stopped. See README.md for the measurement model and the sweeps.
"""

from __future__ import annotations

import argparse
import datetime
import glob
import hashlib
import json
import math
import os
import platform
import shutil
import statistics
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Iterable

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent
RUNNER_NAME = "nautilus-backend-explorer"

ALL_SWEEPS = ["backends", "backend-flags", "ir-passes", "mlir-levels", "llvm-ablation", "llvm-profile"]
OPTIONAL_SWEEPS = ["llvm-greedy"]

# Stats copied into the results per (config, kernel). Everything else the runner reports is dropped to keep the
# results (and the report that embeds them) small; per-IR-pass timings are kept since the report breaks them out.
KEPT_STATS_PREFIXES = (
    "tracing.ms", "ssaCreation.ms", "irGeneration.ms", "irPasses.", "frontend.totalMs", "backend.totalMs",
    "compilation.totalMs", "mlir.", "llvm.", "jit.", "cpp.", "asmjit.", "bc.", "tbc.", "ir.",
)


# ---------------------------------------------------------------------------------------------------------------
# Configurations


@dataclass
class Config:
    """One point of the sweep: a backend plus the options passed to it."""

    id: str
    backend: str
    group: str
    label: str
    options: dict = field(default_factory=dict)
    # Options always passed as strings (e.g. a textual LLVM pipeline that could look like a number).
    string_options: dict = field(default_factory=dict)
    # Free-form facts the report uses: the removed pass, the greedy step, the base level, ...
    meta: dict = field(default_factory=dict)

    def fingerprint(self) -> str:
        payload = json.dumps([self.backend, self.options, self.string_options], sort_keys=True)
        return hashlib.sha1(payload.encode()).hexdigest()[:12]

    def to_json(self) -> dict:
        return {
            "id": self.id, "backend": self.backend, "group": self.group, "label": self.label,
            "options": {**self.options, **self.string_options}, "meta": self.meta,
            "fingerprint": self.fingerprint(),
        }


def make_config(group: str, label: str, backend: str, options: dict | None = None,
                string_options: dict | None = None, meta: dict | None = None) -> Config:
    config = Config(id="", backend=backend, group=group, label=label, options=dict(options or {}),
                    string_options=dict(string_options or {}), meta=dict(meta or {}))
    config.id = f"{group}:{label}:{config.fingerprint()}"
    return config


def sweep_backends(info: dict) -> list[Config]:
    """Each backend at its defaults, the MLIR backend at each of its default optimization levels, the cpp backend
    at each -O level, and the configurations ExecutionBenchmark.cpp tracks. Labels name every option that differs
    from the backend's defaults."""
    backends = set(info["backends"])
    configs = []
    if "mlir" in backends:
        configs.append(make_config("backends", "mlir O3 (default)", "mlir"))
        for level in (0, 1, 2):
            configs.append(make_config("backends", f"mlir O{level}", "mlir", {"optimizationLevel": level},
                                       meta={"optLevel": level, "defaultLevel": True}))
        configs[0].meta = {"optLevel": 3, "defaultLevel": True}
    if "cpp" in backends:
        configs.append(make_config("backends", "cpp -O0 (default)", "cpp"))
        for level in (1, 2, 3):
            configs.append(make_config("backends", f"cpp -O{level}", "cpp", {"cpp.optimizationLevel": level}))
        configs.append(make_config("backends", "cpp -O3 -march=native", "cpp",
                                   {"cpp.optimizationLevel": 3, "cpp.nativeArch": True}))
    ir_licm_cse = {"ir.enableLICM": True, "ir.enableLocalCSE": True}
    if "bc" in backends:
        configs.append(make_config("backends", "bc (default)", "bc"))
        configs.append(make_config(
            "backends", "bc + IR LICM + local CSE, threaded dispatch, regfile reuse, superinstructions, immediates",
            "bc", {**ir_licm_cse, "bc.registerAllocator": True, "bc.dispatch": "threaded", "bc.regfileReuse": True,
                   "bc.superinstructions": True, "bc.immediates": True}))
    if "tbc" in backends:
        configs.append(make_config("backends", "tbc interp (default)", "tbc"))
        configs.append(make_config("backends", "tbc interp + IR LICM + local CSE", "tbc",
                                   {**ir_licm_cse, "tbc.mode": "interp"}))
        if info.get("tbcJit"):
            configs.append(make_config("backends", "tbc jit", "tbc", {"tbc.mode": "jit"}))
            configs.append(make_config("backends", "tbc jit + IR LICM + local CSE", "tbc",
                                       {**ir_licm_cse, "tbc.mode": "jit"}))
    if "asmjit" in backends:
        configs.append(make_config("backends", "asmjit (default)", "asmjit"))
        # Branch fusion, constant folding and cmov are asmjit defaults already; only the IR passes differ.
        configs.append(make_config("backends", "asmjit + IR LICM + local CSE", "asmjit", ir_licm_cse))
    return configs


# Backend-specific flags: (option, default, values to try besides the default).
BACKEND_FLAGS = {
    "asmjit": [
        ("asmjit.enablePostRAPeephole", True, [False]),
        ("asmjit.enableBranchFusion", True, [False]),
        ("asmjit.enableConstFolding", True, [False]),
        ("asmjit.enableSelectCmov", True, [False]),
        ("asmjit.enableIntrinsics", True, [False]),
    ],
    "bc": [
        ("bc.registerAllocator", True, [False]),
        ("bc.registerCoalescing", False, [True]),
        ("bc.dispatch", "call", ["threaded", "switch"]),
        ("bc.regfileReuse", False, [True]),
        ("bc.superinstructions", False, [True]),
        ("bc.immediates", False, [True]),
    ],
    "tbc": [
        ("tbc.registerAllocator", True, [False]),
        ("tbc.coalescing", True, [False]),
        ("tbc.superinstructions", True, [False]),
        ("tbc.immediates", True, [False]),
        ("tbc.dispatch", "auto", ["tailcall", "goto", "switch"]),
    ],
    "mlir": [
        ("mlir.inliner", True, [False]),
        ("mlir.enableMultithreading", True, [False]),
        ("mlir.enableIntrinsics", True, [False]),
    ],
}


def sweep_backend_flags(info: dict) -> list[Config]:
    """One backend flag changed at a time, everything else at its default."""
    configs = []
    for backend, flags in BACKEND_FLAGS.items():
        if backend not in info["backends"]:
            continue
        for option, default, values in flags:
            for value in values:
                configs.append(make_config("backend-flags", f"{backend} {option}={value}", backend, {option: value},
                                           meta={"flag": option, "value": value, "default": default}))
    if "tbc" in info["backends"] and info.get("tbcJit"):
        for option, default, values in BACKEND_FLAGS["tbc"]:
            if option == "tbc.dispatch":
                continue  # the dispatch skin only matters to the interpreter
            for value in values:
                configs.append(make_config("backend-flags", f"tbc jit {option}={value}", "tbc",
                                           {"tbc.mode": "jit", option: value},
                                           meta={"flag": option, "value": value, "default": default}))
    return configs


IR_PASS_TOGGLES = [
    ("ir.disableConstantFolding", True),
    ("ir.disableAlgebraicSimplification", True),
    ("ir.disableConstantBranchFolding", True),
    ("ir.disableEmptyBlockElimination", True),
    ("ir.disableBlockMerging", True),
    ("ir.disableDeadCodeElimination", True),
    ("ir.disableBlockArgumentPruning", True),
    ("ir.disableAttributeInference", True),
    ("ir.enableLocalCSE", True),
    ("ir.enableStrengthReduction", True),
    ("ir.enableLICM", True),
]


def sweep_ir_passes(info: dict) -> list[Config]:
    """Nautilus' own IR pipeline: all of it on/off per backend, and each pass toggled on the backends that take the
    full pipeline by default (asmjit, bc, tbc). MLIR runs only block-argument pruning unless forced."""
    configs = []
    for backend in ("mlir", "asmjit", "bc", "tbc"):
        if backend not in info["backends"]:
            continue
        for value in (True, False):
            configs.append(make_config("ir-passes", f"{backend} ir.runOptimizationPasses={value}", backend,
                                       {"ir.runOptimizationPasses": value},
                                       meta={"flag": "ir.runOptimizationPasses", "value": value}))
    for backend in ("asmjit", "bc"):
        if backend not in info["backends"]:
            continue
        for option, value in IR_PASS_TOGGLES:
            configs.append(make_config("ir-passes", f"{backend} {option}={value}", backend, {option: value},
                                       meta={"flag": option, "value": value}))
    if "mlir" in info["backends"]:
        for option in ("ir.enableLocalCSE", "ir.enableLICM", "ir.enableStrengthReduction"):
            configs.append(make_config("ir-passes", f"mlir full IR pipeline + {option}", "mlir",
                                       {"ir.runOptimizationPasses": True, option: True},
                                       meta={"flag": option, "value": True}))
    return configs


def sweep_mlir_levels(info: dict) -> list[Config]:
    """The LLVM IR optimization level crossed with the machine-code generation level."""
    if "mlir" not in info["backends"]:
        return []
    configs = []
    for level in (0, 1, 2, 3):
        for codegen in (0, 1, 2, 3):
            configs.append(make_config("mlir-levels", f"mlir O{level} / codegen {codegen}", "mlir",
                                       {"optimizationLevel": level, "mlir.codegenOptLevel": codegen},
                                       meta={"optLevel": level, "codegenLevel": codegen}))
        configs.append(make_config("mlir-levels", f"mlir O{level} / codegen 3 / no MLIR inliner", "mlir",
                                   {"optimizationLevel": level, "mlir.inliner": False},
                                   meta={"optLevel": level, "codegenLevel": 3, "inliner": False}))
    return configs


# ---------------------------------------------------------------------------------------------------------------
# Textual LLVM pipelines


@dataclass
class PassNode:
    """One element of a textual new-pass-manager pipeline: `name<params>(children)`."""

    name: str
    params: str = ""  # including the angle brackets, e.g. "<O3>"
    children: list["PassNode"] | None = None  # None for a leaf pass, a list for an adaptor

    def render(self) -> str:
        inner = "" if self.children is None else "(" + render_pipeline(self.children) + ")"
        return f"{self.name}{self.params}{inner}"


def parse_pipeline(text: str) -> list[PassNode]:
    """Parses `opt -passes=` syntax, as printed by `--print-pipeline-passes` / `mlir.recordLLVMPipeline`."""
    pos = 0

    def parse_list(closing: str | None) -> list[PassNode]:
        nonlocal pos
        nodes = []
        while pos < len(text):
            if closing is not None and text[pos] == closing:
                return nodes
            start = pos
            while pos < len(text) and text[pos] not in "<(),":
                pos += 1
            name = text[start:pos].strip()
            params = ""
            if pos < len(text) and text[pos] == "<":
                depth, start = 0, pos
                while pos < len(text):
                    depth += {"<": 1, ">": -1}.get(text[pos], 0)
                    pos += 1
                    if depth == 0:
                        break
                params = text[start:pos]
            children = None
            if pos < len(text) and text[pos] == "(":
                pos += 1
                children = parse_list(")")
                if pos >= len(text) or text[pos] != ")":
                    raise ValueError(f"unbalanced parentheses in pipeline at {pos}")
                pos += 1
            if name:
                nodes.append(PassNode(name, params, children))
            if pos < len(text) and text[pos] == ",":
                pos += 1
        if closing is not None:
            raise ValueError("unterminated adaptor in pipeline")
        return nodes

    return parse_list(None)


def render_pipeline(nodes: list[PassNode]) -> str:
    return ",".join(node.render() for node in nodes)


# Leaves that are bookkeeping rather than transformations; ablating them says nothing about code quality.
NON_TRANSFORM_LEAVES = {"require", "invalidate", "verify", "print", "annotation2metadata", "annotation-remarks"}


def leaf_pass_counts(nodes: list[PassNode]) -> dict[str, int]:
    counts: dict[str, int] = {}

    def visit(items: list[PassNode]) -> None:
        for node in items:
            if node.children is None:
                if node.name not in NON_TRANSFORM_LEAVES:
                    counts[node.name] = counts.get(node.name, 0) + 1
            else:
                visit(node.children)

    visit(nodes)
    return counts


def remove_pass(nodes: list[PassNode], name: str) -> list[PassNode]:
    """A copy of the pipeline without any leaf named @p name; adaptors left empty are dropped with it."""
    result = []
    for node in nodes:
        if node.children is None:
            if node.name != name:
                result.append(PassNode(node.name, node.params, None))
            continue
        children = remove_pass(node.children, name)
        if children:
            result.append(PassNode(node.name, node.params, children))
    return result


def sweep_llvm_ablation(info: dict, pipelines: dict[str, str], levels: Iterable[int]) -> list[Config]:
    """Leave-one-out over the default LLVM pipeline: for each distinct pass, the pipeline without any instance of
    it. The unchanged expanded pipeline is measured too, as the baseline the ablations compare against."""
    if "mlir" not in info["backends"]:
        return []
    configs = []
    for level in levels:
        text = pipelines.get(f"O{level}")
        if not text:
            continue
        nodes = parse_pipeline(text)
        configs.append(make_config("llvm-ablation", f"O{level} (expanded pipeline)", "mlir",
                                   string_options={"mlir.llvmPipeline": text},
                                   meta={"baseLevel": level, "removedPass": None}))
        for name, count in sorted(leaf_pass_counts(nodes).items()):
            ablated = render_pipeline(remove_pass(nodes, name))
            configs.append(make_config("llvm-ablation", f"O{level} without {name}", "mlir",
                                       string_options={"mlir.llvmPipeline": ablated or "verify"},
                                       meta={"baseLevel": level, "removedPass": name, "instances": count}))
    return configs


PROFILE_OPTION = {"mlir.recordPassTimings": True}


def sweep_llvm_profile(info: dict, pipelines: dict[str, str]) -> list[Config]:
    """The default O0-O3 pipelines, and O3 without the LLVM inliner, with every LLVM pass, analysis and
    machine-code generation pass timed (`mlir.recordPassTimings`). Kept apart from the timing sweeps: the
    instrumentation itself costs a little compile time."""
    if "mlir" not in info["backends"]:
        return []
    configs = [make_config("llvm-profile", f"profile: mlir O{level}", "mlir",
                           {**PROFILE_OPTION, "optimizationLevel": level},
                           meta={"profileOf": f"O{level}", "optLevel": level})
               for level in (0, 1, 2, 3)]
    if pipelines.get("O3"):
        without_inline = render_pipeline(remove_pass(parse_pipeline(pipelines["O3"]), "inline"))
        configs.append(make_config("llvm-profile", "profile: mlir O3 without inline", "mlir", PROFILE_OPTION,
                                   string_options={"mlir.llvmPipeline": without_inline},
                                   meta={"profileOf": "O3 without inline", "optLevel": 3}))
    return configs


# Candidates for the greedy pipeline search: (name, pipeline element, scope). Function-scope elements are grouped
# into one `function(...)` adaptor; module-scope ones stand on their own.
GREEDY_CANDIDATES = [
    ("sroa", "sroa<modify-cfg>", "function"),
    ("mem2reg", "mem2reg", "function"),
    ("instcombine", "instcombine", "function"),
    ("aggressive-instcombine", "aggressive-instcombine", "function"),
    ("simplifycfg", "simplifycfg", "function"),
    ("early-cse", "early-cse<memssa>", "function"),
    ("gvn", "gvn", "function"),
    ("newgvn", "newgvn", "function"),
    ("sccp", "sccp", "function"),
    ("reassociate", "reassociate", "function"),
    ("jump-threading", "jump-threading", "function"),
    ("correlated-propagation", "correlated-propagation", "function"),
    ("dse", "dse", "function"),
    ("adce", "adce", "function"),
    ("bdce", "bdce", "function"),
    ("memcpyopt", "memcpyopt", "function"),
    ("mldst-motion", "mldst-motion", "function"),
    ("loop-rotate", "loop(loop-rotate)", "function"),
    # Bundles: a loop transformation only pays off on a rotated loop, and rotation alone gains nothing, so a
    # one-pass-at-a-time search never reaches them without these.
    ("rotate+unroll", "loop(loop-rotate),loop-unroll<O3>", "function"),
    ("rotate+vectorize", "loop(loop-rotate),loop-vectorize", "function"),
    ("rotate+licm+indvars", "loop-mssa(loop-rotate,licm),loop(indvars)", "function"),
    ("licm", "loop-mssa(licm)", "function"),
    ("indvars", "loop(indvars)", "function"),
    ("loop-idiom", "loop(loop-idiom)", "function"),
    ("loop-deletion", "loop(loop-deletion)", "function"),
    ("loop-unroll", "loop-unroll<O3>", "function"),
    ("loop-vectorize", "loop-vectorize", "function"),
    ("slp-vectorizer", "slp-vectorizer", "function"),
    ("tailcallelim", "tailcallelim", "function"),
    ("div-rem-pairs", "div-rem-pairs", "function"),
    ("sink", "sink", "function"),
    ("inline", "cgscc(inline)", "module"),
]


def greedy_pipeline(elements: list[tuple[str, str, str]]) -> str:
    parts: list[str] = []
    pending: list[str] = []
    for _, text, scope in elements:
        if scope == "function":
            pending.append(text)
            continue
        if pending:
            parts.append(f"function({','.join(pending)})")
            pending = []
        parts.append(text)
    if pending:
        parts.append(f"function({','.join(pending)})")
    return ",".join(parts) or "verify"


# ---------------------------------------------------------------------------------------------------------------
# Running


def find_runner(explicit: str | None) -> Path:
    if explicit:
        return Path(explicit)
    relative = Path("tools") / "backend-explorer" / "runner" / RUNNER_NAME
    candidates = sorted(glob.glob(str(REPO / "build*" / relative)) + glob.glob(str(REPO / "cmake-build-*" / relative)),
                        key=os.path.getmtime, reverse=True)
    if not candidates:
        sys.exit(f"could not find {RUNNER_NAME}; build it with Clang (cmake -DENABLE_BACKEND_EXPLORER=ON, target "
                 f"{RUNNER_NAME}) or pass --runner")
    return Path(candidates[0])


@dataclass
class RunSettings:
    compile_reps: int = 5
    samples: int = 10
    sample_ms: float = 5.0
    kernel_budget_ms: float = 2000.0
    timeout_s: float = 900.0
    cpu: int | None = None
    # Independent processes per configuration. Code and data placement differ per process and can shift a
    # kernel's runtime by tens of percent, so one process is one sample of that layout lottery.
    process_reps: int = 3
    # Re-measure the reference configuration after this many measured configurations (0: never). The machine's
    # speed drifts over a long sweep -- by 20-25% over two hours on a cloud VM -- and the reference timeline lets
    # every result be corrected for the speed at the time it was measured.
    reference_every: int = 4


class Runner:
    def __init__(self, path: Path, settings: RunSettings, extra_options: list[str]):
        self.path = path
        self.settings = settings
        self.extra_options = extra_options

    def _prefix(self) -> list[str]:
        if self.settings.cpu is not None and shutil.which("taskset"):
            return ["taskset", "-c", str(self.settings.cpu)]
        return []

    def info(self) -> dict:
        return json.loads(subprocess.check_output([str(self.path), "--list"], text=True))

    def print_pipeline(self, level: int) -> str:
        return subprocess.check_output([str(self.path), "--print-pipeline", "--opt", f"optimizationLevel={level}"],
                                       text=True).strip()

    def _command(self, config: Config, kernels: list[str]) -> list[str]:
        cmd = self._prefix() + [str(self.path), "--backend", config.backend,
                                "--compile-reps", str(self.settings.compile_reps),
                                "--samples", str(self.settings.samples),
                                "--sample-ms", str(self.settings.sample_ms),
                                "--kernel-budget-ms", str(self.settings.kernel_budget_ms)]
        for assignment in self.extra_options:
            cmd += ["--opt", assignment]
        for key, value in config.options.items():
            cmd += ["--opt", f"{key}={json_scalar(value)}"]
        for key, value in config.string_options.items():
            cmd += ["--opt-str", f"{key}={value}"]
        for kernel in kernels:
            cmd += ["--kernel", kernel]
        return cmd

    def _invoke(self, config: Config, kernels: list[str]) -> tuple[dict[str, dict], str | None]:
        """Runs @p kernels in one process; returns the per-kernel results it printed and, if the process failed,
        why."""
        try:
            proc = subprocess.run(self._command(config, kernels), capture_output=True, text=True,
                                  timeout=self.settings.timeout_s)
        except subprocess.TimeoutExpired as e:
            stdout = e.stdout.decode() if isinstance(e.stdout, bytes) else (e.stdout or "")
            return parse_results(stdout), f"timed out after {self.settings.timeout_s:.0f}s"
        results = parse_results(proc.stdout)
        if proc.returncode != 0:
            return results, describe_failure(proc.returncode, proc.stderr)
        return results, None

    def run(self, config: Config, kernels: list[str]) -> dict[str, dict]:
        """All kernels for one configuration, measured in `process_reps` independent processes and merged."""
        reps = [self._run_process(config, kernels) for _ in range(max(1, self.settings.process_reps))]
        return {kernel: merge_processes([rep[kernel] for rep in reps]) for kernel in kernels}

    def _run_process(self, config: Config, kernels: list[str]) -> dict[str, dict]:
        """All kernels in one process. A process that dies mid-sweep is restarted for the kernels it did not
        report, one kernel per process, so the kernel that crashes is pinpointed and the rest still run."""
        results, failure = self._invoke(config, kernels)
        if failure is None:
            return results
        for kernel in kernels:
            if kernel in results:
                continue
            single, single_failure = self._invoke(config, [kernel])
            if kernel in single:
                results[kernel] = single[kernel]
            else:
                results[kernel] = {"kernel": kernel, "status": "crash", "error": single_failure or failure}
        return results


def merge_processes(reps: list[dict]) -> dict:
    """Combines one kernel's results from several processes: repetitions and samples are pooled, statistics
    take the median over the processes, and a checksum that differs between processes marks the kernel
    nondeterministic (its result depends on something like an address, so no configuration can be checked)."""
    failed = [rep for rep in reps if rep.get("status") != "ok"]
    if failed:
        return failed[0]
    checksums = sorted({rep.get("checksum") for rep in reps})
    if len(checksums) > 1:
        return {"kernel": reps[0].get("kernel"), "status": "nondeterministic",
                "error": f"the checksum differs between processes: {', '.join(map(str, checksums))}"}
    merged = dict(reps[0])
    merged["compileWallMs"] = [v for rep in reps for v in rep.get("compileWallMs", [])]
    merged["runNs"] = [v for rep in reps for v in rep.get("runNs", [])]
    merged["firstRunNs"] = statistics.median(rep.get("firstRunNs", 0) for rep in reps)
    merged["processes"] = len(reps)
    stats: dict = {}
    for key in reps[0].get("stats", {}):
        values = [rep["stats"][key] for rep in reps if key in rep.get("stats", {})]
        numeric = [v for v in values if isinstance(v, (int, float))]
        stats[key] = statistics.median(numeric) if len(numeric) == len(values) else values[0]
    merged["stats"] = stats
    return merged


def json_scalar(value) -> str:
    if isinstance(value, bool):
        return "true" if value else "false"
    return str(value)


def parse_results(stdout: str) -> dict[str, dict]:
    results = {}
    for line in stdout.splitlines():
        line = line.strip()
        if not line.startswith("{"):
            continue
        try:
            entry = json.loads(line)
        except json.JSONDecodeError:
            continue
        if "kernel" in entry:
            results[entry["kernel"]] = entry
    return results


def describe_failure(returncode: int, stderr: str) -> str:
    lines = [line for line in stderr.splitlines() if line.strip() and not line.lstrip().startswith("#")]
    tail = lines[-1] if lines else ""
    if returncode < 0:
        return f"killed by signal {-returncode}" + (f": {tail}" if tail else "")
    return f"exit code {returncode}" + (f": {tail}" if tail else "")


def summarize_kernel(raw: dict) -> dict:
    """The per-(config, kernel) record stored in the results: medians plus the stats the report uses."""
    if raw.get("status") != "ok":
        return {"status": raw.get("status", "error"), "error": first_line(raw.get("error", ""))}
    run_ns = raw.get("runNs") or []
    compile_ms = raw.get("compileWallMs") or []
    quartiles = statistics.quantiles(run_ns, n=4) if len(run_ns) >= 2 else [run_ns[0]] * 3 if run_ns else [None] * 3
    stats = {key: round_sig(value) for key, value in raw.get("stats", {}).items()
             if isinstance(value, (int, float)) and key.startswith(KEPT_STATS_PREFIXES)}
    return {
        "status": "ok",
        "checksum": raw.get("checksum"),
        "compileMs": round_sig(statistics.median(compile_ms)) if compile_ms else None,
        "compileMsMin": round_sig(min(compile_ms)) if compile_ms else None,
        "runNs": round_sig(statistics.median(run_ns)) if run_ns else None,
        "runNsMin": round_sig(min(run_ns)) if run_ns else None,
        "runNsP25": round_sig(quartiles[0]),
        "runNsP75": round_sig(quartiles[2]),
        "firstRunNs": round_sig(raw.get("firstRunNs")),
        "codeBytes": code_bytes(raw.get("stats", {})),
        "stats": stats,
    }


def first_line(text: str) -> str:
    text = text.split("Stack trace")[0].strip()
    return text.splitlines()[0] if text else ""


def code_bytes(stats: dict):
    for key in ("jit.code.bytes", "asmjit.codeSize.bytes", "tbc.jit.codeSize.bytes", "tbc.codeSize.bytes",
                "bc.codeSize.bytes"):
        if isinstance(stats.get(key), (int, float)) and stats[key] > 0:
            return int(stats[key])
    return None


def round_sig(value, digits: int = 5):
    if not isinstance(value, (int, float)) or value == 0 or not math.isfinite(value):
        return value
    return round(value, -int(math.floor(math.log10(abs(value)))) + digits - 1)


# ---------------------------------------------------------------------------------------------------------------
# Machine drift

REFERENCE_LABEL = "reference: mlir O3"
# metric -> (result field or stat key)
DRIFT_METRICS = {"run": "runNs", "compile": "compileMs", "optimize": "llvm.optimize.ms", "codegen": "jit.codegen.ms"}


def drift_value(result: dict, metric: str):
    key = DRIFT_METRICS[metric]
    value = result.get(key) if key in result else result.get("stats", {}).get(key)
    return value if isinstance(value, (int, float)) and value > 0 else None


# Running-median window over the reference timeline. One reference measurement carries a few percent of noise of
# its own; the drift worth correcting moves over many minutes (several references), so smoothing removes the
# former and keeps the latter. Measured on configurations that compile to identical machine code: unsmoothed
# correction raised their spread from 2.1% to 2.7% (runtime), a 5-wide median kept it at 2.1%.
DRIFT_SMOOTHING = 5


def smoothed(series: list[tuple[float, float]], window: int = DRIFT_SMOOTHING) -> list[tuple[float, float]]:
    half = window // 2
    return [(t, statistics.median(v for _, v in series[max(0, i - half):i + half + 1]))
            for i, (t, _) in enumerate(series)]


def drift_factors(references: list[dict], kernel: str, t: float | None) -> dict[str, float]:
    """How much slower than usual the machine ran @p kernel at time @p t, per metric: the reference's value
    (smoothed over neighbouring reference measurements) interpolated linearly between the references around @p t
    (clamped at both ends), divided by its median over the whole run. A result divided by its factor is what it
    would have measured at typical speed. Without a timeline (or a timestamp) every factor is 1."""
    factors = {}
    for metric in DRIFT_METRICS:
        series = smoothed(sorted((r["t"], r["results"][kernel][metric]) for r in references
                                 if kernel in r.get("results", {}) and metric in r["results"][kernel]))
        if t is None or len(series) < 2:
            factors[metric] = 1.0
            continue
        typical = statistics.median(v for _, v in series)
        if t <= series[0][0]:
            at = series[0][1]
        elif t >= series[-1][0]:
            at = series[-1][1]
        else:
            at = series[-1][1]
            for (t0, v0), (t1, v1) in zip(series, series[1:]):
                if t0 <= t <= t1:
                    at = v0 + (v1 - v0) * ((t - t0) / (t1 - t0) if t1 > t0 else 0.0)
                    break
        factors[metric] = at / typical if typical > 0 else 1.0
    return factors


def reference_config() -> Config:
    return make_config("reference", REFERENCE_LABEL, "mlir")


class ReferenceTimeline:
    """Measures the reference configuration before the first measured configuration and then after every
    `reference_every` measured configurations, so drift_factors() can interpolate around each of them."""

    def __init__(self, runner: "Runner", results: "Results", kernels: list[str]):
        self.runner, self.results, self.kernels = runner, results, kernels
        self.since = None  # measured configurations since the last reference; None: none measured yet

    def before_measuring(self) -> None:
        every = self.runner.settings.reference_every
        if every <= 0:
            return
        if self.since is None or self.since >= every:
            self.measure_reference()
        self.since += 1

    def finish(self) -> None:
        if self.runner.settings.reference_every > 0 and self.since:
            self.measure_reference()

    def measure_reference(self) -> None:
        started = time.time()
        raw = self.runner.run(reference_config(), self.kernels)
        self.results.add_reference({k: summarize_kernel(raw.get(k, {"status": "missing"})) for k in self.kernels},
                                   started)
        self.since = 0
        print(f"       {'(reference measurement for drift correction)':<60}", file=sys.stderr, flush=True)


# ---------------------------------------------------------------------------------------------------------------
# Results file


class Results:
    def __init__(self, path: Path):
        self.path = path
        self.data = {"version": 1, "meta": {}, "kernels": [], "pipelines": {}, "configs": []}
        if path.exists():
            self.data = json.loads(path.read_text())
        self._by_id = {c["id"]: c for c in self.data["configs"]}

    def has(self, config: Config) -> bool:
        return config.id in self._by_id

    def get(self, config: Config) -> dict | None:
        return self._by_id.get(config.id)

    def add(self, config: Config, kernel_results: dict[str, dict], started: float | None = None) -> dict:
        entry = config.to_json()
        entry["results"] = kernel_results
        entry["measuredAt"] = datetime.datetime.now(datetime.timezone.utc).isoformat(timespec="seconds")
        now = time.time()
        entry["measuredEpoch"] = round((started + now) / 2 if started else now, 1)
        if config.id in self._by_id:
            self.data["configs"] = [c for c in self.data["configs"] if c["id"] != config.id]
        self.data["configs"].append(entry)
        self._by_id[config.id] = entry
        self.save()
        return entry

    def add_reference(self, kernel_results: dict[str, dict], started: float) -> None:
        """Appends one measurement of the reference configuration to the drift timeline (only the metrics the
        correction uses are kept)."""
        compact = {}
        for kernel, result in kernel_results.items():
            if result.get("status") != "ok":
                continue
            compact[kernel] = {metric: value for metric in DRIFT_METRICS
                               if (value := drift_value(result, metric)) is not None}
        self.data.setdefault("references", []).append(
            {"t": round((started + time.time()) / 2, 1), "results": compact})
        self.save()

    def save(self) -> None:
        tmp = self.path.with_suffix(self.path.suffix + ".tmp")
        tmp.write_text(json.dumps(self.data, separators=(",", ":")))
        tmp.replace(self.path)


def host_meta(runner: Runner, settings: RunSettings) -> dict:
    cpu = platform.processor()
    try:
        for line in Path("/proc/cpuinfo").read_text().splitlines():
            if line.startswith("model name"):
                cpu = line.split(":", 1)[1].strip()
                break
    except OSError:
        pass
    try:
        commit = subprocess.check_output(["git", "-C", str(REPO), "rev-parse", "--short", "HEAD"], text=True).strip()
        dirty = bool(subprocess.check_output(["git", "-C", str(REPO), "status", "--porcelain", "--untracked-files=no"],
                                             text=True).strip())
    except (OSError, subprocess.CalledProcessError):
        commit, dirty = "unknown", False
    governor = None
    try:
        governor = Path("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor").read_text().strip()
    except OSError:
        pass
    return {
        "host": platform.node(), "cpu": cpu, "cores": os.cpu_count(), "os": platform.platform(),
        "governor": governor, "commit": commit + ("-dirty" if dirty else ""), "runner": str(runner.path),
        "settings": settings.__dict__, "extraOptions": runner.extra_options,
        "startedAt": datetime.datetime.now(datetime.timezone.utc).isoformat(timespec="seconds"),
    }


# ---------------------------------------------------------------------------------------------------------------
# Aggregation (shared by the greedy search; the report recomputes it in JavaScript)


def geomean(values: list[float]) -> float | None:
    values = [v for v in values if v and v > 0]
    if not values:
        return None
    return math.exp(sum(math.log(v) for v in values) / len(values))


def aggregate(entry: dict, kernels: list[str], best: bool = False,
              references: list[dict] | None = None) -> tuple[float | None, float | None]:
    """Geometric mean of compile ms and run ns over @p kernels, or None if any kernel failed. With @p best, the
    fastest repetition rather than the median. With @p references, corrected for machine drift (drift_factors)."""
    compile_ms, run_ns = [], []
    for kernel in kernels:
        result = entry["results"].get(kernel)
        if not result or result.get("status") != "ok":
            return None, None
        factors = drift_factors(references, kernel, entry.get("measuredEpoch")) if references else None
        compile_ms.append(result["compileMsMin" if best else "compileMs"] / (factors["compile"] if factors else 1.0))
        run_ns.append(result["runNsMin" if best else "runNs"] / (factors["run"] if factors else 1.0))
    return geomean(compile_ms), geomean(run_ns)


# ---------------------------------------------------------------------------------------------------------------
# Commands


def progress(index: int, total: int, config: Config, note: str) -> None:
    print(f"[{index:>4}/{total}] {config.label:<60} {note}", file=sys.stderr, flush=True)


def measure(runner: Runner, results: Results, configs: list[Config], kernels: list[str], rerun: bool,
            timeline: ReferenceTimeline | None = None) -> None:
    for index, config in enumerate(configs, 1):
        if results.has(config) and not rerun:
            progress(index, len(configs), config, "(cached)")
            continue
        if timeline is not None:
            timeline.before_measuring()
        started = time.time()
        raw = runner.run(config, kernels)
        summarized = {kernel: summarize_kernel(raw.get(kernel, {"status": "missing"})) for kernel in kernels}
        results.add(config, summarized, started)
        ok = sum(1 for r in summarized.values() if r["status"] == "ok")
        progress(index, len(configs), config, f"{ok}/{len(kernels)} ok")


def greedy_search(runner: Runner, results: Results, kernels: list[str], steps: int, min_gain: float,
                  rerun: bool, timeline: ReferenceTimeline | None = None) -> None:
    """Forward selection: starting from an empty LLVM pipeline, repeatedly append the candidate pass with the best
    runtime gain per millisecond of added compile time, until no candidate improves the geometric-mean runtime by
    at least @p min_gain. Decisions use drift-corrected medians when a reference timeline is measured (the
    candidates of one step are measured minutes apart, and an uncorrected search prefers whichever candidate
    happened to run while the machine was fast), otherwise the best repetition of each measurement. Every evaluated candidate is kept as a configuration, so the search also maps the
    neighbourhood of the path it takes."""
    chosen: list[tuple[str, str, str]] = []
    base = make_config("llvm-greedy", "step 0: no LLVM passes", "mlir",
                       string_options={"mlir.llvmPipeline": greedy_pipeline([])},
                       meta={"step": 0, "pipeline": [], "added": None, "selected": True})
    def score(entry: dict) -> tuple[float | None, float | None]:
        references = results.data.get("references") if timeline is not None else None
        return aggregate(entry, kernels, best=references is None, references=references)

    measure(runner, results, [base], kernels, rerun, timeline)
    current = score(results.get(base))
    if current[0] is None:
        print("greedy: the empty pipeline failed; aborting the search", file=sys.stderr)
        return
    for step in range(1, steps + 1):
        best = None
        candidates = [c for c in GREEDY_CANDIDATES if c not in chosen]
        for candidate in candidates:
            elements = chosen + [candidate]
            config = make_config("llvm-greedy", f"step {step}: +{candidate[0]}", "mlir",
                                 string_options={"mlir.llvmPipeline": greedy_pipeline(elements)},
                                 meta={"step": step, "pipeline": [e[0] for e in elements], "added": candidate[0],
                                       "selected": False})
            measure(runner, results, [config], kernels, rerun, timeline)
            compile_ms, run_ns = score(results.get(config))
            if compile_ms is None:
                continue
            gain = math.log(current[1] / run_ns)
            cost = max(compile_ms - current[0], 0.05)
            if gain >= min_gain and (best is None or gain / cost > best[0]):
                best = (gain / cost, candidate, config, (compile_ms, run_ns))
        if best is None:
            print(f"greedy: no candidate improves runtime by {min_gain:.0%} at step {step}; stopping",
                  file=sys.stderr)
            break
        # Re-score the whole step with the references measured meanwhile: decisions use the final correction.
        _, candidate, config, _ = best
        current = score(results.get(config))
        chosen.append(candidate)
        entry = results.get(config)
        entry["meta"]["selected"] = True
        results.save()
        print(f"greedy: step {step} selects {candidate[0]} (pipeline: {', '.join(c[0] for c in chosen)})",
              file=sys.stderr)
    if chosen:
        profile = make_config("llvm-profile", "profile: greedy pipeline", "mlir", PROFILE_OPTION,
                              string_options={"mlir.llvmPipeline": greedy_pipeline(chosen)},
                              meta={"profileOf": "greedy", "pipeline": [c[0] for c in chosen]})
        measure(runner, results, [profile], kernels, rerun, timeline)


def cmd_run(args: argparse.Namespace) -> None:
    settings = RunSettings(compile_reps=args.compile_reps, samples=args.samples, sample_ms=args.sample_ms,
                           kernel_budget_ms=args.kernel_budget_ms, timeout_s=args.timeout, cpu=args.cpu)
    settings.process_reps = args.process_reps
    settings.reference_every = args.reference_every
    if args.quick:
        settings.compile_reps, settings.samples, settings.sample_ms, settings.process_reps = 2, 3, 2.0, 1
    runner = Runner(find_runner(args.runner), settings, args.opt or [])
    info = runner.info()
    all_kernels = [k["name"] for k in info["kernels"]]
    kernels = args.kernel or all_kernels
    if args.quick and not args.kernel:
        kernels = [k for k in all_kernels if k in QUICK_KERNELS]
    unknown = sorted(set(kernels) - set(all_kernels))
    if unknown:
        sys.exit(f"unknown kernels: {', '.join(unknown)} (available: {', '.join(all_kernels)})")

    results = Results(Path(args.out))
    results.data["meta"] = {**results.data.get("meta", {}), **host_meta(runner, settings)}
    results.data["kernels"] = [k for k in info["kernels"] if k["name"] in kernels]
    results.data["backends"] = info["backends"]

    sweeps = expand_sweeps(args.sweep or ["all"])
    levels = [int(level) for level in args.ablation_levels.split(",")] if args.ablation_levels else [0, 1, 2, 3]
    if "mlir" in info["backends"]:
        for level in sorted(set(levels) | {0, 1, 2, 3}):
            results.data["pipelines"][f"O{level}"] = runner.print_pipeline(level)
        results.save()

    builders: dict[str, Callable[[], list[Config]]] = {
        "backends": lambda: sweep_backends(info),
        "backend-flags": lambda: sweep_backend_flags(info),
        "ir-passes": lambda: sweep_ir_passes(info),
        "mlir-levels": lambda: sweep_mlir_levels(info),
        "llvm-ablation": lambda: sweep_llvm_ablation(info, results.data["pipelines"], levels),
        "llvm-profile": lambda: sweep_llvm_profile(info, results.data["pipelines"]),
    }
    configs = []
    for sweep in sweeps:
        if sweep in builders:
            configs += builders[sweep]()
    if args.config_file:
        for item in json.loads(Path(args.config_file).read_text()):
            configs.append(make_config(item.get("group", "custom"), item["label"], item["backend"],
                                       item.get("options"), item.get("stringOptions"), item.get("meta")))
    seen = set()
    configs = [c for c in configs if not (c.id in seen or seen.add(c.id))]
    print(f"{len(configs)} configurations x {len(kernels)} kernels using {runner.path}", file=sys.stderr)
    timeline = ReferenceTimeline(runner, results, kernels) if settings.reference_every > 0 else None
    measure(runner, results, configs, kernels, args.rerun, timeline)
    if "llvm-greedy" in sweeps and "mlir" in info["backends"]:
        greedy_search(runner, results, kernels, args.greedy_steps, args.greedy_min_gain, args.rerun, timeline)
    if timeline is not None:
        timeline.finish()
    results.data["meta"]["finishedAt"] = datetime.datetime.now(datetime.timezone.utc).isoformat(timespec="seconds")
    results.save()
    if args.report:
        write_report(results.data, Path(args.report))


QUICK_KERNELS = {"add", "fibonacci", "arraySum", "matMul", "tpchQ6", "hashProbe", "insertionSort", "chainedIf100",
                 "internalCall", "composite"}


def expand_sweeps(names: list[str]) -> list[str]:
    sweeps = []
    for name in names:
        expanded = ALL_SWEEPS if name == "all" else [name]
        for sweep in expanded:
            if sweep not in ALL_SWEEPS + OPTIONAL_SWEEPS:
                sys.exit(f"unknown sweep '{sweep}' (available: all, {', '.join(ALL_SWEEPS + OPTIONAL_SWEEPS)})")
            if sweep not in sweeps:
                sweeps.append(sweep)
    return sweeps


# The statistics report.html reads; the rest stay in results.json only, which keeps the report a few MB.
REPORT_STATS = {
    "tracing.ms", "ssaCreation.ms", "irGeneration.ms", "irPasses.totalMs", "mlir.loweringFromIR.ms",
    "cpp.loweringFromIR.ms", "mlir.pipeline.ms", "llvm.optimize.ms", "jit.codegen.ms", "jit.compile.ms",
    "cpp.compile.ms", "asmjit.compile.ms", "backend.totalMs", "frontend.totalMs", "llvm.ir.instructions.before",
    "llvm.ir.instructions.after",
}


# Pass profiles (`llvm-profile` configurations only) keep their per-pass statistics too.
PROFILE_STATS_PREFIXES = ("llvm.pass.", "llvm.analysis.", "llvm.codegen.", "llvm.ir.", "jit.", "mlir.")


def report_payload(data: dict) -> dict:
    configs = []
    references = data.get("references") or []
    for config in data["configs"]:
        results = {}
        profiled = config.get("group") == "llvm-profile"
        for kernel, result in config["results"].items():
            result = dict(result)
            if references and result.get("status") == "ok":
                result["drift"] = {m: round(f, 4) for m, f in
                                   drift_factors(references, kernel, config.get("measuredEpoch")).items()}
            if "stats" in result:
                result["stats"] = {k: v for k, v in result["stats"].items()
                                   if k in REPORT_STATS or (profiled and k.startswith(PROFILE_STATS_PREFIXES))}
            results[kernel] = result
        configs.append({**config, "results": results})
    # The reference timeline, as the geometric mean over kernels of each metric relative to its median.
    timeline = []
    kernels = sorted({k for r in references for k in r.get("results", {})})
    for reference in references:
        point = {"t": reference["t"]}
        for metric in DRIFT_METRICS:
            ratios = [drift_factors(references, k, reference["t"])[metric] for k in kernels
                      if metric in reference["results"].get(k, {})]
            point[metric] = round(geomean(ratios), 4) if ratios else None
        timeline.append(point)
    return {**{k: v for k, v in data.items() if k != "references"}, "configs": configs, "driftTimeline": timeline}


def write_report(data: dict, out: Path) -> None:
    template = (HERE / "report.html").read_text()
    marker = "/*__RESULTS__*/null"
    if marker not in template:
        sys.exit("report.html is missing the results placeholder")
    payload = json.dumps(report_payload(data), separators=(",", ":")).replace("</", "<\\/")
    out.write_text(template.replace(marker, payload))
    print(f"wrote {out} ({len(data['configs'])} configurations)", file=sys.stderr)


def cmd_report(args: argparse.Namespace) -> None:
    data = json.loads(Path(args.results).read_text())
    write_report(data, Path(args.out))


def cmd_pipeline(args: argparse.Namespace) -> None:
    runner = Runner(find_runner(args.runner), RunSettings(), [])
    text = runner.print_pipeline(args.level)
    if args.tree:
        def show(nodes: list[PassNode], depth: int) -> None:
            for node in nodes:
                print("  " * depth + node.name + node.params)
                if node.children is not None:
                    show(node.children, depth + 1)
        show(parse_pipeline(text), 0)
    else:
        print(text)


def main(argv: list[str] | None = None) -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    run = sub.add_parser("run", help="measure configurations")
    run.add_argument("--out", default="results.json", help="results file (appended to and resumed from)")
    run.add_argument("--report", help="also write the HTML report here")
    run.add_argument("--runner", help=f"path to {RUNNER_NAME} (default: newest build*/ match)")
    run.add_argument("--sweep", action="append",
                     help=f"sweeps to run (repeatable): all (default), {', '.join(ALL_SWEEPS + OPTIONAL_SWEEPS)}")
    run.add_argument("--kernel", action="append", help="restrict to these kernels (repeatable)")
    run.add_argument("--config-file", help="JSON list of extra configurations: "
                                           "[{label, backend, options, stringOptions, group, meta}]")
    run.add_argument("--opt", action="append", help="key=value option applied to every configuration")
    run.add_argument("--ablation-levels", default="0,1,2,3", help="LLVM levels to ablate passes from, e.g. 2,3")
    run.add_argument("--greedy-steps", type=int, default=12, help="maximum passes the greedy search selects")
    run.add_argument("--greedy-min-gain", type=float, default=0.01,
                     help="minimum geometric-mean runtime improvement for a greedy step")
    run.add_argument("--compile-reps", type=int, default=5)
    run.add_argument("--samples", type=int, default=10)
    run.add_argument("--sample-ms", type=float, default=5.0)
    run.add_argument("--kernel-budget-ms", type=float, default=2000.0)
    run.add_argument("--process-reps", type=int, default=3,
                     help="independent runner processes per configuration (code/data layout differs per process)")
    run.add_argument("--reference-every", type=int, default=4,
                     help="re-measure the reference configuration after this many configurations, to correct for "
                          "machine drift (0: off)")
    run.add_argument("--timeout", type=float, default=900.0, help="seconds per runner process")
    run.add_argument("--cpu", type=int, help="pin the runner to this CPU (taskset)")
    run.add_argument("--quick", action="store_true", help="1 process, 2 compiles, 3 samples, a kernel subset")
    run.add_argument("--rerun", action="store_true", help="re-measure configurations already in the results")
    run.set_defaults(func=cmd_run)

    report = sub.add_parser("report", help="render results as a self-contained HTML report")
    report.add_argument("results")
    report.add_argument("--out", default="report.html")
    report.set_defaults(func=cmd_report)

    pipeline = sub.add_parser("pipeline", help="print the expanded LLVM pipeline for an optimization level")
    pipeline.add_argument("--level", type=int, default=3)
    pipeline.add_argument("--tree", action="store_true", help="one pass per line, indented by nesting")
    pipeline.add_argument("--runner")
    pipeline.set_defaults(func=cmd_pipeline)

    args = parser.parse_args(argv)
    args.func(args)


if __name__ == "__main__":
    main()
