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
│    core + provider build identities, LLVM / target / data layout,
│    extensions used (name@version), effective typed options,
│    exports + signatures
├─ NIR image                      (always present; portable recompilation path)
├─ runtime-binding schema         (§4)
└─ native payloads (0..n)         one per (backend, target): object + import table
```

- **The NIR image is mandatory.** A receiver that cannot use a native
  payload recompiles from it without tracing. That happens with another
  target, another backend, a stale build identity, or a missing or corrupt
  payload. This merges #527's bytecode-repair path and #532's cross-backend
  loading into one rule. NIR replaces MLIR bytecode as the portable payload;
  MLIR is regenerated from NIR.
- **Native payloads come from MLIR/LLVM in 0.4.** The single-pass and
  bytecode backends embed process addresses in their output, so their
  artifacts use the NIR image. Recompiling from it skips tracing and costs
  at most the conversion-free part of a compile (legalization and
  codegen).
- **Native payloads keep #527's contract:**
  - exactly one relocatable object;
  - imports identified by ELF build ID and load-bias-relative offset,
    resolved before JIT initialization;
  - exception-personality and unwind-resume bridges;
  - no second ORC implementation.
- **The NIR image follows the canonical-section rules** of
  [02-nir.md §3.6](02-nir.md): deterministic bytes, per-section strides,
  feature bits, CRC-32C, framing via the total size.
- **`inspect(image)`** reads names, signatures, options and extensions
  without materializing anything (#532's `inspectModule`).
- **Loading checks** root signatures against the requested C++ signature,
  including width and signedness (#532).

API, carried over from #532 and #527:

```cpp
auto image  = nautilus::saveModule(compiled);            // NIR image + native payloads
auto info   = nautilus::inspectModule(image);
auto loaded = nautilus::loadModule(engine, image, bindings);
auto sum    = loaded.getFunction<int64_t(int64_t*, int64_t)>("sum");
```

`engine.retainIR` from #532 becomes unnecessary: a compiled module always
keeps its canonical NIR, which is small and flat.

## 3. Pointers

Native addresses never appear in the instruction stream. They live in the
`pointers` section, referenced by index:

- **Kinds:** callees, exception-capture thunks, destructors, `ConstPtr`
  values, and addresses inside extension payloads.
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
for interpreted, C++, `bc`, `tbc`, AsmJit and MLIR. The schema is part of
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

- **Constants:** each `ConstPool` entry carries a provenance class:
  `Literal`, `AuditedInvariant` (from `cacheLiteral`/`cacheInvariant`) or
  `RawProcessValue` (a native scalar traced without an audit).
- **Allocations:** `nautilus_alloca<T>()` records typed size and alignment
  evidence. Raw allocation tables stay uncertified.
- **Pointers:** each `pointers` entry carries its kind and whether it can be
  resolved portably. Extensions declare whether their payloads are portable
  ([04](04-extensions.md)).
- **Eligibility:** a native payload or a `Portable` image is produced only
  if no reachable instruction depends on a `RawProcessValue` constant, an
  unresolvable pointer or a non-portable extension payload. `ProcessLocal`
  images (the L0 cache) need no eligibility check: they are only valid in
  the process that produced them.
  The check runs **before optimization**, so folding cannot hide a raw
  value, and replay disagreement never upgrades evidence (#527).
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

## 7. Risks

- **A raw-address path the provenance classes miss.** Fail closed by
  default; keep #527's negative corpus; add fuzzing of
  traced-native-value patterns.
- **Native payloads are platform-specific** (Linux x86-64 ELF first).
  Other targets use the NIR image path until their payloads are supported.
- **Schema equality is not program equivalence.** The caller's semantic
  key ([08](08-caching.md)) stays responsible for that.
