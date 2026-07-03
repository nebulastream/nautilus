window.BENCHMARK_DATA = {
  "lastUpdate": 1783055078830,
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
          "id": "013ed9107c2d292a434c45836d2550331bc4d682",
          "message": "Bump version to 0.3 after v0.2 release (#305)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-06-20T14:08:12+02:00",
          "tree_id": "d03174306c422abaa75e11435d9240ecdc4b6942",
          "url": "https://github.com/nebulastream/nautilus/commit/013ed9107c2d292a434c45836d2550331bc4d682"
        },
        "date": 1781957595016,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.16765,
            "range": "± 149.394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.7649,
            "range": "± 161.588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.63899,
            "range": "± 135.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.82086,
            "range": "± 164.588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.6391,
            "range": "± 456.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.6944,
            "range": "± 198.517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.49,
            "range": "± 218.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.58947,
            "range": "± 140.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.4931,
            "range": "± 216.296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0821,
            "range": "± 201.495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.1799,
            "range": "± 225.91",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.3497,
            "range": "± 454.388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3845,
            "range": "± 195.386",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.7717,
            "range": "± 1.02909",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 168.256,
            "range": "± 8.17097",
            "unit": "ns",
            "extra": "100 samples\n185 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 414.945,
            "range": "± 26.0965",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07111,
            "range": "± 67.0994",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 443.267,
            "range": "± 24.8821",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 826.865,
            "range": "± 53.0843",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 452.357,
            "range": "± 48.0047",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 542.433,
            "range": "± 36.8721",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 428.229,
            "range": "± 25.8943",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 455.75,
            "range": "± 26.5553",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 418.993,
            "range": "± 50.3156",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.9781,
            "range": "± 3.4156",
            "unit": "ns",
            "extra": "100 samples\n862 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 270.103,
            "range": "± 6.11525",
            "unit": "ns",
            "extra": "100 samples\n9537 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.75006,
            "range": "± 0.287257",
            "unit": "ns",
            "extra": "100 samples\n8640 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.9421,
            "range": "± 2.54811",
            "unit": "ns",
            "extra": "100 samples\n761 iterations"
          },
          {
            "name": "ir_add",
            "value": 754.238,
            "range": "± 45.5396",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.57857,
            "range": "± 95.4948",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.33397,
            "range": "± 214.533",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.56878,
            "range": "± 76.111",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.81167,
            "range": "± 139.194",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.76516,
            "range": "± 105.88",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.95365,
            "range": "± 135.996",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.40907,
            "range": "± 112.838",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.71149,
            "range": "± 170.216",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.46696,
            "range": "± 178.841",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.59813,
            "range": "± 434.475",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.5851,
            "range": "± 5.31119",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.7861,
            "range": "± 1.22781",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 166.727,
            "range": "± 7.3898",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 46.4436,
            "range": "± 15.9647",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.12395,
            "range": "± 175.851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.28791,
            "range": "± 278.385",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.30121,
            "range": "± 210.466",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.30843,
            "range": "± 1.42958",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.59653,
            "range": "± 708.934",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.5385,
            "range": "± 3.58316",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.882,
            "range": "± 1.78403",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.20928,
            "range": "± 1.44233",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.7136,
            "range": "± 577.607",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.803,
            "range": "± 2.3402",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.48083,
            "range": "± 1.21778",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.43488,
            "range": "± 1.52008",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.74269,
            "range": "± 595.408",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.722,
            "range": "± 2.62243",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.1766,
            "range": "± 1.29965",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.46237,
            "range": "± 816.265",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.42981,
            "range": "± 782.063",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.4177,
            "range": "± 2.55004",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.02859,
            "range": "± 722.184",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.58999,
            "range": "± 1.13145",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.88341,
            "range": "± 447.448",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.5505,
            "range": "± 9.54541",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.1441,
            "range": "± 8.29455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.46261,
            "range": "± 31.3839",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.47577,
            "range": "± 31.2672",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 93.7639,
            "range": "± 11.012",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.2736,
            "range": "± 8.93757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.6173,
            "range": "± 46.4157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.31866,
            "range": "± 41.5203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 45.4829,
            "range": "± 16.4812",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.60087,
            "range": "± 126.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.8197,
            "range": "± 369.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.1411,
            "range": "± 16.7698",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.5735,
            "range": "± 17.4255",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.73982,
            "range": "± 206.115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.4501,
            "range": "± 315.324",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 64.872,
            "range": "± 18.2278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.8438,
            "range": "± 0.803267",
            "unit": "ns",
            "extra": "100 samples\n8807 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.2957,
            "range": "± 1.27105",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 197.047,
            "range": "± 13.3443",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.53378,
            "range": "± 0.519937",
            "unit": "ns",
            "extra": "100 samples\n7233 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 108.029,
            "range": "± 5.53386",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.2326,
            "range": "± 52.5702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.0712,
            "range": "± 3.619",
            "unit": "ns",
            "extra": "100 samples\n713 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 685.965,
            "range": "± 10.8413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 157.795,
            "range": "± 6.95821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.61738,
            "range": "± 0.550624",
            "unit": "ns",
            "extra": "100 samples\n8815 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.4928,
            "range": "± 2.39676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.96118,
            "range": "± 28.1781",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.4112,
            "range": "± 2.97254",
            "unit": "ns",
            "extra": "100 samples\n720 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.5432,
            "range": "± 5.34069",
            "unit": "ns",
            "extra": "100 samples\n723 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 687.104,
            "range": "± 13.1411",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 684.099,
            "range": "± 7.89781",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 156.879,
            "range": "± 984.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 159.334,
            "range": "± 8.56067",
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
          "id": "d99fad192a222c7296e351a8d437664fc9bb5310",
          "message": "Add compress-store SIMD operation (#307)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-06-20T19:22:56+02:00",
          "tree_id": "ff423c5b7c9f8ff71ea59c479182b4066863e3c8",
          "url": "https://github.com/nebulastream/nautilus/commit/d99fad192a222c7296e351a8d437664fc9bb5310"
        },
        "date": 1781976492942,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 669.712,
            "range": "± 43.7549",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.40758,
            "range": "± 71.8631",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.18256,
            "range": "± 361.062",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.50353,
            "range": "± 91.3342",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.56251,
            "range": "± 181.905",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.63338,
            "range": "± 106.289",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.85857,
            "range": "± 154.678",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.32672,
            "range": "± 51.8623",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.58397,
            "range": "± 79.0076",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.38565,
            "range": "± 84.9385",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.20673,
            "range": "± 524.23",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 96.2737,
            "range": "± 12.4724",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.0646,
            "range": "± 628.111",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 166.801,
            "range": "± 19.6883",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.3307,
            "range": "± 14.5371",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.59865,
            "range": "± 119.278",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 172.898,
            "range": "± 10.8449",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 424.851,
            "range": "± 42.8312",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07624,
            "range": "± 64.9914",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 442.946,
            "range": "± 25.8672",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 843.826,
            "range": "± 70.3128",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 451.429,
            "range": "± 29.4117",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 553.51,
            "range": "± 32.7485",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 437.011,
            "range": "± 21.322",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 457.071,
            "range": "± 21.8346",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 418.131,
            "range": "± 28.7208",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.30083,
            "range": "± 209.662",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.28114,
            "range": "± 229.358",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.44667,
            "range": "± 1.43762",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.52711,
            "range": "± 577.624",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.2279,
            "range": "± 3.5566",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.8578,
            "range": "± 2.10295",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.51764,
            "range": "± 1.62678",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.65342,
            "range": "± 586.658",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.2325,
            "range": "± 3.3144",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.46947,
            "range": "± 1.32565",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.23805,
            "range": "± 1.48552",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.69639,
            "range": "± 660.361",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.7193,
            "range": "± 2.29565",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2065,
            "range": "± 1.32456",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.50768,
            "range": "± 901.71",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.45296,
            "range": "± 743.492",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.65099,
            "range": "± 1.6265",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.08447,
            "range": "± 733.01",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.59664,
            "range": "± 1.29653",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.84578,
            "range": "± 408.275",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 41.621,
            "range": "± 9.54485",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.3888,
            "range": "± 8.08826",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.47549,
            "range": "± 40.8343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.49902,
            "range": "± 37.0462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 97.8539,
            "range": "± 12.8278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.5681,
            "range": "± 8.60369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.63931,
            "range": "± 37.252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.28644,
            "range": "± 33.3169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 45.6669,
            "range": "± 17.1935",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.1843,
            "range": "± 170.62",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.3573,
            "range": "± 244.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.94252,
            "range": "± 184.039",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.7697,
            "range": "± 161.042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.074,
            "range": "± 218.799",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.6407,
            "range": "± 281.549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.6207,
            "range": "± 192.75",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.9263,
            "range": "± 596.412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.66406,
            "range": "± 185.747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.4364,
            "range": "± 246.71",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.1617,
            "range": "± 234.2",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.5459,
            "range": "± 340.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.1186,
            "range": "± 387.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.4743,
            "range": "± 205.892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.7394,
            "range": "± 268.288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.9461,
            "range": "± 426.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.4258,
            "range": "± 385.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.089,
            "range": "± 442.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.3647,
            "range": "± 622.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.5501,
            "range": "± 1.31263",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3462,
            "range": "± 458.961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.5844,
            "range": "± 559.705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.2547,
            "range": "± 408.042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.241,
            "range": "± 629.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.7886,
            "range": "± 501.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.7293,
            "range": "± 487.552",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.772,
            "range": "± 390.305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.5037,
            "range": "± 480.243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.9098,
            "range": "± 531.786",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3669,
            "range": "± 2.94539",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.6869,
            "range": "± 3.7893",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.1587,
            "range": "± 3.48821",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.7937,
            "range": "± 4.43146",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.9285,
            "range": "± 3.80476",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.1046,
            "range": "± 4.05665",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.682,
            "range": "± 3.5008",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.7879,
            "range": "± 3.87996",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.8717,
            "range": "± 3.66233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.671,
            "range": "± 4.06656",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.328,
            "range": "± 4.56001",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 198.899,
            "range": "± 10.1989",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 42.513,
            "range": "± 8.80758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 312.16,
            "range": "± 15.3542",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.4541,
            "range": "± 5.20833",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.1672,
            "range": "± 6.0287",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.1737,
            "range": "± 10.4114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 28.8717,
            "range": "± 5.56637",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.2685,
            "range": "± 11.5959",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.2423,
            "range": "± 5.82949",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.7223,
            "range": "± 6.17541",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.8276,
            "range": "± 6.16924",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.3274,
            "range": "± 5.85987",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.5304,
            "range": "± 5.38775",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 95.2941,
            "range": "± 13.7093",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.07506,
            "range": "± 29.8638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 149.219,
            "range": "± 16.4277",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.22555,
            "range": "± 22.4919",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.7397,
            "range": "± 1.90349",
            "unit": "ns",
            "extra": "100 samples\n861 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.531,
            "range": "± 6.07552",
            "unit": "ns",
            "extra": "100 samples\n9499 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.95071,
            "range": "± 0.711233",
            "unit": "ns",
            "extra": "100 samples\n8669 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.8451,
            "range": "± 2.0065",
            "unit": "ns",
            "extra": "100 samples\n760 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.9901,
            "range": "± 0.79055",
            "unit": "ns",
            "extra": "100 samples\n8795 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.3889,
            "range": "± 1.52989",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 223.516,
            "range": "± 16.4936",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.54288,
            "range": "± 0.371679",
            "unit": "ns",
            "extra": "100 samples\n7217 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.256,
            "range": "± 7.37699",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.3577,
            "range": "± 833.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.1489,
            "range": "± 3.06562",
            "unit": "ns",
            "extra": "100 samples\n717 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 693.22,
            "range": "± 11.8928",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 157.532,
            "range": "± 862.556",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54352,
            "range": "± 0.241989",
            "unit": "ns",
            "extra": "100 samples\n8776 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.1769,
            "range": "± 1.51977",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.96045,
            "range": "± 34.5449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 42.9466,
            "range": "± 2.06232",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.7893,
            "range": "± 5.33382",
            "unit": "ns",
            "extra": "100 samples\n724 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 689.529,
            "range": "± 6.6995",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 689.551,
            "range": "± 8.62159",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 157.502,
            "range": "± 981.419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 157.994,
            "range": "± 3.36711",
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
          "id": "ff462e569eed53bbee67d2a5fecd8744f6bc23c8",
          "message": "Add nautilus_assume_noalias for restrict-style aliasing hints (#308)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-06-20T19:39:28+02:00",
          "tree_id": "89bd049a2a83ea8af25319cc66dd34bd8c146cbf",
          "url": "https://github.com/nebulastream/nautilus/commit/ff462e569eed53bbee67d2a5fecd8744f6bc23c8"
        },
        "date": 1781977485616,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 165.368,
            "range": "± 7.50834",
            "unit": "ns",
            "extra": "100 samples\n186 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 413.293,
            "range": "± 20.9771",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.40608,
            "range": "± 336.727",
            "unit": "us",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 441.681,
            "range": "± 31.2435",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 793.962,
            "range": "± 69.118",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 445.159,
            "range": "± 36.9107",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 547.642,
            "range": "± 31.4613",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 429.165,
            "range": "± 41.5781",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 458.334,
            "range": "± 25.3475",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 411.273,
            "range": "± 23.4378",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.04194,
            "range": "± 158.26",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.06979,
            "range": "± 147.003",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 7.80538,
            "range": "± 991.835",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.18904,
            "range": "± 421.042",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.4074,
            "range": "± 3.45377",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.7012,
            "range": "± 1.51379",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.76801,
            "range": "± 1.12661",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.27946,
            "range": "± 447.078",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 14.8332,
            "range": "± 1.84172",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.65758,
            "range": "± 825.146",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.7931,
            "range": "± 1.19033",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.29415,
            "range": "± 492.515",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.4813,
            "range": "± 2.23545",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 8.99793,
            "range": "± 1.21973",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.04337,
            "range": "± 656.909",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.18921,
            "range": "± 948.677",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.09384,
            "range": "± 1.46298",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.48092,
            "range": "± 560.075",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.20518,
            "range": "± 1.11111",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.68396,
            "range": "± 731.698",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 36.253,
            "range": "± 5.49017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 36.5213,
            "range": "± 5.70658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.53418,
            "range": "± 19.5583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.47227,
            "range": "± 33.9299",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 95.9358,
            "range": "± 11.5781",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 47.7843,
            "range": "± 8.63628",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.61446,
            "range": "± 38.7422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.2421,
            "range": "± 38.2751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.1002,
            "range": "± 2.2328",
            "unit": "ns",
            "extra": "100 samples\n868 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 278.754,
            "range": "± 7.62311",
            "unit": "ns",
            "extra": "100 samples\n9638 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.77584,
            "range": "± 0.373907",
            "unit": "ns",
            "extra": "100 samples\n8757 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.4774,
            "range": "± 1.90569",
            "unit": "ns",
            "extra": "100 samples\n777 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.0799,
            "range": "± 1.08623",
            "unit": "ns",
            "extra": "100 samples\n8923 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.3271,
            "range": "± 2.31823",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 555.571,
            "range": "± 86.7304",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.7129,
            "range": "± 0.947451",
            "unit": "ns",
            "extra": "100 samples\n7332 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.275,
            "range": "± 8.79742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.2614,
            "range": "± 59.828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.3697,
            "range": "± 2.44759",
            "unit": "ns",
            "extra": "100 samples\n724 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 691.29,
            "range": "± 16.694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 160.088,
            "range": "± 9.94778",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.58828,
            "range": "± 0.397565",
            "unit": "ns",
            "extra": "100 samples\n8928 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.3079,
            "range": "± 1.51558",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.98118,
            "range": "± 26.2966",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.3003,
            "range": "± 4.42819",
            "unit": "ns",
            "extra": "100 samples\n728 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.8682,
            "range": "± 3.32678",
            "unit": "ns",
            "extra": "100 samples\n704 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 689.804,
            "range": "± 13.5395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 690.968,
            "range": "± 18.3281",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 158.96,
            "range": "± 7.96911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 158.244,
            "range": "± 5.01646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 46.0665,
            "range": "± 16.3388",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.28751,
            "range": "± 177.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.39444,
            "range": "± 197.992",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.00887,
            "range": "± 202.543",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.88051,
            "range": "± 177.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.40886,
            "range": "± 265.582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.7961,
            "range": "± 222.933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.4761,
            "range": "± 308.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.871,
            "range": "± 223.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.81065,
            "range": "± 192.575",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.109,
            "range": "± 239.236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.9257,
            "range": "± 268.88",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.7906,
            "range": "± 218.496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.4649,
            "range": "± 286.19",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.163,
            "range": "± 332.046",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.4583,
            "range": "± 208.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.1568,
            "range": "± 422.789",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.5794,
            "range": "± 433.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.5717,
            "range": "± 408.819",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.8353,
            "range": "± 495.873",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.8995,
            "range": "± 351.331",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.2212,
            "range": "± 462.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 30.0652,
            "range": "± 385.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.1159,
            "range": "± 297.016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.5623,
            "range": "± 225.726",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.3317,
            "range": "± 282.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 32.9229,
            "range": "± 381.119",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 66.5612,
            "range": "± 415.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 35.2677,
            "range": "± 380.973",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 96.4494,
            "range": "± 548.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7542,
            "range": "± 2.92059",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.4854,
            "range": "± 3.48687",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.4543,
            "range": "± 4.49777",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.2964,
            "range": "± 2.91241",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.2326,
            "range": "± 4.29663",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.2503,
            "range": "± 4.12746",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.231,
            "range": "± 3.97646",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.7262,
            "range": "± 3.96916",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.6242,
            "range": "± 4.28403",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.9837,
            "range": "± 2.54481",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.0049,
            "range": "± 4.43175",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.903,
            "range": "± 9.35762",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 43.2749,
            "range": "± 8.62494",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 303.634,
            "range": "± 12.0472",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.6385,
            "range": "± 5.2198",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.5333,
            "range": "± 6.10474",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 49.4109,
            "range": "± 11.6126",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.3579,
            "range": "± 6.14295",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 52.0736,
            "range": "± 15.7733",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.9122,
            "range": "± 6.81055",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.9539,
            "range": "± 7.53821",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.0116,
            "range": "± 5.25798",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.466,
            "range": "± 6.84696",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.877,
            "range": "± 7.53766",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 95.8488,
            "range": "± 12.5922",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.03945,
            "range": "± 17.5243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 147.108,
            "range": "± 18.2504",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.20225,
            "range": "± 27.7197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 48.1867,
            "range": "± 16.5893",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.60845,
            "range": "± 131.139",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 29.0736,
            "range": "± 571.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 47.015,
            "range": "± 17.4882",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.8218,
            "range": "± 19.873",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.73332,
            "range": "± 174.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 29.6134,
            "range": "± 312.092",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.5974,
            "range": "± 19.908",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 720.317,
            "range": "± 46.9217",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.50283,
            "range": "± 98.1444",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.26914,
            "range": "± 205.125",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.56254,
            "range": "± 87.5223",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.77434,
            "range": "± 158.933",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.74775,
            "range": "± 99.3398",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.91001,
            "range": "± 120.458",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.38735,
            "range": "± 89.544",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.68427,
            "range": "± 163.619",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.43404,
            "range": "± 122.535",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.69705,
            "range": "± 489.306",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.3224,
            "range": "± 4.02728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.7312,
            "range": "± 810.904",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 167.426,
            "range": "± 5.79658",
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
          "id": "49fb959815097f6760eb2416137c7c1e17d4ff98",
          "message": "Dump AsmJit IR and generated assembly from AsmJit backend\n\nAsmJitCompilationBackend::compile previously ignored the DumpHandler, so\nthe AsmJit pipeline produced no debug artifacts unlike the MLIR/CPP/BC\nbackends. Thread the DumpHandler into the lowering provider and emit two\nnew dumps:\n\n- after_asmjit_generation: the AsmJit builder node list, formatted before\n  finalize() so virtual registers are still visible.\n- after_asmjit_assembly: the generated assembly, captured via a\n  StringLogger attached to the CodeHolder during finalize().\n\nBoth are gated on DumpHandler::shouldDump so the formatting/logging cost\nis only paid when the dump is actually requested. shouldDump is now public\nand the new dump names participate in folder creation.\n\nCo-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_01Y6Ucp9Eo1qujHR5v3P4CqX",
          "timestamp": "2026-06-20T21:37:10+02:00",
          "tree_id": "3b98c1b8ce5048a2e8636e767b42933504227c22",
          "url": "https://github.com/nebulastream/nautilus/commit/49fb959815097f6760eb2416137c7c1e17d4ff98"
        },
        "date": 1781984515945,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 27.9989,
            "range": "± 2.22549",
            "unit": "ns",
            "extra": "100 samples\n862 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 214.012,
            "range": "± 6.18341",
            "unit": "ns",
            "extra": "100 samples\n9616 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.01703,
            "range": "± 0.402236",
            "unit": "ns",
            "extra": "100 samples\n8654 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 29.7528,
            "range": "± 1.14722",
            "unit": "ns",
            "extra": "100 samples\n808 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.67252,
            "range": "± 0.695126",
            "unit": "ns",
            "extra": "100 samples\n8800 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 12.8779,
            "range": "± 1.03338",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 495.159,
            "range": "± 49.8947",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.2645,
            "range": "± 1.14102",
            "unit": "ns",
            "extra": "100 samples\n7209 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 83.5805,
            "range": "± 4.78217",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 18.2725,
            "range": "± 43.0864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 34.7107,
            "range": "± 4.76029",
            "unit": "ns",
            "extra": "100 samples\n707 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 561.637,
            "range": "± 50.8983",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 128.885,
            "range": "± 6.52866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.76038,
            "range": "± 0.248535",
            "unit": "ns",
            "extra": "100 samples\n8809 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 17.2227,
            "range": "± 1.18723",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.0997,
            "range": "± 14.0212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 34.0122,
            "range": "± 1.81285",
            "unit": "ns",
            "extra": "100 samples\n701 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 34.3169,
            "range": "± 3.35191",
            "unit": "ns",
            "extra": "100 samples\n718 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 549.211,
            "range": "± 56.138",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 545.178,
            "range": "± 23.1735",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 128.256,
            "range": "± 492.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 128.282,
            "range": "± 956.636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 128.919,
            "range": "± 3.53787",
            "unit": "ns",
            "extra": "100 samples\n185 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 320.269,
            "range": "± 12.6854",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.827967,
            "range": "± 0.0520582",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 333.932,
            "range": "± 20.3352",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 670.904,
            "range": "± 85.3067",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 340.257,
            "range": "± 18.0635",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 413.998,
            "range": "± 27.6373",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 296.614,
            "range": "± 13.0092",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 354.703,
            "range": "± 26.4612",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 310.65,
            "range": "± 19.8404",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 36.5445,
            "range": "± 14.6779",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.19309,
            "range": "± 192.476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 36.4087,
            "range": "± 14.6444",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 2.92439,
            "range": "± 116.876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 22.6149,
            "range": "± 468.017",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 38.4168,
            "range": "± 16.2505",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 51.1418,
            "range": "± 15.5025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 4.6556,
            "range": "± 168.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 23.6957,
            "range": "± 599.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 53.4545,
            "range": "± 18.1542",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.16855,
            "range": "± 196.246",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 5.74669,
            "range": "± 222.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.76337,
            "range": "± 201.705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.67618,
            "range": "± 271.483",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 26.3744,
            "range": "± 352.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 10.7668,
            "range": "± 422.865",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 25.0778,
            "range": "± 374.16",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.66287,
            "range": "± 164.452",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 9.83357,
            "range": "± 272.871",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 8.7338,
            "range": "± 252.369",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 9.26062,
            "range": "± 309.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 21.4226,
            "range": "± 280.836",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.6819,
            "range": "± 294.714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 49.1964,
            "range": "± 267.502",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 22.6751,
            "range": "± 368.794",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 23.8297,
            "range": "± 2.76415",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 24.1452,
            "range": "± 415.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 23.618,
            "range": "± 1.21208",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 23.9639,
            "range": "± 420.109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 23.708,
            "range": "± 476.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 23.6875,
            "range": "± 687.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 23.1936,
            "range": "± 315.94",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 23.5669,
            "range": "± 1.64187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 23.6471,
            "range": "± 307.462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 25.5271,
            "range": "± 404.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 51.6295,
            "range": "± 809.46",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 27.5408,
            "range": "± 1.39575",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 75.44,
            "range": "± 768.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.245,
            "range": "± 2.19277",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.2239,
            "range": "± 3.25322",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 15.8725,
            "range": "± 4.07541",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 13.4235,
            "range": "± 3.62335",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 16.1399,
            "range": "± 4.52043",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.5702,
            "range": "± 3.18662",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 14.017,
            "range": "± 3.12678",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 13.0151,
            "range": "± 3.06361",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.4321,
            "range": "± 3.04208",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 13.2903,
            "range": "± 3.46837",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 24.4741,
            "range": "± 3.98638",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 152.363,
            "range": "± 8.90841",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 34.5958,
            "range": "± 8.00706",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 242.518,
            "range": "± 10.6758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 13.949,
            "range": "± 4.64381",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 21.4239,
            "range": "± 5.47759",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 37.4353,
            "range": "± 9.18645",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 22.5382,
            "range": "± 5.37079",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 38.5183,
            "range": "± 10.6946",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 25.2306,
            "range": "± 5.45111",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 27.1348,
            "range": "± 6.26958",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 18.9364,
            "range": "± 4.31409",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 24.2266,
            "range": "± 5.67305",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 22.7797,
            "range": "± 5.73086",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 76.0888,
            "range": "± 12.5215",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.8210000000000001,
            "range": "± 0.0227472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 116.099,
            "range": "± 15.0972",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 1.73372,
            "range": "± 24.811",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 564.343,
            "range": "± 69.1825",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.17811,
            "range": "± 153.956",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 2.53998,
            "range": "± 133.693",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.20816,
            "range": "± 44.6189",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.13253,
            "range": "± 116.933",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.35062,
            "range": "± 108.53",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.46558,
            "range": "± 76.8384",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.07338,
            "range": "± 51.9349",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.28777,
            "range": "± 82.51",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.0928,
            "range": "± 61.2169",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 5.9816,
            "range": "± 650.252",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 70.6651,
            "range": "± 3.03677",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 9.19066,
            "range": "± 1.19098",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 132.539,
            "range": "± 4.99275",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.76094,
            "range": "± 179.363",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.74562,
            "range": "± 180.785",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 6.41955,
            "range": "± 1.50031",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.51254,
            "range": "± 499.595",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 19.8149,
            "range": "± 3.23894",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 9.8019,
            "range": "± 1.57762",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 6.44154,
            "range": "± 1.12045",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.57262,
            "range": "± 452.176",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 12.6703,
            "range": "± 2.76934",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 6.50762,
            "range": "± 1.11429",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 6.62566,
            "range": "± 1.46977",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.63652,
            "range": "± 554.573",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 10.7622,
            "range": "± 2.1369",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 7.79926,
            "range": "± 1.12918",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 5.8095,
            "range": "± 686.766",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 5.81392,
            "range": "± 719.113",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 7.58706,
            "range": "± 1.32547",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.60848,
            "range": "± 751.834",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 5.94378,
            "range": "± 950.376",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.00583,
            "range": "± 324.975",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 30.6673,
            "range": "± 6.76474",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 30.6068,
            "range": "± 6.90653",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.14368,
            "range": "± 29.6765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.15863,
            "range": "± 26.9033",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 77.9231,
            "range": "± 10.5973",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 38.8983,
            "range": "± 7.86082",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.57692,
            "range": "± 103.489",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 1.77825,
            "range": "± 29.2895",
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
          "id": "e93b1f155642fa66bb3fddd79867485561752c97",
          "message": "Fold small constant operands into arithmetic ops on the threaded path\n\nAdd a bc.immediates option that folds a compile-time-constant right operand\ndirectly into ADD/SUB/MUL (i32/i64) in the flattened threaded stream, removing a\nregister read per use (e.g. the i = i + 1 loop increment).\n\nThe lowering's arithmetic visitors record, per block, the index and value of any\nright operand that is an integer constant fitting a signed 16-bit field (detected\nvia ir::dyn_cast<ConstIntOperation>). The flat builder consumes those records and\nemits an *_imm opcode with the operand in reg1 and the immediate packed into the\nOpCode's reg2 short — so no parallel side array is needed, and constants that do\nnot fit are simply not folded. The imm opcodes, threaded labels/handlers,\nsource→imm mapping and defensive switch cases are all generated from one\nNAUTILUS_BC_IMM_LIST.\n\nAs with the other threaded-path optimizations, call/switch are untouched, so\nBCDispatchModeTest (now also toggling immediates, with kernels that contain\nconstant operands) validates the folding against the call reference, including\nnegative and too-large-to-fold constants.\n\nIsolated impact is small (~1% on fibonacci — within noise), as expected for one\nfewer read per iteration; it composes with the other threaded-path optimizations\n(superinstructions + immediates together: ~1.08-1.11x over the threaded\nbaseline). ExecutionBenchmark gains a threaded baseline-vs-allOpts A/B.\n\nCo-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_01MzmREyh3TTRaX5wecYosRs",
          "timestamp": "2026-06-30T21:26:21+02:00",
          "tree_id": "fff89ef56fd710e4bcddab23ef9182bef9c30b4c",
          "url": "https://github.com/nebulastream/nautilus/commit/e93b1f155642fa66bb3fddd79867485561752c97"
        },
        "date": 1782848010062,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 35.6197,
            "range": "± 3.35474",
            "unit": "ns",
            "extra": "100 samples\n618 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 419.763,
            "range": "± 38.3871",
            "unit": "ns",
            "extra": "100 samples\n6882 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.92296,
            "range": "± 0.35715",
            "unit": "ns",
            "extra": "100 samples\n6616 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.453,
            "range": "± 1.8005",
            "unit": "ns",
            "extra": "100 samples\n563 iterations"
          },
          {
            "name": "ssa_add",
            "value": 166.938,
            "range": "± 18.9456",
            "unit": "ns",
            "extra": "100 samples\n139 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 438.372,
            "range": "± 23.0167",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.19711,
            "range": "± 67.6231",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 469.629,
            "range": "± 24.2537",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 950.305,
            "range": "± 49.5365",
            "unit": "ns",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 464.533,
            "range": "± 11.6577",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 557.92,
            "range": "± 30.2928",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 384.495,
            "range": "± 49.2498",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 483.333,
            "range": "± 30.4757",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 446.728,
            "range": "± 29.0944",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.09414,
            "range": "± 211.066",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.93204,
            "range": "± 360.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 6.40063,
            "range": "± 282.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.48125,
            "range": "± 210.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.6182,
            "range": "± 838.294",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.5936,
            "range": "± 404.122",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.1043,
            "range": "± 821.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 6.99787,
            "range": "± 319.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.9024,
            "range": "± 448.854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.5387,
            "range": "± 237.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.1186,
            "range": "± 350.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.2351,
            "range": "± 379.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.8201,
            "range": "± 316.59",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 60.5722,
            "range": "± 1.28586",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 23.5447,
            "range": "± 1.54802",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 23.3518,
            "range": "± 1.51112",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.7755,
            "range": "± 2.14296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.6916,
            "range": "± 3.19194",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 37.5913,
            "range": "± 92.7819",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 33.2013,
            "range": "± 61.3157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 30.3676,
            "range": "± 26.3193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.5271,
            "range": "± 1.45289",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 42.9381,
            "range": "± 117.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.3021,
            "range": "± 24.0869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 70.2643,
            "range": "± 191.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.3385,
            "range": "± 27.0284",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.354,
            "range": "± 5.60264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 89.7008,
            "range": "± 5.00453",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 10.3088,
            "range": "± 2.07175",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 12.3898,
            "range": "± 2.14039",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 16.5289,
            "range": "± 3.16754",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.1346,
            "range": "± 2.25119",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 14.6036,
            "range": "± 4.22285",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.385,
            "range": "± 5.53707",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 13.0203,
            "range": "± 2.95288",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.3817,
            "range": "± 2.54977",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 12.0403,
            "range": "± 2.41542",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 11.7071,
            "range": "± 2.02691",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 33.9127,
            "range": "± 4.86649",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 208.468,
            "range": "± 9.29832",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 45.4725,
            "range": "± 6.41797",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 321.243,
            "range": "± 19.0488",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 14.5097,
            "range": "± 3.56082",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 21.9319,
            "range": "± 5.96924",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 40.8542,
            "range": "± 9.67393",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 22.7075,
            "range": "± 3.58503",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 37.099,
            "range": "± 7.00128",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 28.4811,
            "range": "± 10.0061",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 30.702,
            "range": "± 7.2958",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 20.2654,
            "range": "± 3.28752",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 28.22,
            "range": "± 12.0757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 23.337,
            "range": "± 4.6628",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 77.3389,
            "range": "± 8.81602",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.980449,
            "range": "± 0.0472725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 124.434,
            "range": "± 15.9005",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.16968,
            "range": "± 74.9546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.55718,
            "range": "± 177.825",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.94654,
            "range": "± 265.797",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.5878,
            "range": "± 1.64364",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.25732,
            "range": "± 405.004",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.6913,
            "range": "± 1.9996",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.0099,
            "range": "± 1.06581",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.3957,
            "range": "± 1.03967",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.27081,
            "range": "± 334.971",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.4317,
            "range": "± 1.28685",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.52759,
            "range": "± 884.531",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.29134,
            "range": "± 1.34469",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.20274,
            "range": "± 347.219",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.9739,
            "range": "± 909.534",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.4022,
            "range": "± 843.456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.54801,
            "range": "± 436.296",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.60522,
            "range": "± 360.132",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.5421,
            "range": "± 776.898",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.80114,
            "range": "± 459.733",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.20602,
            "range": "± 757.123",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.31882,
            "range": "± 263.75",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.9458,
            "range": "± 2.08592",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 42.0262,
            "range": "± 4.75497",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.2741,
            "range": "± 28.6508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.3335,
            "range": "± 33.1148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.521,
            "range": "± 4.12889",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 61.202,
            "range": "± 4.53775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.30735,
            "range": "± 63.7816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.39314,
            "range": "± 56.3583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 42.0216,
            "range": "± 11.8007",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 4.98625,
            "range": "± 429.818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.6536,
            "range": "± 966.012",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.6195,
            "range": "± 17.2962",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 58.5368,
            "range": "± 20.5265",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 6.86704,
            "range": "± 553.415",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.9367,
            "range": "± 2.85675",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 55.2106,
            "range": "± 12.6658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 43.0997,
            "range": "± 11.2918",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.16732,
            "range": "± 433.28",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 13.7223,
            "range": "± 1.48187",
            "unit": "ns",
            "extra": "100 samples\n6785 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 24.9649,
            "range": "± 3.24582",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 663.528,
            "range": "± 54.494",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.20553,
            "range": "± 0.728119",
            "unit": "ns",
            "extra": "100 samples\n6068 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 60.191,
            "range": "± 5.13562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 14.2345,
            "range": "± 564.125",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 41.8107,
            "range": "± 2.9469",
            "unit": "ns",
            "extra": "100 samples\n562 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 658.456,
            "range": "± 12.7441",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 145.17,
            "range": "± 2.38436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.43391,
            "range": "± 0.373943",
            "unit": "ns",
            "extra": "100 samples\n6809 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.9409,
            "range": "± 3.49622",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.07781,
            "range": "± 84.5207",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 41.2296,
            "range": "± 2.80105",
            "unit": "ns",
            "extra": "100 samples\n534 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 41.8621,
            "range": "± 2.5228",
            "unit": "ns",
            "extra": "100 samples\n558 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 653.866,
            "range": "± 15.3644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 647.953,
            "range": "± 12.8954",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 144.95,
            "range": "± 3.13382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 143.997,
            "range": "± 2.10593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 41.909,
            "range": "± 2.26401",
            "unit": "ns",
            "extra": "100 samples\n556 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 40.4512,
            "range": "± 3.33045",
            "unit": "ns",
            "extra": "100 samples\n573 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 37.0901,
            "range": "± 2.76845",
            "unit": "ns",
            "extra": "100 samples\n635 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 652.623,
            "range": "± 12.8226",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 501.766,
            "range": "± 7.59728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 576.063,
            "range": "± 12.3051",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 143.166,
            "range": "± 1.83566",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 109.565,
            "range": "± 853.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 112.64,
            "range": "± 1.75049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 36.5429,
            "range": "± 2.78799",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 29.7728,
            "range": "± 2.81951",
            "unit": "ns",
            "extra": "100 samples\n809 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 575.491,
            "range": "± 11.9017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 577.844,
            "range": "± 12.0836",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 111.053,
            "range": "± 1.58846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 111.249,
            "range": "± 1.57055",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 37.2852,
            "range": "± 3.40793",
            "unit": "ns",
            "extra": "100 samples\n625 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 37.6478,
            "range": "± 3.47499",
            "unit": "ns",
            "extra": "100 samples\n626 iterations"
          },
          {
            "name": "ir_add",
            "value": 750.248,
            "range": "± 135.359",
            "unit": "ns",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.58024,
            "range": "± 93.6128",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.54378,
            "range": "± 341.268",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.72975,
            "range": "± 195.67",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.06523,
            "range": "± 195.022",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.86511,
            "range": "± 70.0584",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.08644,
            "range": "± 154.6",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45478,
            "range": "± 90.4557",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.7733,
            "range": "± 95.3248",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.56191,
            "range": "± 110.4",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.12244,
            "range": "± 523.13",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 91.3638,
            "range": "± 4.97241",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.8579,
            "range": "± 1.06409",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 169.802,
            "range": "± 14.9489",
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
          "id": "4d41d824e335ef6ebd48e942e55c8b9a0ff6e388",
          "message": "Fold unsigned integer ops with unsigned semantics (#312)\n\nThe IR constant-folding pass evaluated every integer fold in int64_t, so\nunsigned comparison, division, modulo and right-shift on constant operands\nabove INT64_MAX produced signed results — a miscompile shared by all compiled\nbackends (the interpreter, which runs the real ops, was correct). Found by the\ndifferential AST fuzzer.\n\nBranch the affected folds on the operand's unsigned stamp (isUnsignedInteger):\n- foldCompareInt: LT/LE/GT/GE compare as uint64_t when unsigned (EQ/NE unchanged).\n- foldIntArithmetic: DivOp/ModOp use uint64_t division/modulo when unsigned.\n- ShiftOp right-shift: logical (zero-fill) shift for unsigned, arithmetic for signed.\nAdd/Sub/Mul and bitwise ops are sign-agnostic and unchanged. Compare reads the\noperand stamp because CompareOperation's own stamp is bool.\n\nAdd regression tests covering ui64 LT/Div/Mod/right-shift with operands above\nINT64_MAX, plus a signed-i64 case to confirm signed folding is preserved.\n\nVerified: the fuzzer's survey over 4000 generated programs now reports zero\ncross-backend disagreements (previously 84), and the focused cpp/bc/asmjit\ndiagnostics match the interpreter.\n\nCo-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>\nClaude-Session: https://claude.ai/code/session_0156i1wKUmrD1GGrquT5e99c",
          "timestamp": "2026-06-30T21:56:22+02:00",
          "tree_id": "a08f573cbcc6e479c32e082fcaa62ffd4ffcc43e",
          "url": "https://github.com/nebulastream/nautilus/commit/4d41d824e335ef6ebd48e942e55c8b9a0ff6e388"
        },
        "date": 1782849816688,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 53.0628,
            "range": "± 12.6427",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.55639,
            "range": "± 141.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 177.391,
            "range": "± 20.1918",
            "unit": "ns",
            "extra": "100 samples\n170 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 444.164,
            "range": "± 40.6575",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.12898,
            "range": "± 110.318",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 495.014,
            "range": "± 69.0807",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 886.082,
            "range": "± 67.1721",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 486.979,
            "range": "± 42.0816",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 578.026,
            "range": "± 43.6883",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 418.129,
            "range": "± 45.1412",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 498.312,
            "range": "± 55.5237",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 449.131,
            "range": "± 51.4034",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 59.7891,
            "range": "± 13.7637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.301,
            "range": "± 103.385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.1424,
            "range": "± 484.582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 58.0137,
            "range": "± 14.0644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 77.9678,
            "range": "± 15.6027",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.34042,
            "range": "± 129.614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.8062,
            "range": "± 898.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 80.8029,
            "range": "± 15.2215",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.63419,
            "range": "± 206.656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.20463,
            "range": "± 165.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.11918,
            "range": "± 184.846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.27484,
            "range": "± 250.777",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.3988,
            "range": "± 889.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.0764,
            "range": "± 326.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.3581,
            "range": "± 331.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.05943,
            "range": "± 155.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.5846,
            "range": "± 243.871",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.49945,
            "range": "± 191.818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.5671,
            "range": "± 197.555",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.1284,
            "range": "± 302.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.5971,
            "range": "± 161.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.7116,
            "range": "± 507.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.1413,
            "range": "± 422.999",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.9543,
            "range": "± 1.34096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.4285,
            "range": "± 737.06",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.0273,
            "range": "± 464.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.0366,
            "range": "± 1.57039",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.0382,
            "range": "± 7.79728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.5984,
            "range": "± 855.979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.9874,
            "range": "± 553.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.7541,
            "range": "± 724.428",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.1911,
            "range": "± 530.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.1676,
            "range": "± 551.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.4728,
            "range": "± 503.337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.564,
            "range": "± 599.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.4453,
            "range": "± 1.3216",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.453,
            "range": "± 3.02819",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.5991,
            "range": "± 3.69885",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.3943,
            "range": "± 3.60061",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.4847,
            "range": "± 3.5195",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.5805,
            "range": "± 4.34415",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.1722,
            "range": "± 3.85631",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.0069,
            "range": "± 3.70122",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.211,
            "range": "± 4.37756",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.7886,
            "range": "± 3.383",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.2917,
            "range": "± 3.69382",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 44.0002,
            "range": "± 6.76016",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 207.659,
            "range": "± 10.6984",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 54.4637,
            "range": "± 7.70812",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 316.258,
            "range": "± 12.7538",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.6036,
            "range": "± 4.37432",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.2562,
            "range": "± 4.84779",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.5134,
            "range": "± 9.8018",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 42.0064,
            "range": "± 8.29923",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.2581,
            "range": "± 9.78608",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.5776,
            "range": "± 8.68704",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.3834,
            "range": "± 9.67039",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.0106,
            "range": "± 4.24595",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 45.1677,
            "range": "± 9.30245",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 41.435,
            "range": "± 7.37459",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 104.986,
            "range": "± 14.1102",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.03065,
            "range": "± 16.2079",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 153.837,
            "range": "± 15.7456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.14238,
            "range": "± 38.0666",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 803.337,
            "range": "± 85.161",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.63519,
            "range": "± 178.36",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.40605,
            "range": "± 327.867",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.68544,
            "range": "± 132.466",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.80335,
            "range": "± 317.729",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.80473,
            "range": "± 140.688",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.05641,
            "range": "± 197.919",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.46163,
            "range": "± 107.256",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74167,
            "range": "± 120.428",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53956,
            "range": "± 198.896",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.7955,
            "range": "± 1.21685",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 86.9938,
            "range": "± 3.18079",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.764,
            "range": "± 1.29901",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 166.677,
            "range": "± 7.56626",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.38,
            "range": "± 0.786246",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.3448,
            "range": "± 1.15106",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 550.546,
            "range": "± 25.8683",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.64069,
            "range": "± 0.482687",
            "unit": "ns",
            "extra": "100 samples\n6567 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.2451,
            "range": "± 12.3003",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9713,
            "range": "± 105.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.3295,
            "range": "± 7.20414",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 924.809,
            "range": "± 11.6266",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 199.391,
            "range": "± 1.40388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49379,
            "range": "± 0.432852",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.8844,
            "range": "± 3.58816",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.84278,
            "range": "± 27.8809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.6899,
            "range": "± 7.89926",
            "unit": "ns",
            "extra": "100 samples\n631 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.913,
            "range": "± 5.19391",
            "unit": "ns",
            "extra": "100 samples\n627 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 924.772,
            "range": "± 12.2005",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 926.036,
            "range": "± 12.7124",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 199.281,
            "range": "± 1.26788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 199.453,
            "range": "± 1.18979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 49.3591,
            "range": "± 9.02615",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.4218,
            "range": "± 4.76707",
            "unit": "ns",
            "extra": "100 samples\n730 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.7727,
            "range": "± 9.78233",
            "unit": "ns",
            "extra": "100 samples\n756 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 924.626,
            "range": "± 18.5428",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 717.458,
            "range": "± 18.6129",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 591.464,
            "range": "± 9.83311",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 199.087,
            "range": "± 528.815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 138.498,
            "range": "± 13.9211",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 118.501,
            "range": "± 2.30563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.0205,
            "range": "± 5.22648",
            "unit": "ns",
            "extra": "100 samples\n580 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.1335,
            "range": "± 3.79498",
            "unit": "ns",
            "extra": "100 samples\n891 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 591.744,
            "range": "± 10.1918",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 592.922,
            "range": "± 12.6576",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 118.697,
            "range": "± 5.64517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 116.882,
            "range": "± 278.848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.635,
            "range": "± 6.19649",
            "unit": "ns",
            "extra": "100 samples\n718 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.1095,
            "range": "± 7.25996",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.38002,
            "range": "± 278.219",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.37562,
            "range": "± 270.253",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.04128,
            "range": "± 1.80384",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.69637,
            "range": "± 650.376",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.1607,
            "range": "± 3.59983",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.6454,
            "range": "± 1.65701",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.71978,
            "range": "± 1.28031",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.97309,
            "range": "± 844.882",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.5325,
            "range": "± 2.9221",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.99653,
            "range": "± 1.59729",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.81336,
            "range": "± 1.20335",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.8406,
            "range": "± 513.393",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.3629,
            "range": "± 2.95626",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.1733,
            "range": "± 2.45538",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.45636,
            "range": "± 1.09574",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.49835,
            "range": "± 881.042",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.6859,
            "range": "± 2.02711",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.2327,
            "range": "± 825.42",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.14461,
            "range": "± 1.18686",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.06691,
            "range": "± 576.564",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.4177,
            "range": "± 8.4914",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.0562,
            "range": "± 6.87644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35377,
            "range": "± 46.1927",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36623,
            "range": "± 54.8062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.598,
            "range": "± 10.8767",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.1039,
            "range": "± 9.49657",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.4493,
            "range": "± 66.5753",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.28273,
            "range": "± 270.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.2971,
            "range": "± 4.68797",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 286.245,
            "range": "± 7.454",
            "unit": "ns",
            "extra": "100 samples\n8265 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.04213,
            "range": "± 0.784892",
            "unit": "ns",
            "extra": "100 samples\n7736 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.0771,
            "range": "± 4.23512",
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
          "id": "05fe09dc7a8f1683958fb9c5b7fef706f8ee1e02",
          "message": "Extend the differential fuzzer to multi-width ints, casts, and floats (#316)",
          "timestamp": "2026-07-01T00:49:15+02:00",
          "tree_id": "5b85aff20b27ef47d50a0823ebabd5a32dceeb08",
          "url": "https://github.com/nebulastream/nautilus/commit/05fe09dc7a8f1683958fb9c5b7fef706f8ee1e02"
        },
        "date": 1782860223827,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 36.9673,
            "range": "± 1.53767",
            "unit": "ns",
            "extra": "100 samples\n809 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 317.493,
            "range": "± 18.3632",
            "unit": "ns",
            "extra": "100 samples\n8581 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.26753,
            "range": "± 0.619367",
            "unit": "ns",
            "extra": "100 samples\n7840 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.8688,
            "range": "± 2.34539",
            "unit": "ns",
            "extra": "100 samples\n800 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.33043,
            "range": "± 181.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.89899,
            "range": "± 172.828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.72772,
            "range": "± 196.104",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.2264,
            "range": "± 274.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.6902,
            "range": "± 513.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.1088,
            "range": "± 369.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.9406,
            "range": "± 525.577",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.68769,
            "range": "± 160.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.33,
            "range": "± 397.546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.1715,
            "range": "± 376.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.3155,
            "range": "± 334.823",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.4438,
            "range": "± 378.266",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.1361,
            "range": "± 1.6176",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1802,
            "range": "± 714.536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.1791,
            "range": "± 432.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.6509,
            "range": "± 394.379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.3373,
            "range": "± 534.591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.161,
            "range": "± 405.511",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.2101,
            "range": "± 441.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.5026,
            "range": "± 392.398",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.1635,
            "range": "± 611.325",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.8215,
            "range": "± 381.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.4711,
            "range": "± 412.262",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.5143,
            "range": "± 548.358",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.344,
            "range": "± 713.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.2578,
            "range": "± 532.115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 34.1634,
            "range": "± 357.147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.0898,
            "range": "± 486.534",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4941,
            "range": "± 2.54051",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.4119,
            "range": "± 4.23547",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.8161,
            "range": "± 5.38877",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.9963,
            "range": "± 3.73612",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.2562,
            "range": "± 4.22746",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.1314,
            "range": "± 3.82347",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.2287,
            "range": "± 4.31238",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.743,
            "range": "± 3.75765",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.3071,
            "range": "± 3.87746",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.211,
            "range": "± 2.64018",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.7044,
            "range": "± 4.92639",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 201.098,
            "range": "± 9.72349",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 45.3908,
            "range": "± 9.54643",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 316.628,
            "range": "± 20.6454",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.6629,
            "range": "± 5.12752",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.5932,
            "range": "± 5.88011",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 49.8223,
            "range": "± 10.7815",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.7406,
            "range": "± 6.01007",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.7616,
            "range": "± 12.6841",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 33.0572,
            "range": "± 6.03108",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 36.3314,
            "range": "± 6.26812",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 26.1642,
            "range": "± 5.33427",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 32.2764,
            "range": "± 7.65091",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.2525,
            "range": "± 5.75675",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 98.8878,
            "range": "± 15.0789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.06855,
            "range": "± 19.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 149.299,
            "range": "± 14.6551",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.21291,
            "range": "± 40.9192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 164.103,
            "range": "± 7.86917",
            "unit": "ns",
            "extra": "100 samples\n187 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 426.036,
            "range": "± 37.4056",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.06594,
            "range": "± 72.2822",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 454.076,
            "range": "± 24.6217",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 859.825,
            "range": "± 82.5775",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 452.459,
            "range": "± 37.0175",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 519.062,
            "range": "± 32.8844",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 382.628,
            "range": "± 15.1349",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 466.143,
            "range": "± 35.3573",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 424.349,
            "range": "± 36.3939",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ir_add",
            "value": 699.822,
            "range": "± 38.924",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.49259,
            "range": "± 100.261",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.28485,
            "range": "± 221.228",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.57061,
            "range": "± 105.289",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.8202,
            "range": "± 213.834",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.72378,
            "range": "± 115.979",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.92389,
            "range": "± 124.349",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.38476,
            "range": "± 85.0191",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.66332,
            "range": "± 118.263",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.42043,
            "range": "± 87.4477",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.62457,
            "range": "± 610.705",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.7438,
            "range": "± 3.27485",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.7159,
            "range": "± 962.573",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 161.961,
            "range": "± 5.24953",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 45.731,
            "range": "± 16.5663",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.58151,
            "range": "± 120.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.955,
            "range": "± 510.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.8852,
            "range": "± 16.0106",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.2584,
            "range": "± 17.9296",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.65075,
            "range": "± 157.528",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.1306,
            "range": "± 542.565",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.7941,
            "range": "± 19.2774",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.5079,
            "range": "± 0.878005",
            "unit": "ns",
            "extra": "100 samples\n8790 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.7918,
            "range": "± 1.24767",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 570.224,
            "range": "± 59.9875",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.95977,
            "range": "± 1.38574",
            "unit": "ns",
            "extra": "100 samples\n7238 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.365,
            "range": "± 7.96309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5912,
            "range": "± 136.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.8724,
            "range": "± 3.74629",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 629.694,
            "range": "± 10.161",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 141.394,
            "range": "± 2.67382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.76483,
            "range": "± 0.622618",
            "unit": "ns",
            "extra": "100 samples\n8810 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 24.3138,
            "range": "± 2.40972",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.69218,
            "range": "± 261.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.0924,
            "range": "± 2.61154",
            "unit": "ns",
            "extra": "100 samples\n710 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.6201,
            "range": "± 1.74528",
            "unit": "ns",
            "extra": "100 samples\n709 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 636.345,
            "range": "± 12.6038",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 630.838,
            "range": "± 16.5616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 141.37,
            "range": "± 497.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 141.887,
            "range": "± 1.72996",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 44.727,
            "range": "± 4.70681",
            "unit": "ns",
            "extra": "100 samples\n703 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.3958,
            "range": "± 2.84136",
            "unit": "ns",
            "extra": "100 samples\n758 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 38.5266,
            "range": "± 2.62689",
            "unit": "ns",
            "extra": "100 samples\n817 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 630.049,
            "range": "± 11.6289",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 500.207,
            "range": "± 8.43111",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 945.354,
            "range": "± 21.6149",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 141.753,
            "range": "± 744.55",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 107.532,
            "range": "± 2.67354",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 170.752,
            "range": "± 3.78042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 38.8901,
            "range": "± 3.68844",
            "unit": "ns",
            "extra": "100 samples\n816 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.411,
            "range": "± 4.11364",
            "unit": "ns",
            "extra": "100 samples\n880 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 952.541,
            "range": "± 14.1507",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 948.365,
            "range": "± 17.5157",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 171.093,
            "range": "± 4.24036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 167.629,
            "range": "± 964.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 38.8122,
            "range": "± 4.07798",
            "unit": "ns",
            "extra": "100 samples\n761 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 38.804,
            "range": "± 3.09491",
            "unit": "ns",
            "extra": "100 samples\n805 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 46.5643,
            "range": "± 16.4229",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.24737,
            "range": "± 474.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.41508,
            "range": "± 224.681",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.4273,
            "range": "± 249.502",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.43813,
            "range": "± 1.51832",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.72891,
            "range": "± 538.113",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.7404,
            "range": "± 3.73849",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.0664,
            "range": "± 1.80197",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.37997,
            "range": "± 1.468",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.83322,
            "range": "± 543.67",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.906,
            "range": "± 3.32047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.6005,
            "range": "± 1.24969",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.69348,
            "range": "± 1.81875",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.85854,
            "range": "± 590.772",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.7141,
            "range": "± 2.48327",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.3561,
            "range": "± 1.78725",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.61623,
            "range": "± 825.612",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.7005,
            "range": "± 779.91",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.74223,
            "range": "± 1.49429",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.26661,
            "range": "± 758.317",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.7129,
            "range": "± 1.04813",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.07284,
            "range": "± 446.388",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.2703,
            "range": "± 8.01207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.0451,
            "range": "± 7.64876",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.46363,
            "range": "± 33.6792",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.4624,
            "range": "± 50.9059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 93.9505,
            "range": "± 11.0339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.8155,
            "range": "± 8.53376",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.41905,
            "range": "± 244.657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.32584,
            "range": "± 39.3113",
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
          "id": "f78d8e0d33b762ac3414c12ba19cef8d32520f24",
          "message": "Extend differential fuzzer to int↔float casts and bounded loops; fix signed-zero negation bug (#318)",
          "timestamp": "2026-07-01T08:23:35+02:00",
          "tree_id": "6ad0cff6dc06308f4096c72a777ca77dc5428f22",
          "url": "https://github.com/nebulastream/nautilus/commit/f78d8e0d33b762ac3414c12ba19cef8d32520f24"
        },
        "date": 1782887620761,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 11.0543,
            "range": "± 1.12111",
            "unit": "ns",
            "extra": "100 samples\n8798 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 18.6909,
            "range": "± 4.24873",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 574.004,
            "range": "± 60.8521",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.55857,
            "range": "± 0.370408",
            "unit": "ns",
            "extra": "100 samples\n6979 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.598,
            "range": "± 7.58959",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.7178,
            "range": "± 615.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.8796,
            "range": "± 3.7996",
            "unit": "ns",
            "extra": "100 samples\n709 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 638.51,
            "range": "± 32.2814",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 141.381,
            "range": "± 654.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49483,
            "range": "± 0.0109764",
            "unit": "ns",
            "extra": "100 samples\n8814 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 24.4432,
            "range": "± 2.39851",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.70335,
            "range": "± 92.4169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.1591,
            "range": "± 2.92886",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.2079,
            "range": "± 3.91553",
            "unit": "ns",
            "extra": "100 samples\n704 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 638.83,
            "range": "± 28.1936",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 633.418,
            "range": "± 14.4173",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 141.999,
            "range": "± 319.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 141.86,
            "range": "± 2.03139",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 44.6229,
            "range": "± 4.51551",
            "unit": "ns",
            "extra": "100 samples\n715 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.266,
            "range": "± 4.14266",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.3752,
            "range": "± 3.44293",
            "unit": "ns",
            "extra": "100 samples\n822 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 633.194,
            "range": "± 12.5807",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 506.79,
            "range": "± 13.8992",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 928.356,
            "range": "± 28.2847",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 141.675,
            "range": "± 1.88032",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 105.62,
            "range": "± 4.21774",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 167.939,
            "range": "± 1.33907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.4032,
            "range": "± 3.35932",
            "unit": "ns",
            "extra": "100 samples\n818 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.3258,
            "range": "± 3.60659",
            "unit": "ns",
            "extra": "100 samples\n876 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 932.983,
            "range": "± 21.0585",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 925.3,
            "range": "± 20.8577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 168.139,
            "range": "± 1.54735",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 167.478,
            "range": "± 2.42964",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.4696,
            "range": "± 4.35532",
            "unit": "ns",
            "extra": "100 samples\n815 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.5597,
            "range": "± 4.54838",
            "unit": "ns",
            "extra": "100 samples\n807 iterations"
          },
          {
            "name": "ir_add",
            "value": 722.112,
            "range": "± 41.7205",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.49619,
            "range": "± 72.603",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.25993,
            "range": "± 131.411",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.60111,
            "range": "± 114.276",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.81097,
            "range": "± 193.468",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.76497,
            "range": "± 140.174",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98816,
            "range": "± 225.09",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45626,
            "range": "± 176.385",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.68052,
            "range": "± 112.843",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.44333,
            "range": "± 91.3712",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.59799,
            "range": "± 443.239",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.8611,
            "range": "± 5.78181",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.7276,
            "range": "± 719.985",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 166.642,
            "range": "± 6.6962",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.4575,
            "range": "± 304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.48465,
            "range": "± 308.865",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.96989,
            "range": "± 261.486",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.49296,
            "range": "± 431.507",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.5671,
            "range": "± 247.721",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.7752,
            "range": "± 461.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1614,
            "range": "± 392.276",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 6.21138,
            "range": "± 289.844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.3133,
            "range": "± 431.974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.1189,
            "range": "± 515.676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.5526,
            "range": "± 310.169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.6412,
            "range": "± 315.495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.0821,
            "range": "± 320.405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.3864,
            "range": "± 439.477",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.7657,
            "range": "± 457.274",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.4973,
            "range": "± 500.38",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 31.4011,
            "range": "± 718.378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 30.3372,
            "range": "± 544.934",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 31.1852,
            "range": "± 555.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.6508,
            "range": "± 635.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 31.0248,
            "range": "± 1.10695",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.9509,
            "range": "± 602.413",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 30.5745,
            "range": "± 887.534",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.3283,
            "range": "± 817.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 34.2769,
            "range": "± 1.01909",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 67.0649,
            "range": "± 1.75645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 36.7091,
            "range": "± 643.902",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 97.8994,
            "range": "± 798.776",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.6899,
            "range": "± 2.71757",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.1761,
            "range": "± 3.99593",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.8294,
            "range": "± 4.85608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.208,
            "range": "± 4.19873",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.1709,
            "range": "± 4.11911",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.1052,
            "range": "± 3.79832",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.1486,
            "range": "± 4.10419",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.7491,
            "range": "± 4.47771",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.4263,
            "range": "± 4.25369",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.2168,
            "range": "± 4.09829",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 33.7804,
            "range": "± 4.87948",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 206.722,
            "range": "± 11.7779",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 47.3063,
            "range": "± 12.0918",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 348.366,
            "range": "± 14.7698",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.9462,
            "range": "± 5.61986",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.4656,
            "range": "± 7.02376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 50.0876,
            "range": "± 12.1776",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.9169,
            "range": "± 6.93063",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.603,
            "range": "± 12.6074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.7705,
            "range": "± 6.92362",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.4526,
            "range": "± 6.89765",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 25.9519,
            "range": "± 5.54181",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.5203,
            "range": "± 6.0879",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.2259,
            "range": "± 6.5266",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 98.6538,
            "range": "± 14.5404",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.08546,
            "range": "± 22.2397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 149.182,
            "range": "± 15.6582",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.29556,
            "range": "± 37.3242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 47.4694,
            "range": "± 19.8506",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 7.03957,
            "range": "± 329.982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 47.0367,
            "range": "± 18.0438",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.72917,
            "range": "± 401.334",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 30.2983,
            "range": "± 1.8102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 48.21,
            "range": "± 17.6511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.6768,
            "range": "± 19.9426",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 6.42163,
            "range": "± 962.07",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 31.4767,
            "range": "± 546.088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 65.5013,
            "range": "± 19.8864",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 177.519,
            "range": "± 21.3492",
            "unit": "ns",
            "extra": "100 samples\n185 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 411.209,
            "range": "± 18.1261",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.06884,
            "range": "± 75.3213",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 450.512,
            "range": "± 32.4857",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 824.093,
            "range": "± 52.6216",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 442.999,
            "range": "± 31.5687",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 517.593,
            "range": "± 33.7538",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 385.929,
            "range": "± 22.1952",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 456.047,
            "range": "± 31.9951",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 415.006,
            "range": "± 27.7157",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.33096,
            "range": "± 217.603",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.33875,
            "range": "± 211.718",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.32122,
            "range": "± 1.59929",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.52488,
            "range": "± 574.147",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.6886,
            "range": "± 4.04551",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.557,
            "range": "± 2.07663",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.31342,
            "range": "± 1.52999",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.64287,
            "range": "± 549.024",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.9457,
            "range": "± 3.56877",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.41445,
            "range": "± 1.2274",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.51317,
            "range": "± 1.59264",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.74648,
            "range": "± 690.508",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.6216,
            "range": "± 2.62902",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.0244,
            "range": "± 1.43582",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.69522,
            "range": "± 992.508",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.6685,
            "range": "± 787.823",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.53699,
            "range": "± 1.57647",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.97779,
            "range": "± 744.857",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.69486,
            "range": "± 1.31958",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.90626,
            "range": "± 468.863",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.1633,
            "range": "± 8.7888",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.7173,
            "range": "± 7.93671",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.46472,
            "range": "± 36.0427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.48403,
            "range": "± 39.3481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 95.0632,
            "range": "± 12.0176",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.4002,
            "range": "± 9.26302",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.33354,
            "range": "± 78.6792",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.27959,
            "range": "± 43.6834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.0187,
            "range": "± 3.55892",
            "unit": "ns",
            "extra": "100 samples\n844 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 348.513,
            "range": "± 17.0535",
            "unit": "ns",
            "extra": "100 samples\n8584 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.17086,
            "range": "± 0.208111",
            "unit": "ns",
            "extra": "100 samples\n7857 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.7358,
            "range": "± 1.77894",
            "unit": "ns",
            "extra": "100 samples\n802 iterations"
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
      }
    ]
  }
}