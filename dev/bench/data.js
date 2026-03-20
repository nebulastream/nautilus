window.BENCHMARK_DATA = {
  "lastUpdate": 1773983653337,
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
          "id": "c98febd5d5481b38a17dd6c3b845e171e1178cc4",
          "message": "Fix performance regressions in iterative SSA creation phase (#171)",
          "timestamp": "2026-03-01T22:05:21-08:00",
          "tree_id": "b6bd5639776fab299b1ecc950755373cabf0ea0f",
          "url": "https://github.com/nebulastream/nautilus/commit/c98febd5d5481b38a17dd6c3b845e171e1178cc4"
        },
        "date": 1772431758976,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 812.988,
            "range": "± 50.1091",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.42442,
            "range": "± 388.571",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.02252,
            "range": "± 432.512",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.92865,
            "range": "± 472.543",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.27331,
            "range": "± 669.443",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.02177,
            "range": "± 549.539",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.41447,
            "range": "± 293.755",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.11748,
            "range": "± 207.797",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.95107,
            "range": "± 289.572",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.41618,
            "range": "± 164.888",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.0078,
            "range": "± 1.0809",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 191.214,
            "range": "± 5.71036",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 26.684,
            "range": "± 2.37734",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 332.018,
            "range": "± 8.21021",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.25359,
            "range": "± 192.503",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.90092,
            "range": "± 231.425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.72841,
            "range": "± 168.277",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.90784,
            "range": "± 242.693",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.3911,
            "range": "± 721.822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7125,
            "range": "± 239.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.6902,
            "range": "± 408.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.66108,
            "range": "± 313.375",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.277,
            "range": "± 333.438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.7896,
            "range": "± 409.791",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.8197,
            "range": "± 485.662",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.4062,
            "range": "± 298.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.4971,
            "range": "± 399.075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.5911,
            "range": "± 533.365",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.4976,
            "range": "± 580.127",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.0288,
            "range": "± 661.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.7918,
            "range": "± 410.531",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.8856,
            "range": "± 445.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.9863,
            "range": "± 587.735",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3558,
            "range": "± 509.447",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.54,
            "range": "± 487.49",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.824,
            "range": "± 585.477",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.1859,
            "range": "± 492.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.7851,
            "range": "± 576.437",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.9499,
            "range": "± 444.933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.9484,
            "range": "± 504.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 34.6338,
            "range": "± 508.196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 95.7641,
            "range": "± 1.01467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.11897,
            "range": "± 66.1048",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.43767,
            "range": "± 309.807",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.7907,
            "range": "± 502.291",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.46002,
            "range": "± 171.387",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.4527,
            "range": "± 419.117",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.61719,
            "range": "± 188.599",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.16435,
            "range": "± 327.411",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.51015,
            "range": "± 197.868",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.66806,
            "range": "± 210.352",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.28982,
            "range": "± 172.532",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.9572,
            "range": "± 1.95257",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 177.224,
            "range": "± 7.13542",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.885,
            "range": "± 2.21072",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 298.683,
            "range": "± 8.99152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.46368,
            "range": "± 0.330247",
            "unit": "ns",
            "extra": "100 samples\n8422 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.03483,
            "range": "± 909.556",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 698.626,
            "range": "± 20.1666",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.39088,
            "range": "± 0.578634",
            "unit": "ns",
            "extra": "100 samples\n5469 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.7551,
            "range": "± 8.5313",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0533,
            "range": "± 84.4173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 38.925,
            "range": "± 2.59002",
            "unit": "ns",
            "extra": "100 samples\n754 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 916.598,
            "range": "± 4.74691",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.652,
            "range": "± 502.952",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 167.163,
            "range": "± 8.03866",
            "unit": "ns",
            "extra": "100 samples\n171 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 362.179,
            "range": "± 16.5819",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.887854,
            "range": "± 0.0508438",
            "unit": "us",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 376.802,
            "range": "± 20.2063",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 689.577,
            "range": "± 44.8102",
            "unit": "ns",
            "extra": "100 samples\n48 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 388.144,
            "range": "± 21.427",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 463.479,
            "range": "± 38.1613",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 333.63,
            "range": "± 20.5765",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 398.491,
            "range": "± 31.4187",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 356.558,
            "range": "± 35.9086",
            "unit": "ns",
            "extra": "100 samples\n89 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.4362,
            "range": "± 196.996",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.2756,
            "range": "± 1.83087",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.3106,
            "range": "± 6.82236",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1579,
            "range": "± 1.68778",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.6987,
            "range": "± 3.04744",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.5694,
            "range": "± 2.35661",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.2302,
            "range": "± 3.15936",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.01316,
            "range": "± 973.813",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6681,
            "range": "± 1.96649",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.2547,
            "range": "± 1.71416",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 54.629,
            "range": "± 6.30956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.72887,
            "range": "± 45.5256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.412,
            "range": "± 9.37546",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.05025,
            "range": "± 33.2066",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "45949765+keyseven123@users.noreply.github.com",
            "name": "Nils Schubert",
            "username": "keyseven123"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "b67fcf6f1f014ae9f78fb579cee3cc31e20191b0",
          "message": "Increase typed value ref (#165)\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2026-03-04T06:11:11-08:00",
          "tree_id": "fdbaf2392e73369827e4cb8e5abfa299d6c74a41",
          "url": "https://github.com/nebulastream/nautilus/commit/b67fcf6f1f014ae9f78fb579cee3cc31e20191b0"
        },
        "date": 1772633830728,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.21784,
            "range": "± 164.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.79908,
            "range": "± 152.715",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.72808,
            "range": "± 148.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.83308,
            "range": "± 166.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.3422,
            "range": "± 1.15006",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3574,
            "range": "± 206.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.5893,
            "range": "± 521.214",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.76999,
            "range": "± 342.137",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.5193,
            "range": "± 342.189",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.304,
            "range": "± 581.703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0991,
            "range": "± 326.901",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.7944,
            "range": "± 568.935",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.7871,
            "range": "± 272.752",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.1971,
            "range": "± 1.21747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.51966,
            "range": "± 0.476371",
            "unit": "ns",
            "extra": "100 samples\n8433 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.7844,
            "range": "± 550.247",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 690.771,
            "range": "± 20.4052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.49407,
            "range": "± 0.818516",
            "unit": "ns",
            "extra": "100 samples\n5460 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.3472,
            "range": "± 8.20196",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0698,
            "range": "± 172.817",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 40.5994,
            "range": "± 5.92871",
            "unit": "ns",
            "extra": "100 samples\n768 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 817.489,
            "range": "± 7.89704",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.571,
            "range": "± 1.4634",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 942.139,
            "range": "± 156.34",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.59381,
            "range": "± 383.009",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.84883,
            "range": "± 704.369",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.04769,
            "range": "± 282.905",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.99524,
            "range": "± 648.786",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.35371,
            "range": "± 436.094",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.09581,
            "range": "± 543.476",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.28579,
            "range": "± 232.328",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.23608,
            "range": "± 247.642",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.70474,
            "range": "± 275.378",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.2323,
            "range": "± 1.38049",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 189.132,
            "range": "± 8.10001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.3464,
            "range": "± 2.14614",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 358.107,
            "range": "± 12.4383",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.58467,
            "range": "± 255.295",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.959,
            "range": "± 1.60006",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 37.0158,
            "range": "± 5.97093",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.0073,
            "range": "± 1.81613",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.1728,
            "range": "± 3.3277",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.0446,
            "range": "± 2.01735",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.3611,
            "range": "± 3.98551",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.95156,
            "range": "± 1.82571",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.1798,
            "range": "± 2.06136",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.676,
            "range": "± 1.63062",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 58.8349,
            "range": "± 10.5802",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.87151,
            "range": "± 63.8545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 139.922,
            "range": "± 17.4846",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.20008,
            "range": "± 60.7214",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 186.105,
            "range": "± 25.0983",
            "unit": "ns",
            "extra": "100 samples\n170 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 445.776,
            "range": "± 48.5144",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.12585,
            "range": "± 125.546",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 480.191,
            "range": "± 56.4701",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 889.771,
            "range": "± 96.825",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 481.917,
            "range": "± 52.4504",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 569.951,
            "range": "± 66.6496",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 381.305,
            "range": "± 53.2688",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 479.007,
            "range": "± 33.9751",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 471.562,
            "range": "± 81.9768",
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
          "id": "d14ac8998742c100f6492daf5b30351d189f0f33",
          "message": "Improve traceUtil interface for pluggable tracing implementations\n\nIntroduces TracingInterface as an abstract base class decoupling the\nval<T> tracing layer (TracingUtil) from the concrete TraceContext\nimplementation. The active tracer is stored as a thread-local pointer so\nall operations within a single trace use the same implementation while\ndifferent traces can use different ones.\n\nChanges:\n- Add TracingInterface with virtual methods for all tracing operations\n- Replace TraceContext::get() / getIfActive() with setActiveTracer() /\n  getActiveTracer() thread-local dispatch in TracingUtil\n- Mark TraceContext as final to enable devirtualization of virtual calls\n- Unify argument names and ordering across TracingInterface, TracingUtil,\n  and TraceContext (op, resultType, left/right, input, first/second/third)\n- Remove unused traceValueDestruction (was a no-op throughout)\n- Guard log::trace and log::debug against eager fmt::format evaluation\n  when the log level is inactive, fixing a 44% instruction-count overhead\n  visible in callgrind profiles of the tracing hot path\n\nhttps://claude.ai/code/session_01UHRTMU9DCPXMzKMJGTkZkA",
          "timestamp": "2026-03-05T07:19:21-08:00",
          "tree_id": "f6badf77c5c96a47203a3150fcb7a5814e7c98a4",
          "url": "https://github.com/nebulastream/nautilus/commit/d14ac8998742c100f6492daf5b30351d189f0f33"
        },
        "date": 1772724279374,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.24171,
            "range": "± 347.448",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.36517,
            "range": "± 445.229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.92978,
            "range": "± 375.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.0418,
            "range": "± 337.335",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.1289,
            "range": "± 599.676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.6243,
            "range": "± 475.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.7167,
            "range": "± 466.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.80736,
            "range": "± 245.182",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.2094,
            "range": "± 485.98",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5614,
            "range": "± 315.269",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.6129,
            "range": "± 256.837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 28.7437,
            "range": "± 610.753",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.6644,
            "range": "± 405.098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.937,
            "range": "± 326.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.7479,
            "range": "± 461.529",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.7025,
            "range": "± 1.9246",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.1444,
            "range": "± 6.82683",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.5736,
            "range": "± 1.87164",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.3152,
            "range": "± 3.16947",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9562,
            "range": "± 2.16674",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.0493,
            "range": "± 4.16326",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.47194,
            "range": "± 1.69864",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.9301,
            "range": "± 1.94172",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6818,
            "range": "± 2.37806",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.9993,
            "range": "± 9.36132",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.72724,
            "range": "± 55.605",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.58,
            "range": "± 10.993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.09026,
            "range": "± 62.721",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.41993,
            "range": "± 0.0819448",
            "unit": "ns",
            "extra": "100 samples\n8448 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.10848,
            "range": "± 1.16112",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 714.309,
            "range": "± 32.0913",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.33374,
            "range": "± 0.29426",
            "unit": "ns",
            "extra": "100 samples\n5467 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.4599,
            "range": "± 5.93727",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.4288,
            "range": "± 673.149",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 45.2008,
            "range": "± 4.98271",
            "unit": "ns",
            "extra": "100 samples\n687 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 818.694,
            "range": "± 15.3099",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.423,
            "range": "± 617.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 184.305,
            "range": "± 21.9038",
            "unit": "ns",
            "extra": "100 samples\n168 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 436.157,
            "range": "± 35.0321",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.09631,
            "range": "± 114.564",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 476.471,
            "range": "± 58.1604",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 876.081,
            "range": "± 136.278",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 483.136,
            "range": "± 60.1311",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 564.935,
            "range": "± 68.6468",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 387.461,
            "range": "± 53.7298",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 486.781,
            "range": "± 49.5434",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 447.381,
            "range": "± 55.4444",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ir_add",
            "value": 904.312,
            "range": "± 104.676",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48768,
            "range": "± 275.562",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.75892,
            "range": "± 748.934",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 5.95964,
            "range": "± 648.48",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.66311,
            "range": "± 493.886",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.17879,
            "range": "± 261.658",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.9468,
            "range": "± 386.902",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.23151,
            "range": "± 201.934",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.20048,
            "range": "± 343.904",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.64125,
            "range": "± 298.938",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.2461,
            "range": "± 977.524",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 183.019,
            "range": "± 7.6663",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.2014,
            "range": "± 2.06826",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 349.295,
            "range": "± 10.4957",
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
          "id": "dcb58b623b97b1a064c166bfbc7b9d3304c93d04",
          "message": "Combine globalTagMap and localTagMap into a single tagMap\n\nBoth maps were always written together with identical key-value pairs in\naddTag() and processControlFlowMerge(), making localTagMap a strict\nsubset of globalTagMap at all times. The localTagMap check in checkTag()\nwas therefore dead code and the merge() in resetExecution() was a no-op.\n\nReplace both with a single tagMap and simplify the affected methods.\n\nhttps://claude.ai/code/session_012kUtxWe8fHh46p6fxiSZFd",
          "timestamp": "2026-03-07T02:15:34-08:00",
          "tree_id": "d12cfd913c268e7b9eb1f8c9a9670a13cafba49e",
          "url": "https://github.com/nebulastream/nautilus/commit/dcb58b623b97b1a064c166bfbc7b9d3304c93d04"
        },
        "date": 1772878777656,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.43469,
            "range": "± 0.266009",
            "unit": "ns",
            "extra": "100 samples\n8472 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.00165,
            "range": "± 976.046",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 674.663,
            "range": "± 25.7146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.49104,
            "range": "± 1.19288",
            "unit": "ns",
            "extra": "100 samples\n5462 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.563,
            "range": "± 15.2539",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1026,
            "range": "± 113.916",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 40.5644,
            "range": "± 5.62867",
            "unit": "ns",
            "extra": "100 samples\n776 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 814.112,
            "range": "± 4.67861",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.122,
            "range": "± 828.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.21117,
            "range": "± 206.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.86649,
            "range": "± 289.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.74225,
            "range": "± 190.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.91408,
            "range": "± 312.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.364,
            "range": "± 552.615",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4942,
            "range": "± 372.065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.3256,
            "range": "± 562.026",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.58163,
            "range": "± 155.35",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0992,
            "range": "± 278.356",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1069,
            "range": "± 348.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1467,
            "range": "± 295.28",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.3728,
            "range": "± 468.938",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.2635,
            "range": "± 302.246",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.7526,
            "range": "± 486.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.0936,
            "range": "± 777.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.4952,
            "range": "± 1.13492",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.8986,
            "range": "± 812.636",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.9285,
            "range": "± 837.419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.9886,
            "range": "± 898.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3289,
            "range": "± 832.797",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.3768,
            "range": "± 810.327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.0257,
            "range": "± 419.382",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.842,
            "range": "± 640.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.7651,
            "range": "± 691.223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.8407,
            "range": "± 787.925",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 65.3455,
            "range": "± 1.0747",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.2934,
            "range": "± 844.488",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.5544,
            "range": "± 860.073",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.18569,
            "range": "± 115.964",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.47615,
            "range": "± 258.92",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.87851,
            "range": "± 547.316",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.63683,
            "range": "± 348.275",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.64956,
            "range": "± 557.238",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.70377,
            "range": "± 256.215",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.33133,
            "range": "± 543.962",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.73953,
            "range": "± 313.683",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.83602,
            "range": "± 304.378",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36449,
            "range": "± 273.844",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.3113,
            "range": "± 2.47851",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 175.456,
            "range": "± 6.53073",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 27.7932,
            "range": "± 2.96362",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 287.336,
            "range": "± 13.8762",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 193.687,
            "range": "± 31.1693",
            "unit": "ns",
            "extra": "100 samples\n170 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 447.903,
            "range": "± 53.4565",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.12395,
            "range": "± 135.396",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 482.529,
            "range": "± 60.3545",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 869.962,
            "range": "± 95.9387",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 491.706,
            "range": "± 68.7179",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 557.622,
            "range": "± 61.4496",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 398.474,
            "range": "± 76.4803",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 491.757,
            "range": "± 57.6447",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 449.891,
            "range": "± 58.7431",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ir_add",
            "value": 919.377,
            "range": "± 118.185",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.587,
            "range": "± 366.84",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.98164,
            "range": "± 837.732",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.98305,
            "range": "± 335.899",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.90875,
            "range": "± 792.853",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.28967,
            "range": "± 373.428",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.91704,
            "range": "± 421.264",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.42358,
            "range": "± 416.692",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.15317,
            "range": "± 624.897",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.66408,
            "range": "± 305.666",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.7946,
            "range": "± 1.73983",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 188.921,
            "range": "± 9.91344",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.9421,
            "range": "± 2.05239",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 362.887,
            "range": "± 11.5659",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.56153,
            "range": "± 416.269",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.9187,
            "range": "± 1.34116",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.4397,
            "range": "± 6.06037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8262,
            "range": "± 1.57799",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.6121,
            "range": "± 3.73715",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.9813,
            "range": "± 1.67076",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.0053,
            "range": "± 3.49112",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 7.73812,
            "range": "± 1.10555",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6602,
            "range": "± 1.91938",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.1616,
            "range": "± 1.49364",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 52.4991,
            "range": "± 8.42429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.66961,
            "range": "± 57.219",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 128.84,
            "range": "± 9.61326",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.91618,
            "range": "± 70.2856",
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
          "id": "9163f32011ab6a90268b2f65d9f3d1f075ab785e",
          "message": "Revert \"Combine globalTagMap and localTagMap into a single tagMap\"\n\nThis reverts commit dcb58b623b97b1a064c166bfbc7b9d3304c93d04.",
          "timestamp": "2026-03-07T02:31:35-08:00",
          "tree_id": "f6badf77c5c96a47203a3150fcb7a5814e7c98a4",
          "url": "https://github.com/nebulastream/nautilus/commit/9163f32011ab6a90268b2f65d9f3d1f075ab785e"
        },
        "date": 1772879707244,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.43957,
            "range": "± 0.202861",
            "unit": "ns",
            "extra": "100 samples\n8464 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.34236,
            "range": "± 1.63228",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 658.759,
            "range": "± 29.9916",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.336,
            "range": "± 0.532962",
            "unit": "ns",
            "extra": "100 samples\n5474 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.8413,
            "range": "± 8.59458",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1026,
            "range": "± 96.6779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.7054,
            "range": "± 3.70488",
            "unit": "ns",
            "extra": "100 samples\n687 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 818.367,
            "range": "± 12.5456",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.825,
            "range": "± 3.93881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 856.707,
            "range": "± 41.9859",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.45636,
            "range": "± 141.661",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.11941,
            "range": "± 439.999",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.08598,
            "range": "± 219.183",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.9521,
            "range": "± 665.885",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.34462,
            "range": "± 226.825",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.05427,
            "range": "± 261.246",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.31589,
            "range": "± 374.962",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.35719,
            "range": "± 252.351",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.71249,
            "range": "± 165.998",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.634,
            "range": "± 1.74924",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 193.659,
            "range": "± 7.23207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.2835,
            "range": "± 1.54867",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 367.665,
            "range": "± 11.5326",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 198.466,
            "range": "± 54.7858",
            "unit": "ns",
            "extra": "100 samples\n166 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 430.322,
            "range": "± 18.8679",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.08495,
            "range": "± 51.5065",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 452.827,
            "range": "± 37.3427",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 852.85,
            "range": "± 59.7826",
            "unit": "ns",
            "extra": "100 samples\n39 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 457.359,
            "range": "± 27.3036",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 545.002,
            "range": "± 32.5949",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 361.764,
            "range": "± 21.3234",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 472.514,
            "range": "± 29.9437",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 424.138,
            "range": "± 19.9489",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.15089,
            "range": "± 74.9938",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.73891,
            "range": "± 71.332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.70328,
            "range": "± 159.228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.72142,
            "range": "± 85.7075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.7292,
            "range": "± 226.348",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2084,
            "range": "± 184.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.8812,
            "range": "± 437.065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.56451,
            "range": "± 55.734",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9174,
            "range": "± 133.783",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.883,
            "range": "± 125.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9051,
            "range": "± 98.1921",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.898,
            "range": "± 239.516",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0723,
            "range": "± 158.498",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.3106,
            "range": "± 256.35",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.2205,
            "range": "± 279.082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.7575,
            "range": "± 305.582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.8028,
            "range": "± 263.511",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.9022,
            "range": "± 297.149",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.8441,
            "range": "± 307.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.4161,
            "range": "± 353.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.6178,
            "range": "± 452.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.1353,
            "range": "± 585.776",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.3,
            "range": "± 568.962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.953,
            "range": "± 322.822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.0503,
            "range": "± 396.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.37,
            "range": "± 560.587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.2547,
            "range": "± 383.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.2101,
            "range": "± 488.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.13318,
            "range": "± 63.6074",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.49232,
            "range": "± 127.082",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.02351,
            "range": "± 568.937",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.46303,
            "range": "± 187.334",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.46201,
            "range": "± 296.081",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.66206,
            "range": "± 227.429",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.14821,
            "range": "± 289.703",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.6525,
            "range": "± 162.443",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.69808,
            "range": "± 226.604",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36782,
            "range": "± 207.583",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.4849,
            "range": "± 1.20528",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 176.175,
            "range": "± 5.69527",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.3939,
            "range": "± 2.44864",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 291.495,
            "range": "± 8.55471",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.46886,
            "range": "± 144.479",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.2321,
            "range": "± 946.561",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.2624,
            "range": "± 4.84133",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1278,
            "range": "± 1.00712",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.7864,
            "range": "± 3.29813",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3777,
            "range": "± 1.08691",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.1728,
            "range": "± 1.63449",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0753,
            "range": "± 1.53266",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.5258,
            "range": "± 966.6",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.2721,
            "range": "± 872.087",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.1256,
            "range": "± 5.42276",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.76233,
            "range": "± 24.8281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.603,
            "range": "± 5.13061",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.10389,
            "range": "± 62.0678",
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
          "id": "6166b523106e2290d54252d53d58cda59c370013",
          "message": "fix tracing test",
          "timestamp": "2026-03-07T18:39:10-08:00",
          "tree_id": "81695b72f9f59967c53bc19c3714d868772c5640",
          "url": "https://github.com/nebulastream/nautilus/commit/6166b523106e2290d54252d53d58cda59c370013"
        },
        "date": 1772937859572,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.37037,
            "range": "± 310.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.75101,
            "range": "± 203.238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.70157,
            "range": "± 162.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.0507,
            "range": "± 562.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.964,
            "range": "± 377.888",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6102,
            "range": "± 232.585",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.0185,
            "range": "± 527.779",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.64051,
            "range": "± 145.473",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.247,
            "range": "± 220.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1007,
            "range": "± 209.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1336,
            "range": "± 201.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.6612,
            "range": "± 372.43",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.2493,
            "range": "± 196.375",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.3957,
            "range": "± 987.103",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.8343,
            "range": "± 590.048",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.2112,
            "range": "± 2.14179",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.1377,
            "range": "± 7.20408",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.8444,
            "range": "± 2.18144",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.2078,
            "range": "± 3.83679",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8175,
            "range": "± 2.19177",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.3129,
            "range": "± 3.69244",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.2087,
            "range": "± 1.77136",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.279,
            "range": "± 2.22616",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.035,
            "range": "± 2.05739",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 58.5886,
            "range": "± 11.0262",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.74992,
            "range": "± 67.408",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 138.588,
            "range": "± 14.2251",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.061,
            "range": "± 60.5582",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.50414,
            "range": "± 0.42691",
            "unit": "ns",
            "extra": "100 samples\n8428 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.90935,
            "range": "± 799.413",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 695.529,
            "range": "± 15.6684",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.40026,
            "range": "± 0.371577",
            "unit": "ns",
            "extra": "100 samples\n5473 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.8312,
            "range": "± 6.36287",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0738,
            "range": "± 110.856",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 40.1892,
            "range": "± 5.11088",
            "unit": "ns",
            "extra": "100 samples\n772 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 916.03,
            "range": "± 7.52071",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 199.214,
            "range": "± 9.43895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 192.441,
            "range": "± 29.809",
            "unit": "ns",
            "extra": "100 samples\n169 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 461.033,
            "range": "± 59.5078",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11528,
            "range": "± 128.859",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 487.451,
            "range": "± 62.1252",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 873.339,
            "range": "± 97.0994",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 486.266,
            "range": "± 43.2623",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 554.081,
            "range": "± 59.8407",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 389.937,
            "range": "± 47.6728",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 493.943,
            "range": "± 51.5932",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 449.293,
            "range": "± 46.1616",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ir_add",
            "value": 980.449,
            "range": "± 167.982",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.5601,
            "range": "± 306.191",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.91183,
            "range": "± 903.227",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.08399,
            "range": "± 465.57",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.81128,
            "range": "± 705.189",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.34173,
            "range": "± 451.697",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.01856,
            "range": "± 504.735",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.40148,
            "range": "± 392.441",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.31281,
            "range": "± 515.868",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.75692,
            "range": "± 361.995",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.4006,
            "range": "± 2.2393",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 189.188,
            "range": "± 12.6764",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 28.9557,
            "range": "± 2.83398",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 358.344,
            "range": "± 13.036",
            "unit": "us",
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
          "id": "d55f5d1ef3fb648f377213612d2518991c8cd1ab",
          "message": "fix(SSA): Use correct ValueRef width in SSA creation data structures\n\nThe iterative SSA creation algorithm introduced in #166 used uint16_t\nfor value refs in blockDefinitions and packed (blockId, ref) pairs into\nuint32_t for propagatedValues. Since ValueRef was widened to uint32_t\nin #165, these narrow types silently truncate value refs exceeding 65535.\n\nThis causes false-positive locality matches in propagateValue: a\ntruncated ref can collide with a different, smaller ref, stopping\nupward propagation prematurely. The affected value then never appears\nas a block argument, leading to \"Key $N does not exists in frame\"\nerrors during IR lowering for traces with many operations.\n\n- blockDefinitions: uint16_t set → ValueRef set\n- propagatedValues: uint32_t with 16|16 packing → uint64_t with 32|32\n- getOrBuildDefinitions: return type updated to match\n\nCo-Authored-By: Claude Opus 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-03-11T07:30:38+01:00",
          "tree_id": "b3eb56181b6cb698a8fa6ebe33c8bf1232567b00",
          "url": "https://github.com/nebulastream/nautilus/commit/d55f5d1ef3fb648f377213612d2518991c8cd1ab"
        },
        "date": 1773210879183,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 178.876,
            "range": "± 14.9176",
            "unit": "ns",
            "extra": "100 samples\n169 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 435.668,
            "range": "± 42.4679",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07713,
            "range": "± 93.6802",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 467.113,
            "range": "± 60.0918",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 844.695,
            "range": "± 149.658",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 477.485,
            "range": "± 49.6193",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 556.671,
            "range": "± 73.4749",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 366.125,
            "range": "± 27.4893",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 480.927,
            "range": "± 50.0741",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 428.887,
            "range": "± 29.7688",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.41703,
            "range": "± 235.814",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8976,
            "range": "± 937.73",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.8427,
            "range": "± 2.40637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7176,
            "range": "± 815.087",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.4447,
            "range": "± 1.53143",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.4109,
            "range": "± 1.78185",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 22.8185,
            "range": "± 2.40786",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.8066,
            "range": "± 1.08711",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.1319,
            "range": "± 897.73",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.0711,
            "range": "± 767.633",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.2227,
            "range": "± 5.62874",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.67034,
            "range": "± 38.8538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 133.988,
            "range": "± 7.04425",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.94327,
            "range": "± 60.0257",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 856.152,
            "range": "± 97.0727",
            "unit": "ns",
            "extra": "100 samples\n36 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49824,
            "range": "± 329.018",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.50753,
            "range": "± 1.08395",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.91411,
            "range": "± 458.442",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.3068,
            "range": "± 667.866",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.00055,
            "range": "± 294.35",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.63308,
            "range": "± 732.23",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.09609,
            "range": "± 126.987",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.87944,
            "range": "± 175.488",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.41022,
            "range": "± 143.938",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 14.7703,
            "range": "± 1.12977",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 194.333,
            "range": "± 6.41165",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.1192,
            "range": "± 2.35796",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 333.436,
            "range": "± 11.3385",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.05489,
            "range": "± 49.9718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.61965,
            "range": "± 41.2311",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.5641,
            "range": "± 114.296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.59924,
            "range": "± 91.4646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.6167,
            "range": "± 161.329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1975,
            "range": "± 68.5445",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.4635,
            "range": "± 305.018",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.45953,
            "range": "± 68.2142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8874,
            "range": "± 487.609",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7502,
            "range": "± 55.2272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.7621,
            "range": "± 66.1232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.6267,
            "range": "± 142.007",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.8201,
            "range": "± 63.8272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1278,
            "range": "± 87.9017",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.46758,
            "range": "± 0.349019",
            "unit": "ns",
            "extra": "100 samples\n8469 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.84873,
            "range": "± 711.819",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 672.488,
            "range": "± 17.2311",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.35455,
            "range": "± 0.411782",
            "unit": "ns",
            "extra": "100 samples\n5473 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.4277,
            "range": "± 7.57225",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0515,
            "range": "± 77.6404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 40.6846,
            "range": "± 4.62735",
            "unit": "ns",
            "extra": "100 samples\n747 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 921.516,
            "range": "± 59.9122",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 197.042,
            "range": "± 2.64151",
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
          "id": "3d52beda359bb3e8fc06f6bce5cafee7385eebca",
          "message": "Add val<StructType> support with stack allocation, field access, and copy semantics\n\nIntroduces AllocaOperation for stack-allocated struct values, extends tracing and\nall backends (MLIR, C++, BC) to handle struct construction, field get/set, copy\nconstruction/assignment, and destructors. Adds val_std.hpp with higher-level\nstruct wrappers and comprehensive tests covering construction, mutation, loops,\nand non-trivial types.\n\nCo-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>",
          "timestamp": "2026-03-11T08:57:43+01:00",
          "tree_id": "4529843a3e5b7a8b3b2e1b351a06f1274f251097",
          "url": "https://github.com/nebulastream/nautilus/commit/3d52beda359bb3e8fc06f6bce5cafee7385eebca"
        },
        "date": 1773216225808,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 860.292,
            "range": "± 113.518",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.4653,
            "range": "± 324.306",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.03676,
            "range": "± 573.304",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.757,
            "range": "± 322.192",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.1537,
            "range": "± 424.165",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.03637,
            "range": "± 364.156",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.53535,
            "range": "± 428.608",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.66059,
            "range": "± 946.223",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.00227,
            "range": "± 323.764",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.45461,
            "range": "± 208.302",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.3355,
            "range": "± 1.54784",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 208.152,
            "range": "± 45.1814",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.233,
            "range": "± 2.09383",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 333.563,
            "range": "± 13.1334",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 192.18,
            "range": "± 23.5486",
            "unit": "ns",
            "extra": "100 samples\n163 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 446.351,
            "range": "± 33.2871",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.0842,
            "range": "± 97.5963",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 473.935,
            "range": "± 47.323",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 870.288,
            "range": "± 106.368",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 470.649,
            "range": "± 56.8611",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 558.452,
            "range": "± 46.2939",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 388.27,
            "range": "± 60.4098",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 483.406,
            "range": "± 36.9968",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 443.532,
            "range": "± 44.8842",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.7207,
            "range": "± 1.07321",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.5808,
            "range": "± 2.32767",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.4498,
            "range": "± 6.09783",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1185,
            "range": "± 1.67985",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.1833,
            "range": "± 2.93537",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.0401,
            "range": "± 1.9406",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.2627,
            "range": "± 2.75345",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.9542,
            "range": "± 1.00312",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.167,
            "range": "± 1.72415",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.2377,
            "range": "± 1.85219",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 54.3787,
            "range": "± 7.38011",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.70321,
            "range": "± 58.8168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.671,
            "range": "± 10.5408",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.97714,
            "range": "± 65.3818",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.18363,
            "range": "± 130.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.7614,
            "range": "± 125.099",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.86167,
            "range": "± 792.047",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.74047,
            "range": "± 169.548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.2263,
            "range": "± 258.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3318,
            "range": "± 225.161",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.5799,
            "range": "± 447.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.60798,
            "range": "± 122.961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4938,
            "range": "± 263.221",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5599,
            "range": "± 195.023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.2444,
            "range": "± 270.93",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.1058,
            "range": "± 367.847",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.1891,
            "range": "± 164.944",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.9698,
            "range": "± 366.546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.46893,
            "range": "± 0.362589",
            "unit": "ns",
            "extra": "100 samples\n8453 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.93551,
            "range": "± 691.109",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 729.75,
            "range": "± 25.9724",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.42678,
            "range": "± 0.858869",
            "unit": "ns",
            "extra": "100 samples\n5465 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.1873,
            "range": "± 10.0996",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0778,
            "range": "± 87.0268",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 42.2133,
            "range": "± 5.41628",
            "unit": "ns",
            "extra": "100 samples\n739 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 841.289,
            "range": "± 12.3933",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.722,
            "range": "± 868.216",
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
      }
    ]
  }
}