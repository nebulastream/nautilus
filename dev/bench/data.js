window.BENCHMARK_DATA = {
  "lastUpdate": 1780982943152,
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
          "id": "2ffaf1e3d551a54606e830e8c4cb6c9c1a943d8f",
          "message": "Add GPU support: val primitives, CUDA and Metal backends (#212)\n\nCo-authored-by: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-04-21T22:57:35+02:00",
          "tree_id": "1deb1b127bc8f7d2353d8cd1f3369635cbe04179",
          "url": "https://github.com/nebulastream/nautilus/commit/2ffaf1e3d551a54606e830e8c4cb6c9c1a943d8f"
        },
        "date": 1776805551731,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.76627,
            "range": "± 332.405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.95835,
            "range": "± 273.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.37599,
            "range": "± 311.638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.2263,
            "range": "± 405.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.7197,
            "range": "± 616.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.0557,
            "range": "± 301.098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.7766,
            "range": "± 806.438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.55551,
            "range": "± 304.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.843,
            "range": "± 225.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.5026,
            "range": "± 441.728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.3348,
            "range": "± 358.605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.5408,
            "range": "± 500.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 14.2258,
            "range": "± 909.105",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 25.1272,
            "range": "± 1.54082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.1948,
            "range": "± 555.352",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.1286,
            "range": "± 610.986",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5929,
            "range": "± 547.02",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.3429,
            "range": "± 441.857",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.902,
            "range": "± 480.129",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.2206,
            "range": "± 662.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.8687,
            "range": "± 296.807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.4622,
            "range": "± 528.164",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.5222,
            "range": "± 633.469",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.0743,
            "range": "± 1.44027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.8068,
            "range": "± 836.067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.4829,
            "range": "± 1.10422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.5639,
            "range": "± 309.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.2252,
            "range": "± 776.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.2691,
            "range": "± 3.40585",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.6165,
            "range": "± 5.06579",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.9172,
            "range": "± 3.59346",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.8338,
            "range": "± 3.24741",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.9345,
            "range": "± 4.03811",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.7819,
            "range": "± 4.37981",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.142,
            "range": "± 6.15798",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.6114,
            "range": "± 4.31367",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.7324,
            "range": "± 4.59893",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.4547,
            "range": "± 3.15436",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.2171,
            "range": "± 5.29525",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 202.596,
            "range": "± 10.1372",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 52.1024,
            "range": "± 7.90861",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 324.112,
            "range": "± 16.9692",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.9202,
            "range": "± 4.38316",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.0044,
            "range": "± 6.12951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 56.3703,
            "range": "± 9.75886",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.7087,
            "range": "± 5.49668",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.7126,
            "range": "± 10.0651",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.9739,
            "range": "± 8.80549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.9986,
            "range": "± 11.1165",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.8058,
            "range": "± 5.39393",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.33,
            "range": "± 8.22075",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.021,
            "range": "± 5.95339",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 102.01,
            "range": "± 13.3976",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.05967,
            "range": "± 20.5217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 156.759,
            "range": "± 18.6395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.20871,
            "range": "± 48.7181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.3277,
            "range": "± 13.1825",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.73377,
            "range": "± 281.407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.4903,
            "range": "± 420.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.8942,
            "range": "± 14.8212",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 75.3287,
            "range": "± 16.237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 9.07401,
            "range": "± 281.979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.7854,
            "range": "± 557.305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 80.3408,
            "range": "± 17.7822",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.2091,
            "range": "± 1.42934",
            "unit": "ns",
            "extra": "100 samples\n8449 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.2824,
            "range": "± 1.92806",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 597.951,
            "range": "± 23.047",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.80544,
            "range": "± 0.814677",
            "unit": "ns",
            "extra": "100 samples\n6541 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.0914,
            "range": "± 8.16828",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9784,
            "range": "± 138.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.4782,
            "range": "± 4.70274",
            "unit": "ns",
            "extra": "100 samples\n676 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 857.194,
            "range": "± 17.4174",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 184.064,
            "range": "± 8.16741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.60927,
            "range": "± 0.607746",
            "unit": "ns",
            "extra": "100 samples\n8457 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.5429,
            "range": "± 1.9346",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85868,
            "range": "± 49.2357",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.0151,
            "range": "± 5.14882",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 45.238,
            "range": "± 6.37318",
            "unit": "ns",
            "extra": "100 samples\n676 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 855.189,
            "range": "± 9.66849",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 854.978,
            "range": "± 11.2227",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 185.09,
            "range": "± 6.42496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 185.4,
            "range": "± 6.91377",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 193.444,
            "range": "± 14.4574",
            "unit": "ns",
            "extra": "100 samples\n150 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 481.025,
            "range": "± 52.4164",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.22984,
            "range": "± 122.086",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 504.249,
            "range": "± 37.4013",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 933.215,
            "range": "± 66.4233",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 518.198,
            "range": "± 56.5645",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 620.896,
            "range": "± 51.2193",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 510.538,
            "range": "± 45.8424",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 534.56,
            "range": "± 58.5422",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 474.654,
            "range": "± 40.2272",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ir_add",
            "value": 668.487,
            "range": "± 68.8937",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.55566,
            "range": "± 154.265",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.51809,
            "range": "± 317.213",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.6329,
            "range": "± 190.438",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.9639,
            "range": "± 371.031",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.7762,
            "range": "± 185.217",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.19343,
            "range": "± 373.102",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.36351,
            "range": "± 127.388",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.75353,
            "range": "± 145.959",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.44216,
            "range": "± 117.369",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.54684,
            "range": "± 636.769",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.9756,
            "range": "± 5.29577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.6134,
            "range": "± 1.04809",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 174.304,
            "range": "± 9.65732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.36936,
            "range": "± 235.077",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.4785,
            "range": "± 300.96",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.06578,
            "range": "± 1.31373",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.96463,
            "range": "± 680.073",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.4539,
            "range": "± 3.12633",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.4822,
            "range": "± 1.9273",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.26316,
            "range": "± 1.58615",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.14295,
            "range": "± 829.938",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.3075,
            "range": "± 2.76239",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.33254,
            "range": "± 1.66561",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.20987,
            "range": "± 1.29384",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.22485,
            "range": "± 875.968",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.3704,
            "range": "± 2.7101",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.739,
            "range": "± 1.78075",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.76128,
            "range": "± 1.00223",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.6479,
            "range": "± 843.779",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.2995,
            "range": "± 1.73107",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.27241,
            "range": "± 773.99",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.13126,
            "range": "± 1.14014",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.16913,
            "range": "± 516.124",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.803,
            "range": "± 7.7761",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.5214,
            "range": "± 6.88634",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39386,
            "range": "± 54.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.38372,
            "range": "± 55.8981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.552,
            "range": "± 9.62073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.157,
            "range": "± 8.84456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43697,
            "range": "± 41.7797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.25375,
            "range": "± 45.4821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.2979,
            "range": "± 6.35257",
            "unit": "ns",
            "extra": "100 samples\n859 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 297.733,
            "range": "± 10.9758",
            "unit": "ns",
            "extra": "100 samples\n8268 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.08932,
            "range": "± 0.746367",
            "unit": "ns",
            "extra": "100 samples\n7710 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.6547,
            "range": "± 2.22188",
            "unit": "ns",
            "extra": "100 samples\n733 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 55.0947,
            "range": "± 14.543",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.47637,
            "range": "± 262.69",
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
          "id": "46024165b9347d0f7f5e3f8b4e9d21dc703d16e2",
          "message": "Harden the inlining plugin: registry thread-safety, JIT hook safety, tests (#246)\n\nCo-authored-by: Claude Opus 4.7 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-04-21T23:19:18+02:00",
          "tree_id": "991b5d3720cac210d5d7bb9bff6f09499d6b5967",
          "url": "https://github.com/nebulastream/nautilus/commit/46024165b9347d0f7f5e3f8b4e9d21dc703d16e2"
        },
        "date": 1776806673714,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 35.3865,
            "range": "± 4.28447",
            "unit": "ns",
            "extra": "100 samples\n861 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 279.328,
            "range": "± 3.7127",
            "unit": "ns",
            "extra": "100 samples\n8285 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.00184,
            "range": "± 0.802946",
            "unit": "ns",
            "extra": "100 samples\n7748 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 39.5706,
            "range": "± 1.964",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          },
          {
            "name": "ssa_add",
            "value": 183.954,
            "range": "± 9.73075",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 458.789,
            "range": "± 11.2779",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18968,
            "range": "± 74.4717",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 496.71,
            "range": "± 27.9232",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 925.652,
            "range": "± 61.8054",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 496.849,
            "range": "± 36.6132",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 605.177,
            "range": "± 39.2943",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 484.837,
            "range": "± 31.741",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 521.649,
            "range": "± 35.3707",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 452.32,
            "range": "± 25.6002",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.28506,
            "range": "± 132.953",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.30878,
            "range": "± 132.103",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.75318,
            "range": "± 1.01512",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.52338,
            "range": "± 312.956",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.9442,
            "range": "± 1.69359",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.7602,
            "range": "± 952.895",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.68083,
            "range": "± 1.04071",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.66315,
            "range": "± 386.388",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.7423,
            "range": "± 1.38269",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.70168,
            "range": "± 1.04226",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9146,
            "range": "± 5.65649",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.79562,
            "range": "± 412.453",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.0235,
            "range": "± 3.17108",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.4921,
            "range": "± 1.22365",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.26646,
            "range": "± 790.488",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.48803,
            "range": "± 921.949",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1139,
            "range": "± 1.41133",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.11648,
            "range": "± 687.577",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.92349,
            "range": "± 1.3417",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.98782,
            "range": "± 588.581",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.8742,
            "range": "± 7.11188",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.0798,
            "range": "± 6.11528",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.3915,
            "range": "± 25.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.46556,
            "range": "± 30.9473",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.9447,
            "range": "± 9.91357",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.8984,
            "range": "± 8.06197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.46404,
            "range": "± 71.8989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.32265,
            "range": "± 50.7391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 53.0753,
            "range": "± 8.9913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 8.18759,
            "range": "± 170.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.0091,
            "range": "± 7.62784",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 6.23982,
            "range": "± 93.2092",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.3315,
            "range": "± 728.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.1375,
            "range": "± 14.0183",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 73.9768,
            "range": "± 10.5077",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 8.22793,
            "range": "± 45.9556",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 24.9887,
            "range": "± 497.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 75.5952,
            "range": "± 15.4558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.18217,
            "range": "± 124.492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.83464,
            "range": "± 188.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.78707,
            "range": "± 110.723",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.75533,
            "range": "± 57.1609",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.1228,
            "range": "± 372.15",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4405,
            "range": "± 488.466",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.6527,
            "range": "± 717.587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.59031,
            "range": "± 58.9107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.996,
            "range": "± 80.4379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9603,
            "range": "± 110.88",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0351,
            "range": "± 128.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.0546,
            "range": "± 252.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0206,
            "range": "± 62.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.3266,
            "range": "± 110.023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.172,
            "range": "± 273.243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.8408,
            "range": "± 223.603",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.8796,
            "range": "± 219.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.9526,
            "range": "± 312.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 25.82,
            "range": "± 241.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.3217,
            "range": "± 325.548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.4441,
            "range": "± 243.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.8067,
            "range": "± 400.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.1638,
            "range": "± 269.58",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.8562,
            "range": "± 281.3",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.8813,
            "range": "± 240.944",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.8269,
            "range": "± 460.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.1395,
            "range": "± 269.48",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.7091,
            "range": "± 984.812",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.441,
            "range": "± 2.9241",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.1243,
            "range": "± 3.28918",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.2027,
            "range": "± 3.60003",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.1823,
            "range": "± 3.36859",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8275,
            "range": "± 2.98808",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.7606,
            "range": "± 3.09223",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8801,
            "range": "± 3.25984",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.8993,
            "range": "± 3.84761",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.5432,
            "range": "± 3.46523",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.0011,
            "range": "± 3.96605",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.4416,
            "range": "± 4.14209",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 195.257,
            "range": "± 11.2114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.5842,
            "range": "± 7.49923",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 301.348,
            "range": "± 12.1043",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.7889,
            "range": "± 3.86536",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.6031,
            "range": "± 5.23951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 57.3092,
            "range": "± 11.3552",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 33.9114,
            "range": "± 4.15675",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 55.8297,
            "range": "± 9.98323",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.1046,
            "range": "± 9.45127",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.4143,
            "range": "± 8.76097",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.5159,
            "range": "± 4.46123",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 42.3652,
            "range": "± 7.75237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.0563,
            "range": "± 7.8456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 102.035,
            "range": "± 11.5914",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.04556,
            "range": "± 18.2254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 149.222,
            "range": "± 10.1037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.14167,
            "range": "± 30.6905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 656.589,
            "range": "± 35.4979",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.49466,
            "range": "± 100.096",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.46657,
            "range": "± 218.509",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.624,
            "range": "± 118.533",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.88257,
            "range": "± 208.104",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.72484,
            "range": "± 73.824",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.97159,
            "range": "± 163.276",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.33954,
            "range": "± 78.7507",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.72177,
            "range": "± 104.377",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.46473,
            "range": "± 84.9551",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.59077,
            "range": "± 592.179",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 86.4191,
            "range": "± 5.74789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.2545,
            "range": "± 751.315",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.748,
            "range": "± 10.1368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.4405,
            "range": "± 1.01308",
            "unit": "ns",
            "extra": "100 samples\n8449 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.831,
            "range": "± 1.5398",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 608.54,
            "range": "± 20.5981",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.72473,
            "range": "± 0.548913",
            "unit": "ns",
            "extra": "100 samples\n6548 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.8966,
            "range": "± 12.0742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9949,
            "range": "± 209.528",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.4592,
            "range": "± 2.61241",
            "unit": "ns",
            "extra": "100 samples\n680 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 860.076,
            "range": "± 14.6739",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 185.963,
            "range": "± 1.07498",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.47005,
            "range": "± 0.194007",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.1475,
            "range": "± 3.99034",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85045,
            "range": "± 22.5172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.7868,
            "range": "± 8.63127",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.0523,
            "range": "± 4.26555",
            "unit": "ns",
            "extra": "100 samples\n675 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 856.606,
            "range": "± 10.551",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 857.01,
            "range": "± 11.4079",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 186.254,
            "range": "± 1.43595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 186.122,
            "range": "± 415.862",
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
          "id": "f1b569ae33efcea842e7fc5ee44464cedf00e60c",
          "message": "Introduce MLIRJit wrapper around mlir::ExecutionEngine (#264)",
          "timestamp": "2026-04-22T20:14:43+02:00",
          "tree_id": "daf5a9e171f81c8bb4bcaff9bcdd7952c46a96fa",
          "url": "https://github.com/nebulastream/nautilus/commit/f1b569ae33efcea842e7fc5ee44464cedf00e60c"
        },
        "date": 1776882214035,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.15625,
            "range": "± 303.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.77636,
            "range": "± 348.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.71346,
            "range": "± 399.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.05246,
            "range": "± 484.574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0459,
            "range": "± 473.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.7572,
            "range": "± 674.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2404,
            "range": "± 731.087",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.52754,
            "range": "± 349.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.7068,
            "range": "± 500.289",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.0179,
            "range": "± 606.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.79,
            "range": "± 474.546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.577,
            "range": "± 376.382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.9161,
            "range": "± 706.058",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.8504,
            "range": "± 306.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 213.075,
            "range": "± 40.8268",
            "unit": "ns",
            "extra": "100 samples\n161 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 509.048,
            "range": "± 110.156",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.26731,
            "range": "± 226.545",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 543.408,
            "range": "± 104.802",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 985.548,
            "range": "± 174.281",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 540.139,
            "range": "± 76.8559",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 635.238,
            "range": "± 95.5448",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 523.208,
            "range": "± 114.916",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 540.04,
            "range": "± 72.5173",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 483.802,
            "range": "± 76.8216",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 38.8669,
            "range": "± 8.48797",
            "unit": "ns",
            "extra": "100 samples\n824 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 316.48,
            "range": "± 18.5648",
            "unit": "ns",
            "extra": "100 samples\n8233 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.51894,
            "range": "± 0.976781",
            "unit": "ns",
            "extra": "100 samples\n7740 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.6555,
            "range": "± 6.67114",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "ir_add",
            "value": 721.261,
            "range": "± 126.19",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.70107,
            "range": "± 374.782",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.74096,
            "range": "± 754.735",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.78289,
            "range": "± 386.765",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.9735,
            "range": "± 478.32",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.89926,
            "range": "± 287.618",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.09779,
            "range": "± 316.588",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.42025,
            "range": "± 229.726",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.85828,
            "range": "± 348.13",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.5604,
            "range": "± 276.914",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.76773,
            "range": "± 1.20542",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 85.9913,
            "range": "± 4.6035",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.298,
            "range": "± 1.31238",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 174.876,
            "range": "± 9.46844",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.7828,
            "range": "± 16.0603",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.30052,
            "range": "± 127.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 28.0157,
            "range": "± 1.42144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 55.7654,
            "range": "± 13.6604",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 78.3767,
            "range": "± 17.1943",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.77246,
            "range": "± 374.67",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 30.2012,
            "range": "± 2.62731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 79.3881,
            "range": "± 25.8478",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.52328,
            "range": "± 414.793",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.78145,
            "range": "± 606.676",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.15499,
            "range": "± 1.89719",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.22972,
            "range": "± 1.0227",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 27.4265,
            "range": "± 4.37764",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.4735,
            "range": "± 2.80203",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.07422,
            "range": "± 1.47101",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.216,
            "range": "± 1.00623",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.9715,
            "range": "± 3.36536",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.39063,
            "range": "± 2.07364",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.60999,
            "range": "± 1.78952",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.05364,
            "range": "± 830.737",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.4378,
            "range": "± 3.50047",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.9546,
            "range": "± 2.20134",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.49257,
            "range": "± 1.10441",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.06281,
            "range": "± 1.38798",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.7348,
            "range": "± 2.00721",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.66713,
            "range": "± 1.27929",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.38009,
            "range": "± 1.36754",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.32385,
            "range": "± 828.309",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 42.5086,
            "range": "± 12.2621",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.5316,
            "range": "± 8.86573",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35276,
            "range": "± 85.3128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.3573,
            "range": "± 79.1349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 100.804,
            "range": "± 13.549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 52.3929,
            "range": "± 11.7074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43811,
            "range": "± 58.6492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26657,
            "range": "± 67.0771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 56.1278,
            "range": "± 14.5292",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.04734,
            "range": "± 427.763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.8586,
            "range": "± 1.30662",
            "unit": "ns",
            "extra": "100 samples\n8435 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 17.3837,
            "range": "± 3.81913",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 610.856,
            "range": "± 32.8518",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.81187,
            "range": "± 0.70764",
            "unit": "ns",
            "extra": "100 samples\n6458 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.2451,
            "range": "± 10.7937",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9858,
            "range": "± 112.097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.6278,
            "range": "± 7.78666",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 848.49,
            "range": "± 26.9267",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.11,
            "range": "± 345.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54263,
            "range": "± 0.339404",
            "unit": "ns",
            "extra": "100 samples\n8444 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.0789,
            "range": "± 3.0195",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.84693,
            "range": "± 33.8549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 45.6569,
            "range": "± 6.09227",
            "unit": "ns",
            "extra": "100 samples\n681 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.7603,
            "range": "± 11.316",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 843.21,
            "range": "± 18.4569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 845.561,
            "range": "± 21.0756",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 178.97,
            "range": "± 338.631",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 179.001,
            "range": "± 338.902",
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
          "id": "9fb5464a8e80715c508b3c13d2389e5f989ea0c2",
          "message": "Replace stale NESIR codename with .nautilus in IR dumps (#268)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-04-25T17:04:44+02:00",
          "tree_id": "ecaa01fbe2392ea367f50d6bdf6ddbca3d314b5e",
          "url": "https://github.com/nebulastream/nautilus/commit/9fb5464a8e80715c508b3c13d2389e5f989ea0c2"
        },
        "date": 1777129795556,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 138.373,
            "range": "± 7.32871",
            "unit": "ns",
            "extra": "100 samples\n172 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 325.389,
            "range": "± 18.0966",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.8435180000000001,
            "range": "± 0.180735",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 349.106,
            "range": "± 30.8471",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 624.318,
            "range": "± 44.1548",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 350.94,
            "range": "± 32.6573",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 418.433,
            "range": "± 23.6135",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 329.866,
            "range": "± 15.4487",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 360.966,
            "range": "± 20.5787",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 322.127,
            "range": "± 16.0493",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 36.2276,
            "range": "± 13.4346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.15552,
            "range": "± 339.467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 28.3379,
            "range": "± 1.96656",
            "unit": "ns",
            "extra": "100 samples\n840 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 256.569,
            "range": "± 10.6004",
            "unit": "ns",
            "extra": "100 samples\n8671 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.34196,
            "range": "± 0.616277",
            "unit": "ns",
            "extra": "100 samples\n7909 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 31.9886,
            "range": "± 1.67641",
            "unit": "ns",
            "extra": "100 samples\n753 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 8.59958,
            "range": "± 1.19358",
            "unit": "ns",
            "extra": "100 samples\n8792 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.513,
            "range": "± 1.34449",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 525.225,
            "range": "± 53.3368",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.67922,
            "range": "± 0.811903",
            "unit": "ns",
            "extra": "100 samples\n7227 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 83.9984,
            "range": "± 4.82933",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 18.3061,
            "range": "± 161.764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 34.0835,
            "range": "± 1.72115",
            "unit": "ns",
            "extra": "100 samples\n709 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 525.483,
            "range": "± 8.75544",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 121.795,
            "range": "± 980.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.77541,
            "range": "± 0.238009",
            "unit": "ns",
            "extra": "100 samples\n8809 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 17.177,
            "range": "± 922.065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.09164,
            "range": "± 13.2278",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 33.7634,
            "range": "± 2.23986",
            "unit": "ns",
            "extra": "100 samples\n709 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 34.1226,
            "range": "± 2.07605",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 525.659,
            "range": "± 14.7636",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 522.697,
            "range": "± 6.12793",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 121.631,
            "range": "± 377.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 123.881,
            "range": "± 11.7256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 472.519,
            "range": "± 22.4289",
            "unit": "ns",
            "extra": "100 samples\n51 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.10245,
            "range": "± 56.8718",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 2.60794,
            "range": "± 218.537",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.2177,
            "range": "± 68.2452",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.15584,
            "range": "± 138.005",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.31324,
            "range": "± 132.945",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.48275,
            "range": "± 88.4029",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 0.9845900000000001,
            "range": "± 0.061127600000000004",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.30005,
            "range": "± 67.7673",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.07357,
            "range": "± 67.1601",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 5.8564,
            "range": "± 357.945",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 67.8284,
            "range": "± 2.62984",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 9.60605,
            "range": "± 593.912",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 131.857,
            "range": "± 3.68609",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 4.99102,
            "range": "± 184.922",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 5.51107,
            "range": "± 194.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.63724,
            "range": "± 262.943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.42914,
            "range": "± 261.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.9197,
            "range": "± 404.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 9.95457,
            "range": "± 371.462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.5659,
            "range": "± 347.826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.52332,
            "range": "± 146.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 8.86845,
            "range": "± 187.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 8.01767,
            "range": "± 235.656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 8.79452,
            "range": "± 177.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 20.0363,
            "range": "± 937.456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.83094,
            "range": "± 203.638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 47.8921,
            "range": "± 546.736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 42.7949,
            "range": "± 59.7657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 40.3514,
            "range": "± 51.4142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 53.97,
            "range": "± 98.7519",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 34.1145,
            "range": "± 30.3093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 35.4887,
            "range": "± 30.2107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 40.7899,
            "range": "± 46.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 51.8653,
            "range": "± 61.5547",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.8164,
            "range": "± 20.2931",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 44.4791,
            "range": "± 59.8791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.3493,
            "range": "± 23.4542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 39.8744,
            "range": "± 39.6376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 80.9904,
            "range": "± 91.9365",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 40.0757,
            "range": "± 45.1489",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.1654,
            "range": "± 50.7773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.2413,
            "range": "± 2.23587",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 12.5936,
            "range": "± 2.25207",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 15.4525,
            "range": "± 4.15042",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.6179,
            "range": "± 2.79536",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 14.7527,
            "range": "± 3.37745",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.1268,
            "range": "± 2.96684",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 13.9536,
            "range": "± 3.45096",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.699,
            "range": "± 3.28113",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.1513,
            "range": "± 3.33624",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 12.4383,
            "range": "± 2.30001",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 23.8622,
            "range": "± 4.27116",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 144.856,
            "range": "± 8.86019",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 32.6553,
            "range": "± 7.31153",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 234.82,
            "range": "± 12.8432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 13.3676,
            "range": "± 4.33233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 20.2674,
            "range": "± 5.0832",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 36.6596,
            "range": "± 9.1622",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 21.8916,
            "range": "± 4.988",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 37.2603,
            "range": "± 9.43391",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 24.828,
            "range": "± 5.16459",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 26.3979,
            "range": "± 5.54747",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 18.3299,
            "range": "± 4.62805",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 24.098,
            "range": "± 5.40726",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 21.8813,
            "range": "± 5.16793",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 73.5622,
            "range": "± 11.3226",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 809.944,
            "range": "± 22.6819",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 114.346,
            "range": "± 12.8187",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 1.70617,
            "range": "± 53.3082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 34.5292,
            "range": "± 14.2278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 2.8972,
            "range": "± 113.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 40.246,
            "range": "± 71.2071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 34.9898,
            "range": "± 14.1045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 47.7755,
            "range": "± 14.8728",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 4.51592,
            "range": "± 185.666",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 32.3056,
            "range": "± 30.5169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 49.6459,
            "range": "± 16.3775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.79071,
            "range": "± 181.7",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.83846,
            "range": "± 153.197",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 6.42968,
            "range": "± 1.24721",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.56841,
            "range": "± 492.088",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 19.0595,
            "range": "± 3.28438",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 9.80151,
            "range": "± 1.38996",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 6.4129,
            "range": "± 1.20831",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 3.6843,
            "range": "± 498.318",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 12.5007,
            "range": "± 2.96309",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 6.51683,
            "range": "± 1.08462",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 6.49754,
            "range": "± 1.51684",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 3.72241,
            "range": "± 483.529",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 10.5193,
            "range": "± 1.94018",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 7.80922,
            "range": "± 1.23348",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 5.8635,
            "range": "± 604.254",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 5.93927,
            "range": "± 618.967",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 7.44806,
            "range": "± 1.35005",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 4.65529,
            "range": "± 604.034",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 5.82484,
            "range": "± 1.02135",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.02964,
            "range": "± 417.274",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 30.4337,
            "range": "± 6.81674",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 30.1798,
            "range": "± 6.87712",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.13193,
            "range": "± 38.2485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.13391,
            "range": "± 28.1813",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 73.6661,
            "range": "± 11.1804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 38.2564,
            "range": "± 8.16948",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 3.34687,
            "range": "± 54.2151",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 1.79013,
            "range": "± 30.0181",
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
          "id": "372107d97e6597ac0c547ad42f332e45a66bf11d",
          "message": "Print function args and return type in IR dumps (#269)",
          "timestamp": "2026-04-25T18:00:47+02:00",
          "tree_id": "f575d5553f0b91b198f91a988b4de2e328b4aa15",
          "url": "https://github.com/nebulastream/nautilus/commit/372107d97e6597ac0c547ad42f332e45a66bf11d"
        },
        "date": 1777133152511,
        "tool": "catch2",
        "benches": [
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 54.4104,
            "range": "± 8.95961",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.56332,
            "range": "± 137.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 619.537,
            "range": "± 36.4819",
            "unit": "ns",
            "extra": "100 samples\n48 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.50086,
            "range": "± 81.3082",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.40164,
            "range": "± 221.664",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.6028,
            "range": "± 81.8544",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.81556,
            "range": "± 178.402",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.74588,
            "range": "± 150.572",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98007,
            "range": "± 159.867",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.27379,
            "range": "± 75.6583",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74462,
            "range": "± 138.229",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.43131,
            "range": "± 85.7994",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.59059,
            "range": "± 542.627",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.8972,
            "range": "± 5.45101",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.521,
            "range": "± 1.43098",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 182.378,
            "range": "± 11.455",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 182.58,
            "range": "± 9.12009",
            "unit": "ns",
            "extra": "100 samples\n159 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 456.382,
            "range": "± 29.1011",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.16708,
            "range": "± 76.3917",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 483.381,
            "range": "± 27.8521",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 917.335,
            "range": "± 64.8841",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 497.03,
            "range": "± 29.4655",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 592.786,
            "range": "± 37.3542",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 434.579,
            "range": "± 23.1642",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.457,
            "range": "± 31.369",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 451.343,
            "range": "± 43.8671",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.2584,
            "range": "± 121.6",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.26978,
            "range": "± 171.719",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.79006,
            "range": "± 935.383",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.67657,
            "range": "± 442.12",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.0941,
            "range": "± 2.84492",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.8557,
            "range": "± 1.23903",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.73548,
            "range": "± 1.04218",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.85768,
            "range": "± 754.464",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.0237,
            "range": "± 2.29432",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.65966,
            "range": "± 1.23288",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.78359,
            "range": "± 851.083",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.88873,
            "range": "± 470.685",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.2892,
            "range": "± 1.33491",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.3502,
            "range": "± 931.396",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.25092,
            "range": "± 495.313",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.31644,
            "range": "± 711.198",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1527,
            "range": "± 1.06301",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.18477,
            "range": "± 757.785",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.93711,
            "range": "± 907.056",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.9689,
            "range": "± 474.632",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.7926,
            "range": "± 4.39903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.6114,
            "range": "± 3.52553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39609,
            "range": "± 55.2737",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39202,
            "range": "± 16.5559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.5334,
            "range": "± 6.60462",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.8175,
            "range": "± 6.29341",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.48381,
            "range": "± 36.9859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.3486,
            "range": "± 29.2588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.7859,
            "range": "± 8.01448",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.29504,
            "range": "± 91.0984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 40.6336,
            "range": "± 2.31122",
            "unit": "ns",
            "extra": "100 samples\n716 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 287.544,
            "range": "± 6.02503",
            "unit": "ns",
            "extra": "100 samples\n8277 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.01811,
            "range": "± 0.721024",
            "unit": "ns",
            "extra": "100 samples\n7729 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3544,
            "range": "± 1.96528",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.506,
            "range": "± 1.72291",
            "unit": "ns",
            "extra": "100 samples\n8435 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.5047,
            "range": "± 1.82455",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 527.829,
            "range": "± 20.6702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.67513,
            "range": "± 0.602962",
            "unit": "ns",
            "extra": "100 samples\n6425 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.2921,
            "range": "± 6.18815",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9457,
            "range": "± 172.806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 50.0819,
            "range": "± 8.1899",
            "unit": "ns",
            "extra": "100 samples\n599 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 844.825,
            "range": "± 17.5308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 175.618,
            "range": "± 13.0318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.59202,
            "range": "± 0.764682",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.8781,
            "range": "± 3.36653",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85523,
            "range": "± 44.0952",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 49.3641,
            "range": "± 7.08352",
            "unit": "ns",
            "extra": "100 samples\n601 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 50.2469,
            "range": "± 9.12771",
            "unit": "ns",
            "extra": "100 samples\n602 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 843.303,
            "range": "± 14.8413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 845.205,
            "range": "± 19.0519",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 179.055,
            "range": "± 9.21709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 177.824,
            "range": "± 15.5412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.59397,
            "range": "± 148.25",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.1654,
            "range": "± 121.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.09349,
            "range": "± 122.937",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.3224,
            "range": "± 183.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.3991,
            "range": "± 344.056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.1469,
            "range": "± 168.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.4173,
            "range": "± 279.421",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.06699,
            "range": "± 117.709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.6901,
            "range": "± 296.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.88788,
            "range": "± 255.672",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.712,
            "range": "± 138.942",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.184,
            "range": "± 1.23411",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.90083,
            "range": "± 244.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.1712,
            "range": "± 403.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.7615,
            "range": "± 749.522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5843,
            "range": "± 410.074",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.8901,
            "range": "± 523.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4157,
            "range": "± 272.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.3116,
            "range": "± 254.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.7045,
            "range": "± 514.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.7815,
            "range": "± 238.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.0646,
            "range": "± 346.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.4566,
            "range": "± 1.17525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.176,
            "range": "± 289.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.4871,
            "range": "± 337.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.9278,
            "range": "± 700.796",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.8591,
            "range": "± 290.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.9446,
            "range": "± 687.047",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7638,
            "range": "± 2.57308",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 20.159,
            "range": "± 5.01441",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.0415,
            "range": "± 3.92402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.102,
            "range": "± 4.18315",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.9605,
            "range": "± 4.15894",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.4368,
            "range": "± 3.62401",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.3381,
            "range": "± 4.54902",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.7875,
            "range": "± 3.621",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.038,
            "range": "± 4.30861",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.8859,
            "range": "± 4.72402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.5908,
            "range": "± 5.01825",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 200.773,
            "range": "± 15.0961",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.4084,
            "range": "± 7.75536",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 313.218,
            "range": "± 17.4299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.1107,
            "range": "± 8.49649",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.6074,
            "range": "± 5.79668",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 55.9146,
            "range": "± 8.28231",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 41.2731,
            "range": "± 8.38285",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 58.245,
            "range": "± 10.3377",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.468,
            "range": "± 10.8509",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.2615,
            "range": "± 9.19757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3749,
            "range": "± 5.60941",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.4564,
            "range": "± 8.27571",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 40.3962,
            "range": "± 8.29466",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 104.343,
            "range": "± 14.089",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1051.75,
            "range": "± 22730.6",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 155.171,
            "range": "± 12.7413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.19101,
            "range": "± 44.4322",
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
          "id": "0805493c84f7e3ce005b9e03b11ed3db2fab860f",
          "message": "Add VS Code extension for Nautilus IR (#267)",
          "timestamp": "2026-04-25T18:28:38+02:00",
          "tree_id": "f75a2d19f6eb2b19f5ae1230fb8ece28b9d232b7",
          "url": "https://github.com/nebulastream/nautilus/commit/0805493c84f7e3ce005b9e03b11ed3db2fab860f"
        },
        "date": 1777134826666,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 176.367,
            "range": "± 12.4925",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 429.157,
            "range": "± 22.6664",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.0689,
            "range": "± 61.8558",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 467.293,
            "range": "± 62.0738",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 815.578,
            "range": "± 44.8433",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 457.363,
            "range": "± 30.4163",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 545.421,
            "range": "± 38.7409",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 429.744,
            "range": "± 30.9281",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 471.73,
            "range": "± 24.99",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 421.886,
            "range": "± 21.4701",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ir_add",
            "value": 574.097,
            "range": "± 39.6549",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.41565,
            "range": "± 83.0177",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.15374,
            "range": "± 172.282",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.52537,
            "range": "± 219.524",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.60759,
            "range": "± 179.01",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.57927,
            "range": "± 101.415",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.79466,
            "range": "± 106.256",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.20037,
            "range": "± 73.4685",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.56561,
            "range": "± 102.173",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.29546,
            "range": "± 80.3113",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.18524,
            "range": "± 507.475",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 94.1927,
            "range": "± 5.78112",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5913,
            "range": "± 799.917",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.39,
            "range": "± 7.87145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.07515,
            "range": "± 180.296",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.08854,
            "range": "± 169.144",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.00134,
            "range": "± 1.1291",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.20173,
            "range": "± 479.65",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.4615,
            "range": "± 3.68329",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.824,
            "range": "± 1.71419",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.7731,
            "range": "± 1.22503",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.29191,
            "range": "± 579.756",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.074,
            "range": "± 2.01083",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.99542,
            "range": "± 1.29304",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.83059,
            "range": "± 1.40378",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.34534,
            "range": "± 486.03",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.6511,
            "range": "± 2.25646",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.09764,
            "range": "± 1.3512",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.02112,
            "range": "± 704.084",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.05486,
            "range": "± 712.874",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.14423,
            "range": "± 1.3965",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.58248,
            "range": "± 732.254",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.15737,
            "range": "± 1.02195",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.61537,
            "range": "± 385.615",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 36.5526,
            "range": "± 6.80817",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 36.6558,
            "range": "± 6.73606",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.44265,
            "range": "± 37.7869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.47275,
            "range": "± 147.295",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 92.2129,
            "range": "± 10.291",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 46.5739,
            "range": "± 7.94782",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.60619,
            "range": "± 166.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.18757,
            "range": "± 28.9027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 44.7466,
            "range": "± 14.5872",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.58367,
            "range": "± 128.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.6145,
            "range": "± 299.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 45.7908,
            "range": "± 15.5018",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 62.3963,
            "range": "± 17.9242",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.62424,
            "range": "± 131.917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.5536,
            "range": "± 233.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.2456,
            "range": "± 17.6061",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.6576,
            "range": "± 0.991841",
            "unit": "ns",
            "extra": "100 samples\n8808 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.8539,
            "range": "± 1.74734",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 552.824,
            "range": "± 65.3336",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.49869,
            "range": "± 0.483184",
            "unit": "ns",
            "extra": "100 samples\n7212 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 107.013,
            "range": "± 4.74178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5527,
            "range": "± 135.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.8723,
            "range": "± 3.83581",
            "unit": "ns",
            "extra": "100 samples\n731 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 685.516,
            "range": "± 13.395",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 250.23,
            "range": "± 92.4725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56369,
            "range": "± 0.365164",
            "unit": "ns",
            "extra": "100 samples\n8813 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.8027,
            "range": "± 3.41867",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29416,
            "range": "± 28.6855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.6543,
            "range": "± 4.95053",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 42.4954,
            "range": "± 1.9199",
            "unit": "ns",
            "extra": "100 samples\n732 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 683.681,
            "range": "± 14.7038",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 676.079,
            "range": "± 8.52969",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 158.437,
            "range": "± 5.72461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 157.399,
            "range": "± 1.88941",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.4557,
            "range": "± 3.09927",
            "unit": "ns",
            "extra": "100 samples\n861 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 302.347,
            "range": "± 7.6278",
            "unit": "ns",
            "extra": "100 samples\n8599 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09671,
            "range": "± 0.465233",
            "unit": "ns",
            "extra": "100 samples\n7913 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 41.4487,
            "range": "± 2.4407",
            "unit": "ns",
            "extra": "100 samples\n753 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 46.3851,
            "range": "± 17.0725",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.17558,
            "range": "± 174.186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.16591,
            "range": "± 178.123",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.76589,
            "range": "± 168.692",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.62683,
            "range": "± 151.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.80228,
            "range": "± 205.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.3791,
            "range": "± 182.624",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.551,
            "range": "± 203.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.3807,
            "range": "± 212.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.57849,
            "range": "± 151.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.2158,
            "range": "± 282.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0003,
            "range": "± 185.187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.0299,
            "range": "± 187.95",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.2171,
            "range": "± 215.812",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3255,
            "range": "± 180.649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.1679,
            "range": "± 204.988",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.8411,
            "range": "± 297.96",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5363,
            "range": "± 264.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5642,
            "range": "± 281.955",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.6031,
            "range": "± 267.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.4281,
            "range": "± 261.182",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.9387,
            "range": "± 413.373",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.054,
            "range": "± 345.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2919,
            "range": "± 246.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6268,
            "range": "± 287.712",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.5573,
            "range": "± 270.275",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.6656,
            "range": "± 315.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.1685,
            "range": "± 396.885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.1103,
            "range": "± 399.151",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.9826,
            "range": "± 454.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.8795,
            "range": "± 3.06571",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.275,
            "range": "± 3.49937",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 19.8876,
            "range": "± 3.78642",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.3598,
            "range": "± 3.01378",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.1425,
            "range": "± 3.85308",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 17.3628,
            "range": "± 4.10692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.2636,
            "range": "± 4.68784",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.3902,
            "range": "± 3.26792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.6219,
            "range": "± 3.72122",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.8066,
            "range": "± 1.80086",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.441,
            "range": "± 4.56336",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 192.69,
            "range": "± 13.9275",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 43.3099,
            "range": "± 8.11926",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 308.235,
            "range": "± 20.6476",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.3503,
            "range": "± 4.20456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.6174,
            "range": "± 5.95023",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 47.1767,
            "range": "± 8.86768",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 28.5431,
            "range": "± 6.64092",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 47.6898,
            "range": "± 11.6867",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.398,
            "range": "± 6.65969",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 33.9816,
            "range": "± 6.23328",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.1937,
            "range": "± 5.75872",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 30.6312,
            "range": "± 5.78251",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.6682,
            "range": "± 7.13123",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 95.8113,
            "range": "± 12.7893",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1051.25,
            "range": "± 16441.9",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 147.162,
            "range": "± 14.955",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.19622,
            "range": "± 72.4231",
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
          "id": "13d30890af186e2ed0376c40776752ab904c74d6",
          "message": "Fix CUDA backend: identify host/device functions via IR attributes (#263)",
          "timestamp": "2026-04-25T23:16:16+02:00",
          "tree_id": "11675af93b149bc35ef18cb83819133107763602",
          "url": "https://github.com/nebulastream/nautilus/commit/13d30890af186e2ed0376c40776752ab904c74d6"
        },
        "date": 1777152075070,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_bc_addOne",
            "value": 35.9021,
            "range": "± 1.1682",
            "unit": "ns",
            "extra": "100 samples\n856 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 305.081,
            "range": "± 11.1843",
            "unit": "ns",
            "extra": "100 samples\n8613 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.20918,
            "range": "± 0.627832",
            "unit": "ns",
            "extra": "100 samples\n7919 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.0296,
            "range": "± 2.48304",
            "unit": "ns",
            "extra": "100 samples\n744 iterations"
          },
          {
            "name": "ir_add",
            "value": 709.633,
            "range": "± 41.9481",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.52388,
            "range": "± 99.8474",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.46594,
            "range": "± 313.008",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.65312,
            "range": "± 145.092",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.93951,
            "range": "± 285.184",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.78409,
            "range": "± 133.666",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01591,
            "range": "± 143.926",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.3543,
            "range": "± 71.3206",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.77526,
            "range": "± 117.473",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.45312,
            "range": "± 90.0712",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.92445,
            "range": "± 584.986",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.0222,
            "range": "± 3.50703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.7568,
            "range": "± 867.842",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 182.354,
            "range": "± 5.47747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 177.702,
            "range": "± 7.19669",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 432.887,
            "range": "± 29.8481",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.24007,
            "range": "± 306.122",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 456.521,
            "range": "± 34.8997",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 867.29,
            "range": "± 54.9955",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 462.775,
            "range": "± 26.2413",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 559.475,
            "range": "± 30.7388",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 462.337,
            "range": "± 21.216",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 474.266,
            "range": "± 28.0694",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 416.026,
            "range": "± 27.8588",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.30816,
            "range": "± 221.899",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.2639,
            "range": "± 213.499",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.48001,
            "range": "± 1.62181",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.5202,
            "range": "± 520.469",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.8484,
            "range": "± 3.64799",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.885,
            "range": "± 1.97283",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.31457,
            "range": "± 1.34835",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.64205,
            "range": "± 579.064",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.3724,
            "range": "± 3.26305",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.48641,
            "range": "± 1.22458",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.37684,
            "range": "± 1.45777",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.79798,
            "range": "± 708.86",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.9892,
            "range": "± 2.27989",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.2479,
            "range": "± 1.7374",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.46211,
            "range": "± 890.535",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.47262,
            "range": "± 900.713",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.59929,
            "range": "± 1.31667",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.99547,
            "range": "± 667.202",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.56361,
            "range": "± 1.02127",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.82471,
            "range": "± 403.351",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.2904,
            "range": "± 7.50005",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.0729,
            "range": "± 7.81706",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.47913,
            "range": "± 34.5694",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.48976,
            "range": "± 29.3028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.1748,
            "range": "± 11.7272",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.0174,
            "range": "± 10.0785",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.5256,
            "range": "± 141.13",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.31372,
            "range": "± 30.7835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.0893,
            "range": "± 17.9752",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.59559,
            "range": "± 122.555",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.4467,
            "range": "± 331.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.203,
            "range": "± 16.3946",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 61.704,
            "range": "± 17.072",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.61342,
            "range": "± 148.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.3216,
            "range": "± 310.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 63.3109,
            "range": "± 18.5419",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.9793,
            "range": "± 1.14099",
            "unit": "ns",
            "extra": "100 samples\n8832 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.6223,
            "range": "± 2.15963",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 543.961,
            "range": "± 61.2852",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.51991,
            "range": "± 0.514147",
            "unit": "ns",
            "extra": "100 samples\n7246 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 107.059,
            "range": "± 4.45223",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5533,
            "range": "± 116.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.5913,
            "range": "± 3.92889",
            "unit": "ns",
            "extra": "100 samples\n722 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 677.644,
            "range": "± 10.0958",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 156.901,
            "range": "± 405.013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.52569,
            "range": "± 0.250923",
            "unit": "ns",
            "extra": "100 samples\n8829 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.5611,
            "range": "± 3.00886",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29288,
            "range": "± 20.3449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.4519,
            "range": "± 2.53192",
            "unit": "ns",
            "extra": "100 samples\n719 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.2186,
            "range": "± 5.82844",
            "unit": "ns",
            "extra": "100 samples\n717 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 685.257,
            "range": "± 20.3716",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 679.894,
            "range": "± 18.866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 156.884,
            "range": "± 437.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 156.91,
            "range": "± 389.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 44.9885,
            "range": "± 14.9511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.10904,
            "range": "± 153.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.156,
            "range": "± 157.698",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.74269,
            "range": "± 162.581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.6081,
            "range": "± 135.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.75988,
            "range": "± 173.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.1776,
            "range": "± 208.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.4592,
            "range": "± 179.244",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.2178,
            "range": "± 211.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.56878,
            "range": "± 138.768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.144,
            "range": "± 168.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.97434,
            "range": "± 174.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.0325,
            "range": "± 164.639",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.2958,
            "range": "± 164.657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.2674,
            "range": "± 163.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.9186,
            "range": "± 192.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.6378,
            "range": "± 275.425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.1893,
            "range": "± 292.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.2797,
            "range": "± 273.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.3619,
            "range": "± 316.678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.4808,
            "range": "± 1.21598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.695,
            "range": "± 366.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.7832,
            "range": "± 270.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.9003,
            "range": "± 269.573",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6222,
            "range": "± 707.015",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.2069,
            "range": "± 294.022",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.3589,
            "range": "± 279.494",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.6043,
            "range": "± 361.771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.723,
            "range": "± 280.841",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.575,
            "range": "± 1.55263",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.2848,
            "range": "± 2.07335",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.6459,
            "range": "± 3.65676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.2789,
            "range": "± 3.90828",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.1183,
            "range": "± 2.53876",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.7999,
            "range": "± 3.95396",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.8599,
            "range": "± 4.20688",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 18.1012,
            "range": "± 4.16197",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.377,
            "range": "± 3.58766",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.0196,
            "range": "± 4.33979",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.0268,
            "range": "± 2.48101",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 30.9154,
            "range": "± 6.02429",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.738,
            "range": "± 9.42572",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.7196,
            "range": "± 8.34616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 302.309,
            "range": "± 13.1774",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.3136,
            "range": "± 4.9683",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.9575,
            "range": "± 6.07966",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 49.558,
            "range": "± 12.19",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.1257,
            "range": "± 6.18348",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.5736,
            "range": "± 12.7948",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.3175,
            "range": "± 6.64558",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.7806,
            "range": "± 5.68519",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.6859,
            "range": "± 4.57706",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 32.4173,
            "range": "± 6.04141",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.8756,
            "range": "± 6.02228",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 95.9749,
            "range": "± 15.2463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1035.97,
            "range": "± 18995.5",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 148.785,
            "range": "± 14.5985",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.18428,
            "range": "± 36.4572",
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
          "id": "63d7373fcff141e65bca2d458b539bf1d07bdd01",
          "message": "Carry trace-time source locations onto IR operations (#271)",
          "timestamp": "2026-04-26T16:02:49+02:00",
          "tree_id": "9675ec81345a6651480b2e4fd8d09a230c0c5b92",
          "url": "https://github.com/nebulastream/nautilus/commit/63d7373fcff141e65bca2d458b539bf1d07bdd01"
        },
        "date": 1777212575868,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 185.47,
            "range": "± 10.8583",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 466.434,
            "range": "± 28.938",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.15114,
            "range": "± 103.312",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 505.081,
            "range": "± 57.0378",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 895.76,
            "range": "± 69.727",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 505.763,
            "range": "± 47.5456",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 609.238,
            "range": "± 40.5294",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 502.653,
            "range": "± 57.8452",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 519.293,
            "range": "± 36.0981",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 456.287,
            "range": "± 29.3576",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.1677,
            "range": "± 3.32233",
            "unit": "ns",
            "extra": "100 samples\n783 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 282.01,
            "range": "± 4.78701",
            "unit": "ns",
            "extra": "100 samples\n8277 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.01205,
            "range": "± 0.720511",
            "unit": "ns",
            "extra": "100 samples\n7702 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3869,
            "range": "± 1.95004",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.26384,
            "range": "± 134.846",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.26676,
            "range": "± 148.007",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.61794,
            "range": "± 723.043",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.51253,
            "range": "± 329.207",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.896,
            "range": "± 1.68439",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.4744,
            "range": "± 995.991",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.59519,
            "range": "± 749.263",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.58826,
            "range": "± 369.418",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.7598,
            "range": "± 1.62139",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.3764,
            "range": "± 684.152",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.67972,
            "range": "± 776.849",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.76406,
            "range": "± 361.804",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.0504,
            "range": "± 1.74909",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.92695,
            "range": "± 796.703",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.43972,
            "range": "± 1.08414",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.45259,
            "range": "± 1.92793",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1069,
            "range": "± 1.94061",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.9474,
            "range": "± 672.844",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.83767,
            "range": "± 825.448",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.87538,
            "range": "± 299.988",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.3315,
            "range": "± 4.80182",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.5176,
            "range": "± 4.55114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39058,
            "range": "± 15.8534",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.39501,
            "range": "± 16.4655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.8898,
            "range": "± 4.19717",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.2811,
            "range": "± 4.67785",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.49324,
            "range": "± 78.4984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.32025,
            "range": "± 19.5336",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 11.0137,
            "range": "± 1.26432",
            "unit": "ns",
            "extra": "100 samples\n8429 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.8357,
            "range": "± 1.72768",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 515.951,
            "range": "± 15.914",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.68672,
            "range": "± 0.648729",
            "unit": "ns",
            "extra": "100 samples\n6486 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.3115,
            "range": "± 10.4835",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.093,
            "range": "± 638.815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.3895,
            "range": "± 6.77672",
            "unit": "ns",
            "extra": "100 samples\n679 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 845.409,
            "range": "± 31.7346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 209.77,
            "range": "± 30.6229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.46416,
            "range": "± 0.282446",
            "unit": "ns",
            "extra": "100 samples\n8452 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.6293,
            "range": "± 2.13057",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.85394,
            "range": "± 30.3596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.9212,
            "range": "± 6.36911",
            "unit": "ns",
            "extra": "100 samples\n684 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.6164,
            "range": "± 4.02327",
            "unit": "ns",
            "extra": "100 samples\n674 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 842.767,
            "range": "± 12.0415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 863.738,
            "range": "± 111.008",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 180.236,
            "range": "± 1.96681",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 181.219,
            "range": "± 9.35965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 54.8657,
            "range": "± 7.52175",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.52498,
            "range": "± 123.995",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.5262,
            "range": "± 56.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.16488,
            "range": "± 105.145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.04256,
            "range": "± 74.5663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.08001,
            "range": "± 61.7236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.3249,
            "range": "± 150.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.7015,
            "range": "± 113.658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.5447,
            "range": "± 381.342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.02122,
            "range": "± 162.478",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.4735,
            "range": "± 377.84",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.37037,
            "range": "± 138.504",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.5865,
            "range": "± 229.526",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.6768,
            "range": "± 589.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.48194,
            "range": "± 62.9605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 19.959,
            "range": "± 119.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6217,
            "range": "± 308.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.0222,
            "range": "± 293.11",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.0805,
            "range": "± 233.763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3597,
            "range": "± 307.485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2746,
            "range": "± 321.685",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.6312,
            "range": "± 298.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.9335,
            "range": "± 569.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.0072,
            "range": "± 241.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.3594,
            "range": "± 247.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.1837,
            "range": "± 291.232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.1453,
            "range": "± 280.597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.7491,
            "range": "± 591.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.6895,
            "range": "± 430.891",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.2305,
            "range": "± 903.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4327,
            "range": "± 2.07884",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.258,
            "range": "± 2.77051",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.1165,
            "range": "± 2.99661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.0177,
            "range": "± 2.66056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.9003,
            "range": "± 3.01096",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.8844,
            "range": "± 3.44792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8893,
            "range": "± 3.25031",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.9117,
            "range": "± 2.41531",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.5955,
            "range": "± 3.04047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.8936,
            "range": "± 2.97678",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.051,
            "range": "± 3.55238",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 199.719,
            "range": "± 11.7792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.663,
            "range": "± 6.30623",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 305.604,
            "range": "± 13.5113",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.3122,
            "range": "± 3.65446",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 31.9878,
            "range": "± 3.03052",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 56.1641,
            "range": "± 7.28523",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.8279,
            "range": "± 4.21294",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 56.2124,
            "range": "± 6.48303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.9087,
            "range": "± 5.86418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.1392,
            "range": "± 8.29272",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3256,
            "range": "± 5.54004",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.5538,
            "range": "± 6.77694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.3763,
            "range": "± 8.65323",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 102.121,
            "range": "± 7.80504",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1051.8400000000001,
            "range": "± 16503.1",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 155.645,
            "range": "± 13.2491",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.15925,
            "range": "± 31.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 53.9144,
            "range": "± 8.01711",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.39304,
            "range": "± 616.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.3788,
            "range": "± 333.861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.1421,
            "range": "± 9.76956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 72.8987,
            "range": "± 8.33971",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.29028,
            "range": "± 76.8287",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.3196,
            "range": "± 245.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 74.643,
            "range": "± 9.1889",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 732.22,
            "range": "± 43.8461",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.57536,
            "range": "± 118.379",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.31725,
            "range": "± 225.906",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.59726,
            "range": "± 137.68",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.76982,
            "range": "± 169.109",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.73728,
            "range": "± 96.9088",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.95676,
            "range": "± 180.825",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.41797,
            "range": "± 180.493",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.67703,
            "range": "± 82.6636",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.4574,
            "range": "± 156.616",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.6256,
            "range": "± 445.441",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 91.7696,
            "range": "± 6.65442",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.669,
            "range": "± 722.9",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 171.11,
            "range": "± 8.67329",
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
          "id": "dc99ff67846069bcd31420a741057dd4626246ec",
          "message": "Add ENABLE_SHORT_CIRCUIT_BOOL option for short-circuit && / || (#273)",
          "timestamp": "2026-04-26T16:36:34+02:00",
          "tree_id": "c2e1d2fa6bbfb3f9d46bee9efcaee4ac165d5d6d",
          "url": "https://github.com/nebulastream/nautilus/commit/dc99ff67846069bcd31420a741057dd4626246ec"
        },
        "date": 1777214608967,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.88715,
            "range": "± 156.394",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 1.87233,
            "range": "± 140.493",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 7.89554,
            "range": "± 1.30569",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.87661,
            "range": "± 459.575",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 23.4754,
            "range": "± 2.33951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 10.834,
            "range": "± 908.732",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.81114,
            "range": "± 909.761",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.07866,
            "range": "± 560.237",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.3587,
            "range": "± 1.85009",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.42382,
            "range": "± 1.23935",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 7.94756,
            "range": "± 1.14457",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.10163,
            "range": "± 266.166",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 11.9972,
            "range": "± 1.25241",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 8.36639,
            "range": "± 764.998",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 6.46907,
            "range": "± 375.115",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 6.51088,
            "range": "± 425.037",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 8.93424,
            "range": "± 589.798",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.21802,
            "range": "± 732.807",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.24667,
            "range": "± 974.573",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.46068,
            "range": "± 353.775",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 32.6852,
            "range": "± 3.46448",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 33.0774,
            "range": "± 3.17071",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.3007,
            "range": "± 12.8755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.32384,
            "range": "± 17.9172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 90.0147,
            "range": "± 8.98255",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 43.6389,
            "range": "± 7.73664",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.02401,
            "range": "± 23.8633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.12331,
            "range": "± 21.8239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.74575,
            "range": "± 168.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.4353,
            "range": "± 304.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 6.27329,
            "range": "± 161.956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.58429,
            "range": "± 216.404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.091,
            "range": "± 288.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.8299,
            "range": "± 191.442",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.2958,
            "range": "± 295.64",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 6.19563,
            "range": "± 188.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.7443,
            "range": "± 184.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.415,
            "range": "± 200.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.4977,
            "range": "± 260.306",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.8696,
            "range": "± 353.174",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.155,
            "range": "± 485.12",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 60.7139,
            "range": "± 320.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 23.3483,
            "range": "± 388.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 23.7386,
            "range": "± 338.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.128,
            "range": "± 407.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.0156,
            "range": "± 420.144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 24.8608,
            "range": "± 367.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 24.2742,
            "range": "± 370.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 24.5702,
            "range": "± 386.249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 23.8852,
            "range": "± 347.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 24.0337,
            "range": "± 409.265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.0026,
            "range": "± 778.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 26.7036,
            "range": "± 321.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 59.5103,
            "range": "± 609.644",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 28.9074,
            "range": "± 422.257",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 88.7346,
            "range": "± 814.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 10.1865,
            "range": "± 1.71744",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.3242,
            "range": "± 2.35422",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 17.2133,
            "range": "± 2.85723",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 12.7378,
            "range": "± 2.70056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 15.0359,
            "range": "± 2.7018",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.2146,
            "range": "± 2.42466",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 14.0688,
            "range": "± 2.4589",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.3784,
            "range": "± 2.49359",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.1138,
            "range": "± 2.65115",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 12.2534,
            "range": "± 2.26341",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.4725,
            "range": "± 5.379",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 199.632,
            "range": "± 11.0866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 39.7335,
            "range": "± 5.33065",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 305.117,
            "range": "± 12.0584",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 15.0271,
            "range": "± 4.12282",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.1317,
            "range": "± 7.3765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 42.8425,
            "range": "± 7.49401",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.3445,
            "range": "± 8.03976",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 41.6922,
            "range": "± 7.33422",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 31.9392,
            "range": "± 7.63028",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.4745,
            "range": "± 7.03213",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 22.8331,
            "range": "± 5.89124",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.7678,
            "range": "± 7.26336",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.4722,
            "range": "± 7.53366",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 85.3111,
            "range": "± 9.72798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 984.753,
            "range": "± 28.503",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 134.775,
            "range": "± 12.9576",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.13539,
            "range": "± 118.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 12.3715,
            "range": "± 1.11485",
            "unit": "ns",
            "extra": "100 samples\n6265 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 27.2559,
            "range": "± 2.07465",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 540.61,
            "range": "± 19.7788",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.25656,
            "range": "± 0.650045",
            "unit": "ns",
            "extra": "100 samples\n3818 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 130.951,
            "range": "± 5.15779",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 25.3331,
            "range": "± 122.454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 36.0451,
            "range": "± 2.35617",
            "unit": "ns",
            "extra": "100 samples\n515 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 551.221,
            "range": "± 12.3942",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 118.106,
            "range": "± 2.12142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.92109,
            "range": "± 0.132287",
            "unit": "ns",
            "extra": "100 samples\n6286 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 28.6858,
            "range": "± 5.36548",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 6.06047,
            "range": "± 55.0682",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 36.3339,
            "range": "± 4.2149",
            "unit": "ns",
            "extra": "100 samples\n507 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 36.8861,
            "range": "± 6.48166",
            "unit": "ns",
            "extra": "100 samples\n506 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 550.224,
            "range": "± 9.60884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 553.127,
            "range": "± 13.5432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 117.817,
            "range": "± 1.80426",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 117.744,
            "range": "± 1.65366",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 33.7678,
            "range": "± 10.7893",
            "unit": "ns",
            "extra": "100 samples\n548 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 422.81,
            "range": "± 11.3708",
            "unit": "ns",
            "extra": "100 samples\n6194 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.65339,
            "range": "± 0.324812",
            "unit": "ns",
            "extra": "100 samples\n5672 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 36.3663,
            "range": "± 2.01444",
            "unit": "ns",
            "extra": "100 samples\n478 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 37.3993,
            "range": "± 10.5748",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.71856,
            "range": "± 126.948",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 22.9516,
            "range": "± 421.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 38.0715,
            "range": "± 9.42079",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 55.1349,
            "range": "± 12.5935",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.6831,
            "range": "± 135.743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 23.7765,
            "range": "± 488.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 58.7315,
            "range": "± 13.1658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 192.023,
            "range": "± 18.772",
            "unit": "ns",
            "extra": "100 samples\n97 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 451.182,
            "range": "± 30.5224",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.4254,
            "range": "± 252.698",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 488.819,
            "range": "± 54.6127",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1052.08,
            "range": "± 96280",
            "unit": "ns",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 504.765,
            "range": "± 45.3522",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 600.767,
            "range": "± 34.825",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 465.574,
            "range": "± 34.2829",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 511.093,
            "range": "± 42.6039",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 453.689,
            "range": "± 47.4179",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ir_add",
            "value": 819.498,
            "range": "± 72.6208",
            "unit": "ns",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.69289,
            "range": "± 114.732",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.72447,
            "range": "± 235.957",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.76261,
            "range": "± 124.888",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.22553,
            "range": "± 206.271",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.02652,
            "range": "± 133.994",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.151,
            "range": "± 111.019",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.55551,
            "range": "± 119.597",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.88514,
            "range": "± 140.152",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.61542,
            "range": "± 125.807",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.59845,
            "range": "± 454.674",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 94.955,
            "range": "± 4.79425",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 13.3264,
            "range": "± 918.952",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 169.743,
            "range": "± 3.75303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 38.7065,
            "range": "± 12.4613",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.79604,
            "range": "± 184.273",
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
          "id": "0ffec01c3c0170e4c70c4533adf49531af5077b6",
          "message": "Add control-flow graph view to the Nautilus IR VS Code extension (#270)",
          "timestamp": "2026-04-26T17:12:23+02:00",
          "tree_id": "a5c059348f5d212e8588a6c9935700c0d86d062f",
          "url": "https://github.com/nebulastream/nautilus/commit/0ffec01c3c0170e4c70c4533adf49531af5077b6"
        },
        "date": 1777216638565,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.18982,
            "range": "± 187.756",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.22448,
            "range": "± 393.004",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.64446,
            "range": "± 697.498",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.34098,
            "range": "± 404.891",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.0545,
            "range": "± 1.76486",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.8218,
            "range": "± 1.09712",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.44667,
            "range": "± 926.948",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.465,
            "range": "± 591.802",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.8209,
            "range": "± 2.05085",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.08751,
            "range": "± 975.148",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.48671,
            "range": "± 840.278",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.36908,
            "range": "± 410",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.2025,
            "range": "± 1.30529",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.44364,
            "range": "± 1.18662",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.20977,
            "range": "± 701.217",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.14302,
            "range": "± 705.936",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.56472,
            "range": "± 707.034",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.8311,
            "range": "± 698.883",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.64847,
            "range": "± 605.911",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.75943,
            "range": "± 311.686",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.8854,
            "range": "± 10.9115",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.3531,
            "range": "± 5.18513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.33397,
            "range": "± 38.0818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.34967,
            "range": "± 30.5333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.8947,
            "range": "± 8.50794",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 46.5412,
            "range": "± 4.02544",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.41029,
            "range": "± 30.9476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.1957,
            "range": "± 46.0778",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.8628,
            "range": "± 8.01142",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.1977,
            "range": "± 46.6998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.2926,
            "range": "± 487.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.7482,
            "range": "± 7.19672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 76.8317,
            "range": "± 11.6772",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.18857,
            "range": "± 128.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.1951,
            "range": "± 421.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 74.6329,
            "range": "± 8.47303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 727.88,
            "range": "± 43.4906",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.55175,
            "range": "± 165.429",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.3041,
            "range": "± 369.714",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.56329,
            "range": "± 74.723",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.81943,
            "range": "± 265.652",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.75574,
            "range": "± 147.031",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.01545,
            "range": "± 334.003",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.44899,
            "range": "± 104.141",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.68009,
            "range": "± 96.785",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.5104,
            "range": "± 232.778",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.87415,
            "range": "± 613.7",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 90.7636,
            "range": "± 5.93156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.9416,
            "range": "± 1.80149",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 168.779,
            "range": "± 8.18724",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.2733,
            "range": "± 1.33059",
            "unit": "ns",
            "extra": "100 samples\n8442 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.0577,
            "range": "± 947.972",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 517.329,
            "range": "± 14.4095",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.82034,
            "range": "± 0.981281",
            "unit": "ns",
            "extra": "100 samples\n6563 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.4909,
            "range": "± 6.56309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9955,
            "range": "± 99.6003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 45.2538,
            "range": "± 7.37928",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 841.501,
            "range": "± 14.0702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.992,
            "range": "± 584.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.46621,
            "range": "± 0.287861",
            "unit": "ns",
            "extra": "100 samples\n8454 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.6623,
            "range": "± 1.71176",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.84254,
            "range": "± 13.6746",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.0388,
            "range": "± 4.59466",
            "unit": "ns",
            "extra": "100 samples\n680 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.8532,
            "range": "± 4.51833",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 844.052,
            "range": "± 15.3452",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 843.118,
            "range": "± 14.3439",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 179.012,
            "range": "± 301.445",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 178.947,
            "range": "± 298.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 199.178,
            "range": "± 15.5178",
            "unit": "ns",
            "extra": "100 samples\n152 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 478.177,
            "range": "± 33.3409",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.21775,
            "range": "± 171.163",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 507.088,
            "range": "± 32.9499",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 946.462,
            "range": "± 74.6908",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 521.143,
            "range": "± 44.1402",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 631.864,
            "range": "± 55.7045",
            "unit": "ns",
            "extra": "100 samples\n52 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 506.542,
            "range": "± 36.0582",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 518.477,
            "range": "± 12.881",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 460.667,
            "range": "± 27.7852",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.299,
            "range": "± 6.40189",
            "unit": "ns",
            "extra": "100 samples\n844 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 276.65,
            "range": "± 6.54964",
            "unit": "ns",
            "extra": "100 samples\n8271 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.94038,
            "range": "± 0.513396",
            "unit": "ns",
            "extra": "100 samples\n7714 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3386,
            "range": "± 1.93248",
            "unit": "ns",
            "extra": "100 samples\n757 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.40013,
            "range": "± 122.264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 5.96441,
            "range": "± 107.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.93748,
            "range": "± 117.794",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 6.92046,
            "range": "± 130.579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 27.8148,
            "range": "± 433.533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6022,
            "range": "± 635.829",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 26.7999,
            "range": "± 147.177",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.91213,
            "range": "± 148.387",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.1285,
            "range": "± 81.9816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.16004,
            "range": "± 86.1717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.2128,
            "range": "± 98.399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.0339,
            "range": "± 302.071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.2945,
            "range": "± 127.157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 19.6223,
            "range": "± 134.326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6545,
            "range": "± 481.731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.1852,
            "range": "± 374.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.0505,
            "range": "± 314.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3145,
            "range": "± 377.774",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.1374,
            "range": "± 318.435",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5951,
            "range": "± 292.818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.615,
            "range": "± 302.896",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.8811,
            "range": "± 305.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.353,
            "range": "± 422.45",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3132,
            "range": "± 431.055",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.3236,
            "range": "± 469.705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.6278,
            "range": "± 667.077",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.5583,
            "range": "± 485.619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.0721,
            "range": "± 451.987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3337,
            "range": "± 2.01825",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 17.5959,
            "range": "± 2.13998",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 21.4367,
            "range": "± 2.9043",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.8336,
            "range": "± 3.09282",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.5696,
            "range": "± 3.75195",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.2932,
            "range": "± 2.04367",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.2575,
            "range": "± 2.92659",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.6001,
            "range": "± 4.49766",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.1862,
            "range": "± 2.24005",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.4518,
            "range": "± 2.62535",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 33.3669,
            "range": "± 3.88272",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.495,
            "range": "± 9.87401",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 48.2883,
            "range": "± 7.9286",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 298.514,
            "range": "± 17.8381",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 18.9904,
            "range": "± 2.61791",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 30.0614,
            "range": "± 3.12105",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 52.4358,
            "range": "± 5.54008",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 32.4276,
            "range": "± 4.22956",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 52.9854,
            "range": "± 6.99703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 41.9688,
            "range": "± 7.38545",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 44.844,
            "range": "± 5.89949",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 25.6842,
            "range": "± 3.68592",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 34.9043,
            "range": "± 4.73136",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 32.8503,
            "range": "± 5.33044",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 99.3643,
            "range": "± 11.9739",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1011.6899999999999,
            "range": "± 15524.800000000001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 151.116,
            "range": "± 14.1025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.12703,
            "range": "± 34.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 54.0015,
            "range": "± 7.66429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.39858,
            "range": "± 264.626",
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
          "id": "651cdefad5a0479d10977f3d0ed4033d1e51c88c",
          "message": "Fix VS Code extension: bundle debugAdapter.ts with esbuild to include @vscode/debugadapter\n\nThe extension was failing to activate because debugAdapter.js required\n@vscode/debugadapter at runtime, but the package was moved from dependencies\nto devDependencies without adding a separate esbuild bundle step for the\ndebug adapter process. The bundle script now builds both entry points.\n\nCo-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-04-26T17:49:34+02:00",
          "tree_id": "0ab3c65d4b4ba13360395f82a7ca3fd58f312642",
          "url": "https://github.com/nebulastream/nautilus/commit/651cdefad5a0479d10977f3d0ed4033d1e51c88c"
        },
        "date": 1777218934573,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.5617,
            "range": "± 0.791705",
            "unit": "ns",
            "extra": "100 samples\n8801 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.8603,
            "range": "± 1.622",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 561.956,
            "range": "± 62.0463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.23499,
            "range": "± 1.38814",
            "unit": "ns",
            "extra": "100 samples\n7203 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 108.142,
            "range": "± 6.44045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5716,
            "range": "± 68.2459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.7707,
            "range": "± 5.59085",
            "unit": "ns",
            "extra": "100 samples\n496 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 682.614,
            "range": "± 37.6757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 158.674,
            "range": "± 8.42591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.54284,
            "range": "± 0.268783",
            "unit": "ns",
            "extra": "100 samples\n8810 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.2787,
            "range": "± 1.86563",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29175,
            "range": "± 32.8696",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.2892,
            "range": "± 4.70843",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.6503,
            "range": "± 4.75153",
            "unit": "ns",
            "extra": "100 samples\n722 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 680.134,
            "range": "± 15.8751",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 676.818,
            "range": "± 9.25927",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 157.843,
            "range": "± 1.32747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 157.556,
            "range": "± 971.527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 179.278,
            "range": "± 22.6692",
            "unit": "ns",
            "extra": "100 samples\n175 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 433.297,
            "range": "± 24.811",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10603,
            "range": "± 70.404",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 452.156,
            "range": "± 26.079",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 877.52,
            "range": "± 52.8667",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 566.972,
            "range": "± 183.16",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 565.551,
            "range": "± 30.9565",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 465.894,
            "range": "± 24.1451",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 471.844,
            "range": "± 28.8638",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 422.42,
            "range": "± 35.3335",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.28478,
            "range": "± 193.041",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.27526,
            "range": "± 190.284",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.31724,
            "range": "± 1.60194",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.52264,
            "range": "± 492.402",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.3612,
            "range": "± 3.21405",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5478,
            "range": "± 1.60955",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.14236,
            "range": "± 1.33603",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.59446,
            "range": "± 535.705",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.1145,
            "range": "± 3.28102",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.40583,
            "range": "± 1.1458",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.29543,
            "range": "± 1.4451",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.69248,
            "range": "± 545.469",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.6368,
            "range": "± 2.42292",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.0429,
            "range": "± 1.27491",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.37292,
            "range": "± 835.362",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.4982,
            "range": "± 707.028",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.67363,
            "range": "± 1.42768",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.0313,
            "range": "± 848.962",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.46414,
            "range": "± 1.21754",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.82658,
            "range": "± 389.254",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.4912,
            "range": "± 6.58616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.0937,
            "range": "± 6.81027",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.45893,
            "range": "± 29.5134",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.46792,
            "range": "± 32.2068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 95.2784,
            "range": "± 12.1069",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.3746,
            "range": "± 8.69424",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.55689,
            "range": "± 126.844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.29734,
            "range": "± 44.059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 749.023,
            "range": "± 43.0076",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.57978,
            "range": "± 90.0082",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.33528,
            "range": "± 187.51",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.66135,
            "range": "± 94.2898",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.86001,
            "range": "± 255.321",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.82339,
            "range": "± 119.169",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98234,
            "range": "± 85.3355",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45197,
            "range": "± 120.595",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.85269,
            "range": "± 285.212",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.53705,
            "range": "± 143.313",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.68701,
            "range": "± 362.946",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.7044,
            "range": "± 2.87152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.4945,
            "range": "± 678.651",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 162.755,
            "range": "± 6.85308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.26142,
            "range": "± 233.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.9729,
            "range": "± 252.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.64642,
            "range": "± 163.826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.863,
            "range": "± 219.082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.4365,
            "range": "± 406.056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.5784,
            "range": "± 270.747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.3952,
            "range": "± 375.874",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.64765,
            "range": "± 237.329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.191,
            "range": "± 218.219",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0184,
            "range": "± 229.324",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.1573,
            "range": "± 288.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.4429,
            "range": "± 995.489",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3788,
            "range": "± 241.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.1343,
            "range": "± 215.358",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.0005,
            "range": "± 360.468",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.7378,
            "range": "± 602.228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.6723,
            "range": "± 399.632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.6405,
            "range": "± 278.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.8125,
            "range": "± 429.883",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.162,
            "range": "± 790.658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.172,
            "range": "± 443.51",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.4717,
            "range": "± 419.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.8591,
            "range": "± 588.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.9724,
            "range": "± 581.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.1285,
            "range": "± 652.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.7417,
            "range": "± 1.04487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.4974,
            "range": "± 589.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.6642,
            "range": "± 881.22",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4016,
            "range": "± 2.72981",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.5574,
            "range": "± 3.83995",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.0583,
            "range": "± 3.7402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.4561,
            "range": "± 3.20537",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.9611,
            "range": "± 3.92456",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.8137,
            "range": "± 3.67381",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.619,
            "range": "± 3.74376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.6925,
            "range": "± 3.58324",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 16.8324,
            "range": "± 3.73012",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 15.8644,
            "range": "± 2.49605",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 29.9517,
            "range": "± 4.22252",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.376,
            "range": "± 12.2432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.7394,
            "range": "± 7.9142",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 303.676,
            "range": "± 14.8927",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.5067,
            "range": "± 4.87572",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.5766,
            "range": "± 5.42466",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 47.9459,
            "range": "± 12.2851",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 28.3678,
            "range": "± 5.53453",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.0974,
            "range": "± 11.1934",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.0453,
            "range": "± 6.63753",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.2252,
            "range": "± 5.95554",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.6582,
            "range": "± 5.4189",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 30.9108,
            "range": "± 5.75564",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 28.1275,
            "range": "± 6.47573",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 94.5524,
            "range": "± 13.1086",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1036.6200000000001,
            "range": "± 19366.7",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 146.984,
            "range": "± 18.2413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.21645,
            "range": "± 22.6791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 44.5672,
            "range": "± 15.147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.59025,
            "range": "± 119.063",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.8,
            "range": "± 423.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.812,
            "range": "± 16.4375",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 61.4381,
            "range": "± 17.2025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.68219,
            "range": "± 158.655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.0078,
            "range": "± 559.442",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 62.4231,
            "range": "± 17.2653",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 43.7666,
            "range": "± 15.238",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.11678,
            "range": "± 153.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 35.918,
            "range": "± 3.59784",
            "unit": "ns",
            "extra": "100 samples\n856 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 301.366,
            "range": "± 7.05427",
            "unit": "ns",
            "extra": "100 samples\n8593 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 4.09714,
            "range": "± 0.329101",
            "unit": "ns",
            "extra": "100 samples\n7910 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 42.2788,
            "range": "± 2.12339",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
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
          "id": "fbd000841f03892c0455d8d530d1d8ca6260ff52",
          "message": "Add DWARF debug info for MLIR-backend JIT'd code (#265)",
          "timestamp": "2026-04-26T21:30:54+02:00",
          "tree_id": "27dda9e19429f89afcb2c43942576f0dc599c10b",
          "url": "https://github.com/nebulastream/nautilus/commit/fbd000841f03892c0455d8d530d1d8ca6260ff52"
        },
        "date": 1777232202184,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.377,
            "range": "± 172.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.01691,
            "range": "± 248.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.78417,
            "range": "± 190.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.35393,
            "range": "± 324.38",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2604,
            "range": "± 582.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.7503,
            "range": "± 263.994",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5681,
            "range": "± 948.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.64512,
            "range": "± 145.239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.307,
            "range": "± 187.951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.1921,
            "range": "± 258.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.3222,
            "range": "± 231.663",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.7072,
            "range": "± 515.304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.7337,
            "range": "± 406.502",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.7938,
            "range": "± 565.933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.7531,
            "range": "± 408.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.4886,
            "range": "± 1.18424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.5467,
            "range": "± 432.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.1782,
            "range": "± 4.03728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.672,
            "range": "± 404.492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.0444,
            "range": "± 513.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.8736,
            "range": "± 550.805",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.3266,
            "range": "± 504.336",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.83,
            "range": "± 600.848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.6621,
            "range": "± 571.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.517,
            "range": "± 1.02842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.1716,
            "range": "± 1.53225",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.5113,
            "range": "± 452.851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.182,
            "range": "± 436.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.3438,
            "range": "± 2.26851",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.1772,
            "range": "± 2.83898",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 20.3351,
            "range": "± 4.1405",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.6135,
            "range": "± 3.54299",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 18.7632,
            "range": "± 3.64657",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.8706,
            "range": "± 3.56488",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.8911,
            "range": "± 3.89168",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.4539,
            "range": "± 3.34504",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.0151,
            "range": "± 4.24733",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.3401,
            "range": "± 3.44193",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.9,
            "range": "± 5.1301",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 200.356,
            "range": "± 10.6207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 44.7201,
            "range": "± 10.3866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 315.43,
            "range": "± 12.9589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.3357,
            "range": "± 5.7874",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 27.4672,
            "range": "± 7.46295",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.3402,
            "range": "± 10.4588",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.0349,
            "range": "± 5.82163",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 49.227,
            "range": "± 11.1378",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.377,
            "range": "± 6.76528",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.1061,
            "range": "± 5.90692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.0122,
            "range": "± 5.12412",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.5818,
            "range": "± 5.64908",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.06,
            "range": "± 6.34809",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 96.5661,
            "range": "± 12.9167",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1070.25,
            "range": "± 74274.6",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 151.378,
            "range": "± 17.7476",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.27398,
            "range": "± 236.764",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.7525,
            "range": "± 0.679524",
            "unit": "ns",
            "extra": "100 samples\n8803 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 16.9187,
            "range": "± 1.67676",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 560.379,
            "range": "± 61.6932",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45106,
            "range": "± 0.302066",
            "unit": "ns",
            "extra": "100 samples\n7140 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 109.859,
            "range": "± 8.67151",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.6387,
            "range": "± 305.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.1067,
            "range": "± 7.20227",
            "unit": "ns",
            "extra": "100 samples\n724 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 625.821,
            "range": "± 11.0899",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 141.515,
            "range": "± 1.57658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.57412,
            "range": "± 0.404158",
            "unit": "ns",
            "extra": "100 samples\n8817 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 22.2244,
            "range": "± 1.57474",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.29957,
            "range": "± 34.5193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.5689,
            "range": "± 5.76884",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.7931,
            "range": "± 6.39022",
            "unit": "ns",
            "extra": "100 samples\n717 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 635.532,
            "range": "± 22.5573",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 624.517,
            "range": "± 11.8616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 142.153,
            "range": "± 2.08175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 142.506,
            "range": "± 4.38093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.2303,
            "range": "± 3.51708",
            "unit": "ns",
            "extra": "100 samples\n857 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 271.771,
            "range": "± 6.65227",
            "unit": "ns",
            "extra": "100 samples\n9539 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.75101,
            "range": "± 0.429435",
            "unit": "ns",
            "extra": "100 samples\n8670 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.531,
            "range": "± 1.84049",
            "unit": "ns",
            "extra": "100 samples\n808 iterations"
          },
          {
            "name": "ssa_add",
            "value": 179.069,
            "range": "± 9.40586",
            "unit": "ns",
            "extra": "100 samples\n173 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 435.753,
            "range": "± 26.2967",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11537,
            "range": "± 91.3448",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 452.529,
            "range": "± 24.0127",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 843.793,
            "range": "± 40.7455",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 462.963,
            "range": "± 24.3206",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.052,
            "range": "± 40.0071",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 430.417,
            "range": "± 18.362",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 481.922,
            "range": "± 35.7511",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 420.96,
            "range": "± 26.8031",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 46.9348,
            "range": "± 22.4931",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.58179,
            "range": "± 119.249",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.3309,
            "range": "± 434.848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 45.9956,
            "range": "± 14.7115",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.9913,
            "range": "± 18.5359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.65544,
            "range": "± 147.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 28.1095,
            "range": "± 448.351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 64.9117,
            "range": "± 17.2028",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 45.8324,
            "range": "± 15.2855",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.1304,
            "range": "± 147.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.29783,
            "range": "± 194.868",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.28293,
            "range": "± 195.657",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.39651,
            "range": "± 1.51334",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.536,
            "range": "± 579.837",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.7254,
            "range": "± 3.66263",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.5865,
            "range": "± 1.72963",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.36665,
            "range": "± 1.57258",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.57594,
            "range": "± 541.226",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.1426,
            "range": "± 3.65332",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.41512,
            "range": "± 1.23609",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.27417,
            "range": "± 1.47989",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.68052,
            "range": "± 712.241",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.824,
            "range": "± 2.78924",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.199,
            "range": "± 1.51772",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.46664,
            "range": "± 792.799",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.4373,
            "range": "± 743.801",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.62993,
            "range": "± 1.75064",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.98329,
            "range": "± 775.486",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.56161,
            "range": "± 1.08357",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.86545,
            "range": "± 481.362",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.6688,
            "range": "± 7.93865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.4348,
            "range": "± 7.44999",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.47933,
            "range": "± 29.4589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.49096,
            "range": "± 30.1932",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.2765,
            "range": "± 10.9483",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.8909,
            "range": "± 9.56979",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.45426,
            "range": "± 60.0025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.29528,
            "range": "± 43.0008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 714.188,
            "range": "± 61.9818",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.56722,
            "range": "± 100.633",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.32578,
            "range": "± 194.785",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.58636,
            "range": "± 91.8031",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.8972,
            "range": "± 459.129",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.74208,
            "range": "± 107.84",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.90251,
            "range": "± 88.3959",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.4572,
            "range": "± 92.1571",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.71397,
            "range": "± 107.39",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.42298,
            "range": "± 126.153",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.81615,
            "range": "± 432.449",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.3493,
            "range": "± 3.75176",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 12.0695,
            "range": "± 774.013",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 173.153,
            "range": "± 6.55219",
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
          "id": "33d48096e2c8b1eca884acf4840cf7a837e67b94",
          "message": "Clean up CI test matrix: 13 entries down to 8 (#241)",
          "timestamp": "2026-04-27T13:22:12+02:00",
          "tree_id": "e35c880fd463cb9608f00357c318ff8fdc4ae177",
          "url": "https://github.com/nebulastream/nautilus/commit/33d48096e2c8b1eca884acf4840cf7a837e67b94"
        },
        "date": 1777289326236,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.10914,
            "range": "± 129.291",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.15722,
            "range": "± 686.27",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.16989,
            "range": "± 673.424",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 3.84746,
            "range": "± 291.693",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.4337,
            "range": "± 1.37783",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 11.5492,
            "range": "± 1.38246",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 7.78245,
            "range": "± 705.758",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.22483,
            "range": "± 309.682",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 15.8442,
            "range": "± 1.25808",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 7.76169,
            "range": "± 703.593",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.73845,
            "range": "± 1.59231",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.02758,
            "range": "± 317.429",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 12.1955,
            "range": "± 978.272",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 8.97078,
            "range": "± 617.692",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 6.50943,
            "range": "± 537.398",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 6.89057,
            "range": "± 482.163",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.37559,
            "range": "± 686.187",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.45815,
            "range": "± 353.861",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.43962,
            "range": "± 475.5",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.43147,
            "range": "± 273.823",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 34.6419,
            "range": "± 2.44276",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 34.7803,
            "range": "± 2.51998",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.27701,
            "range": "± 49.2363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.32622,
            "range": "± 10.2425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 91.4788,
            "range": "± 4.8685",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 43.7251,
            "range": "± 4.61282",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.28347,
            "range": "± 130.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.18912,
            "range": "± 47.1302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 701.501,
            "range": "± 56.1609",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.50255,
            "range": "± 125.53",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.06961,
            "range": "± 359.493",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.50135,
            "range": "± 134.103",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.66057,
            "range": "± 155.036",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.6955,
            "range": "± 143.047",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.84406,
            "range": "± 117.639",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.24417,
            "range": "± 99.1441",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.65606,
            "range": "± 108.602",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.404,
            "range": "± 96.1755",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.26458,
            "range": "± 235.078",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.2579,
            "range": "± 5.65617",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.4446,
            "range": "± 1.67929",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 160.357,
            "range": "± 7.59841",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 34.593,
            "range": "± 3.228",
            "unit": "ns",
            "extra": "100 samples\n790 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 259.295,
            "range": "± 2.96044",
            "unit": "ns",
            "extra": "100 samples\n8988 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.65344,
            "range": "± 0.697859",
            "unit": "ns",
            "extra": "100 samples\n8333 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 37.1114,
            "range": "± 1.97751",
            "unit": "ns",
            "extra": "100 samples\n753 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.47215,
            "range": "± 58.8819",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.00405,
            "range": "± 79.7763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 4.95988,
            "range": "± 30.8586",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.02792,
            "range": "± 49.0674",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.1649,
            "range": "± 240.304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.592,
            "range": "± 132.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.3078,
            "range": "± 219.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.9481,
            "range": "± 29.6637",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.1756,
            "range": "± 215.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.30068,
            "range": "± 43.1446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.3331,
            "range": "± 50.9001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 24.6675,
            "range": "± 233.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.43661,
            "range": "± 45.3769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 19.9407,
            "range": "± 80.0963",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.0188,
            "range": "± 317.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.4785,
            "range": "± 246.909",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 25.5597,
            "range": "± 185.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 24.71,
            "range": "± 205.467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 25.8834,
            "range": "± 603.936",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.1927,
            "range": "± 604.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.2828,
            "range": "± 442.9",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.4172,
            "range": "± 322.976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 24.8099,
            "range": "± 227.457",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.602,
            "range": "± 207.571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.4625,
            "range": "± 250.223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.2904,
            "range": "± 316.577",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 29.8191,
            "range": "± 330.126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.0495,
            "range": "± 2.10121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.5546,
            "range": "± 2.11084",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.3181,
            "range": "± 3.08808",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.8137,
            "range": "± 4.46161",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.0802,
            "range": "± 2.1872",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.0058,
            "range": "± 2.87874",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.4685,
            "range": "± 2.9003",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.4394,
            "range": "± 1.91966",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.3634,
            "range": "± 2.84779",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.2929,
            "range": "± 2.70562",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.8062,
            "range": "± 2.25548",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.8494,
            "range": "± 1.90336",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 187.331,
            "range": "± 7.86776",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 48.0986,
            "range": "± 4.39251",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 294.804,
            "range": "± 13.6824",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.2078,
            "range": "± 2.87013",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.8723,
            "range": "± 3.36867",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 55.0791,
            "range": "± 5.95151",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.2604,
            "range": "± 3.92724",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 56.2992,
            "range": "± 6.26098",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.6792,
            "range": "± 5.28076",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.7125,
            "range": "± 6.38207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.5631,
            "range": "± 3.12867",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 39.5643,
            "range": "± 4.41574",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.0842,
            "range": "± 4.84093",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 100.53,
            "range": "± 8.45387",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1014.9999999999999,
            "range": "± 33345.2",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 150.901,
            "range": "± 8.23084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.16236,
            "range": "± 44.6497",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 50.9907,
            "range": "± 6.11933",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.19524,
            "range": "± 26.5501",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 23.7856,
            "range": "± 233.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 53.0696,
            "range": "± 7.08515",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 72.5492,
            "range": "± 8.75269",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.13129,
            "range": "± 132.701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 24.6019,
            "range": "± 192.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 69.8906,
            "range": "± 8.37384",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 186.88,
            "range": "± 9.83902",
            "unit": "ns",
            "extra": "100 samples\n155 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 469.613,
            "range": "± 61.3166",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.16923,
            "range": "± 30.418",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 484.043,
            "range": "± 23.8001",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 922.186,
            "range": "± 70.6055",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 502.342,
            "range": "± 33.3632",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 595.34,
            "range": "± 39.3713",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 448.25,
            "range": "± 27.0924",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 511.619,
            "range": "± 26.6456",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 413.154,
            "range": "± 25.2412",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.4871,
            "range": "± 1.00771",
            "unit": "ns",
            "extra": "100 samples\n8524 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 12.8796,
            "range": "± 800.879",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 517.367,
            "range": "± 13.511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.682,
            "range": "± 0.801956",
            "unit": "ns",
            "extra": "100 samples\n6494 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.3872,
            "range": "± 9.92569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9249,
            "range": "± 280.07",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.361,
            "range": "± 3.74323",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 751.872,
            "range": "± 10.3112",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 162.985,
            "range": "± 347.791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.50242,
            "range": "± 0.444526",
            "unit": "ns",
            "extra": "100 samples\n8335 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.5755,
            "range": "± 1.86978",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.8476,
            "range": "± 41.4586",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 42.3339,
            "range": "± 3.05591",
            "unit": "ns",
            "extra": "100 samples\n692 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.363,
            "range": "± 6.25287",
            "unit": "ns",
            "extra": "100 samples\n692 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 790.873,
            "range": "± 81.9751",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 749.516,
            "range": "± 12.3209",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 163.622,
            "range": "± 8.81867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 162.781,
            "range": "± 1.41608",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 53.6961,
            "range": "± 6.74652",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.39297,
            "range": "± 35.864",
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
          "id": "10a6635c7168d3218c719cd97a10ca88d02643d0",
          "message": "vscode-nautilus-ir: render SSA values inline while debugging (#272)",
          "timestamp": "2026-04-27T19:15:55+02:00",
          "tree_id": "01c3a0cd77fef2481b476d9a363f0a7e1b1b5547",
          "url": "https://github.com/nebulastream/nautilus/commit/10a6635c7168d3218c719cd97a10ca88d02643d0"
        },
        "date": 1777310451567,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.7701,
            "range": "± 1.25718",
            "unit": "ns",
            "extra": "100 samples\n8474 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.3631,
            "range": "± 1.67729",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 513.173,
            "range": "± 15.8951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.84074,
            "range": "± 0.975486",
            "unit": "ns",
            "extra": "100 samples\n6599 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.148,
            "range": "± 14.5938",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9689,
            "range": "± 194.582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.5894,
            "range": "± 5.2732",
            "unit": "ns",
            "extra": "100 samples\n703 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 753.817,
            "range": "± 16.9593",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 163.437,
            "range": "± 1.52509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.59554,
            "range": "± 0.522867",
            "unit": "ns",
            "extra": "100 samples\n8346 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.7515,
            "range": "± 1.80854",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.84254,
            "range": "± 26.3078",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.3788,
            "range": "± 6.39372",
            "unit": "ns",
            "extra": "100 samples\n700 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 43.9551,
            "range": "± 5.99039",
            "unit": "ns",
            "extra": "100 samples\n704 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 751.654,
            "range": "± 14.3443",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 751.761,
            "range": "± 11.6866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 163.167,
            "range": "± 446.848",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 163.266,
            "range": "± 1.17473",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.3468,
            "range": "± 8.00576",
            "unit": "ns",
            "extra": "100 samples\n858 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 258.029,
            "range": "± 4.30464",
            "unit": "ns",
            "extra": "100 samples\n9127 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.6505,
            "range": "± 0.452295",
            "unit": "ns",
            "extra": "100 samples\n8464 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.3209,
            "range": "± 4.07534",
            "unit": "ns",
            "extra": "100 samples\n766 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.49638,
            "range": "± 295.356",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.4736,
            "range": "± 334.61",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.30328,
            "range": "± 1.67908",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.70958,
            "range": "± 574.319",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.0099,
            "range": "± 3.07346",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.1091,
            "range": "± 1.70257",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.71348,
            "range": "± 1.22788",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.77327,
            "range": "± 516.624",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.3334,
            "range": "± 2.90873",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.82408,
            "range": "± 1.28846",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.96005,
            "range": "± 1.49533",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.1524,
            "range": "± 762.558",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.2471,
            "range": "± 2.16149",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.4746,
            "range": "± 1.84546",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.6142,
            "range": "± 1.0114",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.46691,
            "range": "± 872.687",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.1531,
            "range": "± 1.57566",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.17947,
            "range": "± 858.737",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.05058,
            "range": "± 1.02295",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.06246,
            "range": "± 448.501",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 37.7878,
            "range": "± 6.23274",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 38.9354,
            "range": "± 7.34432",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34644,
            "range": "± 53.7282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.35177,
            "range": "± 56.6128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 99.7738,
            "range": "± 12.6143",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 49.9159,
            "range": "± 9.0803",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.48434,
            "range": "± 79.2886",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26925,
            "range": "± 73.0211",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 204.202,
            "range": "± 28.1667",
            "unit": "ns",
            "extra": "100 samples\n155 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 484.779,
            "range": "± 51.85",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.20907,
            "range": "± 105.009",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 533.363,
            "range": "± 80.3068",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 959.38,
            "range": "± 91.9966",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 538.332,
            "range": "± 83.9372",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 612.761,
            "range": "± 60.7414",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 475.509,
            "range": "± 52.7257",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 530.832,
            "range": "± 47.7214",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 468.824,
            "range": "± 43.3429",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 55.7034,
            "range": "± 13.3607",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.28567,
            "range": "± 103.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.0471,
            "range": "± 510.067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 56.0618,
            "range": "± 12.9099",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 77.4694,
            "range": "± 16.0654",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.30892,
            "range": "± 116.288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.7075,
            "range": "± 518.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 77.1814,
            "range": "± 14.9321",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.52896,
            "range": "± 150.264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.15129,
            "range": "± 143.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.04123,
            "range": "± 67.5269",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.23834,
            "range": "± 348.192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.78,
            "range": "± 822.48",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.4205,
            "range": "± 456.737",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.18,
            "range": "± 795.83",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.98862,
            "range": "± 45.2884",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.5174,
            "range": "± 254.747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.43739,
            "range": "± 66.7206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.9118,
            "range": "± 562.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.5045,
            "range": "± 614.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.77351,
            "range": "± 299.372",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.2026,
            "range": "± 289.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.3523,
            "range": "± 536.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.6938,
            "range": "± 472.16",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.2772,
            "range": "± 280.811",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.3249,
            "range": "± 302.8",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.0316,
            "range": "± 253.783",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5171,
            "range": "± 227.43",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.7051,
            "range": "± 267.651",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.1416,
            "range": "± 369.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.9456,
            "range": "± 429.397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3955,
            "range": "± 444.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.4827,
            "range": "± 851.989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.4417,
            "range": "± 958.238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.448,
            "range": "± 927.552",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.2053,
            "range": "± 608.636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.582,
            "range": "± 1.75527",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.6811,
            "range": "± 3.58857",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.7163,
            "range": "± 4.31134",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.9731,
            "range": "± 4.056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.2198,
            "range": "± 3.74251",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.9753,
            "range": "± 3.3135",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.1783,
            "range": "± 3.72759",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.3933,
            "range": "± 3.71277",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.4986,
            "range": "± 2.35865",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.9795,
            "range": "± 2.53701",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.8719,
            "range": "± 4.27966",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 191.397,
            "range": "± 12.8541",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.1121,
            "range": "± 5.49462",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 297.447,
            "range": "± 12.8065",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.7886,
            "range": "± 3.14001",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.8265,
            "range": "± 5.22408",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 56.0766,
            "range": "± 9.15709",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.0292,
            "range": "± 5.17887",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.2267,
            "range": "± 11.6151",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.056,
            "range": "± 5.42689",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.1419,
            "range": "± 8.30256",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3138,
            "range": "± 5.3941",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.0434,
            "range": "± 8.60098",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.0731,
            "range": "± 6.56689",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 104.565,
            "range": "± 14.2612",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1068.05,
            "range": "± 146903",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 154.231,
            "range": "± 15.4403",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.13529,
            "range": "± 28.6135",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 56.5271,
            "range": "± 14.5799",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.53847,
            "range": "± 193.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 772.195,
            "range": "± 71.3318",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.61196,
            "range": "± 182.266",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.38468,
            "range": "± 292.836",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.65684,
            "range": "± 243.329",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.89326,
            "range": "± 320.554",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.81889,
            "range": "± 168.556",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.05088,
            "range": "± 272.65",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.51351,
            "range": "± 200.357",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.77363,
            "range": "± 155.603",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.56065,
            "range": "± 170.558",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.02456,
            "range": "± 1.15304",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 87.6189,
            "range": "± 6.58879",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.6437,
            "range": "± 1.02792",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 164.697,
            "range": "± 8.40094",
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
          "id": "25b41d9fba48f29ef99b07ccda0692c68c4b0494",
          "message": "nautilus-review: enforce single-return rule (H1) with rewrite recipes",
          "timestamp": "2026-05-10T14:42:42+02:00",
          "tree_id": "d4b0d971c3fb70c5cf136934bb44e0b0a0a684f1",
          "url": "https://github.com/nebulastream/nautilus/commit/25b41d9fba48f29ef99b07ccda0692c68c4b0494"
        },
        "date": 1778417263329,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 734.446,
            "range": "± 63.9",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.51847,
            "range": "± 128.655",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.42073,
            "range": "± 260.252",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.69977,
            "range": "± 171.144",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.84422,
            "range": "± 231.115",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.75779,
            "range": "± 137.035",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.0896,
            "range": "± 272.14",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.45715,
            "range": "± 264.941",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.78251,
            "range": "± 250.716",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.5374,
            "range": "± 141.179",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.29633,
            "range": "± 631.582",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.5475,
            "range": "± 4.55778",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.3759,
            "range": "± 975.193",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 158.042,
            "range": "± 8.21485",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 55.8925,
            "range": "± 10.8669",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.31591,
            "range": "± 174.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 25.2351,
            "range": "± 405.285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 57.8508,
            "range": "± 15.2114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 77.2209,
            "range": "± 13.6444",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.36862,
            "range": "± 143.239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.3248,
            "range": "± 559.467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 78.8001,
            "range": "± 11.5157",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.8137,
            "range": "± 1.05426",
            "unit": "ns",
            "extra": "100 samples\n8448 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.3105,
            "range": "± 2.05672",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 257.648,
            "range": "± 26.7644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.69644,
            "range": "± 0.652459",
            "unit": "ns",
            "extra": "100 samples\n6562 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.4926,
            "range": "± 8.39016",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.5094,
            "range": "± 108.602",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.2475,
            "range": "± 6.75016",
            "unit": "ns",
            "extra": "100 samples\n695 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 752.99,
            "range": "± 11.9861",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 162.742,
            "range": "± 245.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49851,
            "range": "± 0.288119",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.7735,
            "range": "± 2.10407",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.52552,
            "range": "± 27.7825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 43.7515,
            "range": "± 6.10241",
            "unit": "ns",
            "extra": "100 samples\n698 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.9458,
            "range": "± 7.51131",
            "unit": "ns",
            "extra": "100 samples\n700 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 751.372,
            "range": "± 10.8959",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 749.676,
            "range": "± 8.52225",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 162.731,
            "range": "± 318.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 162.858,
            "range": "± 2.20943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.42997,
            "range": "± 309.88",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.48852,
            "range": "± 418.925",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.91537,
            "range": "± 1.34074",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.9661,
            "range": "± 799.196",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.5827,
            "range": "± 3.21674",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.3029,
            "range": "± 2.03596",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.09284,
            "range": "± 1.27289",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.82584,
            "range": "± 629.82",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.6005,
            "range": "± 2.91126",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.95943,
            "range": "± 1.26168",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.16289,
            "range": "± 1.44439",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.01889,
            "range": "± 626.314",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.4045,
            "range": "± 2.04942",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.6818,
            "range": "± 1.75115",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.58738,
            "range": "± 897.718",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.74344,
            "range": "± 1.02314",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.4831,
            "range": "± 1.8962",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.65948,
            "range": "± 1.09177",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.18374,
            "range": "± 1.38741",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.22401,
            "range": "± 666.601",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.9542,
            "range": "± 7.3717",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 39.5285,
            "range": "± 6.79702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.34797,
            "range": "± 60.4378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.36902,
            "range": "± 75.9206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 98.7625,
            "range": "± 6.6813",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.2343,
            "range": "± 7.67267",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.40963,
            "range": "± 51.2645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.27771,
            "range": "± 61.1858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 203.509,
            "range": "± 25.047",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 509.473,
            "range": "± 76.0556",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.24301,
            "range": "± 115.866",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 525.572,
            "range": "± 68.3671",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 962.109,
            "range": "± 106.853",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 529.092,
            "range": "± 57.768",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 634.792,
            "range": "± 78.0458",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 496.898,
            "range": "± 76.2473",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 539.665,
            "range": "± 56.4171",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 480.457,
            "range": "± 52.7477",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 56.5725,
            "range": "± 12.5387",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.54104,
            "range": "± 141.229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.2793,
            "range": "± 5.82857",
            "unit": "ns",
            "extra": "100 samples\n818 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 258.737,
            "range": "± 6.05419",
            "unit": "ns",
            "extra": "100 samples\n9092 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.74474,
            "range": "± 0.817703",
            "unit": "ns",
            "extra": "100 samples\n8436 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.4971,
            "range": "± 2.56125",
            "unit": "ns",
            "extra": "100 samples\n762 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.53303,
            "range": "± 131.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.16305,
            "range": "± 142.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.06525,
            "range": "± 149.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.1612,
            "range": "± 141.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.8478,
            "range": "± 262.192",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.87,
            "range": "± 211.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.2381,
            "range": "± 686.34",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.01597,
            "range": "± 144.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.5009,
            "range": "± 169.256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.46487,
            "range": "± 169.724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.4865,
            "range": "± 138.84",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.0487,
            "range": "± 353.597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.69323,
            "range": "± 204.239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.3389,
            "range": "± 246.853",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.4129,
            "range": "± 578.391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.887,
            "range": "± 800.889",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.9772,
            "range": "± 416.477",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.0051,
            "range": "± 364.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.8867,
            "range": "± 311.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.6502,
            "range": "± 504.754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.5032,
            "range": "± 420.808",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.7647,
            "range": "± 366.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.1016,
            "range": "± 382.332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.0271,
            "range": "± 454.235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.0207,
            "range": "± 343.419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.8886,
            "range": "± 766.363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.6386,
            "range": "± 252.02",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.8376,
            "range": "± 1.59852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 15.0823,
            "range": "± 2.31728",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.988,
            "range": "± 4.37369",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.6251,
            "range": "± 4.01047",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.5491,
            "range": "± 3.31199",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.3675,
            "range": "± 3.19071",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.5764,
            "range": "± 3.52016",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.6034,
            "range": "± 3.34662",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.4426,
            "range": "± 3.47905",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.0351,
            "range": "± 3.46657",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.9728,
            "range": "± 3.66938",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.6093,
            "range": "± 4.98233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 193.929,
            "range": "± 11.2858",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 52.1641,
            "range": "± 8.41586",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 304.861,
            "range": "± 24.1907",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.0912,
            "range": "± 4.44511",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.6162,
            "range": "± 5.91511",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 57.0595,
            "range": "± 10.0651",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 42.28,
            "range": "± 10.9049",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.3546,
            "range": "± 9.40258",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.7827,
            "range": "± 10.6493",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.2723,
            "range": "± 8.61461",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.618,
            "range": "± 4.71836",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.3864,
            "range": "± 9.24341",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.803,
            "range": "± 5.88217",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 103.161,
            "range": "± 13.1245",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1041.52,
            "range": "± 21256.8",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 153.348,
            "range": "± 15.7319",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.15843,
            "range": "± 45.4091",
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
          "id": "65ed9238693c1850465b1333170d08e1212209e0",
          "message": "Centralise allocas in a per-function table; remove hoisting (#278)",
          "timestamp": "2026-05-10T20:00:59+02:00",
          "tree_id": "a572799b9a33e3ad88516ae47e4ee21ab5e087fd",
          "url": "https://github.com/nebulastream/nautilus/commit/65ed9238693c1850465b1333170d08e1212209e0"
        },
        "date": 1778436579839,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 10.7777,
            "range": "± 1.23356",
            "unit": "ns",
            "extra": "100 samples\n8476 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 14.0105,
            "range": "± 2.80363",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 585.13,
            "range": "± 22.8192",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.92613,
            "range": "± 1.10564",
            "unit": "ns",
            "extra": "100 samples\n6618 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.8444,
            "range": "± 9.73487",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0316,
            "range": "± 246.232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.8597,
            "range": "± 8.97191",
            "unit": "ns",
            "extra": "100 samples\n687 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 848.945,
            "range": "± 27.5178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.302,
            "range": "± 1.21284",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.6387,
            "range": "± 0.531971",
            "unit": "ns",
            "extra": "100 samples\n7992 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.8743,
            "range": "± 2.1647",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.86551,
            "range": "± 56.9585",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 46.4219,
            "range": "± 7.95108",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 46.3956,
            "range": "± 8.46226",
            "unit": "ns",
            "extra": "100 samples\n677 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 840.99,
            "range": "± 16.9453",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 842.707,
            "range": "± 16.162",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 172.931,
            "range": "± 13.9416",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 179.33,
            "range": "± 1.71893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.7541,
            "range": "± 13.2081",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.46748,
            "range": "± 551.801",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 26.4904,
            "range": "± 3.03748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 57.3301,
            "range": "± 12.1657",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 77.6203,
            "range": "± 13.7675",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.36929,
            "range": "± 114.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 27.0277,
            "range": "± 379.927",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 78.778,
            "range": "± 16.0323",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 37.0636,
            "range": "± 6.01332",
            "unit": "ns",
            "extra": "100 samples\n858 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 274.432,
            "range": "± 12.2326",
            "unit": "ns",
            "extra": "100 samples\n9092 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.76975,
            "range": "± 0.720254",
            "unit": "ns",
            "extra": "100 samples\n8485 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 41.9113,
            "range": "± 7.08881",
            "unit": "ns",
            "extra": "100 samples\n731 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.52643,
            "range": "± 426.915",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.40061,
            "range": "± 273.876",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.97175,
            "range": "± 1.31096",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.87941,
            "range": "± 645.818",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.1934,
            "range": "± 3.18251",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.623,
            "range": "± 2.40545",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.94814,
            "range": "± 1.47237",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.95847,
            "range": "± 689.092",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.4444,
            "range": "± 2.63669",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.367,
            "range": "± 1.49704",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.98759,
            "range": "± 1.6767",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.93108,
            "range": "± 638.833",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.9502,
            "range": "± 2.8335",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.8987,
            "range": "± 1.66775",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.40076,
            "range": "± 849.018",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.68185,
            "range": "± 900.101",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.4691,
            "range": "± 1.71674",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.2149,
            "range": "± 786.56",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.23763,
            "range": "± 1.6344",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.20618,
            "range": "± 712.476",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.4846,
            "range": "± 8.20753",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.9029,
            "range": "± 7.42017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.37752,
            "range": "± 51.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.38736,
            "range": "± 53.1329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 103.218,
            "range": "± 13.8001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 55.2207,
            "range": "± 12.1885",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.40891,
            "range": "± 112.738",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.27132,
            "range": "± 57.0933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.07195,
            "range": "± 173.176",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.8862,
            "range": "± 258.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 6.00655,
            "range": "± 442.304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.14849,
            "range": "± 258.305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2082,
            "range": "± 1.09551",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.8444,
            "range": "± 321.568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.4618,
            "range": "± 1.335",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.16587,
            "range": "± 155.941",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.3548,
            "range": "± 267.826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.5058,
            "range": "± 754.719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.6038,
            "range": "± 256.223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.0837,
            "range": "± 264.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.6629,
            "range": "± 475.005",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.7022,
            "range": "± 338.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.0921,
            "range": "± 440.478",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5776,
            "range": "± 567.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.2873,
            "range": "± 397.456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.23,
            "range": "± 266.719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.0429,
            "range": "± 235.54",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.7329,
            "range": "± 324.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.1445,
            "range": "± 264.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2731,
            "range": "± 203.364",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.9781,
            "range": "± 1.07702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.6492,
            "range": "± 194.122",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.908,
            "range": "± 1.02706",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.6696,
            "range": "± 471.042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.491,
            "range": "± 346.626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.452,
            "range": "± 1.01183",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4785,
            "range": "± 2.39558",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.4556,
            "range": "± 3.62569",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.9029,
            "range": "± 4.18426",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.7239,
            "range": "± 3.90508",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 24.3683,
            "range": "± 3.93709",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.3664,
            "range": "± 3.37303",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.3443,
            "range": "± 4.24249",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.1268,
            "range": "± 3.49476",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 53.6407,
            "range": "± 22.8865",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.904,
            "range": "± 5.10306",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.8285,
            "range": "± 4.97152",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 191.506,
            "range": "± 11.2785",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.4982,
            "range": "± 8.5693",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 299.449,
            "range": "± 12.0445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.6039,
            "range": "± 4.61483",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.491,
            "range": "± 5.73255",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 57.6993,
            "range": "± 12.0852",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.8132,
            "range": "± 5.23686",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.5505,
            "range": "± 10.4753",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 45.7867,
            "range": "± 8.68047",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.905,
            "range": "± 8.77482",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.2925,
            "range": "± 4.51614",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.3158,
            "range": "± 8.87417",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.476,
            "range": "± 4.85192",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 106.215,
            "range": "± 14.7855",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1044.71,
            "range": "± 19647.4",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 157.235,
            "range": "± 19.034",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.25074,
            "range": "± 41.9049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 791.653,
            "range": "± 77.6077",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.63699,
            "range": "± 237.44",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.45072,
            "range": "± 485.774",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.71179,
            "range": "± 241.95",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.00718,
            "range": "± 781.152",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.96034,
            "range": "± 307.525",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.09879,
            "range": "± 297.76",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.56646,
            "range": "± 261.328",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.81387,
            "range": "± 267.663",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.46977,
            "range": "± 168.588",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.74112,
            "range": "± 1.0827",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 88.2755,
            "range": "± 9.88732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.5443,
            "range": "± 1.17473",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 164.781,
            "range": "± 8.5445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 56.6991,
            "range": "± 14.0317",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.21929,
            "range": "± 148.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 192.68,
            "range": "± 19.0321",
            "unit": "ns",
            "extra": "100 samples\n159 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 466.16,
            "range": "± 50.1099",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.20943,
            "range": "± 161.639",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 527.761,
            "range": "± 128.944",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 929.884,
            "range": "± 127.948",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 515.674,
            "range": "± 62.7403",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 626.045,
            "range": "± 116.218",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 522.938,
            "range": "± 111.96",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 523.188,
            "range": "± 88.213",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 481.155,
            "range": "± 98.7057",
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
          "id": "2e075619691586841fdec0b08c5089bddc351020",
          "message": "Add AsmJit backend intrinsics for the SIMD plugin (#280)",
          "timestamp": "2026-05-11T11:24:43+02:00",
          "tree_id": "159ec3f1beb8c01e02d1081b679c10de476f2786",
          "url": "https://github.com/nebulastream/nautilus/commit/2e075619691586841fdec0b08c5089bddc351020"
        },
        "date": 1778491789206,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 6.78942,
            "range": "± 1.13394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 7.45917,
            "range": "± 246.878",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 6.4978,
            "range": "± 1.10716",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.53096,
            "range": "± 294.324",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.8448,
            "range": "± 604",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.628,
            "range": "± 1.07483",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.7949,
            "range": "± 492.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 6.15609,
            "range": "± 165.939",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.6072,
            "range": "± 228.894",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.7137,
            "range": "± 331.007",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.4522,
            "range": "± 484.636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.7207,
            "range": "± 446.117",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.7725,
            "range": "± 246.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 60.1355,
            "range": "± 577.613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 22.8568,
            "range": "± 628.616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 22.9613,
            "range": "± 519.007",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 24.0969,
            "range": "± 599.288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 23.6373,
            "range": "± 614.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 24.5712,
            "range": "± 460.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 24.0923,
            "range": "± 586.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 23.7595,
            "range": "± 619.957",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 23.6238,
            "range": "± 484.032",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 24.0688,
            "range": "± 583.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 23.999,
            "range": "± 632.86",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 26.6539,
            "range": "± 943.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 59.3112,
            "range": "± 678.639",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 29.1737,
            "range": "± 433.499",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 88.4046,
            "range": "± 1.15899",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 10.8188,
            "range": "± 2.56661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.4401,
            "range": "± 2.71129",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 18.2535,
            "range": "± 2.82825",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 14.1942,
            "range": "± 2.65192",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 16.6456,
            "range": "± 2.738",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.7515,
            "range": "± 2.58812",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 14.4458,
            "range": "± 2.36506",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 13.1077,
            "range": "± 2.60257",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.345,
            "range": "± 2.49334",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 13.7038,
            "range": "± 2.71518",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.3756,
            "range": "± 6.68125",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 193.782,
            "range": "± 7.46885",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.0144,
            "range": "± 6.65421",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 311.196,
            "range": "± 9.39117",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 16.6723,
            "range": "± 4.54583",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.7933,
            "range": "± 6.73159",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 43.3551,
            "range": "± 6.91705",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.5565,
            "range": "± 8.08103",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 43.2973,
            "range": "± 8.44688",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.2724,
            "range": "± 8.16745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 35.0462,
            "range": "± 8.68361",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 23.6342,
            "range": "± 6.27141",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 30.7992,
            "range": "± 7.57796",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.528,
            "range": "± 8.07263",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 85.6626,
            "range": "± 12.9729",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 996.89,
            "range": "± 27.0293",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 135.011,
            "range": "± 11.9178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.08185,
            "range": "± 78.5044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 872.092,
            "range": "± 66.4113",
            "unit": "ns",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.7342,
            "range": "± 149.695",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.77891,
            "range": "± 233.864",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.78845,
            "range": "± 141.983",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 3.17481,
            "range": "± 223.835",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.05458,
            "range": "± 147.585",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 2.20009,
            "range": "± 232.726",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.5919,
            "range": "± 128.178",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.88343,
            "range": "± 147.793",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.64273,
            "range": "± 139.11",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 8.65321,
            "range": "± 437.919",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.9669,
            "range": "± 2.58308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 13.603,
            "range": "± 666.133",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 172.644,
            "range": "± 5.52743",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 32.0785,
            "range": "± 3.15205",
            "unit": "ns",
            "extra": "100 samples\n589 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 369.202,
            "range": "± 12.3479",
            "unit": "ns",
            "extra": "100 samples\n6932 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.61913,
            "range": "± 0.541506",
            "unit": "ns",
            "extra": "100 samples\n6039 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 36.3575,
            "range": "± 3.12209",
            "unit": "ns",
            "extra": "100 samples\n510 iterations"
          },
          {
            "name": "ssa_add",
            "value": 191.024,
            "range": "± 19.5973",
            "unit": "ns",
            "extra": "100 samples\n96 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 444.782,
            "range": "± 42.1519",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.30016,
            "range": "± 123.308",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 485.551,
            "range": "± 40.8823",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1023.5799999999999,
            "range": "± 94741.4",
            "unit": "ns",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 492.781,
            "range": "± 57.2906",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 577.296,
            "range": "± 56.0866",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 404.117,
            "range": "± 32.5993",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 502.567,
            "range": "± 48.8497",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 445.525,
            "range": "± 49.8882",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 12.2722,
            "range": "± 0.784579",
            "unit": "ns",
            "extra": "100 samples\n6326 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 28.545,
            "range": "± 2.97597",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 736.094,
            "range": "± 32.991",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.18511,
            "range": "± 0.525463",
            "unit": "ns",
            "extra": "100 samples\n3868 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 131.033,
            "range": "± 4.7699",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 25.6589,
            "range": "± 389.026",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 37.5608,
            "range": "± 3.82963",
            "unit": "ns",
            "extra": "100 samples\n510 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 644.766,
            "range": "± 13.7581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 118.194,
            "range": "± 1.60433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.94806,
            "range": "± 0.216608",
            "unit": "ns",
            "extra": "100 samples\n6350 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 27.9505,
            "range": "± 5.59219",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 6.29781,
            "range": "± 57.0842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 36.6973,
            "range": "± 0.785281",
            "unit": "ns",
            "extra": "100 samples\n509 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 37.3168,
            "range": "± 2.83095",
            "unit": "ns",
            "extra": "100 samples\n510 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 548.824,
            "range": "± 10.207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 554.88,
            "range": "± 11.735",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 117.835,
            "range": "± 1.06111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 117.838,
            "range": "± 1.07046",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 38.3882,
            "range": "± 11.5689",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.65369,
            "range": "± 104.014",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 22.9873,
            "range": "± 383.411",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 38.8598,
            "range": "± 11.0527",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 56.2301,
            "range": "± 11.4276",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.69132,
            "range": "± 148.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 23.6922,
            "range": "± 647.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 57.8659,
            "range": "± 13.176",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.74381,
            "range": "± 331.206",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.60219,
            "range": "± 163.044",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 9.09296,
            "range": "± 1.10338",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.24452,
            "range": "± 800.893",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 26.8658,
            "range": "± 4.95222",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.107,
            "range": "± 1.79686",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 9.02133,
            "range": "± 901.656",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.30635,
            "range": "± 556.899",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 17.1277,
            "range": "± 1.12524",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.46752,
            "range": "± 743.983",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 9.33607,
            "range": "± 1.20012",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.42741,
            "range": "± 807.852",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.0039,
            "range": "± 1.73504",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.4536,
            "range": "± 1.80909",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.87994,
            "range": "± 1.01694",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.90565,
            "range": "± 967.736",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.5274,
            "range": "± 2.30054",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.78817,
            "range": "± 561.646",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 8.17626,
            "range": "± 1.17334",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.31711,
            "range": "± 592.293",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 40.7731,
            "range": "± 6.46758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.5952,
            "range": "± 5.88305",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.33875,
            "range": "± 42.4412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.34416,
            "range": "± 28.9576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.8561,
            "range": "± 8.00295",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 52.4812,
            "range": "± 6.72319",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.05643,
            "range": "± 38.316",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.3919,
            "range": "± 43.5607",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 38.5362,
            "range": "± 9.70393",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.83028,
            "range": "± 487.435",
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
          "id": "a548fb12f634b2cc370e41e724be09db7a8e3688",
          "message": "Make plugins installable\n\nEach plugin (`std`, `simd`, `specialization`, `inlining`, `gpu`) now\ndeclares CMake install rules so that `cmake --install` ships its static\nlibrary, public headers, and (where applicable) auxiliary CMake helpers.\nAll plugin targets are registered against the existing `nautilus-config`\nexport set, so a single `find_package(nautilus)` exposes them as\n`nautilus::nautilus-std`, `nautilus::nautilus-simd`,\n`nautilus::nautilus-specialization`, `nautilus::nautilus-inlining`, and\n`nautilus::nautilus-gpu`. The inlining plugin additionally installs the\noptional `InliningPass` LLVM pass plugin and the `NautilusInline.cmake`\nhelper, and the now-redundant\n`list(APPEND EXPORT_TARGETS InliningPass PARENT_SCOPE)` is removed since\nthe plugin's own `install(TARGETS ...)` handles export registration\ndirectly.\n\nAlso fixes a pre-existing `cmake --install` failure in the core nautilus\ninstall: PUBLIC_HEADER DESTINATION was being applied to every target in\nthe install(TARGETS) call, and bundled fmt sets PUBLIC_HEADER with\nrelative paths that CMake resolves against `nautilus/`'s source dir\ninstead of fmt's, aborting install at `nautilus/include/fmt/args.h`. The\nnautilus target does not set PUBLIC_HEADER (its headers come from the\ninstall(DIRECTORY ...) calls) and the bundled third-party libraries are\nlinked PRIVATE, so dropping the clause is safe and lets `cmake --install`\ncomplete in one shot.",
          "timestamp": "2026-05-11T14:55:24+02:00",
          "tree_id": "b449d39861e31074479dd81a93ff6cea35b966ea",
          "url": "https://github.com/nebulastream/nautilus/commit/a548fb12f634b2cc370e41e724be09db7a8e3688"
        },
        "date": 1778504437178,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 677.652,
            "range": "± 52.8572",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.42324,
            "range": "± 82.8625",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.11769,
            "range": "± 164.139",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.50162,
            "range": "± 96.6974",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.6189,
            "range": "± 156.029",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.63139,
            "range": "± 102.879",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.78558,
            "range": "± 95.6107",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.36946,
            "range": "± 91.7915",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.58802,
            "range": "± 84.3098",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.36201,
            "range": "± 78.8385",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.21169,
            "range": "± 708.125",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.5869,
            "range": "± 4.14696",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.1391,
            "range": "± 593.245",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 162.434,
            "range": "± 6.13562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 47.3377,
            "range": "± 15.539",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.61933,
            "range": "± 186.242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 27.8354,
            "range": "± 934.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 46.2688,
            "range": "± 15.1303",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 63.9289,
            "range": "± 17.5593",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.9567,
            "range": "± 185.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 29.1326,
            "range": "± 523.628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 64.5642,
            "range": "± 17.4865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.2736,
            "range": "± 3.85836",
            "unit": "ns",
            "extra": "100 samples\n886 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 274.708,
            "range": "± 7.87413",
            "unit": "ns",
            "extra": "100 samples\n9528 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.87319,
            "range": "± 0.622911",
            "unit": "ns",
            "extra": "100 samples\n8670 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 38.7537,
            "range": "± 1.83335",
            "unit": "ns",
            "extra": "100 samples\n801 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.33325,
            "range": "± 228.595",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.33235,
            "range": "± 226.597",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.42736,
            "range": "± 1.60566",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.72295,
            "range": "± 1.14044",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 24.8631,
            "range": "± 4.20504",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.926,
            "range": "± 2.11981",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.24888,
            "range": "± 1.47455",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.69311,
            "range": "± 635.25",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.1228,
            "range": "± 3.49458",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.59937,
            "range": "± 1.36657",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.3562,
            "range": "± 1.50492",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.73641,
            "range": "± 581.857",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.9988,
            "range": "± 2.72864",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.4039,
            "range": "± 1.52638",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.52973,
            "range": "± 859.672",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.4839,
            "range": "± 957.73",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.66714,
            "range": "± 1.44775",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.09018,
            "range": "± 744.556",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.51248,
            "range": "± 1.1154",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.90117,
            "range": "± 450.028",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 39.9635,
            "range": "± 8.18097",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 40.1344,
            "range": "± 7.69936",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.46953,
            "range": "± 30.9035",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.49424,
            "range": "± 31.2698",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 95.6208,
            "range": "± 12.1148",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 51.0384,
            "range": "± 9.64525",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.53832,
            "range": "± 225.536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.26693,
            "range": "± 38.0898",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 46.2238,
            "range": "± 17.1814",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 6.18103,
            "range": "± 262.219",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.6422,
            "range": "± 1.5018",
            "unit": "ns",
            "extra": "100 samples\n8807 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.5385,
            "range": "± 1.55961",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 549.642,
            "range": "± 56.355",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.51827,
            "range": "± 0.542558",
            "unit": "ns",
            "extra": "100 samples\n7223 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 108.711,
            "range": "± 8.46997",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 23.5445,
            "range": "± 36.4115",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.6759,
            "range": "± 2.40802",
            "unit": "ns",
            "extra": "100 samples\n703 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 700.64,
            "range": "± 11.4325",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 165.484,
            "range": "± 1.66719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.56549,
            "range": "± 0.391693",
            "unit": "ns",
            "extra": "100 samples\n8820 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 23.8133,
            "range": "± 6.16001",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.28098,
            "range": "± 19.2102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.619,
            "range": "± 6.08635",
            "unit": "ns",
            "extra": "100 samples\n712 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.215,
            "range": "± 3.45803",
            "unit": "ns",
            "extra": "100 samples\n712 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 698.234,
            "range": "± 11.2224",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 700.333,
            "range": "± 20.0344",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 165.064,
            "range": "± 1.77231",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 165.094,
            "range": "± 1.78188",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 164.436,
            "range": "± 7.30736",
            "unit": "ns",
            "extra": "100 samples\n189 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 414.993,
            "range": "± 25.2126",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.05166,
            "range": "± 59.4129",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 431.622,
            "range": "± 24.9599",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 819.563,
            "range": "± 56.1361",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 439.273,
            "range": "± 24.8495",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 522.274,
            "range": "± 52.4314",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 377.971,
            "range": "± 21.1117",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 456.154,
            "range": "± 27.6339",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 405.617,
            "range": "± 33.0497",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 6.23404,
            "range": "± 417.113",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.73407,
            "range": "± 201.313",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.69174,
            "range": "± 346.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 8.07825,
            "range": "± 447.093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.5817,
            "range": "± 475.77",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 12.5244,
            "range": "± 252.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.8063,
            "range": "± 1.00347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.57451,
            "range": "± 142.788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 11.1554,
            "range": "± 180.689",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 10.0266,
            "range": "± 179.605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 11.1043,
            "range": "± 174.12",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.5409,
            "range": "± 197.525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 10.3615,
            "range": "± 228.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 21.2925,
            "range": "± 527.061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.9118,
            "range": "± 335.245",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.6471,
            "range": "± 738.732",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.4605,
            "range": "± 482.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.5624,
            "range": "± 623.155",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.2602,
            "range": "± 311.312",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.7322,
            "range": "± 355.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.8911,
            "range": "± 316.895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.0846,
            "range": "± 382.206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.3998,
            "range": "± 251.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.2644,
            "range": "± 226.918",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.3151,
            "range": "± 260.897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.8183,
            "range": "± 1.8776",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.5346,
            "range": "± 709.962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.1664,
            "range": "± 438.418",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4277,
            "range": "± 2.7452",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 16.9023,
            "range": "± 4.10727",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 21.5853,
            "range": "± 3.80977",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 16.788,
            "range": "± 4.67076",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 19.0849,
            "range": "± 3.86312",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 16.9613,
            "range": "± 3.43185",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 17.6641,
            "range": "± 3.7822",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.5745,
            "range": "± 4.16708",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.053,
            "range": "± 4.80913",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 16.0455,
            "range": "± 2.44319",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 31.1757,
            "range": "± 4.51376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 187.392,
            "range": "± 8.65273",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 43.8538,
            "range": "± 8.75011",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 304.875,
            "range": "± 13.3856",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 17.748,
            "range": "± 4.78996",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 26.9536,
            "range": "± 5.741",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 48.1348,
            "range": "± 10.7962",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 29.0873,
            "range": "± 6.49027",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 48.5358,
            "range": "± 11.6885",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 32.4605,
            "range": "± 5.91449",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 34.4072,
            "range": "± 5.83652",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.2043,
            "range": "± 5.06851",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 31.2908,
            "range": "± 6.48204",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 29.0563,
            "range": "± 6.86866",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 94.3273,
            "range": "± 10.9897",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1031.67,
            "range": "± 16901.399999999998",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 147.516,
            "range": "± 14.943",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.18408,
            "range": "± 27.0565",
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
          "id": "f9cd28f2bf4d268969d1b1bcc4a9545d2633dbce",
          "message": "Remove non-path-explosion fixtures, keep only PathExplosionFunctions.hpp\n\nDrops the multipleReturnsRoot (NautilusFunction chain),\nmultipleReturns3 (direct-call chain), and multipleReturns4\n(direct-call chain + post-call branch) fixtures and their reference\ndumps. The path-explosion repro suite in PathExplosionFunctions.hpp\nsubsumes them: pathExplosion_baseline_threeCallsNoBranch and\npathExplosion_postCallBranch_{1,2,3} cover the same shapes with\nmatching empirical path counts and a unified taxonomy.\n\nThe Path Explosion Trace Test section and the path-explosion-tests\nreference data are unchanged.\n\nhttps://claude.ai/code/session_01K8Rt495eVPSkjP1U8dEGjM",
          "timestamp": "2026-05-14T10:50:17+02:00",
          "tree_id": "8b4698c98db09590b2518e54ffa8884333bbc32b",
          "url": "https://github.com/nebulastream/nautilus/commit/f9cd28f2bf4d268969d1b1bcc4a9545d2633dbce"
        },
        "date": 1778748921120,
        "tool": "catch2",
        "benches": [
          {
            "name": "tiered_compile_addOne",
            "value": 55.1658,
            "range": "± 10.7264",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.25749,
            "range": "± 57.5499",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.4391,
            "range": "± 405.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 60.6703,
            "range": "± 11.2707",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 79.6202,
            "range": "± 15.7788",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.35632,
            "range": "± 118.42",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 26.0899,
            "range": "± 453.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 78.6934,
            "range": "± 10.1775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.6651,
            "range": "± 0.946586",
            "unit": "ns",
            "extra": "100 samples\n8372 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.4703,
            "range": "± 2.01788",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 527.422,
            "range": "± 21.2529",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.69868,
            "range": "± 0.745035",
            "unit": "ns",
            "extra": "100 samples\n6593 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.6361,
            "range": "± 9.26507",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9426,
            "range": "± 115.287",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.8635,
            "range": "± 8.015",
            "unit": "ns",
            "extra": "100 samples\n676 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 859.028,
            "range": "± 13.0138",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 186.067,
            "range": "± 1.36597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.48273,
            "range": "± 0.424618",
            "unit": "ns",
            "extra": "100 samples\n8461 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.8625,
            "range": "± 3.3694",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.87965,
            "range": "± 312.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 44.3118,
            "range": "± 3.89068",
            "unit": "ns",
            "extra": "100 samples\n682 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 44.599,
            "range": "± 5.15383",
            "unit": "ns",
            "extra": "100 samples\n681 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 852.767,
            "range": "± 11.3762",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 875.208,
            "range": "± 25.409",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 185.974,
            "range": "± 222.438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 185.982,
            "range": "± 165.251",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.27731,
            "range": "± 155.69",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.32405,
            "range": "± 143.855",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.63915,
            "range": "± 776.354",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.54071,
            "range": "± 401.003",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.5445,
            "range": "± 1.57172",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.559,
            "range": "± 987.646",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.80429,
            "range": "± 1.55155",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.79104,
            "range": "± 376.911",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.6688,
            "range": "± 1.56827",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.40114,
            "range": "± 807.253",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.6864,
            "range": "± 697.439",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.86476,
            "range": "± 354.791",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 14.0902,
            "range": "± 1.18648",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.1206,
            "range": "± 990.611",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.21602,
            "range": "± 496.139",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.43532,
            "range": "± 991.516",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 10.0814,
            "range": "± 861.346",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.00031,
            "range": "± 426.095",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.79585,
            "range": "± 648.305",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.87126,
            "range": "± 272.452",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 38.0283,
            "range": "± 3.64798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 37.3711,
            "range": "± 3.21573",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.35476,
            "range": "± 22.6997",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.35268,
            "range": "± 26.016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 97.7583,
            "range": "± 4.73759",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 48.3258,
            "range": "± 3.59147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.43042,
            "range": "± 29.6138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.24075,
            "range": "± 31.9951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 56.9167,
            "range": "± 7.69512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.52315,
            "range": "± 41.2737",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.55479,
            "range": "± 56.4997",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.14317,
            "range": "± 38.4483",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.06889,
            "range": "± 99.0918",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.14743,
            "range": "± 31.0585",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.6125,
            "range": "± 100.629",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.792,
            "range": "± 44.0495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.7179,
            "range": "± 76.6326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 5.03112,
            "range": "± 38.6111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.4618,
            "range": "± 76.5332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.43388,
            "range": "± 42.8866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.5281,
            "range": "± 45.0755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.0116,
            "range": "± 102.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.56929,
            "range": "± 46.3491",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.2061,
            "range": "± 81.9884",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.3267,
            "range": "± 189.47",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 24.9638,
            "range": "± 283.6",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.0088,
            "range": "± 233.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.0851,
            "range": "± 291.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.2072,
            "range": "± 775.414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.3831,
            "range": "± 531.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.4858,
            "range": "± 203.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.7845,
            "range": "± 176.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.2385,
            "range": "± 205.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.0233,
            "range": "± 436.288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.0721,
            "range": "± 363.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.7276,
            "range": "± 707.638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.7822,
            "range": "± 574.356",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.9107,
            "range": "± 685.096",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.7773,
            "range": "± 1.91246",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.2429,
            "range": "± 3.53172",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.0339,
            "range": "± 3.44593",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.9128,
            "range": "± 2.70016",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.5194,
            "range": "± 2.28647",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.2878,
            "range": "± 2.03113",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.5351,
            "range": "± 2.66896",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.2132,
            "range": "± 2.1155",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.1709,
            "range": "± 2.93634",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.5754,
            "range": "± 2.5724",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.6002,
            "range": "± 4.60432",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 198.41,
            "range": "± 10.198",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 52.6073,
            "range": "± 9.11991",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 309.294,
            "range": "± 26.888",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.1735,
            "range": "± 4.82406",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.4071,
            "range": "± 4.26649",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 56.4176,
            "range": "± 5.25514",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.8631,
            "range": "± 4.35405",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.5183,
            "range": "± 8.7641",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.2559,
            "range": "± 9.00425",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.2785,
            "range": "± 6.56048",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.6917,
            "range": "± 4.31543",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.5157,
            "range": "± 6.92307",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.3711,
            "range": "± 4.19816",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 103.255,
            "range": "± 10.7623",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1057.97,
            "range": "± 54427.5",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 153.183,
            "range": "± 14.6589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.16653,
            "range": "± 46.7063",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 178.473,
            "range": "± 9.97434",
            "unit": "ns",
            "extra": "100 samples\n164 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 459.53,
            "range": "± 43.4269",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.16567,
            "range": "± 124.837",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 487.861,
            "range": "± 38.2642",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 895.76,
            "range": "± 72.8648",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 491.118,
            "range": "± 45.7827",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 583.073,
            "range": "± 41.8959",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 422.539,
            "range": "± 47.7647",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 503.737,
            "range": "± 39.1986",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 445.234,
            "range": "± 36.8343",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.0837,
            "range": "± 6.3975",
            "unit": "ns",
            "extra": "100 samples\n868 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 258.154,
            "range": "± 2.33301",
            "unit": "ns",
            "extra": "100 samples\n9107 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.54853,
            "range": "± 0.249363",
            "unit": "ns",
            "extra": "100 samples\n8425 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 37.1985,
            "range": "± 2.33453",
            "unit": "ns",
            "extra": "100 samples\n764 iterations"
          },
          {
            "name": "ir_add",
            "value": 757.379,
            "range": "± 38.5584",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.58461,
            "range": "± 136.165",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.40826,
            "range": "± 363.019",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.63694,
            "range": "± 149.086",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.83439,
            "range": "± 293.378",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.80584,
            "range": "± 177.074",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.97161,
            "range": "± 153.779",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.44956,
            "range": "± 119.501",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.72022,
            "range": "± 143.401",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.4549,
            "range": "± 101.976",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.70943,
            "range": "± 540.522",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 93.6887,
            "range": "± 16.2569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.594,
            "range": "± 826.837",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 169.138,
            "range": "± 8.3263",
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
          "id": "75ff0dfef6eb9207a3d9f0788b6af7bc50242390",
          "message": "Make active-tracer deregistration exception-safe (#295)",
          "timestamp": "2026-06-09T07:18:34+02:00",
          "tree_id": "51aeed6a91cfdeb6a8633b5dd11dcbc684b819a8",
          "url": "https://github.com/nebulastream/nautilus/commit/75ff0dfef6eb9207a3d9f0788b6af7bc50242390"
        },
        "date": 1780982942323,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 755.617,
            "range": "± 85.6167",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 1.56656,
            "range": "± 209.868",
            "unit": "us",
            "extra": "100 samples\n21 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 3.17249,
            "range": "± 246.779",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 1.57898,
            "range": "± 190.895",
            "unit": "us",
            "extra": "100 samples\n20 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 2.74325,
            "range": "± 433.512",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 1.78335,
            "range": "± 215.207",
            "unit": "us",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 1.98042,
            "range": "± 290.661",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.47127,
            "range": "± 234.541",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 1.74991,
            "range": "± 275.488",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 1.52148,
            "range": "± 268.985",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 7.23919,
            "range": "± 746.39",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 89.8313,
            "range": "± 3.71655",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 11.3618,
            "range": "± 1.30639",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 159.344,
            "range": "± 9.22263",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.22354,
            "range": "± 280.752",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.24229,
            "range": "± 256.329",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 8.84489,
            "range": "± 1.05585",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.65888,
            "range": "± 778.931",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 25.4918,
            "range": "± 2.98048",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 12.1697,
            "range": "± 1.43735",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 8.53241,
            "range": "± 1.00664",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.68945,
            "range": "± 713.015",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 16.8695,
            "range": "± 2.63969",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.5862,
            "range": "± 1.37344",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 8.47066,
            "range": "± 1.28691",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.56686,
            "range": "± 537.529",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 13.3236,
            "range": "± 1.97524",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 9.74705,
            "range": "± 1.41698",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.41319,
            "range": "± 940.622",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 7.19177,
            "range": "± 774.759",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 9.50806,
            "range": "± 897.088",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.95337,
            "range": "± 798.552",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 7.61389,
            "range": "± 809.77",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 3.8648,
            "range": "± 470.038",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 37.6,
            "range": "± 6.92731",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 36.5367,
            "range": "± 6.37019",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.32708,
            "range": "± 27.0587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.33241,
            "range": "± 41.2535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 96.2285,
            "range": "± 6.19633",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 50.8384,
            "range": "± 10.6089",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.37478,
            "range": "± 48.7271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.16285,
            "range": "± 51.5958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 181.812,
            "range": "± 19.4922",
            "unit": "ns",
            "extra": "100 samples\n167 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 453.873,
            "range": "± 48.9664",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11973,
            "range": "± 118.376",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 495.651,
            "range": "± 61.36",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 869.5,
            "range": "± 82.8979",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 491.413,
            "range": "± 45.091",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 604.223,
            "range": "± 82.4348",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 455.5,
            "range": "± 71.5338",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 506.007,
            "range": "± 55.4263",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 457.904,
            "range": "± 52.6793",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 5.51003,
            "range": "± 71.1042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 6.07256,
            "range": "± 116.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 5.023,
            "range": "± 66.6476",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 7.07615,
            "range": "± 220.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 28.2637,
            "range": "± 247.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 11.6148,
            "range": "± 72.7973",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 27.3932,
            "range": "± 346.33",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 4.98431,
            "range": "± 51.7271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 10.2989,
            "range": "± 102.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 9.31315,
            "range": "± 103.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 10.3891,
            "range": "± 133.742",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 25.1333,
            "range": "± 359.651",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 9.46515,
            "range": "± 110.667",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 20.1673,
            "range": "± 144.719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.4739,
            "range": "± 525.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.0252,
            "range": "± 372.893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.0626,
            "range": "± 455.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.1713,
            "range": "± 425.777",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.5659,
            "range": "± 790.563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.5746,
            "range": "± 458.853",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.6221,
            "range": "± 410.088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.8042,
            "range": "± 346.674",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.9345,
            "range": "± 1.23653",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.3848,
            "range": "± 760.719",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.9493,
            "range": "± 459.206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.4586,
            "range": "± 616.891",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.2488,
            "range": "± 516.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.1111,
            "range": "± 803.707",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.518,
            "range": "± 2.03308",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.584,
            "range": "± 3.03603",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.3347,
            "range": "± 2.87886",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.535,
            "range": "± 2.837",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.9218,
            "range": "± 2.23582",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.6336,
            "range": "± 2.61428",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.886,
            "range": "± 2.48076",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.1769,
            "range": "± 2.84177",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.6663,
            "range": "± 2.7382",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.0904,
            "range": "± 2.67433",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.2957,
            "range": "± 3.34758",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 195.197,
            "range": "± 12.2473",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 48.422,
            "range": "± 5.53562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 299.653,
            "range": "± 12.6206",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.0326,
            "range": "± 3.41466",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 32.6304,
            "range": "± 4.60228",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 55.5562,
            "range": "± 9.17703",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 34.4447,
            "range": "± 4.39575",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 55.4285,
            "range": "± 7.06849",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 44.5101,
            "range": "± 6.34768",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 47.4608,
            "range": "± 8.01708",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.8457,
            "range": "± 3.61299",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 42.2186,
            "range": "± 7.38995",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 34.2164,
            "range": "± 4.0308",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 102.177,
            "range": "± 12.5263",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1018.13,
            "range": "± 16306.9",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 153.022,
            "range": "± 14.0383",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.12067,
            "range": "± 36.8643",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_addOne",
            "value": 54.7288,
            "range": "± 10.3191",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_addOne",
            "value": 3.20143,
            "range": "± 89.264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_addOne",
            "value": 24.2002,
            "range": "± 370.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_addOne",
            "value": 54.8253,
            "range": "± 7.39513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "tiered_compile_sumLoop",
            "value": 74.9284,
            "range": "± 9.8608",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_mlir_sumLoop",
            "value": 5.2186,
            "range": "± 127.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_cpp_sumLoop",
            "value": 25.186,
            "range": "± 471.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "single_compile_bc_sumLoop",
            "value": 76.9702,
            "range": "± 15.8736",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 10.3194,
            "range": "± 0.817987",
            "unit": "ns",
            "extra": "100 samples\n8447 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 13.1381,
            "range": "± 1.66876",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 573.654,
            "range": "± 29.4598",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.6173,
            "range": "± 0.497115",
            "unit": "ns",
            "extra": "100 samples\n6587 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.6564,
            "range": "± 7.26798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.909,
            "range": "± 125.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.1608,
            "range": "± 8.4102",
            "unit": "ns",
            "extra": "100 samples\n508 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 863.283,
            "range": "± 21.6704",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 185.856,
            "range": "± 337.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.57062,
            "range": "± 0.515894",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 20.7904,
            "range": "± 1.88624",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.84613,
            "range": "± 40.9779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add_noRegAlloc",
            "value": 46.5737,
            "range": "± 7.49692",
            "unit": "ns",
            "extra": "100 samples\n668 iterations"
          },
          {
            "name": "exec_bc_add_regAlloc",
            "value": 47.2947,
            "range": "± 8.42286",
            "unit": "ns",
            "extra": "100 samples\n674 iterations"
          },
          {
            "name": "exec_bc_fibonacci_noRegAlloc",
            "value": 856.144,
            "range": "± 25.2598",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_fibonacci_regAlloc",
            "value": 859.669,
            "range": "± 17.3758",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_noRegAlloc",
            "value": 185.865,
            "range": "± 339.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum_regAlloc",
            "value": 185.981,
            "range": "± 845.157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_addOne",
            "value": 36.3627,
            "range": "± 5.79257",
            "unit": "ns",
            "extra": "100 samples\n869 iterations"
          },
          {
            "name": "exec_mlir_addOne",
            "value": 255.173,
            "range": "± 6.3202",
            "unit": "ns",
            "extra": "100 samples\n9098 iterations"
          },
          {
            "name": "exec_cpp_addOne",
            "value": 3.67193,
            "range": "± 0.663436",
            "unit": "ns",
            "extra": "100 samples\n8434 iterations"
          },
          {
            "name": "exec_interpreted_addOne",
            "value": 40.3271,
            "range": "± 3.8603",
            "unit": "ns",
            "extra": "100 samples\n781 iterations"
          },
          {
            "name": "e2e_tiered_bc_to_mlir",
            "value": 57.1443,
            "range": "± 11.0124",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "e2e_single_mlir",
            "value": 5.52667,
            "range": "± 358.34",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}