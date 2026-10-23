# Nautilus 0.4: Design

Status: proposal · Release: Nautilus 0.4 (current: 0.3) · Branch: `nautilus-0.4`

This is the main design document for Nautilus 0.4. Every change described
here ships together in the 0.4 release. Each component has its own
document, listed in §4; this document holds the goals, the evidence, the
architecture, the decisions, compatibility, the release plan and the risks.

**How to read this plan:**

- Start with §1–§3 for goals and architecture, and §6 for the release plan.
- Then read the component documents in order. Each one has the same
  structure: purpose, today and what it costs, design, migration from the
  open PRs, milestone exit criteria, risks.
- [p0-baseline.md](p0-baseline.md) has the measurements every claim about
  cost refers to.

**Terms:**

| Term | Meaning |
|---|---|
| NIR | The flat intermediate representation of 0.4 ([02](02-nir.md)) |
| Trace level / SSA level | The two levels of NIR: what tracing records, and what everything after the conversion uses |
| Canonical / derived data | NIR sections that are serialized and hashed, and sections that are rebuilt after load |
| Conversion | The single optimizing pass from trace level to SSA level ([03](03-conversion-and-passes.md)) |
| Legalization | The NIR→NIR rewrite that prepares a module for one backend |
| Extension | A plugin-defined operation, type, rule or pass ([04](04-extensions.md)) |
| Artifact | A serialized module: NIR image, optional native payloads, compatibility descriptor ([07](07-serialization.md)) |
| L0 / L1 / L2 | In-process, semantic-key and structural caches ([08](08-caching.md)) |
| Re-run | A repeated execution of a traced body to explore another branch direction ([01](01-tracing-frontend.md)) |
| M1–M7, D1–D7 | Milestones (§6) and decisions (§8) |

## 1. Summary

Nautilus 0.4 is built around **one flat, extensible, serializable
intermediate representation, NIR.** Tracing, optimization, every backend,
serialization, caching and the C API all read and write it. The
user-facing C++ API (`val<T>`, `static_val`, the engine, modules,
`region()`, `nautilus::function`, `invoke`) stays source-compatible.
Internals, the plugin API and the on-disk formats change.

Goals, in priority order:

1. **Latency.** First call to native code in at most 0.5 µs per IR
   operation end to end on large functions, against 1.3–2.5 µs today. Tracing
   becomes linear in program size for code that uses explicit control flow.
2. **Reuse.** Compiled modules can be serialized, shipped and cached. A warm
   start skips tracing and code generation (the cache stack in #529 measured
   −67% on the NebulaStream suite).
3. **Extensibility.** Plugins add operations, types, rules and passes
   without changes to the core.
4. **One source of truth.** A single IR format for in-memory compilation,
   the wire format, the cache and the C API.

**Scope:**

- **Platforms:** Linux x86-64 and AArch64, macOS AArch64. Persisted native
  payloads target Linux ELF (x86-64 first; AArch64 if the loader work
  allows). Every other platform reuses artifacts through the portable NIR
  image.
- **Concurrency:** concurrent compiles stay safe, as in 0.3. Each compile
  owns its trace and NIR arenas, and a finished NIR module is immutable.
  The extension registry is frozen after the first compile, and caches
  lock per key ([02 §3.4](02-nir.md), [08](08-caching.md)).
- **Not in 0.4:**
  - changes to multi-tier promotion;
  - lazy per-function compilation;
  - a new optimizing tier;
  - a new native backend (the `asmjit` backend is improved instead);
  - any TPDE backend or adapter.

  TPDE, Flying Start, MIR, copy-and-patch and the Wasm baseline compilers
  are related work only.

## 2. Evidence: what P0 measured

From [p0-baseline.md](p0-baseline.md) (`main` and #561, six scalable corpus
functions, three backends, callgrind profiles), plus #561's TPC-H analysis
from nautilus-duck.

| Finding | Consequence |
|---|---|
| **IR passes are the largest stage on linear code:** 62–68% of the compile on `main`, still 44–53% with #561. Building `FunctionRewriter` tables is 44% of all instructions | Optimization moves into the conversion, and the fixed-point pipeline leaves the critical path ([03](03-conversion-and-passes.md)) |
| **Replay dominates branchy code:** 69–78% of the compile; 270,300 replayed ops for 300 `if`s, 900 with `region()` | Explicit control flow that can be mixed with implicit control flow; a cheaper replay cursor ([01](01-tracing-frontend.md)) |
| **On TPC-H (#561), 61% of tracing is re-runs** (4,687 re-runs replaying 631,000 recorded ops), and **39% of recorded ops are `val` copies** | Cheaper follow and paused paths, early end of paused passes, fewer copies ([01 §4](01-tracing-frontend.md)) |
| **Tag-trie fan-out costs about 3x tracing per op** with many distinct call sites | Hashed trie children ([01](01-tracing-frontend.md)) |
| **3–5 heap allocations per IR op on linear code, 18–25 on branchy code;** every op materialized twice | Flat trace and NIR in recycled arrays ([02](02-nir.md)) |
| **AsmJit is superlinear on call-heavy code** (~4 µs per call at 5000 calls) | The `asmjit` backend gets a single-pass `fast` mode with linear call handling ([05](05-backends.md)) |
| **MLIR has a fixed cost of 3–10 ms per compile** | #561's per-thread state reuse becomes part of the MLIR backend ([05](05-backends.md)) |

## 3. Architecture

```
            ┌──────────── C++ frontend ────────────┐      ┌── other frontends ───┐
 user code ─┤ implicit tracing (replay, tags)      │      │ C API builder        │
            │ explicit If/While/For (region-based) │      │ deserializer         │
            └──────────────┬───────────────────────┘      └──────────┬───────────┘
                           ▼                                         │
                 NIR, trace level (flat trace)                       │
                           ▼                                         │
      optimizing conversion: SSA + fold + CSE + use counts           │
                           ▼                                         ▼
     ┌──────────────────────── NIR, SSA level ────────────────────────────┐
     │ insts · extra · ConstPool · pointers · blocks · functions · names   │
     └───┬───────────────┬──────────────────┬────────────────┬────────────┘
         │               │                  │                │
   legalization,     backends           serializer        IR-hash cache key
   optional passes   asmjit (fast) ·     artifacts:            │
                     tbc · bc · MLIR ·   NIR image +           │
                     cpp · GPU           native payloads       │
                         │                  │                  │
                         ▼                  ▼                  ▼
                 executable module ◄── loader ◄── caches (L0 · L1 · L2)
                 + runtime bindings
```

The extension registry sits beside all stages: every stage looks up
operations and types through it.

### 3.1 Why one IR, and where unification stops

| Option | Meaning | Decision |
|---|---|---|
| One encoding and infrastructure | trace and IR share the instruction format, ids, side tables, extension registry, verifier, printer and serializer | **Yes** |
| One level | tracing writes SSA directly | **No.** Two levels of one format, trace and SSA. Implicit tracing needs variables, recording order and stable ids during block splits, which SSA must not have. Direct SSA emission for fully explicit functions is a later option |
| In-memory layout = wire layout | zero-copy loads; the cache key hashes bytes; the C API reads records | **Yes, for canonical sections only.** Derived data (use counts, loop info, snapshots, CSE tables) is rebuilt after load |
| One IR for every backend | no backend-specific IR in core | **Yes, up to legalization**, a NIR→NIR rewrite. Bytecode, MLIR and machine code stay internal to each backend |
| MLIR as the core IR | Nautilus IR as an MLIR dialect | **No.** Its fixed and per-op costs miss the latency budget by an order of magnitude; MLIR stays a backend |
| Host and GPU | kernels are NIR with extension ops | **Yes** |

Details and rationale: [02-nir.md](02-nir.md).

## 4. Components

| # | Document | Content | Built on |
|---|---|---|---|
| 01 | [Tracing frontend](01-tracing-frontend.md) | Cheaper implicit tracing; fewer and cheaper re-runs and copies; explicit `If`/`While`/`For` on region scopes, mixable with implicit control flow; the flat trace | #300, #561 (statistics) |
| 02 | [NIR](02-nir.md) | Instruction format, sections, levels, canonical vs. derived data, mutation model, unified-IR rationale | #532 (format rules) |
| 03 | [Conversion and passes](03-conversion-and-passes.md) | Optimizing trace→SSA conversion, legalization, optional and plugin passes | — |
| 04 | [Extensions](04-extensions.md) | Extension operations, types, rules and passes; registry lifecycle; migration of the in-tree plugins | — |
| 05 | [Backends](05-backends.md) | Backend roles; the improved `asmjit` backend with a single-pass mode; MLIR; bytecode; C++; GPU | #561 (MLIR state reuse) |
| 06 | [Semantics](06-semantics.md) | Declared call effects and cheaper exception handling (exception default unchanged); signless integers | — |
| 07 | [Serialization](07-serialization.md) | Artifacts, pointers, runtime bindings, provenance | #527, #529, #532 |
| 08 | [Caching](08-caching.md) | L0 in-process, L1 semantic, L2 structural caches | #528 |
| 09 | [C API](09-c-api.md) | The C interface to NIR and the engine | #531 |
| 10 | [Observability](10-observability.md) | Statistics, the latency corpus, dumps, tooling | #561, #524 |
| — | [P0 baseline](p0-baseline.md) | Measurements behind this design | design study (closed #562) |

## 5. Compatibility and versioning

| Surface | 0.4 |
|---|---|
| `val<T>`, `static_val`, `region()`, `nautilus::function`, `invoke`, engine, module, options | **Source-compatible.** Renamed options keep deprecated aliases through 0.4.x |
| Explicit control flow | **New**, opt-in, mixable with implicit control flow |
| Exception default of `invoke` | **Unchanged** (D4 = b). New, optional: `invoke<no_throw>`/`nothrow` and effect declarations ([06](06-semantics.md)) |
| Plugin intrinsic API (`IntrinsicRegistry`, `MLIRIntrinsicPlugin`, `AsmJitBackendIntrinsic`) | **Replaced** by the extension API. A deprecated shim registers old intrinsics as extension ops through 0.4.x |
| `IRGraph`, `ir::Operation`, IR passes | **Removed** (internal) |
| IR text dumps, `test/data` | **New format** |
| Serialization, artifacts, cache entries | **First persisted formats.** The PRs are ported onto NIR, not released on the 0.3 IR |
| C API | **New**, on NIR; stable from 0.4 on |

Version identities recorded in every artifact: the core version (semver),
the NIR format (major.minor + feature bits), the artifact format, and each
extension's `name@version`.

## 6. Release plan: everything in 0.4

All changes ship together as Nautilus 0.4. There are no interim feature
releases and no temporary formats.

- **Development happens on `nautilus-0.4`.** `main` takes only fixes;
  merge `main` into `nautilus-0.4` weekly.
- **The open PRs are ported onto NIR:**

  | PR | Lands as | Milestone |
  |---|---|---|
  | #300 explicit control flow | region-based constructs | M2 |
  | #527 / #528 / #529 | artifacts, L1 cache, runtime bindings | M5 |
  | #532 IR serialization | the canonical-section serializer | M1 |
  | #531 C API | the C API on NIR | M5 |
  | #561 | MLIR state reuse (M4); tracing statistics (M2) | M2, M4 |

- **The milestones are internal checkpoints.** Each is merged into
  `nautilus-0.4` when green, with the full CI matrix and the latency corpus
  running on the branch.

| Milestone | Content | Exit criterion |
|---|---|---|
| **M1 Foundations** | NIR format with levels, the core opcode set, exceptions and the canonical/derived split; verifier, printer and serializer; `ExtensionRegistry` absorbing intrinsics; bridge `IRGraph` → NIR (internal, removed before release); latency corpus driver added and running in CI | Every test's IR round-trips through the bridge, printer and serializer; plugin tests green on the registry |
| **M2 Frontend** | Flat trace with the tag-trie, tag-map, replay-cursor and value-ref fixes; re-run and copy reductions (R1–R5, C1–C3); explicit control flow (port of #300); tracing statistics; replay diagnostics | Tracing ≥ 30% cheaper on `distinctSites`; TPC-H re-run share ≤ 35% of tracing; explicit `chainedIf` traced in one pass with zero replayed ops; mixed functions correct under the fuzzer |
| **M3 Conversion** | Optimizing conversion; legalization; optional copy passes; plugin pass hooks | IR passes off the critical path; op counts ≤ today's after the full pipeline; fuzzer clean |
| **M4 Backends** | `bc`/`tbc`, `cpp`, MLIR and GPU on NIR; `asmjit` on NIR (`optimized` mode), then its single-pass `fast` mode on x86-64 and AArch64; decision D3 | End to end ≤ 0.5 µs/op on linear code; execution benchmarks no worse than 0.3 per backend |
| **M5 Reuse** | Artifacts and loader; provenance classes; runtime bindings; L0/L1/L2 caches; C API | Warm NebulaStream suite ≥ 60% faster than cold; every #527/#528 negative test ported and failing closed |
| **M6 Semantics and extensions** | Declared call effects and cheaper exception handling; signless integers; extension types (`simd.vec`, `builtin.i128`); in-tree plugins migrated; SIMD ring buffer removed | Plugin suites green; SIMD tests pass on every backend without the 32-value lifetime rule |
| **M7 Release** | Remove `IRGraph`, the old passes and the bridge; migration guide; user documentation (§6.2); NebulaStream integration; x86-64 and AArch64; sanitizers | Release criteria below |

**Order and parallelism:**

- M1 comes first: everything else writes or reads NIR.
- M2 and M3 run in parallel after M1; M3 starts on the bridge's output.
- M4 starts on the bridge's SSA output.
- M5's portable path (NIR images, L0, L2) can start after M1; its native
  payloads need M4.
- M6 needs M3 and M4.
- M7 closes.

**Release criteria:**

1. All core, plugin and NebulaStream suites green on x86-64 and AArch64,
   with ASan/UBSan and TSan.
2. Latency corpus:
   - end to end ≤ 0.5 µs/op on linear code;
   - tracing linear in the number of `if`s when written explicitly;
   - no case slower than 0.3.
3. No execution-time regression per backend on `ExecutionBenchmark` and on
   TPC-H (nautilus-duck).
4. Warm NebulaStream suite ≥ 60% faster than cold.
5. Migration guide: plugin intrinsics → extensions, signless integers in
   plugins, renamed options.

### 6.1 Testing and validation

These rules apply to every milestone:

- **The differential fuzzer** (production-ready since #544) compares
  backends and the interpreter on every change. It is extended with:
  - mixed implicit and explicit control flow;
  - extension ops with handlers enabled and disabled;
  - serialization round trips and cache hits;
  - signless-integer edge cases.
- **The bridge** (M1) lets each new component be compared against 0.3's
  pipeline on the same input until the bridge is removed in M7.
- **Reference dumps** (`test/data`) are regenerated once per format change,
  in isolated and reviewed commits, never together with behavior changes.
- **The latency corpus and benchmarks** run on `nautilus-0.4` with the 200%
  alert. Each milestone's exit criterion is checked on them.
- **The CI matrix of `main`** runs on `nautilus-0.4` (GCC 14, Clang 19/21,
  macOS, ARM, sanitizers), plus NebulaStream and nautilus-duck (TPC-H).

### 6.2 User documentation

M7 updates the user-facing documents in `docs/` to 0.4:

| Document | Update |
|---|---|
| `tracing.md`, `graphs.md` | the new pipeline and the NIR dumps |
| `control-flow.md`, `loops.md`, `region.md` | explicit control flow |
| `functions.md` | the `invoke` exception default |
| `options.md`, `engine.md` | options, caches, artifacts |
| `simd.md`, `builtin.md`, `std-plugin.md` | extension types; the ring-buffer lifetime rule disappears |
| `exception_handling.md` | landing pads in NIR |
| `getting-started.md` | the default backend |

New documents: the extension API, serialization and caching, the C API, and
the migration guide from 0.3.

## 7. Risks

| Risk | Mitigation |
|---|---|
| One release concentrates risk | Internal milestones with exit criteria; release criteria gate 0.4; the bridge lets each milestone be checked against 0.3 behavior |
| `nautilus-0.4` drifts from `main` | `main` takes fixes only; weekly merges; full CI and corpus on the branch |
| Region-based explicit control flow has corner cases | `region()`'s rules and diagnostics; fuzzer with mixed functions; #300's tests as the base |
| In-memory = wire layout constrains evolution | Per-section strides and feature bits; derived data never serialized |
| Provenance classes miss a raw-address path | Fail closed by default; #527's negative tests |
| `asmjit`'s `fast` mode reaches parity late | `optimized` mode stays the default until it does; `tbc` and MLIR remain available |
| Wrong `nothrow`/`pure` declarations | Debug-mode checks (terminate handler, shadow effect checks) ([06 §3.3](06-semantics.md)) |
| L2 hash collisions | 128-bit hash plus a full image comparison on hit |

## 8. Decisions

| # | Question | Decision or proposal |
|---|---|---|
| D1 | Order of the work | As in §6: foundations first, then frontend and conversion (the measured hot spots) |
| D2 | Native backend | **Decided:** improve the existing `asmjit` backend with a single-pass `fast` mode; no new backend, no TPDE |
| D3 | Default backend | Proposed: `asmjit` in `fast` mode on x86-64 and AArch64, `tbc` elsewhere |
| D4 | Exception default of `invoke` | **Decided: (b)** keep 0.3's default; add optional `nothrow` and effect declarations; cheaper exception handling ([06 §3](06-semantics.md)) |
| D5 | Cache and serialization PRs | **Decided:** ported onto NIR; no separate 0.3 formats |
| D6 | Keep `bc` beside `tbc` | Proposed: keep in 0.4; revisit later |
| D7 | Scope of the unified IR | Proposed: as in §3.1 |

## References

- T. Schwarz, T. Kamm, A. Engelke. *TPDE: A Fast Adaptable Compiler Back-End
  Framework.* CGO 2026. [arXiv:2505.22610](https://arxiv.org/abs/2505.22610)
- A. Engelke, T. Schwarz. *Compile-Time Analysis of Compiler Frameworks for
  Query Compilation.* CGO 2024. [PDF](https://home.cit.tum.de/~engelke/pubs/2403-cgo.pdf)
- T. Kersten, V. Leis, T. Neumann. *Tidy Tuples and Flying Start.* VLDB
  Journal 2021. [DOI](https://link.springer.com/doi/10.1007/s00778-020-00643-4)
- A. Kohn, V. Leis, T. Neumann. *Adaptive Execution of Compiled Queries.*
  ICDE 2018. [PDF](https://db.in.tum.de/~leis/papers/adaptiveexecution.pdf)
- V. Makarov. *MIR.* [Repository](https://github.com/vnmakarov/mir)
- H. Xu, F. Kjolstad. *Copy-and-Patch Compilation.* OOPSLA 2021.
  [arXiv:2011.13127](https://arxiv.org/abs/2011.13127)
- M. Braun et al. *Simple and Efficient Construction of Static Single
  Assignment Form.* CC 2013.
- LuaJIT IR, Cranelift, Zig AIR: flat, index-based IR designs.
