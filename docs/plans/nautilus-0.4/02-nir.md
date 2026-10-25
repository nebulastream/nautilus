# Component: NIR, the Flat Intermediate Representation

Part of the [Nautilus 0.4 design](README.md) · Milestone M1 · Builds on #532
(serialization format rules)

## 1. Purpose

[13-nir-spec.md](13-nir-spec.md) is the normative specification of the
format; this document explains the design behind it.

NIR is the one representation every 0.4 stage reads and writes:
- the frontend (pre-SSA level);
- the conversion, passes and backends (SSA level);
- the serializer, the caches and the C API.

This document defines its format and the rules that keep it fast,
serializable and extensible.

## 2. Today's IR and what it costs

Measured with a `sizeof` probe on x86-64:

| Object | Size | Storage |
|---|---|---|
| `Operation` header | 32 B | arena; inputs in a separate arena span |
| `AddOperation` | 32 B + 16 B inputs | |
| `ConstIntOperation` | 40 B | |
| `BranchOperation` | 80 B | embeds one 48 B `BasicBlockInvocation` |
| `IfOperation` | 144 B | embeds two |
| `CallOperation` | 152 B | two `std::string`s plus a destructor vector, although the callee is interned in the `FunctionTable` |
| `BasicBlock` | 88 B | three heap `std::vector`s (operations, arguments, predecessors) |

Ids are trace `ValueRef`s, so they are almost dense, but nothing guarantees
it. Every backend maps values through
`std::unordered_map<OperationIdentifier, …>` (`Frame`), and every pass
builds `FunctionRewriter`'s hash-map use table. With up to 7 passes × 4
rounds, that is 28 table builds per function.

P0 ([p0-baseline.md](p0-baseline.md)):
- building `FunctionRewriter` tables is **44% of all instructions** on
  linear code;
- hash-map code is about 25% self time;
- there are 3–5 heap allocations per IR op;
- every op is materialized twice (trace record + IR object).

## 3. Format

### 3.1 Instructions

```cpp
struct Inst {          // 16 B; a value's id is its index
	uint16_t op;       // [0, 256): core opcodes; [256, …): extension opcodes
	uint8_t  type;     // TypeId: core scalar types fixed; extension types registered
	uint8_t  flags;    // opcode-specific (13 §5): signedness, predicate, cast kind, alignment, checked
	uint32_t a, b, c;  // operands: value ids or tagged ConstPool refs;
	                   // or table indices, immediates, an index into `extra`
};
```

- **Constants live in `ConstPool`,** referenced with the top bit set (the
  counterpart of LuaJIT's refs below `REF_BIAS`). An add of a constant needs
  no constant instruction, and backends see the immediate directly.
- **Traced host addresses are not constants.** Each is a `pointers` entry
  with a provenance class, read by `ptr.const`
  ([13 §4.7, §5.8](13-nir-spec.md)), so a raw address can never hide in a
  constant and every process-local module is marked as such.
- **Variable-length operands** (call arguments, block-argument lists,
  payloads such as shuffle masks) live in `extra`, referenced from `c`.
- **Calls** are `{op=call, a=calleeId, c=extra offset}`. The callee
  descriptor lives in the callee table, and names are resolved only for
  dumps.
- **Block arguments are instructions** (`op=blockarg`) at the start of their
  block, so they have value ids like everything else.
- **Terminators** are `br target, extraIdx` and `condbr cond, trueBlk,
  extraIdx`; the probability, the false target and both argument lists are
  in `extra` ([13 §5.9](13-nir-spec.md)). There are no block, invocation or
  predecessor objects.

### 3.2 Sections

The normative list of sections, with record layouts, is in
[13-nir-spec.md §4](13-nir-spec.md); this table summarizes it.

| Section | Content | Canonical? |
|---|---|---|
| `insts` | `Inst` records, blocks contiguous, in layout order | yes |
| `extra` | `u32` payloads | yes |
| `consts` | interned constants, each with a provenance class ([07](07-serialization.md)) | yes |
| `pointers` | callees, capture thunks, destructors, traced addresses (`ptr.const`, replacing `ConstPtr`), extension-payload addresses; each process-local or portable, traced addresses with a provenance class ([07](07-serialization.md)) | yes |
| `blocks` | `BlockRecord` per block: first instruction, instruction count, `argCount`, landing-pad flag, in layout order. Loop depth and loop end are derived (§3.6) | yes |
| `functions` | name, signature, attributes, visibility, level, instruction, block and alloca ranges | yes |
| `signatures` | result and parameter types, with a `zext`/`sext` extension kind per narrow integer for native boundaries | yes |
| `callees` | internal, external, imported and intrinsic call targets with their attributes | yes |
| `allocas`, `bindings`, `data` + `dataBytes` | static stack slots; runtime bindings ([07 §4](07-serialization.md)); read-only data objects | yes |
| `functionAttrs` | plugin-owned string attributes of functions (`gpu.kernel`) | yes |
| `module`, `imports` | the module name and process token; imports (in memory only; images bundle them) | yes |
| `names` + `nameBytes` | interned strings: functions, symbols, extension ops and types (`name@version`) | yes |
| `extensions` | extensions used by this module | yes |
| `useCount`, `forward`, loop depth/end, CSE tables, snapshots | derived | **no** |
| debug sections: source locations, regions, parameter names | optional | **no** |

**Format rules, from #532:**
- little-endian, 8-byte-aligned sections;
- every reference is a bounds-checkable `u32`;
- each section declares its record stride, so a later minor version can
  widen records;
- major/minor version plus compatible and incompatible feature bits;
- a CRC-32C checksum and a total size for framing;
- a normative text form with a grammar, which round-trips
  ([13 §9](13-nir-spec.md)), and stable diagnostic codes
  ([13 §6.6](13-nir-spec.md)).

### 3.3 Core operations

The core opcode set covers what today's IR expresses, and stays near its
size (~30 kinds). Everything else is an extension ([04](04-extensions.md)).

| Group | Core opcodes | Replaces today's |
|---|---|---|
| Arithmetic | `add`, `sub`, `mul`, `div`, `rem`, `fneg` | `AddOp`, `SubOp`, `MulOp`, `DivOp`, `ModOp`, `NegateOp` on floats (sign flip) |
| Logical (on `bool`) | `and`, `or`, `not` | `AndOp`, `OrOp`, `NotOp` |
| Bitwise | `band`, `bor`, `bxor`, `bnot`, `shl`, `shr` | `BinaryComp` (`BAND`/`BOR`/`XOR`), `NegateOp` on integers (bitwise NOT), `ShiftOp` |
| Compare and select | `cmp` (predicate in `flags`), `select` | `CompareOp`, `SelectOp` |
| Casts | `cast` (kind in `flags`: extend, truncate, int↔float, bitcast) | `CastOp` |
| Memory | `load`, `store`, `alloca` (index into the function's alloca table) | `LoadOp`, `StoreOp`, `AllocaOp` |
| Calls | `call`, `call.indirect`, `funcaddr` | `CallOp`, `IndirectCallOp`, `FunctionAddressOfOp` |
| Runtime state, data, addresses | `bind.addr` ([07 §4](07-serialization.md)), `data.addr`, `ptr.const` | — (new; from #529); `ConstPtrOp` |
| Values | `blockarg`; constants live in `ConstPool` and are not instructions | `BasicBlockArgument`, `ConstInt`/`ConstFloat`/`ConstBoolean` ops |
| Control flow | `br`, `condbr` (probability and false target in `extra`), `ret`, `resume` (ends a landing pad) | `BranchOp`, `IfOp`, `ReturnOp`, `BlockInvocation`; landing pads of `FunctionExceptionRegion` |
| Pre-SSA level only | `var` (defines a variable from a constant or a copy), `assign` (variable redefinition) | `CONST` and copy `ASSIGN`; `ASSIGN` in today's trace |

`FunctionOp` becomes the `functions` section, and `MLIR_YIELD` disappears.
Signedness moves from types onto the operations that need it
([06](06-semantics.md)).

### 3.4 Functions, modules, exceptions

- **A module is one NIR image** holding several functions. Internal calls
  reference functions by index. External callees, imports and intrinsics
  resolve through the callee table, as today's interned `FunctionId`s do;
  intrinsics are known by `name@version`, independent of the backends
  built ([13 §4.6.1](13-nir-spec.md)).
- **A function record** holds:
  - its name and signature;
  - its attributes (today's `FunctionAttributes`: memory effects,
    `noUnwind`, `willReturn`, and so on);
  - its alloca table (`AllocaSpec`: size and alignment), materialized in
    each backend's prologue as today;
  - its string attributes (0.3's `kernel` becomes `gpu.kernel`,
    [13 §4.5.2](13-nir-spec.md));
  - its region table (`RegionSpec`), parameter names and location, in the
    optional debug sections;
  - its blocks.
- **Exceptions:**
  - a call that may unwind carries an `unwind` entry in its `extra`
    payload, and a checked integer division carries one in `c`: the index
    of a landing-pad block, or "no cleanup";
  - landing-pad blocks hold the ordered destructor calls (reverse
    construction order) as calls of destructor callees, are entered only
    through unwind entries or from another pad, and end in `resume` or in
    a branch to a pad with a shared tail, as today's
    `FunctionExceptionRegion` does ([13 §6.3](13-nir-spec.md));
  - pads are ordinary blocks after the main layout order, flagged in
    `BlockRecord`, so the verifier and every backend see them uniformly;
  - capture thunks (`captureFunc`) and destructors are `pointers` entries
    ([07 §3](07-serialization.md)); every may-throw external, intrinsic or
    indirect call names a thunk;
  - a failing checked division calls the runtime helper
    `nautilus_rt_throw_arith` on a cold path, like a may-throw call
    ([13 §5.10](13-nir-spec.md)).
- **Thread safety:**
  - a finished SSA-level NIR module is immutable, so any number of
    threads may compile or serialize it concurrently;
  - building and rewriting are single-threaded per module;
  - the extension registry is frozen when the first engine is
    constructed.

### 3.5 Levels

Every function carries `level ∈ {PreSSA, SSA}`. The format is the same; the
invariants differ:

| | Pre-SSA level | SSA level |
|---|---|---|
| Values | **Variables:** `var` defines, `assign` redefines a value ref | Every value defined once; block arguments at merges |
| Order | Recording order; blocks split at merges during tracing | Layout order (RPO, reducible loops contiguous) |
| Ids | Stable during splits (the tag map points at them) | Dense; copy passes renumber |
| Extra state | Snapshots, tag map, replay cursor | `useCount`, `forward`, loop info |
| Lifetime | Append-only; discarded after conversion | Build once, rewrite by copying; persisted |
| Consumers | The conversion only; pre-SSA images for debugging | Passes, backends, serializer, caches, C API |

The verifier enforces each level's invariants, and the conversion
([03](03-conversion-and-passes.md)) is the only path from trace to SSA.

A single level is rejected for 0.4. It would either put variables and
recording-order artifacts into the persisted IR, or force tracing to build
SSA before the CFG is final, which Braun-style construction on a complete
CFG avoids. Functions written entirely with explicit control flow know
their CFG while tracing, so they could later be emitted at the SSA level
directly.

### 3.6 Canonical vs. derived data

Making the in-memory layout the wire layout gives:
- zero-copy loading (validate, then use in place);
- a cache key that is a hash over contiguous bytes;
- a C API that reads records directly.

What keeps the in-memory format free to evolve:

- **Only canonical sections** are serialized, hashed and versioned.
- **Derived sections are rebuilt after load** and may change layout at any
  time: use counts, replacement tables, loop depth and end, CSE tables,
  snapshots, debug tags.
- **Canonicalization before writing or hashing:** apply `forward`
  replacements, compact, renumber densely, and order and intern every table
  in first-use order. Two structurally equal modules then produce equal
  bytes, which the L2 cache ([08](08-caching.md)) and deterministic output
  rely on. [13 §7](13-nir-spec.md) defines the algorithm, the scan order and
  the exact bytes the structural hash covers.

## 4. Mutation model

- **Build once, rewrite by copying.** A pass that removes or replaces
  operations streams `insts` into a new array using an old→new id map. This
  is O(n), cache-friendly, and keeps ids dense.
- **Local replacements** record "replaced by" in `forward[]`. Readers
  resolve it lazily, and compaction applies it.
- **No persistent use lists.** `useCount` is maintained by the builder. A
  pass that needs real uses builds a CSR table once (offsets plus a flat use
  array).
- **Side tables are flat arrays indexed by value id.** Backend register
  maps, liveness and spill slots are all arrays. No hash map remains on a
  per-operand path.

### 4.1 Arena management

Trace and NIR memory comes from arenas, so arena policy decides how much
memory a long-running engine keeps. 0.4 replaces 0.3's pool of whole arenas
with a bounded pool of chunks (M1).

**0.3 and what it costs:**
- **Pools of whole arenas:** each engine has one `ArenaPool` for traces and
  one for IR graphs, each keeping up to 8 spare arenas
  (`ArenaPool::DEFAULT_MAX_SPARES`). A returned arena is `softReset()` and
  keeps its heap chunks, so the next compile allocates nothing.
- **The high-water mark is pinned:** the engine keeps the chunks of its
  largest compiles for its lifetime, for up to 8 concurrent compiles per
  pool. **Measured:** after one compile of about 20,000 ops, 22.5 MB of heap
  stays held with `asmjit` or `tbc` and about 22 MB with MLIR.
- **Freeing to `malloc` is not enough:** with the chunks freed on release,
  the heap drops to 0.3–1.4 MB, but RSS stays at about 35 MB, because glibc
  keeps the memory. Bounding RSS needs chunks that go back to the OS.

**Design:**

1. **Fixed chunk sizes.** Chunks grow from 64 KiB to 1 MiB, then stay at
   1 MiB. A request above half a chunk gets a dedicated allocation that is
   freed when its arena is released and never pooled, so a single huge
   compile cannot pin its peak.
2. **One chunk pool per engine, with a per-thread cache.** An arena is a
   cheap object (header plus inline buffer). It takes chunks from a small
   per-thread cache, then from the engine's shared pool, then from the OS,
   and returns them the same way.
   - The steady state stays allocation-free, as in 0.3.
   - The common path takes no lock.
   - The trace and IR pools of 0.3 become one pool of chunks.
3. **A byte budget with decay.**
   - The pool holds at most `engine.arena.retainedBytes`. The default is a
     few MiB per active compile thread under a fixed total cap, set in M1
     from the corpus and the NebulaStream suite.
   - Chunks beyond the budget go back to the OS immediately.
   - Chunks unused for a few seconds are trimmed lazily on acquire and
     release, with no background thread.
   - `NautilusEngine::trimMemory()` lets an embedder react to memory
     pressure.
4. **Page-backed chunks.** 1 MiB chunks are allocated with `mmap`. A retired
   chunk is released with `madvise(MADV_DONTNEED)` on Linux
   (`MADV_FREE_REUSABLE` on macOS), so RSS drops while the address range
   stays reusable.
5. **Transient and persistent memory are separate.**
   - Tracing and conversion scratch memory uses the recycled arenas.
   - At the end of conversion, the SSA-level NIR module is compacted into
     exact-size flat buffers, reserved from the trace's op count. NIR
     already rewrites by copying (§4), so this costs one copy that the
     conversion makes anyway.
   - Modules that outlive the compile (a pending promotion
     ([11](11-tiered-compilation.md)), the L0 cache ([08](08-caching.md)),
     serialization ([07](07-serialization.md))) therefore never hold arena
     chunks. Retention only concerns memory that is reused all the time.
   - The trace arena is returned right after conversion.
6. **An embedder hook.** The engine accepts an optional chunk provider, so
   NebulaStream can charge compiler memory to its own budget and limit it.
7. **Accounting.** Atomic counters for bytes held, bytes in use, the peak per
   compile and chunks trimmed, reported in the statistics
   ([10](10-observability.md)) and the latency corpus.

**Effect:** compile latency stays as in 0.3 (allocation-free steady state).
After a large compile the engine holds at most the budget, and RSS actually
drops. Compiler memory becomes predictable and, through the hook,
enforceable by the embedder.

**Prior art:** HotSpot's arena chunk pools (fixed chunk sizes, periodic
pruning) and V8's compiler zones (pooled segments with a cap) solve the same
problem the same way.

## 5. Cost comparison for one binary `add` (x86-64)

| | 0.3 | 0.4 |
|---|---|---|
| Trace record | 56 B + 2 × 24 B inputs | 16 B + 16 B snapshot (record mode only) |
| Tag-map insert | `unordered_map` node (`malloc`), rewritten on block split | open-addressing slot, never rewritten |
| IR object | 32 B + 16 B inputs + 8 B vector slot | 16 B `Inst` |
| Constant operand | separate 40 B `ConstIntOperation` | interned `ConstPool` entry |
| Branch / if | 80 B / 144 B + block objects | 16 B terminator + args in `extra` |
| Backend value lookup | hash probe | array index |
| Heap allocations per op | ≥ 1 | 0 (recycled arrays) |

That is roughly 5x fewer bytes per traced operation and about 3x fewer per
IR operation.

## 6. Design rationale: why one IR

- **One encoding and infrastructure.** Today the trace, the IR, #532's
  serialized form and #531's C API view are four representations. Regions,
  provenance evidence (#527) and runtime bindings (#529) each had to be
  carried through every layer. With one format, a feature or a plugin op is
  defined once. Tracing, conversion, serialization, the C API and the
  printer then pick it up.
- **Not MLIR.** MLIR's fixed setup (3–10 ms per compile) and per-op costs
  exceed the 16 B-record budget by an order of magnitude. MLIR stays a
  backend fed from NIR ([05](05-backends.md)).
- **Up to legalization, not below.** Backends keep their own internal forms;
  legalization is NIR→NIR ([03](03-conversion-and-passes.md)).
- **Costs of unifying:**
  - **One format to evolve:** handled with strides, feature bits and
    extension versions.
  - **The pull toward a "god IR":** core opcodes stay near today's ~30
    kinds, and everything else is an extension.
  - **A migration that touches everything:** the new pipeline is built
    next to the old one and replaces it in one cut-over (§7).

## 7. Migration

- **No bridge.** The new tracer, the conversion and the NIR backends are
  built next to the 0.3 pipeline and tested on their own (`.nir` text tests,
  `nautilus-nir-run`, golden pre-SSA dumps). The engine switches to them in
  one cut-over, which deletes `IRGraph` and the 0.3 trace
  ([16 §2](16-implementation-plan.md)).
- **Consumers:**
  - `tbc` becomes a near 1:1 translation (`bc` is removed in M1);
  - `asmjit` iterates `insts` with `BlockRecord` in one pass;
  - MLIR builds `cf`/`arith` from blocks and terminators.
- **Dumps:** one printer for both levels; `test/data` regenerated once per
  format change, in isolated commits.

## 8. Exit criteria (M1)

- NIR format, verifier (both levels), printer and serializer merged with
  unit tests.
- Every opcode round-trips through builder → printer → parser →
  serializer → loader with byte-identical re-serialization; the golden
  images load or are rejected with the specified `NIR-` codes; the printer
  and parser implement the grammar of [13 §9](13-nir-spec.md).
- No hash map on any NIR per-operand path.
- The chunk pool of §4.1 in place: after a large compile, held memory and
  RSS return to the budget; no compile-latency regression on the corpus.

## 9. Risks

- **The 16 B budget is tight for plugins.** Wide immediates go to
  `ConstPool`, payloads to `extra`. Each new opcode is reviewed against the
  format.
- **Dense-id drift.** The verifier checks it; ids are minted only by the
  builder or rewriter.
- **Arena budget too small:** compiles allocate from the OS again and slow
  down. The budget is configurable, its defaults come from the corpus, and
  the trimmed-chunk counter makes it visible.
- **Format churn shows up as dump churn.** Regenerate in isolated, reviewed
  commits.
