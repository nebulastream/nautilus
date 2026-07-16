window.BENCHMARK_DATA = {
  "lastUpdate": 1784201793963,
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
          "id": "97b9952cb7f81ef43a66b5299f05da12cbf27382",
          "message": "[fuzz] Fuzz the kernel signature space: param count, mixed types, narrow/void returns (#363)",
          "timestamp": "2026-07-06T21:14:23+02:00",
          "tree_id": "b2680fb9de21c3b4c1994671b3a5337bbd87686c",
          "url": "https://github.com/nebulastream/nautilus/commit/97b9952cb7f81ef43a66b5299f05da12cbf27382"
        },
        "date": 1783365663901,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 11.2759,
            "range": "± 1.40855",
            "unit": "ns",
            "extra": "100 samples\n7760 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.2509,
            "range": "± 1.67195",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 559.607,
            "range": "± 19.9632",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.63731,
            "range": "± 0.439053",
            "unit": "ns",
            "extra": "100 samples\n6493 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.2425,
            "range": "± 3.44172",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.3948,
            "range": "± 106.32",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.6335,
            "range": "± 2.46387",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 409.926,
            "range": "± 13.2722",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 91.3293,
            "range": "± 2.0749",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.1162,
            "range": "± 2.29311",
            "unit": "ns",
            "extra": "100 samples\n1131 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 142.095,
            "range": "± 6.12897",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5726,
            "range": "± 165.353",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.59739,
            "range": "± 0.510887",
            "unit": "ns",
            "extra": "100 samples\n8472 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.2396,
            "range": "± 1.06377",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.82951,
            "range": "± 200.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.0895,
            "range": "± 5.10662",
            "unit": "ns",
            "extra": "100 samples\n641 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 48.2327,
            "range": "± 9.57627",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 416.483,
            "range": "± 34.9873",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 408.271,
            "range": "± 14.9784",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 91.3204,
            "range": "± 1.59109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 90.9089,
            "range": "± 1.16883",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.7401,
            "range": "± 5.47117",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 45.8518,
            "range": "± 2.39606",
            "unit": "ns",
            "extra": "100 samples\n651 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.0382,
            "range": "± 7.32924",
            "unit": "ns",
            "extra": "100 samples\n726 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 407.602,
            "range": "± 10.2802",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 378.01,
            "range": "± 9.11835",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 438.128,
            "range": "± 11.908",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 92.2043,
            "range": "± 3.59826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 84.5146,
            "range": "± 2.12094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 87.9977,
            "range": "± 312.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.2658,
            "range": "± 4.20015",
            "unit": "ns",
            "extra": "100 samples\n730 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.0137,
            "range": "± 7.02361",
            "unit": "ns",
            "extra": "100 samples\n849 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 438.284,
            "range": "± 14.9742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 437.759,
            "range": "± 12.107",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 88.0459,
            "range": "± 785.904",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 87.9252,
            "range": "± 177.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.4815,
            "range": "± 5.74189",
            "unit": "ns",
            "extra": "100 samples\n736 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.1445,
            "range": "± 6.03635",
            "unit": "ns",
            "extra": "100 samples\n738 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.25769,
            "range": "± 170.748",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.39927,
            "range": "± 472.954",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.66857,
            "range": "± 1.22678",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.47209,
            "range": "± 482.155",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.5465,
            "range": "± 2.92594",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.7568,
            "range": "± 1.56238",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.45758,
            "range": "± 1.20916",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.56978,
            "range": "± 467.89",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.1655,
            "range": "± 2.91578",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.51586,
            "range": "± 1.10389",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.71057,
            "range": "± 1.15434",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.70123,
            "range": "± 537.02",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.8243,
            "range": "± 1.93655",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.98723,
            "range": "± 1.53289",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.2475,
            "range": "± 781.041",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.28363,
            "range": "± 703.284",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.8376,
            "range": "± 1.54828",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.9823,
            "range": "± 666.163",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.72669,
            "range": "± 1.08812",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.80965,
            "range": "± 377.94",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 37.1192,
            "range": "± 6.18458",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.8448,
            "range": "± 6.87807",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34852,
            "range": "± 47.1553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36222,
            "range": "± 51.0772",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.3709,
            "range": "± 12.4478",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.8515,
            "range": "± 8.18099",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43324,
            "range": "± 43.5915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.28495,
            "range": "± 34.4961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 177.983,
            "range": "± 10.3219",
            "unit": "ns",
            "extra": "100 samples\n168 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 444.37,
            "range": "± 29.5342",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.17216,
            "range": "± 117.037",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 481.698,
            "range": "± 29.2217",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 904.228,
            "range": "± 138.396",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 498.36,
            "range": "± 53.8633",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 575.79,
            "range": "± 30.8174",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 468.51,
            "range": "± 25.3096",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 498.297,
            "range": "± 32.0136",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 439.808,
            "range": "± 29.5005",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.4471,
            "range": "± 4.18597",
            "unit": "ns",
            "extra": "100 samples\n769 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 284.905,
            "range": "± 8.40944",
            "unit": "ns",
            "extra": "100 samples\n8261 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09553,
            "range": "± 0.818484",
            "unit": "ns",
            "extra": "100 samples\n7753 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.7129,
            "range": "± 2.6939",
            "unit": "ns",
            "extra": "100 samples\n734 iterations"
          },
          {
            "name": "ir_add",
            "value": 754.619,
            "range": "± 60.4666",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.53894,
            "range": "± 111.829",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.28667,
            "range": "± 210.991",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.61974,
            "range": "± 172.156",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.90436,
            "range": "± 297.795",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.87165,
            "range": "± 197.962",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.96636,
            "range": "± 132.295",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.4155,
            "range": "± 105.857",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74722,
            "range": "± 168.834",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.50042,
            "range": "± 115.719",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.82905,
            "range": "± 816.495",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.9668,
            "range": "± 7.20134",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8625,
            "range": "± 1.79949",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 165.466,
            "range": "± 6.95695",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.2739,
            "range": "± 101.342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.19769,
            "range": "± 98.9175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.21784,
            "range": "± 95.7889",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.1064,
            "range": "± 702.821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 73.8916,
            "range": "± 14.9506",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.35121,
            "range": "± 148.752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.28508,
            "range": "± 133.533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.24673,
            "range": "± 142.903",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.7501,
            "range": "± 502.367",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 132.007,
            "range": "± 16.7983",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.48884,
            "range": "± 161.784",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.16812,
            "range": "± 216.189",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.96773,
            "range": "± 128.751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.04281,
            "range": "± 208.614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.7715,
            "range": "± 466.549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6524,
            "range": "± 256.19",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.7852,
            "range": "± 474.639",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.92541,
            "range": "± 145.217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.4015,
            "range": "± 292.304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.48427,
            "range": "± 363.018",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.4671,
            "range": "± 247.008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.9327,
            "range": "± 783.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.54458,
            "range": "± 311.295",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.5138,
            "range": "± 360.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.2393,
            "range": "± 926.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.5502,
            "range": "± 468.851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.6076,
            "range": "± 421.137",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.2667,
            "range": "± 690.748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.9056,
            "range": "± 540.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.5883,
            "range": "± 461.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.9446,
            "range": "± 535.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.9979,
            "range": "± 536.225",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.1759,
            "range": "± 527.299",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.3203,
            "range": "± 690.729",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.155,
            "range": "± 2.65533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.3161,
            "range": "± 1.19387",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.1995,
            "range": "± 489.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.9223,
            "range": "± 906.487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.5731,
            "range": "± 2.18853",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.2787,
            "range": "± 3.21717",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.7169,
            "range": "± 4.82458",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.9144,
            "range": "± 4.48064",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 24.0185,
            "range": "± 3.36363",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.2092,
            "range": "± 3.29887",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.0095,
            "range": "± 4.52759",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 19.0437,
            "range": "± 3.82958",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 21.3168,
            "range": "± 4.65125",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 20.6224,
            "range": "± 4.16261",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 48.9967,
            "range": "± 6.72648",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 288.018,
            "range": "± 15.0906",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 65.0114,
            "range": "± 9.98995",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 453.794,
            "range": "± 19.5705",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.80389,
            "range": "± 110.221",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.06101,
            "range": "± 352.648",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.37404,
            "range": "± 940.544",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.1296,
            "range": "± 307.336",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.15919,
            "range": "± 1.0311",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.44938,
            "range": "± 555.912",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.41846,
            "range": "± 491.251",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.5612,
            "range": "± 523.176",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.69401,
            "range": "± 614.657",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.71758,
            "range": "± 411.664",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.1828,
            "range": "± 2.76563",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 278.845,
            "range": "± 9.74565",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 33.3818,
            "range": "± 3.66952",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 397.546,
            "range": "± 14.0444",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.9221,
            "range": "± 4.53239",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.5642,
            "range": "± 4.7448",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.0454,
            "range": "± 8.06581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.7252,
            "range": "± 5.4715",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.2394,
            "range": "± 8.33821",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.0631,
            "range": "± 5.33126",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 44.195,
            "range": "± 9.18999",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.719,
            "range": "± 4.66999",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 33.9083,
            "range": "± 4.7894",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 32.8987,
            "range": "± 4.57784",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 77.1834,
            "range": "± 11.0222",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 490.342,
            "range": "± 27.6686",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 90.8034,
            "range": "± 8.87984",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 638.643,
            "range": "± 19.2683",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3331.01,
            "range": "± 124878",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.51239,
            "range": "± 193.785",
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
          "id": "eecaa44835bb29e3c016ecebb6e94de31b0120e8",
          "message": "[fuzzer] Expand runtime-call (invoke) coverage: arity, mixed types, void &amp; pointer signatures (#369)",
          "timestamp": "2026-07-07T14:12:19+02:00",
          "tree_id": "b6ce30d05b2adcfa98064b78266e922a64adcbf2",
          "url": "https://github.com/nebulastream/nautilus/commit/eecaa44835bb29e3c016ecebb6e94de31b0120e8"
        },
        "date": 1783426719681,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_twotier_addOne",
            "value": 3.07546,
            "range": "± 159.979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 2.92718,
            "range": "± 142.898",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 2.95901,
            "range": "± 140.355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 62.3133,
            "range": "± 96.1391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 48.5797,
            "range": "± 16.6105",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 4.72882,
            "range": "± 163.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 4.58134,
            "range": "± 181.729",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 4.71794,
            "range": "± 514.947",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.7461,
            "range": "± 23.4439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 90.0907,
            "range": "± 18.575",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.05545,
            "range": "± 177.02",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 5.48887,
            "range": "± 186.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.60364,
            "range": "± 153.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.38086,
            "range": "± 211.575",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 26.2705,
            "range": "± 1.01382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 10.6533,
            "range": "± 412.184",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.3995,
            "range": "± 804.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.59441,
            "range": "± 292.757",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 9.16329,
            "range": "± 275.187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 8.05039,
            "range": "± 211.15",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 9.02123,
            "range": "± 299.744",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 20.8622,
            "range": "± 484.796",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.4034,
            "range": "± 657.508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 48.8665,
            "range": "± 823.75",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 32.7081,
            "range": "± 37.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.7415,
            "range": "± 28.9059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 36.9617,
            "range": "± 39.9201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.867,
            "range": "± 13.2641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2509,
            "range": "± 9.59222",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.714,
            "range": "± 17.0378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.5163,
            "range": "± 20.0845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 33.8659,
            "range": "± 33.0161",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 37.2896,
            "range": "± 44.2261",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 34.5135,
            "range": "± 37.7087",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 34.3313,
            "range": "± 35.4175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.1192,
            "range": "± 31.9951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 41.1622,
            "range": "± 38.0385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.4107,
            "range": "± 43.7939",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.7789,
            "range": "± 4.09185",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.5481,
            "range": "± 3.59166",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 17.0531,
            "range": "± 3.69056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 13.7209,
            "range": "± 3.87353",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 16.2216,
            "range": "± 5.11706",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 14.0974,
            "range": "± 3.50996",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 15.1695,
            "range": "± 4.1768",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 13.3537,
            "range": "± 3.37314",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 14.0451,
            "range": "± 4.40304",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 13.403,
            "range": "± 3.80536",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.2732,
            "range": "± 8.02782",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 217.523,
            "range": "± 12.422",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 42.9397,
            "range": "± 8.59143",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 356.52,
            "range": "± 11.9488",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.34433,
            "range": "± 171.206",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 2.95478,
            "range": "± 265.515",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 6.91043,
            "range": "± 812.457",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 3.0684,
            "range": "± 305.417",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 5.26414,
            "range": "± 644.631",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 3.20884,
            "range": "± 336.428",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 3.84894,
            "range": "± 341.259",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 3.24325,
            "range": "± 282.328",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 3.34413,
            "range": "± 320.24",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 2.80397,
            "range": "± 385.399",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 18.9729,
            "range": "± 2.29077",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 213.795,
            "range": "± 5.49677",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 24.1971,
            "range": "± 2.49406",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 310.773,
            "range": "± 8.1206",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 14.5682,
            "range": "± 4.60042",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 19.1832,
            "range": "± 5.02211",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 27.0669,
            "range": "± 5.47646",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 20.7039,
            "range": "± 6.14769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 25.3826,
            "range": "± 5.79459",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 22.5398,
            "range": "± 7.0619",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 23.4147,
            "range": "± 5.5208",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 18.7332,
            "range": "± 4.37677",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 20.8663,
            "range": "± 4.93647",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 20.4039,
            "range": "± 5.11772",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 53.6377,
            "range": "± 11.5477",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 390.691,
            "range": "± 16.3787",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 68.302,
            "range": "± 10.9395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 521.048,
            "range": "± 17.2256",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.1708,
            "range": "± 0.48161",
            "unit": "ns",
            "extra": "100 samples\n8840 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 12.9587,
            "range": "± 1.17272",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 529.693,
            "range": "± 62.3278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.67945,
            "range": "± 0.990312",
            "unit": "ns",
            "extra": "100 samples\n7220 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 43.7376,
            "range": "± 5.50623",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 9.68636,
            "range": "± 214.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 35.0853,
            "range": "± 2.4767",
            "unit": "ns",
            "extra": "100 samples\n696 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 294.62,
            "range": "± 9.11466",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 66.9989,
            "range": "± 580.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 23.3478,
            "range": "± 1.53479",
            "unit": "ns",
            "extra": "100 samples\n1039 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 117.846,
            "range": "± 3.79795",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 31.0657,
            "range": "± 78.4237",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.74494,
            "range": "± 0.174701",
            "unit": "ns",
            "extra": "100 samples\n8845 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.4026,
            "range": "± 564.502",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.64682,
            "range": "± 148.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 35.4226,
            "range": "± 3.38471",
            "unit": "ns",
            "extra": "100 samples\n698 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 35.2662,
            "range": "± 3.63658",
            "unit": "ns",
            "extra": "100 samples\n693 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 309.638,
            "range": "± 12.2",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 294.505,
            "range": "± 7.90377",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 67.6604,
            "range": "± 829.093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 66.8832,
            "range": "± 889.802",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 35.5807,
            "range": "± 3.78718",
            "unit": "ns",
            "extra": "100 samples\n693 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 32.429,
            "range": "± 4.31599",
            "unit": "ns",
            "extra": "100 samples\n766 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 31.3747,
            "range": "± 3.62768",
            "unit": "ns",
            "extra": "100 samples\n804 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 295.569,
            "range": "± 11.5371",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 247.364,
            "range": "± 6.29146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 365.066,
            "range": "± 9.58589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 67.0953,
            "range": "± 2.75557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 54.4161,
            "range": "± 1.25287",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 73.9584,
            "range": "± 1.34915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 30.6003,
            "range": "± 0.590615",
            "unit": "ns",
            "extra": "100 samples\n787 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 28.5011,
            "range": "± 2.52289",
            "unit": "ns",
            "extra": "100 samples\n883 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 365.66,
            "range": "± 9.19957",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 365.583,
            "range": "± 8.7469",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 73.7078,
            "range": "± 322.844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 73.8253,
            "range": "± 446.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 31.0424,
            "range": "± 3.96506",
            "unit": "ns",
            "extra": "100 samples\n807 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 31.2092,
            "range": "± 3.38769",
            "unit": "ns",
            "extra": "100 samples\n793 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 29.1201,
            "range": "± 2.82444",
            "unit": "ns",
            "extra": "100 samples\n836 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 253.147,
            "range": "± 11.1972",
            "unit": "ns",
            "extra": "100 samples\n8724 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.28287,
            "range": "± 0.299282",
            "unit": "ns",
            "extra": "100 samples\n7824 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 31.4645,
            "range": "± 1.51517",
            "unit": "ns",
            "extra": "100 samples\n767 iterations"
          },
          {
            "name": "ssa_add",
            "value": 160.397,
            "range": "± 52.6231",
            "unit": "ns",
            "extra": "100 samples\n185 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 311.033,
            "range": "± 17.1562",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.807764,
            "range": "± 0.0513172",
            "unit": "us",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 331.081,
            "range": "± 21.1059",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 619.753,
            "range": "± 36.3428",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 336.993,
            "range": "± 25.2556",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 394.062,
            "range": "± 23.0551",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 295.257,
            "range": "± 12.9559",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 346.426,
            "range": "± 29.5977",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 318.555,
            "range": "± 37.606",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.73696,
            "range": "± 175.91",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.74807,
            "range": "± 180.367",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 6.57341,
            "range": "± 1.2158",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.5431,
            "range": "± 577.72",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 19.9006,
            "range": "± 3.46243",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 9.70963,
            "range": "± 1.53608",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 6.48195,
            "range": "± 1.23972",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.58056,
            "range": "± 516.842",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 12.5385,
            "range": "± 3.45135",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 6.54667,
            "range": "± 1.07428",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 6.59815,
            "range": "± 1.60287",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.68487,
            "range": "± 595.106",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 10.5896,
            "range": "± 2.15443",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 7.87371,
            "range": "± 1.32797",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 5.88058,
            "range": "± 836.591",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 5.76859,
            "range": "± 678.726",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 7.71286,
            "range": "± 1.48448",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.67836,
            "range": "± 679.023",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 5.90817,
            "range": "± 952.683",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 2.95436,
            "range": "± 359.958",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 30.5515,
            "range": "± 7.36875",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 30.7891,
            "range": "± 6.87481",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.14032,
            "range": "± 29.6403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.15317,
            "range": "± 30.4311",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 77.3625,
            "range": "± 9.76574",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 38.8686,
            "range": "± 8.65935",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.58764,
            "range": "± 40.6923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 1.76833,
            "range": "± 29.8683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 554.924,
            "range": "± 32.2377",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.1657,
            "range": "± 59.0441",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 2.68613,
            "range": "± 512.853",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.20157,
            "range": "± 53.3524",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.15992,
            "range": "± 304.955",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.34728,
            "range": "± 77.7305",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.48207,
            "range": "± 94.8858",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.08965,
            "range": "± 66.4103",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.28829,
            "range": "± 80.9054",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.09469,
            "range": "± 61.3235",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 6.01934,
            "range": "± 531.268",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 71.0477,
            "range": "± 3.34402",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 9.11657,
            "range": "± 558.113",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 130.022,
            "range": "± 6.27502",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3011.18,
            "range": "± 112318",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 4.98096,
            "range": "± 163.444",
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
          "id": "e38089cb04012ad831b131c8219576040835970a",
          "message": "[IR passes] P2: LocalCSE, LICM, shared LoopInfo, StrengthReduction measurement (#367)",
          "timestamp": "2026-07-08T09:06:57+02:00",
          "tree_id": "1e9b310467d665892158222d1d831d9e3447108d",
          "url": "https://github.com/nebulastream/nautilus/commit/e38089cb04012ad831b131c8219576040835970a"
        },
        "date": 1783494908921,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 683.102,
            "range": "± 33.3483",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.43064,
            "range": "± 87.538",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.08149,
            "range": "± 177.631",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.53822,
            "range": "± 179.554",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.60688,
            "range": "± 291.117",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.63203,
            "range": "± 106.721",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.80219,
            "range": "± 209.394",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.33098,
            "range": "± 81.1992",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.59297,
            "range": "± 86.9427",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.36407,
            "range": "± 144.057",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.03354,
            "range": "± 574.421",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 96.2045,
            "range": "± 6.90986",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.2682,
            "range": "± 969.817",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 163.521,
            "range": "± 6.86881",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.17061,
            "range": "± 329.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.97492,
            "range": "± 302.932",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.00056,
            "range": "± 776.713",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.36717,
            "range": "± 247.829",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.4201,
            "range": "± 1.08775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5394,
            "range": "± 706.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.2615,
            "range": "± 320.057",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 6.44954,
            "range": "± 268.298",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.846,
            "range": "± 175.251",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.5846,
            "range": "± 167.955",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.727,
            "range": "± 154.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.0511,
            "range": "± 169.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9693,
            "range": "± 192.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.3813,
            "range": "± 1.95602",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.7698,
            "range": "± 116.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.60208,
            "range": "± 124.587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.63601,
            "range": "± 126.275",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 28.935,
            "range": "± 312.207",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 59.9626,
            "range": "± 17.9341",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.98645,
            "range": "± 177.236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.90433,
            "range": "± 208.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 6.23368,
            "range": "± 240.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 30.1883,
            "range": "± 237.196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 113.236,
            "range": "± 21.4426",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.5493,
            "range": "± 0.814411",
            "unit": "ns",
            "extra": "100 samples\n8793 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.5751,
            "range": "± 1.58102",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 211.198,
            "range": "± 13.7923",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.58044,
            "range": "± 0.379155",
            "unit": "ns",
            "extra": "100 samples\n7208 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 54.8113,
            "range": "± 4.22941",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 12.1521,
            "range": "± 50.7511",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.4892,
            "range": "± 7.76477",
            "unit": "ns",
            "extra": "100 samples\n672 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 410.148,
            "range": "± 17.7965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 107.092,
            "range": "± 5.85326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 30.148,
            "range": "± 1.68006",
            "unit": "ns",
            "extra": "100 samples\n1036 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 150.93,
            "range": "± 3.87317",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 40.1971,
            "range": "± 95.7625",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.58852,
            "range": "± 0.408488",
            "unit": "ns",
            "extra": "100 samples\n8802 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.7401,
            "range": "± 2.35117",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.37268,
            "range": "± 78.921",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 46.7472,
            "range": "± 4.037",
            "unit": "ns",
            "extra": "100 samples\n666 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 47.7155,
            "range": "± 10.5363",
            "unit": "ns",
            "extra": "100 samples\n663 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 408.46,
            "range": "± 17.7047",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 455.174,
            "range": "± 16.2462",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 107.007,
            "range": "± 5.79717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 90.6709,
            "range": "± 4.61115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 30.0968,
            "range": "± 1.63638",
            "unit": "ns",
            "extra": "100 samples\n1037 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 30.4554,
            "range": "± 2.85228",
            "unit": "ns",
            "extra": "100 samples\n1042 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 151.685,
            "range": "± 4.78539",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 151.382,
            "range": "± 4.65195",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 40.4182,
            "range": "± 423.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 38.6789,
            "range": "± 1.28053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.55222,
            "range": "± 0.266442",
            "unit": "ns",
            "extra": "100 samples\n8575 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.52931,
            "range": "± 0.176684",
            "unit": "ns",
            "extra": "100 samples\n8700 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 21.0522,
            "range": "± 2.31621",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 21.434,
            "range": "± 3.814",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 3.39638,
            "range": "± 124.72",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 3.40678,
            "range": "± 156.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.2608,
            "range": "± 6.1579",
            "unit": "ns",
            "extra": "100 samples\n671 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.8112,
            "range": "± 5.08946",
            "unit": "ns",
            "extra": "100 samples\n647 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 425.544,
            "range": "± 22.0792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 411.579,
            "range": "± 23.0487",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 102.639,
            "range": "± 4.14571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 106.441,
            "range": "± 5.89061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.1134,
            "range": "± 6.19526",
            "unit": "ns",
            "extra": "100 samples\n670 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 40.7668,
            "range": "± 2.21217",
            "unit": "ns",
            "extra": "100 samples\n751 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 40.0711,
            "range": "± 5.64199",
            "unit": "ns",
            "extra": "100 samples\n813 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 406.196,
            "range": "± 12.5383",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 323.342,
            "range": "± 11.307",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 471.08,
            "range": "± 11.3156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 106.18,
            "range": "± 5.96025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 70.0877,
            "range": "± 2.06985",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 94.9558,
            "range": "± 929.723",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 38.9259,
            "range": "± 5.32693",
            "unit": "ns",
            "extra": "100 samples\n815 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.5295,
            "range": "± 4.49108",
            "unit": "ns",
            "extra": "100 samples\n881 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 472.558,
            "range": "± 12.2728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 471.641,
            "range": "± 11.7392",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 94.7005,
            "range": "± 556.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 95.6433,
            "range": "± 1.58203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 38.9818,
            "range": "± 4.14329",
            "unit": "ns",
            "extra": "100 samples\n821 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 38.7054,
            "range": "± 3.34286",
            "unit": "ns",
            "extra": "100 samples\n821 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.31589,
            "range": "± 216.087",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.37876,
            "range": "± 267.051",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.60339,
            "range": "± 1.45993",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.61997,
            "range": "± 544.975",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.9156,
            "range": "± 3.60144",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.9338,
            "range": "± 1.71625",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.4821,
            "range": "± 1.4129",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.68666,
            "range": "± 528.652",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.531,
            "range": "± 3.11174",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.53264,
            "range": "± 1.15411",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.6227,
            "range": "± 1.59072",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.78765,
            "range": "± 586.509",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.0153,
            "range": "± 2.46631",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2518,
            "range": "± 1.34009",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.36381,
            "range": "± 821.129",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.35218,
            "range": "± 765.22",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.91314,
            "range": "± 1.94249",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.05795,
            "range": "± 717",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.71733,
            "range": "± 1.10564",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.91561,
            "range": "± 395.879",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.7879,
            "range": "± 7.6776",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.2556,
            "range": "± 7.63045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.46633,
            "range": "± 28.5724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.48982,
            "range": "± 34.4424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.7851,
            "range": "± 14.2104",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.5576,
            "range": "± 8.95532",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.85219,
            "range": "± 79.3386",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26483,
            "range": "± 47.4855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3713.12,
            "range": "± 101090",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.13781,
            "range": "± 144.34",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.3934,
            "range": "± 2.40823",
            "unit": "ns",
            "extra": "100 samples\n828 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 301.766,
            "range": "± 6.74816",
            "unit": "ns",
            "extra": "100 samples\n8570 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.14721,
            "range": "± 0.62587",
            "unit": "ns",
            "extra": "100 samples\n7930 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.7553,
            "range": "± 1.78536",
            "unit": "ns",
            "extra": "100 samples\n801 iterations"
          },
          {
            "name": "ssa_add",
            "value": 167.893,
            "range": "± 7.65581",
            "unit": "ns",
            "extra": "100 samples\n184 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 414.449,
            "range": "± 22.5528",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.05413,
            "range": "± 57.9664",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 432.529,
            "range": "± 25.1763",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 830.367,
            "range": "± 53.9349",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 436.86,
            "range": "± 30.1106",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 751.355,
            "range": "± 175.645",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 382.088,
            "range": "± 21.5051",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 469.182,
            "range": "± 116.287",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 410.41,
            "range": "± 41.9175",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
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
          "id": "02eb41373e2b20223e6ada18cc0a9c0b7105200a",
          "message": "[fuzzer] Add val&lt;bool&gt; and val&lt;enum&gt; as first-class generated domains (#368)",
          "timestamp": "2026-07-08T15:59:59+02:00",
          "tree_id": "ee34d8ca744aab3e64784c3feb83929ec9d9347a",
          "url": "https://github.com/nebulastream/nautilus/commit/02eb41373e2b20223e6ada18cc0a9c0b7105200a"
        },
        "date": 1783519942831,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 5.76581,
            "range": "± 279.776",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.24871,
            "range": "± 214.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.20192,
            "range": "± 565.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.16201,
            "range": "± 189.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.3234,
            "range": "± 1.30347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.9206,
            "range": "± 285.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.2486,
            "range": "± 603.607",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.04784,
            "range": "± 151.795",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.9129,
            "range": "± 379.319",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0041,
            "range": "± 486.38",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.7036,
            "range": "± 261.379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.6551,
            "range": "± 945.327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.4258,
            "range": "± 450.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2928,
            "range": "± 1.07385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.2156,
            "range": "± 488.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.3559,
            "range": "± 993.675",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.5667,
            "range": "± 786.581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.7376,
            "range": "± 554.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.6073,
            "range": "± 954.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.9173,
            "range": "± 657.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.4106,
            "range": "± 468.678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.683,
            "range": "± 564.447",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.4781,
            "range": "± 690.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.8053,
            "range": "± 302.104",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.2557,
            "range": "± 463.087",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.6859,
            "range": "± 691.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.8178,
            "range": "± 426.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.3235,
            "range": "± 1.84358",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.2706,
            "range": "± 3.18243",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 21.514,
            "range": "± 4.47482",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 24.9158,
            "range": "± 2.83419",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.8076,
            "range": "± 4.40029",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 24.4426,
            "range": "± 4.3913",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.4329,
            "range": "± 3.47233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 22.9933,
            "range": "± 3.64389",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.6817,
            "range": "± 3.71872",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 21.0025,
            "range": "± 3.95243",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 47.0892,
            "range": "± 28.1948",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 49.6753,
            "range": "± 8.61441",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 292.071,
            "range": "± 12.1335",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 64.933,
            "range": "± 8.94968",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 461.226,
            "range": "± 16.1651",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.85629,
            "range": "± 127.888",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.22188,
            "range": "± 404.308",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.78543,
            "range": "± 928.002",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.27732,
            "range": "± 435.191",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.39308,
            "range": "± 789.742",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.54789,
            "range": "± 667.734",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.5887,
            "range": "± 541.31",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.53279,
            "range": "± 562.538",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.64038,
            "range": "± 418.717",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.82065,
            "range": "± 381.623",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 27.6415,
            "range": "± 3.1623",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 290.319,
            "range": "± 15.9052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 35.1358,
            "range": "± 3.24954",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 425.162,
            "range": "± 17.6091",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.9625,
            "range": "± 4.26212",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.4069,
            "range": "± 4.94851",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 49.2606,
            "range": "± 8.79497",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.6592,
            "range": "± 5.06439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.0406,
            "range": "± 7.37702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.329,
            "range": "± 5.54642",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 45.6203,
            "range": "± 9.04067",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.4345,
            "range": "± 4.79976",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.5301,
            "range": "± 7.25774",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.4328,
            "range": "± 4.94372",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 76.5232,
            "range": "± 8.98187",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 499.832,
            "range": "± 18.6177",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 94.9682,
            "range": "± 11.062",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 654.98,
            "range": "± 18.7826",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 174.932,
            "range": "± 10.1436",
            "unit": "ns",
            "extra": "100 samples\n167 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 444.775,
            "range": "± 32.4644",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.12775,
            "range": "± 90.3305",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 479.774,
            "range": "± 40.2977",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 891.959,
            "range": "± 75.7605",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 477.557,
            "range": "± 31.0427",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 573.998,
            "range": "± 53.7071",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 406.486,
            "range": "± 50.0255",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 495.692,
            "range": "± 29.1336",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 438.724,
            "range": "± 23.4568",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.32382,
            "range": "± 188.446",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.27596,
            "range": "± 207.959",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.75272,
            "range": "± 1.15807",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.61366,
            "range": "± 563.924",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.9092,
            "range": "± 4.78596",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.7338,
            "range": "± 1.51943",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.65851,
            "range": "± 1.31363",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.82582,
            "range": "± 533.508",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.0758,
            "range": "± 3.19137",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.54675,
            "range": "± 1.0891",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.75441,
            "range": "± 1.24665",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.74548,
            "range": "± 665.592",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.9512,
            "range": "± 2.14828",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.1537,
            "range": "± 1.45661",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.51308,
            "range": "± 1.03249",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.72226,
            "range": "± 1.56124",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.0007,
            "range": "± 1.78885",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.97189,
            "range": "± 710.851",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.9838,
            "range": "± 1.07925",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.94018,
            "range": "± 504.171",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.3748,
            "range": "± 7.63379",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.9665,
            "range": "± 6.96288",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.36316,
            "range": "± 39.6443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.37321,
            "range": "± 55.3503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.5629,
            "range": "± 9.86903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.2231,
            "range": "± 7.32547",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43758,
            "range": "± 35.6749",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.36221,
            "range": "± 272.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.0981,
            "range": "± 1.07091",
            "unit": "ns",
            "extra": "100 samples\n8430 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.4126,
            "range": "± 915.076",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 547.78,
            "range": "± 21.8496",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.70121,
            "range": "± 0.57892",
            "unit": "ns",
            "extra": "100 samples\n6481 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.4753,
            "range": "± 4.77521",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.3859,
            "range": "± 64.6371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 56.1488,
            "range": "± 6.63411",
            "unit": "ns",
            "extra": "100 samples\n534 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 430.413,
            "range": "± 10.0712",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.5366,
            "range": "± 1.01852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.1041,
            "range": "± 3.74532",
            "unit": "ns",
            "extra": "100 samples\n1140 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 130.857,
            "range": "± 5.11342",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.3309,
            "range": "± 164.751",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.61778,
            "range": "± 0.647381",
            "unit": "ns",
            "extra": "100 samples\n8448 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.5206,
            "range": "± 2.29698",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.8038,
            "range": "± 25.4641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 57.0744,
            "range": "± 12.8998",
            "unit": "ns",
            "extra": "100 samples\n547 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 55.0999,
            "range": "± 3.43044",
            "unit": "ns",
            "extra": "100 samples\n550 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 435.793,
            "range": "± 18.6172",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 389.875,
            "range": "± 12.8201",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 95.5629,
            "range": "± 665.731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.8357,
            "range": "± 1.00345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 26.895,
            "range": "± 3.50812",
            "unit": "ns",
            "extra": "100 samples\n1090 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.1764,
            "range": "± 3.4232",
            "unit": "ns",
            "extra": "100 samples\n1144 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 131.084,
            "range": "± 5.44217",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 143.861,
            "range": "± 7.915",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.3356,
            "range": "± 69.255",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 34.0493,
            "range": "± 160.906",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.53681,
            "range": "± 0.498402",
            "unit": "ns",
            "extra": "100 samples\n8461 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.60012,
            "range": "± 0.572339",
            "unit": "ns",
            "extra": "100 samples\n8468 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.6611,
            "range": "± 3.05523",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.253,
            "range": "± 1.29975",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.81632,
            "range": "± 25.8198",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.81745,
            "range": "± 27.3445",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 55.8496,
            "range": "± 5.19845",
            "unit": "ns",
            "extra": "100 samples\n549 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 56.5699,
            "range": "± 9.81629",
            "unit": "ns",
            "extra": "100 samples\n555 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 431.628,
            "range": "± 11.9822",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 433.481,
            "range": "± 13.3929",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.4901,
            "range": "± 235.206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.3212,
            "range": "± 860.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 56.5687,
            "range": "± 7.57952",
            "unit": "ns",
            "extra": "100 samples\n549 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.1155,
            "range": "± 3.11556",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.1022,
            "range": "± 3.61387",
            "unit": "ns",
            "extra": "100 samples\n780 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 430.588,
            "range": "± 10.0701",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 332.826,
            "range": "± 12.6893",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 435.788,
            "range": "± 10.3839",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 95.4209,
            "range": "± 192.514",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 72.0532,
            "range": "± 2.4239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.3776,
            "range": "± 773.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.2529,
            "range": "± 5.63364",
            "unit": "ns",
            "extra": "100 samples\n779 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.4457,
            "range": "± 7.61996",
            "unit": "ns",
            "extra": "100 samples\n845 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 435.38,
            "range": "± 10.2792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 436.245,
            "range": "± 12.146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 86.5529,
            "range": "± 917.033",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.4198,
            "range": "± 944.141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.8508,
            "range": "± 6.16102",
            "unit": "ns",
            "extra": "100 samples\n778 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.7737,
            "range": "± 5.33035",
            "unit": "ns",
            "extra": "100 samples\n747 iterations"
          },
          {
            "name": "ir_add",
            "value": 760.345,
            "range": "± 51.1436",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.57535,
            "range": "± 136.503",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.41398,
            "range": "± 494.011",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.64229,
            "range": "± 143.873",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.79397,
            "range": "± 172.295",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.80457,
            "range": "± 126.202",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.99755,
            "range": "± 178.42",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.46138,
            "range": "± 128.207",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.81239,
            "range": "± 241.035",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.49241,
            "range": "± 120.752",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.6586,
            "range": "± 517.657",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.9206,
            "range": "± 5.39353",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8267,
            "range": "± 1.15146",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 170.496,
            "range": "± 10.8839",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 42.7195,
            "range": "± 6.39063",
            "unit": "ns",
            "extra": "100 samples\n720 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 289.365,
            "range": "± 10.7064",
            "unit": "ns",
            "extra": "100 samples\n8244 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.97014,
            "range": "± 0.46248",
            "unit": "ns",
            "extra": "100 samples\n7723 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.6214,
            "range": "± 1.97283",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3376.12,
            "range": "± 108022",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.61772,
            "range": "± 168.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.35537,
            "range": "± 102.525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.27882,
            "range": "± 110.725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.29792,
            "range": "± 116.288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.9854,
            "range": "± 1.03235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 74.586,
            "range": "± 15.4037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.445,
            "range": "± 110.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.41008,
            "range": "± 213.682",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.54104,
            "range": "± 254.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.025,
            "range": "± 664.115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 133.993,
            "range": "± 18.0583",
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
          "id": "e4d9a8ef1297b1fc6baa5a263152684d5c2eb5ca",
          "message": "[fuzzer] Allow compounding pointer arithmetic in the memory/pointer domain (#374)",
          "timestamp": "2026-07-08T17:46:39+02:00",
          "tree_id": "f7e14393ba452f8f734f9d955c905297021de3c6",
          "url": "https://github.com/nebulastream/nautilus/commit/e4d9a8ef1297b1fc6baa5a263152684d5c2eb5ca"
        },
        "date": 1783526025954,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_twotier_addOne",
            "value": 3.32275,
            "range": "± 103.068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.2289,
            "range": "± 118.194",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.20464,
            "range": "± 79.8992",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.2781,
            "range": "± 407.13",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 74.9261,
            "range": "± 9.1545",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.35465,
            "range": "± 84.3128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.29269,
            "range": "± 98.867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.29393,
            "range": "± 108.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.0235,
            "range": "± 300.063",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 134.386,
            "range": "± 16.9359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3309.16,
            "range": "± 89539.7",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.52388,
            "range": "± 135.815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 41.4228,
            "range": "± 6.98391",
            "unit": "ns",
            "extra": "100 samples\n741 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 282.015,
            "range": "± 6.41745",
            "unit": "ns",
            "extra": "100 samples\n8292 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.89234,
            "range": "± 0.317178",
            "unit": "ns",
            "extra": "100 samples\n7738 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.6063,
            "range": "± 2.06665",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.3542,
            "range": "± 1.0329",
            "unit": "ns",
            "extra": "100 samples\n8449 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.2129,
            "range": "± 1.51242",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 549.938,
            "range": "± 30.8945",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.77592,
            "range": "± 0.834847",
            "unit": "ns",
            "extra": "100 samples\n6530 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.5556,
            "range": "± 5.29832",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.3907,
            "range": "± 132.882",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 54.0004,
            "range": "± 4.18245",
            "unit": "ns",
            "extra": "100 samples\n551 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 430.819,
            "range": "± 11.1825",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.7942,
            "range": "± 2.66253",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 26.9334,
            "range": "± 4.11144",
            "unit": "ns",
            "extra": "100 samples\n1149 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 151.986,
            "range": "± 24.6833",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.2045,
            "range": "± 53.2399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.55594,
            "range": "± 0.484749",
            "unit": "ns",
            "extra": "100 samples\n8443 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.5786,
            "range": "± 2.08535",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.81612,
            "range": "± 65.1391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 53.9041,
            "range": "± 3.63996",
            "unit": "ns",
            "extra": "100 samples\n550 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 55.049,
            "range": "± 6.47556",
            "unit": "ns",
            "extra": "100 samples\n546 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 431.569,
            "range": "± 10.8543",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 389.152,
            "range": "± 13.1087",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 95.2765,
            "range": "± 1.67099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.6349,
            "range": "± 379.639",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 27.1869,
            "range": "± 4.7401",
            "unit": "ns",
            "extra": "100 samples\n1116 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.0026,
            "range": "± 3.17",
            "unit": "ns",
            "extra": "100 samples\n1136 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 152.181,
            "range": "± 23.9281",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 152.836,
            "range": "± 22.7324",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.2123,
            "range": "± 103.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9479,
            "range": "± 110.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.57809,
            "range": "± 0.415911",
            "unit": "ns",
            "extra": "100 samples\n8466 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.54974,
            "range": "± 0.601327",
            "unit": "ns",
            "extra": "100 samples\n8447 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.404,
            "range": "± 1.83148",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.4345,
            "range": "± 1.57695",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.80241,
            "range": "± 21.83",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.8015,
            "range": "± 27.0265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 55.4783,
            "range": "± 6.12934",
            "unit": "ns",
            "extra": "100 samples\n551 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 55.6521,
            "range": "± 8.05084",
            "unit": "ns",
            "extra": "100 samples\n555 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 431.332,
            "range": "± 14.658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 431.465,
            "range": "± 11.4073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.3291,
            "range": "± 1.50249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.287,
            "range": "± 906.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 55.2983,
            "range": "± 5.83476",
            "unit": "ns",
            "extra": "100 samples\n549 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.0292,
            "range": "± 3.54545",
            "unit": "ns",
            "extra": "100 samples\n715 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.6543,
            "range": "± 5.85023",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 431.402,
            "range": "± 11.7127",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 335.806,
            "range": "± 16.507",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 434.229,
            "range": "± 9.55329",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 95.6747,
            "range": "± 2.89182",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 68.7964,
            "range": "± 2.95924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.6742,
            "range": "± 3.481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.8272,
            "range": "± 7.60521",
            "unit": "ns",
            "extra": "100 samples\n782 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.1557,
            "range": "± 5.78039",
            "unit": "ns",
            "extra": "100 samples\n802 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 437.728,
            "range": "± 21.656",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 435.713,
            "range": "± 10.9912",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 86.4822,
            "range": "± 1.37314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.3863,
            "range": "± 442.166",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.3407,
            "range": "± 6.84495",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.7193,
            "range": "± 8.76231",
            "unit": "ns",
            "extra": "100 samples\n751 iterations"
          },
          {
            "name": "ir_add",
            "value": 803.662,
            "range": "± 79.872",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.66128,
            "range": "± 208.451",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.44842,
            "range": "± 375.956",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.7236,
            "range": "± 166.823",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.88086,
            "range": "± 265.516",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.83978,
            "range": "± 173.213",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.06596,
            "range": "± 215.028",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.52228,
            "range": "± 179.355",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.81841,
            "range": "± 192.874",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53519,
            "range": "± 151.738",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.79768,
            "range": "± 905.951",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.5096,
            "range": "± 5.77691",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.7772,
            "range": "± 1.07734",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 167.355,
            "range": "± 8.61926",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.42096,
            "range": "± 241.102",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.46817,
            "range": "± 250.509",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.13624,
            "range": "± 1.45828",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.8174,
            "range": "± 551.494",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.7829,
            "range": "± 4.16481",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.5964,
            "range": "± 1.9717",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.0357,
            "range": "± 1.21452",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.99088,
            "range": "± 556.897",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.3807,
            "range": "± 2.87202",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.79336,
            "range": "± 1.29556",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.95885,
            "range": "± 1.39848",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.3791,
            "range": "± 920.561",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.3142,
            "range": "± 2.17895",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.6238,
            "range": "± 1.50235",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.63527,
            "range": "± 1.11374",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.53235,
            "range": "± 862.872",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.7024,
            "range": "± 2.09832",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.38963,
            "range": "± 726.057",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.19153,
            "range": "± 1.26994",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.22547,
            "range": "± 503.526",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.6236,
            "range": "± 4.72638",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.856,
            "range": "± 5.3526",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34968,
            "range": "± 61.4233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36423,
            "range": "± 62.527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.427,
            "range": "± 11.5214",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.7491,
            "range": "± 8.62486",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.40333,
            "range": "± 59.7288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.3459,
            "range": "± 44.9061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 181.918,
            "range": "± 17.6491",
            "unit": "ns",
            "extra": "100 samples\n169 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 467.177,
            "range": "± 57.4353",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.15356,
            "range": "± 99.0066",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 484.07,
            "range": "± 39.1856",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 907.495,
            "range": "± 95.7613",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 510.451,
            "range": "± 57.3771",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 599.644,
            "range": "± 67.9035",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 414.545,
            "range": "± 46.7714",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.353,
            "range": "± 52.0291",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 456.028,
            "range": "± 33.8141",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.54636,
            "range": "± 169.424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.08508,
            "range": "± 150.655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.01046,
            "range": "± 130.551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.10138,
            "range": "± 169.818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.9282,
            "range": "± 510.99",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.9816,
            "range": "± 432.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.9936,
            "range": "± 768.313",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.02861,
            "range": "± 175.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.7118,
            "range": "± 346.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.35852,
            "range": "± 216.94",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.6859,
            "range": "± 436.721",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.223,
            "range": "± 386.088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.45718,
            "range": "± 219.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.2051,
            "range": "± 544.886",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.2438,
            "range": "± 536.936",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.581,
            "range": "± 464.645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.8859,
            "range": "± 582.232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.9213,
            "range": "± 427.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.9075,
            "range": "± 458.239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.2236,
            "range": "± 464.852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.58,
            "range": "± 851.184",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.6853,
            "range": "± 413.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.1222,
            "range": "± 404.092",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.2599,
            "range": "± 539.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.7859,
            "range": "± 2.1398",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.7499,
            "range": "± 738.823",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.6916,
            "range": "± 353.174",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.8998,
            "range": "± 536.956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.2825,
            "range": "± 2.38106",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.6395,
            "range": "± 3.75986",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.5209,
            "range": "± 4.19805",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.8278,
            "range": "± 3.53451",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 24.4797,
            "range": "± 4.13261",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.1678,
            "range": "± 3.43534",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 22.4909,
            "range": "± 3.53516",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 19.161,
            "range": "± 3.75308",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 20.8893,
            "range": "± 3.55175",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 20.0253,
            "range": "± 3.50043",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 48.106,
            "range": "± 7.77151",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 274.429,
            "range": "± 13.2012",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 63.5953,
            "range": "± 9.3216",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 440.93,
            "range": "± 13.7386",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.83579,
            "range": "± 161.604",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.07315,
            "range": "± 342.762",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.53533,
            "range": "± 1.20331",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.23061,
            "range": "± 514.061",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.10171,
            "range": "± 777.377",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.38259,
            "range": "± 472.409",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.51154,
            "range": "± 711.696",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.50549,
            "range": "± 436.933",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.54528,
            "range": "± 448.015",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.63813,
            "range": "± 317.422",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.5477,
            "range": "± 2.76426",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 273.698,
            "range": "± 10.6413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 34.1487,
            "range": "± 3.74568",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 392.231,
            "range": "± 11.8913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.9396,
            "range": "± 4.69116",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 31.2513,
            "range": "± 4.70659",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.9785,
            "range": "± 8.23715",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 31.8086,
            "range": "± 5.01864",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.4876,
            "range": "± 8.77898",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 35.2361,
            "range": "± 5.01698",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 37.2939,
            "range": "± 5.69094",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.5978,
            "range": "± 4.65781",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 33.7373,
            "range": "± 5.07115",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 32.7671,
            "range": "± 5.94149",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 77.0142,
            "range": "± 10.5763",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 488.456,
            "range": "± 13.8191",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 93.9485,
            "range": "± 10.4837",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 636.837,
            "range": "± 20.8318",
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
          "id": "39ad15f535087b891ec692f3a9e446c954e129cc",
          "message": "Fix ternary \"double jump\" miscompilation (issue #95) (#375)",
          "timestamp": "2026-07-08T17:56:35+02:00",
          "tree_id": "6239e8e6f9e9fe489564fe3fe3b9697fb3eb4acf",
          "url": "https://github.com/nebulastream/nautilus/commit/39ad15f535087b891ec692f3a9e446c954e129cc"
        },
        "date": 1783526681955,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_twotier_addOne",
            "value": 3.29487,
            "range": "± 86.0124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.1809,
            "range": "± 101.847",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.19389,
            "range": "± 62.9481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.48767,
            "range": "± 137.531",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.04015,
            "range": "± 122.231",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.98565,
            "range": "± 113.175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.97232,
            "range": "± 131.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.3637,
            "range": "± 847.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6005,
            "range": "± 134.414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.3697,
            "range": "± 534.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.90912,
            "range": "± 47.9232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.3306,
            "range": "± 411.278",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.29873,
            "range": "± 181.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.3035,
            "range": "± 130.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.6365,
            "range": "± 163.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.32572,
            "range": "± 73.8037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.0769,
            "range": "± 188.459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.3272,
            "range": "± 362.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.9236,
            "range": "± 256.616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.8976,
            "range": "± 352.681",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.9138,
            "range": "± 337.082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 25.9119,
            "range": "± 351.327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.3831,
            "range": "± 352.307",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.5159,
            "range": "± 356.96",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.5999,
            "range": "± 348.217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.0841,
            "range": "± 312.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.8925,
            "range": "± 344.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.0074,
            "range": "± 354.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.8455,
            "range": "± 567.506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.1626,
            "range": "± 293.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.3352,
            "range": "± 450.528",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.041,
            "range": "± 2.27079",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.7587,
            "range": "± 2.85187",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.0355,
            "range": "± 3.01068",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.2206,
            "range": "± 2.90023",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 24.1043,
            "range": "± 3.2941",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.8299,
            "range": "± 2.64639",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 22.7221,
            "range": "± 4.47782",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.4798,
            "range": "± 3.25009",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 20.4568,
            "range": "± 2.89813",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.7604,
            "range": "± 4.17182",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 49.6424,
            "range": "± 7.85835",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 285.026,
            "range": "± 9.93987",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 64.6774,
            "range": "± 10.0223",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 457.32,
            "range": "± 15.35",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.79739,
            "range": "± 128.218",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.10905,
            "range": "± 398.711",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.60221,
            "range": "± 1.22404",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.13171,
            "range": "± 359.973",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.17858,
            "range": "± 798.026",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.40536,
            "range": "± 337.137",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.37778,
            "range": "± 623.577",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.65426,
            "range": "± 503.883",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.58898,
            "range": "± 486.299",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.722,
            "range": "± 323.656",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.7044,
            "range": "± 2.48421",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 282.756,
            "range": "± 8.75201",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 34.6734,
            "range": "± 3.40476",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 407.168,
            "range": "± 12.1714",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.295,
            "range": "± 3.7062",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 31.4084,
            "range": "± 3.11175",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 47.6533,
            "range": "± 5.98436",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.6068,
            "range": "± 4.24024",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.3641,
            "range": "± 6.84303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.1241,
            "range": "± 4.2147",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 37.8178,
            "range": "± 4.98804",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.8136,
            "range": "± 3.55101",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 33.4719,
            "range": "± 3.44013",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 32.9072,
            "range": "± 3.80244",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 75.8466,
            "range": "± 8.07242",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 494.838,
            "range": "± 12.3445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 93.2325,
            "range": "± 9.72732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 644.888,
            "range": "± 13.9025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.6776,
            "range": "± 3.95756",
            "unit": "ns",
            "extra": "100 samples\n741 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 282.509,
            "range": "± 5.08241",
            "unit": "ns",
            "extra": "100 samples\n8277 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.87069,
            "range": "± 0.212737",
            "unit": "ns",
            "extra": "100 samples\n7748 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.663,
            "range": "± 2.46412",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3262.62,
            "range": "± 65393.19999999999",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.44199,
            "range": "± 82.1711",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.29973,
            "range": "± 198.928",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.3011,
            "range": "± 139.184",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.88282,
            "range": "± 829.804",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.63884,
            "range": "± 483.187",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.9191,
            "range": "± 2.0784",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.7498,
            "range": "± 1.63772",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.66164,
            "range": "± 954.591",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.68231,
            "range": "± 428.482",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.9667,
            "range": "± 2.02029",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.4552,
            "range": "± 729.228",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.82007,
            "range": "± 816.87",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.89143,
            "range": "± 583.339",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.1701,
            "range": "± 1.17624",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.5086,
            "range": "± 950.359",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.26175,
            "range": "± 412.362",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.6083,
            "range": "± 826.238",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.155,
            "range": "± 908.993",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.01284,
            "range": "± 492.163",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.97946,
            "range": "± 775.211",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.124,
            "range": "± 419.63",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.2829,
            "range": "± 5.6954",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.9044,
            "range": "± 4.11511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.383,
            "range": "± 16.4532",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39782,
            "range": "± 31.2237",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.9364,
            "range": "± 5.56451",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.1252,
            "range": "± 6.35718",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.44384,
            "range": "± 35.0718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.29603,
            "range": "± 42.5688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 789.878,
            "range": "± 83.5604",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.60223,
            "range": "± 219.343",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.49713,
            "range": "± 444.196",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.65989,
            "range": "± 121.821",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.94024,
            "range": "± 384.211",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.84729,
            "range": "± 243.753",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.99427,
            "range": "± 132.243",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.47864,
            "range": "± 138.853",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.77397,
            "range": "± 165.732",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.51201,
            "range": "± 84.2388",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.92634,
            "range": "± 777.615",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.9107,
            "range": "± 5.25661",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.1222,
            "range": "± 1.5622",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 168.77,
            "range": "± 8.31767",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.099,
            "range": "± 1.16902",
            "unit": "ns",
            "extra": "100 samples\n8444 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.8513,
            "range": "± 1.07148",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 529.84,
            "range": "± 19.4594",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.71151,
            "range": "± 0.667504",
            "unit": "ns",
            "extra": "100 samples\n6521 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.5012,
            "range": "± 5.43686",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.4266,
            "range": "± 412.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 53.9818,
            "range": "± 6.75889",
            "unit": "ns",
            "extra": "100 samples\n544 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 433.718,
            "range": "± 15.9239",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.0741,
            "range": "± 1.50007",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.2014,
            "range": "± 3.09019",
            "unit": "ns",
            "extra": "100 samples\n1144 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 142.61,
            "range": "± 7.57024",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.2217,
            "range": "± 183.186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.45087,
            "range": "± 0.199234",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.3913,
            "range": "± 1.54834",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80295,
            "range": "± 40.8276",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 54.9398,
            "range": "± 8.69188",
            "unit": "ns",
            "extra": "100 samples\n536 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 53.8159,
            "range": "± 8.26996",
            "unit": "ns",
            "extra": "100 samples\n543 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 434.908,
            "range": "± 24.1534",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 387.059,
            "range": "± 8.97521",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 94.9417,
            "range": "± 1.67579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.6205,
            "range": "± 384.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 26.6086,
            "range": "± 1.84427",
            "unit": "ns",
            "extra": "100 samples\n1140 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.1074,
            "range": "± 3.38631",
            "unit": "ns",
            "extra": "100 samples\n1147 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 140.632,
            "range": "± 6.18489",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 141.071,
            "range": "± 6.33765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.2009,
            "range": "± 144.033",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9749,
            "range": "± 101.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.47295,
            "range": "± 0.345119",
            "unit": "ns",
            "extra": "100 samples\n8436 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.50946,
            "range": "± 0.446682",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.1235,
            "range": "± 810.09",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.3542,
            "range": "± 1.52518",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.78646,
            "range": "± 18.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.79892,
            "range": "± 17.2215",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 53.6829,
            "range": "± 5.33289",
            "unit": "ns",
            "extra": "100 samples\n539 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 54.0663,
            "range": "± 6.56819",
            "unit": "ns",
            "extra": "100 samples\n548 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 430.511,
            "range": "± 11.0416",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 429.677,
            "range": "± 8.7984",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.2629,
            "range": "± 1.03461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.0547,
            "range": "± 1.75243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 52.9314,
            "range": "± 3.64993",
            "unit": "ns",
            "extra": "100 samples\n549 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.539,
            "range": "± 5.42051",
            "unit": "ns",
            "extra": "100 samples\n704 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 38.6892,
            "range": "± 4.36017",
            "unit": "ns",
            "extra": "100 samples\n777 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 429.748,
            "range": "± 9.6727",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 332.891,
            "range": "± 11.9469",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 436.343,
            "range": "± 11.2696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 94.8087,
            "range": "± 1.93543",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 68.6959,
            "range": "± 2.68394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.2063,
            "range": "± 681.249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 38.4776,
            "range": "± 4.3957",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.3698,
            "range": "± 3.82291",
            "unit": "ns",
            "extra": "100 samples\n889 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 436.932,
            "range": "± 11.1891",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 436.903,
            "range": "± 10.1021",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 86.4625,
            "range": "± 448.71",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 85.9693,
            "range": "± 668.693",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 38.5758,
            "range": "± 3.53767",
            "unit": "ns",
            "extra": "100 samples\n778 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 38.0418,
            "range": "± 2.59234",
            "unit": "ns",
            "extra": "100 samples\n779 iterations"
          },
          {
            "name": "ssa_add",
            "value": 183.97,
            "range": "± 16.6524",
            "unit": "ns",
            "extra": "100 samples\n165 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 455.727,
            "range": "± 36.6172",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.13554,
            "range": "± 77.3478",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 471.51,
            "range": "± 26.5733",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 903.863,
            "range": "± 102.714",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 484.164,
            "range": "± 62.7921",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 572.883,
            "range": "± 39.082",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 409.112,
            "range": "± 28.2293",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 490.175,
            "range": "± 34.5055",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 439.861,
            "range": "± 23.4595",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
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
          "id": "f40ce4845ca83f2ae3c1896e288a3f80b867628a",
          "message": "Wire the fuzzer into CI: PR smoke gate + weekly coverage-guided sweep (#376)",
          "timestamp": "2026-07-08T18:49:34+02:00",
          "tree_id": "945062954b82939cc3ebb68c82264d07de1b5be4",
          "url": "https://github.com/nebulastream/nautilus/commit/f40ce4845ca83f2ae3c1896e288a3f80b867628a"
        },
        "date": 1783529882517,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.00104,
            "range": "± 325.341",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.00804,
            "range": "± 905.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.52347,
            "range": "± 227.831",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.29743,
            "range": "± 330.108",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.5552,
            "range": "± 1.07569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.8359,
            "range": "± 474.872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.4418,
            "range": "± 351.512",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.19965,
            "range": "± 204.884",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.6453,
            "range": "± 355.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.4805,
            "range": "± 390.024",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.1312,
            "range": "± 381.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.4105,
            "range": "± 355.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.6172,
            "range": "± 692.589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.0431,
            "range": "± 401.566",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.44247,
            "range": "± 155.265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.3618,
            "range": "± 130.456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.43613,
            "range": "± 177.952",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.4631,
            "range": "± 711.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 98.0348,
            "range": "± 27.4974",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.61799,
            "range": "± 179.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.76543,
            "range": "± 418.806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.767,
            "range": "± 406.338",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.258,
            "range": "± 795.55",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 132.325,
            "range": "± 17.4704",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 743.704,
            "range": "± 43.0261",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.56324,
            "range": "± 129.394",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.33141,
            "range": "± 266.713",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.61495,
            "range": "± 125.262",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.77107,
            "range": "± 242.983",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.81245,
            "range": "± 144.862",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01834,
            "range": "± 171.55",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.41826,
            "range": "± 91.8721",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.72537,
            "range": "± 118.025",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.50566,
            "range": "± 228.854",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.84665,
            "range": "± 845.15",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.6726,
            "range": "± 6.54239",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.9919,
            "range": "± 1.48366",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 168.051,
            "range": "± 8.52439",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.833,
            "range": "± 5.46623",
            "unit": "ns",
            "extra": "100 samples\n740 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 312.181,
            "range": "± 13.3101",
            "unit": "ns",
            "extra": "100 samples\n8256 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.96003,
            "range": "± 0.49718",
            "unit": "ns",
            "extra": "100 samples\n7748 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.6466,
            "range": "± 2.09221",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.32997,
            "range": "± 284.458",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.32121,
            "range": "± 238.704",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.84805,
            "range": "± 1.46534",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.51046,
            "range": "± 495.027",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.1408,
            "range": "± 3.22932",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.6294,
            "range": "± 1.79333",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.56061,
            "range": "± 1.2196",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.66355,
            "range": "± 478.542",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.8386,
            "range": "± 3.29449",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.37888,
            "range": "± 1.24115",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.83234,
            "range": "± 1.32496",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.73988,
            "range": "± 531.606",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.0468,
            "range": "± 2.23189",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.034,
            "range": "± 1.62767",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.26519,
            "range": "± 739.036",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.34913,
            "range": "± 1.20138",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.0727,
            "range": "± 1.82544",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.11759,
            "range": "± 1.03048",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.90518,
            "range": "± 1.21079",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.92041,
            "range": "± 451.136",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.5832,
            "range": "± 7.57042",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.1096,
            "range": "± 6.35015",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.36503,
            "range": "± 44.0102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.37896,
            "range": "± 53.9371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.3859,
            "range": "± 11.5858",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 48.8038,
            "range": "± 8.12306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45164,
            "range": "± 51.5101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.27347,
            "range": "± 48.137",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3424.57,
            "range": "± 104297",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.10746,
            "range": "± 277.545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 178.726,
            "range": "± 10.343",
            "unit": "ns",
            "extra": "100 samples\n164 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 451.646,
            "range": "± 33.9467",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.14266,
            "range": "± 62.247",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 473.159,
            "range": "± 27.2745",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 890.244,
            "range": "± 76.3035",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 510.363,
            "range": "± 88.0778",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 569.753,
            "range": "± 27.8016",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 422.875,
            "range": "± 46.3699",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 503.066,
            "range": "± 51.0695",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 454.45,
            "range": "± 52.9235",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.0862,
            "range": "± 1.14504",
            "unit": "ns",
            "extra": "100 samples\n8459 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.6243,
            "range": "± 2.32465",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 606.846,
            "range": "± 40.7026",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.77289,
            "range": "± 0.792908",
            "unit": "ns",
            "extra": "100 samples\n6576 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.5936,
            "range": "± 4.5575",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.4598,
            "range": "± 71.0041",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 53.5135,
            "range": "± 1.24967",
            "unit": "ns",
            "extra": "100 samples\n547 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 432.118,
            "range": "± 13.2637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.4766,
            "range": "± 189.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.8693,
            "range": "± 3.76267",
            "unit": "ns",
            "extra": "100 samples\n1058 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 144.856,
            "range": "± 9.65742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.7237,
            "range": "± 71.1662",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.55782,
            "range": "± 0.729931",
            "unit": "ns",
            "extra": "100 samples\n8471 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.3868,
            "range": "± 1.69882",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80271,
            "range": "± 21.1561",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 55.7061,
            "range": "± 11.6292",
            "unit": "ns",
            "extra": "100 samples\n547 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 55.1404,
            "range": "± 7.17841",
            "unit": "ns",
            "extra": "100 samples\n543 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 430.671,
            "range": "± 10.1648",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 390.329,
            "range": "± 13.4303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 95.4237,
            "range": "± 432.425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.7055,
            "range": "± 287.679",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 27.3588,
            "range": "± 4.46378",
            "unit": "ns",
            "extra": "100 samples\n1121 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 26.6915,
            "range": "± 3.04041",
            "unit": "ns",
            "extra": "100 samples\n1121 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 145.24,
            "range": "± 11.0975",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 143.12,
            "range": "± 7.02762",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.7523,
            "range": "± 125.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9784,
            "range": "± 68.6602",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.53975,
            "range": "± 0.500097",
            "unit": "ns",
            "extra": "100 samples\n8431 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.6568,
            "range": "± 0.895016",
            "unit": "ns",
            "extra": "100 samples\n8462 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.5441,
            "range": "± 2.24492",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.3295,
            "range": "± 1.43398",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.79484,
            "range": "± 24.0295",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.80573,
            "range": "± 35.3542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 54.7084,
            "range": "± 6.0781",
            "unit": "ns",
            "extra": "100 samples\n551 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 54.7144,
            "range": "± 6.20036",
            "unit": "ns",
            "extra": "100 samples\n547 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 433.338,
            "range": "± 14.1354",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 433.108,
            "range": "± 13.4909",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.5085,
            "range": "± 227.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.4802,
            "range": "± 174.487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 55.0564,
            "range": "± 6.92496",
            "unit": "ns",
            "extra": "100 samples\n548 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.3202,
            "range": "± 4.98038",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 38.9284,
            "range": "± 4.11859",
            "unit": "ns",
            "extra": "100 samples\n766 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 432.62,
            "range": "± 13.9514",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 332.087,
            "range": "± 10.4208",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 436.857,
            "range": "± 12.4801",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 95.4494,
            "range": "± 235.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 69.4439,
            "range": "± 2.71317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.6076,
            "range": "± 717.139",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.3206,
            "range": "± 4.75011",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.2867,
            "range": "± 5.26744",
            "unit": "ns",
            "extra": "100 samples\n853 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 435.479,
            "range": "± 10.6901",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 435.818,
            "range": "± 10.6278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 86.535,
            "range": "± 741.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.4911,
            "range": "± 724.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.9908,
            "range": "± 5.56132",
            "unit": "ns",
            "extra": "100 samples\n782 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.5102,
            "range": "± 4.32363",
            "unit": "ns",
            "extra": "100 samples\n766 iterations"
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
          "id": "14b8779bb31d13a1a9f0359d40aec5bcbe7cac8f",
          "message": "[fuzzer] Sweep compiler-option / IR-pass permutations in the differential check (#366)",
          "timestamp": "2026-07-08T23:54:07+02:00",
          "tree_id": "fe64842e0dc7b5aed9c04898973fdd83cf9cdd61",
          "url": "https://github.com/nebulastream/nautilus/commit/14b8779bb31d13a1a9f0359d40aec5bcbe7cac8f"
        },
        "date": 1783548096468,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 189.683,
            "range": "± 22.1435",
            "unit": "ns",
            "extra": "100 samples\n165 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 447.09,
            "range": "± 56.4127",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10698,
            "range": "± 76.8539",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 473.148,
            "range": "± 28.5543",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 852.487,
            "range": "± 58.293",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 497.983,
            "range": "± 61.104",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 574.984,
            "range": "± 46.7359",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 418.539,
            "range": "± 41.9918",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 495.628,
            "range": "± 41.0553",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 459.397,
            "range": "± 70.174",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3455.64,
            "range": "± 93007.5",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.69342,
            "range": "± 374.152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.37833,
            "range": "± 49.851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.28493,
            "range": "± 45.8553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.28697,
            "range": "± 79.5036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 798.667,
            "range": "± 76.1341",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.56928,
            "range": "± 133.239",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.36304,
            "range": "± 235.3",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.62131,
            "range": "± 182.006",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.87574,
            "range": "± 279.797",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.80265,
            "range": "± 308.099",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.06626,
            "range": "± 227.803",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.46245,
            "range": "± 167.987",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74367,
            "range": "± 138.248",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.49687,
            "range": "± 142.622",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.76069,
            "range": "± 553.136",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 91.7975,
            "range": "± 4.46084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.0102,
            "range": "± 1.09688",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 174.577,
            "range": "± 8.80268",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.59745,
            "range": "± 75.8607",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.18086,
            "range": "± 70.1066",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.10257,
            "range": "± 71.2786",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.26388,
            "range": "± 66.5618",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.2998,
            "range": "± 1.93843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.0253,
            "range": "± 589.45",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.0524,
            "range": "± 211.875",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.0497,
            "range": "± 73.1474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.5517,
            "range": "± 106.49",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.52985,
            "range": "± 100.353",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.6082,
            "range": "± 117.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.217,
            "range": "± 197.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.84473,
            "range": "± 225.364",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.7803,
            "range": "± 335.166",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.6772,
            "range": "± 412.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.3495,
            "range": "± 501.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.8964,
            "range": "± 2.25797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.4103,
            "range": "± 2.09075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.4066,
            "range": "± 679.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.227,
            "range": "± 807.182",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.6674,
            "range": "± 888.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.8375,
            "range": "± 792.768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.3315,
            "range": "± 1.74097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.6174,
            "range": "± 829.412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.7075,
            "range": "± 879.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.3274,
            "range": "± 891.694",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.0567,
            "range": "± 839.436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.663,
            "range": "± 1.15819",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.3679,
            "range": "± 2.1698",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.4755,
            "range": "± 3.35036",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.7216,
            "range": "± 3.94249",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.3691,
            "range": "± 2.66709",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 23.9821,
            "range": "± 3.34349",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.4873,
            "range": "± 3.91159",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.6934,
            "range": "± 5.90259",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.7222,
            "range": "± 3.85038",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 20.9393,
            "range": "± 3.34641",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 20.0257,
            "range": "± 3.28267",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 48.8948,
            "range": "± 6.12696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 290.023,
            "range": "± 24.7282",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 65.2141,
            "range": "± 8.75591",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 495.08,
            "range": "± 17.7343",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.97013,
            "range": "± 191.615",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.33227,
            "range": "± 640.943",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.92629,
            "range": "± 1.27012",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.36166,
            "range": "± 514.496",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.48818,
            "range": "± 782.8",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.67684,
            "range": "± 503.618",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.65328,
            "range": "± 519.113",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.85142,
            "range": "± 409.04",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.82478,
            "range": "± 633.347",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.98474,
            "range": "± 628.041",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 27.5076,
            "range": "± 3.073",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 285.618,
            "range": "± 8.92333",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 35.1404,
            "range": "± 3.57823",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 417.843,
            "range": "± 23.2181",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 24.2717,
            "range": "± 3.3439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.2864,
            "range": "± 3.84337",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 50.0224,
            "range": "± 10.128",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.6945,
            "range": "± 4.74709",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.8415,
            "range": "± 11.603",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 42.9908,
            "range": "± 8.25887",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.3782,
            "range": "± 9.51436",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.7142,
            "range": "± 4.42358",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 33.6551,
            "range": "± 3.9617",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.2792,
            "range": "± 5.05442",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 78.0079,
            "range": "± 9.80308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 492.899,
            "range": "± 18.5419",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 93.4777,
            "range": "± 11.4237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 634.361,
            "range": "± 22.461",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.3397,
            "range": "± 311.307",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.63287,
            "range": "± 778.411",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.8132,
            "range": "± 1.23382",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.57547,
            "range": "± 490.909",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.0124,
            "range": "± 2.80491",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.8904,
            "range": "± 1.78046",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.66303,
            "range": "± 1.18416",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.70593,
            "range": "± 577.807",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.8575,
            "range": "± 2.71583",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.46942,
            "range": "± 989.557",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.8304,
            "range": "± 1.28981",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.8357,
            "range": "± 485.561",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.1048,
            "range": "± 2.78449",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.3108,
            "range": "± 1.51915",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.46717,
            "range": "± 854.023",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.56039,
            "range": "± 754.296",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.0794,
            "range": "± 1.66522",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.11087,
            "range": "± 724.107",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.94831,
            "range": "± 1.13354",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.08816,
            "range": "± 510.52",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 37.9786,
            "range": "± 7.23566",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.1445,
            "range": "± 5.94377",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35173,
            "range": "± 38.332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36772,
            "range": "± 48.8988",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.0727,
            "range": "± 8.94107",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 48.9055,
            "range": "± 7.05805",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.41664,
            "range": "± 40.9728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.3069,
            "range": "± 38.0332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 41.3245,
            "range": "± 7.4719",
            "unit": "ns",
            "extra": "100 samples\n731 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 288.312,
            "range": "± 5.71107",
            "unit": "ns",
            "extra": "100 samples\n8234 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.94593,
            "range": "± 0.530753",
            "unit": "ns",
            "extra": "100 samples\n7738 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.5856,
            "range": "± 1.98647",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.5569,
            "range": "± 0.957746",
            "unit": "ns",
            "extra": "100 samples\n8483 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.5513,
            "range": "± 1.52069",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 543.179,
            "range": "± 34.6101",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.76172,
            "range": "± 0.910991",
            "unit": "ns",
            "extra": "100 samples\n6566 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.0034,
            "range": "± 4.29059",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.4073,
            "range": "± 68.9284",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 53.6233,
            "range": "± 2.39616",
            "unit": "ns",
            "extra": "100 samples\n532 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 431.15,
            "range": "± 11.3551",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 94.7658,
            "range": "± 2.62637",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 26.6071,
            "range": "± 3.02734",
            "unit": "ns",
            "extra": "100 samples\n1157 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 140.487,
            "range": "± 5.7542",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.2511,
            "range": "± 159.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56801,
            "range": "± 0.662199",
            "unit": "ns",
            "extra": "100 samples\n8478 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.7187,
            "range": "± 2.42364",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80822,
            "range": "± 51.931",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 53.3981,
            "range": "± 0.886462",
            "unit": "ns",
            "extra": "100 samples\n532 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 55.0367,
            "range": "± 7.32706",
            "unit": "ns",
            "extra": "100 samples\n533 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 431.164,
            "range": "± 11.6266",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 389.038,
            "range": "± 13.2051",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 94.6557,
            "range": "± 2.21379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.0462,
            "range": "± 2.06405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 26.735,
            "range": "± 3.06805",
            "unit": "ns",
            "extra": "100 samples\n1137 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 26.0811,
            "range": "± 2.41377",
            "unit": "ns",
            "extra": "100 samples\n1138 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 141.757,
            "range": "± 7.00518",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 142.653,
            "range": "± 7.73334",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.2128,
            "range": "± 50.656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9606,
            "range": "± 81.7397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.53616,
            "range": "± 0.444475",
            "unit": "ns",
            "extra": "100 samples\n8500 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.52063,
            "range": "± 0.409302",
            "unit": "ns",
            "extra": "100 samples\n8492 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.1953,
            "range": "± 1.14959",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.4526,
            "range": "± 2.03601",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.79544,
            "range": "± 22.6922",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.83247,
            "range": "± 281.496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 53.7209,
            "range": "± 3.31915",
            "unit": "ns",
            "extra": "100 samples\n533 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 53.6882,
            "range": "± 4.63621",
            "unit": "ns",
            "extra": "100 samples\n531 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 430.284,
            "range": "± 10.0406",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 432.27,
            "range": "± 13.5598",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 94.7041,
            "range": "± 3.50961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 93.7558,
            "range": "± 2.68879",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 53.9547,
            "range": "± 5.69584",
            "unit": "ns",
            "extra": "100 samples\n528 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.5811,
            "range": "± 4.52889",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.7723,
            "range": "± 5.7715",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 434.049,
            "range": "± 18.7702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 332.317,
            "range": "± 11.0427",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 436.228,
            "range": "± 14.7313",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 93.964,
            "range": "± 2.81663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 69.8854,
            "range": "± 3.12756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.725,
            "range": "± 1.37075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.3616,
            "range": "± 5.42663",
            "unit": "ns",
            "extra": "100 samples\n784 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.6287,
            "range": "± 2.89211",
            "unit": "ns",
            "extra": "100 samples\n873 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 435.438,
            "range": "± 11.9375",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 434.497,
            "range": "± 10.2818",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 86.4488,
            "range": "± 590",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.8712,
            "range": "± 3.47558",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.0278,
            "range": "± 3.68365",
            "unit": "ns",
            "extra": "100 samples\n752 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.548,
            "range": "± 6.27968",
            "unit": "ns",
            "extra": "100 samples\n778 iterations"
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
          "id": "b8dd03411d3b274a692e945897f55ae62045cdb5",
          "message": "Add assume_constant, assume_range, assume_nonzero specializations (#306)",
          "timestamp": "2026-07-09T08:11:05+02:00",
          "tree_id": "da60b927670d3e6aa22bce6f5688aee87a159880",
          "url": "https://github.com/nebulastream/nautilus/commit/b8dd03411d3b274a692e945897f55ae62045cdb5"
        },
        "date": 1783577918006,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 183.366,
            "range": "± 15.7922",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 450.743,
            "range": "± 49.1275",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10237,
            "range": "± 81.2423",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 480.131,
            "range": "± 34.2545",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 852.146,
            "range": "± 51.5692",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 512.538,
            "range": "± 79.3245",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 590.898,
            "range": "± 56.5898",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 421.109,
            "range": "± 79.5345",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.209,
            "range": "± 40.3367",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 452.514,
            "range": "± 54.9062",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.3377,
            "range": "± 103.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.22336,
            "range": "± 61.1001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.22428,
            "range": "± 65.8991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 742.281,
            "range": "± 41.9677",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.56339,
            "range": "± 107.218",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.42081,
            "range": "± 271.821",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.6419,
            "range": "± 134.059",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.8871,
            "range": "± 265.433",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.86279,
            "range": "± 195.477",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.02016,
            "range": "± 263.998",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.46367,
            "range": "± 138.16",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.79825,
            "range": "± 144.221",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.51341,
            "range": "± 134.02",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.78206,
            "range": "± 595.531",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.7018,
            "range": "± 6.7776",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.0022,
            "range": "± 993.771",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 170.028,
            "range": "± 7.34473",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.5186,
            "range": "± 135.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.07935,
            "range": "± 168.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.02185,
            "range": "± 151.574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.03297,
            "range": "± 148.394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.2506,
            "range": "± 259.451",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6333,
            "range": "± 138.879",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.548,
            "range": "± 289.828",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.96993,
            "range": "± 92.4576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.392,
            "range": "± 202.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.35415,
            "range": "± 119.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.4034,
            "range": "± 106.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.9788,
            "range": "± 370.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.48514,
            "range": "± 115.029",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.3187,
            "range": "± 235.389",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.4457,
            "range": "± 366.637",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.0058,
            "range": "± 415.217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.8982,
            "range": "± 321.486",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.1046,
            "range": "± 310.986",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.0895,
            "range": "± 372.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.4035,
            "range": "± 265.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.5093,
            "range": "± 319.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.9647,
            "range": "± 475.07",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.224,
            "range": "± 331.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.0461,
            "range": "± 347.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.2036,
            "range": "± 562.953",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.9706,
            "range": "± 353.624",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.3968,
            "range": "± 375.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.5767,
            "range": "± 566.613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7506,
            "range": "± 2.67409",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.6298,
            "range": "± 3.12543",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 24.6746,
            "range": "± 3.94954",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.3255,
            "range": "± 3.92218",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 23.6933,
            "range": "± 3.57835",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.8464,
            "range": "± 3.73989",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.5555,
            "range": "± 5.16724",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.23,
            "range": "± 3.04065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 20.0865,
            "range": "± 2.48422",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.5617,
            "range": "± 3.82711",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 48.7329,
            "range": "± 9.28946",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 273.033,
            "range": "± 15.1814",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 61.7581,
            "range": "± 8.25296",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 431.657,
            "range": "± 13.5542",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.84288,
            "range": "± 253.044",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.22194,
            "range": "± 497.789",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.48021,
            "range": "± 1.03183",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.26501,
            "range": "± 528.041",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.18277,
            "range": "± 790.801",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.4692,
            "range": "± 537.267",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.3662,
            "range": "± 462.091",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.53092,
            "range": "± 529.845",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.61059,
            "range": "± 479.68",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.77157,
            "range": "± 375.579",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.0004,
            "range": "± 3.0856",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 268.54,
            "range": "± 9.14892",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 32.789,
            "range": "± 2.92953",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 381.085,
            "range": "± 10.8024",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.5728,
            "range": "± 4.65715",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.352,
            "range": "± 6.44941",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.8905,
            "range": "± 9.68158",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.528,
            "range": "± 4.19591",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.8938,
            "range": "± 8.33724",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 36.8493,
            "range": "± 5.29293",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 45.2781,
            "range": "± 9.2684",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.7197,
            "range": "± 4.3565",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.6518,
            "range": "± 5.26375",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.7223,
            "range": "± 5.63561",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 76.5731,
            "range": "± 10.1747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 490.942,
            "range": "± 15.0466",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 93.4498,
            "range": "± 11.0021",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 629.576,
            "range": "± 17.626",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.25855,
            "range": "± 217.367",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.29968,
            "range": "± 300.37",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.686,
            "range": "± 1.25914",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.64187,
            "range": "± 617.505",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.8425,
            "range": "± 3.10961",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.4296,
            "range": "± 1.66459",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.51884,
            "range": "± 1.15269",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.65133,
            "range": "± 641.911",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.6081,
            "range": "± 1.90395",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.56428,
            "range": "± 1.11759",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.64894,
            "range": "± 1.21339",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.86008,
            "range": "± 551.4",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.0774,
            "range": "± 2.00367",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.99139,
            "range": "± 1.30127",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.26277,
            "range": "± 563.927",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.19712,
            "range": "± 623.954",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.89803,
            "range": "± 1.20681",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.03325,
            "range": "± 769.842",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.94175,
            "range": "± 673.823",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.92384,
            "range": "± 442.905",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 37.3578,
            "range": "± 3.54105",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.2769,
            "range": "± 5.75806",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.33037,
            "range": "± 30.1631",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.33986,
            "range": "± 36.7337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.784,
            "range": "± 4.57549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 47.8214,
            "range": "± 7.00374",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.41093,
            "range": "± 51.8015",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.25717,
            "range": "± 37.5278",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.2599,
            "range": "± 0.992492",
            "unit": "ns",
            "extra": "100 samples\n8459 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.0377,
            "range": "± 1.24569",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 537.475,
            "range": "± 25.7629",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.80435,
            "range": "± 1.02799",
            "unit": "ns",
            "extra": "100 samples\n6583 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.5904,
            "range": "± 5.15455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.3728,
            "range": "± 64.805",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 53.6301,
            "range": "± 3.18475",
            "unit": "ns",
            "extra": "100 samples\n544 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 431.493,
            "range": "± 14.5913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.5996,
            "range": "± 3.04744",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 26.5216,
            "range": "± 2.46214",
            "unit": "ns",
            "extra": "100 samples\n1154 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 141.662,
            "range": "± 6.09139",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.2433,
            "range": "± 192.856",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56266,
            "range": "± 0.524138",
            "unit": "ns",
            "extra": "100 samples\n8476 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.671,
            "range": "± 1.91917",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.79977,
            "range": "± 49.8118",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 54.4062,
            "range": "± 7.52734",
            "unit": "ns",
            "extra": "100 samples\n549 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 54.8347,
            "range": "± 6.78687",
            "unit": "ns",
            "extra": "100 samples\n547 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 429.789,
            "range": "± 9.27493",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 388.943,
            "range": "± 11.7719",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 95.4539,
            "range": "± 1.74597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.7138,
            "range": "± 431.334",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 27.0077,
            "range": "± 2.68503",
            "unit": "ns",
            "extra": "100 samples\n1143 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 26.6048,
            "range": "± 3.16677",
            "unit": "ns",
            "extra": "100 samples\n1130 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 142.94,
            "range": "± 7.39263",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 152.659,
            "range": "± 18.8212",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.2238,
            "range": "± 86.0456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9653,
            "range": "± 74.1128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.53068,
            "range": "± 0.432697",
            "unit": "ns",
            "extra": "100 samples\n8484 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.52149,
            "range": "± 0.390779",
            "unit": "ns",
            "extra": "100 samples\n8409 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.1191,
            "range": "± 949.378",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.4479,
            "range": "± 2.14753",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.80538,
            "range": "± 43.6552",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.80368,
            "range": "± 43.6946",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 54.5475,
            "range": "± 6.3398",
            "unit": "ns",
            "extra": "100 samples\n545 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 53.8899,
            "range": "± 5.70758",
            "unit": "ns",
            "extra": "100 samples\n546 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 430.165,
            "range": "± 9.71077",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 430.074,
            "range": "± 9.20928",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.1171,
            "range": "± 2.93074",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.1403,
            "range": "± 1.42962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 55.4858,
            "range": "± 8.71539",
            "unit": "ns",
            "extra": "100 samples\n546 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.6231,
            "range": "± 4.1096",
            "unit": "ns",
            "extra": "100 samples\n730 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.6267,
            "range": "± 6.65468",
            "unit": "ns",
            "extra": "100 samples\n782 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 432.963,
            "range": "± 14.5845",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 330.811,
            "range": "± 8.47363",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 434.49,
            "range": "± 9.22697",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 95.6899,
            "range": "± 2.52453",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 69.285,
            "range": "± 2.64056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 87.3571,
            "range": "± 2.84576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 38.8922,
            "range": "± 3.41876",
            "unit": "ns",
            "extra": "100 samples\n785 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.4268,
            "range": "± 3.46021",
            "unit": "ns",
            "extra": "100 samples\n881 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 434.944,
            "range": "± 10.1135",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 435.098,
            "range": "± 10.641",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 87.0089,
            "range": "± 2.65274",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.4087,
            "range": "± 597.879",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.3729,
            "range": "± 5.94698",
            "unit": "ns",
            "extra": "100 samples\n766 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.5814,
            "range": "± 7.68441",
            "unit": "ns",
            "extra": "100 samples\n777 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.9744,
            "range": "± 2.3718",
            "unit": "ns",
            "extra": "100 samples\n743 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 277.677,
            "range": "± 5.23679",
            "unit": "ns",
            "extra": "100 samples\n8301 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.98821,
            "range": "± 0.691522",
            "unit": "ns",
            "extra": "100 samples\n7752 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.6985,
            "range": "± 2.82815",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3295.34,
            "range": "± 75358.2",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.45698,
            "range": "± 103.069",
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
          "id": "d25f7224943c8c40bbd7c62af30fa400adc22846",
          "message": "Erase zero-count entries in AliveVariableHash to stop unbounded memory growth (#379)",
          "timestamp": "2026-07-09T08:32:16+02:00",
          "tree_id": "31ad73b16bc2086ee2bfa6ca51ba3e697a8ba14e",
          "url": "https://github.com/nebulastream/nautilus/commit/d25f7224943c8c40bbd7c62af30fa400adc22846"
        },
        "date": 1783579203930,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.00578,
            "range": "± 182.272",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.02327,
            "range": "± 121.197",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.12468,
            "range": "± 999.571",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.00137,
            "range": "± 460.376",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.3423,
            "range": "± 3.82886",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.1418,
            "range": "± 1.28047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.03241,
            "range": "± 905.469",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.14203,
            "range": "± 423.779",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.7393,
            "range": "± 2.22486",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.32642,
            "range": "± 1.01693",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.31515,
            "range": "± 1.24557",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.21979,
            "range": "± 601.773",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.6746,
            "range": "± 2.27678",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 8.21665,
            "range": "± 799.357",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 6.76854,
            "range": "± 395.791",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 6.8787,
            "range": "± 650.723",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.34059,
            "range": "± 1.44399",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.27635,
            "range": "± 552.867",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.48197,
            "range": "± 1.05304",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.50563,
            "range": "± 377.191",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.4271,
            "range": "± 4.21253",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.5325,
            "range": "± 3.09354",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.70597,
            "range": "± 22.1474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.71878,
            "range": "± 25.1419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 93.0819,
            "range": "± 3.91496",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 41.8705,
            "range": "± 2.21639",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.40827,
            "range": "± 39.6332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.07723,
            "range": "± 15.48",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3761.67,
            "range": "± 148346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.12633,
            "range": "± 152.709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 188.719,
            "range": "± 16.4587",
            "unit": "ns",
            "extra": "100 samples\n99 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 454.825,
            "range": "± 43.0521",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.29512,
            "range": "± 87.6625",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 465.898,
            "range": "± 35.2384",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1005.42,
            "range": "± 90845.6",
            "unit": "ns",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 473.811,
            "range": "± 34.0935",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 555.439,
            "range": "± 24.7793",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 383.569,
            "range": "± 25.2688",
            "unit": "ns",
            "extra": "100 samples\n49 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 499.642,
            "range": "± 44.1459",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 439.145,
            "range": "± 32.8035",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.23497,
            "range": "± 212.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.74573,
            "range": "± 236.122",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.74529,
            "range": "± 227.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.80318,
            "range": "± 226.929",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 27.6035,
            "range": "± 321.084",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.0101,
            "range": "± 236.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.0927,
            "range": "± 1.0441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.70925,
            "range": "± 148.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.7372,
            "range": "± 244.021",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.64721,
            "range": "± 238.127",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.6846,
            "range": "± 171.228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.2398,
            "range": "± 328.83",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.0473,
            "range": "± 211.82",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 60.0582,
            "range": "± 431.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 12.1056,
            "range": "± 0.929988",
            "unit": "ns",
            "extra": "100 samples\n6283 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 26.3846,
            "range": "± 1.92857",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 671.511,
            "range": "± 33.388",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.30796,
            "range": "± 0.828792",
            "unit": "ns",
            "extra": "100 samples\n3828 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 76.1261,
            "range": "± 3.43422",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 16.5028,
            "range": "± 55.2497",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 38.4664,
            "range": "± 5.85495",
            "unit": "ns",
            "extra": "100 samples\n493 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 328.615,
            "range": "± 7.62467",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 70.6089,
            "range": "± 1.52589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 26.645,
            "range": "± 1.49743",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 158.514,
            "range": "± 4.7902",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 43.5724,
            "range": "± 148.849",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.9345,
            "range": "± 0.224817",
            "unit": "ns",
            "extra": "100 samples\n6285 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 13.1809,
            "range": "± 706.078",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.75213,
            "range": "± 18.1094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 38.8887,
            "range": "± 5.16204",
            "unit": "ns",
            "extra": "100 samples\n492 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 37.9674,
            "range": "± 1.89103",
            "unit": "ns",
            "extra": "100 samples\n494 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 330.344,
            "range": "± 9.92677",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 308.57,
            "range": "± 14.1668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 70.377,
            "range": "± 790.06",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 66.3665,
            "range": "± 4.12681",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 26.7964,
            "range": "± 2.04978",
            "unit": "ns",
            "extra": "100 samples\n666 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.0531,
            "range": "± 2.37723",
            "unit": "ns",
            "extra": "100 samples\n683 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 159.506,
            "range": "± 2.51191",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 159.559,
            "range": "± 3.00486",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 43.5155,
            "range": "± 96.3582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 40.1773,
            "range": "± 410.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 2.96318,
            "range": "± 0.248356",
            "unit": "ns",
            "extra": "100 samples\n6281 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 2.92578,
            "range": "± 0.132189",
            "unit": "ns",
            "extra": "100 samples\n6279 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 13.4344,
            "range": "± 1.50571",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 13.2872,
            "range": "± 870.916",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 3.7539,
            "range": "± 14.9201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 3.75457,
            "range": "± 11.1153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 38.1623,
            "range": "± 2.82183",
            "unit": "ns",
            "extra": "100 samples\n492 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 38.2559,
            "range": "± 5.19319",
            "unit": "ns",
            "extra": "100 samples\n492 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 331.138,
            "range": "± 11.0146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 328.072,
            "range": "± 6.75866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 70.251,
            "range": "± 454.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 70.5461,
            "range": "± 1.34866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 37.9065,
            "range": "± 2.4898",
            "unit": "ns",
            "extra": "100 samples\n486 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 37.8615,
            "range": "± 2.77577",
            "unit": "ns",
            "extra": "100 samples\n498 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 38.595,
            "range": "± 5.62536",
            "unit": "ns",
            "extra": "100 samples\n522 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 331.262,
            "range": "± 10.5585",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 319.276,
            "range": "± 10.84",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 386.711,
            "range": "± 6.84535",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 70.9589,
            "range": "± 1.85657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 69.3751,
            "range": "± 1.15172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 83.0498,
            "range": "± 1.44441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 38.6057,
            "range": "± 3.88267",
            "unit": "ns",
            "extra": "100 samples\n510 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.5016,
            "range": "± 4.14955",
            "unit": "ns",
            "extra": "100 samples\n569 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 386.527,
            "range": "± 5.94849",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 387.302,
            "range": "± 7.73343",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 83.1848,
            "range": "± 1.80391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 82.7294,
            "range": "± 526.772",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 38.1366,
            "range": "± 3.51283",
            "unit": "ns",
            "extra": "100 samples\n522 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 38.1025,
            "range": "± 3.0314",
            "unit": "ns",
            "extra": "100 samples\n508 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.72159,
            "range": "± 172.285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.6159,
            "range": "± 128.551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.60637,
            "range": "± 95.7938",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 21.0404,
            "range": "± 507.537",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 60.2266,
            "range": "± 13.1395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.5631,
            "range": "± 219.229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.46331,
            "range": "± 121.492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.4992,
            "range": "± 170.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 21.5673,
            "range": "± 603.982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 117.935,
            "range": "± 14.2438",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 850.764,
            "range": "± 67.3034",
            "unit": "ns",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.72344,
            "range": "± 96.7912",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.82864,
            "range": "± 215.294",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.90886,
            "range": "± 129.046",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.23642,
            "range": "± 163.167",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.04976,
            "range": "± 111.317",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.20949,
            "range": "± 129.308",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.5733,
            "range": "± 168.562",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.9054,
            "range": "± 101.461",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.64988,
            "range": "± 106.624",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.66266,
            "range": "± 415.742",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 97.1768,
            "range": "± 4.98968",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 13.4904,
            "range": "± 1.01273",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 176.459,
            "range": "± 6.79002",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 33.4489,
            "range": "± 2.94667",
            "unit": "ns",
            "extra": "100 samples\n559 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 398.901,
            "range": "± 8.03549",
            "unit": "ns",
            "extra": "100 samples\n6193 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.76956,
            "range": "± 0.4273",
            "unit": "ns",
            "extra": "100 samples\n5495 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 35.8231,
            "range": "± 1.87383",
            "unit": "ns",
            "extra": "100 samples\n500 iterations"
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
          "id": "f41374a53c74c9e5e325f99786bcd7dc88241144",
          "message": "Fix val<bool> default constructor stamping its trace state i32 instead of i1 (#377) (#380)",
          "timestamp": "2026-07-09T10:02:16+02:00",
          "tree_id": "a99ad28d5f98f3212cc7d048a554b007c3e0a1b1",
          "url": "https://github.com/nebulastream/nautilus/commit/f41374a53c74c9e5e325f99786bcd7dc88241144"
        },
        "date": 1783584793684,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3463.76,
            "range": "± 199089",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.74879,
            "range": "± 432.889",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 776.301,
            "range": "± 79.8048",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.66552,
            "range": "± 197.026",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.38832,
            "range": "± 281.408",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.68975,
            "range": "± 214.503",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.90519,
            "range": "± 314.265",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.78207,
            "range": "± 120.577",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.0415,
            "range": "± 219.885",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.48883,
            "range": "± 158.059",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.77205,
            "range": "± 161.076",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53376,
            "range": "± 184.337",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.83858,
            "range": "± 877.414",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.8768,
            "range": "± 5.49107",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8792,
            "range": "± 995.547",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 171.998,
            "range": "± 8.29716",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.59873,
            "range": "± 399.055",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.58653,
            "range": "± 383.618",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.3891,
            "range": "± 1.23953",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.93568,
            "range": "± 642.891",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.1045,
            "range": "± 2.97687",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.5802,
            "range": "± 1.99339",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.40226,
            "range": "± 1.33047",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.02606,
            "range": "± 728.008",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.401,
            "range": "± 2.70805",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.6913,
            "range": "± 1.09544",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.37847,
            "range": "± 1.34723",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.11186,
            "range": "± 645.682",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.3655,
            "range": "± 3.39147",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 15.3243,
            "range": "± 4.30168",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.01932,
            "range": "± 1.12244",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.80501,
            "range": "± 781.477",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.6667,
            "range": "± 1.65863",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.44976,
            "range": "± 925.086",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.46586,
            "range": "± 1.23074",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.29953,
            "range": "± 516.939",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 44.4188,
            "range": "± 6.58941",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 45.4956,
            "range": "± 7.90785",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80642,
            "range": "± 35.4136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.82227,
            "range": "± 58.1152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 106.176,
            "range": "± 8.27667",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.7741,
            "range": "± 7.91397",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.93623,
            "range": "± 205.022",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.21114,
            "range": "± 44.4422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.896,
            "range": "± 346.457",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.30767,
            "range": "± 363.573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.13365,
            "range": "± 140.976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.35789,
            "range": "± 336.052",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2558,
            "range": "± 1.18806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.0919,
            "range": "± 1.20147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.9496,
            "range": "± 792.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.14729,
            "range": "± 237.354",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.0728,
            "range": "± 508.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0482,
            "range": "± 497.236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.951,
            "range": "± 319.645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.0312,
            "range": "± 553.3",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.1778,
            "range": "± 856.013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.6828,
            "range": "± 604.969",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.42561,
            "range": "± 107.294",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.34111,
            "range": "± 162.817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.27521,
            "range": "± 105.829",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.5624,
            "range": "± 878.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 75.4746,
            "range": "± 14.5328",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.44261,
            "range": "± 120.757",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.36154,
            "range": "± 123.521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.36858,
            "range": "± 119.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.7244,
            "range": "± 683.981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 133.9,
            "range": "± 18.5651",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.7864,
            "range": "± 1.33064",
            "unit": "ns",
            "extra": "100 samples\n8480 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.0123,
            "range": "± 1.86314",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 550.158,
            "range": "± 69.58",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.6233,
            "range": "± 0.526639",
            "unit": "ns",
            "extra": "100 samples\n6552 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.1806,
            "range": "± 4.0411",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.4272,
            "range": "± 257.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.3058,
            "range": "± 6.38573",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 441.338,
            "range": "± 53.589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 98.0111,
            "range": "± 2.56027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.5168,
            "range": "± 3.91019",
            "unit": "ns",
            "extra": "100 samples\n1127 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 142.737,
            "range": "± 8.24385",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.538,
            "range": "± 42.7287",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49501,
            "range": "± 0.262662",
            "unit": "ns",
            "extra": "100 samples\n8474 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.226,
            "range": "± 1.04001",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.8109,
            "range": "± 60.563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 47.9748,
            "range": "± 6.08804",
            "unit": "ns",
            "extra": "100 samples\n639 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 48.9161,
            "range": "± 7.81767",
            "unit": "ns",
            "extra": "100 samples\n633 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 441.259,
            "range": "± 53.0676",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 391.024,
            "range": "± 34.5844",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 97.8564,
            "range": "± 1.5048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 82.1116,
            "range": "± 661.324",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 27.6939,
            "range": "± 4.71754",
            "unit": "ns",
            "extra": "100 samples\n1075 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.5584,
            "range": "± 4.32763",
            "unit": "ns",
            "extra": "100 samples\n1133 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 142.074,
            "range": "± 7.24783",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 142.502,
            "range": "± 6.73604",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.5595,
            "range": "± 187.939",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9826,
            "range": "± 58.5671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.55598,
            "range": "± 0.456682",
            "unit": "ns",
            "extra": "100 samples\n8491 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.5902,
            "range": "± 0.767078",
            "unit": "ns",
            "extra": "100 samples\n7285 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.829,
            "range": "± 3.08758",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.5345,
            "range": "± 2.25004",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.8089,
            "range": "± 34.4276",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.80937,
            "range": "± 38.1438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.2529,
            "range": "± 6.59167",
            "unit": "ns",
            "extra": "100 samples\n629 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.2023,
            "range": "± 3.44363",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 431.414,
            "range": "± 13.9557",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 436.805,
            "range": "± 45.1169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 98.1295,
            "range": "± 2.75429",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 97.7088,
            "range": "± 966.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 49.0665,
            "range": "± 9.37831",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.6378,
            "range": "± 5.14195",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 40.1203,
            "range": "± 7.02462",
            "unit": "ns",
            "extra": "100 samples\n770 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 440.685,
            "range": "± 50.1327",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 339.99,
            "range": "± 45.9944",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 439.042,
            "range": "± 14.2326",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 97.6201,
            "range": "± 341.948",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 69.4814,
            "range": "± 1.9765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.9319,
            "range": "± 763.881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.5489,
            "range": "± 7.84818",
            "unit": "ns",
            "extra": "100 samples\n769 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 36.0746,
            "range": "± 8.31595",
            "unit": "ns",
            "extra": "100 samples\n860 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 437.056,
            "range": "± 10.5284",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 438.152,
            "range": "± 13.241",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 87.3435,
            "range": "± 1.85246",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 87.0172,
            "range": "± 1.09887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.7211,
            "range": "± 6.79945",
            "unit": "ns",
            "extra": "100 samples\n769 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.3912,
            "range": "± 5.84741",
            "unit": "ns",
            "extra": "100 samples\n771 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 41.1053,
            "range": "± 6.36012",
            "unit": "ns",
            "extra": "100 samples\n748 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 285.832,
            "range": "± 6.75401",
            "unit": "ns",
            "extra": "100 samples\n8274 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.02278,
            "range": "± 0.663561",
            "unit": "ns",
            "extra": "100 samples\n7769 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.2643,
            "range": "± 3.84524",
            "unit": "ns",
            "extra": "100 samples\n759 iterations"
          },
          {
            "name": "ssa_add",
            "value": 181.424,
            "range": "± 19.4974",
            "unit": "ns",
            "extra": "100 samples\n164 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 458.523,
            "range": "± 51.2103",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.17222,
            "range": "± 167.162",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 488.313,
            "range": "± 62.795",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 938.76,
            "range": "± 102.535",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 494.181,
            "range": "± 48.2228",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 583.022,
            "range": "± 51.4938",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 409.059,
            "range": "± 49.1153",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 509.911,
            "range": "± 52.4863",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 456.237,
            "range": "± 46.5592",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
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
          "id": "60f07ce0cdd8abde19474d9840140d08b3467e60",
          "message": "Guard BC register allocator against 16-bit overflow (#385) (#387)",
          "timestamp": "2026-07-09T11:51:01+02:00",
          "tree_id": "92e2ec557ba6a804c0cad0e5b0c16aa2f8186140",
          "url": "https://github.com/nebulastream/nautilus/commit/60f07ce0cdd8abde19474d9840140d08b3467e60"
        },
        "date": 1783591126938,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.6089,
            "range": "± 0.9508",
            "unit": "ns",
            "extra": "100 samples\n8387 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.4422,
            "range": "± 1.49786",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 514.255,
            "range": "± 16.4196",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.68283,
            "range": "± 0.721362",
            "unit": "ns",
            "extra": "100 samples\n6480 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.0902,
            "range": "± 4.62135",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.3994,
            "range": "± 104.273",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.4651,
            "range": "± 7.94615",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 407.28,
            "range": "± 18.4695",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 98.1007,
            "range": "± 2.46376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 25.6756,
            "range": "± 1.33195",
            "unit": "ns",
            "extra": "100 samples\n1129 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 143.7,
            "range": "± 7.50104",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5516,
            "range": "± 158.629",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.47757,
            "range": "± 0.353337",
            "unit": "ns",
            "extra": "100 samples\n8421 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.2286,
            "range": "± 1.32072",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.79968,
            "range": "± 15.6247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 47.6201,
            "range": "± 6.61424",
            "unit": "ns",
            "extra": "100 samples\n634 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 53.5152,
            "range": "± 14.1563",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 430.372,
            "range": "± 14.9148",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 387.426,
            "range": "± 14.2367",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 97.6287,
            "range": "± 268.412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 82.1458,
            "range": "± 808.508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 25.9653,
            "range": "± 2.28726",
            "unit": "ns",
            "extra": "100 samples\n1134 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 26.0282,
            "range": "± 2.65333",
            "unit": "ns",
            "extra": "100 samples\n1133 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 143.891,
            "range": "± 7.81439",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 143.319,
            "range": "± 7.34258",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.6218,
            "range": "± 697.031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9697,
            "range": "± 67.4997",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.45879,
            "range": "± 0.331065",
            "unit": "ns",
            "extra": "100 samples\n8404 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.55181,
            "range": "± 0.517098",
            "unit": "ns",
            "extra": "100 samples\n8528 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.0738,
            "range": "± 1.40432",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.1447,
            "range": "± 996.87",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.79554,
            "range": "± 28.0165",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.81347,
            "range": "± 23.0979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.8488,
            "range": "± 7.09408",
            "unit": "ns",
            "extra": "100 samples\n633 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 48.6313,
            "range": "± 7.47219",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 429.105,
            "range": "± 11.9169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 430.099,
            "range": "± 13.4913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 97.5027,
            "range": "± 1.04483",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 97.4548,
            "range": "± 451.449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.8753,
            "range": "± 7.97807",
            "unit": "ns",
            "extra": "100 samples\n634 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.2155,
            "range": "± 5.34171",
            "unit": "ns",
            "extra": "100 samples\n728 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 38.4248,
            "range": "± 3.56298",
            "unit": "ns",
            "extra": "100 samples\n772 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 423.869,
            "range": "± 18.4469",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 330.394,
            "range": "± 9.94522",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 423.052,
            "range": "± 18.7179",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 97.6088,
            "range": "± 1.45312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 69.8742,
            "range": "± 1.94857",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 87.3019,
            "range": "± 1.73767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 38.8126,
            "range": "± 4.22231",
            "unit": "ns",
            "extra": "100 samples\n771 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 33.6501,
            "range": "± 4.03311",
            "unit": "ns",
            "extra": "100 samples\n887 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 437.77,
            "range": "± 14.6078",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 436.814,
            "range": "± 11.531",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 87.2209,
            "range": "± 1.28984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.8968,
            "range": "± 646.861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.315,
            "range": "± 6.95749",
            "unit": "ns",
            "extra": "100 samples\n764 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 38.4908,
            "range": "± 2.95851",
            "unit": "ns",
            "extra": "100 samples\n770 iterations"
          },
          {
            "name": "ssa_add",
            "value": 174.899,
            "range": "± 10.3077",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 482.533,
            "range": "± 27.0597",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.17954,
            "range": "± 59.238",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 489.742,
            "range": "± 34.382",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 915.119,
            "range": "± 52.8693",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 518.318,
            "range": "± 30.7473",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 615.218,
            "range": "± 32.4398",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 463.217,
            "range": "± 24.4615",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 534.484,
            "range": "± 35.2628",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 477.576,
            "range": "± 25.7924",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.46507,
            "range": "± 71.0929",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.0382,
            "range": "± 56.8272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.04524,
            "range": "± 250.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.11228,
            "range": "± 312.897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.5017,
            "range": "± 795.207",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6154,
            "range": "± 184.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.2616,
            "range": "± 294.825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.94015,
            "range": "± 47.2554",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.4565,
            "range": "± 763.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.21543,
            "range": "± 186.403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.3684,
            "range": "± 69.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.5927,
            "range": "± 279.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.44021,
            "range": "± 68.9242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.0258,
            "range": "± 242.312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.1575,
            "range": "± 344.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.8124,
            "range": "± 288.82",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.7725,
            "range": "± 330.019",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.0229,
            "range": "± 338.168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 25.8415,
            "range": "± 326.298",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.2317,
            "range": "± 569.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.5038,
            "range": "± 245.164",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.651,
            "range": "± 290.215",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.208,
            "range": "± 312.187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.8839,
            "range": "± 236.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.9314,
            "range": "± 286.68",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.0204,
            "range": "± 746.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.1885,
            "range": "± 365.723",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.5474,
            "range": "± 822.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.0924,
            "range": "± 1.98262",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.8518,
            "range": "± 2.55817",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.0822,
            "range": "± 3.69678",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.8027,
            "range": "± 3.9398",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 23.2604,
            "range": "± 2.42565",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.0244,
            "range": "± 3.38139",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.8842,
            "range": "± 2.4461",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.535,
            "range": "± 2.94887",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 20.5984,
            "range": "± 2.80489",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.2892,
            "range": "± 2.00557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 47.8062,
            "range": "± 6.38308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 282.061,
            "range": "± 11.4545",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 62.9205,
            "range": "± 6.83581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 448.762,
            "range": "± 15.0742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.771,
            "range": "± 120.235",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.12663,
            "range": "± 426.717",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.36716,
            "range": "± 943.864",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.2075,
            "range": "± 452.86",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 6.91412,
            "range": "± 678.027",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.35401,
            "range": "± 397.003",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.36449,
            "range": "± 354.629",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.40096,
            "range": "± 312.688",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.58883,
            "range": "± 482.758",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.75294,
            "range": "± 382.792",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.4778,
            "range": "± 4.68614",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 276.286,
            "range": "± 10.9819",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 33.0862,
            "range": "± 2.84631",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 402.153,
            "range": "± 14.006",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.4769,
            "range": "± 4.40259",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.1455,
            "range": "± 3.64264",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.2915,
            "range": "± 5.44342",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.8675,
            "range": "± 4.17601",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.8314,
            "range": "± 7.54691",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 43.6867,
            "range": "± 6.52328",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 43.9216,
            "range": "± 5.93304",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.5848,
            "range": "± 4.15564",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.6375,
            "range": "± 3.79824",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 31.2966,
            "range": "± 3.37269",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 76.8247,
            "range": "± 8.93052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 490.251,
            "range": "± 19.1241",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 93.2047,
            "range": "± 8.23919",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 633.732,
            "range": "± 20.7172",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.30281,
            "range": "± 40.0578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.23593,
            "range": "± 29.492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.22406,
            "range": "± 57.6859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.1741,
            "range": "± 320.127",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 73.5161,
            "range": "± 7.11207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.3211,
            "range": "± 137.837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.27268,
            "range": "± 59.4995",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.31209,
            "range": "± 46.4859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 24.6472,
            "range": "± 387.377",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 132.143,
            "range": "± 11.823",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.35753,
            "range": "± 129.844",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.36068,
            "range": "± 142.358",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.18812,
            "range": "± 772.967",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.60806,
            "range": "± 324.339",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.5634,
            "range": "± 1.58093",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.8091,
            "range": "± 771.153",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.04418,
            "range": "± 846.599",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.70613,
            "range": "± 433.004",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.3521,
            "range": "± 1.44432",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.40124,
            "range": "± 584.858",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.84204,
            "range": "± 677.062",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.60096,
            "range": "± 419.556",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.6415,
            "range": "± 1.06692",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.0459,
            "range": "± 852.125",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.0634,
            "range": "± 372.472",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.77266,
            "range": "± 506.963",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1986,
            "range": "± 857.926",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.98954,
            "range": "± 477.181",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.70796,
            "range": "± 555.429",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.96833,
            "range": "± 259.687",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 42.5615,
            "range": "± 3.72333",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 42.8283,
            "range": "± 2.88334",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80102,
            "range": "± 14.4539",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.8375,
            "range": "± 18.0754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 104.292,
            "range": "± 4.28428",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 47.6359,
            "range": "± 3.21876",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.87902,
            "range": "± 41.5359",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.17645,
            "range": "± 25.2677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3300.25,
            "range": "± 35789.700000000004",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.50502,
            "range": "± 95.7458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.6198,
            "range": "± 5.08296",
            "unit": "ns",
            "extra": "100 samples\n743 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 270.014,
            "range": "± 2.24248",
            "unit": "ns",
            "extra": "100 samples\n8585 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.14366,
            "range": "± 1.11748",
            "unit": "ns",
            "extra": "100 samples\n7713 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 35.8186,
            "range": "± 3.6284",
            "unit": "ns",
            "extra": "100 samples\n821 iterations"
          },
          {
            "name": "ir_add",
            "value": 740.718,
            "range": "± 32.2143",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.52852,
            "range": "± 94.1119",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.43266,
            "range": "± 654.17",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.59148,
            "range": "± 109.222",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.79487,
            "range": "± 255.346",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.79176,
            "range": "± 253.219",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.92432,
            "range": "± 100.628",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.50133,
            "range": "± 95.9593",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.63166,
            "range": "± 140.066",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.48114,
            "range": "± 166.627",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.50669,
            "range": "± 678.845",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.2001,
            "range": "± 5.34185",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5007,
            "range": "± 1.07067",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 167.688,
            "range": "± 9.07455",
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
          "id": "e36c84d06fd7c5b35b75b2d97b1292c12d1b02ac",
          "message": "Fix trace/IR inconsistency in LazyTraceContext::follow (issue #384) (#388)",
          "timestamp": "2026-07-09T12:57:13+02:00",
          "tree_id": "da60573203cc988ee79b654edfa3b5d02dce7361",
          "url": "https://github.com/nebulastream/nautilus/commit/e36c84d06fd7c5b35b75b2d97b1292c12d1b02ac"
        },
        "date": 1783595445296,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 748.243,
            "range": "± 73.373",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.58814,
            "range": "± 146.272",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.52264,
            "range": "± 681.332",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.70453,
            "range": "± 259.222",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.78137,
            "range": "± 296.809",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.78335,
            "range": "± 161.912",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98326,
            "range": "± 193.687",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.44057,
            "range": "± 131.829",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.76922,
            "range": "± 300.565",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.51772,
            "range": "± 184.371",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.46021,
            "range": "± 980.657",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 92.95,
            "range": "± 5.53436",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.243,
            "range": "± 1.23555",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 161.584,
            "range": "± 17.1316",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.81244,
            "range": "± 134.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.91753,
            "range": "± 358.91",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.92267,
            "range": "± 278.888",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.35815,
            "range": "± 547.703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.1713,
            "range": "± 1.23984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.8683,
            "range": "± 663.667",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.0971,
            "range": "± 1.13466",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.30563,
            "range": "± 167.067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.9599,
            "range": "± 515.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.7497,
            "range": "± 386.481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.9187,
            "range": "± 481.47",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.414,
            "range": "± 353.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.1875,
            "range": "± 317.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2981,
            "range": "± 914.129",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.105,
            "range": "± 8.57502",
            "unit": "ns",
            "extra": "100 samples\n794 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 315.108,
            "range": "± 13.9918",
            "unit": "ns",
            "extra": "100 samples\n8276 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.12339,
            "range": "± 0.795535",
            "unit": "ns",
            "extra": "100 samples\n7715 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.7644,
            "range": "± 2.18529",
            "unit": "ns",
            "extra": "100 samples\n785 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.5198,
            "range": "± 1.23915",
            "unit": "ns",
            "extra": "100 samples\n8452 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.006,
            "range": "± 1.99537",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 605.259,
            "range": "± 33.9663",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.82152,
            "range": "± 0.795191",
            "unit": "ns",
            "extra": "100 samples\n6478 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 50.2503,
            "range": "± 7.78876",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.5795,
            "range": "± 1.04817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.4283,
            "range": "± 5.09126",
            "unit": "ns",
            "extra": "100 samples\n646 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 432.291,
            "range": "± 15.1965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.6013,
            "range": "± 568.776",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.4747,
            "range": "± 4.48998",
            "unit": "ns",
            "extra": "100 samples\n1121 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 148.207,
            "range": "± 16.3568",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.3017,
            "range": "± 235.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.58948,
            "range": "± 0.559893",
            "unit": "ns",
            "extra": "100 samples\n8481 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.8464,
            "range": "± 3.35715",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.81938,
            "range": "± 49.5448",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 48.7551,
            "range": "± 7.09376",
            "unit": "ns",
            "extra": "100 samples\n640 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 48.7808,
            "range": "± 8.5359",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 433.503,
            "range": "± 14.5655",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 393.166,
            "range": "± 19.4345",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 95.5987,
            "range": "± 1.68426",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.851,
            "range": "± 1.02476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 27.3401,
            "range": "± 4.59169",
            "unit": "ns",
            "extra": "100 samples\n1154 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.3757,
            "range": "± 4.11096",
            "unit": "ns",
            "extra": "100 samples\n1154 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 143.659,
            "range": "± 7.54171",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 143.624,
            "range": "± 8.21497",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.2576,
            "range": "± 68.3332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 34.0107,
            "range": "± 138.701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.57307,
            "range": "± 0.453271",
            "unit": "ns",
            "extra": "100 samples\n8478 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.57324,
            "range": "± 0.501989",
            "unit": "ns",
            "extra": "100 samples\n8479 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.1846,
            "range": "± 1.02105",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.4196,
            "range": "± 1.70025",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.81267,
            "range": "± 44.886",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.82496,
            "range": "± 39.9239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 46.6719,
            "range": "± 4.87135",
            "unit": "ns",
            "extra": "100 samples\n652 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.8057,
            "range": "± 5.34806",
            "unit": "ns",
            "extra": "100 samples\n649 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 432.564,
            "range": "± 13.4271",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 431.187,
            "range": "± 11.564",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.6554,
            "range": "± 173.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.6206,
            "range": "± 883.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 48.5186,
            "range": "± 11.3151",
            "unit": "ns",
            "extra": "100 samples\n651 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 43.0164,
            "range": "± 7.50057",
            "unit": "ns",
            "extra": "100 samples\n724 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.0234,
            "range": "± 6.89451",
            "unit": "ns",
            "extra": "100 samples\n773 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 432.926,
            "range": "± 14.7211",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 332.63,
            "range": "± 14.4236",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 435.713,
            "range": "± 11.5666",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 95.6261,
            "range": "± 654.529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 70.1975,
            "range": "± 3.02714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.5755,
            "range": "± 1.7513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 41.2186,
            "range": "± 8.23108",
            "unit": "ns",
            "extra": "100 samples\n773 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 35.2505,
            "range": "± 6.10889",
            "unit": "ns",
            "extra": "100 samples\n893 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 437.002,
            "range": "± 12.0703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 441.3,
            "range": "± 39.6955",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 87.0581,
            "range": "± 3.35648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.9191,
            "range": "± 990.035",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.9267,
            "range": "± 6.88564",
            "unit": "ns",
            "extra": "100 samples\n771 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.0113,
            "range": "± 6.41652",
            "unit": "ns",
            "extra": "100 samples\n773 iterations"
          },
          {
            "name": "ssa_add",
            "value": 190.289,
            "range": "± 24.0295",
            "unit": "ns",
            "extra": "100 samples\n167 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 496.25,
            "range": "± 86.4044",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.22403,
            "range": "± 242.083",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 522.141,
            "range": "± 85.6695",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 950.94,
            "range": "± 159.805",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 512.298,
            "range": "± 69.0917",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 602.761,
            "range": "± 82.111",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 436.855,
            "range": "± 73.8657",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 527.117,
            "range": "± 73.9315",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 469.438,
            "range": "± 61.9669",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3820.75,
            "range": "± 181364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.54673,
            "range": "± 264.946",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.66777,
            "range": "± 421.833",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.66674,
            "range": "± 430.328",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.7294,
            "range": "± 5.51066",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.13651,
            "range": "± 945.862",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.7138,
            "range": "± 3.85725",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.6662,
            "range": "± 2.24451",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.14537,
            "range": "± 1.47636",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.18814,
            "range": "± 765.528",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.3506,
            "range": "± 3.23851",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.15676,
            "range": "± 1.68418",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.43301,
            "range": "± 1.54666",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.16888,
            "range": "± 811.423",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.6084,
            "range": "± 3.9269",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.6963,
            "range": "± 2.6666",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.55285,
            "range": "± 1.4381",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.52388,
            "range": "± 1.34817",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.3382,
            "range": "± 2.45579",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.58874,
            "range": "± 1.25345",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.4947,
            "range": "± 1.53356",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.27467,
            "range": "± 700.257",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 47.6871,
            "range": "± 9.94974",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 46.2873,
            "range": "± 8.18729",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 2.02497,
            "range": "± 63.9363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.95393,
            "range": "± 64.6474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 108.755,
            "range": "± 10.8821",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.6015,
            "range": "± 9.05346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.12609,
            "range": "± 62.768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.34121,
            "range": "± 64.1312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.66493,
            "range": "± 231.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.56805,
            "range": "± 160.642",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.69525,
            "range": "± 209.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 28.5817,
            "range": "± 712.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 77.4168,
            "range": "± 16.4993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.85182,
            "range": "± 247.46",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.8578,
            "range": "± 276.419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 6.00527,
            "range": "± 285.867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.8765,
            "range": "± 675.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 137.619,
            "range": "± 22.0381",
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
          "id": "82fb2699867b63b20141e74ba5cd89b141f50787",
          "message": "Fix tracer dropping Return when a repeated tag assigns a different target (#390)",
          "timestamp": "2026-07-10T08:47:20+02:00",
          "tree_id": "4f04d20c6390a8727cccb87f8264863144d5af51",
          "url": "https://github.com/nebulastream/nautilus/commit/82fb2699867b63b20141e74ba5cd89b141f50787"
        },
        "date": 1783666534059,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 176.932,
            "range": "± 15.0243",
            "unit": "ns",
            "extra": "100 samples\n168 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 445.212,
            "range": "± 46.1463",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.0974,
            "range": "± 82.5486",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 469.649,
            "range": "± 34.2229",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 862.936,
            "range": "± 123.466",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 479.092,
            "range": "± 57.3376",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 577.077,
            "range": "± 66.8387",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 388.429,
            "range": "± 37.5252",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 496.64,
            "range": "± 41.3945",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 444.463,
            "range": "± 35.7022",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.81627,
            "range": "± 288.781",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.23678,
            "range": "± 173.738",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.1839,
            "range": "± 182.464",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.42778,
            "range": "± 330.429",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.0164,
            "range": "± 779.954",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.1943,
            "range": "± 413.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.0414,
            "range": "± 760.069",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.10011,
            "range": "± 131.712",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.9994,
            "range": "± 373.74",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.81166,
            "range": "± 364.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.8619,
            "range": "± 325.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.9364,
            "range": "± 690.5",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.82313,
            "range": "± 284.545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.8615,
            "range": "± 373.238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.0891,
            "range": "± 1.28909",
            "unit": "ns",
            "extra": "100 samples\n8509 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.6824,
            "range": "± 1.49736",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 575.346,
            "range": "± 30.3657",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.8803,
            "range": "± 1.03444",
            "unit": "ns",
            "extra": "100 samples\n6611 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 50.3194,
            "range": "± 8.54567",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.8766,
            "range": "± 1.11927",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.9787,
            "range": "± 6.00974",
            "unit": "ns",
            "extra": "100 samples\n651 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 416.682,
            "range": "± 31.7965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 92.1104,
            "range": "± 1.05409",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 26.9861,
            "range": "± 3.44593",
            "unit": "ns",
            "extra": "100 samples\n1155 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 143.308,
            "range": "± 7.23907",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.8853,
            "range": "± 794.846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56907,
            "range": "± 0.643297",
            "unit": "ns",
            "extra": "100 samples\n8532 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.403,
            "range": "± 2.03653",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.8224,
            "range": "± 52.0688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 46.9348,
            "range": "± 6.47854",
            "unit": "ns",
            "extra": "100 samples\n656 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 46.7939,
            "range": "± 6.59386",
            "unit": "ns",
            "extra": "100 samples\n654 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 416.498,
            "range": "± 26.1534",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 372.555,
            "range": "± 11.3273",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 92.1032,
            "range": "± 1.72355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 78.5211,
            "range": "± 1.09825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 26.4166,
            "range": "± 2.52355",
            "unit": "ns",
            "extra": "100 samples\n1148 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 26.4812,
            "range": "± 2.47872",
            "unit": "ns",
            "extra": "100 samples\n1093 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 144.161,
            "range": "± 7.99194",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 142.926,
            "range": "± 7.36484",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.8563,
            "range": "± 801.897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 34.298,
            "range": "± 696.237",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.53027,
            "range": "± 0.530356",
            "unit": "ns",
            "extra": "100 samples\n8534 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.51767,
            "range": "± 0.441296",
            "unit": "ns",
            "extra": "100 samples\n8521 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.6271,
            "range": "± 3.60155",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.5669,
            "range": "± 2.12316",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.82394,
            "range": "± 124.579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.80829,
            "range": "± 37.1071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.0593,
            "range": "± 10.3702",
            "unit": "ns",
            "extra": "100 samples\n654 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 45.8625,
            "range": "± 3.60727",
            "unit": "ns",
            "extra": "100 samples\n654 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 414.714,
            "range": "± 16.5379",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 412.16,
            "range": "± 11.3568",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 98.5292,
            "range": "± 1.96292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 92.1645,
            "range": "± 672.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.1272,
            "range": "± 6.29866",
            "unit": "ns",
            "extra": "100 samples\n653 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 45.8396,
            "range": "± 0.700939",
            "unit": "ns",
            "extra": "100 samples\n629 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.3235,
            "range": "± 4.35158",
            "unit": "ns",
            "extra": "100 samples\n724 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 414.506,
            "range": "± 24.1117",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 402.89,
            "range": "± 11.7876",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 440.376,
            "range": "± 18.8169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 93.0052,
            "range": "± 5.04147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 90.1329,
            "range": "± 2.36414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 87.8752,
            "range": "± 978.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 41.865,
            "range": "± 4.25438",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.6397,
            "range": "± 3.49756",
            "unit": "ns",
            "extra": "100 samples\n734 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 440.417,
            "range": "± 14.9244",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 438.549,
            "range": "± 13.4611",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 88.0156,
            "range": "± 1.12755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 87.6104,
            "range": "± 210.369",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 42.1763,
            "range": "± 5.9961",
            "unit": "ns",
            "extra": "100 samples\n739 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 42.6106,
            "range": "± 9.11891",
            "unit": "ns",
            "extra": "100 samples\n740 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 43.0194,
            "range": "± 5.24286",
            "unit": "ns",
            "extra": "100 samples\n708 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 297.668,
            "range": "± 11.3804",
            "unit": "ns",
            "extra": "100 samples\n8289 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.92746,
            "range": "± 0.291826",
            "unit": "ns",
            "extra": "100 samples\n7794 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.3278,
            "range": "± 2.13151",
            "unit": "ns",
            "extra": "100 samples\n745 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3406.0899999999997,
            "range": "± 102338",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.73569,
            "range": "± 169.337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.40191,
            "range": "± 107.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.31811,
            "range": "± 102.918",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.40857,
            "range": "± 128.619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.2292,
            "range": "± 995.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 75.4549,
            "range": "± 13.6871",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.62886,
            "range": "± 657.458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.48231,
            "range": "± 127.496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.49151,
            "range": "± 156.987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.4272,
            "range": "± 640.501",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 137.886,
            "range": "± 19.3456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.43874,
            "range": "± 201.906",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.40176,
            "range": "± 207.523",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.07879,
            "range": "± 1.21039",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.64144,
            "range": "± 466.796",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.1354,
            "range": "± 2.9761",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.8937,
            "range": "± 1.46014",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.89516,
            "range": "± 1.42578",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.79504,
            "range": "± 499.492",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.6284,
            "range": "± 2.73253",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.57876,
            "range": "± 1.08184",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.2492,
            "range": "± 1.63747",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.89963,
            "range": "± 533.778",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.0069,
            "range": "± 2.95846",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.359,
            "range": "± 1.52045",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.93877,
            "range": "± 800.54",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.95065,
            "range": "± 1.00576",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.5456,
            "range": "± 1.95715",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.14505,
            "range": "± 707.134",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.18018,
            "range": "± 1.16695",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.03747,
            "range": "± 501.388",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 44.9556,
            "range": "± 5.86001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 45.833,
            "range": "± 5.97469",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.97261,
            "range": "± 41.3034",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.0151,
            "range": "± 46.9436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 106.78,
            "range": "± 9.06965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.6966,
            "range": "± 6.44237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.04658,
            "range": "± 32.4822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.40643,
            "range": "± 29.1104",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 724.919,
            "range": "± 59.5063",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.53201,
            "range": "± 113.124",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.29891,
            "range": "± 255.637",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.60113,
            "range": "± 110.008",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.7779,
            "range": "± 168.035",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.7725,
            "range": "± 143.418",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.95815,
            "range": "± 109.49",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.39177,
            "range": "± 94.7037",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.72703,
            "range": "± 147.227",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.47955,
            "range": "± 134.735",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.62961,
            "range": "± 777.825",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 86.906,
            "range": "± 6.64684",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5424,
            "range": "± 1.02617",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 164.418,
            "range": "± 10.5542",
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
          "id": "c09b79bc5c261b11391ade6645269483e8be9b23",
          "message": "Skip no-op block argument prep in C++ backend (#394)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-07-11T08:25:52+02:00",
          "tree_id": "5aa9c447632b4bd646feb4d026ad893b8b5011f2",
          "url": "https://github.com/nebulastream/nautilus/commit/c09b79bc5c261b11391ade6645269483e8be9b23"
        },
        "date": 1783751592356,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 187.561,
            "range": "± 18.6427",
            "unit": "ns",
            "extra": "100 samples\n171 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 453.324,
            "range": "± 40.7916",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11579,
            "range": "± 119.211",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 501.91,
            "range": "± 111.095",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 859.989,
            "range": "± 61.2846",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 498.968,
            "range": "± 46.5808",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 598.014,
            "range": "± 55.6541",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 411.502,
            "range": "± 31.5452",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 501.86,
            "range": "± 42.7938",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 453.204,
            "range": "± 52.8308",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3408.15,
            "range": "± 112846",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.60554,
            "range": "± 155.421",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.6819,
            "range": "± 1.39873",
            "unit": "ns",
            "extra": "100 samples\n8714 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.8233,
            "range": "± 1.84447",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 587.18,
            "range": "± 32.3454",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.76776,
            "range": "± 0.533983",
            "unit": "ns",
            "extra": "100 samples\n6790 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 49.2349,
            "range": "± 6.73752",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.6139,
            "range": "± 1.13215",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.5655,
            "range": "± 4.71999",
            "unit": "ns",
            "extra": "100 samples\n674 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 432.118,
            "range": "± 12.7696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.6427,
            "range": "± 771.627",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 26.987,
            "range": "± 3.15199",
            "unit": "ns",
            "extra": "100 samples\n1108 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 142.321,
            "range": "± 6.88577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5133,
            "range": "± 88.2632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.48253,
            "range": "± 0.366246",
            "unit": "ns",
            "extra": "100 samples\n8594 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.7163,
            "range": "± 2.84884",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.81549,
            "range": "± 67.9379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 46.4387,
            "range": "± 5.88624",
            "unit": "ns",
            "extra": "100 samples\n668 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 46.6337,
            "range": "± 5.63974",
            "unit": "ns",
            "extra": "100 samples\n669 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 431.012,
            "range": "± 10.4921",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 390.069,
            "range": "± 16.2709",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 95.4333,
            "range": "± 157.076",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.6438,
            "range": "± 95.5517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 26.6874,
            "range": "± 2.63116",
            "unit": "ns",
            "extra": "100 samples\n1142 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.5011,
            "range": "± 3.63596",
            "unit": "ns",
            "extra": "100 samples\n1180 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 143.165,
            "range": "± 7.8211",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 142.997,
            "range": "± 6.94695",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.5228,
            "range": "± 63.1242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.971,
            "range": "± 176.734",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.49831,
            "range": "± 0.388058",
            "unit": "ns",
            "extra": "100 samples\n8742 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.54397,
            "range": "± 0.463242",
            "unit": "ns",
            "extra": "100 samples\n8747 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.4214,
            "range": "± 1.66437",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.0952,
            "range": "± 815.043",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.81455,
            "range": "± 24.6192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.80224,
            "range": "± 22.6856",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.0475,
            "range": "± 7.56764",
            "unit": "ns",
            "extra": "100 samples\n673 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.149,
            "range": "± 4.04995",
            "unit": "ns",
            "extra": "100 samples\n668 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 432.396,
            "range": "± 11.8986",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 432.449,
            "range": "± 13.3509",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.4629,
            "range": "± 245.892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.3771,
            "range": "± 494.909",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 46.567,
            "range": "± 5.54434",
            "unit": "ns",
            "extra": "100 samples\n669 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 41.3926,
            "range": "± 4.23054",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.8935,
            "range": "± 6.44023",
            "unit": "ns",
            "extra": "100 samples\n805 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 431.113,
            "range": "± 11.0595",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 332.014,
            "range": "± 9.86769",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 438.637,
            "range": "± 10.9572",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 95.3637,
            "range": "± 150.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 67.7239,
            "range": "± 2.15734",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.5178,
            "range": "± 710.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.4759,
            "range": "± 4.75548",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.6864,
            "range": "± 3.74675",
            "unit": "ns",
            "extra": "100 samples\n917 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 438.957,
            "range": "± 12.4592",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 438.838,
            "range": "± 11.2154",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 86.521,
            "range": "± 470.783",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.3725,
            "range": "± 523.973",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.0042,
            "range": "± 7.7202",
            "unit": "ns",
            "extra": "100 samples\n802 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 38.9563,
            "range": "± 3.52786",
            "unit": "ns",
            "extra": "100 samples\n802 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.65618,
            "range": "± 231.962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.43187,
            "range": "± 227.296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.30126,
            "range": "± 201.276",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.64637,
            "range": "± 363.66",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.1085,
            "range": "± 874.866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.2741,
            "range": "± 489.011",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.6437,
            "range": "± 627.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.02511,
            "range": "± 140.803",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.3324,
            "range": "± 571.23",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.92803,
            "range": "± 303.508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.7095,
            "range": "± 204.879",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.586,
            "range": "± 489.591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.62625,
            "range": "± 199.061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.8666,
            "range": "± 439.135",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.7219,
            "range": "± 654.372",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.313,
            "range": "± 1.09227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.4038,
            "range": "± 585.057",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.9866,
            "range": "± 572.753",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.6014,
            "range": "± 1.50125",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.7357,
            "range": "± 920.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.6561,
            "range": "± 692.924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.415,
            "range": "± 662.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.7256,
            "range": "± 468.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.8934,
            "range": "± 398.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.1888,
            "range": "± 703.11",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 53.5487,
            "range": "± 864.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 28.697,
            "range": "± 656.171",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 58.5236,
            "range": "± 1.30983",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.2253,
            "range": "± 2.3477",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.2331,
            "range": "± 3.42153",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.4193,
            "range": "± 4.51699",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.4043,
            "range": "± 3.46753",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 25.2308,
            "range": "± 5.01192",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.494,
            "range": "± 3.76104",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.0236,
            "range": "± 4.0031",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.8934,
            "range": "± 3.87904",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 21.2409,
            "range": "± 4.29992",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 20.1625,
            "range": "± 4.23202",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 49.4465,
            "range": "± 7.8924",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 272.508,
            "range": "± 10.2274",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 64.3958,
            "range": "± 8.8905",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 444.472,
            "range": "± 16.6502",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.90203,
            "range": "± 165.432",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.11024,
            "range": "± 364.396",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.41954,
            "range": "± 1.00386",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.20539,
            "range": "± 284.784",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.35922,
            "range": "± 940.403",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.42563,
            "range": "± 393.502",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.5009,
            "range": "± 466.27",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.51782,
            "range": "± 376.877",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.62891,
            "range": "± 580.639",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.79779,
            "range": "± 364.596",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.237,
            "range": "± 3.02759",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 269.481,
            "range": "± 9.3425",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 33.7404,
            "range": "± 3.35796",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 398.393,
            "range": "± 16.0394",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 24.8203,
            "range": "± 5.29972",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.3687,
            "range": "± 5.52414",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 51.5486,
            "range": "± 13.6798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.92,
            "range": "± 5.30811",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.2439,
            "range": "± 9.17291",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 37.3175,
            "range": "± 6.11285",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 38.2988,
            "range": "± 5.35749",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 30.316,
            "range": "± 4.88707",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 35.6718,
            "range": "± 7.00096",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 33.597,
            "range": "± 4.71356",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 79.4687,
            "range": "± 12.2171",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 496.841,
            "range": "± 21.1121",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 95.0332,
            "range": "± 11.2951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 636.014,
            "range": "± 19.7759",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.6811,
            "range": "± 5.80606",
            "unit": "ns",
            "extra": "100 samples\n756 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.958,
            "range": "± 6.43566",
            "unit": "ns",
            "extra": "100 samples\n8536 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.03401,
            "range": "± 0.739845",
            "unit": "ns",
            "extra": "100 samples\n8013 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3377,
            "range": "± 1.83669",
            "unit": "ns",
            "extra": "100 samples\n782 iterations"
          },
          {
            "name": "ir_add",
            "value": 794.934,
            "range": "± 46.3606",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.63348,
            "range": "± 114.165",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.48113,
            "range": "± 414.87",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.71589,
            "range": "± 153.529",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.9738,
            "range": "± 309.573",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.92769,
            "range": "± 254.512",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.1313,
            "range": "± 277.775",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45134,
            "range": "± 120.715",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.85883,
            "range": "± 185.944",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53676,
            "range": "± 116.928",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.72152,
            "range": "± 606.434",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.2424,
            "range": "± 6.36617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.6452,
            "range": "± 828.889",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 165.242,
            "range": "± 9.56224",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.47522,
            "range": "± 255.057",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.35,
            "range": "± 206.492",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.03003,
            "range": "± 1.3322",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.78891,
            "range": "± 490.732",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.8393,
            "range": "± 3.08202",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.3382,
            "range": "± 2.06966",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.89502,
            "range": "± 1.0972",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.77518,
            "range": "± 477.832",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.8516,
            "range": "± 2.79598",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.52896,
            "range": "± 1.04563",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.13044,
            "range": "± 1.31266",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.99249,
            "range": "± 534.124",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.6791,
            "range": "± 2.01417",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.3823,
            "range": "± 1.4835",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.7207,
            "range": "± 782.16",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.88802,
            "range": "± 884.666",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.3939,
            "range": "± 1.57001",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.30888,
            "range": "± 929.534",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.20513,
            "range": "± 1.09603",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.0543,
            "range": "± 409.658",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 45.0942,
            "range": "± 6.58965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 46.5442,
            "range": "± 6.85501",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.94637,
            "range": "± 36.1216",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.00458,
            "range": "± 32.3616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 105.221,
            "range": "± 8.40981",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.9383,
            "range": "± 8.14667",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.05983,
            "range": "± 42.1173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.39421,
            "range": "± 41.2739",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.47247,
            "range": "± 132.877",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.38833,
            "range": "± 125.658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.30667,
            "range": "± 110.369",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.0041,
            "range": "± 607.641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 75.6008,
            "range": "± 14.747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.56945,
            "range": "± 181.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.45563,
            "range": "± 152.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.43117,
            "range": "± 121.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.2478,
            "range": "± 426.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 133.863,
            "range": "± 16.9365",
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
          "id": "57e85856236a157df8a8f4046d497a32a9bcc640",
          "message": "Fix issue #381: evalNautilusCall's argument loop needs static_val, not plain int (#392)",
          "timestamp": "2026-07-11T18:01:18+02:00",
          "tree_id": "34c84aa23903dd58540ffcf85702606f81729461",
          "url": "https://github.com/nebulastream/nautilus/commit/57e85856236a157df8a8f4046d497a32a9bcc640"
        },
        "date": 1783786132921,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3456.46,
            "range": "± 220256",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.68644,
            "range": "± 175.385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 41.2382,
            "range": "± 4.14299",
            "unit": "ns",
            "extra": "100 samples\n730 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 309.518,
            "range": "± 32.9033",
            "unit": "ns",
            "extra": "100 samples\n8255 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.05183,
            "range": "± 0.681424",
            "unit": "ns",
            "extra": "100 samples\n7415 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.4172,
            "range": "± 2.24249",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "ir_add",
            "value": 800.179,
            "range": "± 63.0847",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.64244,
            "range": "± 124.198",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.42801,
            "range": "± 258.335",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.77591,
            "range": "± 261.701",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.99854,
            "range": "± 320.344",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.97267,
            "range": "± 295.523",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.09606,
            "range": "± 222.109",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.51781,
            "range": "± 248.926",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.85757,
            "range": "± 186.618",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.59126,
            "range": "± 280.899",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.93997,
            "range": "± 768.651",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.4148,
            "range": "± 3.75679",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.1461,
            "range": "± 1.10628",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 171.592,
            "range": "± 13.1834",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.0557,
            "range": "± 1.40956",
            "unit": "ns",
            "extra": "100 samples\n8444 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.8369,
            "range": "± 3.98961",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 605.374,
            "range": "± 56.6621",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.03774,
            "range": "± 1.25159",
            "unit": "ns",
            "extra": "100 samples\n6518 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 49.4957,
            "range": "± 7.13745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.4328,
            "range": "± 75.9807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 51.608,
            "range": "± 19.841",
            "unit": "ns",
            "extra": "100 samples\n650 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 432.957,
            "range": "± 13.3195",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.3998,
            "range": "± 324.196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 28.0294,
            "range": "± 4.45194",
            "unit": "ns",
            "extra": "100 samples\n1117 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 143.142,
            "range": "± 8.17062",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.6451,
            "range": "± 242.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.61065,
            "range": "± 0.567519",
            "unit": "ns",
            "extra": "100 samples\n8450 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.7109,
            "range": "± 3.50444",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.8107,
            "range": "± 35.4744",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 49.769,
            "range": "± 12.9652",
            "unit": "ns",
            "extra": "100 samples\n647 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 54.2062,
            "range": "± 26.768",
            "unit": "ns",
            "extra": "100 samples\n650 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 430.978,
            "range": "± 9.9409",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 389.75,
            "range": "± 11.2048",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 95.4443,
            "range": "± 306.724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 80.0464,
            "range": "± 1.55427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 27.6872,
            "range": "± 3.52582",
            "unit": "ns",
            "extra": "100 samples\n1136 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.8079,
            "range": "± 4.07699",
            "unit": "ns",
            "extra": "100 samples\n1117 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 142.754,
            "range": "± 6.42009",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 147.259,
            "range": "± 12.6545",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 37.0381,
            "range": "± 1.12525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 34.4965,
            "range": "± 1.03922",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.53454,
            "range": "± 0.424915",
            "unit": "ns",
            "extra": "100 samples\n8454 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.55599,
            "range": "± 0.505255",
            "unit": "ns",
            "extra": "100 samples\n8434 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.4851,
            "range": "± 1.84471",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.4741,
            "range": "± 1.60525",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.84525,
            "range": "± 129.165",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.80794,
            "range": "± 44.4851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 48.6562,
            "range": "± 9.94503",
            "unit": "ns",
            "extra": "100 samples\n652 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 49.7544,
            "range": "± 11.6185",
            "unit": "ns",
            "extra": "100 samples\n650 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 434.605,
            "range": "± 14.9837",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 435.496,
            "range": "± 21.3106",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 95.539,
            "range": "± 346.951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 95.656,
            "range": "± 1.75854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 50.4321,
            "range": "± 14.0989",
            "unit": "ns",
            "extra": "100 samples\n647 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 46.3651,
            "range": "± 14.2695",
            "unit": "ns",
            "extra": "100 samples\n704 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 43.9361,
            "range": "± 12.2427",
            "unit": "ns",
            "extra": "100 samples\n778 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 435.852,
            "range": "± 23.3171",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 341.778,
            "range": "± 24.7643",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 436.671,
            "range": "± 10.6617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 95.3087,
            "range": "± 1.02482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 78.9231,
            "range": "± 4.83781",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 86.8674,
            "range": "± 3.57788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 44.4622,
            "range": "± 14.8683",
            "unit": "ns",
            "extra": "100 samples\n775 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 37.7233,
            "range": "± 10.1076",
            "unit": "ns",
            "extra": "100 samples\n880 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 437.594,
            "range": "± 12.4193",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 437.654,
            "range": "± 12.2593",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 86.5764,
            "range": "± 857.355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 86.5631,
            "range": "± 728.279",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 43.339,
            "range": "± 14.3218",
            "unit": "ns",
            "extra": "100 samples\n776 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 43.2167,
            "range": "± 13.1311",
            "unit": "ns",
            "extra": "100 samples\n778 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.38869,
            "range": "± 147.902",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.33063,
            "range": "± 172.924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.3162,
            "range": "± 140.71",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.257,
            "range": "± 594.395",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 75.7619,
            "range": "± 15.6831",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.54054,
            "range": "± 173.866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.38694,
            "range": "± 136.019",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.39259,
            "range": "± 134.34",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.8957,
            "range": "± 858.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 135.832,
            "range": "± 20.1429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.64262,
            "range": "± 436.794",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.43034,
            "range": "± 249.32",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.1715,
            "range": "± 1.38032",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.91708,
            "range": "± 616.378",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.2739,
            "range": "± 3.21557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.387,
            "range": "± 1.84215",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.12212,
            "range": "± 1.62307",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.06542,
            "range": "± 721.07",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.5774,
            "range": "± 2.78682",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.65444,
            "range": "± 1.2119",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.21272,
            "range": "± 1.64539",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.87127,
            "range": "± 539.877",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.9751,
            "range": "± 3.21305",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2085,
            "range": "± 1.61247",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.79006,
            "range": "± 979.652",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.20436,
            "range": "± 1.10015",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.6515,
            "range": "± 1.80422",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.26744,
            "range": "± 876.44",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.21963,
            "range": "± 1.18849",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.26185,
            "range": "± 610.605",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 46.7829,
            "range": "± 8.45157",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 46.4003,
            "range": "± 6.51132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.95501,
            "range": "± 57.3054",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.00154,
            "range": "± 47.6321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 105.365,
            "range": "± 8.65087",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.675,
            "range": "± 7.74622",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.03528,
            "range": "± 56.0521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.37795,
            "range": "± 50.9887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 183.83,
            "range": "± 16.6683",
            "unit": "ns",
            "extra": "100 samples\n165 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 467.842,
            "range": "± 68.7583",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.13372,
            "range": "± 117.974",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 468.901,
            "range": "± 28.3934",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 870.088,
            "range": "± 59.6988",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 470.812,
            "range": "± 32.719",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 578.805,
            "range": "± 32.2757",
            "unit": "ns",
            "extra": "100 samples\n51 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 412.877,
            "range": "± 33.2236",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 492.827,
            "range": "± 37.4911",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 443.925,
            "range": "± 35.7483",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.99332,
            "range": "± 491.66",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.38908,
            "range": "± 601.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.3951,
            "range": "± 527.83",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.55451,
            "range": "± 572.693",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2577,
            "range": "± 714.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.409,
            "range": "± 670.246",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.4484,
            "range": "± 790.147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.34851,
            "range": "± 608.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.4544,
            "range": "± 795.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.9064,
            "range": "± 544.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.503,
            "range": "± 786.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.2399,
            "range": "± 671.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3095,
            "range": "± 620.769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.0591,
            "range": "± 551.846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.7938,
            "range": "± 957.905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.2635,
            "range": "± 997.476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.6892,
            "range": "± 449.216",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.7853,
            "range": "± 227.078",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.1871,
            "range": "± 540.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.2963,
            "range": "± 227.416",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.1928,
            "range": "± 481.373",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.7106,
            "range": "± 226.511",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.1174,
            "range": "± 226.305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.0979,
            "range": "± 254.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.5322,
            "range": "± 178.994",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 55.1474,
            "range": "± 1.2752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.0046,
            "range": "± 531.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 59.2427,
            "range": "± 384.59",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.5138,
            "range": "± 2.67823",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.626,
            "range": "± 3.64135",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.6626,
            "range": "± 4.57103",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.5306,
            "range": "± 4.62572",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 24.6636,
            "range": "± 4.17769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.6884,
            "range": "± 3.63588",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.1699,
            "range": "± 3.82171",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.9414,
            "range": "± 3.85837",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 21.6434,
            "range": "± 5.3148",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 20.5197,
            "range": "± 6.34281",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 48.3411,
            "range": "± 7.22903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 284.134,
            "range": "± 22.1062",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 63.6357,
            "range": "± 8.32889",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 468.737,
            "range": "± 15.0929",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.84687,
            "range": "± 108.73",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.13604,
            "range": "± 308.723",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.57435,
            "range": "± 1.47032",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.29019,
            "range": "± 631.173",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.32805,
            "range": "± 840.167",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.44475,
            "range": "± 429.528",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.37766,
            "range": "± 473.795",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.61368,
            "range": "± 388.737",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.67836,
            "range": "± 509.087",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.85544,
            "range": "± 372.962",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.9243,
            "range": "± 4.25127",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 273.569,
            "range": "± 10.3499",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 33.1793,
            "range": "± 3.55761",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 399.195,
            "range": "± 12.933",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.707,
            "range": "± 5.55472",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.0951,
            "range": "± 5.47173",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 50.7312,
            "range": "± 10.1235",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.7064,
            "range": "± 4.90803",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.8386,
            "range": "± 8.93617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 43.2145,
            "range": "± 10.9832",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 45.4758,
            "range": "± 8.99895",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.6573,
            "range": "± 4.66609",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.7377,
            "range": "± 6.71867",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.5033,
            "range": "± 7.38019",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 75.6875,
            "range": "± 9.3828",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 542.237,
            "range": "± 143.719",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 98.3665,
            "range": "± 18.608",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 639.703,
            "range": "± 20.2426",
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
          "id": "15d8286fe1c0c053343d0283bc9687929f6d29ac",
          "message": "[tbc] Copy-and-patch JIT execution mode (tbc.mode=jit) (#378)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-07-14T22:58:10+02:00",
          "tree_id": "80c24d45f1cfa350037ecc3e3a80b6b991fb0d5a",
          "url": "https://github.com/nebulastream/nautilus/commit/15d8286fe1c0c053343d0283bc9687929f6d29ac"
        },
        "date": 1784063606209,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 953.482,
            "range": "± 172.922",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.99287,
            "range": "± 588.775",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.98684,
            "range": "± 1.0069",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.23964,
            "range": "± 537.526",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.61333,
            "range": "± 963.34",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.48056,
            "range": "± 625.135",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.19,
            "range": "± 331.903",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.16406,
            "range": "± 400.311",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.54834,
            "range": "± 508.7",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.73318,
            "range": "± 435.617",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.73798,
            "range": "± 2.27089",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 98.7813,
            "range": "± 9.72234",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 13.5602,
            "range": "± 2.97844",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 167.704,
            "range": "± 17.7661",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.73435,
            "range": "± 252.332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.51224,
            "range": "± 247.741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.47959,
            "range": "± 270.919",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 56.1928,
            "range": "± 13.5346",
            "unit": "ns",
            "extra": "100 samples\n744 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 331.5,
            "range": "± 11.6556",
            "unit": "ns",
            "extra": "100 samples\n8168 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 5.43939,
            "range": "± 1.21141",
            "unit": "ns",
            "extra": "100 samples\n7690 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 51.1662,
            "range": "± 9.63193",
            "unit": "ns",
            "extra": "100 samples\n739 iterations"
          },
          {
            "name": "trace_add",
            "value": 3.82151,
            "range": "± 700.19",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 3.5965,
            "range": "± 741.07",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.3087,
            "range": "± 2.74762",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 7.29592,
            "range": "± 1.28302",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.0023,
            "range": "± 7.95209",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 19.4258,
            "range": "± 3.98081",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.2303,
            "range": "± 2.30823",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 6.97141,
            "range": "± 1.33993",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.7106,
            "range": "± 5.17687",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 12.3956,
            "range": "± 2.49794",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.567,
            "range": "± 2.74059",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 6.55171,
            "range": "± 1.47167",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 19.6787,
            "range": "± 6.08244",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 14.8336,
            "range": "± 3.42047",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.7925,
            "range": "± 2.23154",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 11.0747,
            "range": "± 2.07558",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 14.1998,
            "range": "± 3.47067",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 9.17402,
            "range": "± 1.8285",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.3968,
            "range": "± 2.66044",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 6.1137,
            "range": "± 1.19334",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 69.5752,
            "range": "± 15.854",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 62.9243,
            "range": "± 12.6442",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 2.46732,
            "range": "± 181.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.10385,
            "range": "± 206.112",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 118.662,
            "range": "± 19.7498",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 67.7192,
            "range": "± 18.1959",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.63886,
            "range": "± 300.561",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.50887,
            "range": "± 186.352",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3647.5,
            "range": "± 212251",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.37394,
            "range": "± 299.81",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 12.2741,
            "range": "± 1.86458",
            "unit": "ns",
            "extra": "100 samples\n8414 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 19.3109,
            "range": "± 3.09933",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 606.883,
            "range": "± 24.2837",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 6.50009,
            "range": "± 1.51639",
            "unit": "ns",
            "extra": "100 samples\n6524 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 51.6654,
            "range": "± 7.11851",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.7215,
            "range": "± 238.367",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 66.0013,
            "range": "± 14.7408",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 445.876,
            "range": "± 29.5903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 96.7427,
            "range": "± 1.04745",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 38.8675,
            "range": "± 8.35499",
            "unit": "ns",
            "extra": "100 samples\n1107 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 145.39,
            "range": "± 11.0718",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.7909,
            "range": "± 329.122",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 4.31765,
            "range": "± 0.952263",
            "unit": "ns",
            "extra": "100 samples\n8376 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 15.8853,
            "range": "± 2.74874",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.11154,
            "range": "± 300.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 63.4828,
            "range": "± 13.7551",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 65.6955,
            "range": "± 12.5891",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 444.33,
            "range": "± 46.0674",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 405.403,
            "range": "± 45.528",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 96.3159,
            "range": "± 887.015",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.0925,
            "range": "± 1.23581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 38.7324,
            "range": "± 8.41232",
            "unit": "ns",
            "extra": "100 samples\n1090 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 40.8541,
            "range": "± 7.73348",
            "unit": "ns",
            "extra": "100 samples\n1102 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 144.768,
            "range": "± 10.6403",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 144.729,
            "range": "± 11.1878",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.7382,
            "range": "± 256.87",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 34.1868,
            "range": "± 243.385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 4.16268,
            "range": "± 0.868263",
            "unit": "ns",
            "extra": "100 samples\n8470 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 4.26302,
            "range": "± 0.893631",
            "unit": "ns",
            "extra": "100 samples\n8435 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 16.2878,
            "range": "± 3.31987",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 16.1049,
            "range": "± 2.72819",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 3.09123,
            "range": "± 350.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 3.09956,
            "range": "± 374.155",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 64.9796,
            "range": "± 12.3101",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 66.5212,
            "range": "± 14.3839",
            "unit": "ns",
            "extra": "100 samples\n634 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 444.038,
            "range": "± 41.6562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 454.985,
            "range": "± 38.7697",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 96.839,
            "range": "± 1.13443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 97.0714,
            "range": "± 1.02696",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 62.8661,
            "range": "± 16.3417",
            "unit": "ns",
            "extra": "100 samples\n634 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 61.9497,
            "range": "± 13.6949",
            "unit": "ns",
            "extra": "100 samples\n647 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 57.5087,
            "range": "± 13.8967",
            "unit": "ns",
            "extra": "100 samples\n716 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 448.365,
            "range": "± 50.8711",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 407.58,
            "range": "± 47.6067",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 455.035,
            "range": "± 15.7199",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 97.3987,
            "range": "± 9.243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 92.5574,
            "range": "± 11.4172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 87.4714,
            "range": "± 341.966",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 58.4138,
            "range": "± 14.353",
            "unit": "ns",
            "extra": "100 samples\n710 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 48.4056,
            "range": "± 9.69617",
            "unit": "ns",
            "extra": "100 samples\n927 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 449.045,
            "range": "± 17.5501",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 451.092,
            "range": "± 20.342",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 87.5593,
            "range": "± 814.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 87.4849,
            "range": "± 275.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 59.9914,
            "range": "± 11.52",
            "unit": "ns",
            "extra": "100 samples\n720 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 56.9183,
            "range": "± 10.0123",
            "unit": "ns",
            "extra": "100 samples\n736 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.78023,
            "range": "± 355.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.04872,
            "range": "± 397.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.70407,
            "range": "± 388.637",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.18717,
            "range": "± 377.663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.346,
            "range": "± 327.251",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.6162,
            "range": "± 657.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2567,
            "range": "± 709.761",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.35532,
            "range": "± 252.284",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.8157,
            "range": "± 437.16",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.1637,
            "range": "± 313.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.9196,
            "range": "± 387.745",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5704,
            "range": "± 570.607",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.9031,
            "range": "± 495.219",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.6701,
            "range": "± 404.568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.407,
            "range": "± 675.919",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.9562,
            "range": "± 610.909",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.9176,
            "range": "± 618.581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.7345,
            "range": "± 806.526",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.1447,
            "range": "± 521.929",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.1267,
            "range": "± 729.429",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.2797,
            "range": "± 703.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.9812,
            "range": "± 674.451",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.9654,
            "range": "± 799.357",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.9027,
            "range": "± 719.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.6872,
            "range": "± 643.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 56.106,
            "range": "± 1.15422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 29.8068,
            "range": "± 1.08393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 60.5496,
            "range": "± 972.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 17.3458,
            "range": "± 3.68086",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 21.4046,
            "range": "± 3.92389",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 27.0946,
            "range": "± 4.52967",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 22.4391,
            "range": "± 3.96261",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 25.7951,
            "range": "± 4.12329",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 22.9274,
            "range": "± 5.15774",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 24.132,
            "range": "± 4.82831",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 20.0666,
            "range": "± 3.83513",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 24.1432,
            "range": "± 6.31596",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 21.0164,
            "range": "± 3.87911",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 51.0067,
            "range": "± 12.2153",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 283.828,
            "range": "± 15.4088",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 71.4736,
            "range": "± 16.3587",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 468.496,
            "range": "± 24.16",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 2.5099,
            "range": "± 555.747",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.7664,
            "range": "± 921.09",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 10.3845,
            "range": "± 2.41359",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 5.29407,
            "range": "± 1.26893",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 8.67493,
            "range": "± 2.31165",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 5.66191,
            "range": "± 1.20281",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.81812,
            "range": "± 830.219",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 6.0704,
            "range": "± 1.43761",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 5.88885,
            "range": "± 1.54545",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 4.57403,
            "range": "± 1.21873",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.9305,
            "range": "± 4.74139",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 279.783,
            "range": "± 15.0568",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 37.2041,
            "range": "± 8.31885",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 412.06,
            "range": "± 23.7445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 26.3694,
            "range": "± 5.86651",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.8958,
            "range": "± 6.56155",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 55.0873,
            "range": "± 10.5758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.4945,
            "range": "± 4.61838",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 53.3692,
            "range": "± 13.6794",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.5818,
            "range": "± 9.40818",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.6444,
            "range": "± 11.9579",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 32.6602,
            "range": "± 6.8584",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 38.1365,
            "range": "± 7.38136",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.2773,
            "range": "± 6.94975",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 82.9894,
            "range": "± 14.6",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 514.564,
            "range": "± 27.0355",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 97.7693,
            "range": "± 13.7997",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 701.072,
            "range": "± 60.0719",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 206.466,
            "range": "± 41.2689",
            "unit": "ns",
            "extra": "100 samples\n162 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 665.799,
            "range": "± 125.333",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.68077,
            "range": "± 421.609",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 722.469,
            "range": "± 149.423",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1120.42,
            "range": "± 280259",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 682.505,
            "range": "± 250.704",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 745.36,
            "range": "± 138.973",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 706.926,
            "range": "± 116.865",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 764.676,
            "range": "± 133.441",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 604.492,
            "range": "± 155.34",
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
          "id": "ff1a594f9e695f019be85c09343c05c7506f535f",
          "message": "[fuzzer] Seed corpus + libFuzzer dictionary of boundary values (#395)",
          "timestamp": "2026-07-15T08:27:07+02:00",
          "tree_id": "fbeaed4181b737b241945d7ab97be224230da0f2",
          "url": "https://github.com/nebulastream/nautilus/commit/ff1a594f9e695f019be85c09343c05c7506f535f"
        },
        "date": 1784097282594,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_twotier_addOne",
            "value": 3.39057,
            "range": "± 100.66",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.28958,
            "range": "± 156.622",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.51834,
            "range": "± 438.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.4329,
            "range": "± 930.689",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 75.3317,
            "range": "± 12.3955",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.59179,
            "range": "± 471.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.37453,
            "range": "± 143.876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.43021,
            "range": "± 328.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.7717,
            "range": "± 855.013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 135.199,
            "range": "± 16.0887",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.65362,
            "range": "± 288.586",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.25076,
            "range": "± 259.275",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.05429,
            "range": "± 109.026",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.24499,
            "range": "± 286.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0448,
            "range": "± 675.252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.1061,
            "range": "± 657.521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.9455,
            "range": "± 1.2968",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.41408,
            "range": "± 458.705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.6169,
            "range": "± 707.747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.4907,
            "range": "± 640.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.5769,
            "range": "± 587.478",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.1619,
            "range": "± 758.348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.6607,
            "range": "± 356.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.6913,
            "range": "± 302.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.1068,
            "range": "± 367.501",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.2903,
            "range": "± 481.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.7465,
            "range": "± 603.534",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.4919,
            "range": "± 240.113",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.7973,
            "range": "± 443.574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.5998,
            "range": "± 394.296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.719,
            "range": "± 388.78",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.457,
            "range": "± 404.28",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.976,
            "range": "± 373.597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.4039,
            "range": "± 414.42",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.842,
            "range": "± 389.759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 54.1775,
            "range": "± 1.26614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 29.0364,
            "range": "± 426.63",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 58.441,
            "range": "± 710.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.6695,
            "range": "± 2.40196",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 21.5132,
            "range": "± 4.30439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 26.8283,
            "range": "± 5.02013",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 21.2959,
            "range": "± 3.81471",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 25.2625,
            "range": "± 5.2661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 21.9035,
            "range": "± 4.35291",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.544,
            "range": "± 4.06099",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.9736,
            "range": "± 3.3448",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 21.2702,
            "range": "± 3.34001",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 20.7192,
            "range": "± 4.02153",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 49.3225,
            "range": "± 6.98501",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 290.641,
            "range": "± 16.4577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 65.772,
            "range": "± 8.73652",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 467.422,
            "range": "± 17.332",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.95295,
            "range": "± 171.414",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.33501,
            "range": "± 507.686",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.74203,
            "range": "± 1.15353",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.36825,
            "range": "± 310.415",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.55551,
            "range": "± 796.364",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.74907,
            "range": "± 679.034",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.80187,
            "range": "± 810.666",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.75203,
            "range": "± 496.275",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.78969,
            "range": "± 402.638",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.96208,
            "range": "± 439.736",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 26.5316,
            "range": "± 2.77816",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 282.629,
            "range": "± 9.56001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 55.8333,
            "range": "± 23.3455",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 416.699,
            "range": "± 16.2992",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 24.5355,
            "range": "± 4.93792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.4951,
            "range": "± 5.73338",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 51.3358,
            "range": "± 10.555",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.3155,
            "range": "± 10.5242",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.7711,
            "range": "± 9.21833",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 37.9256,
            "range": "± 6.23401",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.3872,
            "range": "± 9.83748",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 31.3344,
            "range": "± 7.23699",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 35.9327,
            "range": "± 6.10488",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.0319,
            "range": "± 6.10368",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 78.082,
            "range": "± 10.0393",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 510.59,
            "range": "± 20.2249",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 98.5435,
            "range": "± 15.8339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 653.948,
            "range": "± 17.8585",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.51212,
            "range": "± 369.511",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.47463,
            "range": "± 270.28",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.32863,
            "range": "± 1.25811",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.90682,
            "range": "± 582.711",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 28.1464,
            "range": "± 3.10463",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.4811,
            "range": "± 2.05546",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.54772,
            "range": "± 1.64943",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.11818,
            "range": "± 666.092",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.7931,
            "range": "± 3.60859",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.39323,
            "range": "± 1.55013",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.47769,
            "range": "± 1.45056",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.20682,
            "range": "± 721.933",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.5466,
            "range": "± 3.10855",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.9824,
            "range": "± 1.68117",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.2858,
            "range": "± 1.02217",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.49225,
            "range": "± 1.20795",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.1297,
            "range": "± 1.76622",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.48293,
            "range": "± 891.064",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.55945,
            "range": "± 1.14932",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.38215,
            "range": "± 562.453",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 46.892,
            "range": "± 7.19939",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 47.7966,
            "range": "± 7.34375",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.99908,
            "range": "± 51.9886",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.04949,
            "range": "± 64.2803",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 108.212,
            "range": "± 8.94055",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 53.1058,
            "range": "± 8.03398",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.14344,
            "range": "± 50.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.45444,
            "range": "± 44.4228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.8133,
            "range": "± 1.239",
            "unit": "ns",
            "extra": "100 samples\n8470 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.4173,
            "range": "± 1.47183",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 270.672,
            "range": "± 22.0713",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.70618,
            "range": "± 0.575335",
            "unit": "ns",
            "extra": "100 samples\n6562 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 49.0508,
            "range": "± 6.58579",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.0601,
            "range": "± 136.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.1954,
            "range": "± 6.72923",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 431.066,
            "range": "± 10.9553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 95.4272,
            "range": "± 1.07286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.3371,
            "range": "± 2.9004",
            "unit": "ns",
            "extra": "100 samples\n1126 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 143.532,
            "range": "± 8.55091",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5558,
            "range": "± 56.573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49443,
            "range": "± 0.316178",
            "unit": "ns",
            "extra": "100 samples\n8389 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 27.8102,
            "range": "± 12.2144",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80565,
            "range": "± 30.5474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 46.9443,
            "range": "± 4.4281",
            "unit": "ns",
            "extra": "100 samples\n646 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 47.2736,
            "range": "± 6.95068",
            "unit": "ns",
            "extra": "100 samples\n642 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 433.223,
            "range": "± 12.9308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 388.297,
            "range": "± 10.114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 96.2099,
            "range": "± 3.32633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 81.2906,
            "range": "± 2.1803",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 27.3492,
            "range": "± 3.02168",
            "unit": "ns",
            "extra": "100 samples\n1119 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 27.1351,
            "range": "± 3.37395",
            "unit": "ns",
            "extra": "100 samples\n1129 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 142.448,
            "range": "± 7.4371",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 143.205,
            "range": "± 9.29266",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.5791,
            "range": "± 140.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9471,
            "range": "± 148.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 6.46221,
            "range": "± 3.853",
            "unit": "ns",
            "extra": "100 samples\n8367 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.51284,
            "range": "± 0.487117",
            "unit": "ns",
            "extra": "100 samples\n8508 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.2095,
            "range": "± 1.09685",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.5405,
            "range": "± 1.7711",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.81103,
            "range": "± 20.4305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.81063,
            "range": "± 18.701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.121,
            "range": "± 6.05346",
            "unit": "ns",
            "extra": "100 samples\n643 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.2626,
            "range": "± 5.38705",
            "unit": "ns",
            "extra": "100 samples\n649 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 431.578,
            "range": "± 10.7863",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 432.767,
            "range": "± 12.1607",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 96.3403,
            "range": "± 1.62976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 96.2274,
            "range": "± 1.12685",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.4102,
            "range": "± 5.83414",
            "unit": "ns",
            "extra": "100 samples\n648 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 45.7835,
            "range": "± 5.41618",
            "unit": "ns",
            "extra": "100 samples\n657 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 42.6982,
            "range": "± 8.79907",
            "unit": "ns",
            "extra": "100 samples\n712 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 432.24,
            "range": "± 14.1353",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 394.288,
            "range": "± 13.4284",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 444.486,
            "range": "± 17.2304",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 96.9747,
            "range": "± 1.55046",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 90.677,
            "range": "± 702.982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 87.5794,
            "range": "± 1.11159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 42.7582,
            "range": "± 8.99007",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 33.071,
            "range": "± 3.89788",
            "unit": "ns",
            "extra": "100 samples\n942 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 445.921,
            "range": "± 14.0195",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 451.738,
            "range": "± 19.5426",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 87.5008,
            "range": "± 1.11772",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 87.4597,
            "range": "± 1.01399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.6677,
            "range": "± 2.86582",
            "unit": "ns",
            "extra": "100 samples\n737 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.8044,
            "range": "± 4.93423",
            "unit": "ns",
            "extra": "100 samples\n737 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3323.36,
            "range": "± 98065.8",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.51244,
            "range": "± 122.33",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.1261,
            "range": "± 3.79274",
            "unit": "ns",
            "extra": "100 samples\n772 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 284.785,
            "range": "± 6.65083",
            "unit": "ns",
            "extra": "100 samples\n8302 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.0516,
            "range": "± 0.701203",
            "unit": "ns",
            "extra": "100 samples\n7788 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.6142,
            "range": "± 2.19983",
            "unit": "ns",
            "extra": "100 samples\n800 iterations"
          },
          {
            "name": "ir_add",
            "value": 786.485,
            "range": "± 106.292",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.61397,
            "range": "± 99.7667",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.56093,
            "range": "± 385.885",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.70371,
            "range": "± 120.347",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.93463,
            "range": "± 258.115",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.86361,
            "range": "± 134.653",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.07033,
            "range": "± 168.75",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.49515,
            "range": "± 109.525",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.82787,
            "range": "± 119.175",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.55539,
            "range": "± 157.902",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.76762,
            "range": "± 571.085",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.0889,
            "range": "± 5.16622",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8233,
            "range": "± 960.249",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 170.514,
            "range": "± 9.02546",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 188.544,
            "range": "± 14.3902",
            "unit": "ns",
            "extra": "100 samples\n162 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 517.966,
            "range": "± 180.111",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.15681,
            "range": "± 97.8341",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 476.737,
            "range": "± 31.3013",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 920.404,
            "range": "± 91.3512",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 499.492,
            "range": "± 43.7345",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 591.634,
            "range": "± 52.6512",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 412.133,
            "range": "± 31.1822",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 506.52,
            "range": "± 42.2709",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 452.193,
            "range": "± 37.8127",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
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
          "id": "45d355331724ef9999d8caaea947e6460516090c",
          "message": "Dedicated benchmark dashboard UI: trend charts per variation, run detail views (#397)",
          "timestamp": "2026-07-15T20:43:26+02:00",
          "tree_id": "b2c6d520c0d4db1126ddf6338f75f2ed33e98f04",
          "url": "https://github.com/nebulastream/nautilus/commit/45d355331724ef9999d8caaea947e6460516090c"
        },
        "date": 1784141486528,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.44799,
            "range": "± 526.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.96239,
            "range": "± 370.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.85032,
            "range": "± 647.532",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.86066,
            "range": "± 222.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2446,
            "range": "± 800.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.9267,
            "range": "± 565.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.7874,
            "range": "± 331.309",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.69823,
            "range": "± 195.725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.2664,
            "range": "± 259.245",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.3453,
            "range": "± 391.539",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.1536,
            "range": "± 213.364",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.5325,
            "range": "± 269.608",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.7289,
            "range": "± 658.237",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.809,
            "range": "± 780.649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.563,
            "range": "± 1.27328",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0222,
            "range": "± 594.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5368,
            "range": "± 369.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.833,
            "range": "± 444.403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.172,
            "range": "± 493.388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.1471,
            "range": "± 753.235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.3121,
            "range": "± 597.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.8623,
            "range": "± 867.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.8764,
            "range": "± 2.52974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.1195,
            "range": "± 1.05967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.6512,
            "range": "± 766.634",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 55.9456,
            "range": "± 1.79451",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.7718,
            "range": "± 662.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 60.2844,
            "range": "± 2.51362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.9093,
            "range": "± 2.67864",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.8147,
            "range": "± 3.9727",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.4097,
            "range": "± 4.30973",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.0966,
            "range": "± 4.07915",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.1326,
            "range": "± 3.94056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.4235,
            "range": "± 3.98415",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.5341,
            "range": "± 4.10606",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.661,
            "range": "± 3.80139",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.5893,
            "range": "± 4.20732",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.7444,
            "range": "± 3.80172",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 40.8736,
            "range": "± 9.47816",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 277.276,
            "range": "± 12.3867",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 56.9873,
            "range": "± 9.94914",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 495.421,
            "range": "± 14.9645",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.82758,
            "range": "± 106.478",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 3.97803,
            "range": "± 337.215",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.01013,
            "range": "± 1.0003",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.02156,
            "range": "± 352.028",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 6.99005,
            "range": "± 844.343",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.24413,
            "range": "± 394.064",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.21262,
            "range": "± 617.166",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.3366,
            "range": "± 413.669",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.34335,
            "range": "± 341.26",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.58926,
            "range": "± 320.75",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 25.4142,
            "range": "± 2.61969",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 275.466,
            "range": "± 7.32319",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 32.7522,
            "range": "± 3.41037",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 406.702,
            "range": "± 9.78796",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.8616,
            "range": "± 5.16607",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.1575,
            "range": "± 6.53786",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 35.9775,
            "range": "± 6.16551",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 26.3183,
            "range": "± 5.82957",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 33.889,
            "range": "± 6.53769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 29.3461,
            "range": "± 5.96574",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 30.57,
            "range": "± 6.0338",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.3752,
            "range": "± 5.95765",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 27.5364,
            "range": "± 5.82144",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 26.4648,
            "range": "± 5.85391",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 68.7818,
            "range": "± 12.1551",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 495.158,
            "range": "± 18.51",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 92.7952,
            "range": "± 14.9474",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 672.094,
            "range": "± 48.3849",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.3145,
            "range": "± 0.649857",
            "unit": "ns",
            "extra": "100 samples\n8803 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.8281,
            "range": "± 1.19138",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 561.757,
            "range": "± 63.6516",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.56865,
            "range": "± 0.608911",
            "unit": "ns",
            "extra": "100 samples\n7228 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 54.1499,
            "range": "± 2.89421",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 12.4454,
            "range": "± 41.6701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.6335,
            "range": "± 7.47299",
            "unit": "ns",
            "extra": "100 samples\n691 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 389.826,
            "range": "± 19.5729",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 91.5755,
            "range": "± 1.18132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 30.4889,
            "range": "± 1.89964",
            "unit": "ns",
            "extra": "100 samples\n1039 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 150.703,
            "range": "± 4.54594",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 40.031,
            "range": "± 117.625",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.57274,
            "range": "± 0.265194",
            "unit": "ns",
            "extra": "100 samples\n8817 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.1979,
            "range": "± 2.0997",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.93186,
            "range": "± 257.505",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 45.516,
            "range": "± 3.0438",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 45.8578,
            "range": "± 3.79154",
            "unit": "ns",
            "extra": "100 samples\n691 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 388.55,
            "range": "± 14.0533",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 386.083,
            "range": "± 19.1175",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 91.9729,
            "range": "± 3.66152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 77.2031,
            "range": "± 247.344",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 30.2867,
            "range": "± 1.39187",
            "unit": "ns",
            "extra": "100 samples\n1038 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 30.4477,
            "range": "± 1.67261",
            "unit": "ns",
            "extra": "100 samples\n1037 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 150.429,
            "range": "± 4.04543",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 151.067,
            "range": "± 4.53286",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 40.0106,
            "range": "± 84.0644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 37.8905,
            "range": "± 67.1807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.60842,
            "range": "± 0.357668",
            "unit": "ns",
            "extra": "100 samples\n8809 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.55657,
            "range": "± 0.28121",
            "unit": "ns",
            "extra": "100 samples\n8801 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 21.4471,
            "range": "± 2.8402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 21.1866,
            "range": "± 2.15251",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 3.96209,
            "range": "± 220.589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 3.84321,
            "range": "± 290.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 46.0142,
            "range": "± 4.64646",
            "unit": "ns",
            "extra": "100 samples\n676 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.3522,
            "range": "± 5.11897",
            "unit": "ns",
            "extra": "100 samples\n689 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 419.876,
            "range": "± 22.8603",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 388.287,
            "range": "± 14.5966",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 92.8915,
            "range": "± 1.25859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 91.385,
            "range": "± 215.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 45.1849,
            "range": "± 1.04888",
            "unit": "ns",
            "extra": "100 samples\n689 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 43.8242,
            "range": "± 6.04281",
            "unit": "ns",
            "extra": "100 samples\n747 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 40.553,
            "range": "± 4.63653",
            "unit": "ns",
            "extra": "100 samples\n789 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 386.182,
            "range": "± 11.5105",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 339.173,
            "range": "± 11.3252",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 481.575,
            "range": "± 13.2354",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 91.8046,
            "range": "± 1.88687",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 82.3893,
            "range": "± 2.58877",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 96.1153,
            "range": "± 1.8243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.934,
            "range": "± 2.20419",
            "unit": "ns",
            "extra": "100 samples\n796 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 32.8232,
            "range": "± 1.58611",
            "unit": "ns",
            "extra": "100 samples\n962 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 480.503,
            "range": "± 13.6017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 478.669,
            "range": "± 11.705",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 95.7809,
            "range": "± 1.40385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 97.7565,
            "range": "± 194.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.2707,
            "range": "± 3.47421",
            "unit": "ns",
            "extra": "100 samples\n792 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.8621,
            "range": "± 5.5328",
            "unit": "ns",
            "extra": "100 samples\n788 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.72612,
            "range": "± 117.44",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.7463,
            "range": "± 516.332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.58051,
            "range": "± 116.476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.7814,
            "range": "± 431.581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 60.5004,
            "range": "± 17.9734",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.77206,
            "range": "± 130.795",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.6456,
            "range": "± 131.137",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.71587,
            "range": "± 200.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.4385,
            "range": "± 871.147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 120.461,
            "range": "± 25.3645",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3741.06,
            "range": "± 105180",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.19892,
            "range": "± 203.973",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 173.355,
            "range": "± 8.15256",
            "unit": "ns",
            "extra": "100 samples\n177 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 423.072,
            "range": "± 21.4643",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.08055,
            "range": "± 65.9963",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 446.607,
            "range": "± 37.1794",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 830.879,
            "range": "± 53.6337",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 451.253,
            "range": "± 22.6189",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 534.227,
            "range": "± 27.4755",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 395.829,
            "range": "± 22.2009",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 465.632,
            "range": "± 25.2126",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 413.831,
            "range": "± 24.1386",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.42119,
            "range": "± 223.917",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.43249,
            "range": "± 206.188",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.90257,
            "range": "± 1.33381",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.62139,
            "range": "± 488.58",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.9032,
            "range": "± 3.34721",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.9068,
            "range": "± 1.75845",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.66723,
            "range": "± 1.22095",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.74409,
            "range": "± 528.477",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.1087,
            "range": "± 3.17261",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.5124,
            "range": "± 1.15596",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.79493,
            "range": "± 1.3762",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.85324,
            "range": "± 735.11",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.6055,
            "range": "± 3.13225",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.1859,
            "range": "± 1.26993",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.07431,
            "range": "± 924.088",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.98082,
            "range": "± 922.293",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.2266,
            "range": "± 1.4734",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.14621,
            "range": "± 802.713",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.9749,
            "range": "± 1.29153",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.09271,
            "range": "± 626.391",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 45.515,
            "range": "± 7.04627",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 45.4762,
            "range": "± 8.84937",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 2.04027,
            "range": "± 205.172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.00367,
            "range": "± 48.1553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 108.338,
            "range": "± 10.4072",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 52.6058,
            "range": "± 9.96818",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.19056,
            "range": "± 42.9452",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.3591,
            "range": "± 143.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.9748,
            "range": "± 2.46337",
            "unit": "ns",
            "extra": "100 samples\n827 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 277.889,
            "range": "± 9.77208",
            "unit": "ns",
            "extra": "100 samples\n9531 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.76065,
            "range": "± 0.412583",
            "unit": "ns",
            "extra": "100 samples\n8651 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.1517,
            "range": "± 2.18247",
            "unit": "ns",
            "extra": "100 samples\n795 iterations"
          },
          {
            "name": "ir_add",
            "value": 739.027,
            "range": "± 41.5461",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.61241,
            "range": "± 270.053",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.3968,
            "range": "± 222.99",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.675,
            "range": "± 107.136",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.84631,
            "range": "± 166.116",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.82333,
            "range": "± 107.084",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.0051,
            "range": "± 114.13",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45747,
            "range": "± 72.6034",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74956,
            "range": "± 154.621",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.51625,
            "range": "± 96.8076",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.76982,
            "range": "± 650.701",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 92.4569,
            "range": "± 4.72145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.9693,
            "range": "± 928.296",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 169.644,
            "range": "± 6.57067",
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
          "id": "fe10593b74b20dd523ce4a3367c0ac10afa94c9e",
          "message": "Add Nautilus Playground: web UI for exploring the compilation pipeline (#391)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-07-15T22:50:31+02:00",
          "tree_id": "caa39b370bf2b16e2338b39b53a654d48cc67d3f",
          "url": "https://github.com/nebulastream/nautilus/commit/fe10593b74b20dd523ce4a3367c0ac10afa94c9e"
        },
        "date": 1784149102472,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 40.9974,
            "range": "± 6.33228",
            "unit": "ns",
            "extra": "100 samples\n762 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 331.803,
            "range": "± 34.6422",
            "unit": "ns",
            "extra": "100 samples\n8240 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.03191,
            "range": "± 0.589601",
            "unit": "ns",
            "extra": "100 samples\n7733 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 36.6314,
            "range": "± 1.90656",
            "unit": "ns",
            "extra": "100 samples\n743 iterations"
          },
          {
            "name": "ssa_add",
            "value": 176.102,
            "range": "± 9.10571",
            "unit": "ns",
            "extra": "100 samples\n167 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 448.652,
            "range": "± 27.888",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.12934,
            "range": "± 65.674",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 502.999,
            "range": "± 58.5348",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 899.894,
            "range": "± 101.015",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 505.834,
            "range": "± 52.7176",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 590.325,
            "range": "± 56.5317",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 438.148,
            "range": "± 58.824",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 518.5,
            "range": "± 55.8904",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 461.973,
            "range": "± 44.2557",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.58291,
            "range": "± 315.385",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.50054,
            "range": "± 250.317",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.39221,
            "range": "± 1.32397",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.8374,
            "range": "± 560.269",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.3847,
            "range": "± 3.16885",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.7583,
            "range": "± 2.03497",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.06294,
            "range": "± 1.27868",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.87955,
            "range": "± 548.093",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.1313,
            "range": "± 2.79988",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.80433,
            "range": "± 1.01137",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.45655,
            "range": "± 1.37847",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.07064,
            "range": "± 614.553",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.7571,
            "range": "± 3.05508",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.9772,
            "range": "± 1.80394",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.14595,
            "range": "± 1.01514",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.01911,
            "range": "± 809.466",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.118,
            "range": "± 2.01267",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.54569,
            "range": "± 885.328",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.62234,
            "range": "± 1.35825",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.44512,
            "range": "± 753.334",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 47.2879,
            "range": "± 7.50164",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 47.8109,
            "range": "± 7.98074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 2.02654,
            "range": "± 39.4709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.02742,
            "range": "± 38.3133",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 108.463,
            "range": "± 9.8708",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 53.2804,
            "range": "± 15.6745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.28044,
            "range": "± 475.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.41071,
            "range": "± 46.5956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.6997,
            "range": "± 1.82744",
            "unit": "ns",
            "extra": "100 samples\n8442 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.4266,
            "range": "± 2.30391",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 574.718,
            "range": "± 34.7329",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.65588,
            "range": "± 0.426314",
            "unit": "ns",
            "extra": "100 samples\n6594 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.765,
            "range": "± 6.21051",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.4044,
            "range": "± 113.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.5335,
            "range": "± 8.23668",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 414.783,
            "range": "± 36.6434",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 92.3595,
            "range": "± 154.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 27.2109,
            "range": "± 2.95181",
            "unit": "ns",
            "extra": "100 samples\n1112 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 146.863,
            "range": "± 12.5694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5728,
            "range": "± 87.9327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56093,
            "range": "± 0.431",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.545,
            "range": "± 1.87634",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.82183,
            "range": "± 39.1285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 48.5683,
            "range": "± 7.08437",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 49.0842,
            "range": "± 8.54177",
            "unit": "ns",
            "extra": "100 samples\n636 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 408.895,
            "range": "± 12.1141",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 377.123,
            "range": "± 16.481",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 92.3168,
            "range": "± 144.256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 78.9842,
            "range": "± 275.898",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 29.1011,
            "range": "± 5.81394",
            "unit": "ns",
            "extra": "100 samples\n1113 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 28.2857,
            "range": "± 4.4813",
            "unit": "ns",
            "extra": "100 samples\n1112 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 143.56,
            "range": "± 9.44573",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 146.576,
            "range": "± 11.7705",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.5924,
            "range": "± 128.604",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9671,
            "range": "± 57.9924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.57492,
            "range": "± 0.542139",
            "unit": "ns",
            "extra": "100 samples\n8469 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.56598,
            "range": "± 0.426595",
            "unit": "ns",
            "extra": "100 samples\n8468 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.3767,
            "range": "± 1.54848",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.5739,
            "range": "± 1.9628",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.81169,
            "range": "± 70.4799",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.79933,
            "range": "± 21.4789",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.8589,
            "range": "± 5.3421",
            "unit": "ns",
            "extra": "100 samples\n638 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 48.6815,
            "range": "± 7.63279",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 409.302,
            "range": "± 13.7486",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 409.412,
            "range": "± 12.7497",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 92.4233,
            "range": "± 141.644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 92.3806,
            "range": "± 118.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 47.1301,
            "range": "± 5.71914",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 48.2956,
            "range": "± 8.7341",
            "unit": "ns",
            "extra": "100 samples\n641 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.3253,
            "range": "± 6.49169",
            "unit": "ns",
            "extra": "100 samples\n755 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 408.219,
            "range": "± 10.3298",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 405.573,
            "range": "± 14.1244",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 444.466,
            "range": "± 13.572",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 92.4037,
            "range": "± 150.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 88.771,
            "range": "± 1.51551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 87.8944,
            "range": "± 1.0016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 41.0322,
            "range": "± 5.93502",
            "unit": "ns",
            "extra": "100 samples\n754 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 34.0247,
            "range": "± 5.76334",
            "unit": "ns",
            "extra": "100 samples\n923 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 442.577,
            "range": "± 12.4445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 441.848,
            "range": "± 12.424",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 87.7631,
            "range": "± 388.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 87.941,
            "range": "± 1.37759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.9695,
            "range": "± 5.9957",
            "unit": "ns",
            "extra": "100 samples\n753 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.8004,
            "range": "± 5.31315",
            "unit": "ns",
            "extra": "100 samples\n756 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.54157,
            "range": "± 324.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.76033,
            "range": "± 348.624",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.49852,
            "range": "± 170.703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.67851,
            "range": "± 289.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.698,
            "range": "± 1.00025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.9739,
            "range": "± 269.849",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.011,
            "range": "± 458.413",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.38811,
            "range": "± 217.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.6342,
            "range": "± 357.976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.8866,
            "range": "± 226.522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.7893,
            "range": "± 277.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.4581,
            "range": "± 445.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3802,
            "range": "± 343.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2151,
            "range": "± 316.863",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.1993,
            "range": "± 810.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.8737,
            "range": "± 314.662",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.5938,
            "range": "± 336.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.8381,
            "range": "± 582.592",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.5293,
            "range": "± 492.691",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.7373,
            "range": "± 356.244",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.6898,
            "range": "± 667.759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2722,
            "range": "± 587.331",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.0225,
            "range": "± 439.98",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.2676,
            "range": "± 592.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.7079,
            "range": "± 300.3",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 55.0257,
            "range": "± 578.33",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 29.8205,
            "range": "± 645.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 58.8286,
            "range": "± 780.687",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.4672,
            "range": "± 3.51769",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 21.1955,
            "range": "± 4.10111",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 25.9557,
            "range": "± 4.27714",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 21.1591,
            "range": "± 3.62947",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 25.2338,
            "range": "± 5.20692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 22.6151,
            "range": "± 4.09407",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.8051,
            "range": "± 4.74492",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 19.0453,
            "range": "± 3.54989",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 21.4811,
            "range": "± 3.72608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 22.1252,
            "range": "± 5.70566",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 50.3216,
            "range": "± 7.81975",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 295.431,
            "range": "± 23.4534",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 67.0058,
            "range": "± 10.494",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 463.622,
            "range": "± 19.9225",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.96262,
            "range": "± 196.159",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.28811,
            "range": "± 476.899",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.7471,
            "range": "± 1.68226",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.40703,
            "range": "± 645.157",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.37844,
            "range": "± 767.298",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.65179,
            "range": "± 517.616",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.77318,
            "range": "± 554.789",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.88911,
            "range": "± 888.757",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.90169,
            "range": "± 575.407",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.89861,
            "range": "± 582.803",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 27.2401,
            "range": "± 3.84108",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 281.869,
            "range": "± 12.8113",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 33.5826,
            "range": "± 3.19223",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 408.528,
            "range": "± 27.0278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 24.1607,
            "range": "± 5.47585",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.3019,
            "range": "± 6.95009",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 50.223,
            "range": "± 11.1826",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.4421,
            "range": "± 4.52541",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.3546,
            "range": "± 10.8832",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 37.5808,
            "range": "± 6.62557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 45.5553,
            "range": "± 8.56306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.8485,
            "range": "± 5.41405",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 35.2727,
            "range": "± 5.3289",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.1343,
            "range": "± 5.91984",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 78.7339,
            "range": "± 12.6234",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 507.051,
            "range": "± 28.2523",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 97.6672,
            "range": "± 13.2146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 644.045,
            "range": "± 26.8607",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.56031,
            "range": "± 285.746",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.37427,
            "range": "± 122.046",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.38891,
            "range": "± 129.782",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.5727,
            "range": "± 385.59",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 78.6177,
            "range": "± 15.6725",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.78191,
            "range": "± 177.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.83688,
            "range": "± 757.367",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.78762,
            "range": "± 195.985",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.0469,
            "range": "± 349.454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 137.574,
            "range": "± 18.9213",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 770.248,
            "range": "± 56.4669",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.71402,
            "range": "± 305.53",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.39844,
            "range": "± 338.07",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.70354,
            "range": "± 135.229",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.09947,
            "range": "± 492.797",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.91321,
            "range": "± 187.044",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.0473,
            "range": "± 151.761",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.50714,
            "range": "± 175.691",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.85401,
            "range": "± 205.725",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.62495,
            "range": "± 238.25",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.92482,
            "range": "± 915.528",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 92.145,
            "range": "± 5.84463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.1691,
            "range": "± 1.63735",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.998,
            "range": "± 13.395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3452.61,
            "range": "± 116912",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.01763,
            "range": "± 213.339",
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
          "id": "b907b950a285de13b4efcbe94d3c7e76d5810072",
          "message": "Restructure README with clearer layout and playground link (#398)",
          "timestamp": "2026-07-16T08:33:57+02:00",
          "tree_id": "34f19ab0fff889a3be6584a96273cf79a01266f1",
          "url": "https://github.com/nebulastream/nautilus/commit/b907b950a285de13b4efcbe94d3c7e76d5810072"
        },
        "date": 1784184099678,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 825.557,
            "range": "± 119.032",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.61119,
            "range": "± 149.143",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.38089,
            "range": "± 378.593",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.65515,
            "range": "± 131.54",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.77927,
            "range": "± 211.601",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.81982,
            "range": "± 164.895",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.99076,
            "range": "± 151.641",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.49116,
            "range": "± 190.964",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.87378,
            "range": "± 273.377",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.60161,
            "range": "± 271.069",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.53278,
            "range": "± 911.364",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.6076,
            "range": "± 6.57089",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5769,
            "range": "± 1.56017",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 161.046,
            "range": "± 22.4238",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.8039,
            "range": "± 0.896068",
            "unit": "ns",
            "extra": "100 samples\n8433 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.1581,
            "range": "± 1.83547",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 575.851,
            "range": "± 28.1937",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.71059,
            "range": "± 0.57341",
            "unit": "ns",
            "extra": "100 samples\n6537 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 48.6854,
            "range": "± 4.48169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 11.4458,
            "range": "± 188.042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.8412,
            "range": "± 7.71045",
            "unit": "ns",
            "extra": "100 samples\n633 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 492.49,
            "range": "± 11.9134",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 92.4083,
            "range": "± 221.052",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 28.6449,
            "range": "± 3.93765",
            "unit": "ns",
            "extra": "100 samples\n1116 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 146.4,
            "range": "± 11.4469",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 36.5586,
            "range": "± 172.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.55309,
            "range": "± 0.369007",
            "unit": "ns",
            "extra": "100 samples\n8428 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 14.0498,
            "range": "± 610.674",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 2.80703,
            "range": "± 25.0833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 48.5354,
            "range": "± 6.42898",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 47.9697,
            "range": "± 6.14382",
            "unit": "ns",
            "extra": "100 samples\n635 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 493.174,
            "range": "± 10.6453",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 457.038,
            "range": "± 11.1884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 92.3942,
            "range": "± 255.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.071,
            "range": "± 406.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 28.5851,
            "range": "± 3.59932",
            "unit": "ns",
            "extra": "100 samples\n1116 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 28.2206,
            "range": "± 2.95199",
            "unit": "ns",
            "extra": "100 samples\n1115 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 147.991,
            "range": "± 14.09",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 147.143,
            "range": "± 13.5513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 36.5411,
            "range": "± 87.4591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 33.9497,
            "range": "± 56.0908",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.5495,
            "range": "± 0.431967",
            "unit": "ns",
            "extra": "100 samples\n8462 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.56053,
            "range": "± 0.511775",
            "unit": "ns",
            "extra": "100 samples\n8424 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 14.4087,
            "range": "± 1.78117",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 14.197,
            "range": "± 1.08875",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 2.80559,
            "range": "± 31.8721",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 2.80662,
            "range": "± 31.3045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 49.4762,
            "range": "± 8.0484",
            "unit": "ns",
            "extra": "100 samples\n626 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.8217,
            "range": "± 6.75563",
            "unit": "ns",
            "extra": "100 samples\n632 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 410.199,
            "range": "± 18.4877",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 409.269,
            "range": "± 11.6429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 92.3408,
            "range": "± 179.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 92.3521,
            "range": "± 225.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 46.8447,
            "range": "± 2.48937",
            "unit": "ns",
            "extra": "100 samples\n637 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 48.774,
            "range": "± 10.5948",
            "unit": "ns",
            "extra": "100 samples\n639 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 40.3435,
            "range": "± 3.67981",
            "unit": "ns",
            "extra": "100 samples\n756 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 408.672,
            "range": "± 12.3658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 412.627,
            "range": "± 12.267",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 444.922,
            "range": "± 11.8339",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 92.3295,
            "range": "± 216.696",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 90.2434,
            "range": "± 425.847",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 89.5909,
            "range": "± 2.74901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 40.7006,
            "range": "± 4.81191",
            "unit": "ns",
            "extra": "100 samples\n728 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 32.527,
            "range": "± 4.60321",
            "unit": "ns",
            "extra": "100 samples\n931 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 444.928,
            "range": "± 13.0317",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 443.82,
            "range": "± 12.1203",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 89.4442,
            "range": "± 2.43266",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 88.8792,
            "range": "± 827.099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 40.6434,
            "range": "± 4.99197",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 40.3282,
            "range": "± 4.67984",
            "unit": "ns",
            "extra": "100 samples\n756 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 39.5891,
            "range": "± 5.17073",
            "unit": "ns",
            "extra": "100 samples\n746 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 296.357,
            "range": "± 9.47906",
            "unit": "ns",
            "extra": "100 samples\n8245 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.97279,
            "range": "± 0.324962",
            "unit": "ns",
            "extra": "100 samples\n7603 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.0087,
            "range": "± 2.67867",
            "unit": "ns",
            "extra": "100 samples\n734 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.50992,
            "range": "± 273.101",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.51364,
            "range": "± 241.456",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.13897,
            "range": "± 1.22388",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.79008,
            "range": "± 557.602",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.5085,
            "range": "± 3.01566",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.8557,
            "range": "± 2.8307",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.23604,
            "range": "± 1.4155",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.11527,
            "range": "± 664.835",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 18.1502,
            "range": "± 2.70067",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.90191,
            "range": "± 1.46164",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.77984,
            "range": "± 1.80375",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.13307,
            "range": "± 596.931",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.1453,
            "range": "± 2.61383",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.6036,
            "range": "± 1.52971",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.32678,
            "range": "± 1.23349",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.3437,
            "range": "± 1.2283",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.9383,
            "range": "± 1.85141",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.49843,
            "range": "± 863.782",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.40753,
            "range": "± 1.16268",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.1643,
            "range": "± 401.546",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 48.2071,
            "range": "± 8.39551",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 46.6651,
            "range": "± 6.95015",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 2.00051,
            "range": "± 60.7522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.97761,
            "range": "± 41.9921",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 107.676,
            "range": "± 12.4181",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.5895,
            "range": "± 6.24208",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.10228,
            "range": "± 57.0677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.38054,
            "range": "± 59.1028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.65833,
            "range": "± 239.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.17958,
            "range": "± 167.512",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.15913,
            "range": "± 213.81",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.26025,
            "range": "± 194.276",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.654,
            "range": "± 379.152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.6601,
            "range": "± 333.22",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.3518,
            "range": "± 316.114",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.20026,
            "range": "± 172.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.1869,
            "range": "± 280.249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.4115,
            "range": "± 498.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.9618,
            "range": "± 356.794",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.6798,
            "range": "± 353.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.79285,
            "range": "± 331.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.3688,
            "range": "± 918.727",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.1035,
            "range": "± 431.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.5856,
            "range": "± 390.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.1107,
            "range": "± 337.425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.4157,
            "range": "± 563.501",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2867,
            "range": "± 384.194",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.0349,
            "range": "± 420.601",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.4191,
            "range": "± 351.294",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.4941,
            "range": "± 281.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.9699,
            "range": "± 574.323",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.783,
            "range": "± 365.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.6699,
            "range": "± 523.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 53.4804,
            "range": "± 434.634",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 28.8275,
            "range": "± 618.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 58.053,
            "range": "± 634.185",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.2054,
            "range": "± 4.19661",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.662,
            "range": "± 3.66629",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 26.0257,
            "range": "± 4.48364",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 20.9773,
            "range": "± 3.43078",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 24.5563,
            "range": "± 3.92838",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 22.0025,
            "range": "± 4.02145",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 23.348,
            "range": "± 3.86505",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 19.2789,
            "range": "± 3.84239",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 23.4014,
            "range": "± 4.75085",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 20.3839,
            "range": "± 3.81003",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 49.5123,
            "range": "± 8.74953",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 281.913,
            "range": "± 12.6839",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 64.6753,
            "range": "± 9.08051",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 458.43,
            "range": "± 18.365",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.94448,
            "range": "± 255.826",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.35303,
            "range": "± 521.717",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.86927,
            "range": "± 1.31856",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.42784,
            "range": "± 652.361",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.3994,
            "range": "± 751.666",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.63274,
            "range": "± 541.803",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.71713,
            "range": "± 683.126",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.81584,
            "range": "± 762.117",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.81772,
            "range": "± 505.129",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 4.00217,
            "range": "± 419.075",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 27.1064,
            "range": "± 3.36263",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 272.73,
            "range": "± 8.09128",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 33.696,
            "range": "± 3.82",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 402.663,
            "range": "± 15.3434",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 25.2479,
            "range": "± 5.38477",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.6664,
            "range": "± 5.52237",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 52.2171,
            "range": "± 9.63726",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.1631,
            "range": "± 5.29163",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.1267,
            "range": "± 8.51552",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.9598,
            "range": "± 8.37312",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 46.8774,
            "range": "± 8.88565",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 30.5424,
            "range": "± 5.53455",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 36.0522,
            "range": "± 5.94713",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.5807,
            "range": "± 5.5279",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 78.2117,
            "range": "± 10.0337",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 497.962,
            "range": "± 18.4719",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 96.0861,
            "range": "± 11.4052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 644.238,
            "range": "± 18.02",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 193.425,
            "range": "± 26.1399",
            "unit": "ns",
            "extra": "100 samples\n165 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 485.082,
            "range": "± 57.2282",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18412,
            "range": "± 126.746",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 496.774,
            "range": "± 36.5667",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 901.242,
            "range": "± 67.3128",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 496.855,
            "range": "± 34.2618",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 616.212,
            "range": "± 67.8882",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 408.911,
            "range": "± 38.1775",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 506.88,
            "range": "± 27.5921",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 477.529,
            "range": "± 65.7904",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.35578,
            "range": "± 106.666",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.31244,
            "range": "± 137.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.28344,
            "range": "± 109.423",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.6009,
            "range": "± 394.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 75.779,
            "range": "± 15.9911",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.43962,
            "range": "± 143.963",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.41348,
            "range": "± 139.658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.38356,
            "range": "± 161.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.203,
            "range": "± 450.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 136.68,
            "range": "± 19.5699",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3354.52,
            "range": "± 120003",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.87372,
            "range": "± 342.45",
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
          "id": "69523c973e651972b9f3db570bd5870f0bce4b04",
          "message": "Force apt to use IPv4 in CI workflows (#400)",
          "timestamp": "2026-07-16T11:47:18+02:00",
          "tree_id": "4c1f02203bdc600e95a28cbc7fbb3ca79125e73a",
          "url": "https://github.com/nebulastream/nautilus/commit/69523c973e651972b9f3db570bd5870f0bce4b04"
        },
        "date": 1784195721180,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.7876,
            "range": "± 0.863816",
            "unit": "ns",
            "extra": "100 samples\n8837 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.3476,
            "range": "± 2.56426",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 569.76,
            "range": "± 55.4654",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.62191,
            "range": "± 0.812885",
            "unit": "ns",
            "extra": "100 samples\n7265 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 55.112,
            "range": "± 5.05179",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 12.4413,
            "range": "± 54.7965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.9206,
            "range": "± 6.90907",
            "unit": "ns",
            "extra": "100 samples\n683 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 378.499,
            "range": "± 10.8084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 92.2387,
            "range": "± 820.08",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 30.3692,
            "range": "± 1.6056",
            "unit": "ns",
            "extra": "100 samples\n1033 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 152.714,
            "range": "± 7.1023",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 40.573,
            "range": "± 753.95",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.61013,
            "range": "± 0.411504",
            "unit": "ns",
            "extra": "100 samples\n8851 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 19.0169,
            "range": "± 2.6222",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 3.38612,
            "range": "± 45.4894",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 45.9724,
            "range": "± 3.67529",
            "unit": "ns",
            "extra": "100 samples\n688 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 46.3716,
            "range": "± 5.5595",
            "unit": "ns",
            "extra": "100 samples\n692 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 379.409,
            "range": "± 11.12",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 371.408,
            "range": "± 24.9193",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 92.0812,
            "range": "± 210.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 79.9302,
            "range": "± 122.152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 30.4879,
            "range": "± 1.98092",
            "unit": "ns",
            "extra": "100 samples\n1035 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 30.1739,
            "range": "± 0.921888",
            "unit": "ns",
            "extra": "100 samples\n1037 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 152.07,
            "range": "± 5.0125",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 151.435,
            "range": "± 4.62046",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 40.0655,
            "range": "± 293.363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 37.9007,
            "range": "± 43.3262",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.55924,
            "range": "± 0.299243",
            "unit": "ns",
            "extra": "100 samples\n8841 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.58152,
            "range": "± 0.323828",
            "unit": "ns",
            "extra": "100 samples\n8834 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 18.72,
            "range": "± 1.40153",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 18.8363,
            "range": "± 1.63095",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 3.39272,
            "range": "± 53.3353",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 3.39004,
            "range": "± 47.3826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.748,
            "range": "± 3.02283",
            "unit": "ns",
            "extra": "100 samples\n679 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.2377,
            "range": "± 5.1573",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 402.679,
            "range": "± 18.4457",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 378.455,
            "range": "± 10.845",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 89.138,
            "range": "± 1.68926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 92.4779,
            "range": "± 3.43138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 46.6239,
            "range": "± 5.78528",
            "unit": "ns",
            "extra": "100 samples\n686 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 43.9669,
            "range": "± 4.73663",
            "unit": "ns",
            "extra": "100 samples\n721 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 41.2216,
            "range": "± 3.49329",
            "unit": "ns",
            "extra": "100 samples\n749 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 380.366,
            "range": "± 14.1758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 336.615,
            "range": "± 16.2501",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 485.096,
            "range": "± 14.1767",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 92.0814,
            "range": "± 206.539",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 74.631,
            "range": "± 1.23663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 96.3756,
            "range": "± 3.04558",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 41.3654,
            "range": "± 3.58513",
            "unit": "ns",
            "extra": "100 samples\n743 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 33.0482,
            "range": "± 4.08044",
            "unit": "ns",
            "extra": "100 samples\n967 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 483.399,
            "range": "± 13.4929",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 481.978,
            "range": "± 13.4492",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 95.6419,
            "range": "± 1.19312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 95.4676,
            "range": "± 787.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 41.4613,
            "range": "± 3.44901",
            "unit": "ns",
            "extra": "100 samples\n745 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 41.8663,
            "range": "± 4.45775",
            "unit": "ns",
            "extra": "100 samples\n743 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.86603,
            "range": "± 122.398",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.66591,
            "range": "± 142.649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.61108,
            "range": "± 128.648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 28.3491,
            "range": "± 267.983",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 61.3505,
            "range": "± 18.2017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 6.00197,
            "range": "± 153.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.96787,
            "range": "± 514.335",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.88643,
            "range": "± 172.476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.9387,
            "range": "± 297.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 118.931,
            "range": "± 21.6221",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 730.443,
            "range": "± 40.4495",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.55525,
            "range": "± 83.7315",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.43726,
            "range": "± 280.368",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.66078,
            "range": "± 145.676",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.89276,
            "range": "± 271.841",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.91351,
            "range": "± 357.981",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01802,
            "range": "± 194.742",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.44428,
            "range": "± 90.4539",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.73713,
            "range": "± 101.805",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.48191,
            "range": "± 89.6527",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.83719,
            "range": "± 450.21",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.9242,
            "range": "± 4.29562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.8579,
            "range": "± 635.777",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 165.165,
            "range": "± 5.87041",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.37943,
            "range": "± 211.738",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.22161,
            "range": "± 185.594",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.82191,
            "range": "± 164.962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.447,
            "range": "± 205.654",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0521,
            "range": "± 172.87",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.6945,
            "range": "± 231.26",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2301,
            "range": "± 214.571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.85259,
            "range": "± 177.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.2906,
            "range": "± 354.525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.3789,
            "range": "± 274.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.7165,
            "range": "± 666.007",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.0303,
            "range": "± 477.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.4062,
            "range": "± 313.856",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.0002,
            "range": "± 468.921",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.3193,
            "range": "± 471.423",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.3117,
            "range": "± 471.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.4968,
            "range": "± 2.14844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.3375,
            "range": "± 422.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.59,
            "range": "± 335.69",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.6637,
            "range": "± 471.328",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.5971,
            "range": "± 327.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2508,
            "range": "± 238.545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.3857,
            "range": "± 784.7",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.1521,
            "range": "± 229.14",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.7392,
            "range": "± 216.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 54.6947,
            "range": "± 258.59",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.2753,
            "range": "± 545.508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 59.4526,
            "range": "± 572.095",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7679,
            "range": "± 2.43416",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.043,
            "range": "± 4.75521",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.0574,
            "range": "± 3.88981",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.9992,
            "range": "± 3.87482",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.1539,
            "range": "± 4.65177",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.495,
            "range": "± 3.98531",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.5874,
            "range": "± 5.18357",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.37,
            "range": "± 3.65777",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.4981,
            "range": "± 4.54387",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.3232,
            "range": "± 3.67216",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 40.9363,
            "range": "± 8.05583",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 275.72,
            "range": "± 9.86069",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 55.6305,
            "range": "± 9.90166",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 457.749,
            "range": "± 13.1398",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.88326,
            "range": "± 113.375",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.03216,
            "range": "± 316.085",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.19312,
            "range": "± 972.124",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.10357,
            "range": "± 367.929",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.03718,
            "range": "± 694.653",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.3511,
            "range": "± 384.261",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.38408,
            "range": "± 614.565",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.50386,
            "range": "± 414.142",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.47187,
            "range": "± 416.826",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.72621,
            "range": "± 352.825",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 24.8266,
            "range": "± 3.15886",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 276.043,
            "range": "± 7.16305",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 31.5593,
            "range": "± 3.04741",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 403.531,
            "range": "± 27.8337",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.8426,
            "range": "± 6.64868",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 25.8243,
            "range": "± 5.46023",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 35.7748,
            "range": "± 5.74688",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 25.948,
            "range": "± 5.88467",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 34.2604,
            "range": "± 6.86221",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 29.4461,
            "range": "± 7.1525",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 31.5891,
            "range": "± 6.52924",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.2167,
            "range": "± 6.09153",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 27.8355,
            "range": "± 5.86839",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 27.0961,
            "range": "± 5.7848",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 70.0371,
            "range": "± 12.0455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 497.742,
            "range": "± 19.2169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 90.7606,
            "range": "± 13.0028",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 669.363,
            "range": "± 21.6392",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 167.37,
            "range": "± 6.7721",
            "unit": "ns",
            "extra": "100 samples\n186 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 422.606,
            "range": "± 24.4872",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.09488,
            "range": "± 79.2814",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 442.758,
            "range": "± 28.7149",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 840.861,
            "range": "± 58.1162",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 450.43,
            "range": "± 25.2493",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 526.047,
            "range": "± 29.4577",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 399.259,
            "range": "± 18.5781",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 464.557,
            "range": "± 33.6839",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 420.251,
            "range": "± 46.6321",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.41285,
            "range": "± 210.622",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.45994,
            "range": "± 213.019",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.68818,
            "range": "± 1.40542",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.75117,
            "range": "± 546.511",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.7172,
            "range": "± 3.73306",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.8896,
            "range": "± 1.94778",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.63433,
            "range": "± 1.4695",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.85845,
            "range": "± 544.408",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.7565,
            "range": "± 3.03186",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.64995,
            "range": "± 1.21876",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.64802,
            "range": "± 1.46109",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.90015,
            "range": "± 601.869",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.4643,
            "range": "± 3.64133",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2593,
            "range": "± 1.39307",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.9827,
            "range": "± 924.256",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.99096,
            "range": "± 957.698",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1376,
            "range": "± 1.52443",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.20702,
            "range": "± 697.644",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.82637,
            "range": "± 1.17192",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.05944,
            "range": "± 429.554",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 45.7176,
            "range": "± 7.06378",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 45.9017,
            "range": "± 7.31469",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 2.00521,
            "range": "± 53.2032",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.01788,
            "range": "± 34.6053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 102.86,
            "range": "± 9.3192",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.4246,
            "range": "± 7.01221",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.03711,
            "range": "± 211.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.33148,
            "range": "± 34.1206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.4881,
            "range": "± 5.31937",
            "unit": "ns",
            "extra": "100 samples\n795 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 274.285,
            "range": "± 7.91807",
            "unit": "ns",
            "extra": "100 samples\n9550 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.84195,
            "range": "± 0.826217",
            "unit": "ns",
            "extra": "100 samples\n8639 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.4903,
            "range": "± 1.81288",
            "unit": "ns",
            "extra": "100 samples\n795 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3742.0499999999997,
            "range": "± 116201",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.48472,
            "range": "± 279.229",
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
          "id": "3d5ad0cebd6b29f3b7d65f8eaf9d9aac5ee800dd",
          "message": "Surface server response body on failed MLIR download (#401)",
          "timestamp": "2026-07-16T13:28:26+02:00",
          "tree_id": "d6943884a71429a37852aa6f1fdba942dbcd02f3",
          "url": "https://github.com/nebulastream/nautilus/commit/3d5ad0cebd6b29f3b7d65f8eaf9d9aac5ee800dd"
        },
        "date": 1784201792154,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 3802.24,
            "range": "± 114421",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.38146,
            "range": "± 378.929",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.6394,
            "range": "± 6.26167",
            "unit": "ns",
            "extra": "100 samples\n822 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 273.105,
            "range": "± 7.25747",
            "unit": "ns",
            "extra": "100 samples\n9571 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.80386,
            "range": "± 0.47876",
            "unit": "ns",
            "extra": "100 samples\n8736 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.4867,
            "range": "± 2.8509",
            "unit": "ns",
            "extra": "100 samples\n798 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.49793,
            "range": "± 233.438",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.48053,
            "range": "± 242.343",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.75987,
            "range": "± 1.35905",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.75877,
            "range": "± 557.638",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.0503,
            "range": "± 4.4438",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.0623,
            "range": "± 1.99364",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.69723,
            "range": "± 1.50597",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.9012,
            "range": "± 542.737",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.0524,
            "range": "± 3.61185",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.81616,
            "range": "± 1.20797",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.89757,
            "range": "± 1.70834",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.9943,
            "range": "± 654.941",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.5244,
            "range": "± 2.51977",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.5123,
            "range": "± 1.45578",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.09351,
            "range": "± 957.785",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.16867,
            "range": "± 999.881",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.3064,
            "range": "± 1.59474",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.28285,
            "range": "± 766.344",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.94839,
            "range": "± 1.59518",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.18229,
            "range": "± 506.674",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 47.1644,
            "range": "± 8.54099",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 47.3602,
            "range": "± 10.9228",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 2.02538,
            "range": "± 38.229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 2.04222,
            "range": "± 40.6447",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 103.835,
            "range": "± 11.5589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.9776,
            "range": "± 7.99295",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.30122,
            "range": "± 365",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.38988,
            "range": "± 37.6655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.8406,
            "range": "± 1.06881",
            "unit": "ns",
            "extra": "100 samples\n8863 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.304,
            "range": "± 2.05564",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 614.985,
            "range": "± 61.6549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.61587,
            "range": "± 0.637176",
            "unit": "ns",
            "extra": "100 samples\n7264 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 54.6365,
            "range": "± 2.92703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 12.4546,
            "range": "± 106.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.5419,
            "range": "± 5.07354",
            "unit": "ns",
            "extra": "100 samples\n693 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 390.064,
            "range": "± 55.4562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 92.2896,
            "range": "± 1.13916",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add",
            "value": 30.2938,
            "range": "± 1.78062",
            "unit": "ns",
            "extra": "100 samples\n1003 iterations"
          },
          {
            "name": "exec_tbc_fibonacci",
            "value": 151.868,
            "range": "± 4.87346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum",
            "value": 40.152,
            "range": "± 691.161",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56513,
            "range": "± 0.368938",
            "unit": "ns",
            "extra": "100 samples\n8867 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 18.7747,
            "range": "± 1.5927",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.02906,
            "range": "± 100.328",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_passesOff",
            "value": 46.2751,
            "range": "± 5.28002",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_add_passesOn",
            "value": 46.6376,
            "range": "± 6.03116",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOff",
            "value": 393.374,
            "range": "± 61.0139",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_passesOn",
            "value": 376.269,
            "range": "± 21.1056",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOff",
            "value": 92.5986,
            "range": "± 3.26907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_passesOn",
            "value": 80.7971,
            "range": "± 4.00376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_add_passesOff",
            "value": 30.3329,
            "range": "± 1.95788",
            "unit": "ns",
            "extra": "100 samples\n1021 iterations"
          },
          {
            "name": "exec_tbc_add_passesOn",
            "value": 30.5208,
            "range": "± 2.72732",
            "unit": "ns",
            "extra": "100 samples\n1018 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOff",
            "value": 152.944,
            "range": "± 10.525",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_fibonacci_passesOn",
            "value": 152.313,
            "range": "± 6.06139",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOff",
            "value": 40.1425,
            "range": "± 780.238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_tbc_sum_passesOn",
            "value": 37.8833,
            "range": "± 58.5329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOff",
            "value": 3.5832,
            "range": "± 0.353333",
            "unit": "ns",
            "extra": "100 samples\n8873 iterations"
          },
          {
            "name": "exec_asmjit_add_passesOn",
            "value": 3.57726,
            "range": "± 0.384615",
            "unit": "ns",
            "extra": "100 samples\n8869 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOff",
            "value": 18.7444,
            "range": "± 1.56492",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci_passesOn",
            "value": 19.1397,
            "range": "± 3.02851",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOff",
            "value": 4.01836,
            "range": "± 154.865",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum_passesOn",
            "value": 4.00573,
            "range": "± 137.594",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 47.7622,
            "range": "± 8.23146",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 45.8678,
            "range": "± 2.80705",
            "unit": "ns",
            "extra": "100 samples\n691 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 411.748,
            "range": "± 15.8474",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 383.366,
            "range": "± 32.369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 91.9285,
            "range": "± 1.50186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 92.7978,
            "range": "± 3.61627",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_call",
            "value": 45.6895,
            "range": "± 2.53935",
            "unit": "ns",
            "extra": "100 samples\n689 iterations"
          },
          {
            "name": "exec_bc_add_switch",
            "value": 44.3699,
            "range": "± 5.63229",
            "unit": "ns",
            "extra": "100 samples\n710 iterations"
          },
          {
            "name": "exec_bc_add_threaded",
            "value": 39.9708,
            "range": "± 6.22604",
            "unit": "ns",
            "extra": "100 samples\n808 iterations"
          },
          {
            "name": "exec_bc_fibonacci_call",
            "value": 381.246,
            "range": "± 16.9728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_switch",
            "value": 338.469,
            "range": "± 15.9985",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded",
            "value": 475.634,
            "range": "± 10.2559",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_call",
            "value": 92.5046,
            "range": "± 955.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_switch",
            "value": 73.6274,
            "range": "± 9.62836",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded",
            "value": 96.3083,
            "range": "± 1.5172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noReuse",
            "value": 39.2815,
            "range": "± 2.67623",
            "unit": "ns",
            "extra": "100 samples\n808 iterations"
          },
          {
            "name": "exec_bc_add_threaded_reuse",
            "value": 33.4781,
            "range": "± 4.2804",
            "unit": "ns",
            "extra": "100 samples\n959 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_noReuse",
            "value": 478.369,
            "range": "± 17.2665",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_threaded_reuse",
            "value": 475.296,
            "range": "± 11.3001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_noReuse",
            "value": 96.1911,
            "range": "± 1.46913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_threaded_reuse",
            "value": 96.0263,
            "range": "± 496.904",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_threaded_noSuperinstr",
            "value": 39.5708,
            "range": "± 4.65291",
            "unit": "ns",
            "extra": "100 samples\n814 iterations"
          },
          {
            "name": "exec_bc_add_threaded_superinstr",
            "value": 39.1578,
            "range": "± 3.02479",
            "unit": "ns",
            "extra": "100 samples\n807 iterations"
          },
          {
            "name": "tiered_twotier_addOne",
            "value": 3.7179,
            "range": "± 110.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_addOne",
            "value": 3.58888,
            "range": "± 119.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.6361,
            "range": "± 221.016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.7561,
            "range": "± 470.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 61.1589,
            "range": "± 18.9789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_twotier_sumLoop",
            "value": 5.85888,
            "range": "± 242.058",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_singletier_sumLoop",
            "value": 5.67734,
            "range": "± 128.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.68565,
            "range": "± 152.676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.7504,
            "range": "± 304.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 116.22,
            "range": "± 21.8942",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 169.795,
            "range": "± 8.44955",
            "unit": "ns",
            "extra": "100 samples\n184 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 427.775,
            "range": "± 22.1177",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10703,
            "range": "± 78.9867",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 453.064,
            "range": "± 28.7765",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 853.756,
            "range": "± 48.6586",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 453.888,
            "range": "± 27.5951",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 537.022,
            "range": "± 61.6225",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 399.669,
            "range": "± 24.224",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 465.678,
            "range": "± 26.4262",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 430.916,
            "range": "± 25.0463",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ir_add",
            "value": 753.758,
            "range": "± 45.8654",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.58162,
            "range": "± 78.9423",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.52351,
            "range": "± 271.529",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.69827,
            "range": "± 133.802",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.88681,
            "range": "± 181.232",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.83372,
            "range": "± 100.322",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.00129,
            "range": "± 137.223",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.50819,
            "range": "± 104.319",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.77482,
            "range": "± 115.439",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.54615,
            "range": "± 93.6324",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.0254,
            "range": "± 640.45",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.999,
            "range": "± 4.97691",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.1901,
            "range": "± 947.992",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 170.757,
            "range": "± 5.84358",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.28413,
            "range": "± 197.645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.93658,
            "range": "± 225.092",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.72433,
            "range": "± 170.209",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.01842,
            "range": "± 197.862",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.5784,
            "range": "± 1.20144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.7051,
            "range": "± 1.04756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2682,
            "range": "± 1.05431",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.96218,
            "range": "± 622.586",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.1413,
            "range": "± 889.815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.4888,
            "range": "± 327.506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.4595,
            "range": "± 313.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.4173,
            "range": "± 572.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.8463,
            "range": "± 412.583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2604,
            "range": "± 568.031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.1953,
            "range": "± 617.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.6742,
            "range": "± 746.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.2571,
            "range": "± 736.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.7219,
            "range": "± 494.77",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.5975,
            "range": "± 828.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.0508,
            "range": "± 975.2",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.1647,
            "range": "± 941.377",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.8616,
            "range": "± 654.686",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.0734,
            "range": "± 1.08262",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.1844,
            "range": "± 1.22581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 32.5179,
            "range": "± 237.699",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 57.2635,
            "range": "± 689.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.8933,
            "range": "± 564.011",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 60.3307,
            "range": "± 722.656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.9226,
            "range": "± 2.87645",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.9834,
            "range": "± 5.13385",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.9865,
            "range": "± 5.47432",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.1214,
            "range": "± 3.98571",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.6269,
            "range": "± 4.81501",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.8809,
            "range": "± 4.2698",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8299,
            "range": "± 5.65203",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.5336,
            "range": "± 4.32729",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.6194,
            "range": "± 4.15718",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.6575,
            "range": "± 3.87923",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 42.2329,
            "range": "± 9.80784",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 286.365,
            "range": "± 13.4014",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 59.4037,
            "range": "± 12.4215",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 473.318,
            "range": "± 17.4168",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_add",
            "value": 1.88499,
            "range": "± 130.183",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "comp_tbc_ifThenElse",
            "value": 4.12082,
            "range": "± 404.858",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_deeplyNestedIfElse",
            "value": 9.1377,
            "range": "± 956.987",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_tbc_loop",
            "value": 4.1314,
            "range": "± 398.759",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_ifInsideLoop",
            "value": 7.03147,
            "range": "± 791.463",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_tbc_loopDirectCall",
            "value": 4.34744,
            "range": "± 378.873",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_pointerLoop",
            "value": 5.28162,
            "range": "± 477.688",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_tbc_staticLoop",
            "value": 4.42292,
            "range": "± 331.38",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_fibonacci",
            "value": 4.47751,
            "range": "± 387.901",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_tbc_gcd",
            "value": 3.7351,
            "range": "± 326.7",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_tbc_nestedIf10",
            "value": 24.6108,
            "range": "± 2.32067",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_nestedIf100",
            "value": 280.139,
            "range": "± 7.04991",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_tbc_chainedIf10",
            "value": 31.8941,
            "range": "± 3.00306",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_tbc_chainedIf100",
            "value": 435.252,
            "range": "± 53.4629",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.0176,
            "range": "± 5.92727",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.469,
            "range": "± 5.84193",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 36.4304,
            "range": "± 7.42958",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 26.5286,
            "range": "± 5.79535",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 34.5169,
            "range": "± 6.55285",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 29.9682,
            "range": "± 6.1318",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 32.018,
            "range": "± 6.69223",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.8331,
            "range": "± 6.43761",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 28.2598,
            "range": "± 6.42074",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 27.8468,
            "range": "± 5.96029",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 69.8639,
            "range": "± 11.8832",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 498.736,
            "range": "± 14.9691",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 91.2,
            "range": "± 13.124",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 676.769,
            "range": "± 64.2301",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}