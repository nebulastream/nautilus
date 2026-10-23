# Plan: Low-Latency Compilation

> **Superseded** by the release plan in [nautilus-2.md](nautilus-2.md), which
> reorders this plan with the P0 measurements
> ([p0-latency-baseline.md](p0-latency-baseline.md)) and adds serialization,
> caching and the explicit control-flow API. Kept as the detailed record of
> the individual work items.

Status: proposal · Merges every point of
[low-latency-compilation.md](low-latency-compilation.md) (the backend and
pipeline survey) and [ir-representation-latency.md](ir-representation-latency.md)
(trace and IR representation, semantics, extensibility) into one sequenced
plan. The two design docs keep the rationale; this document decides
**what** is done, **when**, and **what it depends on**.

## 1. Goal and ground rules

**Goal.** Minimize the time from a function's first call to running native
code (`T_first = T_trace + T_trace→IR + T_irPasses + T_backend`), while
keeping peak code quality available through MLIR/LLVM.

The decisions so far, from the design iterations:

| Decision | Consequence for the plan |
|---|---|
| Implicit, replay-based control-flow tracing **stays** | Tags, snapshots and merge detection are kept and made cheap, not removed |
| Explicit control-flow API (S1) is a **follow-up** | Formats leave room for it; no work scheduled |
| **Multi-tier work is on hold** | Every change must cut single-backend latency; tier items are parked in §6 |
| Trace and IR become **flat** (16 B instructions, dense ids) | The central refactor; everything else plugs into it |
| IR **must be extensible** by plugins | The extension registry lands before the flat IR is consumed, so the formats are born extensible |

Ground rules for every milestone:

- **Mergeable on its own,** with all backends green, the execution tests
  green and the differential fuzzer clean.
- **Measured.** Each milestone names the M0 metric it must move. A change
  that does not move it is not merged.
- **Adapters, not big bangs.** New formats come with linear translations to
  and from the old ones, so consumers migrate one at a time.

## 2. Baseline numbers available today

From the tracing benchmark CI run on this PR (compile = tracing + IR +
backend, x86-64):

| Backend | Small functions | 100 chained `if`s |
|---|---|---|
| `bc` | ~12–17 µs | ~257 µs |
| `mlir` | ~3.3–5 ms | ~47 ms |
| `cpp` | ~22 ms | ~47 ms |

MLIR's floor of about 3 ms even for trivial functions points to fixed
setup cost (§4, P1-c). How the remaining time divides between tracing,
the passes and the backend is unknown: that is M0.

## 3. Workstreams

| Code | Workstream | Source |
|---|---|---|
| **MEAS** | Measurement corpus, per-stage counters, targets | latency §5, IR §5 |
| **TRACE** | Tracing hot path and replay cost | latency §4.3, IR §2.1, §12 |
| **REP** | Flat trace, flat IR, optimizing conversion, retiring `IRGraph` | IR §4, Part III |
| **EXT** | Extension registry, extension types, plugin rules and passes | IR Part IV |
| **BE** | Backends: single-pass native backend, ports, MLIR fixed costs, lazy compilation | latency §4.2, §4.4, §4.5; IR §15 |
| **SEM** | Semantic changes S3 (optimistic calls) and S4 (signless integers) | IR Part II |
| **CACHE** | Code cache keyed by IR hash; user cache key | latency §4.3.4 |

## 4. Phases and milestones

```
P0 Measure ──► P1 Quick wins on today's code ──► P2 Foundations ──► P3 Backends on flat IR
                                                     │                    │
                                                     └──► P4 Trace → flat IR directly ──► P5 Semantics,
                                                                                            extension types,
                                                                                            caching ──► P6 Retire old IR
```

### P0: Measure (MEAS)

| # | Work | Exit criterion |
|---|---|---|
| P0-a | **Latency corpus** in `test/benchmark`: functions of 1k, 10k and 100k operations (straight-line, branchy with many traced paths, loop-heavy, call-heavy, many call sites per helper), plus real NebulaStream queries from the existing NebulaStream CI job | Corpus runs in the benchmark workflow |
| P0-b | **Per-stage counters** in `CompilationStatistics`: ns/op for trace record, trace → IR, each pass and each backend's lowering; trace iterations and replayed ops per scope; heap allocations/op (counting `operator new` in the benchmark binary); peak trace and IR arena bytes | Reported per corpus entry and kept in the benchmark history with the existing 200% alert |
| P0-c | **Profile shares:** hash-map code (`std::_Hashtable`, `std::hash`), `TrieNode::append`, the tag walk, `MLIRContext`/registry setup | Report confirms or rejects every hypothesis (IR §3, latency §2) |

**Decision D1, after P0.** Reorder P1–P5 by measured share. The order
below assumes the hypotheses hold.

### P1: Quick wins on today's code (independent, parallelizable)

| # | Stream | Work | Exit criterion |
|---|---|---|---|
| P1-a | BE/REP | **Dense value tables.** `ValueTable` replaces `Frame<OperationIdentifier, V>` and the usage, cross-block and label maps in `bc`, `tbc`, `asmjit`, `cpp` and `mlir`. `IRVerifier` checks that ids are dense (IR §4.1–4.2) | No `unordered_map` on a per-operand backend path; lowering ns/op down |
| P1-b | TRACE | **Tracing hot path:** open-addressing `tagMap` in the trace arena; global op ids, so merge splits stop rewriting map entries; hashed trie fan-out above about 8 children; arena-backed block lists (IR §12) | Zero `malloc` per recorded op; all `tracing/` reference dumps unchanged |
| P1-c | BE | **MLIR fixed costs:** reuse `MLIRContext`, the dialect registry and the `TargetMachine` per thread; choose the O-level by function size (O2 below a threshold) (latency §4.4) | MLIR's floor for small functions clearly below today's ~3 ms; no execution regression on `ExecutionBenchmark` |
| P1-d | TRACE | **Replay statistics and guidance:** iterations and replayed ops per scope surfaced in the statistics; `nautilus-review` flags scopes where replay dominates and suggests `region()` (latency §4.3.1) | Visible in the statistics report |

These are already settled and need no work, only verification in P0:

- **Follow mode is cheap already.** It advances a cursor without taking a
  snapshot (`TraceContext::follow`), so latency §4.3.2 holds apart from
  incremental static-value hashing, which P0 decides.
- **Exploration stops at fully explored tags** (`shouldTerminate`). Whether
  depth-first path order shortens replay (latency §4.3.3) is decided by P0.

### P2: Foundations (REP, EXT)

| # | Work | Exit criterion |
|---|---|---|
| P2-a | **`FlatFunction` format** (IR §13): 16 B `Inst` with a 16-bit opcode and an 8-bit `TypeId` (IR §18.1); `extra`, `ConstPool`, `BlockInfo` (layout order, loop depth, loop end), `useCount`, `forward`; verifier and printer | Format, verifier and printer merged with unit tests |
| P2-b | **Bridge `IRGraph` → `FlatFunction`**, run after today's pass pipeline: a linear translation | Every execution test's IR round-trips; this unblocks P3 without waiting for P4 |
| P2-c | **`ExtensionRegistry`** (IR §18): op and type descriptors, traits, effects, fold, simplify and verify hooks, a mandatory fallback, per-backend lowering tables; frozen and lock-free after the first compile. It absorbs `IntrinsicRegistry`, and in-tree intrinsics re-register as extension ops with native fallbacks | All plugin tests green with no behavior change; no mutex on the lookup path |
| P2-d | **`FlatTrace`** (IR §12) behind `TraceModule`: 16 B records, a snapshot side array, constant and call side tables, `traceExtOp`; accessor layer for `CopyAttribution`, region handling and dumps; adapter to today's conversion | Trace bytes/op about 5x lower; `tracing/` dumps regenerated once and reviewed |

P2-a, P2-c and P2-d can proceed in parallel. P2-b needs P2-a.

### P3: Backends on the flat IR (BE)

| # | Work | Exit criterion |
|---|---|---|
| P3-a | **Port `bc` and `tbc`** to read `FlatFunction` and the extension tables | Execution tests and fuzzer green; lowering ns/op ≤ P1-a |
| P3-b | **Single-pass native backend prototype on x86-64** (latency §4.2): linear analysis from `BlockInfo`, greedy local register allocation with value parts, spilling at merges, block arguments as parallel moves, fusion within a block, prologue patching. In-house on `asmjit::Assembler`, with optional encoders from `tools/stencils` (no TPDE backend) | Passes the execution tests; backend time ≥ 10x faster than MLIR at O0; code within 1.2x of today's `asmjit` and ≥ 3x faster than `bc` on the `ExecutionBenchmark` loop kernels |
| P3-c | **Decision D2 (resolved: in-house; no TPDE backend).** Then AArch64, exception regions and unwind info, extension-op lowering through the narrow plugin API (IR §18.5) | Feature parity with today's `asmjit` on both architectures |
| P3-d | **Decision D3: the default backend.** Make the single-pass backend the default where it is available, with `tbc` (plus copy-and-patch) the portable default elsewhere. Today's `asmjit` stays only for features the new backend lacks (latency §7) | Default switched; documentation updated |
| P3-e | **MLIR from `FlatFunction`** (`cf`/`arith` from `BlockInfo` and terminators, extension types mapped) | LLVM IR tests updated; execution unchanged |

### P4: Trace directly to flat IR (REP)

| # | Work | Exit criterion |
|---|---|---|
| P4-a | **Optimizing conversion** `FlatTrace` → `FlatFunction` in one RPO pass (IR §13): Braun SSA, live-block tracking with constant-branch folding, fold and simplify on emission (core and extension rules), block-local CSE keyed on the effect chain, use counts, straight-line threading | Fuzzer clean; op counts ≤ today's after the full pipeline |
| P4-b | **The fixed-point pipeline becomes optional.** With the conversion doing the cleanup it leaves the critical path (this replaces latency §4.1's per-tier IR level for the single-backend case); LICM and strength reduction remain as optional copy passes | `T_irPasses` ≈ 0 on default compiles |
| P4-c | **Dominator-scoped CSE,** a widening of P4-a's block-local scope, behind dedicated fuzzing | Fuzzer clean on merge-heavy tests |
| P4-d | **Plugin passes** at the named points (`afterConversion`, `beforeLowering`), each marked required or optional (IR §18.6) | Only required passes run on default compiles |

### P5: Semantics, extension types, caching, lazy compilation

| # | Stream | Work | Exit criterion |
|---|---|---|---|
| P5-a | EXT | **Extension types:** `simd.vec<T,N>` and `builtin.i128` with a stack-slot fallback; the SIMD ring buffer is removed (IR §18.2, §19) | SIMD tests pass on every backend without the 32-value lifetime rule |
| P5-b | SEM | **S3, optimistic calls:** `invoke` is `noUnwind` by default with opt-in throwing; declared effects; inference passes leave the critical path; debug-mode checking of the declarations. **Decision D4:** the annotation policy for NebulaStream runtime functions | Exception regions only where declared; NebulaStream green |
| P5-c | SEM | **S4, signless integers:** values carry widths only, signedness moves onto operations, integer semantics are defined | Opcode tables over 4 widths; casts that do nothing removed |
| P5-d | CACHE | **Code cache keyed by IR hash**, using extension *names* rather than ids; optional user cache key that skips tracing too (latency §4.3.4) | Hit rate and saved time reported on the NebulaStream corpus |
| P5-e | BE | **Lazy per-function compilation:** compile the entry function first; internal call sites go through patchable stubs (latency §4.5) | Backend time proportional to the functions actually called |
| P5-f | BE | **Parallel function compilation** within a module (ORC concurrent compilation, MLIR multithreading) (latency §4.4) | Wall time down on multi-function modules |

### P6: Retire the old IR (REP)

| # | Work | Exit criterion |
|---|---|---|
| P6-a | Delete `IRGraph`, `ir::Operation`, `FunctionRewriter`, the pointer-based passes, the P2-b bridge and the trace adapter | No users left; build and test time recorded |
| P6-b | Update `docs/tracing.md`, `docs/graphs.md`, the plugin documentation and the `test/data` dumps | Documentation matches the code |

## 5. Dependencies and critical path

```
P0 ─► P1-a, P1-b, P1-c, P1-d                 (all independent)
P0 ─► P2-a ─► P2-b ─► P3-a, P3-b ─► P3-c ─► P3-d
          └─────────────────────► P3-e
      P2-c ─────────────────────► P3-c (extension lowering), P4-a (rules), P5-a
      P2-d ─► P4-a ─► P4-b ─► P4-c, P4-d
                 └─► P5-b, P5-c, P5-d
P3-d + P4-b ─► P5-e, P5-f ─► P6
```

- **Critical path:** P0 → P2-a → P2-b → P3-b → P3-c → P3-d. The latency win
  for users arrives when the single-pass backend becomes the default.
- **Second chain:** P2-d → P4-a → P4-b, which removes the pass pipeline and
  the double materialization.
- **Why the P2-b bridge exists:** backend work does not have to wait for the
  new trace. Both chains run in parallel and meet at P6.

## 6. Parked and deferred items

| Item | Source | Status | Revisit when |
|---|---|---|---|
| Per-tier IR levels; passes on the promotion thread | latency §4.1 | Parked (tiering on hold); for one backend, P4-b supersedes it | Tiering resumes |
| Promotion thread pool, delayed promotion, admission control | latency §4.4, §4.7 | Parked | Tiering resumes |
| Tier lattice (interpreter / `tbc` / native / `asmjit` / MLIR) | latency §4.6 | Parked; P3-d decides the single default | Tiering resumes |
| Mid tier: LLVM O1 passes + TPDE-LLVM codegen | latency §4.4 | Rejected (no TPDE dependency) | — |
| S1: explicit control-flow API (`if_`, `while_`, `for_`) | IR §8 S1 | Deferred follow-up; the flat trace already fits it | After P4 |
| Lazier copies (copies are about half of all traced ops) | IR §12 | Needs its own investigation (copy sites take part in merge detection) | After P1-b, with P0 data |
| Intrusive use lists in `Operation` | IR §4.4 (b) | Rejected (breaks the 32 B layout; moot after P6) | — |
| Recording IR directly while tracing | IR §4.6 | Rejected while implicit control flow stays (the CFG is final only after tracing) | With S1 |

## 7. Consolidated risks

| Risk | Mitigation | Owner milestone |
|---|---|---|
| Hypotheses wrong; effort goes to cold paths | P0 first; D1 reorders the plan | P0 |
| Dense-id invariant drift | Verifier check; ids minted only through the builder or rewriter | P1-a, P2-a |
| 16 B instruction budget too tight for plugins (SIMD masks, GPU, `i128`) | `extra` and `ConstPool` for payloads; review each plugin's ops against the format in P2-c | P2-a, P2-c |
| Scoped CSE miscompiles | Block-local scope first; dominator scope behind fuzzing; CSE off in debug builds | P4-a, P4-c |
| Folded-away predecessors break Braun sealing | Merge-heavy tests (regions, nested loops, `break`) in the fuzzer corpus | P4-a |
| Extension fallbacks disagree with handlers | Fuzzer runs each extension with its handler enabled and disabled | P2-c, P5-a |
| Dishonest traits or effect declarations | Debug mode that ignores traits; checks for `noUnwind` and effects | P2-c, P5-b |
| Single-pass code larger (TPDE: +43–49% vs LLVM -O0) | Measure I-cache impact on NebulaStream; MLIR remains available per module | P3-b |
| Exceptions and unwind in the new backend | Parity requirement in P3-c before D3 | P3-c |
| Plugin-facing lowering API freezes allocator internals | Keep it narrow: operands, results, scratch registers, assembler | P3-c |
| Ids not stable across processes | Names in dumps, hashes and cache keys | P2-c, P5-d |
| Reference-dump churn hides regressions | Regenerate once per format change, in isolated commits, with a reviewed diff | P2-d, P4-a, P6-b |

## 8. Decision points

| # | When | Question | Input |
|---|---|---|---|
| D1 | After P0 | Order of P1–P5 | Per-stage shares from P0 |
| D2 | Resolved | In-house single-pass backend; no TPDE backend or adapter | See [nautilus-2.md §4.6.1](nautilus-2.md) |
| D3 | After P3-c | Default backend | P3-b targets met on both architectures |
| D4 | P5-b | Annotation policy for runtime calls (opt-in throwing, declared effects) | NebulaStream call-site survey |
