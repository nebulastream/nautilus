---
name: build-and-test
description: Build Nautilus and run the right tests for a change. Use before pushing any code change, when asked to build or test Nautilus, when a test fails locally, or when choosing a CMake preset or sanitizer configuration.
---

# Build and test Nautilus

## 1. Pick a preset

| Change touches | Preset | Why |
|---|---|---|
| anything, first iteration | `dev` | no MLIR download; asmjit, tbc, cpp backends |
| the MLIR backend, the inlining plugin, LLVM IR tests | `full` | needs MLIR (~500 MB download on first configure) |
| threads, promotion, caches, concurrency | `tsan` | data races show only under TSan |
| memory ownership, arenas, loaders | `asan` | ASan + LeakSanitizer |
| `#ifdef ENABLE_COMPILER` stubs, interpreted mode | `nocompiler` | a CI leg; stubs drift otherwise |
| tracing that may depend on inlining | `debug` | tracing behaves differently at -O0 (#450) |

```bash
cmake --preset <preset>        # once per build directory
```

## 2. Build only what you need

```bash
cmake --build build/<preset> --target nautilus-execution-tests
```

| Target | When |
|---|---|
| `nautilus-execution-tests` | almost always |
| `nautilus-tracing-tests` | tracing or IR-shape changes (Clang only) |
| `nautilus-tracing-unit-tests` | tracer internals |
| `nautilus-ir-pass-tests` | IR passes |
| `nautilus-value-tests` | `val<T>` API |
| `nautilus-common-tests` | `common/` (arenas, exception transport) |
| `nautilus-<plugin>-tests` | plugin changes (`std`, `simd`, `builtin`, `profiling`, `inlining`, `specialization`) |

A header change under `nautilus/include/` rebuilds most of the tree: build the
targets in one `cmake --build` call so Ninja schedules them together.

## 3. Run

```bash
B=build/<preset>
ctest --preset <preset> -j8                                               # all tests, core and plugins
$B/nautilus/test/execution-tests/nautilus-execution-tests                 # one binary
$B/nautilus/test/execution-tests/nautilus-execution-tests "Tiered*"       # by name (wildcards)
$B/nautilus/test/execution-tests/nautilus-execution-tests "[region]"      # by tag
ctest --test-dir $B/nautilus --output-on-failure -R <regex>               # through ctest
ctest --test-dir $B/plugins/<plugin>/test --output-on-failure             # one plugin
```

- Add `NAUTILUS_IR_VERIFY=1` when changing IR or passes: the verifier then runs
  after every pass.
- Select one backend with `NAUTILUS_OPTIONS="engine.backend=asmjit"` when a
  test is backend-parameterized and you only touched one backend.
- Under `asan`, set `ASAN_OPTIONS=detect_leaks=1`; the test binaries carry
  LLVM leak suppressions (`nautilus/test/common/SanitizerSuppressions.cpp`).

## 4. Before pushing

1. `./format.sh` (clang-format 22; older versions report false violations).
2. The tests of every area you touched, on the backends you changed.
3. If you added `#ifdef` paths or stubs: build the `nocompiler` preset.
4. Re-read the diff for what CI's 14 configurations would reject: GCC 14
   warnings as errors, macOS (no `/proc`, different `dlopen`), AArch64.

## 5. Reading failures

- **"File does not exist … Initializing with current dump"** from
  `nautilus-tracing-tests`: a new reference dump was written; use the
  `regenerate-dumps` skill.
- **A failure only on one backend:** rerun with
  `NAUTILUS_OPTIONS="engine.backend=<x>;dump.all=true;dump.console=true"`
  to see each stage.
- **A crash in tracing:** check for `val<T>` lifetime issues and run under the
  `asan` preset.
