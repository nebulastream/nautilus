# Nautilus Playground

A Godbolt-style web UI for exploring the Nautilus compilation pipeline. Users
write a Nautilus module (real `val<T>` C++ code with multiple functions), hit
**Compile**, and inspect every stage:

- the **execution trace** captured while the traced functions run once,
- the trace **after SSA construction**,
- the **IR** right after trace-to-IR conversion,
- the IR **after each optimization pass** (constant folding, algebraic
  simplification, branch folding, empty-block elimination, block merging,
  DCE, block-argument pruning, plus the opt-in LICM/CSE/strength-reduction),
- an interactive **control-flow graph** for any IR-bearing stage,
- and the **final backend code**: MLIR + LLVM IR, generated C, bytecode,
  tiered bytecode, or AsmJit assembly.

Nautilus itself needs **zero changes** — everything is driven through the
engine's existing options (`dump.all`, `ir.dumpAfterEachPass`,
`engine.backend`, …).

## Architecture

```
browser ──> Caddy ──> API server (Node/Fastify)        tools/playground/server
                          │  queue, rate limit, cache
                          ▼
                 ephemeral runner container            tools/playground/runner
                 (one per compile job)
                   1. clang++ user_module.cpp   (PCH-accelerated)
                   2. link with prebuilt driver + libplayground_core.so
                   3. run driver: trace → SSA → IR → passes → backend
                      with every dump enabled → /out/manifest.json
                          │
                          ▼
                 stage artifacts returned as JSON, rendered by the
                 React app (Monaco + Mermaid)          tools/playground/web
```

Key design points:

- **User-code contract**: the user writes one translation unit defining
  `void playground_register(nautilus::engine::NautilusModule&)` — the exact
  module API from `example/src/DemoModules.cpp`, minus the engine
  boilerplate. All registered functions share one trace, one IR graph, and
  one optimization run.
- **Runner image** (`runner/Dockerfile.runner`) bakes in the expensive work
  once: the full nautilus build with all five backends (the MLIR toolchain
  tarball downloads at image-build time), pre-linked into
  `libplayground_core.so`, plus a precompiled header for the public API.
  A request only compiles the user TU (~1–3 s with the PCH), links one
  object, and runs the driver once.
- **Stage capture**: the driver enables `dump.all` + `ir.dumpAfterEachPass`,
  compiles, then scans `$TMPDIR/dump/*/` (one compile produces two dump
  directories — frontend and backend — each with its own compilation-unit
  id) and writes a single `manifest.json`. Per-pass dumps are only written
  when a pass changed the IR and show the state after the last fixed-point
  iteration in which it fired; the UI labels them accordingly.
- **Graph view**: the React app reuses the IR parser and Mermaid CFG builder
  from `tools/vscode-nautilus-ir` (via a ~50-line `vscode` shim aliased in
  Vite), so a CFG can be rendered for *every* IR stage client-side — more
  than the engine's own `dump.graph` provides.

## Threat model & sandboxing

This is designed to run as a **hosted public service**: both the clang
compile of user source (`#include` tricks, template bombs) and the tracing
run (arbitrary C++ executes once) are treated as adversarial. Every job runs
in its own ephemeral container with:

- `--network=none`, `--read-only` rootfs, tmpfs `/work` (512 MB)
- `--memory=2g --cpus=2 --pids-limit=128`
- `--cap-drop=ALL --security-opt=no-new-privileges`, default seccomp profile
- `--ulimit fsize/core/nofile` caps, non-root user
- in-container `timeout`s per phase (compile 60 s / link 15 s / run 20 s)
  and a hard 120 s wall-clock kill from the orchestrator

Server-side controls: 128 KB source cap, per-IP rate limit (10/min), bounded
in-process queue (concurrency ≈ 2, depth 20), and an LRU result cache keyed
by `sha256(source + backend + options)`.

Residual risks and hardening options:

- The API container mounts the Docker socket to spawn runners; compromise of
  the API process means control of the daemon. For hardened deployments set
  `SANDBOX_CMD` to rootless **podman**, or register **gVisor** (`runsc`) as
  the runtime for the runner containers.
- Kernel attack surface remains (syscalls from user code). gVisor or a
  dedicated VM per host mitigates.
- Runner exit codes map failures to phases: 10 compile, 11 link, 12 trace
  timeout/crash, 13 nautilus pipeline error (partial stages still returned).

## Local development

```sh
# 1. Runner image (slow the first time: full nautilus + MLIR build)
docker build -f tools/playground/runner/Dockerfile.runner -t nautilus-playground-runner .

# 2. API server (spawns runner containers via the local docker daemon)
cd tools/playground/server
npm install
npm run dev          # listens on :8080

# 3. Web app (dev server proxies /api to :8080)
cd ../web
npm install
npm run dev          # open http://localhost:5173
```

Smoke-test the runner without the server:

```sh
tools/playground/runner/smoke/run_smoke.sh
```

## Deployment

```sh
docker build -f tools/playground/runner/Dockerfile.runner -t nautilus-playground-runner .
docker build -f tools/playground/server/Dockerfile -t nautilus-playground-api .
cd tools/playground
# edit Caddyfile (domain) and docker-compose.yml, then:
docker compose up -d
```

## API

| Endpoint | Description |
| --- | --- |
| `POST /api/compile` | `{source, backend: mlir\|cpp\|bc\|tbc\|asmjit, options?}` → `202 {jobId}` or `200 {cached, result}` |
| `GET /api/jobs/:id` | `{status, queuePosition?, result?, error?}` |
| `GET /api/jobs/:id/events` | Server-sent events for status transitions |
| `GET /api/examples` | Curated example gallery |
| `GET /api/meta` | Backends, pass options, limits |

The result payload contains ordered `stages` (`{key, title, phase, lang,
order, text, truncated}`), `diagnostics` (failure phase + captured
stdout/stderr; clang errors map onto editor squiggles), and `timings`.

## Not in v1 (planned)

- Side-by-side diff between consecutive pass snapshots (Monaco diff editor)
- Shareable permalinks (`#code=<lz-string>`)
- Nautilus plugins (std/simd/specialization) in the runner image
- Dark mode
