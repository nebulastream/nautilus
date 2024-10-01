window.BENCHMARK_DATA = {
  "lastUpdate": 1727729208878,
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
          },
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
          },
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
          },
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
          },
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
      }
    ]
  }
}