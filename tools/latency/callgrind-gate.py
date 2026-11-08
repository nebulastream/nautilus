#!/usr/bin/env python3
"""The callgrind instruction-count gate of the 0.4 pipeline (decision D9, docs/plans/nautilus-0.4/10-observability.md §2).

  callgrind-gate.py extract <callgrind.out>          Ir of one compile: total, tracing, frontend, backend
  callgrind-gate.py compare --ref 0.3.tsv --new nir.tsv [--repeat nir2.tsv] [--gates gates.json] [--meta nir.tsv.meta]
  callgrind-gate.py update  --ref 0.3.tsv --new nir.tsv [--baseline-corpus corpus.jsonl] [--gates gates.json]
  callgrind-gate.py --self-test

The .tsv files are what tools/latency/callgrind-corpus.sh writes: one line per (function, N, backend) case,
`function n backend total tracing frontend backend_ir`, the instruction counts (Ir) of the one call that compiles.
The reference is the corpus driver built against `main` (0.3), run in the same container; `new` is the same driver
on this branch.

Metrics per case (ratio = Ir of this branch / Ir of 0.3):
  total      tracing + conversion + passes + backend            what "end to end" means in the release criteria
  tracing    the tracer alone (`NirTracer::Trace`, 0.3's `TraceContext::Trace`)
  ir         conversion and passes (`compileToIR` minus the tracer)
  backend    `CompilationPipeline::compileIR`, the backend

gates.json holds, per case and metric, a `threshold` and optionally a `target`:
  threshold  a ratchet: the measured ratio when it was recorded, plus a margin for build differences. A ratio above it
             FAILS. `update` only ever lowers it; raising one needs `--allow-raise` and a reviewer.
  target     the budget of the plan (README §6 and 05-backends.md §4.3 as a fraction of 0.3's cost, or "no slower
             than 0.3" = 1.0). A ratio above it WARNS, loudly and in every run, until it is met; it is never
             relaxed to what is measured. A target already met is enforced through the threshold.
`compare` also checks the instruction counts of a second run of this branch (`--repeat`) for exact equality (the
identity criterion of M1.10) and that the environment fingerprint is the recorded one.

Exit status: 0 all gates hold, 1 a gate or the identity check failed, 2 usage or input errors.
"""
import argparse
import hashlib
import json
import math
import os
import re
import sys

METRICS = ("total", "tracing", "ir", "backend")
RAISE_TOLERANCE = 0.01  # a threshold is raised only by `update --allow-raise`; smaller moves are 0.3's run-to-run noise
MARGIN = 0.03  # threshold = measured ratio * (1 + MARGIN), rounded up; the counts themselves are exact
UNIVERSAL_TARGET = 1.0  # release criterion: no case slower than 0.3
# the plan's budgets in us/op (README §1, 05-backends.md §4.3, 03 §3.2): tracing, conversion and backend each
TARGET_US_PER_OP = {"total": 0.75, "tracing": 0.25, "ir": 0.25, "backend": 0.25}
LINEAR = ("straight", "loopBody")
LINEAR_MIN_N = 1000  # the budgets are per op on large functions; the fixed cost of a compile dominates below
TRACING_CHEAPER = 0.70  # M2.8: distinctSites1024 tracing at least 30% cheaper than 0.3 (01-tracing-frontend.md §7)

# inclusive cost of these functions, by the start of their demangled name
STAGE_PATTERNS = {
    "tracing": ("nautilus::tracing::nirtrace::NirTracer::Trace(", "nautilus::tracing::TraceContext::Trace("),
    "frontend": ("nautilus::compiler::CompilationPipeline::compileToIR(",),
    "backend": ("nautilus::compiler::CompilationPipeline::compileIR(",),
}


# ---- reading callgrind output ---------------------------------------------------------------------------------


def extract(path):
    """Inclusive Ir of the stages in a callgrind output written with --compress-strings=no --compress-pos=no."""
    self_cost = {}
    call_cost = {}
    current = None
    after_calls = False
    total = None
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            if line.startswith("fn="):
                current = line[3:].rstrip("\n")
                after_calls = False
            elif line.startswith("calls="):
                after_calls = True
            elif line.startswith("totals:"):
                total = int(line.split()[1])
            elif line[:1].isdigit() or line[:1] in "+-*":
                cost = line.split()[-1]
                if not cost.isdigit() or current is None:
                    continue
                if after_calls:
                    call_cost[current] = call_cost.get(current, 0) + int(cost)
                    after_calls = False
                else:
                    self_cost[current] = self_cost.get(current, 0) + int(cost)
    if total is None:
        raise ValueError(f"{path}: no totals line")
    result = {"total": total}
    for stage, prefixes in STAGE_PATTERNS.items():
        inclusive = 0
        for name in set(self_cost) | set(call_cost):
            if name.startswith(prefixes):
                inclusive += self_cost.get(name, 0) + call_cost.get(name, 0)
        result[stage] = inclusive
    return result


# ---- the tables ---------------------------------------------------------------------------------------------


def read_tsv(path):
    """{(function, n, backend): {metric: Ir}} in file order."""
    cases = {}
    with open(path, encoding="utf-8") as f:
        for number, line in enumerate(f, 1):
            parts = line.rstrip("\n").split("\t")
            if len(parts) != 7:
                raise ValueError(f"{path}:{number}: expected 7 columns, got {len(parts)}")
            fn, n, backend = parts[0], int(parts[1]), parts[2]
            total, tracing, frontend, back = (int(p) for p in parts[3:])
            if (fn, n, backend) in cases:
                raise ValueError(f"{path}:{number}: duplicate case")
            cases[(fn, n, backend)] = {
                "total": total,
                "tracing": tracing,
                "ir": frontend - tracing,
                "backend": back,
                "_raw": parts[3:],
            }
    return cases


def case_name(key):
    return f"{key[0]}/{key[1]}/{key[2]}"


def read_meta(path):
    meta = {}
    if path and os.path.exists(path):
        with open(path, encoding="utf-8") as f:
            for line in f:
                key, _, value = line.rstrip("\n").partition(" ")
                meta[key] = value
    return meta


def fingerprint(meta):
    """What the counts depend on besides the sources: tools and libraries of the image, and the pinned dispatch."""
    parts = [meta.get(k, "") for k in ("valgrind", "libc6", "libstdc++6", "driver-compiler", "tunables")]
    return hashlib.sha256("\n".join(parts).encode()).hexdigest()[:16]


def ratio(new, ref):
    return new / ref if ref > 0 else math.inf


# ---- targets (the plan's budgets as fractions of 0.3's cost) --------------------------------------------------


def targets_from_baseline(corpus_path):
    """{(function, backend, metric): target ratio} from the 0.3 baseline's us/op at the largest linear size.

    The plan states budgets in us/op (README §1: end to end 0.75, about 0.25 each for tracing, conversion and backend)
    and 0.3's us/op is in the recorded baseline (`nautilus/test/baselines/0.3/corpus.jsonl`, wall clock of one cloud
    container: an approximation of the IPC-equal assumption of D9). A case whose budget is a fraction f of 0.3's cost
    is gated at f times 0.3's instruction count (10-observability.md §2)."""
    targets = {}
    if not corpus_path or not os.path.exists(corpus_path):
        return targets
    rows = {}
    with open(corpus_path, encoding="utf-8") as f:
        for line in f:
            if line.strip():
                r = json.loads(line)
                rows[(r["function"], r["n"], r["backend"])] = r
    for fn in LINEAR:
        for backend in ("tbc", "asmjit"):
            sizes = sorted(n for (f, n, b) in rows if f == fn and b == backend)
            if not sizes:
                continue
            r = rows[(fn, sizes[-1], backend)]
            ops = r["ir_ops"]
            if ops <= 0:
                continue
            us = {
                "total": r["total_ms"],
                "tracing": r["tracing_ms"],
                "ir": r["irgen_ms"] + r["passes_ms"],
                "backend": r["backend_ms"],
            }
            for metric, ms in us.items():
                per_op = ms * 1000.0 / ops
                if per_op > 0:
                    targets[(fn, backend, metric)] = TARGET_US_PER_OP[metric] / per_op
    # the tracer and the conversion do not depend on the backend: one target, from the mean of the baseline's rows
    for fn in LINEAR:
        for metric in ("tracing", "ir"):
            values = [t for (f, b, m), t in targets.items() if f == fn and m == metric]
            for (f, b, m) in [k for k in targets if k[0] == fn and k[2] == metric]:
                targets[(f, b, m)] = sum(values) / len(values)
    return targets


def target_for(key, metric, derived):
    """(target ratio, where it comes from) of a case and metric."""
    fn, n, backend = key
    if fn in LINEAR and n >= LINEAR_MIN_N and (fn, backend, metric) in derived:
        t = derived[(fn, backend, metric)]
        if metric == "backend" and backend != "asmjit":
            return UNIVERSAL_TARGET, "no slower than 0.3 (README §6.2)"
        return round(t, 3), f"{TARGET_US_PER_OP[metric]} us/op on linear code (README §1) over 0.3's cost"
    if fn == "distinctSites1024" and metric == "tracing":
        return TRACING_CHEAPER, "tracing at least 30% cheaper than 0.3 (01 §7, M2.8)"
    return UNIVERSAL_TARGET, "no slower than 0.3 (README §6.2)"


# ---- gates.json ---------------------------------------------------------------------------------------------


def load_gates(path):
    if not path or not os.path.exists(path):
        return None
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def round_up(x, digits=4):
    scale = 10**digits
    return math.ceil(x * scale - 1e-9) / scale


def build_gates(ref, new, old, derived, meta, allow_raise):
    """The gates for the measured ratios; the ratchet keeps old thresholds unless lowered. Returns (gates, raised)."""
    cases = {}
    raised = []
    for key in new:
        entry = {}
        for metric in METRICS:
            r = ratio(new[key][metric], ref[key][metric])
            threshold = round_up(r * (1 + MARGIN))
            previous = (old or {}).get("cases", {}).get(case_name(key), {}).get(metric, {}).get("threshold")
            if previous is not None and threshold > previous * (1 + RAISE_TOLERANCE):
                raised.append((case_name(key), metric, previous, threshold))
                if not allow_raise:
                    threshold = previous
            elif previous is not None:
                # equal, lower, or higher by less than the tolerance (the 0.3 side has a clock in its ids)
                threshold = min(threshold, previous) if threshold < previous else previous
            target, source = target_for(key, metric, derived)
            entry[metric] = {"threshold": threshold, "target": target, "target_source": source}
        cases[case_name(key)] = entry
    gates = {
        "schema": 1,
        "about": "Ratchet thresholds and plan targets of the callgrind gate (tools/latency/callgrind-gate.py, "
        "docs/plans/nautilus-0.4/10-observability.md §2). Regenerate with `callgrind-gate.py update`.",
        "margin": MARGIN,
        "environment": {"fingerprint": fingerprint(meta) if meta else (old or {}).get("environment", {}).get("fingerprint")},
        "cases": cases,
    }
    return gates, raised


# ---- compare ------------------------------------------------------------------------------------------------


def compare(ref, new, repeat, gates, meta, require_image, out):
    """Prints the tables; returns the number of failures."""
    failures = 0
    warnings = 0
    out.write("callgrind gate: Ir of one compile, NIR (this branch) against 0.3 (main), same driver\n")
    image = meta.get("image", "none") if meta else "none"
    out.write(f"image: {image}\n")
    if meta:
        out.write(f"environment fingerprint: {fingerprint(meta)}\n")

    # the identity check of M1.10: a second run under identical conditions must give exactly the same counts
    if repeat is not None:
        differing = [k for k in new if k not in repeat or new[k]["_raw"] != repeat[k]["_raw"]]
        differing += [k for k in repeat if k not in new]
        if differing:
            failures += 1
            out.write(f"IDENTITY FAIL: {len(differing)} case(s) differ between two runs of the same binary:\n")
            for k in differing:
                a = new.get(k, {}).get("_raw")
                b = repeat.get(k, {}).get("_raw")
                out.write(f"  {case_name(k)}: {a} vs {b}\n")
        else:
            out.write(f"identity: PASS ({len(new)} cases, exactly equal Ir in all metrics on two runs)\n")

    if gates is None:
        failures += 1
        out.write("FAIL: no gates file; run `callgrind-gate.py update` and commit the result\n")
        return failures
    recorded = gates.get("environment", {}).get("fingerprint")
    if meta and recorded and recorded != fingerprint(meta):
        msg = (f"environment fingerprint {fingerprint(meta)} differs from the recorded {recorded} (valgrind, libc, "
               f"libstdc++, compiler or pinned dispatch changed): counts are compared only within one image; "
               f"re-record the thresholds with `callgrind-gate.py update` in this image")
        if require_image:
            failures += 1
            out.write("FAIL: " + msg + "\n")
        else:
            out.write("WARN: " + msg + "\n")

    missing = [k for k in new if case_name(k) not in gates["cases"]]
    stale = [c for c in gates["cases"] if c not in {case_name(k) for k in new}]
    for k in missing:
        failures += 1
        out.write(f"FAIL: {case_name(k)} has no gate; run `callgrind-gate.py update`\n")
    for c in stale:
        failures += 1
        out.write(f"FAIL: gate {c} has no measurement (the case was removed or renamed)\n")
    for k in new:
        if k not in ref:
            failures += 1
            out.write(f"FAIL: {case_name(k)} has no 0.3 reference measurement\n")

    for metric in METRICS:
        out.write(f"\n[{metric}]\n")
        out.write(f"{'case':<32} {'Ir 0.3':>14} {'Ir NIR':>14} {'ratio':>7} {'threshold':>9} {'target':>7}  verdict\n")
        for key in new:
            name = case_name(key)
            gate = gates["cases"].get(name, {}).get(metric)
            if key not in ref or gate is None:
                continue
            r0, r1 = ref[key][metric], new[key][metric]
            rt = ratio(r1, r0)
            verdict = "PASS"
            if rt > gate["threshold"] * (1 + 1e-12):
                verdict = "FAIL"
                failures += 1
            elif gate.get("target") is not None and rt > gate["target"]:
                verdict = "WARN"
                warnings += 1
            out.write(f"{name:<32} {r0:>14} {r1:>14} {rt:>7.4f} {gate['threshold']:>9.4f} "
                      f"{gate.get('target', float('nan')):>7.4f}  {verdict}"
                      f"{' (target not met: ' + gate['target_source'] + ')' if verdict == 'WARN' else ''}\n")
    out.write(f"\n{failures} failure(s), {warnings} unmet target(s) (reported, not gated)\n")
    return failures


# ---- self test ----------------------------------------------------------------------------------------------


def self_test():
    import io
    import tempfile

    def case(total, tracing, frontend, backend):
        return {"total": total, "tracing": tracing, "ir": frontend - tracing, "backend": backend,
                "_raw": [str(total), str(tracing), str(frontend), str(backend)]}

    key_a, key_b, key_c = ("straight", 1000, "asmjit"), ("calls", 100, "tbc"), ("distinctSites1024", 1, "tbc")
    ref = {key_a: case(1000, 200, 800, 200), key_b: case(500, 100, 300, 200), key_c: case(900, 400, 700, 200)}
    new = {key_a: case(200, 100, 120, 80), key_b: case(250, 80, 150, 100), key_c: case(300, 200, 250, 50)}
    meta = {"valgrind": "valgrind-3.22.0", "libc6": "2.39", "libstdc++6": "14", "driver-compiler": "clang", "tunables": "t"}
    derived = {("straight", "asmjit", m): 0.3 for m in METRICS}

    gates, raised = build_gates(ref, new, None, derived, meta, False)
    assert not raised
    g = gates["cases"]["straight/1000/asmjit"]["total"]
    assert abs(g["threshold"] - round_up(0.2 * 1.03)) < 1e-9 and g["target"] == 0.3, g
    assert gates["cases"]["distinctSites1024/1/tbc"]["tracing"]["target"] == TRACING_CHEAPER
    assert gates["cases"]["calls/100/tbc"]["total"]["target"] == 1.0

    def run(ref, new, repeat=None, gates=gates, require_image=True, meta=meta):
        buf = io.StringIO()
        return compare(ref, new, repeat, gates, meta, require_image, buf), buf.getvalue()

    # the recorded numbers pass, the repeat is identical
    failures, text = run(ref, new, new)
    assert failures == 0 and "identity: PASS" in text, text

    # a regression beyond the threshold fails; one within the margin does not
    worse = dict(new)
    worse[key_a] = case(210, 100, 130, 80)  # total 0.21 against threshold 0.2060
    failures, text = run(ref, worse)
    assert failures >= 1 and "FAIL" in text, text

    # an unmet target warns and does not fail: gates recorded when the ratio is 0.6 against a target of 0.3
    slow = {key_a: case(600, 100, 520, 80), key_b: new[key_b], key_c: new[key_c]}
    slow_gates, _ = build_gates(ref, slow, None, derived, meta, False)
    failures, text = run(ref, slow, gates=slow_gates)
    assert failures == 0 and "WARN" in text and "target not met" in text, text

    # the ratchet: a worse measurement is not accepted by `update`, a better one lowers the threshold
    _, raised = build_gates(ref, worse, gates, derived, meta, False)
    assert raised
    kept, _ = build_gates(ref, worse, gates, derived, meta, False)
    assert kept["cases"]["straight/1000/asmjit"]["total"]["threshold"] == g["threshold"]
    better = dict(new)
    better[key_a] = case(100, 50, 60, 40)
    lowered, raised = build_gates(ref, better, gates, derived, meta, False)
    assert not raised and lowered["cases"]["straight/1000/asmjit"]["total"]["threshold"] < g["threshold"]

    # identity: any difference between two runs is a failure that names the case
    other = dict(new)
    other[key_b] = case(251, 80, 150, 100)
    failures, text = run(ref, new, other)
    assert failures >= 1 and "IDENTITY FAIL" in text and "calls/100/tbc" in text, text

    # a case without a gate, a gate without a case, a changed environment
    extra = dict(new)
    extra[("loopBody", 100, "tbc")] = case(10, 1, 5, 5)
    ref_extra = dict(ref)
    ref_extra[("loopBody", 100, "tbc")] = case(100, 10, 50, 50)
    assert run(ref_extra, extra)[0] >= 1
    fewer = {key_a: new[key_a]}
    assert run(ref, fewer)[0] >= 1
    changed = dict(meta, valgrind="valgrind-3.99")
    assert run(ref, new, meta=changed)[0] >= 1
    assert run(ref, new, meta=changed, require_image=False)[0] == 0

    # reading a callgrind file
    with tempfile.TemporaryDirectory() as d:
        p = os.path.join(d, "cg.out")
        with open(p, "w", encoding="utf-8") as f:
            f.write("events: Ir\nfn=outer\n10 5\ncfn=nautilus::compiler::CompilationPipeline::compileToIR(a, b)\n"
                    "calls=1 0\n10 90\nfn=nautilus::compiler::CompilationPipeline::compileToIR(a, b)\n11 20\n"
                    "cfn=nautilus::tracing::TraceContext::Trace(x)\ncalls=1 0\n11 60\n"
                    "fn=nautilus::tracing::TraceContext::Trace(x)\n12 60\ntotals: 95\n")
        got = extract(p)
        assert got == {"total": 95, "tracing": 60, "frontend": 80, "backend": 0}, got

    # tsv round trip
    with tempfile.TemporaryDirectory() as d:
        p = os.path.join(d, "a.tsv")
        with open(p, "w", encoding="utf-8") as f:
            f.write("straight\t100\ttbc\t1000\t200\t800\t100\n")
        t = read_tsv(p)
        assert t[("straight", 100, "tbc")]["ir"] == 600
    print("self-test: ok")
    return 0


# ---- command line ---------------------------------------------------------------------------------------------


def main(argv):
    if "--self-test" in argv:
        return self_test()
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    e = sub.add_parser("extract")
    e.add_argument("callgrind_out")
    for name in ("compare", "update"):
        c = sub.add_parser(name)
        c.add_argument("--ref", required=True, help="0.3 measurements (callgrind-corpus.sh output)")
        c.add_argument("--new", required=True, help="this branch's measurements")
        c.add_argument("--repeat", help="a second run of this branch for the identity check")
        c.add_argument("--meta", help="the .meta file of --new (default: <new>.meta)")
        c.add_argument("--gates", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..",
                                                       "nautilus", "test", "baselines", "callgrind-gates.json"))
        c.add_argument("--baseline-corpus", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..",
                                                                 "nautilus", "test", "baselines", "0.3", "corpus.jsonl"))
        c.add_argument("--require-image", action="store_true",
                       help="a different environment fingerprint is a failure (the CI job), not a warning")
        c.add_argument("--summary", help="also write the tables to this file (GITHUB_STEP_SUMMARY)")
        if name == "update":
            c.add_argument("--allow-raise", action="store_true", help="accept thresholds above the recorded ones")
    args = ap.parse_args(argv)

    if args.cmd == "extract":
        r = extract(args.callgrind_out)
        print(f"{r['total']}\t{r['tracing']}\t{r['frontend']}\t{r['backend']}")
        return 0

    try:
        ref, new = read_tsv(args.ref), read_tsv(args.new)
        repeat = read_tsv(args.repeat) if getattr(args, "repeat", None) else None
    except (OSError, ValueError) as err:
        print(f"error: {err}", file=sys.stderr)
        return 2
    meta = read_meta(args.meta or args.new + ".meta")
    gates = load_gates(args.gates)

    if args.cmd == "update":
        derived = targets_from_baseline(args.baseline_corpus)
        new_gates, raised = build_gates(ref, new, gates, derived, meta, args.allow_raise)
        status = 0
        for name, metric, old, wanted in raised:
            if args.allow_raise:
                print(f"raised {name} {metric}: {old} -> {wanted}")
            else:
                print(f"NOT raised {name} {metric}: recorded {old}, measured needs {wanted} "
                      f"(a regression: fix it, or pass --allow-raise and justify it in the PR)")
                status = 1
        with open(args.gates, "w", encoding="utf-8") as f:
            json.dump(new_gates, f, indent=1, sort_keys=False)
            f.write("\n")
        print(f"wrote {args.gates} ({len(new_gates['cases'])} cases)")
        return status

    sink = sys.stdout
    if args.summary:
        class Tee:
            def __init__(self, *files):
                self.files = files

            def write(self, s):
                for f in self.files:
                    f.write(s)

        summary = open(args.summary, "a", encoding="utf-8")
        sink = Tee(sys.stdout, summary)
    failures = compare(ref, new, repeat, gates, meta, args.require_image, sink)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
