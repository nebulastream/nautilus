window.BENCHMARK_DATA = {
  "lastUpdate": 1765560158122,
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
      },
      {
        "commit": {
          "author": {
            "email": "lukas.schwerdtfeger@gmail.com",
            "name": "Lukas Schwerdtfeger",
            "username": "ls-1801"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "f26ad2f271f4a151e9473b2fffcc7d95f4c2f079",
          "message": "improve linking to MLIR (#35)\n\nThis change moves the configuration of the MLIR dependencies into the MLIR source directory, and reduces the number of MLIR dependencies to include only the necessary libraries.",
          "timestamp": "2024-08-30T15:13:51+02:00",
          "tree_id": "8f4939f3218fffa2a6a8f4c13a0b83091d14871c",
          "url": "https://github.com/nebulastream/nautilus/commit/f26ad2f271f4a151e9473b2fffcc7d95f4c2f079"
        },
        "date": 1725023691495,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 41.5921,
            "range": "± 2.94761",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 177.013,
            "range": "± 6.25344",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 458.27,
            "range": "± 7.64426",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 176.225,
            "range": "± 5.97198",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 366.676,
            "range": "± 76.6252",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 164.078,
            "range": "± 6.94165",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 321.613,
            "range": "± 11.3232",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 430.703,
            "range": "± 6.76154",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 217.98,
            "range": "± 7.73801",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 148.213,
            "range": "± 5.93536",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.389,
            "range": "± 7.52722",
            "unit": "ns",
            "extra": "100 samples\n266 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 406.929,
            "range": "± 19.6179",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10586,
            "range": "± 80.9125",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 444.869,
            "range": "± 29.2832",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 777.749,
            "range": "± 40.3099",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 424.003,
            "range": "± 20.6829",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 572.409,
            "range": "± 32.5627",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 528.997,
            "range": "± 38.9992",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 454.58,
            "range": "± 31.9459",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 364.422,
            "range": "± 18.6807",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ir_add",
            "value": 516.056,
            "range": "± 24.3772",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49765,
            "range": "± 206.651",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.79927,
            "range": "± 549.348",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.11803,
            "range": "± 225.946",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.19661,
            "range": "± 803.993",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.41467,
            "range": "± 957.241",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 4.37331,
            "range": "± 370.847",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.84399,
            "range": "± 163.571",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.40372,
            "range": "± 201.166",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.58379,
            "range": "± 189.998",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "73c9028ab8e87c1e4fd4a2efe9f1d8c6befa9e1b",
          "message": "remove usage of SCFDialect (#37)",
          "timestamp": "2024-09-12T22:35:19+02:00",
          "tree_id": "6b7ee41e79a2ac7e496be38208110c6eaf3ca27b",
          "url": "https://github.com/nebulastream/nautilus/commit/73c9028ab8e87c1e4fd4a2efe9f1d8c6befa9e1b"
        },
        "date": 1726173545447,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 41.5483,
            "range": "± 2.76924",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 176.138,
            "range": "± 6.48986",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 456.172,
            "range": "± 6.78089",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 176.587,
            "range": "± 6.43571",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 289.159,
            "range": "± 7.59089",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 163.12,
            "range": "± 7.41213",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 317.837,
            "range": "± 8.20426",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 428.725,
            "range": "± 5.01577",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 218.956,
            "range": "± 8.5994",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 147.553,
            "range": "± 6.76242",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.632,
            "range": "± 6.21847",
            "unit": "ns",
            "extra": "100 samples\n268 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 400.908,
            "range": "± 20.6302",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.10556,
            "range": "± 79.0358",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 411.999,
            "range": "± 28.9091",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 753.049,
            "range": "± 38.2421",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 442.073,
            "range": "± 41.6122",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 536.378,
            "range": "± 28.7747",
            "unit": "ns",
            "extra": "100 samples\n53 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 498.529,
            "range": "± 29.5907",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 439.01,
            "range": "± 48.599",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 360.324,
            "range": "± 18.8395",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 542.511,
            "range": "± 53.18",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.50742,
            "range": "± 189.118",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.63033,
            "range": "± 386.424",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.90564,
            "range": "± 313.766",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 7.10475,
            "range": "± 2.21043",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.20524,
            "range": "± 311.205",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.77375,
            "range": "± 256.002",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.32796,
            "range": "± 140.663",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.10559,
            "range": "± 299.661",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.5193,
            "range": "± 84.8596",
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
          "id": "c4acd69163b6b02efbab391c943da896f77b9478",
          "message": "cleanup llvm compilation pass (#38)",
          "timestamp": "2024-09-12T23:15:27+02:00",
          "tree_id": "08b114421156cc57295796262706e1f2fa263638",
          "url": "https://github.com/nebulastream/nautilus/commit/c4acd69163b6b02efbab391c943da896f77b9478"
        },
        "date": 1726175797474,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 41.6558,
            "range": "± 3.32294",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 176.571,
            "range": "± 6.36227",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 458.63,
            "range": "± 7.56689",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 176.195,
            "range": "± 5.76093",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 290.276,
            "range": "± 8.14769",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 164.551,
            "range": "± 8.5609",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 317.864,
            "range": "± 7.96049",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 428.347,
            "range": "± 7.11009",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 218.692,
            "range": "± 8.6638",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 148.113,
            "range": "± 6.3131",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 112.238,
            "range": "± 8.35955",
            "unit": "ns",
            "extra": "100 samples\n270 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 417.089,
            "range": "± 42.8756",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.1515,
            "range": "± 219.613",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 437.384,
            "range": "± 32.8936",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 851.076,
            "range": "± 50.4366",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 480.525,
            "range": "± 41.7022",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 608.16,
            "range": "± 38.9545",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 529.524,
            "range": "± 28.6106",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 480.416,
            "range": "± 31.1331",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 374.841,
            "range": "± 23.4772",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "ir_add",
            "value": 538.548,
            "range": "± 56.8312",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.59109,
            "range": "± 174.65",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 7.0317,
            "range": "± 535.832",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.85901,
            "range": "± 205.443",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.7235,
            "range": "± 504.344",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.09086,
            "range": "± 276.361",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.94167,
            "range": "± 400.233",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.45647,
            "range": "± 168.204",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.07774,
            "range": "± 222.53",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.50344,
            "range": "± 161.715",
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
          "id": "1256f1c8d83d392aef0fd9ef58c1d1e51f603115",
          "message": "Additional tracing tests (#34)",
          "timestamp": "2024-09-13T00:27:41+02:00",
          "tree_id": "50e4e0d11245293d062c4779319e4e8e97489acb",
          "url": "https://github.com/nebulastream/nautilus/commit/1256f1c8d83d392aef0fd9ef58c1d1e51f603115"
        },
        "date": 1726180132424,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 41.6031,
            "range": "± 2.95708",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 176.854,
            "range": "± 6.00054",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 458.457,
            "range": "± 7.66146",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 176.487,
            "range": "± 6.03241",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 291.559,
            "range": "± 7.71692",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 164.821,
            "range": "± 7.85812",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 318.086,
            "range": "± 8.21652",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 428.826,
            "range": "± 5.43647",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 217.48,
            "range": "± 6.88698",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 148.027,
            "range": "± 6.112",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_add",
            "value": 109.621,
            "range": "± 7.5231",
            "unit": "ns",
            "extra": "100 samples\n280 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 432.192,
            "range": "± 29.5398",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.11917,
            "range": "± 82.6672",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 448.856,
            "range": "± 24.3735",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 815.737,
            "range": "± 43.5277",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 434.082,
            "range": "± 32.2721",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 684.899,
            "range": "± 189.626",
            "unit": "ns",
            "extra": "100 samples\n57 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 499.727,
            "range": "± 36.3652",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 478.645,
            "range": "± 23.3008",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 399.081,
            "range": "± 63.375",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ir_add",
            "value": 518.578,
            "range": "± 33.925",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.56967,
            "range": "± 309.163",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.82653,
            "range": "± 510.791",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.08805,
            "range": "± 327.801",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.74353,
            "range": "± 386.633",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.19781,
            "range": "± 136.714",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.85945,
            "range": "± 288.118",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.36629,
            "range": "± 110.236",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.18342,
            "range": "± 374.481",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.53352,
            "range": "± 233.863",
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
          "id": "92c44dc97e1e778bcd30daf3f5fae41e86124298",
          "message": "cleanup dynamic values (#40)\n\nWe now use a dynamic vector to keep track of currently in flight val references",
          "timestamp": "2024-09-13T09:38:05+02:00",
          "tree_id": "723fe612096a48d005b5aa34ae628a34d3b9e093",
          "url": "https://github.com/nebulastream/nautilus/commit/92c44dc97e1e778bcd30daf3f5fae41e86124298"
        },
        "date": 1726213187880,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.56353,
            "range": "± 194.67",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 19.3829,
            "range": "± 2.27606",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 63.678,
            "range": "± 4.7415",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 19.5105,
            "range": "± 2.98472",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.4525,
            "range": "± 5.45308",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 20.0749,
            "range": "± 3.17523",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 29.9912,
            "range": "± 2.94939",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 20.4256,
            "range": "± 2.63546",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.9667,
            "range": "± 2.51925",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 18.0265,
            "range": "± 2.87829",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.089,
            "range": "± 5.16515",
            "unit": "ns",
            "extra": "100 samples\n261 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 402.598,
            "range": "± 15.8956",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01477,
            "range": "± 64.1994",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 426.18,
            "range": "± 39.1264",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 766.235,
            "range": "± 48.0019",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 429.921,
            "range": "± 25.8564",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 554.199,
            "range": "± 57.7594",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 461.477,
            "range": "± 28.6418",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 459.868,
            "range": "± 32.1618",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 389.235,
            "range": "± 24.3131",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ir_add",
            "value": 539.785,
            "range": "± 47.6213",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.48477,
            "range": "± 207.863",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.77964,
            "range": "± 477.005",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.86646,
            "range": "± 203.835",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.60356,
            "range": "± 671.549",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.20635,
            "range": "± 300.538",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.76436,
            "range": "± 422.41",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.42868,
            "range": "± 176.479",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.11204,
            "range": "± 183.432",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.50933,
            "range": "± 155.211",
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
          "id": "ae705b60dbcf7a6aee85955cf065964cdb449267",
          "message": "add additional compiler warning and cleanup code (#42)",
          "timestamp": "2024-09-14T09:59:42+02:00",
          "tree_id": "87f25b6118929bb1e119213f4b99cd65c06c9993",
          "url": "https://github.com/nebulastream/nautilus/commit/ae705b60dbcf7a6aee85955cf065964cdb449267"
        },
        "date": 1726300994899,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.46794,
            "range": "± 137.096",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 18.7379,
            "range": "± 1.90036",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 62.7661,
            "range": "± 4.16581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 20.2348,
            "range": "± 10.1066",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 39.3588,
            "range": "± 3.05513",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.5175,
            "range": "± 1.91615",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 34.0532,
            "range": "± 3.64561",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 19.1667,
            "range": "± 1.5885",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.585,
            "range": "± 2.43778",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.6932,
            "range": "± 2.74921",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 109.684,
            "range": "± 4.78727",
            "unit": "ns",
            "extra": "100 samples\n270 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 408.026,
            "range": "± 25.2654",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.09148,
            "range": "± 77.462",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 421.821,
            "range": "± 24.4068",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 797.819,
            "range": "± 70.9097",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 453.938,
            "range": "± 25.6848",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 548.068,
            "range": "± 34.549",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 443.888,
            "range": "± 21.6072",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 486.69,
            "range": "± 32.7197",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 372.101,
            "range": "± 20.9845",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ir_add",
            "value": 521.271,
            "range": "± 25.3716",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.50361,
            "range": "± 157.529",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.61518,
            "range": "± 521.919",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.8691,
            "range": "± 208.575",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.48111,
            "range": "± 276.083",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.26815,
            "range": "± 241.951",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.67967,
            "range": "± 240.266",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.38393,
            "range": "± 145.14",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.07633,
            "range": "± 182.239",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.50479,
            "range": "± 144.138",
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
          "id": "cca60489cc146bb1c6f67ba1ce59798a057cc727",
          "message": "Fix wrong mangled symbol (#43)\n\nCo-authored-by: lukas schwerdtfeger <lukas.schwerdtfeger@gmail.com>",
          "timestamp": "2024-09-15T09:24:55+02:00",
          "tree_id": "3fb61911d677f74ae5e1739bf409e38c27024d85",
          "url": "https://github.com/nebulastream/nautilus/commit/cca60489cc146bb1c6f67ba1ce59798a057cc727"
        },
        "date": 1726385214615,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.38803,
            "range": "± 217.985",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 18.8244,
            "range": "± 2.24144",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 63.2821,
            "range": "± 6.92292",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 19.2046,
            "range": "± 2.711",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.9117,
            "range": "± 5.95914",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.2761,
            "range": "± 2.99425",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 35.5197,
            "range": "± 5.3888",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 18.0942,
            "range": "± 2.19598",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.7317,
            "range": "± 3.10008",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.9317,
            "range": "± 2.81313",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 111.804,
            "range": "± 7.84505",
            "unit": "ns",
            "extra": "100 samples\n276 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 410.708,
            "range": "± 50.9196",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04986,
            "range": "± 44.1043",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 419.802,
            "range": "± 47.5976",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 781.958,
            "range": "± 48.8814",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 446.545,
            "range": "± 25.0701",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 551.857,
            "range": "± 23.5111",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 427.246,
            "range": "± 22.2226",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 439.574,
            "range": "± 21.9623",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 368.947,
            "range": "± 14.0386",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ir_add",
            "value": 531.126,
            "range": "± 27.4254",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49591,
            "range": "± 166.692",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.59093,
            "range": "± 563.559",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.83817,
            "range": "± 174.716",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.55833,
            "range": "± 596.17",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.08697,
            "range": "± 131.199",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.70212,
            "range": "± 253.941",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.39392,
            "range": "± 162.453",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.16565,
            "range": "± 278.42",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.51236,
            "range": "± 148.688",
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
          "id": "5f86ec562502ceaa4f5ad73366d79629f35b228f",
          "message": "Constrain binary operators (#45)",
          "timestamp": "2024-09-16T21:57:01+02:00",
          "tree_id": "2adaa37eff4d5950ce7f89dbdab545fccc6b0079",
          "url": "https://github.com/nebulastream/nautilus/commit/5f86ec562502ceaa4f5ad73366d79629f35b228f"
        },
        "date": 1726516725957,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.4875,
            "range": "± 148.486",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 19.0949,
            "range": "± 2.25707",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 63.0588,
            "range": "± 4",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 19.0263,
            "range": "± 1.99975",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.3217,
            "range": "± 4.09297",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.6465,
            "range": "± 1.90113",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 34.8874,
            "range": "± 3.83993",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 17.9783,
            "range": "± 1.41736",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.8575,
            "range": "± 2.20194",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.7711,
            "range": "± 2.83016",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 109.777,
            "range": "± 5.94575",
            "unit": "ns",
            "extra": "100 samples\n277 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 407.746,
            "range": "± 45.007",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07188,
            "range": "± 94.884",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 422.872,
            "range": "± 27.0013",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 786.585,
            "range": "± 44.0081",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 472.913,
            "range": "± 48.5905",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 574.397,
            "range": "± 92.7091",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 435.865,
            "range": "± 43.2576",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 441.186,
            "range": "± 23.129",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 386.802,
            "range": "± 35.6249",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ir_add",
            "value": 546.214,
            "range": "± 38.5478",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.46751,
            "range": "± 160.9",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.73493,
            "range": "± 785.141",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 3.70575,
            "range": "± 2.4326",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.42715,
            "range": "± 359.816",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.14373,
            "range": "± 208.988",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.71593,
            "range": "± 283.007",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.37295,
            "range": "± 133.752",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.1595,
            "range": "± 263.935",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.86753,
            "range": "± 806.204",
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
          "id": "cef3add4ffad36ac042f6dd99a504a0a4b281bc9",
          "message": "Added std::hex, std::doc, and std::oct (#46)\n\nCo-authored-by: Nils Schubert <nils.schubert@dima.tu-berlin.de>",
          "timestamp": "2024-09-18T17:10:40+02:00",
          "tree_id": "118711cf8c9dd57caa058889432d043b0476073c",
          "url": "https://github.com/nebulastream/nautilus/commit/cef3add4ffad36ac042f6dd99a504a0a4b281bc9"
        },
        "date": 1726672322001,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.43266,
            "range": "± 215.337",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 33.1381,
            "range": "± 7.91127",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 63.5001,
            "range": "± 7.42406",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 18.9767,
            "range": "± 2.89024",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.1526,
            "range": "± 6.35663",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.327,
            "range": "± 3.10197",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 34.4159,
            "range": "± 6.23569",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 18.9123,
            "range": "± 2.21772",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.7348,
            "range": "± 3.45049",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.6827,
            "range": "± 2.83491",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 108.204,
            "range": "± 4.88666",
            "unit": "ns",
            "extra": "100 samples\n275 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 393.269,
            "range": "± 28.6882",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.03166,
            "range": "± 62.2256",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 404.516,
            "range": "± 23.7185",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 764.096,
            "range": "± 39.8726",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 437.038,
            "range": "± 21.5956",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 545.636,
            "range": "± 37.1487",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 427.381,
            "range": "± 50.1944",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 434.963,
            "range": "± 43.8083",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 373.956,
            "range": "± 21.1676",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ir_add",
            "value": 558.832,
            "range": "± 34.5806",
            "unit": "ns",
            "extra": "100 samples\n54 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.54791,
            "range": "± 158.123",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.59712,
            "range": "± 346.399",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.91118,
            "range": "± 151.188",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.54281,
            "range": "± 326.753",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.17705,
            "range": "± 208.638",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.81227,
            "range": "± 291.573",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.37151,
            "range": "± 85.9211",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.1754,
            "range": "± 236.759",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.57567,
            "range": "± 122.882",
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
          "id": "9b6d21233dc4c4d54fd4c440d01b8120513774d9",
          "message": "cleanup tracing util and remove unnecessary templates. (#44)",
          "timestamp": "2024-09-19T19:05:47+02:00",
          "tree_id": "b052a105159c491c8b2761dfb7908880a3f4057c",
          "url": "https://github.com/nebulastream/nautilus/commit/9b6d21233dc4c4d54fd4c440d01b8120513774d9"
        },
        "date": 1726765686990,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.28439,
            "range": "± 162.179",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 17.8339,
            "range": "± 2.37159",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 60.4815,
            "range": "± 4.6972",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 18.0184,
            "range": "± 2.63917",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 37.9981,
            "range": "± 5.55695",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 18.4511,
            "range": "± 3.18907",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 27.8071,
            "range": "± 3.44883",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 17.1967,
            "range": "± 2.72302",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 20.883,
            "range": "± 3.9763",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.185,
            "range": "± 2.97967",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 100.39,
            "range": "± 4.29025",
            "unit": "ns",
            "extra": "100 samples\n298 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 397.597,
            "range": "± 53.8043",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.13457,
            "range": "± 345.545",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 392.071,
            "range": "± 19.9068",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 722.04,
            "range": "± 33.6156",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 425.525,
            "range": "± 23.0453",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 549.033,
            "range": "± 77.5791",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 450.001,
            "range": "± 36.4483",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 431.127,
            "range": "± 23.2233",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 359.517,
            "range": "± 20.5139",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 497.34,
            "range": "± 24.9072",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.31043,
            "range": "± 177.738",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.86581,
            "range": "± 378.939",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.648,
            "range": "± 168.707",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.1263,
            "range": "± 413.606",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.96647,
            "range": "± 315.301",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.419,
            "range": "± 300.37",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.11016,
            "range": "± 208.748",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.85799,
            "range": "± 263.261",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.37841,
            "range": "± 126.657",
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
          "id": "e38e016e67f698ca8ac9ec15df2aaa71ca8c6b29",
          "message": "Backend benchmark fixup (#51)",
          "timestamp": "2024-09-27T03:14:27+02:00",
          "tree_id": "442c29edc634d08dd0c3290afca1e1c4f83a59ac",
          "url": "https://github.com/nebulastream/nautilus/commit/e38e016e67f698ca8ac9ec15df2aaa71ca8c6b29"
        },
        "date": 1727399800882,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.28722,
            "range": "± 146.94",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 18.3182,
            "range": "± 1.76235",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 61.9502,
            "range": "± 6.88869",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 19.0327,
            "range": "± 2.94399",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 39.3323,
            "range": "± 7.19692",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.6002,
            "range": "± 3.28597",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 33.8102,
            "range": "± 4.73769",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 18.5131,
            "range": "± 2.69849",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.5188,
            "range": "± 3.55522",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.0004,
            "range": "± 2.51685",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 112.423,
            "range": "± 16.2336",
            "unit": "ns",
            "extra": "100 samples\n272 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 399.755,
            "range": "± 50.4564",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01421,
            "range": "± 116.795",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 421.469,
            "range": "± 41.0361",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 771.561,
            "range": "± 90.3942",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 447.255,
            "range": "± 36.4376",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 538.501,
            "range": "± 50.9896",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 440.899,
            "range": "± 69.1414",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 451.848,
            "range": "± 42.8056",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 366.866,
            "range": "± 37.1495",
            "unit": "ns",
            "extra": "100 samples\n89 iterations"
          },
          {
            "name": "ir_add",
            "value": 563.631,
            "range": "± 80.9507",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.38616,
            "range": "± 285.026",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.13905,
            "range": "± 744.566",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.70375,
            "range": "± 204.938",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.35075,
            "range": "± 750.82",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.92983,
            "range": "± 219.817",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.49673,
            "range": "± 392.979",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25613,
            "range": "± 337.704",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.90471,
            "range": "± 243.193",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.49104,
            "range": "± 285.185",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "backend_compilation_mlir_add",
            "value": 8.43766,
            "range": "± 353.403",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "backend_compilation_mlir_ifThenElse",
            "value": 8.86367,
            "range": "± 131.523",
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
          "id": "e38e016e67f698ca8ac9ec15df2aaa71ca8c6b29",
          "message": "Backend benchmark fixup (#51)",
          "timestamp": "2024-09-27T03:14:27+02:00",
          "tree_id": "442c29edc634d08dd0c3290afca1e1c4f83a59ac",
          "url": "https://github.com/nebulastream/nautilus/commit/e38e016e67f698ca8ac9ec15df2aaa71ca8c6b29"
        },
        "date": 1727400599127,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.56112,
            "range": "± 404.191",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 18.4801,
            "range": "± 1.89157",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 62.2579,
            "range": "± 7.06295",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 19.9484,
            "range": "± 4.64144",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 39.9638,
            "range": "± 6.3743",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.6693,
            "range": "± 2.96657",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 28.534,
            "range": "± 3.43556",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 18.4761,
            "range": "± 2.6692",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 20.9053,
            "range": "± 2.16135",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.7914,
            "range": "± 1.90207",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.316,
            "range": "± 16.2024",
            "unit": "ns",
            "extra": "100 samples\n292 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 411.987,
            "range": "± 54.8465",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.02218,
            "range": "± 132.9",
            "unit": "us",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 431.427,
            "range": "± 63.1328",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 780.809,
            "range": "± 95.5437",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 448.812,
            "range": "± 40.7957",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 523.807,
            "range": "± 36.6105",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 454.872,
            "range": "± 93.2544",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 430.482,
            "range": "± 37.6731",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 398.788,
            "range": "± 78.0507",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ir_add",
            "value": 557.818,
            "range": "± 72.3829",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.37289,
            "range": "± 246.073",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.12699,
            "range": "± 595.64",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.6704,
            "range": "± 214.377",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.33787,
            "range": "± 618.197",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.10801,
            "range": "± 340.123",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.6792,
            "range": "± 629.441",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.28472,
            "range": "± 365.035",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.03581,
            "range": "± 411.647",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.58244,
            "range": "± 363.407",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "backend_compilation_mlir_add",
            "value": 8.17891,
            "range": "± 122.434",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "backend_compilation_mlir_ifThenElse",
            "value": 8.82218,
            "range": "± 181.656",
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
          "id": "93d23ebf419110b698a7d567b386c313b0caef75",
          "message": "add compilation time benchmarks (#52)",
          "timestamp": "2024-09-27T03:49:03+02:00",
          "tree_id": "94ef4d8b6efcab59e2a9d255da2a2d382ac60918",
          "url": "https://github.com/nebulastream/nautilus/commit/93d23ebf419110b698a7d567b386c313b0caef75"
        },
        "date": 1727401865287,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.28546,
            "range": "± 264.131",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 17.8565,
            "range": "± 3.04747",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 60.096,
            "range": "± 5.66528",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 17.9595,
            "range": "± 2.7627",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 38.3158,
            "range": "± 6.73444",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 18.8788,
            "range": "± 3.49885",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 31.2038,
            "range": "± 6.17167",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 17.3173,
            "range": "± 2.23758",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 20.5679,
            "range": "± 3.13222",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 16.9439,
            "range": "± 2.5492",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 105.188,
            "range": "± 6.17313",
            "unit": "ns",
            "extra": "100 samples\n289 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 376.107,
            "range": "± 28.6033",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 962.713,
            "range": "± 102.85",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 384.478,
            "range": "± 25.7039",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 709.654,
            "range": "± 37.6015",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 408.111,
            "range": "± 11.2969",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 501.791,
            "range": "± 34.6869",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 476.465,
            "range": "± 21.9874",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 404.731,
            "range": "± 16.7858",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 333.665,
            "range": "± 17.468",
            "unit": "ns",
            "extra": "100 samples\n91 iterations"
          },
          {
            "name": "ir_add",
            "value": 520.091,
            "range": "± 29.7586",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.3207,
            "range": "± 125.22",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.02062,
            "range": "± 497.761",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.68577,
            "range": "± 175.519",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.26765,
            "range": "± 398.336",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.01697,
            "range": "± 195.109",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.45801,
            "range": "± 217.135",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.08125,
            "range": "± 110.652",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.92331,
            "range": "± 214.876",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.36581,
            "range": "± 128.312",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.51674,
            "range": "± 203.746",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.1634,
            "range": "± 172.675",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.79427,
            "range": "± 160.003",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.71085,
            "range": "± 153.688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.4321,
            "range": "± 851.481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.1539,
            "range": "± 321.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.2195,
            "range": "± 419.261",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.6375,
            "range": "± 255.781",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9726,
            "range": "± 1.26359",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3784,
            "range": "± 323.551",
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
          "id": "5e6f8fe291e08253c74008ce3280158df4e7da38",
          "message": "Decouple fmt and spdlog dependencies  (#49)",
          "timestamp": "2024-09-30T19:15:42+02:00",
          "tree_id": "8235ad666fe9743663ee2225c9ebd48e2ed2620d",
          "url": "https://github.com/nebulastream/nautilus/commit/5e6f8fe291e08253c74008ce3280158df4e7da38"
        },
        "date": 1727716695501,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.46216,
            "range": "± 283.3",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 19.0496,
            "range": "± 3.62978",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 63.5557,
            "range": "± 7.18924",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 18.9352,
            "range": "± 3.13409",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.9695,
            "range": "± 6.63471",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.8351,
            "range": "± 3.45777",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 35.0154,
            "range": "± 6.06972",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 19.4296,
            "range": "± 3.18823",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.6766,
            "range": "± 3.51608",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.9833,
            "range": "± 2.98245",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.009,
            "range": "± 13.0713",
            "unit": "ns",
            "extra": "100 samples\n287 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 419.887,
            "range": "± 44.0281",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.08123,
            "range": "± 115.435",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 456.296,
            "range": "± 49.3093",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 801.849,
            "range": "± 72.7299",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 469.328,
            "range": "± 37.9618",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.668,
            "range": "± 56.2058",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 449.625,
            "range": "± 71.5311",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 495.481,
            "range": "± 51.7025",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 398.213,
            "range": "± 29.0952",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ir_add",
            "value": 555.19,
            "range": "± 77.3257",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.49239,
            "range": "± 350.321",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.4379,
            "range": "± 587.799",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.79325,
            "range": "± 219.734",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.66721,
            "range": "± 739.982",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.1761,
            "range": "± 410.216",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.54001,
            "range": "± 229.405",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.58894,
            "range": "± 693.807",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.98865,
            "range": "± 275.504",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.47534,
            "range": "± 169.839",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.09595,
            "range": "± 44.4236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.74941,
            "range": "± 191.329",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.68992,
            "range": "± 114.566",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.73723,
            "range": "± 156.788",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.1441,
            "range": "± 374.63",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4519,
            "range": "± 204.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.1754,
            "range": "± 283.062",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.75506,
            "range": "± 171.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1009,
            "range": "± 272.341",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.444,
            "range": "± 265.933",
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
          "id": "3c8cd651958646da0c602b71a2d7841093baefdf",
          "message": "move disable_target_warnings(fmt)",
          "timestamp": "2024-09-30T22:08:09+02:00",
          "tree_id": "4208e06b80f930d00141268625e98ae0336238ba",
          "url": "https://github.com/nebulastream/nautilus/commit/3c8cd651958646da0c602b71a2d7841093baefdf"
        },
        "date": 1727727014530,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.37041,
            "range": "± 219.286",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 18.5387,
            "range": "± 2.87568",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 63.6423,
            "range": "± 7.16644",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 18.6305,
            "range": "± 3.21806",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.0214,
            "range": "± 6.51584",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 19.4906,
            "range": "± 3.20443",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 33.6864,
            "range": "± 6.61096",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 18.9866,
            "range": "± 2.79915",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 22.4484,
            "range": "± 4.53684",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.6092,
            "range": "± 2.99092",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 108.54,
            "range": "± 8.83744",
            "unit": "ns",
            "extra": "100 samples\n286 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 401.951,
            "range": "± 33.7847",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04341,
            "range": "± 65.511",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 432.564,
            "range": "± 16.4331",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 836.451,
            "range": "± 150.808",
            "unit": "ns",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 467.496,
            "range": "± 57.3559",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 542.957,
            "range": "± 38.0363",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 431.545,
            "range": "± 51.7608",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 475.275,
            "range": "± 42.2922",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 387.657,
            "range": "± 35.2742",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ir_add",
            "value": 519.64,
            "range": "± 35.2065",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.39553,
            "range": "± 230.854",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.26018,
            "range": "± 484.892",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.79678,
            "range": "± 249.695",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.28291,
            "range": "± 548.299",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.09513,
            "range": "± 273.471",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.59184,
            "range": "± 364.303",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.2034,
            "range": "± 178.598",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.97447,
            "range": "± 340.82",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.48009,
            "range": "± 171.774",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.22773,
            "range": "± 127.626",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.85445,
            "range": "± 181.683",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.78317,
            "range": "± 133.874",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.2606,
            "range": "± 599.63",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.538,
            "range": "± 679.736",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6232,
            "range": "± 359.162",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.2677,
            "range": "± 432.689",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.70028,
            "range": "± 130.822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8503,
            "range": "± 205.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.6581,
            "range": "± 323.301",
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
          "id": "8f55da95d2dc7a09aa5e7b2c78ee87b40a1e5747",
          "message": "fix backend benchmark",
          "timestamp": "2024-09-30T22:31:03+02:00",
          "tree_id": "31c0f08fdd5aa9cd7cddcd121836a6080c9e14c9",
          "url": "https://github.com/nebulastream/nautilus/commit/8f55da95d2dc7a09aa5e7b2c78ee87b40a1e5747"
        },
        "date": 1727728389366,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.30937,
            "range": "± 140.251",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 18.2666,
            "range": "± 1.58921",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 61.6693,
            "range": "± 3.22144",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 18.3891,
            "range": "± 1.49983",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 40.5381,
            "range": "± 5.38006",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 18.9118,
            "range": "± 1.83481",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 28.0082,
            "range": "± 1.97963",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 17.4282,
            "range": "± 1.61344",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 21.2835,
            "range": "± 2.37178",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.3083,
            "range": "± 1.88538",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 124.634,
            "range": "± 49.6732",
            "unit": "ns",
            "extra": "100 samples\n287 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 387.186,
            "range": "± 16.7864",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01573,
            "range": "± 100.532",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 404.124,
            "range": "± 32.8732",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 751.226,
            "range": "± 65.4418",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 434.341,
            "range": "± 13.4775",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 527.473,
            "range": "± 28.2123",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 415.63,
            "range": "± 32.0575",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 439.057,
            "range": "± 33.4219",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 359.649,
            "range": "± 18.1946",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "ir_add",
            "value": 512.919,
            "range": "± 25.258",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.28933,
            "range": "± 97.4716",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.2056,
            "range": "± 357.892",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.68431,
            "range": "± 105.619",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.22913,
            "range": "± 274.435",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.96414,
            "range": "± 219.529",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.496,
            "range": "± 264.775",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.09175,
            "range": "± 109.992",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.93009,
            "range": "± 710.892",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.37334,
            "range": "± 85.6929",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.26484,
            "range": "± 162.027",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.87483,
            "range": "± 113.05",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.773,
            "range": "± 64.4571",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.82729,
            "range": "± 95.5349",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.9485,
            "range": "± 610.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4563,
            "range": "± 109.253",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.0739,
            "range": "± 171.24",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.68828,
            "range": "± 78.8256",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8954,
            "range": "± 211.043",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4019,
            "range": "± 103.914",
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
          "id": "cc7e9e733d83aae6ec7d1efb0203dbd4f3102d0b",
          "message": "fix backend benchmark",
          "timestamp": "2024-09-30T22:44:33+02:00",
          "tree_id": "00e69219e972682e5d60dcc344a7e2af81d93d63",
          "url": "https://github.com/nebulastream/nautilus/commit/cc7e9e733d83aae6ec7d1efb0203dbd4f3102d0b"
        },
        "date": 1727729207690,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 2.44982,
            "range": "± 234.402",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 19.274,
            "range": "± 3.19253",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 64.1814,
            "range": "± 7.85563",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 19.0921,
            "range": "± 2.7401",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 39.7163,
            "range": "± 5.56034",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 20.0006,
            "range": "± 1.95481",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 32.5857,
            "range": "± 4.32291",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 18.5569,
            "range": "± 2.77269",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 33.8648,
            "range": "± 7.15888",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 17.7415,
            "range": "± 2.2726",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 113.316,
            "range": "± 10.4971",
            "unit": "ns",
            "extra": "100 samples\n261 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 388.044,
            "range": "± 22.2915",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 964.361,
            "range": "± 55.4427",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 399.772,
            "range": "± 33.804",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 733.129,
            "range": "± 107.963",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 429.377,
            "range": "± 44.4851",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 527.24,
            "range": "± 42.6688",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 500.704,
            "range": "± 34.9745",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 420.154,
            "range": "± 12.3693",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 348.576,
            "range": "± 26.7279",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 504.863,
            "range": "± 23.1987",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.75468,
            "range": "± 873.834",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.05626,
            "range": "± 357.793",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.69832,
            "range": "± 178.646",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.1745,
            "range": "± 329.164",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.95883,
            "range": "± 217.423",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.47301,
            "range": "± 279.169",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.05228,
            "range": "± 86.091",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.89488,
            "range": "± 173.791",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.52744,
            "range": "± 319.702",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.18672,
            "range": "± 95.84",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.8152,
            "range": "± 269.416",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.724,
            "range": "± 132.648",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.78401,
            "range": "± 114.255",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.6194,
            "range": "± 164.332",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3562,
            "range": "± 109.517",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.8985,
            "range": "± 230.576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.67777,
            "range": "± 52.4359",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.7843,
            "range": "± 114.339",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3008,
            "range": "± 155.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.1589,
            "range": "± 353.609",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.8798,
            "range": "± 360.422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.9333,
            "range": "± 370.7",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.9762,
            "range": "± 330.915",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.0183,
            "range": "± 287.545",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.4234,
            "range": "± 496.781",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.6023,
            "range": "± 561.422",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.693,
            "range": "± 375.469",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.1288,
            "range": "± 315.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.829,
            "range": "± 362.847",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 979.017,
            "range": "± 60.5319",
            "unit": "ns",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.36153,
            "range": "± 205.049",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.83635,
            "range": "± 486.293",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.40731,
            "range": "± 56.1816",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.28146,
            "range": "± 292.162",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.59953,
            "range": "± 196.359",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.06857,
            "range": "± 322.791",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.12151,
            "range": "± 138.267",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.57208,
            "range": "± 158.912",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.29222,
            "range": "± 156.192",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "8b78e3b5b97c22cc412c6d236ef44c039cbd59bf",
          "message": "Use fmt to format trace and ir (#48)",
          "timestamp": "2024-10-04T12:58:39+02:00",
          "tree_id": "c0335483da89f0b413f607582c0512fbff378202",
          "url": "https://github.com/nebulastream/nautilus/commit/8b78e3b5b97c22cc412c6d236ef44c039cbd59bf"
        },
        "date": 1728039735149,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.86384,
            "range": "± 183.435",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 16.5572,
            "range": "± 1.77035",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 56.8461,
            "range": "± 6.18729",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 17.043,
            "range": "± 2.74066",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 35.9271,
            "range": "± 5.21807",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 17.5821,
            "range": "± 2.53622",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 25.9187,
            "range": "± 3.5924",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 16.6999,
            "range": "± 2.06676",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 19.3941,
            "range": "± 2.59447",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 16.012,
            "range": "± 2.39962",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 112.652,
            "range": "± 16.4375",
            "unit": "ns",
            "extra": "100 samples\n282 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.132,
            "range": "± 38.3316",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 998.198,
            "range": "± 103.576",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 426.899,
            "range": "± 42.1936",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 737.956,
            "range": "± 37.6216",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 436.673,
            "range": "± 28.43",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 559.99,
            "range": "± 69.1999",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 491.859,
            "range": "± 41.6406",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 469.731,
            "range": "± 51.1116",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 386.91,
            "range": "± 63.3784",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ir_add",
            "value": 504.624,
            "range": "± 41.6366",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.69791,
            "range": "± 849.81",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.82854,
            "range": "± 576.663",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.62727,
            "range": "± 217.754",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.02101,
            "range": "± 487.147",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.85602,
            "range": "± 239.674",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.2317,
            "range": "± 249.84",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.07971,
            "range": "± 225.491",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.87395,
            "range": "± 321.043",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.3218,
            "range": "± 167.682",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.1585,
            "range": "± 117.293",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.79894,
            "range": "± 112.443",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.03041,
            "range": "± 213.518",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.2055,
            "range": "± 241.011",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.3837,
            "range": "± 615.619",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5673,
            "range": "± 472.906",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.1834,
            "range": "± 240.769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.63617,
            "range": "± 133.404",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0083,
            "range": "± 181.071",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3378,
            "range": "± 161.064",
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
          "id": "c8b754a33ac3e635eea931c9b08c27539c54a10a",
          "message": "Added support for enum class  (#47)\n\nThis change adds support for enum classes as part nautilus vals\r\nSigned-off-by: Nils Schubert <nilslpschubert@gmail.com>\r\nCo-authored-by: Philipp Grulich <philippgrulich@hotmail.de>",
          "timestamp": "2024-10-04T15:50:38+02:00",
          "tree_id": "880a44b59fe6045c13daed39658f726f32bcf571",
          "url": "https://github.com/nebulastream/nautilus/commit/c8b754a33ac3e635eea931c9b08c27539c54a10a"
        },
        "date": 1728050053598,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.93878,
            "range": "± 261.162",
            "unit": "us",
            "extra": "100 samples\n17 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 17.6822,
            "range": "± 2.99831",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 61.3225,
            "range": "± 14.4135",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 17.9614,
            "range": "± 3.41721",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 37.2803,
            "range": "± 7.0961",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 18.1419,
            "range": "± 3.60957",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 30.8462,
            "range": "± 4.53954",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 17.5881,
            "range": "± 2.50623",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 19.747,
            "range": "± 2.99661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 15.9805,
            "range": "± 2.82146",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 108.814,
            "range": "± 12.2372",
            "unit": "ns",
            "extra": "100 samples\n300 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 595.69,
            "range": "± 227.889",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04215,
            "range": "± 104.418",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 423.548,
            "range": "± 59.232",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 761.341,
            "range": "± 80.0659",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 479.629,
            "range": "± 47.9552",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 557.937,
            "range": "± 49.8082",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 489.251,
            "range": "± 83.9216",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 462.645,
            "range": "± 33.9297",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 381.019,
            "range": "± 57.0983",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 529.55,
            "range": "± 90.2656",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.31834,
            "range": "± 300.794",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.97306,
            "range": "± 695.968",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.62706,
            "range": "± 276.736",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.16516,
            "range": "± 759.836",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.9416,
            "range": "± 388.411",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.28553,
            "range": "± 301.308",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.08005,
            "range": "± 246.588",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.87764,
            "range": "± 368.739",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.56395,
            "range": "± 537.12",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.87324,
            "range": "± 788.338",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.56556,
            "range": "± 744.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.33881,
            "range": "± 722.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 11.1953,
            "range": "± 839.288",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 26.0702,
            "range": "± 1.03561",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.8865,
            "range": "± 874.405",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 26.651,
            "range": "± 1.44594",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.57214,
            "range": "± 881.581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.132,
            "range": "± 1.00216",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.5734,
            "range": "± 919.511",
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
          "id": "93880c8882c045147deacc5af5028211c93e6785",
          "message": "add missing enum trac data",
          "timestamp": "2024-10-05T14:07:53+02:00",
          "tree_id": "57a807a554357a8e9bab73361faa6e1986bfe2c4",
          "url": "https://github.com/nebulastream/nautilus/commit/93880c8882c045147deacc5af5028211c93e6785"
        },
        "date": 1728130202161,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.90198,
            "range": "± 218.844",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 16.5592,
            "range": "± 2.19121",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 57.5165,
            "range": "± 6.72106",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 16.8273,
            "range": "± 2.35487",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 35.5148,
            "range": "± 5.38011",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 17.795,
            "range": "± 3.80423",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 30.324,
            "range": "± 2.70632",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 16.4107,
            "range": "± 1.98785",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 19.8559,
            "range": "± 3.44315",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 15.9433,
            "range": "± 2.50323",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "ssa_add",
            "value": 111.154,
            "range": "± 16.8409",
            "unit": "ns",
            "extra": "100 samples\n303 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 404.155,
            "range": "± 44.4644",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.04047,
            "range": "± 90.4718",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 423.41,
            "range": "± 39.045",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 777.545,
            "range": "± 88.9642",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 449.249,
            "range": "± 40.0605",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 543.032,
            "range": "± 54.518",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 450.836,
            "range": "± 31.3316",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 458.502,
            "range": "± 52.4863",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 375.714,
            "range": "± 39.111",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ir_add",
            "value": 502.654,
            "range": "± 46.8823",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.28967,
            "range": "± 194.768",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.87093,
            "range": "± 505.601",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.66697,
            "range": "± 278.95",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.99981,
            "range": "± 560.986",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.81658,
            "range": "± 213.188",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.36901,
            "range": "± 357.112",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.12242,
            "range": "± 250.503",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.84554,
            "range": "± 216.551",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.38774,
            "range": "± 193.056",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.24404,
            "range": "± 134.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.87422,
            "range": "± 142.656",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.8164,
            "range": "± 120.006",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.9443,
            "range": "± 264.936",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.9401,
            "range": "± 320.581",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.61,
            "range": "± 369.47",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.2036,
            "range": "± 281.769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.67808,
            "range": "± 82.2815",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8658,
            "range": "± 147.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5324,
            "range": "± 339.57",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.4157,
            "range": "± 383.328",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.9583,
            "range": "± 338.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.0488,
            "range": "± 360.025",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.2395,
            "range": "± 478.741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.2007,
            "range": "± 401.27",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.7694,
            "range": "± 612.67",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.8438,
            "range": "± 446.061",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.7812,
            "range": "± 359.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.3793,
            "range": "± 476.633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.1483,
            "range": "± 553.548",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.02622,
            "range": "± 67.1443",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.42821,
            "range": "± 221.373",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.75053,
            "range": "± 471.58",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.47066,
            "range": "± 182.215",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.37951,
            "range": "± 436.195",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.67261,
            "range": "± 249.319",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.03294,
            "range": "± 303.354",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.13042,
            "range": "± 205.824",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.69144,
            "range": "± 326.413",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.45432,
            "range": "± 346.416",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "faf97b89612db3d1eb001488ee70b0e2a6196410",
          "message": "Make spdlog optional (#54)\n\nset -DENABLE_LOGGING=OFF to disable the dependency on spdlog",
          "timestamp": "2024-10-05T17:17:21+02:00",
          "tree_id": "9a4049ab776bd11c4806db5103f06657422f57da",
          "url": "https://github.com/nebulastream/nautilus/commit/faf97b89612db3d1eb001488ee70b0e2a6196410"
        },
        "date": 1728141582848,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.0513,
            "range": "± 95.0345",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.2234,
            "range": "± 2.25857",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.2361,
            "range": "± 5.39463",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.4747,
            "range": "± 2.39754",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.5959,
            "range": "± 2.71192",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.4917,
            "range": "± 2.02601",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 19.2855,
            "range": "± 2.30753",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.0047,
            "range": "± 1.18108",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.0973,
            "range": "± 1.24271",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.85357,
            "range": "± 1.09807",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 102.164,
            "range": "± 4.92748",
            "unit": "ns",
            "extra": "100 samples\n301 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 400.608,
            "range": "± 45.3485",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01881,
            "range": "± 106.203",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 415.359,
            "range": "± 25.7599",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 769.252,
            "range": "± 79.0542",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 451.848,
            "range": "± 43.2646",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 544.759,
            "range": "± 52.6137",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 446.179,
            "range": "± 64.9426",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 444.711,
            "range": "± 34.6794",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 375.773,
            "range": "± 36.7164",
            "unit": "ns",
            "extra": "100 samples\n89 iterations"
          },
          {
            "name": "ir_add",
            "value": 507.835,
            "range": "± 55.2529",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.32375,
            "range": "± 226.46",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.92825,
            "range": "± 696.391",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.64825,
            "range": "± 249.958",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.01889,
            "range": "± 400.003",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.88167,
            "range": "± 223.936",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.28548,
            "range": "± 239.742",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.06867,
            "range": "± 169.64",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.81883,
            "range": "± 202.278",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.37422,
            "range": "± 236.038",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.51218,
            "range": "± 181.299",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.16503,
            "range": "± 141.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.82378,
            "range": "± 167.153",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.87366,
            "range": "± 135.13",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.1315,
            "range": "± 545.293",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6527,
            "range": "± 325.198",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.2753,
            "range": "± 193.13",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.73309,
            "range": "± 78.2811",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0361,
            "range": "± 182.455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5092,
            "range": "± 160.697",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.26,
            "range": "± 616.665",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.9683,
            "range": "± 534.863",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.0313,
            "range": "± 501.491",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.1343,
            "range": "± 507.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.0338,
            "range": "± 295.569",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.3677,
            "range": "± 378.104",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.4777,
            "range": "± 321.793",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.929,
            "range": "± 669.395",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.0874,
            "range": "± 356.481",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.8205,
            "range": "± 335.298",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.07501,
            "range": "± 58.9626",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.56169,
            "range": "± 249.83",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.06724,
            "range": "± 354.374",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.64255,
            "range": "± 202.008",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.63588,
            "range": "± 405.126",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.82889,
            "range": "± 233.488",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.18026,
            "range": "± 211.645",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.16289,
            "range": "± 159.231",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.80993,
            "range": "± 214.178",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.53019,
            "range": "± 143.362",
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
          "id": "2191088107b9b98ca7ba9fbc426680e239a53c1e",
          "message": "Refactor trace context (#55)",
          "timestamp": "2024-10-05T19:20:46+02:00",
          "tree_id": "32c57c56b1404bc20fded5121b7174d0b687e35d",
          "url": "https://github.com/nebulastream/nautilus/commit/2191088107b9b98ca7ba9fbc426680e239a53c1e"
        },
        "date": 1728148911318,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.2898,
            "range": "± 5.04427",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 979.719,
            "range": "± 47.1935",
            "unit": "ns",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.84769,
            "range": "± 314.033",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.10667,
            "range": "± 854.87",
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
          "id": "c5f2bbf28b13f8e946872b1fa4b08ea2f1c0b18a",
          "message": "enable benchmarks again",
          "timestamp": "2024-10-06T17:46:11+02:00",
          "tree_id": "70768348e1f37782233b7e454adb2c54f17c6768",
          "url": "https://github.com/nebulastream/nautilus/commit/c5f2bbf28b13f8e946872b1fa4b08ea2f1c0b18a"
        },
        "date": 1728229708495,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.14148,
            "range": "± 193.268",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.6307,
            "range": "± 1.95004",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.21,
            "range": "± 2.53806",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.4342,
            "range": "± 1.73435",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.1987,
            "range": "± 3.01661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.3451,
            "range": "± 1.93041",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.5403,
            "range": "± 3.26134",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.1938,
            "range": "± 1.59514",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.5559,
            "range": "± 1.28766",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.49296,
            "range": "± 2.10491",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 122.175,
            "range": "± 22.4511",
            "unit": "ns",
            "extra": "100 samples\n276 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 430.474,
            "range": "± 61.918",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.08345,
            "range": "± 172.624",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 446.941,
            "range": "± 66.7536",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 784.24,
            "range": "± 103.194",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 478.04,
            "range": "± 63.8362",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 572.393,
            "range": "± 78.454",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 487.066,
            "range": "± 85.1321",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 475.861,
            "range": "± 74.2701",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 400.951,
            "range": "± 61.3654",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 531.839,
            "range": "± 77.2363",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.34058,
            "range": "± 327.674",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.98863,
            "range": "± 745.069",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.81877,
            "range": "± 450.444",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.11809,
            "range": "± 690.986",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.9596,
            "range": "± 368.594",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.42487,
            "range": "± 503.921",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.17426,
            "range": "± 355.179",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.88667,
            "range": "± 306.254",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.39659,
            "range": "± 285.989",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.27779,
            "range": "± 247.515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.38269,
            "range": "± 966.568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.83851,
            "range": "± 201.199",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.90808,
            "range": "± 225.749",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 25.1972,
            "range": "± 525.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.2496,
            "range": "± 360.09",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 25.247,
            "range": "± 402.862",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.9697,
            "range": "± 224.124",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 14.0811,
            "range": "± 229.193",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.4937,
            "range": "± 254.097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 30.4224,
            "range": "± 437.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 31.5333,
            "range": "± 507.264",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 32.5874,
            "range": "± 561.926",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 31.3786,
            "range": "± 435.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 32.1127,
            "range": "± 447.175",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 31.1104,
            "range": "± 432.966",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 31.2227,
            "range": "± 535.049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 30.2908,
            "range": "± 618.68",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 31.0048,
            "range": "± 413.878",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.9418,
            "range": "± 386.461",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.04494,
            "range": "± 107.987",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.47212,
            "range": "± 367.798",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.94744,
            "range": "± 784.637",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.5136,
            "range": "± 297.991",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.31985,
            "range": "± 510.904",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.69167,
            "range": "± 400.804",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 2.98318,
            "range": "± 322.856",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.10499,
            "range": "± 291.077",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.74384,
            "range": "± 447.277",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.37153,
            "range": "± 236.299",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "7adef5c0714d4bce934e211d7e6887ddff1da8ce",
          "message": "simplify trace operations (#56)",
          "timestamp": "2024-10-06T21:27:19+02:00",
          "tree_id": "4a4f0b7c5311e9bde8391adf80d397281bb0de80",
          "url": "https://github.com/nebulastream/nautilus/commit/7adef5c0714d4bce934e211d7e6887ddff1da8ce"
        },
        "date": 1728243044618,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.1418,
            "range": "± 98.4766",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.2528,
            "range": "± 1.35758",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 38.0533,
            "range": "± 4.4372",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.1406,
            "range": "± 1.36049",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 24.1963,
            "range": "± 2.54348",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.8887,
            "range": "± 2.22321",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.9719,
            "range": "± 2.97541",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.157,
            "range": "± 1.20858",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.7385,
            "range": "± 2.09972",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.4384,
            "range": "± 1.4987",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 110.361,
            "range": "± 5.72064",
            "unit": "ns",
            "extra": "100 samples\n274 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 366.534,
            "range": "± 19.2801",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 960,
            "range": "± 39.9448",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 392.852,
            "range": "± 30.1794",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 717.714,
            "range": "± 79.8408",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 386.694,
            "range": "± 15.1297",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 481.231,
            "range": "± 26.0078",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 367.242,
            "range": "± 20.3284",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 409.003,
            "range": "± 32.3281",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 355.817,
            "range": "± 22.0503",
            "unit": "ns",
            "extra": "100 samples\n90 iterations"
          },
          {
            "name": "ir_add",
            "value": 497.175,
            "range": "± 45.485",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.24275,
            "range": "± 199.583",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.95081,
            "range": "± 794.443",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.58193,
            "range": "± 171.439",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.90752,
            "range": "± 284.885",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.86258,
            "range": "± 270.691",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.28466,
            "range": "± 187.637",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.0384,
            "range": "± 124.719",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.76696,
            "range": "± 195.164",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.30836,
            "range": "± 144.186",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.44228,
            "range": "± 678.861",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.01701,
            "range": "± 193.88",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.8007,
            "range": "± 148.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.99865,
            "range": "± 369.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.4469,
            "range": "± 683.17",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.2533,
            "range": "± 432.595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.1895,
            "range": "± 232.174",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.75098,
            "range": "± 147.308",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8941,
            "range": "± 194.821",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.6399,
            "range": "± 421.835",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.4658,
            "range": "± 559.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.2127,
            "range": "± 1.326",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.1286,
            "range": "± 393.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4735,
            "range": "± 523.555",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.1607,
            "range": "± 726.694",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.8926,
            "range": "± 463.466",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.4644,
            "range": "± 573.893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.9351,
            "range": "± 348.2",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.9125,
            "range": "± 531.227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.9382,
            "range": "± 609.826",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 992.74,
            "range": "± 55.0793",
            "unit": "ns",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.5541,
            "range": "± 215.616",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.00971,
            "range": "± 496.665",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.51595,
            "range": "± 265.984",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.58599,
            "range": "± 413.299",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.60078,
            "range": "± 194.73",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.08812,
            "range": "± 424.352",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.09658,
            "range": "± 127.16",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.73439,
            "range": "± 230.098",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36412,
            "range": "± 192.387",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "1ff420420162fa0c0904bbe664424402ededc31d",
          "message": "small refactoring for mlir backend (#57)",
          "timestamp": "2024-10-08T13:18:34+02:00",
          "tree_id": "bbb74f71d54ca5249330176b0be24dd7bd1049f0",
          "url": "https://github.com/nebulastream/nautilus/commit/1ff420420162fa0c0904bbe664424402ededc31d"
        },
        "date": 1728386459762,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.19811,
            "range": "± 109.293",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.191,
            "range": "± 1.33622",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 38.1825,
            "range": "± 4.84923",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.0459,
            "range": "± 1.38947",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 24.4852,
            "range": "± 2.66792",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.5835,
            "range": "± 1.31395",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.849,
            "range": "± 2.46229",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.3791,
            "range": "± 1.06848",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.7921,
            "range": "± 1.75014",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.583,
            "range": "± 1.90359",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 113.611,
            "range": "± 20.5224",
            "unit": "ns",
            "extra": "100 samples\n279 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 384.267,
            "range": "± 23.223",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00309,
            "range": "± 49.2222",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 391.245,
            "range": "± 22.9378",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 723.12,
            "range": "± 37.4075",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 418.746,
            "range": "± 24.1125",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 526.898,
            "range": "± 43.5291",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 394.978,
            "range": "± 30.7623",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 438.1,
            "range": "± 35.5456",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 356.602,
            "range": "± 16.5009",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ir_add",
            "value": 488.979,
            "range": "± 27.784",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.23623,
            "range": "± 138.198",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.79249,
            "range": "± 320.484",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.579,
            "range": "± 200.65",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.92014,
            "range": "± 277.503",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.85422,
            "range": "± 275.628",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.28999,
            "range": "± 246.793",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.07127,
            "range": "± 184.543",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.80017,
            "range": "± 173.091",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.27461,
            "range": "± 81.0443",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.26963,
            "range": "± 128.125",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.8756,
            "range": "± 133.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.98537,
            "range": "± 202.86",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.93618,
            "range": "± 208.376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.2028,
            "range": "± 470.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7813,
            "range": "± 379.852",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.744,
            "range": "± 413.142",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.85619,
            "range": "± 155.758",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9081,
            "range": "± 259.082",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4723,
            "range": "± 235.688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.8341,
            "range": "± 745.415",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.6054,
            "range": "± 685.695",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.6844,
            "range": "± 588.275",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.4928,
            "range": "± 425.933",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.604,
            "range": "± 466.402",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.8727,
            "range": "± 470.101",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.1389,
            "range": "± 556.674",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.2153,
            "range": "± 607.116",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.3823,
            "range": "± 3.48854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.0263,
            "range": "± 557.094",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.003,
            "range": "± 38.9586",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.5231,
            "range": "± 213.875",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.06086,
            "range": "± 579.256",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.51515,
            "range": "± 217.418",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.57745,
            "range": "± 419.861",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.61307,
            "range": "± 205.849",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.05505,
            "range": "± 265.179",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.07241,
            "range": "± 120.197",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.69774,
            "range": "± 245.467",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.33146,
            "range": "± 146.969",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "0d38c259822c9722428ced0cf867b0be69a6b7b1",
          "message": "Hide addresses in tracing test  (#58)",
          "timestamp": "2024-10-08T13:32:13+02:00",
          "tree_id": "81d270f16242ca30577d0d27a0bcd433e1b7f75c",
          "url": "https://github.com/nebulastream/nautilus/commit/0d38c259822c9722428ced0cf867b0be69a6b7b1"
        },
        "date": 1728387262129,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.11061,
            "range": "± 48.8404",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.2051,
            "range": "± 1.1804",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 37.4327,
            "range": "± 3.06672",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.0209,
            "range": "± 898.432",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.9416,
            "range": "± 1.87909",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.5478,
            "range": "± 980.796",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.5532,
            "range": "± 1.54516",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.1142,
            "range": "± 711.066",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.5965,
            "range": "± 1.96865",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.2638,
            "range": "± 817.547",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 107.041,
            "range": "± 4.29851",
            "unit": "ns",
            "extra": "100 samples\n285 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 385.735,
            "range": "± 19.3939",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 994.6,
            "range": "± 74.4128",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 394.874,
            "range": "± 19.8187",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 728.297,
            "range": "± 64.8944",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 420.003,
            "range": "± 15.6739",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 521.545,
            "range": "± 35.596",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 383.538,
            "range": "± 17.4536",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 428.239,
            "range": "± 35.6148",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 352.54,
            "range": "± 21.0047",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 479.958,
            "range": "± 20.2217",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.20228,
            "range": "± 169.511",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.66581,
            "range": "± 339.018",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.51123,
            "range": "± 104.749",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.83342,
            "range": "± 244.801",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.75267,
            "range": "± 123.278",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.23479,
            "range": "± 258.637",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 1.99939,
            "range": "± 79.2388",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.73843,
            "range": "± 204.991",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.26557,
            "range": "± 111.514",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.12761,
            "range": "± 36.2462",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.73942,
            "range": "± 31.3559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.68293,
            "range": "± 79.2769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.74265,
            "range": "± 31.6883",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.6309,
            "range": "± 140.762",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3232,
            "range": "± 58.4535",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.8805,
            "range": "± 162.375",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.64417,
            "range": "± 48.9434",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.7351,
            "range": "± 34.5769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3556,
            "range": "± 790.56",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.6971,
            "range": "± 350.157",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.3367,
            "range": "± 279.455",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.4593,
            "range": "± 178.559",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.496,
            "range": "± 240.31",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.4957,
            "range": "± 276.628",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.8085,
            "range": "± 154.268",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.0134,
            "range": "± 226.223",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.1846,
            "range": "± 306.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.5414,
            "range": "± 182.496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.5287,
            "range": "± 1.61497",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 990.302,
            "range": "± 44.3075",
            "unit": "ns",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.51725,
            "range": "± 165.791",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.81006,
            "range": "± 421.266",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.47518,
            "range": "± 207.627",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.43912,
            "range": "± 279.274",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.53565,
            "range": "± 104.667",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 2.93791,
            "range": "± 181.205",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 1.99799,
            "range": "± 95.9536",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.64608,
            "range": "± 209.375",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.27326,
            "range": "± 160.706",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "5faf0bcb47e16a3ac559c2b873a8e52a1cf08e2e",
          "message": "Fixed a tracing bug with operands less than 32-bit (#59)\n\nSigned-off-by: Nils Schubert <nilslpschubert@gmail.com>\r\nCo-authored-by: Philipp Grulich <philippgrulich@hotmail.de>",
          "timestamp": "2024-10-08T13:41:34+02:00",
          "tree_id": "1e86e7b09fe4ae3e16566ad0f5d3c6acb60ccdd0",
          "url": "https://github.com/nebulastream/nautilus/commit/5faf0bcb47e16a3ac559c2b873a8e52a1cf08e2e"
        },
        "date": 1728387862607,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.17533,
            "range": "± 58.3551",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.2926,
            "range": "± 749.751",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 37.9119,
            "range": "± 3.13613",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.2624,
            "range": "± 994.01",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 24.3864,
            "range": "± 2.17805",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.9116,
            "range": "± 2.13185",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.3833,
            "range": "± 3.25987",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.8275,
            "range": "± 853.605",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.7344,
            "range": "± 1.10212",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.5127,
            "range": "± 822.684",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 104.512,
            "range": "± 5.66094",
            "unit": "ns",
            "extra": "100 samples\n286 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 379.832,
            "range": "± 14.6017",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 978.425,
            "range": "± 48.56",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 396.731,
            "range": "± 20.2412",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 740.058,
            "range": "± 45.8234",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 415.623,
            "range": "± 20.0532",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 509.711,
            "range": "± 20.025",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 378.857,
            "range": "± 38.8399",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 414.679,
            "range": "± 23.4018",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 364.895,
            "range": "± 22.985",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 484.679,
            "range": "± 20.9608",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.22111,
            "range": "± 119.456",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.79497,
            "range": "± 386.271",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.59065,
            "range": "± 115.968",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.92206,
            "range": "± 329.004",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.80815,
            "range": "± 176.814",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.27995,
            "range": "± 234.839",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.00844,
            "range": "± 105.065",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.78989,
            "range": "± 172.467",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.31218,
            "range": "± 126.624",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.20857,
            "range": "± 80.6563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.82905,
            "range": "± 62.7202",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.74303,
            "range": "± 57.045",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.83004,
            "range": "± 92.4407",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.7376,
            "range": "± 146.985",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4293,
            "range": "± 109.154",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.0019,
            "range": "± 147.185",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.69848,
            "range": "± 103.65",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8614,
            "range": "± 159.591",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3555,
            "range": "± 81.5",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.8994,
            "range": "± 387.097",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.7182,
            "range": "± 553.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.7819,
            "range": "± 341.627",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.002,
            "range": "± 555.592",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.8191,
            "range": "± 1.20159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.3956,
            "range": "± 461.705",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 30.4702,
            "range": "± 626.827",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 30.1036,
            "range": "± 706.904",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 30.1011,
            "range": "± 721.603",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.9614,
            "range": "± 879.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.03674,
            "range": "± 44.8272",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.62623,
            "range": "± 188.862",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.05442,
            "range": "± 423.454",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.5949,
            "range": "± 206.135",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.64508,
            "range": "± 433.307",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.63269,
            "range": "± 214.106",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.08025,
            "range": "± 222.125",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.18465,
            "range": "± 167.972",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.75732,
            "range": "± 220.76",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36488,
            "range": "± 191.011",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "26a1e1b2fbbdd302be03c0ea02ea012b86741d66",
          "message": "make hide addresses a runtime option (#61)",
          "timestamp": "2024-10-08T21:13:44+02:00",
          "tree_id": "6cedba9daa6f80e83b8540bed7f2f7370860950c",
          "url": "https://github.com/nebulastream/nautilus/commit/26a1e1b2fbbdd302be03c0ea02ea012b86741d66"
        },
        "date": 1728414952513,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.14879,
            "range": "± 62.953",
            "unit": "us",
            "extra": "100 samples\n26 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.4602,
            "range": "± 1.50277",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 38.5261,
            "range": "± 3.02715",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.3428,
            "range": "± 1.38208",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 24.5457,
            "range": "± 2.82964",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.8124,
            "range": "± 2.00477",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.0239,
            "range": "± 2.54266",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.4763,
            "range": "± 1.25026",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.7713,
            "range": "± 1.65892",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.535,
            "range": "± 1.3385",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 106.874,
            "range": "± 7.11522",
            "unit": "ns",
            "extra": "100 samples\n292 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 374.778,
            "range": "± 19.588",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 966.575,
            "range": "± 74.2619",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 399.48,
            "range": "± 31.1642",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 726.855,
            "range": "± 39.3101",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 413.965,
            "range": "± 22.0674",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 506.713,
            "range": "± 28.9175",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 394.512,
            "range": "± 46.7748",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 426.329,
            "range": "± 32.8997",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 358.391,
            "range": "± 16.8394",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 491.295,
            "range": "± 28.0018",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.27277,
            "range": "± 213.118",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.80239,
            "range": "± 384.026",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.60477,
            "range": "± 237.207",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.96838,
            "range": "± 308.876",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.7989,
            "range": "± 209.372",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.2722,
            "range": "± 217.111",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.09319,
            "range": "± 260.365",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.83401,
            "range": "± 228.2",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.32703,
            "range": "± 155.741",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.21707,
            "range": "± 138.585",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.94331,
            "range": "± 176.084",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.82517,
            "range": "± 134.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.87023,
            "range": "± 184.179",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.2065,
            "range": "± 353.646",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6359,
            "range": "± 345.9",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.8677,
            "range": "± 729.631",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.70551,
            "range": "± 120.607",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9918,
            "range": "± 195.989",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4654,
            "range": "± 168.676",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.5129,
            "range": "± 516.005",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.1629,
            "range": "± 490.998",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.3534,
            "range": "± 523.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.5324,
            "range": "± 619.074",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.7318,
            "range": "± 739.206",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 28.6733,
            "range": "± 471.328",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.06,
            "range": "± 627.075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.0014,
            "range": "± 571.702",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.563,
            "range": "± 494.214",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.4704,
            "range": "± 557.84",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.05632,
            "range": "± 75.1307",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.61009,
            "range": "± 209.51",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.12077,
            "range": "± 497.844",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.56089,
            "range": "± 231.061",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.70783,
            "range": "± 469.018",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.68802,
            "range": "± 192.041",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.16487,
            "range": "± 241.205",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.14135,
            "range": "± 151.422",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.76467,
            "range": "± 230.12",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.37307,
            "range": "± 188.81",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "042fbcc6784047064e70bf9ee1e7db6ea7bf6f49",
          "message": "fix mlir function type (#62)",
          "timestamp": "2024-10-08T22:57:47+02:00",
          "tree_id": "14eaf1e5c231a7982fe2371b530c0f6241e56293",
          "url": "https://github.com/nebulastream/nautilus/commit/042fbcc6784047064e70bf9ee1e7db6ea7bf6f49"
        },
        "date": 1728421213419,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.20547,
            "range": "± 110.143",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.3947,
            "range": "± 1.72453",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 38.1985,
            "range": "± 4.69739",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.4066,
            "range": "± 1.56836",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 24.8642,
            "range": "± 2.92998",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.8841,
            "range": "± 1.74089",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.09,
            "range": "± 2.49672",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.3784,
            "range": "± 1.22206",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 14.177,
            "range": "± 2.06257",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.8969,
            "range": "± 1.60819",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 115.579,
            "range": "± 17.2446",
            "unit": "ns",
            "extra": "100 samples\n297 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 401.658,
            "range": "± 62.8237",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 982.629,
            "range": "± 91.9801",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 406.697,
            "range": "± 43.2601",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 727.203,
            "range": "± 71.5697",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 420.238,
            "range": "± 42.74",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 514.345,
            "range": "± 28.2081",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 378.079,
            "range": "± 34.5974",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 426.519,
            "range": "± 51.8114",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 375.092,
            "range": "± 41.2228",
            "unit": "ns",
            "extra": "100 samples\n90 iterations"
          },
          {
            "name": "ir_add",
            "value": 515.564,
            "range": "± 62.524",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.31274,
            "range": "± 241.852",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.06718,
            "range": "± 484.142",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.63391,
            "range": "± 211.374",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.13869,
            "range": "± 462.448",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.88948,
            "range": "± 219.903",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.41327,
            "range": "± 381.513",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.1213,
            "range": "± 258.117",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.85873,
            "range": "± 230.745",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.44356,
            "range": "± 228.341",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.42982,
            "range": "± 189.07",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.23031,
            "range": "± 275.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.00213,
            "range": "± 220.126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.3061,
            "range": "± 231.528",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 25.4447,
            "range": "± 269.18",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.1159,
            "range": "± 240.253",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 25.5212,
            "range": "± 346.165",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.83685,
            "range": "± 181.647",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.5459,
            "range": "± 242.258",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 13.1317,
            "range": "± 251.467",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 29.2105,
            "range": "± 281.824",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.8722,
            "range": "± 270.523",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 31.2842,
            "range": "± 285.446",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 30.3157,
            "range": "± 368.229",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 31.3198,
            "range": "± 311.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 30.5184,
            "range": "± 249.795",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 30.8556,
            "range": "± 390.263",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 30.0355,
            "range": "± 318.057",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 30.4144,
            "range": "± 280.427",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 30.0325,
            "range": "± 318.355",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.03865,
            "range": "± 71.1367",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.63797,
            "range": "± 336.435",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.07106,
            "range": "± 599.629",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.65706,
            "range": "± 333.689",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.73131,
            "range": "± 578.34",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.68736,
            "range": "± 266.049",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.17957,
            "range": "± 420.983",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.18082,
            "range": "± 210.085",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.7685,
            "range": "± 319.576",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.45522,
            "range": "± 248.43",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "9ed7afb0e6afb7d872589a6212c019005cb8e58d",
          "message": "refactor(Tracing) Refactored getReturnAddress() (#60)\n\nSigned-off-by: Nils Schubert <nilslpschubert@gmail.com>\r\nCo-authored-by: Philipp Grulich <philippgrulich@hotmail.de>",
          "timestamp": "2024-10-09T20:15:34+02:00",
          "tree_id": "383b44fe9cc683bd629a4f236c813af33e6a271d",
          "url": "https://github.com/nebulastream/nautilus/commit/9ed7afb0e6afb7d872589a6212c019005cb8e58d"
        },
        "date": 1728497924770,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.32286,
            "range": "± 57.4248",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.7865,
            "range": "± 1.10637",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 38.9467,
            "range": "± 3.15086",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.5934,
            "range": "± 801.154",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 24.9246,
            "range": "± 1.75137",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 13.0699,
            "range": "± 992.624",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 18.4891,
            "range": "± 1.53698",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 13.114,
            "range": "± 931.357",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.9714,
            "range": "± 906.798",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 12.1789,
            "range": "± 1.05835",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 102.085,
            "range": "± 4.24746",
            "unit": "ns",
            "extra": "100 samples\n293 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 364.907,
            "range": "± 18.6475",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 950.078,
            "range": "± 39.4222",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 377.687,
            "range": "± 33.0466",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 698.042,
            "range": "± 38.4939",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 395.27,
            "range": "± 15.7621",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 503.926,
            "range": "± 43.0974",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 353.854,
            "range": "± 15.7016",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 402.323,
            "range": "± 44.6496",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 344.403,
            "range": "± 17.9444",
            "unit": "ns",
            "extra": "100 samples\n89 iterations"
          },
          {
            "name": "ir_add",
            "value": 488.041,
            "range": "± 21.5026",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.16526,
            "range": "± 104.708",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.64001,
            "range": "± 314.165",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.51699,
            "range": "± 119.836",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.79408,
            "range": "± 224.505",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.74408,
            "range": "± 141.919",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.20888,
            "range": "± 169.149",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.00171,
            "range": "± 107.73",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.71706,
            "range": "± 108.547",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.25544,
            "range": "± 100.975",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.19618,
            "range": "± 85.4296",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.73399,
            "range": "± 54.463",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.68127,
            "range": "± 45.5761",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.72998,
            "range": "± 39.7242",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.5249,
            "range": "± 78.8496",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2218,
            "range": "± 242.994",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.8548,
            "range": "± 67.3271",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.62494,
            "range": "± 39.2604",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.7736,
            "range": "± 78.0895",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.2299,
            "range": "± 99.6885",
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
          "id": "b11f006b39f68564dbd365cdc5ea8e2d805b2068",
          "message": "remove compiler options (#63)\n\nremove -fPIC and make -fno-omit-frame-pointer required",
          "timestamp": "2024-10-10T06:59:50+02:00",
          "tree_id": "a43b5ecbf67a1d891bccc258c552afe7ef88ca8f",
          "url": "https://github.com/nebulastream/nautilus/commit/b11f006b39f68564dbd365cdc5ea8e2d805b2068"
        },
        "date": 1728536663494,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.4611,
            "range": "± 247.856",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 14.5724,
            "range": "± 16.1644",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 40.1019,
            "range": "± 5.84255",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 12.8132,
            "range": "± 1.4147",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 25.1891,
            "range": "± 2.70669",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 13.2515,
            "range": "± 1.94804",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 19.8367,
            "range": "± 3.05295",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.9312,
            "range": "± 1.7375",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 14.2637,
            "range": "± 1.75667",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 12.1418,
            "range": "± 1.52686",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 118.653,
            "range": "± 19.6004",
            "unit": "ns",
            "extra": "100 samples\n285 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 405.321,
            "range": "± 59.489",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00837,
            "range": "± 100.804",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 406.316,
            "range": "± 43.4659",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 758.685,
            "range": "± 95.9491",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 456.518,
            "range": "± 63.7821",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 526.04,
            "range": "± 46.4386",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 402.461,
            "range": "± 43.721",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 443.51,
            "range": "± 54.0214",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 366.736,
            "range": "± 44.2666",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 508.485,
            "range": "± 55.7026",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.30236,
            "range": "± 204.275",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.86729,
            "range": "± 540.416",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.69271,
            "range": "± 286.295",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.16703,
            "range": "± 649.129",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.9935,
            "range": "± 439.887",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.40172,
            "range": "± 350.275",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25875,
            "range": "± 391.654",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.94352,
            "range": "± 328.592",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.40686,
            "range": "± 353.827",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.27667,
            "range": "± 233.803",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.96387,
            "range": "± 304.236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.77591,
            "range": "± 117.967",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.90266,
            "range": "± 161.748",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.0743,
            "range": "± 337.109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4229,
            "range": "± 183.334",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.2938,
            "range": "± 366.854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.77869,
            "range": "± 157.886",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.0482,
            "range": "± 201.354",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.6642,
            "range": "± 310.812",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.9757,
            "range": "± 669.366",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 29.0682,
            "range": "± 1.86376",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.9717,
            "range": "± 502.726",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.675,
            "range": "± 417.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.474,
            "range": "± 558.897",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.9331,
            "range": "± 666.026",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.7699,
            "range": "± 511.058",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.5088,
            "range": "± 668.449",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.376,
            "range": "± 622.854",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 29.2027,
            "range": "± 768.144",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.24192,
            "range": "± 95.5592",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.92316,
            "range": "± 315.382",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.32263,
            "range": "± 594.987",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.81644,
            "range": "± 273.349",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.97834,
            "range": "± 456.019",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.93048,
            "range": "± 310.7",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.3048,
            "range": "± 321.019",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.37492,
            "range": "± 206.08",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.89582,
            "range": "± 239.377",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.62715,
            "range": "± 290.499",
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
          "id": "4878d8b7f18c03e790a116e632dfc294fa4dd640",
          "message": "use templated tracing function and pass inputs as initializer_list (#64)",
          "timestamp": "2024-10-12T11:12:54+02:00",
          "tree_id": "bc40b1f78f01c6659390233f9df5ca90f0647eca",
          "url": "https://github.com/nebulastream/nautilus/commit/4878d8b7f18c03e790a116e632dfc294fa4dd640"
        },
        "date": 1728724505390,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.25993,
            "range": "± 97.879",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.6813,
            "range": "± 1.6024",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.8356,
            "range": "± 3.25613",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3425,
            "range": "± 961.862",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.5615,
            "range": "± 2.63387",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9355,
            "range": "± 1.07685",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.3096,
            "range": "± 1.59245",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.4498,
            "range": "± 834.48",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.1724,
            "range": "± 1.95034",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.7246,
            "range": "± 1.00768",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 95.975,
            "range": "± 4.7238",
            "unit": "ns",
            "extra": "100 samples\n286 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 360.751,
            "range": "± 20.9781",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 931.45,
            "range": "± 63.1124",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 372.442,
            "range": "± 16.0784",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 691.897,
            "range": "± 31.3071",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 388.307,
            "range": "± 22.2288",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 492.104,
            "range": "± 71.8403",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 357.113,
            "range": "± 25.9501",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 399.005,
            "range": "± 22.1898",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 339.881,
            "range": "± 15.3979",
            "unit": "ns",
            "extra": "100 samples\n92 iterations"
          },
          {
            "name": "ir_add",
            "value": 487.878,
            "range": "± 20.9898",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.3215,
            "range": "± 128.71",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.09675,
            "range": "± 316.111",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.69853,
            "range": "± 150.67",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.0539,
            "range": "± 303.968",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.90273,
            "range": "± 157.857",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.39125,
            "range": "± 184.382",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.06797,
            "range": "± 106.327",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.88041,
            "range": "± 146.76",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.88099,
            "range": "± 766.351",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.08627,
            "range": "± 165.529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.76965,
            "range": "± 94.2152",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.66938,
            "range": "± 75.6741",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.71125,
            "range": "± 293.475",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.5488,
            "range": "± 336.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.1505,
            "range": "± 322.409",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.7774,
            "range": "± 359.379",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.56233,
            "range": "± 133.181",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.7491,
            "range": "± 248.328",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.2947,
            "range": "± 170.611",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 26.7249,
            "range": "± 348.553",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.3906,
            "range": "± 387.052",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.6107,
            "range": "± 353.761",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.5545,
            "range": "± 398.73",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.5846,
            "range": "± 445.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.8531,
            "range": "± 364.019",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.0156,
            "range": "± 477.098",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.2713,
            "range": "± 414.226",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.6883,
            "range": "± 385.241",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.2865,
            "range": "± 514.171",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.20329,
            "range": "± 66.7576",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.79852,
            "range": "± 136.914",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.08907,
            "range": "± 490.757",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.84854,
            "range": "± 212.21",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.98614,
            "range": "± 366.998",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 3.06159,
            "range": "± 269.007",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.48206,
            "range": "± 298.183",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.47557,
            "range": "± 153.463",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.99496,
            "range": "± 187.506",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.63217,
            "range": "± 181.161",
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
          "id": "92f1eb4fce1526c499e44153dba9c50194caab09",
          "message": "fix benchmark (#70)",
          "timestamp": "2024-12-04T22:39:01+01:00",
          "tree_id": "5efc06b106937e0ca06c41f061cc728f8896a975",
          "url": "https://github.com/nebulastream/nautilus/commit/92f1eb4fce1526c499e44153dba9c50194caab09"
        },
        "date": 1733348476387,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.29723,
            "range": "± 113.273",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.242,
            "range": "± 1.25269",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.5052,
            "range": "± 4.80187",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1715,
            "range": "± 1.56998",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.7642,
            "range": "± 2.44795",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.1714,
            "range": "± 2.19541",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.944,
            "range": "± 2.4772",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.4737,
            "range": "± 1.10398",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.5317,
            "range": "± 1.4979",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6339,
            "range": "± 1.85952",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 109.948,
            "range": "± 10.7152",
            "unit": "ns",
            "extra": "100 samples\n258 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 372.442,
            "range": "± 22.5726",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 959.013,
            "range": "± 60.8204",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 395.53,
            "range": "± 59.8134",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 731.437,
            "range": "± 88.6905",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 425.317,
            "range": "± 23.8607",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 512.059,
            "range": "± 46.4425",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 571.583,
            "range": "± 230.202",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 411.21,
            "range": "± 17.163",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 342.574,
            "range": "± 21.2815",
            "unit": "ns",
            "extra": "100 samples\n92 iterations"
          },
          {
            "name": "ir_add",
            "value": 507.243,
            "range": "± 39.1378",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.26594,
            "range": "± 152.61",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.90468,
            "range": "± 322.75",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.63363,
            "range": "± 180.32",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.03365,
            "range": "± 449.96",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.93467,
            "range": "± 488.564",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.40144,
            "range": "± 277.817",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.12559,
            "range": "± 187.968",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.87832,
            "range": "± 246.193",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.33168,
            "range": "± 148.154",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.29677,
            "range": "± 152.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.9424,
            "range": "± 197.209",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.83296,
            "range": "± 140.668",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.3151,
            "range": "± 282.016",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.3225,
            "range": "± 728.54",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5546,
            "range": "± 312.963",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.5329,
            "range": "± 305.45",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.77987,
            "range": "± 190.596",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1755,
            "range": "± 304.623",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5556,
            "range": "± 269.728",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 28.0698,
            "range": "± 440.04",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 28.9171,
            "range": "± 494.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 30.065,
            "range": "± 449.148",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 29.0297,
            "range": "± 474.624",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 30.111,
            "range": "± 493.859",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.2316,
            "range": "± 458.977",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 29.5535,
            "range": "± 407.03",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 28.7628,
            "range": "± 595.506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 29.1756,
            "range": "± 612.865",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.83,
            "range": "± 443.714",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.08007,
            "range": "± 56.7051",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.69933,
            "range": "± 287.102",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.03162,
            "range": "± 525.56",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.60109,
            "range": "± 199.17",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.63915,
            "range": "± 372.571",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.68878,
            "range": "± 242.071",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.08834,
            "range": "± 269.482",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.19304,
            "range": "± 156.104",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.72165,
            "range": "± 175.288",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.36865,
            "range": "± 159.501",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
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
          "id": "d2bee598780174fbd91fcca1b2aa3efdd05b51b1",
          "message": "add graph support (#33)\n\nthis change adds a graph representation for the nautilus ir",
          "timestamp": "2024-12-17T23:58:25+01:00",
          "tree_id": "1e59d4f318a5b08290735b2498238abdb6552483",
          "url": "https://github.com/nebulastream/nautilus/commit/d2bee598780174fbd91fcca1b2aa3efdd05b51b1"
        },
        "date": 1734476625693,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.28894,
            "range": "± 105.74",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.2991,
            "range": "± 1.01949",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 33.9463,
            "range": "± 2.56563",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1028,
            "range": "± 1.35753",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.9894,
            "range": "± 2.64955",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8597,
            "range": "± 1.86496",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.7783,
            "range": "± 2.28384",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.4426,
            "range": "± 1.00755",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.5199,
            "range": "± 1.07629",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.3457,
            "range": "± 936.449",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 111.6,
            "range": "± 10.6988",
            "unit": "ns",
            "extra": "100 samples\n285 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 367.42,
            "range": "± 22.5109",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 970.341,
            "range": "± 58.563",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 379.486,
            "range": "± 24.5378",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 712.53,
            "range": "± 43.0903",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 420.184,
            "range": "± 25.2583",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 509.158,
            "range": "± 29.9922",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 448.32,
            "range": "± 25.2928",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 405.3,
            "range": "± 18.772",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 341.503,
            "range": "± 18.1379",
            "unit": "ns",
            "extra": "100 samples\n93 iterations"
          },
          {
            "name": "ir_add",
            "value": 516.25,
            "range": "± 115.272",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.2398,
            "range": "± 126.056",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.85211,
            "range": "± 373.026",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.58623,
            "range": "± 112.609",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 4.95931,
            "range": "± 286.266",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.89434,
            "range": "± 200.421",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.35523,
            "range": "± 302.548",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.04148,
            "range": "± 119.137",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.8838,
            "range": "± 222.986",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.33702,
            "range": "± 126.167",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.12684,
            "range": "± 43.8135",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.7549,
            "range": "± 110.845",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.69665,
            "range": "± 146.743",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.74828,
            "range": "± 84.4595",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.8569,
            "range": "± 496.178",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2738,
            "range": "± 159.473",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.4458,
            "range": "± 718.057",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.73352,
            "range": "± 190.414",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9791,
            "range": "± 209.425",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.5938,
            "range": "± 318.696",
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
          "id": "26ccafc66d520a58f38fda946a70743bafd3e4a6",
          "message": "Execution benchmark fix (#74)",
          "timestamp": "2024-12-29T09:05:53+01:00",
          "tree_id": "d5947470642a64a6c636c8a283a3082d2635f02d",
          "url": "https://github.com/nebulastream/nautilus/commit/26ccafc66d520a58f38fda946a70743bafd3e4a6"
        },
        "date": 1735459881123,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.3091,
            "range": "± 108.35",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.4074,
            "range": "± 1.45023",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.4611,
            "range": "± 4.85668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1794,
            "range": "± 1.51293",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.0516,
            "range": "± 2.39614",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8295,
            "range": "± 1.78395",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.9524,
            "range": "± 3.05577",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.4318,
            "range": "± 1.20675",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.5957,
            "range": "± 1.65558",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.4984,
            "range": "± 1.57828",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.132,
            "range": "± 8.36355",
            "unit": "ns",
            "extra": "100 samples\n278 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 375.824,
            "range": "± 30.491",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 982.171,
            "range": "± 82.2779",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 416.105,
            "range": "± 48.4446",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 759.589,
            "range": "± 86.7438",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 434.492,
            "range": "± 41.487",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 528.234,
            "range": "± 41.5649",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 464.442,
            "range": "± 54.5688",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 444.414,
            "range": "± 31.1629",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 379.728,
            "range": "± 56.608",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ir_add",
            "value": 516.017,
            "range": "± 69.8658",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.27491,
            "range": "± 200.982",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.85203,
            "range": "± 438.401",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.68256,
            "range": "± 275.223",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.00064,
            "range": "± 370.149",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.86567,
            "range": "± 145.325",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.33331,
            "range": "± 266.821",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.08892,
            "range": "± 239.118",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.93396,
            "range": "± 392.703",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.40219,
            "range": "± 192.703",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.45175,
            "range": "± 537.004",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.6485,
            "range": "± 527.961",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.10988,
            "range": "± 599.769",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.7209,
            "range": "± 531.239",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 25.1668,
            "range": "± 490.367",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.8019,
            "range": "± 388.787",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 25.342,
            "range": "± 559.893",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.85974,
            "range": "± 246.579",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.3086,
            "range": "± 449.313",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.7948,
            "range": "± 454.936",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.4149,
            "range": "± 632.759",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.9082,
            "range": "± 722.068",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.9909,
            "range": "± 624.718",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.832,
            "range": "± 1.2563",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.7702,
            "range": "± 760.969",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.9721,
            "range": "± 589.63",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.9727,
            "range": "± 679.293",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.5823,
            "range": "± 740.167",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.9074,
            "range": "± 745.725",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.0321,
            "range": "± 644.556",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.05688,
            "range": "± 42.5036",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.64612,
            "range": "± 249.234",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 5.93297,
            "range": "± 305.752",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.59425,
            "range": "± 225.07",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.71256,
            "range": "± 605.609",
            "unit": "us",
            "extra": "100 samples\n8 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.69901,
            "range": "± 213.647",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.12355,
            "range": "± 262.249",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.11084,
            "range": "± 157.546",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.73875,
            "range": "± 279.349",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.38871,
            "range": "± 209.267",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.05787,
            "range": "± 196.311",
            "unit": "us",
            "extra": "100 samples\n27 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 15.0607,
            "range": "± 1.46358",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 575.325,
            "range": "± 26.0876",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 70.3234,
            "range": "± 11.6259",
            "unit": "ns",
            "extra": "100 samples\n456 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.8033,
            "range": "± 8.79611",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8879,
            "range": "± 183.384",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 49.1663,
            "range": "± 4.03138",
            "unit": "ns",
            "extra": "100 samples\n639 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 847.798,
            "range": "± 5.85348",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 180.583,
            "range": "± 1.94168",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "lukas.schwerdtfeger@gmail.com",
            "name": "Lukas Schwerdtfeger",
            "username": "ls-1801"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "542d8afcb3bba1f6b706f50fe07cd05e4948b242",
          "message": "Cache Invocable and prevent concurrent access into orc (#71)\n\nCo-authored-by: Philipp Grulich <philippgrulich@hotmail.de>",
          "timestamp": "2024-12-29T15:56:21+01:00",
          "tree_id": "3618865a98bc2c7f2697098f8630819f92586652",
          "url": "https://github.com/nebulastream/nautilus/commit/542d8afcb3bba1f6b706f50fe07cd05e4948b242"
        },
        "date": 1735484409893,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.37666,
            "range": "± 150.83",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.5645,
            "range": "± 1.36166",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.2178,
            "range": "± 3.73195",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.4115,
            "range": "± 1.05625",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.4894,
            "range": "± 2.64577",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8543,
            "range": "± 1.82374",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.5575,
            "range": "± 2.82638",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 13.0381,
            "range": "± 1.48045",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.9539,
            "range": "± 1.48071",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.8129,
            "range": "± 1.56698",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.665,
            "range": "± 15.7267",
            "unit": "ns",
            "extra": "100 samples\n277 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 398.209,
            "range": "± 39.7739",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 999.547,
            "range": "± 74.0168",
            "unit": "ns",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 420.095,
            "range": "± 47.5414",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 750.944,
            "range": "± 72.6208",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 459.595,
            "range": "± 38.5471",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 546.745,
            "range": "± 66.6451",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 382.249,
            "range": "± 39.1926",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 446.87,
            "range": "± 42.1795",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 374.772,
            "range": "± 33.1286",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ir_add",
            "value": 534.419,
            "range": "± 60.289",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.5712,
            "range": "± 481.563",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.67927,
            "range": "± 1.02141",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.84909,
            "range": "± 287.589",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.38027,
            "range": "± 516.558",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.0102,
            "range": "± 428.891",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.57573,
            "range": "± 312.645",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.18033,
            "range": "± 334.989",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.06991,
            "range": "± 325.396",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.55063,
            "range": "± 271.281",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.13253,
            "range": "± 75.3227",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.75359,
            "range": "± 31.0291",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.71257,
            "range": "± 120.637",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.89299,
            "range": "± 266.688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.9171,
            "range": "± 973.975",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.2057,
            "range": "± 116.86",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 23.9612,
            "range": "± 130.746",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.69151,
            "range": "± 30.1053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8207,
            "range": "± 62.5843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.2702,
            "range": "± 43.7506",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.17944,
            "range": "± 0.405536",
            "unit": "ns",
            "extra": "100 samples\n9700 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.81476,
            "range": "± 252.498",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 516.988,
            "range": "± 11.4194",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.46757,
            "range": "± 0.612107",
            "unit": "ns",
            "extra": "100 samples\n6927 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.2411,
            "range": "± 6.82797",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8723,
            "range": "± 131.439",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.7142,
            "range": "± 4.12536",
            "unit": "ns",
            "extra": "100 samples\n1012 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 813.24,
            "range": "± 10.9218",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.323,
            "range": "± 4.09949",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "aljoschalepping@gmail.com",
            "name": "Aljoscha",
            "username": "alepping"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ea13704e7e47eadcf9fc816f74d1e5b742afdcc5",
          "message": "fix(ExecutionTrace): fixed unintentional override of currentOp in get… (#77)\n\nOk lets merge it for now",
          "timestamp": "2025-01-15T10:01:24+01:00",
          "tree_id": "f49f16462824386862a7fd3bb6d0caec8f7810a5",
          "url": "https://github.com/nebulastream/nautilus/commit/ea13704e7e47eadcf9fc816f74d1e5b742afdcc5"
        },
        "date": 1736932021764,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.36203,
            "range": "± 80.1116",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.6627,
            "range": "± 1.65736",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.9594,
            "range": "± 5.183",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1885,
            "range": "± 1.46577",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.0219,
            "range": "± 2.46613",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8945,
            "range": "± 1.77868",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 23.2832,
            "range": "± 2.76121",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.5785,
            "range": "± 1.14523",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.8108,
            "range": "± 1.61829",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.5741,
            "range": "± 1.54301",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 113.603,
            "range": "± 9.03996",
            "unit": "ns",
            "extra": "100 samples\n270 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 412.051,
            "range": "± 26.6201",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.05563,
            "range": "± 57.7168",
            "unit": "us",
            "extra": "100 samples\n31 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 436.734,
            "range": "± 32.0967",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 796.487,
            "range": "± 58.3002",
            "unit": "ns",
            "extra": "100 samples\n40 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 474.131,
            "range": "± 28.883",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 551.164,
            "range": "± 28.4491",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 375.041,
            "range": "± 20.9545",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 468.097,
            "range": "± 29.7569",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 399.615,
            "range": "± 7.33674",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ir_add",
            "value": 517.716,
            "range": "± 24.2798",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.28167,
            "range": "± 128.204",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.93901,
            "range": "± 388.244",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.63933,
            "range": "± 125.666",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.04862,
            "range": "± 285.891",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.91066,
            "range": "± 188.46",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.38928,
            "range": "± 210.683",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.11984,
            "range": "± 129.703",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.93268,
            "range": "± 220.65",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.3992,
            "range": "± 150.23",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.36737,
            "range": "± 1.08417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.75582,
            "range": "± 108.198",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.6947,
            "range": "± 109.129",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.74339,
            "range": "± 109.482",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.0205,
            "range": "± 666.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.3349,
            "range": "± 236.074",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.4091,
            "range": "± 494.515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.96689,
            "range": "± 260.822",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.2203,
            "range": "± 448.88",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.7061,
            "range": "± 630.588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.2691,
            "range": "± 723.938",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.9147,
            "range": "± 962.236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 28.0361,
            "range": "± 815.11",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.243,
            "range": "± 977.432",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.4923,
            "range": "± 804.529",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.6862,
            "range": "± 752.243",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.9934,
            "range": "± 901.084",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.6388,
            "range": "± 802.253",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.5042,
            "range": "± 374.171",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.5065,
            "range": "± 294.447",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.03905,
            "range": "± 47.9664",
            "unit": "us",
            "extra": "100 samples\n30 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.56697,
            "range": "± 199.566",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.00065,
            "range": "± 305.781",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.51212,
            "range": "± 135.508",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.76997,
            "range": "± 577.602",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.60774,
            "range": "± 183.058",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.09364,
            "range": "± 216.173",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.10109,
            "range": "± 114.344",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.70319,
            "range": "± 185.146",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.34914,
            "range": "± 178.782",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.1554,
            "range": "± 0.328522",
            "unit": "ns",
            "extra": "100 samples\n9715 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.8478,
            "range": "± 399.933",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 517.847,
            "range": "± 13.7869",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.41266,
            "range": "± 0.457833",
            "unit": "ns",
            "extra": "100 samples\n6945 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.0297,
            "range": "± 6.71866",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9068,
            "range": "± 382.107",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.5424,
            "range": "± 2.69957",
            "unit": "ns",
            "extra": "100 samples\n1015 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 812.39,
            "range": "± 8.00564",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.426,
            "range": "± 307.338",
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
          "id": "364c89c184430e1ccd0d9422fc73b1cc54c33ca8",
          "message": "Some proposed updates / changes to nautilus (#76)\n\nSigned-off-by: Nils Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-01-20T22:08:27+01:00",
          "tree_id": "e786b1c3402238ff14173880be1cfcb716d78ff6",
          "url": "https://github.com/nebulastream/nautilus/commit/364c89c184430e1ccd0d9422fc73b1cc54c33ca8"
        },
        "date": 1737407531765,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.39902,
            "range": "± 227.421",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 12.2198,
            "range": "± 2.1522",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 36.1323,
            "range": "± 5.109",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.8838,
            "range": "± 1.94181",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 23.7877,
            "range": "± 4.23716",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.6542,
            "range": "± 2.4446",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.4722,
            "range": "± 3.27097",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 13.9848,
            "range": "± 2.58743",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.2509,
            "range": "± 2.23268",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 11.1361,
            "range": "± 1.8717",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 135.097,
            "range": "± 26.4692",
            "unit": "ns",
            "extra": "100 samples\n269 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 406.408,
            "range": "± 62.0384",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.07763,
            "range": "± 188.487",
            "unit": "us",
            "extra": "100 samples\n34 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 472.151,
            "range": "± 73.8849",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 819.707,
            "range": "± 127.088",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 505.416,
            "range": "± 86.5028",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 587.693,
            "range": "± 91.6049",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 484.765,
            "range": "± 71.3911",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 484.505,
            "range": "± 53.6791",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 422.431,
            "range": "± 65.4486",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ir_add",
            "value": 553.131,
            "range": "± 95.5297",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.55539,
            "range": "± 539.888",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.42131,
            "range": "± 1.40721",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.88355,
            "range": "± 532.469",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.47912,
            "range": "± 1.00064",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.09359,
            "range": "± 569.283",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.51852,
            "range": "± 485.015",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.25524,
            "range": "± 426.503",
            "unit": "us",
            "extra": "100 samples\n16 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.03073,
            "range": "± 488.495",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.50909,
            "range": "± 389.147",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.25843,
            "range": "± 169.371",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.93328,
            "range": "± 219.515",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.80344,
            "range": "± 199.474",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.1539,
            "range": "± 379.78",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.0889,
            "range": "± 361.775",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.5299,
            "range": "± 278.537",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.7409,
            "range": "± 600.131",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.93639,
            "range": "± 195.706",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.8757,
            "range": "± 277.266",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.8186,
            "range": "± 292.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 27.4941,
            "range": "± 496.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 27.7413,
            "range": "± 373.689",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 29.0677,
            "range": "± 507.281",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 28.079,
            "range": "± 473.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 29.2836,
            "range": "± 591.309",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 29.0011,
            "range": "± 721.303",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 28.639,
            "range": "± 625.471",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 27.8533,
            "range": "± 453.23",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 28.1793,
            "range": "± 458.541",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 28.2266,
            "range": "± 505.14",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.10044,
            "range": "± 96.741",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.66305,
            "range": "± 245.774",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.33323,
            "range": "± 969.709",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.76542,
            "range": "± 465.17",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.96347,
            "range": "± 724.156",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.84292,
            "range": "± 379.173",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.35883,
            "range": "± 668.301",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.22786,
            "range": "± 267.701",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.92632,
            "range": "± 516.564",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.48012,
            "range": "± 275.234",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.45269,
            "range": "± 0.29365",
            "unit": "ns",
            "extra": "100 samples\n8841 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.01451,
            "range": "± 819.677",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 614.413,
            "range": "± 24.7166",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.54436,
            "range": "± 0.699877",
            "unit": "ns",
            "extra": "100 samples\n6940 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.4359,
            "range": "± 6.92658",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9822,
            "range": "± 224.464",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.8453,
            "range": "± 5.82055",
            "unit": "ns",
            "extra": "100 samples\n1035 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 857.382,
            "range": "± 25.6281",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 180.241,
            "range": "± 1.301",
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
          "id": "364c89c184430e1ccd0d9422fc73b1cc54c33ca8",
          "message": "Some proposed updates / changes to nautilus (#76)\n\nSigned-off-by: Nils Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-01-20T22:08:27+01:00",
          "tree_id": "e786b1c3402238ff14173880be1cfcb716d78ff6",
          "url": "https://github.com/nebulastream/nautilus/commit/364c89c184430e1ccd0d9422fc73b1cc54c33ca8"
        },
        "date": 1747658977089,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.30395,
            "range": "± 197.804",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.5111,
            "range": "± 1.78099",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 38.8375,
            "range": "± 10.5237",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.3047,
            "range": "± 1.52259",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.9081,
            "range": "± 2.61545",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8504,
            "range": "± 1.94496",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 22.9582,
            "range": "± 2.98373",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.6656,
            "range": "± 1.19662",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6097,
            "range": "± 1.77829",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6953,
            "range": "± 1.44007",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 122.556,
            "range": "± 15.1495",
            "unit": "ns",
            "extra": "100 samples\n261 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 392.56,
            "range": "± 47.4302",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00816,
            "range": "± 103.017",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 426.727,
            "range": "± 24.8076",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 784.05,
            "range": "± 74.7185",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 453.531,
            "range": "± 24.1857",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 571.361,
            "range": "± 54.1707",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 484.525,
            "range": "± 57.5862",
            "unit": "ns",
            "extra": "100 samples\n67 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 469.274,
            "range": "± 43.4415",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 389.058,
            "range": "± 34.0749",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ir_add",
            "value": 512.13,
            "range": "± 48.018",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.37943,
            "range": "± 321.977",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.0043,
            "range": "± 640.968",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.67592,
            "range": "± 226.918",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.19709,
            "range": "± 572.819",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.90059,
            "range": "± 225.293",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.4087,
            "range": "± 281.144",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.12128,
            "range": "± 242.534",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.93066,
            "range": "± 250.673",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.45466,
            "range": "± 259.491",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.36846,
            "range": "± 185.053",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.1713,
            "range": "± 252.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.93131,
            "range": "± 158.613",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.1577,
            "range": "± 229.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.4169,
            "range": "± 539.126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.9542,
            "range": "± 270.969",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 25.0217,
            "range": "± 439.303",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.13004,
            "range": "± 755.94",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.3802,
            "range": "± 306.767",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.4188,
            "range": "± 266.722",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.60193,
            "range": "± 0.659888",
            "unit": "ns",
            "extra": "100 samples\n8544 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.86629,
            "range": "± 493.654",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 595.777,
            "range": "± 31.1822",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45665,
            "range": "± 0.530596",
            "unit": "ns",
            "extra": "100 samples\n6790 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.3748,
            "range": "± 10.691",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9444,
            "range": "± 335.044",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.0228,
            "range": "± 4.29076",
            "unit": "ns",
            "extra": "100 samples\n1013 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 851.444,
            "range": "± 10.152",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.114,
            "range": "± 5.73993",
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
          "id": "c77002ee44cc2cdc75e93915f1988568fc870460",
          "message": "remove old ci environments (#79)",
          "timestamp": "2025-06-18T08:46:22+02:00",
          "tree_id": "d020e3aa191c2349da5fc38fc8123842dfb51422",
          "url": "https://github.com/nebulastream/nautilus/commit/c77002ee44cc2cdc75e93915f1988568fc870460"
        },
        "date": 1750229585825,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.28616,
            "range": "± 68.2213",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.3271,
            "range": "± 1.39931",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.384,
            "range": "± 4.67092",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1188,
            "range": "± 847.44",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.9462,
            "range": "± 2.38661",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.6529,
            "range": "± 1.10794",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.8052,
            "range": "± 2.00452",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.7047,
            "range": "± 1.0536",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6218,
            "range": "± 1.78647",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.501,
            "range": "± 745.222",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 118.694,
            "range": "± 12.8514",
            "unit": "ns",
            "extra": "100 samples\n261 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 378.257,
            "range": "± 17.1221",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.01623,
            "range": "± 145.629",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 423.152,
            "range": "± 19.3023",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 785.581,
            "range": "± 58.8395",
            "unit": "ns",
            "extra": "100 samples\n41 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 458.034,
            "range": "± 31.1726",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 544.616,
            "range": "± 26.2108",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 456.708,
            "range": "± 27.9408",
            "unit": "ns",
            "extra": "100 samples\n66 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 459.159,
            "range": "± 32.6368",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 389.494,
            "range": "± 32.2448",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ir_add",
            "value": 531.917,
            "range": "± 61.1388",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.34454,
            "range": "± 247.916",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.00614,
            "range": "± 470.968",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.67581,
            "range": "± 218.842",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.07924,
            "range": "± 424.738",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.90462,
            "range": "± 233.854",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.34007,
            "range": "± 198.567",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.17075,
            "range": "± 216.26",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.90469,
            "range": "± 247.349",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.40199,
            "range": "± 116.553",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.33511,
            "range": "± 358.45",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.97445,
            "range": "± 151.968",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.84621,
            "range": "± 287.735",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.0855,
            "range": "± 309.633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 24.9879,
            "range": "± 521.29",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.6828,
            "range": "± 313.037",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.4219,
            "range": "± 283.197",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 8.29035,
            "range": "± 1.17722",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.2597,
            "range": "± 264.858",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.2891,
            "range": "± 215.497",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.42647,
            "range": "± 0.182374",
            "unit": "ns",
            "extra": "100 samples\n8654 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.87692,
            "range": "± 489.47",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 544.485,
            "range": "± 22.8066",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.4964,
            "range": "± 0.678909",
            "unit": "ns",
            "extra": "100 samples\n6738 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 98.4821,
            "range": "± 16.3167",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 36.013,
            "range": "± 787.126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.2627,
            "range": "± 3.41601",
            "unit": "ns",
            "extra": "100 samples\n1014 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 848.514,
            "range": "± 7.01956",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.232,
            "range": "± 4.68897",
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
          "id": "ff7fbf68e5ee715280c698c9b3c8cb4caab9462f",
          "message": "Fixed wrong data type for initialization (#78)\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-06-18T10:05:44+02:00",
          "tree_id": "b7e599cdc03fb37d21c966276eab2e46a739f46f",
          "url": "https://github.com/nebulastream/nautilus/commit/ff7fbf68e5ee715280c698c9b3c8cb4caab9462f"
        },
        "date": 1750234285564,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.32732,
            "range": "± 125.312",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.5841,
            "range": "± 1.60011",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 59.0805,
            "range": "± 6.09213",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.5038,
            "range": "± 1.53393",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.2916,
            "range": "± 2.34398",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.9075,
            "range": "± 1.72765",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.927,
            "range": "± 2.77735",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 11.2312,
            "range": "± 1.35833",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.8045,
            "range": "± 1.50362",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.7715,
            "range": "± 1.57262",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 118.242,
            "range": "± 16.9526",
            "unit": "ns",
            "extra": "100 samples\n278 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 380.596,
            "range": "± 28.4225",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 999.049,
            "range": "± 105.581",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 401.008,
            "range": "± 41.2718",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 750.831,
            "range": "± 85.4793",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 445.765,
            "range": "± 60.2484",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 511.153,
            "range": "± 48.5596",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 379.274,
            "range": "± 35.1818",
            "unit": "ns",
            "extra": "100 samples\n69 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 436.133,
            "range": "± 43.2874",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 366.672,
            "range": "± 36.1275",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 521.736,
            "range": "± 57.4286",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.3533,
            "range": "± 261.951",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.08494,
            "range": "± 652.635",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.76111,
            "range": "± 303.55",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.63682,
            "range": "± 1.46945",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.98381,
            "range": "± 289.344",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.48743,
            "range": "± 277.478",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.13889,
            "range": "± 191.784",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.955,
            "range": "± 235.343",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.51311,
            "range": "± 290.987",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.21912,
            "range": "± 137.731",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.94924,
            "range": "± 188.884",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.82901,
            "range": "± 178.254",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.2748,
            "range": "± 281.233",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 25.0264,
            "range": "± 716.843",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4729,
            "range": "± 218.212",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.6005,
            "range": "± 430.128",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.71249,
            "range": "± 146.3",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1221,
            "range": "± 297.232",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1759,
            "range": "± 215.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.5271,
            "range": "± 692.138",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.4192,
            "range": "± 1.94198",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.3579,
            "range": "± 594.102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 27.0738,
            "range": "± 1.94876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 28.0485,
            "range": "± 1.74075",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.1242,
            "range": "± 933.981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.8364,
            "range": "± 724.457",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.7783,
            "range": "± 508.662",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 26.562,
            "range": "± 512.787",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 27.1732,
            "range": "± 2.00454",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.09897,
            "range": "± 65.0023",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.64168,
            "range": "± 240.591",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.21565,
            "range": "± 521.293",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.55208,
            "range": "± 151.89",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.77484,
            "range": "± 536.422",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.71424,
            "range": "± 282.342",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.13272,
            "range": "± 218.862",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.16422,
            "range": "± 168.823",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.77809,
            "range": "± 312.526",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.4129,
            "range": "± 231.478",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.15194,
            "range": "± 0.32975",
            "unit": "ns",
            "extra": "100 samples\n9513 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.91838,
            "range": "± 571.544",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 566.524,
            "range": "± 55.6164",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.33891,
            "range": "± 0.0594589",
            "unit": "ns",
            "extra": "100 samples\n6764 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.5706,
            "range": "± 7.83262",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9214,
            "range": "± 77.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 31.9567,
            "range": "± 5.44232",
            "unit": "ns",
            "extra": "100 samples\n990 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 813.671,
            "range": "± 13.7072",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.884,
            "range": "± 1.52804",
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
          "id": "26045212b7200c18bd95baff27ce99c909a4383b",
          "message": "Fixes for llvm-v20 update (#80)",
          "timestamp": "2025-06-19T07:53:24+02:00",
          "tree_id": "05cc1b962bcc0acf534181b995d600cb4587758d",
          "url": "https://github.com/nebulastream/nautilus/commit/26045212b7200c18bd95baff27ce99c909a4383b"
        },
        "date": 1750312747402,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.28315,
            "range": "± 83.6565",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.2476,
            "range": "± 1.34334",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.398,
            "range": "± 4.64246",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.2446,
            "range": "± 1.64845",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.0319,
            "range": "± 2.97933",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.7046,
            "range": "± 1.86593",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.8393,
            "range": "± 3.02441",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.4315,
            "range": "± 1.16003",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6675,
            "range": "± 1.72816",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.3514,
            "range": "± 1.33447",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 118.687,
            "range": "± 5.17713",
            "unit": "ns",
            "extra": "100 samples\n252 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 391.438,
            "range": "± 27.6871",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 985.812,
            "range": "± 64.3105",
            "unit": "ns",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 426.908,
            "range": "± 43.3464",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 730.931,
            "range": "± 41.2945",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 422.882,
            "range": "± 19.563",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 523.915,
            "range": "± 37.5012",
            "unit": "ns",
            "extra": "100 samples\n62 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 384.17,
            "range": "± 21.4335",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 431.374,
            "range": "± 21.8606",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 369.533,
            "range": "± 20.077",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ir_add",
            "value": 503.378,
            "range": "± 25.071",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.30172,
            "range": "± 154.759",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.04441,
            "range": "± 532.224",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.73815,
            "range": "± 199.85",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 6.2645,
            "range": "± 1.80428",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.90386,
            "range": "± 180.57",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.44463,
            "range": "± 265.303",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.1683,
            "range": "± 276.681",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.87307,
            "range": "± 140.969",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.4179,
            "range": "± 155.64",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.4251,
            "range": "± 194.22",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.1227,
            "range": "± 261.832",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 8.10161,
            "range": "± 169.272",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.5028,
            "range": "± 394.123",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 25.3324,
            "range": "± 572.588",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 15.7432,
            "range": "± 300.204",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 25.4184,
            "range": "± 674.311",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.89596,
            "range": "± 173.35",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.559,
            "range": "± 780.766",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.3378,
            "range": "± 187.145",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.13707,
            "range": "± 0.27724",
            "unit": "ns",
            "extra": "100 samples\n9517 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.20178,
            "range": "± 1.04654",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 514.797,
            "range": "± 12.6304",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.48386,
            "range": "± 0.664222",
            "unit": "ns",
            "extra": "100 samples\n6778 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.4897,
            "range": "± 9.93301",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8792,
            "range": "± 106.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 32.5647,
            "range": "± 2.56589",
            "unit": "ns",
            "extra": "100 samples\n928 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 896.522,
            "range": "± 3.52616",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 189.939,
            "range": "± 8.16206",
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
          "id": "8cbc913d2ecaff5beb71ebf02cc6841992e1ca31",
          "message": "Some fixes to comply with clang-tidy from the NebulaStream repo/project (#82)",
          "timestamp": "2025-06-19T09:24:12+02:00",
          "tree_id": "0a38d212f71e4a7f1cb99dfb42bfd45161ece0ae",
          "url": "https://github.com/nebulastream/nautilus/commit/8cbc913d2ecaff5beb71ebf02cc6841992e1ca31"
        },
        "date": 1750318245125,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.26846,
            "range": "± 57.1154",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.5165,
            "range": "± 1.30492",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.8696,
            "range": "± 3.11226",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.4278,
            "range": "± 1.29702",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.5248,
            "range": "± 2.45579",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.8699,
            "range": "± 1.11054",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.7042,
            "range": "± 1.64876",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.889,
            "range": "± 1.22201",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.8343,
            "range": "± 1.42849",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.5798,
            "range": "± 910.771",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 110.78,
            "range": "± 5.02531",
            "unit": "ns",
            "extra": "100 samples\n269 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 380.331,
            "range": "± 20.686",
            "unit": "ns",
            "extra": "100 samples\n82 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 977.622,
            "range": "± 72.8557",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 395.252,
            "range": "± 23.8915",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 737.512,
            "range": "± 45.6959",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 426.846,
            "range": "± 33.6141",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 526.029,
            "range": "± 34.6095",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 452.949,
            "range": "± 42.0948",
            "unit": "ns",
            "extra": "100 samples\n68 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 426.437,
            "range": "± 25.18",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 361.136,
            "range": "± 22.0432",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 542.936,
            "range": "± 27.3819",
            "unit": "ns",
            "extra": "100 samples\n56 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.39164,
            "range": "± 134.806",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.03617,
            "range": "± 337.727",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.732,
            "range": "± 118.604",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.10339,
            "range": "± 205.921",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.98744,
            "range": "± 185.175",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.60316,
            "range": "± 421.589",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.22255,
            "range": "± 116.054",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.96807,
            "range": "± 200.285",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.43709,
            "range": "± 103.539",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.2815,
            "range": "± 160.022",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.18277,
            "range": "± 1.28537",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.80418,
            "range": "± 121.032",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.85739,
            "range": "± 123.028",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 23.861,
            "range": "± 431.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4017,
            "range": "± 175.109",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 24.1395,
            "range": "± 226.688",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.7662,
            "range": "± 176.757",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.9324,
            "range": "± 123.905",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.2694,
            "range": "± 721.546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.15451,
            "range": "± 0.398215",
            "unit": "ns",
            "extra": "100 samples\n9514 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.89234,
            "range": "± 645.961",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 517.699,
            "range": "± 15.1943",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.57401,
            "range": "± 0.84447",
            "unit": "ns",
            "extra": "100 samples\n6786 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.6552,
            "range": "± 6.05062",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9296,
            "range": "± 267.159",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.2334,
            "range": "± 3.16989",
            "unit": "ns",
            "extra": "100 samples\n1012 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 849.611,
            "range": "± 9.60121",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 179.95,
            "range": "± 466.626",
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
          "id": "7b95202f5f0255599f05c8667673845627e17c79",
          "message": "update MLIR to version 20.1.7 and add ArithToLLVM conversion pass (#83)",
          "timestamp": "2025-06-27T11:58:23+02:00",
          "tree_id": "9c48d9e62190d60d937839571e3d35c0b9cdb8ae",
          "url": "https://github.com/nebulastream/nautilus/commit/7b95202f5f0255599f05c8667673845627e17c79"
        },
        "date": 1751018646635,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.30827,
            "range": "± 74.9489",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.5153,
            "range": "± 1.54598",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.3654,
            "range": "± 5.97226",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.2789,
            "range": "± 931.18",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.7418,
            "range": "± 1.49366",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.6337,
            "range": "± 943.476",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.4437,
            "range": "± 2.70802",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.8557,
            "range": "± 922.278",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6891,
            "range": "± 1.41341",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.605,
            "range": "± 1.06304",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 109.849,
            "range": "± 14.736",
            "unit": "ns",
            "extra": "100 samples\n291 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 384.167,
            "range": "± 48.8866",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 976.197,
            "range": "± 59.8277",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 400.111,
            "range": "± 31.5206",
            "unit": "ns",
            "extra": "100 samples\n81 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 743.174,
            "range": "± 83.9236",
            "unit": "ns",
            "extra": "100 samples\n46 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 426.446,
            "range": "± 29.7577",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 507.322,
            "range": "± 31.3022",
            "unit": "ns",
            "extra": "100 samples\n65 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 377.556,
            "range": "± 46.4308",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 417.708,
            "range": "± 21.7658",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 364.193,
            "range": "± 24.8309",
            "unit": "ns",
            "extra": "100 samples\n88 iterations"
          },
          {
            "name": "ir_add",
            "value": 515.787,
            "range": "± 44.3282",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.31746,
            "range": "± 173.946",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.03762,
            "range": "± 583.08",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.71282,
            "range": "± 273.915",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.08369,
            "range": "± 414.392",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.88236,
            "range": "± 171.88",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.38831,
            "range": "± 213.115",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.24597,
            "range": "± 474.026",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.90903,
            "range": "± 220.727",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.50927,
            "range": "± 427.105",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.77764,
            "range": "± 121.428",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.41867,
            "range": "± 138.401",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.38035,
            "range": "± 117.633",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.48302,
            "range": "± 230.267",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.7468,
            "range": "± 340.333",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.8357,
            "range": "± 177.892",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.0876,
            "range": "± 210.493",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.31379,
            "range": "± 130.784",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.6039,
            "range": "± 155.976",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.6455,
            "range": "± 193.888",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.035,
            "range": "± 465.756",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.6361,
            "range": "± 366.091",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.638,
            "range": "± 347.965",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.8323,
            "range": "± 457.213",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.6349,
            "range": "± 377.816",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 26.3475,
            "range": "± 596.298",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 26.2338,
            "range": "± 422.765",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.3898,
            "range": "± 347.941",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.8926,
            "range": "± 355.576",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.6723,
            "range": "± 334.755",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.06772,
            "range": "± 58.0054",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.67083,
            "range": "± 245.988",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.12946,
            "range": "± 475.14",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.58118,
            "range": "± 171.088",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.64271,
            "range": "± 352.363",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.68539,
            "range": "± 211.675",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.16912,
            "range": "± 320.813",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.16507,
            "range": "± 129.722",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.69185,
            "range": "± 231.982",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.40486,
            "range": "± 244.479",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.1186,
            "range": "± 0.18735",
            "unit": "ns",
            "extra": "100 samples\n9525 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.88974,
            "range": "± 545.106",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 513.784,
            "range": "± 18.1935",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.48777,
            "range": "± 0.694019",
            "unit": "ns",
            "extra": "100 samples\n6780 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 95.3151,
            "range": "± 8.14904",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9405,
            "range": "± 199.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.258,
            "range": "± 1.73029",
            "unit": "ns",
            "extra": "100 samples\n1001 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 911.866,
            "range": "± 4.63916",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 194.69,
            "range": "± 1.4691",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "811fc7b29c1e28739bf4348af65d5b93885dfa68",
          "message": "Cpp Backend fix (#89)",
          "timestamp": "2025-07-27T16:45:59+02:00",
          "tree_id": "d4e1dbe77c75e59b3eb8876596d4e7bc4c38dc39",
          "url": "https://github.com/nebulastream/nautilus/commit/811fc7b29c1e28739bf4348af65d5b93885dfa68"
        },
        "date": 1753627953609,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.29089,
            "range": "± 108.319",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.205,
            "range": "± 1.2474",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 34.6503,
            "range": "± 4.65715",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.1484,
            "range": "± 1.56239",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.0435,
            "range": "± 2.93892",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 11.7281,
            "range": "± 1.76796",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 16.9405,
            "range": "± 2.48894",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.871,
            "range": "± 1.23891",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6687,
            "range": "± 1.80591",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.429,
            "range": "± 1.55311",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 114.172,
            "range": "± 7.30281",
            "unit": "ns",
            "extra": "100 samples\n262 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 366.776,
            "range": "± 15.7443",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 974.423,
            "range": "± 66.8155",
            "unit": "ns",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 397.002,
            "range": "± 14.5005",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 717.947,
            "range": "± 34.6292",
            "unit": "ns",
            "extra": "100 samples\n45 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 426.048,
            "range": "± 23.0499",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 502.064,
            "range": "± 32.4834",
            "unit": "ns",
            "extra": "100 samples\n64 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 377.341,
            "range": "± 51.0986",
            "unit": "ns",
            "extra": "100 samples\n83 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 427.392,
            "range": "± 24.0476",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 363.918,
            "range": "± 21.4587",
            "unit": "ns",
            "extra": "100 samples\n85 iterations"
          },
          {
            "name": "ir_add",
            "value": 495.391,
            "range": "± 32.9951",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.27131,
            "range": "± 132.407",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.98367,
            "range": "± 651.348",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.64529,
            "range": "± 196.987",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.04483,
            "range": "± 389.418",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.86387,
            "range": "± 193.993",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.37356,
            "range": "± 231.662",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.08609,
            "range": "± 152.292",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.86295,
            "range": "± 176.096",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.39916,
            "range": "± 294.214",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.04755,
            "range": "± 175.557",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.69255,
            "range": "± 175.417",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.61938,
            "range": "± 165.864",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.82492,
            "range": "± 214.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.1075,
            "range": "± 375.335",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.4722,
            "range": "± 219.964",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.5118,
            "range": "± 271.315",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.53858,
            "range": "± 161.638",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.1966,
            "range": "± 200.76",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.1078,
            "range": "± 189.452",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 25.8285,
            "range": "± 427.337",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 26.7889,
            "range": "± 389.141",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 27.7862,
            "range": "± 379.133",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 26.7671,
            "range": "± 313.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 27.6946,
            "range": "± 275.914",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 27.1637,
            "range": "± 318.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 27.3143,
            "range": "± 319.578",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 26.5278,
            "range": "± 314.499",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 27.1261,
            "range": "± 356.587",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 26.5246,
            "range": "± 357.311",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.04557,
            "range": "± 72.1101",
            "unit": "us",
            "extra": "100 samples\n29 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.49144,
            "range": "± 339.728",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.05549,
            "range": "± 440.255",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.551,
            "range": "± 203.458",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.56041,
            "range": "± 288.458",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.71422,
            "range": "± 212.361",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.12688,
            "range": "± 228.968",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.13383,
            "range": "± 133.711",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.68536,
            "range": "± 265.956",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.52133,
            "range": "± 321.135",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.16494,
            "range": "± 0.380938",
            "unit": "ns",
            "extra": "100 samples\n9509 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.96459,
            "range": "± 758.761",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 572.475,
            "range": "± 30.5052",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.38695,
            "range": "± 0.492514",
            "unit": "ns",
            "extra": "100 samples\n6790 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.3343,
            "range": "± 14.2191",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9208,
            "range": "± 152.612",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.212,
            "range": "± 1.5168",
            "unit": "ns",
            "extra": "100 samples\n990 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 817.77,
            "range": "± 25.0804",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 176.58,
            "range": "± 312.866",
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
          "id": "a072e2fe34e7b3cb0acf0b1fc8aa3405c42ee15d",
          "message": "fix(DumpHelper): Only create folder if necessary in the DumpHelper (#88)\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-08-01T15:57:31+02:00",
          "tree_id": "01ddf9224e69bc574586dbba6104a39b83e2325f",
          "url": "https://github.com/nebulastream/nautilus/commit/a072e2fe34e7b3cb0acf0b1fc8aa3405c42ee15d"
        },
        "date": 1754056993585,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.3586,
            "range": "± 83.4869",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.7527,
            "range": "± 1.73265",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.5197,
            "range": "± 4.09682",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.4665,
            "range": "± 1.33378",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.4278,
            "range": "± 2.84776",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.0494,
            "range": "± 1.73084",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.7312,
            "range": "± 2.51635",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.8538,
            "range": "± 953.618",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.1189,
            "range": "± 1.81108",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.755,
            "range": "± 1.8865",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 121.501,
            "range": "± 9.18698",
            "unit": "ns",
            "extra": "100 samples\n247 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 394.861,
            "range": "± 33.4393",
            "unit": "ns",
            "extra": "100 samples\n79 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.02584,
            "range": "± 95.9123",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 418.16,
            "range": "± 21.9879",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 764.348,
            "range": "± 40.1958",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 447.097,
            "range": "± 26.5941",
            "unit": "ns",
            "extra": "100 samples\n70 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 541.654,
            "range": "± 32.5363",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 404.703,
            "range": "± 33.0659",
            "unit": "ns",
            "extra": "100 samples\n75 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 446.336,
            "range": "± 24.1184",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 381.039,
            "range": "± 29.5685",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ir_add",
            "value": 648.986,
            "range": "± 44.6566",
            "unit": "ns",
            "extra": "100 samples\n47 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.68941,
            "range": "± 171.908",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.41222,
            "range": "± 511.112",
            "unit": "us",
            "extra": "100 samples\n5 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.74394,
            "range": "± 163.506",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.17936,
            "range": "± 425.358",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.9752,
            "range": "± 222.987",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.49021,
            "range": "± 285.781",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.63951,
            "range": "± 210.136",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.97221,
            "range": "± 226.883",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.4506,
            "range": "± 322.32",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 7.94934,
            "range": "± 137.842",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.59601,
            "range": "± 144.92",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.5165,
            "range": "± 123.544",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.57875,
            "range": "± 175.347",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 30.3183,
            "range": "± 1.6283",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.0321,
            "range": "± 239.556",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 29.555,
            "range": "± 380.431",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.48096,
            "range": "± 125.236",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.8137,
            "range": "± 222.841",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.7742,
            "range": "± 186.87",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.12956,
            "range": "± 0.199111",
            "unit": "ns",
            "extra": "100 samples\n9522 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.88829,
            "range": "± 532.175",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 526.322,
            "range": "± 17.1741",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.44816,
            "range": "± 0.5711",
            "unit": "ns",
            "extra": "100 samples\n6799 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.7683,
            "range": "± 7.15966",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9304,
            "range": "± 69.3754",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 38.0035,
            "range": "± 3.05678",
            "unit": "ns",
            "extra": "100 samples\n762 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 897.854,
            "range": "± 9.50438",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 191.207,
            "range": "± 1.43893",
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
          "id": "9860a28efe26c56b0ec872aa4e12c209f8f1caf4",
          "message": "Bug dereferencing ptr to ptr (#84)\n\nSigned-off-by: Nils L. Schubert <nilslpschubert@gmail.com>\nCo-authored-by: Nils L. Schubert <nilslpschubert@gmail.com>",
          "timestamp": "2025-08-01T16:10:23+02:00",
          "tree_id": "b08ad1f37fef68caf15fa305efb64bffd5459dc0",
          "url": "https://github.com/nebulastream/nautilus/commit/9860a28efe26c56b0ec872aa4e12c209f8f1caf4"
        },
        "date": 1754057774861,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.31441,
            "range": "± 138.593",
            "unit": "us",
            "extra": "100 samples\n24 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.3509,
            "range": "± 1.47802",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.9328,
            "range": "± 5.98085",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.5725,
            "range": "± 1.88519",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 21.7045,
            "range": "± 2.38529",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.127,
            "range": "± 2.04345",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.533,
            "range": "± 3.31627",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.8867,
            "range": "± 1.44556",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 13.0366,
            "range": "± 1.8733",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.6661,
            "range": "± 1.63035",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 125.284,
            "range": "± 13.1521",
            "unit": "ns",
            "extra": "100 samples\n251 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 414.614,
            "range": "± 52.7164",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.05325,
            "range": "± 134.666",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 444.894,
            "range": "± 53.5839",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 791.612,
            "range": "± 81.1567",
            "unit": "ns",
            "extra": "100 samples\n42 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 453.735,
            "range": "± 44.9149",
            "unit": "ns",
            "extra": "100 samples\n71 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 560.275,
            "range": "± 54.7748",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 403.167,
            "range": "± 45.4108",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 459.77,
            "range": "± 48.0065",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 375.333,
            "range": "± 35.5901",
            "unit": "ns",
            "extra": "100 samples\n84 iterations"
          },
          {
            "name": "ir_add",
            "value": 538.134,
            "range": "± 94.6556",
            "unit": "ns",
            "extra": "100 samples\n61 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.35194,
            "range": "± 263.087",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.04787,
            "range": "± 510.62",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.78483,
            "range": "± 357.06",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.28757,
            "range": "± 664.13",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.13483,
            "range": "± 481.567",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.53886,
            "range": "± 396.724",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.17035,
            "range": "± 267.302",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.99056,
            "range": "± 360.243",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.56275,
            "range": "± 395.901",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.06453,
            "range": "± 533.363",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.86428,
            "range": "± 665.473",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.72572,
            "range": "± 700.126",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.90205,
            "range": "± 765.953",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.3562,
            "range": "± 1.36049",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.7239,
            "range": "± 927.598",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.7466,
            "range": "± 1.24162",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.65429,
            "range": "± 458.009",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4566,
            "range": "± 868.083",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.393,
            "range": "± 822.981",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.2529,
            "range": "± 0.59132",
            "unit": "ns",
            "extra": "100 samples\n9523 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.96098,
            "range": "± 760.469",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 570.158,
            "range": "± 46.0643",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.37697,
            "range": "± 0.246929",
            "unit": "ns",
            "extra": "100 samples\n6807 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.2646,
            "range": "± 6.33581",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9102,
            "range": "± 75.5282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 33.8713,
            "range": "± 7.24578",
            "unit": "ns",
            "extra": "100 samples\n1015 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 894.998,
            "range": "± 7.53145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 191.251,
            "range": "± 654.635",
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
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "6925e7abd8b3efdd83d05316b940e0f50d54d6c3",
          "message": "Improve formating",
          "timestamp": "2025-08-01T18:59:13+02:00",
          "tree_id": "929ca23771f3a3b1740bd23f198e663bc963fd3d",
          "url": "https://github.com/nebulastream/nautilus/commit/6925e7abd8b3efdd83d05316b940e0f50d54d6c3"
        },
        "date": 1754067958017,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.32949,
            "range": "± 183.827",
            "unit": "us",
            "extra": "100 samples\n23 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 11.4876,
            "range": "± 1.71052",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 35.2336,
            "range": "± 5.08668",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 11.2934,
            "range": "± 1.74604",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 22.4816,
            "range": "± 3.21838",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 12.1076,
            "range": "± 2.07646",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 17.4133,
            "range": "± 3.0431",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 12.2232,
            "range": "± 1.23877",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 12.6374,
            "range": "± 1.74145",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 10.7237,
            "range": "± 1.53014",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "ssa_add",
            "value": 120.441,
            "range": "± 7.42898",
            "unit": "ns",
            "extra": "100 samples\n255 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 397.774,
            "range": "± 26.6463",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.02509,
            "range": "± 94.9715",
            "unit": "us",
            "extra": "100 samples\n33 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 418.408,
            "range": "± 30.8429",
            "unit": "ns",
            "extra": "100 samples\n77 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 776.851,
            "range": "± 74.3705",
            "unit": "ns",
            "extra": "100 samples\n44 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 439.378,
            "range": "± 28.1044",
            "unit": "ns",
            "extra": "100 samples\n72 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 533.439,
            "range": "± 31.8132",
            "unit": "ns",
            "extra": "100 samples\n60 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 399.049,
            "range": "± 47.0901",
            "unit": "ns",
            "extra": "100 samples\n78 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 450.525,
            "range": "± 44.6466",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 375.692,
            "range": "± 28.2413",
            "unit": "ns",
            "extra": "100 samples\n86 iterations"
          },
          {
            "name": "ir_add",
            "value": 519.62,
            "range": "± 38.8529",
            "unit": "ns",
            "extra": "100 samples\n59 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.39286,
            "range": "± 188.69",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 6.20975,
            "range": "± 616.368",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.79211,
            "range": "± 262.929",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.36703,
            "range": "± 572.491",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 3.02517,
            "range": "± 264.26",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.5909,
            "range": "± 347.294",
            "unit": "us",
            "extra": "100 samples\n9 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.18963,
            "range": "± 303.739",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 3.01678,
            "range": "± 370.341",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.57042,
            "range": "± 231.194",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.43998,
            "range": "± 198.433",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 9.06024,
            "range": "± 233.763",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.83005,
            "range": "± 149.855",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 10.4272,
            "range": "± 581.451",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 31.0953,
            "range": "± 554.065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 14.8942,
            "range": "± 226.608",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 30.6354,
            "range": "± 786.654",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.67819,
            "range": "± 146.406",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 13.4992,
            "range": "± 196.913",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 12.7518,
            "range": "± 263.867",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.16714,
            "range": "± 0.375197",
            "unit": "ns",
            "extra": "100 samples\n9627 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 4.93697,
            "range": "± 632.687",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 607.104,
            "range": "± 20.9064",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.54255,
            "range": "± 0.813968",
            "unit": "ns",
            "extra": "100 samples\n6875 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 94.0574,
            "range": "± 5.14756",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.9237,
            "range": "± 131.203",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 30.7037,
            "range": "± 3.1886",
            "unit": "ns",
            "extra": "100 samples\n1026 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 893.559,
            "range": "± 4.43754",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 191.08,
            "range": "± 183.02",
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
          "id": "598041b59644088ed431bb27a35e77f48c1e8bad",
          "message": "Fix clang tidy (#85)",
          "timestamp": "2025-08-04T17:17:59+02:00",
          "tree_id": "0b3be71009d80c070f4f3c35c1c34d8b44f89e88",
          "url": "https://github.com/nebulastream/nautilus/commit/598041b59644088ed431bb27a35e77f48c1e8bad"
        },
        "date": 1754321018436,
        "tool": "catch2",
        "benches": [
          {
            "name": "trace_add",
            "value": 1.26173,
            "range": "± 117.513",
            "unit": "us",
            "extra": "100 samples\n25 iterations"
          },
          {
            "name": "trace_ifThenElse",
            "value": 10.3509,
            "range": "± 1.45055",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_deeplyNestedIfElse",
            "value": 31.6565,
            "range": "± 4.90154",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "trace_loop",
            "value": 10.3319,
            "range": "± 1.53406",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_ifInsideLoop",
            "value": 19.9253,
            "range": "± 2.56917",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_loopDirectCall",
            "value": 10.7472,
            "range": "± 1.72213",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_pointerLoop",
            "value": 15.8798,
            "range": "± 3.01604",
            "unit": "us",
            "extra": "100 samples\n2 iterations"
          },
          {
            "name": "trace_staticLoop",
            "value": 10.727,
            "range": "± 1.56441",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_fibonacci",
            "value": 11.7509,
            "range": "± 1.66055",
            "unit": "us",
            "extra": "100 samples\n3 iterations"
          },
          {
            "name": "trace_gcd",
            "value": 9.54959,
            "range": "± 1.11595",
            "unit": "us",
            "extra": "100 samples\n4 iterations"
          },
          {
            "name": "ssa_add",
            "value": 116.594,
            "range": "± 5.54492",
            "unit": "ns",
            "extra": "100 samples\n261 iterations"
          },
          {
            "name": "ssa_ifThenElse",
            "value": 394.982,
            "range": "± 43.2829",
            "unit": "ns",
            "extra": "100 samples\n80 iterations"
          },
          {
            "name": "ssa_deeplyNestedIfElse",
            "value": 1.00101,
            "range": "± 59.7226",
            "unit": "us",
            "extra": "100 samples\n32 iterations"
          },
          {
            "name": "ssa_loop",
            "value": 410.327,
            "range": "± 31.5821",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_ifInsideLoop",
            "value": 748.75,
            "range": "± 45.308",
            "unit": "ns",
            "extra": "100 samples\n43 iterations"
          },
          {
            "name": "ssa_loopDirectCall",
            "value": 424.149,
            "range": "± 18.3169",
            "unit": "ns",
            "extra": "100 samples\n74 iterations"
          },
          {
            "name": "ssa_pointerLoop",
            "value": 506.915,
            "range": "± 21.1326",
            "unit": "ns",
            "extra": "100 samples\n63 iterations"
          },
          {
            "name": "ssa_staticLoop",
            "value": 395.626,
            "range": "± 30.7825",
            "unit": "ns",
            "extra": "100 samples\n76 iterations"
          },
          {
            "name": "ssa_fibonacci",
            "value": 441.169,
            "range": "± 37.6677",
            "unit": "ns",
            "extra": "100 samples\n73 iterations"
          },
          {
            "name": "ssa_gcd",
            "value": 355.479,
            "range": "± 13.7361",
            "unit": "ns",
            "extra": "100 samples\n87 iterations"
          },
          {
            "name": "ir_add",
            "value": 522.437,
            "range": "± 30.5578",
            "unit": "ns",
            "extra": "100 samples\n58 iterations"
          },
          {
            "name": "ir_ifThenElse",
            "value": 2.32604,
            "range": "± 176.548",
            "unit": "us",
            "extra": "100 samples\n14 iterations"
          },
          {
            "name": "ir_deeplyNestedIfElse",
            "value": 5.94014,
            "range": "± 242.578",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loop",
            "value": 2.71858,
            "range": "± 331.94",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "ir_ifInsideLoop",
            "value": 5.15515,
            "range": "± 508.896",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "ir_loopDirectCall",
            "value": 2.91538,
            "range": "± 204.773",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_pointerLoop",
            "value": 3.39823,
            "range": "± 264.191",
            "unit": "us",
            "extra": "100 samples\n10 iterations"
          },
          {
            "name": "ir_staticLoop",
            "value": 2.09664,
            "range": "± 130.27",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "ir_fibonacci",
            "value": 2.89546,
            "range": "± 210.646",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "ir_gcd",
            "value": 2.39254,
            "range": "± 129.877",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_mlir_add",
            "value": 8.21759,
            "range": "± 272.282",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifThenElse",
            "value": 8.81958,
            "range": "± 319.273",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_deeplyNestedIfElse",
            "value": 7.56346,
            "range": "± 173.887",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loop",
            "value": 9.92369,
            "range": "± 752.883",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_ifInsideLoop",
            "value": 29.9139,
            "range": "± 741.635",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_loopDirectCall",
            "value": 13.7076,
            "range": "± 108.546",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_pointerLoop",
            "value": 28.9425,
            "range": "± 240.301",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_staticLoop",
            "value": 7.35058,
            "range": "± 127.345",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_fibonacci",
            "value": 12.5376,
            "range": "± 118.549",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_mlir_gcd",
            "value": 11.5443,
            "range": "± 93.3102",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_add",
            "value": 24.7661,
            "range": "± 337.065",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifThenElse",
            "value": 25.5181,
            "range": "± 416.173",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_deeplyNestedIfElse",
            "value": 26.5371,
            "range": "± 374.875",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loop",
            "value": 25.5518,
            "range": "± 339.472",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_ifInsideLoop",
            "value": 26.3616,
            "range": "± 400.207",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_loopDirectCall",
            "value": 25.9178,
            "range": "± 322.169",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_pointerLoop",
            "value": 25.9816,
            "range": "± 371.673",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_staticLoop",
            "value": 25.2074,
            "range": "± 352.568",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_fibonacci",
            "value": 25.6705,
            "range": "± 357.876",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_cpp_gcd",
            "value": 25.4141,
            "range": "± 340.273",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "comp_bc_add",
            "value": 1.05068,
            "range": "± 49.3955",
            "unit": "us",
            "extra": "100 samples\n28 iterations"
          },
          {
            "name": "comp_bc_ifThenElse",
            "value": 2.4234,
            "range": "± 159.25",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_deeplyNestedIfElse",
            "value": 6.03526,
            "range": "± 657.242",
            "unit": "us",
            "extra": "100 samples\n6 iterations"
          },
          {
            "name": "comp_bc_loop",
            "value": 2.48618,
            "range": "± 144.366",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "comp_bc_ifInsideLoop",
            "value": 4.58758,
            "range": "± 372.201",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "comp_bc_loopDirectCall",
            "value": 2.64349,
            "range": "± 185.729",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_pointerLoop",
            "value": 3.10902,
            "range": "± 297.696",
            "unit": "us",
            "extra": "100 samples\n11 iterations"
          },
          {
            "name": "comp_bc_staticLoop",
            "value": 2.09279,
            "range": "± 180.16",
            "unit": "us",
            "extra": "100 samples\n15 iterations"
          },
          {
            "name": "comp_bc_fibonacci",
            "value": 2.65888,
            "range": "± 208.109",
            "unit": "us",
            "extra": "100 samples\n12 iterations"
          },
          {
            "name": "comp_bc_gcd",
            "value": 2.35547,
            "range": "± 182.061",
            "unit": "us",
            "extra": "100 samples\n13 iterations"
          },
          {
            "name": "exec_mlir_add",
            "value": 3.18475,
            "range": "± 0.412493",
            "unit": "ns",
            "extra": "100 samples\n9527 iterations"
          },
          {
            "name": "exec_mlir_fibonacci",
            "value": 5.03908,
            "range": "± 936.594",
            "unit": "us",
            "extra": "100 samples\n7 iterations"
          },
          {
            "name": "exec_mlir_sum",
            "value": 519.818,
            "range": "± 17.3001",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_add",
            "value": 4.45763,
            "range": "± 0.550282",
            "unit": "ns",
            "extra": "100 samples\n6798 iterations"
          },
          {
            "name": "exec_cpp_fibonacci",
            "value": 96.8219,
            "range": "± 16.4095",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_cpp_sum",
            "value": 35.8966,
            "range": "± 175.444",
            "unit": "ms",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_add",
            "value": 29.7667,
            "range": "± 1.71088",
            "unit": "ns",
            "extra": "100 samples\n1013 iterations"
          },
          {
            "name": "exec_bc_fibonacci",
            "value": 893.728,
            "range": "± 5.89145",
            "unit": "us",
            "extra": "100 samples\n1 iterations"
          },
          {
            "name": "exec_bc_sum",
            "value": 191.785,
            "range": "± 1.52868",
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
      }
    ]
  }
}