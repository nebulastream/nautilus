# NIR Specification

Part of the [Nautilus 0.4 design](README.md) · Format version **1.0** ·
Status: **release candidate**; final at the M1 exit, once the verifier, the
serializer and the round-trip tests implement it unchanged (§11)

This document is the normative definition of NIR: its data model, types,
binary encoding, operations and their semantics, the invariants of each
level, canonical form, the text form, and versioning. Every producer (the
tracer, the conversion, passes, the C API, the deserializer, the text
parser) and every consumer (backends, the serializer, caches, tools) must
follow it. [02-nir.md](02-nir.md) explains the design and why it is this
way; where the two differ, this document wins.

**Conventions.** "Must" and "must not" are requirements: a module that
violates one is *invalid*, and the verifier rejects it. "Undefined" means
the program's behavior is not constrained, as in C++. All integers in the
binary encoding are little-endian. Sizes and offsets are in bytes unless
stated otherwise. Ranges are written `[first, first + count)`. `NONE` is
`0xFFFFFFFF` wherever a `u32` field allows "no entry". Strings and names
compare as byte sequences (`memcmp` order; on a common prefix the shorter
one is smaller), never by locale.

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
  string attributes, a visibility, a level and a list of **blocks** (§4.5).
  Each block holds **instructions**; the last instruction of a block, and
  only that one, is a terminator.
- A **call target** (callee) is one of four kinds (§4.6): a function of the
  same module (*internal*), a function exported by another module
  (*imported*), a native function of the host process (*external*), or a
  function registered by a plugin and known by name (*intrinsic*).
- An instruction produces **at most one value**. A value's **id** is the
  function-local index of its instruction, starting at 0. Instructions
  without a result (terminators, `store`, void calls) have ids too; no value
  refers to them.
- A **block id** is the function-local index of the block. Block 0 is the
  entry block.
- **Constants** are not instructions. They live in the module's constant
  pool and are referenced from operands (§3.2). Traced host addresses are
  not constants; they are `pointers` entries read by `ptr.const` (§4.7).
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
| 16–255 | extension | from `TypeInfo` | registered types, at most 240 ([04 §3.2](04-extensions.md)) |

- **Integers are signless** ([06 §4](06-semantics.md)). Operations whose
  result depends on signedness select it with a flag (§5). 0.3's
  `ui8`…`ui64` map to `i8`…`i64`. At native boundaries, signatures record
  how narrow values are extended (§4.5.1).
- "Integer" below means `i8`…`i64`; "float" means `f32` or `f64`;
  "scalar" means any core type except `void`.
- **Extension type ids are per process in memory.** In a serialized image
  they are image-local and named in the `extensions` section (§8.4). A
  parameterized type such as `simd.vec<f32,8>` is one registered instance
  with its own id; plugins register every instance at registration time
  (§8.4, [04 §3.7](04-extensions.md)).
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
2³¹ − 2 constants. Operands that are not value references (block ids,
table indices, `extra` offsets, immediates) are plain `u32` values; §5 names
them.

### 3.3 The `extra` array

Variable-length operand lists live in the module's `extra` section, an
array of `u32` words. An instruction refers to its list by the word offset
in `c`. Each list is owned by exactly one referrer (an instruction, a
signature, a pre-SSA block's op-id list, or the unwind word of an
instruction at the pre-SSA level, which owns its cleanup list); lists do
not overlap. A list whose first word is a count still occupies that word
when the count is 0.

### 3.4 Unwind entries

Instructions that may throw (calls and checked integer division; §5) carry
an **unwind entry**, one `u32`:

| Level | Value | Meaning |
|---|---|---|
| both | `0xFFFFFFFF` (`NO_UNWIND`) | the instruction cannot throw |
| SSA | `0xFFFFFFFE` (`NO_CLEANUP`) | may throw; nothing to clean up; the exception propagates to the caller |
| SSA | block id | may throw; on an exception, control enters this landing-pad block |
| Pre-SSA | `extra` offset | may throw; the offset points to a **cleanup list** `[n, (destructor₀, object₀), …]`: `n` pairs of a destructor callee index (§4.6) and a value reference to the object's address (`ptr`), in destruction order. `n` = 0 means no cleanup |

The conversion turns cleanup lists into shared landing pads
([03 §3](03-conversion-and-passes.md)). At the pre-SSA level a may-throw
instruction always has a cleanup list (possibly empty), never `NO_CLEANUP`.

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
| 16 | `debugInstLocations` | `u32` per instruction, parallel to `insts`: index in `debugLocations`, or `NONE` | optional |
| 17 | `debugRegions` | `RegionRecord`, 16 B (§4.9) | optional |
| 18 | `debugInstRegions` | `u32` per instruction, parallel to `insts`: index in `debugRegions`, or `NONE` | optional |
| 19 | `module` | `ModuleRecord`, 16 B, exactly one (§4.4) | canonical; the name is excluded from the structural hash (§7.2) |
| 20 | `imports` | `ImportRecord`, 16 B (§4.4) | canonical in memory; always empty in a serialized image (§8.5) |
| 21 | `debugNames` | `{u32 offset, u32 length}` into `debugNameBytes`, 8 B | optional |
| 22 | `debugNameBytes` | UTF-8 bytes, not NUL-terminated | optional |
| 23 | `data` | `DataRecord`, 16 B (§4.10) | canonical |
| 24 | `dataBytes` | the contents of the data objects | canonical |
| 25 | `functionAttrs` | `FunctionAttrRecord`, 16 B (§4.5.2) | canonical |
| 26 | `debugFunctions` | `DebugFunctionRecord`, 16 B (§4.9) | optional |
| 27 | `debugParamNames` | `u32` index in `debugNames`, or `NONE` (§4.9) | optional |
| 28–1023 | — | reserved for later versions | — |
| ≥ 1024 | — | vendor and experimental sections; ignored by readers that do not know them | optional |

The instructions of one function are contiguous in `insts`; so are its
blocks in `blocks` and its allocas in `allocas`. Functions are laid out in
`functions` order. The `u32` sections are `extra`, `debugInstLocations`,
`debugInstRegions` and `debugParamNames`; the byte sections are
`constBytes`, `nameBytes`, `debugNameBytes` and `dataBytes`.

### 4.2 Constants

```c
struct ConstRecord {     // 16 bytes
	uint8_t  type;       // TypeId; not void, not ptr
	uint8_t  provenance; // 0 Literal, 1 AuditedInvariant, 2 RawProcessValue (07 §5)
	uint16_t pad;        // 0
	uint32_t byteOffset; // into constBytes for types wider than 8 bytes, else 0
	uint64_t bits;       // the bit pattern, zero-extended; 0 if byteOffset is used
};
```

- Floats are stored by bit pattern, so NaN payloads and `-0.0` are exact.
- `bool` constants are 0 or 1. Integer constants narrower than 64 bits
  have their unused upper bits 0.
- **There are no `ptr` constants.** Every traced address, including the
  null pointer, is produced by `ptr.const` (§5.8): a non-null address is a
  `pointers` entry of kind *traced address* with its own provenance (§4.7).
  Addresses of functions, thunks and destructors are `pointers` entries
  too.
- **Provenance.** The tracer gives integer, float and `bool` constants
  `Literal` unless the user marks them (`cacheInvariant` →
  `AuditedInvariant`; [07 §5](07-serialization.md)). A constant produced by
  folding gets the **least trusted** class among its inputs
  (`RawProcessValue` is less trusted than `AuditedInvariant`, which is less
  trusted than `Literal`). `ptrtoint` of a `ptr.const` is never folded to a
  constant.
- **A `RawProcessValue` constant makes the module process-local:** its
  `ModuleRecord.processToken` is set and the image carries
  `PROCESS_LOCAL` (§8.3), exactly as for a `ProcessLocal` pointer.
- An extension-type constant has `size` bytes (from its `TypeInfo`) at
  `byteOffset` if wider than 8, else in `bits` (unused upper bytes 0).
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
	uint32_t nameId;     // module name; NONE for an anonymous module
	uint32_t flags;      // reserved, 0
	uint64_t processToken; // token of the producing process if the module is
	                       // process-local (§4.2, §4.7); else 0
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
  part of the structural hash (§7.2), so equal modules under different names
  share cache entries.
- **Process token.** A 64-bit value drawn at random once per process,
  never 0. A module is **process-local** when it contains a `ProcessLocal`
  pointer (§4.7) or a `RawProcessValue` constant (§4.2); then
  `processToken` is the token of the producing process, and otherwise it
  is 0.
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
| **built** | produced by the tracer, the C API, the text parser or a deserializer; verified; not executable |
| **linked** | every import is resolved against a loaded exporter (§4.4.2) |
| **compiled** | an executable exists for at least one backend; may be promoted to another tier ([11](11-tiered-compilation.md)) |
| **released** | no handle, importer or running call refers to it; its code and its `funcaddr` stubs are freed. Calling a stub of a released module is undefined |

#### 4.4.2 Linking imports

Linking happens when a module is compiled or loaded, against the
**module registry** of its engine: the set of compiled modules with a name.

1. **Resolve.** Each import is looked up by `(moduleNameId, functionNameId)`.
   The exporter must be compiled and the function exported.
2. **Check the signature.** The export's signature must equal the import's
   signature exactly (types, extension kinds, count, result).
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
- **Function string attributes are not inlined.** A function with string
  attributes (§4.5.2) is inlined only if the inlining pass of the plugin
  that owns the keys allows it; the caller never inherits them.
- **Semantics do not change.** An inlined call behaves exactly as the call.
  Since a registered name refers to one module (§4.4), an inlined export
  never goes stale while the importer is linked.
- **Compiled code records what it inlined.** An executable that inlined an
  imported function records `(exporter name, exporter structural hash)`. A
  cache key for compiled code includes these hashes (§7.3,
  [08](08-caching.md)), so a different exporter never reuses the entry.
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
	uint32_t attrs;      // attribute bits (§4.6.2)
	uint8_t  level;      // 0 PreSSA, 1 SSA
	uint8_t  visibility; // 0 internal, 1 exported
	uint16_t pad;        // 0
};

struct SignatureRecord { // 12 bytes
	uint32_t result;     // type word of the result (§4.5.1); void = 0
	uint32_t paramCount;
	uint32_t paramTypes; // extra offset: paramCount type words; 0 if paramCount = 0
};

struct AllocaRecord {    // 8 bytes
	uint32_t size;       // > 0
	uint32_t align;      // a power of two, at most 4096
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
    `bool`) and narrow integers extended as their extension kind states
    (§4.5.1). An extension type may appear in an exported signature only if
    its `TypeInfo` defines a C ABI type for it (for example `__m256` for
    `simd.vec<f32,8>`); the verifier rejects others. The host calls it through
    a plain function pointer (or the `Invocable` wrapper when the backend's
    exception mode needs one, as in 0.3).
  - Calls between internal functions may use any convention the backend
    chooses.
- **Parameters.** The entry block's parameters are the function's
  parameters: at the SSA level its leading `blockarg`s, whose count and
  types equal the signature's; at the pre-SSA level the first `paramCount`
  instructions of the function, which are `blockarg`s and the first entries
  of block 0's list.
- **Signatures** have no variadic form and pass only core scalars and
  extension types; aggregates are passed by pointer, as in 0.3. Signatures
  are interned: equal records with equal type words are one signature.
- **Attributes** (§4.6.2) are inferred by the conversion, or declared, and
  for exported functions become part of the interface.

#### 4.5.1 Type words and extension kinds

A **type word** is a `u32` that holds a type in a signature
(`SignatureRecord.result` and each parameter word):

| Bits | Meaning |
|---|---|
| 0–7 | `TypeId` |
| 8–9 | **extension kind**: 0 none, 1 `zext`, 2 `sext`; 3 is invalid |
| 10–31 | 0 |

Every other `u32` field that holds a type (`BindingRecord.type`, extension
payload words of kind `type`) uses bits 0–7 only, with bits 8–31 equal to 0.

- **Meaning.** The extension kind states how a narrow integer argument or
  result is widened to a register when it crosses a native boundary: calls
  of external callees, intrinsic fallbacks and `call.indirect`, and the
  entry points of exported functions. A backend extends every narrow value
  it passes across such a boundary as stated, and relies on the extension
  of a value it receives only where the platform ABI guarantees it.
- **Rules.** A non-zero kind is allowed only on `bool`, `i8`, `i16` and
  `i32`. `bool` always has `zext`. `i8` and `i16` must have a non-zero kind.
  All other types have kind 0. The tracer fills the kind from the C++ type
  of every signature it builds: unsigned narrow types `zext`, signed narrow
  types `sext` ([01 §6](01-tracing-frontend.md)).
- **Equality.** Signatures are equal only if their type words are equal,
  extension kinds included (callee matching, §4.6; linking, §4.4.2).
- The extension kind is the only signedness information NIR keeps; `i64`
  and `ptr` carry none.

#### 4.5.2 Function string attributes

```c
struct FunctionAttrRecord { // 16 bytes (section 25)
	uint32_t function;      // function index
	uint32_t keyNameId;     // "<plugin>.<key>", e.g. "gpu.kernel"
	uint32_t valueNameId;   // the value, possibly the empty string
	uint32_t pad;           // 0
};
```

- **Key/value strings owned by plugins,** for metadata that is not a
  guarantee: 0.3's `FunctionOperation` string attributes become entries
  with the owning plugin's prefix (0.3 `kernel` is `gpu.kernel`). Core NIR
  defines no keys and gives them no semantics; plugins and their backends
  read them ([17 §4.5](17-gpu.md)).
- Records are sorted by `(function, key bytes)`; a key occurs at most once
  per function. The key has the form `<plugin>.<key>`.
- They are **canonical**: serialized, copied by bundling (§8.5) and covered
  by the structural hash, because a plugin may generate different code from
  them.

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
| 1 | **external** | `pointers` index, kind callee or destructor | a native C/C++ function of the host process, called with `invoke` in C++ | only if the pointer is `Portable` (§4.7) |
| 2 | **imported** | `imports` index | an exported function of another module, through its import slot (§4.4.2) | yes, by name |
| 3 | **intrinsic** | in memory: the process's intrinsic id in the `ExtensionRegistry`; in an image: `extensions` index of kind intrinsic | a plugin function registered by name, with a native fallback (§4.6.1) | yes, by name |

- `signature` must equal the target's: the function's signature (kind 0),
  the import's (kind 2), the registered intrinsic's (kind 3), or the
  traced C++ function type (kind 1).
- `attrs` are what calls through this callee may assume. For kind 0 they
  equal the function's attributes under `GUARANTEE_MASK`; for kind 2 they
  are the import's assumed attributes; for kinds 1 and 3 they come from the
  registration (`noexcept`, `nothrow`, declared effects,
  [06 §3](06-semantics.md)).
- **Destructor callees.** A callee of kind external whose target is a
  `pointers` entry of kind destructor is a *destructor callee*. Its
  signature is `(ptr) -> void` and its attributes come from the
  destructor's registration (normally `noUnwind`). Cleanup lists (§3.4)
  name destructor callees, and the destructor calls in landing pads are
  `call`s of them (§6.3). Callees are interned: equal records are one
  callee.

#### 4.6.1 Intrinsics

An intrinsic is a function that a plugin registers by name in the
`ExtensionRegistry` ([04 §3.7](04-extensions.md)), with:
- a name and version (`builtin.prefetch@1`) and a `minCompatibleVersion`;
- a NIR signature, with extension kinds (§4.5.1), and attributes (§4.6.2);
- a **native fallback**: a C function with exactly that signature, which
  defines the intrinsic's semantics;
- optional per-backend lowerings that replace the call with inline code.

Rules:
- **The identity is backend-independent.** Name, version, signature,
  attributes and fallback are registered by a backend-neutral init file
  that every build compiles, whatever backends it contains. Backend
  lowerings are optional and attach to a registered intrinsic by name.
- **Traced calls become intrinsics by address.** When the tracer records a
  call (`invoke`) of a native address that the registry knows as an
  intrinsic's fallback, it emits a callee of kind intrinsic, not external
  (a lookup by address → name at trace time). Every other `invoke` is an
  external callee.
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
  may throw (with an unwind entry and a capture thunk, §5.7) and its effects
  are call attributes. An extension operation (§5.11) is a value-level
  instruction: it can take extension types, fold, simplify and join CSE
  through its traits, and never throws. 0.3's `IntrinsicRegistry` entries
  become intrinsics; plugins move operations that benefit from folding or
  extension types to extension operations ([04 §4](04-extensions.md)).
  Operations that may throw, such as `gpu.launch`, are intrinsics.

#### 4.6.2 Attribute bits

Attributes are **guarantees**: each set bit promises something, and a clear
bit promises nothing. Zero is therefore always the safe default, and one set
of attributes implies another exactly when it has every guarantee bit the
other has. Used by `FunctionRecord.attrs`, `CalleeRecord.attrs`,
`ImportRecord.attrs` and the `attrs` word of `call.indirect`:

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

`GUARANTEE_MASK` is bits 0–5. Comparisons of attributes (callee against
function, import against export) use only these bits. The names users
declare ([06 §3.3](06-semantics.md)) map to bit sets:

| Declaration | Bits |
|---|---|
| `ModRef` (the default) | none |
| `readonly` | `noArgWrite`, `noOtherWrite`, `willReturn` |
| `argmemonly` | `noOtherRead`, `noOtherWrite` |
| `argmemonly readonly` | `noArgWrite`, `noOtherRead`, `noOtherWrite`, `willReturn` |
| `writeonly` | `noArgRead`, `noOtherRead` |
| `pure` | bits 0–3, `willReturn` |
| `nothrow`, or a `noexcept` C++ function | `noUnwind`, in addition to the memory bits |

- **Implication** is a bitwise subset test, so every combination has a
  defined order and there is no lattice to get wrong.
- **`pure` and `readonly` imply `willReturn`.** A declared callee that may
  loop forever is a wrong declaration and undefined ([06 §3.3](06-semantics.md)).
- **Freedom to optimize.** A call with `noArgWrite`, `noOtherWrite`,
  `noUnwind` and `willReturn` may be merged with an identical earlier call
  when no write lies between them; with bits 0–5 all set it may also be
  hoisted or removed if unused ([03](03-conversion-and-passes.md)). So CSE
  of a call needs a `readonly` or `pure` declaration (or inference) plus
  `noUnwind`, which `nothrow` gives.
- A wrong guarantee is undefined ([06 §7](06-semantics.md)).

### 4.7 Pointers

```c
struct PointerRecord {   // 24 bytes
	uint8_t  kind;       // 0 callee, 1 capture thunk, 2 destructor, 3 traced address, 4 extension payload address
	uint8_t  mode;       // 0 ProcessLocal, 1 Portable (by name), 2 Portable (ELF build id + offset)
	uint8_t  provenance; // kind 3: 1 AuditedInvariant or 2 RawProcessValue; other kinds: 0
	uint8_t  pad;        // 0
	uint32_t nameId;     // mode 1: symbol name; mode 2: build id as lowercase hex string; mode 0: 0
	uint64_t address;    // mode 0: the address; mode 2: offset from the load bias; mode 1: 0
	uint64_t reserved;   // 0
};
```

- **Kinds and their users.** Kind 0 is the target of an external callee;
  kind 1 the `thunk` of a call (§5.7); kind 2 the target of a destructor
  callee (§4.6); kind 3 the operand of `ptr.const` (§5.8); kind 4 a payload
  word of kind `pointer` of an extension instruction (§5.11.1). No other
  field refers to `pointers`.
- **One encoding for traced addresses** ([07 §3](07-serialization.md)): a
  host address that the traced code uses as a value is a kind-3 entry, never
  a constant. Its provenance is `RawProcessValue` unless the user audited it
  (`AuditedInvariant`). A `RawProcessValue` entry is mode 0.
- `ProcessLocal` (mode 0) entries are valid only in the process that
  produced the module: they make the module process-local, and
  `ModuleRecord.processToken` (§4.4) identifies that process. The loader
  rejects an image whose token differs from its own process's token (fail
  closed).
- `Portable` entries are resolved at load: mode 1 by a `SymbolResolver`,
  mode 2 against the loaded module with that build id
  ([07 §3](07-serialization.md)).
- Pointer entries are interned: equal records are one entry.

### 4.8 Runtime bindings

```c
struct BindingRecord {   // 16 bytes
	uint32_t nameId;     // stable binding symbol; unique within the module
	uint32_t type;       // TypeId of the bound object in bits 0–7 (void: opaque); bits 8–31 are 0
	uint32_t size;
	uint32_t align;      // a power of two
};
```

`bind.addr` (§5.8) yields the address supplied for the binding at load
([07 §4](07-serialization.md)). Bindings with equal schemas stay distinct
identities; bindings are never interned.

### 4.9 Debug data (optional)

```c
struct RegionRecord {    // 16 bytes; one region() call site (docs/region.md)
	uint32_t function;   // function index
	uint32_t nameId;     // user-supplied region name (index in debugNames), or NONE
	uint32_t location;   // index in debugLocations of the call site, or NONE
	uint32_t parent;     // enclosing region index, or NONE
};

struct DebugFunctionRecord { // 16 bytes (section 26); at most one per function
	uint32_t function;   // function index
	uint32_t location;   // index in debugLocations of the definition, or NONE
	uint32_t firstParam; // index in debugParamNames (section 27)
	uint32_t paramCount; // the signature's paramCount, or 0 if no names are recorded
};
```

- The debug sections (15–18, 21, 22, 26, 27) are present only when a debug
  or profiling option was set ([05 §7](05-backends.md)), always together
  (possibly empty), and signalled by feature bit `DEBUG_V1`.
- `debugFunctions` carries 0.3's function source location and input
  argument names; each name is a `debugNames` index, or `NONE` for an
  unnamed parameter.
- Debug data has its own string table (`debugNames`), so it never adds to
  the canonical `names` section.
- Debug data does not change semantics. Consumers must work without it, and
  the structural hash never covers it (§7.2).

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
  without synchronization through external or intrinsic calls, are
  undefined (C++'s data-race rule). NIR 1.0 has no atomic operations in
  core; atomics are intrinsics (the `std` plugin's, [04 §4](04-extensions.md)),
  which synchronize as the C++ atomic operations they implement.

Notation: `a`, `b`, `c` are the record fields; *v* is a value reference,
*blk* a block id, *x* an `extra` offset. "Text" gives the instruction's
text syntax (§9); `%r =` precedes it exactly when the result is not `void`.
Unless stated otherwise, operands have the result type, and flags not
listed are reserved (0).

### 5.1 Arithmetic

| Op | Code | Operands | Result | Flags | Text |
|---|---|---|---|---|---|
| `add` | 1 | a, b: *v* | integer or float: the type of a and b; or `ptr` (a `ptr`, b `i64`) | — | `add a, b` |
| `sub` | 2 | as `add` | as `add` | — | `sub a, b` |
| `mul` | 3 | a, b: *v* | integer or float, the type of a and b | — | `mul a, b` |
| `div` | 4 | a, b: *v*; c: unwind entry if checked, else 0 | integer or float, the type of a and b | integers: bit 0 unsigned, bit 1 checked; floats: none | `div.s a, b`, `div.u a, b`, `div.s.checked a, b unwind`; floats `div a, b` |
| `rem` | 5 | as `div` | as `div` | as `div` | as `div`, with `rem` |
| `fneg` | 6 | a: *v* | float, the type of a | — | `fneg a` |

- Integers wrap modulo 2ⁿ. `ptr` arithmetic adds (subtracts) the `i64` byte
  offset b to (from) the address a, modulo 2⁶⁴; `ptr` − `ptr` goes through
  `ptrtoint`.
- Float arithmetic is IEEE 754 with round-to-nearest-even, without
  fast-math (D11). Flag bits of float operations are reserved for
  fast-math flags in a later version and must be 0.
- `div` and `rem`: §5.10. `fneg` flips the sign bit, also of NaN and zero.

### 5.2 Logical (on `bool`)

| Op | Code | Operands | Result | Text |
|---|---|---|---|---|
| `and` | 8 | a, b: `bool` | `bool` | `and a, b` |
| `or` | 9 | a, b: `bool` | `bool` | `or a, b` |
| `not` | 10 | a: `bool` | `bool` | `not a` |

Both operands of `and` and `or` are already evaluated; there is no short
circuit.

### 5.3 Bitwise (on integers)

| Op | Code | Operands | Result | Flags | Text |
|---|---|---|---|---|---|
| `band` | 12 | a, b: *v* | integer, the type of a and b | — | `band a, b` |
| `bor` | 13 | a, b: *v* | as `band` | — | `bor a, b` |
| `bxor` | 14 | a, b: *v* | as `band` | — | `bxor a, b` |
| `bnot` | 15 | a: *v* | integer, the type of a | — | `bnot a` (0.3's integer `NegateOp`) |
| `shl` | 16 | a: value; b: amount, same type | integer, the type of a | — | `shl a, b` |
| `shr` | 17 | a: value; b: amount, same type | integer, the type of a | bit 0: unsigned (logical); else arithmetic | `shr.s a, b`, `shr.u a, b` |

The shift amount is taken modulo the bit width (D10).

### 5.4 Compare and select

| Op | Code | Operands | Result | Flags | Text |
|---|---|---|---|---|---|
| `cmp` | 20 | a, b: *v*, same scalar type | `bool` | bits 0–2 predicate: 0 `eq`, 1 `ne`, 2 `lt`, 3 `le`, 4 `gt`, 5 `ge` (6, 7 invalid); bit 3: unsigned | `cmp.lt.s a, b`, `cmp.lt.u a, b`, `cmp.eq a, b` |
| `select` | 21 | a: `bool`; b, c: *v*, same type (any non-`void`, extension types included) | type of b and c | — | `select a, b, c` |

- Bit 3 is allowed only for the predicates `lt`, `le`, `gt` and `ge` on
  integers; it must be 0 for `eq` and `ne`, for floats, `bool` and `ptr`.
  The text form prints `.s` or `.u` exactly for ordered integer predicates.
- Float comparisons follow C++: with a NaN operand, `ne` is true and all
  other predicates are false. `-0.0` equals `0.0`.
- `bool` allows only `eq` and `ne`. `ptr` orders as unsigned 64-bit.
- `select` yields b if a is true, else c; both are already evaluated.

### 5.5 Casts

| Op | Code | Operands | Result | Flags bits 0–3: kind | Text |
|---|---|---|---|---|---|
| `cast` | 24 | a: *v* | the type in `type` | 0 `sext`, 1 `zext`, 2 `trunc`, 3 `sitofp`, 4 `uitofp`, 5 `fptosi`, 6 `fptoui`, 7 `fpext`, 8 `fptrunc`, 9 `bitcast`, 10 `ptrtoint`, 11 `inttoptr` (12–15 invalid) | `cast.sext a : i64` |

| Kind | Source | Result | Semantics |
|---|---|---|---|
| `sext`, `zext` | integer or `bool` | integer wider than the source (`bool` counts as 1 bit) | sign- or zero-extends; `bool` gives 0/−1 (`sext`) or 0/1 (`zext`) |
| `trunc` | integer | integer narrower than the source | keeps the low bits. Truncation to `bool` is not a cast; use `cmp.ne a, 0` |
| `sitofp`, `uitofp` | integer | float | the source as signed (unsigned) value, rounded to nearest even |
| `fptosi`, `fptoui` | float | integer | toward zero; **saturates**: NaN gives 0, values outside the result's signed (unsigned) range give its minimum or maximum (D11) |
| `fpext` | `f32` | `f64` | exact |
| `fptrunc` | `f64` | `f32` | rounds to nearest even |
| `bitcast` | `i32`, `f32`, `i64`, `f64` | a type of the same size from that set, different from the source | reinterprets the bits |
| `ptrtoint` | `ptr` | `i64` | the address |
| `inttoptr` | `i64` | `ptr` | the address |

- Every other combination, including a cast to the source type and any
  cast of an extension type, is invalid. Conversions between `ptr` and
  integers use only `ptrtoint` and `inttoptr`, so pointer provenance stays
  visible.

### 5.6 Memory

| Op | Code | Operands | Result | Flags | Text |
|---|---|---|---|---|---|
| `load` | 28 | a: `ptr` | the loaded type (scalar or extension) | bits 0–3: alignment code *k* | `load a : T`, `load a : T align N` |
| `store` | 29 | a: `ptr`; b: the value (scalar or extension type) | `void` | as `load` | `store a, b`, `store a, b align N` |
| `alloca` | 30 | a: function-local alloca slot, in `[0, allocaCount)` | `ptr` | — | `alloca N` |

- **Alignment code.** *k* = 0 means the type's natural alignment (its
  `TypeInfo` alignment for extension types); *k* in 1…13 means a
  guaranteed alignment of 2^(*k*−1) bytes (1 to 4096). A *k* that denotes the
  natural alignment must be written as 0; *k* 14 and 15 are invalid.
- An access through an address that is not valid for the type's size, or
  that is less aligned than the flags state, is undefined. Loading a
  `bool` from a byte that is not 0 or 1 is undefined.
- `alloca` yields the address of the function's static stack slot `a`,
  sized and aligned by its `AllocaRecord`. Every execution in the same
  invocation yields the same address. The slot lives until the invocation
  returns or unwinds; its initial contents are indeterminate.

### 5.7 Calls

| Op | Code | Operands | Result | Text |
|---|---|---|---|---|
| `call` | 32 | a: callee index (§4.6); c: *x* → `[argc, arg₀ … argₙ₋₁, unwind, thunk]` | the signature's result type (may be `void`) | `call #cN(args) unwind thunk #pM` |
| `call.indirect` | 33 | a: `ptr` callee (*v*); b: signature index; c: *x* → `[argc, arg₀ … argₙ₋₁, unwind, thunk, attrs]` | the signature's result type (may be `void`) | `call.indirect a(args) : signature attrs(…) unwind thunk #pM` |
| `funcaddr` | 34 | a: callee index of kind internal, external or imported | `ptr` | `funcaddr #cN` |

- Arguments are value references matching the signature's parameter types;
  `argc` equals the signature's `paramCount`.
- **By callee kind:** an internal call may use the backend's own convention
  (§4.5); an external call calls the native function; an imported call
  loads the import slot and calls the exporter's entry point (one indirect
  call); an intrinsic call is lowered inline by a backend that has a
  lowering, and otherwise calls the fallback. `call.indirect` calls the
  address a with the platform C convention of its signature.
- **`funcaddr` yields a stable address.** For an internal function it is
  the address of a forwarding stub with the function's C convention that
  always reaches the module's current code; for an imported function, a
  stub that calls through the import slot; for an external function, the
  native address. A stub stays valid until the module that took the
  address is released (§4.4.1), across every promotion. Code may store
  the address and call it later. Intrinsics have no address.
- `unwind` is an unwind entry (§3.4). It must be `NO_UNWIND` if the call's
  attributes (the callee's, or `call.indirect`'s `attrs` word) have
  `noUnwind`.
- **Capture thunks.** `thunk` is a `pointers` index of kind capture thunk,
  or `NONE`. A call of an external or intrinsic callee and every
  `call.indirect` whose unwind entry is not `NO_UNWIND` must name a thunk;
  every other call (internal, imported, or with `NO_UNWIND`) has `NONE`,
  because the engine bridges exceptions between Nautilus frames itself.
  Backends without unwind tables call through the thunk when the call may
  throw ([06 §3.2](06-semantics.md)); for an intrinsic, the thunk wraps
  the fallback, and a backend that lowers the intrinsic inline ignores it.
- **Freedom to optimize** follows the guarantee bits of the callee, or of
  `call.indirect`'s `attrs` (§4.6.2).
- **`call.indirect`'s `attrs`** are the guarantees the producer states for
  every possible target; 0 if unknown. 0.3's `IndirectCallOperation`
  attributes map here.
- **A call with `NO_UNWIND` that throws is undefined.** Debug builds detect
  it with a terminate handler ([06 §3.3](06-semantics.md)).
- **Calls inside landing pads** must have `NO_UNWIND` or `NO_CLEANUP`. An
  exception that escapes a destructor while a landing pad runs terminates
  the process, as in C++.

### 5.8 Runtime state, data and traced addresses

| Op | Code | Operands | Result | Text |
|---|---|---|---|---|
| `bind.addr` | 36 | a: binding index (§4.8) | `ptr` | `bind.addr #bN` |
| `data.addr` | 37 | a: data object index (§4.10) | `ptr` | `data.addr #dN` |
| `ptr.const` | 38 | a: `pointers` index of kind traced address (§4.7), or `NONE` for the null pointer | `ptr` | `ptr.const #pN`, `ptr.const null` |

- `ptr.const` yields the address its entry denotes: the recorded address
  (mode 0), the resolved symbol (mode 1), or the load bias plus the offset
  (mode 2). It is pure: two `ptr.const` of the same entry yield the same
  value and may be merged. It is never folded into a constant (§4.2).
- **Loads from data fold.** A `load` whose address is `data.addr d` plus a
  constant byte offset, entirely within `d` and aligned as the load states,
  reads the known bytes and may be replaced by a constant. A load outside
  the object is undefined.

### 5.9 Values and control flow

| Op | Code | Operands | Result | Notes | Text |
|---|---|---|---|---|---|
| `blockarg` | 40 | a: argument position | any non-`void` type | SSA: only the first `argCount` instructions of a block, with `a` equal to the instruction's index in the block; pre-SSA: only the first `paramCount` instructions of the function, with `a` equal to the parameter index | in the block header: `^bN(%r: T, …):` |
| `br` | 48 | a: target *blk*; c: *x* → `[argc, args…]` | `void` | terminator | `br ^bN(args)` |
| `condbr` | 49 | a: `bool` condition; b: true *blk*; c: *x* → `[prob, trueArgc, trueArgs…, falseBlk, falseArgc, falseArgs…]` | `void` | terminator | `condbr a, ^bT(args), ^bF(args) prob P` |
| `ret` | 50 | a: value, or `NONE` for a `void` function | `void` | terminator | `ret a`, `ret` |
| `resume` | 51 | — | `void` | terminator; only in landing pads; continues unwinding the current exception | `resume` |
| `assign` | 56 | a: variable (value id); b: new value | `void` | **Pre-SSA level only** (§6.2) | `assign %v, b` |
| `var` | 57 | a: *v*, a value or a constant | the type of a | **Pre-SSA level only** (§6.2): defines a new variable initialized to a | `var a` |

- `prob` is the probability that the condition is true, as
  `round(P × 0xFFFFFFFE)`; `0xFFFFFFFF` means unknown. `trueBlk` may equal
  `falseBlk`; the two argument lists are still separate.
- At the pre-SSA level, argument counts are always 0.
- `var` exists so that every variable has a defining instruction: the
  tracer records a constant or a copy that may later be assigned as
  `var` ([01 §6](01-tracing-frontend.md)). The conversion forwards `var` to
  its operand and emits nothing for it ([03 §3](03-conversion-and-passes.md)).
- Codes 1–255 not listed here are reserved for core opcodes of later minor
  versions (§8.2). NIR 1.0 has no `unreachable` terminator: tracing always
  ends a block with one of the terminators above.

### 5.10 Division and remainder

- **Floats:** IEEE. Division by zero gives ±∞ or NaN and never throws;
  float `rem` is C++ `fmod`. Flag bits 0 and 1 are 0 and `c` is 0.
- **Integers**, bit 0 selecting unsigned or signed:
  - if the divisor is 0, or (signed) the dividend is the type's minimum and
    the divisor −1, the instruction **throws
    `nautilus::ArithmeticException`** (D10);
  - otherwise the quotient rounds toward zero, and `rem` takes the sign of
    the dividend, as in C++.
- **The checked bit.** An integer `div`/`rem` whose divisor is not proven
  safe must set bit 1 and carry an unwind entry other than `NO_UNWIND` in
  `c`. A divisor is safe if it is a constant other than 0, and for signed
  operations also other than −1. With bit 1 clear, `c` is 0 and the
  instruction cannot throw. An instruction with bit 1 clear and an unsafe
  divisor at run time is undefined. The checked bit is a property of the
  instruction, not of the backend: every backend implements it from M4 on
  ([06 §4](06-semantics.md)).
- **Pre-SSA level.** The tracer records every integer `div`/`rem` with bit
  1 set and the current cleanup list in `c` (§3.4), because whether the
  divisor is safe is known only after folding. The conversion clears the
  bit and sets `c` to 0 when it proves the divisor safe.
- **How a failing check throws.** A failing check behaves exactly as a call
  of the runtime helper `nautilus_rt_throw_arith` ([06 §4](06-semantics.md))
  with the instruction's unwind entry. Backends emit the check as a compare
  and branch to a cold path that makes that call; backends without unwind
  tables make it through the runtime's own capture thunk, so the exception
  reaches the landing pad as from any may-throw call. The helper and its
  thunk are part of the runtime, never `pointers` entries.

### 5.11 Extension operations

- An extension instruction (`op` ≥ 256) uses the same record. Its operand
  fields and `extra` payload follow its **operand shape** (§5.11.1); its
  types and flags are defined by its `OpInfo` ([04 §3.3](04-extensions.md))
  and checked by its `Signature` and `verify` hook after the core checks.
- It produces at most one value. An operation with several results returns
  an extension aggregate type, or is split into several operations.
- Extension operations are never terminators and never throw; an operation
  that may fail calls a native function, which may throw like any call. An
  operation with call semantics is an intrinsic (§4.6.1).
- Its effects come from `OpInfo.effects` and take part in §5.0's ordering.
- Text: `%r = <name> flags(F) f₀, f₁, … : T` (§9.2), printed generically
  from the operand shape. `OpInfo.print` may only add a trailing comment.

#### 5.11.1 Operand shape

Every extension operation declares an operand shape in `OpInfo.shape`. The
core verifier, canonicalizer, loader, printer and parser use only the
shape, never plugin code, to bounds-check, renumber and compact an
extension instruction:

- **Fields.** For each of `a`, `b` and `c` a field kind: `unused` (must be
  0), `value` (a value reference), `const` (a value reference that must be
  a constant), `imm` (a `u32` immediate), or, for `c` only, `list` (an
  `extra` offset).
- **Payload.** A `list` is `[n, w₀ … wₙ₋₁]`. The shape gives a fixed prefix
  of word kinds and an optional repeated group of word kinds; `n` must
  equal the prefix length plus *k* times the group length for some *k* ≥ 0
  (*k* = 0 without a group). Word kinds: `value`, `const`, `imm`, `pointer`
  (a `pointers` index of kind extension payload address) and `type` (a
  type in bits 0–7, upper bits 0).
- **Canonicalization** renumbers `value` and `type` words, orders constants
  and pointers through `const`, `value` and `pointer` words (§7.1), and
  copies `imm` words unchanged.
- An extension instruction never holds block ids, callee indices, unwind
  entries, or table indices other than through `pointer` and `type` words.

## 6. Levels and invariants

### 6.1 Both levels

1. Every function has at least one block. Every block ends in exactly one
   terminator; terminators appear nowhere else.
2. Every value reference points to an instruction with a non-`void` result
   in the same function, or to an existing constant.
3. Operand and result types follow §2 and §5; reserved flag bits, unused
   operand fields and padding are 0.
4. Branch argument counts and types match the target block's `blockarg`s.
5. Every table index (callee, signature, alloca, binding, pointer, data
   object, name, extension, import, `extra` offset) is in range, and every
   `extra` list and data object is in bounds of its section; lists do not
   overlap (§3.3).
6. A function's `ret` operands match its signature's result type.
7. Calls match their signatures; a call whose attributes have `noUnwind`
   has `NO_UNWIND`; a may-throw call of an external or intrinsic callee and
   every may-throw `call.indirect` names a capture thunk, and all other
   calls have `thunk = NONE` (§5.7). `funcaddr`'s callee is not an
   intrinsic.
8. Exactly one `module` record. Function names are unique and non-empty.
   Callee and import signatures match §4.6; a callee's attributes for an
   internal function equal the function's under `GUARANTEE_MASK`. Reserved
   attribute bits are 0. Exported signatures use only types with a C ABI
   (§4.5).
9. Imports are unique by `(module, function)` and never name the module
   itself. A deserialized image has no imports (§8.5).
10. Type words follow §4.5.1; other type fields have bits 8–31 equal to 0.
11. Constants follow §4.2 (no `ptr` type, `bool` 0 or 1, unused bits 0,
    provenance 0–2, `byteOffset` exactly for types wider than 8 bytes).
    Pointer entries follow §4.7 (kind and mode in range, provenance as the
    kind requires, `RawProcessValue` only in mode 0, fields unused by the
    mode 0). `processToken` is non-zero exactly when the module is
    process-local (§4.4).
12. Binding names are unique. Data objects and allocas have size > 0 and a
    power-of-two alignment of at most 4096; `byteOffset` is aligned.
13. Function string attributes are sorted, unique per function, and have a
    `<plugin>.<key>` key (§4.5.2). Destructor callees have signature
    `(ptr) -> void`.

### 6.2 Pre-SSA level

- **Variables.** A value id may be redefined by `assign`. Its operand `a`
  must be the id of an earlier `var`, `blockarg` or other value-producing
  instruction (the variable's definition), and `b` must have the same type.
  A use of a variable sees the value of the most recent `assign` (or the
  defining instruction) on the path executed so far. Every use must be
  reached by a definition on every path. `a` is never a constant.
  Parameters are variables like any other and may be assigned.
- **`var`** defines a new variable whose initial value is its operand (a
  value or a constant). It is how a constant or a copy that may later be
  assigned gets an id ([01 §6](01-tracing-frontend.md)).
- **No block arguments** except the function parameters, and branch
  argument lists are empty. Merges are expressed through variables.
- **No landing pads.** Instructions that may throw carry cleanup lists
  (§3.4). Every integer `div`/`rem` has the checked bit and a cleanup list
  (§5.10). Cleanup-list entries name destructor callees (§4.6) and `ptr`
  values.
- Blocks are in recording order and list their instructions in `extra`
  (§4.3).
- Snapshots, the tag map and the replay cursor are derived data
  ([01 §6](01-tracing-frontend.md)).
- A pre-SSA function is never executed and never passed to a backend;
  the conversion ([03](03-conversion-and-passes.md)) is the only way to the
  SSA level.

### 6.3 SSA level

- No `assign` and no `var`. Every value is defined exactly once.
- **Entry block.** Block 0 is not a landing pad; its `argCount` and the
  types of its `blockarg`s equal the signature's parameters.
- **Dominance:** every use of a value is dominated by its definition. A use
  as a branch argument counts at the end of the predecessor block.
- **Unwind edges.** An unwind entry is a CFG edge from the instruction that
  carries it to the pad. A value used in a pad must be defined in a block
  that dominates every block containing an instruction whose unwind entry
  names that pad (or whose `br` enters it, for a pad reached from another
  pad), and, within such a block, before each such instruction. The
  throwing instruction's own result is not available in the pad.
- **Layout:** block 0 is the entry block. The other blocks that are not
  landing pads follow in an order in which every forward edge (an edge
  whose target does not dominate its source) goes from an earlier to a
  later block, and every **reducible** loop's blocks are contiguous.
  Irreducible regions (strongly connected subgraphs with more than one
  entry) keep the conversion's plain reverse post-order and are exempt from
  both rules ([03 §3.1](03-conversion-and-passes.md)). Landing pads come
  after all other blocks, in the order of their first reference by an
  unwind entry or a pad's `br`, scanning instructions in id order.
- Every block is reachable from the entry block, landing pads through
  unwind entries.
- **Landing pads.** A pad has `argCount` = 0. It is entered through unwind
  entries, or by `br` from another pad; no other block branches to a pad.
  A pad ends in `resume` or in `br` (with no arguments) to another pad;
  `condbr` and `ret` do not occur in pads, and the `br` edges between pads
  form no cycle. A pad's destructor calls are `call`s of destructor callees
  (§4.6) with `NO_UNWIND` (for a `noUnwind` destructor) or `NO_CLEANUP`.
- Unwind entries are `NO_UNWIND`, `NO_CLEANUP` or landing-pad block ids,
  never cleanup lists.

### 6.4 The verifier

The verifier checks §3, §4, §5 and the invariants of each function's level,
every extension instruction against its operand shape (§5.11.1), and then
against its `Signature` and `verify` hook. It runs on every module that
comes from a deserializer, the text parser or the C API, and in debug
builds after every producer. A module that fails verification is never
passed to a backend. Each diagnostic code (§6.6) has a positive and a
negative test ([12](12-validation.md)).

### 6.5 Concurrency

- A finished SSA-level module is **immutable**. Any number of threads may
  read, compile, hash, serialize or inline from it concurrently.
- Building and rewriting a module are single-threaded per module.
- The module registry is thread-safe: registering, looking up, linking and
  releasing modules may happen concurrently. Linking pins exporters under the
  registry's lock (§4.4.2).
- An import slot is written with a release store and read with an acquire
  load (or a load the target orders equivalently).
- The extension registry is frozen when the first `NautilusEngine` is
  constructed, with a release store; afterwards it is read-only and
  lock-free ([04 §3.7](04-extensions.md)).

### 6.6 Diagnostic codes

Every check has a stable code: `NIR-V` for the verifier, `NIR-L` for the
loader, `NIR-P` for the text parser. Codes are never renumbered or reused
within a major version; `Diagnostics.def` lists the same codes
([15](15-agent-experience.md)). A diagnostic names the code, the function,
and the instruction or block id (as printed in the text form, §9).

| Code | Rule | Check |
|---|---|---|
| `NIR-V001` | §6.1.1 | a function has at least one block |
| `NIR-V002` | §6.1.1 | a block ends in exactly one terminator; no terminator elsewhere |
| `NIR-V003` | §3.1 | the opcode is a core opcode of this version or a registered extension |
| `NIR-V004` | §3.1, §6.1.3 | reserved flag bits, unused operand fields and padding are 0 |
| `NIR-V005` | §3.2, §6.1.2 | a value reference is in range, not reserved, and names a non-`void` instruction of the same function or a constant |
| `NIR-V006` | §5, §6.1.3 | operand and result types match the opcode's table |
| `NIR-V007` | §6.1.5 | a table index is in range |
| `NIR-V008` | §3.3, §6.1.5 | an `extra` list is in bounds and does not overlap another |
| `NIR-V009` | §6.1.4 | branch argument count and types match the target's `blockarg`s |
| `NIR-V010` | §6.1.6 | `ret` matches the signature's result |
| `NIR-V011` | §5.7 | call arguments match the signature |
| `NIR-V012` | §3.4 | the unwind entry has a form allowed at the function's level |
| `NIR-V013` | §5.7 | a call with `noUnwind` has `NO_UNWIND` |
| `NIR-V014` | §5.7 | a thunk is present exactly where required, and is a capture-thunk entry |
| `NIR-V015` | §5.7 | `funcaddr` does not name an intrinsic |
| `NIR-V016` | §5.10 | `div`/`rem` flags: checked only on integers, with an unwind entry other than `NO_UNWIND`; `c` = 0 otherwise |
| `NIR-V017` | §5.4 | `cmp` predicate in range; unsigned bit only on ordered integer predicates; `bool` only `eq`/`ne` |
| `NIR-V018` | §5.5 | the cast kind allows the source and result types |
| `NIR-V019` | §5.6 | the alignment code is valid and not an explicit natural alignment |
| `NIR-V020` | §5.9 | `blockarg` position and placement |
| `NIR-V030` | §6.1.8 | exactly one `module` record |
| `NIR-V031` | §6.1.8 | function names are unique and non-empty |
| `NIR-V032` | §4.6 | a callee's signature equals its target's |
| `NIR-V033` | §4.6 | an internal callee's attributes equal the function's under `GUARANTEE_MASK` |
| `NIR-V034` | §4.6.2 | reserved attribute bits are 0 |
| `NIR-V035` | §4.5 | an exported signature uses only types with a C ABI |
| `NIR-V036` | §4.5.1 | type words and extension kinds are valid |
| `NIR-V037` | §6.1.9 | imports are unique and do not name the module itself |
| `NIR-V038` | §8.5 | a deserialized image has no imports |
| `NIR-V039` | §4.2 | constant records are valid |
| `NIR-V040` | §4.7 | pointer records are valid; `processToken` is set exactly when needed |
| `NIR-V041` | §4.8 | binding names are unique; binding records are valid |
| `NIR-V042` | §4.10 | data objects are valid |
| `NIR-V043` | §4.5 | allocas are valid |
| `NIR-V044` | §4.5.2 | function string attributes are sorted, unique and prefixed |
| `NIR-V045` | §4.6 | destructor callees have signature `(ptr) -> void` |
| `NIR-V046` | §8.4 | extension records are valid and their names parse |
| `NIR-V050` | §6.2 | `assign` targets a variable definition of the same type, never a constant |
| `NIR-V051` | §6.2 | every use is reached by a definition on every path |
| `NIR-V052` | §6.2 | no block arguments except parameters; branch argument lists are empty |
| `NIR-V053` | §6.2 | no landing-pad flags |
| `NIR-V054` | §4.3 | every instruction belongs to exactly one block |
| `NIR-V055` | §5.10 | every integer `div`/`rem` is checked and has a cleanup list |
| `NIR-V056` | §3.4 | cleanup-list entries name destructor callees and `ptr` values |
| `NIR-V060` | §6.3 | no `assign` or `var` |
| `NIR-V061` | §6.3 | dominance, including over unwind edges |
| `NIR-V062` | §6.3 | layout: forward edges, reducible loops contiguous, pads last and in order |
| `NIR-V063` | §6.3 | every block is reachable |
| `NIR-V064` | §6.3 | landing-pad entry, arguments and exits |
| `NIR-V065` | §5.7, §5.9 | calls in pads have `NO_UNWIND` or `NO_CLEANUP`; `resume` only in pads |
| `NIR-V066` | §6.3 | the entry block's arguments equal the signature's parameters |
| `NIR-V067` | §4.3 | blocks are contiguous and cover the function's instructions |
| `NIR-V070` | §5.11.1 | an extension instruction matches its operand shape |
| `NIR-V071` | §5.11 | an extension instruction passes its `Signature` and `verify` hook |
| `NIR-V072` | [04 §3.3](04-extensions.md) | a target-restricted extension operation is valid for the compile target |
| `NIR-L001` | §8.1 | magic |
| `NIR-L002` | §8.2 | major version higher than the reader's |
| `NIR-L003` | §8.2 | unknown `incompatFeatures` bit |
| `NIR-L004` | §8.1 | `totalSize` differs from the buffer length |
| `NIR-L005` | §8.1 | checksum |
| `NIR-L006` | §8.1 | section table: order, duplicates, alignment, bounds, overlap |
| `NIR-L007` | §8.1 | stride below the record size, or a byte or `u32` section with another stride |
| `NIR-L008` | §8.1 | a configured limit is exceeded |
| `NIR-L009` | §8.1 | the `module` section is missing |
| `NIR-L010` | §8.4 | an extension is missing in the registry |
| `NIR-L011` | §8.4 | an extension version is outside the accepted range |
| `NIR-L012` | §8.4 | an intrinsic callee's signature differs from the registered one |
| `NIR-L013` | §8.4 | an extension type instance is unavailable on this host |
| `NIR-L014` | §4.7 | the process token differs from this process's |
| `NIR-L015` | §4.7 | a portable pointer does not resolve |
| `NIR-L016` | §8.1 | a pre-SSA function in an image loaded for execution |
| `NIR-L017` | §8.1 | the verifier rejects the loaded module (the diagnostic also carries the `NIR-V` code) |
| `NIR-P001` | §9.1 | lexical error |
| `NIR-P002` | §9.2 | syntax error |
| `NIR-P003` | §9.4 | undefined or out-of-order reference (`%N`, `^bN`, `#…`, `@…`) |
| `NIR-P004` | §9.4 | a printed id differs from the position it denotes |
| `NIR-P005` | §9.4 | unknown extension name, or a version the registry does not accept |
| `NIR-P006` | §9.1 | a literal out of range for its type |

Rule numbers such as §6.1.8 denote item 8 of §6.1. Codes 21–29, 47–49,
57–59, 68–69 and 73–99 of each class are reserved for later rules.

## 7. Canonical form

A canonical module is the form that is serialized and hashed
([02 §3.6](02-nir.md), [08](08-caching.md)). **Canonical form, hashing,
caching and bundling (§8.5) apply to modules whose functions are all at the
SSA level.** A module with pre-SSA functions is serialized as it is, for
debugging and reproducers only; it is never hashed, cached or executed.

**Structural equality.** Two SSA-level modules are *structurally equal*
when they differ only in: the order and indices of table entries
(constants, signatures, callees, pointers, names, imports, data objects,
bindings, extension entries, and allocas within a function); duplicate
entries of interned tables; unused table entries and unreferenced internal
functions; the order of internal functions; the placement of `extra` lists;
pending replacements; padding, unused fields and reserved bits; process
ids of extensions (as long as they denote the same `name@version`); and
the debug sections. Value ids and block order are part of the structure:
they follow from the layout (§6.3), which canonicalization does not change.

**Canonicalization** is a deterministic, idempotent function of the module.
Structurally equal modules have byte-identical canonical images.

1. **Compact.** Apply pending replacements and compact each function, so
   ids are dense and in layout order.
2. **Order functions.** First the exported functions, sorted by name. Then,
   walking that list and every function appended to it in order, scan each
   function's instructions in id order and append the target function of
   every `call` and `funcaddr` whose callee is internal and whose target is
   not yet listed. Internal functions that are never appended are dropped.
3. **Scan.** Visit every reference in the scan order of §7.1. A table entry
   gets the next index of its table at its first visit; entries never
   visited are dropped.
4. **Intern** during the scan: when an entry is visited for the first time,
   after its own fields are visited, it takes the index of an equal entry
   already placed in that table, if any. Equality: names by bytes;
   constants by `(type, provenance, value bytes)`; signatures by their type
   words; callees, pointers and imports by all their fields (after
   renumbering); data objects by `(size, align, contents)`. Bindings,
   allocas and functions are never interned.
5. **Write the byte sections.** `nameBytes`: each name's bytes in `names`
   order, each new name at the next offset, no padding, no sharing of
   substrings. `constBytes`: the bytes of each constant wider than 8 bytes,
   in constant order, each at the next offset aligned to its type's
   alignment (at most 16), zero-padded. `dataBytes`: each data object in
   data order, at the next offset aligned to its alignment, zero-padded.
6. **Number extensions.** The `extensions` section holds the entries in
   first-visit order, all kinds together; image opcodes and types follow
   from it (§8.4).
7. **Write `extra`** without gaps: first the parameter lists of the
   signatures in signature order (a signature with `paramCount` = 0 has
   `paramTypes` = 0 and no words), then, per function in function order and
   per instruction in id order, the instruction's list. Every `br`,
   `condbr`, `call` and `call.indirect`, and every extension instruction
   with a `list` field, writes its own list, including the count word when
   the count is 0.
8. **Write records.** Zero all padding, unused fields and reserved bits;
   sort `functionAttrs` by `(function, key bytes)`; lay out the image as
   §8.1 requires for canonical images.

### 7.1 Scan order

The scan visits, in this order:

1. For each function in canonical order:
   1. its `nameId`, then its signature;
   2. its function string attributes, sorted by key bytes: key, then value;
   3. its instructions in id order. Per instruction: the extension opcode
      (if `op` ≥ 256), the `type` field (if an extension type), then `a`,
      `b`, `c`, then the words of its `extra` list in order. Each field and
      word is interpreted as §5 (or the operand shape, §5.11.1) defines it:
      a constant reference visits the constant; a callee, signature,
      binding, data or pointers index visits that entry; an `alloca` slot
      visits the function's alloca; block ids, value ids and immediates
      visit nothing.
2. The `module` record's `nameId`.

**Visiting an entry** for the first time visits its fields in declaration
order, before it is placed:
- constant: its type (if an extension type);
- signature: the result, then each parameter (extension types);
- callee: the target (function: nothing, it is already ordered; pointer;
  import; intrinsic: its extension entry), then the signature;
- import: `moduleNameId`, `functionNameId`, then the signature;
- pointer: `nameId` (modes 1 and 2);
- binding: `nameId`, then the type;
- extension entry: `nameId`;
- data object, alloca, name: nothing.

If debug sections are kept (serialization with `DEBUG_V1`; never for the
hash), their records keep their relative order with unreferenced entries
dropped, are renumbered for the canonical instruction and function ids,
and `debugNames` is ordered by first use in section order 15, 17, 26, 27.

### 7.2 Structural hash

The **structural hash** (the L2 key, [08](08-caching.md)) is
`XXH3_128bits_withSeed(input, length, 0)`, where `input` is the
concatenation of:

1. **The image.** Bytes `[48, totalSize)` of the canonical image (§7, §8.1)
   of the module canonicalized **as if it were anonymous** (the `module`
   record's `nameId` = `NONE`, so the name is in `names` only if something
   else uses it) and **without debug sections** (`DEBUG_V1` clear). The
   48-byte header is excluded; the section table and all section data are
   included.
2. **The options block.** A `u32` entry count, then one entry per option
   whose `OptionSpec` has the `affectsCode` flag (the option registry of
   M1.4a, [docs/options.md](../../options.md)), with its **effective
   value**: the registry default overlaid with the value set by the user,
   so setting an option to its default does not change the hash. Entries
   are sorted by name bytes; each is `u32 nameLength, name bytes, u8 kind
   (0 bool, 1 i64, 2 f64, 3 string), value` (bool: one byte 0 or 1; i64 and
   f64: eight bytes, f64 by bit pattern, integer options widened to i64;
   string: `u32 length` and bytes). Options without `affectsCode` and
   unknown keys are not hashed.

Extension names and versions are covered by item 1, since every
`name@version` in use is in `names`. The **key** is the 16-byte result:
the low 64 bits, then the high 64 bits, each little-endian. It is a cache
key, not a security property; a hit is confirmed by comparing the images.

- **A serialized image** is self-contained (§8.5): its hash covers the
  bundled bodies, so persistent entries (L1, L2) of an importer change when
  an exporter it imports changes.
- **The structural hash of a process-local module** covers its
  `processToken` and mode-0 addresses, so it only ever matches in the
  producing process.

### 7.3 Derived keys

- **In memory (the L0 key)** the module is not bundled: item 1 is the
  image the module would have after §7 without bundling (§8.5), with its
  `imports` section holding the import records in first-use order (callees
  of kind imported stay imported) and its pointers in whatever mode they
  have. Imports are thus hashed by name, signature and assumed attributes,
  and an importer's key does not change when the exporter's body does. This
  form is a hash input only; it is never loaded (rule §6.1.9). L0 and L2
  keys of the same module differ when it imports or has mode-0 pointers.
- **Compiled code that inlined imports** (§4.4.3) is keyed by
  `XXH3_128bits_withSeed` over the module's key (16 bytes), a `u32` count,
  and per inlined exporter, sorted by name bytes, `u32 nameLength`, the
  name bytes and the exporter's 16-byte structural hash.

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
	uint64_t count;           // number of records (bytes for byte sections), < 2³²
};
```

- Section entries appear in increasing `kind` order, each kind at most
  once. Section data does not overlap and is padded with zeros to 8 bytes.
- A reader uses the fields it knows and skips the rest of each record, so a
  later minor version can append fields by widening `stride`. Byte sections
  have stride 1 and `u32` sections stride 4, always.
- **Canonical images** (§7) list every canonical section kind (1–14, 19,
  20, 23, 24, 25), including empty ones, and the optional debug sections
  exactly when `DEBUG_V1` is set; no vendor sections. Each `stride` is this
  spec's record size. Section data starts right after the table (offset
  48 + 24 × `sectionCount`, which is 8-byte aligned), in table order, each
  section padded with zeros to a multiple of 8; an empty section has
  `count` 0 and the `offset` where the next section's data starts.
  `totalSize` is the end of the last section's padding. `reserved` is 0.
  `compatFeatures` and `incompatFeatures` have exactly the bits §8.3
  requires for the module.
- **Loading** validates, in this order, and rejects the image on the first
  failure with the `NIR-L` code of §6.6: the magic, the version and feature
  bits, `totalSize` (it must equal the buffer length), the checksum, the
  limits, the section table (order, alignment, bounds, overlap, strides),
  every reference, the process token and portable pointers, the extensions
  (§8.4), then the verifier. A malformed image must never cause undefined
  behavior.
  - Sections absent from the table are empty; `module` must be present.
  - Unknown kinds below 1024, and all kinds from 1024 up, are skipped.
    (A new section that readers must understand comes with an
    `incompatFeatures` bit, which already rejects the image, §8.2.)
  - A known section whose `stride` equals this spec's record size is used
    in place; one with a wider stride is copied into a compact array.
    Images with extensions are rewritten once (§8.4).
  - An image with a pre-SSA function loads only through the debugging API
    (`inspectModule`, the text printer); `loadModule` rejects it
    (`NIR-L016`).
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
| New diagnostic code | minor |
| Changed meaning or encoding of an existing field, opcode or rule | major |

- Within 0.4.x, images stay loadable (D12): patch releases do not change
  the format, and minor versions only add.
- Opcodes, section kinds, feature bits and diagnostic codes are never
  renumbered within a major version. The C API's enums map one to one
  ([09](09-c-api.md)).
- A reader rejects an image with a higher major version, or with an
  `incompatFeatures` bit it does not know.

### 8.3 Feature bits (1.0)

| Bit | Name | Class | Meaning |
|---|---|---|---|
| 0 | `DEBUG_V1` | compat | the debug sections 15–18, 21, 22, 26 and 27 are present |
| 1 | `EXTENSIONS` | incompat | the `extensions` section is not empty: the module uses extension operations, types or intrinsics |
| 2 | `PROCESS_LOCAL` | incompat | the module is process-local (`ProcessLocal` pointers or `RawProcessValue` constants); `ModuleRecord.processToken` is set (§4.4) |

### 8.4 Extensions in images

```c
struct ExtensionRecord { // 8 bytes
	uint32_t nameId;     // "name@version", e.g. "simd.add@1"
	uint8_t  kind;       // 0 operation, 1 type, 2 intrinsic
	uint8_t  pad[3];     // 0
};
```

- Image opcode 256 + *i* is the *i*-th entry of kind 0; image type 16 + *j*
  is the *j*-th entry of kind 1 (so at most 240 type entries). Intrinsic
  callees (kind 2) refer to their entry by its index in the section.
- **Names** are `<plugin>.<name>`, optionally followed by a parameter list
  in angle brackets for a type instance (`simd.vec<f32,8>`); the version is
  a decimal `u32` after the **last** `@`. Each `name@version` occurs at
  most once.
- **Loading maps them to process ids** in one pass over `insts`, the type
  fields of all records and the `type` words of extension payloads. An
  image without extensions is used in place; an image with extensions is
  rewritten once while loading.
- Each extension declares a `version` and a `minCompatibleVersion`
  ([04 §3.2](04-extensions.md)). An image entry is accepted if its version
  lies in that range. A missing extension or a version outside the range
  rejects the image.
- **Type instances** are registered by their plugin before the registry
  freezes, including instances that depend on the CPU; an instance the host
  cannot execute is registered as unavailable. An image that uses an
  unavailable instance is rejected (`NIR-L013`); the caller recompiles from
  its source.

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
   calls and its function string attributes.
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

### 8.6 Reuse of the framing

The header layout and the section table of §8.1 are also the framing of
the artifact container ([07 §2.2](07-serialization.md)), with its own magic
(`"NART"`), version and section kinds. Changes to `ImageHeader` or
`SectionEntry` are therefore changes to both formats and need a major
version of both.

## 9. Text form

The printer writes a text form for dumps, tests and bug reports, and 0.4
also **parses** it, so tests can be written as `.nir` text
([15 §3.3](15-agent-experience.md)). The text form is a view of the
in-memory module, not a second format: every record of §4 and every
opcode of §5 has a spelling, except vendor sections (≥ 1024), which are not
printed. The grammar (§9.2) is normative; printers and parsers of other
implementations must accept and produce exactly this language.

```
nir 1.0
func export @divideFirst : (ptr, i64) -> i64 {
^b0(%0: ptr, %1: i64):
  %2 = cmp.lt.s %1, 0:i64
  condbr %2, ^b1, ^b2(%1) prob 429496729
^b1:
  ret 0:i64
^b2(%5: i64):
  %6 = load %0 : i64
  %7 = div.s.checked %6, %5 unwind caller
  ret %7
}
```

`%N` is the value's id as defined in §1, the index of its instruction, so
the ids of void instructions (`condbr` is 3, the first `ret` is 4, the last
is 8) do not appear. Diagnostics use the same numbers.

A module with tables, a destructor and a landing pad:

```
nir 1.0
module "nes.q1"
pointer #p0 = callee symbol "process"
pointer #p1 = thunk symbol "process$thunk"
pointer #p2 = destructor symbol "Buffer::~Buffer"
callee #c0 = external #p0 : (ptr) -> i64
callee #c1 = external #p2 : (ptr) -> void attrs(noUnwind)

func export @run : (ptr) -> i64 {
  alloca 16 align 8
^b0(%0: ptr):
  %1 = alloca 0
  %2 = call #c0(%1) unwind ^b1 thunk #p1
  call #c1(%1)
  ret %2
^b1 pad:
  call #c1(%1)
  resume
}
```

### 9.1 Lexical structure

- The text is UTF-8. Whitespace (space, tab, CR, LF) separates tokens and
  is otherwise insignificant, except that a **constant** (`5:i64`,
  `1.5:f64[audited]`) and an **extension type** (`!simd.vec<f32,8>`) are
  single tokens with no whitespace inside. A comment runs from `;` to the
  end of the line.
- `decimal` is `[0-9]+`; `hex` is `0x[0-9a-fA-F]+`; `uint` is either.
- `number` is an optional `-` followed by a decimal or hex integer, a
  decimal floating literal (`1.5`, `1e-3`, as `std::from_chars` with
  `chars_format::general` accepts it), or a hex floating literal
  (`0x1.8p+1`, as `std::from_chars` with `chars_format::hex` accepts it
  after the `0x`). Its meaning depends on the type it is used with (§9.4).
- `fraction` is a decimal floating literal in [0, 1].
- `string` is `"…"` with the escapes `\\`, `\"` and `\xHH`; other bytes
  stand for themselves.
- `bytes` is `x"…"` with an even number of hex digits, in memory order.
- References: `%N` value, `^bN` block, `#cN` callee, `#pN` pointer, `#iN`
  import, `#bN` binding, `#dN` data object, `#lN` location, `#rN` region
  (N decimal).
- `fname` is `@` followed by a bare name `[A-Za-z_$][A-Za-z0-9_$.]*` or a
  string (`@"lib.math::hash"`).
- `extname` is a bare name with at least one `.`, whose first component is
  a plugin name; plugin names must not be one of the core mnemonics of §5
  or `nir`, `core` (the registry rejects them).
- `exttype` is `!` followed by an `extname`, optionally followed by a
  balanced `<…>` group; the token without the `!` is the `TypeInfo` name.
- Mnemonics are bare names that may contain dots (`div.s.checked`); the
  grammar's dotted terminals match whole mnemonic tokens.

### 9.2 Grammar

EBNF: terminals in quotes, `{ }` zero or more, `[ ]` optional, `|` choice;
lexical classes as in §9.1.

```ebnf
module    = "nir" version [ "module" string ] { decl } { function } ;
version   = decimal "." decimal ;                            (* one token: "1.0" *)

decl      = "extension" string                                    (* "name@version" *)
          | "import" iref "=" string "::" string ":" signature [ attrs ]
          | "pointer" pref "=" pkind pmode [ "audited" | "raw" ]
          | "binding" bref "=" string ":" type "size" uint "align" uint
          | "data" dref "align" uint "=" bytes
          | "callee" cref "=" ctarget ":" signature [ attrs ]
          | "location" lref "=" string ":" decimal ":" decimal
          | "region" rref "=" fname [ "name" string ] [ "at" lref ] [ "in" rref ] ;
pkind     = "callee" | "thunk" | "destructor" | "address" | "payload" ;
pmode     = "local" hex | "symbol" string | "image" string "+" hex ;
ctarget   = "internal" fname | "external" pref | "imported" iref | "intrinsic" extname ;

function  = "func" [ "export" ] [ "pressa" ] fname ":" signature [ attrs ]
            "{" { fitem } block { block } "}" ;
fitem     = "alloca" uint "align" uint
          | "attr" string "=" string
          | "debug" [ "at" lref ] [ "params" "(" pname { "," pname } ")" ] ;
pname     = string | "?" ;
block     = blk [ "(" barg { "," barg } ")" ] [ "pad" ] ":" { inst } ;
barg      = vid ":" type ;
inst      = [ vid "=" | "[" decimal "]" ] op [ "loc" lref ] [ "region" rref ] ;

op        = binary | unary | divrem | shr | cmp | select | cast | load | store
          | alloca | call | icall | funcaddr | addr | br | condbr | ret | "resume"
          | assign | var | extop ;
binary    = ( "add" | "sub" | "mul" | "and" | "or" | "band" | "bor" | "bxor" | "shl" )
            operand "," operand ;
unary     = ( "fneg" | "not" | "bnot" ) operand ;
divrem    = ( "div" | "rem" | "div.s" | "div.u" | "rem.s" | "rem.u"
            | "div.s.checked" | "div.u.checked" | "rem.s.checked" | "rem.u.checked" )
            operand "," operand [ unwind ] ;
shr       = ( "shr.s" | "shr.u" ) operand "," operand ;
cmp       = cmpmnem operand "," operand ;
cmpmnem   = "cmp." pred [ ".s" | ".u" ] ;                    (* one mnemonic token *)
pred      = "eq" | "ne" | "lt" | "le" | "gt" | "ge" ;
select    = "select" operand "," operand "," operand ;
cast      = castmnem operand ":" type ;
castmnem  = "cast." ( "sext" | "zext" | "trunc" | "sitofp" | "uitofp" | "fptosi"
          | "fptoui" | "fpext" | "fptrunc" | "bitcast" | "ptrtoint" | "inttoptr" ) ;  (* one token *)
load      = "load" operand ":" type [ "align" uint ] ;
store     = "store" operand "," operand [ "align" uint ] ;
alloca    = "alloca" decimal ;
call      = "call" cref args [ unwind ] [ "thunk" pref ] ;
icall     = "call.indirect" operand args ":" signature [ attrs ] [ unwind ] [ "thunk" pref ] ;
funcaddr  = "funcaddr" cref ;
addr      = "bind.addr" bref | "data.addr" dref | "ptr.const" ( pref | "null" ) ;
br        = "br" target ;
condbr    = "condbr" operand "," target "," target [ "prob" ( uint | fraction ) ] ;
target    = blk [ args ] ;
args      = "(" [ operand { "," operand } ] ")" ;
ret       = "ret" [ operand ] ;
assign    = "assign" vid "," operand ;
var       = "var" operand ;
unwind    = "unwind" ( "caller" | blk ) | "cleanup" "[" [ cpair { "," cpair } ] "]" ;
cpair     = "(" cref "," operand ")" ;
extop     = extname [ "flags" "(" uint ")" ] [ efield { "," efield } ] [ ":" type ] ;
efield    = operand | uint | "[" [ eword { "," eword } ] "]" ;
eword     = operand | uint | pref | type ;

operand   = vid | constant ;
constant  = literal ":" type [ "[" ( "audited" | "raw" ) "]" ] ;   (* one token *)
literal   = number | "true" | "false" | [ "-" ] "inf" | [ "-" ] "nan(" hex ")" | bytes ;
type      = "void" | "bool" | "i8" | "i16" | "i32" | "i64" | "f32" | "f64" | "ptr" | exttype ;
signature = "(" [ ptype { "," ptype } ] ")" "->" ptype ;
ptype     = type [ "zext" | "sext" ] ;
attrs     = "attrs" "(" attr { "," attr } ")" ;
attr      = "noArgRead" | "noArgWrite" | "noOtherRead" | "noOtherWrite"
          | "noUnwind" | "willReturn" | "declared" ;
```

How the constructs map to records:

| Text | Record |
|---|---|
| `nir 1.0`, `module "…"` | the format version; `ModuleRecord.nameId` (absent: anonymous) |
| `extension "n@v"` | an `ExtensionRecord`; checked against the registry (§9.4) |
| `import`, `pointer`, `binding`, `data`, `callee` | one record of that table each, index N = order of declaration |
| `pointer … local 0x… ` | mode 0; the module's `processToken` is the parsing process's token |
| `pointer … audited`/`raw` | provenance of a traced-address entry; required exactly for kind `address` |
| `location`, `region`, `debug`, `loc`, `region` suffix | the debug sections (§4.9) |
| `func`, `export`, `pressa`, `attrs` | `FunctionRecord` (`pressa`: level 0; otherwise 1) |
| `alloca S align A` | the function's next `AllocaRecord` |
| `attr "k" = "v"` | a `FunctionAttrRecord` |
| `^bN(…)`, `pad` | a `BlockRecord`, its `blockarg`s and the landing-pad flag |
| `[N]` | the explicit id of a void instruction (pre-SSA only) |
| constant suffix `[audited]`, `[raw]` | `ConstRecord.provenance` 1, 2 (none: `Literal`) |
| `zext`, `sext` | the extension kind of a type word |
| `unwind caller`, `unwind ^bN`, `cleanup […]`, none | `NO_CLEANUP`, a pad, a cleanup list, `NO_UNWIND` |
| `thunk #pN`, none | the call's thunk, `NONE` |
| `prob P`, none | `prob` (a `uint` as stored, or a `fraction` stored as `nearbyint(P × 4294967294.0)` in round-to-nearest-even), `0xFFFFFFFF` |
| `align N`, none | alignment code *k* with 2^(*k*−1) = N, 0 |

### 9.3 Printing

The printer's output is a fixed function of the module, so printing,
parsing and printing again gives identical text:

- **Order:** `nir 1.0`; `module` if named; `extension` lines for every
  extension the module uses, in first-use order (§7.1); then imports,
  pointers, bindings, data objects and callees, each table in index order;
  locations and regions; then the functions in `functions` order, each
  preceded by one empty line.
- **Layout:** one declaration, function header, `fitem`, block label or
  instruction per line; `fitem`s and instructions are indented by two
  spaces; block labels, `func` and `}` start at column 0. Tokens are
  separated by one space, a comma is followed by one space, nothing precedes
  a comma or `:` in a block argument, and there is a space on both sides of
  the `:` before a result type or signature. Lines end in LF without
  trailing spaces; the file ends with LF. No comments.
- **Functions:** `export` for exported functions, `pressa` for pre-SSA
  ones; `fitem`s in the order allocas (slot order), string attributes (key
  order), `debug`. Blocks in block-id order.
- **Ids:** at the SSA level, value-producing instructions print `%N =`,
  void instructions nothing. At the pre-SSA level every instruction prints
  its id: `%N =` or `[N]`, and each block lists its instructions in its
  op-id list order; block 0's header lists the parameters.
- **Optional parts** are printed exactly when they differ from the
  default: `attrs(…)` when any bit is set (names in bit order); `unwind`
  unless `NO_UNWIND`; `thunk` unless `NONE`; `prob` unless unknown, as the
  stored `u32` in decimal; `align N` unless *k* = 0; argument lists of
  `br`/`condbr` targets unless empty; `flags(F)` of an extension
  instruction unless 0; `zext`/`sext` unless kind 0.
- **Suffixes:** integer `div`/`rem` always print `.s` or `.u`, float ones
  neither; `.checked` exactly when bit 1 is set; `shr` always `.s` or `.u`;
  `cmp` prints `.s`/`.u` exactly for ordered integer predicates.
- **Constants:** integers as signed decimal (the two's complement value of
  the type's width); `bool` as `true`/`false`; finite floats as
  `std::to_chars(value)` (shortest round-trip form, plain overload) of the
  `float` or `double`; infinities as `inf`/`-inf`; NaNs as `nan(0x…)` or
  `-nan(0x…)` with the trailing significand field in lowercase hex without
  leading zeros; extension-type constants as `bytes`. The provenance
  suffix unless `Literal`.
- **Strings:** `"`, `\` and the bytes 0x00–0x1F and 0x7F are escaped
  (`\"`, `\\`, `\xHH` with lowercase hex); other bytes are written as they
  are. A function name is printed bare if it matches the bare pattern,
  otherwise as a string. Addresses, tokens and offsets in `pointer` lines
  are lowercase `0x` hex; other numbers are decimal.
- **Extension instructions** are printed generically from the operand
  shape (§5.11.1): `value` and `const` fields as operands, `imm` as decimal,
  a `list` as `[…]` with `pointer` words as `#pN` and `type` words as
  types; `unused` fields are omitted; `: T` exactly when the result is not
  `void`.

### 9.4 Parsing

- The version must be `1.x` with x at most the reader's minor version.
- **Declarations** of each table are numbered from 0 in order: the N-th
  `callee` must be `#cN`, and so on (`NIR-P003`). References may point
  forward to later declarations and functions.
- **Ids.** At the SSA level, blocks must appear as `^b0`, `^b1`, … and every
  `%N` must equal the position of its instruction, so void instructions are
  numbered implicitly (`NIR-P004`); `[N]` is accepted if it equals the
  position. At the pre-SSA level every instruction carries its id, and the
  ids of the function are exactly 0 … count − 1.
- **Result types** come from the operands where §5 defines them by the
  operands (arithmetic, bitwise, `var`), are fixed (`bool` for `and`, `or`,
  `not`, `cmp`; `ptr` for `alloca`, `funcaddr`, `bind.addr`, `data.addr`,
  `ptr.const`), come from the signature (calls), from b (`select`), from the
  block header (`blockarg`), or are written (`cast`, `load`, extension
  instructions).
- **Literals** are interpreted by their type: integers accept decimal or
  hex values in [−2^(n−1), 2^n − 1] and store the low n bits; floats accept
  integer, decimal or hex floating literals, rounded to nearest even, and
  `inf`, `nan(0x…)` (a NaN with that non-zero trailing significand);
  extension types accept `bytes` of exactly `TypeInfo.size`; `ptr` has no
  constants (`NIR-P006`).
- **Inline items** (constants, signatures, names) are interned in the
  order of their first occurrence in the text.
- **Extensions** are looked up by name in the process's registry
  (`NIR-P005`); an `extension` line fixes the version the text expects,
  which the registry must accept (§8.4). Without one, the registered
  version is used.
- After parsing, the module is verified (§6.4).

### 9.5 Round trip

- `print(parse(t)) = t` for every text `t` that the printer produced.
- `parse(print(m))` is structurally equal to `m` (§7) for an SSA-level
  module; for a canonical module, canonicalizing it reproduces `m`'s
  canonical image byte for byte.
- For a pre-SSA module, `parse(print(m))` has the same instructions, ids,
  blocks and block lists as `m`; table indices may differ.

## 10. Decisions taken for 1.0

These questions were open in the draft and are settled here:

| Question | 1.0 |
|---|---|
| Encoding of debug data | optional sections 15–18, 21, 22, 26, 27 with their own string table (§4.1, §4.9), feature bit `DEBUG_V1` |
| `unreachable` terminator | not in 1.0; tracing ends every block with a real terminator (§5.9) |
| Atomic operations | not in core in 1.0; atomics are intrinsics (§5.0). Core atomics are candidates for 1.1 if the intrinsics show a measured cost |
| Fast-math | not in 1.0; flag bits reserved (§5.1) |
| Pointer arithmetic | `add`/`sub` on `ptr` with an `i64` byte offset, as 0.3's `ADD` on pointers (§5.1) |
| Traced pointers | never constants; `pointers` entries of kind traced address with a provenance class, read by `ptr.const`; raw ones make the module process-local (§4.2, §4.7) |
| Destructors | cleanup lists of destructor callees at the pre-SSA level (§3.4), turned into shared landing pads that call the same callees (§6.3) |
| Copies and constants that are assigned later | the pre-SSA opcode `var` (§5.9) |
| Checked division | the checked bit with an unwind entry; failing checks call `nautilus_rt_throw_arith` (§5.10) |
| Narrow integers at native boundaries | `zext`/`sext` extension kinds in type words (§4.5.1) |
| Function metadata | plugin-owned string attributes, canonical (§4.5.2); parameter names and locations as debug data (§4.9) |
| Indirect calls | stated attributes and a capture thunk, like external calls (§5.7) |
| Extension payloads | declared operand shapes, interpreted by the core (§5.11.1) |
| 0.3's unused trace op `FREE` | dropped |
| Extension versioning | `version` plus `minCompatibleVersion` (§8.4) |
| Structural hash | XXH3-128 with seed 0 over the anonymous canonical image without header and debug sections, plus the code-affecting options (§7.2) |
| Text form | normative grammar, deterministic printer, parser (§9) |
| Diagnostics | stable `NIR-V`, `NIR-L`, `NIR-P` codes (§6.6) |
| Modules and imports | named modules with exports and imports; linking against the engine's module registry, by name and signature, acyclic; import slots updated on promotion (§4.4) |
| Call targets | four callee kinds: internal, external, imported, intrinsic (§4.6) |
| Memory effects | four independent guarantee bits (no reads/writes of argument memory and of other memory), zero = no guarantee, implication by bitwise subset (§4.6.2) |
| `funcaddr` | stable forwarding stubs, valid until the module is released (§5.7) |
| Global data | read-only data objects with `data.addr`, folded loads, no relocations, no mutable globals (§4.10) |
| Interning imports | never at import or link time; inlining is a separate optional pass that works across modules (§4.4.3); exported images always bundle their imports and are self-contained (§8.5) |

## 11. Conformance and change control

- **Becoming final.** This specification is a release candidate. It becomes
  final at the M1 exit, when the verifier, the serializer, the loader, the
  printer, the parser and the round-trip tests implement it without changes
  to the text. Changes found during M1 are made here first.
- **Conformance material,** maintained with the specification under
  `nautilus/test/data/nir-conformance/` and produced as part of M1.4 and
  M1.5 ([16](16-implementation-plan.md)):
  - a hex dump of a small canonical image with every byte explained (the
    second example of §9, canonicalized), with its structural hash;
  - a corpus of golden images (valid and invalid), each with the expected
    verifier or loader result and `NIR-` code;
  - a corpus of `.nir` texts that print back identically (§9.5);
  - a positive and a negative test per diagnostic code of §6.6 and per
    opcode of §5 ([12](12-validation.md)).
- **Change control.** After 1.0 is final, every change to this document
  states its version effect (§8.2), is recorded in a change log at the end
  of this document, and needs review by a maintainer of the format.

## Change log

| Version | Change |
|---|---|
| 1.0 RC | Initial specification, including read-only data objects (§4.10) |
| 1.0 RC | The level formerly called `Trace` is named `PreSSA` (value 0, unchanged) |
| 1.0 RC | The text form is parsed as well as printed (§9) |
| 1.0 RC | Review round 1: normative text grammar and printing rules (§9.1–§9.5); deterministic canonicalization and scan order (§7, §7.1); exact structural-hash input and derived keys (§7.2, §7.3); canonical image layout and loader rules (§8.1); diagnostic codes (§6.6) |
| 1.0 RC | Review round 1: `var` (§5.9); `ptr.const` and traced-address pointers, no `ptr` constants, `ModuleRecord.processToken` (§4.2, §4.4, §4.7, §5.8); destructor callees (§4.6); `call.indirect` attributes and thunk (§5.7); checked division throws through `nautilus_rt_throw_arith`, pre-SSA cleanup lists on integer `div`/`rem` (§5.10); extension kinds (§4.5.1); function string attributes, section 25 (§4.5.2); debug function records, sections 26–27 (§4.9); extension operand shapes (§5.11.1); landing-pad and unwind-dominance rules, reducible-loop layout (§6.3); `pure`/`readonly` imply `willReturn` (§4.6.2); cast table and alignment codes (§5.5, §5.6); registry frozen at the first engine construction (§6.5) |
