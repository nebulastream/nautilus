# Component: NIR, the Flat Intermediate Representation

Part of the [Nautilus 0.4 design](README.md) · Milestone M1 · Builds on #532
(serialization format rules)

## 1. Purpose

NIR is the one representation every 0.4 stage reads and writes:
- the frontend (trace level);
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
	uint8_t  flags;    // branch hint, effect class, …
	uint32_t a, b, c;  // operands: value ids or tagged ConstPool refs;
	                   // c may also be an immediate or an index into `extra`
};
```

- **Constants live in `ConstPool`,** referenced with the top bit set (the
  counterpart of LuaJIT's refs below `REF_BIAS`). An add of a constant needs
  no constant instruction, and backends see the immediate directly.
- **Variable-length operands** (call arguments, block-argument lists,
  payloads such as shuffle masks) live in `extra`, referenced from `c`.
- **Calls** are `{op=call, a=calleeId, c=extra offset}`. The callee
  descriptor lives in the function table, and names are resolved only for
  dumps.
- **Block arguments are instructions** (`op=blockarg`) at the start of their
  block, so they have value ids like everything else.
- **Terminators** are `br target, extraIdx` and `condbr cond, t, f, extraIdx`,
  with argument lists in `extra`. There are no block, invocation or
  predecessor objects.

### 3.2 Sections

| Section | Content | Canonical? |
|---|---|---|
| `insts` | `Inst` records, blocks contiguous, in layout order | yes |
| `extra` | `u32` payloads | yes |
| `consts` | interned constants, each with a provenance class ([07](07-serialization.md)) | yes |
| `pointers` | callees, thunks, destructors, `ConstPtr`, extension-payload addresses; process-local or portable ([07](07-serialization.md)) | yes |
| `blocks` | `BlockInfo` per block: first inst, argument range, in layout order. Loop depth and loop end are derived fields (§3.6) | yes |
| `functions` | name, signature, attributes, blocks | yes |
| `names` | interned strings: functions, symbols, extension ops and types (`name@version`) | yes |
| `extensions` | extensions used by this module | yes |
| `useCount`, `forward`, loop depth/end, CSE tables, snapshots, `debug` (source tags, regions) | derived or optional | **no** |

**Format rules, from #532:**
- little-endian, 8-byte-aligned sections;
- every reference is a bounds-checkable `u32`;
- each section declares its record stride, so a later minor version can
  widen records;
- major/minor version plus compatible and incompatible feature bits;
- a CRC-32C checksum and a total size for framing.

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
| Runtime state | `bind.addr` ([07 §4](07-serialization.md)) | — (new; from #529) |
| Values | `blockarg`; constants live in `ConstPool` and are not instructions | `BasicBlockArgument`, `Const*Op` |
| Control flow | `br`, `condbr` (probability in `extra`), `ret`, `resume` (ends a landing pad) | `BranchOp`, `IfOp`, `ReturnOp`, `BlockInvocation`; landing pads of `FunctionExceptionRegion` |
| Trace level only | `assign` (variable redefinition) | `ASSIGN` in today's trace |

`FunctionOp` becomes the `functions` section, and `MLIR_YIELD` disappears.
Signedness moves from types onto the operations that need it
([06](06-semantics.md)).

### 3.4 Functions, modules, exceptions

- **A module is one NIR image** holding several functions. Internal calls
  reference functions by index. External callees, intrinsics and
  extensions resolve through the function table, as today's interned
  `FunctionId`s do.
- **A function record** holds:
  - its name and signature;
  - its attributes (today's `FunctionAttributes`: memory effects,
    `noUnwind`, `willReturn`, and so on);
  - its alloca table (`AllocaSpec`: size and alignment), materialized in
    each backend's prologue as today;
  - its region table (`RegionSpec`), in the optional `debug` section;
  - its blocks.
- **Exceptions:**
  - a call that may unwind carries an `unwind` entry in its `extra`
    payload: the index of a landing-pad block, or "no cleanup";
  - landing-pad blocks hold the ordered destructor calls (reverse
    construction order) and end in `resume`, as today's
    `FunctionExceptionRegion` does;
  - pads are ordinary blocks outside the main layout order, flagged in
    `BlockInfo`, so the verifier and every backend see them uniformly;
  - capture thunks (`captureFunc`) and destructors are `pointers` entries
    ([07 §3](07-serialization.md)).
- **Thread safety:**
  - a finished SSA-level NIR module is immutable, so any number of
    threads may compile or serialize it concurrently;
  - building and rewriting are single-threaded per module;
  - the extension registry is frozen after the first compile.

### 3.5 Levels

Every function carries `level ∈ {Trace, SSA}`. The format is the same; the
invariants differ:

| | Trace level | SSA level |
|---|---|---|
| Values | **Variables:** `assign` redefines a value ref | Every value defined once; block arguments at merges |
| Order | Recording order; blocks split at merges during tracing | Layout order (RPO, loops contiguous) |
| Ids | Stable during splits (the tag map points at them) | Dense; copy passes renumber |
| Extra state | Snapshots, tag map, replay cursor | `useCount`, `forward`, loop info |
| Lifetime | Append-only; discarded after conversion | Build once, rewrite by copying; persisted |
| Consumers | The conversion only; trace-level images for debugging | Passes, backends, serializer, caches, C API |

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
  replacements, compact, renumber densely, and intern names in first-use
  order. Two structurally equal modules then produce equal bytes, which the
  L2 cache ([08](08-caching.md)) and deterministic output rely on.

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
  - **A migration that touches everything:** the bridge (§7) and
    backend-by-backend porting.

## 7. Migration

- **M1 adds an internal bridge `IRGraph` → NIR,** run after today's passes.
  Backend work (M4) and the serializer (M5) can start before the new
  conversion (M3) exists. The bridge is removed before release.
- **Consumers:**
  - `bc`/`tbc` become a near 1:1 translation;
  - `asmjit` iterates `insts` with `BlockInfo` in one pass;
  - MLIR builds `cf`/`arith` from blocks and terminators.
- **Dumps:** one printer for both levels; `test/data` regenerated once per
  format change, in isolated commits.

## 8. Exit criteria (M1)

- NIR format, verifier (both levels), printer and serializer merged with
  unit tests.
- Every execution test's IR round-trips through bridge → printer →
  serializer → loader with byte-identical re-serialization.
- No hash map on any NIR per-operand path.

## 9. Risks

- **The 16 B budget is tight for plugins.** Wide immediates go to
  `ConstPool`, payloads to `extra`. Each new opcode is reviewed against the
  format.
- **Dense-id drift.** The verifier checks it; ids are minted only by the
  builder or rewriter.
- **Format churn shows up as dump churn.** Regenerate in isolated, reviewed
  commits.
