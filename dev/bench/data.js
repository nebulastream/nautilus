window.BENCHMARK_DATA = {
  "lastUpdate": 1724357401088,
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
      }
    ]
  }
}