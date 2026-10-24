# Implementation Plan

Part of the [Nautilus 0.4 design](README.md) · All milestones · Turns the
component documents (01–15) into ordered work packages

## 1. Purpose and how to use it

The component documents say *what* 0.4 builds and why. This document says
*in which order*, *in which files*, *behind which switches*, and *how each
step is proven done*. Every work package (WP) has:

- an id (`M2.4`), a size and its dependencies;
- what it builds, and which files it creates, changes or deletes;
- its tests;
- a "done when" check that a reviewer can verify.

Work packages are units of planning and review, not of merging: they are
grouped into **21 larger PRs** (§1.1), each of which lands one coherent
capability. Inside a PR, each WP is one or a few commits, so the PR can be
reviewed commit by commit.

**Sizes** are rough engineering effort for sequencing, not commitments:
**S** ≤ 1 week, **M** 1–3 weeks, **L** 3–6 weeks, **XL** 6–10 weeks.

### 1.1 Pull requests

Every PR targets `nautilus-0.4`, is titled `[PR n] <capability>`, and is
merged only with the full CI matrix green. Reference-dump regenerations
stay separate commits inside the PR (§2 rule 6). A PR may be opened as a
draft early and kept up to date with `nautilus-0.4`, but it merges only
when every WP in it meets its "done when".

| PR | Capability | Work packages | Needs | Size |
|---|---|---|---|---|
| 1 | NIR format: data structures, verifier, text form, canonical form and hash, binary image, loader fuzzer | M1.1–M1.6 | — | XL |
| 2 | NIR from today's pipeline; one extension registry | M1.7, M1.8 | 1 | L |
| 3 | Chunk pool; `bc` removed | M1.9, M1.10 | — | L |
| 4 | Measurement and agent infrastructure: corpus, reference machine, NebulaStream job, doc check, skills | M1.11–M1.13 | 1 | L |
| 5 | Flat trace (S1): statistics, flat records, trie, cleanup lists, source tags | M2.0–M2.2, M2.8, M2.9 | 1 | XL |
| 6 | Cheaper re-runs and copies; tracing diagnostics; TPC-H job | M2.3–M2.5 | 5 | L |
| 7 | Explicit control flow; constant data | M2.6, M2.7 | 5 | L |
| 8 | Optimizing conversion | M3.1–M3.6, M3.10 | 5 | XL |
| 9 | Legalization, optional passes, plugin pass hooks | M3.7–M3.9 | 2, 8 | L |
| 10 | Backends on NIR, part 1 (S2): backend interface, `tbc`, `cpp`, runtime-state ops | M4.0, M4.1, M4.4, M4.6 | 2 | XL |
| 11 | `asmjit` on NIR; debug info and profiling | M4.2, M4.9 | 10 | XL |
| 12 | MLIR and GPU on NIR | M4.3, M4.5 | 9, 10 | XL |
| 13 | Modules and linking; promotion service; `explain()` | M4.7, M4.8, M4.10 | 10, 12 | XL |
| 14 | Switch-over S3: NIR pipeline by default, dumps regenerated, D3 | M4.11 | 8, 11, 12, 13 | M |
| 15 | Portable reuse: pointers, bindings, provenance, self-contained images, save/load API, compatibility fixtures | M5.1–M5.4, M5.6, M5.9 | 1, 13 | XL |
| 16 | Native payloads and caches L0/L1/L2 | M5.5, M5.7 | 12, 15 | XL |
| 17 | External interfaces: C API, playground MCP server | M5.8, M5.10 | 13, 15 | L |
| 18 | Semantics: D10/D11, declared effects, cheaper exceptions, signless plugin API | M6.1–M6.4 | 14 | L |
| 19 | Extension types and plugin migration; diagnostics catalog and compiled docs | M6.5–M6.7 | 14 | XL |
| 20 | Legacy pipeline and bridge deleted (S4) | M7.1 | 18, 19 | M |
| 21 | Documentation, migration guide, release validation, merge and tag | M7.2–M7.4 | 20 | L |

M2.10 (lazy copies) is an investigation; if it is adopted, it lands as its
own PR after PR 6.

The PRs follow the tracks of §4: PRs 1, 15 (format); 5–7 (frontend); 8, 9
(conversion); 2, 10–12 (backends); 3, 13, 16 (runtime); 4, 17 (infra and
agents). Right after PR 1, PRs 2–5 can be worked on at the same time.

## 2. Ground rules

1. **The tree always works.** From the first PR to the release, every
   merge into `nautilus-0.4` builds and passes every test on the CI matrix.
   New code lands next to old code and takes over at a switch-over point
   (§4). Nothing is half-replaced.
2. **Two pipelines until M4 ends.** An internal option
   `engine.pipeline=legacy|nir` selects the 0.3 pipeline (tracing →
   `IRGraph` → passes → backends) or the 0.4 pipeline (flat trace →
   conversion → legalization → NIR backends). `legacy` is the default until
   switch-over S3; `legacy` is deleted in M7. CI runs both from
   switch-over S2 on (rule 7).
3. **Two bridges, both temporary.**
   - **Bridge B** (M1.7): `IRGraph` after the 0.3 passes → SSA-level NIR. It
     lets the serializer, backends and caches start before the new
     conversion exists, and is the comparison baseline for the conversion.
   - **The legacy conversion reads the flat trace** after M2.1, so the
     tracer is changed once and both pipelines share it.
   - Bridge B and the legacy pipeline are deleted together in M7.
4. **The spec comes first.** A WP that finds a gap or error in
   [13-nir-spec.md](13-nir-spec.md) changes the spec in the same PR, before
   the code. After the M1 exit (spec final), every change follows §11 of the
   spec.
5. **One opcode table.** Opcodes, their operand shapes, flags, verifier
   rules and text names are defined once in `nir/Opcodes.def` (an X-macro
   table). The verifier, printer, parser, C API enum and backend dispatch
   tables are generated from it. This shrinks the `ir-change` checklist and
   keeps the C enum in step with the wire values (09 §3).
6. **Reference dumps change only in isolated commits,** never together with
   a behavior change (`regenerate-dumps` skill); a PR that regenerates dumps
   carries them as their own commit.
7. **CI legs added by this plan** (each added by the WP that needs it):

   | Leg | From | What it runs |
   |---|---|---|
   | `nir-roundtrip` | M1.7 | every execution test with `NAUTILUS_NIR_ROUNDTRIP=1`: bridge → print → parse → serialize → load, byte-identical |
   | `latency-corpus` | M1.11 | corpus under callgrind (gated) and wall clock (reported) |
   | `nebulastream-0.4` | M1.12 | NebulaStream against `nautilus-0.4` (D16) |
   | `nir-pipeline` | M4.1 (S2) | every execution test with `engine.pipeline=nir` on the NIR backends ported so far |
   | `tpch-duck` | M2.4 | nautilus-duck TPC-H tracing and execution |
   | `asm-diff` | M4.2 | `asmjit` assembly of the corpus against the recorded 0.3 baseline |
   | `doc-check` | M1.13 | paths, targets, options and skills named in `AGENTS.md` and skills exist |

8. **Agent docs are part of the change.** A WP that adds, moves or renames
   a component updates the `AGENTS.md` that describes it and the affected
   skills ([15 §3.1](15-agent-experience.md)).
9. **Measure before and after.** Every WP with a performance claim records
   the corpus numbers before and after in its PR description.

## 3. Source layout at the end of 0.4

| Path | Content | Created in |
|---|---|---|
| `nautilus/src/nautilus/nir/` | `Opcodes.def`, `Types`, `Inst`, `Module` (sections), `ConstPool`, `Builder`, `Rewriter`, `Verifier`, `Printer`, `Parser`, `Canonicalize`, `Hash`, `Image` (writer, loader, inspect) | M1 |
| `nautilus/src/nautilus/extensions/` | `ExtensionRegistry`, `OpInfo`/`TypeInfo`, intrinsic shim | M1 |
| `nautilus/src/nautilus/common/ChunkPool.*` | the bounded chunk pool and its arenas | M1 |
| `nautilus/src/nautilus/tracing/` | flat-trace recorder, tag trie and tag map, replay cursor, explicit control flow | M2 |
| `nautilus/src/nautilus/conversion/` | optimizing conversion, fold/simplify rules, CSE, landing pads, attribute inference | M3 |
| `nautilus/src/nautilus/passes/` | legalization; optional copy passes (LICM, strength reduction, inlining); plugin pass hooks | M3 |
| `nautilus/src/nautilus/compiler/backends/*` | the backends, reading NIR | M4 |
| `nautilus/src/nautilus/compiler/PromotionService.*` | tier-1 workers, jobs, status | M4 |
| `nautilus/src/nautilus/link/` | module registry, import slots, `funcaddr` stubs | M4 |
| `nautilus/src/nautilus/serialization/` | pointers, bindings, provenance, artifacts, self-contained export | M5 |
| `plugins/cache/` | L0/L1/L2 cache provider (`ENABLE_CACHE_PLUGIN`) | M5 |
| `nautilus/include/nautilus/c/` | `common.h`, `ir.h`, `engine.h` | M5 |
| `nautilus/include/nautilus/control_flow.hpp`, `data.hpp` | explicit control flow; `constData` | M2 |
| `third_party/xxhash/` | XXH3-128 for the structural hash (BSD-2) | M1 |

**Deleted in 0.4:** `compiler/backends/bc/` (M1.10);
`compiler/ir/` including `IRGraph`, 40 operation classes, `passes/`,
`FunctionRewriter` and `IntrinsicRegistry` (M7, except the shim);
`tracing/TraceOperation.*`, `tracing/Block.*` and the old
`ExecutionTrace` records (M2.1); the bridge (M7).

## 4. Order, dependencies and switch-overs

```
M1.1 NIR core ──┬─► M1.2 verifier ─► M1.3 printer/parser ─► M1.4 canon+hash ─► M1.5 image
                ├─► M1.7 bridge B ──────────────────────────────────────┐
                ├─► M1.8 extension registry                             │
                └─► M2.1 flat trace ─► M2.2…M2.9 frontend               │
                                   └─► M3.1 conversion skeleton ◄───────┤ (compare)
M1.9 chunk pool, M1.10 drop bc, M1.11 corpus, M1.12 NebulaStream (independent)
                                                                        ▼
                         M4.0 backend interface ─► M4.1 tbc ─► (S2: nir pipeline runs on tbc)
                                                     ├─► M4.2 asmjit ─┐
M3.2…M3.10 conversion, legalization, passes ─────────┤   M4.3 MLIR ───┤
                                                     │   M4.4 cpp, M4.5 GPU, M4.6–M4.10
                                                     ▼                ▼
                                      S3: engine.pipeline=nir default (M4.11)
                                                     ▼
              M5 reuse (M5.1–M5.4 can start after M1.5; native payloads need M4.3)
                                                     ▼
              M6 semantics and extensions ─► M7 cleanup and release
```

**Critical path:** two chains of similar length meet at M4.11:
- frontend and conversion: M1.1 → M2.1 → M3.1 → M3.2–M3.5;
- backends: M1.1 → M1.7 → M4.0 → M4.1 → M4.2 (and M4.3 → M4.8).

After M4.11 it continues M5.5 → M5.7 → M6.5 → M6.6 → M7. The `tbc` port
(M4.1) is deliberately early: once it runs NIR, the conversion's output can
be executed and fuzzed before the other backends are ported.

**Switch-overs:**

| Id | When | What changes for users of `nautilus-0.4` |
|---|---|---|
| S1 | end of M2.1 | the tracer records the flat trace; both pipelines read it |
| S2 | end of M4.1 | `engine.pipeline=nir` works end to end on `tbc`; the `nir-pipeline` CI leg starts |
| S3 | end of M4 (M4.11) | `nir` becomes the default with every backend; `test/data` regenerated in NIR format; `legacy` kept only as a fallback |
| S4 | M7.1 | `legacy`, bridge B and the 0.3 IR are deleted |

**Parallel tracks** (who can work at the same time once M1.1 has landed):

| Track | Work packages |
|---|---|
| A: format | M1.2–M1.6, then M5.1–M5.6 |
| B: frontend | M2.1–M2.11 |
| C: conversion | M3.1–M3.10 |
| D: backends | M1.7, M4.0–M4.5, M4.10 |
| E: runtime | M1.9, M1.10, M4.6–M4.9, M5.7 |
| F: infra and agents | M1.11–M1.13, CI legs, agent items per milestone |

## 5. M1 Foundations

Goal: NIR exists, is specified, verified, printed, parsed, serialized and
fed from today's pipeline; the measuring infrastructure runs. Exit
criteria: README §6 and [02 §8](02-nir.md).

#### M1.1 NIR core data structures · L · deps: —
- **Builds:** `Inst` (16 B), value references with the constant bit and
  `NONE`, `extra`, `ConstPool` with interning and provenance class, every
  section of [13 §4.1](13-nir-spec.md) as flat arrays, `Module`,
  `Function`, `BlockInfo`; `Builder` (appends, mints dense ids, maintains
  `useCount`); `Rewriter` (copy with old→new map, `forward[]`); CSR use
  table on request. Signless types and op flags from the start
  ([13 §2](13-nir-spec.md)).
- **Files:** new `nir/Opcodes.def`, `Types.hpp`, `Inst.hpp`, `Module.{hpp,cpp}`,
  `ConstPool.{hpp,cpp}`, `Builder.{hpp,cpp}`, `Rewriter.{hpp,cpp}`; CMake
  target `nautilus-nir` (no dependency on `compiler/ir`).
- **Tests:** `test/nir-tests/` (new binary `nautilus-nir-tests`): record
  sizes (`static_assert`), builder invariants, interning, rewriter
  compaction, CSR uses.
- **Done when:** a hand-built module of every opcode in `Opcodes.def` can
  be built and rewritten; no hash map on a per-operand path.

#### M1.2 Verifier, both levels · M · deps: M1.1
- **Builds:** every rule of [13 §6](13-nir-spec.md) and the per-opcode
  operand rules from `Opcodes.def`; level-specific checks; extension
  instructions against `Signature` and `verify`. Each rule has a stable code
  (`NIR-V001`…) and a message naming function, block and value.
- **Files:** `nir/Verifier.{hpp,cpp}`, `nir/Diagnostics.def` (codes).
- **Tests:** one positive and one negative test per rule and per opcode;
  `NAUTILUS_IR_VERIFY=1` also runs it on NIR.
- **Done when:** the code list in `Diagnostics.def` and the spec agree, and
  each code is triggered by a test.

#### M1.3 Printer and parser · M · deps: M1.2
- **Builds:** the text form of [13 §9](13-nir-spec.md) for both levels;
  a parser producing the same module as the binary image; parse errors with
  line and column.
- **Files:** `nir/Printer.{hpp,cpp}`, `nir/Parser.{hpp,cpp}`; a
  `nautilus-nir-opt` tool in `tools/` (parse → optional pass → print) used
  by FileCheck-style tests.
- **Tests:** print → parse → print byte-identical on every test module;
  `.nir` test files under `test/nir-tests/text/` with expected output.
- **Done when:** a test can be written as a `.nir` file plus expected output
  and run through ctest.

#### M1.4 Canonical form and structural hash · M · deps: M1.3
- **Builds:** canonicalization of [13 §7](13-nir-spec.md) (apply
  `forward`, compact, renumber, intern in first-use order); XXH3-128 over
  the anonymous canonical image, typed sorted options and extension
  versions; interface hashing of imports.
- **Files:** `nir/Canonicalize.{hpp,cpp}`, `nir/Hash.{hpp,cpp}`,
  `third_party/xxhash/` (vendored, pinned).
- **Tests:** structurally equal modules built in different orders hash
  equally; renaming the module does not change the hash; changing an option
  does.
- **Done when:** two processes produce the same hash for the same module
  (the determinism test of [12 §3](12-validation.md) row 07).

#### M1.5 Binary image: writer, loader, inspect · L · deps: M1.4
- **Builds:** [13 §8](13-nir-spec.md): layout, strides, feature bits,
  CRC-32C, framing, load limits; zero-copy load (validate, then use in
  place); `inspect` without materializing; extension names mapped to
  process ids on load ([13 §8.4](13-nir-spec.md)). The format rules of #532
  carried over; #532's own 0.3 format is not.
- **Files:** `nir/Image.{hpp,cpp}`, `nir/ImageLoader.{hpp,cpp}`,
  `nir/Inspect.{hpp,cpp}`.
- **Tests:** golden images (valid and invalid) with expected results; the
  annotated hex dump of [13 §11](13-nir-spec.md) checked byte for byte;
  re-serialization byte-identical.
- **Done when:** every golden image loads or is rejected exactly as listed.

#### M1.6 Loader and verifier fuzzer · S · deps: M1.5
- **Builds:** a libFuzzer target over malformed images and mutated text,
  seeded with the golden corpus; runs in the existing libFuzzer CI canary.
- **Files:** `test/fuzz/NirImageFuzz.cpp`; `fuzz-triage` skill.
- **Done when:** one CPU-hour without a crash or sanitizer report; the CI
  canary includes it.

#### M1.7 Bridge B: `IRGraph` → SSA NIR · M · deps: M1.1
- **Builds:** a converter from the 0.3 IR after its passes to SSA-level
  NIR: signed types → widths plus op flags, exception regions → landing
  pads, intrinsic calls → intrinsic callees, constants → `ConstPool`.
- **Files:** `nir/bridge/IRGraphToNIR.{hpp,cpp}` (deleted in M7);
  `NAUTILUS_NIR_ROUNDTRIP` hook in `CompilationPipeline`.
- **Tests:** the `nir-roundtrip` CI leg over every execution test.
- **Done when:** every execution test's IR round-trips byte-identically
  (02 §8).

#### M1.8 `ExtensionRegistry` · M · deps: M1.1
- **Builds:** one registry for ops, types, intrinsics and passes with dense
  ids ([04 §3](04-extensions.md)); frozen after the first compile; lock-free
  lookups; late registration throws (D13). `IntrinsicRegistry` becomes a
  shim that registers 0.3 intrinsics as intrinsic callees and warns once
  (D17).
- **Files:** `extensions/ExtensionRegistry.{hpp,cpp}`, `extensions/OpInfo.hpp`,
  `extensions/TypeInfo.hpp`; `compiler/ir/IntrinsicRegistry.*` forwards to it.
- **Tests:** every plugin suite unchanged and green; late registration
  test; TSan plus a lock-count check on lookups.
- **Done when:** M1 exit criterion of [04 §5](04-extensions.md).

#### M1.9 Bounded chunk pool · M · deps: —
- **Builds:** [02 §4.1](02-nir.md): fixed chunk sizes, dedicated large
  allocations, per-thread cache plus engine pool, byte budget with decay,
  `mmap` and `madvise` release, `NautilusEngine::trimMemory()`, embedder
  chunk provider, counters. Replaces both `ArenaPool`s.
- **Files:** `common/ChunkPool.{hpp,cpp}`, `common/Arena.*` (adapted);
  option `engine.arena.retainedBytes` in `docs/options.md`.
- **Tests:** pool unit tests (budget, decay, trim, page release); the
  40,000-module long-running test on counters.
- **Done when:** after a 20,000-op compile, held bytes return to the budget;
  no corpus latency regression.

#### M1.10 Remove `bc` · S · deps: —
- **Builds:** [05 §6.1](05-backends.md): first confirm `tbc` passes every
  test `bc` passes and is no slower on `ExecutionBenchmark`; then delete.
- **Files:** delete `compiler/backends/bc/`, `tools/bc_guard.sh`,
  `tools/bc_perf_verify.py`; `ENABLE_BC_BACKEND` removed with an obsolete
  message; tier-0 fallback `asmjit` → `tbc` → interpreter; `bc` cases in
  tests move to `tbc`; `example/src/DemoBackends.cpp`, `docs/engine.md`,
  `.github/nebulastream/benchmark.toml`; check whether `third_party/dyncall`
  is still used.
- **Done when:** no reference to `bc` remains outside the change log and
  the migration notes; `engine.backend=bc` fails naming `tbc`.

#### M1.11 Latency corpus and reference machine · M · deps: —
- **Builds:** the corpus driver from the design study (closed #562) as
  `tools/latency-corpus/` (target `nautilus-latency-corpus`) with JSON
  output; callgrind gates in CI; the dedicated reference machine set up and
  recorded ([10 §2](10-observability.md)).
- **Done when:** the P0 numbers are reproduced and stored as the baseline;
  the `latency-corpus` leg runs.

#### M1.12 NebulaStream job against `nautilus-0.4` · S · deps: —
- **Builds:** the existing NebulaStream CI job parameterized on the branch
  (D16).
- **Done when:** the job runs on every `nautilus-0.4` push.

#### M1.13 Agent items for M1 · M · deps: M1.3
- **Builds:** `doc-check` script and CI leg; `ir-change` skill rewritten
  for NIR (pointing at `Opcodes.def` and the spec sections);
  `steward` skill (PR conventions for this branch); `fuzz-triage` (M1.6).
- **Done when:** `doc-check` passes and fails on a planted stale path.

**M1 exit:** the spec is marked final ([13 §11](13-nir-spec.md)); M1.1–M1.12
done; README §6 M1 criterion met.

## 6. M2 Frontend

Goal: the tracer writes pre-SSA NIR and is cheaper; explicit control flow
can be mixed with implicit. Exit criteria: [01 §7](01-tracing-frontend.md).

#### M2.0 Tracing statistics and counters · S · deps: —
- **Builds:** port #561's tracing statistics; add R1 (follow, record and
  paused time, `tracing.pausedOperations`, per-scope re-runs) and C1 (copies
  per kind). Done first so every later WP is measured.
- **Files:** `tracing/TraceContext.*`, `CompilationStatistics.*`.
- **Done when:** all keys of [10 §3](10-observability.md) for tracing are
  reported and unit-tested.

#### M2.1 Flat trace · XL · deps: M1.1, M2.0
- **Builds:** the recorder writes `FlatTrace` ([01 §6](01-tracing-frontend.md)):
  16 B records, operands as value refs, global op ids, blocks as arena lists
  of op ids, `traceExtOp`; the tag map as open addressing from snapshot to
  op id (merge splits move one list tail, no map rewrites); the index-based
  replay cursor; the value-ref free list (R2). The legacy
  `TraceToIRConversionPhase` is adapted to read the flat trace (switch-over
  S1).
- **Files:** `tracing/TraceContext.*`, `tracing/ExecutionTrace.*` (rewritten
  on NIR), `tracing/Snapshot.*`; delete `tracing/TraceOperation.*` and
  `tracing/Block.*`; `tracing/phases/TraceToIRConversionPhase.cpp`
  (input side only).
- **Tests:** every execution and tracing test; the trace dumps change format,
  so `test/data/*/tracing/` is regenerated in an isolated commit right after.
- **Done when:** no `malloc` per recorded op in steady state (corpus
  allocation counter); results unchanged on every test.

#### M2.2 Hashed trie children · S · deps: —
- **Builds:** open-addressing children above ~8 per node in `tag/Trie.hpp`.
- **Done when:** `distinctSites1024` tag creation is linear; part of the
  ≥ 30% target.

#### M2.3 Cheaper paused passes: R3, then R4 · M · deps: M2.1
- **Builds:** R3, the minimal paused path; R4, unwinding paused passes with
  a tracer-private exception, behind `tracing.unwindPausedPasses` (default
  off), with fallback to R3 when user code swallows it; regions in follow
  mode look up their memo by region-entry op id.
- **Tests:** fuzzer runs with R4 on and off; tests with `catch (...)` and
  `noexcept` frames in traced code.
- **Done when:** `tracing.pausedOperations` ≈ 0 with R4 on; the R4 default
  is decided from fuzzer, NebulaStream and TPC-H data and recorded.

#### M2.4 TPC-H tracing job and copy reduction (C2) · M · deps: M2.0
- **Builds:** the `tpch-duck` CI leg; `const val<T>&` parameters in the
  `std` plugin and hot helpers; `nautilus-review` flags by-value `val`
  parameters.
- **Done when:** recorded copies on TPC-H fall by at least a quarter (with
  M2.1) and the re-run share is reported per run.

#### M2.5 Replay diagnostics (R5) and tracing diagnostic codes · M · deps: M2.0
- **Builds:** diagnostics with code, source location and hint
  (`NAUT-T001`…) for path explosion, replay share, constant loops and values
  escaping regions ([15 §4.1](15-agent-experience.md)); `debug-trace` skill.
- **Done when:** each code has a test that triggers it.

#### M2.6 Explicit control flow on region scopes · L · deps: M2.1
- **Builds:** port of #300 ([01 §5](01-tracing-frontend.md)): `If` (both
  forms), `While`, `For`, `Break`, `Continue`, `Return`; arms and bodies as
  region scopes; memoized like a region; optimistic recording with bounded
  restart; direct mode runs lambdas natively.
- **Files:** `include/nautilus/control_flow.hpp`; `tracing/` region support;
  `test/execution-tests/ExplicitControlFlowTest.cpp` from #300, with its
  mixing guard turned into mixing tests; fuzzer generator for explicit
  constructs and a mode that forces restarts.
- **Done when:** explicit `chainedIf` traces in one pass with zero replayed
  ops at every corpus size; mixed functions clean under the fuzzer.

#### M2.7 Constant data · S · deps: M2.1
- **Builds:** `nautilus::constData` (span and string overloads) creating
  data objects and `data.addr` ([13 §4.10](13-nir-spec.md)); the legacy
  pipeline lowers `data.addr` to a constant pointer into module-owned
  memory until S3.
- **Files:** `include/nautilus/data.hpp`.
- **Done when:** a lookup table traced with `constData` produces a
  persistable module and loads from it fold (after M3.2).

#### M2.8 Cleanup lists in the trace · S · deps: M2.1
- **Builds:** destructor cleanup lists in unwind entries at the pre-SSA
  level ([13 §3.4](13-nir-spec.md)) instead of 0.3's exception-region
  bookkeeping on the trace.
- **Done when:** exception tests green on both pipelines.

#### M2.9 Source tags on request · S · deps: M2.1
- **Builds:** `debugLocations`/`debugRegions` sections filled only when a
  debug or profiling option is set (D14).
- **Done when:** a default compile records no source tags (counter check).

#### M2.10 Lazy copies (C4), investigation · M · deps: M2.1 · optional
- **Builds:** a prototype behind a flag. Adopted only if the fuzzer and every
  `tracing/` reference trace agree; otherwise the result is written up and
  the flag removed.

**M2 exit:** [01 §7](01-tracing-frontend.md) on the corpus and TPC-H.

## 7. M3 Conversion and passes

Goal: one optimizing conversion replaces the fixed-point pipeline on the
critical path. Exit criteria: [03 §6](03-conversion-and-passes.md).

#### M3.1 Conversion skeleton · L · deps: M2.1, M1.2
- **Builds:** pre-SSA → SSA in RPO with Braun-style SSA construction,
  live-edge tracking, layout order (RPO, loops contiguous), `BlockInfo`
  loop depth and end, `useCount` while emitting. No folding yet.
  `engine.pipeline=nir` wired up to it; until `tbc` runs NIR (S2), its
  output is verified and compared against bridge B.
- **Files:** `conversion/Conversion.{hpp,cpp}`, `conversion/SSABuilder.*`,
  `conversion/Layout.*`.
- **Tests:** verifier on every converted function; op counts compared with
  bridge B output (report).
- **Done when:** every execution test converts and verifies.

#### M3.2 Folding and simplification rules · M · deps: M3.1
- **Builds:** the rules of `ConstantFoldingAndCopyPropagationPass` and
  `AlgebraicSimplificationPass` as builder rules, following IEEE (D11: no
  `x + 0.0 → x`) and wrapping arithmetic; extension `fold`/`simplify` hooks;
  loads from data objects fold.
- **Files:** `conversion/FoldRules.*`, `conversion/SimplifyRules.*`.
- **Tests:** a `.nir` test per rule (M1.3); fuzzer.

#### M3.3 Branch folding, dead blocks, threading · M · deps: M3.1
- **Builds:** constant `condbr` adds only the taken edge; dead blocks never
  emitted; single-predecessor jump threading (block merging, empty-block
  elimination); sealing never waits on folded-away predecessors.
- **Tests:** merge-heavy corpora (regions, nested loops, `break`).

#### M3.4 CSE · M · deps: M3.2
- **Builds:** block-local CSE with an undo log; loads and calls keyed on the
  effect id; dominator-scoped CSE (Cooper–Harvey–Kennedy) behind a flag
  until its fuzzing is clean.

#### M3.5 Landing pads · M · deps: M2.8, M3.1
- **Builds:** cleanup lists → shared landing pads keyed by ordered
  destructor set; "no cleanup" entries where nothing is live (replaces
  `ExceptionRegionPreparationPass`).
- **Tests:** every exception test on the NIR pipeline.

#### M3.6 Attribute inference in the conversion · S · deps: M3.1
- **Builds:** the analyses of `NoThrowInferencePass` and
  `FunctionAttributeInferencePass` run once on the function table; guarantee
  bits of [13 §4.6.2](13-nir-spec.md).

#### M3.7 Legalization · M · deps: M3.1, M1.8
- **Builds:** one copy pass per backend: extension ops without a handler →
  fallback call or expansion; extension types without support → stack slots;
  ABI lowering where a backend asks for it ([03 §4](03-conversion-and-passes.md)).
- **Files:** `passes/Legalization.*`.

#### M3.8 Optional passes as copy passes · L · deps: M3.1
- **Builds:** LICM, strength reduction and inlining on NIR, off on
  latency-critical compiles; inlining across modules records exporter hashes
  and skips functions using `bind.addr` ([13 §4.4.3](13-nir-spec.md));
  `ir.runOptimizationPasses` and `ir.disable*` kept; options of removed
  passes warn.
- **Files:** `passes/LICM.*`, `passes/StrengthReduction.*`, `passes/Inline.*`.

#### M3.9 Plugin pass hooks · S · deps: M3.7
- **Builds:** `afterConversion` and `beforeLowering` with required/optional
  marking ([03 §5](03-conversion-and-passes.md)).

#### M3.10 Op-count report and M3 gate · S · deps: M3.2–M3.5
- **Builds:** the bridge-vs-conversion op-count report in the corpus output;
  compile-fail suite for the public headers and the `extension-plugin` skill
  (agent items for M3, [15 §5](15-agent-experience.md)).
- **Done when:** [03 §6](03-conversion-and-passes.md) holds on the corpus
  and TPC-H kernels.

## 8. M4 Backends and tiering

Goal: every backend reads NIR; `nir` becomes the default pipeline; the
promotion service replaces thread-per-module. Exit criteria:
[05 §9](05-backends.md), [11 §7](11-tiered-compilation.md).

#### M4.0 Backend interface on NIR · M · deps: M1.7, M1.8
- **Builds:** `CompilationBackend` takes a legalized `nir::Module`; array
  side tables helpers (value → slot, block → label); per-backend extension
  lowering tables (`extLowering`, `extTypes`) and their registration API,
  replacing `MLIRIntrinsicPlugin` and `AsmJitBackendIntrinsic` (shims kept).
- **Files:** `compiler/backends/CompilationBackend.*`, `compiler/Frame.hpp`
  (removed in favor of arrays).

#### M4.1 `tbc` on NIR · L · deps: M4.0
- **Builds:** near 1:1 lowering; register allocation on value ids; handler
  table indexed by extension opcode; copy-and-patch mode kept.
- **Done when:** switch-over S2: every execution test passes with
  `engine.pipeline=nir engine.backend=tbc`; the `nir-pipeline` leg runs.

#### M4.2 `asmjit` on NIR · XL · deps: M4.1
- **Builds:** the steps of [05 §4.4](05-backends.md): providers on NIR with
  array side tables and `ConstPool` immediates; virtual registers created on
  first use and reused after last use; #565's call-heavy fix confirmed on
  NIR on x86-64 and AArch64. AsmJit's `Compiler`, RA and post-RA peepholes
  unchanged.
- **Files:** `compiler/backends/asmjit/*LoweringProvider.*`,
  `AsmJitCompilationBackend.*`; `asm-diff` CI leg and skill.
- **Done when:** [05 §4.3](05-backends.md): assembly diff shows no
  code-quality regression; backend ≤ 0.25 µs/op on linear code; every
  `asmjit` test green on both architectures.

#### M4.3 MLIR on NIR with state reuse · L · deps: M4.0
- **Builds:** NIR blocks and terminators → `cf`, arithmetic → `arith`,
  extension types through the plugin type mapping; #561's per-thread
  `TargetMachine` and `MLIRContext` reuse, single verification, data layout
  for `LLJIT`; DWARF from the debug sections.
- **Files:** `compiler/backends/mlir/MLIRLoweringProvider.*`,
  `MLIRCompilationBackend.*`, `JITCompiler.*`; `test/llvm-ir-test/` reference
  IR regenerated in an isolated commit.

#### M4.4 `cpp` on NIR · S · deps: M4.0
- **Builds:** printing from NIR; extension ops as expression templates or
  calls.

#### M4.5 GPU backends on NIR · M · deps: M4.0, M3.7
- **Builds:** CUDA and Metal plugin backends read NIR; thread ids and
  barriers as target-restricted extension ops; address spaces as extension
  types (D15: CUDA compile-only in CI, Metal where the runner exposes a
  device).

#### M4.6 Runtime-state ops in every backend · S · deps: M4.1–M4.4
- **Builds:** `bind.addr` (addresses supplied by the module until M5.2),
  `data.addr`, `funcaddr` with stable forwarding stubs.

#### M4.7 Module registry and linking · M · deps: M4.6
- **Builds:** [13 §4.4](13-nir-spec.md): named modules, exports, import
  declarations; linking by exact signature and guarantee bits, acyclic;
  import slots updated with a release store on promotion; lifecycle built →
  linked → compiled → released; host API of §4.4.4.
- **Files:** `link/ModuleRegistry.*`, `link/ImportSlots.*`; public API in
  `include/nautilus/Module.hpp`.
- **Tests:** cycles, signature mismatch and guarantee mismatch rejected;
  promotion of an exporter seen by importers; release order.

#### M4.8 Promotion service · L · deps: M4.3
- **Builds:** [11 §4](11-tiered-compilation.md): engine-wide worker pool,
  admission cap, lower OS priority; job states and cancellation at stage
  boundaries; shutdown without held mutexes; status API
  (`tier()`, `promotionStatus()`, `waitForPromotion`, `promote()`,
  `NautilusEngine::waitForPromotions()`); policies `eager` and `manual`
  (D8); early start after conversion; options settable alone; one compile
  path with linked unit IDs; `waitForPendingPromotions()` kept as a
  deprecated alias.
- **Files:** `compiler/PromotionService.*`, `compiler/TieredCompiler.*`,
  `include/nautilus/Module.hpp`, `docs/options.md`, `docs/engine.md`.
- **Tests:** [11 §6](11-tiered-compilation.md) including fault injection.

#### M4.9 Debug info and profiling (D14) · M · deps: M4.2, M4.1
- **Builds:** `asmjit` symbols per function (and per region with source
  locations) in `JitSymbolRegistry` and the jitdump; `tbc` handler ranges;
  `DebugInfoExecutionTest` and `PerfJitDumpExecutionTest` on NIR; a
  `perf.sample` test on `asmjit`.

#### M4.10 Agent items for M4 · M · deps: M4.8
- **Builds:** `CompiledModule::explain()` (JSON too);
  `tools/agent/affected-tests`; `memory-check` skill
  ([15 §5](15-agent-experience.md)).

#### M4.11 Switch-over S3 · M · deps: M4.1–M4.9, M3 exit
- **Builds:** `engine.pipeline=nir` becomes the default; `test/data`
  regenerated in NIR format in one isolated commit; the M4 gates measured;
  decision D3 taken and recorded in the README.
- **Done when:** [05 §9](05-backends.md) and [11 §7](11-tiered-compilation.md)
  hold; NebulaStream and TPC-H green on the default pipeline.

## 9. M5 Reuse

Goal: modules can be saved, loaded and cached; the C API is available.
Exit criteria: [07 §6](07-serialization.md), [08 §5](08-caching.md),
[09 §4](09-c-api.md). M5.1–M5.4 can start after M1.5 on bridge output.

#### M5.1 Pointers: process-local and portable · M · deps: M1.5
- **Builds:** pointer kinds; `ProcessLocal` with a per-process token;
  `Portable` through `SymbolNamer`/`SymbolResolver` (`SymbolTable`,
  `dladdr`/`dlsym`) and ELF build ID plus offset ([07 §3](07-serialization.md)).

#### M5.2 Runtime bindings · M · deps: M5.1, M4.6
- **Builds:** port of #529 onto `bind.addr` and the `bindings` section:
  `RuntimeBinding<T>`, address-free schema, fresh resolution per load,
  identities never merged by aliasing (no `bc` path).

#### M5.3 Provenance and eligibility · M · deps: M5.1
- **Builds:** provenance classes on constants, allocation evidence, pointer
  portability, extension payload portability; eligibility scan before
  optimization; fail closed with `artifact.ineligibleReason`
  ([07 §5](07-serialization.md)).
- **Tests:** every negative test of #527 and #528 ported.

#### M5.4 Self-contained export · S · deps: M4.7, M1.5
- **Builds:** saving bundles the transitive closure of imports as internal
  functions ([13 §8.5](13-nir-spec.md)); `imports` is empty in every image.

#### M5.5 Artifacts and native payloads · L · deps: M5.3, M4.3
- **Builds:** the artifact container and compatibility descriptor
  ([07 §2](07-serialization.md)); one relocatable object per payload from
  MLIR/LLVM with #527's import table, personality and unwind-resume bridges;
  loader that falls back to the NIR image.
- **Done when:** a native hit in a fresh process with ASLR records no
  tracing, conversion or codegen.

#### M5.6 Public save/load API · S · deps: M5.5
- **Builds:** `saveModule`, `inspectModule`, `loadModule` with signature
  checks including width and signedness; `engine.retainIR` removed.

#### M5.7 Caches L0, L1, L2 and tier interplay · L · deps: M5.6, M4.8
- **Builds:** `plugins/cache/` from #528 (storage model unchanged), L0 in
  memory with LRU, L1 semantic key with compatibility identity, L2
  structural key with full comparison on hit; tier-1 cache hit skips tier 0;
  promotion results feed the caches ([11 §4.6](11-tiered-compilation.md));
  cache statistics; cache cases in the corpus.
- **Done when:** [08 §5](08-caching.md) holds; warm NebulaStream ≥ 60%
  faster than cold.

#### M5.8 C API on NIR · L · deps: M1.5, M4.7
- **Builds:** port of #531 ([09](09-c-api.md)): handles as `u32` ids, C
  enums generated from `Opcodes.def`, extension ops by name, modules,
  imports and data objects, save/load/inspect; `libnautilus-c` export test
  in the matrix; a Rust bindgen smoke test.

#### M5.9 Compatibility fixtures · S · deps: M5.6
- **Builds:** images saved by this milestone checked into
  `test/nir-tests/compat/` and loaded by every later build (D12).

#### M5.10 Playground MCP server · M · deps: M4.10
- **Builds:** MCP tools `compile`, `stages`, `run`, `benchmark`, `explain`
  over the playground runner, local by default
  ([15 §4.4](15-agent-experience.md)).

## 10. M6 Semantics and extensions

Goal: the defined semantics of D10/D11, declared effects, and plugins on
extension ops and types. Exit criteria: [06 §6](06-semantics.md),
[04 §5](04-extensions.md).

Signless integers are not a separate step here: NIR is signless from M1.1
and the tracer emits signless types with op flags from M2.1. M6.4 removes
the old signed `Type` enum from the plugin-facing API.

#### M6.1 Defined integer and float semantics · M · deps: M4.11
- **Builds:** checked `div`/`rem` with unwind entries throwing
  `nautilus::ArithmeticException`, checks dropped where folding proves the
  divisor safe; shift masks only where the target does not mask; saturating
  float→int casts in every backend; fuzzer generation of the edge cases.
- **Done when:** D10/D11 cases pass on every backend; the cost on
  `ExecutionBenchmark` and TPC-H is measured and recorded (the `unchecked`
  fallback of [06 §7](06-semantics.md) taken only if it is large).

#### M6.2 Declared effects and `nothrow` · M · deps: M3.6
- **Builds:** `invoke<no_throw>`, the `nothrow` attribute and effect
  declarations mapped to guarantee bits; declarations take precedence over
  inference; debug-mode checks (terminate handler, shadow effect checks).
- **Tests:** wrongly declared callees caught by the debug checks.

#### M6.3 Cheaper exception handling · M · deps: M6.2, M4.3
- **Builds:** no capture thunk on MLIR call sites without cleanup;
  annotations in the `std` plugin and the NebulaStream hot runtime
  functions; share of call sites with exception handling in the statistics.

#### M6.4 Plugin API on widths and op flags · S · deps: M4.11
- **Builds:** `isUnsignedOp(inst)`; the signed `Type` enum removed from
  plugin-facing headers; plugins switched.

#### M6.5 Extension types · L · deps: M3.7, M4.2, M4.3
- **Builds:** `TypeInfo`, interned parameterized types, multi-part values,
  stack-slot fallback; `simd.vec<T,N>` and `builtin.i128` on every backend.

#### M6.6 In-tree plugin migration · L · deps: M6.5
- **Builds:** the table of [04 §4](04-extensions.md): `builtin` prefetch,
  overflow and int128; `simd` ops and **removal of the ring buffer**;
  `specialization` assume; `gpu` ops; `std` effect declarations.
- **Tests:** every plugin suite on every backend; fuzzer with handlers on
  and off; debug mode that ignores traits.

#### M6.7 Agent items for M6 · M · deps: M2.5
- **Builds:** `docs/diagnostics.md` complete; `docs/` code examples
  extracted and compiled in CI.

## 11. M7 Release

#### M7.1 Delete the legacy pipeline (S4) · M · deps: M6
- **Deletes:** `compiler/ir/` (except the intrinsic shim), bridge B,
  `engine.pipeline`, the old passes, `IRPassBenchmark`, legacy
  `TraceToIRConversionPhase`.
- **Done when:** the tree builds without them and every suite is green.

#### M7.2 Documentation and migration guide · M · deps: M7.1
- **Builds:** README §6.2 updates; new documents (extension API,
  serialization and caching, C API, migration guide from 0.3); `llms.txt`
  and `docs/api.md`; user plugin with its skills, including
  `migrate-0.3-to-0.4` ([15 §4.5](15-agent-experience.md)).

#### M7.3 Release validation · M · deps: M7.1
- The release criteria of README §6 on x86-64 and AArch64 with ASan/UBSan
  and TSan; the GPU manual run recorded (D15); the reference-machine
  numbers in the release notes; NebulaStream and nautilus-duck on the
  release candidate.

#### M7.4 Merge and tag · S · deps: M7.2, M7.3
- `nautilus-0.4` merged into `main`; version set to 0.4.0; tag and release
  notes. From then on, the session hook and `AGENTS.md` apply to every cloud
  session.

## 12. Effort summary

| Milestone | WPs | Rough effort |
|---|---|---|
| M1 | 13 | 20–35 engineer-weeks |
| M2 | 11 | 18–32 |
| M3 | 10 | 15–28 |
| M4 | 12 | 25–45 |
| M5 | 10 | 15–33 |
| M6 | 7 | 11–25 |
| M7 | 4 | 4–10 |
| **Total** | **67** | **≈ 110–210 engineer-weeks** |

With tracks A–F of §4 running in parallel, the critical path (§4) is about
40–60 weeks of elapsed time; it is dominated by M2.1, M3.1, M4.1–M4.3,
M5.5 and M6.6. The numbers are for planning order and staffing, and are revised at
each milestone exit.

## 13. Risks specific to the plan

| Risk | Mitigation |
|---|---|
| Large PRs are hard to review and drift from the branch | one commit (or a few) per WP, reviewable in order; drafts opened early and merged with `nautilus-0.4` weekly; the "done when" of each WP is a checklist in the PR description |
| Two pipelines double the test load | the `nir-pipeline` leg runs only the backends ported so far; legacy is deleted in M7 |
| Bridge B hides conversion bugs (it starts from optimized IR) | the conversion is compared on results and op counts, not on IR equality; the fuzzer runs on the NIR pipeline from S2 |
| The legacy conversion adapted to the flat trace (M2.1) breaks 0.3 behavior | it is covered by every execution and tracing test before S1 lands |
| M2.1 is large and blocks M3.1 | M3.2–M3.9 develop rule and pass code against bridge B and `.nir` tests while M2.1 is in progress |
| Spec changes after M1 ripple through the code | spec-first rule (§2 rule 4); `Opcodes.def` generates the dependent tables |
| `asmjit` port regresses code quality | the `asm-diff` leg against the 0.3 baseline from the first M4.2 PR |
| `main` diverges | weekly merges of `main` into `nautilus-0.4` (README §6) |
