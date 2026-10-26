# AGENTS.md: working on Nautilus

Nautilus is a tracing JIT compiler for C++: user code written with `val<T>`
is executed once per explored path, the recorded trace becomes an SSA IR, the
IR passes clean it up, and a backend turns it into executable code. It is
used for query compilation in NebulaStream. C++20; CMake + Ninja.

This file is for agents and people changing Nautilus itself. Users of the
library start at `docs/getting-started.md`. Subsystem notes live in
`AGENTS.md` files next to the code (§8).

## 1. Setup

- **Cloud sessions:** `.claude/hooks/session-start.sh` installs everything
  below. Nothing to do.
- **Elsewhere:** CMake ≥ 3.25, Ninja, Clang 21 (default) or GCC 14, ccache
  (optional), and clang-format 22 for the format check
  (`pip install clang-format==22.1.8`; `format.sh` prefers a binary named
  `clang-format-22`). Clang 18 and GCC 13 are too old.
- `.claude/settings.json` sets `CC=clang-21` and `CXX=clang++-21`.

## 2. Build

Presets (`CMakePresets.json`) build into `build/<preset>`:

| Preset | Use |
|---|---|
| `dev` | **default for iteration:** Release, every backend except MLIR, so no 500 MB MLIR download |
| `full` | what CI builds by default, MLIR included; first configure downloads MLIR |
| `debug` | Debug, no MLIR; tracing behaves differently at -O0 (#450) |
| `asan`, `tsan` | sanitizers, MLIR included |
| `nocompiler` | `ENABLE_COMPILER=OFF`, interpreted only (a CI leg) |
| `bench` | `ENABLE_BENCHMARKS=ON` |

```bash
cmake --preset dev
cmake --build --preset dev                                 # everything
cmake --build build/dev --target nautilus-execution-tests  # one target
```

Build only the targets you need; a full `dev` build takes about 6.5 minutes
from scratch (§9). Changing a header under `nautilus/include/` rebuilds almost everything.

## 3. Test

Test binaries (Catch2) under `build/<preset>/`:

| Binary | Covers |
|---|---|
| `nautilus/test/execution-tests/nautilus-execution-tests` | end-to-end behavior on every built backend |
| `nautilus/test/execution-tests/nautilus-tracing-tests` | reference dumps in `nautilus/test/data` (**Clang only**) |
| `nautilus/test/tracing-tests/nautilus-tracing-unit-tests` | tracer internals |
| `nautilus/test/ir-pass-tests/nautilus-ir-pass-tests` | each IR pass in isolation |
| `nautilus/test/val-tests/nautilus-value-tests` | `val<T>` semantics |
| `nautilus/test/common/nautilus-common-tests` | arenas, exception transport, utilities |
| `nautilus/test/nir-tests/nautilus-nir-tests` | NIR (0.4): data structures, verifier, text form, canonical form and hash, images, option registry, conformance corpus |
| `plugins/<name>/test/nautilus-<name>-tests` | each plugin (`std`, `simd`, `builtin`, `profiling`, `inlining`, …) |

```bash
ctest --preset dev -j8                                                         # everything: core and plugins
./build/dev/nautilus/test/execution-tests/nautilus-execution-tests "Tiered*"   # by test-case name
./build/dev/nautilus/test/execution-tests/nautilus-execution-tests "[tag]"     # by tag
ctest --test-dir build/dev/nautilus --output-on-failure -R Tiered              # through ctest
ctest --test-dir build/dev/plugins/std/test --output-on-failure                # one plugin
```

- Run the tests of what you changed before pushing; CI runs all of them on
  14 configurations (`.github/workflows/pr.yml`).
- `NAUTILUS_IR_VERIFY=1` runs the IR verifier after every pass in the tests.
- **Reference dumps:** see the `regenerate-dumps` skill. Never regenerate
  dumps in the same commit as a behavior change.
- The differential fuzzer (`nautilus/test/fuzz/README.md`) checks every
  backend against a C++ oracle; `-DENABLE_FUZZER_REPLAY=ON` builds a replay
  driver that works with any compiler.

## 4. Format and style

- `./format.sh` checks, `./format.sh -i` fixes. CI uses **clang-format 22**;
  older versions report false violations and `format.sh` warns about it.
- Tabs, 120 columns, `#pragma once`. Follow the surrounding code:
  - classes and structs `PascalCase` (`TieredJITCompiler`);
  - functions, methods, locals and parameters `camelCase` (`compileTier`,
    `moduleOptions`);
  - private members of classes with a trailing underscore (`pipeline_`);
    plain structs without (`ModuleState::executable`);
  - macros and enumerators of `enum` tables `UPPER_CASE`.
  The `.clang-tidy` naming section is older than the code and not enforced;
  the code is the reference.
- Comments explain *why*; keep their density close to the surrounding code.

## 5. Where things are

| Path | Content |
|---|---|
| `nautilus/include/nautilus/` | public API: `val.hpp` and `val_*.hpp`, `Engine.hpp`, `Module.hpp`, `options.hpp`, `region.hpp`, `static.hpp`, `select.hpp`, `function.hpp` |
| `nautilus/src/nautilus/tracing/` | the tracer: `TraceContext`, `ExecutionTrace`, snapshots, tags (`tag/`), symbolic execution, trace → IR conversion (`phases/TraceToIRConversionPhase.cpp`) |
| `nautilus/src/nautilus/compiler/` | `CompilationPipeline`, `TieredCompiler` (tier 0 / tier 1 promotion), `Engine.cpp`, dumps and statistics |
| `nautilus/src/nautilus/compiler/ir/` | the IR (`IRGraph`, `operations/`, `blocks/`) and its passes (`passes/`, run by `IRPassManager`) |
| `nautilus/src/nautilus/compiler/backends/` | `asmjit` (x86-64, AArch64), `tbc` (threaded bytecode, optional copy-and-patch JIT), `bc` (bytecode), `mlir` (MLIR → LLVM), `cpp` (C++ source + system compiler) |
| `nautilus/src/nautilus/nir/` | NIR, the 0.4 IR: built next to the 0.3 pipeline, not yet called by the engine (see its `AGENTS.md`) |
| `third_party/asmjit/` | vendored AsmJit with Nautilus patches, marked `[Nautilus]` |
| `third_party/xxhash/` | vendored xxHash (XXH3-128 for the NIR structural hash) |
| `plugins/` | `std`, `simd`, `builtin`, `specialization`, `inlining`, `gpu`, `profiling`, each with its own tests |
| `nautilus/test/` | tests (§3), `data/` reference dumps, `benchmark/`, `fuzz/` |
| `docs/` | user documentation; `docs/options.md` lists every option |
| `tools/` | playground, stencil generator for `tbc`, perf helpers, `nir-opt` (`nautilus-nir-opt`: read, verify, print, hash NIR) |

## 6. Debugging and measuring

- **Options from the environment:** `NAUTILUS_OPTIONS="engine.backend=asmjit;dump.all=true"`
  layers options over every engine (`docs/options.md`).
- **Dumps:** `dump.after_tracing`, `dump.after_ir_creation`, `dump.all`,
  `dump.console=true` (to stdout), `dump.graph`; backend dumps such as
  `dump.after_asmjit_generation` and `dump.after_asmjit_assembly`.
- **Statistics:** `engine.logStatistics=true` logs per-stage timings;
  `NAUTILUS_STATISTICS_FILE=<path>` appends one JSON line per compile.
- **Performance work:** see the `perf-investigation` skill (callgrind recipes,
  what the benchmark bot's numbers mean).
- `engine.backend=<x>` pins a single backend; without it the engine compiles
  tier 0 (`asmjit` where available) and promotes to MLIR in the background.

## 7. Changes, commits, pull requests

- 0.4 work branches from `nautilus-0.4` and targets it; `main` takes only
  fixes and is merged into `nautilus-0.4` regularly.
- One logical change per commit; imperative commit subjects ("Fix …",
  "Add …"), with the why in the body.
- Format before committing; run the affected tests.
- CI (`pr.yml`) builds GCC 14, Clang 19/21, Debug, macOS, ARM, ASan, TSan,
  `ENABLE_COMPILER=OFF`, FFI closures, GPU and short-circuit-bool variants,
  plus the format check and the fuzzer smoke corpus. The benchmark bot
  comments with ratios against `main`: shared runners vary by ±30%, and only
  the 200% alert is a signal. Compare suspicious ratios with code your
  change cannot affect before acting on them.
- The 0.4 design is in `docs/plans/nautilus-0.4/`.

## 8. Skills and subsystem notes

Project skills in `.claude/skills/`:

| Skill | Use it to |
|---|---|
| `build-and-test` | pick a preset, build the right targets, run and interpret the tests |
| `ir-change` | add or change an IR operation everywhere it must be handled |
| `perf-investigation` | measure compile time or generated-code quality before and after a change |
| `regenerate-dumps` | update `nautilus/test/data` reference dumps correctly |
| `nautilus-review` | review user code written with `val<T>` for tracing and codegen quality |

Subsystem notes: `nautilus/src/nautilus/nir/AGENTS.md`, `nautilus/src/nautilus/tracing/AGENTS.md`,
`nautilus/src/nautilus/compiler/AGENTS.md`,
`nautilus/src/nautilus/compiler/backends/AGENTS.md`, `plugins/AGENTS.md`.

## 9. Reference numbers

Measured in a cloud session (Clang 21, `dev` preset): configure ~4 s, full
build ~6.5 min from scratch, `ctest --preset dev -j8` (604 tests) ~25 s,
`nautilus-execution-tests` alone ~45 s. The session hook takes ~30 s on a
fresh container and well under a second afterwards.
