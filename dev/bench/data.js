window.BENCHMARK_DATA = {
  "lastUpdate": 1772414426093,
  "repoUrl": "https://github.com/nebulastream/nautilus",
  "entries": {
    "Tracing Benchmark": [
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
          "id": "977de8e1b86faa0b53b25ec5ac5c30795408a2fd",
          "message": "Improve Block and TraceOperation performance and safety\n\nPerformance fixes:\n- Fix std::move bugs in Block::addOperation and TraceOperation constructors\n- Replace std::initializer_list with std::vector for move semantics support\n- Fix vector copy in processControlFlowMerge to properly move operations\n\nSafety fixes:\n- Add bounds checking to ExecutionTrace::getBlock, getCurrentBlock,\n  setCurrentBlock, nextOperation, and getCurrentOperation\n- Add documentation for void* ptr lifetime requirements in FunctionCall",
          "timestamp": "2025-12-25T22:05:27+07:00",
          "tree_id": "e1eaf3b51a966c9110af8ba181c44c031ccbd85c",
          "url": "https://github.com/nebulastream/nautilus/commit/977de8e1b86faa0b53b25ec5ac5c30795408a2fd"
        },
        "date": 1766675372428,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 9.40802,
            "range": "± 610.926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 10.0091,
            "range": "± 458.423",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.6423,
            "range": "± 696.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.3138,
            "range": "± 455.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.9918,
            "range": "± 1.16023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.2526,
            "range": "± 533.994",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.586,
            "range": "± 822.83",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.49381,
            "range": "± 451.184",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.8364,
            "range": "± 399.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.5002,
            "range": "± 419.002",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 14.9054,
            "range": "± 399.833",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 29.8735,
            "range": "± 575.022",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.7732,
            "range": "± 524.158",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 25.321,
            "range": "± 483.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.0435,
            "range": "± 1.04003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.7936,
            "range": "± 879.37",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 31.257,
            "range": "± 1.0464",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.9185,
            "range": "± 1.00419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 31.8097,
            "range": "± 1.10936",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 32.2449,
            "range": "± 1.31238",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 32.0956,
            "range": "± 1.17204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 30.5898,
            "range": "± 1.0461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 31.0359,
            "range": "± 1.07302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.0923,
            "range": "± 931.422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 33.424,
            "range": "± 951.786",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 68.5426,
            "range": "± 1.62035",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 35.3589,
            "range": "± 962.183",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 98.898,
            "range": "± 1.6283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.29991,
            "range": "± 249.167",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.76367,
            "range": "± 581.348",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.22997,
            "range": "± 1.20926",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.64176,
            "range": "± 460.033",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.78601,
            "range": "± 1.09075",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.99482,
            "range": "± 687.352",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.97347,
            "range": "± 1.05297",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.78802,
            "range": "± 486.732",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.97123,
            "range": "± 614.546",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.77521,
            "range": "± 697.426",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 20.0986,
            "range": "± 4.78086",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 174.992,
            "range": "± 6.98665",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 29.5882,
            "range": "± 3.54705",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 293.209,
            "range": "± 14.24",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 1013.17,
            "range": "± 197983",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.74838,
            "range": "± 627.391",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.37822,
            "range": "± 1.8255",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.38854,
            "range": "± 765.736",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.24356,
            "range": "± 1.53064",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.50382,
            "range": "± 796.882",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.23185,
            "range": "± 939.619",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.70414,
            "range": "± 790.178",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.57599,
            "range": "± 802.317",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 3.02606,
            "range": "± 927.487",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 17.0423,
            "range": "± 3.47023",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 201.217,
            "range": "± 7.29308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.7125,
            "range": "± 4.81768",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 361.732,
            "range": "± 14.7219",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 173.132,
            "range": "± 37.8414",
            "unit": "ns",
            "extra": "100 samples\n211 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 472.125,
            "range": "± 103.37",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.25537,
            "range": "± 273.122",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 504.395,
            "range": "± 121.556",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 851.238,
            "range": "± 144.546",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 544.895,
            "range": "± 114.213",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 643.686,
            "range": "± 137.928",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 513.868,
            "range": "± 103.232",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 520.962,
            "range": "± 125.382",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 433.964,
            "range": "± 77.6067",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.72665,
            "range": "± 438.389",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.4242,
            "range": "± 2.40141",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.7802,
            "range": "± 7.23549",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.0595,
            "range": "± 2.56737",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.0458,
            "range": "± 4.36644",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.2364,
            "range": "± 2.45742",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 19.1313,
            "range": "± 4.94421",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0679,
            "range": "± 2.13185",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.3273,
            "range": "± 2.50487",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.1688,
            "range": "± 2.24463",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 59.3492,
            "range": "± 12.0284",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.68499,
            "range": "± 68.4656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 135.008,
            "range": "± 11.5334",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.98886,
            "range": "± 64.9982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.5429,
            "range": "± 0.489037",
            "unit": "ns",
            "extra": "100 samples\n8461 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89324,
            "range": "± 540.105",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 610.283,
            "range": "± 38.3078",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.54287,
            "range": "± 0.766508",
            "unit": "ns",
            "extra": "100 samples\n5472 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.7643,
            "range": "± 18.7686",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9537,
            "range": "± 86.7038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 47.5271,
            "range": "± 11.1373",
            "unit": "ns",
            "extra": "100 samples\n759 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 922.08,
            "range": "± 10.6309",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.435,
            "range": "± 213.711",
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
          "id": "c97f5bb9317839fbfc9d19a7aacf47ebb5e8a0af",
          "message": "Improve val<arithmetic> C++ semantic equivalence\n\nFix several issues to make val<T> behave identically to C++ arithmetic types:\n\n- Prefix ++/-- now return non-const reference, enabling ++(++x) chaining\n- Unary + and - operators are now const-qualified, working on const values\n- Add move assignment operator for proper move semantics\n- Add is_integral_val constraint to operator~ for proper SFINAE\n- Add is_arithmetic/is_integral constraints to compound assignment operators\n- Fix operator+ return type using std::conditional_t for proper type deduction\n\nAdd comprehensive tests for all semantic equivalence fixes in both\nval-tests (non-tracing) and execution-tests (tracing mode).",
          "timestamp": "2025-12-25T22:16:14+07:00",
          "tree_id": "ad1ef4df603d43197f496003c1ddd5ead39e319d",
          "url": "https://github.com/nebulastream/nautilus/commit/c97f5bb9317839fbfc9d19a7aacf47ebb5e8a0af"
        },
        "date": 1766676039972,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 2.42987,
            "range": "± 0.457889",
            "unit": "ns",
            "extra": "100 samples\n7659 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 6.92489,
            "range": "± 1.34208",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 665.075,
            "range": "± 32.1981",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 3.92284,
            "range": "± 0.457692",
            "unit": "ns",
            "extra": "100 samples\n4729 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 132.608,
            "range": "± 10.8813",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 25.6181,
            "range": "± 507.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 32.9082,
            "range": "± 1.76215",
            "unit": "ns",
            "extra": "100 samples\n555 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 573.541,
            "range": "± 11.6921",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 124.693,
            "range": "± 4.21524",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 152.278,
            "range": "± 13.9128",
            "unit": "ns",
            "extra": "100 samples\n119 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 395.97,
            "range": "± 30.9672",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.17508,
            "range": "± 50.3817",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 421.279,
            "range": "± 21.2494",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 803.934,
            "range": "± 52.0942",
            "unit": "ns",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 427.281,
            "range": "± 19.5375",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 525.325,
            "range": "± 39.4806",
            "unit": "ns",
            "extra": "100 samples\n37 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 384.532,
            "range": "± 39.1466",
            "unit": "ns",
            "extra": "100 samples\n49 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 450.332,
            "range": "± 36.4108",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 388.186,
            "range": "± 39.2441",
            "unit": "ns",
            "extra": "100 samples\n49 iterations"
          },
          {
            "name": "ir_add",
            "value": 1043.95,
            "range": "± 52458.2",
            "unit": "ns",
            "extra": "100 samples\n18 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.79142,
            "range": "± 127.624",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.57575,
            "range": "± 328.903",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.24586,
            "range": "± 179.552",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.40345,
            "range": "± 259.974",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.47276,
            "range": "± 197.806",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.14024,
            "range": "± 204.581",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.52763,
            "range": "± 130.262",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.75037,
            "range": "± 279.367",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.92238,
            "range": "± 178.15",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 18.809,
            "range": "± 876.695",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 212.825,
            "range": "± 2.99013",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 36.7019,
            "range": "± 2.16472",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 370.102,
            "range": "± 5.29897",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.69323,
            "range": "± 149.038",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.1965,
            "range": "± 231.174",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.35572,
            "range": "± 477.593",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.2072,
            "range": "± 209.172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.8228,
            "range": "± 812.078",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2821,
            "range": "± 200.664",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.9884,
            "range": "± 367.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.16935,
            "range": "± 212.079",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8262,
            "range": "± 211.477",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8529,
            "range": "± 178.235",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8567,
            "range": "± 178.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.7979,
            "range": "± 293.049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 13.3603,
            "range": "± 212.614",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 61.5803,
            "range": "± 379.943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 21.4719,
            "range": "± 602.95",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 22.1605,
            "range": "± 681.918",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 23.079,
            "range": "± 623.865",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 22.6262,
            "range": "± 657.056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 23.5224,
            "range": "± 523.943",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 23.0814,
            "range": "± 540.948",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 22.6945,
            "range": "± 594.023",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 22.1958,
            "range": "± 526.145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 22.7594,
            "range": "± 613.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 22.207,
            "range": "± 623.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 25.1474,
            "range": "± 613.802",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 57.8444,
            "range": "± 542.697",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 27.6568,
            "range": "± 582.826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 86.434,
            "range": "± 615.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.18545,
            "range": "± 53.3765",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.49911,
            "range": "± 111.153",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.08007,
            "range": "± 415.266",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.53072,
            "range": "± 198.928",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.39525,
            "range": "± 339.585",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.54113,
            "range": "± 111.754",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.27782,
            "range": "± 210.023",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.88461,
            "range": "± 235.078",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.6779,
            "range": "± 146.698",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.26823,
            "range": "± 228.475",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.1493,
            "range": "± 1.14184",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 183.93,
            "range": "± 4.07599",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 32.2729,
            "range": "± 1.97699",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 289.31,
            "range": "± 18.2212",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.95564,
            "range": "± 181.286",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.5728,
            "range": "± 1.17312",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.8647,
            "range": "± 4.61951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.5613,
            "range": "± 2.22363",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.2603,
            "range": "± 3.60541",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.7881,
            "range": "± 1.87022",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.3292,
            "range": "± 4.94485",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.89501,
            "range": "± 1.3962",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.0393,
            "range": "± 1.03836",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.5877,
            "range": "± 1.75716",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.7469,
            "range": "± 5.85496",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.59073,
            "range": "± 30.1151",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 130.602,
            "range": "± 3.54324",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.56799,
            "range": "± 32.1419",
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
          "id": "dc04137362e370f2b097036091ef4e2afe629d98",
          "message": "update benchmark",
          "timestamp": "2025-12-26T14:25:59Z",
          "tree_id": "dff746baafc386b8b7b07d401f8a346ac0f54b16",
          "url": "https://github.com/nebulastream/nautilus/commit/dc04137362e370f2b097036091ef4e2afe629d98"
        },
        "date": 1766759411114,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.2527,
            "range": "± 160.94",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.5919,
            "range": "± 844.992",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.5484,
            "range": "± 2.57568",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5791,
            "range": "± 1.2252",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.8247,
            "range": "± 1.98882",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7106,
            "range": "± 1.21069",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.753,
            "range": "± 2.14586",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.14427,
            "range": "± 827.458",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.9728,
            "range": "± 985.514",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.814,
            "range": "± 855.393",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 54.7563,
            "range": "± 4.4376",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.56008,
            "range": "± 19.2302",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 129.784,
            "range": "± 4.22338",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.84937,
            "range": "± 54.8638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.21549,
            "range": "± 0.482508",
            "unit": "ns",
            "extra": "100 samples\n9296 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.90008,
            "range": "± 741.822",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 517.482,
            "range": "± 15.9293",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.36103,
            "range": "± 0.43028",
            "unit": "ns",
            "extra": "100 samples\n5469 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.3207,
            "range": "± 10.1038",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9649,
            "range": "± 382.205",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 39.1438,
            "range": "± 3.35182",
            "unit": "ns",
            "extra": "100 samples\n768 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 889.907,
            "range": "± 169.906",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.056,
            "range": "± 188.823",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 855.501,
            "range": "± 43.96",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.43274,
            "range": "± 188.39",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.66199,
            "range": "± 316.952",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.91136,
            "range": "± 130.118",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.66372,
            "range": "± 255.95",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.11454,
            "range": "± 225.895",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.73986,
            "range": "± 333.383",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.2225,
            "range": "± 170.134",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.25475,
            "range": "± 191.229",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.57682,
            "range": "± 122.175",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.822,
            "range": "± 816.477",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 206.185,
            "range": "± 6.40158",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.7873,
            "range": "± 1.6589",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 368.409,
            "range": "± 8.96458",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 139.229,
            "range": "± 7.31956",
            "unit": "ns",
            "extra": "100 samples\n208 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 400.093,
            "range": "± 15.0899",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07019,
            "range": "± 57.3919",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 418.758,
            "range": "± 20.6621",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 773.253,
            "range": "± 51.3149",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 436.013,
            "range": "± 20.4187",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 527.276,
            "range": "± 36.4077",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 388.072,
            "range": "± 22.6625",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 436.063,
            "range": "± 13.6331",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 381.809,
            "range": "± 18.1291",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.97918,
            "range": "± 50.0277",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.5762,
            "range": "± 61.9383",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.54077,
            "range": "± 65.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.53479,
            "range": "± 57.4508",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.5197,
            "range": "± 182.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1721,
            "range": "± 136.039",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.6636,
            "range": "± 127.545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.44032,
            "range": "± 57.3913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.7895,
            "range": "± 141.209",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7734,
            "range": "± 51.9077",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.8138,
            "range": "± 78.3487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.8235,
            "range": "± 250.117",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.8987,
            "range": "± 72.0433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.2268,
            "range": "± 214.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.6255,
            "range": "± 233.418",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.1441,
            "range": "± 272.501",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.1694,
            "range": "± 247.837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.4372,
            "range": "± 403.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.3393,
            "range": "± 404.672",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.7111,
            "range": "± 383.669",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.7095,
            "range": "± 203.696",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 24.9358,
            "range": "± 289.917",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.5187,
            "range": "± 272.904",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.2679,
            "range": "± 275.196",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.2065,
            "range": "± 302.248",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 61.4536,
            "range": "± 427.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 30.5269,
            "range": "± 288.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 91.5655,
            "range": "± 568.388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.11155,
            "range": "± 64.6416",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.4206,
            "range": "± 303.683",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.85208,
            "range": "± 555.68",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.42326,
            "range": "± 156.769",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.27483,
            "range": "± 318.827",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.54599,
            "range": "± 208.987",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.10107,
            "range": "± 227.648",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.54564,
            "range": "± 133.752",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.59594,
            "range": "± 165.734",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.26243,
            "range": "± 142.415",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.9476,
            "range": "± 1.5249",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 187.951,
            "range": "± 6.20071",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 29.5427,
            "range": "± 1.61396",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 299.31,
            "range": "± 9.08641",
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
          "id": "8543ef20e60bbc74904bdc69383c49ceebc37418",
          "message": "Fix data corruption in ExecutionTrace::getCurrentOperation()\n\nCommit 977de8e introduced a bug by caching a Block reference and then\ntrying to \"rebind\" it after following a JMP. In C++, references cannot\nbe rebound — `block = getCurrentBlock()` is a copy assignment, not a\nrebind, so it was silently overwriting blocks[old_index] with the data\nfrom blocks[new_index] on every JMP traversal, corrupting the trace.\n\nFix by reverting to the original pattern of calling getCurrentBlock()\ndirectly on each access, while retaining the bounds checking that\n977de8e intended to add.\n\nhttps://claude.ai/code/session_0181nv2XJ96cWEyCvhsuBQ8i",
          "timestamp": "2026-03-01T09:17:08-08:00",
          "tree_id": "739951a17e74d4a8b9b7a55d5bc138c68e389f1c",
          "url": "https://github.com/nebulastream/nautilus/commit/8543ef20e60bbc74904bdc69383c49ceebc37418"
        },
        "date": 1772385844307,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.36727,
            "range": "± 193.754",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8789,
            "range": "± 1.06336",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.1268,
            "range": "± 3.02136",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.9726,
            "range": "± 1.40113",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.2988,
            "range": "± 1.89651",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.9034,
            "range": "± 1.30447",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.3749,
            "range": "± 2.05247",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.17994,
            "range": "± 881.373",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2055,
            "range": "± 1.43614",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.0573,
            "range": "± 878.095",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 57.8711,
            "range": "± 6.77908",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.65953,
            "range": "± 26.8408",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.075,
            "range": "± 11.0908",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.97215,
            "range": "± 92.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 150.053,
            "range": "± 20.7122",
            "unit": "ns",
            "extra": "100 samples\n214 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.36,
            "range": "± 34.2142",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07113,
            "range": "± 143.495",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 434.152,
            "range": "± 52.9881",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 762.265,
            "range": "± 64.1524",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 447.02,
            "range": "± 48.3558",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 540.765,
            "range": "± 53.3308",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 442.317,
            "range": "± 42.7483",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 444.245,
            "range": "± 48.4337",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 382.023,
            "range": "± 47.0736",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ir_add",
            "value": 891.25,
            "range": "± 75.3727",
            "unit": "ns",
            "extra": "100 samples\n35 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.3915,
            "range": "± 186.07",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.19294,
            "range": "± 461.642",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.76977,
            "range": "± 246.404",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.38725,
            "range": "± 606.068",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.05903,
            "range": "± 330.422",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.55698,
            "range": "± 327.476",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.16964,
            "range": "± 193.226",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.12129,
            "range": "± 550.461",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.52455,
            "range": "± 255.761",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 14.9233,
            "range": "± 1.73758",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 221.269,
            "range": "± 10.1053",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.792,
            "range": "± 1.98418",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 333.196,
            "range": "± 11.527",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.10372,
            "range": "± 99.4041",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.71996,
            "range": "± 125.441",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.61889,
            "range": "± 113.365",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.71269,
            "range": "± 88.2905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0502,
            "range": "± 203.785",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4552,
            "range": "± 254.948",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1099,
            "range": "± 214.478",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.55249,
            "range": "± 72.377",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9422,
            "range": "± 96.5142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9427,
            "range": "± 68.5631",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0021,
            "range": "± 182.667",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.5208,
            "range": "± 892.327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3104,
            "range": "± 447.949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.4457,
            "range": "± 963.874",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.5322,
            "range": "± 419.799",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.0972,
            "range": "± 496.088",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.9949,
            "range": "± 1.78201",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.0635,
            "range": "± 258.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.5949,
            "range": "± 666.085",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.5981,
            "range": "± 513.735",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.5632,
            "range": "± 323.921",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.6262,
            "range": "± 471.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.254,
            "range": "± 305.109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.9288,
            "range": "± 368.735",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.0331,
            "range": "± 297.563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.9792,
            "range": "± 539.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.2359,
            "range": "± 316.982",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.8073,
            "range": "± 617.64",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.22491,
            "range": "± 124.155",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.44766,
            "range": "± 292.291",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.90947,
            "range": "± 531.021",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.52238,
            "range": "± 262.178",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.5137,
            "range": "± 487.708",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.60464,
            "range": "± 287.283",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 4.61476,
            "range": "± 1.6311",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.6629,
            "range": "± 273.868",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.62604,
            "range": "± 209.214",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.30244,
            "range": "± 253.988",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.7969,
            "range": "± 2.09276",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 186.961,
            "range": "± 6.08316",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 30.2859,
            "range": "± 2.39497",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 299.052,
            "range": "± 9.63994",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.23802,
            "range": "± 0.566381",
            "unit": "ns",
            "extra": "100 samples\n9290 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.03263,
            "range": "± 888.726",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 679.489,
            "range": "± 26.6155",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.44484,
            "range": "± 0.840497",
            "unit": "ns",
            "extra": "100 samples\n5472 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.8424,
            "range": "± 8.70248",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0356,
            "range": "± 87.172",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 40.4249,
            "range": "± 4.99593",
            "unit": "ns",
            "extra": "100 samples\n760 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 817.474,
            "range": "± 7.00346",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.356,
            "range": "± 262.655",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "benjamingreb01@gmail.com",
            "name": "Benjamin Greb",
            "username": "bgrb1"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "3d366e79ba5ff205be4343ba2ea7d44f564495a4",
          "message": "fix inline variable name generation collisions for nameless variables",
          "timestamp": "2026-03-01T09:41:49-08:00",
          "tree_id": "79d2424069c339a1080a19567123fcb8119c1279",
          "url": "https://github.com/nebulastream/nautilus/commit/3d366e79ba5ff205be4343ba2ea7d44f564495a4"
        },
        "date": 1772387123971,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 156.31,
            "range": "± 25.3728",
            "unit": "ns",
            "extra": "100 samples\n214 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 394.604,
            "range": "± 42.1376",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.0493,
            "range": "± 102.341",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 404.444,
            "range": "± 34.6418",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 773.489,
            "range": "± 82.2618",
            "unit": "ns",
            "extra": "100 samples\n38 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 428.156,
            "range": "± 54.8129",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 534.998,
            "range": "± 79.8122",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 466.442,
            "range": "± 73.7036",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 433.9,
            "range": "± 55.0203",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 359.71,
            "range": "± 22.778",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.1645,
            "range": "± 0.35156",
            "unit": "ns",
            "extra": "100 samples\n9277 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.12899,
            "range": "± 1.14641",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 693.695,
            "range": "± 32.8826",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.36362,
            "range": "± 0.440141",
            "unit": "ns",
            "extra": "100 samples\n5465 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.7939,
            "range": "± 11.3382",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1398,
            "range": "± 508.168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 39.1578,
            "range": "± 2.90273",
            "unit": "ns",
            "extra": "100 samples\n770 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 816.179,
            "range": "± 8.69379",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.067,
            "range": "± 4.98272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 888.641,
            "range": "± 80.1413",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.53492,
            "range": "± 272.95",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.64543,
            "range": "± 571.6",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.90919,
            "range": "± 235.012",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.6939,
            "range": "± 478.463",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.12139,
            "range": "± 238.368",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.80883,
            "range": "± 362.586",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.23253,
            "range": "± 158.201",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.21512,
            "range": "± 259.469",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.62085,
            "range": "± 356.125",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.9848,
            "range": "± 1.26315",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 203.683,
            "range": "± 10.6177",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.814,
            "range": "± 2.09373",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 357.65,
            "range": "± 15.5666",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.11394,
            "range": "± 169.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.7658,
            "range": "± 201.168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.66556,
            "range": "± 190.012",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.8253,
            "range": "± 230.49",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.8819,
            "range": "± 554.389",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.2018,
            "range": "± 427.19",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.9034,
            "range": "± 431.265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.92381,
            "range": "± 1.4444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0906,
            "range": "± 305.632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9852,
            "range": "± 271.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.7091,
            "range": "± 382.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.8026,
            "range": "± 552.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9874,
            "range": "± 173.574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.4516,
            "range": "± 679.97",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.551,
            "range": "± 433.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.0351,
            "range": "± 447.408",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.6304,
            "range": "± 1.44148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4329,
            "range": "± 1.05439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.9129,
            "range": "± 1.67261",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.0138,
            "range": "± 559.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.9334,
            "range": "± 641.036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.9197,
            "range": "± 430.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.3993,
            "range": "± 488.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.2106,
            "range": "± 475.689",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.3951,
            "range": "± 602.623",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.6445,
            "range": "± 679.9",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.8166,
            "range": "± 624.63",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.8375,
            "range": "± 2.00782",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.28853,
            "range": "± 267.443",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.54334,
            "range": "± 403.879",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.88569,
            "range": "± 748.779",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.52975,
            "range": "± 324.05",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.40552,
            "range": "± 461.354",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.62985,
            "range": "± 303.325",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.12936,
            "range": "± 272.561",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.64971,
            "range": "± 264.98",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.62607,
            "range": "± 243.984",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.32565,
            "range": "± 318.45",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.3535,
            "range": "± 1.89163",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 176.797,
            "range": "± 6.59312",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 28.4594,
            "range": "± 1.93677",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 283.082,
            "range": "± 8.77274",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.63789,
            "range": "± 238.547",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.6545,
            "range": "± 1.67523",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.853,
            "range": "± 6.02236",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.6898,
            "range": "± 1.7092",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.9007,
            "range": "± 3.61344",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.0639,
            "range": "± 2.10182",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.842,
            "range": "± 3.17736",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.77285,
            "range": "± 1.22823",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.0066,
            "range": "± 1.97293",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.5001,
            "range": "± 1.25776",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 55.5966,
            "range": "± 6.1512",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.68417,
            "range": "± 53.627",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.851,
            "range": "± 9.21902",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.02359,
            "range": "± 70.09",
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
          "id": "cfbb0f31e1c5eb3d050618130c7ff7937e08907f",
          "message": "Add logging option to print static val array state\n\nAdd log::options::getLogStaticVars() / setLogStaticVars() toggle\n(default: off). When enabled, pushStaticVal and popStaticVal log\nthe full contents of the static variable array after each mutation,\nshowing the dereferenced values of all tracked static_val instances.\n\nhttps://claude.ai/code/session_016dPQCzgSEJMFbFrUj3aiQA",
          "timestamp": "2026-03-01T13:18:45-08:00",
          "tree_id": "3ced6a5de83fe159b2e2e88bfe3042154cd2ca04",
          "url": "https://github.com/nebulastream/nautilus/commit/cfbb0f31e1c5eb3d050618130c7ff7937e08907f"
        },
        "date": 1772400210924,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.51608,
            "range": "± 0.514793",
            "unit": "ns",
            "extra": "100 samples\n8456 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.87375,
            "range": "± 501.152",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 642.798,
            "range": "± 14.6567",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.48037,
            "range": "± 0.898624",
            "unit": "ns",
            "extra": "100 samples\n5470 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.7285,
            "range": "± 10.0351",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0698,
            "range": "± 119.321",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 38.5803,
            "range": "± 1.35952",
            "unit": "ns",
            "extra": "100 samples\n747 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 921.276,
            "range": "± 10.0515",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 199.642,
            "range": "± 4.88965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 147.979,
            "range": "± 8.24528",
            "unit": "ns",
            "extra": "100 samples\n198 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.664,
            "range": "± 28.102",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04087,
            "range": "± 75.3063",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 424.016,
            "range": "± 61.2733",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 745.097,
            "range": "± 36.9928",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 448.401,
            "range": "± 32.0838",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 533.958,
            "range": "± 22.0374",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 394.146,
            "range": "± 21.0951",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 438.504,
            "range": "± 33.2173",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 373.405,
            "range": "± 25.7972",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.18692,
            "range": "± 50.6342",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.71377,
            "range": "± 128.16",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.69311,
            "range": "± 65.4188",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.73206,
            "range": "± 72.3899",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.6351,
            "range": "± 171.989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3324,
            "range": "± 72.576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.0338,
            "range": "± 230.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.61914,
            "range": "± 89.7144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9719,
            "range": "± 102.075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8959,
            "range": "± 146.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9463,
            "range": "± 66.292",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.9217,
            "range": "± 140.144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.1265,
            "range": "± 102.355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.3836,
            "range": "± 83.8678",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.0592,
            "range": "± 225.727",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.5358,
            "range": "± 204.271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.6423,
            "range": "± 233.183",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.6963,
            "range": "± 240.898",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.6572,
            "range": "± 251.355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.1676,
            "range": "± 228.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.1105,
            "range": "± 261.409",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.4372,
            "range": "± 254.99",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.9397,
            "range": "± 299.691",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.5775,
            "range": "± 266.831",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.59,
            "range": "± 258.923",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.0122,
            "range": "± 505.53",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.9788,
            "range": "± 227.388",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.8852,
            "range": "± 930.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.10165,
            "range": "± 67.3881",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.34865,
            "range": "± 179.243",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.69868,
            "range": "± 497.72",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.44347,
            "range": "± 459.853",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.31277,
            "range": "± 503.162",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.53427,
            "range": "± 130.324",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.08302,
            "range": "± 230.592",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.45617,
            "range": "± 115.748",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.575,
            "range": "± 153.044",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.22414,
            "range": "± 149.488",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 17.9826,
            "range": "± 1.69293",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 182.999,
            "range": "± 7.17143",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 29.1298,
            "range": "± 1.94404",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 295.944,
            "range": "± 13.6343",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 873.741,
            "range": "± 50.2047",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.60383,
            "range": "± 248.084",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.71444,
            "range": "± 484.928",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.90886,
            "range": "± 160.2",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.61481,
            "range": "± 280.11",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.15353,
            "range": "± 338.831",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.71005,
            "range": "± 234.391",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.41873,
            "range": "± 207.032",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.26932,
            "range": "± 599.06",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.56458,
            "range": "± 67.858",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.2422,
            "range": "± 1.14256",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 206.57,
            "range": "± 7.18705",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.6035,
            "range": "± 1.94209",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 373.027,
            "range": "± 11.2396",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.38224,
            "range": "± 119.9",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.149,
            "range": "± 869.619",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.5986,
            "range": "± 2.8409",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.0246,
            "range": "± 929.142",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.4345,
            "range": "± 1.46722",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.1115,
            "range": "± 859.286",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.913,
            "range": "± 1.51119",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 8.72668,
            "range": "± 603.437",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.4613,
            "range": "± 939.692",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.1592,
            "range": "± 770.755",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 54.1471,
            "range": "± 3.84542",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.74786,
            "range": "± 17.1239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 133.966,
            "range": "± 5.73585",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.15073,
            "range": "± 358.312",
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
          "id": "c6538521d7b5818ae89e53fd0f8a99723ef0c66d",
          "message": "Replace recursive SSA creation with iterative worklist algorithm (#166)\n\nCo-authored-by: Claude <noreply@anthropic.com>",
          "timestamp": "2026-03-01T17:16:22-08:00",
          "tree_id": "345fa51daa198542ba51a5b8e0f5c8568ac1cc98",
          "url": "https://github.com/nebulastream/nautilus/commit/c6538521d7b5818ae89e53fd0f8a99723ef0c66d"
        },
        "date": 1772414425309,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.56705,
            "range": "± 0.621098",
            "unit": "ns",
            "extra": "100 samples\n8497 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.95924,
            "range": "± 682.476",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 713.753,
            "range": "± 34.7594",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.65033,
            "range": "± 1.04376",
            "unit": "ns",
            "extra": "100 samples\n5489 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.2563,
            "range": "± 5.3405",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1387,
            "range": "± 503.559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 43.1498,
            "range": "± 7.55338",
            "unit": "ns",
            "extra": "100 samples\n756 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 937.168,
            "range": "± 48.0906",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.211,
            "range": "± 2.50649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 971.937,
            "range": "± 165.578",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.59306,
            "range": "± 373.041",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.96416,
            "range": "± 1.06966",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.17632,
            "range": "± 586.196",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.97047,
            "range": "± 1.00058",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.29099,
            "range": "± 464.088",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.88051,
            "range": "± 571.341",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.41947,
            "range": "± 325.046",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.36031,
            "range": "± 460.295",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.77291,
            "range": "± 415.872",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.711,
            "range": "± 1.8223",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 186.088,
            "range": "± 6.70495",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 32.7466,
            "range": "± 9.38273",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 341.479,
            "range": "± 10.6864",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 2.70712,
            "range": "± 471.707",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.4627,
            "range": "± 1.8223",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.3729,
            "range": "± 6.1413",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.7865,
            "range": "± 1.92586",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.3233,
            "range": "± 3.63593",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.0627,
            "range": "± 2.36641",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.062,
            "range": "± 3.54786",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0977,
            "range": "± 2.06713",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.2035,
            "range": "± 2.0807",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.9068,
            "range": "± 1.79589",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 56.6042,
            "range": "± 8.77137",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.80523,
            "range": "± 296.013",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 138.314,
            "range": "± 17.7794",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.0981,
            "range": "± 155.499",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 422.773,
            "range": "± 80.1565",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 1054.76,
            "range": "± 141376",
            "unit": "ns",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 2.5965,
            "range": "± 197.284",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 1071.69,
            "range": "± 125694",
            "unit": "ns",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 1782.99,
            "range": "± 135522",
            "unit": "ns",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 1044.89,
            "range": "± 113809",
            "unit": "ns",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 1301.78,
            "range": "± 155789",
            "unit": "ns",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 1112.33,
            "range": "± 104410",
            "unit": "ns",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 1084.44,
            "range": "± 90470.5",
            "unit": "ns",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 960.817,
            "range": "± 113.619",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.30879,
            "range": "± 169.838",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.87601,
            "range": "± 148.422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.75728,
            "range": "± 143.521",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.0694,
            "range": "± 212.188",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.7352,
            "range": "± 612.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.9102,
            "range": "± 237.992",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.7159,
            "range": "± 295.737",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.62653,
            "range": "± 171.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.5835,
            "range": "± 1.64327",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3738,
            "range": "± 245.146",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.3924,
            "range": "± 447.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.6562,
            "range": "± 944.111",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.5013,
            "range": "± 392.708",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.7371,
            "range": "± 539.466",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.2822,
            "range": "± 831.924",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.9791,
            "range": "± 703.121",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.0927,
            "range": "± 779.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.8373,
            "range": "± 650.574",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.7123,
            "range": "± 1.80403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.1588,
            "range": "± 685.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 30.0872,
            "range": "± 844.891",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 29.2612,
            "range": "± 566.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.7505,
            "range": "± 673.851",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.352,
            "range": "± 478.794",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 32.0935,
            "range": "± 413.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 66.9275,
            "range": "± 2.59217",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 34.8692,
            "range": "± 670.519",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 97.4763,
            "range": "± 1.99453",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.24634,
            "range": "± 182.346",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.51838,
            "range": "± 346.368",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.89397,
            "range": "± 795.519",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.59923,
            "range": "± 387.533",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.57623,
            "range": "± 775.523",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.78714,
            "range": "± 519.17",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.29652,
            "range": "± 530.209",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.74378,
            "range": "± 483.411",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.7194,
            "range": "± 357.02",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36092,
            "range": "± 306.534",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 18.8837,
            "range": "± 2.37822",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 173.919,
            "range": "± 11.9",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 29.7495,
            "range": "± 3.24073",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 294.979,
            "range": "± 17.5425",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}