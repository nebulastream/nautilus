# Component: Serialization and Artifacts

Part of the [Nautilus 0.4 design](README.md) · Milestones M1 (NIR images),
M5 (artifacts, bindings, provenance) · Builds on #532, #527, #529

## 1. Purpose

Compiled modules can be saved, shipped and loaded. Loading either uses
native code directly, skipping tracing and code generation, or recompiles
from portable IR, skipping tracing. 0.4 unifies the two serialization lines
that exist as PRs:

| PR | What it does | Restrictions |
|---|---|---|
| **#532, IR images** | portable IR plus a manifest of signatures and options; recompiled on the receiving engine with any backend | no machine code shipped |
| **#527, native artifacts** | relocatable native objects from MLIR/LLVM with checked imports; loaded without tracing, IR, MLIR parsing or code generation; MLIR bytecode as a repair path | Linux x86-64 ELF |

## 2. Artifact layout

```
Artifact
├─ header: magic, format version, feature bits, CRC-32C, total size
├─ compatibility descriptor
│    core + provider build identities, code-affecting options;
│    per payload: LLVM version, target triple, data layout,
│    target CPU name and feature string (§2.2)
├─ NIR image                      (always present; portable recompilation path)
├─ runtime-binding schema         (§4; the image's `bindings` section, not a copy)
└─ native payloads (0..n)         one per (backend, target, CPU): object + import table
```

- **The NIR image is mandatory.** A receiver that cannot use a native
  payload recompiles from it without tracing. That happens with another
  target, another target CPU or feature set, another backend, a stale build
  identity, or a missing or corrupt payload. A payload whose descriptor
  does not match is never executed. This merges #527's bytecode-repair
  path and #532's cross-backend loading into one rule. NIR replaces MLIR bytecode as the portable payload;
  MLIR is regenerated from NIR.
- **Native payloads come from MLIR/LLVM in 0.4.** The `asmjit`, `tbc` and
  `cpp` backends embed process addresses in their output, so their
  artifacts use the NIR image. Recompiling from it skips tracing and costs
  at most the conversion-free part of a compile (legalization and
  codegen).
- **Native payloads keep #527's contract:**
  - exactly one relocatable object;
  - native imports identified by ELF build ID and load-bias-relative
    offset, resolved before JIT initialization;
  - exception-personality and unwind-resume bridges;
  - no second ORC implementation.

  0.4 extends the import contract to every address a payload needs
  (§2.1).
- **Native payloads are CPU-specific.** The MLIR backend today builds its
  target machine with `JITTargetMachineBuilder::detectHost()` and stamps
  `target-cpu` and `target-features` from the host on every function, so a
  payload built on an AVX-512 host faults on an AVX2 host. The target CPU
  name and the feature string passed to LLVM are therefore part of each
  payload's compatibility descriptor (§2.2), of the L1 identity and of the
  L2 key ([08 §2](08-caching.md)). The option `engine.mlir.targetCpu`
  (`host` by default, `generic`, or an LLVM CPU name) selects them; it is
  code-affecting (M1.4a). `host` gives the best code for in-process and
  same-machine caches; `generic` gives payloads that ship to any x86-64
  host. A mismatch recompiles from the NIR image.
- **The `inlining` plugin's LLVM hooks** rename external calls to process
  addresses (`callNameOverride` → `ptrToHex`) and splice registered bitcode
  into the module. A payload compiled with those hooks active is
  non-persistable unless every renamed symbol goes through the import table
  as a `Portable` pointer name (§2.1); the XXH3-128 hash of the spliced
  bitcode then joins the payload's descriptor and the L1/L2 keys
  ([08 §2](08-caching.md)). Otherwise the artifact carries only the NIR
  image, and the statistics record `artifact.ineligibleReason=inlining`.
- **The NIR image follows the canonical-section rules** of
  [02-nir.md §3.6](02-nir.md): deterministic bytes, per-section strides,
  feature bits, CRC-32C, framing via the total size.
- **`inspect(image)`** reads names, signatures, options and extensions
  from the descriptor and the image's tables without materializing
  anything (#532's `inspectModule`).
- **Loading checks** root signatures against the requested C++ signature:
  each parameter's and the result's width, and the extension kind
  (`none`/`zext`/`sext`) that the signature record carries per parameter
  and result ([13 §4.5](13-nir-spec.md)): requesting a `uint8_t` where
  the record says `sext` is rejected. NIR records no other
  signedness, so `int64_t` and `uint64_t` are not distinguished; #532
  checked full signedness on its typed manifest, which 0.4 drops.
- **Images are self-contained.** Saving a module bundles the transitive
  closure of its imports as internal functions
  ([13 §8.5](13-nir-spec.md)), so loading an image never needs another
  module or a link step. Native payloads are compiled from the bundled
  module.

API, carried over from #532 and #527:

```cpp
auto image  = nautilus::saveModule(compiled);            // NIR image + native payloads
auto info   = nautilus::inspectModule(image);
auto loaded = nautilus::loadModule(engine, image, bindings);
auto sum    = loaded.getFunction<int64_t(int64_t*, int64_t)>("sum");
```

`engine.retainIR` from #532 becomes unnecessary: a compiled module always
keeps its canonical NIR, which is small and flat.

### 2.1 Native payload linkage

The object of a native payload contains no absolute host address. Every
address it needs is an undefined symbol, resolved by the existing ORC/JITLink
instance at load from the image's tables and the load environment:

| Symbol | Refers to | Resolved from |
|---|---|---|
| `__nautilus_ptr_<i>` | `pointers` entry *i* ([13 §4.7](13-nir-spec.md)): callees, thunks, destructors, traced addresses | mode 1 by the `SymbolResolver`, mode 2 by build ID + offset (§3); mode 0 entries never appear in a payload |
| `__nautilus_bind_<i>` | `bindings` entry *i* | the binding environment of this load (§4) |
| `__nautilus_data_<i>` | data object *i* ([13 §4.10](13-nir-spec.md)) | the address where the engine placed it; the same for every tier of the module |
| `__nautilus_stub_<i>` | the `funcaddr` forwarding stub of callee *i* ([13 §5.7](13-nir-spec.md)) | the stub the engine creates for the loaded module |
| `nautilus_rt_*` | runtime helpers: the capture-thunk entry, `nautilus_rt_throw_arith` for checked division ([06 §4](06-semantics.md)), the personality and unwind-resume bridges | by name from `libnautilus`; each name is listed in the payload's import table |

- **The 0.4 MLIR lowering emits these references as external symbols** for
  modules that may be persisted. 0.3's lowering embeds callee addresses as
  immediates; that form is kept only for in-process compiles whose result is
  never stored (ProcessLocal, L0).
- **Code model:** position-independent code (`llvm::Reloc::PIC_`) with the
  small code model; external symbols are reached through the GOT and PLT
  stubs that JITLink creates, so a payload loads anywhere in the address
  space.
- **Accepted relocations** (ELF x86-64): `R_X86_64_PC32`,
  `R_X86_64_PLT32`, `R_X86_64_GOTPCREL`, `R_X86_64_GOTPCRELX`,
  `R_X86_64_REX_GOTPCRELX`, and `R_X86_64_64` in non-executable sections.
  The loader checks every relocation before linking. Any other type, or an
  undefined symbol that is not in the import table, rejects the payload,
  and the module is recompiled from the NIR image.
- **The import table** lists every undefined symbol with its class
  (pointer, binding, data, stub, runtime helper, native import) and index,
  so the loader resolves them without parsing symbol names and rejects
  payloads that reference an entry the image does not have.

### 2.2 Container encoding

The artifact container reuses the image framing of
[13 §8.1](13-nir-spec.md): the same 48-byte header layout and the same
24-byte section table, with its own magic and section kinds.

```c
struct ArtifactHeader {       // 48 bytes; same layout as ImageHeader (13 §8.1)
	char     magic[4];        // "NART"
	uint16_t major, minor;    // artifact format version; 0.4.0 writes 1.0
	uint32_t compatFeatures;  // bits a reader may ignore
	uint32_t incompatFeatures;// bits a reader must understand
	uint64_t totalSize;       // bytes, including the header
	uint32_t sectionCount;
	uint32_t crc32c;          // CRC-32C of bytes [48, totalSize)
	uint8_t  reserved[16];    // 0
};
```

| Kind | Section | Record |
|---|---|---|
| 1 | `descriptor` | `DescriptorRecord`, 16 B, sorted by key, each key at most once |
| 2 | `strings` | `{u32 offset, u32 length}` into `stringBytes`, 8 B |
| 3 | `stringBytes` | UTF-8 or binary bytes |
| 4 | `image` | the NIR image, verbatim, with its own header and checksum (13 §8) |
| 5 | `payloads` | `PayloadRecord`, 56 B |
| 6 | `payloadImports` | `PayloadImport`, 16 B |
| 7 | `payloadBytes` | the payload objects, each 16-byte aligned |

```c
struct DescriptorRecord {     // 16 bytes
	uint16_t key;             // see below
	uint16_t kind;            // 0 u64 in value; 1 string: value = index in strings; 2 bytes: likewise
	uint32_t reserved;        // 0
	uint64_t value;
};

struct PayloadRecord {        // 56 bytes; strings are indices in strings
	uint32_t backend;         // "mlir"
	uint32_t tier;            // 0 or 1; informational
	uint32_t llvmVersion;     // "22.1.8"
	uint32_t triple;          // "x86_64-unknown-linux-gnu"
	uint32_t dataLayout;      // as LLVM prints it
	uint32_t cpu;             // target CPU name passed to LLVM, or "generic"
	uint32_t features;        // complete target feature string passed to LLVM ("+avx2,+fma,-avx512f,...")
	uint32_t inlinedBitcode;  // bytes: XXH3-128 of the spliced bitcode (§2), or 0xFFFFFFFF
	uint32_t firstImport, importCount; // range in payloadImports
	uint64_t offset, size;    // the object in payloadBytes
};

struct PayloadImport {        // 16 bytes
	uint32_t symbol;          // string: the undefined symbol (§2.1)
	uint8_t  cls;             // 0 pointer, 1 binding, 2 data, 3 stub, 4 runtime helper, 5 native import
	uint8_t  pad[3];          // 0
	uint32_t index;           // entry in the image's table for classes 0-3; else 0
	uint32_t reserved;        // 0
};
```

Descriptor keys (1.0): 1 core version string, 2 core build identity (ELF
build ID of `libnautilus`, bytes), 3 cache-provider build identity, 4 NIR
format version of the embedded image (`major << 16 | minor`), 5 the
code-affecting options block, encoded exactly as item 2 of the structural
hash ([13 §7](13-nir-spec.md)), 6 the structural hash (16 bytes, the L2
key), 7 the caller's semantic key (L1 entries only). Keys 1, 2, 4, 5 and 6
are required. Unknown keys are ignored; a later key that readers must not
ignore comes with an `incompatFeatures` bit.

- **Nothing is duplicated.** Exports, signatures, extensions and the
  binding schema are read from the embedded image; `inspect` reads them
  without materializing the module.
- **Using a payload** requires, in this order: a valid header and checksum,
  the required descriptor keys, an equal core build identity, an image that
  loads ([13 §8.1](13-nir-spec.md)), and a payload record whose backend,
  LLVM version, triple and data layout are equal to the receiver's, whose
  `cpu` is `generic` or equal to the host CPU name, and every feature
  enabled in `features` is present on the host. Any failure falls back to
  the image, and the statistics record which check failed.
- **Versioning** follows [13 §8.2](13-nir-spec.md): within 0.4.x the
  container gains only sections, keys and appended fields (D12); an older
  build's payload is stale but its image stays loadable.

## 3. Pointers

Native addresses never appear in the instruction stream. They live in the
`pointers` section, referenced by index:

- **Kinds:** callees, exception-capture thunks, destructors, traced
  addresses, and addresses inside extension payloads where an extension
  declares them ([04](04-extensions.md)).
- **One encoding for traced pointers.** A `val<T*>` built from a host
  address is never a `ConstPool` entry: it is a `pointers` entry with a
  provenance class (§5). An entry that no `SymbolNamer` names and that is
  not a runtime binding stays `RawProcessValue` in mode `ProcessLocal`: the
  writer sets the image's `PROCESS_LOCAL` bit and the process token
  ([13 §8.3](13-nir-spec.md)), and the image never loads in another process
  (fail closed).
- **Two modes (#532):**
  - `ProcessLocal`: raw addresses guarded by a per-process token, valid
    only for in-process reuse (the L0 cache, [08](08-caching.md));
  - `Portable`: symbol names mapped by a client `SymbolNamer`/
    `SymbolResolver` (with `SymbolTable` and `dladdr`/`dlsym`
    implementations), or ELF build ID + offset for native imports (#527).
- **Runtime state is never a pointer entry.** It is a runtime binding (§4).

## 4. Runtime bindings (#529)

Module-local process state (hash tables, buffers, operator state) is
referenced through typed `RuntimeBinding<T>` handles:

- an immutable, module-owned registration snapshot;
- an address-free schema: stable symbols, and compiler-derived type, size
  and alignment;
- fresh resolution on every load;
- the application keeps owning the storage and its synchronization.

In NIR a binding is a core op, `bind.addr id`. Its identity is stable and
its address is supplied at load. Every backend implements it, as #529 does
for interpreted, C++, `bc`, `tbc`, AsmJit and MLIR (the port drops `bc`, removed in 0.4). The schema is part of
the artifact and of the compatibility check. Address aliasing never merges
identities, so equal, distinct and overlapping bindings keep their
relationships across modules and loads.

## 5. Provenance and safety

#527 and #528 run mandatory checks before optimization:
- scalar and typed-allocation evidence;
- rejection of encoded addresses;
- guards against laundering through memory and callbacks;
- destructor-only operand trees.

On NIR these become **scans of the side tables plus a short dataflow
pass**:

- **Data objects** ([13 §4.10](13-nir-spec.md)) are `Literal` by
  construction: their bytes are part of the image. Constant tables created
  with `constData` therefore never block persistence, unlike traced
  pointers to host arrays.
- **Constants:** each `ConstPool` entry and each `pointers` entry carries
  a provenance class: `Literal`, `AuditedInvariant` (from
  `cacheLiteral`/`cacheInvariant`) or `RawProcessValue`.
  - The tracer gives integer, float and bool constants `Literal` unless the
    user marks them. This is what keeps ordinary traced modules eligible
    (#529's 1500/1500 native hits).
  - Traced pointers are `pointers` entries (§3), `RawProcessValue` unless
    they resolve portably or are audited.
  - An integer derived from a traced pointer inside the traced code
    (pointer-to-integer cast, arithmetic on the address) inherits the
    pointer's class through the dataflow pass. An address the host casts to
    an integer before tracing is caught only by #527's encoded-address
    rejection, which keeps running on pointer-width `Literal` constants.
- **Allocations:** `nautilus_alloca<T>()` records typed size and alignment
  evidence. Raw allocation tables stay uncertified.
- **Pointers:** each `pointers` entry carries its kind and whether it can be
  resolved portably. Extensions declare whether their payloads are portable
  ([04](04-extensions.md)).
- **Eligibility:** a native payload or a `Portable` image is produced only
  if no reachable instruction depends on a `RawProcessValue` constant or
  pointer, an unresolvable pointer, a non-portable extension payload, or
  code spliced by the `inlining` plugin's hooks without portable names
  (§2). `ProcessLocal`
  images (the L0 cache) need no eligibility check: they are only valid in
  the process that produced them.
  The check runs **on the pre-SSA level, before the conversion folds
  anything** ([03](03-conversion-and-passes.md) folds while it emits), and
  folding gives every folded constant the **least trusted class among its
  inputs** (`RawProcessValue` > `AuditedInvariant` > `Literal`), so folding
  cannot hide a raw value. Modules built through the C
  API have no pre-SSA level and are checked at the SSA level. Replay
  disagreement never upgrades evidence (#527).
- **Failure:** fail closed. The module still compiles normally; the
  statistics record why it was not persistable.
- **Trust:** artifacts and cache directories are trusted executable input.
  Checksums detect accidental corruption, not malicious producers.

## 6. Exit criteria (M5)

- Round trip on every backend:
  - save on one engine, load on every backend and the default compiler;
  - identical results;
  - re-saving gives identical bytes.
- Native payload hits in a fresh process with ASLR: no tracing, no
  conversion, no code generation.
- Every negative test of #527, #528 and #532 ported and failing closed:
  - truncation, bit flips, malformed descriptors;
  - incompatible identities or ABI;
  - encoded addresses, laundering, callback-only and destructor-only
    values.
- Runtime bindings resolve per load across concurrent modules.
- A payload built with `engine.mlir.targetCpu` set to a CPU whose features
  the host lacks is rejected and the module recompiled from the image; a
  `generic` payload loads on every x86-64 runner.
- A payload with an unaccepted relocation type, or an undefined symbol
  missing from its import table, is rejected before linking (§2.1).
- A module compiled with `NAUTILUS_INLINE` functions is either reported
  ineligible or reloads correctly in a fresh process.
- The D12 fixtures (M5.9) include artifacts with a payload: after a build
  change the payload is rejected and the image is used.

## 7. Risks

- **A raw-address path the provenance classes miss.** Fail closed by
  default; keep #527's negative corpus; add fuzzing of
  traced-native-value patterns.
- **Native payloads are platform-specific** (Linux x86-64 ELF first).
  Other targets use the NIR image path until their payloads are supported.
- **Schema equality is not program equivalence.** The caller's semantic
  key ([08](08-caching.md)) stays responsible for that.
