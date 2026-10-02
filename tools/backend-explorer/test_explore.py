"""Unit tests for explore.py's pipeline handling and result bookkeeping (no runner needed):

    python3 -m unittest discover tools/backend-explorer
"""

import json
import tempfile
import unittest
from pathlib import Path

import explore

O1_EXCERPT = (
    "annotation2metadata,function<eager-inv>(lower-expect,simplifycfg<bonus-inst-threshold=1;keep-loops>,"
    "sroa<modify-cfg>,early-cse<>),cgscc(devirt<4>(inline,function<eager-inv;no-rerun>(sroa<modify-cfg>,"
    "instcombine<max-iterations=1>,loop-mssa(licm<allowspeculation>)))),require<globals-aa>,"
    "function(invalidate<aa>),globaldce"
)


class PipelineTest(unittest.TestCase):
    def test_round_trip(self):
        self.assertEqual(explore.render_pipeline(explore.parse_pipeline(O1_EXCERPT)), O1_EXCERPT)

    def test_leaf_counts_skip_bookkeeping(self):
        counts = explore.leaf_pass_counts(explore.parse_pipeline(O1_EXCERPT))
        self.assertEqual(counts["sroa"], 2)
        self.assertEqual(counts["licm"], 1)
        self.assertNotIn("require", counts)
        self.assertNotIn("invalidate", counts)
        self.assertNotIn("annotation2metadata", counts)
        self.assertNotIn("function", counts)

    def test_remove_pass_removes_every_instance(self):
        nodes = explore.parse_pipeline(O1_EXCERPT)
        text = explore.render_pipeline(explore.remove_pass(nodes, "sroa"))
        self.assertNotIn("sroa", text)
        self.assertIn("early-cse<>", text)
        self.assertIn("instcombine<max-iterations=1>", text)

    def test_remove_pass_drops_empty_adaptors(self):
        nodes = explore.parse_pipeline(O1_EXCERPT)
        text = explore.render_pipeline(explore.remove_pass(nodes, "licm"))
        self.assertNotIn("loop-mssa", text)
        self.assertIn("instcombine", text)
        text = explore.render_pipeline(explore.remove_pass(explore.parse_pipeline("function(instcombine),globaldce"),
                                                           "instcombine"))
        self.assertEqual(text, "globaldce")

    def test_unbalanced_pipeline_is_rejected(self):
        with self.assertRaises(ValueError):
            explore.parse_pipeline("function(instcombine")

    def test_greedy_pipeline_groups_function_passes(self):
        candidates = {name: (name, text, scope) for name, text, scope in explore.GREEDY_CANDIDATES}
        self.assertEqual(explore.greedy_pipeline([]), "verify")
        self.assertEqual(
            explore.greedy_pipeline([candidates["sroa"], candidates["licm"], candidates["inline"],
                                     candidates["gvn"]]),
            "function(sroa<modify-cfg>,loop-mssa(licm)),cgscc(inline),function(gvn)")


class ConfigTest(unittest.TestCase):
    def test_id_depends_on_options_not_label_order(self):
        a = explore.make_config("g", "x", "mlir", {"optimizationLevel": 2, "mlir.inliner": False})
        b = explore.make_config("g", "x", "mlir", {"mlir.inliner": False, "optimizationLevel": 2})
        c = explore.make_config("g", "x", "mlir", {"optimizationLevel": 3})
        self.assertEqual(a.id, b.id)
        self.assertNotEqual(a.id, c.id)

    def test_ablation_sweep(self):
        info = {"backends": ["mlir"]}
        configs = explore.sweep_llvm_ablation(info, {"O3": O1_EXCERPT}, [3])
        removed = [c.meta["removedPass"] for c in configs]
        self.assertIsNone(removed[0])
        self.assertIn("licm", removed)
        self.assertEqual(len(removed), len(set(removed)))
        for config in configs:
            self.assertTrue(config.string_options["mlir.llvmPipeline"])

    def test_sweeps_respect_available_backends(self):
        configs = explore.sweep_backends({"backends": ["bc"], "tbcJit": False})
        self.assertTrue(configs)
        self.assertTrue(all(c.backend == "bc" for c in configs))
        self.assertEqual(explore.sweep_mlir_levels({"backends": ["bc"]}), [])


class ResultsTest(unittest.TestCase):
    def test_summarize_kernel(self):
        raw = {"kernel": "k", "status": "ok", "checksum": "7", "compileWallMs": [3.0, 1.0, 2.0],
               "runNs": [10.0, 30.0, 20.0, 40.0], "firstRunNs": 99.0,
               "stats": {"jit.code.bytes": 512, "llvm.optimize.ms": 0.5, "irrelevant": 1}}
        summary = explore.summarize_kernel(raw)
        self.assertEqual(summary["compileMs"], 2.0)
        self.assertEqual(summary["runNs"], 25.0)
        self.assertEqual(summary["runNsMin"], 10.0)
        self.assertEqual(summary["codeBytes"], 512)
        self.assertIn("llvm.optimize.ms", summary["stats"])
        self.assertNotIn("irrelevant", summary["stats"])

    def test_merge_processes_pools_samples(self):
        a = {"kernel": "k", "status": "ok", "checksum": "1", "compileWallMs": [1.0], "runNs": [10.0, 11.0],
             "firstRunNs": 5.0, "stats": {"jit.code.bytes": 100, "llvm.optimize.ms": 1.0}}
        b = {"kernel": "k", "status": "ok", "checksum": "1", "compileWallMs": [3.0], "runNs": [30.0],
             "firstRunNs": 7.0, "stats": {"jit.code.bytes": 100, "llvm.optimize.ms": 3.0}}
        merged = explore.merge_processes([a, b])
        self.assertEqual(merged["compileWallMs"], [1.0, 3.0])
        self.assertEqual(merged["runNs"], [10.0, 11.0, 30.0])
        self.assertEqual(merged["stats"]["llvm.optimize.ms"], 2.0)
        self.assertEqual(merged["processes"], 2)

    def test_merge_processes_flags_nondeterminism_and_failures(self):
        ok = {"kernel": "k", "status": "ok", "checksum": "1", "stats": {}}
        other = {**ok, "checksum": "2"}
        crash = {"kernel": "k", "status": "crash", "error": "signal 11"}
        self.assertEqual(explore.merge_processes([ok, other])["status"], "nondeterministic")
        self.assertEqual(explore.merge_processes([ok, crash])["status"], "crash")

    def test_failed_kernel_keeps_first_error_line(self):
        summary = explore.summarize_kernel({"status": "error", "error": "boom\nStack trace (most recent call last):"})
        self.assertEqual(summary, {"status": "error", "error": "boom"})

    def test_results_resume(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "r.json"
            config = explore.make_config("g", "x", "bc")
            results = explore.Results(path)
            results.add(config, {"k": {"status": "ok"}})
            reloaded = explore.Results(path)
            self.assertTrue(reloaded.has(config))
            self.assertEqual(json.loads(path.read_text())["configs"][0]["results"]["k"]["status"], "ok")

    def test_report_payload_keeps_only_report_stats(self):
        data = {"configs": [{"id": "c", "results": {"k": {"status": "ok", "stats": {
            "llvm.optimize.ms": 1.0, "irPasses.deadCodeElimination.ms": 0.1}}}}]}
        payload = explore.report_payload(data)
        self.assertEqual(payload["configs"][0]["results"]["k"]["stats"], {"llvm.optimize.ms": 1.0})
        self.assertIn("irPasses.deadCodeElimination.ms", data["configs"][0]["results"]["k"]["stats"])

    def test_aggregate_is_geomean_and_requires_every_kernel(self):
        entry = {"results": {"a": {"status": "ok", "compileMs": 1.0, "runNs": 4.0, "compileMsMin": 1.0, "runNsMin": 1.0},
                             "b": {"status": "ok", "compileMs": 4.0, "runNs": 1.0, "compileMsMin": 1.0, "runNsMin": 1.0},
                             "c": {"status": "crash"}}}
        compile_ms, run_ns = explore.aggregate(entry, ["a", "b"])
        self.assertAlmostEqual(compile_ms, 2.0)
        self.assertAlmostEqual(run_ns, 2.0)
        self.assertEqual(explore.aggregate(entry, ["a", "b"], best=True), (1.0, 1.0))
        self.assertEqual(explore.aggregate(entry, ["a", "c"]), (None, None))


if __name__ == "__main__":
    unittest.main()
