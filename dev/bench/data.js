window.BENCHMARK_DATA = {
  "lastUpdate": 1724931629086,
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
          "id": "3c784030e6db45b043a8424571f5df55a5792365",
          "message": "support of ostream, stringstream, string, and string_view  (#28)\n\nThis change, extends the support for support of ostream, stringstream, string, and string_view in nautilus. \r\nIn general, we provide wrappers for common functions of the std lib.",
          "timestamp": "2024-08-22T23:11:37+02:00",
          "tree_id": "3bc450a6c4170f33fa02c18edf7661d18a49d2ef",
          "url": "https://github.com/nebulastream/nautilus/commit/3c784030e6db45b043a8424571f5df55a5792365"
        },
        "date": 1724361204634,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 3.40421,
            "range": "± 170.737",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 22.6681,
            "range": "± 1.45998",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 72.8315,
            "range": "± 3.65188",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 22.8982,
            "range": "± 2.97448",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 45.8778,
            "range": "± 4.23713",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 22.9863,
            "range": "± 1.54076",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 37.2941,
            "range": "± 3.47762",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 28.7237,
            "range": "± 2.34121",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 26.443,
            "range": "± 2.98187",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 20.8603,
            "range": "± 2.62793",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 112.598,
            "range": "± 5.30956",
            "unit": "ns",
            "extra": "100 samples\n269 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 405.879,
            "range": "± 19.7441",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00943,
            "range": "± 47.4668",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 429.843,
            "range": "± 14.4531",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 769.778,
            "range": "± 48.707",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 441.328,
            "range": "± 19.9703",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 556.916,
            "range": "± 42.8765",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 636.881,
            "range": "± 248.964",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 460.011,
            "range": "± 15.0128",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 383.605,
            "range": "± 19.8964",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ir_add",
            "value": 545.508,
            "range": "± 26.2191",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.45771,
            "range": "± 215.985",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.51639,
            "range": "± 637.51",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.8335,
            "range": "± 210.344",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.47982,
            "range": "± 606.028",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.52454,
            "range": "± 1.04461",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.68585,
            "range": "± 243.488",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.32045,
            "range": "± 102.641",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.03496,
            "range": "± 159.682",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.48159,
            "range": "± 163.502",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
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
          "id": "b040dca8c6759389ce6e3fbc964717cb318b2103",
          "message": "add compiler options to specific dumping of compilation steps",
          "timestamp": "2024-08-23T21:57:59+02:00",
          "tree_id": "784cf16834b122de7d84a173116d689317e2f9f0",
          "url": "https://github.com/nebulastream/nautilus/commit/b040dca8c6759389ce6e3fbc964717cb318b2103"
        },
        "date": 1724443204257,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 3.57967,
            "range": "± 364.928",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 23.0699,
            "range": "± 2.87506",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 75.1869,
            "range": "± 8.28993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 23.66,
            "range": "± 3.45761",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 48.721,
            "range": "± 10.1014",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 23.6179,
            "range": "± 3.58317",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 37.6676,
            "range": "± 7.0798",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 29.0969,
            "range": "± 3.03608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 27.44,
            "range": "± 4.14569",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 21.3455,
            "range": "± 3.20457",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 128.257,
            "range": "± 20.7444",
            "unit": "ns",
            "extra": "100 samples\n261 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 409.346,
            "range": "± 27.8844",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04125,
            "range": "± 92.5745",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 437.713,
            "range": "± 42.2229",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 772.636,
            "range": "± 72.1296",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 452.917,
            "range": "± 32.7669",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 553.453,
            "range": "± 50.1385",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 495.565,
            "range": "± 79.4329",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 467.158,
            "range": "± 71.1758",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 387.545,
            "range": "± 36.5392",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ir_add",
            "value": 549.229,
            "range": "± 36.7017",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.56879,
            "range": "± 276.242",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.78277,
            "range": "± 565.489",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.02502,
            "range": "± 411.231",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.81486,
            "range": "± 916.913",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.23825,
            "range": "± 381.546",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.91419,
            "range": "± 538.724",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.48374,
            "range": "± 201.45",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.14644,
            "range": "± 257.089",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.58908,
            "range": "± 266.121",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
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
          "id": "4654fd60e84a68fb29b96bcbe88ca2626550212f",
          "message": "Improvements for using nautilus as lib in nebulastream (#32)\n\nAuthored-by: Nils Schubert <nils.schubert@dima.tu-berlin.de>\r\n\r\n-Improved SSA debugging by providing an additional comment\r\n-Added missing abs and logf definitions\r\n- Increased size of varRefMap array.\r\n- Fixed pendantic warnings for extra ‘;’",
          "timestamp": "2024-08-29T13:36:44+02:00",
          "tree_id": "0bddc01d632c821e82976b34ef5b441a5987eaef",
          "url": "https://github.com/nebulastream/nautilus/commit/4654fd60e84a68fb29b96bcbe88ca2626550212f"
        },
        "date": 1724931628154,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 41.5821,
            "range": "± 3.13002",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 176.808,
            "range": "± 6.1769",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 457.791,
            "range": "± 7.49197",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 176.126,
            "range": "± 6.13558",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 291.083,
            "range": "± 6.17762",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 163.626,
            "range": "± 6.59144",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 318.608,
            "range": "± 7.93369",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 428.904,
            "range": "± 5.47422",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 218.704,
            "range": "± 7.53977",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 148.377,
            "range": "± 6.1518",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 112.404,
            "range": "± 6.76991",
            "unit": "ns",
            "extra": "100 samples\n270 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 439.567,
            "range": "± 47.2016",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.20562,
            "range": "± 60.8549",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 473.557,
            "range": "± 27.8732",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 879.878,
            "range": "± 52.4589",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 468.249,
            "range": "± 21.2585",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 611.676,
            "range": "± 40.385",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 539.198,
            "range": "± 32.3818",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 495.008,
            "range": "± 41.3947",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 383.841,
            "range": "± 23.4749",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 528.862,
            "range": "± 48.0407",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.81333,
            "range": "± 203.608",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.2789,
            "range": "± 468.934",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.0383,
            "range": "± 381.427",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.73187,
            "range": "± 407.4",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.21509,
            "range": "± 259.176",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.20492,
            "range": "± 285.767",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.65079,
            "range": "± 152.196",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.23835,
            "range": "± 304.332",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.65642,
            "range": "± 209.886",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          }
        ]
      }
    ]
  }
}