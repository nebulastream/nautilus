# Nautilus 2: Design

Status: proposal · Supersedes the plan in [low-latency-plan.md](low-latency-plan.md)
· Rationale and details:
[low-latency-compilation.md](low-latency-compilation.md) (backends and
pipeline), [ir-representation-latency.md](ir-representation-latency.md)
(trace/IR representation, semantics, extensibility) · Measurements:
[p0-latency-baseline.md](p0-latency-baseline.md) · Builds on the open PRs
#300 (explicit control flow), #521/#527/#528/#529 (serialization, cache,
runtime bindings), #531 (C API), #532 (IR serialization) and #561
(compile-time cuts).

## 1. Summary

Nautilus 2 is a new major version built around **one flat, extensible,
serializable IR** ("NIR"). Every stage reads and writes it: tracing,
optimization, every backend, serialization, caching and the C API. The
user-facing C++ API (`val<T>`, the engine, modules, `region()`,
`nautilus::function`, `invoke`) stays source-compatible. Internals,
the plugin API and on-disk formats change.

Goals, in priority order:

1. **Latency.** First call to native code at about 0.3–0.4 µs per IR
   operation end to end on large functions, against 1.3–2.5 µs today (P0);
   tracing linear in program size for code using explicit control flow.
2. **Reuse.** Compiled modules can be serialized, shipped and cached; a warm
   start skips tracing and code generation entirely (the #529 stack measured
   −67% on the NebulaStream suite).
3. **Extensibility.** Plugins add operations, types, rules and passes
   without forking the core.
4. **One source of truth.** A single IR format for in-memory compilation,
   the wire format, the cache and the C API.

Non-goals for 2.0: multi-tier promotion changes (parked), a new
optimizing tier, or changes to the generated code quality of MLIR/LLVM.

## 2. What P0 established

From [p0-latency-baseline.md](p0-latency-baseline.md) (`main` and #561, 6
corpus functions, 3 backends):

| Finding | Consequence for 2.0 |
|---|---|
| **IR passes are the largest stage on linear code** (62–68% on `main`, still 44–53% with #561); `FunctionRewriter` table construction is 44% of all instructions | Optimization moves into the conversion; the fixed-point pipeline leaves the critical path (§4.4) |
| **Replay dominates branchy code** (69–78%): 270k replayed ops for 300 `if`s; `region()` cuts it to 900 | Explicit control flow becomes a first-class API that can be mixed with implicit control flow (§4.1.2); replay cursor fix |
| **Tag trie fan-out costs about 3x tracing** with many distinct call sites | Hashed trie children (§4.1.1) |
| 3–5 heap allocations per IR op on linear code, 18–25 on branchy code; two materializations | Flat trace and NIR with recycled arrays (§4.2, §4.3) |
| AsmJit is superlinear on call-heavy code (~4 µs per call at 5000 calls) | Replaced by the single-pass backend (§4.6) |
| MLIR is dominated by fixed setup (3–5 ms minimum per compile) | #561's per-thread MLIR/LLVM state reuse is part of the MLIR backend on NIR (§4.6) |

## 3. Architecture

```
            ┌──────────── C++ frontend ────────────┐      ┌── other frontends ──┐
 user code ─┤ implicit tracing (replay, tags)      │      │ C API builder (#531) │
            │ explicit If/While/For (region-based) │      │ deserializer (§4.8)  │
            └──────────────┬───────────────────────┘      └──────────┬──────────┘
                           ▼                                         │
                    Flat trace (§4.2)                                │
                           ▼                                         │
         Optimizing conversion: SSA + fold + CSE + use counts (§4.4) │
                           ▼                                         ▼
     ┌──────────────── NIR: flat, dense, extensible (§4.3) ─────────────────┐
     │  instructions · extra · ConstPool · pointer table · BlockInfo · names │
     └───┬───────────────┬──────────────────┬────────────────┬─────────────┘
         │               │                  │                │
   optional passes   backends (§4.6)    serializer (§4.8)  IR-hash cache key (§4.9)
                     bc/tbc · native      artifacts:          │
                     single-pass · MLIR   NIR + native objects│
                     · cpp · GPU          + descriptor        │
                         │                   │                │
                         ▼                   ▼                ▼
                 executable module ◄── loader ◄── cache (pre-trace key / IR hash)
                 + runtime bindings (§4.8.3)
```

The extension registry (§4.5) sits beside all of it: every stage looks up
ops and types through it.

## 3a. Does a unified IR make sense?

"Unified" can mean several things. Each has a different answer.

| Option | What it means | Verdict |
|---|---|---|
| **A. One encoding and infrastructure** | Trace and IR share the instruction format, id scheme, `extra`, `ConstPool`, pointer table, extension registry, verifier framework, printer and serializer | **Yes**: the core of Nautilus 2 |
| **B. One level** | The trace *is* the SSA IR: tracing writes SSA directly | **No**, while implicit control flow exists (see below) |
| **C. In-memory = wire format** | Images load in place; the cache key hashes the image; the C API reads records | **Yes, for canonical sections only.** Derived data stays out of the wire format |
| **D. One IR for every backend** | No backend-specific IR in core | **Yes up to legalization.** Backends keep their own internal forms (bytecode, MLIR, machine code) |
| **E. MLIR as the IR** | Nautilus IR becomes an MLIR dialect | **No**: incompatible with the latency goal |
| **F. One IR for host and GPU** | Kernels are NIR with extension ops and address-space types | **Yes** |

### Why A: one infrastructure

Today the trace and the IR have separate data structures, printers,
verifiers and (in #532) a third, serialized representation. #531 adds a
fourth view for the C API. Every new feature touches all of them: regions
needed trace fields, IR fields, dump support and conversion; #527 and #529
had to carry provenance and binding evidence through each layer.

With one encoding:

- **A feature is defined once** (op, type, payload, extension), and
  tracing, conversion, serialization, the C API and the printer pick it up.
- **Plugins register once.** An extension op recorded by `traceExtOp` is the
  same record the conversion folds, the serializer writes and a backend
  lowers.
- **Tooling works at both levels.** A serialized *trace* (not only the IR)
  becomes a reproducer for tracing and conversion bugs, which helps the
  fuzzer, the playground and bug reports.
- **The P0 costs shrink:** no second materialization of differently shaped
  objects, and the conversion becomes array-to-array.

### Why not B: two levels, one format

Implicit control-flow tracing needs things SSA must not have:

| Trace level | SSA level |
|---|---|
| **Variables** (`ASSIGN` redefines a value ref); SSA does not exist yet | Every value defined once; block arguments at merges |
| **Recording order;** blocks are split at merge points while tracing runs | **Layout order** (RPO, loops contiguous) with `BlockInfo` |
| Ops must keep stable ids during splits (the tag map points at them) | Ids are dense and may be renumbered by copy passes |
| Snapshot side array, tag map, replay cursor | None of these |
| Append-only, discarded after conversion | Build once, rewrite by copying, persisted |

Forcing one level would either put variables and recording-order artifacts
into the persisted IR, where every consumer must handle them, or force
tracing to build SSA before the CFG is final. The second is exactly what
Braun-style construction on a complete CFG avoids.

The design therefore has **two levels of one format**. Every function
carries `level ∈ {Trace, SSA}`:

- the verifier enforces each level's invariants;
- the conversion is the only path from `Trace` to `SSA`;
- everything downstream (passes, backends, serializer images, caches, the
  C API) accepts only `SSA`;
- trace-level images exist for debugging and reproduction, never for
  execution.

**When explicit control flow is used throughout** (§4.1.2), the CFG is
known while tracing. Such functions could later be emitted directly at the
SSA level, with no conversion pass: option B for the explicit subset. That
is a 2.x optimization. The two-level split keeps the door open without
paying for it now.

### Why C, but only for canonical sections

Making the in-memory layout the wire layout gives zero-copy loading, a
cache key that is a hash over contiguous bytes, and a C API that reads
records directly. The cost is that the in-memory format can no longer
change freely. The split that bounds this cost:

- **Canonical sections** are serialized, hashed and versioned: `insts`,
  `extra`, `ConstPool`, `pointers`, `functions`, the canonical part of
  `blocks`, `names` and the extension table.
- **Derived sections** are recomputed or rebuilt after load and are never
  serialized: `useCount`, `forward`, loop depth and loop end, CSE tables,
  snapshot arrays, debug tags. They may change layout at any time.
- **Canonicalization before hashing and writing:** copy-compact the
  `forward` replacements, renumber densely, and intern names in
  first-use order. Two structurally equal modules then produce equal bytes
  (L2 cache, deterministic output as in #532).

### Why D, with a legalization step

There is no separate core "LIR": every backend reads SSA-level NIR. What
differs per target is **legalization**, a NIR→NIR rewrite in the same
format:

- extension ops without a handler on the target expand to their fallback;
- extension types without support become their memory fallback;
- calls are lowered to the target ABI where a backend asks for it.

Below that, backends keep their own forms (bytecode, MLIR, machine code).
Those are implementation details, not a second shared IR.

### Why not E: MLIR as the core IR

MLIR would provide dialects, passes and bytecode serialization for free.
But P0 and #561 measure its fixed cost at 3–10 ms per compile, plus
per-operation costs for creating ops, attributes and uniquing that are an
order of magnitude above the 16 B-record budget. The single-pass backend
and the cache hit path must not depend on it. MLIR stays the
peak-performance backend, fed from NIR.

### Costs of unifying

- **One format to evolve.** Mitigated by versioning discipline: per-section
  strides, feature bits, extension `name@version`.
- **Risk of a "god IR"** that every feature wants to grow. Mitigated by
  keeping core small (the core opcode list stays near today's 30 kinds)
  and pushing everything else into extensions.
- **Migration touches everything at once.** Mitigated by the bridge from
  `IRGraph` to NIR and backend-by-backend porting (§6).

**Decision D7:** adopt A, C (canonical sections) and D; two levels instead
of B, with direct SSA emission for explicit control flow as a 2.x option;
reject E.

## 4. Components

### 4.1 Frontend

#### 4.1.1 Implicit tracing: same semantics, cheaper

Replay, tags, snapshots, merge detection, `static_val` and `region()` keep
their semantics. Changes:

- **Tag trie:** hashed children above about 8 (P0: 32% of instructions in
  tag creation with 1024 call sites).
- **Tag map:** open addressing in the trace arena, keyed to global op ids,
  so merge splits no longer rewrite entries.
- **Replay cursor:** index-based walk over the block's op-id list. The
  reconciliation skip compares `snapshot[id]` without re-fetching the block
  per step (P0: about 30% self time in `getCurrentBlock`,
  `getCurrentOperation` and `follow` on replay-heavy code).
- **Value refs:** `allocateValRef`/`freeValRef` (11% on replay-heavy code)
  use a free list in the trace arena.
- **Statistics:** #561's tracing counters (iterations, replayed ops,
  copies) are kept and surfaced. A diagnostic flags scopes whose replay
  exceeds a threshold and suggests `region()` or the explicit API.

#### 4.1.2 Explicit control flow, mixable with implicit

#300 adds `If`/`While`/`For` and traces them in a single pass, 230x faster
on `chainedIf100`. It forbids mixing with native `if` in the same function,
because replay would re-enter and double-emit the explicit constructs.
Nautilus 2 removes that restriction by building the constructs on region
scopes:

```cpp
If(x > 0, [&] { r = r + 1; }, [&] { r = r - 1; });
While([&] { return i < n; }, [&] { sum = sum + data[i]; i = i + 1; });
For(val<int64_t>(0), n, [&](val<int64_t> i) { sum = sum + data[i]; });
```

- **Each arm, loop header and loop body is a region scope** with its own
  exploration loop. An implicit `if` *inside* an arm replays only that arm.
- **The whole construct is memoized like a region,** keyed on the
  enclosing state. When an implicit branch *outside* forces the enclosing
  function to replay, the construct is skipped and resumes at its exit
  block, exactly as a memoized region is today. It is never emitted twice.
- **Loops emit a header block, a body and a back edge directly.**
  Loop-carried variables become block arguments through the existing SSA
  construction. No snapshot is taken for the loop itself.
- **The rules are those of `region()`:** values created inside a body must
  not outlive it, and results flow out through captured `val`s. `If` with
  a result (`auto r = If(c, [&]{ return a; }, [&]{ return b; })`) is sugar
  over a captured variable.
- **Early exits** are `Break()` and `Continue()` inside `While`/`For`.
  `Return(v)` leaves the function. Each is a terminator emitted directly.

A function written entirely with explicit constructs traces in one pass
with no snapshots or tag-map lookups. Tags are then needed only for source
locations, which are recorded lazily. Implicit control flow remains the
default and the compatibility path.

### 4.2 Flat trace (NIR, trace level)

NIR at `level = Trace` (§3a), as designed in [ir-representation-latency.md §12](ir-representation-latency.md):

- 16 B records indexed by global op id;
- a snapshot side array (written in record mode only);
- operands as 4 B value refs;
- constants, call descriptors, block refs and payloads in side tables;
- per-block `u32` op-id lists;
- `traceExtOp` for plugins;
- no per-operation heap allocation.

The explicit constructs (§4.1.2) write the same format and skip the
snapshot array.

### 4.3 NIR: the flat IR (SSA level)

NIR at `level = SSA` (§3a), as designed in [ir-representation-latency.md §13](ir-representation-latency.md),
with one more constraint: **the in-memory layout is the wire layout**.

- **Instructions:** 16 B `Inst {u16 op, u8 type, u8 flags, u32 a, b, c}`.
  A value's id is its index, and a block's id is its index.
- **Sections:** `insts`, `extra` (variable-length operands), `ConstPool`,
  `pointers` (§4.8.2), `blocks` (`BlockInfo`: first inst, argument range,
  loop depth, loop end, in layout order), `functions`, `names` (interned
  strings: functions, extension ops and types, symbols), and optional
  `debug` (source tags, regions).
- **The serialization rules of #532, applied to the in-memory format:**
  - little-endian, 8-byte-aligned sections;
  - every reference is a bounds-checkable `u32`;
  - each section declares its record stride, so a later minor version can
    widen records;
  - major/minor version plus compatible and incompatible feature bits.

  An image is therefore loaded by validating it and then using it in
  place, with no materialization pass.
- **Mutation:** build once, then rewrite by copying, plus `forward[]`
  replacement tables (§13 of the IR design). There are no persistent use
  lists; `useCount` is maintained by the builder.

### 4.4 Optimizing conversion and passes

- **One RPO pass from the flat trace to NIR:**
  - Braun SSA;
  - live-block tracking with constant-branch folding;
  - fold and simplify on emission (core and extension rules);
  - CSE keyed on the effect chain, block-local first and later
    dominator-scoped;
  - use counts;
  - straight-line threading.
- **The fixed-point pipeline is optional.** LICM and strength reduction
  remain as optional copy passes. Plugin passes register at
  `afterConversion` and `beforeLowering`, each marked required or optional.
- **No separate interim step.** #561's pass-table work speeds up today's pipeline, which 2.0 removes from the critical path; its tracing statistics carry over (§4.1.1).

### 4.5 Extension registry

As designed in [ir-representation-latency.md Part IV](ir-representation-latency.md):

- **Extension ops and types** get dense ids at registration and carry:
  - traits and effects;
  - fold, simplify and verify hooks;
  - a mandatory fallback (a native function or an expansion into core
    ops);
  - per-backend lowering tables.
- **One frozen, lock-free `ExtensionRegistry`** absorbs `IntrinsicRegistry`.
- **Extension types** (`simd.vec<T,N>`, `builtin.i128`) replace the SIMD
  ring buffer.

Additions for 2.0:

- **Every extension declares a name and a semantic version.** Images and
  artifacts record the extensions they use (`name@version`), never numeric
  ids. Loading maps names to the current process's ids and fails closed on
  a missing extension or an incompatible version. This is the "unknown or
  changing extensions fail closed" rule of #528, made structural.
- **Extensions declare whether they are portable:** whether their payloads
  contain process addresses. That feeds the provenance checks (§4.8.4).

### 4.6 Backends

| Backend | 2.0 role | Notes |
|---|---|---|
| **single-pass native** (TPDE-style; TPDE adapter or in-house, decision D2) | **Default** on x86-64 and AArch64 | Reads NIR and `BlockInfo` directly; value parts for `i128`; extension lowering via a narrow API |
| `tbc` (+ copy-and-patch) | Portable default; iOS and no-JIT platforms | Near 1:1 from NIR |
| `bc` | Kept for debugging and simplicity; could be merged into `tbc` later | |
| MLIR → LLVM | Peak-performance backend; the persisted-native-object path | #561's state reuse; reads NIR; maps extension types |
| `cpp` | Debugging and reference | |
| `asmjit` | Removed in 2.0 if the single-pass backend reaches parity (D2/D3); otherwise ported to NIR as a fallback | Superlinear call handling (P0) is a reason not to keep it as a default |
| GPU (CUDA, Metal) | Plugin backends on NIR | Extension ops for GPU intrinsics |

### 4.7 Semantics changes

- **S3, optimistic calls:**
  - `invoke` is `noUnwind` by default, with opt-in throwing;
  - calls declare their effects;
  - the inference passes leave the critical path;
  - debug-mode checking of the declarations.
- **S4, signless integers:**
  - values carry a width only;
  - signedness moves onto the operations that need it;
  - integer semantics are defined;
  - `val<uint32_t>` keeps its C++ meaning.

Both are breaking for plugins and for IR consumers, so they belong in the
major version. Details: [ir-representation-latency.md §8](ir-representation-latency.md).

### 4.8 Serialization and artifacts

Nautilus 2 unifies the two serialization lines that exist as PRs today:

- **IR images (#532):** portable IR plus a manifest, recompiled on the
  receiving engine with any backend;
- **native artifacts (#527):** relocatable native objects (MLIR/LLVM, ELF
  x86-64) with checked imports, loaded without tracing or code generation.

#### 4.8.1 Artifact layout

```
Artifact
├─ header: magic, format version, feature bits, checksum (CRC-32C), total size
├─ compatibility descriptor
│    core + provider build identities, LLVM / target / data layout,
│    extensions used (name@version), effective typed options, exports + signatures
├─ NIR image                      (always present; portable recompilation path)
├─ runtime-binding schema         (§4.8.3)
└─ native payloads (0..n)         per (backend, target): object + import table
```

- **The NIR image is mandatory.** A receiver that cannot use a native
  payload (another target, another backend, a stale build identity)
  recompiles from the NIR image without tracing. This merges #527's
  bytecode-repair path and #532's cross-backend loading into one rule.
  NIR replaces MLIR bytecode as the portable payload; MLIR is regenerated
  from NIR.
- **Native payloads keep #527's contract:**
  - exactly one relocatable object per payload;
  - imports identified by ELF build ID and load-bias-relative offset,
    resolved before JIT initialization;
  - exception-personality and unwind bridges.
- **`inspect(image)`** reads names, signatures, options and extensions
  without materializing anything (#532's `inspectModule`).
- **Loading checks** the root signatures against the requested C++
  signature, including width and signedness (#532).

#### 4.8.2 Pointers

NIR keeps every native address out of the instruction stream. Addresses
live in the `pointers` section, and instructions reference them by index:

- **Kinds:** callees, exception-capture thunks, destructors, `ConstPtr`
  values, and the addresses inside extension payloads.
- **Two modes, from #532:**
  - `ProcessLocal`: raw addresses guarded by a per-process token, for
    in-process caching only;
  - `Portable`: symbol names mapped by a client `SymbolNamer`/
    `SymbolResolver`, or build-ID + offset for native imports (#527).
- **Runtime state** is never a pointer entry. It is a runtime binding
  (§4.8.3).

#### 4.8.3 Runtime bindings (#529)

Module-local process state (hash tables, buffers, operator state) is
referenced through typed `RuntimeBinding<T>` handles:

- an immutable, module-owned registration snapshot;
- an address-free schema (stable symbols, compiler-derived
  type/size/alignment);
- fresh resolution on every load.

In NIR a binding is a core op (`bind.addr id`) whose identity is stable and
whose address is supplied at load. Every backend implements it, as #529
does for interpreted, C++, bc, tbc, AsmJit and MLIR. The schema is part of
the artifact and of the compatibility check.

#### 4.8.4 Provenance and safety

#527 and #528 run mandatory pre-optimization checks: scalar and
typed-allocation evidence, rejection of encoded addresses, guards against
laundering through memory and callbacks. In NIR these become **scans of
the side tables plus a short dataflow pass:**

- **Constants:** every `ConstPool` entry carries a provenance class:
  `Literal`, `AuditedInvariant` (`cacheLiteral`/`cacheInvariant`) or
  `RawProcessValue`. Raw values are produced by tracing native scalars that
  were not audited.
- **Pointers:** every `pointers` entry carries its kind and whether it is
  resolvable portably.
- **Eligibility:** an artifact is persistable only if no reachable
  instruction depends on a `RawProcessValue` constant or an unresolvable
  pointer. The check runs before optimization (folding could hide a raw
  value), as #527 requires. It is a linear scan over `useCount`-reachable
  instructions, not a separate analysis framework.
- **Failure:** fail closed, falling back to ordinary compilation, and
  report the reason in the statistics. Artifacts and cache directories
  remain trusted executable input: checksums detect accidental corruption,
  not malicious producers.

#### 4.8.5 C API (#531)

The C API exposes NIR directly:

- handles are `u32` ids into the flat sections;
- builders append instructions;
- inspection reads the records.

Because NIR's opcodes and record layouts are versioned, the C API's frozen
enums map one-to-one to wire opcodes, without the `switch` mapping layer
#531 needs today. `nautilus_ir_load` and `nautilus_ir_save` are the
serializer.

### 4.9 Caching

Three levels, each optional, all producing or consuming the artifacts of
§4.8:

| Level | Key | Lookup point | Hit skips | Source |
|---|---|---|---|---|
| **L1 semantic** | caller-supplied `engine.cache.key` + compatibility identity | **before tracing** | tracing, conversion, passes, codegen (native payload) | #528 |
| **L2 structural** | hash of the canonical NIR image (names, not ids; portable pointers; options; extension versions) | after conversion | passes and codegen | new |
| **L0 in-process** | the same L2 hash, with `ProcessLocal` pointers allowed | after conversion | passes and codegen, within one process | new |

- **L1** keeps #528's contracts:
  - the caller's key must cover interchangeable bodies, captures and
    specialization choices;
  - compatibility separately identifies core and provider builds, the LLVM
    target and layout, exports, typed effective options and extensions;
  - #529 measured all 1500 warm requests as native hits and −67% on the
    full NebulaStream suite.
- **L2 needs no caller key** and is safe by construction, because NIR fully
  determines the generated code. Hashing a flat image is a linear pass over
  contiguous memory. It catches the structurally equal queries that L1 key
  policy cannot express.
- **L0** serves repeated compiles inside one process (operators recompiled
  per query) with no filesystem involved.
- **Storage** keeps #528's model:
  - private directories, no symlink traversal, bounded reads;
  - atomic manifest-last publication with fsync;
  - per-key mutexes and cooperative file locks;
  - no eviction in 2.0; size-bounded eviction is a 2.x item.
- **Selection** is #528's injected cache compiler strategy, extended to L0
  and L2. With no cache configured, nothing changes.

### 4.10 Observability and tooling

- The **latency corpus** (`nautilus-latency-corpus`, P0) runs in the
  benchmark workflow with the 200% alert. Per-stage counters go into
  `CompilationStatistics`, including tracing replay and cache hit or miss
  reasons.
- The **backend explorer (#524)** reads NIR and the statistics to compare
  compile time and code quality per backend.
- **Dumps:** one printer for the flat trace and NIR. `test/data` is
  regenerated once, in isolated commits.

## 5. Compatibility and versioning

| Surface | 2.0 |
|---|---|
| `val<T>`, `static_val`, `region()`, `nautilus::function`, `invoke`, engine, module, options | **Source-compatible.** Renamed options keep deprecated aliases through 2.x |
| Explicit control flow | **New**, opt-in, mixable with implicit control flow |
| `invoke` exception default (S3) | **Breaking:** throwing callees must opt in (detected from `noexcept(false)` where the signature allows) |
| Plugin intrinsic API (`IntrinsicRegistry`, `MLIRIntrinsicPlugin`, `AsmJitBackendIntrinsic`) | **Replaced** by the extension API; a deprecated shim registers old intrinsics as extension ops through 2.x |
| `IRGraph`, `ir::Operation`, IR passes | **Removed** (internal) |
| IR text dumps, `test/data` | **New format** |
| Serialization (#532 format, #527 artifacts, #528 cache entries) | **First persisted formats, NIR-based.** No earlier format exists to support: the PRs are ported onto NIR, not released separately |
| C API (#531) | Ships with 2.0 on NIR; frozen from 2.0 onwards |

Version identities recorded in artifacts:

- the core version (semver);
- the NIR format (major.minor + feature bits);
- the artifact format;
- each extension's `name@version`.

## 6. Release plan: everything in one release, Nautilus 2.0

All changes ship together as **Nautilus 2.0**. There are no interim 1.x
feature releases and no temporary formats:

- **Development happens on an integration branch (`next`).** `main` takes
  only fixes until 2.0 merges.
- **The open PRs are ported onto NIR, not landed on today's IR:**

  | PR | Lands as |
  |---|---|
  | #300 explicit control flow | region-based constructs, M2 |
  | #527 / #528 / #529 artifacts, cache, runtime bindings | M5 |
  | #532 IR serialization | the canonical-section serializer, M1 |
  | #531 C API | M5 |
  | #561 compile-time cuts | MLIR state reuse in M4; tracing statistics in M2 |

  Features are therefore built once, on the final representation.
- **The milestones below are internal checkpoints on `next`.** Each one is
  merged when green, with the full CI matrix and the latency corpus
  running on `next`.

| Milestone | Content | Exit criterion |
|---|---|---|
| **M1 Foundations** | NIR format with levels (Trace/SSA) and the canonical/derived split; verifier, printer and canonical serializer (#532's format rules); `ExtensionRegistry` absorbing intrinsics; a bridge from `IRGraph` to NIR (internal, removed before release); latency corpus in CI on `next` | Every test's IR round-trips through the bridge, printer and serializer; plugin tests green on the registry |
| **M2 Frontend** | Flat trace (NIR trace level) with the hashed tag trie, open-addressing tag map, index-based replay cursor and value-ref free list; explicit `If`/`While`/`For` on region scopes, mixable with implicit control flow (port of #300); #561's tracing statistics; replay diagnostics | Tracing ≥ 30% cheaper on `distinctSites`; explicit `chainedIf`: one pass, zero replayed ops; mixed functions correct under the fuzzer |
| **M3 Conversion** | Optimizing conversion Trace→SSA (Braun SSA, folding, CSE, use counts, threading); legalization NIR→NIR; optional copy passes (LICM, strength reduction); plugin pass hooks | IR passes off the critical path; op counts ≤ today's after the full pipeline; fuzzer clean |
| **M4 Backends** | `bc`/`tbc`, `cpp`, MLIR (with #561's state reuse) and GPU on NIR; single-pass native prototype → D2 → parity (AArch64, exceptions, extension lowering) → D3 | End to end ≤ 0.5 µs/op on linear code; execution benchmarks no worse than 1.x per backend |
| **M5 Reuse** | Artifacts (NIR image + native payloads + descriptor, #527's import and loader contract); provenance classes and pre-optimization eligibility; runtime bindings (#529); L1 (#528), L2 and L0 caches; C API on NIR (#531) | NebulaStream warm suite ≥ 60% faster (−67% measured in #529); every #527/#528 negative test ported and failing closed |
| **M6 Semantics and extensions** | S3 (optimistic calls) and S4 (signless integers); extension types (`simd.vec`, `builtin.i128`) with the SIMD ring buffer removed; in-tree plugins migrated | Plugin suites green; SIMD tests pass on every backend without the lifetime rule |
| **M7 Release** | Remove `IRGraph`, the old passes, the bridge and (per D3) `asmjit`; migration guide; documentation; NebulaStream integration; x86-64 and AArch64; sanitizers | The release criteria below |

Order and parallelism:

- M1 comes first: everything else writes or reads NIR.
- M2 and M3 can proceed in parallel once M1 lands (M2 produces trace-level
  NIR; M3 can start on the bridge's output).
- M4 starts on the bridge's SSA output and switches to M3's when it lands.
- M5 needs M4 for native payloads, but its NIR-image path (portable
  artifacts, L0/L2) can start right after M1.
- M6 needs M3 (semantics live in the conversion) and M4 (lowering).
- M7 closes.

**Release criteria for 2.0:**

1. All core, plugin and NebulaStream suites green on x86-64 and AArch64,
   with ASan/UBSan and TSan.
2. Latency corpus targets met:
   - end to end ≤ 0.5 µs/op on linear code;
   - tracing linear in the number of `if`s when written explicitly;
   - no corpus case slower than 1.x.
3. No execution-time regression on `ExecutionBenchmark` or TPC-H
   (nautilus-duck) per backend.
4. Warm NebulaStream suite ≥ 60% faster than cold.
5. Migration guide covering:
   - plugin intrinsics → extensions;
   - throwing `invoke` callees (S3);
   - option renames.

## 7. Risks

| Risk | Mitigation |
|---|---|
| Region-based explicit control flow has corner cases (values escaping a body, nested memo keys) | Same rules and diagnostics as `region()`; fuzzer with mixed implicit and explicit functions; #300's tests as the base |
| In-memory layout = wire layout constrains in-memory evolution | Per-section strides and feature bits (#532); in-memory-only fields go into side tables that are not serialized |
| A long-lived `next` branch drifts from `main` | `main` takes fixes only; merge `main` into `next` weekly; full CI and corpus on `next`; milestones merged only when green |
| One big release concentrates risk | Internal milestones with exit criteria; release criteria gate 2.0; adapters (the bridge) let each milestone be validated against today's behavior |
| Provenance classes miss a raw-address path | Fail closed by default; keep #527's negative tests (encoded, laundered, callback, destructor-only); persistence only for eligible modules |
| L2 hash collisions | 128-bit hash plus a full image comparison on hit (images are small and flat) |
| Single-pass backend decision (D2) delays 2.0-beta | `tbc` is a complete fallback default; MLIR remains available |
| Breaking `invoke` default (S3) | Derive the default from `noexcept` where possible; migration guide and compile-time warnings for `noexcept(false)` callees without an opt-in; debug-mode checks |

## 8. Decisions needed

| # | Question | Proposed answer |
|---|---|---|
| D1 | Order after P0 | As in §6: one release; foundations first, then frontend and conversion (the measured hot spots) |
| D2 | TPDE adapter or in-house single-pass backend | Prototype the TPDE adapter first (2.0-alpha), decide on latency, exceptions and extension support |
| D3 | Default backend in 2.0 | Single-pass native where available, `tbc` elsewhere |
| D4 | Annotation policy for runtime calls (S3) | Derived from `noexcept`; explicit opt-in otherwise |
| D5 | Land the cache stack (#527–#529) and #532 separately, or port them onto NIR | **Port onto NIR (M1, M5).** Everything ships in 2.0, so there are no temporary v1 formats |
| D6 | Keep `bc` beside `tbc` | Keep in 2.0; revisit merging in 2.x |
| D7 | Unified IR: what exactly to unify | One encoding and infrastructure; two levels (trace, SSA); in-memory = wire for canonical sections; legalization NIR→NIR; MLIR stays a backend (§3a) |
