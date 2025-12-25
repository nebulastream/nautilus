window.BENCHMARK_DATA = {
  "lastUpdate": 1766652628575,
  "repoUrl": "https://github.com/nebulastream/nautilus",
  "entries": {
    "Tracing Benchmark": [
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "18086f29d2438a35fecb8316468c3287ac553d17",
          "message": "Use getFunctionName() in MLIR backend for function names\n\n- Changed MLIR backend to use getFunctionName() instead of getFunctionSymbol()\n- This ensures normalized/demangled function names from tracing are used in MLIR IR\n- Function names now respect engine.normalizeFunctionNames and\n  engine.demangleFunctionNames options\n- Provides cleaner, more readable MLIR IR output with human-readable function names\n\nWhen normalizeFunctionNames=true: functions appear as runtimeFunc0, runtimeFunc1, etc.\nWhen demangleFunctionNames=true (default): functions appear as add(int, int), foo::bar(), etc.\nWhen demangleFunctionNames=false: functions appear as _Z3addii, _ZN3foo3barEv, etc.",
          "timestamp": "2025-12-13T21:38:52+07:00",
          "tree_id": "4b30ae2bcde2f380fc0593605110ee18613b1e9d",
          "url": "https://github.com/nebulastream/nautilus/commit/18086f29d2438a35fecb8316468c3287ac553d17"
        },
        "date": 1765636967515,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 121.62,
            "range": "± 5.21177",
            "unit": "ns",
            "extra": "100 samples\n238 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 390.034,
            "range": "± 33.0451",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.993495,
            "range": "± 0.04784770000000001",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 413.602,
            "range": "± 45.0394",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 714.986,
            "range": "± 59.2989",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 432.476,
            "range": "± 28.4311",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 527.977,
            "range": "± 87.3039",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 387.696,
            "range": "± 52.5799",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 399.391,
            "range": "± 20.5895",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 367.884,
            "range": "± 19.3535",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.7771,
            "range": "± 162.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.39229,
            "range": "± 241.904",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.39193,
            "range": "± 142.889",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.40053,
            "range": "± 155.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.1405,
            "range": "± 457.411",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.0421,
            "range": "± 594.061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.4205,
            "range": "± 419.351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.35937,
            "range": "± 111.828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.748,
            "range": "± 471.924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.5868,
            "range": "± 229.866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.707,
            "range": "± 98.0307",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.4477,
            "range": "± 379.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.7968,
            "range": "± 197.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.0639,
            "range": "± 421.931",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.27026,
            "range": "± 88.1442",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6045,
            "range": "± 1.25549",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.0969,
            "range": "± 3.56709",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.4686,
            "range": "± 1.3086",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.4644,
            "range": "± 2.18469",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.5176,
            "range": "± 893.115",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.6369,
            "range": "± 1.63461",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0029,
            "range": "± 929.177",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.9179,
            "range": "± 981.205",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.52289,
            "range": "± 839.338",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 51.0663,
            "range": "± 3.56033",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.38185,
            "range": "± 10.6786",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 125.845,
            "range": "± 4.47648",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.6243,
            "range": "± 21.853",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.43102,
            "range": "± 0.220596",
            "unit": "ns",
            "extra": "100 samples\n8383 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.85677,
            "range": "± 477.858",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 534.506,
            "range": "± 17.9056",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.47332,
            "range": "± 0.712946",
            "unit": "ns",
            "extra": "100 samples\n6582 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.6048,
            "range": "± 9.52229",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8997,
            "range": "± 74.7898",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.1458,
            "range": "± 2.53229",
            "unit": "ns",
            "extra": "100 samples\n958 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 815.203,
            "range": "± 6.0116",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.397,
            "range": "± 911.024",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 501.772,
            "range": "± 24.0774",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.36773,
            "range": "± 119.67",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.33955,
            "range": "± 524.117",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.75419,
            "range": "± 167.362",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.28344,
            "range": "± 429.172",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.98032,
            "range": "± 172.151",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.54529,
            "range": "± 274.802",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.19701,
            "range": "± 114.512",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.00372,
            "range": "± 157.751",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.47806,
            "range": "± 154.002",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.5962,
            "range": "± 864.569",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 197.42,
            "range": "± 17.0491",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.7841,
            "range": "± 1.47028",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 347.655,
            "range": "± 8.57272",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "5b778cda76c3592a812c2c3cd18be69308108f22",
          "message": "feat: Add LLVM IR regression testing framework\n\nThis commit introduces a comprehensive LLVM IR regression testing\nframework to ensure generated LLVM IR remains stable across changes.\n\n- **LLVM IR Regression Tests**: New test suite that compares generated\n  LLVM IR against reference files using llvm-diff\n- **CMake Option**: Added ENABLE_LLVM_IR_TEST flag (default: OFF) to\n  control when IR tests are built\n- **GitHub Action**: New CI job that runs IR tests with clang-21 to\n  ensure llvm-diff-21 compatibility\n\n- **Executable::getGeneratedFile()**: Implemented method to retrieve\n  generated IR files from compiled executables\n- **DumpHandler Fixes**:\n  - Changed map key type from std::string_view to std::string to avoid\n    dangling reference issues\n  - Fixed lambda capture in LLVMIROptimizer to capture DumpHandler by\n    reference instead of by value, ensuring generated files are\n    properly tracked\n\n- Created nautilus/test/llvm-ir-test/ directory with:\n  - LLVMIRTest.cpp: Test implementation\n  - reference-ir/test_add.ll: Reference IR for basic add operation\n  - CMakeLists.txt: Test configuration\n- Tests use llvm-diff-19 (or later) to compare generated IR with\n  reference files\n- Handles LLVM version differences (e.g., captures attribute)\n\n- New llvm-ir-test job in pr.yml workflow\n- Runs on ubuntu-24.04 with clang-21 and llvm-21\n- Ensures IR tests pass before PRs can be merged\n\nThe test framework:\n1. Compiles Nautilus functions using the MLIR backend\n2. Captures generated LLVM IR via DumpHandler\n3. Compares against reference IR using llvm-diff\n4. Reports any differences as test failures\n\nThis enables catching unintended IR changes early in development.\n\n🤖 Generated with [Claude Code](https://claude.com/claude-code)\n\nCo-Authored-By: Claude Sonnet 4.5 <noreply@anthropic.com>",
          "timestamp": "2025-12-14T00:03:49+07:00",
          "tree_id": "5f1e5bb9c6d15fce7a922bc2e306f22d27aeddba",
          "url": "https://github.com/nebulastream/nautilus/commit/5b778cda76c3592a812c2c3cd18be69308108f22"
        },
        "date": 1765645707209,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 501.576,
            "range": "± 27.9488",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.38921,
            "range": "± 452.396",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.08417,
            "range": "± 539.28",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.68541,
            "range": "± 315.212",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.19874,
            "range": "± 468.214",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.99131,
            "range": "± 275.161",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.51203,
            "range": "± 336.271",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.15915,
            "range": "± 246.948",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.97223,
            "range": "± 286.625",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.42143,
            "range": "± 267.433",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.6423,
            "range": "± 2.32969",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 215.032,
            "range": "± 7.3552",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.2514,
            "range": "± 2.05973",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 325.11,
            "range": "± 10.6707",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 110.194,
            "range": "± 7.05993",
            "unit": "ns",
            "extra": "100 samples\n272 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 374.226,
            "range": "± 30.5984",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.9940220000000001,
            "range": "± 0.107455",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 393.988,
            "range": "± 27.6418",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 745.6,
            "range": "± 81.3475",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 434.735,
            "range": "± 36.4295",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 507.143,
            "range": "± 34.6123",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 393.272,
            "range": "± 36.6693",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 439.672,
            "range": "± 47.8244",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 362.209,
            "range": "± 23.4344",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.45202,
            "range": "± 0.274353",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.00127,
            "range": "± 997.951",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 549.511,
            "range": "± 35.4384",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.43684,
            "range": "± 0.608329",
            "unit": "ns",
            "extra": "100 samples\n6599 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.1143,
            "range": "± 6.94456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9019,
            "range": "± 65.3035",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.5867,
            "range": "± 2.62079",
            "unit": "ns",
            "extra": "100 samples\n979 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 915.458,
            "range": "± 5.36577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 182.263,
            "range": "± 18.6235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.36159,
            "range": "± 122.616",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.9003,
            "range": "± 1.56165",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.6665,
            "range": "± 5.84072",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5194,
            "range": "± 1.59517",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.7083,
            "range": "± 3.25241",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7249,
            "range": "± 1.85414",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.1555,
            "range": "± 2.80108",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.5176,
            "range": "± 1.55067",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2326,
            "range": "± 1.99207",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.64622,
            "range": "± 1.11984",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.2343,
            "range": "± 7.21782",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34453,
            "range": "± 32.3413",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 126.831,
            "range": "± 7.17397",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.59663,
            "range": "± 33.6132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.95589,
            "range": "± 167.335",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.57496,
            "range": "± 161.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.50838,
            "range": "± 132.786",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.56226,
            "range": "± 178.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.9591,
            "range": "± 430.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2272,
            "range": "± 244.133",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1124,
            "range": "± 496.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.45823,
            "range": "± 149.823",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8313,
            "range": "± 193.979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7899,
            "range": "± 165.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0224,
            "range": "± 359.664",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.0732,
            "range": "± 420.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9811,
            "range": "± 234.944",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1114,
            "range": "± 472.77",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.7305,
            "range": "± 396.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.4145,
            "range": "± 510.494",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.4658,
            "range": "± 483.993",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.9302,
            "range": "± 586.218",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.9976,
            "range": "± 759.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.8937,
            "range": "± 805.872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.3852,
            "range": "± 608.201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.3403,
            "range": "± 361.122",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.9882,
            "range": "± 716.264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.6043,
            "range": "± 486.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.5071,
            "range": "± 451.119",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.0397,
            "range": "± 1.30706",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.4001,
            "range": "± 741.641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.6137,
            "range": "± 1.03611",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 880.615,
            "range": "± 62.8798",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.5012,
            "range": "± 205.545",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.83852,
            "range": "± 536.46",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.57445,
            "range": "± 239.804",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.68927,
            "range": "± 576.083",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.68821,
            "range": "± 253.473",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.21197,
            "range": "± 253.805",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.51849,
            "range": "± 151.403",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.65364,
            "range": "± 188.494",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.40611,
            "range": "± 199.138",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 17.9755,
            "range": "± 1.34947",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 177.472,
            "range": "± 6.58246",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.374,
            "range": "± 1.9475",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 279.219,
            "range": "± 20.2902",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "e3de5548e710d8a82d6bb4dd36cb191a47a0c7d7",
          "message": "Fix 'annotate' attribute warning on non-Clang compilers\n\nThe NAUTILUS_INLINE macro uses __attribute__((annotate(...))) which is\nClang-specific. When building with GCC or other compilers, this attribute\nis ignored and generates warnings.\n\nChanges:\n- Made NAUTILUS_INLINE conditional based on compiler\n- Only apply annotate attribute when using Clang\n- Use empty macro for GCC and other compilers (no-op)\n\nThis is correct because:\n- The nautilus inlining LLVM pass only works with Clang\n- The annotate attribute is only used by this Clang-specific pass\n- Function inlining tests are already skipped for non-Clang compilers\n\nFixes warnings like:\nwarning: 'annotate' attribute directive ignored [-Wattributes]",
          "timestamp": "2025-12-14T12:34:01+07:00",
          "tree_id": "a5edaad1aed42521f8117ed32ebb3f962e567b0c",
          "url": "https://github.com/nebulastream/nautilus/commit/e3de5548e710d8a82d6bb4dd36cb191a47a0c7d7"
        },
        "date": 1765690744362,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 546.037,
            "range": "± 45.8151",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.44622,
            "range": "± 214.829",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.09987,
            "range": "± 698.078",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.65515,
            "range": "± 164.759",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.1018,
            "range": "± 470.716",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.97146,
            "range": "± 216.134",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.39974,
            "range": "± 291.092",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.05828,
            "range": "± 126.653",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.83718,
            "range": "± 151.254",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.39481,
            "range": "± 140.422",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.023,
            "range": "± 1.80294",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 216.59,
            "range": "± 8.0475",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.2741,
            "range": "± 1.83073",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 324.611,
            "range": "± 9.38854",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.91335,
            "range": "± 233.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.50353,
            "range": "± 92.4374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.47862,
            "range": "± 94.0973",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.5336,
            "range": "± 140.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.6731,
            "range": "± 198.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2321,
            "range": "± 195.817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.8279,
            "range": "± 184.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.4979,
            "range": "± 344.976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8236,
            "range": "± 121.943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8008,
            "range": "± 139.511",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.7873,
            "range": "± 80.4517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.681,
            "range": "± 330.177",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9042,
            "range": "± 81.6382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.831,
            "range": "± 160.273",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.46069,
            "range": "± 0.337525",
            "unit": "ns",
            "extra": "100 samples\n8454 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.95327,
            "range": "± 734.048",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 531.04,
            "range": "± 19.0346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.46002,
            "range": "± 0.606296",
            "unit": "ns",
            "extra": "100 samples\n6635 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.0607,
            "range": "± 7.6439",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9201,
            "range": "± 94.6683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 32.9893,
            "range": "± 3.99111",
            "unit": "ns",
            "extra": "100 samples\n949 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 923.053,
            "range": "± 10.3612",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.354,
            "range": "± 318.883",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.35379,
            "range": "± 69.6444",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8034,
            "range": "± 1.37319",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.4052,
            "range": "± 6.88693",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.6249,
            "range": "± 1.15008",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.9758,
            "range": "± 2.36488",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7333,
            "range": "± 1.16959",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.2671,
            "range": "± 3.57547",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.3591,
            "range": "± 912.427",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.3928,
            "range": "± 1.09574",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.94791,
            "range": "± 1.13151",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 54.1704,
            "range": "± 4.52675",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.41183,
            "range": "± 21.3582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 127.141,
            "range": "± 7.23632",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.65013,
            "range": "± 15.1362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 105.833,
            "range": "± 5.41777",
            "unit": "ns",
            "extra": "100 samples\n281 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 380.173,
            "range": "± 25.3242",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00368,
            "range": "± 46.8359",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 390.268,
            "range": "± 22.4751",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 720.677,
            "range": "± 51.1999",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 414.586,
            "range": "± 29.9672",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 496.579,
            "range": "± 33.1828",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 371.287,
            "range": "± 33.1007",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 432.902,
            "range": "± 50.079",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 371.333,
            "range": "± 39.1065",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "e51033fc61ba8e5a6cc44ceb6e1c766f4decba8d",
          "message": "Fix unused variable warning in traceConstant template\n\nThe cached context pointer was not being used since traceConstant(Type, ConstantLiteral)\ninternally calls TraceContext::get() again. Revert to using inTracer() for the check.",
          "timestamp": "2025-12-14T13:12:51+07:00",
          "tree_id": "cf48967a9f8e908829b552553196cf1ce332cd81",
          "url": "https://github.com/nebulastream/nautilus/commit/e51033fc61ba8e5a6cc44ceb6e1c766f4decba8d"
        },
        "date": 1765693059017,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.66246,
            "range": "± 0.774555",
            "unit": "ns",
            "extra": "100 samples\n8434 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89938,
            "range": "± 857.254",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 535.06,
            "range": "± 24.0957",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.55514,
            "range": "± 0.740986",
            "unit": "ns",
            "extra": "100 samples\n6642 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.5276,
            "range": "± 11.1615",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8859,
            "range": "± 54.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 33.697,
            "range": "± 6.06199",
            "unit": "ns",
            "extra": "100 samples\n986 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 814.001,
            "range": "± 4.21399",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.129,
            "range": "± 627.303",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.90788,
            "range": "± 156.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.53608,
            "range": "± 179.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.47408,
            "range": "± 137.955",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.5835,
            "range": "± 221.505",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.8106,
            "range": "± 245.978",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3103,
            "range": "± 181.888",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2301,
            "range": "± 399.683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.43399,
            "range": "± 133.737",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8584,
            "range": "± 188.278",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8484,
            "range": "± 210.616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.7429,
            "range": "± 199.618",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.9154,
            "range": "± 509.065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9306,
            "range": "± 181.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1017,
            "range": "± 307.31",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.8873,
            "range": "± 434.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.4068,
            "range": "± 294.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.4822,
            "range": "± 339.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.7416,
            "range": "± 330.033",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.9022,
            "range": "± 541.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.9724,
            "range": "± 332.954",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.3754,
            "range": "± 488.31",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.4315,
            "range": "± 393.193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.9808,
            "range": "± 638.008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.8112,
            "range": "± 422.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.6366,
            "range": "± 324.908",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.9529,
            "range": "± 386.097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.085,
            "range": "± 410.85",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.9767,
            "range": "± 446.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 903.195,
            "range": "± 71.5867",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.43485,
            "range": "± 187.016",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.82944,
            "range": "± 568.561",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.46346,
            "range": "± 192.899",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.38959,
            "range": "± 379.432",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.60299,
            "range": "± 235.636",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.13917,
            "range": "± 261.795",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.58198,
            "range": "± 188.597",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.67439,
            "range": "± 279.015",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.29998,
            "range": "± 192.151",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.0936,
            "range": "± 1.23332",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 175.278,
            "range": "± 6.6141",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.5156,
            "range": "± 2.33023",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 279.732,
            "range": "± 11.5556",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.697,
            "range": "± 12.3735",
            "unit": "ns",
            "extra": "100 samples\n265 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.166,
            "range": "± 33.6269",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00603,
            "range": "± 93.6643",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 408.086,
            "range": "± 24.0869",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 750.573,
            "range": "± 57.0204",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 429.555,
            "range": "± 26.7731",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 540.214,
            "range": "± 46.3143",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 461.422,
            "range": "± 39.4966",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 443.152,
            "range": "± 43.5825",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 377.339,
            "range": "± 53.2763",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ir_add",
            "value": 523.914,
            "range": "± 41.0663",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.47076,
            "range": "± 168.807",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.54897,
            "range": "± 673.69",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.90849,
            "range": "± 199.255",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.72649,
            "range": "± 492.105",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.18335,
            "range": "± 263.561",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.69855,
            "range": "± 275.88",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.38665,
            "range": "± 403.09",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.11733,
            "range": "± 252.772",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.6637,
            "range": "± 425.513",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.0427,
            "range": "± 1.29542",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 199.287,
            "range": "± 7.2607",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.1184,
            "range": "± 1.37022",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 358.103,
            "range": "± 12.3944",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.3145,
            "range": "± 94.5422",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8172,
            "range": "± 1.38603",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.4374,
            "range": "± 5.42801",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5772,
            "range": "± 1.12892",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.5218,
            "range": "± 2.90205",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.6553,
            "range": "± 1.71225",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.5282,
            "range": "± 1.81425",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.3292,
            "range": "± 1.43471",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.9824,
            "range": "± 1.68966",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.85246,
            "range": "± 1.77789",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 51.5342,
            "range": "± 4.10103",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.21768,
            "range": "± 49.7608",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 126.242,
            "range": "± 9.88186",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45245,
            "range": "± 33.1673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "4d7a0e4450ada1ceff3acc2e4c8531b9baeec080",
          "message": "Fix formatting with clang-format\n\nReorder includes in LLVMIRTest.cpp to match project style guidelines.",
          "timestamp": "2025-12-14T16:55:45+07:00",
          "tree_id": "76f8d1ef8864ae6139a86abced8a626d351aa161",
          "url": "https://github.com/nebulastream/nautilus/commit/4d7a0e4450ada1ceff3acc2e4c8531b9baeec080"
        },
        "date": 1765706369854,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.44296,
            "range": "± 0.306085",
            "unit": "ns",
            "extra": "100 samples\n8445 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.83642,
            "range": "± 659.703",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 239.49,
            "range": "± 22.4024",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.35321,
            "range": "± 0.0691582",
            "unit": "ns",
            "extra": "100 samples\n6612 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.5288,
            "range": "± 10.9073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.4913,
            "range": "± 208.55",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.4145,
            "range": "± 6.18103",
            "unit": "ns",
            "extra": "100 samples\n995 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 948.76,
            "range": "± 105.9",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.931,
            "range": "± 18.0055",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 529.144,
            "range": "± 55.206",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.45251,
            "range": "± 182.198",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.3011,
            "range": "± 631.179",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.86028,
            "range": "± 236.56",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.71617,
            "range": "± 667.337",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.07611,
            "range": "± 208.036",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.69067,
            "range": "± 373.198",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.24805,
            "range": "± 197.097",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.29496,
            "range": "± 561.137",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.51738,
            "range": "± 145.806",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.5668,
            "range": "± 1.04123",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 197.207,
            "range": "± 11.3557",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.9273,
            "range": "± 1.76886",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 345.956,
            "range": "± 8.93194",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.37803,
            "range": "± 120.162",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.7254,
            "range": "± 787.547",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.5482,
            "range": "± 5.28953",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5537,
            "range": "± 1.54738",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.6782,
            "range": "± 3.38332",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.6471,
            "range": "± 1.67555",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.8566,
            "range": "± 3.24234",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.3565,
            "range": "± 1.4104",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2994,
            "range": "± 2.05513",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.53542,
            "range": "± 1.21947",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 51.9424,
            "range": "± 7.65911",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.17003,
            "range": "± 38.6536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 124.79,
            "range": "± 9.88758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.4091,
            "range": "± 46.6573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.9057,
            "range": "± 157.299",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.55717,
            "range": "± 183.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.50775,
            "range": "± 155.449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.70994,
            "range": "± 158.514",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.6579,
            "range": "± 259.088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6388,
            "range": "± 663.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.1472,
            "range": "± 2.33844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.39235,
            "range": "± 143.655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.3493,
            "range": "± 334.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.344,
            "range": "± 338.063",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.088,
            "range": "± 671.217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.45,
            "range": "± 321.451",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3698,
            "range": "± 233.334",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.9269,
            "range": "± 236.355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.86,
            "range": "± 461.357",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.8406,
            "range": "± 808.312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.2218,
            "range": "± 596.38",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.5512,
            "range": "± 586.602",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.3608,
            "range": "± 592.592",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.2572,
            "range": "± 727.588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.8745,
            "range": "± 875.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.1565,
            "range": "± 443.839",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.2168,
            "range": "± 730.315",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.1132,
            "range": "± 835.565",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.1201,
            "range": "± 709.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.0747,
            "range": "± 1.0332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.0495,
            "range": "± 603.99",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.3383,
            "range": "± 464.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 902.191,
            "range": "± 64.6294",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.45629,
            "range": "± 220.081",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.68508,
            "range": "± 444.216",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.45542,
            "range": "± 196.756",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.4508,
            "range": "± 466.991",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.57271,
            "range": "± 222.25",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.13456,
            "range": "± 344.592",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.55968,
            "range": "± 263.032",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.6301,
            "range": "± 348.389",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.24836,
            "range": "± 159.418",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.1927,
            "range": "± 1.94192",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 174.008,
            "range": "± 6.87346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.9628,
            "range": "± 2.29841",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 275.544,
            "range": "± 8.90603",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 121.896,
            "range": "± 21.4475",
            "unit": "ns",
            "extra": "100 samples\n270 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 395.158,
            "range": "± 38.6198",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01222,
            "range": "± 106.106",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 401.751,
            "range": "± 36.0954",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 736.795,
            "range": "± 67.7952",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 433.138,
            "range": "± 44.6546",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 506.665,
            "range": "± 35.5017",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 388.983,
            "range": "± 45.0587",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 428.14,
            "range": "± 31.0805",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 373.069,
            "range": "± 40.1126",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "6c209a23a9934d1142d18f05455817bc76b97fd2",
          "message": "Add LLVM IR tests for cmath intrinsics with MLIR integration\n\nThis commit adds comprehensive LLVM IR tests for cmath intrinsics,\ntesting both with and without MLIR intrinsic lowering enabled.\n\nKey changes:\n- Implement MLIRMathIntrinsicPlugin for lowering cmath operations to LLVM intrinsics\n- Register math intrinsic plugin in MLIRCompilationBackend\n- Add 19 cmath test functions (sin, cos, sqrt, exp, log, ceil, floor, fabs,\n  tan, asin, acos, atan, pow, atan2, fmin, fmax, fmod, copysign, fma)\n- Generate 38 LLVM IR reference files (with/without intrinsics for each function)\n- Integrate tests into main LLVM IR test executable\n\nThe math intrinsic plugin uses template-based registration for float and\ndouble variants of unary, binary, and ternary operations, mapping to\nLLVM intrinsics like @llvm.sin.f32, @llvm.cos.f32, @llvm.sqrt.f32, etc.",
          "timestamp": "2025-12-14T22:40:38+07:00",
          "tree_id": "b534a426e5269e96a8e0dd82f30451fb120d82f3",
          "url": "https://github.com/nebulastream/nautilus/commit/6c209a23a9934d1142d18f05455817bc76b97fd2"
        },
        "date": 1765727060310,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.19557,
            "range": "± 59.1595",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.0364,
            "range": "± 742.397",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 30.7577,
            "range": "± 3.73068",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.8799,
            "range": "± 825.134",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 19.3323,
            "range": "± 1.38534",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.0585,
            "range": "± 888.24",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.4069,
            "range": "± 1.5757",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.6035,
            "range": "± 618.341",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.4136,
            "range": "± 875.855",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.27576,
            "range": "± 1.03689",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.6755,
            "range": "± 4.50866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.23834,
            "range": "± 195.863",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 122.4,
            "range": "± 4.66607",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.44538,
            "range": "± 44.5815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 515.569,
            "range": "± 26.7646",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.28659,
            "range": "± 125.454",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.01461,
            "range": "± 414.196",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.64668,
            "range": "± 174.383",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.08592,
            "range": "± 395.301",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.89408,
            "range": "± 215.868",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.38841,
            "range": "± 204.798",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.08561,
            "range": "± 116.85",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.91012,
            "range": "± 326.094",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.38671,
            "range": "± 147.787",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 14.3242,
            "range": "± 850.846",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 219.268,
            "range": "± 21.2491",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 26.9971,
            "range": "± 1.50387",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 323.427,
            "range": "± 8.24644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 112.115,
            "range": "± 4.93644",
            "unit": "ns",
            "extra": "100 samples\n259 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 381.582,
            "range": "± 23.4111",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.977718,
            "range": "± 0.0809998",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 402.512,
            "range": "± 19.1193",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 729.42,
            "range": "± 56.2102",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 420.615,
            "range": "± 24.3032",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 508.601,
            "range": "± 24.1742",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 387.546,
            "range": "± 21.001",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 429.089,
            "range": "± 25.6362",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 358.254,
            "range": "± 14.5186",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.91845,
            "range": "± 444.077",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.42706,
            "range": "± 62.9584",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.42153,
            "range": "± 126.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.39054,
            "range": "± 37.0803",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2777,
            "range": "± 66.1744",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.9578,
            "range": "± 41.5111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.4113,
            "range": "± 55.5079",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.35386,
            "range": "± 39.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.6394,
            "range": "± 119.451",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.6192,
            "range": "± 39.4377",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.6581,
            "range": "± 56.6204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.493,
            "range": "± 207.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.7087,
            "range": "± 33.8703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.7134,
            "range": "± 78.1977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.50565,
            "range": "± 0.47008",
            "unit": "ns",
            "extra": "100 samples\n8471 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.81397,
            "range": "± 730.457",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 518.289,
            "range": "± 13.6121",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.49139,
            "range": "± 0.799699",
            "unit": "ns",
            "extra": "100 samples\n6630 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.0165,
            "range": "± 6.9852",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9048,
            "range": "± 136.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 34.9068,
            "range": "± 1.74794",
            "unit": "ns",
            "extra": "100 samples\n830 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 914.605,
            "range": "± 6.31943",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 197.228,
            "range": "± 3.27647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "6735017a60ef9bc84053eda886b04899c67cc743",
          "message": "This commit adds support for MLIR intrinsics for memory operations similar to the existing math intrinsics implementation:\n\n    Created MLIRMemoryIntrinsics plugin to register memcpy, memmove, and memset intrinsics\n    Added registration in MLIRCompilationBackend\n Created test functions in MemoryIntrinsicFunctions.hpp\n    Added comprehensive LLVM IR tests with reference files for both intrinsics enabled and disabled modes\n    All tests pass with proper formatting",
          "timestamp": "2025-12-16T17:53:22+07:00",
          "tree_id": "9e623298401af451eb6982ff9eb1366f4d0e6677",
          "url": "https://github.com/nebulastream/nautilus/commit/6735017a60ef9bc84053eda886b04899c67cc743"
        },
        "date": 1765882621721,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 111.554,
            "range": "± 8.54416",
            "unit": "ns",
            "extra": "100 samples\n265 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 377.565,
            "range": "± 38.8253",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10345,
            "range": "± 320.066",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 405.806,
            "range": "± 28.1738",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 733.553,
            "range": "± 97.3057",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 427.086,
            "range": "± 38.0479",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 521.095,
            "range": "± 32.7018",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 450.184,
            "range": "± 26.1122",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 433.94,
            "range": "± 36.0845",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 355.736,
            "range": "± 19.0752",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.82936,
            "range": "± 1.11044",
            "unit": "ns",
            "extra": "100 samples\n8471 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.8186,
            "range": "± 604.923",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 532.551,
            "range": "± 27.4188",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.40021,
            "range": "± 0.376284",
            "unit": "ns",
            "extra": "100 samples\n6645 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 93.8402,
            "range": "± 5.15078",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9925,
            "range": "± 346.046",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.5334,
            "range": "± 2.3402",
            "unit": "ns",
            "extra": "100 samples\n988 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 816.57,
            "range": "± 7.08505",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.573,
            "range": "± 5.53065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.38258,
            "range": "± 121.722",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.7964,
            "range": "± 1.74073",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.6292,
            "range": "± 6.59404",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5052,
            "range": "± 1.71817",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.5308,
            "range": "± 3.30493",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.8834,
            "range": "± 1.61998",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.7348,
            "range": "± 3.30311",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.136,
            "range": "± 1.34223",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2008,
            "range": "± 1.8518",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.75844,
            "range": "± 1.44702",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.3726,
            "range": "± 6.06445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.20983,
            "range": "± 47.8993",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 124.819,
            "range": "± 9.65368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.47789,
            "range": "± 40.4724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 509.096,
            "range": "± 30.8372",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48522,
            "range": "± 240.833",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.43654,
            "range": "± 593.091",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.95493,
            "range": "± 434.862",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.58305,
            "range": "± 466.065",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.19527,
            "range": "± 249.192",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.57159,
            "range": "± 277.993",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.27911,
            "range": "± 191.927",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.17647,
            "range": "± 272.52",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.54848,
            "range": "± 158.068",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.7635,
            "range": "± 1.07304",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 196.635,
            "range": "± 7.55012",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.188,
            "range": "± 1.55869",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 370.304,
            "range": "± 19.5132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.45506,
            "range": "± 303.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.30292,
            "range": "± 388.49",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.20996,
            "range": "± 499.592",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.0707,
            "range": "± 416.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.7564,
            "range": "± 1.15115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.237,
            "range": "± 266.208",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2108,
            "range": "± 1.75725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.43046,
            "range": "± 110.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.435,
            "range": "± 794.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.0493,
            "range": "± 457.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.7802,
            "range": "± 206.155",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.5998,
            "range": "± 225.704",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0366,
            "range": "± 172.162",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.9925,
            "range": "± 460.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.1792,
            "range": "± 492.639",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.2116,
            "range": "± 625.382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.2551,
            "range": "± 700.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.1461,
            "range": "± 452.531",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.879,
            "range": "± 477.632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.8704,
            "range": "± 799.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.1913,
            "range": "± 1.91354",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.1038,
            "range": "± 767.34",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.2043,
            "range": "± 1.88146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.8216,
            "range": "± 854.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.0758,
            "range": "± 694.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.8984,
            "range": "± 831.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.3055,
            "range": "± 836.501",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.8674,
            "range": "± 1.93287",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 887.116,
            "range": "± 53.8462",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.381,
            "range": "± 196.384",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.71131,
            "range": "± 492.56",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.37645,
            "range": "± 154.632",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.40332,
            "range": "± 510.656",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.48416,
            "range": "± 163.725",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.04419,
            "range": "± 258.016",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.53688,
            "range": "± 150.909",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.52872,
            "range": "± 192.94",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.28285,
            "range": "± 273.033",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.1752,
            "range": "± 1.7743",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 175.34,
            "range": "± 5.49454",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.7987,
            "range": "± 1.83458",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 276.714,
            "range": "± 14.1488",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "nilslpschubert@gmail.com",
            "name": "Nils L. Schubert",
            "username": "keyseven123"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "db9b81f0b0fddb6e18acd775deb9432810cbf284",
          "message": "Adds possibility to print nautilus pointer as hex\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-12-16T18:34:35+07:00",
          "tree_id": "ffdaf587d50b9710d144d89eaeea4ffd096baab9",
          "url": "https://github.com/nebulastream/nautilus/commit/db9b81f0b0fddb6e18acd775deb9432810cbf284"
        },
        "date": 1765885089866,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.65272,
            "range": "± 0.713809",
            "unit": "ns",
            "extra": "100 samples\n8450 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.8322,
            "range": "± 629.253",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 540.491,
            "range": "± 24.1812",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.36274,
            "range": "± 0.167857",
            "unit": "ns",
            "extra": "100 samples\n6643 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.7864,
            "range": "± 6.92889",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0039,
            "range": "± 561.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.734,
            "range": "± 2.7036",
            "unit": "ns",
            "extra": "100 samples\n987 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 815.516,
            "range": "± 7.06547",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 177.99,
            "range": "± 1.30837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.9003,
            "range": "± 137.88",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.53448,
            "range": "± 198.231",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.46818,
            "range": "± 117.788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.48552,
            "range": "± 168.602",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.5527,
            "range": "± 380.562",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.0358,
            "range": "± 119.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.2315,
            "range": "± 470.751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.5854,
            "range": "± 179.637",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.5578,
            "range": "± 280.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.519,
            "range": "± 259.362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.4148,
            "range": "± 236.926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.8757,
            "range": "± 301.643",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5876,
            "range": "± 228.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.4209,
            "range": "± 266.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.9577,
            "range": "± 445.123",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0109,
            "range": "± 522.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.903,
            "range": "± 516.791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.6261,
            "range": "± 526.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.9462,
            "range": "± 411.964",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3125,
            "range": "± 1.43869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.1669,
            "range": "± 395.486",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.8006,
            "range": "± 518.095",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.2067,
            "range": "± 366.934",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.9598,
            "range": "± 479.391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.0094,
            "range": "± 399.976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.6327,
            "range": "± 588.997",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.6298,
            "range": "± 714.521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.0009,
            "range": "± 1.50621",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 934.634,
            "range": "± 128.985",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.47132,
            "range": "± 248.142",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.74461,
            "range": "± 568.29",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.43366,
            "range": "± 272.027",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.34333,
            "range": "± 499.339",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.72552,
            "range": "± 457.715",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.24284,
            "range": "± 574.452",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.78393,
            "range": "± 294.781",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.59664,
            "range": "± 313.677",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.37436,
            "range": "± 411.95",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.7054,
            "range": "± 3.08654",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 174.324,
            "range": "± 8.85736",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.0822,
            "range": "± 2.36514",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 275.906,
            "range": "± 9.81869",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 553.811,
            "range": "± 82.0418",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.65603,
            "range": "± 361.711",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.15523,
            "range": "± 1.41812",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.24223,
            "range": "± 584.456",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.61031,
            "range": "± 609.81",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.41034,
            "range": "± 656.063",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.00983,
            "range": "± 696.323",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.39267,
            "range": "± 406.652",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.32397,
            "range": "± 360.61",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.78808,
            "range": "± 420.327",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.7868,
            "range": "± 1.17054",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 197.095,
            "range": "± 7.32575",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 31.029,
            "range": "± 4.58097",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 356.91,
            "range": "± 10.4886",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.41832,
            "range": "± 185.706",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.1561,
            "range": "± 1.85132",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.9724,
            "range": "± 7.3507",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8833,
            "range": "± 1.84407",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.4928,
            "range": "± 3.48534",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.684,
            "range": "± 2.36414",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.2945,
            "range": "± 3.27899",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.0639,
            "range": "± 2.05106",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.5835,
            "range": "± 2.12152",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.1278,
            "range": "± 1.49085",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.6474,
            "range": "± 12.9451",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.21719,
            "range": "± 75.1625",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 129.663,
            "range": "± 19.4186",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.48875,
            "range": "± 76.7882",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 141.846,
            "range": "± 33.7727",
            "unit": "ns",
            "extra": "100 samples\n272 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 385.118,
            "range": "± 49.0387",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.06329,
            "range": "± 141.9",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 443.112,
            "range": "± 76.3334",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 761.876,
            "range": "± 79.3315",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 440.587,
            "range": "± 54.9671",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 557.368,
            "range": "± 81.8005",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 538.377,
            "range": "± 108.981",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 465.079,
            "range": "± 88.7986",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 381.999,
            "range": "± 43.7973",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "nilslpschubert@gmail.com",
            "name": "Nils L. Schubert",
            "username": "keyseven123"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "d43978c507559b444a5edb2ba4cefe5abdf97e69",
          "message": "Changed member visibility of value in base_ptr_val to protected\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-12-16T18:38:19+07:00",
          "tree_id": "24c106a3684a5b2335e9a234b8db05b21d29c09c",
          "url": "https://github.com/nebulastream/nautilus/commit/d43978c507559b444a5edb2ba4cefe5abdf97e69"
        },
        "date": 1765885363429,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 118.564,
            "range": "± 17.5388",
            "unit": "ns",
            "extra": "100 samples\n271 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 380.289,
            "range": "± 31.6001",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.991066,
            "range": "± 0.0927105",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 400.501,
            "range": "± 27.5873",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 766.126,
            "range": "± 80.8291",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 419.484,
            "range": "± 30.0232",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 542.381,
            "range": "± 37.1725",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 466.714,
            "range": "± 40.3281",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 436.071,
            "range": "± 48.9848",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 361.282,
            "range": "± 29.2624",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.6077,
            "range": "± 0.632762",
            "unit": "ns",
            "extra": "100 samples\n8218 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.82042,
            "range": "± 589.169",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 525.114,
            "range": "± 20.1726",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.43763,
            "range": "± 0.474032",
            "unit": "ns",
            "extra": "100 samples\n6630 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.9595,
            "range": "± 8.11928",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9095,
            "range": "± 120.496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 32.2423,
            "range": "± 3.79247",
            "unit": "ns",
            "extra": "100 samples\n959 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 955.998,
            "range": "± 277.798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.194,
            "range": "± 313.023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.40311,
            "range": "± 166.306",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.7674,
            "range": "± 1.71966",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.0037,
            "range": "± 5.81855",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7563,
            "range": "± 1.40599",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.7759,
            "range": "± 2.76356",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.728,
            "range": "± 1.55694",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.7212,
            "range": "± 2.978",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.2943,
            "range": "± 1.06695",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.1506,
            "range": "± 1.592",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.75509,
            "range": "± 1.05135",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.2168,
            "range": "± 5.08423",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.23276,
            "range": "± 30.3899",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 126.025,
            "range": "± 10.1867",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.48528,
            "range": "± 45.9797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.03236,
            "range": "± 158.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.65663,
            "range": "± 148.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.59798,
            "range": "± 138.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.7623,
            "range": "± 337.715",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.9748,
            "range": "± 457.621",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3291,
            "range": "± 239.066",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1455,
            "range": "± 556.034",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.53809,
            "range": "± 138.252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0196,
            "range": "± 228.617",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9679,
            "range": "± 270.06",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9099,
            "range": "± 365.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.6247,
            "range": "± 246.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.1308,
            "range": "± 457.488",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.159,
            "range": "± 394.359",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.9525,
            "range": "± 642.412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.6715,
            "range": "± 1.21832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.8343,
            "range": "± 1.0179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.7854,
            "range": "± 776.729",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.6975,
            "range": "± 502.679",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.1656,
            "range": "± 746.549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.2345,
            "range": "± 682.546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.4563,
            "range": "± 510.902",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.8979,
            "range": "± 690.081",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.6533,
            "range": "± 492.551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.7115,
            "range": "± 889.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.2096,
            "range": "± 1.2172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.0423,
            "range": "± 723.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.2024,
            "range": "± 626.399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 921.758,
            "range": "± 67.9102",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.41131,
            "range": "± 215.457",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.8404,
            "range": "± 625.931",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.43355,
            "range": "± 189.498",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.20748,
            "range": "± 269.071",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.57823,
            "range": "± 250.67",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.12832,
            "range": "± 292.099",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.58216,
            "range": "± 202.188",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.57015,
            "range": "± 206.644",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.33428,
            "range": "± 226.927",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 17.9003,
            "range": "± 1.68677",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 178.612,
            "range": "± 6.4893",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.9407,
            "range": "± 2.5154",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 282.331,
            "range": "± 9.00599",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 552.179,
            "range": "± 71.5385",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48854,
            "range": "± 229.844",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.48497,
            "range": "± 519.91",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.87303,
            "range": "± 248.318",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.74033,
            "range": "± 652.751",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.19101,
            "range": "± 346.809",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.77132,
            "range": "± 483.213",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.38127,
            "range": "± 363.331",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.16516,
            "range": "± 268.734",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.63567,
            "range": "± 205.764",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.9412,
            "range": "± 1.14921",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 203.597,
            "range": "± 8.14418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.3097,
            "range": "± 2.51544",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 353.348,
            "range": "± 10.4614",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "58b00cefa61a75d19959bc3a9ff77858e2df77dd",
          "message": "Fix MLIR math intrinsic type handling",
          "timestamp": "2025-12-16T19:23:59+07:00",
          "tree_id": "71a650f72eba93f150bfb8b201628acb0799c1ae",
          "url": "https://github.com/nebulastream/nautilus/commit/58b00cefa61a75d19959bc3a9ff77858e2df77dd"
        },
        "date": 1765888068548,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.27148,
            "range": "± 113.913",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.2622,
            "range": "± 1.41881",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.3874,
            "range": "± 4.91453",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.293,
            "range": "± 1.92104",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 19.7352,
            "range": "± 2.7381",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.2581,
            "range": "± 1.56802",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.7979,
            "range": "± 2.56769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.689,
            "range": "± 1.21838",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.6693,
            "range": "± 1.63507",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.32219,
            "range": "± 984.477",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.663,
            "range": "± 6.84217",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.33477,
            "range": "± 294.375",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 125.4,
            "range": "± 10.0897",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45031,
            "range": "± 80.7748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.14167,
            "range": "± 293.217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.73462,
            "range": "± 185.469",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.63458,
            "range": "± 157.771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.83743,
            "range": "± 240.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.4707,
            "range": "± 496.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5731,
            "range": "± 514.869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.7922,
            "range": "± 565.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.62202,
            "range": "± 207.928",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.2666,
            "range": "± 316.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3548,
            "range": "± 421.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.5432,
            "range": "± 443.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5964,
            "range": "± 414.634",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3344,
            "range": "± 473.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.9828,
            "range": "± 441.626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.48084,
            "range": "± 0.37669",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89828,
            "range": "± 716.048",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 569.885,
            "range": "± 26.2455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.43277,
            "range": "± 0.498354",
            "unit": "ns",
            "extra": "100 samples\n6640 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.9834,
            "range": "± 10.8402",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9731,
            "range": "± 579.036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.8751,
            "range": "± 4.99672",
            "unit": "ns",
            "extra": "100 samples\n995 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 914.044,
            "range": "± 9.35668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.854,
            "range": "± 282.906",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 134.064,
            "range": "± 8.38184",
            "unit": "ns",
            "extra": "100 samples\n231 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 472.893,
            "range": "± 33.4398",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.13249,
            "range": "± 116.274",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 481.82,
            "range": "± 65.5937",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 828.503,
            "range": "± 48.6487",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 470.378,
            "range": "± 47.0906",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 600.382,
            "range": "± 83.6105",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 419.373,
            "range": "± 44.0678",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 452.349,
            "range": "± 33.6934",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 385.825,
            "range": "± 32.6927",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ir_add",
            "value": 525.679,
            "range": "± 78.7566",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.4648,
            "range": "± 174.094",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.45592,
            "range": "± 580.088",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.91761,
            "range": "± 342.513",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.64041,
            "range": "± 586.389",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.11268,
            "range": "± 273.909",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.71961,
            "range": "± 363.003",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.32229,
            "range": "± 295.919",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.13518,
            "range": "± 192.324",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.53335,
            "range": "± 169.484",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.8499,
            "range": "± 1.11328",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 200.491,
            "range": "± 5.81193",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.9702,
            "range": "± 2.09243",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 358.541,
            "range": "± 10.7849",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "77a2e028e69ce0b6ed4be5867395c489f19ab9c8",
          "message": "Address comments in pr",
          "timestamp": "2025-12-16T19:38:23+07:00",
          "tree_id": "695eeda4a0265389922731d85c0d6e2d4701b0d8",
          "url": "https://github.com/nebulastream/nautilus/commit/77a2e028e69ce0b6ed4be5867395c489f19ab9c8"
        },
        "date": 1765889040394,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.27472,
            "range": "± 203.252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.26719,
            "range": "± 284.61",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.91756,
            "range": "± 188.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.5143,
            "range": "± 471.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.6748,
            "range": "± 359.14",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.729,
            "range": "± 573.961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.9634,
            "range": "± 419.117",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.63597,
            "range": "± 172.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.7241,
            "range": "± 204.3",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.2111,
            "range": "± 487.392",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.1562,
            "range": "± 234.211",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.8748,
            "range": "± 274.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.3927,
            "range": "± 288.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.9642,
            "range": "± 423.262",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.914,
            "range": "± 10.8539",
            "unit": "ns",
            "extra": "100 samples\n273 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.138,
            "range": "± 18.9418",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04824,
            "range": "± 87.3889",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 415.088,
            "range": "± 23.83",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 777.769,
            "range": "± 88.9831",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 436.145,
            "range": "± 59.2073",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 522.807,
            "range": "± 33.3198",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 445.287,
            "range": "± 26.809",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 442.615,
            "range": "± 33.4788",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 367.436,
            "range": "± 19.0669",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.30854,
            "range": "± 96.5877",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.7812,
            "range": "± 1.76619",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.5741,
            "range": "± 6.20415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5796,
            "range": "± 1.74534",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.5638,
            "range": "± 2.96328",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7035,
            "range": "± 1.99779",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.5869,
            "range": "± 3.26544",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.81496,
            "range": "± 991.373",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.0799,
            "range": "± 1.83438",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.4319,
            "range": "± 2.19584",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.3229,
            "range": "± 6.13656",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.25933,
            "range": "± 28.2427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 125.457,
            "range": "± 9.26673",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.50408,
            "range": "± 49.6581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 519.91,
            "range": "± 36.3751",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.45113,
            "range": "± 202.034",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.46979,
            "range": "± 888.963",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.79009,
            "range": "± 349.794",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.48886,
            "range": "± 394.309",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.05097,
            "range": "± 248.747",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.57427,
            "range": "± 270.345",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.1531,
            "range": "± 103.606",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.06663,
            "range": "± 206.217",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.49126,
            "range": "± 160.262",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.8172,
            "range": "± 1.17226",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 199.591,
            "range": "± 8.96027",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.6111,
            "range": "± 1.98625",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 359.2,
            "range": "± 17.8581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.44459,
            "range": "± 0.271002",
            "unit": "ns",
            "extra": "100 samples\n8629 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.98046,
            "range": "± 756.865",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 581.24,
            "range": "± 22.296",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.43172,
            "range": "± 0.514401",
            "unit": "ns",
            "extra": "100 samples\n6743 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.8122,
            "range": "± 11.5874",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9262,
            "range": "± 156.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.2313,
            "range": "± 2.69756",
            "unit": "ns",
            "extra": "100 samples\n982 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 815.906,
            "range": "± 7.33531",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.856,
            "range": "± 1.73961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "fe607618bb0683a3335e3199c7c4651523ab2a4f",
          "message": "Optimize logical operators to accept const references\n\nChange logical operator overloads (||, &&, !) to accept const references\ninstead of by-value parameters. This eliminates unnecessary copies when\ncalling these operators.\n\nRegenerate test reference traces to reflect the optimized behavior where\nfewer copy operations are recorded during tracing.\n\nChanges:\n- operator||(bool, const val<bool>&)\n- operator||(const val<bool>&, bool)\n- operator||(const val<bool>&, const val<bool>&)\n- operator&&(bool, const val<bool>&)\n- operator&&(const val<bool>&, bool)\n- operator&&(const val<bool>&, const val<bool>&)\n- operator!(const val<bool>&)",
          "timestamp": "2025-12-16T23:26:02+07:00",
          "tree_id": "b80ef31598625b15ea0cb532225f01f8de8cb31a",
          "url": "https://github.com/nebulastream/nautilus/commit/fe607618bb0683a3335e3199c7c4651523ab2a4f"
        },
        "date": 1765902632173,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 507.752,
            "range": "± 62.7883",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.30897,
            "range": "± 127.453",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.94644,
            "range": "± 336.772",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.65219,
            "range": "± 156.561",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.16073,
            "range": "± 321.044",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.87816,
            "range": "± 237.297",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.39693,
            "range": "± 180.716",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.19907,
            "range": "± 142.349",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.937,
            "range": "± 261.315",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.38396,
            "range": "± 166.102",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.0416,
            "range": "± 2.98963",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 226.03,
            "range": "± 30.7306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.5218,
            "range": "± 1.56658",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 325.639,
            "range": "± 14.4727",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.93853,
            "range": "± 41.0364",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.56594,
            "range": "± 52.6056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.53346,
            "range": "± 37.651",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.54024,
            "range": "± 32.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.4145,
            "range": "± 173.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1763,
            "range": "± 59.0891",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5782,
            "range": "± 72.9622",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.48391,
            "range": "± 46.8965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.891,
            "range": "± 598.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7703,
            "range": "± 40.2914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8003,
            "range": "± 50.8031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.6823,
            "range": "± 72.2539",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9384,
            "range": "± 347.771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2858,
            "range": "± 154.445",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.4342,
            "range": "± 253.951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.0619,
            "range": "± 329.589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.9941,
            "range": "± 242.876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.0137,
            "range": "± 256.577",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.0605,
            "range": "± 212.235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.4572,
            "range": "± 272.016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.5564,
            "range": "± 209.504",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.7827,
            "range": "± 225.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.2389,
            "range": "± 252.789",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.9713,
            "range": "± 191.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.8346,
            "range": "± 270.11",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.0784,
            "range": "± 515.479",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.3063,
            "range": "± 277.394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.8289,
            "range": "± 810.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 857.488,
            "range": "± 55.6398",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.29405,
            "range": "± 150.679",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.64745,
            "range": "± 475.478",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.37175,
            "range": "± 204.504",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.24112,
            "range": "± 273.52",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.53284,
            "range": "± 205.117",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.08702,
            "range": "± 291.302",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.45859,
            "range": "± 150.311",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.47362,
            "range": "± 152.945",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.26407,
            "range": "± 294.678",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 17.9266,
            "range": "± 1.84445",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 171.816,
            "range": "± 5.10863",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.4124,
            "range": "± 2.83589",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 276.64,
            "range": "± 9.33244",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.1665,
            "range": "± 0.38768",
            "unit": "ns",
            "extra": "100 samples\n9294 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.94843,
            "range": "± 730.272",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 520.16,
            "range": "± 11.6226",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.41029,
            "range": "± 0.401357",
            "unit": "ns",
            "extra": "100 samples\n6603 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.3895,
            "range": "± 10.8354",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9129,
            "range": "± 261.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.7097,
            "range": "± 3.37902",
            "unit": "ns",
            "extra": "100 samples\n970 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 924.442,
            "range": "± 11.1834",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.38,
            "range": "± 348.426",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 117.779,
            "range": "± 11.5108",
            "unit": "ns",
            "extra": "100 samples\n265 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 401.898,
            "range": "± 32.3111",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07023,
            "range": "± 68.053",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 414.932,
            "range": "± 25.9879",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 773.351,
            "range": "± 69.5562",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 455.698,
            "range": "± 60.6561",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 538.42,
            "range": "± 39.2376",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 401.831,
            "range": "± 31.1112",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 440.18,
            "range": "± 24.2878",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 388.994,
            "range": "± 33.1288",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.29629,
            "range": "± 93.9624",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.7284,
            "range": "± 1.5266",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.6082,
            "range": "± 2.64977",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.495,
            "range": "± 1.09691",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.8943,
            "range": "± 3.45888",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.8396,
            "range": "± 1.17406",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.2719,
            "range": "± 1.69754",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0027,
            "range": "± 878.367",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.8714,
            "range": "± 1.03731",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.57996,
            "range": "± 649.289",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.2687,
            "range": "± 5.27118",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.19257,
            "range": "± 13.5769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 124.29,
            "range": "± 5.23812",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42078,
            "range": "± 18.6583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "d824bcc50d466119d6d18a5e277ea7c055b62b60",
          "message": "Fix template function address stability for all std::bit functions\n\nAdd extern \"C\" wrapper functions for countl_one, countr_one, has_single_bit,\nbit_width, bit_ceil, and bit_floor. These template functions don't have stable\naddresses and need wrappers just like the intrinsic functions.\n\nThis fixes compilation errors on Clang where template function addresses\ncannot be used directly with invoke().",
          "timestamp": "2025-12-18T22:10:14+07:00",
          "tree_id": "bc9d1ecc85eb76a54011200eea502b8ea886f872",
          "url": "https://github.com/nebulastream/nautilus/commit/d824bcc50d466119d6d18a5e277ea7c055b62b60"
        },
        "date": 1766070838737,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 135.313,
            "range": "± 28.9967",
            "unit": "ns",
            "extra": "100 samples\n252 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 414.499,
            "range": "± 88.2379",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04731,
            "range": "± 134.656",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 425.678,
            "range": "± 59.8998",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 775.308,
            "range": "± 105.747",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 458.284,
            "range": "± 56.3911",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 578.687,
            "range": "± 103.589",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 433.104,
            "range": "± 78.6013",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 487.902,
            "range": "± 101.225",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 412.501,
            "range": "± 92.8932",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.37569,
            "range": "± 243.155",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.2451,
            "range": "± 4.12187",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.7319,
            "range": "± 5.80361",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7657,
            "range": "± 1.81393",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.9822,
            "range": "± 3.99776",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.1704,
            "range": "± 2.32284",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.7068,
            "range": "± 3.29644",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.1189,
            "range": "± 1.72121",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2119,
            "range": "± 1.92058",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.2422,
            "range": "± 1.75732",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.988,
            "range": "± 8.75474",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.17151,
            "range": "± 44.0873",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 126.102,
            "range": "± 13.5558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.39307,
            "range": "± 39.8188",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 538.556,
            "range": "± 58.8823",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.40336,
            "range": "± 266.383",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.1001,
            "range": "± 579.091",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.78601,
            "range": "± 375.524",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.25458,
            "range": "± 583.995",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.02223,
            "range": "± 334.351",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.49326,
            "range": "± 353.728",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.20561,
            "range": "± 330.251",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.92892,
            "range": "± 346.091",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.63076,
            "range": "± 524.156",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 14.7742,
            "range": "± 1.79627",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 219.035,
            "range": "± 10.0745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.9533,
            "range": "± 2.80981",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 328.623,
            "range": "± 11.0373",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.24818,
            "range": "± 0.544884",
            "unit": "ns",
            "extra": "100 samples\n9299 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.98216,
            "range": "± 788.329",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 525.153,
            "range": "± 19.2588",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.55305,
            "range": "± 0.705931",
            "unit": "ns",
            "extra": "100 samples\n6640 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.2385,
            "range": "± 7.08637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9117,
            "range": "± 134.522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 34.9663,
            "range": "± 7.38583",
            "unit": "ns",
            "extra": "100 samples\n972 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 942.214,
            "range": "± 108.8",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.901,
            "range": "± 322.293",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.05974,
            "range": "± 368.574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.63912,
            "range": "± 159.492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.56616,
            "range": "± 142.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.67601,
            "range": "± 185.562",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.7529,
            "range": "± 321.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3326,
            "range": "± 243.455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.0074,
            "range": "± 331.872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.54338,
            "range": "± 127.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8679,
            "range": "± 171.615",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8743,
            "range": "± 189.431",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9945,
            "range": "± 193.753",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.2618,
            "range": "± 234.952",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0585,
            "range": "± 198.099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.6174,
            "range": "± 438.5",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.9322,
            "range": "± 283.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.7474,
            "range": "± 423.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.0716,
            "range": "± 431.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.2754,
            "range": "± 546.555",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.8114,
            "range": "± 392.975",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.2574,
            "range": "± 396.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.5109,
            "range": "± 490.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.4793,
            "range": "± 323.852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.0547,
            "range": "± 457.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.7531,
            "range": "± 357.386",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.85,
            "range": "± 682.141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.9825,
            "range": "± 754.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.3594,
            "range": "± 587.436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.4908,
            "range": "± 999.749",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 954.893,
            "range": "± 163.307",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.59946,
            "range": "± 419.757",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.81257,
            "range": "± 841.636",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.43898,
            "range": "± 265.29",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.31182,
            "range": "± 460.939",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.59568,
            "range": "± 238.098",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.08681,
            "range": "± 215.117",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.58234,
            "range": "± 292.499",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.59307,
            "range": "± 290.886",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.24196,
            "range": "± 202.14",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.4559,
            "range": "± 2.44676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 177.672,
            "range": "± 5.97306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.9056,
            "range": "± 2.18081",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 279.442,
            "range": "± 10.4439",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "c483cee4b1bda54811abc62a011ba25ef4fe1360",
          "message": "Add comprehensive CLAUDE.md documentation for AI assistants\n\nThis document provides AI assistants with:\n- Complete codebase architecture overview\n- Directory structure and component organization\n- Code conventions and style guidelines (naming, formatting, etc.)\n- Development workflows (building, testing, CI/CD)\n- Detailed explanation of key systems (val<T>, IR, optimization phases, backends, tracing)\n- Common development patterns and best practices\n- Debugging and troubleshooting guidance\n- Performance considerations\n- AI assistant-specific guidelines and code review checklist\n\nThe guide covers all major subsystems including the tracing system, intermediate\nrepresentation, optimization passes, compilation backends, and test infrastructure.\nIt serves as a comprehensive reference for understanding and contributing to Nautilus.",
          "timestamp": "2025-12-21T15:44:06+07:00",
          "tree_id": "a688152521d20a4cea2c4968b0766420e59a65d4",
          "url": "https://github.com/nebulastream/nautilus/commit/c483cee4b1bda54811abc62a011ba25ef4fe1360"
        },
        "date": 1766306854024,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 501.164,
            "range": "± 26.4627",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.28458,
            "range": "± 138.114",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.9437,
            "range": "± 303.208",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.6959,
            "range": "± 392.496",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.09257,
            "range": "± 659.355",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.86463,
            "range": "± 235.211",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.34121,
            "range": "± 198.775",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.04489,
            "range": "± 97.8369",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.8456,
            "range": "± 137.051",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.34867,
            "range": "± 132.97",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.5756,
            "range": "± 1.80689",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 218.953,
            "range": "± 6.55205",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.9207,
            "range": "± 1.73051",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 326.806,
            "range": "± 7.13249",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 115.157,
            "range": "± 5.1312",
            "unit": "ns",
            "extra": "100 samples\n251 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 387.676,
            "range": "± 29.3746",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.997529,
            "range": "± 0.0439118",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 409.826,
            "range": "± 29.7199",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 746.521,
            "range": "± 37.8061",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 430.608,
            "range": "± 23.2509",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 535.85,
            "range": "± 30.3489",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 398.606,
            "range": "± 31.7553",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 437.242,
            "range": "± 20.0737",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 370.957,
            "range": "± 25.5544",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.01679,
            "range": "± 129.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.73598,
            "range": "± 101.235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.67683,
            "range": "± 169.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.68368,
            "range": "± 249.536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.7611,
            "range": "± 369.769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2931,
            "range": "± 252.08",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.8842,
            "range": "± 971.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.59266,
            "range": "± 103.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8284,
            "range": "± 230.015",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8357,
            "range": "± 155.123",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8022,
            "range": "± 186.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.7829,
            "range": "± 164.145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9521,
            "range": "± 107.805",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1948,
            "range": "± 314.418",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6374,
            "range": "± 276.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.1805,
            "range": "± 264.99",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.4457,
            "range": "± 348.26",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.5647,
            "range": "± 289.22",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.5415,
            "range": "± 321.139",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.924,
            "range": "± 348.774",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.8568,
            "range": "± 260.515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.1783,
            "range": "± 349.245",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.6588,
            "range": "± 404.215",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3273,
            "range": "± 264.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.3933,
            "range": "± 354.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.7381,
            "range": "± 443.552",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.711,
            "range": "± 357.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.3783,
            "range": "± 449.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 856.168,
            "range": "± 37.4672",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.32995,
            "range": "± 169.556",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.64229,
            "range": "± 385.892",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.37316,
            "range": "± 222.196",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.19747,
            "range": "± 248.466",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.48972,
            "range": "± 178.775",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.06822,
            "range": "± 306.803",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.52921,
            "range": "± 245.552",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.57103,
            "range": "± 282.292",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.21013,
            "range": "± 201.748",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 17.9557,
            "range": "± 1.85803",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 179.163,
            "range": "± 6.23536",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.6185,
            "range": "± 2.5963",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 280.75,
            "range": "± 10.2192",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.16967,
            "range": "± 0.345568",
            "unit": "ns",
            "extra": "100 samples\n9297 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.055,
            "range": "± 1.07453",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 525.89,
            "range": "± 17.1477",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.38363,
            "range": "± 0.305011",
            "unit": "ns",
            "extra": "100 samples\n6630 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.3484,
            "range": "± 7.62525",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9144,
            "range": "± 173.539",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 32.1624,
            "range": "± 5.18403",
            "unit": "ns",
            "extra": "100 samples\n978 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 913.722,
            "range": "± 4.73114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.917,
            "range": "± 524.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.32125,
            "range": "± 75.2974",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6307,
            "range": "± 1.33021",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.9159,
            "range": "± 2.57238",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.3072,
            "range": "± 1.1905",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.2446,
            "range": "± 1.68321",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.9955,
            "range": "± 2.24401",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.9995,
            "range": "± 1.87811",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.69572,
            "range": "± 688.581",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.886,
            "range": "± 1.12843",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.54391,
            "range": "± 750.272",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 51.4477,
            "range": "± 3.66865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.19871,
            "range": "± 13.2408",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 124.034,
            "range": "± 4.47198",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.51267,
            "range": "± 570.007",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "lukas.schwerdtfeger@gmail.com",
            "name": "lukas schwerdtfeger",
            "username": "ls-1801"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "109318fcf52a619b48f5152bd16249c0a00f4688",
          "message": "adjust traces for new integer promotion rules",
          "timestamp": "2025-12-25T08:59:43+07:00",
          "tree_id": "48b97823e5500b3655a61770ad9824dfb53e1555",
          "url": "https://github.com/nebulastream/nautilus/commit/109318fcf52a619b48f5152bd16249c0a00f4688"
        },
        "date": 1766628404872,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 145.531,
            "range": "± 13.6346",
            "unit": "ns",
            "extra": "100 samples\n217 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 380.031,
            "range": "± 23.4385",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.1218,
            "range": "± 317.174",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 413.754,
            "range": "± 39.4713",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 760.573,
            "range": "± 76.8024",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 429.289,
            "range": "± 40.0226",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 507.238,
            "range": "± 26.5538",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 369.536,
            "range": "± 24.6718",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 421.608,
            "range": "± 39.8128",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 362.985,
            "range": "± 16.6431",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.70027,
            "range": "± 260.666",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.34188,
            "range": "± 211.346",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.1442,
            "range": "± 276.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.4424,
            "range": "± 505.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.5477,
            "range": "± 291.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.5721,
            "range": "± 343.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.6482,
            "range": "± 268.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.77571,
            "range": "± 167.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.6906,
            "range": "± 379.583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5516,
            "range": "± 577.095",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.711,
            "range": "± 329.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.7027,
            "range": "± 282.847",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.0534,
            "range": "± 222.796",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.1976,
            "range": "± 255.383",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 900.887,
            "range": "± 55.372",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.51348,
            "range": "± 216.864",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.43281,
            "range": "± 383.976",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.87024,
            "range": "± 226.61",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.56667,
            "range": "± 391.039",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.17901,
            "range": "± 267.314",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.72248,
            "range": "± 300.429",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.27643,
            "range": "± 263.381",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.31562,
            "range": "± 808.966",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.57762,
            "range": "± 151.115",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.7264,
            "range": "± 739.694",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 204.601,
            "range": "± 6.56495",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.6083,
            "range": "± 1.58282",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 362.91,
            "range": "± 28.0792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.58435,
            "range": "± 190.619",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6501,
            "range": "± 1.51518",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.5026,
            "range": "± 7.58828",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.4747,
            "range": "± 1.36971",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.5325,
            "range": "± 3.2677",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.5855,
            "range": "± 1.80122",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.6212,
            "range": "± 3.50645",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.62847,
            "range": "± 991.075",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.9467,
            "range": "± 1.6164",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.61888,
            "range": "± 1.22025",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 51.3723,
            "range": "± 6.61357",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.24433,
            "range": "± 32.8635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 124.523,
            "range": "± 8.64418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.47416,
            "range": "± 42.4837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.47745,
            "range": "± 0.389062",
            "unit": "ns",
            "extra": "100 samples\n8390 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.86974,
            "range": "± 519.883",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 239.945,
            "range": "± 21.5665",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.43321,
            "range": "± 0.840452",
            "unit": "ns",
            "extra": "100 samples\n5463 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.8986,
            "range": "± 7.35074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.5804,
            "range": "± 103.231",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 37.3453,
            "range": "± 1.59782",
            "unit": "ns",
            "extra": "100 samples\n778 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 814.388,
            "range": "± 6.49303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.472,
            "range": "± 449.154",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "604722d8200251263dd2231f633b594a2b9ea402",
          "message": "Refactor val<T> specializations and integrate integer promotion rules\n\nThis commit combines the following changes:\n\n1. Refactor val<bool> code into dedicated documentation header\n   - Separate val<bool> specialization with probability tracking\n   - Comprehensive documentation of boolean value tracing\n\n2. Implement clean separation of val<T> specializations\n   - val_arith.hpp: Partial specialization for arithmetic types\n   - val_bool.hpp: Explicit specialization for boolean values\n   - val_details.hpp: Shared utilities (RawValueResolver, StateResolver, make_value, cast_value)\n   - val.hpp: Minimal orchestrator header\n\n3. Restore type safety with partial specialization\n   - Use partial specialization syntax for arithmetic types constraint\n   - Ensure forward declarations don't conflict with specializations\n\n4. Apply code formatting and style consistency\n   - Run clang-format with project standards\n   - Alphabetical include ordering\n   - 120-character line limit\n\n5. Update CLAUDE.md documentation\n   - Document new val<T> specialization structure\n   - Explain relationships between header files\n   - Provide architecture overview\n\n6. Refactor template operators to specialization-specific headers\n   - Move arithmetic operators to val_arith.hpp\n   - Move logical operators to val_bool.hpp\n   - Move utility functions to val_details.hpp\n   - Improve code organization and maintainability\n\n7. Integrate integer promotion rules into val_arith.hpp\n   - Add ArithmeticResultType type alias for promotion-aware type deduction\n   - Implement DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION macro\n   - Apply to operators: +, -, *, /, %, >>, <<\n   - Ensure comparison and bitwise operators remain non-promoting\n\nAll tests passing (38/38)",
          "timestamp": "2025-12-25T13:45:47+07:00",
          "tree_id": "56e9f4e8cb73e2f9fa4a2cc3d2dc33c8b2182ae3",
          "url": "https://github.com/nebulastream/nautilus/commit/604722d8200251263dd2231f633b594a2b9ea402"
        },
        "date": 1766645437279,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 9.01549,
            "range": "± 395.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.67327,
            "range": "± 639.72",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.35574,
            "range": "± 196.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.9571,
            "range": "± 444.988",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.7419,
            "range": "± 401.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.6956,
            "range": "± 250.052",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.0055,
            "range": "± 1.05607",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.15613,
            "range": "± 170.86",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.2677,
            "range": "± 245.137",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.2114,
            "range": "± 283.867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.1445,
            "range": "± 294.218",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.2406,
            "range": "± 209.885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.3914,
            "range": "± 447.954",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.3266,
            "range": "± 1.48027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 892.414,
            "range": "± 49.2343",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48486,
            "range": "± 189.439",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.65976,
            "range": "± 502.152",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.8921,
            "range": "± 239.087",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.62647,
            "range": "± 350.073",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.18696,
            "range": "± 257.186",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.66645,
            "range": "± 259.52",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.23608,
            "range": "± 127.751",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.21025,
            "range": "± 279.168",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.5528,
            "range": "± 174.864",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.005,
            "range": "± 915.448",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 205.367,
            "range": "± 6.63426",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.825,
            "range": "± 1.74506",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 360.841,
            "range": "± 9.36053",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 142.667,
            "range": "± 13.0301",
            "unit": "ns",
            "extra": "100 samples\n209 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 388.237,
            "range": "± 24.8671",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.06845,
            "range": "± 137.669",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 416.022,
            "range": "± 38.2197",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 748.867,
            "range": "± 49.6668",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 427.024,
            "range": "± 26.2505",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 535.891,
            "range": "± 51.5979",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 382.287,
            "range": "± 40.4898",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 425.226,
            "range": "± 20.6432",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 377.117,
            "range": "± 30.9152",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.44765,
            "range": "± 0.276951",
            "unit": "ns",
            "extra": "100 samples\n8451 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.01098,
            "range": "± 888.5",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 613.304,
            "range": "± 22.8162",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.33528,
            "range": "± 0.431546",
            "unit": "ns",
            "extra": "100 samples\n5464 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.7161,
            "range": "± 8.0692",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9882,
            "range": "± 280.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 38.3701,
            "range": "± 2.69467",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 814.838,
            "range": "± 5.51534",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 156.708,
            "range": "± 23.2244",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.53615,
            "range": "± 198.123",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.7131,
            "range": "± 1.74043",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.4048,
            "range": "± 6.25567",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.6212,
            "range": "± 1.61407",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.4517,
            "range": "± 2.89741",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.6609,
            "range": "± 1.71315",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.9298,
            "range": "± 3.29282",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.84205,
            "range": "± 1.00538",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.9918,
            "range": "± 1.77151",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.78047,
            "range": "± 1.66788",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.5471,
            "range": "± 6.52615",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.2545,
            "range": "± 41.5374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 128.036,
            "range": "± 14.0926",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.53434,
            "range": "± 42.7752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "1c4af1a69892f01bd56f771e29086f01f8d3f74e",
          "message": "Make s/README.md more concise",
          "timestamp": "2025-12-25T15:13:48+07:00",
          "tree_id": "3206ffdd75d7c4582b0c246f13b3de60287cfeb3",
          "url": "https://github.com/nebulastream/nautilus/commit/1c4af1a69892f01bd56f771e29086f01f8d3f74e"
        },
        "date": 1766650651887,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 145.3,
            "range": "± 18.8134",
            "unit": "ns",
            "extra": "100 samples\n218 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.61,
            "range": "± 30.9201",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04122,
            "range": "± 114.354",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 418.973,
            "range": "± 28.2434",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 754.547,
            "range": "± 45.0271",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 430.71,
            "range": "± 26.7493",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 526.656,
            "range": "± 36.4489",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 387.989,
            "range": "± 50.8692",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 425.696,
            "range": "± 36.7433",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 379.082,
            "range": "± 37.8095",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.3143,
            "range": "± 486.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.83013,
            "range": "± 268.252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.68152,
            "range": "± 194.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.88292,
            "range": "± 275.737",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.1364,
            "range": "± 307.717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.8287,
            "range": "± 277.141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.9884,
            "range": "± 375.725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.57457,
            "range": "± 164.745",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.063,
            "range": "± 608.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8844,
            "range": "± 185.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9585,
            "range": "± 215.795",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.2814,
            "range": "± 276.616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.2631,
            "range": "± 323.588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.6515,
            "range": "± 423.933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.57722,
            "range": "± 228.226",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8689,
            "range": "± 1.80935",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.592,
            "range": "± 5.97309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7876,
            "range": "± 1.94692",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.5059,
            "range": "± 3.00165",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.9774,
            "range": "± 2.04124",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.831,
            "range": "± 3.13682",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.4305,
            "range": "± 1.60663",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.3392,
            "range": "± 1.88247",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.0402,
            "range": "± 1.41781",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.7975,
            "range": "± 7.97036",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.25305,
            "range": "± 53.531",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 124.785,
            "range": "± 9.43193",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.52612,
            "range": "± 189.069",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 953.227,
            "range": "± 137.078",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48229,
            "range": "± 179.118",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.45739,
            "range": "± 598.315",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.95843,
            "range": "± 295.861",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.55527,
            "range": "± 580.881",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.15772,
            "range": "± 368.62",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.62322,
            "range": "± 318.641",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25221,
            "range": "± 194.459",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.10696,
            "range": "± 272.12",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.61761,
            "range": "± 369.177",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.449,
            "range": "± 806.725",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 199.698,
            "range": "± 8.04401",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.5324,
            "range": "± 1.84813",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 355.099,
            "range": "± 10.1539",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.47736,
            "range": "± 0.404555",
            "unit": "ns",
            "extra": "100 samples\n8457 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.93129,
            "range": "± 758.312",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 547.366,
            "range": "± 25.7318",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.35508,
            "range": "± 0.559956",
            "unit": "ns",
            "extra": "100 samples\n5470 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.6452,
            "range": "± 8.93428",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0444,
            "range": "± 730.424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 37.9315,
            "range": "± 2.52719",
            "unit": "ns",
            "extra": "100 samples\n776 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 815.441,
            "range": "± 7.08112",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.397,
            "range": "± 3.90194",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "66a9901c043b8e37214b990d38cb4e824a5b2bc7",
          "message": "Add instructions for CMake format target and Clang 21 default compiler\n\n- Add Clang 21 as the default compiler in the Building the Project section\n- Emphasize use of CMake format target for all code changes\n- Update Code Formatting section with authoritative formatting workflow\n- Add formatting requirements to Code Review Checklist\n- Include Clang 21 verification in pre-commit requirements\n- Update document version to 1.1",
          "timestamp": "2025-12-25T15:29:04+07:00",
          "tree_id": "a79210a8abdd36d17947a187aee89ed00570f4f9",
          "url": "https://github.com/nebulastream/nautilus/commit/66a9901c043b8e37214b990d38cb4e824a5b2bc7"
        },
        "date": 1766651567171,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.48635,
            "range": "± 0.505764",
            "unit": "ns",
            "extra": "100 samples\n8451 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.86516,
            "range": "± 518.699",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 527.737,
            "range": "± 28.5648",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.50688,
            "range": "± 0.831177",
            "unit": "ns",
            "extra": "100 samples\n5467 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.6204,
            "range": "± 6.99197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9139,
            "range": "± 101.975",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 39.905,
            "range": "± 4.49259",
            "unit": "ns",
            "extra": "100 samples\n784 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 817.143,
            "range": "± 6.82118",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.138,
            "range": "± 5.04187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 149.466,
            "range": "± 27.9191",
            "unit": "ns",
            "extra": "100 samples\n214 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 409.686,
            "range": "± 38.0179",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.0975,
            "range": "± 134.98",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 433.557,
            "range": "± 44.5922",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 768.721,
            "range": "± 75.2791",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 450.64,
            "range": "± 49.4973",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 545.619,
            "range": "± 68.181",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 408.617,
            "range": "± 73.449",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 440.901,
            "range": "± 47.732",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 377.53,
            "range": "± 41.9576",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.68429,
            "range": "± 311.933",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.0123,
            "range": "± 1.6949",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.1738,
            "range": "± 2.72442",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7825,
            "range": "± 1.26774",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.6285,
            "range": "± 3.48405",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.1287,
            "range": "± 1.30017",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.7495,
            "range": "± 2.60198",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.98978,
            "range": "± 1.16916",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.3967,
            "range": "± 1.53",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.0814,
            "range": "± 1.14538",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.1267,
            "range": "± 5.43435",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.2367,
            "range": "± 37.6046",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 127.491,
            "range": "± 10.4831",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.47045,
            "range": "± 24.5935",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.091,
            "range": "± 204.458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.68183,
            "range": "± 264.423",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.60911,
            "range": "± 104.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.6449,
            "range": "± 153.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.8706,
            "range": "± 281.312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3981,
            "range": "± 384.893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.0603,
            "range": "± 642.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.54013,
            "range": "± 130.547",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9138,
            "range": "± 154.514",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.925,
            "range": "± 248.342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8424,
            "range": "± 146.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.6951,
            "range": "± 212.633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0018,
            "range": "± 183.225",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.4688,
            "range": "± 425.255",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.0354,
            "range": "± 545.908",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.5845,
            "range": "± 341.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.6238,
            "range": "± 887.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.7685,
            "range": "± 553.814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.7002,
            "range": "± 346.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.9907,
            "range": "± 457.964",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.2229,
            "range": "± 513.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.7794,
            "range": "± 465.97",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.9577,
            "range": "± 400.609",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.7894,
            "range": "± 486.717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.7831,
            "range": "± 322.975",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.4758,
            "range": "± 1.53733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.0818,
            "range": "± 384.651",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.3161,
            "range": "± 581.017",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.17083,
            "range": "± 98.0922",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.57079,
            "range": "± 365.994",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.83835,
            "range": "± 624.668",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.5556,
            "range": "± 370.54",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.45745,
            "range": "± 648.529",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.7859,
            "range": "± 475.634",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.25564,
            "range": "± 341.263",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.62957,
            "range": "± 238.219",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.72415,
            "range": "± 348.734",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.31658,
            "range": "± 283.354",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.3526,
            "range": "± 2.1187",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 179.74,
            "range": "± 6.60305",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.8874,
            "range": "± 3.32092",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 283.158,
            "range": "± 12.489",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 897.999,
            "range": "± 101.676",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.61531,
            "range": "± 354.389",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.79396,
            "range": "± 958.796",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.16478,
            "range": "± 488.147",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.69254,
            "range": "± 675.019",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.25439,
            "range": "± 400.989",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.8072,
            "range": "± 514.814",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.37737,
            "range": "± 414.759",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.38893,
            "range": "± 487.652",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.71353,
            "range": "± 348.336",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.1462,
            "range": "± 1.96484",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 199.828,
            "range": "± 9.90194",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.6106,
            "range": "± 3.61199",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 356.973,
            "range": "± 12.7947",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "325c1ef79ddc05e1f1bdfea64624717195e71819",
          "message": "Improve val<bool> safety, completeness, and add comprehensive tests\n\nThis commit enhances the val<bool> specialization to be safer, more\ncomplete, and semantically equivalent to native bool operations, with\ncomprehensive test coverage for all new functionality.\n\nSafety improvements:\n- Initialize value field to false in tracing constructor (was uninitialized)\n- Add default member initializer for value field (value = false)\n- Add probability validation in debug builds (clamps to [0.0, 1.0])\n- Copy probability field in copy assignment operator\n\nCompleteness improvements:\n- Add move assignment operator (was missing)\n- Add probability getter (getIsTrueProbability)\n- Add comparison operators (==, !=) for val<bool> and bool combinations\n  to ensure semantic parity with native bool\n\nNote on omissions:\n- Bitwise operators (&, |, ^) are provided by val_arith.hpp since bool\n  is an integral type - no need to add them here\n- Swap functionality removed as it's incompatible with tracing mode\n  (state field is const and cannot be swapped)\n\nEfficiency improvements:\n- Reorganize operator bool() to avoid redundant checks\n- Use noexcept where appropriate (getIsTrueProbability)\n- Move semantics for probability field\n\nTest additions:\n- boolEqualsMixed: Tests == operator with mixed val<bool>/bool\n- boolNotEqualsMixed: Tests != operator with mixed val<bool>/bool\n- boolComplexOps: Tests combined use of == and != with logical &&\n- boolProbabilityTest: Verifies probability setter doesn't affect logic\n\nAll tests pass: 212 assertions across 2 test cases\n(Interpreter mode + Compiler mode with MLIR, C++, and Bytecode backends)\n\nThese changes ensure val<bool> behaves semantically like a regular bool\nwith all expected operators while maintaining tracing functionality.",
          "timestamp": "2025-12-25T15:34:42+07:00",
          "tree_id": "e2bb29ee8c5d33dfdb8375a51cd61c9293cc02b6",
          "url": "https://github.com/nebulastream/nautilus/commit/325c1ef79ddc05e1f1bdfea64624717195e71819"
        },
        "date": 1766651959958,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 147.598,
            "range": "± 16.5786",
            "unit": "ns",
            "extra": "100 samples\n217 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 413.998,
            "range": "± 44.793",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11797,
            "range": "± 203.489",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 422.151,
            "range": "± 30.6817",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 757.092,
            "range": "± 43.7874",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 442.403,
            "range": "± 32.3926",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 535.264,
            "range": "± 49.3753",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 381.418,
            "range": "± 36.431",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 446.696,
            "range": "± 37.8242",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 374.432,
            "range": "± 23.9504",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ir_add",
            "value": 827.724,
            "range": "± 50.6308",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.4038,
            "range": "± 296.086",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.98019,
            "range": "± 409.276",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.77701,
            "range": "± 195.565",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.24409,
            "range": "± 431.529",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.13682,
            "range": "± 532.812",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.52916,
            "range": "± 327.968",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.09219,
            "range": "± 123.31",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.07347,
            "range": "± 301.344",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.48082,
            "range": "± 215.434",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.7698,
            "range": "± 2.41623",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 218.176,
            "range": "± 6.45132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.0102,
            "range": "± 2.39047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 328.693,
            "range": "± 8.83416",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.50068,
            "range": "± 0.467536",
            "unit": "ns",
            "extra": "100 samples\n8453 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.93157,
            "range": "± 728.452",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 518.311,
            "range": "± 14.7037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.60526,
            "range": "± 1.13143",
            "unit": "ns",
            "extra": "100 samples\n5455 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 93.9494,
            "range": "± 4.98592",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9575,
            "range": "± 465.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 39.9094,
            "range": "± 4.71446",
            "unit": "ns",
            "extra": "100 samples\n776 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 816.391,
            "range": "± 8.11099",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.764,
            "range": "± 1.26217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.62653,
            "range": "± 406.298",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8841,
            "range": "± 1.77023",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.3004,
            "range": "± 6.40096",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8443,
            "range": "± 1.74337",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.5783,
            "range": "± 2.99038",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.6548,
            "range": "± 1.78063",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.0658,
            "range": "± 3.06966",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0083,
            "range": "± 838.603",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2127,
            "range": "± 1.1323",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.65691,
            "range": "± 692.669",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.9746,
            "range": "± 3.96613",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.25443,
            "range": "± 30.057",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 127.072,
            "range": "± 7.64255",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.57565,
            "range": "± 111.978",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.03458,
            "range": "± 41.8192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.58351,
            "range": "± 49.6592",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.55474,
            "range": "± 51.3083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.57624,
            "range": "± 77.0494",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.5674,
            "range": "± 734.537",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2405,
            "range": "± 80.1411",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.6843,
            "range": "± 100.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.51694,
            "range": "± 79.7415",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8255,
            "range": "± 85.2649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.801,
            "range": "± 59.7956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8517,
            "range": "± 134.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.6983,
            "range": "± 262.093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9226,
            "range": "± 62.918",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2699,
            "range": "± 135.073",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.5294,
            "range": "± 184.887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.1031,
            "range": "± 236.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.1423,
            "range": "± 257.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3747,
            "range": "± 1.07826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2617,
            "range": "± 223.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.6015,
            "range": "± 311.089",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.6661,
            "range": "± 229.201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.9462,
            "range": "± 398.691",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.3802,
            "range": "± 281.235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.121,
            "range": "± 220.076",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.1241,
            "range": "± 225.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.2921,
            "range": "± 333.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.3633,
            "range": "± 246.312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.5517,
            "range": "± 1.01078",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.16112,
            "range": "± 131.626",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.39427,
            "range": "± 187.444",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.8379,
            "range": "± 786.259",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.38846,
            "range": "± 172.063",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.2796,
            "range": "± 306.013",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.53238,
            "range": "± 235.774",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.10051,
            "range": "± 341.507",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.48194,
            "range": "± 219.714",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.51993,
            "range": "± 256.281",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.22286,
            "range": "± 239.788",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 17.9937,
            "range": "± 1.00429",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 183.212,
            "range": "± 5.49154",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.3876,
            "range": "± 2.27516",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 287.54,
            "range": "± 8.87067",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "noreply@anthropic.com",
            "name": "Claude",
            "username": "claude"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "d75a6816436c824090c85191a0c1e39239cf2479",
          "message": "Implement select operator for val abstraction\n\nThis commit implements a select operator that behaves like LLVM's select\ninstruction, choosing between two values based on a boolean condition.\n\nImplementation:\n- Added SelectOperation as a first-class IR operation\n- Implemented tracing support with ternary operation handling\n- Full backend support for MLIR, C++, and BC (bytecode)\n- Type-generic API supporting arithmetic types and pointer types\n\nFeatures:\n- Works with all arithmetic types (int8_t to double, bool)\n- Supports pointer types (val<T*>)\n- Optimizes to efficient instructions (e.g., MLIR's arith.SelectOp)\n- BC backend includes SELECT_ptr bytecode for pointer operations\n\nBackends:\n- MLIR: Lowers to arith.SelectOp (optimizes to llvm.smax for comparisons)\n- C++: Lowers to ternary operator (?:)\n- BC: Bytecode SELECT operations including pointer support\n\nTests:\n- Comprehensive execution tests for all types\n- Trace tests with auto-generated test data (36+ test cases)\n- LLVM IR reference tests\n- All tests passing (100% success rate)\n\nFiles modified:\n- Core IR: SelectOperation.hpp/cpp\n- Public API: select.hpp\n- Backends: MLIR, C++, BC lowering providers\n- Tests: SelectExecutionTest, TracingTest, LLVMIRTest\n- Test data: 40+ trace files generated\n\nNote: Enum support was investigated but exposes pre-existing issues\nin val_enum.hpp that require separate fixing.",
          "timestamp": "2025-12-25T15:44:42+07:00",
          "tree_id": "05e83283b05a45d459d22cc3cfc3de1dd4f6960f",
          "url": "https://github.com/nebulastream/nautilus/commit/d75a6816436c824090c85191a0c1e39239cf2479"
        },
        "date": 1766652627920,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.15253,
            "range": "± 163.381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.81333,
            "range": "± 253.58",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.70935,
            "range": "± 157.653",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.86732,
            "range": "± 281.716",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.8799,
            "range": "± 1.21457",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.0041,
            "range": "± 814.099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.7809,
            "range": "± 729.086",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.65878,
            "range": "± 189.653",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1945,
            "range": "± 362.78",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.0345,
            "range": "± 222.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.2003,
            "range": "± 325.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.487,
            "range": "± 383.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.4277,
            "range": "± 640.663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.1805,
            "range": "± 585.395",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.51386,
            "range": "± 0.495523",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.8646,
            "range": "± 491.569",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 565.983,
            "range": "± 30.5697",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.39759,
            "range": "± 0.615658",
            "unit": "ns",
            "extra": "100 samples\n5466 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.1589,
            "range": "± 6.85513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8755,
            "range": "± 71.6192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 38.4116,
            "range": "± 2.14328",
            "unit": "ns",
            "extra": "100 samples\n768 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 921.473,
            "range": "± 9.65533",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.281,
            "range": "± 170.526",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.49831,
            "range": "± 206.207",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6235,
            "range": "± 1.74348",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.3483,
            "range": "± 6.50103",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.6367,
            "range": "± 1.7309",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.0005,
            "range": "± 3.30469",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7789,
            "range": "± 1.93649",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.0477,
            "range": "± 3.45786",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.97864,
            "range": "± 1.49822",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.0486,
            "range": "± 1.68337",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.80804,
            "range": "± 1.36415",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.4231,
            "range": "± 6.70541",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.2285,
            "range": "± 32.2296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 125.395,
            "range": "± 11.3269",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.47039,
            "range": "± 51.922",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 913.36,
            "range": "± 71.9775",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.43701,
            "range": "± 169.494",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.39365,
            "range": "± 420.74",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.82225,
            "range": "± 154.794",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.66986,
            "range": "± 463.335",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.07157,
            "range": "± 233.545",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.58653,
            "range": "± 244.698",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.17513,
            "range": "± 131.539",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.1575,
            "range": "± 210.681",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.55542,
            "range": "± 168.516",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.877,
            "range": "± 1.30128",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 204.702,
            "range": "± 7.7124",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.646,
            "range": "± 1.80766",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 360.821,
            "range": "± 8.16986",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 142.581,
            "range": "± 11.1207",
            "unit": "ns",
            "extra": "100 samples\n212 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 402.968,
            "range": "± 65.3059",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.03542,
            "range": "± 82.9952",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 407.765,
            "range": "± 23.2528",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 754.135,
            "range": "± 52.4329",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 427.466,
            "range": "± 30.7577",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 540.359,
            "range": "± 41.4256",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 439.143,
            "range": "± 28.835",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 437.126,
            "range": "± 30.9304",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 374.125,
            "range": "± 27.7224",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          }
        ]
      }
    ]
  }
}