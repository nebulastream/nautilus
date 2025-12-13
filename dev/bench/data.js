window.BENCHMARK_DATA = {
  "lastUpdate": 1765615353099,
  "repoUrl": "https://github.com/nebulastream/nautilus",
  "entries": {
    "Tracing Benchmark": [
      {
        "commit": {
          "author": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d4340824afed0a1080d62a0c236c637386a1a609",
          "message": "fix gcc14 compile error with fmt (#92)",
          "timestamp": "2025-08-22T10:11:34+02:00",
          "tree_id": "680fc55fbb40ebd4f9198a8e1f5453ea5c7b375e",
          "url": "https://github.com/nebulastream/nautilus/commit/d4340824afed0a1080d62a0c236c637386a1a609"
        },
        "date": 1755850692003,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.23973,
            "range": "± 119.94",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8055,
            "range": "± 1.51347",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.3781,
            "range": "± 6.12965",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.147,
            "range": "± 1.50897",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 19.7759,
            "range": "± 2.59207",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7442,
            "range": "± 1.96757",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.8543,
            "range": "± 2.59189",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.4545,
            "range": "± 1.78414",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.9387,
            "range": "± 2.04427",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.65244,
            "range": "± 1.64234",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 115.829,
            "range": "± 7.19277",
            "unit": "ns",
            "extra": "100 samples\n237 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 393.256,
            "range": "± 33.8939",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00247,
            "range": "± 62.0739",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 403.848,
            "range": "± 22.7467",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 755.869,
            "range": "± 64.7146",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 431.312,
            "range": "± 23.9284",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 527.437,
            "range": "± 53.1569",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 382.869,
            "range": "± 12.7451",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 444.243,
            "range": "± 36.3862",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 364.628,
            "range": "± 15.5852",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ir_add",
            "value": 508.558,
            "range": "± 46.0918",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.34537,
            "range": "± 210.034",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.9045,
            "range": "± 417.795",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.66334,
            "range": "± 186.919",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.98389,
            "range": "± 339.271",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.93865,
            "range": "± 220.897",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.39464,
            "range": "± 265.687",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.05278,
            "range": "± 101.42",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.83739,
            "range": "± 194.091",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.42599,
            "range": "± 188.09",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.05274,
            "range": "± 135.1",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.55527,
            "range": "± 321.067",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.95594,
            "range": "± 199.366",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.4865,
            "range": "± 375.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.8591,
            "range": "± 1.82002",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.9658,
            "range": "± 462.714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.7951,
            "range": "± 309.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.57336,
            "range": "± 162.634",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.5536,
            "range": "± 391.378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4346,
            "range": "± 284.47",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.19936,
            "range": "± 0.518287",
            "unit": "ns",
            "extra": "100 samples\n9531 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89928,
            "range": "± 504.431",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 578.351,
            "range": "± 18.1708",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.37823,
            "range": "± 0.23948",
            "unit": "ns",
            "extra": "100 samples\n6798 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.9817,
            "range": "± 6.5505",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9044,
            "range": "± 102.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.9225,
            "range": "± 3.38119",
            "unit": "ns",
            "extra": "100 samples\n1015 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 903.7,
            "range": "± 54.2873",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 191.298,
            "range": "± 1.63371",
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
          "id": "82bce4ecb28883ba45d02cc1af27b7b00a837cdc",
          "message": "Add Ubuntu 24.04 with gcc-14 and g++-14 to build matrix (#94)",
          "timestamp": "2025-08-25T11:25:17+02:00",
          "tree_id": "b0ecf2fbf96b495c5e5dd9a16e1fd09d2b03c98f",
          "url": "https://github.com/nebulastream/nautilus/commit/82bce4ecb28883ba45d02cc1af27b7b00a837cdc"
        },
        "date": 1756114331423,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.27724,
            "range": "± 125.153",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.3093,
            "range": "± 1.29831",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.9743,
            "range": "± 5.04637",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.1986,
            "range": "± 1.57878",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.1484,
            "range": "± 2.92636",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.8758,
            "range": "± 1.78312",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.0278,
            "range": "± 2.46384",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.0798,
            "range": "± 1.04192",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.7528,
            "range": "± 1.6638",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.56511,
            "range": "± 1.13962",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 119.738,
            "range": "± 9.88589",
            "unit": "ns",
            "extra": "100 samples\n262 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 395.256,
            "range": "± 32.2514",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.03276,
            "range": "± 120.48",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 409.764,
            "range": "± 37.0982",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 744.237,
            "range": "± 55.4279",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 425.605,
            "range": "± 26.2483",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 512.963,
            "range": "± 33.5336",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 408.648,
            "range": "± 43.9161",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 429.829,
            "range": "± 23.2572",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 370.103,
            "range": "± 27.5421",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 527.586,
            "range": "± 28.635",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.40139,
            "range": "± 250.364",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.16472,
            "range": "± 716.165",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.75005,
            "range": "± 331.899",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.17125,
            "range": "± 479.385",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.93819,
            "range": "± 219.565",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.48494,
            "range": "± 363.979",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.09329,
            "range": "± 145.799",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.96174,
            "range": "± 237.372",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.41492,
            "range": "± 205.33",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.88599,
            "range": "± 154.4",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.54435,
            "range": "± 197.191",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.60325,
            "range": "± 203.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.9595,
            "range": "± 505.036",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2546,
            "range": "± 419.54",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.0954,
            "range": "± 396.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.5628,
            "range": "± 382.903",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.36794,
            "range": "± 130.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.664,
            "range": "± 195.429",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8128,
            "range": "± 301.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.16044,
            "range": "± 0.325411",
            "unit": "ns",
            "extra": "100 samples\n9557 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.01324,
            "range": "± 980.135",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 580.825,
            "range": "± 29.7603",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.41151,
            "range": "± 0.397287",
            "unit": "ns",
            "extra": "100 samples\n6817 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.6938,
            "range": "± 6.48953",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9253,
            "range": "± 141.959",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 29.7152,
            "range": "± 1.65796",
            "unit": "ns",
            "extra": "100 samples\n1017 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 894.058,
            "range": "± 6.73359",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 190.821,
            "range": "± 2.56208",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "committer": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "distinct": true,
          "id": "a1ff92c799c4e5a32a395ec72ebc33e6034ab335",
          "message": "val.hpp: fix includes",
          "timestamp": "2025-10-04T12:08:40+02:00",
          "tree_id": "f17220dc85329bcea0893967dd23f68913803bf4",
          "url": "https://github.com/nebulastream/nautilus/commit/a1ff92c799c4e5a32a395ec72ebc33e6034ab335"
        },
        "date": 1759572958464,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.20979,
            "range": "± 68.7682",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.636,
            "range": "± 1.62222",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.5349,
            "range": "± 4.78809",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.4099,
            "range": "± 1.06729",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.4756,
            "range": "± 2.71087",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.0343,
            "range": "± 1.91521",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.207,
            "range": "± 1.80926",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.2485,
            "range": "± 882.729",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.9947,
            "range": "± 1.46141",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.75872,
            "range": "± 1.02508",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 117.464,
            "range": "± 7.16418",
            "unit": "ns",
            "extra": "100 samples\n262 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 389.84,
            "range": "± 20.7342",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.9868049999999999,
            "range": "± 0.055548200000000006",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 402.474,
            "range": "± 21.5282",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 743.008,
            "range": "± 42.7546",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 420.794,
            "range": "± 18.3459",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 509.566,
            "range": "± 28.6209",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 400.298,
            "range": "± 32.7707",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 427.439,
            "range": "± 14.9774",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 362.727,
            "range": "± 20.9491",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "ir_add",
            "value": 486.763,
            "range": "± 23.2399",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.23462,
            "range": "± 127.976",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.96776,
            "range": "± 575.827",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.591,
            "range": "± 141.789",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.9606,
            "range": "± 303.369",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.82453,
            "range": "± 179.101",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.34849,
            "range": "± 224.985",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.06571,
            "range": "± 146.438",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.82779,
            "range": "± 194.95",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.32315,
            "range": "± 134.442",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.90247,
            "range": "± 172.827",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.5811,
            "range": "± 189.632",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.45347,
            "range": "± 129.002",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.61815,
            "range": "± 390.266",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2426,
            "range": "± 577.603",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.9959,
            "range": "± 195.837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.4907,
            "range": "± 391.377",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.40578,
            "range": "± 125.926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.7366,
            "range": "± 185.378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7353,
            "range": "± 157.392",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.21421,
            "range": "± 0.495583",
            "unit": "ns",
            "extra": "100 samples\n9517 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.88215,
            "range": "± 511.469",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 534.655,
            "range": "± 24.8657",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.46687,
            "range": "± 0.612592",
            "unit": "ns",
            "extra": "100 samples\n6824 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.4435,
            "range": "± 8.11328",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9205,
            "range": "± 130.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.8385,
            "range": "± 2.69225",
            "unit": "ns",
            "extra": "100 samples\n1017 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 892.57,
            "range": "± 3.57366",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 191.301,
            "range": "± 1.52641",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "committer": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "distinct": true,
          "id": "d5088062ae61c7016e668d0fea3510881df0882e",
          "message": "devcontainer: add clangd extension",
          "timestamp": "2025-10-04T12:16:58+02:00",
          "tree_id": "02523179616fcc5bfd698292ea617b878dbe93c3",
          "url": "https://github.com/nebulastream/nautilus/commit/d5088062ae61c7016e668d0fea3510881df0882e"
        },
        "date": 1759573384934,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.30236,
            "range": "± 161.913",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6866,
            "range": "± 1.407",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.8833,
            "range": "± 4.92052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8133,
            "range": "± 1.82077",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.1025,
            "range": "± 2.4855",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.9803,
            "range": "± 1.68017",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.1026,
            "range": "± 2.6044",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.0317,
            "range": "± 1.56087",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.1326,
            "range": "± 2.18728",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.64126,
            "range": "± 1.23343",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 122.019,
            "range": "± 11.5372",
            "unit": "ns",
            "extra": "100 samples\n255 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 404.691,
            "range": "± 42.0323",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.03945,
            "range": "± 139.012",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 411.347,
            "range": "± 50.4238",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 782.902,
            "range": "± 81.4356",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 442.129,
            "range": "± 33.1452",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 540.317,
            "range": "± 68.1013",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 428.897,
            "range": "± 55.3491",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 590.821,
            "range": "± 224.869",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 368.96,
            "range": "± 26.324",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 561.637,
            "range": "± 73.2265",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.3517,
            "range": "± 175.468",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.16567,
            "range": "± 638.517",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.68547,
            "range": "± 187.302",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.14615,
            "range": "± 454.19",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.86877,
            "range": "± 222.055",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.56655,
            "range": "± 453.636",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.05516,
            "range": "± 90.5531",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.9081,
            "range": "± 235.13",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.39617,
            "range": "± 177.808",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.0298,
            "range": "± 235.628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.59168,
            "range": "± 177.683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.54854,
            "range": "± 150.259",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.59462,
            "range": "± 197.364",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.2739,
            "range": "± 639.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1791,
            "range": "± 329.926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1478,
            "range": "± 681.029",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.6855,
            "range": "± 316.889",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9789,
            "range": "± 595.834",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8066,
            "range": "± 175.252",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.18367,
            "range": "± 0.426895",
            "unit": "ns",
            "extra": "100 samples\n9532 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.87257,
            "range": "± 482.913",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 576.699,
            "range": "± 44.2427",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45277,
            "range": "± 0.651276",
            "unit": "ns",
            "extra": "100 samples\n6794 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.7683,
            "range": "± 6.83277",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9166,
            "range": "± 120.351",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.0461,
            "range": "± 3.46949",
            "unit": "ns",
            "extra": "100 samples\n993 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 644.221,
            "range": "± 11.6548",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 177.626,
            "range": "± 18.9857",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "committer": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "distinct": true,
          "id": "38151295d928896027b5ef83ce8b1fc471ccdb01",
          "message": "val_ptr<bool>: simplify store",
          "timestamp": "2025-10-04T12:48:56+02:00",
          "tree_id": "bc0f31b336209107fe7e7c17da538de2c9ff1ed2",
          "url": "https://github.com/nebulastream/nautilus/commit/38151295d928896027b5ef83ce8b1fc471ccdb01"
        },
        "date": 1759575393063,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.3344,
            "range": "± 112.198",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6429,
            "range": "± 1.67652",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.7775,
            "range": "± 5.87908",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.5929,
            "range": "± 1.75878",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.3009,
            "range": "± 2.84744",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.2183,
            "range": "± 2.62918",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.1024,
            "range": "± 2.67344",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.6469,
            "range": "± 1.41254",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.337,
            "range": "± 4.55458",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.0106,
            "range": "± 3.56694",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 117.579,
            "range": "± 16.5371",
            "unit": "ns",
            "extra": "100 samples\n277 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 387.887,
            "range": "± 44.8211",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.999045,
            "range": "± 0.0916769",
            "unit": "us",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 397.516,
            "range": "± 31.9873",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 764.098,
            "range": "± 88.6121",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 443.841,
            "range": "± 43.0167",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 513.1,
            "range": "± 41.1225",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 379.489,
            "range": "± 40.8785",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 430.461,
            "range": "± 31.6332",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 367.225,
            "range": "± 27.8552",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 561.77,
            "range": "± 40.3369",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.41468,
            "range": "± 267.369",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.26047,
            "range": "± 684.815",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.72536,
            "range": "± 244.907",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.25517,
            "range": "± 452.101",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.95707,
            "range": "± 199.942",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.52614,
            "range": "± 336.4",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.21039,
            "range": "± 274.01",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.02967,
            "range": "± 335.172",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.5221,
            "range": "± 280.432",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 9.1769,
            "range": "± 432.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 10.3092,
            "range": "± 2.42479",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.32908,
            "range": "± 1.1385",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.187,
            "range": "± 1.78743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.792,
            "range": "± 2.02223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 16.0847,
            "range": "± 1.26615",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 32.8752,
            "range": "± 1.27742",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 9.70074,
            "range": "± 980.873",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 15.6584,
            "range": "± 749.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 14.1622,
            "range": "± 508.069",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.44025,
            "range": "± 0.224891",
            "unit": "ns",
            "extra": "100 samples\n8718 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.80574,
            "range": "± 459.897",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 600.012,
            "range": "± 35.7842",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45037,
            "range": "± 0.449152",
            "unit": "ns",
            "extra": "100 samples\n6847 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.9026,
            "range": "± 11.1222",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9925,
            "range": "± 228.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 32.8891,
            "range": "± 5.34622",
            "unit": "ns",
            "extra": "100 samples\n1007 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 809.61,
            "range": "± 8.1336",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.549,
            "range": "± 275.02",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philipp.grulich@observeinc.com",
            "name": "Philipp Grulich"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "2cba94224f4e5fa206965c43176355f934d0c40b",
          "message": "update to catch2 v3.11.0",
          "timestamp": "2025-10-05T19:58:21+02:00",
          "tree_id": "ad008603c3cc6db892e14a49b1a26e78ab14e0d6",
          "url": "https://github.com/nebulastream/nautilus/commit/2cba94224f4e5fa206965c43176355f934d0c40b"
        },
        "date": 1759687463047,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.16218,
            "range": "± 232.854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.82417,
            "range": "± 199.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.83236,
            "range": "± 495.522",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.79789,
            "range": "± 234.729",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.1786,
            "range": "± 258.705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7994,
            "range": "± 292.435",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.7927,
            "range": "± 331.362",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.57148,
            "range": "± 165.795",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.6285,
            "range": "± 277.932",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1517,
            "range": "± 272.639",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 495.44,
            "range": "± 23.222",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46197,
            "range": "± 172.648",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.35935,
            "range": "± 503.095",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.90772,
            "range": "± 214.548",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.59547,
            "range": "± 472.86",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.03938,
            "range": "± 205.285",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.59294,
            "range": "± 173.926",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.2882,
            "range": "± 134.489",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.21718,
            "range": "± 308.776",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.60827,
            "range": "± 225.698",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.588,
            "range": "± 6.36639",
            "unit": "ns",
            "extra": "100 samples\n261 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 391.584,
            "range": "± 29.1798",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00507,
            "range": "± 59.9141",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 424.711,
            "range": "± 72.8495",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 758.667,
            "range": "± 64.8396",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 433.295,
            "range": "± 22.0216",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 517.239,
            "range": "± 34.2836",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 379.269,
            "range": "± 35.5197",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 433.568,
            "range": "± 31.5697",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 376.753,
            "range": "± 27.5545",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.37075,
            "range": "± 88.5063",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.9111,
            "range": "± 1.59421",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.3877,
            "range": "± 6.64403",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7125,
            "range": "± 1.51845",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.8341,
            "range": "± 2.95371",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3158,
            "range": "± 1.98315",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.3083,
            "range": "± 3.11676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.7347,
            "range": "± 1.7254",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.4821,
            "range": "± 1.86409",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.0079,
            "range": "± 1.54311",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.50421,
            "range": "± 0.448713",
            "unit": "ns",
            "extra": "100 samples\n8451 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.97602,
            "range": "± 889.497",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 570.846,
            "range": "± 20.9323",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.48839,
            "range": "± 0.693952",
            "unit": "ns",
            "extra": "100 samples\n6647 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.4556,
            "range": "± 7.01553",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9717,
            "range": "± 291.905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 32.7895,
            "range": "± 3.21748",
            "unit": "ns",
            "extra": "100 samples\n920 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 910.565,
            "range": "± 10.2679",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.188,
            "range": "± 596.599",
            "unit": "ms",
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
            "email": "45949765+keyseven123@users.noreply.github.com",
            "name": "Nils Schubert",
            "username": "keyseven123"
          },
          "distinct": true,
          "id": "37ba8a7c1155c48c27607a48a971be3bd8cf86ed",
          "message": "fix: Uninitialized val_ptr\n\nFixed a bug that would result in a compiler error if `val<int8_t*>` is not initialized. The culprit was that a nullptr is not of type `void` and therefore, `tracing::traceConstant` would not find a constexpr case.\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-10-06T21:11:11+02:00",
          "tree_id": "6de39674e659e040ca6e2ce0ef2f8049af2cdabc",
          "url": "https://github.com/nebulastream/nautilus/commit/37ba8a7c1155c48c27607a48a971be3bd8cf86ed"
        },
        "date": 1759778338625,
        "tool": "catch2",
        "benches": [
          {
            "name": "ssa_add",
            "value": 111.777,
            "range": "± 16.1604",
            "unit": "ns",
            "extra": "100 samples\n267 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 370.859,
            "range": "± 21.1915",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.965356,
            "range": "± 0.0776619",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 391.382,
            "range": "± 20.9675",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 714.377,
            "range": "± 32.5689",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 428.666,
            "range": "± 29.9443",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 507.487,
            "range": "± 24.865",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 367.632,
            "range": "± 24.5891",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 437.08,
            "range": "± 36.8961",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 361.796,
            "range": "± 23.4935",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.26179,
            "range": "± 99.9271",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.5137,
            "range": "± 1.52896",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.9434,
            "range": "± 5.18028",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.341,
            "range": "± 1.68893",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 19.8998,
            "range": "± 2.54768",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.8691,
            "range": "± 1.7404",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.9953,
            "range": "± 2.62953",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.4956,
            "range": "± 1.03028",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.7175,
            "range": "± 1.81814",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.51553,
            "range": "± 1.10365",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.94445,
            "range": "± 271.436",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.58104,
            "range": "± 181.408",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.49798,
            "range": "± 190.33",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.74273,
            "range": "± 499.868",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.5342,
            "range": "± 555.265",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2544,
            "range": "± 374.175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1321,
            "range": "± 1.13993",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.60467,
            "range": "± 363.887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9001,
            "range": "± 301.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9011,
            "range": "± 242.438",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.48721,
            "range": "± 0.403124",
            "unit": "ns",
            "extra": "100 samples\n8445 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.99866,
            "range": "± 763.686",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 550.836,
            "range": "± 25.978",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.54717,
            "range": "± 0.846369",
            "unit": "ns",
            "extra": "100 samples\n6633 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.7272,
            "range": "± 11.0741",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9504,
            "range": "± 76.3915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.6569,
            "range": "± 2.25904",
            "unit": "ns",
            "extra": "100 samples\n928 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 909.443,
            "range": "± 4.99243",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.139,
            "range": "± 3.03532",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 512.23,
            "range": "± 42.8203",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.45127,
            "range": "± 293.847",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.45694,
            "range": "± 449.26",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.85957,
            "range": "± 390.756",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.52703,
            "range": "± 519.252",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.12696,
            "range": "± 282.121",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.64498,
            "range": "± 357.849",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.23665,
            "range": "± 132.981",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.079,
            "range": "± 205.873",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.62827,
            "range": "± 450.323",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
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
            "email": "45949765+keyseven123@users.noreply.github.com",
            "name": "Nils Schubert",
            "username": "keyseven123"
          },
          "distinct": true,
          "id": "8e822c5ace0ebd4fd11a49cd6c0bf29b622d7956",
          "message": "chore: Clang-tidy no function as variable name\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-10-06T21:27:26+02:00",
          "tree_id": "1ce838a55797b7cf0d68f9ec4d28aebf6e7d35e7",
          "url": "https://github.com/nebulastream/nautilus/commit/8e822c5ace0ebd4fd11a49cd6c0bf29b622d7956"
        },
        "date": 1759779274998,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.02263,
            "range": "± 390.337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.59195,
            "range": "± 564.749",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.40559,
            "range": "± 348.617",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.40488,
            "range": "± 160.962",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.1691,
            "range": "± 484.542",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.0116,
            "range": "± 582.129",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.7818,
            "range": "± 628.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.36755,
            "range": "± 142.535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.6434,
            "range": "± 239.788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7285,
            "range": "± 223.619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 518.202,
            "range": "± 40.3984",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.5392,
            "range": "± 269.146",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.52233,
            "range": "± 596.276",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.95513,
            "range": "± 329.351",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.55867,
            "range": "± 483.439",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.04324,
            "range": "± 175.225",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.69259,
            "range": "± 310.561",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.3971,
            "range": "± 188.717",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.14392,
            "range": "± 221.692",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.55261,
            "range": "± 186.879",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.526,
            "range": "± 11.8523",
            "unit": "ns",
            "extra": "100 samples\n262 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 373.384,
            "range": "± 19.4447",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04101,
            "range": "± 130.708",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 411.005,
            "range": "± 46.5426",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 760.657,
            "range": "± 82.6897",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 437.325,
            "range": "± 33.5157",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 505.318,
            "range": "± 41.6035",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 386.551,
            "range": "± 46.6185",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 444.621,
            "range": "± 52.9213",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 373.131,
            "range": "± 46.4773",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.47269,
            "range": "± 180.001",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.9446,
            "range": "± 1.56169",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.1706,
            "range": "± 6.0757",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7873,
            "range": "± 1.39115",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.7976,
            "range": "± 3.04842",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.2758,
            "range": "± 1.90649",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.0569,
            "range": "± 3.08921",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.7494,
            "range": "± 1.23484",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6407,
            "range": "± 1.80772",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.94833,
            "range": "± 1.53581",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.45078,
            "range": "± 0.316042",
            "unit": "ns",
            "extra": "100 samples\n8455 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.94694,
            "range": "± 738.272",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 544.138,
            "range": "± 27.5656",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.39006,
            "range": "± 0.349507",
            "unit": "ns",
            "extra": "100 samples\n6636 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.7064,
            "range": "± 6.32502",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8971,
            "range": "± 85.3531",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 34.2196,
            "range": "± 5.72701",
            "unit": "ns",
            "extra": "100 samples\n936 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 912.143,
            "range": "± 25.7904",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.108,
            "range": "± 874.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "philipp.grulich@observeinc.com",
            "name": "Philipp Grulich"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "821c611dd700e38128c1fe6bed99065e338bd418",
          "message": "update to mlir 21.1.2",
          "timestamp": "2025-10-07T09:58:24+02:00",
          "tree_id": "26b6ba26ebce792a09f9c0f5c31a2e0bc921d9c9",
          "url": "https://github.com/nebulastream/nautilus/commit/821c611dd700e38128c1fe6bed99065e338bd418"
        },
        "date": 1759824332998,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.12117,
            "range": "± 0.207238",
            "unit": "ns",
            "extra": "100 samples\n9345 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.78787,
            "range": "± 12.2404",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 561.191,
            "range": "± 33.0194",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45621,
            "range": "± 0.521198",
            "unit": "ns",
            "extra": "100 samples\n6652 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.9293,
            "range": "± 7.93213",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9295,
            "range": "± 145.94",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.9851,
            "range": "± 4.20408",
            "unit": "ns",
            "extra": "100 samples\n984 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 910.024,
            "range": "± 7.19207",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.141,
            "range": "± 732.114",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 113.006,
            "range": "± 6.98424",
            "unit": "ns",
            "extra": "100 samples\n267 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 372.134,
            "range": "± 17.7972",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.994324,
            "range": "± 0.0622519",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 388.228,
            "range": "± 21.1471",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 717.791,
            "range": "± 53.7736",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 420.293,
            "range": "± 22.2703",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 515.06,
            "range": "± 37.9397",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 364.043,
            "range": "± 16.049",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 421.52,
            "range": "± 21.3383",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 355,
            "range": "± 16.7301",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "ir_add",
            "value": 509.5,
            "range": "± 27.2258",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46083,
            "range": "± 163.291",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.63802,
            "range": "± 500.42",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.9745,
            "range": "± 302.969",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.87901,
            "range": "± 413.861",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.10278,
            "range": "± 439.442",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.58467,
            "range": "± 160.078",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.23206,
            "range": "± 184.832",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.16087,
            "range": "± 310.328",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.53882,
            "range": "± 148.471",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.39071,
            "range": "± 94.091",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.9544,
            "range": "± 1.44641",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.5617,
            "range": "± 6.4574",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8123,
            "range": "± 1.66663",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.8848,
            "range": "± 3.04822",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3701,
            "range": "± 1.80373",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.1682,
            "range": "± 2.96467",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.8084,
            "range": "± 1.39025",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.4067,
            "range": "± 1.9859",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.97265,
            "range": "± 1.56855",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.61455,
            "range": "± 347.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.92507,
            "range": "± 446.8",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.61157,
            "range": "± 229.435",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.59802,
            "range": "± 184.065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0363,
            "range": "± 774.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.9316,
            "range": "± 279.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.3896,
            "range": "± 244.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.74274,
            "range": "± 200.195",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.6785,
            "range": "± 246.926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.674,
            "range": "± 257.23",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.6227,
            "range": "± 596.666",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.5374,
            "range": "± 950.537",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.4493,
            "range": "± 1.19926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.7838,
            "range": "± 453.378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.9787,
            "range": "± 735.831",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.8104,
            "range": "± 794.916",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.7575,
            "range": "± 564.727",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.6239,
            "range": "± 375.051",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.2973,
            "range": "± 666.056",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.905,
            "range": "± 724.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.04557,
            "range": "± 57.1759",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.55012,
            "range": "± 197.457",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.96609,
            "range": "± 573.105",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.52764,
            "range": "± 179.349",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.58986,
            "range": "± 423.684",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.6811,
            "range": "± 243.774",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.07856,
            "range": "± 262.646",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.0969,
            "range": "± 138.62",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.71043,
            "range": "± 216.362",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.3197,
            "range": "± 152.478",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "committer": {
            "email": "matthis@posteo.de",
            "name": "Matthis Gördel",
            "username": "fwc"
          },
          "distinct": true,
          "id": "fa8004b4d688a1801e1dd978bce154a402a5dd00",
          "message": "val_ptr: add binary comparison operators",
          "timestamp": "2025-10-10T15:36:26+02:00",
          "tree_id": "073855a7f97a8904285914bcf97526e022747bfb",
          "url": "https://github.com/nebulastream/nautilus/commit/fa8004b4d688a1801e1dd978bce154a402a5dd00"
        },
        "date": 1760103830076,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.12409,
            "range": "± 0.2243",
            "unit": "ns",
            "extra": "100 samples\n9316 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.86683,
            "range": "± 514.074",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 560.403,
            "range": "± 25.7282",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.47762,
            "range": "± 0.620362",
            "unit": "ns",
            "extra": "100 samples\n6637 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.018,
            "range": "± 6.32221",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9547,
            "range": "± 124.628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.9107,
            "range": "± 2.81827",
            "unit": "ns",
            "extra": "100 samples\n981 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 911.004,
            "range": "± 10.4763",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.813,
            "range": "± 4.58894",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 516.631,
            "range": "± 33.9316",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.40255,
            "range": "± 161.567",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.40939,
            "range": "± 486.367",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.90774,
            "range": "± 274.666",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.45765,
            "range": "± 510.918",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.04178,
            "range": "± 267.961",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.60226,
            "range": "± 294.105",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.21651,
            "range": "± 117.661",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.11408,
            "range": "± 248.694",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.56001,
            "range": "± 236.123",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ssa_add",
            "value": 111.141,
            "range": "± 6.39162",
            "unit": "ns",
            "extra": "100 samples\n266 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 376.882,
            "range": "± 28.1505",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 0.9825990000000001,
            "range": "± 0.0566373",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 401.673,
            "range": "± 13.6284",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 730.67,
            "range": "± 40.7498",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 428.902,
            "range": "± 21.3042",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 517.68,
            "range": "± 34.3572",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 387.675,
            "range": "± 44.9977",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 427.971,
            "range": "± 25.493",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 364.167,
            "range": "± 23.3646",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.35096,
            "range": "± 100.259",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.0307,
            "range": "± 1.73372",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.1396,
            "range": "± 5.81487",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.726,
            "range": "± 1.61398",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.7891,
            "range": "± 2.94896",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.2455,
            "range": "± 1.57476",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.8927,
            "range": "± 3.02426",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.3161,
            "range": "± 1.22308",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2669,
            "range": "± 1.60148",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.86526,
            "range": "± 1.36006",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.08139,
            "range": "± 276.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.75258,
            "range": "± 254.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.78827,
            "range": "± 233.49",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.9164,
            "range": "± 369.419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.1713,
            "range": "± 655.671",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2177,
            "range": "± 376.743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.4222,
            "range": "± 752.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.48694,
            "range": "± 185.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1665,
            "range": "± 562.134",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7757,
            "range": "± 160.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.9135,
            "range": "± 586.105",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.8717,
            "range": "± 385.958",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.2672,
            "range": "± 727.205",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.8472,
            "range": "± 532.089",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.846,
            "range": "± 687.823",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.1177,
            "range": "± 719.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.9423,
            "range": "± 742.738",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.2807,
            "range": "± 656.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.0772,
            "range": "± 766.309",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.9258,
            "range": "± 697.736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.11802,
            "range": "± 59.1855",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.68163,
            "range": "± 284.37",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.99393,
            "range": "± 486.316",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.61203,
            "range": "± 214.153",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.63603,
            "range": "± 424.813",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.71693,
            "range": "± 283.345",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.12318,
            "range": "± 372.414",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.24627,
            "range": "± 243.572",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.7452,
            "range": "± 174.8",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.38557,
            "range": "± 155.817",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "aljoschalepping@gmail.com",
            "name": "alepping",
            "username": "alepping"
          },
          "committer": {
            "email": "aljoschalepping@gmail.com",
            "name": "Aljoscha",
            "username": "alepping"
          },
          "distinct": true,
          "id": "5fa4c9043d961238d283bf129b82c59e1476974a",
          "message": "chore(FunctionAttributes): adds nounwind attribute",
          "timestamp": "2025-11-16T17:50:30+01:00",
          "tree_id": "ebc84664333b313416316bf3d3fd7cbc50ef7448",
          "url": "https://github.com/nebulastream/nautilus/commit/5fa4c9043d961238d283bf129b82c59e1476974a"
        },
        "date": 1763312175321,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.156,
            "range": "± 0.340405",
            "unit": "ns",
            "extra": "100 samples\n9316 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.90214,
            "range": "± 584.164",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 554.708,
            "range": "± 28.7492",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.78744,
            "range": "± 1.01801",
            "unit": "ns",
            "extra": "100 samples\n6629 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.4818,
            "range": "± 7.45183",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9107,
            "range": "± 97.8572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.2534,
            "range": "± 3.42499",
            "unit": "ns",
            "extra": "100 samples\n969 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 812.452,
            "range": "± 12.8577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.496,
            "range": "± 197.831",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.90183,
            "range": "± 155.019",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.65019,
            "range": "± 228.956",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.48535,
            "range": "± 135.881",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.49993,
            "range": "± 123.368",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.9537,
            "range": "± 293.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5305,
            "range": "± 377.974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.9922,
            "range": "± 451.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.44696,
            "range": "± 137.098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.806,
            "range": "± 176.79",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7991,
            "range": "± 199.239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.0972,
            "range": "± 356.794",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.6331,
            "range": "± 356.625",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.9483,
            "range": "± 644.485",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.1363,
            "range": "± 626.21",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.9995,
            "range": "± 655.627",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.5031,
            "range": "± 679.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.6415,
            "range": "± 805.513",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.7016,
            "range": "± 657.829",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.074,
            "range": "± 606.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.7224,
            "range": "± 584.931",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.0628,
            "range": "± 59.9412",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.6082,
            "range": "± 150.104",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.01307,
            "range": "± 548.971",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.54536,
            "range": "± 181.878",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.63476,
            "range": "± 515.72",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.70453,
            "range": "± 241.068",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.07139,
            "range": "± 224.672",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.17636,
            "range": "± 147.362",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.69352,
            "range": "± 172.811",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36363,
            "range": "± 168.648",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.37085,
            "range": "± 86.9987",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8121,
            "range": "± 1.02494",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.7291,
            "range": "± 3.75107",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.639,
            "range": "± 1.11822",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.8321,
            "range": "± 2.15789",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.2637,
            "range": "± 1.80256",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.382,
            "range": "± 3.13412",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.8533,
            "range": "± 938.284",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2605,
            "range": "± 1.38891",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.77961,
            "range": "± 1.13437",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.189,
            "range": "± 9.41115",
            "unit": "ns",
            "extra": "100 samples\n264 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.197,
            "range": "± 39.3642",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.03877,
            "range": "± 77.3356",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 404.933,
            "range": "± 29.3699",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 755.093,
            "range": "± 60.8927",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 443.771,
            "range": "± 21.5705",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 506.771,
            "range": "± 21.4482",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 393.651,
            "range": "± 42.6517",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 449.514,
            "range": "± 31.7627",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 366.202,
            "range": "± 17.1749",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ir_add",
            "value": 512.514,
            "range": "± 41.7395",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.36249,
            "range": "± 138.784",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.35043,
            "range": "± 715.794",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.81946,
            "range": "± 222.994",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.54569,
            "range": "± 304.44",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.06804,
            "range": "± 204.685",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.58124,
            "range": "± 219.367",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.21318,
            "range": "± 186.805",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.18038,
            "range": "± 285.909",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.54446,
            "range": "± 186.813",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "aljoschalepping@gmail.com",
            "name": "alepping",
            "username": "alepping"
          },
          "committer": {
            "email": "aljoschalepping@gmail.com",
            "name": "Aljoscha",
            "username": "alepping"
          },
          "distinct": true,
          "id": "a6d9260061b017d59afe90d84e8669ed3463dffd",
          "message": "fix(VarVal): fixing ambiguous overload",
          "timestamp": "2025-11-19T17:03:22+01:00",
          "tree_id": "011efd75380af0a5cbf7cfa7270cc40f31b343ab",
          "url": "https://github.com/nebulastream/nautilus/commit/a6d9260061b017d59afe90d84e8669ed3463dffd"
        },
        "date": 1763568532628,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 8.01044,
            "range": "± 165.408",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.78516,
            "range": "± 305.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.94022,
            "range": "± 505.711",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.69689,
            "range": "± 213.161",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0479,
            "range": "± 347.491",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2876,
            "range": "± 546.352",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1978,
            "range": "± 365.631",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.54073,
            "range": "± 115.071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.005,
            "range": "± 281.274",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9444,
            "range": "± 222.894",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.2094,
            "range": "± 461.89",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.955,
            "range": "± 473.547",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.9594,
            "range": "± 423.484",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.1588,
            "range": "± 584.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.1888,
            "range": "± 798.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.5587,
            "range": "± 486.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.5429,
            "range": "± 461.06",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.7405,
            "range": "± 438.67",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.3235,
            "range": "± 649.006",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.8436,
            "range": "± 385.964",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.23987,
            "range": "± 66.939",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.83915,
            "range": "± 233.817",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.32851,
            "range": "± 561.694",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.82079,
            "range": "± 210.999",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.85709,
            "range": "± 388.909",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.97026,
            "range": "± 370.01",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.37051,
            "range": "± 381.256",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.40142,
            "range": "± 308.064",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 3.02721,
            "range": "± 258.768",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.60325,
            "range": "± 226.259",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_add",
            "value": 502.103,
            "range": "± 29.3486",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46101,
            "range": "± 221.434",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.49772,
            "range": "± 620.286",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.97432,
            "range": "± 357.843",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.67366,
            "range": "± 576.528",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.11411,
            "range": "± 292.54",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.67316,
            "range": "± 275.919",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.27628,
            "range": "± 211.945",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.19027,
            "range": "± 306.053",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.55529,
            "range": "± 210.058",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.16528,
            "range": "± 0.370766",
            "unit": "ns",
            "extra": "100 samples\n9310 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.03657,
            "range": "± 894.344",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 533.582,
            "range": "± 25.3835",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.4978,
            "range": "± 0.762459",
            "unit": "ns",
            "extra": "100 samples\n6642 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.0087,
            "range": "± 4.80315",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9206,
            "range": "± 97.2895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.5186,
            "range": "± 2.87919",
            "unit": "ns",
            "extra": "100 samples\n969 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 809.386,
            "range": "± 18.2058",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.636,
            "range": "± 736.575",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.165,
            "range": "± 8.5851",
            "unit": "ns",
            "extra": "100 samples\n263 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 389.821,
            "range": "± 22.2537",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00331,
            "range": "± 59.5461",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 403.801,
            "range": "± 21.8795",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 772.49,
            "range": "± 76.3242",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 442.178,
            "range": "± 20.16",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 508.577,
            "range": "± 28.8275",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 393.467,
            "range": "± 61.3182",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 449.985,
            "range": "± 31.0178",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 378.16,
            "range": "± 34.792",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.3974,
            "range": "± 121.218",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.0036,
            "range": "± 1.47929",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.1172,
            "range": "± 5.51655",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7015,
            "range": "± 1.35798",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.2491,
            "range": "± 3.45311",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.2656,
            "range": "± 1.73414",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.3213,
            "range": "± 2.6052",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.6768,
            "range": "± 1.11903",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.4141,
            "range": "± 1.76253",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.77902,
            "range": "± 1.56626",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
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
          "id": "530817c24a94343dffca6b37aa1e7e1040c35c5a",
          "message": "hotfix benchmarks (#112)\n\nThe last change broke the nautilus benchmarks, which we will fix with this change.",
          "timestamp": "2025-12-04T21:21:36+07:00",
          "tree_id": "a040eb9d2659efada0b1d60fe2a67e27c9fb126f",
          "url": "https://github.com/nebulastream/nautilus/commit/530817c24a94343dffca6b37aa1e7e1040c35c5a"
        },
        "date": 1764858510335,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.33807,
            "range": "± 193.442",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8168,
            "range": "± 1.17524",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.1907,
            "range": "± 4.60795",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.2539,
            "range": "± 1.80788",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.1166,
            "range": "± 2.18983",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.713,
            "range": "± 1.85278",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.2327,
            "range": "± 2.42641",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.0924,
            "range": "± 1.79536",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.3548,
            "range": "± 1.595",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.94357,
            "range": "± 1.33234",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 63.7351,
            "range": "± 8.74646",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 4.42603,
            "range": "± 52.8144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 134.621,
            "range": "± 9.91006",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 7.97043,
            "range": "± 556.745",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.6598,
            "range": "± 0.740186",
            "unit": "ns",
            "extra": "100 samples\n8453 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.79259,
            "range": "± 434.137",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 536.76,
            "range": "± 25.2921",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.47134,
            "range": "± 0.518299",
            "unit": "ns",
            "extra": "100 samples\n6645 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.8096,
            "range": "± 11.4263",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.0226,
            "range": "± 705.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.577,
            "range": "± 3.95854",
            "unit": "ns",
            "extra": "100 samples\n995 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 911.08,
            "range": "± 6.39789",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.759,
            "range": "± 2.13199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 532.402,
            "range": "± 61.2143",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48015,
            "range": "± 205.508",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.68986,
            "range": "± 865.849",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.01525,
            "range": "± 580.686",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.81777,
            "range": "± 980.746",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.2075,
            "range": "± 333.021",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.70728,
            "range": "± 308.521",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25977,
            "range": "± 181.335",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.22807,
            "range": "± 238.499",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.65361,
            "range": "± 291.428",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.9069,
            "range": "± 918.49",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 206.026,
            "range": "± 8.3397",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.8151,
            "range": "± 1.78131",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 364.577,
            "range": "± 12.017",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.01139,
            "range": "± 109.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.65858,
            "range": "± 88.9515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.62388,
            "range": "± 133.117",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.71849,
            "range": "± 161.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.8961,
            "range": "± 214.33",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.194,
            "range": "± 178.987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.1395,
            "range": "± 314.283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.53375,
            "range": "± 103.052",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9447,
            "range": "± 146.99",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9671,
            "range": "± 196.15",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9026,
            "range": "± 135.452",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.1632,
            "range": "± 169.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0462,
            "range": "± 150.829",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.7249,
            "range": "± 225.361",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.1605,
            "range": "± 335.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.8463,
            "range": "± 392.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.9349,
            "range": "± 311.465",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.1233,
            "range": "± 363.52",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.982,
            "range": "± 315.054",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.0697,
            "range": "± 330.032",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.3438,
            "range": "± 312.675",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.6157,
            "range": "± 276.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.4109,
            "range": "± 1.91455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.8926,
            "range": "± 728.399",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.7742,
            "range": "± 320.839",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 62.3445,
            "range": "± 476.627",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.1529,
            "range": "± 324.228",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.5224,
            "range": "± 1.12429",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 42.5753,
            "range": "± 3.07041",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 46.3798,
            "range": "± 3.35971",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 53.1276,
            "range": "± 3.62848",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 30.7015,
            "range": "± 3.43045",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 50.5944,
            "range": "± 4.05745",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 32.5312,
            "range": "± 4.47633",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 49.9802,
            "range": "± 3.87056",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 45.6448,
            "range": "± 3.82738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 47.4661,
            "range": "± 4.24902",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 47.0669,
            "range": "± 3.06322",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 69.2306,
            "range": "± 4.44488",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 203.496,
            "range": "± 6.36429",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 75.5825,
            "range": "± 4.16786",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 297.072,
            "range": "± 10.2325",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 117.815,
            "range": "± 20.9425",
            "unit": "ns",
            "extra": "100 samples\n284 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 406.29,
            "range": "± 80.2132",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.03046,
            "range": "± 130.806",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 424.267,
            "range": "± 61.5776",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 781.52,
            "range": "± 103.269",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 468.646,
            "range": "± 82.5646",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 543.288,
            "range": "± 61.6442",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 412.026,
            "range": "± 73.2857",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 455.367,
            "range": "± 75.981",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 397.676,
            "range": "± 80.0639",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "michalke@tu-berlin.de",
            "name": "Adrian Michalke",
            "username": "adrianmichalke"
          },
          "committer": {
            "email": "philippgrulich@hotmail.de",
            "name": "Philipp Grulich",
            "username": "PhilippGrulich"
          },
          "distinct": true,
          "id": "5bde5cf63387bde3b77adaa9027cf13f33a62854",
          "message": "ubsan fix",
          "timestamp": "2025-12-09T07:29:34+07:00",
          "tree_id": "dd5f3dd374312e05de33654b8fbe30f627a9fb99",
          "url": "https://github.com/nebulastream/nautilus/commit/5bde5cf63387bde3b77adaa9027cf13f33a62854"
        },
        "date": 1765240571709,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.53112,
            "range": "± 0.553457",
            "unit": "ns",
            "extra": "100 samples\n8451 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.87307,
            "range": "± 463.563",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 535.463,
            "range": "± 22.3722",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 5.18311,
            "range": "± 1.36168",
            "unit": "ns",
            "extra": "100 samples\n6614 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 97.2141,
            "range": "± 12.5616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.1359,
            "range": "± 1.57742",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.4007,
            "range": "± 4.82776",
            "unit": "ns",
            "extra": "100 samples\n990 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 899.29,
            "range": "± 42.3329",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 191.303,
            "range": "± 665.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 516.955,
            "range": "± 40.8295",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46718,
            "range": "± 213.686",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.48881,
            "range": "± 536.451",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.82089,
            "range": "± 238.777",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.62552,
            "range": "± 535.189",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.13731,
            "range": "± 324.693",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.6604,
            "range": "± 317.083",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.30454,
            "range": "± 256.496",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.19548,
            "range": "± 323.09",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.5748,
            "range": "± 224.438",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.1517,
            "range": "± 1.50021",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 205.941,
            "range": "± 6.59683",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.7807,
            "range": "± 1.82535",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 360.907,
            "range": "± 11.694",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.98025,
            "range": "± 138.92",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.63404,
            "range": "± 166.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.56269,
            "range": "± 136.402",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.63954,
            "range": "± 198.813",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.3074,
            "range": "± 1.45786",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3217,
            "range": "± 580.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2479,
            "range": "± 580.703",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.51939,
            "range": "± 193.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0941,
            "range": "± 431.086",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1079,
            "range": "± 493.517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.9303,
            "range": "± 226.512",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.541,
            "range": "± 472.313",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.042,
            "range": "± 217.809",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.0977,
            "range": "± 461.527",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.3213,
            "range": "± 620.018",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.5366,
            "range": "± 620.6",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.4439,
            "range": "± 648.511",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.323,
            "range": "± 404.049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.3841,
            "range": "± 615.256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.2036,
            "range": "± 679.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.1217,
            "range": "± 632.325",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.9248,
            "range": "± 524.008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.4735,
            "range": "± 597.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.0757,
            "range": "± 559.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.1896,
            "range": "± 1.283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.1306,
            "range": "± 657.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.6024,
            "range": "± 553.256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.4226,
            "range": "± 818.42",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 45.6365,
            "range": "± 4.72799",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 50.6508,
            "range": "± 4.93035",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 54.4731,
            "range": "± 4.70044",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 46.5324,
            "range": "± 3.7002",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 50.3416,
            "range": "± 3.98483",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 47.9148,
            "range": "± 3.9197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 32.7858,
            "range": "± 3.0429",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 44.6699,
            "range": "± 3.80287",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 48.4414,
            "range": "± 4.89738",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 47.386,
            "range": "± 4.20537",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 67.2191,
            "range": "± 5.2099",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 203.131,
            "range": "± 11.5522",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 74.8376,
            "range": "± 5.62335",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 291.347,
            "range": "± 9.08623",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 121.04,
            "range": "± 10.5988",
            "unit": "ns",
            "extra": "100 samples\n258 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 398.583,
            "range": "± 34.0839",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.03907,
            "range": "± 107.321",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 424.131,
            "range": "± 54.1833",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 760.179,
            "range": "± 78.6451",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 444.954,
            "range": "± 45.8124",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 530.936,
            "range": "± 55.2786",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 400.844,
            "range": "± 36.9986",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 450.328,
            "range": "± 62.3646",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 358.144,
            "range": "± 19.2154",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.37409,
            "range": "± 132.8",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.0348,
            "range": "± 1.55813",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.78,
            "range": "± 5.87369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.615,
            "range": "± 1.59593",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.8453,
            "range": "± 3.27604",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.1739,
            "range": "± 1.96912",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.1147,
            "range": "± 2.87844",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.8412,
            "range": "± 1.66831",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2545,
            "range": "± 1.87114",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.80459,
            "range": "± 1.29872",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 58.8805,
            "range": "± 7.13013",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 4.42824,
            "range": "± 35.8049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 133.361,
            "range": "± 9.48364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 7.80581,
            "range": "± 47.8149",
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
            "email": "lukas.schwerdtfeger@gmail.com",
            "name": "Lukas Schwerdtfeger",
            "username": "ls-1801"
          },
          "distinct": true,
          "id": "413c123da42c478e2da5dc8c8c5155a62d3760a6",
          "message": "add inline option to docs",
          "timestamp": "2025-12-09T10:44:55+01:00",
          "tree_id": "fbc2d24080d73588a9fc8fae93e1f59145d14022",
          "url": "https://github.com/nebulastream/nautilus/commit/413c123da42c478e2da5dc8c8c5155a62d3760a6"
        },
        "date": 1765273895781,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 592.06,
            "range": "± 128.258",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.41555,
            "range": "± 387.713",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.52706,
            "range": "± 1.38799",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.77185,
            "range": "± 366.466",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.53191,
            "range": "± 787.63",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.87741,
            "range": "± 252.098",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.61449,
            "range": "± 502.868",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.24682,
            "range": "± 355.59",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.20056,
            "range": "± 669.199",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.53673,
            "range": "± 389.109",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.9067,
            "range": "± 3.11562",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 222.99,
            "range": "± 7.56",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.9915,
            "range": "± 3.20163",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 332.824,
            "range": "± 40.4196",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.27553,
            "range": "± 130.829",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.2926,
            "range": "± 1.25204",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.8511,
            "range": "± 4.94931",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.8702,
            "range": "± 1.86886",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.692,
            "range": "± 3.06557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.9509,
            "range": "± 1.74935",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.9138,
            "range": "± 3.2322",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.8844,
            "range": "± 1.22823",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.6761,
            "range": "± 1.55218",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.0277,
            "range": "± 2.4213",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 62.2766,
            "range": "± 11.9707",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 4.50126,
            "range": "± 91.4657",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 131.71,
            "range": "± 8.06761",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 7.7495,
            "range": "± 51.3509",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 117.795,
            "range": "± 14.9178",
            "unit": "ns",
            "extra": "100 samples\n268 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 381.121,
            "range": "± 27.0113",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00882,
            "range": "± 97.0296",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 412.257,
            "range": "± 63.1455",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 744.183,
            "range": "± 48.4909",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 419.9,
            "range": "± 27.4661",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 496.919,
            "range": "± 34.628",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 393.004,
            "range": "± 41.9123",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 434.498,
            "range": "± 46.4056",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 366.532,
            "range": "± 32.6708",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.52408,
            "range": "± 0.520852",
            "unit": "ns",
            "extra": "100 samples\n8447 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.99649,
            "range": "± 1.07386",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 539.668,
            "range": "± 30.4208",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.39268,
            "range": "± 0.346393",
            "unit": "ns",
            "extra": "100 samples\n6632 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.2616,
            "range": "± 8.48178",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8967,
            "range": "± 73.0754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.4181,
            "range": "± 3.10667",
            "unit": "ns",
            "extra": "100 samples\n959 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 910.967,
            "range": "± 8.67022",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.148,
            "range": "± 1.23126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.88186,
            "range": "± 105.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.60163,
            "range": "± 159.974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.48335,
            "range": "± 122.412",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.62591,
            "range": "± 259.247",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0956,
            "range": "± 583.972",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3468,
            "range": "± 981.318",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.0996,
            "range": "± 422.276",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.43021,
            "range": "± 120.234",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8969,
            "range": "± 229.286",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9096,
            "range": "± 436.218",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 12.7441,
            "range": "± 173.896",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.4406,
            "range": "± 302.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.8269,
            "range": "± 151.069",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1422,
            "range": "± 391.576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.2876,
            "range": "± 863.528",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.9521,
            "range": "± 941.978",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.2823,
            "range": "± 584.387",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.6104,
            "range": "± 579.725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.217,
            "range": "± 480.599",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.8034,
            "range": "± 649.71",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.6778,
            "range": "± 509.698",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.8866,
            "range": "± 513.902",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.3769,
            "range": "± 633.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.751,
            "range": "± 718.863",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 29.5139,
            "range": "± 666.931",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.237,
            "range": "± 1.00423",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.7116,
            "range": "± 507.991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.9702,
            "range": "± 1.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 27.6519,
            "range": "± 2.70574",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 32.651,
            "range": "± 3.56791",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 39.9834,
            "range": "± 6.73258",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 34.3831,
            "range": "± 3.34418",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 34.6344,
            "range": "± 4.28369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 35.596,
            "range": "± 3.74045",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 34.9868,
            "range": "± 5.88571",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 32.8062,
            "range": "± 3.45185",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 36.3107,
            "range": "± 4.92836",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 33.8906,
            "range": "± 3.12022",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 55.6259,
            "range": "± 5.91326",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 212.318,
            "range": "± 10.5361",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 59.5901,
            "range": "± 5.60742",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 323.912,
            "range": "± 16.7687",
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
          "id": "9d2ba9acc77a324feaf9bb2c77451a18a90678e5",
          "message": "feat(Assume): implement assume functions and MLIR intrinsic",
          "timestamp": "2025-12-11T18:44:18+07:00",
          "tree_id": "7786c0e281f2d687dc2b3e5d468a079fc074c62d",
          "url": "https://github.com/nebulastream/nautilus/commit/9d2ba9acc77a324feaf9bb2c77451a18a90678e5"
        },
        "date": 1765453861885,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 7.98498,
            "range": "± 149.876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.64187,
            "range": "± 149.295",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.57388,
            "range": "± 167.209",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.66013,
            "range": "± 183.677",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.5326,
            "range": "± 737.326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3575,
            "range": "± 525.538",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.2262,
            "range": "± 383.654",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.57373,
            "range": "± 188.653",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9595,
            "range": "± 210.169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.9675,
            "range": "± 270.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0438,
            "range": "± 296.649",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.8874,
            "range": "± 238.837",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.0551,
            "range": "± 281.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.4024,
            "range": "± 427.974",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.1725,
            "range": "± 306.761",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.8006,
            "range": "± 335.628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.8869,
            "range": "± 376.511",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.7919,
            "range": "± 457.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.8588,
            "range": "± 404.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.9643,
            "range": "± 297.679",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.2695,
            "range": "± 490.606",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.352,
            "range": "± 432.871",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.8321,
            "range": "± 340.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.6605,
            "range": "± 333.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 28.5959,
            "range": "± 349.645",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.135,
            "range": "± 1.09073",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 31.3953,
            "range": "± 660.7",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 92.4151,
            "range": "± 977.394",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 27.302,
            "range": "± 2.88198",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 30.4015,
            "range": "± 3.11557",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 38.4444,
            "range": "± 5.58445",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 32.3672,
            "range": "± 3.22478",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 33.8934,
            "range": "± 3.98444",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 30.6825,
            "range": "± 3.06206",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 33.862,
            "range": "± 4.37537",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 29.3091,
            "range": "± 3.20164",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 31.2652,
            "range": "± 3.69143",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 29.7576,
            "range": "± 3.13466",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 61.9403,
            "range": "± 6.95064",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 206.949,
            "range": "± 12.8565",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 59.3937,
            "range": "± 6.62521",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 299.122,
            "range": "± 9.94886",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 510.915,
            "range": "± 29.1521",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46653,
            "range": "± 149.856",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.52001,
            "range": "± 495.199",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.79912,
            "range": "± 239.497",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.65929,
            "range": "± 500.298",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.06259,
            "range": "± 244.436",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.58474,
            "range": "± 255.958",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.21902,
            "range": "± 138.188",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.06849,
            "range": "± 224.4",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.5046,
            "range": "± 179.217",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.9366,
            "range": "± 1.07918",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 208.701,
            "range": "± 9.01677",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 30.3751,
            "range": "± 1.48792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 362.73,
            "range": "± 10.4216",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.32774,
            "range": "± 85.1773",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.9642,
            "range": "± 1.73733",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.7632,
            "range": "± 2.92618",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7741,
            "range": "± 1.49986",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.8912,
            "range": "± 3.09302",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.1477,
            "range": "± 1.30044",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.0497,
            "range": "± 1.79461",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.4559,
            "range": "± 889.656",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.2057,
            "range": "± 1.10465",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.83369,
            "range": "± 1.26586",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 59.1106,
            "range": "± 3.50443",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 4.45674,
            "range": "± 60.8487",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 133.95,
            "range": "± 5.63447",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 7.88013,
            "range": "± 116.031",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.45097,
            "range": "± 0.3308",
            "unit": "ns",
            "extra": "100 samples\n8461 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.78727,
            "range": "± 13.8419",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 534.493,
            "range": "± 22.4062",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.38125,
            "range": "± 0.360686",
            "unit": "ns",
            "extra": "100 samples\n6641 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.1472,
            "range": "± 8.77967",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8701,
            "range": "± 109.563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.1415,
            "range": "± 2.59343",
            "unit": "ns",
            "extra": "100 samples\n955 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 912.12,
            "range": "± 12.6851",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.272,
            "range": "± 1.18419",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 105.768,
            "range": "± 9.49353",
            "unit": "ns",
            "extra": "100 samples\n289 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 372.76,
            "range": "± 20.4643",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01204,
            "range": "± 105.064",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 383.666,
            "range": "± 18.7188",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 718.95,
            "range": "± 43.4072",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 423.064,
            "range": "± 18.6369",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 484.808,
            "range": "± 24.6584",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 368.286,
            "range": "± 29.0371",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 412.029,
            "range": "± 23.5556",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 360.522,
            "range": "± 36.5889",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
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
          "id": "a297a2cf9175e181121419b0ff80c9f0d482ba1a",
          "message": "Optimize TraceContext reference counting with incremental hashing\n\nReplace vector-based dynamic variable tracking with AliveVariableHash,\na specialized data structure that maintains reference counts and computes\nhash values incrementally using XOR operations.\n\nKey improvements:\n- O(1) hash updates instead of O(n) recomputation on each snapshot\n- Reduced memory footprint from dynamic vector to fixed 256KB array\n- Inline hash computation using golden ratio multiplier (no lookup table)\n- Reference count incorporated into hash for better state differentiation\n\nPerformance results (compared to original vector-based implementation):\n- trace_add: 42% faster (4.37μs → 2.52μs)\n- trace_ifInsideLoop: 4% faster (38.19μs → 36.48μs)\n- trace_loop: 20% faster (22.04μs → 17.66μs)\n- trace_nestedIf100: 11% faster (23.09ms → 20.43ms)\n- Most other benchmarks show 2-8% improvements\n\nImplementation uses XOR-based incremental hashing where each variable\ncontributes (id * HASH_MULTIPLIER) * count to the total hash. When\nreference counts change, we XOR out the old contribution and XOR in\nthe new one, maintaining the hash in O(1) time.\n\n🤖 Generated with [Claude Code](https://claude.com/claude-code)\n\nCo-Authored-By: Claude <noreply@anthropic.com>",
          "timestamp": "2025-12-13T00:15:45+07:00",
          "tree_id": "3a8987884aff4881dbedc966b71be5117803842b",
          "url": "https://github.com/nebulastream/nautilus/commit/a297a2cf9175e181121419b0ff80c9f0d482ba1a"
        },
        "date": 1765560157541,
        "tool": "catch2",
        "benches": [
          {
            "name": "ir_add",
            "value": 542.354,
            "range": "± 80.921",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.42725,
            "range": "± 272.689",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.191,
            "range": "± 697.178",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.85423,
            "range": "± 417.14",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.45407,
            "range": "± 622.094",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.02514,
            "range": "± 261.357",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.58846,
            "range": "± 527.15",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.27462,
            "range": "± 422.208",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.10253,
            "range": "± 470.731",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.56171,
            "range": "± 374.157",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 14.7449,
            "range": "± 1.78998",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 208.662,
            "range": "± 8.10926",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 27.5463,
            "range": "± 3.08976",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 370.857,
            "range": "± 38.8601",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 8.89176,
            "range": "± 2.14785",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 24.4535,
            "range": "± 4.63114",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 57.6284,
            "range": "± 8.90732",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 24.5965,
            "range": "± 4.51623",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.9981,
            "range": "± 8.43322",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 25.0579,
            "range": "± 5.17601",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 35.8993,
            "range": "± 8.91096",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 19.1356,
            "range": "± 4.15365",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 26.9198,
            "range": "± 4.47241",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 22.7414,
            "range": "± 3.46826",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 97.6121,
            "range": "± 12.889",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.63721,
            "range": "± 45.2893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 172.768,
            "range": "± 15.6232",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 5.04705,
            "range": "± 384.183",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.4086,
            "range": "± 979.952",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.96937,
            "range": "± 925.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.58827,
            "range": "± 209.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.3675,
            "range": "± 1.45378",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 33.1149,
            "range": "± 1.66055",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.4144,
            "range": "± 956.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.0141,
            "range": "± 1.01987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.57107,
            "range": "± 160.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.026,
            "range": "± 654.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1638,
            "range": "± 919.218",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.3154,
            "range": "± 536.086",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.0872,
            "range": "± 853.134",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.516,
            "range": "± 1.04993",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.4622,
            "range": "± 971.773",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.42294,
            "range": "± 0.115488",
            "unit": "ns",
            "extra": "100 samples\n8412 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.91764,
            "range": "± 598.525",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 616.199,
            "range": "± 42.5364",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.43085,
            "range": "± 0.517989",
            "unit": "ns",
            "extra": "100 samples\n6647 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.9408,
            "range": "± 7.01834",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.939,
            "range": "± 129.533",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.5874,
            "range": "± 3.42757",
            "unit": "ns",
            "extra": "100 samples\n960 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 912.015,
            "range": "± 5.96697",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 196.181,
            "range": "± 444.989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 115.587,
            "range": "± 30.1677",
            "unit": "ns",
            "extra": "100 samples\n270 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 400.325,
            "range": "± 57.6256",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.0231,
            "range": "± 109.912",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 420.587,
            "range": "± 47.8148",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 754.402,
            "range": "± 73.9338",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 453.033,
            "range": "± 53.9819",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 517.03,
            "range": "± 45.1608",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 384.179,
            "range": "± 49.5576",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 444.352,
            "range": "± 39.997",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 392.085,
            "range": "± 48.616",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
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
          "id": "4df2898ecd2b2f4691eb3af6e5068b00c3041538",
          "message": "Make TraceContext allocation-free with stack-allocated dependencies\n\nThis commit refactors TraceContext to eliminate all heap allocations during\ntracing, significantly improving performance by using thread-local storage\nand stack-allocated objects.\n\nKey changes:\n\n1. TraceContext Architecture:\n   - Changed from pointer-based to simple thread_local object\n   - Introduced TraceState struct to hold references to injected dependencies\n   - Separated injected state (TraceState) from persistent state (staticVars, aliveVars)\n   - TraceContext::get() returns nullptr when not initialized, otherwise returns &traceContext\n\n2. Stack Allocation:\n   - ExecutionTrace allocated on stack in trace() method\n   - SymbolicExecutionContext allocated on stack in trace() method\n   - Both objects injected as references via TraceState\n   - Final ExecutionTrace moved into unique_ptr for return\n\n3. Optimizations:\n   - AliveVariableHash::reset() optimized to skip fill when hash is already 0\n   - Direct access to local variables in trace() instead of indirect state access\n   - Incremental XOR-based hashing for alive variable tracking\n\n4. ExecutionTrace:\n   - Made move-only by deleting copy operations\n   - Fixes -Wdeprecated-copy-with-dtor warning\n   - Supports efficient move semantics for stack-to-heap transfer\n\n5. Documentation:\n   - Added comprehensive class-level documentation explaining design philosophy\n   - Documented lifecycle (initialization, trace iterations, cleanup)\n   - Added inline comments for member variables and key methods\n\nPerformance benefits:\n- Zero heap allocations for TraceContext and dependencies during tracing\n- Reduced memory fragmentation\n- Improved cache locality with stack allocations\n- Faster reset operations with optimized hash clearing\n\nAll tests pass (2967 assertions).",
          "timestamp": "2025-12-13T11:30:07+07:00",
          "tree_id": "389ec6b9cd35a68127c323b74bd2bbae36112b49",
          "url": "https://github.com/nebulastream/nautilus/commit/4df2898ecd2b2f4691eb3af6e5068b00c3041538"
        },
        "date": 1765600622347,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.5533,
            "range": "± 0.592677",
            "unit": "ns",
            "extra": "100 samples\n8466 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.891,
            "range": "± 513.84",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 564.844,
            "range": "± 25.7592",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45378,
            "range": "± 0.591741",
            "unit": "ns",
            "extra": "100 samples\n6293 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.6469,
            "range": "± 7.2952",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9405,
            "range": "± 149.987",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.5123,
            "range": "± 3.8846",
            "unit": "ns",
            "extra": "100 samples\n960 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 910.231,
            "range": "± 6.61657",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 195.939,
            "range": "± 2.84991",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.39116,
            "range": "± 182.302",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.0083,
            "range": "± 1.95835",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.8872,
            "range": "± 6.44792",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.7754,
            "range": "± 2.10299",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.4895,
            "range": "± 3.44448",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3089,
            "range": "± 2.81547",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.8452,
            "range": "± 3.24925",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.5673,
            "range": "± 1.36044",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.559,
            "range": "± 2.14139",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.71332,
            "range": "± 1.24299",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.7019,
            "range": "± 7.33201",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.36052,
            "range": "± 39.6371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 127.04,
            "range": "± 10.7527",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.62282,
            "range": "± 34.2967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 509.997,
            "range": "± 34.6875",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49274,
            "range": "± 197.788",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.64787,
            "range": "± 575.857",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.93485,
            "range": "± 264.049",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.54673,
            "range": "± 384.565",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.20947,
            "range": "± 310.119",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.79295,
            "range": "± 447.271",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.28181,
            "range": "± 255.812",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.32078,
            "range": "± 414.174",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.64277,
            "range": "± 239.476",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.9322,
            "range": "± 1.46778",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 198.773,
            "range": "± 10.8465",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.6746,
            "range": "± 2.14269",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 355.989,
            "range": "± 22.951",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.18462,
            "range": "± 314.097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.8436,
            "range": "± 392.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.64574,
            "range": "± 260.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.90053,
            "range": "± 303.296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 32.312,
            "range": "± 935.343",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.1125,
            "range": "± 747.32",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 31.6334,
            "range": "± 1.21459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.70313,
            "range": "± 258.853",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4382,
            "range": "± 421.069",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4764,
            "range": "± 433.763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.4249,
            "range": "± 455.748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.675,
            "range": "± 394.562",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.6606,
            "range": "± 541.984",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 23.3172,
            "range": "± 410.154",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.9819,
            "range": "± 757.366",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.6945,
            "range": "± 634.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.5775,
            "range": "± 697.106",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.9646,
            "range": "± 699.458",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.8923,
            "range": "± 652.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3181,
            "range": "± 641.983",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.1787,
            "range": "± 624.225",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.334,
            "range": "± 685.052",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.9343,
            "range": "± 663.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.7648,
            "range": "± 673.634",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.8437,
            "range": "± 664.32",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 64.2364,
            "range": "± 713.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 33.7163,
            "range": "± 621.825",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 94.4606,
            "range": "± 1.45581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 28.0768,
            "range": "± 3.20418",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 34.3111,
            "range": "± 6.11225",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 37.2863,
            "range": "± 4.79337",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 31.5858,
            "range": "± 3.81482",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 41.8808,
            "range": "± 8.03074",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 31.4033,
            "range": "± 3.27008",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 36.0851,
            "range": "± 4.80219",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 29.866,
            "range": "± 2.90053",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 33.7319,
            "range": "± 6.7846",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 31.8255,
            "range": "± 3.33986",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 53.8149,
            "range": "± 6.29702",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 213.84,
            "range": "± 11.5683",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 62.6764,
            "range": "± 7.21747",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 310.768,
            "range": "± 18.2008",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 130.601,
            "range": "± 15.5129",
            "unit": "ns",
            "extra": "100 samples\n234 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 409.061,
            "range": "± 44.3619",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.06456,
            "range": "± 120.283",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 443.273,
            "range": "± 66.551",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 784.275,
            "range": "± 96.3246",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 469.52,
            "range": "± 49.7593",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 585.62,
            "range": "± 83.2049",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 497.763,
            "range": "± 82.1799",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 457.496,
            "range": "± 49.7859",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 395.779,
            "range": "± 63.9258",
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
          "id": "29c739a7fc9ed681d7ea232ea0b3bc9a38575d13",
          "message": "Make bytecode register file size dynamic\n\nReplace fixed-size std::array with std::vector for RegisterFile to\navoid allocating large amounts of memory upfront. The register file\nis now dynamically sized based on actual register usage.\n\nChanges:\n- ByteCode.hpp: Change RegisterFile from std::array<int64_t, SHRT_MAX>\n  to std::vector<int64_t>\n- BCLoweringProvider.hpp: Add getRegisterCount() method to track\n  allocated registers and allocateRegister() helper to grow the\n  register file on demand\n- BCLoweringProvider.cpp: Call allocateRegister() before writing to\n  defaultRegisterFile in all constant operation handlers (ConstPtrOp,\n  ConstBooleanOp, ConstIntOp, ConstFloatOp) and processDynamicCall().\n  Resize RegisterFile to actual register count at the end of process()\n\nThis reduces memory overhead from ~256KB per RegisterFile instance\nto only the memory needed for registers actually used. The register\nfile grows incrementally as registers are allocated rather than being\npre-allocated to maximum size.",
          "timestamp": "2025-12-13T13:53:03+07:00",
          "tree_id": "28cae0196c3f0b2c59991cddddaed5bf4e020cc7",
          "url": "https://github.com/nebulastream/nautilus/commit/29c739a7fc9ed681d7ea232ea0b3bc9a38575d13"
        },
        "date": 1765609196707,
        "tool": "catch2",
        "benches": [
          {
            "name": "exec_mlir_add",
            "value": 3.50988,
            "range": "± 0.493822",
            "unit": "ns",
            "extra": "100 samples\n8439 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89506,
            "range": "± 574.086",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 573.773,
            "range": "± 33.3674",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.46983,
            "range": "± 0.680985",
            "unit": "ns",
            "extra": "100 samples\n6612 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.1518,
            "range": "± 8.72719",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9352,
            "range": "± 98.6877",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.9897,
            "range": "± 2.48658",
            "unit": "ns",
            "extra": "100 samples\n952 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 816.732,
            "range": "± 14.7901",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.933,
            "range": "± 863.661",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 507.321,
            "range": "± 39.9868",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48659,
            "range": "± 178.614",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.68634,
            "range": "± 643.043",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.84631,
            "range": "± 156.889",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.5703,
            "range": "± 371.304",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.26491,
            "range": "± 551.505",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.74025,
            "range": "± 345.048",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.30093,
            "range": "± 219.093",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.13338,
            "range": "± 215.589",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.61948,
            "range": "± 253.519",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 16.0599,
            "range": "± 1.08007",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 207.475,
            "range": "± 11.5557",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.9067,
            "range": "± 1.7304",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 362.908,
            "range": "± 17.197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.31462,
            "range": "± 93.3421",
            "unit": "us",
            "extra": "100 samples\n22 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6952,
            "range": "± 1.68076",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.3616,
            "range": "± 6.22993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.4879,
            "range": "± 1.62746",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.5329,
            "range": "± 2.93618",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.0114,
            "range": "± 1.97867",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.7265,
            "range": "± 3.04098",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 9.88992,
            "range": "± 1.28105",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.1127,
            "range": "± 2.04032",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.72748,
            "range": "± 1.25491",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 53.3985,
            "range": "± 6.68836",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.39477,
            "range": "± 30.0032",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 127.058,
            "range": "± 9.55093",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.6585,
            "range": "± 34.1397",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.993,
            "range": "± 5.39369",
            "unit": "ns",
            "extra": "100 samples\n247 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.759,
            "range": "± 23.3128",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04187,
            "range": "± 201.402",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 398.82,
            "range": "± 26.4378",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 714.25,
            "range": "± 40.9837",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 424.104,
            "range": "± 19.9467",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 526.415,
            "range": "± 41.0929",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 390.232,
            "range": "± 25.9931",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 426.631,
            "range": "± 24.6472",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 366.042,
            "range": "± 44.2037",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.23392,
            "range": "± 341.232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.00085,
            "range": "± 440.064",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.6468,
            "range": "± 137.852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.1406,
            "range": "± 360.968",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.6256,
            "range": "± 432.357",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6042,
            "range": "± 236.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.8576,
            "range": "± 519.733",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.58813,
            "range": "± 136.498",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1428,
            "range": "± 206.152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.2649,
            "range": "± 226.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.1447,
            "range": "± 258.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 27.2593,
            "range": "± 423.317",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 12.3585,
            "range": "± 284.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.7209,
            "range": "± 451.163",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.111,
            "range": "± 598.028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.0369,
            "range": "± 548.624",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.8202,
            "range": "± 524.203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.4441,
            "range": "± 603.356",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.4435,
            "range": "± 786.459",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.7531,
            "range": "± 635.572",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.6845,
            "range": "± 593.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.0855,
            "range": "± 566.414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.1892,
            "range": "± 529.322",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.104,
            "range": "± 626.863",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf10",
            "value": 30.1776,
            "range": "± 568.356",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_nestedIf100",
            "value": 63.8887,
            "range": "± 741.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf10",
            "value": 32.8075,
            "range": "± 609.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_chainedIf100",
            "value": 93.8484,
            "range": "± 1.08564",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 0.628124,
            "range": "± 0.0358696",
            "unit": "us",
            "extra": "100 samples\n48 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 1.92977,
            "range": "± 128.588",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.06616,
            "range": "± 527.231",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.04804,
            "range": "± 181.929",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 3.66754,
            "range": "± 272.69",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.13784,
            "range": "± 146.491",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 2.46131,
            "range": "± 156.264",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 1.62997,
            "range": "± 190.478",
            "unit": "us",
            "extra": "100 samples\n19 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.17242,
            "range": "± 185.694",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 1.89481,
            "range": "± 171.7",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_bc_nestedIf10",
            "value": 15.0583,
            "range": "± 1.0325",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "comp_bc_nestedIf100",
            "value": 162.631,
            "range": "± 7.62044",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_chainedIf10",
            "value": 24.8147,
            "range": "± 2.39294",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "comp_bc_chainedIf100",
            "value": 254.349,
            "range": "± 10.7583",
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
          "id": "d13024c18001f3b6a13e1674cc9da0c4bb5de031",
          "message": "benchmark.yml fix",
          "timestamp": "2025-12-13T15:35:56+07:00",
          "tree_id": "d581c97558ce8a3835ebff68f4ea532612404d8c",
          "url": "https://github.com/nebulastream/nautilus/commit/d13024c18001f3b6a13e1674cc9da0c4bb5de031"
        },
        "date": 1765615352586,
        "tool": "catch2",
        "benches": [
          {
            "name": "comp_mlir_add",
            "value": 7.92237,
            "range": "± 139.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.60221,
            "range": "± 232.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.48563,
            "range": "± 143.957",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.49698,
            "range": "± 159.026",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.939,
            "range": "± 554.008",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1865,
            "range": "± 227.558",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.9527,
            "range": "± 914.393",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.454,
            "range": "± 142.291",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8201,
            "range": "± 250.002",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.8397,
            "range": "± 264.497",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf10",
            "value": 13.0113,
            "range": "± 583.001",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_nestedIf100",
            "value": 26.3275,
            "range": "± 178.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf10",
            "value": 11.9045,
            "range": "± 392.792",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_chainedIf100",
            "value": 22.1905,
            "range": "± 408.418",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_add",
            "value": 1.31602,
            "range": "± 92.6565",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.8198,
            "range": "± 1.89298",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 32.5496,
            "range": "± 6.05406",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.4699,
            "range": "± 1.16637",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 20.7725,
            "range": "± 2.87503",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.1128,
            "range": "± 1.56698",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.8954,
            "range": "± 3.61361",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.4419,
            "range": "± 1.27915",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.1381,
            "range": "± 1.74505",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.72123,
            "range": "± 1.52595",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_nestedIf10",
            "value": 54.0524,
            "range": "± 5.60931",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_nestedIf100",
            "value": 1.40407,
            "range": "± 26.7505",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf10",
            "value": 126.5,
            "range": "± 5.92565",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_chainedIf100",
            "value": 4.63872,
            "range": "± 31.6102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_add",
            "value": 493.843,
            "range": "± 26.1907",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.38377,
            "range": "± 190.346",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.47281,
            "range": "± 611.645",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.81003,
            "range": "± 173.495",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.46113,
            "range": "± 373.845",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.03927,
            "range": "± 221.283",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.55169,
            "range": "± 232.033",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.18327,
            "range": "± 119.75",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.06209,
            "range": "± 222.229",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.54049,
            "range": "± 170.475",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_nestedIf10",
            "value": 15.6752,
            "range": "± 880.937",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ir_nestedIf100",
            "value": 197.136,
            "range": "± 6.34522",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ir_chainedIf10",
            "value": 29.1232,
            "range": "± 1.74098",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ir_chainedIf100",
            "value": 356.719,
            "range": "± 20.2511",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.537,
            "range": "± 4.16211",
            "unit": "ns",
            "extra": "100 samples\n250 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 404.043,
            "range": "± 32.912",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07839,
            "range": "± 193.041",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 393.74,
            "range": "± 17.8104",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 723.68,
            "range": "± 48.3",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 416.895,
            "range": "± 26.8855",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 496.908,
            "range": "± 7.95894",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 391.042,
            "range": "± 21.2797",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 426.771,
            "range": "± 25.4757",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 388.152,
            "range": "± 67.0646",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.53882,
            "range": "± 0.541727",
            "unit": "ns",
            "extra": "100 samples\n8520 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.88763,
            "range": "± 505.697",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 533.62,
            "range": "± 18.4412",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.43207,
            "range": "± 0.494594",
            "unit": "ns",
            "extra": "100 samples\n6680 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.5688,
            "range": "± 6.77956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9073,
            "range": "± 129.132",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.5751,
            "range": "± 2.20396",
            "unit": "ns",
            "extra": "100 samples\n978 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 824.502,
            "range": "± 17.2886",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 178.856,
            "range": "± 436.147",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      }
    ]
  }
}