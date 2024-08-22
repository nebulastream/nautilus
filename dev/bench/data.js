window.BENCHMARK_DATA = {
  "lastUpdate": 1724360265114,
  "repoUrl": "https://github.com/nebulastream/nautilus",
  "entries": {
    "Catch2 Benchmark (v3)": [
      {
        "commit": {
          "author": {
            "name": "nebulastream",
            "username": "nebulastream"
          },
          "committer": {
            "name": "nebulastream",
            "username": "nebulastream"
          },
          "id": "6e914c07233b20d60ce938f49f0dc44d8f620b0d",
          "message": "Benchmark",
          "timestamp": "2024-08-20T21:53:19Z",
          "url": "https://github.com/nebulastream/nautilus/pull/30/commits/6e914c07233b20d60ce938f49f0dc44d8f620b0d"
        },
        "date": 1724357400554,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 3.42672,
            "range": "± 317.866",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 32.4503,
            "range": "± 6.13884",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 29.1133,
            "range": "± 3.37523",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 110.57,
            "range": "± 9.08514",
            "unit": "ns",
            "extra": "100 samples\n283 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 508.06,
            "range": "± 27.5767",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 475.469,
            "range": "± 44.3412",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ir_add",
            "value": 548.466,
            "range": "± 49.4358",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 3.39231,
            "range": "± 338.372",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.15444,
            "range": "± 289.966",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "name": "nebulastream",
            "username": "nebulastream"
          },
          "committer": {
            "name": "nebulastream",
            "username": "nebulastream"
          },
          "id": "07cf4fc346f3063f315c432551c42b1f820d4fed",
          "message": "Benchmark",
          "timestamp": "2024-08-20T21:53:19Z",
          "url": "https://github.com/nebulastream/nautilus/pull/30/commits/07cf4fc346f3063f315c432551c42b1f820d4fed"
        },
        "date": 1724358837351,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 3.38835,
            "range": "± 313.303",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 22.6936,
            "range": "± 2.85419",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 73.0605,
            "range": "± 6.92037",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 22.7521,
            "range": "± 3.7162",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 46.6913,
            "range": "± 8.18857",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 23.0841,
            "range": "± 3.15792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 37.5503,
            "range": "± 6.99361",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 28.2425,
            "range": "± 2.4066",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 26.3168,
            "range": "± 2.83766",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 20.7188,
            "range": "± 2.68027",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 117.398,
            "range": "± 5.26195",
            "unit": "ns",
            "extra": "100 samples\n260 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 407.588,
            "range": "± 24.4582",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.02133,
            "range": "± 51.5247",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 416.882,
            "range": "± 23.5171",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 753.928,
            "range": "± 49.3254",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 431.205,
            "range": "± 27.8583",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 686.644,
            "range": "± 533.669",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 486.388,
            "range": "± 46.5435",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 440.508,
            "range": "± 21.8969",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 370.876,
            "range": "± 23.8796",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "ir_add",
            "value": 521.605,
            "range": "± 23.2996",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.4509,
            "range": "± 177.247",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.53224,
            "range": "± 499.428",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.85214,
            "range": "± 237.02",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.57112,
            "range": "± 356.997",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.09294,
            "range": "± 215.531",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.67158,
            "range": "± 257.741",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.32527,
            "range": "± 157.033",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.10668,
            "range": "± 257.465",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.47827,
            "range": "± 73.637",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          }
        ]
      }
    ],
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
          "id": "f3b17428bf6954c74ead59cb93779608ffb52263",
          "message": "Add initial support for benchmarks in the ci\n\nBenchmarks run on the main branch and are uploaded to https://nebulastream.github.io/nautilus/dev/bench/",
          "timestamp": "2024-08-22T22:54:06+02:00",
          "tree_id": "5c047bbe0104774fbbc9f00fe4e2f18d4a280a15",
          "url": "https://github.com/nebulastream/nautilus/commit/f3b17428bf6954c74ead59cb93779608ffb52263"
        },
        "date": 1724360264650,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 3.49964,
            "range": "± 389.371",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 22.9815,
            "range": "± 2.79128",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 73.0748,
            "range": "± 6.37502",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 22.9237,
            "range": "± 2.80379",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 46.0446,
            "range": "± 6.04706",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 23.4218,
            "range": "± 3.12512",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 37.2199,
            "range": "± 6.78845",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 28.939,
            "range": "± 2.38478",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 26.8919,
            "range": "± 3.12485",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 21.011,
            "range": "± 3.44836",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 112.979,
            "range": "± 5.56395",
            "unit": "ns",
            "extra": "100 samples\n269 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 413.757,
            "range": "± 19.2721",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.02669,
            "range": "± 100.33",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 430.089,
            "range": "± 19.9501",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 765.4,
            "range": "± 45.7667",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 458.281,
            "range": "± 40.1438",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 564.335,
            "range": "± 49.7058",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 517.731,
            "range": "± 28.6636",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 456.474,
            "range": "± 25.5872",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 387.178,
            "range": "± 27.6966",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ir_add",
            "value": 552.307,
            "range": "± 42.444",
            "unit": "ns",
            "extra": "100 samples\n55 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46371,
            "range": "± 168.346",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.59329,
            "range": "± 562.009",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.85051,
            "range": "± 200.921",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.54805,
            "range": "± 578.042",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.17732,
            "range": "± 310.075",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.74844,
            "range": "± 247.822",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.40698,
            "range": "± 261.088",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.0905,
            "range": "± 266.928",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.52811,
            "range": "± 216.228",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          }
        ]
      }
    ]
  }
}