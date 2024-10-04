window.BENCHMARK_DATA = {
  "lastUpdate": 1728038103331,
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
          "id": "b79795fcd7db9bc0b24aec7a983a6b70aa70737d",
          "message": "refactor trace to ir conversion phase (#53)",
          "timestamp": "2024-10-04T12:32:39+02:00",
          "tree_id": "034bd16d66a7d9502ef63f7e6259240d72a2ddcc",
          "url": "https://github.com/nebulastream/nautilus/commit/b79795fcd7db9bc0b24aec7a983a6b70aa70737d"
        },
        "date": 1728038102942,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.39393,
            "range": "± 155.552",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 18.9145,
            "range": "± 2.9931",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 63.337,
            "range": "± 5.33135",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 19.8388,
            "range": "± 3.72866",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 41.2094,
            "range": "± 7.32046",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.9225,
            "range": "± 2.90333",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 29.0907,
            "range": "± 3.74062",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 18.2105,
            "range": "± 2.18056",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.6526,
            "range": "± 2.37992",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 18.0727,
            "range": "± 2.24783",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 111.373,
            "range": "± 15.8481",
            "unit": "ns",
            "extra": "100 samples\n286 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 382.145,
            "range": "± 26.5788",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01768,
            "range": "± 81.9232",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 415.664,
            "range": "± 37.6891",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 757.218,
            "range": "± 83.1367",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 447.054,
            "range": "± 51.6297",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 534.177,
            "range": "± 42.7055",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 429.306,
            "range": "± 45.2331",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 427.13,
            "range": "± 44.6641",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 365.032,
            "range": "± 22.4403",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 515.566,
            "range": "± 38.3276",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.33132,
            "range": "± 250.517",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.98928,
            "range": "± 577.84",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.71086,
            "range": "± 253.487",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.08492,
            "range": "± 540.231",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.98134,
            "range": "± 292.955",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.35116,
            "range": "± 200.247",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.07761,
            "range": "± 172.489",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.84935,
            "range": "± 225.247",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.40293,
            "range": "± 177.193",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.22584,
            "range": "± 111.196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.8227,
            "range": "± 166.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.70863,
            "range": "± 58.7406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.79664,
            "range": "± 75.1456",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.9255,
            "range": "± 337.316",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4733,
            "range": "± 175.685",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.206,
            "range": "± 284.399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.71224,
            "range": "± 154.363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8952,
            "range": "± 124.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3974,
            "range": "± 98.7355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.2696,
            "range": "± 492.495",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.7903,
            "range": "± 388.693",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.2012,
            "range": "± 506.402",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.8532,
            "range": "± 277.892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.1511,
            "range": "± 473.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.5349,
            "range": "± 746.151",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.6409,
            "range": "± 509.695",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.7185,
            "range": "± 398.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.0228,
            "range": "± 380.685",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.7903,
            "range": "± 482.869",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.06083,
            "range": "± 66.4313",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.44604,
            "range": "± 165.232",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.07322,
            "range": "± 512.1",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.53862,
            "range": "± 162.661",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.41125,
            "range": "± 314.391",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.68809,
            "range": "± 191.669",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.08294,
            "range": "± 293.509",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.16688,
            "range": "± 147.48",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.74295,
            "range": "± 270.504",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.44099,
            "range": "± 162.553",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          }
        ]
      }
    ]
  }
}