window.BENCHMARK_DATA = {
  "lastUpdate": 1775020853895,
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
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "79f942229da7676594e9dfada038906983305422",
          "message": "add",
          "timestamp": "2026-03-11T21:30:20+01:00",
          "tree_id": "e1e8117f374f9fb737b24f2624b9ff6db252f422",
          "url": "https://github.com/nebulastream/nautilus/commit/79f942229da7676594e9dfada038906983305422"
        },
        "date": 1773261388845,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 833.197,
            "range": "± 100.91",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.42522,
            "range": "± 315.43",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.1058,
            "range": "± 609.386",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.79704,
            "range": "± 333.04",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.63151,
            "range": "± 1.60087",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.11314,
            "range": "± 353.352",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.5556,
            "range": "± 331.927",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.22676,
            "range": "± 346.761",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.09305,
            "range": "± 356.2",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.55443,
            "range": "± 343.827",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.3523,
            "range": "± 1.79192",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 193.378,
            "range": "± 8.27678",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 26.9945,
            "range": "± 1.85229",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 331.337,
            "range": "± 10.2576",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.51452,
            "range": "± 423.495",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.421,
            "range": "± 1.87314",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.7357,
            "range": "± 6.49182",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1979,
            "range": "± 1.67575",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.3851,
            "range": "± 3.10349",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.2706,
            "range": "± 1.76003",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.0674,
            "range": "± 3.49663",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.42764,
            "range": "± 1.21745",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2535,
            "range": "± 1.53539",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.2851,
            "range": "± 1.66701",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.4194,
            "range": "± 9.56421",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.69135,
            "range": "± 75.3193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.778,
            "range": "± 9.94956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.00654,
            "range": "± 139.386",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 191.025,
            "range": "± 27.2621",
            "unit": "ns",
            "extra": "100 samples\n156 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 455.149,
            "range": "± 43.2646",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.08691,
            "range": "± 99.3407",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 484.441,
            "range": "± 41.7335",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 856.468,
            "range": "± 96.258",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 496.227,
            "range": "± 55.3885",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 566.593,
            "range": "± 47.6744",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 384.974,
            "range": "± 38.6843",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.171,
            "range": "± 61.5343",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 451.341,
            "range": "± 40.0032",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.2128,
            "range": "± 0.487002",
            "unit": "ns",
            "extra": "100 samples\n9303 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.90567,
            "range": "± 838.948",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 727.363,
            "range": "± 32.6014",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.42384,
            "range": "± 0.76218",
            "unit": "ns",
            "extra": "100 samples\n5471 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.4949,
            "range": "± 9.5804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0693,
            "range": "± 98.9053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.2322,
            "range": "± 5.6097",
            "unit": "ns",
            "extra": "100 samples\n735 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 913.637,
            "range": "± 6.90465",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.974,
            "range": "± 4.87541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.55551,
            "range": "± 201.939",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.19554,
            "range": "± 335.394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.06517,
            "range": "± 313.526",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.6096,
            "range": "± 296.142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.7325,
            "range": "± 382.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.8029,
            "range": "± 291.878",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.6364,
            "range": "± 668.656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.46184,
            "range": "± 500.759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.2621,
            "range": "± 611.849",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1302,
            "range": "± 311.36",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.6131,
            "range": "± 410.67",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.9533,
            "range": "± 329.784",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.436,
            "range": "± 344.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.6183,
            "range": "± 684.918",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.2121,
            "range": "± 1.03437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.3444,
            "range": "± 836.118",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.2787,
            "range": "± 718.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.598,
            "range": "± 736.91",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.4997,
            "range": "± 429.971",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.1077,
            "range": "± 355.458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 30.1728,
            "range": "± 516.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.0255,
            "range": "± 420.823",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.9429,
            "range": "± 705.366",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.2415,
            "range": "± 625.623",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 32.0361,
            "range": "± 880.113",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.8883,
            "range": "± 299.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 34.4447,
            "range": "± 636.69",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 96.018,
            "range": "± 569.113",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.16504,
            "range": "± 91.8024",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.46071,
            "range": "± 366.601",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.89255,
            "range": "± 880.249",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.47936,
            "range": "± 215.553",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.42442,
            "range": "± 492.907",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.69324,
            "range": "± 278.206",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.21805,
            "range": "± 333.622",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.63251,
            "range": "± 257.949",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.58752,
            "range": "± 138.293",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.3306,
            "range": "± 275.98",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.5719,
            "range": "± 1.94507",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 174.036,
            "range": "± 11.9673",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.8098,
            "range": "± 2.39661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 284.34,
            "range": "± 10.1707",
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
          "id": "bc3ff4abeede13f9afa2ab7116ba3d770531e8e1",
          "message": "Fix bc backend argument dispatch rejecting unsigned long / const void*\n\nOn macOS (and any platform where size_t == unsigned long != uint64_t),\ncalling a registered function with a size_t argument threw\n\"This type is not supported\" because BCInterpreter only tried\nstd::any_cast<uint64_t>. Since uint64_t is unsigned long long there,\nthe cast fails and the catch-all throw fires.\n\nAdd unsigned long and const void* branches to the any_cast chain so\nall pointer/integer variants accepted by the Nautilus type system are\ncovered.\n\nhttps://claude.ai/code/session_01JhefWS3GbXoU13aQZmzaUK",
          "timestamp": "2026-03-11T22:54:47+01:00",
          "tree_id": "f45371507cbf0686fc3b7ecdb65229fdb90f4be8",
          "url": "https://github.com/nebulastream/nautilus/commit/bc3ff4abeede13f9afa2ab7116ba3d770531e8e1"
        },
        "date": 1773266356305,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.25087,
            "range": "± 253.335",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8176,
            "range": "± 1.41559",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.1527,
            "range": "± 5.23322",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8253,
            "range": "± 1.48415",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.3058,
            "range": "± 2.75566",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.0173,
            "range": "± 1.69326",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.2354,
            "range": "± 2.92882",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.10381,
            "range": "± 1.12021",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2284,
            "range": "± 2.14081",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.4573,
            "range": "± 1.96837",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.335,
            "range": "± 6.30419",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.6644,
            "range": "± 63.3599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.001,
            "range": "± 9.37555",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.97144,
            "range": "± 50.8256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.23421,
            "range": "± 248.506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.90391,
            "range": "± 361.684",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.77218,
            "range": "± 228.519",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.80012,
            "range": "± 397.363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.2997,
            "range": "± 656.745",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5366,
            "range": "± 503.559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.455,
            "range": "± 663.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.60352,
            "range": "± 202.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.258,
            "range": "± 596.206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1404,
            "range": "± 410.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.112,
            "range": "± 399.296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.4038,
            "range": "± 694.979",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5916,
            "range": "± 539.863",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.7173,
            "range": "± 634.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.16517,
            "range": "± 0.380612",
            "unit": "ns",
            "extra": "100 samples\n9317 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.84681,
            "range": "± 381.303",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 756.848,
            "range": "± 21.344",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.51979,
            "range": "± 0.973819",
            "unit": "ns",
            "extra": "100 samples\n5483 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.7109,
            "range": "± 17.5171",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1768,
            "range": "± 470.66",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.4543,
            "range": "± 4.3966",
            "unit": "ns",
            "extra": "100 samples\n714 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 918.488,
            "range": "± 9.04364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 198.191,
            "range": "± 772.983",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 195.151,
            "range": "± 22.8947",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 464.695,
            "range": "± 66.9164",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.12276,
            "range": "± 123.25",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 462.42,
            "range": "± 26.2424",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 876.763,
            "range": "± 93.907",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 492.411,
            "range": "± 54.5269",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.3,
            "range": "± 38.942",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 384.408,
            "range": "± 43.3959",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.005,
            "range": "± 55.9363",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 470.846,
            "range": "± 74.8718",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ir_add",
            "value": 897.467,
            "range": "± 88.0248",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.4728,
            "range": "± 226.825",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.59651,
            "range": "± 790.731",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.95247,
            "range": "± 312.831",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.66317,
            "range": "± 610.826",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.11491,
            "range": "± 285.106",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.77208,
            "range": "± 445.117",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.32257,
            "range": "± 306.095",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.14993,
            "range": "± 286.603",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.71428,
            "range": "± 310.165",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.4894,
            "range": "± 1.6747",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 185.444,
            "range": "± 5.71041",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.0826,
            "range": "± 2.57493",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 353.99,
            "range": "± 13.9044",
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
          "id": "42b06f2e5bf70902b8d85ae4e22442fef27bf673",
          "message": "Redesign tracing infrastructure and add struct/pointer value support\n\n- Introduce `LazyTraceContext` (formerly `ExceptionBasedTraceContext`) as the\n  default hot-path trace context; the old exception-based approach is retained\n  as `ExceptionBasedTraceContext`\n- Add `CompletingTraceContext` for optimized completion of in-progress traces\n- Fix `SSACreationPhase` to use the correct `ValueRef` width in data structures\n\n- Add `val<StructType>` support in `val_std.hpp` with stack allocation, field\n  access (`getField`/`setField`), and full copy semantics (copy-construct,\n  copy-assign, move)\n- Introduce `AllocaOperation` IR node and lower it in all three backends\n  (MLIR, C++, bytecode)\n\n- Extend `val<T*>` with compound assignment (`+=`, `-=`), pre-increment (`++`),\n  and mixed signed/unsigned integer offset types\n- Add comprehensive pointer arithmetic test data (60+ trace reference files)\n\n- Fix 64-bit dyncall arguments being silently truncated to 32 bits\n- Fix argument dispatch rejecting `unsigned long` / `const void*` parameter types\n\n- Add `PointerExecutionTest` with dedicated interpreter and compiler test cases\n- Add `BitIntrinsicExecutionTest` and `MemoryIntrinsicExecutionTest`\n- Add `ValueTypeFunctions.hpp` with struct construction, copy, loop, and\n  destructor scenarios; add matching trace/IR/after_ssa reference data\n- Expand `IntegerValTypeTest` and `FloatValTypeTest` coverage\n- Refactor `ExecutionTest.cpp` to use the new shared helper headers\n\nCo-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-03-13T00:01:25+01:00",
          "tree_id": "7a35363ef284d7217e51c387e26d54a98ab2fc67",
          "url": "https://github.com/nebulastream/nautilus/commit/42b06f2e5bf70902b8d85ae4e22442fef27bf673"
        },
        "date": 1773356897611,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 875.67,
            "range": "± 117.539",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.39865,
            "range": "± 195.178",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.30964,
            "range": "± 739.798",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.79973,
            "range": "± 288.253",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.48127,
            "range": "± 844.469",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.03419,
            "range": "± 294.66",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.62788,
            "range": "± 587.766",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.18144,
            "range": "± 239.361",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.95236,
            "range": "± 252.305",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.50228,
            "range": "± 232.901",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.2751,
            "range": "± 1.88505",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 194.296,
            "range": "± 6.45836",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.4491,
            "range": "± 2.81121",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 335.882,
            "range": "± 15.4768",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.34541,
            "range": "± 233.795",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.39801,
            "range": "± 328.913",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.1343,
            "range": "± 1.77564",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.9901,
            "range": "± 662.595",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.84,
            "range": "± 6.76788",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.0847,
            "range": "± 1.85096",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1334,
            "range": "± 1.71971",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.11854,
            "range": "± 862.352",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.1691,
            "range": "± 2.94759",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.11277,
            "range": "± 1.14259",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.056,
            "range": "± 1.82917",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.13736,
            "range": "± 612.883",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.2433,
            "range": "± 3.15972",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.1855,
            "range": "± 1.70993",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.94896,
            "range": "± 964.499",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.70924,
            "range": "± 850.833",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2231,
            "range": "± 1.78476",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.0743,
            "range": "± 709.296",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.3102,
            "range": "± 1.85057",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.20987,
            "range": "± 539.909",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.8469,
            "range": "± 7.73898",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 52.9195,
            "range": "± 6.68343",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.68826,
            "range": "± 31.4391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.70665,
            "range": "± 34.6779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.463,
            "range": "± 10.2249",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 67.2255,
            "range": "± 8.41805",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.033,
            "range": "± 193.898",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.59485,
            "range": "± 40.1513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_exceptionBasedTracing_add",
            "value": 3.49536,
            "range": "± 0.434358",
            "unit": "ns",
            "extra": "100 samples\n8453 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.26934,
            "range": "± 304.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.79053,
            "range": "± 244.329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.65906,
            "range": "± 218.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.95348,
            "range": "± 368.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.8594,
            "range": "± 555.508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.8529,
            "range": "± 462.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.2327,
            "range": "± 461.238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.87485,
            "range": "± 428.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.2682,
            "range": "± 444.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1474,
            "range": "± 387.577",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.2458,
            "range": "± 810.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.4517,
            "range": "± 574.633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.1962,
            "range": "± 367.028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.058,
            "range": "± 434.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.6769,
            "range": "± 770.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.8178,
            "range": "± 835.506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.137,
            "range": "± 901.69",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4648,
            "range": "± 731.866",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.6567,
            "range": "± 785.016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.1435,
            "range": "± 1.05651",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.7042,
            "range": "± 788.02",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.2341,
            "range": "± 1.1287",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.1025,
            "range": "± 917.306",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.7169,
            "range": "± 831.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.85,
            "range": "± 961.771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.0396,
            "range": "± 645.732",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.9167,
            "range": "± 873.709",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.1515,
            "range": "± 945.125",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.19199,
            "range": "± 76.1629",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.49254,
            "range": "± 353.597",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.79116,
            "range": "± 441.541",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.48399,
            "range": "± 186.026",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.43329,
            "range": "± 319.433",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.61252,
            "range": "± 172.089",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.11321,
            "range": "± 251.952",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.59205,
            "range": "± 173.921",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.67762,
            "range": "± 199.407",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.28471,
            "range": "± 157.07",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.3737,
            "range": "± 1.83705",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 174.271,
            "range": "± 8.13386",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.3713,
            "range": "± 2.3302",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 287.586,
            "range": "± 20.9015",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 178.663,
            "range": "± 12.483",
            "unit": "ns",
            "extra": "100 samples\n163 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 449.395,
            "range": "± 37.8686",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.09061,
            "range": "± 60.9353",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 466.368,
            "range": "± 48.2074",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 858.796,
            "range": "± 59.6165",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 472.298,
            "range": "± 52.2188",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 559.06,
            "range": "± 37.5033",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 416.568,
            "range": "± 123.211",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 474.498,
            "range": "± 26.1928",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 430.374,
            "range": "± 25.5837",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
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
          "id": "a94c041122ad7019405ee551a9ddd4e578949668",
          "message": "Add function pointer support in val<> objects\n\nIntroduces val<FnPtrType> specialization enabling function pointers to\nbe traced and called through the Nautilus tracing infrastructure:\n\n- Add val_func.hpp with val<R(*)(Args...)> specialization supporting\n  call, comparison, null-check, and cast operations\n- Add IndirectCallOperation IR node and lower it in all backends\n  (MLIR, C++, bytecode)\n- Extend tracing infrastructure with traceIndirectCall\n- Add FunctionPtrExecutionTest, FunctionPtrValTypeTest, and\n  tracing/IR reference data for 18 function pointer scenarios\n- Fix clang-format-21 violations (operator spacing, pointer-to-member,\n  formatter signatures, lambda bodies, block formatting)\n\nhttps://claude.ai/code/session_01Pd5dwPbj3FrkdgdC1rAsug",
          "timestamp": "2026-03-13T20:56:35+01:00",
          "tree_id": "c37abcde4538f9fdc0a30e69182b1baffcb232b0",
          "url": "https://github.com/nebulastream/nautilus/commit/a94c041122ad7019405ee551a9ddd4e578949668"
        },
        "date": 1773432218299,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.31269,
            "range": "± 152.08",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.27177,
            "range": "± 297.347",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.201,
            "range": "± 1.48931",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.89995,
            "range": "± 502.813",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.8433,
            "range": "± 6.70751",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.977,
            "range": "± 1.69294",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8828,
            "range": "± 1.629",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.01563,
            "range": "± 471.123",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.5118,
            "range": "± 2.69996",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.24287,
            "range": "± 1.02439",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.0114,
            "range": "± 1.58471",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.91857,
            "range": "± 542.048",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.3969,
            "range": "± 2.52489",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 16.8165,
            "range": "± 1.85588",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.6353,
            "range": "± 1.3183",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.60258,
            "range": "± 1.03259",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2836,
            "range": "± 1.65846",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.21119,
            "range": "± 669.975",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.1338,
            "range": "± 1.52478",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.25056,
            "range": "± 426.246",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.3211,
            "range": "± 6.28067",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 58.8817,
            "range": "± 6.66274",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.73114,
            "range": "± 61.9958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.78391,
            "range": "± 58.1985",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 135.901,
            "range": "± 11.1646",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 68.0422,
            "range": "± 7.32388",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.07229,
            "range": "± 53.4982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.72815,
            "range": "± 29.9088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 886.753,
            "range": "± 155.841",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.37082,
            "range": "± 144.788",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.4817,
            "range": "± 594.719",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.76964,
            "range": "± 259.632",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.47838,
            "range": "± 506.981",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.1426,
            "range": "± 310.146",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.62442,
            "range": "± 253.194",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.11912,
            "range": "± 112.156",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.95137,
            "range": "± 236.135",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.49399,
            "range": "± 222.552",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 14.8752,
            "range": "± 962.385",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 180.715,
            "range": "± 6.96718",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.9073,
            "range": "± 1.88501",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 369.384,
            "range": "± 21.5391",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 189.854,
            "range": "± 8.8728",
            "unit": "ns",
            "extra": "100 samples\n154 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 470.398,
            "range": "± 33.5247",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.18264,
            "range": "± 67.8288",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 500.311,
            "range": "± 24.3476",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 934.871,
            "range": "± 68.4223",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 544.303,
            "range": "± 58.6122",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 602.57,
            "range": "± 47.5605",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 417.082,
            "range": "± 20.4485",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 522.363,
            "range": "± 33.9407",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 454.516,
            "range": "± 22.3378",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.76337,
            "range": "± 359.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.59782,
            "range": "± 491.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.85152,
            "range": "± 257.963",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.9616,
            "range": "± 384.277",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.828,
            "range": "± 574.216",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.5591,
            "range": "± 501.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.5597,
            "range": "± 813.897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.99904,
            "range": "± 470.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.8717,
            "range": "± 740.229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.8624,
            "range": "± 521.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.5905,
            "range": "± 395.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.4807,
            "range": "± 803.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.0733,
            "range": "± 490.62",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.6901,
            "range": "± 684.597",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.5027,
            "range": "± 1.26729",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.2713,
            "range": "± 1.34548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.0089,
            "range": "± 884.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.9151,
            "range": "± 1.45235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.7793,
            "range": "± 1.22846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.1899,
            "range": "± 1.09701",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.7113,
            "range": "± 1.08924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.6982,
            "range": "± 1.34717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.3696,
            "range": "± 1.30053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.9277,
            "range": "± 1.15231",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 33.4369,
            "range": "± 988.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 66.8613,
            "range": "± 1.72143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 35.7876,
            "range": "± 1.28741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 96.7916,
            "range": "± 1.54812",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.16043,
            "range": "± 68.7809",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.5821,
            "range": "± 935.645",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.95746,
            "range": "± 645.183",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.44889,
            "range": "± 141.26",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.50943,
            "range": "± 397.636",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.60618,
            "range": "± 159.847",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.19648,
            "range": "± 611.096",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.55605,
            "range": "± 150.218",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.66599,
            "range": "± 240.094",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.26902,
            "range": "± 186.223",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 19.6324,
            "range": "± 2.59418",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 182.875,
            "range": "± 21.9627",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.6953,
            "range": "± 1.81753",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 294.781,
            "range": "± 9.41496",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_exceptionBasedTracing_add",
            "value": 3.47926,
            "range": "± 0.419387",
            "unit": "ns",
            "extra": "100 samples\n8464 iterations"
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
          "id": "9a7307dfbb77d9c3425815f56062ba26bad2a8ca",
          "message": "Update nautilus version to 0.2",
          "timestamp": "2026-03-15T21:34:49+01:00",
          "tree_id": "4c45fa4d2f2bfe322834446ca468b0b83fdec8ab",
          "url": "https://github.com/nebulastream/nautilus/commit/9a7307dfbb77d9c3425815f56062ba26bad2a8ca"
        },
        "date": 1773607386993,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_exceptionBasedTracing_add",
            "value": 3.48046,
            "range": "± 0.406099",
            "unit": "ns",
            "extra": "100 samples\n8487 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.44559,
            "range": "± 283.395",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.14339,
            "range": "± 323.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.95403,
            "range": "± 328.567",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.6629,
            "range": "± 352.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.1626,
            "range": "± 665.55",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.5933,
            "range": "± 484.005",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.0252,
            "range": "± 722.374",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.58285,
            "range": "± 203.157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0035,
            "range": "± 170.696",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1274,
            "range": "± 282.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.962,
            "range": "± 230.43",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.4551,
            "range": "± 585.369",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0449,
            "range": "± 275.735",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.6421,
            "range": "± 304.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.3507,
            "range": "± 11.6581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.4667,
            "range": "± 4.71049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.6016,
            "range": "± 966.495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.187,
            "range": "± 501.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.6059,
            "range": "± 715.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.0114,
            "range": "± 956.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.6922,
            "range": "± 439.957",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.4643,
            "range": "± 662.897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.5782,
            "range": "± 606.912",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.9952,
            "range": "± 513.327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.4356,
            "range": "± 836.846",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.4458,
            "range": "± 650.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.3093,
            "range": "± 736.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.2998,
            "range": "± 832.98",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.16388,
            "range": "± 101.727",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.54147,
            "range": "± 274.105",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.8377,
            "range": "± 496.293",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.4616,
            "range": "± 217.051",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.45922,
            "range": "± 438.668",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.64654,
            "range": "± 251.661",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.1392,
            "range": "± 269.834",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.56921,
            "range": "± 224.759",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.75945,
            "range": "± 215.061",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.2933,
            "range": "± 295.611",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.4222,
            "range": "± 1.65035",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 171.45,
            "range": "± 6.11061",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.5894,
            "range": "± 2.55002",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 281.49,
            "range": "± 10.1982",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 899.874,
            "range": "± 88.4603",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.45968,
            "range": "± 194.458",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.8956,
            "range": "± 699.797",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.01259,
            "range": "± 294.629",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.81649,
            "range": "± 504.084",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.19327,
            "range": "± 229.541",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.91827,
            "range": "± 323.211",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.28957,
            "range": "± 191.462",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.21663,
            "range": "± 249.608",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.60746,
            "range": "± 176.594",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.7656,
            "range": "± 1.48957",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 189.931,
            "range": "± 16.6957",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.7299,
            "range": "± 2.47063",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 362.521,
            "range": "± 13.0853",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.57588,
            "range": "± 237.328",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.47982,
            "range": "± 207.934",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.7962,
            "range": "± 2.17104",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.2445,
            "range": "± 449.173",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.6607,
            "range": "± 6.4553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.2547,
            "range": "± 2.72577",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3576,
            "range": "± 1.62464",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.52113,
            "range": "± 748.495",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.8893,
            "range": "± 3.49203",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.2581,
            "range": "± 1.24555",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.4882,
            "range": "± 1.60555",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.47582,
            "range": "± 581.106",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.4877,
            "range": "± 2.73773",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.237,
            "range": "± 1.09322",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.70491,
            "range": "± 1.3193",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.13828,
            "range": "± 1.15965",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.8134,
            "range": "± 1.35344",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.86734,
            "range": "± 729.09",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6638,
            "range": "± 1.55074",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.52686,
            "range": "± 435.747",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.0957,
            "range": "± 4.53114",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.8575,
            "range": "± 7.84418",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.75553,
            "range": "± 40.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.78787,
            "range": "± 43.5404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 136.106,
            "range": "± 7.36407",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.6913,
            "range": "± 6.2499",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.17692,
            "range": "± 66.0881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.80988,
            "range": "± 32.5042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 200.511,
            "range": "± 20.566",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 480.401,
            "range": "± 39.186",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.2346,
            "range": "± 146.528",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 517.286,
            "range": "± 41.3038",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 955.734,
            "range": "± 78.3557",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 548.195,
            "range": "± 61.4799",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 623.738,
            "range": "± 88.4019",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 417.794,
            "range": "± 36.1275",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 541.134,
            "range": "± 72.6656",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 511.417,
            "range": "± 66.4978",
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
          "id": "8f4ae3808e7ca74c1d0fb58a73474087ee203b6a",
          "message": "ci: auto-apply clang-format in PR workflow instead of just checking",
          "timestamp": "2026-03-15T21:54:49+01:00",
          "tree_id": "837092c33b3c869f919f038ed0a297fcabda38f9",
          "url": "https://github.com/nebulastream/nautilus/commit/8f4ae3808e7ca74c1d0fb58a73474087ee203b6a"
        },
        "date": 1773608371157,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 9.14059,
            "range": "± 208.114",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.64632,
            "range": "± 516.517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.48324,
            "range": "± 522.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.1337,
            "range": "± 207.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.9808,
            "range": "± 1.50736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.0157,
            "range": "± 774.428",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.6611,
            "range": "± 573.372",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.1111,
            "range": "± 214.08",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.457,
            "range": "± 227.633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.1087,
            "range": "± 274.558",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.2784,
            "range": "± 300.429",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.9713,
            "range": "± 407.363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.4465,
            "range": "± 503.39",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.7725,
            "range": "± 354.266",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.0629,
            "range": "± 596.623",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.4753,
            "range": "± 661.069",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.5934,
            "range": "± 496.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.9827,
            "range": "± 475.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.9702,
            "range": "± 739.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.111,
            "range": "± 528.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.7543,
            "range": "± 453.589",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.392,
            "range": "± 402.648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.8773,
            "range": "± 449.938",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.5339,
            "range": "± 808.088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 32.8191,
            "range": "± 779.874",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 66.9711,
            "range": "± 1.03536",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 35.0216,
            "range": "± 581.048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 96.7746,
            "range": "± 1.49282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.22573,
            "range": "± 151.613",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.55029,
            "range": "± 354.319",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.9014,
            "range": "± 703.839",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.50556,
            "range": "± 205.203",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.62736,
            "range": "± 493.87",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.68881,
            "range": "± 243.085",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.31906,
            "range": "± 556.592",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.65734,
            "range": "± 395.177",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.85781,
            "range": "± 555.065",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36594,
            "range": "± 379.921",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.9218,
            "range": "± 2.52059",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 183.133,
            "range": "± 9.43804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 29.1115,
            "range": "± 2.49221",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 308.753,
            "range": "± 13.7695",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_exceptionBasedTracing_add",
            "value": 3.54347,
            "range": "± 0.670031",
            "unit": "ns",
            "extra": "100 samples\n8454 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.65823,
            "range": "± 348.867",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.60382,
            "range": "± 344.449",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.8746,
            "range": "± 1.8353",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.52976,
            "range": "± 835.709",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.6735,
            "range": "± 6.49569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.3904,
            "range": "± 3.18376",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.1729,
            "range": "± 2.07514",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.58629,
            "range": "± 905.813",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.6209,
            "range": "± 3.34165",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 11.0946,
            "range": "± 2.12194",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.6472,
            "range": "± 1.91599",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.66265,
            "range": "± 920.575",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.5569,
            "range": "± 3.94056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.1685,
            "range": "± 2.20339",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.88096,
            "range": "± 1.48939",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.58508,
            "range": "± 1.28573",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.1042,
            "range": "± 2.02165",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.11487,
            "range": "± 991.834",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6568,
            "range": "± 1.83025",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.76612,
            "range": "± 677.627",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.8803,
            "range": "± 8.04462",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 56.9079,
            "range": "± 8.87364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80672,
            "range": "± 65.8324",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.81535,
            "range": "± 74.9717",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 138.185,
            "range": "± 11.4025",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.212,
            "range": "± 9.80877",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.15079,
            "range": "± 56.6822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 3.00683,
            "range": "± 553.459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 201.067,
            "range": "± 22.7099",
            "unit": "ns",
            "extra": "100 samples\n158 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 490.553,
            "range": "± 57.3313",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.25869,
            "range": "± 150.395",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 519.225,
            "range": "± 52.0749",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 984.679,
            "range": "± 110.268",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 527.762,
            "range": "± 46.2574",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 629.164,
            "range": "± 62.2299",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 428.248,
            "range": "± 47.0471",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 550.02,
            "range": "± 59.4941",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 492.662,
            "range": "± 56.0116",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ir_add",
            "value": 919.485,
            "range": "± 119.311",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.5596,
            "range": "± 306.223",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.66555,
            "range": "± 493.748",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.03924,
            "range": "± 317.768",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.83398,
            "range": "± 609.797",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.30006,
            "range": "± 358.607",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.95047,
            "range": "± 432.656",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.44016,
            "range": "± 339.689",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.30908,
            "range": "± 398.495",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.74317,
            "range": "± 293.309",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.6319,
            "range": "± 1.94122",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 195.225,
            "range": "± 8.60083",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.4235,
            "range": "± 2.20987",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 372.936,
            "range": "± 17.5288",
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
          "id": "328fb5615e620b37e8faba28b1557ba69b90be5c",
          "message": "Improve CMake setup, fix bugs, and apply formatting\n\n- Fix 13 CMake issues: bugs, naming errors, and code quality\n- Suppress warnings on Catch2 third-party target\n- Fix three pre-existing bugs exposed by AddressSanitizer\n- Apply clang-format to files with formatting drift\n- Use non-type template parameter for memberFunc to fix same-type collision\n\nCo-Authored-By: Claude Opus 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-03-16T20:17:52+01:00",
          "tree_id": "204640443d8dba46ac3973deb9722c374f137814",
          "url": "https://github.com/nebulastream/nautilus/commit/328fb5615e620b37e8faba28b1557ba69b90be5c"
        },
        "date": 1773689175019,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.62816,
            "range": "± 188.724",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.35392,
            "range": "± 222.397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.03719,
            "range": "± 201.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.5684,
            "range": "± 192.571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.967,
            "range": "± 222.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.418,
            "range": "± 218.544",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.0844,
            "range": "± 359.041",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.09436,
            "range": "± 222.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.8355,
            "range": "± 241.61",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.6843,
            "range": "± 204.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.9405,
            "range": "± 441.093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.3902,
            "range": "± 1.21768",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.6975,
            "range": "± 288.877",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.9265,
            "range": "± 217.951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.51244,
            "range": "± 222.138",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.6814,
            "range": "± 346.117",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.6196,
            "range": "± 1.66543",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.58019,
            "range": "± 762.828",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.6451,
            "range": "± 6.81741",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.7299,
            "range": "± 2.60804",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3839,
            "range": "± 2.11522",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.35487,
            "range": "± 683.628",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.4536,
            "range": "± 3.24658",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.3777,
            "range": "± 1.74006",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.5893,
            "range": "± 1.95118",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.4868,
            "range": "± 789.909",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.5394,
            "range": "± 3.4163",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.4607,
            "range": "± 1.65568",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.50214,
            "range": "± 1.25819",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.15417,
            "range": "± 1.34041",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.9555,
            "range": "± 1.84762",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.9681,
            "range": "± 1.11662",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.5763,
            "range": "± 1.76841",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.58492,
            "range": "± 546.812",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.6451,
            "range": "± 9.11438",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.6305,
            "range": "± 8.07692",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.76424,
            "range": "± 41.9435",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.76226,
            "range": "± 52.844",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 139.085,
            "range": "± 12.5304",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.8363,
            "range": "± 10.6063",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.08093,
            "range": "± 59.6802",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.77328,
            "range": "± 94.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 195.336,
            "range": "± 20.0194",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 484.223,
            "range": "± 57.0633",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.22656,
            "range": "± 151.232",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 504.045,
            "range": "± 57.1274",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 935.057,
            "range": "± 95.8663",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 522.226,
            "range": "± 56.7494",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 613.833,
            "range": "± 74.5904",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 444.839,
            "range": "± 46.125",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 519.081,
            "range": "± 41.5688",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 469.298,
            "range": "± 38.0213",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ir_add",
            "value": 937.486,
            "range": "± 124.131",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.55599,
            "range": "± 254.809",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.7007,
            "range": "± 837.418",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.97809,
            "range": "± 246.196",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.88716,
            "range": "± 698.469",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.32064,
            "range": "± 369.222",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.83833,
            "range": "± 358.177",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25539,
            "range": "± 193.196",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.20711,
            "range": "± 296.661",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.73826,
            "range": "± 296.369",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.1939,
            "range": "± 1.52345",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 191.897,
            "range": "± 6.75116",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.7843,
            "range": "± 4.62042",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 366.137,
            "range": "± 12.4661",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_exceptionBasedTracing_add",
            "value": 3.55869,
            "range": "± 0.581567",
            "unit": "ns",
            "extra": "100 samples\n8459 iterations"
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
          "id": "8cc59fb824da2941cb8146fea9243fcc66d8dab9",
          "message": "Add LSan suppressions for known LLVM/MLIR internal memory leaks\n\nCompiles __lsan_default_suppressions() into all test executables via\nan OBJECT library, suppressing false-positive leak reports from LLVM's\nJIT infrastructure, target lowering, and MLIR execution engine.\n\nCo-Authored-By: Claude Opus 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-03-16T21:50:39+01:00",
          "tree_id": "8c3b46488d3085a201087191638e1b3568a1b234",
          "url": "https://github.com/nebulastream/nautilus/commit/8cc59fb824da2941cb8146fea9243fcc66d8dab9"
        },
        "date": 1773694611353,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.53334,
            "range": "± 0.500692",
            "unit": "ns",
            "extra": "100 samples\n8451 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89912,
            "range": "± 868.515",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 704.687,
            "range": "± 25.7623",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.34472,
            "range": "± 0.362613",
            "unit": "ns",
            "extra": "100 samples\n5471 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.596,
            "range": "± 8.42217",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.173,
            "range": "± 718.748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 27.9123,
            "range": "± 2.46425",
            "unit": "ns",
            "extra": "100 samples\n1074 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 938.181,
            "range": "± 12.977",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 199.905,
            "range": "± 700.225",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.36824,
            "range": "± 250.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.98219,
            "range": "± 281.071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.98325,
            "range": "± 340.688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.2299,
            "range": "± 374.975",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.6639,
            "range": "± 687.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.3182,
            "range": "± 440.849",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.0162,
            "range": "± 535.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.0167,
            "range": "± 345.066",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.5852,
            "range": "± 459.562",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5804,
            "range": "± 680.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.3932,
            "range": "± 342.485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.9694,
            "range": "± 388.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5834,
            "range": "± 324.798",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.6268,
            "range": "± 377.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.1344,
            "range": "± 464.956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.3823,
            "range": "± 541.601",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.1287,
            "range": "± 568.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.307,
            "range": "± 594.66",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.6223,
            "range": "± 638.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.6022,
            "range": "± 492.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.858,
            "range": "± 484.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.9471,
            "range": "± 518.904",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.1416,
            "range": "± 463.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.0643,
            "range": "± 613.554",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.3881,
            "range": "± 669.861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.0144,
            "range": "± 578.336",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.4704,
            "range": "± 458.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.0264,
            "range": "± 692.46",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.2172,
            "range": "± 3.16656",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.1232,
            "range": "± 3.2982",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.8026,
            "range": "± 3.88907",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.358,
            "range": "± 3.54532",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.918,
            "range": "± 3.10809",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.1445,
            "range": "± 3.36012",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.006,
            "range": "± 3.16612",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.3433,
            "range": "± 3.00784",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.8082,
            "range": "± 3.39128",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.3544,
            "range": "± 3.67408",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 42.7178,
            "range": "± 7.78642",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 179.657,
            "range": "± 8.96673",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.1513,
            "range": "± 6.91781",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 284.318,
            "range": "± 13.6887",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.59674,
            "range": "± 281.599",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.48621,
            "range": "± 238.667",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.375,
            "range": "± 1.76361",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.39635,
            "range": "± 667.613",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.4684,
            "range": "± 7.748",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.4155,
            "range": "± 2.79544",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.2365,
            "range": "± 1.88118",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.37193,
            "range": "± 679.778",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.4849,
            "range": "± 3.40881",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.0008,
            "range": "± 1.81148",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.4438,
            "range": "± 1.76525",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.35968,
            "range": "± 627.934",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.7979,
            "range": "± 3.28843",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.1615,
            "range": "± 1.78616",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.02956,
            "range": "± 1.02679",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.65975,
            "range": "± 1.00797",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.7657,
            "range": "± 1.77023",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.83746,
            "range": "± 838.775",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.4522,
            "range": "± 1.62214",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.48872,
            "range": "± 469.833",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.3888,
            "range": "± 8.53217",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 56.1171,
            "range": "± 9.01147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.7352,
            "range": "± 33.1781",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.74262,
            "range": "± 47.2529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 136.564,
            "range": "± 10.7948",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.8661,
            "range": "± 8.01183",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.08003,
            "range": "± 37.5048",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.76723,
            "range": "± 50.9903",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 856.213,
            "range": "± 73.2612",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.52135,
            "range": "± 164.606",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.58802,
            "range": "± 433.618",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.02379,
            "range": "± 247.131",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.69699,
            "range": "± 546.248",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.23176,
            "range": "± 206.311",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.82923,
            "range": "± 384.675",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.33074,
            "range": "± 152.09",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.10269,
            "range": "± 194.924",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.62749,
            "range": "± 154.589",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.8566,
            "range": "± 1.0993",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 191.158,
            "range": "± 5.76096",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.6145,
            "range": "± 1.81053",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 368.801,
            "range": "± 8.92011",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 183.875,
            "range": "± 10.5565",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 488.091,
            "range": "± 53.5745",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.16131,
            "range": "± 73.3087",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 507.417,
            "range": "± 50.1964",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 945.531,
            "range": "± 83.6547",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 501.338,
            "range": "± 38.5233",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 584.413,
            "range": "± 43.907",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 403.38,
            "range": "± 27.7446",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 511.641,
            "range": "± 45.2889",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 457.885,
            "range": "± 46.7882",
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
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "c1dedb3264e94e3e32b01d7ecba75a53cf667d07",
          "message": "Implement AsmJit backend and multi-function support\n\n- Add x86-64 AsmJit compilation backend with full IR lowering\n- Add multi-function support via BC-style dyncall thunks\n- Add val<StructType>, nested function support, and pointer improvements\n- Enable AsmJit backend by default in CI and execution tests\n- Fix AsmJit backend to use getFunctionOperations() API\n- Refactor AsmJit backend to use forward-reference pattern\n- Add CI workflow: auto-format then gate build/test on clean tree\n- Add LSan suppressions for known LLVM/MLIR internal leaks\n\nCo-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-03-17T21:31:48+01:00",
          "tree_id": "3ed0ae07c3c19773ed200b99b3713586b9e637ab",
          "url": "https://github.com/nebulastream/nautilus/commit/c1dedb3264e94e3e32b01d7ecba75a53cf667d07"
        },
        "date": 1773779899701,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.23494,
            "range": "± 150.707",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.21919,
            "range": "± 188.563",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.9986,
            "range": "± 1.47925",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.73592,
            "range": "± 414.772",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.3068,
            "range": "± 5.80635",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.2444,
            "range": "± 1.48402",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.9608,
            "range": "± 1.66897",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.93988,
            "range": "± 462.387",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.6787,
            "range": "± 2.9938",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.5202,
            "range": "± 1.14526",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.2631,
            "range": "± 1.7966",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.04126,
            "range": "± 681.218",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.6703,
            "range": "± 2.86734",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 17.0299,
            "range": "± 1.86532",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.7305,
            "range": "± 1.11731",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 10.1818,
            "range": "± 1.09482",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.3765,
            "range": "± 1.64057",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.13022,
            "range": "± 650.583",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.4523,
            "range": "± 1.70101",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.26388,
            "range": "± 466.686",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.0207,
            "range": "± 5.19624",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 59.3346,
            "range": "± 5.12279",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.69448,
            "range": "± 42.4397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.72011,
            "range": "± 46.3532",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 137.014,
            "range": "± 12.4005",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 68.6225,
            "range": "± 7.1216",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.03427,
            "range": "± 57.2728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.66705,
            "range": "± 41.8945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.50242,
            "range": "± 465.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.98343,
            "range": "± 219.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.77289,
            "range": "± 144.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.0582,
            "range": "± 230.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.2758,
            "range": "± 494.807",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.8899,
            "range": "± 526.676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.9919,
            "range": "± 467.308",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.76269,
            "range": "± 180.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.5238,
            "range": "± 660.946",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.7511,
            "range": "± 301.162",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.6072,
            "range": "± 556.966",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.4256,
            "range": "± 410.395",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.4923,
            "range": "± 279.072",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.9932,
            "range": "± 465.359",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.4908,
            "range": "± 362.012",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5647,
            "range": "± 370.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.9679,
            "range": "± 392.897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4074,
            "range": "± 520.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.651,
            "range": "± 1.33833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.9453,
            "range": "± 721.466",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.9816,
            "range": "± 804.187",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.8251,
            "range": "± 523.302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6591,
            "range": "± 270.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.6905,
            "range": "± 494.605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.0109,
            "range": "± 1.07166",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.617,
            "range": "± 1.12658",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.886,
            "range": "± 851.862",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 96.8529,
            "range": "± 2.45128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.0909,
            "range": "± 3.09169",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.65,
            "range": "± 5.17267",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.7857,
            "range": "± 3.53435",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.8842,
            "range": "± 3.56035",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.6583,
            "range": "± 3.06232",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.5045,
            "range": "± 3.41642",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.0285,
            "range": "± 3.58438",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.9674,
            "range": "± 3.358",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.3124,
            "range": "± 3.17817",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.9197,
            "range": "± 3.0128",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.615,
            "range": "± 4.26718",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 183.272,
            "range": "± 9.443",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.4039,
            "range": "± 6.36345",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 280.102,
            "range": "± 11.7704",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.0093,
            "range": "± 4.54655",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.8239,
            "range": "± 4.88091",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.4811,
            "range": "± 8.88876",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 42.076,
            "range": "± 9.05309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.9002,
            "range": "± 8.76738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.5631,
            "range": "± 7.97614",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.1764,
            "range": "± 8.5037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.2963,
            "range": "± 3.93393",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.6251,
            "range": "± 8.30186",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.902,
            "range": "± 4.6142",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 108.244,
            "range": "± 11.4435",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.24597,
            "range": "± 237.604",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 162.296,
            "range": "± 11.7048",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.3241,
            "range": "± 80.3855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.51138,
            "range": "± 0.472487",
            "unit": "ns",
            "extra": "100 samples\n8504 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.835,
            "range": "± 412.684",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 724.001,
            "range": "± 22.5776",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.32548,
            "range": "± 0.339456",
            "unit": "ns",
            "extra": "100 samples\n5498 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.7824,
            "range": "± 13.5216",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1186,
            "range": "± 238.921",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 27.6238,
            "range": "± 4.16086",
            "unit": "ns",
            "extra": "100 samples\n1107 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 836.563,
            "range": "± 7.09092",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.385,
            "range": "± 1.00508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.4863,
            "range": "± 0.282972",
            "unit": "ns",
            "extra": "100 samples\n8469 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.3575,
            "range": "± 2.69537",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.57214,
            "range": "± 54.0842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 844.221,
            "range": "± 59.0129",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.54079,
            "range": "± 293.79",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.65185,
            "range": "± 545.176",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.04464,
            "range": "± 384.632",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.81928,
            "range": "± 676.388",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.18895,
            "range": "± 227.411",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.8632,
            "range": "± 312.36",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.2839,
            "range": "± 179.17",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.24389,
            "range": "± 254.591",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.75962,
            "range": "± 299.161",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.5052,
            "range": "± 1.29662",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 184.205,
            "range": "± 6.53246",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.3958,
            "range": "± 1.77316",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 354.667,
            "range": "± 11.5752",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 192.201,
            "range": "± 17.7234",
            "unit": "ns",
            "extra": "100 samples\n161 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 472.799,
            "range": "± 36.4189",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.22376,
            "range": "± 134.582",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 500.984,
            "range": "± 35.0366",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 930.503,
            "range": "± 85.6623",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 499.755,
            "range": "± 34.7331",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 594.245,
            "range": "± 43.1431",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 433.005,
            "range": "± 41.5407",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 512.826,
            "range": "± 31.1273",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 452.435,
            "range": "± 23.277",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
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
          "id": "1631427e3029f39307fa53ad40e4d20e7451cfe4",
          "message": "Remove auto-format CI job\n\nThe auto-format job's commits (pushed with GITHUB_TOKEN) don't trigger\nsubsequent CI runs due to a GitHub Actions limitation. Removing it for\nnow; formatting can be enforced locally via ./format.sh or reintroduced\nas a merge-to-main step later.\n\nhttps://claude.ai/code/session_019SB52Y8jncbwAmhbiKf8s3",
          "timestamp": "2026-03-20T05:18:03+01:00",
          "tree_id": "6674cb69794d199b39ec88e73904a39bb5413acc",
          "url": "https://github.com/nebulastream/nautilus/commit/1631427e3029f39307fa53ad40e4d20e7451cfe4"
        },
        "date": 1773980533826,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.20748,
            "range": "± 185.566",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.12843,
            "range": "± 182.685",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.5788,
            "range": "± 2.36968",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.59247,
            "range": "± 753.311",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.8556,
            "range": "± 7.10404",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 13.4209,
            "range": "± 2.36773",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.6148,
            "range": "± 2.66829",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.68823,
            "range": "± 666.86",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.7745,
            "range": "± 2.98833",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 8.48706,
            "range": "± 852.705",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.371,
            "range": "± 1.28292",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 4.57583,
            "range": "± 629.028",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.0505,
            "range": "± 2.69478",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 16.5969,
            "range": "± 2.32689",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.86027,
            "range": "± 853.708",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.66705,
            "range": "± 1.0561",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.7998,
            "range": "± 2.59238",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 5.80466,
            "range": "± 961.057",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.88588,
            "range": "± 2.31131",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.04829,
            "range": "± 636.291",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.3708,
            "range": "± 6.18164",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 58.9415,
            "range": "± 8.43431",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.68866,
            "range": "± 37.2042",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.69427,
            "range": "± 47.9636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 129.294,
            "range": "± 5.71435",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 65.9744,
            "range": "± 11.3327",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.61426,
            "range": "± 55.5592",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.50538,
            "range": "± 53.0661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 191.278,
            "range": "± 30.0937",
            "unit": "ns",
            "extra": "100 samples\n98 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 446.104,
            "range": "± 34.8195",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.14898,
            "range": "± 67.0473",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 485.238,
            "range": "± 56.4915",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 883.854,
            "range": "± 62.4652",
            "unit": "ns",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 481.748,
            "range": "± 27.3095",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.539,
            "range": "± 32.0526",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 388.783,
            "range": "± 36.9818",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 496.066,
            "range": "± 40.3347",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 451.179,
            "range": "± 41.9342",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ir_add",
            "value": 794.61,
            "range": "± 54.8569",
            "unit": "ns",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.38402,
            "range": "± 155.476",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.27921,
            "range": "± 402.006",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.75109,
            "range": "± 265.142",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.3745,
            "range": "± 634.33",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.98509,
            "range": "± 223.017",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.50886,
            "range": "± 171.657",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.02546,
            "range": "± 119.709",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.90928,
            "range": "± 245.034",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.46363,
            "range": "± 188.565",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.049,
            "range": "± 751.131",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 168.612,
            "range": "± 4.08942",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 35.6151,
            "range": "± 13.0731",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 341.763,
            "range": "± 11.5289",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 2.67247,
            "range": "± 0.314305",
            "unit": "ns",
            "extra": "100 samples\n6909 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 6.64509,
            "range": "± 725.673",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 807.747,
            "range": "± 37.4179",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.75947,
            "range": "± 0.213952",
            "unit": "ns",
            "extra": "100 samples\n4747 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 130.103,
            "range": "± 6.43661",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 25.7347,
            "range": "± 125.689",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 21.848,
            "range": "± 3.25053",
            "unit": "ns",
            "extra": "100 samples\n795 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 585.898,
            "range": "± 23.6599",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 119.436,
            "range": "± 2.23633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 2.65306,
            "range": "± 0.241722",
            "unit": "ns",
            "extra": "100 samples\n6907 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 30.4315,
            "range": "± 4.4013",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 5.09255,
            "range": "± 47.2792",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.95837,
            "range": "± 246.963",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.38966,
            "range": "± 219.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.36125,
            "range": "± 207.196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.3718,
            "range": "± 373.403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.8269,
            "range": "± 841.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3461,
            "range": "± 422.394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5897,
            "range": "± 819.88",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.47521,
            "range": "± 361.885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.9078,
            "range": "± 395.305",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4098,
            "range": "± 378.168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.177,
            "range": "± 288.988",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5861,
            "range": "± 477.929",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 14.017,
            "range": "± 435.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 62.5488,
            "range": "± 923.97",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 22.637,
            "range": "± 867.784",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 23.1129,
            "range": "± 720.414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 24.5987,
            "range": "± 763.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 23.6871,
            "range": "± 668.862",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 24.7696,
            "range": "± 815.64",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.1031,
            "range": "± 782.771",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.0011,
            "range": "± 1.6108",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.622,
            "range": "± 676.827",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.2126,
            "range": "± 1.8389",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 24.6862,
            "range": "± 615.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 27.8774,
            "range": "± 586.35",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 60.9419,
            "range": "± 760.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.0979,
            "range": "± 961.391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 90.1485,
            "range": "± 854.208",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 11.249,
            "range": "± 2.54675",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 13.0535,
            "range": "± 2.88394",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 16.9788,
            "range": "± 2.77803",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 13.0777,
            "range": "± 2.84217",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 15.3054,
            "range": "± 2.56565",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 13.225,
            "range": "± 2.68542",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 14.6157,
            "range": "± 3.09485",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 12.4551,
            "range": "± 2.34402",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 13.2219,
            "range": "± 2.59993",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 13.0463,
            "range": "± 3.58884",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 32.6268,
            "range": "± 7.18207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 172.636,
            "range": "± 9.29578",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 41.4586,
            "range": "± 6.83031",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 283.517,
            "range": "± 13.5264",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 15.4551,
            "range": "± 4.53165",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 28.8706,
            "range": "± 8.18644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 46.0048,
            "range": "± 8.76507",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 30.8144,
            "range": "± 9.72711",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 46.7782,
            "range": "± 10.45",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 35.6417,
            "range": "± 9.78573",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 38.0723,
            "range": "± 10.3793",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 24.2081,
            "range": "± 7.93757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 33.6943,
            "range": "± 9.63385",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 30.7584,
            "range": "± 7.74037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 93.5034,
            "range": "± 14.1798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.1529,
            "range": "± 42.95",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 148.877,
            "range": "± 16.7585",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.24894,
            "range": "± 80.3009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "github-actions[bot]@users.noreply.github.com",
            "name": "github-actions[bot]",
            "username": "github-actions[bot]"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "481906fac1efdbd42419e7935f859d47c0527fb1",
          "message": "style: apply automatic clang-format",
          "timestamp": "2026-03-20T05:30:59+01:00",
          "tree_id": "eb8da8a3d69252a6681e6de4189914e9ba465d2c",
          "url": "https://github.com/nebulastream/nautilus/commit/481906fac1efdbd42419e7935f859d47c0527fb1"
        },
        "date": 1773981312541,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.46761,
            "range": "± 332.259",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.31906,
            "range": "± 215.685",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.111,
            "range": "± 1.41617",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 4.79633,
            "range": "± 468.591",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.4318,
            "range": "± 8.16749",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.2132,
            "range": "± 1.51839",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.5178,
            "range": "± 1.91006",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 4.92807,
            "range": "± 517.236",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.0481,
            "range": "± 3.44094",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.42387,
            "range": "± 1.146",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3165,
            "range": "± 1.97487",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.23383,
            "range": "± 701.445",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 23.146,
            "range": "± 3.14983",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 10.9074,
            "range": "± 2.14626",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.1406,
            "range": "± 1.24686",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 10.4499,
            "range": "± 1.44538",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.4184,
            "range": "± 1.5701",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.2668,
            "range": "± 722.915",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6062,
            "range": "± 1.55416",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.46871,
            "range": "± 607.868",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 58.9286,
            "range": "± 5.9134",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 61.3884,
            "range": "± 8.49738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.7335,
            "range": "± 54.8636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.75798,
            "range": "± 74.2362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 137.49,
            "range": "± 10.2363",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.0529,
            "range": "± 7.69306",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.05143,
            "range": "± 55.6885",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.71912,
            "range": "± 49.0053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 197.718,
            "range": "± 23.0711",
            "unit": "ns",
            "extra": "100 samples\n162 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 470.369,
            "range": "± 50.9551",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.15174,
            "range": "± 99.5186",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 494.807,
            "range": "± 52.2488",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 906.164,
            "range": "± 83.0561",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 505.224,
            "range": "± 56.6779",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 580.433,
            "range": "± 40.9895",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 397.802,
            "range": "± 36.0038",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 836.525,
            "range": "± 218.343",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 460.395,
            "range": "± 39.1041",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.2854,
            "range": "± 326.804",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.75192,
            "range": "± 167.175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.72139,
            "range": "± 193.892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.8889,
            "range": "± 333.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.8951,
            "range": "± 706.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6278,
            "range": "± 424.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.5471,
            "range": "± 520.366",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.74853,
            "range": "± 294.878",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.3754,
            "range": "± 393.133",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1076,
            "range": "± 221.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.019,
            "range": "± 392.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5506,
            "range": "± 543.562",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0606,
            "range": "± 175.911",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.0376,
            "range": "± 508.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 918.808,
            "range": "± 154.819",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.69465,
            "range": "± 459.721",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.01554,
            "range": "± 1.0657",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.1733,
            "range": "± 479.851",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.05799,
            "range": "± 1.06935",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.36562,
            "range": "± 489.506",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.89136,
            "range": "± 514.117",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.42362,
            "range": "± 389.453",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.24437,
            "range": "± 362.016",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.7592,
            "range": "± 292.024",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.9224,
            "range": "± 1.79436",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 188.045,
            "range": "± 7.42887",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.8589,
            "range": "± 2.43065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 371.525,
            "range": "± 12.3752",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.5205,
            "range": "± 0.533902",
            "unit": "ns",
            "extra": "100 samples\n8481 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.84471,
            "range": "± 366.345",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 706.998,
            "range": "± 32.464",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.6969,
            "range": "± 1.36003",
            "unit": "ns",
            "extra": "100 samples\n5481 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.2819,
            "range": "± 8.92013",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0789,
            "range": "± 88.5758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.116,
            "range": "± 8.67483",
            "unit": "ns",
            "extra": "100 samples\n685 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 814.736,
            "range": "± 7.11986",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 175.865,
            "range": "± 488.143",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.60134,
            "range": "± 0.646242",
            "unit": "ns",
            "extra": "100 samples\n8474 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.5342,
            "range": "± 2.39672",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.57918,
            "range": "± 38.1847",
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
          "id": "a87711005a153fb8cc9064cf297603edc203432e",
          "message": "Code formatting and cleanup: improve consistency and remove unused LoopOp (#195)\n\nCo-authored-by: Claude <noreply@anthropic.com>\nCo-authored-by: github-actions[bot] <github-actions[bot]@users.noreply.github.com>",
          "timestamp": "2026-03-20T06:08:50+01:00",
          "tree_id": "e79e9b1c9055a0424051e61655cd4adc347c997d",
          "url": "https://github.com/nebulastream/nautilus/commit/a87711005a153fb8cc9064cf297603edc203432e"
        },
        "date": 1773983651998,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 181.238,
            "range": "± 9.75423",
            "unit": "ns",
            "extra": "100 samples\n161 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 454.99,
            "range": "± 33.0564",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.13634,
            "range": "± 90.9876",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 475.974,
            "range": "± 29.117",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 868.214,
            "range": "± 46.3916",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 481.064,
            "range": "± 35.976",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 586.138,
            "range": "± 117.155",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 389.655,
            "range": "± 14.6858",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 489.795,
            "range": "± 20.8879",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 440.47,
            "range": "± 25.6656",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.57758,
            "range": "± 0.611458",
            "unit": "ns",
            "extra": "100 samples\n8446 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.00333,
            "range": "± 1.01754",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 242.05,
            "range": "± 22.421",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.29772,
            "range": "± 0.192515",
            "unit": "ns",
            "extra": "100 samples\n5471 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.5778,
            "range": "± 8.5822",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.5194,
            "range": "± 241.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 48.2326,
            "range": "± 10.9075",
            "unit": "ns",
            "extra": "100 samples\n560 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 814.79,
            "range": "± 12.3672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 175.285,
            "range": "± 289.704",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.40058,
            "range": "± 0.0134566",
            "unit": "ns",
            "extra": "100 samples\n8452 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.372,
            "range": "± 1.86088",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.12882,
            "range": "± 25.2665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.23201,
            "range": "± 157.7",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.81169,
            "range": "± 165.041",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.77312,
            "range": "± 149.337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.80401,
            "range": "± 157.529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.178,
            "range": "± 361.099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4629,
            "range": "± 163.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.5016,
            "range": "± 427.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.70412,
            "range": "± 172.524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.144,
            "range": "± 221.068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1617,
            "range": "± 283.893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1546,
            "range": "± 188.981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5946,
            "range": "± 284.725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.2346,
            "range": "± 204.208",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.8411,
            "range": "± 296.62",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.4956,
            "range": "± 371.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.1443,
            "range": "± 503.015",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.0832,
            "range": "± 522.338",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.2184,
            "range": "± 405.412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.1573,
            "range": "± 481.784",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.7901,
            "range": "± 625.304",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.0931,
            "range": "± 770.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2517,
            "range": "± 861.495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.3741,
            "range": "± 514.953",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.1239,
            "range": "± 394.974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.1215,
            "range": "± 386.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.3741,
            "range": "± 614.269",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.7281,
            "range": "± 621.793",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.5786,
            "range": "± 791.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.2572,
            "range": "± 2.87199",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.0159,
            "range": "± 2.4567",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.799,
            "range": "± 3.28623",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.2714,
            "range": "± 3.24083",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.886,
            "range": "± 2.89269",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.0534,
            "range": "± 3.37145",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.8747,
            "range": "± 4.01066",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.0413,
            "range": "± 2.72378",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.6579,
            "range": "± 2.92566",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.3577,
            "range": "± 3.03929",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 36.7441,
            "range": "± 4.7318",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 181.901,
            "range": "± 12.9796",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.8897,
            "range": "± 5.35156",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 283.982,
            "range": "± 13.3526",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.2981,
            "range": "± 3.94942",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.8447,
            "range": "± 4.83678",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.4877,
            "range": "± 8.14652",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 42.6838,
            "range": "± 8.40569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.5223,
            "range": "± 8.39369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.2526,
            "range": "± 8.84477",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.8956,
            "range": "± 9.01528",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.9444,
            "range": "± 4.04207",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.8558,
            "range": "± 6.35865",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.8624,
            "range": "± 3.93158",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 109.592,
            "range": "± 10.0423",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.13424,
            "range": "± 17.0626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 162.861,
            "range": "± 10.7538",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.25525,
            "range": "± 24.8791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.62465,
            "range": "± 305.313",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.45969,
            "range": "± 177.852",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.3228,
            "range": "± 1.45825",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.12495,
            "range": "± 550.938",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.3793,
            "range": "± 3.96259",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 14.9027,
            "range": "± 1.71921",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1507,
            "range": "± 1.13796",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.1868,
            "range": "± 562.333",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.2263,
            "range": "± 2.58799",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.89112,
            "range": "± 1.51897",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3458,
            "range": "± 1.45176",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.23554,
            "range": "± 482.16",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.3166,
            "range": "± 2.75053",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.0715,
            "range": "± 1.10625",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.0916,
            "range": "± 943.769",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.78275,
            "range": "± 958.427",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.7337,
            "range": "± 1.56987",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.59838,
            "range": "± 654.746",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.4949,
            "range": "± 1.50505",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.47336,
            "range": "± 352.122",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.9854,
            "range": "± 5.03278",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 53.5047,
            "range": "± 6.3299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.75408,
            "range": "± 26.5919",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.76732,
            "range": "± 36.0628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 135.693,
            "range": "± 9.09602",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 68.813,
            "range": "± 4.8884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.09655,
            "range": "± 40.9818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.81492,
            "range": "± 60.7431",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 863.703,
            "range": "± 59.7714",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46099,
            "range": "± 156.632",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.46484,
            "range": "± 433.054",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.99006,
            "range": "± 481.139",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.6424,
            "range": "± 469.238",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.22183,
            "range": "± 292.618",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.73237,
            "range": "± 271.18",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25097,
            "range": "± 121.364",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.1372,
            "range": "± 264.049",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.62877,
            "range": "± 330.057",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.2441,
            "range": "± 1.16937",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 190.125,
            "range": "± 8.53779",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.1935,
            "range": "± 2.11329",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 359.644,
            "range": "± 8.93462",
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
          "id": "ffc2b4a56014ec1872b5c39d0e1ec8e87a1626dc",
          "message": "Add codespace and Claude Code environment setup with Clang 21 (#200)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-03-20T06:22:05+01:00",
          "tree_id": "9c73053bac33ffc04f8fdc5f7c7ba3e5d07e6e26",
          "url": "https://github.com/nebulastream/nautilus/commit/ffc2b4a56014ec1872b5c39d0e1ec8e87a1626dc"
        },
        "date": 1773984365654,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.30076,
            "range": "± 196.867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.96916,
            "range": "± 226.881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.90782,
            "range": "± 279.582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.96545,
            "range": "± 329.774",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.96,
            "range": "± 898.445",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7873,
            "range": "± 418.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.1405,
            "range": "± 547.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.72716,
            "range": "± 213.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.3749,
            "range": "± 317.601",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.275,
            "range": "± 294.519",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.3919,
            "range": "± 633.736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.2214,
            "range": "± 477.554",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5574,
            "range": "± 383.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.5302,
            "range": "± 717.029",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.521,
            "range": "± 662.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0789,
            "range": "± 807.814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.9247,
            "range": "± 624.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.2674,
            "range": "± 889.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.8408,
            "range": "± 899.473",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3091,
            "range": "± 689.77",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.3725,
            "range": "± 616.695",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.7961,
            "range": "± 635.533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.5988,
            "range": "± 651.449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.8489,
            "range": "± 654.002",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.9838,
            "range": "± 636.095",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.3484,
            "range": "± 472.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.3525,
            "range": "± 569.414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.1674,
            "range": "± 1.3996",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.049,
            "range": "± 2.98522",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.3237,
            "range": "± 3.85832",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.7533,
            "range": "± 3.8615",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.4577,
            "range": "± 3.34172",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.7039,
            "range": "± 4.60834",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.1248,
            "range": "± 3.35748",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.2483,
            "range": "± 3.01674",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.8928,
            "range": "± 2.70087",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.4426,
            "range": "± 2.82329",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.2124,
            "range": "± 3.05115",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.7107,
            "range": "± 3.98449",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 179.59,
            "range": "± 11.9601",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.6787,
            "range": "± 6.13926",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 283.424,
            "range": "± 17.9779",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.5428,
            "range": "± 4.51229",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 35.1474,
            "range": "± 4.15809",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.0586,
            "range": "± 7.56084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 43.3629,
            "range": "± 8.00804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.9587,
            "range": "± 9.55033",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 48.3048,
            "range": "± 8.08901",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 51.4576,
            "range": "± 8.97409",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.2503,
            "range": "± 4.47311",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 45.4904,
            "range": "± 7.82013",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.8092,
            "range": "± 4.97391",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 111.616,
            "range": "± 12.7419",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.13824,
            "range": "± 19.2808",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 167.224,
            "range": "± 14.5765",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.27698,
            "range": "± 34.8574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 848.991,
            "range": "± 51.3136",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49238,
            "range": "± 198.117",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.68141,
            "range": "± 628.05",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.92257,
            "range": "± 211.278",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.77909,
            "range": "± 790.166",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.29032,
            "range": "± 427.073",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.89037,
            "range": "± 442.816",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.2806,
            "range": "± 188.824",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.16919,
            "range": "± 254.301",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.67813,
            "range": "± 223.972",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.2231,
            "range": "± 2.2601",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 188.995,
            "range": "± 8.50915",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 32.338,
            "range": "± 8.33311",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 356.948,
            "range": "± 8.14336",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.59344,
            "range": "± 324.94",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.47983,
            "range": "± 209.014",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.4424,
            "range": "± 1.59491",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.21216,
            "range": "± 579.175",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.0076,
            "range": "± 6.5152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.5256,
            "range": "± 2.7752",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.2621,
            "range": "± 1.63156",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.20025,
            "range": "± 581.775",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.1944,
            "range": "± 3.03302",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.99846,
            "range": "± 1.55669",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.4863,
            "range": "± 1.84092",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.51169,
            "range": "± 732.898",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.452,
            "range": "± 3.64872",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.8708,
            "range": "± 2.15727",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.54696,
            "range": "± 1.28397",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.04532,
            "range": "± 1.45015",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 22.5337,
            "range": "± 9.84658",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.03758,
            "range": "± 1.49415",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.5092,
            "range": "± 1.68505",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.77923,
            "range": "± 1.13075",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.376,
            "range": "± 7.23956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.6138,
            "range": "± 7.9145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.75872,
            "range": "± 34.2939",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.77261,
            "range": "± 61.8732",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 137.148,
            "range": "± 11.9901",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.6572,
            "range": "± 8.54976",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.12009,
            "range": "± 43.8401",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.79822,
            "range": "± 48.7547",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 185.851,
            "range": "± 8.06073",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 457.533,
            "range": "± 42.3179",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.16795,
            "range": "± 80.0709",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 488.568,
            "range": "± 36.6751",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 899.294,
            "range": "± 38.7084",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 486.443,
            "range": "± 25.8008",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 575.229,
            "range": "± 28.4146",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 395.135,
            "range": "± 23.2437",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 507.614,
            "range": "± 71.353",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 468.094,
            "range": "± 38.9166",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.49147,
            "range": "± 0.420345",
            "unit": "ns",
            "extra": "100 samples\n8441 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.82247,
            "range": "± 169.055",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 718.66,
            "range": "± 23.6581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.4392,
            "range": "± 1.04007",
            "unit": "ns",
            "extra": "100 samples\n5465 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.0902,
            "range": "± 11.7235",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0639,
            "range": "± 86.1091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.478,
            "range": "± 4.975",
            "unit": "ns",
            "extra": "100 samples\n680 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 818.838,
            "range": "± 9.68548",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 175.969,
            "range": "± 1.49163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.45389,
            "range": "± 0.314597",
            "unit": "ns",
            "extra": "100 samples\n8449 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.3186,
            "range": "± 2.54057",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.5843,
            "range": "± 54.469",
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
          "id": "86fa2f249e776ca455b08461d4388836c23c3563",
          "message": "Add Module API for multi-function compilation with runtime swapping (#198)\n\nCo-authored-by: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-03-21T11:25:49+01:00",
          "tree_id": "3b0dc5806839618e0cf9ca88fd9209caf0b9ffc8",
          "url": "https://github.com/nebulastream/nautilus/commit/86fa2f249e776ca455b08461d4388836c23c3563"
        },
        "date": 1774089100480,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 190.299,
            "range": "± 15.6887",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 467.823,
            "range": "± 39.5337",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.19875,
            "range": "± 158.138",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 490.73,
            "range": "± 66.2358",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 896.543,
            "range": "± 56.5753",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 506.748,
            "range": "± 59.0119",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 591.094,
            "range": "± 38.8244",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 421.549,
            "range": "± 23.3224",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 511.506,
            "range": "± 41.8526",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 456.564,
            "range": "± 30.0115",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.27495,
            "range": "± 241.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.74862,
            "range": "± 88.7271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.71727,
            "range": "± 124.741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.73253,
            "range": "± 78.7351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.7928,
            "range": "± 502.152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3516,
            "range": "± 543.435",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.0386,
            "range": "± 561.4",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.62411,
            "range": "± 56.2265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0795,
            "range": "± 146.91",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.216,
            "range": "± 327.426",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9513,
            "range": "± 214.156",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.9529,
            "range": "± 140.642",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.1126,
            "range": "± 112.892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.8296,
            "range": "± 983.906",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.9127,
            "range": "± 270.703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.478,
            "range": "± 262.754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.7785,
            "range": "± 611.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.0148,
            "range": "± 2.25",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.6032,
            "range": "± 313.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.9727,
            "range": "± 366.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.1427,
            "range": "± 406.752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.2082,
            "range": "± 263.156",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.1357,
            "range": "± 610.537",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.4006,
            "range": "± 266.857",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.8561,
            "range": "± 752.549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.1928,
            "range": "± 686.075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.3696,
            "range": "± 699.324",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.1646,
            "range": "± 817.522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.0585,
            "range": "± 2.85616",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.9045,
            "range": "± 2.47744",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.8153,
            "range": "± 3.40891",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.9957,
            "range": "± 2.26787",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.5197,
            "range": "± 2.19243",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.0794,
            "range": "± 2.63782",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.8135,
            "range": "± 2.2602",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.2299,
            "range": "± 2.26971",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.754,
            "range": "± 3.25557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.2517,
            "range": "± 3.02906",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 37.4039,
            "range": "± 4.94884",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 188.344,
            "range": "± 9.79084",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 51.8869,
            "range": "± 6.46294",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 292.816,
            "range": "± 12.973",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.0768,
            "range": "± 3.85895",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 35.7355,
            "range": "± 3.59351",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 59.9588,
            "range": "± 6.27392",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 42.5462,
            "range": "± 4.71018",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.8552,
            "range": "± 7.05078",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.3702,
            "range": "± 4.86834",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.9565,
            "range": "± 6.0766",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 30.0006,
            "range": "± 4.00476",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 45.1579,
            "range": "± 5.53229",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 36.5159,
            "range": "± 3.70506",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 110.784,
            "range": "± 8.89218",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.14785,
            "range": "± 14.8061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 164.787,
            "range": "± 15.3506",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.26011,
            "range": "± 31.9103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.50554,
            "range": "± 0.476821",
            "unit": "ns",
            "extra": "100 samples\n8262 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.867,
            "range": "± 436.711",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 665.116,
            "range": "± 20.7589",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.45754,
            "range": "± 0.757293",
            "unit": "ns",
            "extra": "100 samples\n5493 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.1108,
            "range": "± 5.62496",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0496,
            "range": "± 90.1749",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.6674,
            "range": "± 5.20376",
            "unit": "ns",
            "extra": "100 samples\n715 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 931.726,
            "range": "± 9.14572",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.573,
            "range": "± 581.215",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.60231,
            "range": "± 0.529903",
            "unit": "ns",
            "extra": "100 samples\n8430 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.2567,
            "range": "± 1.92818",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.56949,
            "range": "± 38.5222",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 868.773,
            "range": "± 71.7201",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.57486,
            "range": "± 190.294",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.89713,
            "range": "± 1.18463",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.10915,
            "range": "± 338.401",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.75696,
            "range": "± 517.173",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.2094,
            "range": "± 263.764",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.83795,
            "range": "± 323.519",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.31838,
            "range": "± 229.823",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.17213,
            "range": "± 251.287",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.65832,
            "range": "± 195.897",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.6982,
            "range": "± 1.7657",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 186.162,
            "range": "± 6.16518",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.4695,
            "range": "± 1.71065",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 354.339,
            "range": "± 12.598",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.51992,
            "range": "± 219.848",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.57121,
            "range": "± 297.198",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.6002,
            "range": "± 1.00357",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.48294,
            "range": "± 640.216",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.8409,
            "range": "± 3.26731",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.4259,
            "range": "± 1.7976",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3261,
            "range": "± 983.682",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.35213,
            "range": "± 595.798",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.2125,
            "range": "± 1.61345",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 9.79458,
            "range": "± 863.217",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.4806,
            "range": "± 1.209",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.61289,
            "range": "± 667.623",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.2172,
            "range": "± 1.59991",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.6563,
            "range": "± 1.3634",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.47026,
            "range": "± 1.09275",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.66013,
            "range": "± 869.198",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.1401,
            "range": "± 1.32679",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.74476,
            "range": "± 668.941",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.47,
            "range": "± 1.08045",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.43638,
            "range": "± 309.71",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.7749,
            "range": "± 4.55197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.3869,
            "range": "± 5.9038",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.72542,
            "range": "± 54.2105",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.73507,
            "range": "± 46.3059",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.451,
            "range": "± 6.10858",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 68.8119,
            "range": "± 5.2145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.05474,
            "range": "± 36.4585",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.73675,
            "range": "± 50.479",
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
          "id": "4e75d8a2c1027332cb09fb4a0e13665835e40200",
          "message": "Apply clang-format fixes to existing files (#202)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-03-21T13:31:32+01:00",
          "tree_id": "3255222cd13bc38c99a9d5267cb3015df6922fb6",
          "url": "https://github.com/nebulastream/nautilus/commit/4e75d8a2c1027332cb09fb4a0e13665835e40200"
        },
        "date": 1774096654430,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 814.741,
            "range": "± 52.4093",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.43127,
            "range": "± 214.194",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.29198,
            "range": "± 701.223",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.78653,
            "range": "± 265.315",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.16837,
            "range": "± 420.923",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.08799,
            "range": "± 324.357",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.53925,
            "range": "± 490.057",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.12539,
            "range": "± 240.442",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.95006,
            "range": "± 223.392",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.47549,
            "range": "± 212.172",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.5992,
            "range": "± 4.96464",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 193.68,
            "range": "± 7.00179",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.5415,
            "range": "± 2.04488",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 336.733,
            "range": "± 22.0554",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.49248,
            "range": "± 0.473817",
            "unit": "ns",
            "extra": "100 samples\n8462 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.91326,
            "range": "± 665.337",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 717.271,
            "range": "± 22.0701",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.55185,
            "range": "± 0.980428",
            "unit": "ns",
            "extra": "100 samples\n5452 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.9167,
            "range": "± 8.43525",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1152,
            "range": "± 110.552",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.2095,
            "range": "± 5.26732",
            "unit": "ns",
            "extra": "100 samples\n691 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 816.631,
            "range": "± 7.88194",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 175.75,
            "range": "± 318.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.49429,
            "range": "± 0.309096",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.1654,
            "range": "± 1.47911",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.56965,
            "range": "± 23.1083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.72599,
            "range": "± 247.835",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.61594,
            "range": "± 226.335",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.9763,
            "range": "± 1.72966",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.49132,
            "range": "± 597.697",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.6281,
            "range": "± 6.3169",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.5137,
            "range": "± 2.74928",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.66,
            "range": "± 2.53365",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.41059,
            "range": "± 627.165",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.7191,
            "range": "± 3.06365",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.4802,
            "range": "± 1.59482",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8602,
            "range": "± 1.90247",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.73878,
            "range": "± 868.249",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.1139,
            "range": "± 3.42041",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.736,
            "range": "± 1.69617",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.7857,
            "range": "± 1.20657",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.41317,
            "range": "± 1.24189",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.1822,
            "range": "± 1.81997",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.22804,
            "range": "± 961.863",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.9273,
            "range": "± 1.66294",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.78967,
            "range": "± 491.848",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.4646,
            "range": "± 7.99937",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 57.8601,
            "range": "± 8.3438",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.79726,
            "range": "± 61.9917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.80898,
            "range": "± 45.5872",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 141.66,
            "range": "± 13.6071",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 73.6501,
            "range": "± 9.60727",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.16247,
            "range": "± 63.1311",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.90328,
            "range": "± 34.2901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.39456,
            "range": "± 208.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.0151,
            "range": "± 257.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.92168,
            "range": "± 163.298",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.4223,
            "range": "± 227.117",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.7174,
            "range": "± 358.352",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.913,
            "range": "± 292.638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.2073,
            "range": "± 535.357",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.8341,
            "range": "± 216.945",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.6842,
            "range": "± 627.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3206,
            "range": "± 214.907",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.3326,
            "range": "± 203.411",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.7085,
            "range": "± 277.692",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5086,
            "range": "± 269.788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.536,
            "range": "± 332.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.4695,
            "range": "± 331.354",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0743,
            "range": "± 461.987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.3356,
            "range": "± 423.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.9659,
            "range": "± 359.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.1458,
            "range": "± 433.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.4026,
            "range": "± 329.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.5924,
            "range": "± 288.205",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.815,
            "range": "± 342.185",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.4275,
            "range": "± 459.314",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.2104,
            "range": "± 590.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.7562,
            "range": "± 852.91",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.448,
            "range": "± 696.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.7137,
            "range": "± 371.713",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.6154,
            "range": "± 964.616",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 16.2446,
            "range": "± 3.12448",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 19.4915,
            "range": "± 3.66468",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.7356,
            "range": "± 3.4452",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 19.5095,
            "range": "± 3.40358",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 22.1047,
            "range": "± 3.40194",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 20.0899,
            "range": "± 3.74421",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 21.4201,
            "range": "± 4.11818",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 18.179,
            "range": "± 3.21344",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.7293,
            "range": "± 3.25418",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 19.1811,
            "range": "± 3.18496",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 42.8307,
            "range": "± 7.02611",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 181.585,
            "range": "± 9.27412",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.9307,
            "range": "± 5.72057",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 297.62,
            "range": "± 16.1718",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 23.0879,
            "range": "± 5.0177",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 35.7066,
            "range": "± 5.0794",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.2537,
            "range": "± 9.63402",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 42.512,
            "range": "± 8.30006",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.4068,
            "range": "± 8.5843",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.1188,
            "range": "± 8.86078",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.8593,
            "range": "± 7.90745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.8684,
            "range": "± 5.69338",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 46.64,
            "range": "± 10.1817",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 47.1859,
            "range": "± 12.6739",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 111.818,
            "range": "± 12.4972",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.17548,
            "range": "± 21.6346",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 168.242,
            "range": "± 14.4934",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.36332,
            "range": "± 48.7711",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 186.002,
            "range": "± 12.2779",
            "unit": "ns",
            "extra": "100 samples\n160 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 468.272,
            "range": "± 44.698",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.1808,
            "range": "± 101.042",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 486.096,
            "range": "± 39.61",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 903.147,
            "range": "± 49.8407",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 504.157,
            "range": "± 58.1886",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 589.867,
            "range": "± 48.1898",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 403.156,
            "range": "± 27.7286",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 505.467,
            "range": "± 33.8405",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 460.281,
            "range": "± 36.5977",
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
          "id": "f51ebe37f9244065a9da5457874582bbfaffd7d3",
          "message": "Add tracing internals documentation (#203)",
          "timestamp": "2026-03-21T17:56:09+01:00",
          "tree_id": "e3f98bccbcd4654c0c56bb598af1882fded57bd2",
          "url": "https://github.com/nebulastream/nautilus/commit/f51ebe37f9244065a9da5457874582bbfaffd7d3"
        },
        "date": 1774112406756,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.7298,
            "range": "± 186.806",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.52224,
            "range": "± 554.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.25527,
            "range": "± 419.822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.6419,
            "range": "± 251.073",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.1487,
            "range": "± 263.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.5211,
            "range": "± 268.299",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.2398,
            "range": "± 260.64",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.10823,
            "range": "± 156.814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.2322,
            "range": "± 271.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.121,
            "range": "± 269.554",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.0932,
            "range": "± 268.431",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.7419,
            "range": "± 453.804",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.9803,
            "range": "± 243.113",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.4755,
            "range": "± 215.14",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.4399,
            "range": "± 0.256188",
            "unit": "ns",
            "extra": "100 samples\n8491 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.85884,
            "range": "± 505.209",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 757.372,
            "range": "± 21.5287",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.42716,
            "range": "± 0.643648",
            "unit": "ns",
            "extra": "100 samples\n5491 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.9613,
            "range": "± 7.72533",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1061,
            "range": "± 88.2329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 46.6504,
            "range": "± 7.90573",
            "unit": "ns",
            "extra": "100 samples\n689 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 818.424,
            "range": "± 13.2887",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.076,
            "range": "± 1.29583",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.59536,
            "range": "± 0.444299",
            "unit": "ns",
            "extra": "100 samples\n8495 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.522,
            "range": "± 2.09311",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.58218,
            "range": "± 49.4414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 941.34,
            "range": "± 196.289",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.68536,
            "range": "± 395.882",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.97561,
            "range": "± 1.01891",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.12539,
            "range": "± 474.086",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.02447,
            "range": "± 888.076",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.36947,
            "range": "± 519.547",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.07891,
            "range": "± 562.593",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.51948,
            "range": "± 368.522",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.47394,
            "range": "± 610.161",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.81634,
            "range": "± 432.104",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.9811,
            "range": "± 2.25068",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 193.594,
            "range": "± 9.55543",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.9914,
            "range": "± 2.92488",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 364.92,
            "range": "± 10.804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 209.202,
            "range": "± 31.4313",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 496.015,
            "range": "± 62.277",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.24287,
            "range": "± 163.114",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 517.327,
            "range": "± 70.5396",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 981.206,
            "range": "± 134.06",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 528.178,
            "range": "± 64.8561",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 617.838,
            "range": "± 71.4512",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 434.379,
            "range": "± 73.0346",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 556.56,
            "range": "± 93.6299",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 493.534,
            "range": "± 57.2873",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "trace_add",
            "value": 3.16624,
            "range": "± 902.417",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.73966,
            "range": "± 452.086",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.0375,
            "range": "± 2.09207",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.70137,
            "range": "± 1.00096",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.9404,
            "range": "± 7.64993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 17.1665,
            "range": "± 3.58801",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.9224,
            "range": "± 1.95407",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.64838,
            "range": "± 876.29",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.4585,
            "range": "± 3.68738",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.8813,
            "range": "± 2.06765",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.0761,
            "range": "± 2.07061",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.74402,
            "range": "± 967.082",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.4038,
            "range": "± 3.58652",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.2491,
            "range": "± 2.2413",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0716,
            "range": "± 1.68548",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.63622,
            "range": "± 1.71665",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.5531,
            "range": "± 2.25598",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.90938,
            "range": "± 2.93657",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.3036,
            "range": "± 2.07997",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 5.00729,
            "range": "± 882.318",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 59.154,
            "range": "± 10.3985",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 59.6604,
            "range": "± 11.6553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.82749,
            "range": "± 211.093",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.83584,
            "range": "± 184.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 141.329,
            "range": "± 14.1364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 75.6039,
            "range": "± 11.9562",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.20817,
            "range": "± 151.216",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.96615,
            "range": "± 113.982",
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
          "id": "ec8a40b9bc528466ed7e38f8b59640753cd76db0",
          "message": "Fix ALLOCA hoisting to preserve operationIndex validity in SSA creation (#210)\n\nCo-authored-by: Claude <noreply@anthropic.com>\nCo-authored-by: Lukas Schwerdtfeger <lukas.schwerdtfeger@gmail.com>",
          "timestamp": "2026-03-28T12:23:49+01:00",
          "tree_id": "42e06a03368c2c2e677267bc67de11aa1c3190c7",
          "url": "https://github.com/nebulastream/nautilus/commit/ec8a40b9bc528466ed7e38f8b59640753cd76db0"
        },
        "date": 1774697395430,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.48595,
            "range": "± 0.416346",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89617,
            "range": "± 614.722",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 566.436,
            "range": "± 16.8011",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.51809,
            "range": "± 1.00481",
            "unit": "ns",
            "extra": "100 samples\n5472 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.844,
            "range": "± 9.34209",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.016,
            "range": "± 337.784",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.6237,
            "range": "± 4.95805",
            "unit": "ns",
            "extra": "100 samples\n688 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 814.936,
            "range": "± 9.79136",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 175.942,
            "range": "± 588.757",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.5817,
            "range": "± 0.438904",
            "unit": "ns",
            "extra": "100 samples\n8478 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.1275,
            "range": "± 1.56511",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.43347,
            "range": "± 21.7991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.65002,
            "range": "± 327.803",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.41624,
            "range": "± 401.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.41737,
            "range": "± 280.405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.6921,
            "range": "± 321.145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.2356,
            "range": "± 532.424",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.6024,
            "range": "± 599.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.7026,
            "range": "± 372.693",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.0042,
            "range": "± 342.85",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.9773,
            "range": "± 577.571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.6227,
            "range": "± 308.136",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.3335,
            "range": "± 860.118",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.1207,
            "range": "± 482.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5759,
            "range": "± 298.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.8457,
            "range": "± 467.164",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.6125,
            "range": "± 1.08133",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.7927,
            "range": "± 895.623",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.6511,
            "range": "± 2.49322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.8703,
            "range": "± 779.48",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.3976,
            "range": "± 734.215",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.7387,
            "range": "± 898.454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.3903,
            "range": "± 582.368",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.5109,
            "range": "± 608.64",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6221,
            "range": "± 831.207",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.8483,
            "range": "± 666.84",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.8117,
            "range": "± 607.63",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.4875,
            "range": "± 427.961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.4937,
            "range": "± 678.617",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.3473,
            "range": "± 920.223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.1842,
            "range": "± 2.22942",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.2737,
            "range": "± 5.02597",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.8314,
            "range": "± 4.00586",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 17.7547,
            "range": "± 3.42031",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.4511,
            "range": "± 3.53678",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.5457,
            "range": "± 3.20076",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.3572,
            "range": "± 2.95196",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.8245,
            "range": "± 3.39514",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 17.9977,
            "range": "± 3.15292",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 17.4989,
            "range": "± 3.22107",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 34.8571,
            "range": "± 4.35434",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 178.981,
            "range": "± 10.6712",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.9855,
            "range": "± 7.67972",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 289.304,
            "range": "± 14.2063",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.243,
            "range": "± 4.58737",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.174,
            "range": "± 5.49457",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.3885,
            "range": "± 11.091",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 36.2915,
            "range": "± 6.03022",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 57.6561,
            "range": "± 9.44949",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.4471,
            "range": "± 9.96513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.6075,
            "range": "± 9.77699",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 27.5826,
            "range": "± 4.80955",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 44.7428,
            "range": "± 9.36716",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.3258,
            "range": "± 5.7481",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 112.347,
            "range": "± 12.838",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.16997,
            "range": "± 21.5358",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 168.76,
            "range": "± 16.9299",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.30704,
            "range": "± 35.9652",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.62352,
            "range": "± 370.589",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.56335,
            "range": "± 326.42",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.6875,
            "range": "± 1.83314",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.51196,
            "range": "± 988.075",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.0751,
            "range": "± 6.84466",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 16.1303,
            "range": "± 2.98046",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3572,
            "range": "± 1.79682",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.33728,
            "range": "± 745.303",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.6546,
            "range": "± 3.33939",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.0895,
            "range": "± 1.86237",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.4869,
            "range": "± 2.15969",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.45157,
            "range": "± 684",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.7608,
            "range": "± 3.51934",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.4478,
            "range": "± 1.77409",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.3927,
            "range": "± 1.20048",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.04481,
            "range": "± 1.22155",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.0315,
            "range": "± 2.06741",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.86927,
            "range": "± 860.719",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.4762,
            "range": "± 1.61705",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 7.61749,
            "range": "± 2.15276",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.9963,
            "range": "± 8.23206",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 54.8448,
            "range": "± 8.29818",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.76509,
            "range": "± 63.0661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.75428,
            "range": "± 61.3762",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 137.601,
            "range": "± 11.4842",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 70.0332,
            "range": "± 9.06289",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.1037,
            "range": "± 52.0471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.76895,
            "range": "± 45.5175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 200.715,
            "range": "± 25.7248",
            "unit": "ns",
            "extra": "100 samples\n157 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 492.155,
            "range": "± 59.0328",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.19821,
            "range": "± 103.421",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 505.044,
            "range": "± 36.8866",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 942.176,
            "range": "± 79.9549",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 506.128,
            "range": "± 39.1388",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 618.503,
            "range": "± 57.5937",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 504.649,
            "range": "± 31.9501",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 541.084,
            "range": "± 74.3719",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 475.903,
            "range": "± 51.2476",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ir_add",
            "value": 866.177,
            "range": "± 74.7873",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.60437,
            "range": "± 365.281",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.75749,
            "range": "± 746.722",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.25743,
            "range": "± 672.259",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.79207,
            "range": "± 690.634",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.24582,
            "range": "± 266.104",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.86129,
            "range": "± 382.385",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.31342,
            "range": "± 278.144",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.20926,
            "range": "± 308.684",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.65514,
            "range": "± 227.062",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.8109,
            "range": "± 1.52462",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 191.116,
            "range": "± 7.05558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.3668,
            "range": "± 2.36589",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 367.371,
            "range": "± 12.2071",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "me@alanli.org",
            "name": "Alan Li",
            "username": "lialan"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "878df1ded70d35120c78f14554768d8a0b56c02b",
          "message": "Add pointer↔integer/float cast support\n\nEnable casts between pointer types and integer/float types across\nall compilation backends in the Nautilus JIT compiler.\n\nBackend changes:\n- BC: add integer↔ptr and float↔ptr cast paths treating ptr as ui64\n- MLIR: add IntToPtrOp/PtrToIntOp for integer↔ptr casts with\n  sign-aware extension for narrow integers, add ptr↔float via\n  intermediate i64 using UIToFPOp/FPToUIOp\n- C++: route all ptr↔arithmetic casts through (uintptr_t) intermediate\n  to avoid illegal C-style casts like (int8_t)ptr or (float)ptr\n\nval<T> API changes:\n- Add ptr→arithmetic conversion operator to base_ptr_val and both\n  derived ptr specializations (duplicated to avoid name-hiding)\n- Add integral→ptr conversion operator to val_arith\n\nCo-Authored-By: Claude Opus 4.6 (1M context) <noreply@anthropic.com>",
          "timestamp": "2026-03-28T18:52:10+01:00",
          "tree_id": "f01d2ee72e12fbf2b1bc05a064e9d815de5f65b6",
          "url": "https://github.com/nebulastream/nautilus/commit/878df1ded70d35120c78f14554768d8a0b56c02b"
        },
        "date": 1774720661275,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 834.543,
            "range": "± 103.66",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.399,
            "range": "± 233.523",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.22705,
            "range": "± 606.507",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.81539,
            "range": "± 325.126",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.28773,
            "range": "± 535.562",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.06283,
            "range": "± 326.594",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.64112,
            "range": "± 484.649",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.17117,
            "range": "± 247.815",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.92866,
            "range": "± 295.886",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.48505,
            "range": "± 269.501",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.6177,
            "range": "± 1.99285",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 198.924,
            "range": "± 8.78633",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.3197,
            "range": "± 2.09233",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 342.088,
            "range": "± 11.3005",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.33266,
            "range": "± 253.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.01648,
            "range": "± 256.814",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.84715,
            "range": "± 185.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.1957,
            "range": "± 273.297",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.5104,
            "range": "± 397.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.9509,
            "range": "± 273.763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.3818,
            "range": "± 318.028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.76835,
            "range": "± 187.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.6313,
            "range": "± 276.253",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5071,
            "range": "± 294.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.5701,
            "range": "± 320.621",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.6006,
            "range": "± 298.736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.9316,
            "range": "± 373.879",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.1907,
            "range": "± 368.344",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.1906,
            "range": "± 378.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.8693,
            "range": "± 486.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.8497,
            "range": "± 326.707",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.3703,
            "range": "± 2.29544",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.2431,
            "range": "± 486.739",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.3291,
            "range": "± 307.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.5202,
            "range": "± 378.285",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.6622,
            "range": "± 632.126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.154,
            "range": "± 430.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.9545,
            "range": "± 428.634",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.0518,
            "range": "± 356.024",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.8823,
            "range": "± 518.175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.3018,
            "range": "± 451.98",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.0015,
            "range": "± 601.202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.5824,
            "range": "± 2.35508",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.7373,
            "range": "± 4.09431",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 23.3243,
            "range": "± 3.91246",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.9767,
            "range": "± 3.26179",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 21.7828,
            "range": "± 4.24229",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.933,
            "range": "± 2.84989",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 20.3968,
            "range": "± 3.57496",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.4094,
            "range": "± 3.21559",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 19.3523,
            "range": "± 3.63311",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.3158,
            "range": "± 3.81951",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 37.6028,
            "range": "± 6.05996",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 190.502,
            "range": "± 13.3996",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.5941,
            "range": "± 7.35131",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 302.938,
            "range": "± 18.5622",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 20.7937,
            "range": "± 4.16168",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.1716,
            "range": "± 5.43529",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 61.4362,
            "range": "± 13.0253",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 36.8332,
            "range": "± 6.73084",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 60.2916,
            "range": "± 13.2799",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.2389,
            "range": "± 10.9073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 49.9506,
            "range": "± 9.5502",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.5747,
            "range": "± 5.99797",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 46.3654,
            "range": "± 10.138",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 37.3391,
            "range": "± 6.80373",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 113.978,
            "range": "± 17.2014",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.16427,
            "range": "± 30.6845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 165.983,
            "range": "± 18.9993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.3149,
            "range": "± 48.985",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 209.716,
            "range": "± 37.7386",
            "unit": "ns",
            "extra": "100 samples\n159 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 517.319,
            "range": "± 98.3004",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.2658,
            "range": "± 183.177",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 554.007,
            "range": "± 118.218",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 974.258,
            "range": "± 131.638",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 545.844,
            "range": "± 85.0256",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 654.707,
            "range": "± 109.102",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 525.569,
            "range": "± 81.8989",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 571.235,
            "range": "± 103.099",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 487.833,
            "range": "± 68.206",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.8001,
            "range": "± 630.918",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.62679,
            "range": "± 391.441",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.3865,
            "range": "± 2.46961",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.64013,
            "range": "± 870.453",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.872,
            "range": "± 7.36294",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 17.3994,
            "range": "± 4.16617",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3067,
            "range": "± 1.77894",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.56532,
            "range": "± 980.917",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.5652,
            "range": "± 4.41897",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.6475,
            "range": "± 2.28288",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.864,
            "range": "± 2.01728",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.79805,
            "range": "± 991.476",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.3008,
            "range": "± 3.99661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 12.4311,
            "range": "± 2.84997",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.93229,
            "range": "± 1.87811",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.4744,
            "range": "± 1.61375",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.8503,
            "range": "± 2.73978",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 7.33619,
            "range": "± 1.30825",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.0084,
            "range": "± 1.76717",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.84225,
            "range": "± 872.061",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 59.6863,
            "range": "± 11.5981",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 57.4599,
            "range": "± 10.434",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.82388,
            "range": "± 64.0264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.7745,
            "range": "± 67.7731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 139.16,
            "range": "± 14.5141",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 72.5604,
            "range": "± 11.3905",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.16827,
            "range": "± 79.6974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.80617,
            "range": "± 86.1825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.63387,
            "range": "± 0.767105",
            "unit": "ns",
            "extra": "100 samples\n8484 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.92945,
            "range": "± 689.484",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 597.232,
            "range": "± 33.8834",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.63975,
            "range": "± 1.24565",
            "unit": "ns",
            "extra": "100 samples\n5446 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.4524,
            "range": "± 8.51141",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0405,
            "range": "± 269.585",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.8924,
            "range": "± 5.96976",
            "unit": "ns",
            "extra": "100 samples\n683 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 813.888,
            "range": "± 9.28375",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 174.815,
            "range": "± 1.23546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.6319,
            "range": "± 0.523222",
            "unit": "ns",
            "extra": "100 samples\n8514 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.7043,
            "range": "± 2.62555",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.46272,
            "range": "± 32.9916",
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
          "id": "98f10878663268763879d5d3c2a5901c349342bd",
          "message": "Add val_base virtual interface as common base for all val<T> types (#206)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-03-28T19:41:06+01:00",
          "tree_id": "c5ec9e1a487a0588a96110d078039fcf82687948",
          "url": "https://github.com/nebulastream/nautilus/commit/98f10878663268763879d5d3c2a5901c349342bd"
        },
        "date": 1774723593974,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 9.28974,
            "range": "± 242.01",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.80745,
            "range": "± 320.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.65768,
            "range": "± 251.591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.0709,
            "range": "± 217.973",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.8795,
            "range": "± 547.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.1155,
            "range": "± 230.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.5207,
            "range": "± 237.453",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.48217,
            "range": "± 202.44",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.7296,
            "range": "± 173.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.7062,
            "range": "± 182.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.5255,
            "range": "± 266.669",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.8102,
            "range": "± 187.559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.5931,
            "range": "± 234.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 24.526,
            "range": "± 151.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.5524,
            "range": "± 567.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0998,
            "range": "± 403.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.5028,
            "range": "± 397.703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.3379,
            "range": "± 596.186",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.2214,
            "range": "± 332.477",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.2418,
            "range": "± 1.42649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.7423,
            "range": "± 563.349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.0265,
            "range": "± 276.017",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.8291,
            "range": "± 393.509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.096,
            "range": "± 332.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 31.3309,
            "range": "± 461.335",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.6046,
            "range": "± 450.161",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.3509,
            "range": "± 476.07",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.8428,
            "range": "± 977.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.4255,
            "range": "± 2.69696",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.4573,
            "range": "± 4.48489",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 22.9502,
            "range": "± 3.71522",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.2137,
            "range": "± 3.58647",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8408,
            "range": "± 3.70448",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 18.7349,
            "range": "± 3.07764",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.9285,
            "range": "± 4.20932",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 17.1467,
            "range": "± 3.83331",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.458,
            "range": "± 3.76242",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.5168,
            "range": "± 4.08101",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.8135,
            "range": "± 5.25916",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 182.228,
            "range": "± 12.1637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 50.5837,
            "range": "± 7.28559",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 292.032,
            "range": "± 16.2967",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 22.2058,
            "range": "± 5.60713",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 34.9229,
            "range": "± 5.81883",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 60.7708,
            "range": "± 9.63904",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 36.79,
            "range": "± 6.21681",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 61.9986,
            "range": "± 11.5399",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 47.3789,
            "range": "± 8.44338",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 50.9276,
            "range": "± 9.8482",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 29.4923,
            "range": "± 5.20176",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 46.2084,
            "range": "± 9.4704",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 37.3213,
            "range": "± 8.00727",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 111.962,
            "range": "± 16.6924",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.1612,
            "range": "± 28.2525",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 168.064,
            "range": "± 18.5511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.44861,
            "range": "± 63.7873",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 858.515,
            "range": "± 75.3718",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49295,
            "range": "± 236.528",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.69027,
            "range": "± 705.451",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.02857,
            "range": "± 460.993",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.74266,
            "range": "± 561.568",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.11332,
            "range": "± 228.532",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.74791,
            "range": "± 310.09",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.27217,
            "range": "± 215.942",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.14027,
            "range": "± 263.795",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.63309,
            "range": "± 185.375",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.3985,
            "range": "± 1.4504",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 186.194,
            "range": "± 7.90244",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.887,
            "range": "± 2.58019",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 363.038,
            "range": "± 11.5965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.53867,
            "range": "± 283.514",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.5434,
            "range": "± 316.71",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.4377,
            "range": "± 1.60024",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.21401,
            "range": "± 618.542",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.6286,
            "range": "± 6.42367",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.2722,
            "range": "± 2.69722",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.271,
            "range": "± 1.62667",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.20556,
            "range": "± 635.487",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.8058,
            "range": "± 3.77091",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.1411,
            "range": "± 1.66165",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.5502,
            "range": "± 1.87969",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.34138,
            "range": "± 675.724",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.5247,
            "range": "± 3.60937",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.7795,
            "range": "± 2.20632",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.30811,
            "range": "± 1.16537",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 9.15653,
            "range": "± 1.28528",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.1898,
            "range": "± 2.03666",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.90489,
            "range": "± 968.21",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6178,
            "range": "± 1.64188",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.60527,
            "range": "± 624.479",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.3773,
            "range": "± 9.71283",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 54.5516,
            "range": "± 8.18775",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.73907,
            "range": "± 48.1712",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.77056,
            "range": "± 35.487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 136.683,
            "range": "± 12.2756",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 69.591,
            "range": "± 8.75641",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.10545,
            "range": "± 48.3901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.76399,
            "range": "± 57.8992",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.46789,
            "range": "± 0.365302",
            "unit": "ns",
            "extra": "100 samples\n8462 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.09784,
            "range": "± 1.14514",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 610.787,
            "range": "± 30.362",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.43373,
            "range": "± 0.821121",
            "unit": "ns",
            "extra": "100 samples\n5470 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.7219,
            "range": "± 11.8012",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.6417,
            "range": "± 1.67391",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.0013,
            "range": "± 11.8014",
            "unit": "ns",
            "extra": "100 samples\n667 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 946.368,
            "range": "± 72.505",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 198.26,
            "range": "± 6.95734",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.47284,
            "range": "± 0.294697",
            "unit": "ns",
            "extra": "100 samples\n8477 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.3384,
            "range": "± 2.37257",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.4479,
            "range": "± 35.9954",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 196.717,
            "range": "± 22.9474",
            "unit": "ns",
            "extra": "100 samples\n147 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 481.078,
            "range": "± 30.3255",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.22432,
            "range": "± 98.6583",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 510.445,
            "range": "± 39.6191",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 981.533,
            "range": "± 76.0699",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 517.198,
            "range": "± 34.5995",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 622.029,
            "range": "± 49.0606",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 491.693,
            "range": "± 43.3557",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 542.791,
            "range": "± 53.0014",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 470.209,
            "range": "± 36.2873",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
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
          "id": "a3f3e9c51cfadda2c2c57d80d270e4c99eb39ed8",
          "message": "Fix AddOp lowering in CPP backend using wrong operation type (#214)",
          "timestamp": "2026-04-01T07:16:43+02:00",
          "tree_id": "31cac4dba238a64629b8719bd286a5a8a28025d1",
          "url": "https://github.com/nebulastream/nautilus/commit/a3f3e9c51cfadda2c2c57d80d270e4c99eb39ed8"
        },
        "date": 1775020852964,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.47518,
            "range": "± 0.405246",
            "unit": "ns",
            "extra": "100 samples\n8445 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.04737,
            "range": "± 1.08062",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 578.145,
            "range": "± 64.0147",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.33205,
            "range": "± 0.543434",
            "unit": "ns",
            "extra": "100 samples\n5471 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.6188,
            "range": "± 9.36912",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9484,
            "range": "± 76.5862",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 44.2215,
            "range": "± 6.47502",
            "unit": "ns",
            "extra": "100 samples\n691 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 822.327,
            "range": "± 12.6536",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.028,
            "range": "± 659.013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_asmjit_add",
            "value": 3.60814,
            "range": "± 0.578975",
            "unit": "ns",
            "extra": "100 samples\n8460 iterations"
          },
          {
            "name": "exec_asmjit_fibonacci",
            "value": 21.6288,
            "range": "± 3.24613",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "exec_asmjit_sum",
            "value": 4.45697,
            "range": "± 45.4201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 854.33,
            "range": "± 81.6744",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.671,
            "range": "± 456.984",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.88012,
            "range": "± 823.252",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.04024,
            "range": "± 373.687",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.81858,
            "range": "± 746.136",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.2659,
            "range": "± 376.995",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.7749,
            "range": "± 569.99",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25725,
            "range": "± 158.869",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.17196,
            "range": "± 234.985",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.66647,
            "range": "± 302.248",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.1584,
            "range": "± 1.86538",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 193.558,
            "range": "± 8.17045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.8691,
            "range": "± 1.67191",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 370.333,
            "range": "± 13.0525",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.57955,
            "range": "± 268.144",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "completing_trace_add",
            "value": 2.54844,
            "range": "± 288.334",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.1076,
            "range": "± 2.12239",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_ifThenElse",
            "value": 5.30708,
            "range": "± 652.345",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.6173,
            "range": "± 7.37154",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_deeplyNestedIfElse",
            "value": 15.8437,
            "range": "± 3.45161",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.8298,
            "range": "± 1.86512",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loop",
            "value": 5.39947,
            "range": "± 682.77",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.4552,
            "range": "± 3.32729",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_ifInsideLoop",
            "value": 10.2411,
            "range": "± 1.82447",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9955,
            "range": "± 1.81973",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_loopDirectCall",
            "value": 5.34824,
            "range": "± 671.043",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.3703,
            "range": "± 3.33774",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "completing_trace_pointerLoop",
            "value": 11.493,
            "range": "± 1.73451",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.20622,
            "range": "± 1.0699",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "completing_trace_staticLoop",
            "value": 8.9735,
            "range": "± 1.10899",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 14.1719,
            "range": "± 2.66331",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_fibonacci",
            "value": 6.59787,
            "range": "± 812.253",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.0463,
            "range": "± 1.73258",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "completing_trace_gcd",
            "value": 4.65348,
            "range": "± 511.873",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.4514,
            "range": "± 8.08936",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf10",
            "value": 55.6325,
            "range": "± 7.83651",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.81343,
            "range": "± 205.134",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_nestedIf100",
            "value": 1.82224,
            "range": "± 39.2657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 141.567,
            "range": "± 11.4945",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf10",
            "value": 71.1739,
            "range": "± 10.1789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.13347,
            "range": "± 45.2127",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "completing_trace_chainedIf100",
            "value": 2.80508,
            "range": "± 38.7944",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 194.671,
            "range": "± 16.1002",
            "unit": "ns",
            "extra": "100 samples\n156 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 481.598,
            "range": "± 33.58",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.25854,
            "range": "± 168.561",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 523.862,
            "range": "± 48.3283",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 967.22,
            "range": "± 92.4438",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 521.524,
            "range": "± 35.9294",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 645.002,
            "range": "± 54.8434",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 510.093,
            "range": "± 48.6925",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 547.878,
            "range": "± 46.9578",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 498.361,
            "range": "± 58.7311",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.81596,
            "range": "± 534.547",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.38068,
            "range": "± 535.218",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.13313,
            "range": "± 485.529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.0294,
            "range": "± 376.951",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.1179,
            "range": "± 321.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.1037,
            "range": "± 329.851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.5576,
            "range": "± 340.952",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.7594,
            "range": "± 256.495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4276,
            "range": "± 339.291",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.254,
            "range": "± 311.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.5831,
            "range": "± 902.405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.6773,
            "range": "± 317.1",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.6642,
            "range": "± 379.543",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.8916,
            "range": "± 324.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.8794,
            "range": "± 368.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5994,
            "range": "± 1.11786",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.1188,
            "range": "± 667.155",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.7929,
            "range": "± 361.011",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.855,
            "range": "± 437.402",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.3539,
            "range": "± 383.478",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.1963,
            "range": "± 330.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.1362,
            "range": "± 340.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.6565,
            "range": "± 383.856",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.3853,
            "range": "± 522.799",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.3989,
            "range": "± 349.145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.5396,
            "range": "± 487.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.1014,
            "range": "± 671.871",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.7786,
            "range": "± 1.07619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 14.781,
            "range": "± 2.37726",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 18.3066,
            "range": "± 3.31947",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 21.973,
            "range": "± 3.41507",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 18.2333,
            "range": "± 3.69041",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 20.8739,
            "range": "± 4.33151",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 19.2007,
            "range": "± 3.78413",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 19.8644,
            "range": "± 3.77911",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 16.7879,
            "range": "± 3.38173",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 18.3663,
            "range": "± 3.27136",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 18.0152,
            "range": "± 2.76342",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 35.8522,
            "range": "± 4.28134",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 181.097,
            "range": "± 11.8962",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 49.3702,
            "range": "± 7.50218",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 290.818,
            "range": "± 17.3544",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_add",
            "value": 21.188,
            "range": "± 4.87693",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifThenElse",
            "value": 33.5501,
            "range": "± 5.38552",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_deeplyNestedIfElse",
            "value": 57.7212,
            "range": "± 9.17995",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loop",
            "value": 35.6318,
            "range": "± 5.27654",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_ifInsideLoop",
            "value": 59.4218,
            "range": "± 11.9057",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_loopDirectCall",
            "value": 46.0328,
            "range": "± 9.68253",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_pointerLoop",
            "value": 48.8339,
            "range": "± 10.9407",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_staticLoop",
            "value": 28.3494,
            "range": "± 4.25178",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_fibonacci",
            "value": 43.6687,
            "range": "± 8.65062",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_gcd",
            "value": 35.1428,
            "range": "± 5.70744",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf10",
            "value": 109.174,
            "range": "± 12.5732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_nestedIf100",
            "value": 1.14775,
            "range": "± 40.6487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf10",
            "value": 165.37,
            "range": "± 18.1415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_asmjit_chainedIf100",
            "value": 2.26744,
            "range": "± 44.0847",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}