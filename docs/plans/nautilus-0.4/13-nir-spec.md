# NIR Specification

Part of the [Nautilus 0.4 design](README.md) · Format version **1.0** ·
Status: **release candidate**; final at the M1 exit, once the verifier, the
serializer and the round-trip tests implement it unchanged (§11)

This document is the normative definition of NIR: its data model, types,
binary encoding, operations and their semantics, the invariants of each
level, canonical form, and versioning. Every producer (the tracer, the
conversion, passes, the C API, the deserializer) and every consumer
(backends, the serializer, caches, tools) must follow it.
[02-nir.md](02-nir.md) explains the design and why it is this way; where
the two differ, this document wins.

**Conventions.** "Must" and "must not" are requirements: a module that
violates one is *invalid*, and the verifier rejects it. "Undefined" means
the program's behavior is not constrained, as in C++. All integers in the
binary encoding are little-endian. Sizes and offsets are in bytes unless
stated otherwise. Ranges are written `[first, first + count)`.

**Targets.** NIR 1.0 is defined for little-endian targets with 64-bit
pointers (Linux x86-64 and AArch64, macOS AArch64 in 0.4). Other targets
need a new major version or a conversion step.

## 1. Data model

- A **module** is the unit of compilation, loading, caching and
  serialization (§4.4). It has a name, **exports** (its exported functions),
  **imports** (functions it uses from other modules), and the tables its
  functions share: constants, read-only data objects, names, signatures,
  callees, allocas, pointers, bindings and extensions.
- A **function** is defined in a module: a name, a signature, attributes,
  a visibility, a level and a list of **blocks** (§4.5). Each block holds
  **instructions**; the last instruction of a block, and only that one, is
  a terminator.
- A **call target** (callee) is one of four kinds (§4.6): a function of the
  same module (*internal*), a function exported by another module
  (*imported*), a native function of the host process (*external*), or a
  function registered by a plugin and known by name (*intrinsic*).
- An instruction produces **at most one value**. A value's **id** is the
  function-local index of its instruction, starting at 0.
- A **block id** is the function-local index of the block. Block 0 is the
  entry block.
- **Constants** are not instructions. They live in the module's constant
  pool and are referenced from operands (§3.2).
- **Data objects** are read-only byte arrays owned by the module, with an
  address (§4.10). They hold constant tables and strings.
- Every function has a **level**, `PreSSA` or `SSA` (§6). Both levels use
  the same records; they differ in invariants and in how blocks list their
  instructions (§4.3).

## 2. Types

A type is a `u8` `TypeId`.

| Id | Name | Size, alignment | Meaning |
|---|---|---|---|
| 0 | `void` | — | no value; the type of instructions without a result |
| 1 | `bool` | 1, 1 | false or true; stored in memory as one byte, 0 or 1 |
| 2 | `i8` | 1, 1 | 8-bit integer, signless |
| 3 | `i16` | 2, 2 | 16-bit integer, signless |
| 4 | `i32` | 4, 4 | 32-bit integer, signless |
| 5 | `i64` | 8, 8 | 64-bit integer, signless |
| 6 | `f32` | 4, 4 | IEEE 754 binary32 |
| 7 | `f64` | 8, 8 | IEEE 754 binary64 |
| 8 | `ptr` | 8, 8 | address; 64 bits on every 0.4 target |
| 9–15 | — | — | reserved for core |
| 16–255 | extension | from `TypeInfo` | registered types ([04 §3.2](04-extensions.md)) |

- **Integers are signless** ([06 §4](06-semantics.md)). Operations whose
  result depends on signedness select it with a flag (§5). 0.3's
  `ui8`…`ui64` map to `i8`…`i64`.
- "Integer" below means `i8`…`i64`; "float" means `f32` or `f64`;
  "scalar" means any core type except `void`.
- **Extension type ids are per process in memory.** In a serialized image
  they are image-local and named in the `extensions` section (§8.4).
- **Core operations accept extension types only where stated** (`select`,
  `load`, `store`, `blockarg`, branch arguments, call arguments and results,
  `ret`). Everything else on an extension type goes through extension
  operations.

## 3. Instructions and operands

### 3.1 Instruction record

```c
struct Inst {          // 16 bytes, 4-byte aligned
	uint16_t op;       // opcode (§5); 0 is invalid
	uint8_t  type;     // result TypeId; 0 (void) if there is no result
	uint8_t  flags;    // opcode-specific (§5)
	uint32_t a, b, c;  // operands; meaning per opcode (§5)
};
```

- Opcodes below 256 are core (§5). Opcodes from 256 up are extension
  operations, per process in memory and image-local in an image (§8.4).
- Unused operand fields must be 0, and unused or reserved flag bits must
  be 0.

### 3.2 Value references

An operand that holds a value is a 32-bit **value reference**:

| Bit pattern | Meaning |
|---|---|
| `0x00000000`–`0x7FFFFFFE` | value id within the same function |
| `0x80000000`–`0xFFFFFFFD` | constant: index `ref & 0x7FFFFFFF` in the constant pool |
| `0x7FFFFFFF`, `0xFFFFFFFE` | reserved; must not occur |
| `0xFFFFFFFF` | `NONE`: no value, only where an opcode allows it |

A function holds at most 2³¹ − 1 instructions, and a module at most
2³¹ − 3 constants. Operands that are not value references (block ids,
table indices, `extra` offsets) are plain `u32` indices; §5 names them.

### 3.3 The `extra` array

Variable-length operand lists live in the module's `extra` section, an
array of `u32` words. An instruction refers to its list by the word offset
in `c`. Each list belongs to exactly one instruction; lists do not
overlap.

### 3.4 Unwind entries

Instructions that may throw (calls, checked division; §5) carry an **unwind
entry**, one `u32`:

| Level | Value | Meaning |
|---|---|---|
| both | `0xFFFFFFFF` (`NO_UNWIND`) | the instruction cannot throw |
| SSA | `0xFFFFFFFE` (`NO_CLEANUP`) | may throw; nothing to clean up; the exception propagates to the caller |
| SSA | block id | may throw; on an exception, control enters this landing-pad block |
| Pre-SSA | `extra` offset | may throw; the offset points to a **cleanup list** `[n, (destructor₀, object₀), …]`: `n` pairs of a `pointers` index (kind destructor) and a value reference to the object's address, in destruction order. `n` = 0 means no cleanup |

The conversion turns cleanup lists into shared landing pads
([03 §3](03-conversion-and-passes.md)).

## 4. Module structure

### 4.1 Sections

A module is a set of sections. **Canonical** sections are serialized,
hashed and versioned. **Optional** sections are serialized when present
and do not affect the structural hash. **Derived** data (use counts,
replacement tables, loop information, CSE tables, snapshots) is never part
of an image ([02 §3.6](02-nir.md)).

| Kind | Section | Record | Class |
|---|---|---|---|
| 1 | `insts` | `Inst`, 16 B | canonical |
| 2 | `extra` | `u32` | canonical |
| 3 | `consts` | `ConstRecord`, 16 B (§4.2) | canonical |
| 4 | `constBytes` | bytes, for constants wider than 8 bytes | canonical |
| 5 | `blocks` | `BlockRecord`, 16 B (§4.3) | canonical |
| 6 | `functions` | `FunctionRecord`, 40 B (§4.5) | canonical |
| 7 | `signatures` | `SignatureRecord`, 12 B (§4.5) | canonical |
| 8 | `callees` | `CalleeRecord`, 16 B (§4.6) | canonical |
| 9 | `allocas` | `AllocaRecord`, 8 B (§4.5) | canonical |
| 10 | `pointers` | `PointerRecord`, 24 B (§4.7) | canonical |
| 11 | `bindings` | `BindingRecord`, 16 B (§4.8) | canonical |
| 12 | `names` | `{u32 offset, u32 length}` into `nameBytes`, 8 B | canonical |
| 13 | `nameBytes` | UTF-8 bytes, not NUL-terminated | canonical |
| 14 | `extensions` | `ExtensionRecord`, 8 B: extension operations, types and intrinsics (§8.4) | canonical |
| 15 | `debugLocations` | `{u32 file (index in debugNames), u32 line, u32 column}`, 12 B | optional |
| 16 | `debugInstLocations` | `u32` per instruction, parallel to `insts`: index in `debugLocations`, or `0xFFFFFFFF` | optional |
| 17 | `debugRegions` | `RegionRecord`, 16 B (§4.9) | optional |
| 18 | `debugInstRegions` | `u32` per instruction, parallel to `insts`: index in `debugRegions`, or `0xFFFFFFFF` | optional |
| 19 | `module` | `ModuleRecord`, 16 B, exactly one (§4.4) | canonical; the name is excluded from the structural hash (§7) |
| 20 | `imports` | `ImportRecord`, 16 B (§4.4) | canonical in memory; always empty in a serialized image (§8.5) |
| 21 | `debugNames` | `{u32 offset, u32 length}` into `debugNameBytes`, 8 B | optional |
| 22 | `debugNameBytes` | UTF-8 bytes, not NUL-terminated | optional |
| 23 | `data` | `DataRecord`, 16 B (§4.10) | canonical |
| 24 | `dataBytes` | the contents of the data objects | canonical |
| 25–1023 | — | reserved for later versions | — |
| ≥ 1024 | — | vendor and experimental sections; ignored by readers that do not know them | optional |

The instructions of one function are contiguous in `insts`; so are its
blocks in `blocks` and its allocas in `allocas`. Functions are laid out in
`functions` order.

### 4.2 Constants

```c
struct ConstRecord {     // 16 bytes
	uint8_t  type;       // TypeId; not void
	uint8_t  provenance; // 0 Literal, 1 AuditedInvariant, 2 RawProcessValue (07 §5)
	uint16_t pad;        // 0
	uint32_t byteOffset; // into constBytes for types wider than 8 bytes, else 0
	uint64_t bits;       // the bit pattern, zero-extended; 0 if byteOffset is used
};
```

- Floats are stored by bit pattern, so NaN payloads and `-0.0` are exact.
- `bool` constants are 0 or 1.
- A `ptr` constant is the null pointer (`Literal`, bits 0) or a traced
  address (`RawProcessValue` unless audited). Addresses of functions,
  thunks and destructors are `pointers` entries, not constants.
- An extension-type constant has `size` bytes (from its `TypeInfo`) at
  `byteOffset` if wider than 8, else in `bits`.
- Constants are interned per module by `(type, provenance, value bytes)`.

### 4.3 Blocks

```c
struct BlockRecord {     // 16 bytes
	uint32_t first;      // SSA: function-local id of the first instruction
	                     // PreSSA: extra offset of the block's op-id list
	uint32_t count;      // number of instructions, including blockargs and the terminator
	uint16_t argCount;   // SSA: the first argCount instructions are blockarg; PreSSA: 0
	uint16_t flags;      // bit 0: landing pad (SSA only); other bits 0
	uint32_t pad;        // 0
};
```

- **SSA level:** a block is the contiguous range `[first, first + count)`
  of the function's instructions, and blocks are contiguous and in layout
  order, so the function's instructions are exactly its blocks in order.
- **Pre-SSA level:** instructions are in recording order, and a block lists
  its instructions' ids in `extra[first … first + count)`. This lets
  tracing split a block without moving instructions or changing ids
  ([01 §6](01-tracing-frontend.md)). Every instruction belongs to exactly
  one block.

### 4.4 Modules

```c
struct ModuleRecord {    // 16 bytes; exactly one per module (section 19)
	uint32_t nameId;     // module name; 0xFFFFFFFF for an anonymous module
	uint32_t flags;      // reserved, 0
	uint64_t reserved;   // 0
};

struct ImportRecord {    // 16 bytes (section 20)
	uint32_t moduleNameId;   // name of the exporting module
	uint32_t functionNameId; // name of the exported function
	uint32_t signature;      // the signature the importer was built against
	uint32_t attrs;          // the attributes the importer assumed (§4.6)
};
```

- **Name.** A module that other modules import from must have a name. Names
  are UTF-8, unique among the modules linked together (§4.4.2), and
  conventionally dot-separated (`nes.query42.pipeline3`). The name is not
  part of the structural hash (§7), so equal modules under different names
  share cache entries.
- **Exports** are the module's functions with visibility `exported`
  (§4.5). An export is identified by `(module name, function name)`; its
  signature and attributes are part of its interface.
- **Imports** are the functions the module calls in other modules, listed
  in the `imports` section and called through callees of kind `imported`
  (§4.6). An import names the exporter by module and function name, never by
  address.
- **Imports are always references.** Building or linking a module never
  copies an exporter's body into the importer. Bodies are copied only by
  inlining (§4.4.3), an optional optimization, and when an image is
  exported, which bundles every import (§8.5).
- **A registered name refers to one module.** While a module is registered
  under a name, registering another module under the same name is an error.
  Promotion to another tier does not change a module, so an exporter's
  semantics never change while an importer is linked to it.

#### 4.4.1 Lifecycle

A module is in one of four states:

| State | Meaning |
|---|---|
| **built** | produced by the tracer, the C API or a deserializer; verified; not executable |
| **linked** | every import is resolved against a loaded exporter (§4.4.2) |
| **compiled** | an executable exists for at least one backend; may be promoted to another tier ([11](11-tiered-compilation.md)) |
| **released** | no handle, importer or running call refers to it; its code and its `funcaddr` stubs are freed. Calling a stub of a released module is undefined |

#### 4.4.2 Linking imports

Linking happens when a module is compiled or loaded, against the
**module registry** of its engine: the set of compiled modules with a name.

1. **Resolve.** Each import is looked up by `(moduleNameId, functionNameId)`.
   The exporter must be compiled and the function exported.
2. **Check the signature.** The export's signature must equal the import's
   signature exactly (types, count, result).
3. **Check the attributes.** The export must guarantee everything the
   importer assumed: `(import.attrs & ~export.attrs & GUARANTEE_MASK) == 0`
   (§4.6.2). An importer that assumes nothing has no guarantee bits set,
   which every export satisfies.
4. **Bind.** The importer gets one **import slot** per import: a pointer to
   the exporter's current entry point. Calls go through the slot (§5.7).
5. **Pin.** The importer holds a reference to the exporter, so an exporter
   is not released while an importer exists.

Any failure is a **link error**: the compile or load fails with a message
naming the import and the reason (missing module, missing or non-exported
function, signature mismatch, attributes not implied, cycle). No partial
linking.

- **The import graph is acyclic.** A module cannot import, directly or
  transitively, from itself. Mutually recursive functions belong in one
  module.
- **Tier changes are transparent.** When an exporter is promoted
  ([11](11-tiered-compilation.md)), the engine updates the import slots of
  its importers with a release store. Calls already running finish in the
  old code, which stays alive as long as it may be executing (as for
  `ModuleFunction`, #506).
- **Importers see only the interface,** unless they inline (§4.4.3). Without
  inlining, an importer's code depends on the export's signature and
  attributes, never on its body, so promoting the exporter never
  invalidates an importer.
- **Assumed attributes.** When an importer is built while its exporter is
  already compiled (always the case for the C++ frontend, since the graph is
  acyclic), the builder copies the export's attributes into the import
  record. A producer that builds against an interface it cannot see (the C
  API, a deserialized image) states the attributes itself, or `ModRef` with
  no flags.
- **Exceptions** cross module boundaries like any call: the import's
  `noUnwind` decides whether the call site needs an unwind entry, and the
  engine bridges the exporter's and the importer's exception-propagation
  modes in the import slot's entry point when they differ.

#### 4.4.3 Inlining

Inlining is a separate, optional NIR→NIR pass at the SSA level
([03 §5](03-conversion-and-passes.md)). It is always possible, because every
compiled module keeps its canonical NIR ([07 §2](07-serialization.md)):

- **What can be inlined:** callees of kind `internal`, and of kind
  `imported`, whose body is taken from the exporter's SSA-level NIR. External
  callees are native code and cannot be inlined; intrinsics are lowered by
  backends, not inlined.
- **Bindings stay with their module.** A function whose body, or whose
  inlined callees, contain `bind.addr` is inlined only within its own
  module. Across modules it is called, so its bindings resolve against its
  own module.
- **Semantics do not change.** An inlined call behaves exactly as the call.
  Since a registered name refers to one module (§4.4), an inlined export
  never goes stale while the importer is linked.
- **Compiled code records what it inlined.** An executable that inlined an
  imported function records `(exporter name, exporter structural hash)`. A
  cache key for compiled code includes these hashes
  ([08](08-caching.md)), so a different exporter never reuses the entry.
- **Control:** the option `ir.inline` (off by default on tier 0, decided per
  tier in M3) and a size limit `ir.inline.maxInsts`. Plugins and the C API
  can request inlining per module.

#### 4.4.4 Host API (informative)

```cpp
auto math = engine.createModule("lib.math");
math.registerFunction("hash", hashFn);            // exported
auto lib = math.compile();                        // registered as "lib.math"

auto q = engine.createModule("nes.query42");
auto hash = q.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
q.registerFunction("run", [&](val<uint64_t*> p, val<int64_t> n) {
	...
	auto h = hash(p[i]);                           // traced as a call to an imported callee
	...
});
auto compiled = q.compile();                      // links "lib.math"."hash"
```

`importFunction` checks the signature against the registry when the
exporter is already compiled, and the link step (§4.4.2) checks it again.
The C API has the same operations ([09](09-c-api.md)).

### 4.5 Functions

```c
struct FunctionRecord {  // 40 bytes
	uint32_t nameId;     // in names; unique within the module
	uint32_t signature;  // in signatures
	uint32_t firstInst, instCount;     // range in insts
	uint32_t firstBlock, blockCount;   // range in blocks
	uint32_t firstAlloca, allocaCount; // range in allocas
	uint32_t attrs;      // attribute bits (§4.6)
	uint8_t  level;      // 0 PreSSA, 1 SSA
	uint8_t  visibility; // 0 internal, 1 exported
	uint16_t pad;        // 0
};

struct SignatureRecord { // 12 bytes
	uint32_t result;     // TypeId of the result; 0 = void
	uint32_t paramCount;
	uint32_t paramTypes; // extra offset: paramCount TypeIds, one per word
};

struct AllocaRecord {    // 8 bytes
	uint32_t size;       // > 0
	uint32_t align;      // a power of two
};
```

- **Every function in a module is a definition**, with a body. Declarations
  of functions defined elsewhere are callees (§4.6), not functions.
- **Visibility.**
  - `exported`: part of the module's interface, callable from the host
    (`CompiledModule::getFunction`) and from importing modules. Its name and
    signature are fixed by the user; it is never removed.
  - `internal`: callable only from the same module. Unreferenced internal
    functions are removed by canonicalization (§7), and a backend may
    inline them.
  - In C++, functions registered on a module (`registerFunction`) are
    exported; `nautilus::function` callees traced into the module are
    internal.
- **Calling convention.**
  - An exported function's entry point uses the platform's C calling
    convention for its signature, with each NIR type passed as the
    corresponding C type (`i64` as `int64_t`, `ptr` as `void*`, `bool` as
    `bool`). An extension type may appear in an exported signature only if
    its `TypeInfo` defines a C ABI type for it (for example `__m256` for
    `simd.vec<f32,8>`); the verifier rejects others. The host calls it through
    a plain function pointer (or the `Invocable` wrapper when the backend's
    exception mode needs one, as in 0.3).
  - Calls between internal functions may use any convention the backend
    chooses.
- **Parameters.** The entry block's parameters are the function's
  parameters: at the SSA level its leading `blockarg`s, at the pre-SSA level
  the first `paramCount` instructions, which are `blockarg`s.
- **Signatures** have no variadic form and pass only core scalars and
  extension types; aggregates are passed by pointer, as in 0.3.
- **Attributes** (§4.6) are inferred by the conversion, or declared, and
  for exported functions become part of the interface.

### 4.6 Callees and attributes

Every direct call and every `funcaddr` goes through the callee table.

```c
struct CalleeRecord {    // 16 bytes
	uint8_t  kind;       // see below
	uint8_t  pad[3];     // 0
	uint32_t target;     // depends on kind
	uint32_t signature;
	uint32_t attrs;      // attribute bits below
};
```

| Kind | Name | `target` | What is called | Portable in an image |
|---|---|---|---|---|
| 0 | **internal** | function index | a function defined in this module | yes |
| 1 | **external** | `pointers` index, kind callee | a native C/C++ function of the host process, called with `invoke` in C++ | only if the pointer is `Portable` (§4.7) |
| 2 | **imported** | `imports` index | an exported function of another module, through its import slot (§4.4.2) | yes, by name |
| 3 | **intrinsic** | `extensions` index of kind intrinsic | a plugin function registered by name, with a native fallback (§4.6.1) | yes, by name |

- `signature` must equal the target's: the function's signature (kind 0),
  the import's (kind 2), the registered intrinsic's (kind 3), or the
  traced C++ function type (kind 1).
- `attrs` are what calls through this callee may assume. For kind 0 they
  equal the function's attributes; for kind 2 they are the import's
  assumed attributes; for kinds 1 and 3 they come from the registration
  (`noexcept`, `nothrow`, declared effects, [06 §3](06-semantics.md)).

#### 4.6.1 Intrinsics

An intrinsic is a function that a plugin registers by name in the
`ExtensionRegistry` ([04 §3.7](04-extensions.md)), with:
- a name and version (`builtin.prefetch@1`) and a `minCompatibleVersion`;
- a signature and attributes;
- a **native fallback**: a C function with exactly that signature, which
  defines the intrinsic's semantics;
- optional per-backend lowerings that replace the call with inline code.

Rules:
- **The fallback defines the semantics.** A lowering must behave exactly as
  a call to the fallback (the invariant of 0.3's intrinsics). A backend
  without a lowering calls the fallback.
- **Identified by name, not address.** An image refers to an intrinsic
  through the `extensions` section, so it is portable; the loader resolves
  it in the receiving process's registry. A missing intrinsic, an
  incompatible version, or a callee signature that differs from the
  registered signature rejects the image (§8.4).
- **Intrinsic or extension operation?** Both are plugin-defined. An
  intrinsic keeps call semantics: operands are a signature's arguments, it
  may throw (with an unwind entry) and its effects are call attributes. An
  extension operation (§5.11) is a value-level instruction: it can take
  extension types, fold, simplify and join CSE through its traits, and
  never throws. 0.3's `IntrinsicRegistry` entries become intrinsics
  unchanged; plugins move operations that benefit from folding or extension
  types to extension operations ([04 §4](04-extensions.md)).

#### 4.6.2 Attribute bits

Attributes are **guarantees**: each set bit promises something, and a clear
bit promises nothing. Zero is therefore always the safe default, and one set
of attributes implies another exactly when it has every guarantee bit the
other has. Used by `FunctionRecord.attrs`, `CalleeRecord.attrs` and
`ImportRecord.attrs`:

| Bit | Name | Guarantee |
|---|---|---|
| 0 | `noArgRead` | never reads memory reachable through its pointer arguments |
| 1 | `noArgWrite` | never writes memory reachable through its pointer arguments |
| 2 | `noOtherRead` | never reads any other memory: globals, the heap through other pointers, runtime bindings |
| 3 | `noOtherWrite` | never writes any other memory |
| 4 | `noUnwind` | never throws (`noexcept`, a `nothrow` declaration, or inferred) |
| 5 | `willReturn` | always returns or throws; never loops forever or exits the process |
| 6 | `declared` | bits 0–5 come from a user declaration, not from inference; not a guarantee |
| 7–31 | — | reserved, 0 |

`GUARANTEE_MASK` is bits 0–5. The names users declare
([06 §3.3](06-semantics.md)) map to bit sets:

| Declaration | Bits |
|---|---|
| `ModRef` (the default) | none |
| `readonly` | `noArgWrite`, `noOtherWrite` |
| `argmemonly` | `noOtherRead`, `noOtherWrite` |
| `argmemonly readonly` | `noArgWrite`, `noOtherRead`, `noOtherWrite` |
| `writeonly` | `noArgRead`, `noOtherRead` |
| `pure` | bits 0–3 |

- **Implication** is a bitwise subset test, so every combination has a
  defined order and there is no lattice to get wrong.
- **Freedom to optimize.** A call with `noArgWrite`, `noOtherWrite`,
  `noUnwind` and `willReturn` may be merged with an identical earlier call
  when no write lies between them; with bits 0–5 all set it may also be
  hoisted or removed if unused ([03](03-conversion-and-passes.md)).
- A wrong guarantee is undefined ([06 §7](06-semantics.md)).

### 4.7 Pointers

```c
struct PointerRecord {   // 24 bytes
	uint8_t  kind;       // 0 callee, 1 capture thunk, 2 destructor, 3 constant address, 4 extension payload
	uint8_t  mode;       // 0 ProcessLocal, 1 Portable (by name), 2 Portable (ELF build id + offset)
	uint16_t pad;        // 0
	uint32_t nameId;     // mode 1: symbol name; mode 2: build id as hex string; mode 0: 0
	uint64_t address;    // mode 0: the address; mode 2: offset from the load bias; mode 1: 0
	uint64_t processToken; // mode 0: token of the producing process; else 0
};
```

- `ProcessLocal` (mode 0) entries are valid only in the process whose
  token they carry. The token is a 64-bit value drawn at random once per
  process. The loader rejects a mismatching token.
- `Portable` entries are resolved at load: mode 1 by a `SymbolResolver`,
  mode 2 against the loaded module with that build id
  ([07 §3](07-serialization.md)).

### 4.8 Runtime bindings

```c
struct BindingRecord {   // 16 bytes
	uint32_t nameId;     // stable binding symbol
	uint32_t type;       // TypeId of the bound object in the low 8 bits; upper bits 0
	uint32_t size;
	uint32_t align;
};
```

`bind.addr` (§5.8) yields the address supplied for the binding at load
([07 §4](07-serialization.md)). Bindings with equal schemas stay distinct
identities.

### 4.9 Debug data (optional)

```c
struct RegionRecord {    // 16 bytes; one region() call site (docs/region.md)
	uint32_t function;   // function index
	uint32_t nameId;     // user-supplied region name (index in debugNames), or 0xFFFFFFFF
	uint32_t location;   // index in debugLocations of the call site, or 0xFFFFFFFF
	uint32_t parent;     // enclosing region index, or 0xFFFFFFFF
};
```

- The debug sections (15–18, 21, 22) are present only when a debug or
  profiling option was set ([05 §7](05-backends.md)), always together, and
  signalled by feature bit `DEBUG_V1`.
- Debug data has its own string table (`debugNames`), so it never adds to
  the canonical `names` section.
- Debug data does not change semantics. Consumers must work without it, and
  the structural hash never covers it (§7).

### 4.10 Data objects

```c
struct DataRecord {      // 16 bytes
	uint32_t size;       // > 0
	uint32_t align;      // a power of two, at most 4096
	uint32_t byteOffset; // into dataBytes, aligned to align
	uint32_t flags;      // reserved, 0
};
```

- **Read-only.** A data object is an immutable byte array owned by the
  module. Writing to it is undefined. Its contents are part of the image and
  of the structural hash, and have `Literal` provenance
  ([07 §5](07-serialization.md)), so a module whose constant tables live in
  data objects stays persistable.
- **Address and lifetime.** `data.addr` (§5.8) yields its address. The
  engine places each data object once per module, in read-only memory, and
  every tier and every promotion of the module uses the same address. The
  address stays valid until the module is released (§4.4.1).
- **Identity.** Data objects with equal size, alignment and contents may
  share an address, as C++ string literals may. Programs must not rely on
  distinct data objects having distinct addresses.
- **No relocations in 1.0.** Data contains bytes only, never addresses of
  functions or other data, so it is position-independent. A relocation
  table can be added in a minor version (§8.2).
- **No mutable globals.** Mutable module state is a runtime binding (§4.8),
  owned by the application ([07 §4](07-serialization.md)).
- **Not exported or imported.** Data objects are private to their module.
  Bundling (§8.5) and cross-module inlining (§4.4.3) copy the data objects
  they need.

## 5. Operations

### 5.0 Execution model

- A function executes from block 0. Within a block, instructions execute in
  order; a terminator transfers control.
- **Observable behavior** is the sequence of memory writes and reads
  through pointers, calls to external and imported functions and to
  intrinsics (as far as their guarantees allow), thrown exceptions and the
  returned value. A transformation may reorder, merge or remove other
  instructions as long as observable behavior is preserved.
- Instructions with effects (`load`, `store`, calls, checked division)
  keep their relative order, except as their guarantees (§4.6.2) allow.
- **Memory model:** plain loads and stores only. Concurrent accesses by
  several threads to the same location, at least one of them a write,
  without synchronization through external calls, are undefined (C++'s
  data-race rule). NIR 1.0 has no atomic operations.

Notation: `a`, `b`, `c` are the record fields; *v* is a value reference,
*blk* a block id, *x* an `extra` offset. Unless stated otherwise, operands
have the result type.

### 5.1 Arithmetic

| Op | Code | Operands | Types | Flags | Semantics |
|---|---|---|---|---|---|
| `add` | 1 | a, b: *v* | integer, float, `ptr` | — | integers wrap modulo 2ⁿ; floats IEEE; `ptr`: a is `ptr`, b is an `i64` byte offset, the result wraps modulo 2⁶⁴ |
| `sub` | 2 | a, b: *v* | integer, float, `ptr` | — | as `add` |
| `mul` | 3 | a, b: *v* | integer, float | — | as `add` |
| `div` | 4 | a, b: *v*; c: unwind entry if checked, else 0 | integer, float | bit 0 unsigned; bit 1 checked | §5.10 |
| `rem` | 5 | a, b: *v*; c: as `div` | integer, float | bit 0 unsigned; bit 1 checked | §5.10 |
| `fneg` | 6 | a: *v* | float | — | flips the sign bit, also of NaN and zero |

Float arithmetic is IEEE 754 with round-to-nearest-even, without fast-math
(D11). Flag bits 2–7 of float operations are reserved for fast-math flags
in a later version and must be 0.

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
| `shl` | 16 | a: value; b: amount, same type | — | the amount is taken modulo the bit width (D10) |
| `shr` | 17 | a: value; b: amount, same type | bit 0: unsigned (logical); else arithmetic | as `shl` |

### 5.4 Compare and select

| Op | Code | Operands | Result | Flags |
|---|---|---|---|---|
| `cmp` | 20 | a, b: *v*, same scalar type | `bool` | bits 0–2 predicate: 0 `eq`, 1 `ne`, 2 `lt`, 3 `le`, 4 `gt`, 5 `ge`; bit 3: unsigned (integers only) |
| `select` | 21 | a: `bool`; b, c: *v*, same type (any non-`void`, extension types included) | type of b and c | — |

- Float comparisons follow C++: with a NaN operand, `ne` is true and all
  other predicates are false. `-0.0` equals `0.0`.
- `bool` allows only `eq` and `ne`. `ptr` compares as unsigned 64-bit.

### 5.5 Casts

| Op | Code | Operands | Flags bits 0–3: kind |
|---|---|---|---|
| `cast` | 24 | a: *v*; result type in `type` | 0 `sext`, 1 `zext`, 2 `trunc`, 3 `sitofp`, 4 `uitofp`, 5 `fptosi`, 6 `fptoui`, 7 `fpext`, 8 `fptrunc`, 9 `bitcast`, 10 `ptrtoint`, 11 `inttoptr` |

- `sext`/`zext` widen an integer (or `bool`: `zext` gives 0 or 1, `sext`
  gives 0 or −1); `trunc` narrows an integer. Truncation to `bool` is not
  a cast; use `cmp ne 0`.
- `fptosi`/`fptoui` **saturate**: NaN gives 0, and values outside the
  target range give its minimum or maximum (D11).
- `fptrunc` rounds to nearest even; `fpext` is exact.
- `bitcast` requires equal sizes and scalar types other than `bool` and
  `ptr`. Conversions between `ptr` and integers use only `ptrtoint`
  (giving `i64`) and `inttoptr` (taking `i64`), so pointer provenance stays
  visible.

### 5.6 Memory

| Op | Code | Operands | Result | Flags |
|---|---|---|---|---|
| `load` | 28 | a: `ptr` | the loaded type (scalar or extension) | bits 0–3: log₂ of the guaranteed alignment; 0 = the type's natural alignment |
| `store` | 29 | a: `ptr`; b: the value | `void` | as `load` |
| `alloca` | 30 | a: index into the function's alloca range | `ptr` | — |

- An access through an address that is not valid for the type's size, or
  that is less aligned than the flags state, is undefined. Loading a
  `bool` from a byte that is not 0 or 1 is undefined.
- `alloca` yields the address of the function's static stack slot `a`,
  sized and aligned by its `AllocaRecord`. Every execution in the same
  invocation yields the same address. The slot lives until the invocation
  returns or unwinds; its initial contents are indeterminate.

### 5.7 Calls

| Op | Code | Operands | Result |
|---|---|---|---|
| `call` | 32 | a: callee index (§4.6); c: *x* → `[argc, arg₀ … argₙ₋₁, unwind, thunk]` | the signature's result type |
| `call.indirect` | 33 | a: `ptr` callee (*v*); b: signature index; c: *x* as `call` | the signature's result type |
| `funcaddr` | 34 | a: callee index of kind internal, external or imported | `ptr` |

- Arguments are value references matching the signature's parameter types.
- **By callee kind:** an internal call may use the backend's own convention
  (§4.5); an external call calls the native function; an imported call
  loads the import slot and calls the exporter's entry point (one indirect
  call); an intrinsic call is lowered inline by a backend that has a
  lowering, and otherwise calls the fallback.
- **`funcaddr` yields a stable address.** For an internal function it is
  the address of a forwarding stub with the function's C convention that
  always reaches the module's current code; for an imported function, a
  stub that calls through the import slot; for an external function, the
  native address. A stub stays valid until the module that took the
  address is released (§4.4.1), across every promotion. Code may store
  the address and call it later. Intrinsics have no address.
- `unwind` is an unwind entry (§3.4). It must be `NO_UNWIND` if the callee
  has `noUnwind`.
- `thunk` is a `pointers` index of kind capture thunk, or `0xFFFFFFFF`. It
  is used only by external calls and by intrinsic calls that reach the
  fallback; internal, imported and indirect calls set `0xFFFFFFFF`, because
  the engine bridges exceptions between Nautilus frames itself. Backends
  without unwind tables call through the thunk when the call may throw
  ([06 §3.2](06-semantics.md)).
- **Freedom to optimize** follows the callee's guarantee bits (§4.6.2).
- **A call with `NO_UNWIND` that throws is undefined.** Debug builds detect
  it with a terminate handler ([06 §3.3](06-semantics.md)).
- **`call.indirect` has no callee record,** so it guarantees nothing: its
  memory effects are unknown, and it may throw unless its unwind entry is
  `NO_UNWIND`, which the producer may state only when it knows every
  possible target cannot throw.
- **Calls inside landing pads** must have `NO_UNWIND` or `NO_CLEANUP`. An
  exception that escapes a destructor while a landing pad runs terminates
  the process, as in C++.

### 5.8 Runtime state and data

| Op | Code | Operands | Result |
|---|---|---|---|
| `bind.addr` | 36 | a: binding index (§4.8) | `ptr` |
| `data.addr` | 37 | a: data object index (§4.10) | `ptr` |

- **Loads from data fold.** A `load` whose address is `data.addr d` plus a
  constant byte offset, entirely within `d` and aligned as the load states,
  reads the known bytes and may be replaced by a constant. A load outside
  the object is undefined.

### 5.9 Values and control flow

| Op | Code | Operands | Result | Notes |
|---|---|---|---|---|
| `blockarg` | 40 | a: argument position | any non-`void` type | SSA: only the first `argCount` instructions of a block; pre-SSA: only function parameters |
| `br` | 48 | a: target *blk*; c: *x* → `[argc, args…]` | `void` | terminator |
| `condbr` | 49 | a: `bool` condition; b: true *blk*; c: *x* → `[prob, trueArgc, trueArgs…, falseBlk, falseArgc, falseArgs…]` | `void` | terminator |
| `ret` | 50 | a: value, or `NONE` for a `void` function | `void` | terminator |
| `resume` | 51 | — | `void` | terminator; only in landing pads; continues unwinding the current exception |
| `assign` | 56 | a: variable (value id); b: new value | `void` | **Pre-SSA level only** (§6.2) |

- `prob` is the probability that the condition is true, as
  `round(P × 0xFFFFFFFE)`; `0xFFFFFFFF` means unknown.
- At the pre-SSA level, argument counts are always 0.
- Codes 1–255 not listed here are reserved for core opcodes of later minor
  versions (§8.2). NIR 1.0 has no `unreachable` terminator: tracing always
  ends a block with one of the terminators above.

### 5.10 Division and remainder

- **Floats:** IEEE. Division by zero gives ±∞ or NaN and never throws;
  float `rem` is C++ `fmod`. The checked bit must be 0.
- **Integers**, bit 0 selecting unsigned or signed:
  - if the divisor is 0, or (signed) the dividend is the type's minimum and
    the divisor −1, the instruction **throws
    `nautilus::ArithmeticException`** (D10);
  - otherwise the quotient rounds toward zero, and `rem` takes the sign of
    the dividend, as in C++.
- **The checked bit.** An integer `div`/`rem` whose divisor is not proven
  safe must set bit 1 and carry an unwind entry in `c`. A divisor is safe
  if it is a constant other than 0, and for signed operations also other
  than −1. With bit 1 clear, `c` is 0 and the instruction cannot throw. An
  instruction with bit 1 clear and an unsafe divisor at run time is
  undefined.

### 5.11 Extension operations

- An extension instruction (`op` ≥ 256) uses the same record. Its operands,
  types, flags and `extra` payload are defined by its `OpInfo`
  ([04 §3.3](04-extensions.md)) and checked by its `verify` hook.
- It produces at most one value. An operation with several results returns
  an extension aggregate type, or is split into several operations.
- Extension operations are never terminators and never throw; an operation
  that may fail calls a native function, which may throw like any call.
- Its effects come from `OpInfo.effects` and take part in §5.0's ordering.

## 6. Levels and invariants

### 6.1 Both levels

1. Every function has at least one block. Every block ends in exactly one
   terminator; terminators appear nowhere else.
2. Every value reference points to an instruction with a non-`void` result
   in the same function, or to an existing constant.
3. Operand and result types follow §2 and §5; reserved flag bits and unused
   operand fields are 0.
4. Branch argument counts and types match the target block's `blockarg`s.
5. Every table index (callee, signature, alloca, binding, pointer, data
   object, name, extension, `extra` offset) is in range, and every `extra`
   list and data object is in bounds of its section.
6. A function's `ret` operands match its signature's result type.
7. Calls match their signatures; a `noUnwind` callee has `NO_UNWIND`.
8. Exactly one `module` record. Function names are unique; every exported
   function has a name. Callee and import signatures match §4.6; a callee's
   attributes for an internal function equal the function's. Reserved
   attribute bits are 0. Exported signatures use only types with a C ABI
   (§4.5).
9. Imports are unique by `(module, function)` and never name the module
   itself. A deserialized image has no imports (§8.5).

### 6.2 Pre-SSA level

- **Variables.** A value id may be redefined by `assign`. Its operand `a`
  must be the id of an earlier non-`void` instruction (the variable's
  definition), and `b` must have the same type. A use of a variable sees the
  value of the most recent `assign` (or the defining instruction) on the
  path executed so far. Every use must be reached by a definition on every
  path. `a` is never a constant. Parameters are variables like any other
  and may be assigned.
- **No block arguments** except the function parameters, and branch
  argument lists are empty. Merges are expressed through variables.
- **No landing pads.** Instructions that may throw carry cleanup lists
  (§3.4).
- Blocks are in recording order and list their instructions in `extra`
  (§4.3).
- Snapshots, the tag map and the replay cursor are derived data
  ([01 §6](01-tracing-frontend.md)).
- A pre-SSA function is never executed and never passed to a backend;
  the conversion ([03](03-conversion-and-passes.md)) is the only way to the
  SSA level.

### 6.3 SSA level

- No `assign`. Every value is defined exactly once.
- **Dominance:** every use of a value is dominated by its definition. A use
  as a branch argument counts at the end of the predecessor block.
- **Layout:** block 0 is the entry block. The other blocks that are not
  landing pads follow in reverse post-order, with each loop's blocks
  contiguous. Landing pads come after all other blocks.
- Every block is reachable from the entry block, landing pads through
  unwind entries. A landing pad is entered only through unwind entries,
  never by `br`/`condbr`, and ends in `resume` or in `br` to another landing
  pad.
- Unwind entries are `NO_UNWIND`, `NO_CLEANUP` or landing-pad block ids,
  never cleanup lists.

### 6.4 The verifier

The verifier checks §3, §4, §5 and the invariants of each function's level,
and every extension instruction against its `Signature` and `verify` hook.
It runs on every module that comes from a deserializer or the C API, and in
debug builds after every producer. A module that fails verification is
never passed to a backend. Each rule has a positive and a negative test
([12](12-validation.md)).

### 6.5 Concurrency

- A finished SSA-level module is **immutable**. Any number of threads may
  read, compile, hash, serialize or inline from it concurrently.
- Building and rewriting a module are single-threaded per module.
- The module registry is thread-safe: registering, looking up, linking and
  releasing modules may happen concurrently. Linking pins exporters under the
  registry's lock (§4.4.2).
- An import slot is written with a release store and read with an acquire
  load (or a load the target orders equivalently).
- The extension registry is frozen after the first compile
  ([04 §3.7](04-extensions.md)).

## 7. Canonical form

A canonical module is the form that is serialized and hashed
([02 §3.6](02-nir.md), [08](08-caching.md)). **Canonical form, hashing,
caching and bundling (§8.5) apply to modules whose functions are all at the
SSA level.** A module with pre-SSA functions is serialized as it is, for
debugging and reproducers only; it is never hashed, cached or executed.

Canonicalization:

1. applies pending replacements and compacts each function, so ids are
   dense and, at the SSA level, in layout order;
2. orders functions: exported functions by name, then the other functions
   in order of first reference from the functions before them; orders
   imports by `(module name, function name)`;
3. drops unused constants, data objects, names, signatures, callees,
   allocas, pointers and bindings, and orders each table by **first use** in this scan order:
   functions in canonical order; within a function, its signature, then its
   instructions in order; within an instruction, the fields `a`, `b`, `c`,
   then its `extra` list word by word; a referenced record (a callee, an
   import, a signature) is scanned at its first reference, before
   continuing;
4. interns names, constants, signatures and data objects (equal size,
   alignment and contents), so no table holds duplicates; writes
   `dataBytes` in data-object order, each object at the next offset aligned
   to its alignment, padding with zeros;
5. numbers extension opcodes and types image-locally in first-use order
   (§8.4);
6. writes `extra` without gaps in this order: the parameter lists of the
   signatures in signature order, then the lists of the instructions in
   function and instruction order;
7. zeroes all padding, unused fields and reserved bits, and drops the debug
   sections for hashing.

Two structurally equal modules have byte-identical canonical images.

**The structural hash** (the L2 key, [08](08-caching.md)) is XXH3-128 over:
1. the canonical sections of the canonical image of the module **as if it
   were anonymous**: the `module` record's `nameId` set to `0xFFFFFFFF` and
   the name omitted from `names` unless something else uses it;
2. the **effective compile options**: the options marked code-affecting in
   the option registry, as a list sorted by key, each entry encoded as
   `u32 keyLength, key bytes, u8 kind (0 bool, 1 i64, 2 f64, 3 string),
   value` (bool: one byte; i64 and f64: eight bytes, f64 by bit pattern;
   string: `u32 length` and bytes), preceded by the `u32` entry count;
3. the `name@version` of every extension in use, in `extensions` order.

It is a cache key, not a security property; a hit is confirmed by comparing
the images.

- **In memory** (the L0 cache), imports are hashed by name, signature and
  assumed attributes, so an importer's key does not change when the
  exporter's body does.
- **A serialized image** is self-contained (§8.5): its hash covers the
  bundled bodies, so persistent entries (L1, L2) of an importer change when
  an exporter it imports changes.
- **Compiled code** that inlined imports adds the inlined exporters' hashes
  (§4.4.3).

## 8. Binary image and versioning

### 8.1 Layout

```c
struct ImageHeader {          // 48 bytes, at offset 0
	char     magic[4];        // "NIR\0"
	uint16_t major, minor;    // format version; this spec: 1.0
	uint32_t compatFeatures;  // bits a reader may ignore
	uint32_t incompatFeatures;// bits a reader must understand
	uint64_t totalSize;       // bytes, including the header
	uint32_t sectionCount;
	uint32_t crc32c;          // CRC-32C (Castagnoli) of bytes [48, totalSize)
	uint8_t  reserved[16];    // 0
};

struct SectionEntry {         // 24 bytes; sectionCount entries directly after the header
	uint32_t kind;            // §4.1
	uint32_t stride;          // record size in bytes; ≥ this spec's record size
	uint64_t offset;          // from the image start; 8-byte aligned
	uint64_t count;           // number of records, < 2³²
};
```

- Section entries appear in increasing `kind` order, each kind at most
  once. Section data does not overlap and is padded with zeros to 8 bytes.
- A reader uses the fields it knows and skips the rest of each record, so a
  later minor version can append fields by widening `stride`.
- **Loading** validates the magic, the version, `totalSize`, the checksum,
  section bounds and strides, then every reference, then runs the
  verifier. Any failure rejects the image; a malformed image must never
  cause undefined behavior.

- **Limits.** Before allocating, the loader checks the image against
  configurable limits (defaults in brackets): `totalSize` [1 GiB], functions
  per module [2²⁰], instructions per function [2²⁸], blocks per function
  [2²⁴], entries per table [2²⁸], `extra` words [2²⁸], extension entries
  [2¹⁶], `dataBytes` [256 MiB]. An image above a limit is rejected before any section is
  materialized.

### 8.2 Versioning

| Change | Version |
|---|---|
| New core opcode, new attribute or flag bit | minor. An image that uses it is rejected by older readers (they see an unknown opcode or a non-zero reserved bit), which is safe; images that do not use it stay loadable |
| New optional section | minor. Older readers ignore it |
| Field appended to a record (wider `stride`) | minor, only if a zero value keeps the old meaning; older readers skip it. A field whose non-zero value older readers must not ignore also needs an `incompatFeatures` bit |
| New canonical section a reader must understand | minor, plus an `incompatFeatures` bit |
| Changed meaning or encoding of an existing field, opcode or rule | major |

- Within 0.4.x, images stay loadable (D12): patch releases do not change
  the format, and minor versions only add.
- Opcodes, section kinds and feature bits are never renumbered within a
  major version. The C API's enums map one to one ([09](09-c-api.md)).
- A reader rejects an image with a higher major version, or with an
  `incompatFeatures` bit it does not know.

### 8.3 Feature bits (1.0)

| Bit | Name | Class | Meaning |
|---|---|---|---|
| 0 | `DEBUG_V1` | compat | the debug sections 15–18, 21 and 22 are present |
| 1 | `EXTENSIONS` | incompat | the module uses extension operations, types or intrinsics |
| 2 | `PROCESS_LOCAL` | incompat | the module contains `ProcessLocal` pointers (mode 0) |

### 8.4 Extensions in images

```c
struct ExtensionRecord { // 8 bytes
	uint32_t nameId;     // "name@version", e.g. "simd.add@1"
	uint8_t  kind;       // 0 operation, 1 type, 2 intrinsic
	uint8_t  pad[3];     // 0
};
```

- Image opcode 256 + *i* is the *i*-th entry of kind 0; image type 16 + *j*
  is the *j*-th entry of kind 1. Intrinsic callees (kind 2) refer to their
  entry by its index in the section. Names are `<plugin>.<name>`, and the
  version is a decimal `u32` after `@`.
- **Loading maps them to process ids** in one pass over `insts` and the
  type fields of all records. An image without extensions is used in
  place; an image with extensions is rewritten once while loading.
- Each extension declares a `version` and a `minCompatibleVersion`
  ([04 §3.2](04-extensions.md)). An image entry is accepted if its version
  lies in that range. A missing extension or a version outside the range
  rejects the image.

### 8.5 Self-contained images

**Every serialized image is self-contained.** Exporting a module to an
image (`saveModule`, `nautilus_ir_save`, an L1/L2 cache store) bundles its
imports, so loading an image never needs another module.

1. **Collect** the transitive closure of the module's imports: every
   imported function, and everything it calls in its own module and through
   its own imports.
2. **Copy** each collected function into the image as an `internal`
   function named `<module>::<function>`, with `#2`, `#3`, … appended if
   that name is already taken, together with the internal functions it
   calls.
3. **Rewrite** every callee of kind `imported` into kind `internal`,
   pointing at the copy, with the copy's attributes (invariant 8 of §6.1),
   and leave the `imports` section empty.
4. **Merge the tables:** constants, signatures and data objects are
   interned; pointers
   are copied (a `ProcessLocal` pointer keeps the image process-local, as
   for any module); runtime bindings are merged by name, and two bindings
   with the same name but different schemas fail the export; extensions
   are united.
5. **Canonicalize** (§7) and write. Only the module's own exported functions
   stay exported.

- **Native payloads** in an artifact ([07 §2](07-serialization.md)) are
  compiled from the bundled module, so they contain the copies instead of
  import slots. Saving an importer may therefore compile it once more for
  the payload.
- **Loading** a self-contained image needs no link step. The loaded module
  can itself be registered under its name and export functions to others.

## 9. Text form

The printer writes a text form for dumps, tests and bug reports, and 0.4
also **parses** it, so tests can be written as `.nir` text
([15 §3.3](15-agent-experience.md)). Printing, parsing and printing again
gives identical text. The text form is a view of the binary format, not a
second format: every construct prints, and parsing produces the same module
as the binary image of that text.

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
  %5 = div.s.checked %4, %3 unwind(caller)
  ret %5
}
```

- Values print as `%id`, constants as `value:type`, blocks as `^bN`, data
  objects as `@dN` (defined once at the top as `data @dN align A = x"…"`).
- Flags print as suffixes: `.s`/`.u` for signedness, the predicate for
  `cmp`, `.checked` for checked division, the kind for `cast`.
- Extension operations print by name (`simd.add`).
- Unwind entries print as `unwind(caller)` for `NO_CLEANUP`, `unwind(^bN)`
  for a landing pad, `cleanup(...)` for a pre-SSA cleanup list, and
  nothing for `NO_UNWIND`.
- Pre-SSA functions print `assign %v, %w` and list blocks in recording
  order.

## 10. Decisions taken for 1.0

These questions were open in the draft and are settled here:

| Question | 1.0 |
|---|---|
| Encoding of debug data | optional sections 15–18, 21, 22 with their own string table (§4.1, §4.9), feature bit `DEBUG_V1` |
| `unreachable` terminator | not in 1.0; tracing ends every block with a real terminator (§5.9) |
| Atomic operations | not in 1.0; data races are undefined (§5.0). Candidates for 1.1 if atomics through `invoke` show a measured cost |
| Fast-math | not in 1.0; flag bits reserved (§5.1) |
| Pointer arithmetic | `add`/`sub` on `ptr` with an `i64` byte offset, as 0.3's `ADD` on pointers (§5.1) |
| Destructors at the pre-SSA level | cleanup lists in the unwind entry (§3.4), turned into shared landing pads by the conversion |
| 0.3's unused trace op `FREE` | dropped |
| Extension versioning | `version` plus `minCompatibleVersion` (§8.4) |
| Structural hash | XXH3-128 over the canonical image plus options and extension versions (§7) |
| Modules and imports | named modules with exports and imports; linking against the engine's module registry, by name and signature, acyclic; import slots updated on promotion (§4.4) |
| Call targets | four callee kinds: internal, external, imported, intrinsic (§4.6) |
| Memory effects | four independent guarantee bits (no reads/writes of argument memory and of other memory), zero = no guarantee, implication by bitwise subset (§4.6.2) |
| `funcaddr` | stable forwarding stubs, valid until the module is released (§5.7) |
| Global data | read-only data objects with `data.addr`, folded loads, no relocations, no mutable globals (§4.10) |
| Interning imports | never at import or link time; inlining is a separate optional pass that works across modules (§4.4.3); exported images always bundle their imports and are self-contained (§8.5) |

## 11. Conformance and change control

- **Becoming final.** This specification is a release candidate. It becomes
  final at the M1 exit, when the verifier, the serializer, the loader and
  the round-trip tests implement it without changes to the text. Changes
  found during M1 are made here first.
- **Conformance material,** maintained with the specification:
  - a hex dump of a small canonical image with every byte explained;
  - a corpus of golden images (valid and invalid), each with the expected
    verifier or loader result;
  - a positive and a negative test per rule of §6 and per opcode of §5
    ([12](12-validation.md)).
- **Change control.** After 1.0 is final, every change to this document
  states its version effect (§8.2), is recorded in a change log at the end
  of this document, and needs review by a maintainer of the format.

## Change log

| Version | Change |
|---|---|
| 1.0 RC | Initial specification, including read-only data objects (§4.10) |
| 1.0 RC | The level formerly called `Trace` is named `PreSSA` (value 0, unchanged) |
| 1.0 RC | The text form is parsed as well as printed (§9) |
