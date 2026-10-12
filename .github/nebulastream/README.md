# NebulaStream against nautilus

`.github/workflows/nebulastream.yml` builds NebulaStream `main` against this nautilus, runs its tests, and benchmarks it. The files here do the work:

| File | Purpose |
|---|---|
| `build-nautilus.sh` | Builds this nautilus inside NebulaStream's `nes-ci` image, with every backend enabled, and installs it in place of the nautilus package prebuilt into the image. |
| `benchmark.toml` | The benchmark: which systest queries to measure, and the nautilus configurations to measure them under. |
| `nes-bench.py` | Runs the benchmark (`run`), turns the measurements into metrics and a summary (`report`), and compares two runs (`compare`). |

## How the benchmark measures

- **Configurations at runtime.** A configuration is a set of nautilus options, e.g. `{ "engine.backend" = "tbc", "tbc.mode" = "jit" }`. `nes-bench.py` passes them in `NAUTILUS_OPTIONS`, which nautilus layers over the options NebulaStream sets itself. So one NebulaStream build serves every configuration, and adding one is a few lines of TOML.
- **One query per process.** Every (round, query, configuration) is a fresh `systest -b` process, about 0.3 s of overhead. `NAUTILUS_STATISTICS_FILE` collects the statistics of each pipeline nautilus compiles for that query. Configurations are interleaved within a round, so drift on the machine spreads evenly over them.
- **One node, one worker thread.**
  - With NebulaStream's default two-node topology, a query's execution time includes the channel registration backoff between the nodes, up to 500 ms.
  - Several worker threads add idle jumps of about 30 ms.
  - With one node and one thread, a query's rounds stay within about ±20% of each other.
- **What is reported.**
  - Compilation is the median round, execution the best round. Execution is the worker's running-to-stopped time, which excludes compilation unless the configuration defers code generation to the first call (lazy MLIR).
  - Configurations with `suite = true` also run every systest query once, which gives suite-wide compile phases and execution means.
- **Pinned queries.** Each query is pinned by the start of its SQL, not its number, so NebulaStream renumbering a test file can't silently swap what a label measures. A query whose SQL changed is skipped with a warning.

## Running it locally

Build NebulaStream against your nautilus as the workflow does, then:

```sh
nes-bench.py run --config benchmark.toml --container nes \
    --build-dir $NES/build-benchmark --systest-dir $NES/nes-systests \
    --configurations mlir-eager asmjit --queries 'join' --rounds 3 --no-suite --out head.json
nes-bench.py compare base.json head.json
```

Without `--container`, systest runs on the host. Paths must be the same inside and outside the container.
