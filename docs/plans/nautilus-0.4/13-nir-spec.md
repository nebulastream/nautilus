# NIR Specification

Part of the [Nautilus 0.4 design](README.md) · Status: **draft**, normative
from the M1 exit · Format version 1.0

This document is the normative definition of NIR: its data model, binary
encoding, operations, semantics, invariants and versioning rules. Every
producer (the tracer, the conversion, passes, the C API, the deserializer)
and every consumer (backends, the serializer, caches, tools) must follow it.
[02-nir.md](02-nir.md) explains the design and why it is this way; where the
two differ, this document wins.

**Conventions.** "Must" and "must not" are requirements; a module that
violates one is *invalid* and the verifier rejects it. "Undefined" means the
program's behavior is not constrained, as in C++. All integers in the binary
encoding are little-endian. Offsets and sizes are in bytes unless stated.

## 1. Data model

- A **module** holds functions and the tables they share (constants,
  names, callees, signatures, pointers, bindings, extensions).
- A **function** is a list of **blocks**. A block is a contiguous run of
  **instructions**; its last instruction, and only that one, is a
  terminator.
- An instruction produces **at most one value**. A value's **id** is the
  index of its instruction within the function, starting at 0. Ids are
  function-local.
- A **block id** is the index of the block within its function. Block 0 is
  the entry block.
- **Constants** are not instructions. They live in the module's constant
  pool and are referenced from operands (§3.2).
- Every function carries a **level**, `Trace` or `SSA` (§6). Both levels
  use this encoding.

## 2. Types

A type is a `u8` `TypeId`.

| Id | Name | Meaning |
|---|---|---|
| 0 | `void` | no value; type of instructions without a result |
| 1 | `bool` | 1 bit, stored as one byte (0 or 1) |
| 2 | `i8` | 8-bit integer, signless |
| 3 | `i16` | 16-bit integer, signless |
| 4 | `i32` | 32-bit integer, signless |
| 5 | `i64` | 64-bit integer, signless |
| 6 | `f32` | IEEE 754 binary32 |
| 7 | `f64` | IEEE 754 binary64 |
| 8 | `ptr` | address, 64 bits on every 0.4 target |
| 9–15 | — | reserved for core |
| 16–255 | extension | registered types ([04 §3.2](04-extensions.md)) |

- **Integers are signless** ([06 §4](06-semantics.md)). Operations that
  depend on signedness say so in their flags (§5).
- **Extension type ids are per process in memory.** In a serialized image
  they are image-local (16, 17, … in first-use order) and named in the
  `extensions` section; the loader maps them to process ids (§8.4).
- 0.3's `ui8`…`ui64` map to `i8`…`i64`; the signedness moves to the
  operations that use the values.

## 3. Instructions and operands

### 3.1 Instruction record

```c
struct Inst {          // 16 bytes, 4-byte aligned
	uint16_t op;       // opcode (§5); 0 is invalid
	uint8_t  type;     // result TypeId (§2); 0 (void) if there is no result
	uint8_t  flags;    // opcode-specific (§5)
	uint32_t a, b, c;  // operands; meaning per opcode (§5)
};
```

- **Opcodes** below 256 are core (§5). Opcodes from 256 up are extension
  operations; like extension types they are per process in memory and
  image-local in a serialized image.
- **Unused operands must be 0.** Unused flag bits must be 0.

### 3.2 Value references

An operand that holds a value is a 32-bit **value reference**:

| Bit pattern | Meaning |
|---|---|
| `0x00000000`–`0x7FFFFFFE` | value id within the same function |
| `0x80000000`–`0xFFFFFFFE` | constant: index `ref & 0x7FFFFFFF` in the constant pool |
| `0x7FFFFFFF` | reserved |
| `0xFFFFFFFF` | `NONE`: no value (only where an opcode allows it) |

A function therefore holds at most 2³¹ − 1 instructions. Operands that are
not value references (block ids, table indices, `extra` offsets) are plain
`u32` indices, named in §5.

### 3.3 The `extra` array

Variable-length operand lists live in the module's `extra` section, an
array of `u32` words. An instruction refers to a list by its word offset in
`c`. The layouts are given per opcode (§5). Lists of one instruction must not
overlap those of another.

### 3.4 Unwind entries

A **unwind entry** is one `u32` attached to an instruction that may throw
(calls, checked division; §5):

| Value | Meaning |
|---|---|
| `0xFFFFFFFF` (`NO_UNWIND`) | the instruction cannot throw |
| `0xFFFFFFFE` (`NO_CLEANUP`) | may throw; nothing to clean up; the exception propagates to the caller |
| block id | may throw; on an exception, control enters this landing-pad block (§4.4) |

## 4. Module structure

### 4.1 Sections

A module is a set of sections. **Canonical** sections are serialized, hashed
and versioned. **Optional** sections are serialized when present and do not
affect the structural hash. **Derived** data is never serialized ([02 §3.6](02-nir.md)).

| Kind | Section | Record | Class |
|---|---|---|---|
| 1 | `insts` | `Inst`, 16 B | canonical |
| 2 | `extra` | `u32` | canonical |
| 3 | `consts` | `ConstRecord`, 16 B (§4.2) | canonical |
| 4 | `constBytes` | bytes, for constants wider than 8 bytes | canonical |
| 5 | `blocks` | `BlockRecord`, 16 B (§4.3) | canonical |
| 6 | `functions` | `FunctionRecord`, 40 B (§4.4) | canonical |
| 7 | `signatures` | `SignatureRecord`, 12 B, types in `extra` | canonical |
| 8 | `callees` | `CalleeRecord`, 16 B (§4.5) | canonical |
| 9 | `allocas` | `{u32 size, u32 align}`, 8 B | canonical |
| 10 | `pointers` | `PointerRecord`, 24 B (§4.6) | canonical |
| 11 | `bindings` | `BindingRecord`, 16 B (§4.7) | canonical |
| 12 | `names` | `{u32 offset, u32 length}` into `nameBytes` | canonical |
| 13 | `nameBytes` | UTF-8 bytes, not NUL-terminated | canonical |
| 14 | `extensions` | `{u32 nameId, u8 kind, u8 pad[3]}`, 8 B (§8.4) | canonical |
| 15 | `debug` | source tags and region tables (§4.8) | optional |
| ≥ 1024 | — | reserved for vendor and experimental sections | optional |

Instructions of one function, and blocks of one function, are contiguous;
functions are laid out in `functions` order.

### 4.2 Constants

```c
struct ConstRecord {     // 16 bytes
	uint8_t  type;       // TypeId
	uint8_t  provenance; // 0 Literal, 1 AuditedInvariant, 2 RawProcessValue (07 §5)
	uint16_t pad;        // 0
	uint32_t byteOffset; // into constBytes for types wider than 8 bytes, else 0
	uint64_t bits;       // the value's bit pattern, zero-extended
};
```

- Floats are stored by bit pattern, so NaN payloads and `-0.0` are exact.
- `bool` constants are 0 or 1. `ptr` constants are `RawProcessValue` unless
  audited ([07 §5](07-serialization.md)); addresses of functions and other
  symbols are `pointers` entries, not constants.
- Constants are interned per module by `(type, provenance, bits, bytes)`.
  A canonical image contains no duplicates and no unused entries.

### 4.3 Blocks

```c
struct BlockRecord {     // 16 bytes
	uint32_t firstInst;  // function-local id of the first instruction
	uint32_t instCount;  // including block arguments and the terminator
	uint16_t argCount;   // the first argCount instructions are blockarg
	uint16_t flags;      // bit 0: landing pad; other bits 0
	uint32_t pad;        // 0
};
```

Loop depth, loop end and loop-header marks are derived data.

### 4.4 Functions

```c
struct FunctionRecord {  // 40 bytes
	uint32_t nameId;     // in names
	uint32_t signature;  // in signatures
	uint32_t firstInst, instCount;    // range in insts
	uint32_t firstBlock, blockCount;  // range in blocks
	uint32_t firstAlloca, allocaCount;// range in allocas
	uint32_t attrs;      // §4.5 attribute bits
	uint8_t  level;      // 0 Trace, 1 SSA
	uint8_t  exported;   // 1 if callable from outside the module
	uint16_t pad;        // 0
};

struct SignatureRecord { // 12 bytes
	uint32_t result;     // TypeId of the result (0 = void)
	uint32_t paramCount;
	uint32_t paramTypes; // offset in extra: paramCount TypeIds, one per word
};
```

The entry block's block arguments are the function's parameters, in order,
with the signature's parameter types.

### 4.5 Callees

Every direct call goes through the callee table.

```c
struct CalleeRecord {    // 16 bytes
	uint8_t  kind;       // 0 internal function, 1 external native function
	uint8_t  pad[3];     // 0
	uint32_t target;     // function index (kind 0) or pointers index (kind 1)
	uint32_t signature;
	uint32_t attrs;      // attribute bits below
};
```

Attribute bits, shared by `FunctionRecord.attrs` and `CalleeRecord.attrs`:

| Bits | Meaning |
|---|---|
| 0–1 | memory effects: 0 `ModRef`, 1 `argmemonly`, 2 `readonly`, 3 `pure` ([06 §3.3](06-semantics.md)) |
| 2 | `noUnwind`: never throws (`noexcept`, `nothrow` or inferred) |
| 3 | `willReturn` |
| 4 | `declared`: bits 0–3 come from a user declaration, not inference |
| 5–31 | reserved, 0 |

### 4.6 Pointers

```c
struct PointerRecord {   // 24 bytes
	uint8_t  kind;       // 0 callee, 1 capture thunk, 2 destructor, 3 constant address, 4 extension payload
	uint8_t  mode;       // 0 ProcessLocal, 1 Portable
	uint16_t pad;        // 0
	uint32_t nameId;     // symbol name (Portable), else 0
	uint64_t address;    // ProcessLocal: the address, guarded by processToken
	uint64_t processToken; // ProcessLocal: token of the producing process; Portable: 0
};
```

A `ProcessLocal` pointer is valid only in the process whose token it
carries; the loader rejects it elsewhere. A `Portable` pointer is resolved
by name at load ([07 §3](07-serialization.md)).

### 4.7 Runtime bindings

```c
struct BindingRecord {   // 16 bytes
	uint32_t nameId;     // stable binding symbol
	uint32_t type;       // TypeId of the bound object (16+: extension type)
	uint32_t size, align;
};
```

`bind.addr` (§5.8) yields the address supplied for the binding at load
([07 §4](07-serialization.md)).

### 4.8 Debug data (optional)

The `debug` section maps instructions to source locations and holds the
region table ([02 §3.4](02-nir.md)). It is present only when a debug or
profiling option was set ([05 §7](05-backends.md)). Its encoding is defined
in M1 and versioned with feature bit `DEBUG_V1`. Consumers must work without it.

## 5. Operations

Notation: `a`, `b`, `c` are the record fields; *v* marks a value reference,
*blk* a block id, *x* an `extra` offset. "Integer" means `i8`…`i64`;
"float" means `f32`/`f64`. Unless stated, operands have the result type.

### 5.1 Arithmetic

| Op | Code | Operands | Types | Flags | Semantics |
|---|---|---|---|---|---|
| `add` | 1 | a, b: *v* | integer, float | — | integers wrap modulo 2ⁿ; floats IEEE |
| `sub` | 2 | a, b: *v* | integer, float | — | as `add` |
| `mul` | 3 | a, b: *v* | integer, float | — | as `add` |
| `div` | 4 | a, b: *v*; c: unwind entry (checked only) | integer, float | bit 0 unsigned; bit 1 checked | §5.10 |
| `rem` | 5 | a, b: *v*; c: unwind entry (checked only) | integer, float | bit 0 unsigned; bit 1 checked | §5.10; float `rem` is C++ `fmod` |
| `fneg` | 6 | a: *v* | float | — | flips the sign bit, including of NaN and zero |

### 5.2 Logical (on `bool`)

| Op | Code | Operands | Semantics |
|---|---|---|---|
| `and` | 8 | a, b: *v* | both operands are already evaluated; no short circuit |
| `or` | 9 | a, b: *v* | as `and` |
| `not` | 10 | a: *v* | |

### 5.3 Bitwise (on integers)

| Op | Code | Operands | Flags | Semantics |
|---|---|---|---|---|
| `band` | 12 | a, b: *v* | — | |
| `bor` | 13 | a, b: *v* | — | |
| `bxor` | 14 | a, b: *v* | — | |
| `bnot` | 15 | a: *v* | — | bitwise complement (0.3's integer `NegateOp`) |
| `shl` | 16 | a: value, b: amount (*v*, same type) | — | amount masked to `width − 1` (D10) |
| `shr` | 17 | a: value, b: amount | bit 0 unsigned (logical), else arithmetic | amount masked as `shl` |

### 5.4 Compare and select

| Op | Code | Operands | Result | Flags |
|---|---|---|---|---|
| `cmp` | 20 | a, b: *v*, same type | `bool` | bits 0–2 predicate: 0 `eq`, 1 `ne`, 2 `lt`, 3 `le`, 4 `gt`, 5 `ge`; bit 3 unsigned (integers) |
| `select` | 21 | a: `bool` condition; b, c: *v* | type of b and c | — |

Float comparisons follow C++: with a NaN operand, `ne` is true and every
other predicate is false. `ptr` compares as unsigned 64-bit.

### 5.5 Casts

| Op | Code | Operands | Flags: kind (bits 0–3) |
|---|---|---|---|
| `cast` | 24 | a: *v*; result type in `type` | 0 `sext`, 1 `zext`, 2 `trunc`, 3 `sitofp`, 4 `uitofp`, 5 `fptosi`, 6 `fptoui`, 7 `fpext`, 8 `fptrunc`, 9 `bitcast`, 10 `ptrtoint`, 11 `inttoptr` |

- `zext` from `bool` gives 0 or 1; `trunc` to `bool` is not allowed (use
  `cmp ne 0`).
- `fptosi`/`fptoui` **saturate**: NaN gives 0, values beyond the range give
  the minimum or maximum (D11).
- `bitcast` requires equal sizes. `ptrtoint`/`inttoptr` use `i64`.

### 5.6 Memory

| Op | Code | Operands | Result | Flags |
|---|---|---|---|---|
| `load` | 28 | a: `ptr` | loaded type | bits 0–3: log₂ of the alignment, 0 = natural |
| `store` | 29 | a: `ptr`; b: value | `void` | as `load` |
| `alloca` | 30 | a: index into the function's alloca range | `ptr` | — |

Memory safety is the program's responsibility, as in C++: an access through
an invalid or insufficiently aligned address is undefined. An `alloca`
lives until the function returns or unwinds.

### 5.7 Calls

| Op | Code | Operands | Result |
|---|---|---|---|
| `call` | 32 | a: callee index (§4.5); c: *x* → `[argc, arg₀ … argₙ₋₁, unwind]` | the callee signature's result |
| `call.indirect` | 33 | a: `ptr` callee (*v*); b: signature index; c: *x* as `call` | the signature's result |
| `funcaddr` | 34 | a: callee index | `ptr` |

- Arguments are value references and must match the signature's parameter
  types.
- The unwind entry (§3.4) must be `NO_UNWIND` if the callee is `noUnwind`.
- A callee's effects come from its `attrs`. A `pure` or `readonly` call
  with `NO_UNWIND` has no side effects the IR can observe and may be merged
  or hoisted ([03](03-conversion-and-passes.md)).

### 5.8 Runtime state

| Op | Code | Operands | Result |
|---|---|---|---|
| `bind.addr` | 36 | a: binding index (§4.7) | `ptr` |

### 5.9 Values and control flow

| Op | Code | Operands | Result | Notes |
|---|---|---|---|---|
| `blockarg` | 40 | a: argument position | any non-`void` type | only at the start of a block (§4.3) |
| `br` | 48 | a: target *blk*; c: *x* → `[argc, args…]` | `void` | terminator |
| `condbr` | 49 | a: `bool` condition; b: true *blk*; c: *x* → `[prob, trueArgc, trueArgs…, false blk, falseArgc, falseArgs…]` | `void` | terminator; `prob` is P(true) × 2³² − 1, `0xFFFFFFFF` unknown |
| `ret` | 50 | a: value or `NONE` for `void` | `void` | terminator |
| `resume` | 51 | — | `void` | terminator of a landing pad: continues unwinding |
| `assign` | 56 | a: variable (value id); b: new value | `void` | **Trace level only** (§6) |

Codes 1–255 not listed are reserved for core opcodes added in later minor
versions (§8.2).

### 5.10 Division and remainder

- **Floats:** IEEE; division by zero gives ±∞ or NaN; never throws; the
  checked bit must be 0.
- **Integers**, with bit 0 selecting unsigned or signed:
  - if the divisor is 0, or (signed) the dividend is the minimum value and
    the divisor is −1, the instruction **throws
    `nautilus::ArithmeticException`** (D10);
  - otherwise the quotient rounds toward zero, and `rem` has the dividend's
    sign, as in C++.
- **Checked bit.** An integer `div`/`rem` whose divisor is not proven safe
  must set bit 1 and carry an unwind entry in `c` (`NO_CLEANUP` or a landing
  pad). If the producer proves the divisor safe (a non-zero constant, and
  not −1 for signed), it clears bit 1, `c` is 0, and the instruction cannot
  throw.

### 5.11 Extension operations

An extension instruction (`op` ≥ 256) uses the same record. Its operands,
types, flags and `extra` payload are defined by its `OpInfo`
([04 §3.3](04-extensions.md)) and checked by its `verify` hook. It produces
at most one value; an operation with several results returns an extension
aggregate type or is split into several operations. Extension operations
are never terminators.

## 6. Levels and invariants

### 6.1 Both levels

1. Every block ends in exactly one terminator, and terminators appear
   nowhere else.
2. `blockarg` instructions occur only as the first `argCount` instructions
   of their block, positions 0 … `argCount − 1` in order.
3. Every value reference points to an instruction with a non-`void` result
   in the same function, or to an existing constant.
4. Branch targets and argument counts match the target block's `argCount`
   and argument types.
5. Operand and result types follow §5.
6. A landing-pad block (flag bit 0) is entered only through unwind entries,
   never through `br`/`condbr`; it ends in `resume` or in a `br` to another
   landing pad.
7. Every table index (callee, signature, alloca, binding, pointer, name,
   extension) is in range.

### 6.2 Trace level

- `assign` may redefine a variable: a later use of the variable's id sees
  the most recent `assign` along the executed path.
- Blocks are in recording order; ids are stable while tracing splits
  blocks.
- Snapshots, the tag map and the replay cursor are derived data
  ([01 §6](01-tracing-frontend.md)).
- Trace-level functions are never executed and never compiled by a backend.

### 6.3 SSA level

- No `assign`. Every value is defined exactly once.
- **Dominance:** every use of a value is dominated by its definition; a use
  as a branch argument counts at the end of the predecessor block.
- **Layout:** the entry block is block 0; the other non-pad blocks follow
  in reverse post-order with each loop body contiguous; landing pads come
  after all other blocks.
- Unreachable blocks are not present.

### 6.4 The verifier

The verifier checks §3, §4, §5 and the invariants of the function's level,
and extension instructions against their `Signature` and `verify` hook. It
runs on every module from a deserializer or the C API, and in debug builds
after every producer. A module that fails verification is never passed to
a backend.

## 7. Canonical form

A **canonical** module is the form that is serialized and hashed
([02 §3.6](02-nir.md), [08](08-caching.md)). Canonicalization:

1. applies pending replacements and compacts each function, so ids are
   dense and in layout order;
2. drops unused constants, names, callees, signatures, pointers and
   bindings, and orders each table by first use in instruction order;
3. interns names, constants and signatures (no duplicates);
4. numbers extension opcodes and types image-locally in first-use order;
5. writes `extra` lists in instruction order, without gaps;
6. zeroes all padding and unused fields.

Two structurally equal modules have byte-identical canonical images. The
structural hash (L2 key) covers the canonical sections plus the effective
options and extension versions; it excludes `debug`.

## 8. Binary image and versioning

### 8.1 Layout

```c
struct ImageHeader {          // 48 bytes, at offset 0
	char     magic[4];        // "NIR\0"
	uint16_t major, minor;    // format version (this spec: 1.0)
	uint32_t compatFeatures;  // bits a reader may ignore
	uint32_t incompatFeatures;// bits a reader must understand
	uint64_t totalSize;       // bytes, including the header
	uint32_t sectionCount;
	uint32_t crc32c;          // CRC-32C of bytes [48, totalSize)
	uint8_t  reserved[16];    // 0
};

struct SectionEntry {         // 24 bytes, directly after the header
	uint32_t kind;            // §4.1
	uint32_t stride;          // record size in bytes
	uint64_t offset;          // from the image start, 8-byte aligned
	uint64_t count;           // number of records
};
```

- Sections appear in increasing `kind` order, without overlap, each padded
  with zeros to 8 bytes.
- A section's `stride` may be larger than this spec's record size. Readers
  use the fields they know and skip the rest (§8.2).
- The loader validates the magic, the version, `totalSize`, the checksum,
  section bounds, strides and every reference, then runs the verifier. A
  failed check rejects the image; it never causes undefined behavior.

### 8.2 Versioning

| Change | Version |
|---|---|
| New core opcode, new optional section, new attribute or flag bit, wider stride (fields appended) | minor; old readers reject images that use new opcodes, ignore the rest |
| New canonical section a reader must understand | minor plus an `incompatFeatures` bit |
| Changed meaning or encoding of an existing field, opcode or rule | major |

- Within 0.4.x, images stay loadable (D12): patch releases do not change
  the format, and minor versions only add.
- Opcodes and section kinds are never renumbered within a major version;
  the C API's enums map one to one ([09](09-c-api.md)).

### 8.3 Feature bits (1.0)

| Bit | Name | Class | Meaning |
|---|---|---|---|
| 0 | `DEBUG_V1` | compat | the `debug` section is present |
| 1 | `EXTENSIONS` | incompat | the module uses extension ops or types |
| 2 | `PROCESS_LOCAL` | incompat | the module contains `ProcessLocal` pointers |

### 8.4 Extensions in images

- The `extensions` section lists each extension op (`kind` 0) and type
  (`kind` 1) used by the module as `name@version`, in image-local order:
  image opcode 256 + *i* is the *i*-th op entry, image type 16 + *j* the
  *j*-th type entry.
- **Loading maps them to process ids** with one pass over `insts` and the
  type fields. An image without extensions is used in place; an image with
  extensions is rewritten once while loading.
- A missing extension, or a version the registered extension does not
  accept, rejects the image.

## 9. Text form

The printer writes a text form for dumps, tests and bug reports. It is
informative and not parsed in 0.4.

```
nir 1.0
func @divideFirst(%0: ptr, %1: i64) -> i64 {
^b0(%0: ptr, %1: i64):
  %2 = cmp.lt.s %1, 0:i64
  condbr %2, ^b1(), ^b2(%1) p=0.1
^b1:
  ret 0:i64
^b2(%3: i64):
  %4 = load.i64 %0
  %5 = div.s.checked %4, %3 unwind(none)
  ret %5
}
```

Values print as `%id`, constants as `value:type`, blocks as `^bN`,
extension ops by name (`simd.add`), unwind entries as `unwind(none)`,
`unwind(^bN)` or nothing for `NO_UNWIND`.

## 10. Open items before M1 exit

- The encoding of the `debug` section (§4.8).
- Whether an `unreachable` terminator is needed (for code after a call that
  never returns), or `ret` of an undefined value suffices.
- Atomic memory operations are not core in 1.0. NebulaStream uses atomics
  through `invoke` today; if that becomes a measured cost, atomics come as
  core opcodes in 1.1.
- Fast-math flags: bits are reserved, unused in 1.0 (D11).
