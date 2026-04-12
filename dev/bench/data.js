window.BENCHMARK_DATA = {
  "lastUpdate": 1775976069065,
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
          "id": "c01247b4b72cf75d04e2fcb4bd9c5607a902e5f2",
          "message": "Add SIMD Vector API with hardware-auto-detected lane widths (#207)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-04-04T15:41:30+02:00",
          "tree_id": "ece608dcffe5ab5b899e653d471f7129485969d6",
          "url": "https://github.com/nebulastream/nautilus/commit/c01247b4b72cf75d04e2fcb4bd9c5607a902e5f2"
        },
        "date": 1775310531164,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 37.5093,
            "range": "± 11.7494",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.19571,
            "range": "± 167.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.52067,
            "range": "± 245.618",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.6538,
            "range": "± 358.325",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.8139,
            "range": "± 1.83892",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.51813,
            "range": "± 747.257",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 37.1472,
            "range": "± 6.76066",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 17.0804,
            "range": "± 3.53069",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.7774,
            "range": "± 1.9669",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.66643,
            "range": "± 1.08795",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.9594,
            "range": "± 3.28725",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.6377,
            "range": "± 1.71834",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.861,
            "range": "± 1.96453",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.53426,
            "range": "± 799.207",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.179,
            "range": "± 3.86525",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.9649,
            "range": "± 2.0581",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.83869,
            "range": "± 1.37239",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.32965,
            "range": "± 1.35495",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.0407,
            "range": "± 1.88404",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.10904,
            "range": "± 961.419",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.7828,
            "range": "± 1.7374",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.64365,
            "range": "± 522.599",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.6997,
            "range": "± 7.73545",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 58.0938,
            "range": "± 8.82738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80438,
            "range": "± 39.6808",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.81009,
            "range": "± 37.214",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 140.21,
            "range": "± 12.5902",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 71.212,
            "range": "± 9.26146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.15008,
            "range": "± 37.6043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.86089,
            "range": "± 51.0819",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.3849,
            "range": "± 9.06161",
            "unit": "ns",
            "extra": "100 samples\n871 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 282.268,
            "range": "± 7.59911",
            "unit": "ns",
            "extra": "100 samples\n8265 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.05985,
            "range": "± 0.739266",
            "unit": "ns",
            "extra": "100 samples\n7746 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.3263,
            "range": "± 2.05037",
            "unit": "ns",
            "extra": "100 samples\n739 iterations"
          },
          {
            "name": "ssa_add",
            "value": 206.987,
            "range": "± 29.7624",
            "unit": "ns",
            "extra": "100 samples\n156 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 507.968,
            "range": "± 67.906",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.2589,
            "range": "± 139.2",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 512.462,
            "range": "± 42.9444",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 978.605,
            "range": "± 90.4827",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 528.28,
            "range": "± 61.6351",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 636.582,
            "range": "± 92.2697",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 498.308,
            "range": "± 51.1977",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 537.388,
            "range": "± 52.4788",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 490.895,
            "range": "± 82.9043",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 42.5661,
            "range": "± 12.2529",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.56363,
            "range": "± 204.754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.3529,
            "range": "± 376.871",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.2573,
            "range": "± 11.3992",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 61.9797,
            "range": "± 12.8559",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.3116,
            "range": "± 164.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.3052,
            "range": "± 333.381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 60.6621,
            "range": "± 13.2576",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.008,
            "range": "± 1.2622",
            "unit": "ns",
            "extra": "100 samples\n9292 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.7086,
            "range": "± 2.13282",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 561.635,
            "range": "± 24.7226",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.70719,
            "range": "± 0.588182",
            "unit": "ns",
            "extra": "100 samples\n6551 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.611,
            "range": "± 3.80368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1303,
            "range": "± 533.696",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 55.5753,
            "range": "± 15.5573",
            "unit": "ns",
            "extra": "100 samples\n667 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 932.445,
            "range": "± 28.0037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.151,
            "range": "± 1.34432",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.23032,
            "range": "± 0.307965",
            "unit": "ns",
            "extra": "100 samples\n9268 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.5547,
            "range": "± 2.23995",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.61075,
            "range": "± 40.9482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 915.956,
            "range": "± 119.273",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.5734,
            "range": "± 292.207",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.87058,
            "range": "± 634.436",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.06223,
            "range": "± 361.375",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.91892,
            "range": "± 738.714",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.30114,
            "range": "± 364.361",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.90241,
            "range": "± 328.825",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.30728,
            "range": "± 272.394",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.25706,
            "range": "± 357.786",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.75793,
            "range": "± 297.114",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.4979,
            "range": "± 1.85667",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 196.116,
            "range": "± 16.1689",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.2126,
            "range": "± 2.07206",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 372.703,
            "range": "± 10.9432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.25938,
            "range": "± 184.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.83948,
            "range": "± 177.957",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.78595,
            "range": "± 212.777",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.0365,
            "range": "± 327.485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.429,
            "range": "± 343.986",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7347,
            "range": "± 216.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.13,
            "range": "± 294.021",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.63437,
            "range": "± 172.018",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.6997,
            "range": "± 354.395",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1509,
            "range": "± 201.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1972,
            "range": "± 204.189",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.7343,
            "range": "± 269.018",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5453,
            "range": "± 829.926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.6239,
            "range": "± 333.055",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.866,
            "range": "± 438.323",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.0439,
            "range": "± 482.648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.2588,
            "range": "± 426.676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.1953,
            "range": "± 282.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.2569,
            "range": "± 493.711",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.7647,
            "range": "± 373.636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.5196,
            "range": "± 309.284",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.3795,
            "range": "± 521.078",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.4091,
            "range": "± 341.807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.2101,
            "range": "± 367.548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.4624,
            "range": "± 394.264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.783,
            "range": "± 397.881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.7236,
            "range": "± 618.486",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.5733,
            "range": "± 532.692",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2352,
            "range": "± 2.37181",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.369,
            "range": "± 2.88438",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.1564,
            "range": "± 3.70898",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.8152,
            "range": "± 3.42614",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.6383,
            "range": "± 4.07876",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.9324,
            "range": "± 3.463",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8306,
            "range": "± 4.67596",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.2518,
            "range": "± 4.40826",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.3354,
            "range": "± 4.51067",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.6809,
            "range": "± 3.31926",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.7376,
            "range": "± 4.99439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 175.939,
            "range": "± 11.7728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 48.2046,
            "range": "± 7.65958",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 279.616,
            "range": "± 16.7199",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.3334,
            "range": "± 5.82024",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.8085,
            "range": "± 4.55905",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 58.1518,
            "range": "± 10.5121",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.476,
            "range": "± 5.10508",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.8955,
            "range": "± 9.42696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.666,
            "range": "± 8.06252",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.0129,
            "range": "± 8.49623",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.6994,
            "range": "± 4.60402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.9836,
            "range": "± 8.35777",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.3579,
            "range": "± 6.11817",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 109.6,
            "range": "± 13.0658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.12905,
            "range": "± 22.2548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 164.837,
            "range": "± 19.0884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.27325,
            "range": "± 47.0784",
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
          "id": "8302e6a1ee55d977e3ee46e2ae72b5dbf7fb1fb4",
          "message": "Add SIMD plugin documentation (#217)",
          "timestamp": "2026-04-06T14:36:31+02:00",
          "tree_id": "37913c9964f589880977bfb0d7818e404f9d6601",
          "url": "https://github.com/nebulastream/nautilus/commit/8302e6a1ee55d977e3ee46e2ae72b5dbf7fb1fb4"
        },
        "date": 1775479256818,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 11.0949,
            "range": "± 1.29632",
            "unit": "ns",
            "extra": "100 samples\n9290 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.8568,
            "range": "± 3.22702",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 304.314,
            "range": "± 42.761",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.75988,
            "range": "± 0.646483",
            "unit": "ns",
            "extra": "100 samples\n6573 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.8324,
            "range": "± 14.0666",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.585,
            "range": "± 151.436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.1457,
            "range": "± 4.11157",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 929.348,
            "range": "± 7.95643",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.54,
            "range": "± 1.17177",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.30991,
            "range": "± 0.469976",
            "unit": "ns",
            "extra": "100 samples\n9264 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.0088,
            "range": "± 3.63647",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.32247,
            "range": "± 169.648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.69883,
            "range": "± 496.385",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.6678,
            "range": "± 478.088",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.0363,
            "range": "± 2.05028",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.63088,
            "range": "± 981.96",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.7238,
            "range": "± 7.3392",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.4805,
            "range": "± 3.44461",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.9167,
            "range": "± 2.0218",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.60814,
            "range": "± 1.01262",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.1897,
            "range": "± 3.74648",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.7107,
            "range": "± 2.18864",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.0042,
            "range": "± 2.2455",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.68309,
            "range": "± 1.02179",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.3428,
            "range": "± 3.79065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.5797,
            "range": "± 2.78419",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.9262,
            "range": "± 1.70775",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.85515,
            "range": "± 2.04627",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.5597,
            "range": "± 2.34349",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.09075,
            "range": "± 1.22535",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.8977,
            "range": "± 1.93006",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.84332,
            "range": "± 783.821",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 60.4644,
            "range": "± 13.4597",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 58.401,
            "range": "± 10.7811",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.782,
            "range": "± 70.0337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.80402,
            "range": "± 52.6496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 137.9,
            "range": "± 13.4767",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.2092,
            "range": "± 12.7566",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.11369,
            "range": "± 56.9167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.80445,
            "range": "± 60.1348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 44.1249,
            "range": "± 12.4366",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.46514,
            "range": "± 322.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 43.7296,
            "range": "± 12.9616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.60338,
            "range": "± 409.761",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.0542,
            "range": "± 306.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 43.9086,
            "range": "± 11.7182",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.7134,
            "range": "± 14.4821",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.7102,
            "range": "± 532.381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.7358,
            "range": "± 746.575",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 62.6638,
            "range": "± 12.4114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 211.479,
            "range": "± 31.8818",
            "unit": "ns",
            "extra": "100 samples\n148 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 512.135,
            "range": "± 63.1464",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.32127,
            "range": "± 212.152",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 552.615,
            "range": "± 81.4632",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1021.2300000000001,
            "range": "± 132615",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 577.075,
            "range": "± 106.142",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 656.92,
            "range": "± 75.6936",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 539.673,
            "range": "± 73.6582",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 571.164,
            "range": "± 93.2951",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 501.533,
            "range": "± 67.6632",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.156,
            "range": "± 5.89026",
            "unit": "ns",
            "extra": "100 samples\n845 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 285.781,
            "range": "± 9.63109",
            "unit": "ns",
            "extra": "100 samples\n8261 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.00596,
            "range": "± 0.516395",
            "unit": "ns",
            "extra": "100 samples\n7690 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.3042,
            "range": "± 5.20693",
            "unit": "ns",
            "extra": "100 samples\n739 iterations"
          },
          {
            "name": "ir_add",
            "value": 973.043,
            "range": "± 133.547",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.69755,
            "range": "± 357.066",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.31856,
            "range": "± 879.841",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.28548,
            "range": "± 613.217",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.08138,
            "range": "± 713.1",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.49128,
            "range": "± 508.537",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.15825,
            "range": "± 495.352",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.47799,
            "range": "± 384.236",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.38408,
            "range": "± 395.082",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.92172,
            "range": "± 431.83",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.7668,
            "range": "± 2.63009",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 187.918,
            "range": "± 6.65592",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.5366,
            "range": "± 2.86585",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 362.111,
            "range": "± 11.237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.5278,
            "range": "± 446.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.68938,
            "range": "± 660.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.92133,
            "range": "± 275.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.4036,
            "range": "± 692.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.7283,
            "range": "± 768.94",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.3064,
            "range": "± 663.198",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.9114,
            "range": "± 812.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.69908,
            "range": "± 196.931",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.2986,
            "range": "± 494.455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.645,
            "range": "± 418.269",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.4618,
            "range": "± 428.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.7697,
            "range": "± 450.524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.1322,
            "range": "± 476.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.354,
            "range": "± 439.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.3992,
            "range": "± 2.44574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.1179,
            "range": "± 841.935",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5921,
            "range": "± 585.524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.3064,
            "range": "± 677.098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.5503,
            "range": "± 430.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.8797,
            "range": "± 624.042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.5454,
            "range": "± 652.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.3639,
            "range": "± 472.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.4163,
            "range": "± 1.13507",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.4519,
            "range": "± 569.524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.6842,
            "range": "± 753.414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.1642,
            "range": "± 777.118",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.3795,
            "range": "± 684.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.2251,
            "range": "± 829.678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.5829,
            "range": "± 3.42948",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.1192,
            "range": "± 3.95203",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.2548,
            "range": "± 4.20626",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.372,
            "range": "± 3.49844",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.2462,
            "range": "± 4.50687",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.4428,
            "range": "± 4.21588",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.2392,
            "range": "± 3.80031",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.3533,
            "range": "± 3.34723",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.4348,
            "range": "± 2.99071",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.26,
            "range": "± 3.70398",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.887,
            "range": "± 4.87788",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 181.696,
            "range": "± 11.4359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.5142,
            "range": "± 7.66123",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 283.02,
            "range": "± 14.0898",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.7785,
            "range": "± 4.68244",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.3737,
            "range": "± 5.34917",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 65.5983,
            "range": "± 16.1109",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 36.1569,
            "range": "± 5.90865",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 60.5369,
            "range": "± 10.7703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 50.2692,
            "range": "± 10.3124",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.2805,
            "range": "± 9.38789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.8958,
            "range": "± 5.03721",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.8373,
            "range": "± 8.58599",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.0785,
            "range": "± 6.50274",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 109.977,
            "range": "± 14.9309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.14825,
            "range": "± 33.5141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 164.515,
            "range": "± 18.0223",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.31821,
            "range": "± 58.2365",
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
          "id": "352c5580baf648b60e28ae3110968bda98bc7544",
          "message": "Apply clang-format to unrelated files\n\nhttps://claude.ai/code/session_01LAwmrK8Utd9Kp5mi1uaPs3",
          "timestamp": "2026-04-06T20:53:07+02:00",
          "tree_id": "da3cee748dde33e6619545d03fdae5fe521de4e5",
          "url": "https://github.com/nebulastream/nautilus/commit/352c5580baf648b60e28ae3110968bda98bc7544"
        },
        "date": 1775501967683,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 197.996,
            "range": "± 15.3424",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 485.467,
            "range": "± 38.8075",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.1944,
            "range": "± 101.957",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 535.911,
            "range": "± 67.5563",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 947.762,
            "range": "± 93.031",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 541.984,
            "range": "± 72.9355",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 659.023,
            "range": "± 91.7085",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 488.1,
            "range": "± 48.9968",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 562.94,
            "range": "± 90.5853",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 481.651,
            "range": "± 49.4868",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ir_add",
            "value": 860.35,
            "range": "± 114.208",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.4826,
            "range": "± 365.107",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.26128,
            "range": "± 699.529",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.83794,
            "range": "± 385.332",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.34069,
            "range": "± 639.134",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.08219,
            "range": "± 373.595",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.581,
            "range": "± 431.039",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.14755,
            "range": "± 275.846",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.96241,
            "range": "± 277.225",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.45073,
            "range": "± 196.055",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.3263,
            "range": "± 2.08175",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 194.347,
            "range": "± 7.82663",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.1262,
            "range": "± 2.03167",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 346.78,
            "range": "± 19.1017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.699,
            "range": "± 8.46211",
            "unit": "ns",
            "extra": "100 samples\n874 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 303.277,
            "range": "± 12.885",
            "unit": "ns",
            "extra": "100 samples\n8262 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.12269,
            "range": "± 0.745043",
            "unit": "ns",
            "extra": "100 samples\n7749 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 41.1195,
            "range": "± 3.06145",
            "unit": "ns",
            "extra": "100 samples\n740 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.9769,
            "range": "± 1.73019",
            "unit": "ns",
            "extra": "100 samples\n9299 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 18.097,
            "range": "± 2.52045",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 599.899,
            "range": "± 27.6775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.70582,
            "range": "± 0.543321",
            "unit": "ns",
            "extra": "100 samples\n6570 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.3241,
            "range": "± 9.62618",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9882,
            "range": "± 163.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.8428,
            "range": "± 8.50935",
            "unit": "ns",
            "extra": "100 samples\n680 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 931.495,
            "range": "± 13.3546",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.418,
            "range": "± 2.27817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.22116,
            "range": "± 0.242953",
            "unit": "ns",
            "extra": "100 samples\n9278 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.5178,
            "range": "± 2.48231",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.60166,
            "range": "± 22.5888",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.58175,
            "range": "± 275.606",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.67553,
            "range": "± 348.554",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.8185,
            "range": "± 1.88208",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.68939,
            "range": "± 833.812",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.583,
            "range": "± 6.93635",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.5829,
            "range": "± 3.08222",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.8006,
            "range": "± 1.91169",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.57965,
            "range": "± 783.872",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.776,
            "range": "± 4.03088",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 11.1141,
            "range": "± 1.93705",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9839,
            "range": "± 2.10249",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.70684,
            "range": "± 846.956",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.627,
            "range": "± 3.57341",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.1024,
            "range": "± 2.25722",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.74095,
            "range": "± 1.55949",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.70536,
            "range": "± 1.52494",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.2698,
            "range": "± 1.9188",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.17949,
            "range": "± 1.10894",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.9101,
            "range": "± 2.05256",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.93815,
            "range": "± 656.703",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 58.527,
            "range": "± 10.0834",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 58.4304,
            "range": "± 10.1079",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80002,
            "range": "± 46.8973",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.82541,
            "range": "± 67.6479",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 142.196,
            "range": "± 16.0084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.2396,
            "range": "± 8.52253",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.21646,
            "range": "± 119.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.85235,
            "range": "± 52.2163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 42.0569,
            "range": "± 11.5492",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.98367,
            "range": "± 313.796",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.7879,
            "range": "± 486.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 43.3356,
            "range": "± 12.367",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.7502,
            "range": "± 14.5309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.59025,
            "range": "± 356.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.22,
            "range": "± 539.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.2788,
            "range": "± 14.177",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 45.0188,
            "range": "± 12.8745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.74427,
            "range": "± 296.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.83876,
            "range": "± 411.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.73305,
            "range": "± 572.924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.26594,
            "range": "± 245.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.956,
            "range": "± 434.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.0965,
            "range": "± 393.348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.8403,
            "range": "± 372.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.2705,
            "range": "± 290.392",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.46269,
            "range": "± 251.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1124,
            "range": "± 427.804",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3348,
            "range": "± 559.006",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1637,
            "range": "± 347.778",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.764,
            "range": "± 646.982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3911,
            "range": "± 1.07049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.1835,
            "range": "± 817.802",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.0322,
            "range": "± 563.328",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.673,
            "range": "± 1.15544",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.0853,
            "range": "± 898.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.7405,
            "range": "± 1.53768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.3156,
            "range": "± 1.38646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.9421,
            "range": "± 885.506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.6826,
            "range": "± 704.793",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.622,
            "range": "± 555.917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.8996,
            "range": "± 1.01031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.13,
            "range": "± 647.206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.959,
            "range": "± 814.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.4564,
            "range": "± 993.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.297,
            "range": "± 3.17436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.3257,
            "range": "± 3.05763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2299,
            "range": "± 2.52326",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.8833,
            "range": "± 3.08254",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.182,
            "range": "± 3.64848",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.0334,
            "range": "± 3.07005",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.7041,
            "range": "± 3.28257",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.9515,
            "range": "± 3.42844",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.055,
            "range": "± 3.51884",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.1561,
            "range": "± 3.53716",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.2814,
            "range": "± 2.93456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.0889,
            "range": "± 3.27674",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.6386,
            "range": "± 5.27093",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 182.805,
            "range": "± 9.49706",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.2286,
            "range": "± 8.44933",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 296.153,
            "range": "± 13.141",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.304,
            "range": "± 3.7668",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.1666,
            "range": "± 5.95938",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.6448,
            "range": "± 10.3669",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 37.2019,
            "range": "± 6.43815",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.8717,
            "range": "± 8.03486",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.1629,
            "range": "± 8.34024",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.0719,
            "range": "± 9.23191",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.0879,
            "range": "± 4.93912",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.9552,
            "range": "± 8.1797",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.0631,
            "range": "± 5.10189",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 111.494,
            "range": "± 15.1638",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.15723,
            "range": "± 22.2506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 163.651,
            "range": "± 14.0831",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.2936,
            "range": "± 47.3037",
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
          "id": "055aaf8e4c492113b6cca45ab20b8bb8dbcabe46",
          "message": "Harden clang-format config for deterministic output across versions 17-22 (#218)",
          "timestamp": "2026-04-06T20:59:49+02:00",
          "tree_id": "d2e9fb2c3e2db16ffe031361efcd543b3cd91ee0",
          "url": "https://github.com/nebulastream/nautilus/commit/055aaf8e4c492113b6cca45ab20b8bb8dbcabe46"
        },
        "date": 1775502254415,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 40.8219,
            "range": "± 10.2671",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.39239,
            "range": "± 208.817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.0636,
            "range": "± 1.43427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 45.2124,
            "range": "± 12.4175",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 65.4603,
            "range": "± 15.2319",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.56268,
            "range": "± 290.22",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.0645,
            "range": "± 653.72",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 61.7847,
            "range": "± 11.4975",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 952.131,
            "range": "± 142.933",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.72421,
            "range": "± 399.43",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.02347,
            "range": "± 972.224",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.1782,
            "range": "± 497.214",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.10768,
            "range": "± 861.204",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.4719,
            "range": "± 575.425",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.07202,
            "range": "± 627.351",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.57713,
            "range": "± 367.904",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.58982,
            "range": "± 582.317",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.91173,
            "range": "± 552.405",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.6092,
            "range": "± 1.92952",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 198.243,
            "range": "± 16.3941",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.7462,
            "range": "± 4.02096",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 368.279,
            "range": "± 14.8378",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.96961,
            "range": "± 1.44413",
            "unit": "ns",
            "extra": "100 samples\n9294 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.7465,
            "range": "± 2.06326",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 545.892,
            "range": "± 32.6696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.85193,
            "range": "± 0.904165",
            "unit": "ns",
            "extra": "100 samples\n6549 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.8871,
            "range": "± 8.79456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9396,
            "range": "± 194.549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.6392,
            "range": "± 7.08465",
            "unit": "ns",
            "extra": "100 samples\n706 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 930.6,
            "range": "± 19.1582",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.891,
            "range": "± 925.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.31767,
            "range": "± 0.433066",
            "unit": "ns",
            "extra": "100 samples\n9285 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.7547,
            "range": "± 2.2252",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.60285,
            "range": "± 30.4842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.10993,
            "range": "± 164.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.86807,
            "range": "± 388.804",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.82952,
            "range": "± 214.772",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.92963,
            "range": "± 252.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.8341,
            "range": "± 429.793",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.8958,
            "range": "± 1.46471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.5651,
            "range": "± 553.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.60849,
            "range": "± 178.2",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9928,
            "range": "± 198.75",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9902,
            "range": "± 228.892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9994,
            "range": "± 199.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.7364,
            "range": "± 705.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.8219,
            "range": "± 810.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.8473,
            "range": "± 1.93751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.1864,
            "range": "± 14.7671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0561,
            "range": "± 1.25659",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.2789,
            "range": "± 1.313",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.4708,
            "range": "± 986.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.6307,
            "range": "± 826.056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.9329,
            "range": "± 1.16913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.5335,
            "range": "± 799.577",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.3937,
            "range": "± 472.626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.8756,
            "range": "± 629.149",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.9237,
            "range": "± 790.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.5314,
            "range": "± 357.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.0045,
            "range": "± 697.356",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.1412,
            "range": "± 886.961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.6757,
            "range": "± 963.829",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3287,
            "range": "± 2.11704",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.9925,
            "range": "± 2.85467",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.7038,
            "range": "± 3.55309",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.094,
            "range": "± 2.91074",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8977,
            "range": "± 3.59419",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.8667,
            "range": "± 3.33034",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.7571,
            "range": "± 3.30721",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.7986,
            "range": "± 3.35942",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 21.1434,
            "range": "± 7.02328",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.2991,
            "range": "± 3.06684",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.6376,
            "range": "± 3.9749",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 177.111,
            "range": "± 10.0545",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.7277,
            "range": "± 6.60081",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 277.834,
            "range": "± 13.4702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.1746,
            "range": "± 3.5353",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.6714,
            "range": "± 3.34299",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.0769,
            "range": "± 9.74302",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 36.3133,
            "range": "± 4.98348",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 60.1246,
            "range": "± 10.9729",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.1773,
            "range": "± 6.44551",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.445,
            "range": "± 7.80666",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.5894,
            "range": "± 4.21905",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.7716,
            "range": "± 7.35899",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.5174,
            "range": "± 6.38312",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 110.487,
            "range": "± 14.9513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.14159,
            "range": "± 21.2036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 164.136,
            "range": "± 14.5019",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.26915,
            "range": "± 36.1861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.2908,
            "range": "± 7.13975",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.03213,
            "range": "± 147.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.68081,
            "range": "± 444.989",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.70152,
            "range": "± 435.298",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.0379,
            "range": "± 1.90498",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.72585,
            "range": "± 1.08273",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 37.9468,
            "range": "± 9.13646",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 17.4281,
            "range": "± 3.67035",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.5092,
            "range": "± 1.50485",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.77241,
            "range": "± 1.09175",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.0699,
            "range": "± 2.66769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.9058,
            "range": "± 1.78562",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.0801,
            "range": "± 2.29458",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 6.22781,
            "range": "± 1.20073",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.2457,
            "range": "± 2.24963",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.6065,
            "range": "± 2.56058",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.0378,
            "range": "± 2.32557",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.54248,
            "range": "± 1.49079",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.4966,
            "range": "± 1.46679",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.60998,
            "range": "± 1.42154",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.1104,
            "range": "± 1.90572",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 5.18658,
            "range": "± 943.426",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.8577,
            "range": "± 9.89344",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 59.1012,
            "range": "± 11.2248",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.76782,
            "range": "± 109.454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.79006,
            "range": "± 76.3801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 139.638,
            "range": "± 15.5514",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 73.9599,
            "range": "± 13.1239",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.10843,
            "range": "± 65.4746",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.90835,
            "range": "± 413.399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.6122,
            "range": "± 5.77437",
            "unit": "ns",
            "extra": "100 samples\n877 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.858,
            "range": "± 5.73372",
            "unit": "ns",
            "extra": "100 samples\n8301 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.0582,
            "range": "± 0.557762",
            "unit": "ns",
            "extra": "100 samples\n7742 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.3021,
            "range": "± 4.9286",
            "unit": "ns",
            "extra": "100 samples\n740 iterations"
          },
          {
            "name": "ssa_add",
            "value": 238.203,
            "range": "± 47.8256",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 509.73,
            "range": "± 69.9686",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.28702,
            "range": "± 221.447",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 624.589,
            "range": "± 143.694",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1030.4,
            "range": "± 187870",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 562.178,
            "range": "± 105.013",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 637.696,
            "range": "± 92.5994",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 543.019,
            "range": "± 96.7618",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 551.1,
            "range": "± 79.4328",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 497.515,
            "range": "± 75.5493",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
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
          "id": "e24c921478a4edb5ba187e32fd2063b23fc32066",
          "message": "Move std library wrappers from core to plugins/std plugin\n\nExtract all standard library wrappers (cmath, cstring, cstdlib, string,\nstring_view, ostream, iostream, sstream, bit) into a standalone plugin at\nplugins/std/, following the existing SIMD plugin pattern.\n\nKey changes:\n- Create val_memcpy.hpp in core for memcpy/memmove/memset (needed by val_std.hpp)\n- Move std headers, sources, and tests to plugins/std/\n- Move MLIR math and bit intrinsics to the plugin with static auto-registration\n- Split LLVMIRIntrinsicTest: cmath/bit tests to plugin, assume+memory stay in core\n- Add std and simd plugin tests to CI workflow (build-test and llvm-ir-test jobs)\n- Gate plugin LLVM IR tests behind ENABLE_LLVM_IR_TEST to match core pattern\n\nhttps://claude.ai/code/session_01B9gVGqn3z3mWW2LnzfY9mw",
          "timestamp": "2026-04-06T21:13:31+02:00",
          "tree_id": "27b05ea890de96c5dc020442d3ac24ca331f8f31",
          "url": "https://github.com/nebulastream/nautilus/commit/e24c921478a4edb5ba187e32fd2063b23fc32066"
        },
        "date": 1775503133187,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.1973,
            "range": "± 176.856",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.93683,
            "range": "± 314.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.81673,
            "range": "± 359.616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.87719,
            "range": "± 271.969",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.5803,
            "range": "± 1.56757",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.0732,
            "range": "± 228.908",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.4304,
            "range": "± 946.932",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.59535,
            "range": "± 316.424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4529,
            "range": "± 747.48",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.0623,
            "range": "± 564.749",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.8636,
            "range": "± 694.678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5924,
            "range": "± 685.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9839,
            "range": "± 238.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.6165,
            "range": "± 917.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.8485,
            "range": "± 1.00115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.4728,
            "range": "± 880.265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.0405,
            "range": "± 1.51009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.8189,
            "range": "± 700.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.6214,
            "range": "± 1.12221",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.2575,
            "range": "± 895.388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.1165,
            "range": "± 988.827",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.0508,
            "range": "± 835.359",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.0566,
            "range": "± 984.521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.584,
            "range": "± 921.07",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.5021,
            "range": "± 849.768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.8561,
            "range": "± 1.31875",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.0901,
            "range": "± 693.543",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.0201,
            "range": "± 2.92112",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2555,
            "range": "± 2.24709",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.0961,
            "range": "± 3.78205",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.1308,
            "range": "± 3.62242",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.9015,
            "range": "± 3.63801",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.74,
            "range": "± 3.54377",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.3625,
            "range": "± 3.32729",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.974,
            "range": "± 3.51505",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.0525,
            "range": "± 3.87101",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.1608,
            "range": "± 3.89798",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.794,
            "range": "± 2.78211",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.9381,
            "range": "± 5.04679",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 178.485,
            "range": "± 9.09511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.8698,
            "range": "± 7.8923",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 279.998,
            "range": "± 21.7794",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.9124,
            "range": "± 5.11509",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.044,
            "range": "± 4.62756",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.1315,
            "range": "± 11.188",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.8256,
            "range": "± 6.0075",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.1045,
            "range": "± 8.29967",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.3398,
            "range": "± 7.30291",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.9177,
            "range": "± 10.7004",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3085,
            "range": "± 4.60654",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.2049,
            "range": "± 8.98617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.4254,
            "range": "± 4.03928",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 107.014,
            "range": "± 8.11123",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.15836,
            "range": "± 23.4189",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 164.941,
            "range": "± 14.2742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.28219,
            "range": "± 49.4911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.1769,
            "range": "± 4.18603",
            "unit": "ns",
            "extra": "100 samples\n867 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.766,
            "range": "± 5.11231",
            "unit": "ns",
            "extra": "100 samples\n8311 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.00391,
            "range": "± 0.505738",
            "unit": "ns",
            "extra": "100 samples\n7756 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3574,
            "range": "± 1.92835",
            "unit": "ns",
            "extra": "100 samples\n759 iterations"
          },
          {
            "name": "ssa_add",
            "value": 192.819,
            "range": "± 11.7736",
            "unit": "ns",
            "extra": "100 samples\n155 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 496.671,
            "range": "± 50.5799",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.25998,
            "range": "± 100.091",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 504.611,
            "range": "± 39.1384",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1001.33,
            "range": "± 183068",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 526.024,
            "range": "± 45.1197",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 631.96,
            "range": "± 50.9461",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 493.18,
            "range": "± 29.9934",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 538.866,
            "range": "± 41.8463",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 472.387,
            "range": "± 45.3456",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.61465,
            "range": "± 1.01765",
            "unit": "ns",
            "extra": "100 samples\n9311 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.2754,
            "range": "± 1.11295",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 567.099,
            "range": "± 31.7775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.70057,
            "range": "± 0.557549",
            "unit": "ns",
            "extra": "100 samples\n6573 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.6919,
            "range": "± 8.48715",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9566,
            "range": "± 266.447",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.7939,
            "range": "± 4.78789",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 1180.25,
            "range": "± 35263.5",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 200.664,
            "range": "± 2.48652",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.28935,
            "range": "± 0.388025",
            "unit": "ns",
            "extra": "100 samples\n9308 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.5321,
            "range": "± 2.21397",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.61273,
            "range": "± 17.6026",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.1154,
            "range": "± 11.1789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 7.99416,
            "range": "± 190.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 854.965,
            "range": "± 76.8673",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.51798,
            "range": "± 206.38",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.71363,
            "range": "± 564.617",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.94653,
            "range": "± 247.833",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.72456,
            "range": "± 613.66",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.22726,
            "range": "± 332.856",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.90448,
            "range": "± 451.255",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.26051,
            "range": "± 249.168",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.09581,
            "range": "± 257.205",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.63914,
            "range": "± 233.297",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.2053,
            "range": "± 2.23327",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 185.502,
            "range": "± 7.26041",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.6137,
            "range": "± 1.93863",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 354.247,
            "range": "± 9.7045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.48326,
            "range": "± 283.067",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.58834,
            "range": "± 376.18",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.755,
            "range": "± 1.63638",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.27671,
            "range": "± 613.577",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.9139,
            "range": "± 6.77411",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.7521,
            "range": "± 2.65288",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3559,
            "range": "± 1.77262",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.51315,
            "range": "± 904.126",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 31.7752,
            "range": "± 11.1148",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.5193,
            "range": "± 1.71284",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.6273,
            "range": "± 2.02183",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 6.53844,
            "range": "± 2.06109",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.855,
            "range": "± 3.5994",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.3283,
            "range": "± 1.77297",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.54334,
            "range": "± 1.26016",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.08912,
            "range": "± 1.21896",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.3569,
            "range": "± 2.18503",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.94265,
            "range": "± 1.03652",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.9212,
            "range": "± 2.63702",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.65901,
            "range": "± 695.694",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.4527,
            "range": "± 9.25873",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.9558,
            "range": "± 7.9972",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.78276,
            "range": "± 45.1149",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.79773,
            "range": "± 37.9156",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 138.849,
            "range": "± 11.2823",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.5203,
            "range": "± 8.21777",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.13737,
            "range": "± 47.4472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.80729,
            "range": "± 105.289",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 41.2705,
            "range": "± 10.7492",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 7.04706,
            "range": "± 964.368",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.598,
            "range": "± 1.26133",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.3922,
            "range": "± 10.2238",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 60.8961,
            "range": "± 12.913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.12534,
            "range": "± 210.177",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.1955,
            "range": "± 982.256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 60.3862,
            "range": "± 11.9194",
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
          "id": "92a309d16b6c651e909c117dc3453fde1ea45f8c",
          "message": "Fix SIMD LLVM IR test determinism by allowing explicit SIMD width override (#220)",
          "timestamp": "2026-04-07T07:37:51+02:00",
          "tree_id": "ac5aca45ec17f35d68d0f05fe4f8a8e8292e70d6",
          "url": "https://github.com/nebulastream/nautilus/commit/92a309d16b6c651e909c117dc3453fde1ea45f8c"
        },
        "date": 1775540552296,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 36.6874,
            "range": "± 7.2539",
            "unit": "ns",
            "extra": "100 samples\n866 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 287.388,
            "range": "± 7.32333",
            "unit": "ns",
            "extra": "100 samples\n8273 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.06732,
            "range": "± 0.815504",
            "unit": "ns",
            "extra": "100 samples\n7738 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.4068,
            "range": "± 3.2596",
            "unit": "ns",
            "extra": "100 samples\n736 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.47104,
            "range": "± 342.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.92684,
            "range": "± 161.192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.82682,
            "range": "± 404.547",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.99092,
            "range": "± 237.891",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.4939,
            "range": "± 769.467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5091,
            "range": "± 172.876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.1756,
            "range": "± 544.44",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.73651,
            "range": "± 145.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.3227,
            "range": "± 219.067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3131,
            "range": "± 593.201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.4398,
            "range": "± 291.164",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.9337,
            "range": "± 341.96",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3062,
            "range": "± 178.479",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.1753,
            "range": "± 241.251",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.8816,
            "range": "± 399.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.4246,
            "range": "± 546.807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.679,
            "range": "± 441.353",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.0211,
            "range": "± 677.87",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.4496,
            "range": "± 670.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.3314,
            "range": "± 617.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.1428,
            "range": "± 477.672",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.0579,
            "range": "± 722.331",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.2855,
            "range": "± 881.683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.1318,
            "range": "± 922.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.6545,
            "range": "± 433.228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.094,
            "range": "± 546.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.693,
            "range": "± 536.344",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.614,
            "range": "± 817.142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4068,
            "range": "± 1.94865",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.0381,
            "range": "± 3.30543",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.9951,
            "range": "± 4.04337",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.6168,
            "range": "± 3.27078",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.9006,
            "range": "± 3.23723",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.8481,
            "range": "± 4.13657",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8033,
            "range": "± 2.89292",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.0596,
            "range": "± 2.82022",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.3798,
            "range": "± 3.30459",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.2771,
            "range": "± 4.01222",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.6268,
            "range": "± 5.81633",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 197.157,
            "range": "± 49.6627",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.6776,
            "range": "± 6.91874",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 292.861,
            "range": "± 58.8614",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.0246,
            "range": "± 4.57722",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.0066,
            "range": "± 4.48102",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.2918,
            "range": "± 11.5282",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.1978,
            "range": "± 5.25954",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 56.5594,
            "range": "± 9.2463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.0229,
            "range": "± 8.90342",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.8064,
            "range": "± 9.32558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 26.5244,
            "range": "± 4.16833",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 36.4696,
            "range": "± 4.73797",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.7499,
            "range": "± 5.28888",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 108.372,
            "range": "± 13.7405",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.12754,
            "range": "± 27.0996",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 162.489,
            "range": "± 16.618",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.27965,
            "range": "± 73.2176",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 43.698,
            "range": "± 11.7422",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 9.09382,
            "range": "± 414.449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.2807,
            "range": "± 1.49913",
            "unit": "ns",
            "extra": "100 samples\n9282 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.5726,
            "range": "± 1.69988",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 286.518,
            "range": "± 30.6608",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.69285,
            "range": "± 0.631452",
            "unit": "ns",
            "extra": "100 samples\n6582 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.816,
            "range": "± 7.47674",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.5382,
            "range": "± 100.346",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 45.7507,
            "range": "± 7.90134",
            "unit": "ns",
            "extra": "100 samples\n686 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 934.503,
            "range": "± 21.9487",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 199.28,
            "range": "± 3.28394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.20375,
            "range": "± 0.307005",
            "unit": "ns",
            "extra": "100 samples\n9227 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.1704,
            "range": "± 1.20195",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.28388,
            "range": "± 45.3866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 211.647,
            "range": "± 31.6308",
            "unit": "ns",
            "extra": "100 samples\n155 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 521.003,
            "range": "± 69.7136",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.29566,
            "range": "± 170.828",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 546.927,
            "range": "± 94.6412",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1022.9699999999999,
            "range": "± 136926",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 538.26,
            "range": "± 61.6161",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 635.787,
            "range": "± 54.6522",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 502.499,
            "range": "± 48.9051",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 555.756,
            "range": "± 59.0743",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 502.846,
            "range": "± 45.5372",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ir_add",
            "value": 913.07,
            "range": "± 145.373",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.64825,
            "range": "± 414.426",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.81127,
            "range": "± 658.965",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.04688,
            "range": "± 317.42",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.77302,
            "range": "± 613.77",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.27797,
            "range": "± 411.759",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.83442,
            "range": "± 376.735",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.30557,
            "range": "± 295.991",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.24536,
            "range": "± 382.049",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.77487,
            "range": "± 417.089",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.4664,
            "range": "± 2.51598",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 193.174,
            "range": "± 9.17763",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.4275,
            "range": "± 2.83754",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 368.126,
            "range": "± 35.1566",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.56071,
            "range": "± 334.508",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.63532,
            "range": "± 469.173",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.7338,
            "range": "± 1.91535",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.54651,
            "range": "± 793.494",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.6317,
            "range": "± 6.83659",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.5694,
            "range": "± 3.4388",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.6319,
            "range": "± 1.89029",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.57105,
            "range": "± 869.548",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.0483,
            "range": "± 3.9226",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 11.1597,
            "range": "± 2.01699",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.2248,
            "range": "± 2.29284",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.93206,
            "range": "± 1.08621",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.5478,
            "range": "± 3.95682",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.2601,
            "range": "± 2.31038",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.3203,
            "range": "± 2.06926",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.54397,
            "range": "± 1.34284",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.094,
            "range": "± 1.96057",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.06828,
            "range": "± 1.04146",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.8376,
            "range": "± 1.58884",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.98032,
            "range": "± 872.066",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 58.3678,
            "range": "± 9.3765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 57.8167,
            "range": "± 11.0514",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80391,
            "range": "± 34.8831",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.84061,
            "range": "± 53.4759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 139.585,
            "range": "± 9.40872",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.7252,
            "range": "± 10.943",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.16869,
            "range": "± 46.076",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.82114,
            "range": "± 43.9782",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 41.8962,
            "range": "± 9.03138",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.37638,
            "range": "± 210.48",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.6445,
            "range": "± 637.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 43.8269,
            "range": "± 11.1284",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.5813,
            "range": "± 13.807",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.22161,
            "range": "± 395.962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.6842,
            "range": "± 586.282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.7589,
            "range": "± 13.3952",
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
          "id": "b38d4c4d190274f139236b864bd7dcb651499435",
          "message": "Re-add equals-false vector tests, skipped on bc backend",
          "timestamp": "2026-04-07T09:58:18+02:00",
          "tree_id": "5b80c8cbfddb9e910ef6ded65864db4f0263f5ae",
          "url": "https://github.com/nebulastream/nautilus/commit/b38d4c4d190274f139236b864bd7dcb651499435"
        },
        "date": 1775548973313,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 195.377,
            "range": "± 21.1727",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 488.807,
            "range": "± 42.5563",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.20668,
            "range": "± 114.909",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 528.579,
            "range": "± 68.319",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 958.793,
            "range": "± 129.481",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 517.093,
            "range": "± 42.4258",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 625.164,
            "range": "± 67.303",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 500.569,
            "range": "± 50.0628",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 532.876,
            "range": "± 43.4361",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 467.502,
            "range": "± 35.9027",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.7432,
            "range": "± 5.18118",
            "unit": "ns",
            "extra": "100 samples\n875 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 280.9,
            "range": "± 4.57897",
            "unit": "ns",
            "extra": "100 samples\n8249 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.06336,
            "range": "± 0.669566",
            "unit": "ns",
            "extra": "100 samples\n7685 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.2554,
            "range": "± 2.39548",
            "unit": "ns",
            "extra": "100 samples\n763 iterations"
          },
          {
            "name": "ir_add",
            "value": 905.263,
            "range": "± 119.575",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.59711,
            "range": "± 294.23",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.8868,
            "range": "± 862.779",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.00995,
            "range": "± 241.152",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.80764,
            "range": "± 630.07",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.30416,
            "range": "± 348.496",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.85183,
            "range": "± 465.572",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.37431,
            "range": "± 373.796",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.31805,
            "range": "± 367.612",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.71809,
            "range": "± 232.153",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.3832,
            "range": "± 1.7381",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 196.319,
            "range": "± 8.78511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.3065,
            "range": "± 2.79216",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 369.176,
            "range": "± 28.7182",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.58098,
            "range": "± 301.657",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.49706,
            "range": "± 291.695",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.8976,
            "range": "± 2.00525",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.35751,
            "range": "± 797.615",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.5491,
            "range": "± 6.76236",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.1414,
            "range": "± 3.19244",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.7505,
            "range": "± 1.83658",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.2944,
            "range": "± 663.96",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.7953,
            "range": "± 3.26512",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.5853,
            "range": "± 1.92754",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.7023,
            "range": "± 1.9104",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.48007,
            "range": "± 819.457",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.9786,
            "range": "± 3.50323",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.0639,
            "range": "± 2.05056",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.44581,
            "range": "± 1.15896",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.49627,
            "range": "± 1.27852",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.251,
            "range": "± 2.21689",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.9321,
            "range": "± 1.05239",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.8337,
            "range": "± 1.60831",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 5.08691,
            "range": "± 1.56422",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 58.5348,
            "range": "± 7.84102",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 58.0439,
            "range": "± 8.26823",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.78578,
            "range": "± 55.2457",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.7822,
            "range": "± 44.3105",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 139.523,
            "range": "± 12.3638",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 73.1932,
            "range": "± 10.7681",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.08957,
            "range": "± 36.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.79659,
            "range": "± 47.0481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.0808,
            "range": "± 11.7903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.12419,
            "range": "± 186.727",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 41.3045,
            "range": "± 11.6778",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.19114,
            "range": "± 160.873",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.1925,
            "range": "± 616.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.6122,
            "range": "± 12.42",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.6363,
            "range": "± 15.2843",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.40835,
            "range": "± 296.498",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.8161,
            "range": "± 697.502",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 61.4093,
            "range": "± 7.35763",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.2669,
            "range": "± 1.13477",
            "unit": "ns",
            "extra": "100 samples\n9274 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.5056,
            "range": "± 1.65416",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 523.624,
            "range": "± 25.94",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.62683,
            "range": "± 0.604682",
            "unit": "ns",
            "extra": "100 samples\n6578 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.5623,
            "range": "± 9.96305",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9673,
            "range": "± 136.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.2212,
            "range": "± 6.18124",
            "unit": "ns",
            "extra": "100 samples\n693 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 928.877,
            "range": "± 5.18062",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 199.77,
            "range": "± 703.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.29466,
            "range": "± 0.449057",
            "unit": "ns",
            "extra": "100 samples\n9204 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.084,
            "range": "± 3.18615",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.59923,
            "range": "± 20.1613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.14674,
            "range": "± 197.663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.68465,
            "range": "± 51.7621",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.69668,
            "range": "± 268.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.59712,
            "range": "± 78.9564",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.4791,
            "range": "± 189.981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1752,
            "range": "± 64.7436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.8093,
            "range": "± 323.587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.57631,
            "range": "± 56.0296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8822,
            "range": "± 58.6602",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1376,
            "range": "± 1.37671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9488,
            "range": "± 86.1433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.0833,
            "range": "± 677.381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0179,
            "range": "± 61.3636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.3982,
            "range": "± 417.303",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.4814,
            "range": "± 368.196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.2284,
            "range": "± 497.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.1269,
            "range": "± 365.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.1428,
            "range": "± 321.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.3126,
            "range": "± 607.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.4411,
            "range": "± 243.905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.714,
            "range": "± 250.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.1508,
            "range": "± 1.57807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.5311,
            "range": "± 547.454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.1385,
            "range": "± 491.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.503,
            "range": "± 1.72012",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.8534,
            "range": "± 791.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.7127,
            "range": "± 480.68",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.2114,
            "range": "± 1.72524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2531,
            "range": "± 2.37906",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.7552,
            "range": "± 2.44603",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.6494,
            "range": "± 3.26166",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.2438,
            "range": "± 4.39455",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8406,
            "range": "± 3.33361",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.1015,
            "range": "± 4.28604",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8728,
            "range": "± 4.17891",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.6777,
            "range": "± 3.06736",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.531,
            "range": "± 3.63358",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.2894,
            "range": "± 4.19745",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.243,
            "range": "± 5.14845",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 185.283,
            "range": "± 12.5192",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 52.6051,
            "range": "± 10.6667",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 294.26,
            "range": "± 13.7831",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.8129,
            "range": "± 4.81736",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.2497,
            "range": "± 6.23319",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 58.8913,
            "range": "± 8.92396",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.6071,
            "range": "± 5.1107",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 56.8739,
            "range": "± 9.2097",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 48.0573,
            "range": "± 10.0467",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.7187,
            "range": "± 6.62848",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.9782,
            "range": "± 4.40963",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.5058,
            "range": "± 9.25759",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.2365,
            "range": "± 5.21572",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 109.172,
            "range": "± 12.2903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.1731,
            "range": "± 21.2518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 167.435,
            "range": "± 14.8367",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.2865,
            "range": "± 41.7479",
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
          "id": "4be9134f17f2039841173d57060d80a4c7a90b1c",
          "message": "Fix bc backend bool/narrow value handling and re-enable vector tests\n\nTwo related bugs caused vectorEqualsFalse / vectorEqualsDifferentSize\nto fail on the bc backend (the tests were previously guarded behind\n`if (backend != \"bc\")`):\n\n1. writeReg<T> in ByteCode.hpp wrote only the low sizeof(T) bytes\n   into the 64-bit register slot, leaving stale upper bytes from\n   prior register use. Those bytes leaked out via the int64_t\n   function-return path, where bcCallbackHandler does\n   static_cast<bool>(raw) and any non-zero upper bits flipped a\n   false result to true. Fix: zero-extend integral writes (and\n   zero-fill before memcpy for floats/pointers) so the full slot\n   is always well-defined. This covers comparison ops, loads,\n   casts, logical ops, selectOp, dyncall returns, and arg unpack\n   in one place. readReg<T> also switched to memcpy to avoid\n   strict-aliasing UB.\n\n2. dyncall's DCbool is typedef'd to int. On SysV x86-64 a callee\n   returning C++ bool is only required to set the low byte of the\n   return register; upper bytes are caller-defined. GCC's\n   std::vector::operator== leaves non-zero upper bytes, so an\n   implicit int->bool conversion in Dyncall::callB turned a\n   logically-false return into true on gcc. clang happened to\n   zero-extend, hiding the bug. Fix: mask to the low byte in\n   Dyncall::callB and in the bcCallback bool arg-unpack path.\n\nRe-enable the two vector tests on the bc backend, and add focused\nregression tests in BoolOperations / BoolExecutionTest:\n * invokeReturnsFalse/TrueAfterDirtyReg and invokePointerCmpFalse\n   reproduce the bool-return path without depending on plugins/std.\n * invokeReturnsI{8,16,32,64}/U{8,16,32,64}AfterDirtyReg and\n   invoke{I,U}{8,16,32}ThenCastToI/U64 cover every narrow integer\n   width through the writeReg/readReg paths with 0, -1/max and a\n   typical mid value.",
          "timestamp": "2026-04-07T11:03:29+02:00",
          "tree_id": "7e0d5bc4681bca493101e2a330c8646529a7a762",
          "url": "https://github.com/nebulastream/nautilus/commit/4be9134f17f2039841173d57060d80a4c7a90b1c"
        },
        "date": 1775552883373,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.72903,
            "range": "± 139.85",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.71612,
            "range": "± 133.131",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 7.86972,
            "range": "± 1.17779",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.64354,
            "range": "± 397.181",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.9323,
            "range": "± 3.50401",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 10.7586,
            "range": "± 1.52443",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.84252,
            "range": "± 1.33512",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.65368,
            "range": "± 441.154",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.3419,
            "range": "± 3.30171",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 6.87541,
            "range": "± 957.938",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.01306,
            "range": "± 1.36941",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.90745,
            "range": "± 870.962",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.1391,
            "range": "± 3.02596",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 7.74913,
            "range": "± 1.19123",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.96709,
            "range": "± 1.36341",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.61729,
            "range": "± 924.051",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 8.93541,
            "range": "± 1.86823",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.69335,
            "range": "± 609.971",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.2355,
            "range": "± 1.24252",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.14835,
            "range": "± 369.356",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 45.8446,
            "range": "± 6.77875",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 46.3357,
            "range": "± 6.87136",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39402,
            "range": "± 37.4099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39644,
            "range": "± 27.3494",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 101.071,
            "range": "± 11.645",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 53.6467,
            "range": "± 7.91444",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.83212,
            "range": "± 130.846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.03811,
            "range": "± 29.5763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 28.6293,
            "range": "± 14.339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 5.75625,
            "range": "± 189.632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 34.9349,
            "range": "± 44.8752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 29.1627,
            "range": "± 12.8942",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 40.1019,
            "range": "± 15.488",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 7.85663,
            "range": "± 261.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 47.1533,
            "range": "± 86.2594",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 40.3212,
            "range": "± 15.8878",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 29.1707,
            "range": "± 13.3069",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 7.63863,
            "range": "± 190.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 138.191,
            "range": "± 5.93728",
            "unit": "ns",
            "extra": "100 samples\n170 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 339.155,
            "range": "± 16.0757",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.8862720000000001,
            "range": "± 0.0922978",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 359.699,
            "range": "± 25.0193",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 675.346,
            "range": "± 49.1173",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 359.903,
            "range": "± 20.3514",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 432.877,
            "range": "± 21.3737",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 321.374,
            "range": "± 12.341",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 372.16,
            "range": "± 23.4835",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 330.217,
            "range": "± 15.4998",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ir_add",
            "value": 627.942,
            "range": "± 38.1137",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.8613,
            "range": "± 142.936",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.21806,
            "range": "± 493.528",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.26141,
            "range": "± 152.895",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.38778,
            "range": "± 404.666",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.40001,
            "range": "± 167.559",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.89613,
            "range": "± 284.157",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.81137,
            "range": "± 122.215",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.39983,
            "range": "± 175.019",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.9937,
            "range": "± 135.856",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 12.1763,
            "range": "± 849.082",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 147.435,
            "range": "± 7.46283",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 22.7103,
            "range": "± 1.32399",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 279.001,
            "range": "± 10.4322",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.28869,
            "range": "± 0.688037",
            "unit": "ns",
            "extra": "100 samples\n9785 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.3854,
            "range": "± 1.71357",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 505.854,
            "range": "± 51.389",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.57388,
            "range": "± 0.250782",
            "unit": "ns",
            "extra": "100 samples\n7214 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 84.1932,
            "range": "± 4.45713",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 18.3033,
            "range": "± 98.2966",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 33.8129,
            "range": "± 1.47888",
            "unit": "ns",
            "extra": "100 samples\n679 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 511.277,
            "range": "± 19.6414",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 115.707,
            "range": "± 618.827",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.64434,
            "range": "± 0.182567",
            "unit": "ns",
            "extra": "100 samples\n9791 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 17.2429,
            "range": "± 1.11577",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.97391,
            "range": "± 21.5409",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 28.1289,
            "range": "± 3.3901",
            "unit": "ns",
            "extra": "100 samples\n873 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 240.683,
            "range": "± 11.0319",
            "unit": "ns",
            "extra": "100 samples\n9655 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.07118,
            "range": "± 0.676816",
            "unit": "ns",
            "extra": "100 samples\n8773 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 30.8222,
            "range": "± 1.19301",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.14635,
            "range": "± 292.641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.80314,
            "range": "± 326.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.43769,
            "range": "± 256.893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.63845,
            "range": "± 321.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.4749,
            "range": "± 497.589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.9587,
            "range": "± 270.707",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.203,
            "range": "± 380.667",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.53213,
            "range": "± 288.035",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0436,
            "range": "± 206.724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.6612,
            "range": "± 290.391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.6136,
            "range": "± 258.351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.7287,
            "range": "± 240.512",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.756,
            "range": "± 271.626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 52.0967,
            "range": "± 340.092",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.2201,
            "range": "± 31.8648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.4899,
            "range": "± 3.29759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.4359,
            "range": "± 2.8428",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.7394,
            "range": "± 2.78551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 24.6129,
            "range": "± 2.25677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 24.328,
            "range": "± 2.77909",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 32.6398,
            "range": "± 41.4457",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.12,
            "range": "± 7.37168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 23.8501,
            "range": "± 489.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.0641,
            "range": "± 464.877",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 26.3309,
            "range": "± 467.551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 53.7664,
            "range": "± 13.6762",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 28.3727,
            "range": "± 454.712",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 77.1369,
            "range": "± 8.62978",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.1813,
            "range": "± 2.30045",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 12.3859,
            "range": "± 2.63514",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 15.7606,
            "range": "± 3.65737",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.7485,
            "range": "± 2.63438",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 14.3249,
            "range": "± 3.28687",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 12.5385,
            "range": "± 2.20259",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 13.5764,
            "range": "± 3.37606",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.612,
            "range": "± 2.85531",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.1845,
            "range": "± 3.31499",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 12.5684,
            "range": "± 3.6103",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 24.403,
            "range": "± 3.86382",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 135.205,
            "range": "± 8.58357",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 32.5079,
            "range": "± 6.55032",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 221.237,
            "range": "± 10.3432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 13.8201,
            "range": "± 4.89952",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 21.6798,
            "range": "± 5.74685",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 39.9196,
            "range": "± 9.75519",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 22.9636,
            "range": "± 5.52702",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 39.3726,
            "range": "± 9.96561",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 25.6009,
            "range": "± 5.4618",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 27.5547,
            "range": "± 5.37495",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 19.0893,
            "range": "± 4.66739",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 24.9193,
            "range": "± 5.25969",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 22.6488,
            "range": "± 5.19839",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 79.4596,
            "range": "± 13.64",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 0.901695,
            "range": "± 0.0209866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 120.45,
            "range": "± 12.4859",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 1.81964,
            "range": "± 24.2221",
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
          "id": "172119eed4e510c3292885db2686a5c8995c7907",
          "message": "Add std::atomic wrapper, atomic free functions and MLIR atomic intrinsics\n\nIntroduces a representative for std::atomic in the nautilus-std plugin:\n\n* val<std::atomic<T>> template wrapper with default ctor, value ctor and\n  a non-owning ctor that wraps an existing val<std::atomic<T>*>. The\n  destructor only frees atomics it allocated itself.\n\n* Member functions load / store / exchange / fetch_{add,sub,and,or,xor}\n  / compare_exchange_strong, all parameterised on a non-type\n  std::memory_order template argument so that each ordering instantiates\n  a distinct call site.\n\n* Free atomic_load / atomic_store / atomic_exchange / atomic_fetch_*\n  / atomic_compare_exchange_strong / atomic_thread_fence overloads on\n  both val<std::atomic<T>*> and val<T*>. The val<T*> path uses\n  std::atomic_ref<T> at runtime so user code can perform atomic\n  operations directly on plain pointers.\n\n* Per (type, ordering) named extern \"C\" thunks (int32_t, int64_t,\n  uint32_t, uint64_t × seq_cst, acquire, release, acq_rel, relaxed)\n  give the call sites stable function pointers that can be matched by\n  the MLIR backend intrinsic registry.\n\n* MLIRAtomicIntrinsics plugin keys on those thunk addresses and lowers\n  the runtime calls directly to LLVM::LoadOp / StoreOp / AtomicRMWOp /\n  FenceOp with the matching AtomicOrdering, so the mlir backend emits\n  real atomic machine instructions instead of opaque proxy calls.\n\nTests cover member and free-function paths on int32_t, int64_t,\nuint32_t and uint64_t; seq_cst, acquire, release and relaxed orderings;\nall fetch_* RMW variants; default-ctor + a.get() interop; member and\nfree compare_exchange_strong success and failure paths; the non-owning\nview ctor; loop fetch_add; INT64_MAX wrap-around; and atomic_thread_fence\nwith all five orderings — exercised against the interpreter and every\nenabled compiler backend.",
          "timestamp": "2026-04-07T13:03:43+02:00",
          "tree_id": "6684653f5617344414332fd4a6ffc5d9250bde72",
          "url": "https://github.com/nebulastream/nautilus/commit/172119eed4e510c3292885db2686a5c8995c7907"
        },
        "date": 1775560094956,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 785.862,
            "range": "± 69.5204",
            "unit": "ns",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.35322,
            "range": "± 127.373",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.1541,
            "range": "± 417.225",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.65396,
            "range": "± 159.503",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.01373,
            "range": "± 290.873",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.90539,
            "range": "± 185.106",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.4185,
            "range": "± 264.49",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.05033,
            "range": "± 284.306",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.92,
            "range": "± 194.356",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.43599,
            "range": "± 165.442",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 20.0315,
            "range": "± 913.523",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 190.01,
            "range": "± 5.33457",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.2993,
            "range": "± 1.73565",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 305.84,
            "range": "± 7.11095",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 31.5001,
            "range": "± 0.734315",
            "unit": "ns",
            "extra": "100 samples\n587 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 406.932,
            "range": "± 19.2869",
            "unit": "ns",
            "extra": "100 samples\n6201 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.69011,
            "range": "± 0.42795",
            "unit": "ns",
            "extra": "100 samples\n5672 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 35.8905,
            "range": "± 1.46007",
            "unit": "ns",
            "extra": "100 samples\n508 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.71468,
            "range": "± 193.591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.2203,
            "range": "± 152.362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.20847,
            "range": "± 127.382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.3078,
            "range": "± 423.409",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2413,
            "range": "± 626.74",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3096,
            "range": "± 268.689",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.1905,
            "range": "± 339.156",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.18399,
            "range": "± 149.221",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.274,
            "range": "± 230.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1729,
            "range": "± 186.23",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.2995,
            "range": "± 251.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.2869,
            "range": "± 381.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.635,
            "range": "± 246.498",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 61.8528,
            "range": "± 408.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 20.7252,
            "range": "± 555.654",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 21.4284,
            "range": "± 618.865",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 22.1779,
            "range": "± 498.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 21.3761,
            "range": "± 472.155",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 22.2612,
            "range": "± 494.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 21.6752,
            "range": "± 537.141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 22.2568,
            "range": "± 827.672",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 20.9914,
            "range": "± 539.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 21.6802,
            "range": "± 415.644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 21.5712,
            "range": "± 562.68",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 24.3624,
            "range": "± 619.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 56.5468,
            "range": "± 626.751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 26.4174,
            "range": "± 530.978",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 85.0041,
            "range": "± 840.55",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.1512,
            "range": "± 2.25618",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 12.2155,
            "range": "± 2.2376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 16.459,
            "range": "± 2.44197",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.5906,
            "range": "± 2.32566",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 14.7335,
            "range": "± 2.1157",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 12.8715,
            "range": "± 2.14066",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 13.511,
            "range": "± 1.78731",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.9857,
            "range": "± 2.06087",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 12.6209,
            "range": "± 2.5236",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 13.25,
            "range": "± 2.24604",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.4532,
            "range": "± 6.55989",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 172.93,
            "range": "± 8.19827",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 39.2789,
            "range": "± 4.39299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 280.665,
            "range": "± 10.9598",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 15.0253,
            "range": "± 3.27441",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.659,
            "range": "± 5.1553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 45.5648,
            "range": "± 7.42419",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.0798,
            "range": "± 6.48666",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 43.9704,
            "range": "± 7.6908",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.9468,
            "range": "± 7.31466",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.3834,
            "range": "± 7.82072",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.0342,
            "range": "± 6.31345",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 32.899,
            "range": "± 6.98763",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.0496,
            "range": "± 6.76113",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 92.4075,
            "range": "± 9.23668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.09571,
            "range": "± 24.7172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 147.785,
            "range": "± 13.359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.16491,
            "range": "± 66.3666",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 32.5673,
            "range": "± 8.97002",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.69272,
            "range": "± 437.106",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 31.9891,
            "range": "± 8.56956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.661,
            "range": "± 148.423",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 20.6055,
            "range": "± 490.981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 29.9976,
            "range": "± 9.10917",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 46.449,
            "range": "± 11.5558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.63333,
            "range": "± 252.367",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 21.6489,
            "range": "± 508.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 48.4537,
            "range": "± 10.8829",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 200.091,
            "range": "± 36.4041",
            "unit": "ns",
            "extra": "100 samples\n94 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 480.463,
            "range": "± 49.4253",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.35663,
            "range": "± 85.2506",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 514.733,
            "range": "± 48.9865",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1081.1100000000001,
            "range": "± 69225.7",
            "unit": "ns",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 506.945,
            "range": "± 29.4388",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 620.164,
            "range": "± 36.0163",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 435.271,
            "range": "± 29.8577",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 528.109,
            "range": "± 26.1907",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 475.336,
            "range": "± 41.6712",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.92301,
            "range": "± 265.404",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.78975,
            "range": "± 167.441",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.8941,
            "range": "± 2.31495",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.90725,
            "range": "± 633.009",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.6404,
            "range": "± 6.17006",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 17.0212,
            "range": "± 2.91522",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.8882,
            "range": "± 2.22602",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.90209,
            "range": "± 688.468",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.0648,
            "range": "± 3.78219",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 11.1008,
            "range": "± 1.15367",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9512,
            "range": "± 2.05785",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.9593,
            "range": "± 728.837",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 20.4104,
            "range": "± 4.23806",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 13.1437,
            "range": "± 2.55104",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.1432,
            "range": "± 1.81349",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.57014,
            "range": "± 832.601",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.3603,
            "range": "± 2.70924",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.51835,
            "range": "± 535.868",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.9316,
            "range": "± 1.99798",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 5.07326,
            "range": "± 796.141",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.1614,
            "range": "± 6.78398",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 56.4403,
            "range": "± 7.50336",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.79333,
            "range": "± 27.9897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.72384,
            "range": "± 29.2282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 135.109,
            "range": "± 9.91123",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.1907,
            "range": "± 8.31259",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.6996,
            "range": "± 34.6271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.86621,
            "range": "± 37.6894",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.9185,
            "range": "± 1.00834",
            "unit": "ns",
            "extra": "100 samples\n6974 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 27.2453,
            "range": "± 2.11736",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 673.627,
            "range": "± 23.7527",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.27629,
            "range": "± 0.810122",
            "unit": "ns",
            "extra": "100 samples\n3834 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 131.906,
            "range": "± 6.73967",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 25.5573,
            "range": "± 134.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 36.2066,
            "range": "± 1.48847",
            "unit": "ns",
            "extra": "100 samples\n511 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 556.918,
            "range": "± 35.6927",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 121.737,
            "range": "± 6.60808",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.64906,
            "range": "± 0.241854",
            "unit": "ns",
            "extra": "100 samples\n6984 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 31.0858,
            "range": "± 4.55491",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 6.27193,
            "range": "± 56.4877",
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
          "id": "a01b07905755c7e2f39eda561354ebd5805c0e87",
          "message": "Add documentation for the nautilus-std plugin",
          "timestamp": "2026-04-07T13:42:40+02:00",
          "tree_id": "e02259dae72701e74265b5944b84695eefeff952",
          "url": "https://github.com/nebulastream/nautilus/commit/a01b07905755c7e2f39eda561354ebd5805c0e87"
        },
        "date": 1775562421192,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 35.9151,
            "range": "± 4.03511",
            "unit": "ns",
            "extra": "100 samples\n849 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 289.542,
            "range": "± 8.65906",
            "unit": "ns",
            "extra": "100 samples\n8280 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.89215,
            "range": "± 1.14149",
            "unit": "ns",
            "extra": "100 samples\n7355 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 41.109,
            "range": "± 2.54839",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "ssa_add",
            "value": 194.404,
            "range": "± 9.45466",
            "unit": "ns",
            "extra": "100 samples\n152 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 501.193,
            "range": "± 44.4748",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.2968,
            "range": "± 125.822",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 550.926,
            "range": "± 68.3432",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 971.373,
            "range": "± 70.0105",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 539.229,
            "range": "± 46.1203",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 646.15,
            "range": "± 55.9445",
            "unit": "ns",
            "extra": "100 samples\n51 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 513.545,
            "range": "± 78.5362",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 548.044,
            "range": "± 46.46",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 489.735,
            "range": "± 56.7511",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.5006,
            "range": "± 590.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.92581,
            "range": "± 171.006",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.89447,
            "range": "± 178.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.99408,
            "range": "± 326.166",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.9581,
            "range": "± 372.387",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.9712,
            "range": "± 397.251",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.5935,
            "range": "± 753.748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.84001,
            "range": "± 197.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4973,
            "range": "± 335.023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.2942,
            "range": "± 237.419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.3433,
            "range": "± 197.657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.2532,
            "range": "± 889.994",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3833,
            "range": "± 399.269",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.5066,
            "range": "± 815.209",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.6523,
            "range": "± 1.24551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.0542,
            "range": "± 509.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.1227,
            "range": "± 405.943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.1394,
            "range": "± 385.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.1548,
            "range": "± 341.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.4709,
            "range": "± 524.8",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.3658,
            "range": "± 282.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.7277,
            "range": "± 375.031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.1764,
            "range": "± 396.825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.6687,
            "range": "± 264.565",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.7677,
            "range": "± 276.25",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.44,
            "range": "± 1.85398",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.3783,
            "range": "± 675.686",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.8873,
            "range": "± 2.19886",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.6431,
            "range": "± 2.99726",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.1066,
            "range": "± 4.12283",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.6678,
            "range": "± 3.79912",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.0196,
            "range": "± 3.79853",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.2842,
            "range": "± 2.94404",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.7769,
            "range": "± 3.27534",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8151,
            "range": "± 3.23608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.8505,
            "range": "± 3.33795",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.0107,
            "range": "± 3.05528",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.8937,
            "range": "± 5.10201",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.5322,
            "range": "± 4.76937",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 183.971,
            "range": "± 9.82183",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.9791,
            "range": "± 8.03735",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 292.695,
            "range": "± 25.0279",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.2774,
            "range": "± 4.33942",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.3961,
            "range": "± 6.17776",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 58.7983,
            "range": "± 10.3148",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.6733,
            "range": "± 5.73842",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.1834,
            "range": "± 9.67122",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.0332,
            "range": "± 8.22846",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.7323,
            "range": "± 10.2884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.7887,
            "range": "± 4.46578",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.0715,
            "range": "± 7.87745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.1367,
            "range": "± 6.69245",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 110.591,
            "range": "± 13.3561",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.17167,
            "range": "± 154.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 164.953,
            "range": "± 16.2678",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.26516,
            "range": "± 48.7805",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.2467,
            "range": "± 10.8806",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.4793,
            "range": "± 766.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.64203,
            "range": "± 354.218",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.55291,
            "range": "± 277.917",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.7661,
            "range": "± 1.84363",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.30277,
            "range": "± 647.6",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.7562,
            "range": "± 8.12769",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.3839,
            "range": "± 2.84349",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.487,
            "range": "± 1.78997",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.38236,
            "range": "± 712.317",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.545,
            "range": "± 4.2429",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.0554,
            "range": "± 1.52787",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.7589,
            "range": "± 2.10373",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.56802,
            "range": "± 679.006",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.7662,
            "range": "± 3.23748",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.4968,
            "range": "± 1.77462",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.71975,
            "range": "± 1.18021",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.76394,
            "range": "± 1.62",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.9203,
            "range": "± 1.69099",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.76517,
            "range": "± 738.611",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.7492,
            "range": "± 1.6592",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.92657,
            "range": "± 782.542",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.1655,
            "range": "± 7.47339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 57.4916,
            "range": "± 8.74867",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.78314,
            "range": "± 36.1621",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.78087,
            "range": "± 38.2725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 138.305,
            "range": "± 11.5896",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 104.654,
            "range": "± 21.1529",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.11151,
            "range": "± 42.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.88801,
            "range": "± 156.645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 41.4345,
            "range": "± 10.5149",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.22889,
            "range": "± 141.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.0657,
            "range": "± 488.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.333,
            "range": "± 9.98034",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.0895,
            "range": "± 14.2709",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.3129,
            "range": "± 174.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.9351,
            "range": "± 372.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 62.1414,
            "range": "± 14.0884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.7229,
            "range": "± 0.808724",
            "unit": "ns",
            "extra": "100 samples\n9289 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.8972,
            "range": "± 1.32201",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 552.269,
            "range": "± 16.2959",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.73806,
            "range": "± 0.784088",
            "unit": "ns",
            "extra": "100 samples\n6600 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.8059,
            "range": "± 7.46066",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9561,
            "range": "± 99.8658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.523,
            "range": "± 5.79782",
            "unit": "ns",
            "extra": "100 samples\n705 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 841.009,
            "range": "± 10.015",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 181.84,
            "range": "± 7.3196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.20292,
            "range": "± 0.336904",
            "unit": "ns",
            "extra": "100 samples\n9272 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.5831,
            "range": "± 2.4908",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.61212,
            "range": "± 61.5393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 888.971,
            "range": "± 77.4277",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.72714,
            "range": "± 449.715",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.93408,
            "range": "± 602.02",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.11683,
            "range": "± 394.669",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.9499,
            "range": "± 562.619",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.34563,
            "range": "± 389.788",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.96016,
            "range": "± 525.355",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.3789,
            "range": "± 277.142",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.27579,
            "range": "± 402.271",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.65146,
            "range": "± 155.761",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.4882,
            "range": "± 1.61363",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 194.696,
            "range": "± 9.94019",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.3433,
            "range": "± 4.22932",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 362.716,
            "range": "± 12.6399",
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
          "id": "cd479ae926954f01ab98e08558af3cc678087720",
          "message": "Add shared execution test infrastructure\n\nIntroduce nautilus/test/common/ExecutionTest.hpp providing availableBackends(), makeEngine(), forEachBackend(), and forEachBackendWithTraceMode() helpers so execution tests no longer need to repeat the ENABLE_*_BACKEND ifdef matrix and DYNAMIC_SECTION loops. Both helpers accept an include_asmjit toggle for tests that intentionally exclude asmjit.\n\nMigrate all in-tree execution tests (Bool, Pointer, Memory, Select, Cast x2, FunctionPtr, ExecutionTest engine + nautilus function, Concurrent, Bit/Memory intrinsic orphans) and the std + simd plugin tests (Bit, Atomic, STDProxy, String, Ostream, Vector, MemoryIntrinsic, simd Vector) onto the new helpers. plugins/{std,simd}/test/CMakeLists.txt expose nautilus/test/common on their include path.",
          "timestamp": "2026-04-07T15:14:50+02:00",
          "tree_id": "1590ed1a7929967b33f99566602e872f0b307b60",
          "url": "https://github.com/nebulastream/nautilus/commit/cd479ae926954f01ab98e08558af3cc678087720"
        },
        "date": 1775568031752,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 201.719,
            "range": "± 21.8179",
            "unit": "ns",
            "extra": "100 samples\n155 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 504.064,
            "range": "± 54.7725",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.196,
            "range": "± 119.208",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 539.992,
            "range": "± 73.3221",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 973.629,
            "range": "± 148.077",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 536.679,
            "range": "± 58.315",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 654.108,
            "range": "± 88.2376",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 489.375,
            "range": "± 48.032",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 555.594,
            "range": "± 67.7761",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 516.423,
            "range": "± 81.8829",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.45468,
            "range": "± 324.617",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.40556,
            "range": "± 343.636",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.8831,
            "range": "± 1.55374",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.96067,
            "range": "± 547.582",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.8698,
            "range": "± 5.46584",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 22.3636,
            "range": "± 2.69985",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.7386,
            "range": "± 1.6777",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.04552,
            "range": "± 656.271",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.7564,
            "range": "± 2.87329",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.35583,
            "range": "± 1.3055",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.7918,
            "range": "± 1.85969",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.29064,
            "range": "± 900.487",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.7773,
            "range": "± 3.29225",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.3383,
            "range": "± 1.59588",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.193,
            "range": "± 1.83328",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 10.5681,
            "range": "± 1.67827",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.4633,
            "range": "± 1.88384",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.38036,
            "range": "± 765.161",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.9448,
            "range": "± 1.63548",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.32089,
            "range": "± 584.191",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 60.7424,
            "range": "± 10.1658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 60.8202,
            "range": "± 9.70214",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.73573,
            "range": "± 53.7598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.74876,
            "range": "± 71.1712",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 139.132,
            "range": "± 12.6395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.6115,
            "range": "± 8.95038",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.06865,
            "range": "± 51.7655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.71857,
            "range": "± 59.2389",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.80632,
            "range": "± 1.03581",
            "unit": "ns",
            "extra": "100 samples\n9351 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.4343,
            "range": "± 1.66054",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 575.393,
            "range": "± 22.7658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.79323,
            "range": "± 0.939911",
            "unit": "ns",
            "extra": "100 samples\n6614 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.3557,
            "range": "± 7.6076",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9268,
            "range": "± 236.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.074,
            "range": "± 6.55218",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 842.397,
            "range": "± 11.8747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 184.018,
            "range": "± 1.0162",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.29486,
            "range": "± 0.560832",
            "unit": "ns",
            "extra": "100 samples\n9312 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.7243,
            "range": "± 2.96757",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.61603,
            "range": "± 23.3678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.27728,
            "range": "± 219.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.06635,
            "range": "± 479.282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.75974,
            "range": "± 181.64",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.95331,
            "range": "± 181.147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.1471,
            "range": "± 342.219",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7203,
            "range": "± 340.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.6184,
            "range": "± 359.861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.94896,
            "range": "± 207.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.1446,
            "range": "± 428.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3946,
            "range": "± 497.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.4552,
            "range": "± 391.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.3016,
            "range": "± 329.409",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.2495,
            "range": "± 616.086",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.6145,
            "range": "± 923.507",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.231,
            "range": "± 506.162",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.8378,
            "range": "± 481.265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.8154,
            "range": "± 488.186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.6822,
            "range": "± 378.165",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.6261,
            "range": "± 1.22302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3547,
            "range": "± 997.814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.5071,
            "range": "± 581.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.493,
            "range": "± 386.416",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.1632,
            "range": "± 552.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.8614,
            "range": "± 771.603",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.3974,
            "range": "± 675.063",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.9289,
            "range": "± 1.5611",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.2312,
            "range": "± 447.955",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.0946,
            "range": "± 1.38067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2556,
            "range": "± 2.57401",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.8056,
            "range": "± 3.53687",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.2409,
            "range": "± 2.86948",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.2952,
            "range": "± 3.82533",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8946,
            "range": "± 4.38224",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.0751,
            "range": "± 3.75799",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.1974,
            "range": "± 4.41733",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.7107,
            "range": "± 3.29906",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.1967,
            "range": "± 2.88997",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.2893,
            "range": "± 3.81909",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.7131,
            "range": "± 4.95563",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 183.168,
            "range": "± 20.0979",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 52.4408,
            "range": "± 12.0951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 285.792,
            "range": "± 15.7064",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.6452,
            "range": "± 4.91859",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.5445,
            "range": "± 5.30602",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.0053,
            "range": "± 10.7444",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.731,
            "range": "± 5.19291",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.6337,
            "range": "± 10.36",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.0928,
            "range": "± 8.76117",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.9381,
            "range": "± 9.40348",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.7404,
            "range": "± 5.35068",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 45.12,
            "range": "± 9.68881",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.5834,
            "range": "± 5.47106",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 111.703,
            "range": "± 15.0915",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.14818,
            "range": "± 21.5483",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 163.64,
            "range": "± 14.8592",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.27826,
            "range": "± 33.2068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 43.0751,
            "range": "± 11.0488",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.33548,
            "range": "± 515.325",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 43.0712,
            "range": "± 12.5729",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.24148,
            "range": "± 144.96",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.0481,
            "range": "± 508.884",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.8122,
            "range": "± 12.0917",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.3618,
            "range": "± 15.0246",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.04582,
            "range": "± 346.464",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.2411,
            "range": "± 1.44512",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 60.6102,
            "range": "± 12.254",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.9375,
            "range": "± 7.07299",
            "unit": "ns",
            "extra": "100 samples\n866 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 278.836,
            "range": "± 8.0612",
            "unit": "ns",
            "extra": "100 samples\n8340 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.9411,
            "range": "± 0.259879",
            "unit": "ns",
            "extra": "100 samples\n7789 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.6648,
            "range": "± 4.47366",
            "unit": "ns",
            "extra": "100 samples\n731 iterations"
          },
          {
            "name": "ir_add",
            "value": 871.751,
            "range": "± 107.382",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.51856,
            "range": "± 244.411",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.67066,
            "range": "± 663.279",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.95919,
            "range": "± 242.547",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.62056,
            "range": "± 478.613",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.19199,
            "range": "± 349.393",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.83878,
            "range": "± 524.08",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.36737,
            "range": "± 392.039",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.22125,
            "range": "± 502.45",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.69855,
            "range": "± 351.87",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.4551,
            "range": "± 1.4315",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 186.61,
            "range": "± 7.32368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.5815,
            "range": "± 2.46007",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 355.432,
            "range": "± 15.1017",
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
          "id": "8c754ac6ac0bb2441cfa9eb8cb73fe7e04c458ec",
          "message": "Add iterator support to val<std::vector>\n\nIntroduce a random-access iterator nested inside val<std::vector<T>> that\nmirrors normal std::vector::iterator semantics: dereference, increment,\ndecrement, random-access arithmetic (+, -, +=, -=), distance, subscript,\nand the full set of comparison operators. begin()/end() return iterators\nwrapping a val<T*> cursor, enabling natural C++-style iteration over\nvectors traced by Nautilus.\n\nIncludes comprehensive tests covering empty-vector iteration, pre/post\nincrement and decrement, all comparison operators, partial distance,\nmutation through *it, double and struct (class T) element types, and\nloop-based summation/write under the interpreter.",
          "timestamp": "2026-04-07T16:09:47+02:00",
          "tree_id": "2054ff193b96e8e389110b2930dc2a70f8fa2dd7",
          "url": "https://github.com/nebulastream/nautilus/commit/8c754ac6ac0bb2441cfa9eb8cb73fe7e04c458ec"
        },
        "date": 1775571263552,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 40.0949,
            "range": "± 5.40633",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.28392,
            "range": "± 227.885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 198.95,
            "range": "± 22.8363",
            "unit": "ns",
            "extra": "100 samples\n148 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 495.032,
            "range": "± 50.0573",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.26966,
            "range": "± 161.897",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 541.502,
            "range": "± 93.6273",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 983.314,
            "range": "± 112.495",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 536.098,
            "range": "± 55.4888",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 630.105,
            "range": "± 42.5939",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 518.654,
            "range": "± 83.5763",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 547.272,
            "range": "± 53.63",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 512.089,
            "range": "± 71.3299",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.65832,
            "range": "± 357.522",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.58253,
            "range": "± 313.549",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.823,
            "range": "± 1.12758",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.48645,
            "range": "± 652.484",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.0857,
            "range": "± 3.82084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.538,
            "range": "± 2.27836",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3361,
            "range": "± 1.02261",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.55261,
            "range": "± 816.441",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.7296,
            "range": "± 2.11787",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.1367,
            "range": "± 970.779",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.7412,
            "range": "± 1.07322",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.60116,
            "range": "± 614.065",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.9425,
            "range": "± 1.9527",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.8641,
            "range": "± 1.21589",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.74689,
            "range": "± 1.02054",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.46574,
            "range": "± 1.50234",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.2687,
            "range": "± 1.39504",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.97925,
            "range": "± 749.886",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.8672,
            "range": "± 1.1918",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.81033,
            "range": "± 622.643",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.9149,
            "range": "± 6.38789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 57.8247,
            "range": "± 8.3033",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80276,
            "range": "± 51.4307",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.78879,
            "range": "± 31.2619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 140.002,
            "range": "± 7.77951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.9559,
            "range": "± 7.94724",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.10856,
            "range": "± 79.8581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.8185,
            "range": "± 26.531",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.23821,
            "range": "± 105.906",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.82642,
            "range": "± 80.8376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.73675,
            "range": "± 46.6974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.80975,
            "range": "± 62.5013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0427,
            "range": "± 254.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3878,
            "range": "± 72.1754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1411,
            "range": "± 138.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.66515,
            "range": "± 47.3643",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0758,
            "range": "± 106.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.0812,
            "range": "± 72.8951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1338,
            "range": "± 118.378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.6052,
            "range": "± 407.098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.1344,
            "range": "± 84.161",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.7364,
            "range": "± 904.426",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.3836,
            "range": "± 211.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.974,
            "range": "± 218.018",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.0593,
            "range": "± 228.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.1671,
            "range": "± 242.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.1372,
            "range": "± 274.154",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5499,
            "range": "± 226.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.6808,
            "range": "± 236.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.9171,
            "range": "± 293.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.4157,
            "range": "± 221.903",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.0909,
            "range": "± 305.404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.0682,
            "range": "± 280.008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.3015,
            "range": "± 1.19542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.4916,
            "range": "± 346.963",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.0789,
            "range": "± 1.29497",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2036,
            "range": "± 1.68531",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.7655,
            "range": "± 1.92631",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.0056,
            "range": "± 2.94861",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.9385,
            "range": "± 2.15213",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 26.5252,
            "range": "± 9.97119",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.6777,
            "range": "± 4.46336",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.9626,
            "range": "± 2.97878",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 15.9915,
            "range": "± 2.48124",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.7209,
            "range": "± 3.31004",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.1493,
            "range": "± 2.80447",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.7952,
            "range": "± 3.71324",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 185.357,
            "range": "± 10.2291",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.2164,
            "range": "± 5.76243",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 290.749,
            "range": "± 25.2878",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.1033,
            "range": "± 2.8383",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.4154,
            "range": "± 3.23008",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.0676,
            "range": "± 8.25925",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 36.3199,
            "range": "± 5.66383",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.0573,
            "range": "± 6.86644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.1586,
            "range": "± 7.90636",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 85.7081,
            "range": "± 9.37816",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3938,
            "range": "± 3.73371",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.4982,
            "range": "± 5.91593",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.7114,
            "range": "± 4.3363",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 110.039,
            "range": "± 12.2481",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.1329,
            "range": "± 19.2034",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 165.939,
            "range": "± 13.7731",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.26475,
            "range": "± 43.1814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 949.94,
            "range": "± 159.797",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.67139,
            "range": "± 374.698",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.82937,
            "range": "± 1.0099",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.07289,
            "range": "± 465.305",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.86053,
            "range": "± 815.171",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.34532,
            "range": "± 491.125",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.9893,
            "range": "± 555.217",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.36779,
            "range": "± 379.113",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.27709,
            "range": "± 512.449",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.81167,
            "range": "± 359.809",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.8219,
            "range": "± 1.75153",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 189.235,
            "range": "± 8.94376",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.7065,
            "range": "± 3.36358",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 368.951,
            "range": "± 10.2008",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.5858,
            "range": "± 0.723539",
            "unit": "ns",
            "extra": "100 samples\n9596 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.0408,
            "range": "± 1.62812",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 522.036,
            "range": "± 13.8917",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.58679,
            "range": "± 0.414774",
            "unit": "ns",
            "extra": "100 samples\n6764 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.7027,
            "range": "± 13.0111",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0449,
            "range": "± 337.115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.8044,
            "range": "± 11.9826",
            "unit": "ns",
            "extra": "100 samples\n728 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 840.628,
            "range": "± 11.5409",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 183.926,
            "range": "± 952.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.36423,
            "range": "± 0.520482",
            "unit": "ns",
            "extra": "100 samples\n9606 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.8587,
            "range": "± 3.2104",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.60173,
            "range": "± 37.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.7696,
            "range": "± 6.2661",
            "unit": "ns",
            "extra": "100 samples\n927 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 269.655,
            "range": "± 3.07478",
            "unit": "ns",
            "extra": "100 samples\n8566 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.96846,
            "range": "± 0.348391",
            "unit": "ns",
            "extra": "100 samples\n7990 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 43.0829,
            "range": "± 4.51524",
            "unit": "ns",
            "extra": "100 samples\n752 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 40.9244,
            "range": "± 6.57563",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.18881,
            "range": "± 60.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.5194,
            "range": "± 462.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 41.7046,
            "range": "± 7.62012",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 60.8856,
            "range": "± 8.60616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.22894,
            "range": "± 71.3554",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.2432,
            "range": "± 317.279",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 60.3308,
            "range": "± 7.48969",
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
          "id": "20ce7006b22e6aa4137495938a02cf867abae18b",
          "message": "Enable asmjit backend for Pointer Cast and Vector compiler tests (#228)",
          "timestamp": "2026-04-07T16:45:48+02:00",
          "tree_id": "b25fd87c1b94ae359b2dff67dbda10b63b703dce",
          "url": "https://github.com/nebulastream/nautilus/commit/20ce7006b22e6aa4137495938a02cf867abae18b"
        },
        "date": 1775573449109,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 36.8327,
            "range": "± 13.6145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.8837,
            "range": "± 162.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.8034,
            "range": "± 294.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 37.8771,
            "range": "± 14.0168",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 51.6635,
            "range": "± 18.0331",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.02455,
            "range": "± 315.839",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.8217,
            "range": "± 501.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 52.0797,
            "range": "± 18.2983",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.02033,
            "range": "± 187.975",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.61929,
            "range": "± 178.583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.48057,
            "range": "± 173.108",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.6635,
            "range": "± 189.157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.316,
            "range": "± 273.35",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.4573,
            "range": "± 207.223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.3837,
            "range": "± 333.992",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.49672,
            "range": "± 686.415",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.089,
            "range": "± 181.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.8816,
            "range": "± 357.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.8627,
            "range": "± 179.299",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.4783,
            "range": "± 185.818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.1644,
            "range": "± 182.278",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.9296,
            "range": "± 166.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.8901,
            "range": "± 279.846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.4323,
            "range": "± 263.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5021,
            "range": "± 296.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.5611,
            "range": "± 273.462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.5906,
            "range": "± 291.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.101,
            "range": "± 900.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.1822,
            "range": "± 362.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2966,
            "range": "± 281.382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.9389,
            "range": "± 918.982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.6502,
            "range": "± 589.168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.6354,
            "range": "± 397.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.0459,
            "range": "± 339.348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.2166,
            "range": "± 1.12002",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.1855,
            "range": "± 2.90401",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.435,
            "range": "± 2.60619",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 15.9771,
            "range": "± 2.78032",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.2044,
            "range": "± 3.95546",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.1286,
            "range": "± 3.27931",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.3907,
            "range": "± 3.59445",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.4488,
            "range": "± 2.78484",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.5778,
            "range": "± 4.34911",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.2008,
            "range": "± 2.72795",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.609,
            "range": "± 3.60042",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.7593,
            "range": "± 2.54598",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.5933,
            "range": "± 4.03308",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 169.888,
            "range": "± 9.48035",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 42.603,
            "range": "± 7.71487",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 276.941,
            "range": "± 13.4058",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.0342,
            "range": "± 6.37508",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 38.1207,
            "range": "± 19.1649",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 51.2174,
            "range": "± 11.1211",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.2565,
            "range": "± 6.31373",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 52.5656,
            "range": "± 12.6179",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 33.6377,
            "range": "± 6.38847",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 36.1215,
            "range": "± 6.55743",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.1733,
            "range": "± 5.88319",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 32.7518,
            "range": "± 7.41435",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 30.3343,
            "range": "± 7.088",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 104.538,
            "range": "± 16.2847",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.16598,
            "range": "± 20.878",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 160.277,
            "range": "± 15.0858",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.34024,
            "range": "± 59.3164",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.48093,
            "range": "± 0.865854",
            "unit": "ns",
            "extra": "100 samples\n9776 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.9733,
            "range": "± 1.35242",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 562.54,
            "range": "± 63.1274",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.54199,
            "range": "± 0.562672",
            "unit": "ns",
            "extra": "100 samples\n7198 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.643,
            "range": "± 10.641",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5705,
            "range": "± 41.9149",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.4205,
            "range": "± 5.16",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 658.638,
            "range": "± 18.6596",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 168.297,
            "range": "± 40.857",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.4194,
            "range": "± 0.310671",
            "unit": "ns",
            "extra": "100 samples\n9781 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.6184,
            "range": "± 3.86054",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.14545,
            "range": "± 54.6927",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.4056,
            "range": "± 3.91577",
            "unit": "ns",
            "extra": "100 samples\n878 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 272.097,
            "range": "± 6.86619",
            "unit": "ns",
            "extra": "100 samples\n9542 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.81628,
            "range": "± 0.584236",
            "unit": "ns",
            "extra": "100 samples\n8642 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.7964,
            "range": "± 2.01541",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 37.4035,
            "range": "± 15.1308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.90593,
            "range": "± 173.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 816.837,
            "range": "± 55.6838",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.38996,
            "range": "± 181.448",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.36464,
            "range": "± 1.50114",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.84019,
            "range": "± 231.656",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.62214,
            "range": "± 496.718",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.03276,
            "range": "± 245.109",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.63151,
            "range": "± 266.428",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.2062,
            "range": "± 142.003",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.01854,
            "range": "± 214.842",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.51633,
            "range": "± 148.725",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.2918,
            "range": "± 902.946",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 182.128,
            "range": "± 8.95139",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.5608,
            "range": "± 1.75638",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 353.202,
            "range": "± 14.5552",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.41025,
            "range": "± 230.027",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.42386,
            "range": "± 235.391",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.7501,
            "range": "± 2.22638",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.1619,
            "range": "± 688.716",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.2973,
            "range": "± 8.89133",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.6659,
            "range": "± 2.81922",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.6261,
            "range": "± 2.07573",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.13815,
            "range": "± 623.002",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.8983,
            "range": "± 4.27441",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.76816,
            "range": "± 1.59152",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.653,
            "range": "± 2.25374",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.22169,
            "range": "± 785.267",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.7498,
            "range": "± 4.17751",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.0596,
            "range": "± 2.20153",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.51714,
            "range": "± 1.57323",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.19425,
            "range": "± 1.32759",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.1533,
            "range": "± 2.21408",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.56892,
            "range": "± 968.205",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.64814,
            "range": "± 1.78078",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.34209,
            "range": "± 521.642",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.5681,
            "range": "± 10.5965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 79.1786,
            "range": "± 29.4638",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80832,
            "range": "± 40.1558",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.806,
            "range": "± 39.7801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 130.1,
            "range": "± 13.1809",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.8334,
            "range": "± 10.6306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.1849,
            "range": "± 457.081",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.67458,
            "range": "± 39.1478",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 176.68,
            "range": "± 11.6575",
            "unit": "ns",
            "extra": "100 samples\n174 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 437.379,
            "range": "± 25.1703",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11759,
            "range": "± 73.8592",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 481.253,
            "range": "± 111.878",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 861.759,
            "range": "± 68.0438",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 468.875,
            "range": "± 41.5534",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.914,
            "range": "± 36.8505",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 409.382,
            "range": "± 18.5515",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 481.375,
            "range": "± 52.7139",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 431.229,
            "range": "± 39.5013",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
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
          "id": "95e8887eadcab9082f544de66a0ff1b42d041c0b",
          "message": "Extract profile/assume into specialization plugin\n\nMove nautilus/include/nautilus/profile, the orphaned Profile.cpp,\nnautilus/src/nautilus/api/profile/, and the MLIR assume intrinsic\ninto a new plugins/specialization/ plugin (nautilus-specialization),\nmirroring the existing plugins/std and plugins/simd layouts. The\nplugin owns all code-specialization hints (branch-probability profile\ndata and nautilus_assume / nautilus_assume_aligned), is gated by the\nnew ENABLE_SPECIALIZATION_PLUGIN CMake option, and conditionally\ncompiles its MLIR-intrinsic glue when ENABLE_MLIR_BACKEND is on.\n\nThe corresponding assume tests and reference IR files move from\nnautilus/test into plugins/specialization/test, and core nautilus no\nlonger depends on profile.hpp via core.hpp.",
          "timestamp": "2026-04-07T17:44:50+02:00",
          "tree_id": "3985956d3f19df93fb846c24afaa5664a5c750b5",
          "url": "https://github.com/nebulastream/nautilus/commit/95e8887eadcab9082f544de66a0ff1b42d041c0b"
        },
        "date": 1775577073628,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 828.84,
            "range": "± 82.8408",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.327,
            "range": "± 173.678",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.04255,
            "range": "± 892.815",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.73721,
            "range": "± 341.761",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.38331,
            "range": "± 719.711",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.06827,
            "range": "± 423.488",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.53539,
            "range": "± 345.028",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.21064,
            "range": "± 284.943",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.90143,
            "range": "± 244.366",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.44606,
            "range": "± 270.622",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 17.9662,
            "range": "± 1.46283",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 192.903,
            "range": "± 8.96549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.1134,
            "range": "± 2.48612",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 328.153,
            "range": "± 11.1599",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.56063,
            "range": "± 543.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.74628,
            "range": "± 182.559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.65178,
            "range": "± 154.069",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.97263,
            "range": "± 369.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.6847,
            "range": "± 628.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5333,
            "range": "± 335.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.5773,
            "range": "± 705.955",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.77456,
            "range": "± 253.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4356,
            "range": "± 1.06491",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9623,
            "range": "± 164.38",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.4457,
            "range": "± 380.764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5814,
            "range": "± 409.777",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3656,
            "range": "± 373.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.6196,
            "range": "± 469.362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.389,
            "range": "± 585.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.7309,
            "range": "± 565.674",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.4164,
            "range": "± 444.738",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.5711,
            "range": "± 558.246",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.4761,
            "range": "± 568.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.6092,
            "range": "± 355.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.898,
            "range": "± 564.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.1455,
            "range": "± 561.703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.1894,
            "range": "± 714.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.435,
            "range": "± 749.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.7773,
            "range": "± 500.117",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.0098,
            "range": "± 633.939",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.466,
            "range": "± 483.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.387,
            "range": "± 1.38384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.5896,
            "range": "± 2.61709",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.1622,
            "range": "± 3.80834",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.1227,
            "range": "± 4.36256",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.4381,
            "range": "± 3.85839",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8386,
            "range": "± 3.31124",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.0956,
            "range": "± 3.80828",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.3831,
            "range": "± 4.21148",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.8146,
            "range": "± 2.77516",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.598,
            "range": "± 3.40187",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.4043,
            "range": "± 3.6494",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.5784,
            "range": "± 3.90233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 186.483,
            "range": "± 12.5132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.7139,
            "range": "± 8.20885",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 299.525,
            "range": "± 16.2828",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.73,
            "range": "± 5.16618",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 35.2904,
            "range": "± 6.86847",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 62.0638,
            "range": "± 9.89244",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 37.1357,
            "range": "± 5.52314",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.7663,
            "range": "± 10.4637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.6748,
            "range": "± 9.74794",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.9905,
            "range": "± 8.30711",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.2548,
            "range": "± 4.55249",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.8958,
            "range": "± 8.54968",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.7774,
            "range": "± 5.19491",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 111.247,
            "range": "± 13.9778",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.13736,
            "range": "± 22.5195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 166.553,
            "range": "± 15.5738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.29339,
            "range": "± 33.5179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 34.7667,
            "range": "± 4.73696",
            "unit": "ns",
            "extra": "100 samples\n874 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 279.973,
            "range": "± 10.1435",
            "unit": "ns",
            "extra": "100 samples\n8317 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09266,
            "range": "± 0.843569",
            "unit": "ns",
            "extra": "100 samples\n7760 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.7335,
            "range": "± 3.81894",
            "unit": "ns",
            "extra": "100 samples\n772 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 9.82703,
            "range": "± 1.19291",
            "unit": "ns",
            "extra": "100 samples\n9187 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.2455,
            "range": "± 1.80667",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 593.588,
            "range": "± 38.1341",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.78618,
            "range": "± 0.866728",
            "unit": "ns",
            "extra": "100 samples\n6614 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.4229,
            "range": "± 9.96581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0618,
            "range": "± 726.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.697,
            "range": "± 4.97672",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 900.218,
            "range": "± 11.7035",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 190.557,
            "range": "± 580.979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.21561,
            "range": "± 0.421704",
            "unit": "ns",
            "extra": "100 samples\n9353 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.4569,
            "range": "± 2.10364",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.60086,
            "range": "± 20.9016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.47069,
            "range": "± 250.901",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.50548,
            "range": "± 250.353",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.4901,
            "range": "± 1.56322",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.32981,
            "range": "± 605.531",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.4085,
            "range": "± 6.26712",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.6684,
            "range": "± 2.3932",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3268,
            "range": "± 1.54147",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.31795,
            "range": "± 601.613",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.2736,
            "range": "± 3.03981",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.2543,
            "range": "± 1.59787",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.521,
            "range": "± 1.81108",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.42546,
            "range": "± 657.84",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.8171,
            "range": "± 3.36746",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.3919,
            "range": "± 1.60025",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.23362,
            "range": "± 1.08211",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.95766,
            "range": "± 992.359",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.8777,
            "range": "± 1.81245",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.78543,
            "range": "± 860.229",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6716,
            "range": "± 1.6349",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.58411,
            "range": "± 477.927",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.115,
            "range": "± 8.59747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.2226,
            "range": "± 7.70709",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.74536,
            "range": "± 48.8679",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.79904,
            "range": "± 51.1365",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 142.554,
            "range": "± 23.4004",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.9689,
            "range": "± 8.48319",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.13355,
            "range": "± 153.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.76711,
            "range": "± 38.7664",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 185.546,
            "range": "± 6.1432",
            "unit": "ns",
            "extra": "100 samples\n156 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 472.625,
            "range": "± 41.2207",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.17951,
            "range": "± 116.072",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 483.839,
            "range": "± 28.9373",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 917.686,
            "range": "± 89.7529",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 505.34,
            "range": "± 39.7427",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 613.64,
            "range": "± 81.2383",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 497.755,
            "range": "± 32.5169",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 515.834,
            "range": "± 39.9811",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 457.468,
            "range": "± 32.8931",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 43.0024,
            "range": "± 14.081",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.29266,
            "range": "± 226.594",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.0653,
            "range": "± 770.35",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 43.4575,
            "range": "± 12.0455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 61.9168,
            "range": "± 13.0366",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.39337,
            "range": "± 317.623",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.7968,
            "range": "± 664.836",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.0301,
            "range": "± 13.0382",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 44.1003,
            "range": "± 11.8406",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.22028,
            "range": "± 239.865",
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
          "id": "7dd7ded569c89a9c171ee471704c59c9d7582c7c",
          "message": "Fix SessionStart hook config to use nested hooks format (#230)",
          "timestamp": "2026-04-07T21:29:26+02:00",
          "tree_id": "696180a80bcf33941da9791d74da1ccc8ba1d09e",
          "url": "https://github.com/nebulastream/nautilus/commit/7dd7ded569c89a9c171ee471704c59c9d7582c7c"
        },
        "date": 1775590444913,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 11.0294,
            "range": "± 0.603469",
            "unit": "ns",
            "extra": "100 samples\n6972 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 26.8136,
            "range": "± 3.14254",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 614.807,
            "range": "± 43.9001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.20464,
            "range": "± 0.561767",
            "unit": "ns",
            "extra": "100 samples\n3804 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 135.579,
            "range": "± 13.3432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 25.5231,
            "range": "± 156.676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 36.5842,
            "range": "± 4.05246",
            "unit": "ns",
            "extra": "100 samples\n518 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 553.216,
            "range": "± 7.99074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 119.732,
            "range": "± 2.97732",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.64308,
            "range": "± 0.26869",
            "unit": "ns",
            "extra": "100 samples\n6616 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 31.4886,
            "range": "± 3.51843",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 6.25855,
            "range": "± 27.5887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.77481,
            "range": "± 180.791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.32897,
            "range": "± 135.266",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.34742,
            "range": "± 169.228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.3405,
            "range": "± 153.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.5292,
            "range": "± 346.741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2806,
            "range": "± 201.065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.8419,
            "range": "± 337.512",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.10598,
            "range": "± 113.714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0514,
            "range": "± 162.165",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.041,
            "range": "± 181.716",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0831,
            "range": "± 182.397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.858,
            "range": "± 306.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.5672,
            "range": "± 186.033",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 61.2949,
            "range": "± 316.69",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 20.5121,
            "range": "± 443.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 21.4636,
            "range": "± 537.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 22.5806,
            "range": "± 1.29615",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 21.3561,
            "range": "± 510.811",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 22.743,
            "range": "± 1.16351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 21.6898,
            "range": "± 631.908",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 21.6663,
            "range": "± 440.274",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 21.5597,
            "range": "± 321.772",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 21.1358,
            "range": "± 402.59",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 21.4742,
            "range": "± 399.205",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 24.175,
            "range": "± 472.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 56.3779,
            "range": "± 651.424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 26.3953,
            "range": "± 493.336",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 84.601,
            "range": "± 639.777",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 9.72866,
            "range": "± 1.62073",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 12.0263,
            "range": "± 2.45637",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 16.149,
            "range": "± 2.3084",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.116,
            "range": "± 2.38486",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 15.1574,
            "range": "± 2.52039",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 12.4449,
            "range": "± 1.93608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 13.7825,
            "range": "± 2.1957",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 11.7076,
            "range": "± 2.23981",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 12.6031,
            "range": "± 2.32978",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 12.2939,
            "range": "± 2.93608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.8599,
            "range": "± 4.40075",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 173.852,
            "range": "± 9.81585",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 39.3915,
            "range": "± 5.19911",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 272.443,
            "range": "± 7.64838",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 14.6077,
            "range": "± 3.61815",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.8515,
            "range": "± 7.30207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 45.6242,
            "range": "± 5.93825",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 28.9618,
            "range": "± 6.61477",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 43.1982,
            "range": "± 5.8359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 33.9389,
            "range": "± 6.64444",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.3132,
            "range": "± 6.52215",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.7839,
            "range": "± 6.58456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 33.0371,
            "range": "± 6.86636",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.1259,
            "range": "± 7.44217",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 92.1167,
            "range": "± 10.5315",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.14953,
            "range": "± 28.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 148.429,
            "range": "± 14.0795",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.20779,
            "range": "± 88.1008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 30.1651,
            "range": "± 9.30785",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.59213,
            "range": "± 219.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 29.2563,
            "range": "± 8.58773",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.74062,
            "range": "± 136.7",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 20.6409,
            "range": "± 477.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 29.7801,
            "range": "± 7.71166",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 45.7219,
            "range": "± 10.6074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.53938,
            "range": "± 243.534",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 21.6508,
            "range": "± 1.88714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 48.0456,
            "range": "± 11.0947",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 31.8321,
            "range": "± 2.75426",
            "unit": "ns",
            "extra": "100 samples\n588 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 396.536,
            "range": "± 9.98973",
            "unit": "ns",
            "extra": "100 samples\n6200 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.77511,
            "range": "± 0.536213",
            "unit": "ns",
            "extra": "100 samples\n5650 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 35.7228,
            "range": "± 2.054",
            "unit": "ns",
            "extra": "100 samples\n494 iterations"
          },
          {
            "name": "ir_add",
            "value": 879.574,
            "range": "± 51.7679",
            "unit": "ns",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.80733,
            "range": "± 172.337",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.88459,
            "range": "± 558.644",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.46198,
            "range": "± 172.77",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.70178,
            "range": "± 353.66",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.76281,
            "range": "± 274.67",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.48214,
            "range": "± 271.778",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.51811,
            "range": "± 199.194",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.75338,
            "range": "± 245.272",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 3.15505,
            "range": "± 372.668",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.4914,
            "range": "± 752.92",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 200.601,
            "range": "± 3.42312",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 36.0266,
            "range": "± 2.39128",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 381.372,
            "range": "± 8.70052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.92644,
            "range": "± 233.616",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.84497,
            "range": "± 328.532",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.9129,
            "range": "± 1.77311",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.99067,
            "range": "± 689.734",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.006,
            "range": "± 4.97286",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 17.0667,
            "range": "± 2.50747",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.0702,
            "range": "± 2.49141",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 6.31386,
            "range": "± 1.14353",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.2113,
            "range": "± 5.02586",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 11.2965,
            "range": "± 1.61701",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.89,
            "range": "± 2.26742",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.96112,
            "range": "± 518.877",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.7238,
            "range": "± 4.99758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.9419,
            "range": "± 1.23945",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.93685,
            "range": "± 1.01228",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.71926,
            "range": "± 1.46314",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.2135,
            "range": "± 1.48796",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.68904,
            "range": "± 698.267",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.7953,
            "range": "± 1.49385",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.99688,
            "range": "± 396.924",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.5275,
            "range": "± 6.29741",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 56.0437,
            "range": "± 6.79789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.72585,
            "range": "± 35.4979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.73121,
            "range": "± 30.215",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 135.714,
            "range": "± 10.8866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.5128,
            "range": "± 6.96267",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.69034,
            "range": "± 38.0962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.93197,
            "range": "± 44.1039",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 194.025,
            "range": "± 19.6652",
            "unit": "ns",
            "extra": "100 samples\n94 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 445.54,
            "range": "± 19.5244",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.29414,
            "range": "± 93.0739",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 472.727,
            "range": "± 13.8183",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1055.6000000000001,
            "range": "± 65045.59999999999",
            "unit": "ns",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 488.778,
            "range": "± 25.993",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 589.188,
            "range": "± 33.7869",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 454.927,
            "range": "± 23.8021",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 501.477,
            "range": "± 23.8892",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 453.101,
            "range": "± 20.0905",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
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
          "id": "4ddc6902dd11422a5839cfdc838866ebca051811",
          "message": "Fix SessionStart hook timing out on Claude Code web (#232)\n\nCo-authored-by: Claude Opus 4.6 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-04-08T22:25:59+02:00",
          "tree_id": "c029f0fcc121720b71cc5353ab20db1128f3d0be",
          "url": "https://github.com/nebulastream/nautilus/commit/4ddc6902dd11422a5839cfdc838866ebca051811"
        },
        "date": 1775680220147,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 36.1323,
            "range": "± 10.3872",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.69496,
            "range": "± 416.394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.6951,
            "range": "± 481.83",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 42.6381,
            "range": "± 11.2121",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 64.5505,
            "range": "± 12.6459",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.04856,
            "range": "± 286.628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.4246,
            "range": "± 571.506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 61.7793,
            "range": "± 14.2547",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.2965,
            "range": "± 1.03277",
            "unit": "ns",
            "extra": "100 samples\n9295 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.9968,
            "range": "± 2.35392",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 594.472,
            "range": "± 29.2638",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.73691,
            "range": "± 0.799371",
            "unit": "ns",
            "extra": "100 samples\n6596 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 99.3508,
            "range": "± 15.1486",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9916,
            "range": "± 159.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.2275,
            "range": "± 6.98237",
            "unit": "ns",
            "extra": "100 samples\n707 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 902.474,
            "range": "± 10.0672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 190.799,
            "range": "± 397.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.24248,
            "range": "± 0.494265",
            "unit": "ns",
            "extra": "100 samples\n9294 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.3141,
            "range": "± 1.36548",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.62116,
            "range": "± 62.4678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.742,
            "range": "± 12.6185",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.36387,
            "range": "± 191.751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.2873,
            "range": "± 7.38083",
            "unit": "ns",
            "extra": "100 samples\n870 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 299.046,
            "range": "± 11.2662",
            "unit": "ns",
            "extra": "100 samples\n8290 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.03145,
            "range": "± 0.734027",
            "unit": "ns",
            "extra": "100 samples\n7741 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.1182,
            "range": "± 2.30459",
            "unit": "ns",
            "extra": "100 samples\n777 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.56751,
            "range": "± 312.946",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.62229,
            "range": "± 366.074",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.1875,
            "range": "± 2.1946",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.66397,
            "range": "± 836.071",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.7092,
            "range": "± 7.02971",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.7523,
            "range": "± 3.59692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3827,
            "range": "± 1.74982",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.34366,
            "range": "± 671.493",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.2593,
            "range": "± 3.89688",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.3211,
            "range": "± 1.4055",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.5762,
            "range": "± 1.87128",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.55179,
            "range": "± 776.341",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.7094,
            "range": "± 3.40857",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.8254,
            "range": "± 1.97681",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.39437,
            "range": "± 1.13559",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.27847,
            "range": "± 1.29479",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.8422,
            "range": "± 3.27903",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.03076,
            "range": "± 996.33",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.8536,
            "range": "± 2.31146",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.59746,
            "range": "± 532.736",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.6039,
            "range": "± 9.13368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 57.1039,
            "range": "± 8.66097",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.75731,
            "range": "± 48.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.80528,
            "range": "± 61.1031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 142.906,
            "range": "± 20.5703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 71.253,
            "range": "± 10.5092",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.11334,
            "range": "± 59.9812",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.77679,
            "range": "± 53.4349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 196.852,
            "range": "± 18.2072",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 486.861,
            "range": "± 70.2234",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.1809,
            "range": "± 106.412",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 514.039,
            "range": "± 54.5865",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 943.877,
            "range": "± 70.5854",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 506.317,
            "range": "± 59.915",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 614.184,
            "range": "± 51.7513",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 541.031,
            "range": "± 83.2034",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 527.105,
            "range": "± 56.114",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 487.012,
            "range": "± 85.6455",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.38057,
            "range": "± 192.605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.14395,
            "range": "± 187.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.7788,
            "range": "± 165.806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.9357,
            "range": "± 189.58",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.4149,
            "range": "± 339.44",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7687,
            "range": "± 880.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.4114,
            "range": "± 282.375",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.69672,
            "range": "± 127.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.2546,
            "range": "± 177.381",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4331,
            "range": "± 330.253",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.5025,
            "range": "± 306.971",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.4857,
            "range": "± 419.81",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.7826,
            "range": "± 270.187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.0902,
            "range": "± 540.118",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.6204,
            "range": "± 845.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.9943,
            "range": "± 587.031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.8513,
            "range": "± 1.22198",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.5132,
            "range": "± 801.141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.3091,
            "range": "± 703.368",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.1496,
            "range": "± 557.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.0546,
            "range": "± 527.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.7821,
            "range": "± 833.768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.1467,
            "range": "± 746.022",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.283,
            "range": "± 1.10201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.2778,
            "range": "± 475.46",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.5057,
            "range": "± 680.203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.8186,
            "range": "± 570.568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.8954,
            "range": "± 3.63672",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.5932,
            "range": "± 2.09862",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.5758,
            "range": "± 2.73918",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 24.34,
            "range": "± 5.64301",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.7744,
            "range": "± 4.12069",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.7524,
            "range": "± 2.69374",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.1342,
            "range": "± 2.85075",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.412,
            "range": "± 3.05277",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.3441,
            "range": "± 3.25445",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.1498,
            "range": "± 2.86824",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.3923,
            "range": "± 3.5189",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.96,
            "range": "± 4.1926",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 201.72,
            "range": "± 33.1657",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 55.8101,
            "range": "± 13.0197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 302.553,
            "range": "± 23.62",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.3937,
            "range": "± 4.48756",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.8408,
            "range": "± 4.85456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.2774,
            "range": "± 9.17425",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 42.025,
            "range": "± 7.05867",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 60.0379,
            "range": "± 10.8082",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.8621,
            "range": "± 9.77711",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.4541,
            "range": "± 11.156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.8697,
            "range": "± 4.41559",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 45.3356,
            "range": "± 9.25608",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.7476,
            "range": "± 5.30607",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 111.032,
            "range": "± 14.5965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.14527,
            "range": "± 20.1439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 166.561,
            "range": "± 15.152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.27493,
            "range": "± 36.9212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 899.987,
            "range": "± 116.357",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.50916,
            "range": "± 309.896",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.62996,
            "range": "± 689.783",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.15779,
            "range": "± 491.542",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.78225,
            "range": "± 651.21",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.43855,
            "range": "± 555.66",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.81923,
            "range": "± 449.197",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.28226,
            "range": "± 197.398",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.15796,
            "range": "± 298.215",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.71337,
            "range": "± 324.906",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.2159,
            "range": "± 1.70068",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 195.124,
            "range": "± 18.2958",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.4978,
            "range": "± 3.21451",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 372.238,
            "range": "± 13.6387",
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
          "id": "d2b8a291514ea9e3fd20f5c2ac65b8075f961e9e",
          "message": "Remove SessionStart hook and setup.sh (#233)\n\nCo-authored-by: Claude Opus 4.6 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-04-08T22:36:20+02:00",
          "tree_id": "ed55e648c63c1c03917195683294516a2b244854",
          "url": "https://github.com/nebulastream/nautilus/commit/d2b8a291514ea9e3fd20f5c2ac65b8075f961e9e"
        },
        "date": 1775680856535,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.3348,
            "range": "± 5.17593",
            "unit": "ns",
            "extra": "100 samples\n9768 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.2866,
            "range": "± 2.47701",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 543.107,
            "range": "± 59.3544",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.56947,
            "range": "± 0.479919",
            "unit": "ns",
            "extra": "100 samples\n7215 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 108.166,
            "range": "± 5.97934",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5361,
            "range": "± 66.8694",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 41.8684,
            "range": "± 0.546566",
            "unit": "ns",
            "extra": "100 samples\n738 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 719.008,
            "range": "± 9.79299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 167.896,
            "range": "± 20.3337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.18457,
            "range": "± 0.21706",
            "unit": "ns",
            "extra": "100 samples\n9772 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.0087,
            "range": "± 769.359",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.15039,
            "range": "± 39.3607",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 170.98,
            "range": "± 4.85832",
            "unit": "ns",
            "extra": "100 samples\n179 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 425.963,
            "range": "± 22.4285",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.08509,
            "range": "± 68.59",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 448.9,
            "range": "± 26.019",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 843.341,
            "range": "± 56.6592",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 451.031,
            "range": "± 26.9919",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 550.226,
            "range": "± 36.6782",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 460.955,
            "range": "± 29.753",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 468.788,
            "range": "± 48.5415",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 416.121,
            "range": "± 27.4092",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 38.0228,
            "range": "± 12.787",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.87792,
            "range": "± 143.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.42275,
            "range": "± 239.757",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.4231,
            "range": "± 221.483",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.5884,
            "range": "± 1.94099",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.04761,
            "range": "± 602.813",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.9434,
            "range": "± 7.43363",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.0339,
            "range": "± 2.08351",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.6337,
            "range": "± 1.94705",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.0417,
            "range": "± 551.304",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.0288,
            "range": "± 4.34503",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.6898,
            "range": "± 1.57466",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.662,
            "range": "± 1.61478",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.17839,
            "range": "± 690.186",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.5925,
            "range": "± 3.67511",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.9194,
            "range": "± 1.87274",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.77025,
            "range": "± 1.88176",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.04461,
            "range": "± 1.06801",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2238,
            "range": "± 2.46455",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.42545,
            "range": "± 840.647",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.59467,
            "range": "± 1.47377",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.42914,
            "range": "± 649.545",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.197,
            "range": "± 8.47069",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.4724,
            "range": "± 8.89742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80644,
            "range": "± 43.545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.78683,
            "range": "± 37.8815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 131.821,
            "range": "± 14.7711",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.9571,
            "range": "± 9.72198",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.24673,
            "range": "± 108.063",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.65692,
            "range": "± 40.1471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 37.6291,
            "range": "± 12.6215",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.83481,
            "range": "± 149.404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.1148,
            "range": "± 318.385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 37.403,
            "range": "± 13.5509",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 50.6443,
            "range": "± 15.8765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.93392,
            "range": "± 182.052",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.8107,
            "range": "± 312.621",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 51.5223,
            "range": "± 15.9763",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.93355,
            "range": "± 157.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.63533,
            "range": "± 193.249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.45589,
            "range": "± 151.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.7365,
            "range": "± 241.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.5726,
            "range": "± 3.31609",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.5118,
            "range": "± 321.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.4419,
            "range": "± 1.40568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.41661,
            "range": "± 486.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.015,
            "range": "± 205.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.8711,
            "range": "± 246.139",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.8338,
            "range": "± 146.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.0278,
            "range": "± 230.365",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.1486,
            "range": "± 410.011",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.9331,
            "range": "± 145.681",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.8229,
            "range": "± 912.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.2266,
            "range": "± 273.169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.3637,
            "range": "± 243.367",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.5741,
            "range": "± 772.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.3177,
            "range": "± 288.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.6434,
            "range": "± 433.208",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.8337,
            "range": "± 354.326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2192,
            "range": "± 553.46",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.7481,
            "range": "± 339.84",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.4252,
            "range": "± 288.006",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.5084,
            "range": "± 335.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.1334,
            "range": "± 1.8847",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.961,
            "range": "± 389.531",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.6512,
            "range": "± 527.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.6461,
            "range": "± 2.42219",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.1314,
            "range": "± 2.51491",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.5705,
            "range": "± 3.71083",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 15.8675,
            "range": "± 2.58165",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.4541,
            "range": "± 3.41446",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.204,
            "range": "± 2.67337",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.6459,
            "range": "± 3.61179",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.6485,
            "range": "± 3.30633",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.8708,
            "range": "± 3.39159",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.0544,
            "range": "± 3.3158",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 33.0065,
            "range": "± 4.2385",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 171.511,
            "range": "± 9.72957",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 42.1429,
            "range": "± 7.06907",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 270.863,
            "range": "± 14.2749",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.18,
            "range": "± 4.76377",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.6899,
            "range": "± 5.52798",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 53.0395,
            "range": "± 14.1167",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.1359,
            "range": "± 5.57951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 51.8255,
            "range": "± 12.4362",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 33.3045,
            "range": "± 5.8913",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.9761,
            "range": "± 6.59086",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.2139,
            "range": "± 4.63454",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 32.0782,
            "range": "± 5.9502",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.1099,
            "range": "± 5.88173",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 104.705,
            "range": "± 15.271",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.16511,
            "range": "± 20.2782",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 157.946,
            "range": "± 14.904",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.34178,
            "range": "± 28.6578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 842.653,
            "range": "± 50.7158",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.37089,
            "range": "± 163.767",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.59418,
            "range": "± 469.519",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.82116,
            "range": "± 206.856",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.70098,
            "range": "± 497.6",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.02479,
            "range": "± 191.905",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.65627,
            "range": "± 296.73",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.21155,
            "range": "± 111.873",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.10902,
            "range": "± 233.325",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.55584,
            "range": "± 187.075",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.4874,
            "range": "± 1.15822",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 182.747,
            "range": "± 6.57732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.1806,
            "range": "± 1.81878",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 356.294,
            "range": "± 11.9496",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.7665,
            "range": "± 1.54517",
            "unit": "ns",
            "extra": "100 samples\n854 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 275.255,
            "range": "± 24.2807",
            "unit": "ns",
            "extra": "100 samples\n9551 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.77955,
            "range": "± 0.521555",
            "unit": "ns",
            "extra": "100 samples\n8689 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.4683,
            "range": "± 2.23378",
            "unit": "ns",
            "extra": "100 samples\n790 iterations"
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
          "id": "fe6a9d3d5698e5cb343e71585b56c5694b44712e",
          "message": "Expand example/ with module, backend, and plugin demos (#237)",
          "timestamp": "2026-04-09T11:09:52+02:00",
          "tree_id": "8d82faca549ecc8a299a5d55a311046182dc3691",
          "url": "https://github.com/nebulastream/nautilus/commit/fe6a9d3d5698e5cb343e71585b56c5694b44712e"
        },
        "date": 1775726079274,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 41.9402,
            "range": "± 11.479",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.19261,
            "range": "± 319.514",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 41.9319,
            "range": "± 12.9892",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.2406,
            "range": "± 179.957",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.7772,
            "range": "± 1.13268",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 45.7755,
            "range": "± 14.0983",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.8482,
            "range": "± 18.8346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.33829,
            "range": "± 302.6",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 31.3679,
            "range": "± 38.4374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 61.6821,
            "range": "± 13.8625",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.6295,
            "range": "± 393.784",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.81877,
            "range": "± 470.153",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.8187,
            "range": "± 2.08855",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.63162,
            "range": "± 880.397",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.8108,
            "range": "± 7.02691",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.4573,
            "range": "± 3.2013",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.506,
            "range": "± 1.91965",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.52025,
            "range": "± 802.616",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.7038,
            "range": "± 4.03005",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.4004,
            "range": "± 1.66738",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.4685,
            "range": "± 2.44647",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.70929,
            "range": "± 966.155",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.4329,
            "range": "± 2.88224",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.0943,
            "range": "± 2.60049",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.9204,
            "range": "± 1.78609",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.24751,
            "range": "± 1.3093",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.6497,
            "range": "± 2.47465",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.27782,
            "range": "± 1.24886",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.9443,
            "range": "± 1.9959",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.71866,
            "range": "± 752.855",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.6254,
            "range": "± 9.95174",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.7434,
            "range": "± 8.55476",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.77313,
            "range": "± 40.0186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.80575,
            "range": "± 58.8597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 140.635,
            "range": "± 13.7738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.6933,
            "range": "± 10.2417",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.11147,
            "range": "± 47.5098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.82818,
            "range": "± 50.1021",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.6249,
            "range": "± 1.42148",
            "unit": "ns",
            "extra": "100 samples\n9245 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.857,
            "range": "± 1.5226",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 541.035,
            "range": "± 17.7471",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.62287,
            "range": "± 0.488662",
            "unit": "ns",
            "extra": "100 samples\n6572 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.7492,
            "range": "± 9.93046",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.946,
            "range": "± 297.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 41.6262,
            "range": "± 3.93318",
            "unit": "ns",
            "extra": "100 samples\n707 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 901.122,
            "range": "± 9.20178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 190.988,
            "range": "± 1.91552",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.21029,
            "range": "± 0.378762",
            "unit": "ns",
            "extra": "100 samples\n9306 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.3882,
            "range": "± 1.92957",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.61101,
            "range": "± 109.34",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.0049,
            "range": "± 4.82373",
            "unit": "ns",
            "extra": "100 samples\n867 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 281.225,
            "range": "± 6.32514",
            "unit": "ns",
            "extra": "100 samples\n8293 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.08113,
            "range": "± 0.914413",
            "unit": "ns",
            "extra": "100 samples\n7732 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.1748,
            "range": "± 2.39122",
            "unit": "ns",
            "extra": "100 samples\n778 iterations"
          },
          {
            "name": "ir_add",
            "value": 833.369,
            "range": "± 48.8473",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48942,
            "range": "± 213.932",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.77143,
            "range": "± 635.178",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.96153,
            "range": "± 350.1",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.7119,
            "range": "± 530.053",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.21513,
            "range": "± 408.706",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.79664,
            "range": "± 363.331",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.26301,
            "range": "± 142.174",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.1412,
            "range": "± 283.133",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.59626,
            "range": "± 204.827",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.6459,
            "range": "± 4.17028",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 195.488,
            "range": "± 9.72315",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.2523,
            "range": "± 2.10356",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 371.301,
            "range": "± 13.1921",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.20744,
            "range": "± 168.383",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.80395,
            "range": "± 179.401",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.70838,
            "range": "± 139.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.78855,
            "range": "± 199.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.3979,
            "range": "± 399.83",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4993,
            "range": "± 292.55",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.4333,
            "range": "± 470.701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.66245,
            "range": "± 154.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1799,
            "range": "± 323.177",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1083,
            "range": "± 228.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1603,
            "range": "± 229.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.4259,
            "range": "± 347.355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.2247,
            "range": "± 226.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.1401,
            "range": "± 305.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.9648,
            "range": "± 340.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.6026,
            "range": "± 463.604",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.6265,
            "range": "± 397.68",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.6468,
            "range": "± 323.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.572,
            "range": "± 398.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.9929,
            "range": "± 408.174",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.1117,
            "range": "± 373.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.2515,
            "range": "± 375.628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.8287,
            "range": "± 356.428",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.5869,
            "range": "± 371.504",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.7596,
            "range": "± 539.363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.2869,
            "range": "± 561.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.9914,
            "range": "± 428.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.0165,
            "range": "± 685.375",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2972,
            "range": "± 2.34444",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.8677,
            "range": "± 4.35107",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.6857,
            "range": "± 3.83584",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.905,
            "range": "± 3.08772",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.9264,
            "range": "± 3.77924",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.665,
            "range": "± 3.00054",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.9587,
            "range": "± 3.88089",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.009,
            "range": "± 3.29938",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.2126,
            "range": "± 3.04775",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.8563,
            "range": "± 3.05088",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.9368,
            "range": "± 7.20614",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 186.739,
            "range": "± 13.9736",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.4926,
            "range": "± 7.96463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 295.288,
            "range": "± 14.6911",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.4028,
            "range": "± 4.14196",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.0824,
            "range": "± 4.93667",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.9513,
            "range": "± 9.61591",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 41.8765,
            "range": "± 8.96387",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 60.0248,
            "range": "± 9.96152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.0714,
            "range": "± 8.8334",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.7225,
            "range": "± 9.62593",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.6752,
            "range": "± 4.66518",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 45.3499,
            "range": "± 10.2094",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.6273,
            "range": "± 4.88533",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 110.989,
            "range": "± 12.5767",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.1563,
            "range": "± 51.5047",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 168.129,
            "range": "± 17.3891",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.31108,
            "range": "± 25.7821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 186.603,
            "range": "± 11.4032",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 488.751,
            "range": "± 49.9582",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18965,
            "range": "± 140.949",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 499.441,
            "range": "± 35.4731",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 932.937,
            "range": "± 87.1075",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 501.577,
            "range": "± 41.0118",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 608.833,
            "range": "± 39.2644",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 512.791,
            "range": "± 42.2168",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 513.534,
            "range": "± 37.5048",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 455.224,
            "range": "± 35.7354",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
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
          "id": "22ca33ff501e75ee5f674d169bc978fa533737c4",
          "message": "Fix static plugin registration: dead ifdef guard and missing simd init (#236)",
          "timestamp": "2026-04-10T10:31:12+02:00",
          "tree_id": "4f71a2a62fbe2a86a45379c02194d0578cbe4422",
          "url": "https://github.com/nebulastream/nautilus/commit/22ca33ff501e75ee5f674d169bc978fa533737c4"
        },
        "date": 1775810181504,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 171.539,
            "range": "± 9.25368",
            "unit": "ns",
            "extra": "100 samples\n179 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 423.638,
            "range": "± 28.1211",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04774,
            "range": "± 68.5887",
            "unit": "us",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 442.997,
            "range": "± 28.7764",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 803.327,
            "range": "± 67.8625",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 448.846,
            "range": "± 32.8085",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 559.886,
            "range": "± 78.5828",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 467.307,
            "range": "± 95.2847",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 462.488,
            "range": "± 26.9806",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 412.439,
            "range": "± 24.3313",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 37.3702,
            "range": "± 15.8583",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 11.5599,
            "range": "± 96.5854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 37.5981,
            "range": "± 16.8852",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 9.46712,
            "range": "± 578.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 34.4258,
            "range": "± 13.0258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 38.2487,
            "range": "± 16.3597",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 51.9731,
            "range": "± 20.3457",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 11.8173,
            "range": "± 329.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 33.8231,
            "range": "± 439.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 52.4656,
            "range": "± 21.256",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 11.7485,
            "range": "± 99.9854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 12.4908,
            "range": "± 89.993",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 11.0167,
            "range": "± 95.6175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 14.1865,
            "range": "± 809.011",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 41.8996,
            "range": "± 475.694",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 19.1377,
            "range": "± 339.189",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 40.6643,
            "range": "± 379.263",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 10.9627,
            "range": "± 76.6244",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 17.658,
            "range": "± 290.369",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 16.2351,
            "range": "± 125.794",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 17.3716,
            "range": "± 119.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 32.5583,
            "range": "± 677.681",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 16.7107,
            "range": "± 283.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 28.4055,
            "range": "± 140.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 32.9337,
            "range": "± 257.244",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 33.5191,
            "range": "± 170.916",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 34.572,
            "range": "± 210.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 33.7965,
            "range": "± 705.688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 34.8206,
            "range": "± 299.005",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 34.0292,
            "range": "± 198.788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 34.2479,
            "range": "± 162.337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 33.5391,
            "range": "± 2.20025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 33.804,
            "range": "± 155.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 33.6367,
            "range": "± 178.209",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 37.0222,
            "range": "± 753.983",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 72.8204,
            "range": "± 313.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 39.3456,
            "range": "± 361.041",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 106.784,
            "range": "± 1.63251",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7572,
            "range": "± 3.69206",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.0461,
            "range": "± 3.12913",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.9672,
            "range": "± 4.32852",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.4296,
            "range": "± 3.63443",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.2384,
            "range": "± 7.07054",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.1852,
            "range": "± 5.64079",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.0018,
            "range": "± 4.78301",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.8486,
            "range": "± 4.53102",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.905,
            "range": "± 4.1884",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.9891,
            "range": "± 3.15459",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.6403,
            "range": "± 5.38152",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 220.993,
            "range": "± 20.5731",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 44.2729,
            "range": "± 9.3964",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 355.721,
            "range": "± 18.3642",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.0998,
            "range": "± 6.3528",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 29.1058,
            "range": "± 9.82676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 54.4413,
            "range": "± 14.6152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 31.0195,
            "range": "± 8.0288",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 52.6171,
            "range": "± 13.3132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 34.4858,
            "range": "± 7.2221",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 36.7578,
            "range": "± 7.86122",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.5178,
            "range": "± 6.9802",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 33.5993,
            "range": "± 7.26043",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 31.1843,
            "range": "± 9.22113",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 110.782,
            "range": "± 19.3137",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.52806,
            "range": "± 96.7242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 170.948,
            "range": "± 22.8617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.61033,
            "range": "± 102.458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.37803,
            "range": "± 265.317",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.34569,
            "range": "± 249.197",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.0543,
            "range": "± 2.53309",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.10003,
            "range": "± 717.674",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.1107,
            "range": "± 9.82728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.0294,
            "range": "± 2.91673",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5105,
            "range": "± 1.85076",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.08596,
            "range": "± 846.706",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.907,
            "range": "± 4.01075",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.71674,
            "range": "± 1.72338",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7433,
            "range": "± 2.48961",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.17473,
            "range": "± 956.115",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.8775,
            "range": "± 5.10256",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.3721,
            "range": "± 2.43775",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.4871,
            "range": "± 1.52484",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.19397,
            "range": "± 1.48704",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.5223,
            "range": "± 4.4623",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.56579,
            "range": "± 1.01417",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.64268,
            "range": "± 1.88434",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.30809,
            "range": "± 620.376",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.4261,
            "range": "± 10.9831",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.9247,
            "range": "± 10.6085",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.84309,
            "range": "± 49.0471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.83372,
            "range": "± 52.7736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 128.399,
            "range": "± 15.6269",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 70.8233,
            "range": "± 12.1771",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.18712,
            "range": "± 153.99",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.7516,
            "range": "± 65.4295",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 14.8182,
            "range": "± 4.3945",
            "unit": "ns",
            "extra": "100 samples\n9768 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 18.0786,
            "range": "± 1.7764",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 303.528,
            "range": "± 37.3116",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.96538,
            "range": "± 1.38453",
            "unit": "ns",
            "extra": "100 samples\n7116 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.115,
            "range": "± 7.11952",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.3506,
            "range": "± 112.431",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.0925,
            "range": "± 4.27633",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 720.447,
            "range": "± 11.2675",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 163.834,
            "range": "± 4.96291",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.59237,
            "range": "± 0.430107",
            "unit": "ns",
            "extra": "100 samples\n9782 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.5169,
            "range": "± 3.23847",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85686,
            "range": "± 58.7515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.2349,
            "range": "± 5.72998",
            "unit": "ns",
            "extra": "100 samples\n864 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 370.863,
            "range": "± 3.25993",
            "unit": "ns",
            "extra": "100 samples\n9478 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.98697,
            "range": "± 0.828769",
            "unit": "ns",
            "extra": "100 samples\n8677 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3099,
            "range": "± 1.49155",
            "unit": "ns",
            "extra": "100 samples\n808 iterations"
          },
          {
            "name": "ir_add",
            "value": 810.674,
            "range": "± 42.8225",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.39728,
            "range": "± 193.038",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.59593,
            "range": "± 729.407",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.81865,
            "range": "± 225.12",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.5236,
            "range": "± 550.673",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.99675,
            "range": "± 266.497",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.62353,
            "range": "± 330.583",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25829,
            "range": "± 149.869",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.0519,
            "range": "± 216.019",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.51091,
            "range": "± 216.285",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.8579,
            "range": "± 2.25157",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 189.331,
            "range": "± 6.48985",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.5171,
            "range": "± 2.16578",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 396.003,
            "range": "± 17.9253",
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
          "id": "734a687d22d3c7eb8103c7ce391ec16492c51a65",
          "message": "Remove dead llvm-diff CMake block that emitted misleading warning\n\nThe find_program(LLVM_DIFF llvm-diff) block in\nnautilus/test/llvm-ir-test/CMakeLists.txt looked for an unversioned\nllvm-diff binary, which is never installed (CI only installs\nllvm-diff-21), so configure always emitted a CMake warning claiming\n\"llvm-diff not found - IR comparison tests will be skipped\".\n\nThe block was entirely dead code: the ctest it registered ran a\ncompare-ir.sh script that does not exist in the repository, and the\nmatching install(PROGRAMS compare-ir.sh) rule referenced the same\nmissing file. The real IR comparison happens inside the Catch2 test\nbinary via LLVMIRTestUtil.hpp, which does its own runtime lookup for\nllvm-diff-21/20/19 and is unaffected.\n\nRemove the dead block (and the install rule) to silence the warning\nwithout changing any test behavior. The generate-reference-ir custom\ntarget is preserved.",
          "timestamp": "2026-04-12T08:36:51+02:00",
          "tree_id": "d63f3aae0f0fdc8a8985f4c7867ee30f13faee86",
          "url": "https://github.com/nebulastream/nautilus/commit/734a687d22d3c7eb8103c7ce391ec16492c51a65"
        },
        "date": 1775976068181,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.11654,
            "range": "± 350.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.64677,
            "range": "± 125.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.58879,
            "range": "± 124.641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.76225,
            "range": "± 484.735",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.9505,
            "range": "± 1.0374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1357,
            "range": "± 182.814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.8572,
            "range": "± 276.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.54251,
            "range": "± 127.024",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9104,
            "range": "± 275.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8131,
            "range": "± 166.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8986,
            "range": "± 124.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.9755,
            "range": "± 208.667",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0152,
            "range": "± 238.244",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.6531,
            "range": "± 403.723",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6775,
            "range": "± 369.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.1598,
            "range": "± 333.875",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.3034,
            "range": "± 429.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3542,
            "range": "± 411.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2128,
            "range": "± 375.659",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5322,
            "range": "± 328.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.7501,
            "range": "± 434.525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.0082,
            "range": "± 347.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.4773,
            "range": "± 356.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3192,
            "range": "± 496.499",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.1689,
            "range": "± 385.903",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.7218,
            "range": "± 691.759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.5303,
            "range": "± 432.466",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.9244,
            "range": "± 436.203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.6066,
            "range": "± 2.47233",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.4776,
            "range": "± 3.68718",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 28.7794,
            "range": "± 6.94227",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.186,
            "range": "± 2.94494",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8035,
            "range": "± 2.9607",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.9318,
            "range": "± 2.95228",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.1052,
            "range": "± 3.50296",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.9466,
            "range": "± 2.82843",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.9218,
            "range": "± 3.51599",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.1209,
            "range": "± 3.14316",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.6096,
            "range": "± 3.68157",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 180.99,
            "range": "± 12.0941",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.7417,
            "range": "± 6.0257",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 289.508,
            "range": "± 17.8685",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.8707,
            "range": "± 3.24238",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.7344,
            "range": "± 5.90323",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.2853,
            "range": "± 11.6856",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 43.2016,
            "range": "± 6.52364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.1199,
            "range": "± 8.6738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.5011,
            "range": "± 8.93761",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.0321,
            "range": "± 7.45935",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.787,
            "range": "± 3.30794",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.6548,
            "range": "± 5.79775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.9398,
            "range": "± 4.43321",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 112.103,
            "range": "± 12.3865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.13428,
            "range": "± 26.7736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 166.73,
            "range": "± 12.677",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.25556,
            "range": "± 24.0638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.3172,
            "range": "± 5.33778",
            "unit": "ns",
            "extra": "100 samples\n859 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 275.961,
            "range": "± 3.50229",
            "unit": "ns",
            "extra": "100 samples\n8245 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.93351,
            "range": "± 0.435425",
            "unit": "ns",
            "extra": "100 samples\n7721 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 37.5257,
            "range": "± 1.93697",
            "unit": "ns",
            "extra": "100 samples\n761 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 41.9396,
            "range": "± 11.0264",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.10285,
            "range": "± 174.793",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.3627,
            "range": "± 334.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 41.96,
            "range": "± 5.76055",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 60.767,
            "range": "± 9.15065",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.06318,
            "range": "± 86.0483",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.4062,
            "range": "± 695.31",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 62.8869,
            "range": "± 9.58163",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 193.047,
            "range": "± 20.7548",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 471.205,
            "range": "± 40.3802",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18334,
            "range": "± 162.878",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 499.417,
            "range": "± 38.1097",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 921.199,
            "range": "± 62.9442",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 496.814,
            "range": "± 36.9528",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 611.186,
            "range": "± 52.4866",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 513.457,
            "range": "± 38.3485",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 534.422,
            "range": "± 62.4689",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 453.38,
            "range": "± 32.5864",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.4392,
            "range": "± 180.745",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.45286,
            "range": "± 191.978",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.7487,
            "range": "± 1.30728",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.18214,
            "range": "± 392.447",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.9869,
            "range": "± 4.4834",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.4097,
            "range": "± 1.58294",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1445,
            "range": "± 954.994",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.2798,
            "range": "± 607.684",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.5774,
            "range": "± 3.07268",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.0183,
            "range": "± 967.987",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3618,
            "range": "± 1.47979",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.4634,
            "range": "± 754.326",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.1083,
            "range": "± 1.76229",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.3701,
            "range": "± 1.2317",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.2941,
            "range": "± 1.02779",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.96682,
            "range": "± 812.292",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.789,
            "range": "± 1.73326",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.82843,
            "range": "± 945.573",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.5029,
            "range": "± 1.81498",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.69584,
            "range": "± 740.337",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.1571,
            "range": "± 9.78082",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.2239,
            "range": "± 7.23451",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.74203,
            "range": "± 90.6521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.79364,
            "range": "± 60.6892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 137.48,
            "range": "± 8.52592",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.7058,
            "range": "± 5.83721",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.09966,
            "range": "± 71.7828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.72678,
            "range": "± 52.2318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 42.4637,
            "range": "± 8.8699",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 7.97631,
            "range": "± 70.4279",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.2818,
            "range": "± 1.88948",
            "unit": "ns",
            "extra": "100 samples\n9289 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.4095,
            "range": "± 1.18125",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 532.354,
            "range": "± 19.7575",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.57424,
            "range": "± 0.385019",
            "unit": "ns",
            "extra": "100 samples\n6576 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.2673,
            "range": "± 10.0518",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9635,
            "range": "± 79.2536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.6205,
            "range": "± 4.87597",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 902.209,
            "range": "± 11.589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 190.763,
            "range": "± 272.224",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.30853,
            "range": "± 0.47113",
            "unit": "ns",
            "extra": "100 samples\n9267 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.62,
            "range": "± 2.40537",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.59948,
            "range": "± 29.8167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 892.903,
            "range": "± 81.289",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49856,
            "range": "± 232.477",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.88454,
            "range": "± 748.717",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.96431,
            "range": "± 393.086",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.69548,
            "range": "± 454.388",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.23299,
            "range": "± 281.932",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.10135,
            "range": "± 965.575",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.27446,
            "range": "± 248.935",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.22398,
            "range": "± 363.911",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.68115,
            "range": "± 303.286",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.6136,
            "range": "± 1.48377",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 186.867,
            "range": "± 7.11541",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.7337,
            "range": "± 2.42309",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 355.689,
            "range": "± 14.4836",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}