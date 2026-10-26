# Implementation Plan

Part of the [Nautilus 0.4 design](README.md) · All milestones · Turns the
component documents (01–15, 17) into ordered work packages and pull requests

## 1. Purpose and how to use it

The component documents say *what* 0.4 builds and why. This document says
*in which order*, *in which files*, *how it is tested*, and *how each step
is proven done*. Every work package (WP) has:

- an id (`M2.4`), a size and its dependencies;
- what it builds, and which files it creates, changes or deletes;
- its tests;
- a "done when" check that a reviewer can verify.

Work packages are units of planning and review. They are grouped into **19
larger PRs** (§4), each landing one coherent capability. Inside a PR, each
WP is one or a few commits, so the PR can be reviewed commit by commit.

**Sizes** are rough engineering effort for sequencing, not commitments:
**S** 0.5–1 week, **M** 1–3 weeks, **L** 3–6 weeks, **XL** 6–10 weeks.

## 2. Strategy: build alongside, cut over once

**The only compatibility requirement** is the user-facing C++ API of
README §5: `val<T>`, `static_val`, `region()`, `nautilus::function`,
`invoke`, the engine, modules and options (renamed options keep deprecated
aliases). Everything internal may change, including the plugin API, the
dump formats and the on-disk formats. Not every PR has to work end to end.

That allows a simple strategy with **no bridge between the 0.3 IR and NIR,
and no second pipeline in the engine**:

1. **Phase A, before the cut-over.** The new pipeline is built next to the
   old one, in new libraries that the engine does not call yet:
   - NIR, its verifier, printer, parser and image (PR 1);
   - a new tracer that writes pre-SSA NIR. `val<T>` reaches the tracer
     only through the abstract `tracing::TracingInterface`, set per trace,
     so the new tracer is a second implementation of that interface; it
     maps today's `Type`/`Op` to widths and flags at that boundary (PR 3).
     The interface itself gains `traceExtOp`, `traceData`, `traceBinding`
     and the explicit-construct hooks, with no-op defaults in the 0.3
     `TraceContext`; the user-facing headers of README §5 do not change;
   - the conversion (PR 4) and the NIR backends (PRs 5, 7, 8).

   Each is tested on its own (§3 rule 6). A test-only fixture, `NirChain`,
   runs the existing execution-test functions through new tracer →
   conversion → `tbc`, so the chain is exercised on real programs before
   the engine uses it. The old pipeline keeps its behavior and keeps
   running every existing test; only M1.8 (allocator), M1.9 (`bc`), M2.1
   (the shared `tag/Trie.hpp` and `symbolic_execution/`) and the
   `TracingInterface` additions above touch it.
2. **The cut-over (PR 6),** as soon as the chain new tracer → conversion →
   legalization → `tbc` passes every execution test in `NirChain`:
   - the engine switches to the new pipeline;
   - the old tracer implementation, `ExecutionTrace`, the trace-to-IR
     conversion, `compiler/ir/` (`IRGraph`, operations, passes,
     `IntrinsicRegistry`) and every backend's 0.3 lowering are deleted;
   - backends whose NIR port has not landed yet are switched off in CMake
     with a message and their sources excluded from the build, until
     their port PR (MLIR and the `inlining` plugin until PR 8, `asmjit`
     and `cpp` until PR 7 if it has not merged); the GPU plugin stays off
     until M7 ([17](17-gpu.md));
   - the engine-side files that use the 0.3 IR (compilation pipeline,
     tiered compiler, backend interface, exception transport, the `tbc`
     backend entry) and the tests, benchmarks and tools that include 0.3
     tracer or IR headers are ported to NIR (M4.2 lists them);
   - `test/data` is regenerated in NIR format, in its own commit;
   - `NirChain` is deleted; the engine is the chain.
3. **Phase B, after the cut-over.** The remaining backends, frontend
   features, modules and tiering, reuse, semantics and extensions, each
   built once, on NIR only.

**What replaces side-by-side comparison with 0.3:** recorded baselines
(M1.10). A job on `nautilus-0.4` checks out `main` in the same pinned CI
container, builds it with the corpus driver (which uses only the public
engine API) and records 0.3's numbers, once at the branch point and then
on every refresh: corpus instruction counts and stage timings, IR op counts
after 0.3's passes, the per-function attributes 0.3 infers, `asmjit`
assembly of the corpus, `ExecutionBenchmark`, and, from M2.3, TPC-H
numbers built from nautilus-duck against `main`. "No slower
than 0.3", "op counts ≤ 0.3" and "no code-quality regression" are checked
against them. CI gates are callgrind instruction counts in that container,
expressed as ratios against the recorded 0.3 baseline of the same function;
absolute µs/op targets are checked on the reference machine at milestone
exits only (D9). Correctness needs no 0.3 at all: the differential fuzzer
has its own C++ oracle.

**What proves API compatibility:** `test/api-compat/`, a copy of 0.3's
user-API tests and the `example/` programs, taken after `bc` is removed
(M1.9) and never edited afterwards (M1.10). A test is copied only if it
includes public headers only, does not name a backend string and does not
parse IR or dump text; the suite's own harness runs every test on every
enabled backend, and each example on the backends it supports (an example
that pins a backend runs only when that backend is enabled; its source is
not changed). The GPU tests and `example/src/DemoGPU.cpp` are copied
too but enabled only from PR 18 (M7.7). The suite must compile and pass
unchanged from the cut-over on, on every enabled backend.

## 3. Ground rules

1. **The branch always builds**, and its component tests pass. Before the
   cut-over, every existing test also passes (the old pipeline keeps its
   behavior, §2). From the cut-over on, every test passes on every enabled
   backend, and `test/api-compat/` passes.
2. **The spec comes first.** A WP that finds a gap or error in
   [13-nir-spec.md](13-nir-spec.md) changes the spec in the same PR, before
   the code. After the M1 exit (spec final), every change follows §11 of the
   spec.
3. **One opcode table.** Opcodes, their operand shapes, flags, verifier
   rules and text names are defined once in `nir/Opcodes.def` (an X-macro
   table). The verifier, printer, parser, C API enum and backend dispatch
   tables are generated from it. This shrinks the `ir-change` checklist and
   keeps the C enum in step with the wire values ([09 §3](09-c-api.md)).
4. **Reference dumps change only in isolated commits,** never together with
   a behavior change (`regenerate-dumps` skill).
5. **CI legs added by this plan:**

   | Leg | From | What it runs | Gates |
   |---|---|---|---|
   | `pr.yml` matrix, benchmark bot | PR 2 | the existing `main` matrix and `benchmark.yml`, retargeted to also run on `nautilus-0.4` (M1.11) | always |
   | `ubsan` | PR 2 | the test suite with `ENABLE_UB_SANITIZER`, a leg of its own beside ASan and TSan (they are mutually exclusive in `CMakeLists.txt`) | always |
   | `nir-components` | PR 1 | `nautilus-nir-tests`: unit tests, `.nir` text tests, golden images | always |
   | `nir-chain` | PR 4 | execution-test functions through `NirChain`, plus the fuzzer replay and corpus op-count `NirChain` modes (M3.1) | until the cut-over, then replaced by the normal matrix |
   | `api-compat` | PR 6 | `test/api-compat/` on every enabled backend (GPU tests from PR 18) | from the cut-over |
   | `latency-corpus` | PR 2 | corpus under callgrind in the pinned container (single-tier, ASLR off, CPU dispatch pinned, [10 §2](10-observability.md)), as instruction-count ratios against the recorded 0.3 baselines (gated); wall clock (reported) | from the cut-over (before: reports the 0.3 pipeline) |
   | `baselines-0.3` | PR 2 | on `nautilus-0.4`: checks out `main`, builds it in the same container with the corpus driver and refreshes `test/baselines/0.3/` | — |
   | `nebulastream-0.4` | PR 2 | NebulaStream against `nautilus-0.4` (D16) | from the M4 exit (before: reports) |
   | `tpch-duck` | PR 9 | nautilus-duck TPC-H tracing and execution | from the M4 exit (before: reports) |
   | `stencil-check` | PR 5 | `tools/stencils/build_stencils.py` output matches the checked-in `tbc` stencil tables for all three targets | always |
   | `asm-diff` | PR 7 | `asmjit` assembly of the corpus against the 0.3 baseline | from PR 7 or PR 6, whichever is later (before the cut-over it would diff 0.3 against itself) |
   | `nebulastream-cache` | PR 13 | the NebulaStream suite cold, then warm, with `ENABLE_CACHE_PLUGIN=ON` (M5.7, [08 §5.1](08-caching.md)), manual or nightly | reports; the M5 exit is checked on the reference machine |
   | `llvm-ir-test` and the inlining-plugin step (existing) | — | MLIR reference IR and the `inlining` plugin suite | off from PR 6 to PR 8 (M4.5), while MLIR is switched off |
   | `doc-check` | PR 2 | paths, targets, options and skills named in `AGENTS.md` and skills exist | always |
   | GPU legs (existing) | — | the three `-DENABLE_GPU_PLUGIN=ON` legs | build without the plugin from PR 6 to PR 17 ([17 §3](17-gpu.md)) |
   | `gpu-metal` (existing macOS leg) | PR 17 | the Metal suite on the hosted macOS runner, with Metal API and shader validation | from PR 17 |
   | `gpu-emulator` | PR 17 | the GPU suite on the CPU grid emulator ([17 §5.1](17-gpu.md)) | from PR 17 |
   | `cuda-compile` | PR 17 | NVRTC to PTX and `ptxas` for every test kernel, with the CUDA toolkit and no GPU | from PR 17 |

6. **How components are tested before they are wired together:**
   - NIR and passes: `.nir` text tests run through `nautilus-nir-opt`
     (parse → pass → print, compared with expected text);
   - backends: `nautilus-nir-run` executes a `.nir` file on one backend with
     given arguments and compares the result;
   - the tracer: every execution-test function traced with the new tracer;
     pre-SSA output verified and compared with golden pre-SSA dumps;
   - the chain: `NirChain` (rule 5).
7. **Agent docs are part of the change.** A WP that adds, moves or renames a
   component updates the `AGENTS.md` that describes it and the affected
   skills ([15 §3.1](15-agent-experience.md)).
8. **Measure against the baselines.** Every WP with a performance claim
   records the corpus numbers against the recorded 0.3 baselines in its PR
   description: callgrind instruction-count ratios from the pinned CI
   container for every PR; µs/op from the reference machine at milestone
   exits (D9).
9. **NebulaStream.** From the M4 exit on, a PR that turns the
   `nebulastream-0.4` job red ships the NebulaStream patch (a NebulaStream
   branch referenced from the PR) and merges only with the job green.
   Because the user-facing API does not change, this is expected only for
   behavior changes and for NebulaStream's own plugins.
10. **Merges from `main`.** Weekly, as in README §6. After the cut-over, a
    fix on `main` to code that no longer exists is re-implemented on NIR or
    recorded as obsolete; every fix on `main` brings a regression test,
    which carries over unchanged.

## 4. Pull requests

Every PR targets `nautilus-0.4`, is titled `[PR n] <capability>`, and merges
only when rule 1 holds and every WP in it meets its "done when". PRs may be
opened as drafts early and kept up to date with the branch.

| PR | Capability | Work packages | Needs | Effort (eng-weeks) |
|---|---|---|---|---|
| 1 | NIR format: data structures, verifier, text form, option registry, canonical form and hash, binary image, loader fuzzer | M1.1–M1.6 (with M1.4a) | — | 10–23 |
| 2 | Foundations and infrastructure: extension registry with backend-independent intrinsic identities, extensions in the text form, canonical form and images, chunk pool, `bc` removed, corpus and 0.3 baselines, API-compat suite, CI on `nautilus-0.4`, agent items | M1.3a, M1.7, M1.7a, M1.8–M1.12 | 1 | 7–20 |
| 3 | The NIR tracer | M2.0, M2.1 | 2 | 6.5–11 |
| 4 | Optimizing conversion; `NirChain` | M3.1–M3.6 | 3, 5 | 8–21 |
| 5 | Backend interface, `nautilus-nir-run`, `tbc` on NIR | M4.0, M4.1 | 2 | 4–9 |
| 6 | **Cut-over:** legalization; the engine on the new pipeline; 0.3-dependent tests and tools ported; the 0.3 pipeline deleted | M3.7, M4.2 | 4, 5 | 4–9 |
| 7 | `cpp` and `asmjit` on NIR | M4.3, M4.4 | 5 | 6.5–11 |
| 8 | MLIR on NIR | M4.5 | 6 | 3–6 |
| 9 | Frontend features: cheaper re-runs, TPC-H job and fewer copies, tracing diagnostics, explicit control flow, constant data; M2 gate | M2.2–M2.6, M2.8 | 6 | 7–17 |
| 10 | Optional passes, plugin pass hooks, M3 gate | M3.8–M3.10 | 6 | 4–8 |
| 11 | Modules and linking, promotion service, debug info and profiling, `explain()`; M4 exit | M4.6–M4.10 | 7, 8, 9, 10 | 6.5–16 |
| 12 | Portable reuse: pointers, bindings, provenance, self-contained images, save/load | M5.1–M5.5 | 11 | 4–11 |
| 13 | Native payloads and caches L0/L1/L2; warm NebulaStream harness; compatibility fixtures (images and artifacts with payloads) | M5.6, M5.7, M5.9 | 12 | 6.5–13 |
| 14 | External interfaces: C API, playground MCP server | M5.8, M5.10 | 12 | 4–9 |
| 15 | Semantics: D10/D11 folding, declared effects, cheaper exceptions, plugin API on widths | M6.1–M6.4 | 11 | 3–8 |
| 16 | Extension types and plugin migration; diagnostics catalog and compiled docs | M6.5–M6.7 | 11 | 7–15 |
| 17 | GPU core ([17](17-gpu.md)): device ops, launch, partition, runtime, memory, errors, grid emulator, CUDA and Metal device compilers | M7.1–M7.4 | 10, 11, 15 | 13–25 |
| 18 | GPU production features and validation: shared memory, atomics, warp ops, math; device-code caching, artifacts carrying the NIR image; Metal benchmarks and validation tooling, `docs/gpu.md` | M7.5–M7.7 | 13, 17 | 5–12 |
| 19 | Documentation, migration guide, release validation, merge and tag | M8.1–M8.3 | 13–18 | 2.5–7 |

**PR 1 status.** M1.1–M1.6 are implemented on `claude/nir-format-pr1` (code in `nautilus/src/nautilus/nir/`,
tests in `nautilus/test/nir-tests/`, conformance material in `nautilus/test/data/nir-conformance/`). Where the code
differs from the work packages above:

- **Load is not zero-copy yet** (M1.5): `loadImage` copies the sections into the `Module` vectors. Zero-copy needs a
  `Module` that can hold spans over a buffer; it is deferred to the PR that first needs the saving (PR 12, M5.5).
- **`OptionRegistry` is compiled into `nautilus-nir`** (M1.4a), not into `nautilus`, until the cut-over links the two;
  `options.hpp` is untouched.
- **Debug sections** are read and written but dropped by `canonicalize` (M1.4): remapping them needs the debug
  producers of M4.9.
- **Not in this PR** (they depend on the registry of PR 2): extension productions in the text form (M1.3a), extension
  checks `NIR-V070`–`V072`, `NIR-L010`–`L012`, `NIR-L015`, and `extension` lines. Extension types print as
  `!ext.t<N>` placeholders and the parser does not read them.
- **Bundling** (§8.5) is M5.4: `writeImage` serializes imports as they are, which the loader then rejects
  (`NIR-V038`); the hash input of such a module (§7.3) is written the same way.
- **Fuzzing:** `nautilus-nir-fuzz` (libFuzzer) and `nautilus-nir-fuzz-replay` (any compiler, deterministic mutation).
  Four crashes were found and fixed while writing them (see the commit messages); the one-CPU-hour exit of M1.6 is to be
  confirmed by the first weekly sweep.

M2.7 (lazy copies) is an investigation; if it is adopted, it lands as its
own PR after PR 9.

**Before the cut-over,** PRs 1–5 and 7 can land: they add code the engine
does not call yet (PR 7's backends are switched on by PR 6). After PR 2,
PRs 3 and 5 run in parallel; PR 4 needs both; PR 7 runs beside PR 4.

## 5. Source layout at the end of 0.4

| Path | Content | PR |
|---|---|---|
| `nautilus/src/nautilus/nir/` | `Opcodes.def`, `Types`, `Inst`, `Module` (sections), `ConstPool`, `Builder`, `Rewriter`, `Verifier`, `Diagnostics.def`, `Printer`, `Parser`, `Canonicalize`, `Hash`, `Image` (writer, loader, inspect) | 1 |
| `nautilus/src/nautilus/common/OptionRegistry.*` (new) | the typed option registry (`OptionSpec`) | 1 |
| `nautilus/src/nautilus/extensions/` | `ExtensionRegistry`, `OpInfo`/`TypeInfo`, lowering-table registration, `CoreIntrinsics.cpp` (core `mem*` identities) | 2 |
| `plugins/{std,builtin,simd,specialization}/src/*Intrinsics.cpp` (new, one per plugin) | backend-independent intrinsic identities: `name@version`, NIR signature, effect attributes, fallback address; compiled in every configuration | 2 |
| `nautilus/src/nautilus/common/ChunkPool.*` | the bounded chunk pool and its arenas | 2 |
| `nautilus/src/nautilus/tracing/nir/` | the NIR tracer (`TracingInterface` implementation), tag map, replay cursor, explicit control flow; `tracing/tag/` and `Snapshot.*` are shared and kept; `symbolic_execution/` is ported in M2.1 (tag map replaced by `tracing/nir/TagMap`, no dependency on `ExecutionTrace`) | 3 |
| `nautilus/src/nautilus/conversion/` | optimizing conversion, fold/simplify rules, CSE, landing pads, attribute inference | 4 |
| `nautilus/src/nautilus/passes/` | legalization; optional copy passes (LICM, strength reduction, inlining); plugin pass hooks | 6, 10 |
| `nautilus/src/nautilus/compiler/backends/*` | the backends, reading NIR; `FunctionStubs.*` for `funcaddr` | 5, 7, 8 |
| `nautilus/src/nautilus/compiler/PromotionService.*` | tier-1 workers, jobs, status | 11 |
| `nautilus/src/nautilus/link/` | module registry, import slots | 11 |
| `nautilus/src/nautilus/serialization/` | pointers, bindings, provenance, artifacts, self-contained export | 12, 13 |
| `plugins/cache/` | L0/L1/L2 cache provider (`ENABLE_CACHE_PLUGIN`) | 13 |
| `nautilus/include/nautilus/c/` | `common.h`, `ir.h`, `engine.h` | 14 |
| `plugins/gpu/src/` | `gpu` extension ops, `gpu.partition` pass, shared NIR emitter, CUDA (NVRTC) and Metal device compilers, `runtime/` | 17, 18 |
| `nautilus/include/nautilus/control_flow.hpp`, `data.hpp` | explicit control flow; `constData` | 9 |
| `nautilus/test/nir-tests/` | unit tests, `text/` (`.nir` tests), `trace/` (golden pre-SSA dumps), `compat/` (D12 fixtures) | 1, 3, 13 |
| `nautilus/test/data/nir-conformance/` (new) | the conformance material of [13 §11](13-nir-spec.md): golden images (`images/`), the annotated hex dump, the round-trip `.nir` corpus, canonical forms and hashes, per-code tests | 1 |
| `nautilus/test/api-compat/` | 0.3's user-API tests and examples, frozen | 2 |
| `nautilus/test/baselines/0.3/` | recorded 0.3 baselines | 2 |
| `nautilus/test/benchmark/latency-corpus/` (new) | the latency-corpus driver (public engine API only) | 2 |
| `tools/nautilus-nir-opt/`, `tools/nautilus-nir-run/` | text-form pass driver; `.nir` runner per backend | 1, 5 |
| `tools/agent/` | `affected-tests` | 11 |
| `tools/claude-plugin/` (new) | the user plugin and its skills | 19 |
| `third_party/xxhash/` | XXH3-128 for the structural hash (BSD-2) | 1 |

**Deleted:**
- PR 2: `compiler/backends/bc/`, `tools/bc_guard.sh`,
  `tools/bc_perf_verify.py`, `ENABLE_BC_BACKEND`, `dump.after_bc_generation`;
- PR 6: `tracing/TraceContext.*` (the old implementation),
  `tracing/ExecutionTrace.*`, `tracing/TraceOperation.*`, `tracing/Block.*`,
  `tracing/phases/`, all of `compiler/ir/` (including `IntrinsicRegistry`
  and `IRLocationMap`), `compiler/Frame.hpp`, every backend's 0.3 lowering
  provider (including `plugins/gpu`'s `GPULoweringProviderBase` and its
  CUDA and Metal providers), the 0.3 versions of the plugin AsmJit and
  MLIR intrinsic handlers (`plugins/simd/src/AsmJitVectorIntrinsics.*`,
  `plugins/builtin/src/prefetch/AsmJitPrefetchIntrinsics.*`,
  `plugins/*/src/MLIR*Intrinsics.*`; their NIR versions come with M4.4
  and M4.5),
  `IRPassBenchmark`, `test/ir-pass-tests/`, the `ir::IRGraph` forward
  declaration in `include/nautilus/JITCompiler.hpp`, the `NirChain`
  fixture.

**Ported at the cut-over, not deleted (M4.2):**
`compiler/CompilationPipeline.*`, `compiler/TieredCompiler.*`,
`compiler/backends/CompilationBackend.*`,
`compiler/backends/CapturedExceptionTransport.*`, `tbc/TBCBackend.*`;
`TracingTest.cpp`
(`nautilus-tracing-tests`), `RegionTest.cpp`,
`NoThrowInferenceIntegrationTest.cpp`, `InvokeExceptionExecutionTest.cpp`,
`tracing-tests/AliveVariableHashTest.cpp`, `tracing/CopyAttribution.*`,
`benchmark/TracingBenchmark.cpp`,
the playground runner (`tools/playground/runner/`, `web/src/lib/traceSections.ts`)
and `tools/vscode-nautilus-ir`.

**Switched off at the cut-over, ported later:** the rest of
`compiler/backends/mlir/` (`MLIRCompilationBackend`, `MLIRBackendIntrinsic`,
`PropagatePersonalityPass`, `debug/`, `jit/`) and `plugins/inlining` until
PR 8 (M4.5); `compiler/backends/asmjit/` (`AsmJitCompilationBackend`,
`AsmJitRegister.hpp`, `intrinsics/AsmJitBackendIntrinsic`) and
`compiler/backends/cpp/` until PR 7 (M4.4, M4.3) if it has not merged;
`plugins/gpu` with its public `include/nautilus/gpu/backends.hpp`,
`src/gpu_backends.cpp`, the CUDA and Metal compilation backends and
`GPUTracingTest`/`GPUCodegenTest` until PRs 17–18 (M7.1, M7.3, M7.4).
Their sources are excluded from the build in every preset while they are
off.

## 6. Order, dependencies and tracks

```
PR 1 NIR format ─► PR 2 foundations, infra ─┬─► PR 3 NIR tracer ─► PR 4 conversion ─┐
                                            │                        ▲              │
                                            └─► PR 5 nir-run, tbc ───┴──────────────┤
                                                 └─► PR 7 cpp, asmjit (dormant)     ▼
                                                                  PR 6 CUT-OVER (tbc, + PR 7)
                                                                              │
                         ┌───────────────┬───────────────┬────────────────────┤
                         ▼               ▼               ▼                    ▼
                    PR 8 MLIR       PR 9 frontend   PR 10 passes       (PR 7 if later)
                         └───────────────┴───────────────┴─────► PR 11 modules, tiering; M4 exit
                                                                              │
                         ┌────────────────┬────────────────┬──────────────────┤
                         ▼                ▼                ▼                  ▼
                    PR 12 portable   PR 15 semantics  PR 16 ext.       (PR 14 after 12)
                         ▼                ▼
                    PR 13 native,    PR 17 GPU core (also needs 10) ─► PR 18 GPU prod ─► PR 19 release
                    caches ───────────────────────────────────────────────┘ (18 needs 13)
```

**Critical path,** from the WP sizes and dependencies (§15 has the sums):
PR 1 (M1.1, M1.2) → PR 2 (M1.7) → PR 3 (M2.1) → PR 4 (M3.1, M3.2/M3.6,
M3.4/M3.5) → PR 6 (M4.2) → PR 8 (M4.5) → PR 11 (M4.7, M4.9, M4.10) → PR 15
(M6.1) → PR 17 → PR 18 (M7.5, M7.7) → PR 19 (M8.2, M8.3). The `tbc` path
PR 1 → PR 2 → PR 5 → PR 4 is near-critical (1–2 weeks shorter), because
M4.0 needs only the parser (M1.3) and the registry (M1.7), not the image.
The reuse chain (PRs 12, 13) joins the GPU chain at M7.6 and is shorter by
5–7 weeks. PR 7 runs beside PR 4; PRs 9, 10, 14 and 16 run beside the
reuse and GPU chains.

**Tracks** (who can work in parallel):

| Track | PRs |
|---|---|
| A: format and reuse | 1, 12, 13, 14 |
| B: frontend | 3, 9 |
| C: conversion and passes | 4, 6 (legalization), 10 |
| D: backends | 5, 7, 8 |
| H: GPU | 17, 18 |
| E: runtime | 2 (pool, `bc`), 11 |
| F: semantics and extensions | 15, 16 |
| G: infra and agents | 2 (corpus, baselines, CI), agent items in 11, 14, 16, 19 |

## 7. M1 Foundations

Goal: NIR exists, is specified, verified, printed, parsed and serialized;
the measuring and compatibility infrastructure runs. Exit criteria:
README §6 and [02 §8](02-nir.md).

#### M1.1 NIR core data structures · L · deps: —
- **Builds:** `Inst` (16 B), value references with the constant bit and
  `NONE`, `extra`, `ConstPool` with interning and a provenance-class field,
  every section of [13 §4.1](13-nir-spec.md) as flat arrays, `Module`,
  `Function`, `BlockRecord`; `Builder` (appends, mints dense ids, maintains
  `useCount`); `Rewriter` (copy with old→new map, `forward[]`); CSR use
  table on request. Signless types and op flags ([13 §2](13-nir-spec.md));
  signature records whose type words carry the per-parameter and result
  extension kind (none / zext / sext, [13 §4.5.1](13-nir-spec.md));
  function string attributes (section 25, [13 §4.5.2](13-nir-spec.md));
  the optional debug sections, including the debug function records and
  parameter names (sections 26–27, [13 §4.9](13-nir-spec.md)); the
  pre-SSA-only `var` opcode ([13 §5.9](13-nir-spec.md)).
- **Files:** `nir/Opcodes.def`, `Types.hpp`, `Inst.hpp`, `Module.*`,
  `ConstPool.*`, `Builder.*`, `Rewriter.*`; CMake target `nautilus-nir`;
  the `nir-components` leg in `pr.yml` (§3 rule 5).
- **Tests:** `nautilus-nir-tests`: record sizes (`static_assert`), builder
  invariants, interning, rewriter compaction, CSR uses.
- **Done when:** a module using every opcode of `Opcodes.def` is built and
  rewritten in a test; no hash map on a per-operand path.

#### M1.2 Verifier, both levels · M · deps: M1.1
- **Builds:** every rule of [13 §6](13-nir-spec.md) and the per-opcode rules
  from `Opcodes.def`; level-specific checks; the stable codes of the
  [13 §6.6](13-nir-spec.md) table (`NIR-V001`…), with messages naming the
  code, function, and instruction or block id as printed in the text form.
  Checks of extension instructions are added by M1.7.
- **Files:** `nir/Verifier.*`, `nir/Diagnostics.def`.
- **Tests:** one positive and one negative test per atomic check and per
  opcode.
- **Done when:** every rule of [13 §6](13-nir-spec.md), split into atomic
  checks, has a stable `NIR-V` code in `Diagnostics.def` that matches the
  code table of [13 §6.6](13-nir-spec.md), and each code is triggered by a
  positive and a negative test, kept with the conformance material
  (`nautilus/test/data/nir-conformance/`, [13 §11](13-nir-spec.md)).

#### M1.3 Printer, parser, `nautilus-nir-opt` · M · deps: M1.2
- **Builds:** the text form of [13 §9](13-nir-spec.md) for both levels:
  the lexical structure and normative grammar (EBNF) of §9.1–§9.2, the
  deterministic printing rules of §9.3, the parsing rules of §9.4 with the
  `NIR-P` codes of [13 §6.6](13-nir-spec.md), and the round-trip laws of
  §9.5, for the core language (the extension productions are added by
  M1.3a); a parser with line and column in errors; `tools/nautilus-nir-opt` (parse → optional
  pass → print).
- **Tests:** print → parse → print byte-identical on every test module;
  `.nir` tests under `test/nir-tests/text/`; one test per grammar
  production (provenance marks, float spelling, `prob`, callee records).
- **Done when:** a test is a `.nir` file plus expected output, run by ctest;
  every core production of the 13 §9.2 grammar is exercised by a test,
  and every `NIR-P` code by a positive and a negative test; the
  round-trip corpus of `.nir` texts (§9.5) is committed under `nautilus/test/data/nir-conformance/`
  ([13 §11](13-nir-spec.md)).

#### M1.3a Extensions in the text form · S · deps: M1.3, M1.7
- **Builds:** the extension productions of [13 §9.2](13-nir-spec.md):
  `extension "name@version"` declarations checked against the registry
  (§9.4, `NIR-P005`), intrinsic callee targets, extension types, and
  extension instructions printed and parsed through their `OpInfo` operand
  shapes ([13 §5.11.1](13-nir-spec.md)). It is split from M1.3 so that
  the printer and parser do not wait for the registry, which would put
  the `tbc` path (M4.0, M4.1) on the critical path.
- **Files:** `nir/Printer.*`, `nir/Parser.*`.
- **Done when:** every extension production and `NIR-P005` are exercised
  by `.nir` tests with a test extension; `.nir` texts with intrinsic
  callees and extension instructions are in the round-trip corpus.

#### M1.4 Canonical form and structural hash · M · deps: M1.3, M1.4a
- **Builds:** canonicalization of [13 §7](13-nir-spec.md) (structural
  equality, deterministic canonical form, the scan order of §7.1, for core
  NIR; extension entries and instructions are added by M1.7a); the
  structural hash of §7.2: XXH3-128 over the anonymous canonical image without header and debug
  sections (extension names and versions enter through its `names`
  section) and the code-affecting
  options of M1.4a, hashed as (name, canonical value encoding) sorted by
  name (options without `affectsCode` and unknown keys are not hashed);
  the derived keys of §7.3 (the unbundled L0 form, in which imports
  are hashed by name, signature and assumed attributes, and the key of
  code that inlined imports).
- **Files:** `nir/Canonicalize.*`, `nir/Hash.*`, `third_party/xxhash/`
  (vendored, pinned).
- **Tests:** equal modules built in different orders hash equally; renaming
  the module does not change the hash; changing a code-affecting option
  does; setting an option to its default does not; changing an option
  without `affectsCode` does not.
- **Done when:** two processes produce the same hash for the same module
  ([12 §3](12-validation.md), row 07); an importer's L0 key does not
  change when the exporter's body does; the canonical forms and hashes of
  the golden modules are committed under
  `nautilus/test/data/nir-conformance/` as conformance material of
  [13 §11](13-nir-spec.md).

#### M1.4a Typed option registry · S · deps: —
- **Builds:** a static registry of every option in `docs/options.md` as an
  `OptionSpec` (name, type, default, `affectsCode` flag, deprecated
  aliases, an optional `resolve` hook that maps the effective value to the
  value that is hashed, used by `gpu.device=auto` in M7.2); effective
  options = registry defaults overlaid with the set values, then mapped
  by the `resolve` hook if there is one, normalized to the value kinds of [13 §7](13-nir-spec.md)
  (`int` → `i64`); unknown keys are application options, which the
  structural hash does not cover (13 §7.2) and the L1 key hashes as
  strings ([08 §2](08-caching.md)).
  Code-affecting options are exactly those with `affectsCode`. Deprecated
  aliases warn once ([12 §2](12-validation.md));
  `engine.tiered.backgroundPromotion` is registered as a deprecated alias
  that maps to `engine.tiered.promotion=none`
  ([11 §4.7](11-tiered-compilation.md)).
  Options added later (`engine.mlir.targetCpu` in M5.6, cache options in
  M5.7, `gpu.device` in M7.2, code-affecting) are registered by the WP
  that adds them.
- **Files:** `common/OptionRegistry.*` (new); `include/nautilus/options.hpp`
  (additions only); `docs/options.md` (type and code-affecting column).
- **Done when:** every `docs/options.md` entry is in the registry and every
  registry entry is documented (checked by `doc-check` once M1.12 lands);
  setting an option to its default does not change the effective options.

#### M1.5 Binary image: writer, loader, inspect · L · deps: M1.4
- **Builds:** [13 §8.1–§8.3](13-nir-spec.md): layout, strides,
  feature bits, CRC-32C, framing, load limits; zero-copy load; `inspect`
  without materializing; loader rejections with the stable `NIR-L` codes of
  [13 §6.6](13-nir-spec.md). The header and section-table code is a
  component of its own, because the artifact container reuses the framing
  ([13 §8.6](13-nir-spec.md), M5.6). The format rules of #532 are carried
  over, its 0.3 format is not. Mapping extension names to process ids on
  load (§8.4) is added by M1.7a, bundling of imports on save (§8.5) by
  M5.4.
- **Files:** `nir/Image.*`, `nir/ImageLoader.*`, `nir/Inspect.*`.
- **Tests:** golden images (valid and invalid) under
  `nautilus/test/data/nir-conformance/images/`, read by
  `nautilus-nir-tests`; the annotated hex dump checked byte for byte;
  re-serialization byte-identical; every round-trip `.nir` text of M1.3
  parses to the same module as its image loads to.
- **Done when:** every golden image loads or is rejected exactly as listed,
  with the listed `NIR-L` code; the annotated hex dump (the second example
  of 13 §9, canonicalized, with its structural hash) and the golden-image
  list are produced by this WP and committed under
  `nautilus/test/data/nir-conformance/` as the conformance material of
  [13 §11](13-nir-spec.md) (they do not exist before it).

#### M1.6 Loader and verifier fuzzer · S · deps: M1.5
- **Builds:** a libFuzzer target over malformed images and mutated text,
  seeded with the golden images; run coverage-guided in the weekly sweep
  (`fuzz-weekly.yml`), and its seeds replayed in the PR job
  `fuzz-libfuzzer-smoke` of `pr.yml` (which runs only the seed corpus,
  `-runs=0`); `fuzz-triage` skill.
- **Files:** `test/fuzz/NirImageFuzz.cpp`.
- **Done when:** one CPU-hour without a crash or sanitizer report; the
  weekly sweep runs the target and the PR smoke job replays its seeds.

#### M1.7 `ExtensionRegistry` and intrinsic identities · M · deps: M1.2
- **Builds:** one registry for ops, types, intrinsics and passes with dense
  ids ([04 §3](04-extensions.md)); frozen at the first `NautilusEngine`
  construction, image load, text parse or C-API module creation (release
  store; registration checks the flag under a mutex); lock-free lookups after the freeze; late registration throws
  (D13); extension type ids limited to 240; names split at the last `@`;
  the plugin-name rule of [13 §9.1](13-nir-spec.md) enforced at
  registration: no core mnemonic of §5 or first dot-separated component of
  one (`div`, `cmp`, `ptr`, …), no type name of §2, no keyword of §9.2,
  not `nir`; the prefix `core` reserved for the core's own intrinsics
  (`core.memcpy@1` is valid, a plugin under `core` is rejected); `OpInfo` with its operand shape
  ([04 §3.3](04-extensions.md), [13 §5.11.1](13-nir-spec.md));
  extension checks in the verifier (signature, operand shape and `verify`
  hook, naming the plugin). **Intrinsic identity is
  backend-independent:** each in-tree plugin (`std`, `builtin`, `simd`,
  `specialization`) and the core (`mem*`) gets a backend-neutral
  registration TU compiled in every configuration, which registers each
  intrinsic by `name@version` with its NIR signature, effect attributes and
  native fallback, and, for an intrinsic without `noUnwind`, a capture
  thunk (`captureThrowingCall<R, Args...>`) that the registration helper
  instantiates from the fallback's C++ type and stores in the
  registration ([13 §4.6.1, §5.7](13-nir-spec.md)); backend handlers are optional and attach by name. A
  traced `invoke` of a registered fallback address becomes an intrinsic
  callee at trace time (lookup by address → name). Until the cut-over the
  0.3 backends keep their own handler registration with 0.3's
  `IntrinsicRegistry`. **No public shim:** the plugin API is not part of
  the compatibility promise (§2).
- **Files:** `extensions/ExtensionRegistry.*`, `extensions/OpInfo.hpp`,
  `extensions/TypeInfo.hpp`, `extensions/CoreIntrinsics.cpp`;
  `plugins/{std,builtin,simd,specialization}/src/*Intrinsics.cpp` (new)
  and their `CMakeLists.txt`, outside the `ENABLE_MLIR_BACKEND` guards.
- **Tests:** registration, freezing, late registration, in a test binary of
  their own with a test-only `ExtensionRegistry::resetForTesting()`; TSan
  plus a lock-count check on lookups; registration racing the first engine
  construction; image load, text parse and C-API module creation also
  freeze the registry ([04 §3.7](04-extensions.md)); each class of
  reserved plugin name rejected, the core's
  `core.*` intrinsics accepted; an extension instruction
  violating each operand-shape rule rejected by the verifier; `NIR-V014`
  positive and negative tests for intrinsic calls (an intrinsic call that
  names a thunk is rejected; a may-throw intrinsic registered with a
  thunk is accepted); a `NIR-V035` negative test with `funcaddr` of an
  internal function whose signature has a test extension type without
  `cabi` ([13 §4.5, §5.7, §6.1.8](13-nir-spec.md)); every
  in-tree intrinsic visible by name, in a visibility test that links only
  each plugin's public API (registration TUs anchored, not dropped from
  the static archive).
- **Done when:** [04 §5](04-extensions.md) M1 criterion: every in-tree
  intrinsic is visible through the registry in every preset, including
  `dev` and `nocompiler`; no mutex on lookups after the freeze.

#### M1.7a Extensions in canonical form and images · M · deps: M1.5, M1.7
- **Builds:** canonicalization of extension entries, types and
  instructions through their operand shapes (`value` and `type` words
  renumbered, constants and pointers ordered through `const`, `value` and
  `pointer` words, `imm` words copied; [13 §5.11.1, §7.1](13-nir-spec.md)),
  so extension names and versions enter the structural hash; mapping
  extension `name@version` to process ids on load and back on save
  ([13 §8.4](13-nir-spec.md)); a missing or incompatible extension rejects
  the image with a `NIR-L` code.
- **Files:** `nir/Canonicalize.*`, `nir/ImageLoader.*`, `nir/Image.*`.
- **Done when:** golden images with extension ops and types round-trip
  and are added to the conformance material with their hashes; the same
  extension module built in two operand orders hashes equally; an image
  naming an unregistered extension is rejected as listed; an image with a
  test type instance registered as unavailable on the host in an exported
  signature fails `NIR-V035` (`NIR-L017`) on load
  ([13 §8.4](13-nir-spec.md)).

#### M1.8 Bounded chunk pool · M · deps: M1.10 (for its corpus check)
- **Builds:** [02 §4.1](02-nir.md): fixed chunk sizes, dedicated large
  allocations, per-thread cache plus engine pool, byte budget with decay,
  `mmap`/`madvise` release, `NautilusEngine::trimMemory()`, embedder chunk
  provider, counters. Replaces both `ArenaPool`s; the 0.3 pipeline uses it
  until the cut-over, the new one after.
- **Files:** `common/ChunkPool.*`, `common/Arena.*`;
  `engine.arena.retainedBytes` in `docs/options.md`.
- **Tests:** budget, decay, trim, page release; the 40,000-module
  long-running test on counters.
- **Done when:** after a 20,000-op compile, held bytes return to the budget;
  no corpus regression against the baseline.

#### M1.9 Remove `bc` · S · deps: —
- **Builds:** [05 §6.1](05-backends.md): confirm `tbc` passes every test
  `bc` passes and is no slower on `ExecutionBenchmark`, then delete;
  tier-0 fallback `asmjit` → `tbc` → interpreter; `bc` cases move to `tbc`;
  `example/src/DemoBackends.cpp`, `docs/engine.md`, `docs/options.md`,
  `.github/nebulastream/benchmark.toml`; `third_party/dyncall` stays (used
  by `tbc`).
- **Done when:** no reference to `bc` remains outside the change log and the
  migration notes; `engine.backend=bc` fails naming `tbc`.

#### M1.10 Corpus, 0.3 baselines, API-compat suite, reference machine · M · deps: M1.9, M1.11
- **Builds:**
  - the corpus driver from the design study (closed #562) as
    `nautilus/test/benchmark/latency-corpus/` with JSON output, using only
    the public engine API, and the `latency-corpus` leg (§3 rule 5),
    reporting until the cut-over;
  - the `baselines-0.3` job (§2) on `nautilus-0.4`: it checks out `main`,
    builds it in the same pinned container with that driver and writes
    `test/baselines/0.3/`; besides the corpus numbers it records the
    per-function inferred `FunctionAttributes` (`noUnwind`, `willReturn`,
    memory effects) of every execution-test function, mapped to the
    [13 §4.6.2](13-nir-spec.md) bits, for M3.6, and the per-function IR op
    counts after 0.3's passes of every execution-test function, for the
    `NirChain` checks of M3.2 and M3.3 (TPC-H numbers are added by
    M2.3);
  - `test/api-compat/`, copied after M1.9: the test cases of 0.3's
    execution and value tests that include public headers only, do not
    name a backend string and do not parse IR or dump text, with the
    function headers of `test/common/` they use and a harness (from
    `test/common/ExecutionTest.hpp`) that runs them on every enabled
    backend; and `example/`, where an example is copied only if it pins
    no backend, or if `test/api-compat/CMakeLists.txt` records the backend
    it pins and the harness runs it only when that backend is enabled
    (sources unchanged; `DemoJit.cpp` and `DemoRegions.cpp` pin `cpp`,
    the `simd`, `specialization`, inlining and profiling demos pin
    `mlir`). Left out by the
    rule, among others:
    `CompilationStatisticsTest.cpp`, `DebugInfoExecutionTest.cpp`,
    `PerfJitDumpExecutionTest.cpp`, the dump checks of
    `ABIExtensionAttributeTest.cpp`, and the test cases that branch on a
    backend name. The GPU tests and `example/src/DemoGPU.cpp` are copied
    but disabled until M7.7, except the two "CPU fallback" sections of
    `plugins/gpu/test/GPUExecutionTest.cpp`, which launch 256 threads over
    3-element arrays and would write out of bounds on the full-grid `cpu`
    device; they are rewritten in M7 and stay outside the suite
    ([17 §5.2](17-gpu.md));
  - the dedicated reference machine set up and recorded
    ([10 §2](10-observability.md)); it has to be procured and needs an owner
    (§16). On it, 0.3's mean time to tier 1 on the NebulaStream suite is
    recorded for [11 §7](11-tiered-compilation.md); if the machine is not
    ready by the M1 exit, this is recorded at the M4 exit by running
    `main`.
- **Done when:** the P0 numbers are reproduced and stored as the baseline;
  two runs of the same container image give identical instruction counts
  ([10 §5](10-observability.md)); `test/api-compat/` builds and passes on
  the 0.3 pipeline without `bc`.

#### M1.11 CI on `nautilus-0.4` · M · deps: —
- **Builds:**
  - `nautilus-0.4` added to the `push`/`pull_request` branch filters of
    `pr.yml` and `benchmark.yml`; benchmark data for the branch stored
    under its own key;
  - `nebulastream.yml` parameterized on the branch, its nightly schedule
    also run for `nautilus-0.4`; reporting only until the M4 exit (§3
    rule 9);
  - a `ubsan` preset in `CMakePresets.json` and a UBSan leg in `pr.yml`,
    separate from the ASan and TSan legs;
  - the pinned CI container image (valgrind, compilers) in which the
    `latency-corpus` and `baselines-0.3` jobs run (D9,
    [10 §2](10-observability.md)).
- **Done when:** a PR against `nautilus-0.4` runs the full `pr.yml` matrix
  (with the ASan, UBSan and TSan legs) and the benchmark bot; the
  NebulaStream job runs on every `nautilus-0.4` push.

#### M1.12 Agent items for M1 · M · deps: M1.3
- **Builds:** `doc-check` script and CI leg; `ir-change` skill rewritten for
  NIR (pointing at `Opcodes.def` and the spec); `steward` skill (PR
  conventions for this branch).
- **Done when:** `doc-check` passes and fails on a planted stale path.

**M1 exit:** M1.1–M1.12 (with M1.3a, M1.4a and M1.7a) done; the spec is
marked final ([13 §11](13-nir-spec.md)).

## 8. M2 Frontend

Goal: a tracer that writes pre-SSA NIR and is cheaper; explicit control
flow mixable with implicit. Exit criteria: [01 §7](01-tracing-frontend.md).

#### M2.0 Tracing statistics and counters · S · deps: M1.1
- **Builds:** #561's tracing statistics plus R1 (follow, record and paused
  time, `tracing.pausedOperations`, per-scope re-runs) and C1 (copies per
  kind) in the new tracer, with the same keys as the baselines.
- **Done when:** the tracing keys of [10 §3](10-observability.md) are
  reported and unit-tested.

#### M2.1 The NIR tracer · XL · deps: M1.2, M1.7, M2.0 (and the printer of M1.3, M1.3a for its golden dumps, needed only at its end)
- **Builds:** a `TracingInterface` implementation writing `FlatTrace`
  ([01 §6](01-tracing-frontend.md)): 16 B records, value-ref operands,
  global op ids, blocks as arena lists of op ids; constants and copies
  recorded as `var` ([13 §5.9](13-nir-spec.md)), so the value a `val<T>`
  holds is always an instruction id at the pre-SSA level; the tag map as
  open addressing from snapshot to op id; the index-based replay cursor and
  cheaper alive-variable bookkeeping (value refs are op ids,
  [01 §3](01-tracing-frontend.md)); a `var` at a known tag records a
  reconciling `assign` onto the original id instead of merging (#95, #384;
  [01 §6](01-tracing-frontend.md)); hashed trie children in the shared
  `tag/Trie.hpp`; `traceExtOp`, `traceData`, `traceBinding` and the
  explicit-construct hooks added to `TracingInterface` (no-op defaults in
  the 0.3 `TraceContext`); destructor cleanup lists in unwind entries of
  calls **and of every integer `div`/`rem`** ([13 §3.4](13-nir-spec.md),
  [06 §4.2](06-semantics.md)); traced pointers as `pointers` entries with a
  provenance class, integer constants as `Literal`
  ([07 §5](07-serialization.md)); signature extension kinds filled from
  `val<T>`'s C++ type; calls of registered intrinsic fallbacks recorded as
  intrinsic callees (M1.7); source tags, regions and the debug function
  records with parameter names only on request (D14,
  [13 §4.9](13-nir-spec.md)); `Type`/`Op`
  mapped to widths and flags at the interface; `symbolic_execution/`
  ported (its tag map replaced by `TagMap`, its dependency on
  `ExecutionTrace` removed).
- **Files:** `tracing/nir/NirTracer.*`, `tracing/nir/TagMap.*`,
  `tracing/nir/ReplayCursor.*`; `tracing/tag/Trie.hpp`;
  `tracing/symbolic_execution/*`;
  `include/nautilus/tracing/TracingInterface.hpp`.
- **Tests:** every execution-test function traced; pre-SSA output verified
  and compared with golden dumps (`test/nir-tests/trace/`); allocation
  counter in steady state; a `val` initialized from a constant and later
  assigned, and a copy later assigned, in a loop; a copy constructed from
  different sources on the two arms of a native `c ? a : b` (#95).
- **Done when:** every execution-test function traces and verifies; no
  `malloc` per recorded op in steady state; `distinctSites1024` tag
  creation linear and tracing on it ≥ 30% cheaper than the 0.3 baseline
  (callgrind ratio, the new tracer driven directly; M2.8 repeats the check
  through the engine).

#### M2.2 Cheaper paused passes: R3, R4, rest of R2 · M · deps: M4.2
- **Builds:** the minimal paused path (R3); unwinding paused passes behind
  `tracing.unwindPausedPasses` with fallback to R3 (R4); regions in follow
  mode look up their memo by region-entry op id.
- **Tests:** fuzzer with R4 on and off; `catch (...)` and `noexcept` frames
  in traced code.
- **Done when:** `tracing.pausedOperations` ≈ 0 with R4 on; the R4 default
  decided from fuzzer, NebulaStream and TPC-H data and recorded.

#### M2.3 TPC-H job and fewer copies (C2) · M · deps: M4.2
- **Builds:** the `tpch-duck` CI leg (external nautilus-duck repository;
  needs an owner, §16); the `baselines-0.3` job extended to build
  nautilus-duck against `main` and record 0.3's TPC-H tracing time,
  re-run share, copy counts, IR op counts after 0.3's passes and
  execution time per backend (the baseline of M2.3, M2.8, M3.10, M4.10,
  M6.1 and README release criterion 3); `const val<T>&` in the `std`
  plugin and hot helpers;
  `nautilus-review` flags by-value `val` parameters.
- **Done when:** recorded copies on TPC-H at least a quarter below the 0.3
  baseline; re-run share reported per run.

#### M2.4 Tracing diagnostics (R5) · M · deps: M4.2
- **Builds:** diagnostics with code, source location and hint
  (`NAUT-T001`…) for path explosion, replay share, constant loops and values
  escaping regions ([15 §4.1](15-agent-experience.md));
  `engine.diagnostics=json`; `debug-trace` skill.
- **Done when:** each code has a test that triggers it.

#### M2.5 Explicit control flow on region scopes · L · deps: M4.2
- **Builds:** port of #300 ([01 §5](01-tracing-frontend.md)): `If` (both
  forms), `While`, `For`, `Break`, `Continue`, `Return`; region-scope arms
  and bodies; memoized like a region; optimistic recording with bounded
  restart, where a restart truncates every table the scope appended to
  (ops, `extra`, blocks, allocas, data objects, callee queue, destructor
  stack, next value ref) back to the scope's entry checkpoint, with an
  undo log for predecessor appends to older blocks and tag-map
  insertions, and drops memo entries created after it; the restarting
  pass is paused and not counted as explored; non-local exits (`Break`,
  `Continue`, `Return`) emit the destructor calls of the owned values
  above the target's depth (all of them for `Return`) without changing
  the destructor list, and only the scope that executes the exit is
  paused; exits may not cross a `region()`
  ([01 §5.1, §5.2](01-tracing-frontend.md)); direct mode runs lambdas
  natively.
- **Files:** `include/nautilus/control_flow.hpp`; `tracing/nir/`;
  `ExplicitControlFlowTest.cpp` from #300, with its mixing guard turned
  into mixing tests; fuzzer generator for explicit constructs and a mode
  that forces restarts; the explicit control-flow twins of `chainedIf`
  and `loopBody` in the latency corpus ([10 §2](10-observability.md)).
- **Tests:** owned values with `Break`; an arm that always `Break`s;
  `Return` in a nested arm; ops after `Break` in the same lambda; a
  restart after allocas, `constData` and a `nautilus::function` callee were
  recorded; `Break` inside an implicit `if` in a `While` body;
  `If(c,[&]{ Break(); })` followed by recorded ops in the same body; an arm
  whose last explored pass ends in `Return` while a function-scope owned
  value is live; `Continue` in `For` reaching the increment block; a
  restart after a nested `Break` and after a nested `region()`.
- **Done when:** explicit `chainedIf` traces in one pass with zero replayed
  ops at every corpus size, and `tracing.snapshots` on an all-explicit
  corpus function equals the count of [01 §5.2](01-tracing-frontend.md);
  mixed functions clean under the fuzzer.

#### M2.6 Constant data · S · deps: M4.2
- **Builds:** `nautilus::constData` (span and string overloads) creating
  data objects and `data.addr` ([13 §4.10](13-nir-spec.md)).
- **Files:** `include/nautilus/data.hpp`.
- **Done when:** a lookup table traced with `constData` runs on every
  enabled backend and its loads fold; persistability is checked in M5.3.

#### M2.7 Lazy copies (C4), investigation · M · deps: M4.2 · optional
- **Builds:** a prototype behind a flag; adopted only if the fuzzer and
  every golden trace agree, otherwise written up and removed.

#### M2.8 M2 gate · S · deps: M2.2–M2.6
- **Builds:** the check of [01 §7](01-tracing-frontend.md) on the corpus
  and TPC-H: re-run share and copy counts in the corpus and `tpch-duck`
  output against the 0.3 baseline; the golden pre-SSA dumps
  (`test/nir-tests/trace/`) and the `test/data` tracing dumps regenerated
  in isolated commits, once per behavior-changing WP of M2.
- **Done when:** TPC-H re-run share ≤ 35% of tracing with the shipped
  default of `tracing.unwindPausedPasses`; tracing on `distinctSites1024`
  ≥ 30% cheaper than the 0.3 baseline (callgrind ratio); explicit
  `chainedIf` traced in one pass with zero replayed ops; mixed functions
  clean under the fuzzer; the regenerated dumps reviewed in their own
  commits.

**M2 exit:** [01 §7](01-tracing-frontend.md) on the corpus and TPC-H,
checked by M2.8.

## 9. M3 Conversion and passes

Goal: one optimizing conversion replaces the fixed-point pipeline. Exit
criteria: [03 §6](03-conversion-and-passes.md).

#### M3.1 Conversion skeleton and `NirChain` · L · deps: M2.1, M4.1
- **Builds:** pre-SSA → SSA with Braun-style SSA construction in a
  loop-nest-aware RPO (each reducible loop contiguous; irreducible regions
  in plain RPO with loop depth 0), iterative traversal with an explicit
  worklist; the sealing rule of [03 §3](03-conversion-and-passes.md)
  (non-header blocks sealed when visited, headers after the last block of
  their loop, edges only from live predecessors); loop-assigned variable
  sets so that only variables assigned in a loop get header phis
  ([03 §3.2](03-conversion-and-passes.md));
  live-edge tracking, layout order, the derived loop table (depth and
  end), `useCount` while emitting; a bounded refold sweep over the users
  of phis that became trivial at sealing, limited to the sealed loop's
  block range; reads in a live-treated block with no live edge resolve to
  a zero placeholder; the mark-from-roots dead-value sweep
  ([03 §3.3](03-conversion-and-passes.md)) during final compaction;
  `nir::computeStatistics` (functions, blocks, phis/block parameters,
  instructions per function) and the `conversion.*` keys of
  [10 §3](10-observability.md) (`conversion.ms`, `.folded`, `.cse`,
  `.deadBlocks`, `.refoldSweeps`, `.refoldedInsts`); the test-only
  `NirChain` fixture (new tracer → conversion → `tbc`), with a `NirChain`
  mode of `nautilus-fuzz-replay` (and of the `checkOne*` functions of the
  fuzzer harness) and of the corpus driver's op-count output, so that op-count and fuzzer
  criteria can be checked before the cut-over; the `nir-chain` CI leg,
  which runs all three.
- **Files:** `conversion/Conversion.*`, `conversion/SSABuilder.*`,
  `conversion/Layout.*`, `nir/Statistics.*`; `test/common/NirChain.hpp`;
  `test/fuzz/` (the `NirChain` mode).
- **Tests:** an irreducible CFG; a function with more blocks than the
  default stack depth would allow recursively; a loop whose only latch is
  folded away.
- **Done when:** every execution-test function converts, verifies and
  returns the right result on `tbc` through `NirChain` (exceptions excepted
  until M3.5); the `conversion.*` keys are reported and unit-tested.

#### M3.2 Folding and simplification rules · M · deps: M3.1
- **Builds:** the rules of `ConstantFoldingAndCopyPropagationPass` and
  `AlgebraicSimplificationPass` as builder rules, IEEE-strict (D11: no
  `x + 0.0 → x`) with wrapping arithmetic; extension `fold`/`simplify`
  hooks; loads from data objects fold; the checked bit of `div`/`rem`
  cleared for safe constant divisors (non-zero, and not −1 when signed)
  and constant shift amounts masked at conversion
  ([06 §1](06-semantics.md)). Until M6.1 adds their folding rules,
  division or remainder by a constant zero or −1 with a non-constant
  dividend, `MIN / -1` and shifts by at least the bit width are never
  folded: they stay checked or masked instructions, which every backend
  lowers from its M4 port ([06 §4.2](06-semantics.md)).
- **Files:** `conversion/FoldRules.*`, `conversion/SimplifyRules.*`.
- **Done when:** a `.nir` test per rule passes, including a safe constant
  divisor (unchecked) and an unsafe one (still checked); a corpus case with a
  loop-invariant variable reassigned in the loop body folds as in 0.3
  (a `.nir` test of the corpus function); op counts of the linear
  execution-test functions through `NirChain` at most their recorded 0.3
  counts (M1.10); the corpus check is M3.10's.

#### M3.3 Branch folding, dead blocks, threading · M · deps: M3.1
- **Builds:** constant `condbr` adds only the taken edge; dead blocks never
  emitted; single-predecessor jump threading; sealing never waits on
  folded-away predecessors.
- **Done when:** merge-heavy corpora (regions, nested loops, `break`) pass
  in `NirChain`; dead loop-carried variables and chains of dead values are
  absent from the output (`.nir` tests); op counts of the branchy
  execution-test functions through `NirChain` at most their recorded 0.3
  counts; the corpus check is M3.10's.

#### M3.4 CSE · M · deps: M3.2, M3.6
- **Builds:** block-local CSE with an undo log; loads and calls keyed on the
  effect id, which is bumped by every may-write instruction and at the
  entry of every merge block, loop header and irreducible-region entry
  ([03 §3.4](03-conversion-and-passes.md)); dominator-scoped CSE
  (Cooper–Harvey–Kennedy) for pure values, behind a flag until its fuzzing
  is clean; CSE of calls only for readonly-or-pure + `willReturn` +
  `noUnwind` callees; CSE off in debug builds that need per-call-site
  provenance ([03 §7](03-conversion-and-passes.md)).
- **Tests:** negative `.nir` tests: a load after an if/else merge with a
  store in one arm; a load in a loop header with a store in the body.
- **Done when:** `.nir` tests for each CSE case pass; the fuzzer is clean
  through the `NirChain` mode (M3.1) with block-local CSE on.

#### M3.5 Landing pads · M · deps: M3.1, M3.6
- **Builds:** cleanup lists → shared landing pads keyed by ordered
  destructor set; "no cleanup" entries where nothing is live; unwind
  entries only where the callee's final `noUnwind` bit (M3.6) is clear.
- **Done when:** every exception test passes on `tbc` through `NirChain`.

#### M3.6 Attribute inference (pre-pass) · M · deps: M3.1
- **Builds:** the analyses of `NoThrowInferencePass` and
  `FunctionAttributeInferencePass` as one pre-pass
  ([03 §3.5](03-conversion-and-passes.md)) over the pre-SSA bodies
  of all functions of a module, before any function is converted:
  per-function facts, then a fixed point over the SCCs of the call graph
  (optimistic for `noUnwind`, pessimistic for memory effects on cycles),
  producing the guarantee bits of [13 §4.6.2](13-nir-spec.md) in the
  callee records; a `div`/`rem` by a safe constant (or an unassigned
  `var` of one) does not make a function may-throw
  ([03 §3.5](03-conversion-and-passes.md)); declarations (M6.2) override
  inference later.
- **Done when:** inferred bits on the test functions match the 0.3
  attributes recorded in the baselines (M1.10), mapped to the guarantee
  bits, except for the differences listed with their reason in a
  committed expected-delta file; the only accepted reasons are a checked
  `div`/`rem` (D10: 0.3 inferred `noUnwind` for a division by a variable,
  such as `divideTenBy` in `test/common/BoolOperations.hpp`) and an
  unfolded-body pessimization ([03 §3.5](03-conversion-and-passes.md)).
  Any bit more optimistic than 0.3's is a failure.

#### M3.7 Legalization · M · deps: M3.1, M1.3a
- **Builds:** one copy pass per backend: extension ops without a handler →
  fallback call or expansion; extension types without support → stack
  slots (used from M6.5), with the fallback calling convention of
  [04 §3.3](04-extensions.md) (extension-typed operands and results through
  pointers to slots, results through a leading out-pointer); ABI lowering
  where a backend asks for it ([03 §4](03-conversion-and-passes.md)).
- **Files:** `passes/Legalization.*`.
- **Done when:** `.nir` tests cover each rule; every intrinsic without a
  handler runs through its fallback on `tbc`.

#### M3.8 Optional passes as copy passes · L · deps: M4.2
- **Builds:** LICM, strength reduction and inlining of internal callees on
  NIR, off on latency-critical compiles; `ir.runOptimizationPasses` and
  `ir.disable*` keep their meaning; options of removed passes warn (the
  0.3 → 0.4 pass mapping of [03 §2](03-conversion-and-passes.md)).
  Cross-module inlining is part of M4.6.
- **Files:** `passes/LICM.*`, `passes/StrengthReduction.*`,
  `passes/Inline.*`.
- **Done when:** `.nir` tests per pass; fuzzer clean with every optional
  pass enabled.

#### M3.9 Plugin pass hooks · S · deps: M3.7
- **Builds:** `afterConversion` and `beforeLowering` with required/optional
  marking ([03 §5](03-conversion-and-passes.md)).
- **Tests:** the test plugin registers from a static initializer, before
  any engine is constructed (the registry freezes then, M1.7).
- **Done when:** a test plugin's required pass runs on latency-critical
  compiles and its optional pass does not.

#### M3.10 M3 gate · S · deps: M3.2–M3.6
- **Builds:** the op-count report against the 0.3 baseline in the corpus
  output, including the loop-invariant reassignment case and the refold
  sweep count; the compile-fail suite for the public headers (agent item
  for M3, [15 §5](15-agent-experience.md)).
- **Done when:** [03 §6](03-conversion-and-passes.md) holds on the corpus
  and, once the `tpch-duck` job exists, on TPC-H kernels; no IR pass runs
  on a default compile (`irPasses.pipelineIterations` = 0, checked
  exactly).

## 10. M4 Backends, the cut-over, tiering

Goal: every backend except GPU (M7) reads NIR; the engine runs the new pipeline; the
promotion service replaces thread-per-module. Exit criteria:
[05 §9](05-backends.md), [11 §7](11-tiered-compilation.md).

**Every backend port implements the full core opcode set** of
[13 §5](13-nir-spec.md), including `data.addr` and `funcaddr` (stable
forwarding stubs in `compiler/backends/FunctionStubs.*`), **and the defined
integer and float semantics of D10/D11** per the per-backend table of
[06 §4.2](06-semantics.md): checked `div`/`rem` throws through a cold-path
call to the runtime helper `nautilus_rt_throw_arith`, made through the
capture thunk with the instruction's cleanup list; dynamic shift amounts
masked where the hardware does not mask; saturating float→int casts. The
conversion clears checks for safe constant divisors and masks constant
shift amounts from M3.2 on; M6.1 adds the edge-case folding and the
plugin side. May-throw intrinsic calls go through the capture thunk
registered with the intrinsic (M1.7, [13 §5.7](13-nir-spec.md)) in the
`tbc`, `cpp` and `asmjit` ports. Imported calls are added
by M4.6, `bind.addr` by M5.2, extension *type* lowering by M6.5. A port
that lands before the cut-over lives in new files next to its 0.3 lowering,
which the cut-over deletes; until then it is tested with `nautilus-nir-run`,
and its execution-test criteria are checked at the cut-over (M4.2). The
same holds for the `asm-diff` and callgrind criteria of a port that lands
before the cut-over: before it, both measure 0.3's lowering, so they are
checked at M4.2.

#### M4.0 Backend interface and `nautilus-nir-run` · M · deps: M1.3, M1.7 (and M1.3a for `.nir` tests with extension items, needed only at its end)
- **Builds:** `CompilationBackend` on a legalized `nir::Module`, with
  `compile` taking a `const CancellationToken*` (null for synchronous
  compiles; checked at stage boundaries, [11 §4.2](11-tiered-compilation.md));
  array side tables helpers; per-backend extension lowering tables
  (`extLowering`, `extTypes`) and their registration API, attached to
  intrinsics and ops by name, which replaces `MLIRIntrinsicPlugin` and
  `AsmJitIntrinsicPlugin` at the cut-over; the runtime helper
  `nautilus_rt_throw_arith`; `tools/nautilus-nir-run` on `.nir` text (binary
  images as input once M1.5 has landed).
- **Done when:** a test backend runs `.nir` files through `nautilus-nir-run`.

#### M4.1 `tbc` on NIR · L · deps: M4.0
- **Builds:** near 1:1 lowering in new files next to the 0.3 lowering;
  register allocation on value ids; one core bytecode opcode `EXT` with a
  per-site `ExtSite` record, like `CALL_EXT`, dispatching to an
  out-of-line extension handler through one generic stencil, in every
  dispatch skin (plugins never add dispatch entries or stencils); D10/D11
  as checked C++ helpers without UB; copy-and-patch mode with the stencils
  for all three targets regenerated by `tools/stencils/build_stencils.py`
  in their own commit; the `stencil-check` CI leg.
- **Done when:** every opcode's `.nir` tests, including the D10/D11 edge
  cases, pass on `tbc` in both modes; `stencil-check` is green.

#### M4.2 Cut-over · L · deps: M3.1–M3.7, M4.1
- **Builds:** §2 step 2:
  - the engine uses new tracer → conversion → legalization → backend;
    backends without a NIR port switched off with a message; the in-tree
    plugins' identities (M1.7) used directly, handlers coming back with
    each backend port (fallbacks until then);
  - ports of everything that includes 0.3 tracer or IR headers:
    `TracingTest.cpp` to the NIR stages (after tracing, after conversion,
    after legalization) before `test/data` is regenerated; `RegionTest`,
    `NoThrowInferenceIntegrationTest` and `InvokeExceptionExecutionTest`
    to the NIR equivalents (the M3.1 statistics, M3.6 inference,
    M3.5 landing pads); `AliveVariableHashTest` to the new tracer;
    `tracing/CopyAttribution.*` (`dump.copySites`) to the flat trace;
    `TracingBenchmark.cpp`; `compiler/CompilationPipeline.*`,
    `compiler/TieredCompiler.*` and `compiler/backends/CompilationBackend.*`
    onto `nir::Module` and the M4.0 interface;
    `compiler/backends/CapturedExceptionTransport.*` onto NIR unwind
    entries and landing pads (used by `tbc`, `cpp` and `asmjit`,
    [05 §4.2](05-backends.md)); `tbc/TBCBackend.*` onto the M4.1 lowering;
    the playground runner
    (`tools/playground/runner/manifest.hpp`,
    `web/src/lib/traceSections.ts`) and `tools/vscode-nautilus-ir` switched
    to the NIR stages and the NIR text form;
  - the deletions of §5, including the `ir::IRGraph` forward declaration
    in `JITCompiler.hpp`;
  - fuzzer generation of the D10/D11 edge cases;
  - `test/data` regenerated in its own commit (`test/data/nir-conformance/`
    excepted: it changes only with the spec, [13 §11](13-nir-spec.md));
    the `api-compat` leg starts;
  - the `example-demos` job of `pr.yml` skips the demos whose backend is
    switched off (`cpp` until PR 7 if it has not merged, `mlir` until
    PR 8).
- **Done when:** every core test, and every plugin test whose backends are
  enabled, passes on `tbc` (and on the backends of PR 7 if merged);
  `test/api-compat/` passes (each example on the backends it supports,
  M1.10); the fuzzer is clean on `tbc`; nothing built in any preset
  includes a deleted header (a CI grep over the configured source set);
  the sources of switched-off components (§5) are excluded from the build
  and listed in the cut-over PR.

#### M4.3 `cpp` on NIR · S · deps: M4.0
- **Builds:** printing from NIR; extension ops as expression templates or
  calls; D10/D11 through checked C++ helpers (no UB).
- **Done when:** every execution test passes on `cpp` (before the cut-over:
  every `.nir` test).

#### M4.4 `asmjit` on NIR · XL · deps: M4.0
- **Builds:** [05 §4.4](05-backends.md): providers on NIR with array side
  tables and `ConstPool` immediates; virtual registers created on first use
  and reused only for values defined and dead within one basic block;
  #565's call-heavy fix confirmed on NIR on x86-64 and AArch64; AsmJit's
  `Compiler`, RA and post-RA peepholes unchanged; D10/D11 as explicit
  compare and branch to a cold block for checked `div`/`rem`, a masked
  shift only where the hardware does not mask, saturating casts (`cvtt`
  plus fixup); plugin intrinsic handlers written on the NIR lowering API
  beside their 0.3 versions, which M4.2 deletes; `asm-diff` leg and skill.
- **Done when:** [05 §4.3](05-backends.md): no code-quality regression
  against the recorded 0.3 assembly (`asm-diff`); the backend callgrind
  ratio gate on linear code in CI (both checked at M4.2 if PR 7 lands
  before the cut-over, §10), and ≤ 0.25 µs/op on the reference
  machine at the M4 exit; every test green on both architectures.

#### M4.5 MLIR on NIR with state reuse · L · deps: M4.2
- **Builds:** blocks and terminators → `cf`, arithmetic → `arith`, with
  explicit checks for D10 and `llvm.fptosi.sat`/`fptoui.sat` for D11; the
  hook for extension type mappings (used from M6.5); #561's per-thread
  `TargetMachine` and `MLIRContext` reuse, with worker contexts created
  without multithreading or sharing one engine-owned thread pool sized by
  `engine.tiered.promotionThreads` ([11 §4.1](11-tiered-compilation.md));
  single verification, data layout for `LLJIT`; DWARF from the debug
  sections, with the NIR printer's instruction → line map as the side
  output that replaces `IRLocationPass` ([03 §2](03-conversion-and-passes.md),
  also used by M4.8); intrinsic handlers
  re-registered; the rest of the backend (`MLIRCompilationBackend`,
  `PropagatePersonalityPass`, the `debug/` passes, `jit/MLIRJit`) moved
  off `IRGraph` and back into the build; the `inlining` plugin restored on
  NIR; `test/llvm-ir-test/` reference IR regenerated in its own commit;
  `DebugInfoExecutionTest` updated to the NIR text form of the DWARF
  source (it checks the 0.3 dump bracket `nautilus {` … `//nautilus` of
  `IRGraph::toString()` today); the `llvm-ir-test` job and the
  inlining-plugin step of `pr.yml` switched back on.
- **Done when:** every execution test passes on MLIR; `DebugInfoExecutionTest`
  passes; the `inlining` plugin suite passes; MLIR fixed cost per compile
  at #561's level.

#### M4.6 Modules and linking · M · deps: M4.4, M4.5, M3.8
- **Builds:** [13 §4.4](13-nir-spec.md): named modules, exports and import
  declarations; linking by exact signature and guarantee bits, acyclic;
  import slots updated with a release store on promotion; lifecycle built →
  linked → compiled → released; host API of §4.4.4; **imported calls in
  every backend**; cross-module inlining (exporter hashes recorded,
  functions using `bind.addr` skipped).
- **Files:** `link/ModuleRegistry.*`, `link/ImportSlots.*`;
  `include/nautilus/Module.hpp` (additions only).
- **Done when:** cycles, signature and guarantee mismatches are rejected;
  an exporter's promotion is seen by importers; release order holds; every
  backend runs cross-module calls.

#### M4.7 Promotion service · L · deps: M4.5
- **Builds:** [11 §4](11-tiered-compilation.md): engine-wide worker pool,
  admission cap, lower OS priority; job states and cancellation at stage
  boundaries; shutdown without held mutexes; status API (`tier()`,
  `promotionStatus()`, `waitForPromotion`, `promote()`,
  `NautilusEngine::waitForPromotions()`); policies `eager` and `manual`
  (D8); early start after conversion, with a late tier-0 result dropped
  when tier 1 has already been installed; options settable alone; one
  compile path with linked unit IDs; `TieredJITCompiler::waitForPendingPromotions`
  removed (D17) and `TieredCompilationTest.cpp` switched to
  `waitForPromotions()`.
- **Files:** `compiler/PromotionService.*`, `compiler/TieredCompiler.*`,
  `include/nautilus/Module.hpp` (additions only), `docs/options.md`,
  `docs/engine.md`.
- **Done when:** the tests of [11 §6](11-tiered-compilation.md), except
  its Caches bullet (checked by M5.7), pass, including fault injection,
  the fuzzer with forced promotion and the thread bound with MLIR workers.

#### M4.8 Debug info and profiling (D14) · M · deps: M4.4, M4.5
- **Builds:** `asmjit` symbols per function (and per region with source
  locations) in `JitSymbolRegistry` and the jitdump; with source locations
  on, an `asmjit` line table (code offset → NIR instruction) for per-op
  attribution; `tbc` handler ranges.
- **Done when:** `DebugInfoExecutionTest` and `PerfJitDumpExecutionTest`
  pass; a `perf.sample` test passes on `asmjit`, including per-operation
  attribution through `JitSymbolRegistry::resolveLine`; the `profiling`
  plugin suite passes.

#### M4.9 Agent items for M4 · M · deps: M4.7, M3.8
- **Builds:** `CompiledModule::explain()` (also as JSON; fields for
  persistability and inlining decisions are filled as M3.8 and M5.3 land);
  `tools/agent/affected-tests`; `memory-check` skill
  ([15 §5](15-agent-experience.md)).
- **Done when:** `explain()` is covered by a test; `affected-tests` maps a
  sample of changed files to the expected targets.

#### M4.10 M4 exit · S · deps: M4.3–M4.9
- **Builds:** every backend switched on except GPU (M7,
  [17](17-gpu.md)); the M4 gates measured against the baselines (callgrind
  ratios in CI, µs/op on the reference machine); decision D3 taken by its
  criterion and recorded in the README; the `nebulastream-0.4` and
  `tpch-duck` legs start gating.
- **Done when:** [05 §9](05-backends.md) and [11 §7](11-tiered-compilation.md)
  hold; NebulaStream and TPC-H green.

## 11. M5 Reuse

Goal: modules are saved, loaded and cached; the C API is available. Exit
criteria: [07 §6](07-serialization.md), [08 §5](08-caching.md),
[09 §4](09-c-api.md).

#### M5.1 Pointers: process-local and portable · M · deps: M4.10
- **Builds:** pointer kinds; `ProcessLocal` with a per-process token;
  `Portable` through `SymbolNamer`/`SymbolResolver` (`SymbolTable`,
  `dladdr`/`dlsym`) and ELF build ID plus offset ([07 §3](07-serialization.md));
  both installed engine-wide before the first compile
  (`NautilusEngine::setSymbolNamer`, `setSymbolResolver`; a later call is
  an error), with the tracer applying the namer as it creates each
  `pointers` entry, so mode and provenance are final before the
  eligibility scan and every cache key ([07 §3](07-serialization.md));
  naming order: the namer first, then ELF build ID plus offset (mode 2),
  otherwise mode 0.
- **Done when:** a module with external callees and destructors saves
  portably and resolves in a fresh process; a process-local image is
  rejected in another process; a pointer named by the engine's namer is
  mode 1 in the pre-SSA module, before any save; with an engine namer, a
  traced module with external callees is eligible at the pre-SSA scan and
  reloads in a fresh process ([07 §6](07-serialization.md)).

#### M5.2 Runtime bindings · M · deps: M5.1
- **Builds:** port of #529 onto the `bindings` section: `RuntimeBinding<T>`,
  address-free schema, fresh resolution per load, identities never merged by
  aliasing; `bind.addr` in the tracer, the conversion and every backend.
- **Done when:** #529's binding tests pass on every backend, including
  concurrent modules.

#### M5.3 Provenance and eligibility · M · deps: M5.1
- **Builds:** provenance classes on constants, allocation evidence, pointer
  portability, extension payload portability ([07 §5](07-serialization.md)).
  The scan runs **on the pre-SSA level, before the conversion folds**, and
  folding gives every folded constant the least trusted class among its
  inputs (`RawProcessValue` > `AuditedInvariant` > `Literal`), so folding
  can never launder a raw value. Traced pointers are always `pointers`
  entries, never ConstPool constants; an entry without a binding defaults
  to `RawProcessValue` with the `PROCESS_LOCAL` bit and a process token,
  and such an image never loads in another process. Modules built through
  the C API have no pre-SSA level and are scanned at the SSA level. Fail
  closed with `artifact.ineligibleReason`.
- **Done when:** every negative test of #527 and #528 is ported and fails
  closed; a `constData` table is eligible, a traced host pointer is not.

#### M5.4 Self-contained export · S · deps: M4.6, M5.1
- **Builds:** saving bundles the transitive closure of imports as internal
  functions ([13 §8.5](13-nir-spec.md)); `imports` is empty in every image.
- **Done when:** an image of an importer loads and runs in an engine that
  has never seen the exporter.

#### M5.5 Save, inspect and load NIR images · S · deps: M5.2–M5.4
- **Builds:** `saveModule`, `inspectModule`, `loadModule` for the NIR-image
  path, with signature checks including width and the per-parameter
  extension kind (none / zext / sext); native
  payloads are added by M5.6; `engine.retainIR` from #532 is not needed;
  the overloads `saveModule(compiled, namer)`, which names only the
  entries still in mode 0, reruns the SSA-level eligibility check on the
  copy and writes only the NIR image if the copy changed, and
  `loadModule(engine, image, bindings, resolver)`, which overrides the
  engine's resolver ([07 §3](07-serialization.md)); a fuzzer mode that saves, reloads and runs each generated module on
  every backend ([10 §4](10-observability.md)).
- **Done when:** save on one engine, load on every backend: identical
  results, identical bytes on re-save; without an engine namer,
  `saveModule(compiled, namer)` gives a `Portable` NIR-only image that
  loads in a fresh process; the save/load fuzzer mode is clean.

#### M5.6 Artifacts and native payloads · L · deps: M5.5, M4.5
- **Builds:** the artifact container (`NART`, [07 §2.2](07-serialization.md)),
  on the image framing of M1.5 ([13 §8.6](13-nir-spec.md)), and the
  compatibility descriptor ([07 §2](07-serialization.md)), with the
  target CPU name and feature string; the option `engine.mlir.targetCpu`
  (default `host`, `generic` for shippable payloads; code-affecting in the
  M1.4a registry); one relocatable object per payload from MLIR/LLVM with
  #527's import table, personality and unwind-resume bridges; the MLIR
  lowering of a module that may be persisted emits every pointer, binding,
  data-object and `funcaddr` stub reference, every runtime helper
  (`nautilus_rt_*`), every intrinsic callee without an inline lowering
  (`__nautilus_intr_<i>`) and the registered capture thunk of every
  may-throw intrinsic call made through a thunk (`__nautilus_intr_thunk_<i>`,
  `PayloadImport.cls` 7; an intrinsic registered without a thunk rejects
  the payload) as an external symbol that the payload loader
  binds, built position-independent with the small code model
  ([07 §2.1](07-serialization.md), [05 §5.2](05-backends.md)); payloads
  that used the `inlining` plugin's LLVM hooks marked non-persistable, or
  their symbols routed through the import table; `loadModule` uses a matching payload and
  otherwise recompiles from the NIR image, never executing a mismatched
  payload.
- **Done when:** a native hit in a fresh process with ASLR records no
  tracing, conversion or codegen; every corrupt or incompatible payload
  falls back, including one built for a CPU whose features the host lacks,
  one with an unaccepted relocation and one with an unlisted symbol;
  a module compiled with `NAUTILUS_INLINE` functions is either rejected
  for export or reloads correctly in a fresh process; a payload with a
  may-throw intrinsic call loads in a fresh process and the exception
  reaches the landing pad ([07 §6](07-serialization.md)).

#### M5.7 Caches L0, L1, L2 and tiers · L · deps: M5.6, M4.7
- **Builds:** `plugins/cache/` from #528 (storage model unchanged); L0 in
  memory with LRU, its key including the module's binding environment
  (names and addresses), and each L0 entry owning the executable with its
  data-object placement, `funcaddr` stubs and import slots, shared by
  reference by every module that hits it and freed when the last of them
  is released and the entry is evicted ([08 §2](08-caching.md),
  [13 §4.4.1](13-nir-spec.md)); one L0 entry for all tiers, keyed by the
  configured backend(s) and never re-keyed on promotion; L0 tier state
  (installed executable and promotion) per entry, so promotion runs once for all sharing modules
  ([11 §4.6](11-tiered-compilation.md)); L0 entries pin their exporters
  like importers, and registering a module under a name pinned only by
  L0 entries evicts those entries first; the L1 key of [08 §2](08-caching.md),
  XXH3-128 over `engine.cache.key`, the code-affecting options block, the
  application options, the declared exports and the declared imports
  (item 5: `u32` count, 0 without imports, then per import sorted by
  (module, function) bytes the names, the signature type words and the
  exporter's structural hash over its self-contained image, memoized per
  compiled exporter), looked up **before tracing**, with
  the rest of the compatibility identity (extensions, core and provider
  builds, per payload the target, CPU and feature string) checked on the
  stored entry at a hit, not keyed; L1 stores the canonical NIR image of
  every compiled module, of any tier, plus native payloads of tier 1
  (MLIR) only, so a warm L1 hit skips tracing even when only tier 0 ran,
  and a changed build, CPU or feature string recompiles the image without
  tracing; L2
  structural key with full comparison on hit, whose native payload is used
  only if its descriptor matches; a tier-1 hit skips tier 0; promotion
  results feed the caches ([11 §4.6](11-tiered-compilation.md)); the
  bounds and failure rules of [08 §3.1](08-caching.md)
  (`engine.cache.maxBytes`, corrupt or stale entries replaced, store
  failures as misses); single-flight compiles per key; cache statistics
  including `cache.hashMs`; cache cases in the corpus; a fuzzer mode that
  compiles each generated module twice under different caller keys and
  checks the result of the L2 hit ([10 §4](10-observability.md)); the
  warm-NebulaStream
  harness of [08 §5.1](08-caching.md): the `nebulastream-cache` workflow
  (cold, then warm, `ENABLE_CACHE_PLUGIN=ON`) and the NebulaStream patch
  that sets `engine.cache.key` per query (port of the #529 branch).
- **Done when:** [08 §5](08-caching.md) holds; a module that hits an L0
  entry after promotion keeps running after every earlier sharer is
  released (ASan clean); the Caches tests of
  [11 §6](11-tiered-compilation.md) pass (a warm L1/L2 hit with a matching
  payload runs tier 1 from the first call with no tier-0 compile; an L1
  hit with the `manual` policy and no promotion, and one with
  `engine.backend=asmjit`, skip tracing; a payload for another CPU is not
  executed and the module is recompiled from its NIR image); the
  cache-hit fuzzer mode is clean; two modules with equal NIR and binding
  names but different binding addresses miss each other at L0; two
  modules sharing an L0 entry keep running after either is released, and
  code, data and stubs are freed only after both are released and the
  entry is evicted (ASan clean); an importer misses L1 after its exporter
  changed and was re-registered in a new process; an L1 hit whose image
  has a type instance unavailable on the host (a test type with
  `available = false`) is treated as `incompatible`
  ([13 §8.4](13-nir-spec.md)); warm NebulaStream wall time ≤ 40% of
  cold on the reference machine ([08 §5.1](08-caching.md)).

#### M5.8 C API on NIR · L · deps: M5.5
- **Builds:** port of #531 ([09](09-c-api.md)): handles as `u32` ids, C
  enums generated from `Opcodes.def`, extension ops by name, modules,
  imports and data objects, save/load/inspect; id handles carrying the
  module revision, stale handles rejected; `nautilus_executable_call`,
  which catches every exception and returns a status; a bindings
  environment on load and compile; engine-level namer and resolver
  callbacks (`nautilus_engine_set_namer`, `nautilus_engine_set_resolver`),
  with the namer argument of `nautilus_ir_save` naming only the entries
  still in mode 0, and a save whose namer changed the copy writing no
  native payloads; a builder created for an engine applies the engine
  namer to pointer entries added by address, entries added by name are
  mode 1; an optional per-call resolver override on compile and load;
  may-throw external callees and `call.indirect` naming a thunk entry
  added by name ([07 §3](07-serialization.md), [09 §3](09-c-api.md));
  stability as in [09 §6](09-c-api.md); `libnautilus-c` export test in
  the matrix; a Rust bindgen smoke test.
- **Done when:** [09 §4](09-c-api.md), including the round trip with namer
  and resolver set on the engine, and a namer given only to
  `nautilus_ir_save` that gives a `Portable` image without payloads.

#### M5.9 Compatibility fixtures · S · deps: M5.5, M5.6
- **Builds:** images and artifacts (with a native payload) saved by this
  milestone in `test/nir-tests/compat/`, loaded by every later build
  (D12).
- **Done when:** the fixtures load and run on every backend in CI; after a
  build change, an artifact's payload is rejected and its NIR image used.

#### M5.10 Playground MCP server · M · deps: M4.9 (and the runner port of M4.2)
- **Builds:** MCP tools `compile`, `stages`, `run`, `benchmark`, `explain`
  over the playground runner, local by default
  ([15 §4.4](15-agent-experience.md)); pre-SSA images as reproducers
  ([10 §4](10-observability.md), [01 §6](01-tracing-frontend.md)): the
  fuzzer attaches the pre-SSA image of a finding, and the playground
  loads it.
- **Done when:** each tool has an end-to-end test against the runner; a
  fuzzer finding's pre-SSA image reproduces it in the playground.

## 12. M6 Semantics and extensions

Goal: the defined semantics of D10/D11, declared effects, and plugins on
extension ops and types. Exit criteria: [06 §6](06-semantics.md),
[04 §5](04-extensions.md). NIR and the tracer are signless from M1 and M2;
M6.4 moves the plugin-facing API to widths.

#### M6.1 Folding and plugin side of D10/D11 · S · deps: M4.10
- **Builds:** the backends lower D10/D11 from their M4 ports (§10); this
  WP adds the rest of the folding (safe constant divisors and constant
  shift masks are M3.2's): unsafe constant operands folded to their
  defined results or to an unconditional throw, range-based proofs that
  drop further checks ([06 §1](06-semantics.md)); the plugin side (in-tree plugin handlers and
  fallbacks follow D10/D11, [06 §4.2](06-semantics.md)); `tbc` stencils
  regenerated in their own commit if handlers change.
- **Done when:** a `.nir` test per folding rule; D10/D11 cases still pass on
  every backend and under the fuzzer; the cost on `ExecutionBenchmark` and
  TPC-H is recorded (the `unchecked` fallback of [06 §7](06-semantics.md)
  only if it is large).

#### M6.2 Declared effects and `nothrow` · M · deps: M3.6, M4.10
- **Builds:** `invoke<no_throw>`, the `nothrow` attribute and effect
  declarations mapped to guarantee bits; declarations take precedence over
  inference; debug-mode checks (terminate handler, shadow effect checks).
- **Done when:** wrongly declared callees are caught by the debug checks;
  external callees declared `pure` (which implies `willReturn`) and
  `nothrow` (which gives `noUnwind`) are merged and hoisted in `.nir` and
  execution tests; a `pure` callee without `nothrow` is not merged.

#### M6.3 Cheaper exception handling · M · deps: M6.2
- **Builds:** no capture thunk on MLIR call sites without cleanup;
  annotations in the `std` plugin and the NebulaStream hot runtime
  functions; share of call sites with exception handling in the statistics.
- **Done when:** [06 §6](06-semantics.md): no thunk on MLIR call sites
  without cleanup; the number of may-unwind call sites and of capture
  thunks per corpus function is below the 0.3 baseline.

#### M6.4 Plugin API on widths and op flags · S · deps: M4.10
- **Builds:** `isUnsignedOp(inst)`; signed types removed from plugin-facing
  headers; in-tree plugins switched.
- **Done when:** no plugin source switches on a signed type.

#### M6.5 Extension types · L · deps: M3.7, M4.10
- **Builds:** `TypeInfo`; parameterized types enumerated by the plugin at
  registration, each instance with its own type id (CPU-dependent
  instances registered but marked unavailable on the host), so the type
  table is complete at the freeze; payload interning per module, never in
  the registry; multi-part values, stack-slot fallback with the fallback
  convention of [04 §3.3](04-extensions.md), type mappings in every
  backend (`tbc`: extension-typed values in the frame's alloca area, the
  16-byte alloca alignment limit of `TBCLoweringProvider.cpp` lifted to
  4096, [04 §3.5](04-extensions.md)); `simd.vec<T,N>` and `builtin.i128`;
  the SIMD extension-op case of the latency corpus
  ([10 §2](10-observability.md)), gated against its first recording.
- **Done when:** both types run on every backend, with and without handlers;
  an image with an AVX-512 `simd.vec` in an exported signature fails
  `NIR-V035` (`NIR-L017`) on an AVX2 host (or with the instance marked
  unavailable) ([13 §8.4](13-nir-spec.md)).

#### M6.6 In-tree plugin migration · L · deps: M6.5
- **Builds:** the table of [04 §4](04-extensions.md): `builtin` prefetch,
  overflow and int128 with **removal of the 64-slot int128 ring**; `simd`
  ops and **removal of the ring buffer**; `specialization` assume (all
  `assume_*` variants) and `ConditionProfile` mapped to `condbr`
  probabilities; `std` math, bit and atomic intrinsics with MLIR lowerings
  and optional `asmjit` lowerings; the core `mem*` intrinsics. The `std`
  effect declarations are M6.3's. The `gpu` plugin moves in M7
  ([17](17-gpu.md)).
- **Done when:** [04 §5](04-extensions.md) M6 criteria: every plugin suite on
  every backend; SIMD without the 32-value rule; int128 tests without the
  64-value lifetime rule; fuzzer with handlers on and off; the
  trait-ignoring debug mode passes.

#### M6.7 Agent items for M6 · M · deps: M2.4, M6.5
- **Builds:** `docs/diagnostics.md` complete; `docs/` code examples
  extracted and compiled in CI; the `extension-plugin` skill on the final
  extension API.
- **Done when:** every diagnostic code has a catalog entry and a test; every
  `docs/` example compiles in CI.

## 13. M7 GPU

The GPU plugin is rebuilt last and made production ready, from the design
in [17-gpu.md](17-gpu.md). It is off from the cut-over until this
milestone. Two PRs: the core (PR 17) and the production features and
validation (PR 18).

#### M7.1 Device operations, launch and partition · L · deps: M3.9, M4.10, M6.1
- **Builds:** `gpu.thread_idx`/`block_idx`/`block_dim`/`grid_dim` and
  `gpu.sync_threads` with the 0.3 CPU fallbacks; `gpu.launch` as an
  **intrinsic with call semantics** (may throw, carries the cleanup list
  and thunk), with `nautilus_gpu_launch` as its native fallback, attributes
  0 and the signature `(ptr kernel, ptr hostEntry, i32 zext ×6, ptr argv,
  i32 zext argc) -> void`; `gpu::launch` tracing it (the `gpu.hpp` API
  unchanged); the launch checks in `gpu.partition` (intrinsics have no
  `verify` hook), which rewrites `kernel` to `data.addr` of a read-only
  kernel descriptor and, on `cpu`/`emulator`, `hostEntry` to
  `funcaddr @<kernel>$argv` ([17 §4.3, §4.5](17-gpu.md));
  kernels marked with the `gpu.kernel` function string attribute
  ([13 §4.5.2](13-nir-spec.md)); `GPUTracingTest` ported to pre-SSA NIR,
  its golden dumps regenerated in their own commit
  ([17 §5.2](17-gpu.md)); the required `gpu.partition` pass with the
  device code rules, address-space inference and the D19 division rewrite
  ([17 §4.2–§4.5, §4.7](17-gpu.md)).
- **Done when:** every 0.3 GPU test function runs through the fallbacks on
  `tbc`, `asmjit` and MLIR with the 0.3 results, except the two
  `gpuLaunchVecAdd*` "CPU fallback" sections of `GPUExecutionTest.cpp`,
  which are rewritten ([17 §5.2](17-gpu.md)); each device code rule has
  a negative test.

#### M7.2 GPU runtime, memory, errors and the grid emulator · XL · deps: M7.1
- **Builds:** `plugins/gpu/src/runtime/` with its C ABI
  (`nautilus_gpu_launch`, buffers, streams); devices, contexts, streams and
  queues; `gpu.device`, registered in the M1.4a registry with
  `affectsCode` and a `resolve` hook that maps `auto` at engine
  construction (a device probe through the `dlopen` loader) to `cuda`,
  `metal` or `none`, the value that enters every cache key (`auto` errors
  when a module with a `gpu.launch` is compiled and no device is present; `cpu` runs
  the full grid serially; kernels using barriers, shared memory or warp
  ops require `emulator`) and the deprecated `engine.backend=cuda|metal`
  aliases; `libcuda` and `libnvrtc` loaded with `dlopen` at the first CUDA
  device query, never linked; `DeviceBuffer`, `ManagedBuffer`, pointer-argument checks and the
  0.3 Metal compatibility path; `launchAsync` and `synchronize`; launch
  validation; `GPUCompileError` and `GPUException`; the CPU grid emulator
  (`gpu.device=emulator`: fibers per thread, one `tbc` VM stack per fiber,
  cooperative barriers with
  divergence detection, per-block shared memory, warps of 32 lanes) and the
  `gpu-emulator` leg ([17 §4.1, §4.3, §4.4, §4.6, §5.1](17-gpu.md)).
- **Done when:** the memory, error, concurrency and selection tests of
  [17 §5.2](17-gpu.md) pass on the emulator, including `gpu.device=auto`
  hashed as its resolved value, never as `auto`; the 0.3 GPU test functions run on the emulator with
  the 0.3 results.

#### M7.3 Shared emitter and CUDA device compiler · L · deps: M7.2
- **Builds:** the NIR emitter replacing `GPULoweringProviderBase` (array
  side tables, signedness from op flags); CUDA device code through NVRTC to
  PTX, loaded with the driver API for the current device; kernel handles;
  `lowerToCUDA` on NIR (device code only); CUDA references of
  `GPUCodegenTest` regenerated in their own commit; the `cuda-compile` leg.
- **Done when:** every test kernel compiles through NVRTC and `ptxas` in CI;
  the 0.3 GPU tests pass on the emulator with CUDA-style code; the Linux GPU
  legs are restored. CUDA is experimental: there is no NVIDIA hardware to
  execute it on ([17 §1, §5](17-gpu.md)).

#### M7.4 Metal device compiler · M · deps: M7.3
- **Builds:** MSL from the shared emitter, compiled with
  `newLibraryWithSource`; pipeline states created once per kernel; dispatch
  through the runtime; `lowerToMetal` on NIR (device code only); Metal
  references regenerated in their own commit.
- **Done when:** the 0.3 GPU tests pass on the hosted macOS runner with
  Metal validation on; the macOS GPU leg is restored; no per-launch library,
  pipeline or queue creation (G2).

#### M7.5 Device features · L · deps: M7.4
- **Builds:** `gpu.shared_alloca`, the `gpu.atomic.*` and `gpu.warp.*` ops
  with fallbacks and `gpu.hpp` wrappers; the device mapping of the `std`
  math intrinsics ([17 §4.2](17-gpu.md)).
- **Done when:** every op gives the same results on Metal, on the emulator
  and through its fallback, and compiles for CUDA; a shared-memory reduction
  kernel passes.

#### M7.6 Caching, artifacts and observability · M · deps: M7.4, M5.7
- **Builds:** an in-process (L0) cache of device code, loaded kernels and
  pipeline states, keyed by the device module's structural hash plus the
  toolkit and driver versions; artifacts and L1/L2 carry only the NIR
  image ([07 §2](07-serialization.md)), and device code is recompiled at
  load (GPU payloads in artifacts are a later container extension);
  `gpu.*` statistics, `explain()` entries and dumps
  ([17 §4.8, §4.9](17-gpu.md)).
- **Done when:** a warm L0 hit compiles no device code; an artifact or
  L1/L2 hit of a GPU module skips tracing and compiles the device code
  once per process ([17 §4.8](17-gpu.md)); a toolkit or driver change misses the L0 cache and recompiles.

#### M7.7 GPU validation and documentation · M · deps: M7.5, M7.6
- **Builds:** `tools/gpu-validate/` (Metal benchmarks on an Apple-silicon
  Mac now; the CUDA run of [17 §5.3](17-gpu.md) whenever NVIDIA hardware is
  available) and `plugins/gpu/validation/` for the reports; the GPU
  fuzzer mode on the emulator and Metal in CI; the §4.8 benchmarks against
  hand-written kernels; the frozen 0.3 GPU tests and `DemoGPU.cpp` in
  `test/api-compat/` enabled (they were copied, disabled, in M1.10; the
  two rewritten "CPU fallback" sections of `GPUExecutionTest.cpp` stay
  outside the suite), with the `api-compat` harness setting
  `gpu.device=emulator` through options on hosts without a device, where
  `auto` errors ([17 §4.1](17-gpu.md)); the sources stay unchanged;
  `docs/gpu.md` ([17 §5](17-gpu.md)).
- **Done when:** [17 §6](17-gpu.md); D19 and D20 decided.

## 14. M8 Release

#### M8.1 Documentation and migration guide · M · deps: M6, M7
- **Builds:** README §6.2 updates; new documents (extension API,
  serialization and caching, C API, migration guide from 0.3 for plugin
  authors); `llms.txt` and `docs/api.md`; the user plugin with its skills,
  including `migrate-0.3-to-0.4`, in `tools/claude-plugin/`
  ([15 §4.5](15-agent-experience.md)).
- **Done when:** `doc-check` passes; every page in README §6.2 is updated;
  a fresh cloud session reaches a green `dev` test run with only the
  commands in `AGENTS.md` ([15 §6](15-agent-experience.md)).

#### M8.2 Release validation · M · deps: M6, M7
- **Builds:** the release criteria of README §6 on x86-64 and AArch64 with
  ASan, UBSan and TSan (separate legs); `test/api-compat/` on every backend; the manual Metal
  run (D15); reference-machine numbers in the release notes; NebulaStream
  and nautilus-duck on the release candidate.
- **Done when:** every release criterion is met and recorded.

#### M8.3 Merge and tag · S · deps: M8.1, M8.2
- `nautilus-0.4` merged into `main`; version 0.4.0; tag and release notes.
  From then on, the session hook and `AGENTS.md` apply to every cloud
  session.

## 15. Effort summary

| Milestone | WPs | Rough effort (engineer-weeks) |
|---|---|---|
| M1 | 15 | 17–43 |
| M2 | 9 | 13.5–28 (plus 1–3 for the optional M2.7) |
| M3 | 10 | 13–32 |
| M4 | 11 | 23–48 |
| M5 | 10 | 14.5–33 |
| M6 | 7 | 10–23 |
| M7 | 7 | 18–37 |
| M8 | 3 | 2.5–7 |
| **Total** | **72** | **≈ 111.5–251** |

These are sums of the WP sizes (M2.7 counted as a WP, its effort not
included) and agree with the per-PR numbers of §4.

**Elapsed time** follows from the longest WP chain of §6, summed from the
sizes of §1:

| Segment | WPs on the chain | Weeks |
|---|---|---|
| to the cut-over | M1.1 → M1.2 → M1.7 → M2.1 → M3.1 → M3.2/M3.6 → M3.4/M3.5 → M4.2 | 19–40 |
| to the M4 exit | M4.5 → M4.7 → M4.9 → M4.10 | 7.5–16 |
| to the release | M6.1 → M7.1 → M7.2 → M7.3 → M7.4 → M7.5 → M7.7 → M8.2 → M8.3 | 19–39 |
| **End to end** | | **45.5–95** |

With mid-range sizes (S 0.75, M 2, L 4.5, XL 8 weeks) the chain is about
70 weeks; that is the planning value. The bound holds only if every WP on
the chain is staffed when its dependencies are done; the tracks of §6 need
about 2.5 engineers on average (111.5–251 engineer-weeks over 45.5–95 weeks).
The upper bound shrinks only by splitting the XL WPs on the chain (M2.1,
M7.2) across engineers or by starting the GPU chain earlier. The `tbc`
path to the cut-over (M1.3 → M4.0 → M4.1) is 1–2 weeks shorter than the
tracer path, and the reuse chain (M5.1 → M5.7) joins the GPU chain at
M7.6 with 5–7 weeks of slack. The numbers are for order and staffing and
are revised at each milestone exit.

## 16. Risks specific to the plan

| Risk | Mitigation |
|---|---|
| Integration bugs show up at the cut-over instead of gradually | `NirChain` runs every execution-test function through the new chain before the cut-over; the cut-over needs only `tbc` |
| Few backends between the cut-over and the M4 exit | PR 7 can land before the cut-over, so `asmjit` and `cpp` are on from the start; MLIR follows in PR 8 |
| No side-by-side comparison with 0.3 | recorded baselines refreshed from a `main` checkout in the same container; correctness from the fuzzer's own oracle; `test/api-compat/` frozen after M1.9 |
| Merges from `main` get harder after the cut-over | §3 rule 10: re-implement or record as obsolete; every fix brings a regression test |
| NebulaStream breaks late | its job reports from M1 and gates from the M4 exit; the user-facing API does not change |
| Large PRs are hard to review and drift | one commit (or a few) per WP; drafts opened early and kept up to date; each WP's "done when" is a checklist in the PR description |
| Spec changes after M1 ripple through the code | spec-first rule; `Opcodes.def` generates the dependent tables |
| `asmjit` port regresses code quality | the `asm-diff` leg against the recorded 0.3 assembly |
| GPU support is off from the cut-over to M7 | declared in [17](17-gpu.md); the user API headers stay in the tree, but code that calls the device functions compiles and does not link while the plugin is off (they are defined in `plugins/gpu/src/gpu.cpp`); the 0.3 GPU tests and reference sources are ported, not rewritten |
| No GPU runner and no NVIDIA hardware | D21: Metal on the hosted macOS runner (production ready); the grid emulator and the CUDA compile leg on every PR; CUDA ships as experimental ([17 §1, §5](17-gpu.md)) |
| External prerequisites without an owner | the reference machine (M1.10) and the `tpch-duck` job (M2.3) each get a named owner before M1 starts; until then gates use callgrind counts only |
| No owner for the bare-hardware Metal benchmark machine | an Apple-silicon Mac for the M7.7 benchmarks and the M8.2 manual Metal run (D15) gets a named owner before M7 starts; until then the Metal performance targets are reported from the hosted runner only |
