# NebulaStream against nautilus

`.github/workflows/nebulastream.yml` builds NebulaStream `main` against this nautilus, runs its tests, and benchmarks it. It runs nightly when nautilus `main` has changed since the last successful nightly run, on demand, and on PRs labeled `nebulastream`. The files here do the work:

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
  - Compilation is the median round, execution the best round. Execution is the worker's running-to-stopped time, which excludes compilation. MLIR is measured with `mlir.eager_compilation=true`, so its machine code generation counts as compilation instead of landing in the first execution.
  - Configurations with `suite = true` also run every systest query once, which gives suite-wide compile phases and execution means.
  - Each query's compilation metric also carries the median time of every compilation pass, tracing first, in its `extra` as `passes name=ms ...`. The passes add up to the compilation time: `backend` is backend time no pass of its own covers (all of it for BC and TBC), `other` the rest. The benchmark UI draws them as a per-query breakdown. They ride along in `extra` rather than as metrics of their own, which would multiply the published history and alert on sub-millisecond passes.
- **Pinned queries.** Each query is pinned by the start of its SQL, not its number, so NebulaStream renumbering a test file can't silently swap what a label measures. A query whose SQL changed is skipped with a warning.

## Running it locally

Build NebulaStream against your nautilus as the workflow does (one build serves the tests and the benchmark), then:

```sh
nes-bench.py run --config benchmark.toml --container nes \
    --build-dir $NES/build --systest-dir $NES/nes-systests \
    --configurations mlir asmjit --queries 'join' --rounds 3 --no-suite --out head.json
nes-bench.py compare base.json head.json
```

Without `--container`, systest runs on the host. Paths must be the same inside and outside the container.
