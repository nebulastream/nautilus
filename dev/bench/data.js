window.BENCHMARK_DATA = {
  "lastUpdate": 1783364219394,
  "repoUrl": "https://github.com/nebulastream/nautilus",
  "entries": {
    "Tracing Benchmark": [
      {
        "commit": {
          "author": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "42c4aab4280d93b1cbaa0e7baca9e5e7452aa5ae",
          "message": "Extend differential fuzzer to pointer arithmetic and memory access (#320)",
          "timestamp": "2026-07-01T15:14:05+02:00",
          "tree_id": "a8ff28066535e414a50b6557a1e8f540b691fef9",
          "url": "https://github.com/nebulastream/nautilus/commit/42c4aab4280d93b1cbaa0e7baca9e5e7452aa5ae"
        },
        "date": 1782912120137,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 5.57695,
            "range": "± 156.076",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.30479,
            "range": "± 431.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.20042,
            "range": "± 159.871",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.41816,
            "range": "± 238.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.7679,
            "range": "± 325.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.0961,
            "range": "± 324.582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.9057,
            "range": "± 863.036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.97331,
            "range": "± 165.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.3457,
            "range": "± 182.726",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.39868,
            "range": "± 448.551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.429,
            "range": "± 155.462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.2876,
            "range": "± 448.099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.54174,
            "range": "± 174.442",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.4377,
            "range": "± 637.651",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 56.7192,
            "range": "± 13.9779",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.21124,
            "range": "± 107.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.1297,
            "range": "± 461.641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 56.4562,
            "range": "± 13.4015",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 77.7915,
            "range": "± 14.418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.28124,
            "range": "± 160.837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.7332,
            "range": "± 662.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 79.1369,
            "range": "± 14.7973",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.4312,
            "range": "± 1.08257",
            "unit": "ns",
            "extra": "100 samples\n8395 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.2837,
            "range": "± 1.24286",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 565.557,
            "range": "± 30.6503",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.82025,
            "range": "± 0.88819",
            "unit": "ns",
            "extra": "100 samples\n6567 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.0052,
            "range": "± 8.54418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9835,
            "range": "± 179.261",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.9081,
            "range": "± 1.59818",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 747.026,
            "range": "± 11.763",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 164.254,
            "range": "± 713.999",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54238,
            "range": "± 0.437326",
            "unit": "ns",
            "extra": "100 samples\n8442 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.5504,
            "range": "± 1.95244",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.25485,
            "range": "± 21.9538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.1177,
            "range": "± 6.44419",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 48.4988,
            "range": "± 8.65085",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 743.09,
            "range": "± 12.0039",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 742.149,
            "range": "± 8.87887",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 164.147,
            "range": "± 358.527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 164.214,
            "range": "± 519.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 45.3232,
            "range": "± 3.96823",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 47.567,
            "range": "± 8.11086",
            "unit": "ns",
            "extra": "100 samples\n660 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 47.0292,
            "range": "± 6.43998",
            "unit": "ns",
            "extra": "100 samples\n655 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 744.323,
            "range": "± 12.4888",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 733.51,
            "range": "± 17.736",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 1076.43,
            "range": "± 33830.8",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 164.234,
            "range": "± 669.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 156.093,
            "range": "± 15.1745",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 170.266,
            "range": "± 2.84657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 46.8353,
            "range": "± 7.24128",
            "unit": "ns",
            "extra": "100 samples\n655 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.7172,
            "range": "± 4.70509",
            "unit": "ns",
            "extra": "100 samples\n872 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 1080.1499999999999,
            "range": "± 31093.6",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 1063.3999999999999,
            "range": "± 42464.3",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 169.724,
            "range": "± 2.91871",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 172.329,
            "range": "± 2.79764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 47.1875,
            "range": "± 7.44274",
            "unit": "ns",
            "extra": "100 samples\n656 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 50.0171,
            "range": "± 10.5194",
            "unit": "ns",
            "extra": "100 samples\n655 iterations"
          },
          {
            "name": "ssa_add",
            "value": 181.225,
            "range": "± 14.6932",
            "unit": "ns",
            "extra": "100 samples\n168 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 467.634,
            "range": "± 50.6545",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.24593,
            "range": "± 223.678",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 493.97,
            "range": "± 50.2134",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 908.379,
            "range": "± 105.268",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 523.592,
            "range": "± 67.4836",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 600.675,
            "range": "± 65.4111",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 442.156,
            "range": "± 65.8063",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 549.934,
            "range": "± 88.5684",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 447.56,
            "range": "± 25.2674",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.0028,
            "range": "± 7.97939",
            "unit": "ns",
            "extra": "100 samples\n788 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 283.786,
            "range": "± 7.83127",
            "unit": "ns",
            "extra": "100 samples\n8258 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09868,
            "range": "± 0.791051",
            "unit": "ns",
            "extra": "100 samples\n7747 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.7554,
            "range": "± 2.51725",
            "unit": "ns",
            "extra": "100 samples\n751 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 57.101,
            "range": "± 16.1823",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.54993,
            "range": "± 188.714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 776.549,
            "range": "± 82.4747",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.73986,
            "range": "± 319.72",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.41207,
            "range": "± 365.766",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.69148,
            "range": "± 207.79",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.90853,
            "range": "± 461.09",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.97158,
            "range": "± 298.061",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01664,
            "range": "± 237.641",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45313,
            "range": "± 157.072",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.99154,
            "range": "± 527.345",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.46637,
            "range": "± 134.438",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.66077,
            "range": "± 584.371",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 86.9958,
            "range": "± 5.13963",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8145,
            "range": "± 1.79277",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 163.603,
            "range": "± 8.52104",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.54852,
            "range": "± 432.884",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.47242,
            "range": "± 391.417",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.73079,
            "range": "± 1.19734",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.8157,
            "range": "± 644.003",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.2427,
            "range": "± 3.09927",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.7455,
            "range": "± 1.94249",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.62496,
            "range": "± 1.26277",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.16482,
            "range": "± 1.07924",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.107,
            "range": "± 2.96376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.90585,
            "range": "± 1.24144",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.17589,
            "range": "± 1.44202",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.02372,
            "range": "± 837.049",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.1879,
            "range": "± 2.1779",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.0591,
            "range": "± 1.75667",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.60249,
            "range": "± 1.03105",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.38503,
            "range": "± 903.738",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.5261,
            "range": "± 2.2391",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.57134,
            "range": "± 1.19221",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.83026,
            "range": "± 1.82115",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.06679,
            "range": "± 562.158",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.23,
            "range": "± 7.72257",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 42.7438,
            "range": "± 10.4997",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35227,
            "range": "± 59.2729",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.34893,
            "range": "± 53.0863",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.7849,
            "range": "± 10.6772",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.7229,
            "range": "± 9.20014",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43253,
            "range": "± 79.6885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.2558,
            "range": "± 39.2304",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3972fe310b02378b2ecf32cbc0650abac499123e",
          "message": "Bring AsmJit x64 and ARM backends to code-gen parity (#314)",
          "timestamp": "2026-07-01T17:22:39+02:00",
          "tree_id": "854daea4e00726593acf0492d35d384d03debfe2",
          "url": "https://github.com/nebulastream/nautilus/commit/3972fe310b02378b2ecf32cbc0650abac499123e"
        },
        "date": 1782919849545,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 803.271,
            "range": "± 135.889",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.60068,
            "range": "± 222.702",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.4474,
            "range": "± 607.322",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.6174,
            "range": "± 188.128",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.88059,
            "range": "± 443.912",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.83458,
            "range": "± 284.847",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.97662,
            "range": "± 223.808",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.46983,
            "range": "± 208.444",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.72355,
            "range": "± 176.917",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.50799,
            "range": "± 243.268",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.49824,
            "range": "± 995.484",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.2808,
            "range": "± 4.5051",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5443,
            "range": "± 1.37065",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 160.502,
            "range": "± 15.6902",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.7469,
            "range": "± 1.14157",
            "unit": "ns",
            "extra": "100 samples\n8450 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.1419,
            "range": "± 1.56309",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 536.445,
            "range": "± 20.7081",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.78015,
            "range": "± 0.899508",
            "unit": "ns",
            "extra": "100 samples\n6604 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.592,
            "range": "± 6.32252",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.6001,
            "range": "± 468.093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.5379,
            "range": "± 6.20484",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 743.379,
            "range": "± 9.57584",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 162.488,
            "range": "± 517.787",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.63165,
            "range": "± 0.523311",
            "unit": "ns",
            "extra": "100 samples\n8463 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.9391,
            "range": "± 2.57074",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.96483,
            "range": "± 248.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.094,
            "range": "± 6.89291",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.8983,
            "range": "± 6.09682",
            "unit": "ns",
            "extra": "100 samples\n653 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 744.429,
            "range": "± 14.3306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 741.889,
            "range": "± 8.33877",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 160.819,
            "range": "± 7.70417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 162.651,
            "range": "± 786.359",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.1985,
            "range": "± 10.6297",
            "unit": "ns",
            "extra": "100 samples\n659 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 47.5272,
            "range": "± 7.35478",
            "unit": "ns",
            "extra": "100 samples\n660 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.2755,
            "range": "± 7.49261",
            "unit": "ns",
            "extra": "100 samples\n761 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 745.955,
            "range": "± 12.5074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 724.827,
            "range": "± 14.804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 921.009,
            "range": "± 36.414",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 161.976,
            "range": "± 4.79214",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 152.744,
            "range": "± 206.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 173.28,
            "range": "± 3.65181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 42.3206,
            "range": "± 10.0577",
            "unit": "ns",
            "extra": "100 samples\n752 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.939,
            "range": "± 7.66989",
            "unit": "ns",
            "extra": "100 samples\n872 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 920.472,
            "range": "± 25.1088",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 918.706,
            "range": "± 34.7376",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 173.94,
            "range": "± 4.33849",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 174.682,
            "range": "± 4.10369",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 41.0054,
            "range": "± 7.4154",
            "unit": "ns",
            "extra": "100 samples\n766 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.6106,
            "range": "± 8.6463",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.48955,
            "range": "± 149.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.10235,
            "range": "± 221.348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.98973,
            "range": "± 106.841",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.04696,
            "range": "± 164.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.9288,
            "range": "± 469.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.9133,
            "range": "± 283.709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.2929,
            "range": "± 617.974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.95841,
            "range": "± 53.109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.4693,
            "range": "± 223.844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.64651,
            "range": "± 256.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.7069,
            "range": "± 566.185",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.193,
            "range": "± 468.875",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.42076,
            "range": "± 76.4707",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.0297,
            "range": "± 134.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.2745,
            "range": "± 266.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.8126,
            "range": "± 332.51",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.1636,
            "range": "± 914.597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.0209,
            "range": "± 492.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.5475,
            "range": "± 511.296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.887,
            "range": "± 551.78",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.6921,
            "range": "± 524.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.2525,
            "range": "± 322.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.4998,
            "range": "± 573.028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.159,
            "range": "± 378.156",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.5106,
            "range": "± 558.608",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.187,
            "range": "± 489.495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.1428,
            "range": "± 300.516",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.7621,
            "range": "± 1.32404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7262,
            "range": "± 1.80421",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.5067,
            "range": "± 2.84805",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.6296,
            "range": "± 3.4694",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.6346,
            "range": "± 2.92658",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.8724,
            "range": "± 2.85223",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.3686,
            "range": "± 2.72268",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.6706,
            "range": "± 3.02262",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.2602,
            "range": "± 2.1274",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.1229,
            "range": "± 2.57247",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.5287,
            "range": "± 2.62708",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 43.4322,
            "range": "± 6.31521",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 206.796,
            "range": "± 9.51581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 54.8429,
            "range": "± 8.60738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 317.078,
            "range": "± 13.0635",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.8787,
            "range": "± 3.33054",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.41,
            "range": "± 4.52339",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 57.9022,
            "range": "± 8.57618",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.1171,
            "range": "± 4.10944",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 56.9766,
            "range": "± 7.28485",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.5817,
            "range": "± 5.89906",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.1869,
            "range": "± 6.69223",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 30.1938,
            "range": "± 4.45344",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.6247,
            "range": "± 5.93595",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.0761,
            "range": "± 3.95641",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 104.296,
            "range": "± 11.9315",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.05323,
            "range": "± 11.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 157.563,
            "range": "± 22.1685",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.16592,
            "range": "± 47.2984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 59.0216,
            "range": "± 14.3254",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.52155,
            "range": "± 182.78",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.64149,
            "range": "± 538.419",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.69987,
            "range": "± 526.296",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.02993,
            "range": "± 1.30409",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.83341,
            "range": "± 735.854",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.2458,
            "range": "± 3.37966",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.219,
            "range": "± 1.90423",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.94167,
            "range": "± 919.14",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.1816,
            "range": "± 976.944",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.2598,
            "range": "± 2.72938",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.2044,
            "range": "± 2.23379",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.954,
            "range": "± 1.84858",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.06359,
            "range": "± 700.776",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.2661,
            "range": "± 2.31286",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.7222,
            "range": "± 1.60016",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.86099,
            "range": "± 1.3814",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.05701,
            "range": "± 1.4705",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.5092,
            "range": "± 1.34527",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.55678,
            "range": "± 1.03493",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.67508,
            "range": "± 1.66024",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.35118,
            "range": "± 862.127",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.1298,
            "range": "± 6.73355",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.5867,
            "range": "± 6.668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35141,
            "range": "± 74.3562",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.3564,
            "range": "± 68.0821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.9377,
            "range": "± 7.2467",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 52.2143,
            "range": "± 9.76684",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42502,
            "range": "± 39.9605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26605,
            "range": "± 43.9612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 208.775,
            "range": "± 43.3666",
            "unit": "ns",
            "extra": "100 samples\n162 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 465.928,
            "range": "± 36.4947",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.20231,
            "range": "± 141.64",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 520.549,
            "range": "± 74.0325",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 936.938,
            "range": "± 110.178",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 506.63,
            "range": "± 61.4998",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 615.451,
            "range": "± 67.1973",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 461.791,
            "range": "± 61.0566",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 533.173,
            "range": "± 62.7943",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 489.408,
            "range": "± 68.3893",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.7566,
            "range": "± 9.18943",
            "unit": "ns",
            "extra": "100 samples\n758 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 280.906,
            "range": "± 3.64358",
            "unit": "ns",
            "extra": "100 samples\n8282 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.15073,
            "range": "± 1.1666",
            "unit": "ns",
            "extra": "100 samples\n7749 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 41.1995,
            "range": "± 3.60415",
            "unit": "ns",
            "extra": "100 samples\n749 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 55.4195,
            "range": "± 7.8968",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.21639,
            "range": "± 114.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.1042,
            "range": "± 302.308",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 55.7888,
            "range": "± 6.98738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 76.006,
            "range": "± 8.68398",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.21222,
            "range": "± 44.8529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 24.9183,
            "range": "± 262.619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 78.6477,
            "range": "± 7.99751",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "231252de465113248762ac4d825c548f03dce44e",
          "message": "Add opt-in BC interpreter performance options: dispatch/register-file tuning + register coalescing (#323)",
          "timestamp": "2026-07-01T22:12:59+02:00",
          "tree_id": "34b4980762597f8ede13fe72569f05ae6d433e20",
          "url": "https://github.com/nebulastream/nautilus/commit/231252de465113248762ac4d825c548f03dce44e"
        },
        "date": 1782937305263,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.13268,
            "range": "± 160.584",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.06785,
            "range": "± 158.303",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 7.79719,
            "range": "± 1.30813",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.13602,
            "range": "± 506.039",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.4255,
            "range": "± 3.0607",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.2082,
            "range": "± 2.33003",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.69935,
            "range": "± 1.07607",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.24449,
            "range": "± 425.875",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.3182,
            "range": "± 2.01386",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.79375,
            "range": "± 1.07727",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.82299,
            "range": "± 1.54376",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.35525,
            "range": "± 566.807",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.9231,
            "range": "± 2.86042",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.04532,
            "range": "± 1.26201",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.05488,
            "range": "± 668.835",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.05226,
            "range": "± 583.486",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.52809,
            "range": "± 2.1139",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.45988,
            "range": "± 775.741",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.13837,
            "range": "± 1.02653",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.55713,
            "range": "± 343.198",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 35.824,
            "range": "± 5.53771",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 36.1888,
            "range": "± 5.53917",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.4274,
            "range": "± 24.2662",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.48773,
            "range": "± 52.6676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 94.3427,
            "range": "± 12.0974",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 47.1152,
            "range": "± 8.82197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.47504,
            "range": "± 528.554",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.19494,
            "range": "± 57.2529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 48.2467,
            "range": "± 21.4451",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.60815,
            "range": "± 118.725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 166.107,
            "range": "± 14.3633",
            "unit": "ns",
            "extra": "100 samples\n187 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 417.328,
            "range": "± 26.7626",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.06873,
            "range": "± 69.1763",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 437.197,
            "range": "± 50.9167",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 826.209,
            "range": "± 56.4986",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 441.431,
            "range": "± 27.3546",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 532.116,
            "range": "± 37.1131",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 389.141,
            "range": "± 32.0329",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 452.24,
            "range": "± 26.305",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 402.509,
            "range": "± 34.5106",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.21109,
            "range": "± 179.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.80494,
            "range": "± 161.071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.66403,
            "range": "± 146.061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.84276,
            "range": "± 161.262",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.5201,
            "range": "± 373.031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.5757,
            "range": "± 214.283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.5263,
            "range": "± 208.387",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.62392,
            "range": "± 158.808",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.2625,
            "range": "± 183.092",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.7969,
            "range": "± 1.58828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.1317,
            "range": "± 165.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.6571,
            "range": "± 607.14",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.4115,
            "range": "± 191.701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.3905,
            "range": "± 255.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.9833,
            "range": "± 271.586",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5603,
            "range": "± 289.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.9235,
            "range": "± 1.07391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.0425,
            "range": "± 2.65432",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.057,
            "range": "± 1.15163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3835,
            "range": "± 919.234",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.371,
            "range": "± 786.85",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.4701,
            "range": "± 660.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.7527,
            "range": "± 581.589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.4378,
            "range": "± 278.334",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.4726,
            "range": "± 283.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.1794,
            "range": "± 705.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.4488,
            "range": "± 469.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.909,
            "range": "± 606.817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.5618,
            "range": "± 2.75588",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.6652,
            "range": "± 3.86477",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.4202,
            "range": "± 4.05344",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.677,
            "range": "± 3.94922",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.1335,
            "range": "± 4.30767",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.437,
            "range": "± 6.55413",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.1825,
            "range": "± 4.00899",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.7399,
            "range": "± 3.98439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.5956,
            "range": "± 4.83605",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.0569,
            "range": "± 2.60581",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 33.4886,
            "range": "± 4.70363",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 207.703,
            "range": "± 11.3926",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 45.5914,
            "range": "± 8.90854",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 354.754,
            "range": "± 21.2274",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.6492,
            "range": "± 5.32181",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.6902,
            "range": "± 5.85923",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 50.1889,
            "range": "± 11.5175",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.7275,
            "range": "± 5.70065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.7646,
            "range": "± 11.1163",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 33.4219,
            "range": "± 7.05515",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.3327,
            "range": "± 6.30366",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 26.2513,
            "range": "± 5.32033",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.7305,
            "range": "± 6.3035",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.9918,
            "range": "± 6.80187",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 98.335,
            "range": "± 15.1149",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.06818,
            "range": "± 50.2788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 152.053,
            "range": "± 17.5893",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.21013,
            "range": "± 24.4427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.5237,
            "range": "± 0.904872",
            "unit": "ns",
            "extra": "100 samples\n8796 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.8787,
            "range": "± 1.13495",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 562.967,
            "range": "± 57.8694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.62536,
            "range": "± 0.642334",
            "unit": "ns",
            "extra": "100 samples\n7225 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 108.251,
            "range": "± 6.56975",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5529,
            "range": "± 112.877",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.2576,
            "range": "± 2.49207",
            "unit": "ns",
            "extra": "100 samples\n670 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 792.52,
            "range": "± 16.1372",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 181.904,
            "range": "± 2.28839",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.95465,
            "range": "± 1.4708",
            "unit": "ns",
            "extra": "100 samples\n8814 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 24.1558,
            "range": "± 1.8932",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.6825,
            "range": "± 25.3173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 46.5642,
            "range": "± 3.22216",
            "unit": "ns",
            "extra": "100 samples\n673 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.3897,
            "range": "± 2.88916",
            "unit": "ns",
            "extra": "100 samples\n677 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 788.026,
            "range": "± 23.8103",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 784.691,
            "range": "± 6.37058",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 181.988,
            "range": "± 2.98638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 181.429,
            "range": "± 459.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.4405,
            "range": "± 5.04588",
            "unit": "ns",
            "extra": "100 samples\n644 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 40.0857,
            "range": "± 2.10924",
            "unit": "ns",
            "extra": "100 samples\n776 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.7269,
            "range": "± 7.06745",
            "unit": "ns",
            "extra": "100 samples\n793 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 787.853,
            "range": "± 26.6271",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 519.95,
            "range": "± 18.4561",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 616.912,
            "range": "± 7.53482",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 181.533,
            "range": "± 1.12093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 105.673,
            "range": "± 1.9193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 124.663,
            "range": "± 656.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.2375,
            "range": "± 5.43979",
            "unit": "ns",
            "extra": "100 samples\n785 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.7766,
            "range": "± 3.25204",
            "unit": "ns",
            "extra": "100 samples\n867 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 618.831,
            "range": "± 9.24409",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 617.533,
            "range": "± 9.23816",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 124.844,
            "range": "± 1.3047",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 125.08,
            "range": "± 2.11523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.3576,
            "range": "± 4.03418",
            "unit": "ns",
            "extra": "100 samples\n793 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.5359,
            "range": "± 3.61093",
            "unit": "ns",
            "extra": "100 samples\n799 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 46.411,
            "range": "± 15.1512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.13886,
            "range": "± 172.589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.4272,
            "range": "± 4.16688",
            "unit": "ns",
            "extra": "100 samples\n827 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 300.924,
            "range": "± 7.34851",
            "unit": "ns",
            "extra": "100 samples\n8603 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.1973,
            "range": "± 1.08496",
            "unit": "ns",
            "extra": "100 samples\n7848 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.4147,
            "range": "± 1.7969",
            "unit": "ns",
            "extra": "100 samples\n808 iterations"
          },
          {
            "name": "ir_add",
            "value": 703.558,
            "range": "± 43.9498",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.48676,
            "range": "± 97.6498",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.25663,
            "range": "± 186.775",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.56406,
            "range": "± 87.5943",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.77346,
            "range": "± 212.542",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.7232,
            "range": "± 107.739",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.91164,
            "range": "± 110.86",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.38056,
            "range": "± 59.6706",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.65529,
            "range": "± 88.1472",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.41587,
            "range": "± 106.419",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.61903,
            "range": "± 473.412",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.6689,
            "range": "± 3.47256",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5082,
            "range": "± 671.118",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 161.663,
            "range": "± 5.69375",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "affd1e0663b617a135a387084aaf23c4db268c0a",
          "message": "Fix AsmJit merged-value corruption from block-arg identifier collision (#321) (#322)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-07-01T22:26:54+02:00",
          "tree_id": "ef3f7a1435818d8fb00370011cdf214fa663f51b",
          "url": "https://github.com/nebulastream/nautilus/commit/affd1e0663b617a135a387084aaf23c4db268c0a"
        },
        "date": 1782938112039,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 55.704,
            "range": "± 13.8894",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.50232,
            "range": "± 359.12",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.8309,
            "range": "± 1.042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 57.6197,
            "range": "± 12.0273",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 78.9619,
            "range": "± 14.8566",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.46496,
            "range": "± 344.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.5911,
            "range": "± 1.46669",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 79.7599,
            "range": "± 14.4906",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 59.9196,
            "range": "± 16.0117",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.02803,
            "range": "± 448.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 190.808,
            "range": "± 20.6819",
            "unit": "ns",
            "extra": "100 samples\n162 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 476.801,
            "range": "± 67.5257",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18351,
            "range": "± 135.507",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 514.906,
            "range": "± 60.8351",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 889.691,
            "range": "± 87.0734",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 502.446,
            "range": "± 51.0152",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 610.627,
            "range": "± 85.7419",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 429.679,
            "range": "± 54.429",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 504.979,
            "range": "± 41.9677",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 481.147,
            "range": "± 51.7829",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_add",
            "value": 812.905,
            "range": "± 79.5389",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.6645,
            "range": "± 288.203",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.48,
            "range": "± 386.302",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.67246,
            "range": "± 156.419",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.87998,
            "range": "± 252.178",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.86839,
            "range": "± 175.484",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01378,
            "range": "± 182.061",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.48359,
            "range": "± 162.502",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.81378,
            "range": "± 219.458",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.59532,
            "range": "± 232.021",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.94064,
            "range": "± 879.592",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 91.4777,
            "range": "± 5.2765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8875,
            "range": "± 1.09294",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 170.833,
            "range": "± 8.93114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.2565,
            "range": "± 1.92406",
            "unit": "ns",
            "extra": "100 samples\n8503 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.3596,
            "range": "± 2.22018",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 608.074,
            "range": "± 65.7254",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.8035,
            "range": "± 0.794324",
            "unit": "ns",
            "extra": "100 samples\n6617 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.4418,
            "range": "± 8.27461",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0474,
            "range": "± 263.42",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 50.7847,
            "range": "± 9.17373",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 751.888,
            "range": "± 45.5094",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 163.201,
            "range": "± 972.507",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.55413,
            "range": "± 0.389164",
            "unit": "ns",
            "extra": "100 samples\n8516 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.9195,
            "range": "± 3.64044",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.27497,
            "range": "± 34.6447",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 49.1736,
            "range": "± 9.52174",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 48.9552,
            "range": "± 6.941",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 745.21,
            "range": "± 8.64304",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 750.43,
            "range": "± 22.06",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 163.561,
            "range": "± 2.67178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 163.67,
            "range": "± 1.76081",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.6961,
            "range": "± 7.92824",
            "unit": "ns",
            "extra": "100 samples\n629 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 46.6311,
            "range": "± 6.16602",
            "unit": "ns",
            "extra": "100 samples\n659 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.4551,
            "range": "± 4.42524",
            "unit": "ns",
            "extra": "100 samples\n786 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 746.479,
            "range": "± 12.4638",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 734.281,
            "range": "± 13.2164",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 911.074,
            "range": "± 43.5813",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 163.239,
            "range": "± 1.48917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 155.154,
            "range": "± 2.53542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 174.445,
            "range": "± 4.80596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 43.9111,
            "range": "± 13.5604",
            "unit": "ns",
            "extra": "100 samples\n789 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 38.0112,
            "range": "± 8.96852",
            "unit": "ns",
            "extra": "100 samples\n871 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 908.811,
            "range": "± 47.1301",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 926.397,
            "range": "± 29.4056",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 173.874,
            "range": "± 4.34269",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 174.214,
            "range": "± 4.59223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.8791,
            "range": "± 6.68558",
            "unit": "ns",
            "extra": "100 samples\n788 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.9129,
            "range": "± 9.08994",
            "unit": "ns",
            "extra": "100 samples\n767 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.24017,
            "range": "± 951.282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.57564,
            "range": "± 504.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.8534,
            "range": "± 1.00652",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.80229,
            "range": "± 364.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.1294,
            "range": "± 1.0752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.4061,
            "range": "± 546.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.6972,
            "range": "± 689.968",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.64964,
            "range": "± 395.992",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.6871,
            "range": "± 371.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.6099,
            "range": "± 550.517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.6284,
            "range": "± 340.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.8435,
            "range": "± 1.70346",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.2102,
            "range": "± 350.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2083,
            "range": "± 1.09707",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.3359,
            "range": "± 454.1",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.7148,
            "range": "± 507.118",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.991,
            "range": "± 540.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.1388,
            "range": "± 433.288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.0783,
            "range": "± 508.726",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.3798,
            "range": "± 709.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.5377,
            "range": "± 571.184",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.7499,
            "range": "± 612.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6256,
            "range": "± 1.8697",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.8781,
            "range": "± 477.658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.3198,
            "range": "± 984.494",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.0921,
            "range": "± 1.57199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.441,
            "range": "± 1.07846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.1427,
            "range": "± 1.29782",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.9196,
            "range": "± 2.90761",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.1485,
            "range": "± 3.77227",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.6006,
            "range": "± 4.22307",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.1524,
            "range": "± 3.73959",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.868,
            "range": "± 3.72208",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.1777,
            "range": "± 4.13735",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.7456,
            "range": "± 3.85683",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.8401,
            "range": "± 3.49846",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.5447,
            "range": "± 5.20031",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.9318,
            "range": "± 3.39991",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 43.6564,
            "range": "± 7.46788",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 220.72,
            "range": "± 12.8812",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 55.2042,
            "range": "± 9.74332",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 325.489,
            "range": "± 23.7169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.1948,
            "range": "± 5.14858",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 37.6331,
            "range": "± 7.69237",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.2035,
            "range": "± 10.3539",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 43.7219,
            "range": "± 9.32542",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 60.4105,
            "range": "± 12.218",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 48.1011,
            "range": "± 10.5512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 51.9608,
            "range": "± 9.7242",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 32.2805,
            "range": "± 5.48441",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 46.3587,
            "range": "± 9.86646",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 44.3256,
            "range": "± 10.3578",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 107.504,
            "range": "± 14.7389",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.06585,
            "range": "± 23.2961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 160.561,
            "range": "± 17.2516",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.23458,
            "range": "± 85.3434",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 41.7892,
            "range": "± 7.61824",
            "unit": "ns",
            "extra": "100 samples\n758 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 333.187,
            "range": "± 15.9963",
            "unit": "ns",
            "extra": "100 samples\n8268 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.14412,
            "range": "± 0.768326",
            "unit": "ns",
            "extra": "100 samples\n7768 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.3736,
            "range": "± 2.94208",
            "unit": "ns",
            "extra": "100 samples\n755 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.43142,
            "range": "± 300.726",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.39766,
            "range": "± 300.309",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.39502,
            "range": "± 1.7218",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.86637,
            "range": "± 862.887",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.6697,
            "range": "± 3.30114",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.0329,
            "range": "± 2.47775",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.85123,
            "range": "± 1.31042",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.87675,
            "range": "± 641.002",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.25,
            "range": "± 2.91645",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.11758,
            "range": "± 1.38235",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.28069,
            "range": "± 1.5596",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.21595,
            "range": "± 700.15",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.6547,
            "range": "± 3.13475",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.3599,
            "range": "± 2.27921",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.6117,
            "range": "± 1.17018",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.66318,
            "range": "± 921.137",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.9866,
            "range": "± 2.0513",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.49034,
            "range": "± 952.78",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.96309,
            "range": "± 1.12154",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.06335,
            "range": "± 532.462",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.3655,
            "range": "± 8.93305",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.0858,
            "range": "± 7.49784",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.3555,
            "range": "± 49.5102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.35987,
            "range": "± 77.1836",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 102.475,
            "range": "± 15.892",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.4552,
            "range": "± 10.1562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43145,
            "range": "± 60.3145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.28148,
            "range": "± 61.2958",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "6c74bdd560463b51ca5b5f0faf9deaf42bfccbf7",
          "message": "Extend differential fuzzer (logical ops, invoke(), static loops) and fix 4 miscompiles it found (#324)",
          "timestamp": "2026-07-02T08:16:53+02:00",
          "tree_id": "739eacd2acdc0277d794eda7ee5455f67a82ca7c",
          "url": "https://github.com/nebulastream/nautilus/commit/6c74bdd560463b51ca5b5f0faf9deaf42bfccbf7"
        },
        "date": 1782973527156,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 744.696,
            "range": "± 83.2561",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.55388,
            "range": "± 197.9",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.2191,
            "range": "± 351.578",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.6576,
            "range": "± 184.421",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.80686,
            "range": "± 382.268",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.73828,
            "range": "± 183.021",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.9232,
            "range": "± 131.805",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.41161,
            "range": "± 176.979",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.67248,
            "range": "± 126.627",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.44612,
            "range": "± 126.077",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.19907,
            "range": "± 654.406",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 91.5124,
            "range": "± 4.55428",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.1633,
            "range": "± 966.101",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 162.482,
            "range": "± 13.3904",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 56.1772,
            "range": "± 11.9984",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.25992,
            "range": "± 116.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.0077,
            "range": "± 1.33817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 56.9789,
            "range": "± 6.72277",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 76.5275,
            "range": "± 8.45625",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.28251,
            "range": "± 59.1738",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.2191,
            "range": "± 427.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 82.0573,
            "range": "± 14.148",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.5388,
            "range": "± 5.65237",
            "unit": "ns",
            "extra": "100 samples\n728 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 291.15,
            "range": "± 47.7249",
            "unit": "ns",
            "extra": "100 samples\n8349 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.05833,
            "range": "± 0.905439",
            "unit": "ns",
            "extra": "100 samples\n7823 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.3281,
            "range": "± 2.77241",
            "unit": "ns",
            "extra": "100 samples\n753 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.38473,
            "range": "± 157.247",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.4101,
            "range": "± 243.19",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.96754,
            "range": "± 944.448",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.76932,
            "range": "± 419.407",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.1694,
            "range": "± 2.00004",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.061,
            "range": "± 1.68505",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.71885,
            "range": "± 1.21701",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.89908,
            "range": "± 580.603",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.3302,
            "range": "± 3.2107",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.5641,
            "range": "± 635.901",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.00653,
            "range": "± 1.28289",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.09249,
            "range": "± 584.062",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.6378,
            "range": "± 2.25252",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.5359,
            "range": "± 994.279",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.49793,
            "range": "± 653.78",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.96964,
            "range": "± 1.20501",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.2472,
            "range": "± 933.451",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.42676,
            "range": "± 679.445",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.19936,
            "range": "± 893.578",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.15433,
            "range": "± 630.443",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.384,
            "range": "± 6.98974",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.7597,
            "range": "± 7.12794",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.3751,
            "range": "± 41.9248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39828,
            "range": "± 57.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.0448,
            "range": "± 5.86796",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.1867,
            "range": "± 5.64979",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.44657,
            "range": "± 42.9799",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.34593,
            "range": "± 158.771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.9047,
            "range": "± 1.26515",
            "unit": "ns",
            "extra": "100 samples\n8545 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.9426,
            "range": "± 1.76187",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 520.373,
            "range": "± 19.9365",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.56425,
            "range": "± 0.226846",
            "unit": "ns",
            "extra": "100 samples\n6635 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.2334,
            "range": "± 11.2839",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9322,
            "range": "± 166.115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.967,
            "range": "± 7.56491",
            "unit": "ns",
            "extra": "100 samples\n592 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 924.795,
            "range": "± 14.956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 198.962,
            "range": "± 3.71393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56696,
            "range": "± 0.514517",
            "unit": "ns",
            "extra": "100 samples\n8551 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.7425,
            "range": "± 3.00602",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.27205,
            "range": "± 23.0653",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.8882,
            "range": "± 4.27281",
            "unit": "ns",
            "extra": "100 samples\n594 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 48.8551,
            "range": "± 8.60021",
            "unit": "ns",
            "extra": "100 samples\n571 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 926.576,
            "range": "± 13.7443",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 924.522,
            "range": "± 14.6754",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 199.003,
            "range": "± 274.311",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 199.114,
            "range": "± 1.0706",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 49.3761,
            "range": "± 8.53998",
            "unit": "ns",
            "extra": "100 samples\n605 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 42.2959,
            "range": "± 8.44114",
            "unit": "ns",
            "extra": "100 samples\n681 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.2007,
            "range": "± 6.99316",
            "unit": "ns",
            "extra": "100 samples\n675 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 922.376,
            "range": "± 11.2026",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 633,
            "range": "± 53.5576",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 591.815,
            "range": "± 11.3512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 198.971,
            "range": "± 1.94887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 132.775,
            "range": "± 9.91959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 117.195,
            "range": "± 1.5857",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.2649,
            "range": "± 5.61582",
            "unit": "ns",
            "extra": "100 samples\n706 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.6605,
            "range": "± 4.60537",
            "unit": "ns",
            "extra": "100 samples\n849 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 591.488,
            "range": "± 11.0119",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 590.952,
            "range": "± 9.13531",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 116.971,
            "range": "± 347.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 117.011,
            "range": "± 229.458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.4447,
            "range": "± 5.49487",
            "unit": "ns",
            "extra": "100 samples\n748 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.0634,
            "range": "± 9.76325",
            "unit": "ns",
            "extra": "100 samples\n691 iterations"
          },
          {
            "name": "ssa_add",
            "value": 179.108,
            "range": "± 13.2184",
            "unit": "ns",
            "extra": "100 samples\n168 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 476.552,
            "range": "± 47.8634",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.21395,
            "range": "± 124.581",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 488.58,
            "range": "± 28.7216",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 899.553,
            "range": "± 77.9831",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 508.951,
            "range": "± 57.2481",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 606.508,
            "range": "± 46.1794",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 478.743,
            "range": "± 35.2664",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 523.241,
            "range": "± 49.7108",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 448.953,
            "range": "± 36.2824",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.58098,
            "range": "± 140.772",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.17403,
            "range": "± 165.277",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.15352,
            "range": "± 384.566",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.18611,
            "range": "± 140.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.0371,
            "range": "± 343.246",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.8936,
            "range": "± 186.996",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.1725,
            "range": "± 188.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.02791,
            "range": "± 121.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.5576,
            "range": "± 243.6",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.52827,
            "range": "± 190.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.5914,
            "range": "± 498.803",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.1311,
            "range": "± 1.92638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.57457,
            "range": "± 145.028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.25,
            "range": "± 211.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.9234,
            "range": "± 288.648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.3075,
            "range": "± 7.25742",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.5088,
            "range": "± 280.97",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.5109,
            "range": "± 312.273",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.7964,
            "range": "± 1.17316",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.0387,
            "range": "± 356.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.9153,
            "range": "± 261.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.2084,
            "range": "± 332.72",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.6315,
            "range": "± 371.924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3488,
            "range": "± 273.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.558,
            "range": "± 1.1237",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.4266,
            "range": "± 535.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.5029,
            "range": "± 569.853",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.4413,
            "range": "± 1.02717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.0207,
            "range": "± 2.62573",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.6125,
            "range": "± 4.92499",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.4078,
            "range": "± 3.33751",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.6429,
            "range": "± 3.21778",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.9339,
            "range": "± 2.92225",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.9054,
            "range": "± 2.25806",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.9124,
            "range": "± 2.74667",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.6526,
            "range": "± 2.95183",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.2422,
            "range": "± 2.76278",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.1211,
            "range": "± 3.93835",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 43.9669,
            "range": "± 7.2899",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 206.9,
            "range": "± 11.169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 54.0745,
            "range": "± 7.86053",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 315.611,
            "range": "± 19.7904",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 24.0611,
            "range": "± 5.24477",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 35.9463,
            "range": "± 5.26635",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.6604,
            "range": "± 9.76765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 44.0141,
            "range": "± 9.21832",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.6651,
            "range": "± 10.771",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.5263,
            "range": "± 8.57182",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.0187,
            "range": "± 8.04114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 32.3525,
            "range": "± 5.51887",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 45.3988,
            "range": "± 8.25197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 43.0153,
            "range": "± 8.61768",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 107.918,
            "range": "± 13.3898",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.05939,
            "range": "± 19.4909",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 158.456,
            "range": "± 16.2429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.19764,
            "range": "± 43.1762",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 59.0585,
            "range": "± 13.9342",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.52431,
            "range": "± 119.755",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "cb898a13ab9e612ea8ac61437d71ad6f2b6638dd",
          "message": "AsmJit x86-64 backend: branch fusion, immediate folding, and lowering optimizations (#326)",
          "timestamp": "2026-07-02T14:29:53+02:00",
          "tree_id": "9f3812449c3f7a05404018a446702a5f738d8fe2",
          "url": "https://github.com/nebulastream/nautilus/commit/cb898a13ab9e612ea8ac61437d71ad6f2b6638dd"
        },
        "date": 1782995828974,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 52.9647,
            "range": "± 12.1316",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.53719,
            "range": "± 164.66",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.33404,
            "range": "± 218.889",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.4355,
            "range": "± 324.719",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.95608,
            "range": "± 1.51874",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.69152,
            "range": "± 515.667",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.859,
            "range": "± 3.02485",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.882,
            "range": "± 1.66276",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.60148,
            "range": "± 1.24424",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.82554,
            "range": "± 727.922",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.2378,
            "range": "± 2.93414",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.48642,
            "range": "± 1.10782",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.87117,
            "range": "± 1.37723",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.96222,
            "range": "± 652.686",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.3252,
            "range": "± 2.48411",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.1392,
            "range": "± 3.47726",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.43306,
            "range": "± 940.031",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.35809,
            "range": "± 666.366",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1244,
            "range": "± 1.63985",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.18977,
            "range": "± 762.652",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.97124,
            "range": "± 1.10493",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.95542,
            "range": "± 438.359",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.3432,
            "range": "± 7.96247",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.3182,
            "range": "± 6.51534",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.38335,
            "range": "± 46.9228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39082,
            "range": "± 53.8626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.2881,
            "range": "± 9.66337",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 53.5301,
            "range": "± 10.3668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.49899,
            "range": "± 152.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.4582,
            "range": "± 569.753",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.5855,
            "range": "± 0.953475",
            "unit": "ns",
            "extra": "100 samples\n8465 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.5404,
            "range": "± 1.93146",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 295.068,
            "range": "± 21.758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.74787,
            "range": "± 0.760487",
            "unit": "ns",
            "extra": "100 samples\n6589 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.2802,
            "range": "± 8.74072",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.4901,
            "range": "± 215.629",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.2237,
            "range": "± 5.17036",
            "unit": "ns",
            "extra": "100 samples\n648 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 777.397,
            "range": "± 36.8212",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 164.464,
            "range": "± 4.02166",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.51309,
            "range": "± 0.297009",
            "unit": "ns",
            "extra": "100 samples\n8473 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.2439,
            "range": "± 1.19758",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.48656,
            "range": "± 28.2616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.7352,
            "range": "± 8.34456",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.3543,
            "range": "± 7.09728",
            "unit": "ns",
            "extra": "100 samples\n645 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 771.438,
            "range": "± 13.8188",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 771.768,
            "range": "± 12.8935",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 164.542,
            "range": "± 658.353",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 164.44,
            "range": "± 220.831",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.7317,
            "range": "± 10.5303",
            "unit": "ns",
            "extra": "100 samples\n644 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 42.0844,
            "range": "± 4.02994",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 40.9296,
            "range": "± 5.69839",
            "unit": "ns",
            "extra": "100 samples\n736 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 769.67,
            "range": "± 10.775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 728.645,
            "range": "± 11.6225",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 587.435,
            "range": "± 11.8289",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 164.545,
            "range": "± 1.52674",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 145.789,
            "range": "± 4.42122",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 114.243,
            "range": "± 1.44035",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 41.545,
            "range": "± 8.8702",
            "unit": "ns",
            "extra": "100 samples\n756 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.868,
            "range": "± 7.1515",
            "unit": "ns",
            "extra": "100 samples\n880 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 588.461,
            "range": "± 12.694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 587.419,
            "range": "± 10.9672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 114.105,
            "range": "± 372.238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 114.426,
            "range": "± 2.09995",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.9211,
            "range": "± 5.99557",
            "unit": "ns",
            "extra": "100 samples\n749 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.3081,
            "range": "± 7.16145",
            "unit": "ns",
            "extra": "100 samples\n748 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.51322,
            "range": "± 137.039",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.11981,
            "range": "± 166.073",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.97276,
            "range": "± 110.62",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.05796,
            "range": "± 123.887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.476,
            "range": "± 185.828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.7607,
            "range": "± 303.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.5683,
            "range": "± 322.485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.9489,
            "range": "± 141.211",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.399,
            "range": "± 289.46",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.33289,
            "range": "± 230.527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.4584,
            "range": "± 183.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.1642,
            "range": "± 368.32",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.42796,
            "range": "± 132.07",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.2698,
            "range": "± 437.573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.7317,
            "range": "± 330.274",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.2855,
            "range": "± 462.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.5082,
            "range": "± 454.717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.5906,
            "range": "± 395.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.1494,
            "range": "± 317.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.7778,
            "range": "± 1.03301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.7777,
            "range": "± 628.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.9488,
            "range": "± 250.685",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.3659,
            "range": "± 273.643",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.1518,
            "range": "± 244.127",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.2179,
            "range": "± 388.872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.6878,
            "range": "± 389.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.941,
            "range": "± 533.529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.2734,
            "range": "± 1.18102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.7535,
            "range": "± 3.37242",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.1079,
            "range": "± 3.32842",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.5764,
            "range": "± 3.3814",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.4333,
            "range": "± 3.43422",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.5041,
            "range": "± 4.01868",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.0951,
            "range": "± 4.32468",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.8574,
            "range": "± 3.08221",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.6563,
            "range": "± 2.87268",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.894,
            "range": "± 4.05644",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.5686,
            "range": "± 3.73259",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 42.7474,
            "range": "± 5.92132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 212.599,
            "range": "± 19.8229",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 54.2397,
            "range": "± 7.97483",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 319.075,
            "range": "± 17.2549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.5688,
            "range": "± 4.32557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.1182,
            "range": "± 4.87756",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 51.2364,
            "range": "± 9.12864",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.696,
            "range": "± 5.01953",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.4143,
            "range": "± 8.14915",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 42.4362,
            "range": "± 7.4833",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.1692,
            "range": "± 10.6222",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 30.3551,
            "range": "± 5.28906",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 35.3626,
            "range": "± 6.31122",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.9653,
            "range": "± 5.77915",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 79.6188,
            "range": "± 10.8271",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.513275,
            "range": "± 0.0151481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 98.1581,
            "range": "± 11.3881",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 0.6768970000000001,
            "range": "± 0.0179348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.3863,
            "range": "± 5.30012",
            "unit": "ns",
            "extra": "100 samples\n789 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 290.043,
            "range": "± 10.3708",
            "unit": "ns",
            "extra": "100 samples\n8282 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.93511,
            "range": "± 0.35316",
            "unit": "ns",
            "extra": "100 samples\n7755 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.666,
            "range": "± 1.99232",
            "unit": "ns",
            "extra": "100 samples\n740 iterations"
          },
          {
            "name": "ir_add",
            "value": 775.453,
            "range": "± 121.649",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.56374,
            "range": "± 134.554",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.32928,
            "range": "± 283.704",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.61341,
            "range": "± 117.434",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.85752,
            "range": "± 309.308",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.79794,
            "range": "± 161.033",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.96979,
            "range": "± 201.866",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.48022,
            "range": "± 204.856",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.76458,
            "range": "± 243.242",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.50566,
            "range": "± 161.017",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.68077,
            "range": "± 598.705",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.1212,
            "range": "± 5.47887",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.7042,
            "range": "± 904.101",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 166.538,
            "range": "± 9.1964",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 57.7109,
            "range": "± 12.7696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.21216,
            "range": "± 103.041",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.1173,
            "range": "± 439.753",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 62.8394,
            "range": "± 17.3299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 78.902,
            "range": "± 13.3996",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.27144,
            "range": "± 139.881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.8589,
            "range": "± 557.081",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 78.8008,
            "range": "± 15.5655",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 179.522,
            "range": "± 9.75461",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 445.621,
            "range": "± 20.9163",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.15259,
            "range": "± 90.668",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 495.238,
            "range": "± 45.2056",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 887.103,
            "range": "± 72.574",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 490.247,
            "range": "± 37.1486",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 584.454,
            "range": "± 38.1092",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 409.468,
            "range": "± 38.8823",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 498.642,
            "range": "± 34.5062",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 462.023,
            "range": "± 43.4414",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a0a0e6b5ee675abed3e78eb08ced70bdf5bd9784",
          "message": "Rewire all consumer input edges in constant folding pass (#327) (#330)",
          "timestamp": "2026-07-02T15:52:34+02:00",
          "tree_id": "ebf13c3f8e2a67bf344a784f9c8922ca238b205e",
          "url": "https://github.com/nebulastream/nautilus/commit/a0a0e6b5ee675abed3e78eb08ced70bdf5bd9784"
        },
        "date": 1783000905750,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 674.431,
            "range": "± 41.3019",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.43237,
            "range": "± 158.178",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.17578,
            "range": "± 356.922",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.49072,
            "range": "± 98.5579",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.60092,
            "range": "± 170.015",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.61745,
            "range": "± 99.5926",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.81266,
            "range": "± 140.623",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.3338,
            "range": "± 103.051",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.58644,
            "range": "± 109.918",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.3333,
            "range": "± 75.5134",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.21,
            "range": "± 799.003",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 95.4698,
            "range": "± 6.96273",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.0872,
            "range": "± 841.105",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 162.824,
            "range": "± 5.71214",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.06335,
            "range": "± 203.374",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.05609,
            "range": "± 180.795",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 7.83169,
            "range": "± 1.09609",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.12849,
            "range": "± 591.802",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.4607,
            "range": "± 4.15103",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.6132,
            "range": "± 1.64759",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.75117,
            "range": "± 1.16174",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.22385,
            "range": "± 469.674",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.2733,
            "range": "± 2.49987",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.58677,
            "range": "± 935.536",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.94234,
            "range": "± 1.60297",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.2919,
            "range": "± 506.717",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.5228,
            "range": "± 2.38811",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.00769,
            "range": "± 1.37345",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.06911,
            "range": "± 794.943",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.04592,
            "range": "± 815.812",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.07476,
            "range": "± 1.6373",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.46495,
            "range": "± 669.162",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.16978,
            "range": "± 1.19108",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.57032,
            "range": "± 538.665",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 36.3397,
            "range": "± 7.90698",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 36.3656,
            "range": "± 7.17563",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.44472,
            "range": "± 61.9453",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.44748,
            "range": "± 31.1476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 93.4252,
            "range": "± 11.156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 46.4592,
            "range": "± 9.1177",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.41645,
            "range": "± 48.2403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.18009,
            "range": "± 42.3436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.5715,
            "range": "± 3.79633",
            "unit": "ns",
            "extra": "100 samples\n809 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 303.181,
            "range": "± 10.1078",
            "unit": "ns",
            "extra": "100 samples\n8590 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.16987,
            "range": "± 0.448893",
            "unit": "ns",
            "extra": "100 samples\n7854 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.8115,
            "range": "± 1.90709",
            "unit": "ns",
            "extra": "100 samples\n779 iterations"
          },
          {
            "name": "ssa_add",
            "value": 167.227,
            "range": "± 9.59631",
            "unit": "ns",
            "extra": "100 samples\n186 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 417.049,
            "range": "± 44.0559",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07563,
            "range": "± 68.3381",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 435.376,
            "range": "± 26.4909",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 831.085,
            "range": "± 82.3497",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 441.606,
            "range": "± 26.9246",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.962,
            "range": "± 88.8349",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 430.891,
            "range": "± 22.6366",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 455.466,
            "range": "± 26.4769",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 406.902,
            "range": "± 24.1124",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.29178,
            "range": "± 240.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.97643,
            "range": "± 224.373",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.7771,
            "range": "± 171.36",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.09942,
            "range": "± 282.074",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.1752,
            "range": "± 622.722",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.8918,
            "range": "± 334.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.2646,
            "range": "± 609.104",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.6268,
            "range": "± 171.986",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.7083,
            "range": "± 329.326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.2013,
            "range": "± 306.137",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.2939,
            "range": "± 326.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.1569,
            "range": "± 471.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.6982,
            "range": "± 707.849",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.6757,
            "range": "± 1.00513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.9675,
            "range": "± 531.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.5212,
            "range": "± 320.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.3172,
            "range": "± 717.651",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4656,
            "range": "± 598.304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.384,
            "range": "± 472.517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.1133,
            "range": "± 710.453",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.9707,
            "range": "± 421.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.9749,
            "range": "± 361.877",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.8016,
            "range": "± 287.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.5676,
            "range": "± 486.071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 32.6525,
            "range": "± 804.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 66.4003,
            "range": "± 2.48461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 35.2072,
            "range": "± 715.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 96.9136,
            "range": "± 961.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.6587,
            "range": "± 2.36962",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.1619,
            "range": "± 3.75069",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.7898,
            "range": "± 4.42561",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.9057,
            "range": "± 3.62894",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.2022,
            "range": "± 4.26099",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.3412,
            "range": "± 3.72778",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.9678,
            "range": "± 3.76557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.9805,
            "range": "± 5.20039",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.3695,
            "range": "± 3.90126",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.6172,
            "range": "± 4.47633",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 33.4335,
            "range": "± 5.99439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 202,
            "range": "± 10.4799",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 45.1829,
            "range": "± 8.4264",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 316.811,
            "range": "± 12.441",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 19.2349,
            "range": "± 5.81529",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.4991,
            "range": "± 6.61837",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 36.4478,
            "range": "± 6.42368",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 27.0097,
            "range": "± 6.34547",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 34.3295,
            "range": "± 6.28497",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 29.5329,
            "range": "± 6.39178",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 32.0117,
            "range": "± 6.85205",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.2614,
            "range": "± 5.36079",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 27.8502,
            "range": "± 6.02907",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 27.1379,
            "range": "± 6.60413",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 69.6254,
            "range": "± 12.1345",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.49985199999999996,
            "range": "± 0.0196346",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 110.906,
            "range": "± 36.3829",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 0.662042,
            "range": "± 0.017179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 48.922,
            "range": "± 19.5169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.49213,
            "range": "± 494.704",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.2622,
            "range": "± 1.10987",
            "unit": "ns",
            "extra": "100 samples\n8729 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.131,
            "range": "± 1.70741",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 573.9,
            "range": "± 60.7211",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.64298,
            "range": "± 0.708093",
            "unit": "ns",
            "extra": "100 samples\n7213 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.491,
            "range": "± 9.31276",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.6091,
            "range": "± 386.743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.8416,
            "range": "± 2.82979",
            "unit": "ns",
            "extra": "100 samples\n661 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 789.016,
            "range": "± 10.3725",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 182.999,
            "range": "± 7.54612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.52699,
            "range": "± 0.253475",
            "unit": "ns",
            "extra": "100 samples\n8803 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 18.5668,
            "range": "± 964.299",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.36922,
            "range": "± 49.3949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.7677,
            "range": "± 5.59206",
            "unit": "ns",
            "extra": "100 samples\n659 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.8424,
            "range": "± 4.85846",
            "unit": "ns",
            "extra": "100 samples\n666 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 786.791,
            "range": "± 6.8651",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 787.267,
            "range": "± 8.4238",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 181.611,
            "range": "± 865.452",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 181.43,
            "range": "± 287.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.0067,
            "range": "± 5.15846",
            "unit": "ns",
            "extra": "100 samples\n653 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 42.9075,
            "range": "± 8.23364",
            "unit": "ns",
            "extra": "100 samples\n760 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.2797,
            "range": "± 4.89428",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 788.637,
            "range": "± 8.91147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 518.796,
            "range": "± 17.557",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 619.935,
            "range": "± 9.30438",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 181.922,
            "range": "± 3.34516",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 104.826,
            "range": "± 1.33482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 131.943,
            "range": "± 5.03338",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.3185,
            "range": "± 2.56646",
            "unit": "ns",
            "extra": "100 samples\n783 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.9953,
            "range": "± 2.51274",
            "unit": "ns",
            "extra": "100 samples\n886 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 624.67,
            "range": "± 11.9341",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 620.475,
            "range": "± 8.95296",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 131.012,
            "range": "± 361.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 124.706,
            "range": "± 248.082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 41.4455,
            "range": "± 4.41555",
            "unit": "ns",
            "extra": "100 samples\n769 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.4608,
            "range": "± 5.92676",
            "unit": "ns",
            "extra": "100 samples\n769 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.0703,
            "range": "± 16.3239",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.57139,
            "range": "± 129.392",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.7547,
            "range": "± 353.969",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.9119,
            "range": "± 16.849",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.4585,
            "range": "± 19.6594",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.62252,
            "range": "± 150.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.718,
            "range": "± 377.066",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.7637,
            "range": "± 18.306",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "82706158a1c3414e31301e4f84c62159a39bcade",
          "message": "Fix float→uint64 cast overflow in AsmJit x64 backend (#329)",
          "timestamp": "2026-07-02T19:33:13+02:00",
          "tree_id": "bee5503707e81b45cacdf6c89b63d0393704c007",
          "url": "https://github.com/nebulastream/nautilus/commit/82706158a1c3414e31301e4f84c62159a39bcade"
        },
        "date": 1783014022629,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 548.478,
            "range": "± 23.56",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.12256,
            "range": "± 51.103",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 2.44191,
            "range": "± 190.52",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.21482,
            "range": "± 63.5192",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.05561,
            "range": "± 135.618",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.28121,
            "range": "± 60.9627",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.44551,
            "range": "± 76.9319",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.06226,
            "range": "± 55.8378",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.2798,
            "range": "± 68.0741",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.09077,
            "range": "± 60.1563",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 5.48195,
            "range": "± 361.348",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 73.0511,
            "range": "± 2.8306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 8.55887,
            "range": "± 664.325",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 127.233,
            "range": "± 3.837",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.61547,
            "range": "± 163.53",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.60885,
            "range": "± 183.528",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 6.10636,
            "range": "± 1.05447",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.26477,
            "range": "± 417.237",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 18.4376,
            "range": "± 3.05574",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 9.18624,
            "range": "± 1.43083",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 6.19806,
            "range": "± 1.35704",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.30031,
            "range": "± 420.683",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 11.7194,
            "range": "± 1.89371",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 6.01374,
            "range": "± 997.842",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 6.15448,
            "range": "± 1.1163",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.36442,
            "range": "± 477.57",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 9.84545,
            "range": "± 2.14929",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 6.97298,
            "range": "± 1.10317",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 5.50512,
            "range": "± 645.833",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 5.51088,
            "range": "± 724.132",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 7.00564,
            "range": "± 1.36633",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.27665,
            "range": "± 557.321",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 5.51249,
            "range": "± 867.59",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 2.81211,
            "range": "± 353.681",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 28.5861,
            "range": "± 7.04317",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 28.6539,
            "range": "± 6.09248",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.12067,
            "range": "± 48.3533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.12948,
            "range": "± 25.5869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 72.7358,
            "range": "± 10.4422",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 37.0885,
            "range": "± 7.63533",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.5864,
            "range": "± 66.5455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 1.71344,
            "range": "± 27.4331",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 37.7587,
            "range": "± 15.5521",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.81829,
            "range": "± 262.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 37.3682,
            "range": "± 16.393",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.04544,
            "range": "± 170.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 29.4771,
            "range": "± 25.8463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 37.2754,
            "range": "± 15.0136",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 50.0592,
            "range": "± 18.2235",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.22256,
            "range": "± 259.644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 24.8643,
            "range": "± 2.97817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 50.5601,
            "range": "± 17.4165",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.26102,
            "range": "± 506.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.66542,
            "range": "± 301.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.36977,
            "range": "± 246.481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.97691,
            "range": "± 1.63912",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 27.867,
            "range": "± 1.86571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.1898,
            "range": "± 1.10148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 25.2291,
            "range": "± 1.30989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.69898,
            "range": "± 235.225",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.2039,
            "range": "± 576.316",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 8.37759,
            "range": "± 288.664",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 9.45891,
            "range": "± 522.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 21.9336,
            "range": "± 1.60806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.8552,
            "range": "± 644.224",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 49.6179,
            "range": "± 385.659",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 40.0773,
            "range": "± 64.4826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 30.2796,
            "range": "± 25.8616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 35.8108,
            "range": "± 33.5743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.6097,
            "range": "± 19.1346",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 34.1709,
            "range": "± 33.186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 43.6513,
            "range": "± 52.5143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 44.6789,
            "range": "± 74.4326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.222,
            "range": "± 22.7136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.2176,
            "range": "± 17.7224",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 31.2173,
            "range": "± 22.2885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 35.3414,
            "range": "± 28.8748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.6192,
            "range": "± 53.7057",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 43.6062,
            "range": "± 42.0527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 82.1329,
            "range": "± 28.2932",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.2674,
            "range": "± 2.18152",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.0497,
            "range": "± 3.29163",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 15.8877,
            "range": "± 3.55252",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 13.1884,
            "range": "± 3.67743",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 14.9253,
            "range": "± 3.39637",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.2439,
            "range": "± 3.27513",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 14.0972,
            "range": "± 4.30404",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 13.1455,
            "range": "± 3.53866",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.4018,
            "range": "± 3.25885",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 12.6328,
            "range": "± 2.48613",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 26.5538,
            "range": "± 6.58817",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 161.07,
            "range": "± 10.174",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 37.2075,
            "range": "± 8.34939",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 251.569,
            "range": "± 9.24923",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 14.919,
            "range": "± 5.53046",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 19.8054,
            "range": "± 4.87589",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 27.0947,
            "range": "± 5.38542",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 20.258,
            "range": "± 5.07652",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 25.9969,
            "range": "± 5.29119",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 22.6854,
            "range": "± 5.50532",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 23.5392,
            "range": "± 5.19493",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 18.6887,
            "range": "± 4.95993",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 21.4705,
            "range": "± 5.2084",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 20.4604,
            "range": "± 5.48928",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 53.1283,
            "range": "± 10.8786",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.38691800000000004,
            "range": "± 0.0195764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 68.906,
            "range": "± 9.99695",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 0.520973,
            "range": "± 0.0146664",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 131.295,
            "range": "± 5.76071",
            "unit": "ns",
            "extra": "100 samples\n186 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 332.583,
            "range": "± 23.0924",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.837495,
            "range": "± 0.0500632",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 339.64,
            "range": "± 17.3694",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 641.718,
            "range": "± 36.3339",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 346.444,
            "range": "± 16.9643",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 423.407,
            "range": "± 38.789",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 334.28,
            "range": "± 16.0984",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 355.222,
            "range": "± 18.0023",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 315.175,
            "range": "± 17.8376",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 30.0329,
            "range": "± 3.67321",
            "unit": "ns",
            "extra": "100 samples\n814 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 238.298,
            "range": "± 6.11868",
            "unit": "ns",
            "extra": "100 samples\n8698 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.19416,
            "range": "± 0.0813506",
            "unit": "ns",
            "extra": "100 samples\n7890 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 31.6705,
            "range": "± 1.24281",
            "unit": "ns",
            "extra": "100 samples\n758 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.63273,
            "range": "± 0.761652",
            "unit": "ns",
            "extra": "100 samples\n8743 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 12.328,
            "range": "± 1.17229",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 510.46,
            "range": "± 67.4515",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.5291,
            "range": "± 0.422384",
            "unit": "ns",
            "extra": "100 samples\n7233 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 83.9742,
            "range": "± 4.99147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 18.2408,
            "range": "± 52.6406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 37.7435,
            "range": "± 4.80744",
            "unit": "ns",
            "extra": "100 samples\n653 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 610.003,
            "range": "± 7.31074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 141.848,
            "range": "± 5.38787",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.77182,
            "range": "± 0.336155",
            "unit": "ns",
            "extra": "100 samples\n8815 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.8824,
            "range": "± 1.74733",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.07119,
            "range": "± 188.512",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 37.899,
            "range": "± 4.15477",
            "unit": "ns",
            "extra": "100 samples\n649 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 38.2904,
            "range": "± 4.7846",
            "unit": "ns",
            "extra": "100 samples\n651 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 610.093,
            "range": "± 7.70608",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 611.153,
            "range": "± 10.2672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 141.071,
            "range": "± 1.59562",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 140.737,
            "range": "± 272.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 39.041,
            "range": "± 8.68064",
            "unit": "ns",
            "extra": "100 samples\n624 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 32.3416,
            "range": "± 2.9855",
            "unit": "ns",
            "extra": "100 samples\n745 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 32.3643,
            "range": "± 2.645",
            "unit": "ns",
            "extra": "100 samples\n754 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 610.347,
            "range": "± 8.77245",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 401.912,
            "range": "± 10.0014",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 478.969,
            "range": "± 6.97928",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 140.871,
            "range": "± 938.805",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 81.3946,
            "range": "± 1.89758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 96.9988,
            "range": "± 1.11249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 32.498,
            "range": "± 2.76461",
            "unit": "ns",
            "extra": "100 samples\n761 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 28.1127,
            "range": "± 3.45577",
            "unit": "ns",
            "extra": "100 samples\n888 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 510.954,
            "range": "± 32.9283",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 480.33,
            "range": "± 9.19536",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 96.9239,
            "range": "± 798.55",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 96.7014,
            "range": "± 260.067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 32.0066,
            "range": "± 0.464547",
            "unit": "ns",
            "extra": "100 samples\n694 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 32.2098,
            "range": "± 2.28446",
            "unit": "ns",
            "extra": "100 samples\n761 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "a2e638a8cb2ed7982c2104f0445377ce22b9fe42",
          "message": "Add tbc backend: state-of-the-art tail-call-threaded bytecode interpreter (#331)",
          "timestamp": "2026-07-02T21:01:55+02:00",
          "tree_id": "a06656c31cdad4e676a304f85c8aaaea03f6efe6",
          "url": "https://github.com/nebulastream/nautilus/commit/a2e638a8cb2ed7982c2104f0445377ce22b9fe42"
        },
        "date": 1783019565454,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 11.1394,
            "range": "± 1.29642",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.5533,
            "range": "± 2.23765",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 619.778,
            "range": "± 51.0882",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.74538,
            "range": "± 0.556794",
            "unit": "ns",
            "extra": "100 samples\n6554 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.8153,
            "range": "± 12.2369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0661,
            "range": "± 145.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 50.5048,
            "range": "± 7.16377",
            "unit": "ns",
            "extra": "100 samples\n603 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 770.736,
            "range": "± 18.925",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 165.077,
            "range": "± 542.614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 28.3048,
            "range": "± 4.72749",
            "unit": "ns",
            "extra": "100 samples\n1112 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 184.33,
            "range": "± 12.457",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 38.644,
            "range": "± 74.6056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.67728,
            "range": "± 0.701657",
            "unit": "ns",
            "extra": "100 samples\n8467 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.6189,
            "range": "± 1.85894",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80868,
            "range": "± 44.6852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 50.4985,
            "range": "± 8.6408",
            "unit": "ns",
            "extra": "100 samples\n604 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 51.9944,
            "range": "± 10.0806",
            "unit": "ns",
            "extra": "100 samples\n603 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 771.164,
            "range": "± 15.9512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 772.622,
            "range": "± 18.5247",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 165.015,
            "range": "± 499.9",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 165.086,
            "range": "± 696.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 51.9094,
            "range": "± 10.4397",
            "unit": "ns",
            "extra": "100 samples\n604 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 43.4898,
            "range": "± 6.0129",
            "unit": "ns",
            "extra": "100 samples\n690 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.257,
            "range": "± 7.53302",
            "unit": "ns",
            "extra": "100 samples\n769 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 770.167,
            "range": "± 18.3878",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 728.385,
            "range": "± 14.1532",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 591.249,
            "range": "± 13.4773",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 164.978,
            "range": "± 598.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 147.599,
            "range": "± 3.74683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 116.44,
            "range": "± 2.24151",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 41.954,
            "range": "± 8.77941",
            "unit": "ns",
            "extra": "100 samples\n720 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 37.3017,
            "range": "± 9.17477",
            "unit": "ns",
            "extra": "100 samples\n886 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 589.3,
            "range": "± 13.4751",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 589.328,
            "range": "± 12.5641",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 114.97,
            "range": "± 689.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 115.509,
            "range": "± 1.2739",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 41.3972,
            "range": "± 8.60283",
            "unit": "ns",
            "extra": "100 samples\n713 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.8918,
            "range": "± 7.16347",
            "unit": "ns",
            "extra": "100 samples\n736 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.6867,
            "range": "± 8.41213",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 320.623,
            "range": "± 18.8335",
            "unit": "ns",
            "extra": "100 samples\n8254 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.08651,
            "range": "± 0.658097",
            "unit": "ns",
            "extra": "100 samples\n7736 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.0215,
            "range": "± 3.31851",
            "unit": "ns",
            "extra": "100 samples\n734 iterations"
          },
          {
            "name": "ssa_add",
            "value": 190.099,
            "range": "± 20.161",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 472.742,
            "range": "± 64.9479",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18842,
            "range": "± 161.68",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 502.005,
            "range": "± 53.7494",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 932.708,
            "range": "± 123.071",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 511.084,
            "range": "± 56.6048",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 621.555,
            "range": "± 83.6319",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 434.039,
            "range": "± 68.3385",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 534.988,
            "range": "± 76.6089",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 477.995,
            "range": "± 59.69",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.11358,
            "range": "± 266.613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.6521,
            "range": "± 313.042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.2672,
            "range": "± 201.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.82831,
            "range": "± 333.379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.8672,
            "range": "± 469.058",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.9834,
            "range": "± 543.821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5134,
            "range": "± 498.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.84021,
            "range": "± 369.425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.7413,
            "range": "± 369.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.398,
            "range": "± 399.848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.4528,
            "range": "± 421.438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.2369,
            "range": "± 335.3",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.6661,
            "range": "± 433.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.0062,
            "range": "± 364.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.5048,
            "range": "± 662.203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0358,
            "range": "± 708.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.3071,
            "range": "± 765.183",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.8153,
            "range": "± 534.41",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.3701,
            "range": "± 1.34342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.398,
            "range": "± 546.125",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.1493,
            "range": "± 483.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.2911,
            "range": "± 506.54",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.028,
            "range": "± 532.155",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.7407,
            "range": "± 643.156",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.8031,
            "range": "± 436.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.8442,
            "range": "± 833.12",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 34.5453,
            "range": "± 621.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.1626,
            "range": "± 932.293",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.988,
            "range": "± 2.52574",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.9841,
            "range": "± 3.74247",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.2223,
            "range": "± 4.40988",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.4032,
            "range": "± 4.43819",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.2485,
            "range": "± 4.7286",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.2994,
            "range": "± 4.43385",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.8919,
            "range": "± 4.56765",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.7043,
            "range": "± 4.04607",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.9205,
            "range": "± 4.21298",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.3948,
            "range": "± 3.61939",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 44.2637,
            "range": "± 10.104",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 209.024,
            "range": "± 14.2025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 53.9659,
            "range": "± 9.31445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 331.827,
            "range": "± 21.0202",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.56843,
            "range": "± 188.732",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 2.83905,
            "range": "± 287.138",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 6.32146,
            "range": "± 774.319",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.01326,
            "range": "± 427.252",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 4.3913,
            "range": "± 677.008",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.04385,
            "range": "± 190.507",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 3.55564,
            "range": "± 172.693",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 3.12322,
            "range": "± 444.455",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.12891,
            "range": "± 354.25",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.70572,
            "range": "± 285.809",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 18.7178,
            "range": "± 2.87936",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 156.245,
            "range": "± 10.5886",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 21.2689,
            "range": "± 2.58659",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 208.611,
            "range": "± 9.56424",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.9234,
            "range": "± 5.25357",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.9076,
            "range": "± 5.50471",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 50.8557,
            "range": "± 10.112",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.3387,
            "range": "± 6.69604",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.3743,
            "range": "± 9.32906",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 43.436,
            "range": "± 7.39246",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.6572,
            "range": "± 9.01712",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.7531,
            "range": "± 5.6085",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 35.1146,
            "range": "± 6.01297",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.127,
            "range": "± 5.21268",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 77.1082,
            "range": "± 11.0101",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.509205,
            "range": "± 0.0187933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 97.181,
            "range": "± 13.9924",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 0.661891,
            "range": "± 0.0234462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.45523,
            "range": "± 361.949",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.48523,
            "range": "± 307.133",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.42845,
            "range": "± 1.96181",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.03625,
            "range": "± 935.682",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.6017,
            "range": "± 3.69024",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.9175,
            "range": "± 2.78922",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.83303,
            "range": "± 2.03945",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.93336,
            "range": "± 1.00157",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.4831,
            "range": "± 3.86388",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.90398,
            "range": "± 1.30929",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.07244,
            "range": "± 1.68541",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.20973,
            "range": "± 953.536",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.4992,
            "range": "± 2.32925",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.6253,
            "range": "± 1.96017",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.48969,
            "range": "± 910.063",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.83033,
            "range": "± 1.23219",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1775,
            "range": "± 1.78163",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.33168,
            "range": "± 1.07442",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.52136,
            "range": "± 2.2001",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.28571,
            "range": "± 669.127",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.839,
            "range": "± 7.28023",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 41.1683,
            "range": "± 8.65039",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35281,
            "range": "± 71.0242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.3697,
            "range": "± 70.9454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.7762,
            "range": "± 12.2897",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.7013,
            "range": "± 10.1998",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.44285,
            "range": "± 52.6309",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.24995,
            "range": "± 69.8375",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 57.2398,
            "range": "± 14.4228",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.37149,
            "range": "± 140.78",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.6322,
            "range": "± 719.211",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 58.2783,
            "range": "± 14.7574",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 79.5342,
            "range": "± 16.4051",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.62135,
            "range": "± 257.453",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.9744,
            "range": "± 567.367",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 79.5937,
            "range": "± 16.5534",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 775.745,
            "range": "± 82.4078",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.60929,
            "range": "± 174.537",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.47648,
            "range": "± 512.44",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.74851,
            "range": "± 242.727",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.9474,
            "range": "± 425.507",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.86325,
            "range": "± 200.451",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.04614,
            "range": "± 254.778",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.48271,
            "range": "± 246.878",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.84995,
            "range": "± 285.18",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53034,
            "range": "± 185.633",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.73151,
            "range": "± 973.174",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.1143,
            "range": "± 5.97395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.756,
            "range": "± 1.35651",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 166.397,
            "range": "± 8.02266",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 57.1422,
            "range": "± 13.6389",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.06769,
            "range": "± 292.45",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "074c4edb00f6ac5c54d1307c2451fa25e376764b",
          "message": "Extend NautilusFunction tests to cover all val&lt;T&gt; data types (#332)",
          "timestamp": "2026-07-02T22:01:43+02:00",
          "tree_id": "b8b18f779a493d3f39ef43ce52863295dd1c937c",
          "url": "https://github.com/nebulastream/nautilus/commit/074c4edb00f6ac5c54d1307c2451fa25e376764b"
        },
        "date": 1783023000556,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.1218,
            "range": "± 198.334",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.1751,
            "range": "± 202.705",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.54916,
            "range": "± 1.19913",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.19066,
            "range": "± 393.471",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.9338,
            "range": "± 3.02664",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.2594,
            "range": "± 1.69859",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.28133,
            "range": "± 1.38373",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.95183,
            "range": "± 1.14763",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.0868,
            "range": "± 5.32967",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.33188,
            "range": "± 1.15339",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.53723,
            "range": "± 1.28249",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.5322,
            "range": "± 598.27",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.2374,
            "range": "± 1.86889",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.13772,
            "range": "± 1.08781",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.14414,
            "range": "± 743.188",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.19991,
            "range": "± 687.234",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.58294,
            "range": "± 1.24306",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.5376,
            "range": "± 580.692",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.53163,
            "range": "± 1.04652",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.69236,
            "range": "± 412.874",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 36.8105,
            "range": "± 6.17245",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 35.6578,
            "range": "± 4.71073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.32451,
            "range": "± 32.9259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.34956,
            "range": "± 46.5067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.1556,
            "range": "± 10.9896",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 47.2685,
            "range": "± 8.03058",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42266,
            "range": "± 48.9677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.18663,
            "range": "± 45.4591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 55.0896,
            "range": "± 14.7118",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.43751,
            "range": "± 240.119",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.6818,
            "range": "± 570.683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 57.5464,
            "range": "± 14.4133",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 79.2171,
            "range": "± 18.497",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.8598,
            "range": "± 361.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.5037,
            "range": "± 580.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 79.3208,
            "range": "± 14.3572",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 58.469,
            "range": "± 17.421",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.28359,
            "range": "± 543.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 13.2972,
            "range": "± 3.10893",
            "unit": "ns",
            "extra": "100 samples\n8429 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.0356,
            "range": "± 1.73314",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 583.796,
            "range": "± 23.9609",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.76399,
            "range": "± 0.686796",
            "unit": "ns",
            "extra": "100 samples\n6569 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.5812,
            "range": "± 6.80302",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.953,
            "range": "± 102.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.2758,
            "range": "± 6.38097",
            "unit": "ns",
            "extra": "100 samples\n604 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 768.426,
            "range": "± 8.42818",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 164.958,
            "range": "± 1.73769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 25.9141,
            "range": "± 2.2477",
            "unit": "ns",
            "extra": "100 samples\n1102 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 176.308,
            "range": "± 7.30433",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 38.6258,
            "range": "± 153.752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.50879,
            "range": "± 0.391789",
            "unit": "ns",
            "extra": "100 samples\n8447 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.188,
            "range": "± 1.14094",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.79642,
            "range": "± 21.1677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.5766,
            "range": "± 7.29704",
            "unit": "ns",
            "extra": "100 samples\n605 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.0881,
            "range": "± 3.96886",
            "unit": "ns",
            "extra": "100 samples\n606 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 766.038,
            "range": "± 9.60965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 768.837,
            "range": "± 10.0066",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 164.767,
            "range": "± 275.548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 180.989,
            "range": "± 15.7767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.6982,
            "range": "± 8.2113",
            "unit": "ns",
            "extra": "100 samples\n600 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 43.6482,
            "range": "± 6.46323",
            "unit": "ns",
            "extra": "100 samples\n691 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 40.8723,
            "range": "± 5.68378",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 769.9,
            "range": "± 12.3435",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 727.444,
            "range": "± 9.86178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 589.09,
            "range": "± 12.8252",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 164.862,
            "range": "± 599.109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 147.683,
            "range": "± 4.38591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 115.179,
            "range": "± 1.84068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 38.9783,
            "range": "± 2.39514",
            "unit": "ns",
            "extra": "100 samples\n748 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 37.6537,
            "range": "± 8.91929",
            "unit": "ns",
            "extra": "100 samples\n873 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 586.81,
            "range": "± 14.8617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 588.362,
            "range": "± 13.2339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 114.766,
            "range": "± 407.905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 115.945,
            "range": "± 996.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 42.4661,
            "range": "± 10.6941",
            "unit": "ns",
            "extra": "100 samples\n740 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.1981,
            "range": "± 6.25817",
            "unit": "ns",
            "extra": "100 samples\n737 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.13641,
            "range": "± 401.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.97062,
            "range": "± 469.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.76703,
            "range": "± 310.329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.85283,
            "range": "± 549.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0883,
            "range": "± 988.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.1959,
            "range": "± 367.252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.9548,
            "range": "± 629.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.4767,
            "range": "± 356.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.5975,
            "range": "± 417.155",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.6818,
            "range": "± 346.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.186,
            "range": "± 1.06632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.2001,
            "range": "± 269.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.4897,
            "range": "± 917.198",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.4056,
            "range": "± 510.63",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.8741,
            "range": "± 671.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.1905,
            "range": "± 713.505",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.0127,
            "range": "± 569.811",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.6587,
            "range": "± 474.586",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.0354,
            "range": "± 688.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.5826,
            "range": "± 551.25",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.7885,
            "range": "± 729.659",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.597,
            "range": "± 1.01222",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.2791,
            "range": "± 1.11085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.4815,
            "range": "± 751.184",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.1501,
            "range": "± 1.34962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.0251,
            "range": "± 1.48897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.021,
            "range": "± 675.28",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.5993,
            "range": "± 1.52935",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.1536,
            "range": "± 3.15076",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.4687,
            "range": "± 4.45806",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.1274,
            "range": "± 3.68767",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.2203,
            "range": "± 3.37207",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.372,
            "range": "± 3.76326",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.833,
            "range": "± 3.73072",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.3617,
            "range": "± 4.21932",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.0812,
            "range": "± 5.35738",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.5933,
            "range": "± 3.8096",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.5318,
            "range": "± 3.22326",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 44.6559,
            "range": "± 8.73589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 215.92,
            "range": "± 12.3981",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 54.3584,
            "range": "± 8.01017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 329.036,
            "range": "± 15.6925",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.46094,
            "range": "± 134.23",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 2.82698,
            "range": "± 226.49",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 6.1784,
            "range": "± 598.55",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 2.87262,
            "range": "± 242.009",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 4.28569,
            "range": "± 358.009",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.09538,
            "range": "± 432.092",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 3.63022,
            "range": "± 316.691",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 3.07307,
            "range": "± 355.329",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.09954,
            "range": "± 343.408",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.68946,
            "range": "± 204.867",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 18.6882,
            "range": "± 2.58542",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 162.534,
            "range": "± 8.22662",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 21.2328,
            "range": "± 2.00238",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 221.62,
            "range": "± 15.5031",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.2078,
            "range": "± 4.79948",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.1068,
            "range": "± 5.70645",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 50.6355,
            "range": "± 10.599",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.9785,
            "range": "± 5.19491",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.2665,
            "range": "± 10.9352",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.7565,
            "range": "± 5.69014",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.0874,
            "range": "± 9.84671",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 30.9532,
            "range": "± 5.96131",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.1223,
            "range": "± 5.34521",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.1614,
            "range": "± 6.80666",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 78.2374,
            "range": "± 12.9299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.507773,
            "range": "± 0.0183614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 92.7049,
            "range": "± 10.7661",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 0.679177,
            "range": "± 0.0739655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 185.469,
            "range": "± 12.4971",
            "unit": "ns",
            "extra": "100 samples\n165 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 457.179,
            "range": "± 42.5631",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.14706,
            "range": "± 95.2601",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 481.407,
            "range": "± 47.1806",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 899.793,
            "range": "± 81.0553",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 493.382,
            "range": "± 40.831",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 586.62,
            "range": "± 43.5175",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 397.149,
            "range": "± 26.9336",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 518.865,
            "range": "± 58.8508",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 454.337,
            "range": "± 37.0803",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ir_add",
            "value": 751.931,
            "range": "± 63.2856",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.59595,
            "range": "± 175.66",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.45037,
            "range": "± 403.631",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.67988,
            "range": "± 164.154",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.85825,
            "range": "± 397.149",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.8541,
            "range": "± 243.544",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.07445,
            "range": "± 303.588",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.44153,
            "range": "± 125.033",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.73646,
            "range": "± 136.068",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.50267,
            "range": "± 142.909",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.67969,
            "range": "± 607.77",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 92.9323,
            "range": "± 6.00156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.0348,
            "range": "± 1.55222",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 174.102,
            "range": "± 13.0168",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.1146,
            "range": "± 6.56146",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 306.336,
            "range": "± 33.9985",
            "unit": "ns",
            "extra": "100 samples\n8252 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.0283,
            "range": "± 0.640002",
            "unit": "ns",
            "extra": "100 samples\n7717 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.6852,
            "range": "± 3.25726",
            "unit": "ns",
            "extra": "100 samples\n763 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "f5ab7ae0e7784b57e3b8593034b39de9c270acd3",
          "message": "Merge duplicated Int/Float evaluators in the differential fuzzer (#333)",
          "timestamp": "2026-07-02T22:41:31+02:00",
          "tree_id": "74e1da18f0279351b80edf819d8dbdb44e1a415e",
          "url": "https://github.com/nebulastream/nautilus/commit/f5ab7ae0e7784b57e3b8593034b39de9c270acd3"
        },
        "date": 1783025341360,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 731.312,
            "range": "± 57.7813",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.52346,
            "range": "± 148.533",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.34544,
            "range": "± 494.085",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.68999,
            "range": "± 229.314",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.73257,
            "range": "± 351.88",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.75136,
            "range": "± 197.509",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.9991,
            "range": "± 283.989",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.37227,
            "range": "± 132.654",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.76064,
            "range": "± 194.095",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.45987,
            "range": "± 149.619",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.29897,
            "range": "± 737.325",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.4055,
            "range": "± 4.34471",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.2679,
            "range": "± 1.04269",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 157.035,
            "range": "± 8.64714",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.4331,
            "range": "± 0.799174",
            "unit": "ns",
            "extra": "100 samples\n8445 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.2491,
            "range": "± 1.39203",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 520.767,
            "range": "± 16.618",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.78527,
            "range": "± 0.795891",
            "unit": "ns",
            "extra": "100 samples\n6562 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.2448,
            "range": "± 11.8772",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9849,
            "range": "± 983.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 54.7958,
            "range": "± 14.334",
            "unit": "ns",
            "extra": "100 samples\n616 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 765.196,
            "range": "± 10.901",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 161.296,
            "range": "± 9.18127",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 28.7342,
            "range": "± 4.44597",
            "unit": "ns",
            "extra": "100 samples\n1062 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 174.269,
            "range": "± 5.74177",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 38.5784,
            "range": "± 86.9059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.5139,
            "range": "± 0.35231",
            "unit": "ns",
            "extra": "100 samples\n8478 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.7408,
            "range": "± 2.60169",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.79515,
            "range": "± 24.5234",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 50.827,
            "range": "± 4.1855",
            "unit": "ns",
            "extra": "100 samples\n610 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 52.5836,
            "range": "± 9.65715",
            "unit": "ns",
            "extra": "100 samples\n614 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 764.849,
            "range": "± 15.2233",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 763.502,
            "range": "± 10.2728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 161.793,
            "range": "± 8.63687",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 162.365,
            "range": "± 7.92303",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 51.9573,
            "range": "± 7.35159",
            "unit": "ns",
            "extra": "100 samples\n613 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 42.9115,
            "range": "± 5.18667",
            "unit": "ns",
            "extra": "100 samples\n693 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.639,
            "range": "± 4.84585",
            "unit": "ns",
            "extra": "100 samples\n750 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 762.474,
            "range": "± 8.44486",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 734.633,
            "range": "± 7.13697",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 583.256,
            "range": "± 9.58056",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 159.928,
            "range": "± 10.8304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 149.25,
            "range": "± 4.48285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 115.782,
            "range": "± 1.55232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.1757,
            "range": "± 3.76309",
            "unit": "ns",
            "extra": "100 samples\n749 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.452,
            "range": "± 4.7516",
            "unit": "ns",
            "extra": "100 samples\n885 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 586.443,
            "range": "± 11.9201",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 587.126,
            "range": "± 10.721",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 115.635,
            "range": "± 1.23342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 114.756,
            "range": "± 676.961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.8537,
            "range": "± 7.44851",
            "unit": "ns",
            "extra": "100 samples\n721 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.6108,
            "range": "± 5.81142",
            "unit": "ns",
            "extra": "100 samples\n743 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 58.6641,
            "range": "± 15.3289",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.189,
            "range": "± 94.7752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.0781,
            "range": "± 480.435",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 57.9865,
            "range": "± 12.9742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 77.3108,
            "range": "± 14.6013",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.15167,
            "range": "± 127.623",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.4388,
            "range": "± 387.908",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 78.637,
            "range": "± 14.186",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.54113,
            "range": "± 371.511",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.52099,
            "range": "± 399.373",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.08629,
            "range": "± 1.56532",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.86752,
            "range": "± 723.387",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.4409,
            "range": "± 3.15361",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.433,
            "range": "± 2.00027",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.93354,
            "range": "± 1.27391",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.0566,
            "range": "± 826.52",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.0332,
            "range": "± 2.96948",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.82294,
            "range": "± 1.39402",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.99389,
            "range": "± 1.17972",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.97656,
            "range": "± 619.812",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.1068,
            "range": "± 1.54887",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.5603,
            "range": "± 1.77242",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.96981,
            "range": "± 1.39746",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.5376,
            "range": "± 852.907",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1526,
            "range": "± 1.6759",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.41995,
            "range": "± 930.565",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.97456,
            "range": "± 1.21902",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.08581,
            "range": "± 549.677",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.9854,
            "range": "± 7.12828",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.1921,
            "range": "± 6.25051",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34269,
            "range": "± 38.144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36264,
            "range": "± 53.5774",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.3486,
            "range": "± 10.4458",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.7992,
            "range": "± 8.58838",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.4309,
            "range": "± 43.9415",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.28669,
            "range": "± 35.3939",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 187.742,
            "range": "± 18.4781",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 454.832,
            "range": "± 46.189",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.16939,
            "range": "± 146.692",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 486.628,
            "range": "± 41.4327",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 903.433,
            "range": "± 84.1419",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 490.925,
            "range": "± 45.5689",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 576.082,
            "range": "± 47.3598",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 408.55,
            "range": "± 37.9031",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 522.412,
            "range": "± 117.672",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 481.865,
            "range": "± 64.5801",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.50727,
            "range": "± 209.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.0961,
            "range": "± 212.534",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.97219,
            "range": "± 127.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.03872,
            "range": "± 198.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.311,
            "range": "± 289.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6572,
            "range": "± 203.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.5616,
            "range": "± 438.594",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.91875,
            "range": "± 129.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.2477,
            "range": "± 198.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.2402,
            "range": "± 164.997",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.2925,
            "range": "± 204.454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.7398,
            "range": "± 832.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.4771,
            "range": "± 196.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.3126,
            "range": "± 281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.8333,
            "range": "± 303.744",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.0199,
            "range": "± 379.468",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.4084,
            "range": "± 358.398",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.5708,
            "range": "± 549.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.0957,
            "range": "± 349.426",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.7654,
            "range": "± 429.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.5245,
            "range": "± 357.266",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.925,
            "range": "± 555.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.5589,
            "range": "± 396.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3514,
            "range": "± 524.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.2465,
            "range": "± 461.16",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.8871,
            "range": "± 739.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.7949,
            "range": "± 491.147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.0756,
            "range": "± 689.874",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.9331,
            "range": "± 2.81822",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.8752,
            "range": "± 5.26853",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.8984,
            "range": "± 3.29815",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.2192,
            "range": "± 3.46114",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.9519,
            "range": "± 3.2401",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.5906,
            "range": "± 3.64137",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.2413,
            "range": "± 3.43413",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.852,
            "range": "± 3.37215",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.6449,
            "range": "± 3.73933",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.8241,
            "range": "± 3.47737",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 43.6097,
            "range": "± 8.02617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 207.216,
            "range": "± 11.4993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 57.2982,
            "range": "± 9.89483",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 343.473,
            "range": "± 12.5534",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.5199,
            "range": "± 182.241",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 2.90827,
            "range": "± 435.057",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 6.32727,
            "range": "± 884.843",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.00274,
            "range": "± 471.671",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 4.32389,
            "range": "± 476.792",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.09694,
            "range": "± 284.382",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 3.72534,
            "range": "± 422.217",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 2.97082,
            "range": "± 238.42",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.07421,
            "range": "± 296.071",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.74045,
            "range": "± 475.506",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 18.571,
            "range": "± 2.40206",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 155.275,
            "range": "± 6.03759",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 21.5686,
            "range": "± 2.24574",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 205.195,
            "range": "± 7.64511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.2119,
            "range": "± 4.41374",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.6261,
            "range": "± 5.59394",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 49.9457,
            "range": "± 8.93667",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.8771,
            "range": "± 4.88533",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.1953,
            "range": "± 8.00402",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.6429,
            "range": "± 5.13256",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 38.2506,
            "range": "± 5.09237",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.3053,
            "range": "± 5.16832",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 35.07,
            "range": "± 6.63313",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.435,
            "range": "± 5.28586",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 78.983,
            "range": "± 12.0497",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.497952,
            "range": "± 0.0480867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 94.1845,
            "range": "± 11.854",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 0.633514,
            "range": "± 0.0173828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 56.7835,
            "range": "± 13.679",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.41692,
            "range": "± 146.515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.9108,
            "range": "± 7.28019",
            "unit": "ns",
            "extra": "100 samples\n771 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 279.135,
            "range": "± 9.03633",
            "unit": "ns",
            "extra": "100 samples\n8296 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.06671,
            "range": "± 0.831728",
            "unit": "ns",
            "extra": "100 samples\n7770 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.7579,
            "range": "± 3.64356",
            "unit": "ns",
            "extra": "100 samples\n766 iterations"
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
          "id": "ecf067b4a069b6a382db27107b0607d7842d4084",
          "message": "Document preserve_none's actual measured impact in tbc README\n\nLocal codegen inspection (LLVM IR + assembly diff, Clang 21) and isolated\nmicro-benchmarks show the win is real but narrow: only handlers that call\na plain-ABI helper across the tail-call boundary (CALL, CALL_EXT/CALL_IND)\nsee fewer register spills, and only CALL (internal Nautilus-to-Nautilus\ncalls) translates into a measurable ~4% speedup -- CALL_EXT's codegen\nimproves the same way but is swamped by dyncall's own FFI overhead. The\n~305 non-call opcode handlers are byte-identical either way. Records why\nexec_tbc_add/fibonacci/sum (pure arithmetic/loop kernels, no calls) show\nno benefit: they don't exercise the path that improves.",
          "timestamp": "2026-07-03T06:56:06+02:00",
          "tree_id": "f775938484e094eb61d6deff24fce3f064640dfb",
          "url": "https://github.com/nebulastream/nautilus/commit/ecf067b4a069b6a382db27107b0607d7842d4084"
        },
        "date": 1783055077288,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 40.428,
            "range": "± 7.08469",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 320.939,
            "range": "± 57.1339",
            "unit": "ns",
            "extra": "100 samples\n8245 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09976,
            "range": "± 0.670721",
            "unit": "ns",
            "extra": "100 samples\n7705 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.4986,
            "range": "± 2.41648",
            "unit": "ns",
            "extra": "100 samples\n739 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.9684,
            "range": "± 1.29149",
            "unit": "ns",
            "extra": "100 samples\n8423 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.4589,
            "range": "± 1.71905",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 313.93,
            "range": "± 106.452",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.72994,
            "range": "± 0.593387",
            "unit": "ns",
            "extra": "100 samples\n6558 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.1238,
            "range": "± 7.6993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.6194,
            "range": "± 270.06",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.5844,
            "range": "± 8.43439",
            "unit": "ns",
            "extra": "100 samples\n564 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 747.037,
            "range": "± 8.41256",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 164.492,
            "range": "± 4.73417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 28.1924,
            "range": "± 2.80441",
            "unit": "ns",
            "extra": "100 samples\n1071 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 179.19,
            "range": "± 7.86646",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 41.77,
            "range": "± 512.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54808,
            "range": "± 0.50505",
            "unit": "ns",
            "extra": "100 samples\n8457 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.3203,
            "range": "± 1.74304",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80887,
            "range": "± 35.4088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.5766,
            "range": "± 6.00708",
            "unit": "ns",
            "extra": "100 samples\n641 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.811,
            "range": "± 6.76089",
            "unit": "ns",
            "extra": "100 samples\n641 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 750.238,
            "range": "± 11.1266",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 753.703,
            "range": "± 26.1942",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 164.258,
            "range": "± 645.517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 164.749,
            "range": "± 4.35326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.189,
            "range": "± 7.29732",
            "unit": "ns",
            "extra": "100 samples\n645 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 45.7012,
            "range": "± 3.19438",
            "unit": "ns",
            "extra": "100 samples\n640 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.3071,
            "range": "± 3.87374",
            "unit": "ns",
            "extra": "100 samples\n782 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 748.058,
            "range": "± 9.72962",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 713.281,
            "range": "± 13.1037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 905.774,
            "range": "± 26.8771",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 165.385,
            "range": "± 7.01772",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 152.367,
            "range": "± 662.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 169.182,
            "range": "± 3.44495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.2277,
            "range": "± 5.03579",
            "unit": "ns",
            "extra": "100 samples\n777 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.4633,
            "range": "± 4.00554",
            "unit": "ns",
            "extra": "100 samples\n862 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 901.964,
            "range": "± 29.5783",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 908.886,
            "range": "± 41.7598",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 169.547,
            "range": "± 3.46967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 168.73,
            "range": "± 2.91771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.6323,
            "range": "± 4.61169",
            "unit": "ns",
            "extra": "100 samples\n774 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.9042,
            "range": "± 5.53312",
            "unit": "ns",
            "extra": "100 samples\n728 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.84226,
            "range": "± 217.533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.2994,
            "range": "± 213.519",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.2914,
            "range": "± 238.09",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.58097,
            "range": "± 354.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.0216,
            "range": "± 361.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.7955,
            "range": "± 250.389",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.2474,
            "range": "± 305.073",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.17699,
            "range": "± 218.282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.21,
            "range": "± 293.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.87564,
            "range": "± 300.908",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.9291,
            "range": "± 328.836",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.5943,
            "range": "± 1.73645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.0863,
            "range": "± 415.047",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.7618,
            "range": "± 1.7443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.3628,
            "range": "± 586.659",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.8935,
            "range": "± 404.664",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.7703,
            "range": "± 524.279",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.3219,
            "range": "± 504.692",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.413,
            "range": "± 382.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.5379,
            "range": "± 595.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.699,
            "range": "± 418.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.5852,
            "range": "± 567.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.4414,
            "range": "± 2.27513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.0977,
            "range": "± 648.488",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.0391,
            "range": "± 374.795",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.2134,
            "range": "± 2.29079",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.8543,
            "range": "± 552.614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.2726,
            "range": "± 696.716",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.3049,
            "range": "± 2.61999",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.067,
            "range": "± 3.8703",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 24.1481,
            "range": "± 4.15885",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.3855,
            "range": "± 4.60567",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.5763,
            "range": "± 3.69959",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.6585,
            "range": "± 3.37932",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.4593,
            "range": "± 4.06525",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.3099,
            "range": "± 3.20143",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.7463,
            "range": "± 3.16137",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.8846,
            "range": "± 3.92646",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 45.0052,
            "range": "± 8.72377",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 206.771,
            "range": "± 10.6695",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 56.4552,
            "range": "± 9.6244",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 321.359,
            "range": "± 11.6596",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.52791,
            "range": "± 117.926",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 2.90955,
            "range": "± 315.266",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 6.31528,
            "range": "± 807.659",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.01444,
            "range": "± 351.039",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 4.42839,
            "range": "± 523.283",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.12574,
            "range": "± 251.699",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 3.66505,
            "range": "± 423.569",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 3.13741,
            "range": "± 363.158",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.2354,
            "range": "± 461.074",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.73983,
            "range": "± 275.525",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 18.5875,
            "range": "± 2.55058",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 157.183,
            "range": "± 7.13152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 22.3735,
            "range": "± 5.51424",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 205.243,
            "range": "± 9.13369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.6915,
            "range": "± 4.68813",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.2059,
            "range": "± 6.02583",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 49.0727,
            "range": "± 9.34694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.0212,
            "range": "± 5.14494",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.8673,
            "range": "± 9.60713",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 37.3402,
            "range": "± 5.81205",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.6927,
            "range": "± 10.1267",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.5652,
            "range": "± 6.21978",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.7587,
            "range": "± 5.3639",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.6776,
            "range": "± 7.08604",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 78.0122,
            "range": "± 11.5842",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.490646,
            "range": "± 0.020412600000000003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 92.8585,
            "range": "± 10.6889",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 0.645993,
            "range": "± 0.0202304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 801.319,
            "range": "± 98.4382",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.6451,
            "range": "± 223.106",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.45163,
            "range": "± 375.647",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.64477,
            "range": "± 139.515",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.97366,
            "range": "± 361.157",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.87355,
            "range": "± 200.835",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.05719,
            "range": "± 248.048",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.49516,
            "range": "± 159.83",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.80113,
            "range": "± 187.051",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.55615,
            "range": "± 160.574",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.81934,
            "range": "± 879.814",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 86.9764,
            "range": "± 4.89077",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8825,
            "range": "± 1.39161",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 167.202,
            "range": "± 9.59674",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.40987,
            "range": "± 266.779",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.41132,
            "range": "± 281.688",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.03902,
            "range": "± 1.34707",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.77979,
            "range": "± 651.2",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.3668,
            "range": "± 3.5426",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.0576,
            "range": "± 2.00041",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.78057,
            "range": "± 1.54191",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.89217,
            "range": "± 643.673",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.295,
            "range": "± 3.3148",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.86255,
            "range": "± 1.66568",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.97138,
            "range": "± 1.40803",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.06036,
            "range": "± 914.208",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.3499,
            "range": "± 2.35456",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.5842,
            "range": "± 1.76031",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.48748,
            "range": "± 907.551",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.39656,
            "range": "± 844.382",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1076,
            "range": "± 1.86768",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.19475,
            "range": "± 841.6",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.27293,
            "range": "± 1.80967",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.01819,
            "range": "± 480.84",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.5599,
            "range": "± 8.00958",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.3775,
            "range": "± 7.8001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.36459,
            "range": "± 54.4203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.38109,
            "range": "± 112.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.9199,
            "range": "± 10.263",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.1607,
            "range": "± 9.03928",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45516,
            "range": "± 67.9054",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.34291,
            "range": "± 44.92",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 60.8033,
            "range": "± 14.8558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.74133,
            "range": "± 251.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 58.5325,
            "range": "± 17.6008",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.29716,
            "range": "± 162.989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.543,
            "range": "± 1.31282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 58.8536,
            "range": "± 13.994",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 78.3631,
            "range": "± 15.372",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.5041,
            "range": "± 380.817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.5152,
            "range": "± 395.754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 79.8333,
            "range": "± 15.578",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 193.507,
            "range": "± 30.8206",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 466.682,
            "range": "± 51.6669",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.19422,
            "range": "± 126.191",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 501.487,
            "range": "± 51.4835",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 940.676,
            "range": "± 132.702",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 502.608,
            "range": "± 56.309",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 598.216,
            "range": "± 51.1614",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 503.7,
            "range": "± 47.1417",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 525.933,
            "range": "± 51.7292",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 460.115,
            "range": "± 36.0849",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
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
          "id": "6532dfcb15d8ac09caf933fdb32e538962703bac",
          "message": "Add iOS-safe libffi closure path to the bytecode backend\n\nThe bytecode interpreter is the only iOS-viable backend (MLIR/LLVM-JIT,\nAsmJit, and C++ all generate executable code at runtime), but it still\nexposes every compiled function through a dyncall callback\n(dcbNewCallback), which writes a trampoline into RWX memory at runtime —\nforbidden on iOS without a JIT entitlement App Store apps cannot get.\n\nAdd an opt-in ENABLE_BC_LIBFFI build flag that swaps the dyncall callback\nfor a libffi closure. libffi >= 3.4 builds closures with static\ntrampolines (a read-only code page plus a separate writable data page, no\nruntime PROT_EXEC), so the closure stays a real C function pointer usable\nwhere runtime codegen is banned. Default (OFF) behavior is unchanged:\ndyncall callbacks, dyncallback linked.\n\nOnly the closure boundary differs between the two modes; the interpreter\ncore, register marshalling, lowering, and the Executable function-pointer\ncontract are shared. dyncall forward calls (dcCall*) stay in both modes for\nexternal proxy/runtime calls — they generate no code and are iOS-safe.\n\n- ENABLE_BC_LIBFFI option + NAUTILUS_BC_LIBFFI config define\n- cmake/FindNautilusFFI.cmake locates libffi (pkg-config / find_library)\n  and exposes it as nautilus::ffi; an iOS toolchain supplies Apple's\n  static-trampoline libffi the same way\n- BC backend links ffi (ON) vs dyncallback_s (OFF); dyncall_s always\n- bcFFIHandler + Type->ffi_type mapping; ffi_cif/ffi_closure created per\n  function with cif/atypes stored to outlive the closure\n- BCInterpreter::invoke(void**) reads args from the libffi argument array;\n  shared invokeImpl keeps register-file/alloca setup identical to the\n  dyncall path\n\nValidated on Linux x86-64 with both flag values: all 180 tests pass. In\nthe ON build the final binary links libffi and contains zero dcbNewCallback\nreferences, confirming no runtime-executable-memory dependency.\n\nCo-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_01Xuz3fvuoaaKmGVW9SNLZ9B",
          "timestamp": "2026-07-03T07:14:46+02:00",
          "tree_id": "a80615adaaf1a2f03b11517354ad7a4ea5cccb6b",
          "url": "https://github.com/nebulastream/nautilus/commit/6532dfcb15d8ac09caf933fdb32e538962703bac"
        },
        "date": 1783056214443,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 5.69707,
            "range": "± 149.182",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.45146,
            "range": "± 188.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.12668,
            "range": "± 140.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.55421,
            "range": "± 395.899",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.0552,
            "range": "± 1.39605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.5679,
            "range": "± 443.644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.2147,
            "range": "± 561.739",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.13796,
            "range": "± 142.614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.0783,
            "range": "± 470.643",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.97462,
            "range": "± 381.67",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.977,
            "range": "± 238.777",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.7195,
            "range": "± 352.9",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.428,
            "range": "± 302.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.0824,
            "range": "± 1.20837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.0438,
            "range": "± 3.67777",
            "unit": "ns",
            "extra": "100 samples\n770 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 289.964,
            "range": "± 7.0397",
            "unit": "ns",
            "extra": "100 samples\n8267 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.10361,
            "range": "± 0.69205",
            "unit": "ns",
            "extra": "100 samples\n7748 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.4957,
            "range": "± 2.18763",
            "unit": "ns",
            "extra": "100 samples\n686 iterations"
          },
          {
            "name": "ssa_add",
            "value": 201.875,
            "range": "± 38.7301",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 459.921,
            "range": "± 32.4128",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.19015,
            "range": "± 100.576",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 516.105,
            "range": "± 72.4176",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 947.068,
            "range": "± 94.1272",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 507.12,
            "range": "± 43.6824",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 622.276,
            "range": "± 78.2291",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 513.237,
            "range": "± 41.1853",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 539.31,
            "range": "± 61.4756",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 478.441,
            "range": "± 49.7919",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.46222,
            "range": "± 367.281",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.54007,
            "range": "± 404.802",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.91124,
            "range": "± 1.21964",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.56291,
            "range": "± 505.11",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.433,
            "range": "± 3.50491",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.0536,
            "range": "± 1.83297",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.90944,
            "range": "± 1.32744",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.91935,
            "range": "± 661.591",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.3932,
            "range": "± 3.04785",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.70686,
            "range": "± 1.10688",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.93514,
            "range": "± 1.39609",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.92507,
            "range": "± 555.812",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.888,
            "range": "± 2.96878",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.0281,
            "range": "± 2.0854",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.52878,
            "range": "± 1.12406",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.73526,
            "range": "± 1.09906",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.4579,
            "range": "± 1.87266",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.14966,
            "range": "± 1.04796",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.2358,
            "range": "± 1.35425",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.01372,
            "range": "± 682.437",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.7583,
            "range": "± 7.09153",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 41.2775,
            "range": "± 10.1421",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35315,
            "range": "± 50.0388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.37813,
            "range": "± 58.5792",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.1775,
            "range": "± 10.5947",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.153,
            "range": "± 8.59602",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.46595,
            "range": "± 46.3579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.38571,
            "range": "± 39.8678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 58.2027,
            "range": "± 14.436",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.36499,
            "range": "± 150.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.1603,
            "range": "± 414.354",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 59.2048,
            "range": "± 14.8726",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 79.3227,
            "range": "± 14.4158",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.36996,
            "range": "± 150.822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.5669,
            "range": "± 409.648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 80.8344,
            "range": "± 16.1334",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 811.092,
            "range": "± 120.414",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.61219,
            "range": "± 135.697",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.41579,
            "range": "± 331.532",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.65069,
            "range": "± 113.956",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.90712,
            "range": "± 290.453",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.88538,
            "range": "± 224.322",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.10979,
            "range": "± 264.214",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.52327,
            "range": "± 186.029",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.77866,
            "range": "± 152.711",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.52246,
            "range": "± 196.64",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.91621,
            "range": "± 837.249",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.8741,
            "range": "± 7.87352",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.2804,
            "range": "± 1.4862",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 171.263,
            "range": "± 8.49117",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 58.7657,
            "range": "± 13.1999",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.81004,
            "range": "± 216.276",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.7064,
            "range": "± 0.890912",
            "unit": "ns",
            "extra": "100 samples\n8456 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.8748,
            "range": "± 1.66226",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 611.698,
            "range": "± 28.0637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.71154,
            "range": "± 0.658268",
            "unit": "ns",
            "extra": "100 samples\n6566 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.0191,
            "range": "± 9.70039",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9202,
            "range": "± 126.203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.2677,
            "range": "± 7.40763",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 772.635,
            "range": "± 20.5461",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 163.893,
            "range": "± 282.002",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.7625,
            "range": "± 4.51937",
            "unit": "ns",
            "extra": "100 samples\n1145 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 209.483,
            "range": "± 10.3386",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 38.7061,
            "range": "± 685.521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.51685,
            "range": "± 0.368364",
            "unit": "ns",
            "extra": "100 samples\n8473 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.2742,
            "range": "± 1.2728",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.8081,
            "range": "± 63.3692",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.7083,
            "range": "± 6.76022",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 49.0184,
            "range": "± 8.26917",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 770.005,
            "range": "± 14.7415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 763.192,
            "range": "± 20.1398",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 164.576,
            "range": "± 697.987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 164.549,
            "range": "± 586.536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.2694,
            "range": "± 7.57001",
            "unit": "ns",
            "extra": "100 samples\n635 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 48.1387,
            "range": "± 9.97757",
            "unit": "ns",
            "extra": "100 samples\n650 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 43.1255,
            "range": "± 11.084",
            "unit": "ns",
            "extra": "100 samples\n730 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 761.334,
            "range": "± 12.8216",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 710.5,
            "range": "± 16.0571",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 944.319,
            "range": "± 19.7493",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 164.089,
            "range": "± 3.42497",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 152.373,
            "range": "± 566.236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 180.768,
            "range": "± 2.69206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 41.6055,
            "range": "± 7.19066",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 37.3678,
            "range": "± 6.73034",
            "unit": "ns",
            "extra": "100 samples\n855 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 941.17,
            "range": "± 15.6754",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 953.77,
            "range": "± 32.2069",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 180.415,
            "range": "± 2.00527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 180.392,
            "range": "± 1.94482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 42.0034,
            "range": "± 7.04742",
            "unit": "ns",
            "extra": "100 samples\n731 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 42.9549,
            "range": "± 9.94653",
            "unit": "ns",
            "extra": "100 samples\n731 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "b90ed51da7e86eb2c2254924802c81f5aa0728c8",
          "message": "Design doc: IR optimization passes for the direct-lowering backends (asmjit, bc, tbc) (#337)",
          "timestamp": "2026-07-03T08:07:15+02:00",
          "tree_id": "67808384d412cd9a8ea900e9e9cdd259bd9bbc23",
          "url": "https://github.com/nebulastream/nautilus/commit/b90ed51da7e86eb2c2254924802c81f5aa0728c8"
        },
        "date": 1783059298908,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 57.7515,
            "range": "± 13.338",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.35068,
            "range": "± 152.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 785.987,
            "range": "± 107.519",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.60184,
            "range": "± 176.295",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.27717,
            "range": "± 218.313",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.66571,
            "range": "± 151.137",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.86077,
            "range": "± 340.465",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.87474,
            "range": "± 204.571",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.00923,
            "range": "± 227.259",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45246,
            "range": "± 134.598",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.76306,
            "range": "± 146.437",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53405,
            "range": "± 203.274",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.7185,
            "range": "± 696.541",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.3222,
            "range": "± 10.361",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.817,
            "range": "± 1.06841",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 171.381,
            "range": "± 7.67565",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 174.986,
            "range": "± 9.72805",
            "unit": "ns",
            "extra": "100 samples\n170 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 438.988,
            "range": "± 32.539",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.15685,
            "range": "± 121.422",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 465.501,
            "range": "± 25.8082",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 942.069,
            "range": "± 179.569",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 475.406,
            "range": "± 39.0899",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 580.56,
            "range": "± 50.0112",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 396.258,
            "range": "± 43.4347",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 494.621,
            "range": "± 41.654",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 443.212,
            "range": "± 50.5523",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 12.2254,
            "range": "± 2.02802",
            "unit": "ns",
            "extra": "100 samples\n8487 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.3206,
            "range": "± 2.20682",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 554.577,
            "range": "± 27.3314",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.91402,
            "range": "± 1.0726",
            "unit": "ns",
            "extra": "100 samples\n6561 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.1788,
            "range": "± 7.20924",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0213,
            "range": "± 120.285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.9649,
            "range": "± 7.85763",
            "unit": "ns",
            "extra": "100 samples\n643 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 773.688,
            "range": "± 21.859",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 165.257,
            "range": "± 7.10756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.1456,
            "range": "± 3.25713",
            "unit": "ns",
            "extra": "100 samples\n1135 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 162.892,
            "range": "± 8.24209",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5309,
            "range": "± 104.58",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.52367,
            "range": "± 0.389344",
            "unit": "ns",
            "extra": "100 samples\n8475 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.3479,
            "range": "± 1.43044",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.81509,
            "range": "± 72.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.1616,
            "range": "± 5.78132",
            "unit": "ns",
            "extra": "100 samples\n656 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.2383,
            "range": "± 5.30989",
            "unit": "ns",
            "extra": "100 samples\n656 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 767.044,
            "range": "± 10.4197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 764.307,
            "range": "± 9.81505",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 164.893,
            "range": "± 5.86873",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 164.596,
            "range": "± 7.70307",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.8809,
            "range": "± 10.3183",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 42.2169,
            "range": "± 2.97136",
            "unit": "ns",
            "extra": "100 samples\n695 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 40.1022,
            "range": "± 6.64072",
            "unit": "ns",
            "extra": "100 samples\n750 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 771.844,
            "range": "± 11.9276",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 741.439,
            "range": "± 12.3175",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 594.757,
            "range": "± 18.6043",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 165.523,
            "range": "± 4.70245",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 146.069,
            "range": "± 9.13546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 117.867,
            "range": "± 5.36237",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.8608,
            "range": "± 5.6607",
            "unit": "ns",
            "extra": "100 samples\n759 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.0393,
            "range": "± 5.20112",
            "unit": "ns",
            "extra": "100 samples\n896 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 593.665,
            "range": "± 11.0547",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 593.507,
            "range": "± 13.1825",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 116.897,
            "range": "± 284.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 116.98,
            "range": "± 365.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 41.8548,
            "range": "± 10.9148",
            "unit": "ns",
            "extra": "100 samples\n760 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.1658,
            "range": "± 5.52556",
            "unit": "ns",
            "extra": "100 samples\n758 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 59.2155,
            "range": "± 13.3423",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.82034,
            "range": "± 205.209",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.34815,
            "range": "± 237.318",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.47564,
            "range": "± 296.405",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.76909,
            "range": "± 1.4301",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.59252,
            "range": "± 526.539",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.1818,
            "range": "± 3.06444",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5876,
            "range": "± 1.79449",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.61548,
            "range": "± 1.14435",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.75143,
            "range": "± 503.763",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.8971,
            "range": "± 1.89074",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.58757,
            "range": "± 777.609",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.80775,
            "range": "± 765.304",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.84943,
            "range": "± 385.397",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.03,
            "range": "± 1.01531",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2092,
            "range": "± 1.09345",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.13883,
            "range": "± 477.945",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.4249,
            "range": "± 539.798",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1029,
            "range": "± 999.556",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.13927,
            "range": "± 688.373",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.0274,
            "range": "± 939.645",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.88416,
            "range": "± 340.64",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.2783,
            "range": "± 4.28306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.0347,
            "range": "± 6.54607",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.3445,
            "range": "± 28.8181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.35303,
            "range": "± 26.3633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 97.8771,
            "range": "± 5.11108",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.5309,
            "range": "± 6.65288",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42128,
            "range": "± 45.7309",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.23716,
            "range": "± 36.3171",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.54688,
            "range": "± 98.9757",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.14796,
            "range": "± 124.129",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.04752,
            "range": "± 100.192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.16989,
            "range": "± 149.283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.0528,
            "range": "± 1.06799",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.8227,
            "range": "± 129.536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.7187,
            "range": "± 157.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.01114,
            "range": "± 135.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.5224,
            "range": "± 176.831",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.44491,
            "range": "± 112.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.5141,
            "range": "± 115.026",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.5259,
            "range": "± 2.27598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.60589,
            "range": "± 142.093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.3674,
            "range": "± 178.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6885,
            "range": "± 299.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.3551,
            "range": "± 844.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.3222,
            "range": "± 344.862",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.4277,
            "range": "± 312.362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.1975,
            "range": "± 272.211",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5809,
            "range": "± 311.65",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.8141,
            "range": "± 268.565",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.0096,
            "range": "± 219.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.4409,
            "range": "± 228.608",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.1461,
            "range": "± 238.7",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.198,
            "range": "± 254.524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.7457,
            "range": "± 960.113",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.7329,
            "range": "± 404.922",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.7983,
            "range": "± 388.573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.9839,
            "range": "± 2.65367",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.7587,
            "range": "± 2.72219",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.0847,
            "range": "± 3.2539",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.8681,
            "range": "± 2.44581",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.6628,
            "range": "± 2.193",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.8337,
            "range": "± 2.81099",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.7609,
            "range": "± 2.76122",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.7326,
            "range": "± 2.52227",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.4931,
            "range": "± 3.69622",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.5392,
            "range": "± 2.97306",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 43.2861,
            "range": "± 5.78076",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 207.945,
            "range": "± 12.381",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 57.5564,
            "range": "± 10.1359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 324.055,
            "range": "± 17.0059",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.61852,
            "range": "± 106.374",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 3.52542,
            "range": "± 426.15",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 7.48681,
            "range": "± 726.641",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.42889,
            "range": "± 342.806",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 5.26324,
            "range": "± 587.29",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.5439,
            "range": "± 398.229",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 4.46793,
            "range": "± 516.284",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.02976,
            "range": "± 534.085",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.90257,
            "range": "± 481.132",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.01534,
            "range": "± 323.532",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 22.3191,
            "range": "± 2.90271",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 204.761,
            "range": "± 19.5146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 25.2406,
            "range": "± 2.28505",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 270.327,
            "range": "± 10.8153",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.0202,
            "range": "± 3.04063",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 31.8252,
            "range": "± 3.64304",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.6708,
            "range": "± 6.47561",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.2411,
            "range": "± 3.23182",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.4951,
            "range": "± 6.52288",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.2337,
            "range": "± 3.91509",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 37.7143,
            "range": "± 3.85156",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.5764,
            "range": "± 4.23055",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 40.7687,
            "range": "± 8.02283",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.0627,
            "range": "± 4.12336",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 76.6129,
            "range": "± 8.54029",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 490.917,
            "range": "± 12.4571",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 93.1523,
            "range": "± 8.94497",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 634.153,
            "range": "± 16.0755",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.8314,
            "range": "± 7.81744",
            "unit": "ns",
            "extra": "100 samples\n791 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 281.442,
            "range": "± 4.23429",
            "unit": "ns",
            "extra": "100 samples\n8292 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.91149,
            "range": "± 0.483347",
            "unit": "ns",
            "extra": "100 samples\n7775 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.3464,
            "range": "± 1.92543",
            "unit": "ns",
            "extra": "100 samples\n787 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d54f940a458972d61b45750b3341aabfd592564d",
          "message": "Milestone IR-0: verifier hardening, FunctionRewriter, pass retrofit, and pass-manager mechanics (#351)",
          "timestamp": "2026-07-03T12:14:13+02:00",
          "tree_id": "1dd712bc455c0da5e06d92afab22605510a5fe03",
          "url": "https://github.com/nebulastream/nautilus/commit/d54f940a458972d61b45750b3341aabfd592564d"
        },
        "date": 1783074295845,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.49764,
            "range": "± 353.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.29769,
            "range": "± 426.348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.98024,
            "range": "± 530.61",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.50544,
            "range": "± 555.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0133,
            "range": "± 853.32",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.3496,
            "range": "± 994.415",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5775,
            "range": "± 598.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.67826,
            "range": "± 189.611",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.5566,
            "range": "± 351.501",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.4175,
            "range": "± 355.844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.3117,
            "range": "± 225.144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.4316,
            "range": "± 302.358",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.6549,
            "range": "± 320.373",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1344,
            "range": "± 378.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 49.9294,
            "range": "± 17.5918",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.19974,
            "range": "± 169.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 49.044,
            "range": "± 18.6639",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.60541,
            "range": "± 126.528",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 29.6502,
            "range": "± 3.38272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 49.9997,
            "range": "± 17.3217",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 73.0426,
            "range": "± 18.9823",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.71292,
            "range": "± 169.644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.9761,
            "range": "± 414.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 72.2172,
            "range": "± 18.54",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.8584,
            "range": "± 2.21773",
            "unit": "ns",
            "extra": "100 samples\n833 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 307.738,
            "range": "± 7.97032",
            "unit": "ns",
            "extra": "100 samples\n8577 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.2279,
            "range": "± 0.379752",
            "unit": "ns",
            "extra": "100 samples\n7792 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.78,
            "range": "± 1.82194",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.3926,
            "range": "± 0.935269",
            "unit": "ns",
            "extra": "100 samples\n8789 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.6242,
            "range": "± 902.44",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 566.882,
            "range": "± 55.5535",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.61376,
            "range": "± 0.612994",
            "unit": "ns",
            "extra": "100 samples\n7228 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 108.651,
            "range": "± 7.38816",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5921,
            "range": "± 149.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.7469,
            "range": "± 4.69764",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 631.837,
            "range": "± 21.1004",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 145.726,
            "range": "± 179.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 29.7138,
            "range": "± 1.00016",
            "unit": "ns",
            "extra": "100 samples\n1036 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 177.49,
            "range": "± 4.76253",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 40.0542,
            "range": "± 129.852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.5492,
            "range": "± 0.2951",
            "unit": "ns",
            "extra": "100 samples\n8805 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.5589,
            "range": "± 1.98108",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.39447,
            "range": "± 58.4192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.002,
            "range": "± 4.71296",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 45.3647,
            "range": "± 4.38264",
            "unit": "ns",
            "extra": "100 samples\n707 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 641.399,
            "range": "± 24.357",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 641.297,
            "range": "± 21.1512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 145.99,
            "range": "± 1.14776",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 145.888,
            "range": "± 1.26108",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 44.7405,
            "range": "± 3.31081",
            "unit": "ns",
            "extra": "100 samples\n703 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 40.8951,
            "range": "± 3.4788",
            "unit": "ns",
            "extra": "100 samples\n738 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.9371,
            "range": "± 3.05607",
            "unit": "ns",
            "extra": "100 samples\n807 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 627.393,
            "range": "± 9.02339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 519.08,
            "range": "± 23.1508",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 618.814,
            "range": "± 8.51968",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 145.72,
            "range": "± 185.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 105.92,
            "range": "± 463.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 124.851,
            "range": "± 479.028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.4856,
            "range": "± 4.27905",
            "unit": "ns",
            "extra": "100 samples\n793 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.5183,
            "range": "± 3.58038",
            "unit": "ns",
            "extra": "100 samples\n887 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 621.795,
            "range": "± 14.5455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 619.977,
            "range": "± 10.8925",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 124.855,
            "range": "± 483.62",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 124.696,
            "range": "± 312.088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.7329,
            "range": "± 2.62767",
            "unit": "ns",
            "extra": "100 samples\n802 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.8149,
            "range": "± 2.52663",
            "unit": "ns",
            "extra": "100 samples\n795 iterations"
          },
          {
            "name": "ssa_add",
            "value": 167.339,
            "range": "± 8.40637",
            "unit": "ns",
            "extra": "100 samples\n182 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 419.123,
            "range": "± 26.3113",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.05538,
            "range": "± 59.3614",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 435.743,
            "range": "± 40.842",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 818.879,
            "range": "± 49.4556",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 444.309,
            "range": "± 23.2908",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 518.041,
            "range": "± 31.2787",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 382.27,
            "range": "± 22.8959",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 460.179,
            "range": "± 48.4145",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 400.371,
            "range": "± 28.6787",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.31004,
            "range": "± 259.981",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.3068,
            "range": "± 208.614",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.26649,
            "range": "± 1.40433",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.59402,
            "range": "± 585.982",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.5721,
            "range": "± 3.74871",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.6232,
            "range": "± 1.86003",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.22186,
            "range": "± 1.58533",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.67622,
            "range": "± 594.599",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.0086,
            "range": "± 3.95404",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.50783,
            "range": "± 1.25248",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.29053,
            "range": "± 1.61432",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.67243,
            "range": "± 571.469",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.8207,
            "range": "± 3.18355",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2786,
            "range": "± 1.85273",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.50986,
            "range": "± 929.813",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.57478,
            "range": "± 897.814",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.54894,
            "range": "± 1.69022",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.99298,
            "range": "± 777.629",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.50534,
            "range": "± 1.14091",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.8825,
            "range": "± 468.773",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.7415,
            "range": "± 8.18662",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.9135,
            "range": "± 7.47792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.44666,
            "range": "± 31.8459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.45417,
            "range": "± 33.6175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 94.8355,
            "range": "± 11.8633",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.3069,
            "range": "± 9.47159",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.42175,
            "range": "± 145.957",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.22811,
            "range": "± 42.5848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 732.877,
            "range": "± 59.4948",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.50249,
            "range": "± 103.544",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.43048,
            "range": "± 267.011",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.57639,
            "range": "± 96.6458",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.75377,
            "range": "± 167.546",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.75221,
            "range": "± 100.529",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.92818,
            "range": "± 134.873",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.42617,
            "range": "± 78.8784",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.67483,
            "range": "± 95.1284",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.43948,
            "range": "± 129.955",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.59514,
            "range": "± 397.842",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.3792,
            "range": "± 5.76481",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.6286,
            "range": "± 931.225",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 163.675,
            "range": "± 7.05055",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "1738f0307c5f7ee47fc574268ba62c81ed7e02bb",
          "message": "[IR passes] P0 cleanup passes: DCE, algebraic simplification, constant branch folding (#352)",
          "timestamp": "2026-07-04T21:46:13+02:00",
          "tree_id": "012d47d1f02fc6f4784c841791d900d801b60a2d",
          "url": "https://github.com/nebulastream/nautilus/commit/1738f0307c5f7ee47fc574268ba62c81ed7e02bb"
        },
        "date": 1783194847521,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 62.5214,
            "range": "± 14.0673",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.90034,
            "range": "± 202.31",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 53.0869,
            "range": "± 13.1141",
            "unit": "ns",
            "extra": "100 samples\n770 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 369.66,
            "range": "± 15.1934",
            "unit": "ns",
            "extra": "100 samples\n8226 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 5.50962,
            "range": "± 1.26562",
            "unit": "ns",
            "extra": "100 samples\n7719 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 41.3102,
            "range": "± 6.33698",
            "unit": "ns",
            "extra": "100 samples\n762 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.63276,
            "range": "± 514.451",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 3.13826,
            "range": "± 591.262",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.063,
            "range": "± 2.2823",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.15367,
            "range": "± 1.0275",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 28.8647,
            "range": "± 6.58348",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 17.052,
            "range": "± 3.66884",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.91685,
            "range": "± 2.38056",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.21866,
            "range": "± 1.04755",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 19.6239,
            "range": "± 5.0124",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.7603,
            "range": "± 3.84453",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.65474,
            "range": "± 1.96246",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.22901,
            "range": "± 989.9",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.3274,
            "range": "± 4.74224",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.8382,
            "range": "± 2.90745",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.82322,
            "range": "± 1.16848",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.81619,
            "range": "± 2.0004",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.8687,
            "range": "± 4.21684",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 8.39133,
            "range": "± 2.09071",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.59182,
            "range": "± 2.19934",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.79484,
            "range": "± 1.13506",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.2071,
            "range": "± 14.6842",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 56.0009,
            "range": "± 13.939",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.40153,
            "range": "± 89.9381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.45407,
            "range": "± 164.31",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 104.958,
            "range": "± 18.6762",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 59.3334,
            "range": "± 14.8536",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.53328,
            "range": "± 89.0751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.37007,
            "range": "± 170.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 13.8276,
            "range": "± 2.28154",
            "unit": "ns",
            "extra": "100 samples\n8440 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.0557,
            "range": "± 2.58482",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 579.562,
            "range": "± 27.3755",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.86633,
            "range": "± 1.00606",
            "unit": "ns",
            "extra": "100 samples\n6469 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.4466,
            "range": "± 9.82415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1225,
            "range": "± 588.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 50.499,
            "range": "± 10.1355",
            "unit": "ns",
            "extra": "100 samples\n598 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 771.254,
            "range": "± 26.2479",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 163.954,
            "range": "± 8.40533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 36.865,
            "range": "± 8.49872",
            "unit": "ns",
            "extra": "100 samples\n1135 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 165.205,
            "range": "± 13.4169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.56,
            "range": "± 109.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.76751,
            "range": "± 0.706567",
            "unit": "ns",
            "extra": "100 samples\n8466 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.4499,
            "range": "± 1.52674",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.04674,
            "range": "± 324.996",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 51.5069,
            "range": "± 12.6668",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 57.8712,
            "range": "± 17.4207",
            "unit": "ns",
            "extra": "100 samples\n647 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 770.859,
            "range": "± 23.1705",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 781.54,
            "range": "± 58.3193",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 161.717,
            "range": "± 10.2286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 164.82,
            "range": "± 8.51296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.6495,
            "range": "± 8.11926",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 45.4823,
            "range": "± 10.4528",
            "unit": "ns",
            "extra": "100 samples\n703 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 56.9988,
            "range": "± 13.2784",
            "unit": "ns",
            "extra": "100 samples\n704 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 773.012,
            "range": "± 52.8353",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 747.321,
            "range": "± 64.0866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 606.699,
            "range": "± 32.2254",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 163.706,
            "range": "± 10.2921",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 149.314,
            "range": "± 3.08865",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 117.251,
            "range": "± 1.16979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 57.7177,
            "range": "± 13.7225",
            "unit": "ns",
            "extra": "100 samples\n695 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 50.1668,
            "range": "± 9.55468",
            "unit": "ns",
            "extra": "100 samples\n630 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 619.703,
            "range": "± 46.7297",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 608.149,
            "range": "± 43.0801",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 117.396,
            "range": "± 1.09588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 117.101,
            "range": "± 714.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 44.4173,
            "range": "± 7.39818",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 54.8031,
            "range": "± 15.4841",
            "unit": "ns",
            "extra": "100 samples\n701 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 69.9466,
            "range": "± 20.069",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.91874,
            "range": "± 223.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 943.342,
            "range": "± 191.222",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.215,
            "range": "± 519.514",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.7639,
            "range": "± 1.21141",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.73222,
            "range": "± 257.937",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.1765,
            "range": "± 695.176",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.26826,
            "range": "± 490.911",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.27284,
            "range": "± 516.971",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.00841,
            "range": "± 500.304",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.0505,
            "range": "± 443.85",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.91767,
            "range": "± 507.238",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.96526,
            "range": "± 2.36711",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.2694,
            "range": "± 7.81564",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.8198,
            "range": "± 2.43289",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 184.113,
            "range": "± 12.3823",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 216.524,
            "range": "± 46.3204",
            "unit": "ns",
            "extra": "100 samples\n170 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 524.316,
            "range": "± 138.133",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.29742,
            "range": "± 297.653",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 579.135,
            "range": "± 132.934",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1134.02,
            "range": "± 282657",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 531.556,
            "range": "± 104.267",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 689.549,
            "range": "± 154.262",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 609.306,
            "range": "± 142.677",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 544.351,
            "range": "± 98.3058",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 646.749,
            "range": "± 141.603",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.58937,
            "range": "± 361.168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.83869,
            "range": "± 419.434",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 6.10126,
            "range": "± 328.618",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.32062,
            "range": "± 375.567",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.4104,
            "range": "± 509.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6173,
            "range": "± 1.10526",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.9092,
            "range": "± 794.652",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.96174,
            "range": "± 281.372",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.183,
            "range": "± 395.16",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7906,
            "range": "± 409.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.4362,
            "range": "± 490.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.1186,
            "range": "± 476.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.6215,
            "range": "± 541.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.76,
            "range": "± 637.872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.705,
            "range": "± 1.25301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 30.2974,
            "range": "± 1.30765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 31.319,
            "range": "± 1.23027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 30.1749,
            "range": "± 1.74207",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 31.8634,
            "range": "± 1.28643",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.5858,
            "range": "± 1.42149",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 31.6388,
            "range": "± 1.52841",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.8202,
            "range": "± 1.22693",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 30.1383,
            "range": "± 1.15956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.8792,
            "range": "± 1.03745",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 33.7586,
            "range": "± 1.44205",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 68.4324,
            "range": "± 2.12858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 37.9088,
            "range": "± 1.71589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 98.0764,
            "range": "± 2.99888",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.8434,
            "range": "± 2.83865",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.5494,
            "range": "± 3.84326",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.5713,
            "range": "± 5.18917",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.8209,
            "range": "± 4.53118",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.2838,
            "range": "± 4.46661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.4861,
            "range": "± 4.48045",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.6535,
            "range": "± 5.26769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.999,
            "range": "± 3.91809",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 20.7733,
            "range": "± 6.83741",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.285,
            "range": "± 4.42213",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 46.171,
            "range": "± 11.7214",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 236.903,
            "range": "± 57.0443",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 58.0852,
            "range": "± 11.8858",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 339.485,
            "range": "± 21.3298",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.91251,
            "range": "± 409.102",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.02976,
            "range": "± 1.07605",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 8.06377,
            "range": "± 1.65494",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.15832,
            "range": "± 1.03983",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 6.949,
            "range": "± 1.69556",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.80028,
            "range": "± 781.596",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 4.69827,
            "range": "± 948.582",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.03496,
            "range": "± 754.489",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.45555,
            "range": "± 1.19031",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.25392,
            "range": "± 644.526",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 23.2271,
            "range": "± 5.1939",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 214.135,
            "range": "± 14.0841",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 29.4156,
            "range": "± 7.23035",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 286.077,
            "range": "± 11.4367",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 24.9394,
            "range": "± 6.36166",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 35.185,
            "range": "± 8.29432",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 92.0318,
            "range": "± 46.9361",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 36.0292,
            "range": "± 8.00349",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 52.4258,
            "range": "± 16.6513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 37.6898,
            "range": "± 7.95684",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.0453,
            "range": "± 12.4971",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 34.4654,
            "range": "± 8.85676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 36.0231,
            "range": "± 7.55552",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.8838,
            "range": "± 7.57166",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 88.089,
            "range": "± 18.7649",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 519.5,
            "range": "± 24.292",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 96.6752,
            "range": "± 14.3761",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 665.799,
            "range": "± 27.1411",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d22e923ef0cd9ef884be7d7c675a2e9613290224",
          "message": "Make NautilusEngine thread-safe for concurrent compile() (#304)",
          "timestamp": "2026-07-04T22:02:03+02:00",
          "tree_id": "5f2ea214caf6db5099ccf36262d7af9cdfc61b0b",
          "url": "https://github.com/nebulastream/nautilus/commit/d22e923ef0cd9ef884be7d7c675a2e9613290224"
        },
        "date": 1783196027781,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3363.68,
            "range": "± 108395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.55008,
            "range": "± 152.51",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.55877,
            "range": "± 207.527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.11101,
            "range": "± 132.273",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.18414,
            "range": "± 276.079",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.51658,
            "range": "± 623.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.0117,
            "range": "± 567.643",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.3074,
            "range": "± 450.206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.5749,
            "range": "± 560.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.04658,
            "range": "± 187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.554,
            "range": "± 212.616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.53822,
            "range": "± 254.438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.6317,
            "range": "± 173.095",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.9618,
            "range": "± 609.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.61727,
            "range": "± 224.5",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.6277,
            "range": "± 610.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.5356,
            "range": "± 6.65747",
            "unit": "ns",
            "extra": "100 samples\n799 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 286.979,
            "range": "± 7.03424",
            "unit": "ns",
            "extra": "100 samples\n8277 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.99919,
            "range": "± 0.67157",
            "unit": "ns",
            "extra": "100 samples\n7742 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.4997,
            "range": "± 3.8448",
            "unit": "ns",
            "extra": "100 samples\n763 iterations"
          },
          {
            "name": "ir_add",
            "value": 773.701,
            "range": "± 87.7752",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.62377,
            "range": "± 165.753",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.49669,
            "range": "± 336.255",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.67038,
            "range": "± 190.196",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.82855,
            "range": "± 248.53",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.85909,
            "range": "± 319.722",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98773,
            "range": "± 153.799",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.50229,
            "range": "± 213.354",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74754,
            "range": "± 148.477",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.51707,
            "range": "± 154.531",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.76493,
            "range": "± 658.301",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.6727,
            "range": "± 5.09898",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.9169,
            "range": "± 1.24922",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 168.325,
            "range": "± 13.0802",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.35,
            "range": "± 139.768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.38902,
            "range": "± 191.455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.26779,
            "range": "± 101.114",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.7491,
            "range": "± 589.405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 67.1881,
            "range": "± 15.5916",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.41792,
            "range": "± 103.723",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.29379,
            "range": "± 66.8943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.30395,
            "range": "± 96.8436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.9221,
            "range": "± 706.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 100.268,
            "range": "± 14.1644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 194.841,
            "range": "± 29.1157",
            "unit": "ns",
            "extra": "100 samples\n163 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 458.985,
            "range": "± 47.3607",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.1691,
            "range": "± 124.798",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 496.791,
            "range": "± 59.4643",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 918.343,
            "range": "± 127.117",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 505.604,
            "range": "± 65.2984",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 607.713,
            "range": "± 76.1752",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 423.971,
            "range": "± 56.315",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.01,
            "range": "± 45.0223",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 469.354,
            "range": "± 52.5294",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.692,
            "range": "± 0.93174",
            "unit": "ns",
            "extra": "100 samples\n8440 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.6551,
            "range": "± 1.43418",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 532.812,
            "range": "± 24.6028",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.76258,
            "range": "± 0.948144",
            "unit": "ns",
            "extra": "100 samples\n6561 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.31,
            "range": "± 7.84561",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0637,
            "range": "± 997.047",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.985,
            "range": "± 8.39851",
            "unit": "ns",
            "extra": "100 samples\n635 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 930.559,
            "range": "± 36.9906",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 197.872,
            "range": "± 1.28926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.4293,
            "range": "± 4.73109",
            "unit": "ns",
            "extra": "100 samples\n1146 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 161.342,
            "range": "± 7.60276",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 38.6023,
            "range": "± 4.55049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.60933,
            "range": "± 0.669765",
            "unit": "ns",
            "extra": "100 samples\n8445 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.2671,
            "range": "± 1.22883",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.8155,
            "range": "± 37.3131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 49.6126,
            "range": "± 11.0575",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 48.7355,
            "range": "± 10.3759",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 925.349,
            "range": "± 15.7037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 922.986,
            "range": "± 15.7274",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 197.349,
            "range": "± 965.369",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 197.947,
            "range": "± 2.08898",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.2277,
            "range": "± 6.7013",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 42.8692,
            "range": "± 6.23507",
            "unit": "ns",
            "extra": "100 samples\n704 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.4498,
            "range": "± 4.82184",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 925.949,
            "range": "± 15.5411",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 652.621,
            "range": "± 24.1618",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 592.577,
            "range": "± 11.7808",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 197.54,
            "range": "± 332.344",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 134.279,
            "range": "± 3.75211",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 117.348,
            "range": "± 1.61162",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.5045,
            "range": "± 6.29321",
            "unit": "ns",
            "extra": "100 samples\n779 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 44.9687,
            "range": "± 3.83924",
            "unit": "ns",
            "extra": "100 samples\n660 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 596.707,
            "range": "± 12.1618",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 594.878,
            "range": "± 12.304",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 115.97,
            "range": "± 311.545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 118.442,
            "range": "± 4.28349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 42.9799,
            "range": "± 11.1112",
            "unit": "ns",
            "extra": "100 samples\n779 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 38.5101,
            "range": "± 2.85415",
            "unit": "ns",
            "extra": "100 samples\n754 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.4361,
            "range": "± 383.596",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.33379,
            "range": "± 196.399",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.87533,
            "range": "± 787.895",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.63992,
            "range": "± 492.951",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.8656,
            "range": "± 1.76598",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.9859,
            "range": "± 1.82125",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.58472,
            "range": "± 629.328",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.78429,
            "range": "± 506.871",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.1911,
            "range": "± 1.5593",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.81029,
            "range": "± 915.002",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.81567,
            "range": "± 748.8",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.96667,
            "range": "± 576.001",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.1844,
            "range": "± 1.97844",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.6025,
            "range": "± 1.18857",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.50515,
            "range": "± 691.492",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.76151,
            "range": "± 2.29201",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.4183,
            "range": "± 1.1842",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.28237,
            "range": "± 833.535",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.96117,
            "range": "± 725.292",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.12667,
            "range": "± 669.056",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.0312,
            "range": "± 5.22563",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.3058,
            "range": "± 4.49398",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.41511,
            "range": "± 67.9249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36834,
            "range": "± 30.8724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.8245,
            "range": "± 5.61981",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.094,
            "range": "± 7.04377",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43831,
            "range": "± 52.3221",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26939,
            "range": "± 58.0036",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "b4759e2e8ee6c6c38d4299398541adea7bcfb339",
          "message": "Remove legacy compiler strategy; tiered compiler is the only strategy (#353)",
          "timestamp": "2026-07-05T08:26:42+02:00",
          "tree_id": "5284abf2e4fcd3457a01164b82bde24d7fec2cc8",
          "url": "https://github.com/nebulastream/nautilus/commit/b4759e2e8ee6c6c38d4299398541adea7bcfb339"
        },
        "date": 1783233347389,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.7076,
            "range": "± 1.11799",
            "unit": "ns",
            "extra": "100 samples\n8448 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 12.8896,
            "range": "± 2.22827",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 527.779,
            "range": "± 17.9818",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.85334,
            "range": "± 1.03083",
            "unit": "ns",
            "extra": "100 samples\n6557 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.487,
            "range": "± 7.66858",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9863,
            "range": "± 554.186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.5291,
            "range": "± 5.37134",
            "unit": "ns",
            "extra": "100 samples\n641 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 762.993,
            "range": "± 13.6604",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 162.516,
            "range": "± 4.47719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 26.0712,
            "range": "± 1.87959",
            "unit": "ns",
            "extra": "100 samples\n1147 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 159.67,
            "range": "± 7.52869",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5529,
            "range": "± 122.706",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54125,
            "range": "± 0.62084",
            "unit": "ns",
            "extra": "100 samples\n8456 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 16.9275,
            "range": "± 8.63224",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80599,
            "range": "± 46.8",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.9774,
            "range": "± 3.38432",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.9066,
            "range": "± 6.23063",
            "unit": "ns",
            "extra": "100 samples\n641 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 759.746,
            "range": "± 8.93365",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 762.898,
            "range": "± 13.5773",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 161.378,
            "range": "± 4.18526",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 162.971,
            "range": "± 3.85568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 45.5471,
            "range": "± 0.451041",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 46.2065,
            "range": "± 7.23506",
            "unit": "ns",
            "extra": "100 samples\n651 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.2065,
            "range": "± 3.91947",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 759.482,
            "range": "± 12.0904",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 708.048,
            "range": "± 8.95824",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 939.499,
            "range": "± 17.4174",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 162.585,
            "range": "± 3.16207",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 147.714,
            "range": "± 4.50483",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 178.724,
            "range": "± 2.96568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.48,
            "range": "± 4.11314",
            "unit": "ns",
            "extra": "100 samples\n698 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.8367,
            "range": "± 5.53892",
            "unit": "ns",
            "extra": "100 samples\n867 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 955.435,
            "range": "± 18.8769",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 941.675,
            "range": "± 29.0218",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 178.142,
            "range": "± 2.68544",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 178.825,
            "range": "± 3.41926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.0111,
            "range": "± 3.85816",
            "unit": "ns",
            "extra": "100 samples\n718 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.2848,
            "range": "± 4.57281",
            "unit": "ns",
            "extra": "100 samples\n736 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.3923,
            "range": "± 5.83526",
            "unit": "ns",
            "extra": "100 samples\n767 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 280.947,
            "range": "± 6.0255",
            "unit": "ns",
            "extra": "100 samples\n8296 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.98195,
            "range": "± 0.581262",
            "unit": "ns",
            "extra": "100 samples\n7748 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.3337,
            "range": "± 7.23226",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "ssa_add",
            "value": 311.023,
            "range": "± 72.7092",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 439.255,
            "range": "± 22.0367",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.14169,
            "range": "± 80.9983",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 474.395,
            "range": "± 30.0711",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 880.196,
            "range": "± 46.011",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 504.194,
            "range": "± 42.9489",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 574.346,
            "range": "± 37.1945",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 468.037,
            "range": "± 24.214",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 490.701,
            "range": "± 31.897",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 436.958,
            "range": "± 41.899",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.29861,
            "range": "± 93.5449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.24833,
            "range": "± 113.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.23763,
            "range": "± 84.9293",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.5316,
            "range": "± 384.536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 69.9264,
            "range": "± 15.2012",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.34801,
            "range": "± 100.224",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.2694,
            "range": "± 77.6845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.27033,
            "range": "± 115.881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.4037,
            "range": "± 335.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 102.292,
            "range": "± 12.4754",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.71264,
            "range": "± 1.01507",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.0572,
            "range": "± 119.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.99987,
            "range": "± 148.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.09254,
            "range": "± 151.99",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.7177,
            "range": "± 811.011",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.7718,
            "range": "± 205.428",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.6952,
            "range": "± 268.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.96692,
            "range": "± 99.1416",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.3689,
            "range": "± 154.962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.42575,
            "range": "± 181.876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.483,
            "range": "± 157.989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.7091,
            "range": "± 175.489",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.5027,
            "range": "± 179.869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.0837,
            "range": "± 353.597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.5094,
            "range": "± 260.814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.0878,
            "range": "± 318.579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.208,
            "range": "± 318.575",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3167,
            "range": "± 313.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2739,
            "range": "± 464.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5531,
            "range": "± 589.139",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.7276,
            "range": "± 272.203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.9884,
            "range": "± 320.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.3404,
            "range": "± 248.941",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.1264,
            "range": "± 605.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.0933,
            "range": "± 347.413",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.5879,
            "range": "± 630.486",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.4716,
            "range": "± 329.563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.1012,
            "range": "± 657.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.8744,
            "range": "± 2.38472",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.8043,
            "range": "± 2.97657",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.9664,
            "range": "± 3.0755",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.9087,
            "range": "± 3.06803",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.0001,
            "range": "± 4.42768",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.0512,
            "range": "± 2.18419",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.5647,
            "range": "± 4.24443",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.7136,
            "range": "± 3.48755",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.179,
            "range": "± 2.5635",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.6919,
            "range": "± 2.94994",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 43.6569,
            "range": "± 5.80549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 209.554,
            "range": "± 11.026",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 53.2383,
            "range": "± 8.13672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 320.329,
            "range": "± 15.8013",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.56232,
            "range": "± 147.214",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 3.41265,
            "range": "± 494",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 7.48979,
            "range": "± 986.426",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.26428,
            "range": "± 279.265",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 5.37654,
            "range": "± 491.974",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.55168,
            "range": "± 398.715",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 4.23135,
            "range": "± 441.293",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 3.8358,
            "range": "± 361.175",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.59177,
            "range": "± 279.956",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.89822,
            "range": "± 288.216",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 22.086,
            "range": "± 2.45673",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 209.958,
            "range": "± 11.2016",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 25.8193,
            "range": "± 2.44833",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 277.034,
            "range": "± 11.9322",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.7686,
            "range": "± 5.10341",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.371,
            "range": "± 4.76811",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 52.9867,
            "range": "± 8.77793",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.337,
            "range": "± 3.94287",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.7044,
            "range": "± 5.93047",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.7206,
            "range": "± 4.3607",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.0416,
            "range": "± 7.71356",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.5487,
            "range": "± 3.14882",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.8757,
            "range": "± 5.20513",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.5231,
            "range": "± 4.73951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 77.6307,
            "range": "± 11.3356",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 513.128,
            "range": "± 19.489",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 94.6043,
            "range": "± 10.3225",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 685.538,
            "range": "± 167.344",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 731.941,
            "range": "± 46.6229",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.55083,
            "range": "± 108.751",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.42739,
            "range": "± 203.522",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.63044,
            "range": "± 153.3",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.78717,
            "range": "± 183.11",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.78898,
            "range": "± 139.405",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.9546,
            "range": "± 184.087",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.54195,
            "range": "± 133.665",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.7112,
            "range": "± 125.525",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.50824,
            "range": "± 206.629",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.69234,
            "range": "± 404.146",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 91.9531,
            "range": "± 6.03389",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.7942,
            "range": "± 1.01696",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.396,
            "range": "± 11.6601",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3309.1400000000003,
            "range": "± 80961",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.4858,
            "range": "± 103.825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.20829,
            "range": "± 135.881",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.2164,
            "range": "± 162.812",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.66672,
            "range": "± 901.479",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.50145,
            "range": "± 324.517",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.7175,
            "range": "± 1.64694",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5515,
            "range": "± 1.8138",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.60159,
            "range": "± 868.891",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.59703,
            "range": "± 341.157",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.197,
            "range": "± 3.09951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.27354,
            "range": "± 796.155",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.75174,
            "range": "± 868.684",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.71571,
            "range": "± 428.559",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.1267,
            "range": "± 2.11209",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.9674,
            "range": "± 1.44116",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.11777,
            "range": "± 682.64",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.19875,
            "range": "± 675.454",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.0482,
            "range": "± 1.68195",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.95464,
            "range": "± 714.963",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.86883,
            "range": "± 1.21056",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.82781,
            "range": "± 517.373",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 37.8778,
            "range": "± 5.07246",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.2354,
            "range": "± 6.0645",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35793,
            "range": "± 25.0716",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.37684,
            "range": "± 39.5806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.8731,
            "range": "± 9.31678",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.4031,
            "range": "± 7.29682",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.44752,
            "range": "± 40.5285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.34579,
            "range": "± 32.208",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "c77f87c9b79b37c69a74792d2806719976592885",
          "message": "[IR passes] P1 CFG passes: block merging + block-argument pruning (#362)",
          "timestamp": "2026-07-05T18:05:04+02:00",
          "tree_id": "179221b01aafd22a4150c3bf79ecd2898a34b997",
          "url": "https://github.com/nebulastream/nautilus/commit/c77f87c9b79b37c69a74792d2806719976592885"
        },
        "date": 1783267932433,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 544.697,
            "range": "± 32.6585",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.17054,
            "range": "± 61.2809",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 2.53386,
            "range": "± 244.081",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.23367,
            "range": "± 69.7823",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.05919,
            "range": "± 135.605",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.33041,
            "range": "± 76.8637",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.47364,
            "range": "± 97.5119",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.06686,
            "range": "± 60.5458",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.34206,
            "range": "± 77.1837",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.11189,
            "range": "± 75.5401",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 5.53328,
            "range": "± 457.836",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 73.3246,
            "range": "± 2.45277",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 8.81635,
            "range": "± 608.623",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 133.984,
            "range": "± 8.4063",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 131.055,
            "range": "± 10.0773",
            "unit": "ns",
            "extra": "100 samples\n180 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 319.584,
            "range": "± 30.8723",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.801327,
            "range": "± 0.0639314",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 335.367,
            "range": "± 20.0398",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 606.505,
            "range": "± 37.2646",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 340.995,
            "range": "± 34.1262",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 416.312,
            "range": "± 30.8972",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 328.786,
            "range": "± 16.7152",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 349.17,
            "range": "± 18.388",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 313.546,
            "range": "± 17.1398",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.58303,
            "range": "± 134.015",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.58863,
            "range": "± 160.319",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 6.03173,
            "range": "± 916.193",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.21705,
            "range": "± 381.625",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 18.2775,
            "range": "± 2.93607",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 8.95125,
            "range": "± 1.37051",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 6.02095,
            "range": "± 878.967",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.29182,
            "range": "± 380.726",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 11.9032,
            "range": "± 2.05836",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 5.91545,
            "range": "± 691.673",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 6.15238,
            "range": "± 1.19872",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.37173,
            "range": "± 428.14",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 9.82976,
            "range": "± 1.88369",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 7.04545,
            "range": "± 1.03135",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 5.45806,
            "range": "± 609.578",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 5.47379,
            "range": "± 532.988",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 7.01647,
            "range": "± 1.29932",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.26531,
            "range": "± 483.147",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 5.59145,
            "range": "± 961.009",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 2.7455,
            "range": "± 317.541",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 28.2394,
            "range": "± 6.03547",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 28.2011,
            "range": "± 5.75396",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.11439,
            "range": "± 29.727",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.12568,
            "range": "± 28.681",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 72.575,
            "range": "± 9.83129",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 36.3767,
            "range": "± 7.17902",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.57559,
            "range": "± 52.6773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 1.77397,
            "range": "± 30.4776",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.42559,
            "range": "± 206.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.92141,
            "range": "± 175.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.86104,
            "range": "± 259.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.39302,
            "range": "± 404.079",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 27.6279,
            "range": "± 588.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.7677,
            "range": "± 1.2339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 26.5317,
            "range": "± 734.097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.17018,
            "range": "± 228.223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.3438,
            "range": "± 587.587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.82077,
            "range": "± 231.434",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.3337,
            "range": "± 331.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 21.9562,
            "range": "± 367.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.6753,
            "range": "± 203.613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 49.9327,
            "range": "± 254.327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 31.9994,
            "range": "± 21.334",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.0252,
            "range": "± 9.40851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.9654,
            "range": "± 4.11825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 43.8678,
            "range": "± 45.9557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 38.254,
            "range": "± 42.4901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.5869,
            "range": "± 2.613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 36.0232,
            "range": "± 30.2825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 38.4859,
            "range": "± 33.4551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.0849,
            "range": "± 8.77846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 43.8237,
            "range": "± 59.0502",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 32.8022,
            "range": "± 19.2867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 54.5783,
            "range": "± 7.25872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 34.3796,
            "range": "± 17.6941",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 80.7363,
            "range": "± 18.2403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.5605,
            "range": "± 2.16913",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.9803,
            "range": "± 4.03917",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 17.2164,
            "range": "± 4.37986",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 14.2682,
            "range": "± 3.88592",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 16.2306,
            "range": "± 3.47364",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 14.3805,
            "range": "± 3.67721",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 14.877,
            "range": "± 3.67481",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 13.8947,
            "range": "± 3.45336",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 14.3133,
            "range": "± 3.58077",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 13.5755,
            "range": "± 3.21604",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.4849,
            "range": "± 8.34475",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 221.574,
            "range": "± 11.4296",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 43.1529,
            "range": "± 9.23572",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 356.32,
            "range": "± 11.7462",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.34997,
            "range": "± 126.896",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 3.01402,
            "range": "± 277.698",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 6.81623,
            "range": "± 734.206",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.09836,
            "range": "± 402.355",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 5.28741,
            "range": "± 807.202",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.2586,
            "range": "± 376.569",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 3.96489,
            "range": "± 352.413",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 3.26973,
            "range": "± 266.01",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.37791,
            "range": "± 307.768",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.77696,
            "range": "± 267.651",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 19.6829,
            "range": "± 2.39726",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 218.546,
            "range": "± 6.56052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 25.1972,
            "range": "± 2.42257",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 316.247,
            "range": "± 5.8143",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 14.6102,
            "range": "± 4.87387",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 20.0117,
            "range": "± 6.67379",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 27.2575,
            "range": "± 5.49268",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 20.328,
            "range": "± 7.46623",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 26.3739,
            "range": "± 5.87347",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 22.6198,
            "range": "± 5.24203",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 23.569,
            "range": "± 5.27673",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 18.6564,
            "range": "± 4.62079",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 21.621,
            "range": "± 5.15412",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 20.7631,
            "range": "± 5.28471",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 53.7106,
            "range": "± 10.4745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 395.03,
            "range": "± 17.7309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 70.6702,
            "range": "± 10.6707",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 524.073,
            "range": "± 35.6813",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 29.7965,
            "range": "± 2.8601",
            "unit": "ns",
            "extra": "100 samples\n812 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 242.708,
            "range": "± 9.25746",
            "unit": "ns",
            "extra": "100 samples\n8689 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.24434,
            "range": "± 0.0361672",
            "unit": "ns",
            "extra": "100 samples\n7727 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 31.2625,
            "range": "± 1.88315",
            "unit": "ns",
            "extra": "100 samples\n773 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3038.6099999999997,
            "range": "± 115325",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.00321,
            "range": "± 182.815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.00607,
            "range": "± 103.905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 2.95509,
            "range": "± 141.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.01928,
            "range": "± 197.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 35.7056,
            "range": "± 33.9374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 47.0783,
            "range": "± 17.044",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 4.70066,
            "range": "± 159.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 4.58143,
            "range": "± 177.305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 4.59613,
            "range": "± 162.917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 33.6045,
            "range": "± 63.0199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 90.6906,
            "range": "± 20.1681",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.33509,
            "range": "± 0.552752",
            "unit": "ns",
            "extra": "100 samples\n8807 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.1358,
            "range": "± 1.16593",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 164.785,
            "range": "± 17.4339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.54975,
            "range": "± 0.440586",
            "unit": "ns",
            "extra": "100 samples\n7217 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 42.9763,
            "range": "± 4.64999",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 9.43834,
            "range": "± 291.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 35.7586,
            "range": "± 3.09087",
            "unit": "ns",
            "extra": "100 samples\n644 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 296.559,
            "range": "± 11.2219",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 67.2099,
            "range": "± 2.24774",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 23.4187,
            "range": "± 1.30281",
            "unit": "ns",
            "extra": "100 samples\n1044 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 117.484,
            "range": "± 4.08132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 31.2191,
            "range": "± 442.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.78392,
            "range": "± 0.277068",
            "unit": "ns",
            "extra": "100 samples\n8811 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 16.6057,
            "range": "± 2.34349",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.1165,
            "range": "± 76.4577",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 35.3732,
            "range": "± 1.66887",
            "unit": "ns",
            "extra": "100 samples\n673 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 35.2355,
            "range": "± 1.4212",
            "unit": "ns",
            "extra": "100 samples\n688 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 313.25,
            "range": "± 13.3391",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 294.383,
            "range": "± 9.26407",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 68.8861,
            "range": "± 978.152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 67.6905,
            "range": "± 2.98631",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 35.8686,
            "range": "± 4.73158",
            "unit": "ns",
            "extra": "100 samples\n681 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 33.2338,
            "range": "± 3.73726",
            "unit": "ns",
            "extra": "100 samples\n729 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 31.3307,
            "range": "± 1.45631",
            "unit": "ns",
            "extra": "100 samples\n771 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 296.185,
            "range": "± 12.9825",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 248.593,
            "range": "± 7.15431",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 365.251,
            "range": "± 9.20508",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 67.8783,
            "range": "± 2.98872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 55.045,
            "range": "± 3.42826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 73.7945,
            "range": "± 1.25743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 33.053,
            "range": "± 4.5329",
            "unit": "ns",
            "extra": "100 samples\n784 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 27.517,
            "range": "± 1.18042",
            "unit": "ns",
            "extra": "100 samples\n885 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 365.113,
            "range": "± 8.18449",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 364.333,
            "range": "± 7.26901",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 73.9094,
            "range": "± 1.41576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 73.6102,
            "range": "± 369.613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 31.6263,
            "range": "± 2.73837",
            "unit": "ns",
            "extra": "100 samples\n782 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 31.5918,
            "range": "± 4.56751",
            "unit": "ns",
            "extra": "100 samples\n795 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "21b44c4f0ffb4f29c20f0ba8cb25147def917f1a",
          "message": "[fuzz] Add data-dependent While loops with break/continue (#365)",
          "timestamp": "2026-07-06T20:50:54+02:00",
          "tree_id": "83781f5d5a9dc03ce0b1d5b812c80a56d0d21402",
          "url": "https://github.com/nebulastream/nautilus/commit/21b44c4f0ffb4f29c20f0ba8cb25147def917f1a"
        },
        "date": 1783364218059,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.5907,
            "range": "± 124.254",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.6118,
            "range": "± 113.558",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 5.97744,
            "range": "± 1.01435",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.22892,
            "range": "± 342.877",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 18.0451,
            "range": "± 2.57436",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 8.98709,
            "range": "± 1.03025",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 5.91992,
            "range": "± 1.03335",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.31555,
            "range": "± 354.57",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 12.786,
            "range": "± 2.85752",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 5.97063,
            "range": "± 692.586",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 6.07194,
            "range": "± 914.071",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.34933,
            "range": "± 365.692",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 9.57268,
            "range": "± 1.86503",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 6.90873,
            "range": "± 833.353",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 5.60942,
            "range": "± 796.767",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 5.4893,
            "range": "± 542.346",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 7.09168,
            "range": "± 1.73285",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.27872,
            "range": "± 535.569",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 5.47351,
            "range": "± 926.233",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 2.75097,
            "range": "± 280.944",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 27.6305,
            "range": "± 4.64591",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 27.9385,
            "range": "± 5.01187",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.11595,
            "range": "± 15.9509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.13112,
            "range": "± 22.0083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 71.9593,
            "range": "± 10.2388",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 36.5883,
            "range": "± 7.20809",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.58872,
            "range": "± 62.2515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 1.71068,
            "range": "± 39.0436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 130.239,
            "range": "± 6.30023",
            "unit": "ns",
            "extra": "100 samples\n185 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 325.037,
            "range": "± 32.4343",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.809486,
            "range": "± 0.0524702",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 334.619,
            "range": "± 20.0482",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 607.98,
            "range": "± 38.7025",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 340.708,
            "range": "± 18.1659",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 413.903,
            "range": "± 24.8715",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 332.638,
            "range": "± 15.6876",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 356.214,
            "range": "± 30.022",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 321.856,
            "range": "± 24.5342",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3008.73,
            "range": "± 102828",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.11486,
            "range": "± 172.918",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.37357,
            "range": "± 0.507714",
            "unit": "ns",
            "extra": "100 samples\n8812 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 12.7434,
            "range": "± 1.3448",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 513.212,
            "range": "± 93.1962",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.65015,
            "range": "± 0.603381",
            "unit": "ns",
            "extra": "100 samples\n7206 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 43.1312,
            "range": "± 4.14174",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 9.63986,
            "range": "± 32.6343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 35.7169,
            "range": "± 2.30562",
            "unit": "ns",
            "extra": "100 samples\n689 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 295.345,
            "range": "± 10.2156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 66.8234,
            "range": "± 1.82758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 23.5126,
            "range": "± 1.5497",
            "unit": "ns",
            "extra": "100 samples\n1049 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 117.523,
            "range": "± 3.44653",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 31.1413,
            "range": "± 687.398",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.76716,
            "range": "± 0.291683",
            "unit": "ns",
            "extra": "100 samples\n8817 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.6434,
            "range": "± 1.51087",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.59022,
            "range": "± 29.2454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 36.3845,
            "range": "± 4.74491",
            "unit": "ns",
            "extra": "100 samples\n678 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 35.7076,
            "range": "± 2.2681",
            "unit": "ns",
            "extra": "100 samples\n681 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 324.685,
            "range": "± 13.4987",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 315.913,
            "range": "± 21.9303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 67.8977,
            "range": "± 1.04525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 66.7653,
            "range": "± 984.313",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 35.0793,
            "range": "± 0.487798",
            "unit": "ns",
            "extra": "100 samples\n686 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 32.5938,
            "range": "± 3.16085",
            "unit": "ns",
            "extra": "100 samples\n750 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 31.2596,
            "range": "± 0.607629",
            "unit": "ns",
            "extra": "100 samples\n754 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 309.314,
            "range": "± 11.3643",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 247.222,
            "range": "± 6.26452",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 366.382,
            "range": "± 8.28553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 66.6784,
            "range": "± 623.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 54.2359,
            "range": "± 1.52046",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 76.9008,
            "range": "± 8.34161",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 31.7075,
            "range": "± 3.01643",
            "unit": "ns",
            "extra": "100 samples\n790 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 27.6,
            "range": "± 2.48052",
            "unit": "ns",
            "extra": "100 samples\n891 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 367.226,
            "range": "± 10.8474",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 366.592,
            "range": "± 8.23325",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 78.3294,
            "range": "± 11.191",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 74.0108,
            "range": "± 1.54564",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 30.9621,
            "range": "± 2.64864",
            "unit": "ns",
            "extra": "100 samples\n769 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 30.8278,
            "range": "± 1.26895",
            "unit": "ns",
            "extra": "100 samples\n794 iterations"
          },
          {
            "name": "ir_add",
            "value": 605.305,
            "range": "± 32.8277",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.19976,
            "range": "± 64.9818",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 2.6139,
            "range": "± 200.233",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.25918,
            "range": "± 75.2581",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.18689,
            "range": "± 149.5",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.40097,
            "range": "± 54.6253",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.52975,
            "range": "± 65.6065",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.14125,
            "range": "± 72.1235",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.34817,
            "range": "± 90.7165",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.16335,
            "range": "± 69.2547",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 5.99631,
            "range": "± 376.05",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 70.7125,
            "range": "± 4.58043",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 9.39621,
            "range": "± 945.111",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 127.119,
            "range": "± 1.84111",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 29.5877,
            "range": "± 2.12598",
            "unit": "ns",
            "extra": "100 samples\n816 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 237.8,
            "range": "± 5.70512",
            "unit": "ns",
            "extra": "100 samples\n8693 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.26621,
            "range": "± 0.35225",
            "unit": "ns",
            "extra": "100 samples\n7791 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 31.3922,
            "range": "± 1.34227",
            "unit": "ns",
            "extra": "100 samples\n741 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 2.97179,
            "range": "± 101.867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 2.90033,
            "range": "± 251.997",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 2.87287,
            "range": "± 111.431",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 30.8995,
            "range": "± 34.5532",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 44.8788,
            "range": "± 14.9992",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 4.66752,
            "range": "± 210.976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 4.62309,
            "range": "± 496.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 4.52097,
            "range": "± 134.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 38.7819,
            "range": "± 55.65",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 88.7963,
            "range": "± 23.4801",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 4.97946,
            "range": "± 164.579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 5.51511,
            "range": "± 270.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.5571,
            "range": "± 153.115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.21274,
            "range": "± 203.235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.6773,
            "range": "± 213.839",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 9.86362,
            "range": "± 167.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.4404,
            "range": "± 379.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.49787,
            "range": "± 152.806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 8.87789,
            "range": "± 243.095",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 7.97,
            "range": "± 231.033",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 8.82703,
            "range": "± 164.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 19.9089,
            "range": "± 143.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.90729,
            "range": "± 266.109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 47.6099,
            "range": "± 287.555",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.6882,
            "range": "± 25.3347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 38.7057,
            "range": "± 44.4982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 39.8311,
            "range": "± 41.1663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 31.8718,
            "range": "± 37.8137",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.1105,
            "range": "± 26.388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 41.0147,
            "range": "± 45.6355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.0086,
            "range": "± 22.573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 35.5361,
            "range": "± 38.0644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 37.2336,
            "range": "± 44.9463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.229,
            "range": "± 35.3016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.2195,
            "range": "± 23.0254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 55.7043,
            "range": "± 21.3919",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 26.1215,
            "range": "± 5.27525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 83.8413,
            "range": "± 32.719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.4909,
            "range": "± 2.16822",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 14.3742,
            "range": "± 4.61295",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 17.4219,
            "range": "± 3.55772",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 14.0494,
            "range": "± 2.8291",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 16.0576,
            "range": "± 3.4577",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 14.1624,
            "range": "± 3.3381",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 15.0174,
            "range": "± 3.80358",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 13.5877,
            "range": "± 3.28976",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 14.5803,
            "range": "± 4.68535",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 13.3796,
            "range": "± 3.22387",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.5807,
            "range": "± 8.91668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 216.142,
            "range": "± 8.2043",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.865,
            "range": "± 9.3078",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 354.186,
            "range": "± 10.5244",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.33573,
            "range": "± 106.663",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 2.98214,
            "range": "± 288.63",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 6.67848,
            "range": "± 740.796",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.06396,
            "range": "± 278.031",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 5.09296,
            "range": "± 485.435",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.26449,
            "range": "± 364.139",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 3.97469,
            "range": "± 398.144",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 3.24983,
            "range": "± 273.404",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.35085,
            "range": "± 317.046",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.77197,
            "range": "± 270.972",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 19.4233,
            "range": "± 2.13902",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 213.052,
            "range": "± 5.40569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 24.4795,
            "range": "± 2.37544",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 312.186,
            "range": "± 7.58838",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 14.4854,
            "range": "± 4.52074",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 19.4237,
            "range": "± 5.00611",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 26.6686,
            "range": "± 6.18506",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 19.8696,
            "range": "± 4.96498",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 25.5749,
            "range": "± 5.28659",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 22.1713,
            "range": "± 4.38484",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 23.4566,
            "range": "± 5.77901",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 18.3676,
            "range": "± 4.6339",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 21.0958,
            "range": "± 5.23012",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 20.1448,
            "range": "± 4.82814",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 51.8987,
            "range": "± 12.3729",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 386.864,
            "range": "± 11.5865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 67.4225,
            "range": "± 9.75068",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 516.412,
            "range": "± 11.3838",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}