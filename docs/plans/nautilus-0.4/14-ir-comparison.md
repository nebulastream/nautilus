# NIR Compared with Other IRs

Part of the [Nautilus 0.4 design](README.md) · Compares the NIR
specification ([13](13-nir-spec.md), 1.0 RC) with eight IRs that solve
related problems

## 1. Purpose and scope

This report compares NIR with the IRs that its design borrows from or
competes with, to check three things: that NIR's choices are deliberate, that
it is not missing something the others found necessary, and where it does
something they do not.

| IR | Role | Why it is in this comparison |
|---|---|---|
| **LLVM IR** | optimizing compiler IR | the MLIR backend's target; the reference for exceptions and attributes |
| **MLIR** (`func`, `cf`, `arith`, `llvm`, `scf`) | extensible multi-level IR | NIR's extension model ("dialects lite") is modeled on it |
| **WebAssembly** | portable binary format, VM input | closest in serialization, module linking and defined semantics |
| **Cranelift IR (CLIF)** | fast JIT and baseline compiler IR | closest in purpose: low compile latency, SSA with block parameters |
| **QBE IL** | small optimizing backend IR | a minimal design point |
| **SPIR-V** | portable GPU binary format | structured control flow, capabilities, word encoding |
| **LuaJIT IR** | tracing JIT IR | the other tracing JIT; inspiration for constant references |
| **Zig AIR** | compiler middle-end IR | inspiration for the flat instruction array and `extra` |

The descriptions of the other IRs are from their public documentation and
source as of 2026; details of fast-moving projects (MLIR, Cranelift,
WebAssembly proposals) may change.

## 2. Summary

**NIR is a Cranelift-style SSA IR** (block parameters, signless integers, a
CFG) **stored like LuaJIT and Zig IR** (flat 16-byte records, dense `u32`
references, a side `extra` array), **serialized and linked like WebAssembly**
(a sectioned binary with checked two-level imports), **with LLVM's exception
and memory-effect model**, and **MLIR-style extensibility** limited to what a
latency budget allows.

What only NIR combines:
1. **One format for the trace and the SSA level,** so tracing and compilation
   share records, the verifier, the printer and the serializer.
2. **An in-memory layout that is the wire layout** for canonical sections:
   loading is validation, and the cache key is a hash over bytes.
3. **A canonical form with a structural hash** defined in the specification,
   so equal modules produce equal bytes across processes.
4. **Provenance in the IR:** every constant carries where its value came
   from, so the IR knows which modules are safe to persist.
5. **Self-contained images by construction** while linking by reference in
   memory.

What NIR deliberately does not have, compared with the others: atomics,
aggregate and vector types in core, structured control flow, `poison`/`undef`,
and mutable globals. §5 lists which of these are worth revisiting.

## 3. Comparison by topic

### 3.1 In-memory representation

| IR | Representation | Size of a binary integer add (approx.) |
|---|---|---|
| **NIR** | flat `Inst` array per module; value id = index; operands are `u32` refs; constants tagged in the reference | **16 B** |
| LLVM IR | heap objects (`Instruction` subclasses) with use lists, in intrusive lists, uniqued constants in an `LLVMContext` | ~ 70–100 B plus `Use` records |
| MLIR | heap `Operation`s with operand, result, attribute and region storage; uniqued types and attributes in an `MLIRContext` | ~ 100 B and more |
| Cranelift | entity arrays in a `DataFlowGraph`: instructions, values, blocks referenced by `u32` entity indices; variable-length lists in a list pool | ~ 16 B instruction data plus value entries |
| QBE | per-block arrays of fixed-size `Ins` structs (opcode and class packed in one word, two 4-byte refs, a result ref) | ~ 12–16 B |
| SPIR-V | a stream of 32-bit words; each instruction is word count + opcode plus operand ids | 20 B (5 words) |
| LuaJIT IR | one 64-bit `IRIns` per instruction: two 16-bit operand refs, opcode, type, a hash chain link; constants grow downward below `REF_BIAS` | **8 B** |
| Zig AIR | `MultiArrayList` of a 1-byte tag and 8 bytes of data per instruction, plus an `extra: []u32` array | **9 B** (tag + data, stored column-wise) |

**Reading:** NIR sits with Cranelift, LuaJIT and Zig in the flat, index-based
family that allocates nothing per instruction. It is twice LuaJIT's size
because NIR needs 32-bit references (functions larger than 64 k values) and a
third operand, and because the same record must also be the wire format.
LLVM and MLIR pay for generality: rich per-object metadata and use lists,
which NIR replaces with derived arrays built on demand
([02 §4](02-nir.md)).

### 3.2 SSA and control flow

| IR | SSA form | Merges | Control flow |
|---|---|---|---|
| **NIR** | SSA level; trace level with variables (`assign`) | block arguments | CFG with `br`, `condbr`, `ret`, `resume`; layout invariant (RPO, loops contiguous) |
| LLVM IR | SSA | `phi` | CFG; `switch`, `indirectbr`, `invoke`, `callbr` |
| MLIR | SSA | block arguments | regions: unstructured (`cf`) or structured (`scf.if`, `scf.for`, `scf.while`) |
| WebAssembly | not SSA: a typed stack machine with locals | locals | **structured only:** `block`, `loop`, `if`, branches to enclosing labels, `br_table` |
| Cranelift | SSA | block parameters | CFG; `brif`, `jump`, `br_table` |
| QBE | SSA (built from non-SSA input by QBE) | `phi` | CFG; `jmp`, `jnz` |
| SPIR-V | SSA | `OpPhi` | CFG, but **structured** for shaders: each selection and loop declares its merge and continue blocks |
| LuaJIT IR | SSA | `PHI` at the loop end only | **no CFG:** a linear trace with guards that exit to snapshots; one loop |
| Zig AIR | SSA | block results | **structured:** `block`, `loop`, `cond_br` with nested bodies |

**Reading:**
- **Block arguments,** as in MLIR and Cranelift, are the modern choice: no
  `phi` placement rules, and a branch carries its own operands.
- **A CFG** is the right default for a tracer, whose merges come from
  snapshot matches rather than from source nesting. WebAssembly, SPIR-V and
  Zig AIR show that structure pays for consumers that need it (GPU, Wasm, a
  readable C++ backend); NIR leaves that to a restructuring pass on reducible
  graphs (the open structured-control-flow question in the plan).
- **A two-level IR** (trace with variables, then SSA) has no direct
  counterpart. LuaJIT records SSA directly, because its traces are linear;
  Nautilus traces branching code, so it needs variables until the CFG is
  complete.

### 3.3 Types

| IR | Integers | Floats | Pointers | Vectors, aggregates |
|---|---|---|---|---|
| **NIR** | `i8`–`i64`, signless; `bool` separate | `f32`, `f64` | one opaque `ptr`, 64-bit | extension types only (`simd.vec`, `builtin.i128`) |
| LLVM IR | `iN` for any N, signless; `i1` as bool | half to `fp128`, bfloat | opaque `ptr` with address spaces | vectors (fixed and scalable), structs, arrays |
| MLIR | `iN` signless, signed and unsigned variants | many | `memref` with layouts; `!llvm.ptr` | `vector`, `tensor`, tuples; any dialect type |
| WebAssembly | `i32`, `i64`; signedness on operations | `f32`, `f64` | none: addresses are `i32`/`i64` offsets into linear memory | `v128`; reference types |
| Cranelift | `i8`–`i128`, signless | `f16`–`f128` | none: pointers are the target's integer type | SIMD vectors |
| QBE | `w` (32), `l` (64); `b`, `h` for memory | `s`, `d` | none: `l` | aggregates as named memory layouts |
| SPIR-V | `OpTypeInt` with width and signedness | widths 16–64 | typed pointers with storage classes | vectors, matrices, arrays, structs |
| LuaJIT IR | 8–64-bit, signed and unsigned | `num` | typed (`p32`/`p64`, `tab`, `str`, …) | none |
| Zig AIR | any `uN`/`iN` | many | typed, with alignment and address space | arrays, structs, vectors, optionals, error unions |

**Reading:** NIR's core type set is the smallest besides QBE's, by design:
everything else is an extension type with a mandatory fallback. Keeping a
real `ptr` type, unlike WebAssembly, Cranelift and QBE, is what lets NIR carry
provenance and distinguish pointer conversions (`ptrtoint`/`inttoptr`).
`i128` is the one type the low-latency peers (Cranelift) have in core and
NIR has only as an extension.

### 3.4 Semantics of arithmetic

| IR | Integer overflow | Division by zero | Oversized shift | float → int out of range | Undefined values |
|---|---|---|---|---|---|
| **NIR** | wraps | **throws** (`checked`) | amount modulo width | **saturates** | none: no `poison` or `undef` |
| LLVM IR | wraps; `nsw`/`nuw` make it `poison` | undefined behavior | `poison` | `poison` (`fptosi.sat` saturates) | `poison`, `undef` |
| WebAssembly | wraps | **traps** | amount modulo width | traps; `trunc_sat` saturates | none; NaN bits nondeterministic |
| Cranelift | wraps | **traps** (`udiv`, `sdiv`) | amount modulo width | traps; `fcvt_to_*_sat` saturates | none |
| QBE | wraps | target-dependent | target-dependent | target-dependent | — |
| SPIR-V | wraps | undefined | undefined | undefined | `OpUndef` |
| LuaJIT IR | wraps (checked variants for guards) | guarded by the recorder | masked | — | — |

**Reading:** NIR follows WebAssembly and Cranelift: every arithmetic
operation is fully defined, which is what a differential fuzzer and formal
verification need. The one difference is that NIR's integer division
**throws a C++ exception** instead of trapping, because Nautilus code runs
inside a host process that must survive the error. LLVM's `poison` model
gives more optimization freedom but is the hardest part of LLVM to get right
(the Alive2 work); NIR avoids it.

### 3.5 Memory and effects

| IR | Memory model | Atomics | Call effects |
|---|---|---|---|
| **NIR** | plain loads and stores; data races undefined | **none in 1.0** | four guarantee bits (argument and other memory × read and write) plus `noUnwind`, `willReturn` |
| LLVM IR | C++11 model | full (`atomicrmw`, `cmpxchg`, fences, orderings) | `memory(argmem: …, inaccessiblemem: …, other: …)`, `nounwind`, `willreturn`, many more |
| MLIR | dialect-defined | `memref.atomic_rmw`, `llvm` atomics | side-effect interfaces per operation |
| WebAssembly | linear memory; threads proposal adds shared memory | yes (threads) | none: every call may do anything |
| Cranelift | flags per access (`notrap`, `aligned`, `readonly`) | yes | none in IR |
| SPIR-V | Vulkan memory model, scopes | yes | function control (`Pure`, `Const`) |
| LuaJIT IR | alias analysis on typed references | none | per-opcode |
| Zig AIR | typed pointers | yes | — |

**Reading:** NIR's guarantee bits are essentially LLVM's `MemoryEffects`
(LLVM 16 on) with two locations instead of three, encoded so that zero is
always safe and implication is a subset test. The visible gap is atomics:
every peer except LuaJIT and QBE has them. NebulaStream calls atomics through
`invoke` today; the spec defers them to 1.1 pending a measured cost.
Cranelift's per-load `readonly` flag is worth noting: NIR gets the same
effect for data objects, whose loads fold or hoist.

### 3.6 Exceptions

| IR | Model |
|---|---|
| **NIR** | unwind entries on calls and checked division: `NO_UNWIND`, `NO_CLEANUP` or a landing-pad block ending in `resume`; trace-level cleanup lists; capture thunks for backends without unwind tables |
| LLVM IR | `invoke` with `landingpad`/`resume` (Itanium), funclets (`catchswitch`, `cleanuppad`) for Windows |
| MLIR | none in core; the `llvm` dialect mirrors LLVM |
| WebAssembly | exception handling (Wasm 3.0): `throw`, `try_table` with catch clauses, `exnref` |
| Cranelift | recent versions add `try_call` with exception tables, for Wasm exception handling |
| QBE, SPIR-V, LuaJIT IR | none |
| Zig AIR | none (errors are values) |

**Reading:** NIR's model is LLVM's Itanium model reduced to cleanup only:
Nautilus code never catches, it only runs destructors and continues
unwinding to the host. Two things have no counterpart elsewhere: cleanup
lists at the trace level, turned into shared landing pads by the conversion,
and the defined bridge to backends without unwind tables.

### 3.7 Modules, linking and call targets

| IR | Unit | Imports | Call targets |
|---|---|---|---|
| **NIR** | named module with exported and internal functions | **two-level** `(module, function)` with exact signature and implied guarantees; acyclic; linked by reference through import slots | internal, external (native), imported, intrinsic |
| LLVM IR | module with linkage types (`internal`, `external`, `weak`, …) | declarations resolved by symbol name (`llvm-link`, ORC) | functions, declarations, intrinsics (`llvm.*`) |
| MLIR | `builtin.module` with symbol tables | symbol references across nested modules | `func.call`, `func.call_indirect`, dialect ops |
| WebAssembly | module | **two-level** `(module, name)` with types checked at instantiation | defined, imported, `call_indirect` through tables |
| Cranelift | function at a time; `cranelift-module` adds linkage | `Linkage::Import`/`Export` | direct, indirect, libcalls |
| QBE | file | symbols, `export` | direct, indirect |
| SPIR-V | module | `Import`/`Export` linkage decorations; extended instruction sets | functions, `OpExtInst` |
| LuaJIT IR | trace | — | `CALLN`, `CALLL`, `CALLS` to C functions |
| Zig AIR | per function, within a compilation unit | — | direct, indirect |

**Reading:** NIR's imports are WebAssembly's two-level namespace with two
additions: the importer states which guarantees it relies on, and promotion
replaces the exporter's code under running importers. Intrinsics match
LLVM's `llvm.*` intrinsics and SPIR-V's extended instruction sets, with one
stronger rule: every NIR intrinsic has a native fallback that defines it, so
every backend supports every intrinsic.

### 3.8 Global data

| IR | Model |
|---|---|
| **NIR** | read-only data objects (`data.addr`), aligned bytes, no relocations; mutable state through runtime bindings |
| LLVM IR | `global` variables, constant or mutable, with linkage, alignment, initializers and relocations |
| MLIR | `memref.global`, `llvm.mlir.global` |
| WebAssembly | data segments (active and passive) and typed `global`s |
| Cranelift | data objects in `cranelift-module`, with relocations |
| QBE | `data` definitions with bytes, strings and symbol references |
| SPIR-V | module-scope `OpVariable`s with storage classes |
| LuaJIT IR | constants only (`KGC`, `KPTR`) |
| Zig AIR | through the compilation unit's declarations |

**Reading:** NIR is the most restrictive here, on purpose: no mutable
globals (state belongs to the application, through bindings) and no
relocations (data stays position-independent and the format simple). LLVM,
Cranelift and QBE allow relocations in data because they produce object
files; NIR can add a relocation table in a minor version if a use case
appears.

### 3.9 Extensibility

| IR | Mechanism |
|---|---|
| **NIR** | registered extension operations, types, intrinsics and passes, with dense ids, traits, mandatory fallbacks and `name@version` in images |
| LLVM IR | fixed instruction set; extension through intrinsics and metadata |
| MLIR | open: dialects with operations, types, attributes, interfaces and rewrite patterns |
| WebAssembly | standardized proposals, enabled per feature |
| Cranelift | fixed instruction set, defined in its meta crate |
| QBE | none |
| SPIR-V | capabilities, extensions, extended instruction sets |
| LuaJIT IR | none |
| Zig AIR | none (compiler-internal) |

**Reading:** NIR takes MLIR's idea of plugin-defined operations and types, but
removes what costs latency: no uniqued attributes, no interface dispatch per
operation, no nested regions. Its mandatory fallback is stricter than
MLIR's: an MLIR operation without a lowering is a compile error; a NIR
extension without a handler always runs.

### 3.10 Serialization and versioning

| IR | Format | Versioning |
|---|---|---|
| **NIR** | sectioned binary; canonical sections are the in-memory layout (zero-copy load); CRC-32C; canonical form with a structural hash | major.minor, per-section strides, compat and incompat feature bits; images readable across 0.4.x |
| LLVM IR | bitcode (bitstream, abbreviations) and text `.ll` | newer LLVM reads older bitcode; no forward compatibility |
| MLIR | bytecode (with lazy loading and dialect versioning) and text | per dialect version |
| WebAssembly | binary with typed sections and LEB128 integers; text `.wat` | version 1 forever; features added by proposals |
| Cranelift | text `.clif` for tests; no stable binary | none: internal to the compiler |
| QBE | text only | none |
| SPIR-V | 32-bit word stream | version number plus capabilities and extensions |
| LuaJIT IR | none (in memory only) | — |
| Zig AIR | none stable (cached for incremental compilation) | — |

**Reading:** NIR's format is closest to WebAssembly's (sections, validation
before use) and SPIR-V's (fixed-width words), but is the only one designed so
that the loaded bytes *are* the IR. No other format here defines a canonical
form and a content hash as part of the specification; build systems and
caches around them hash whatever bytes they get.

### 3.11 Verification and formal semantics

| IR | Status |
|---|---|
| **NIR** | verifier rules per level (§6), one positive and one negative test per rule; no formal semantics yet |
| LLVM IR | verifier; informal semantics; Alive2 checks optimizations against an SMT model; Vellvm formalizes a subset in Coq |
| MLIR | per-operation verifiers; no global semantics |
| WebAssembly | **mechanized formal semantics** (Isabelle, Coq); validation is a type system |
| Cranelift | verifier; lowering rules in ISLE, with SMT-based verification of the rules (Crocus) |
| QBE | none |
| SPIR-V | validator (`spirv-val`) |
| LuaJIT IR | none |
| Zig AIR | none |

**Reading:** NIR's defined arithmetic, small opcode set and flat encoding put
it in the same position as WebAssembly: a formal semantics is feasible. The
two practical steps from the plan's discussion, SMT verification of folding
rules (as Alive2 and Crocus do) and symbolic equivalence in the fuzzer, are
the cheapest way to close the gap.

### 3.12 Latency orientation

| IR | Designed for compile latency? |
|---|---|
| **NIR** | **yes:** budget of ~0.25 µs per op each for tracing, conversion and backend |
| LLVM IR | no: optimizing pipeline; milliseconds for small functions |
| MLIR | no: context and pass setup in milliseconds |
| WebAssembly | yes, as an input: baseline compilers translate in one pass |
| Cranelift | **yes:** the JIT of Wasmtime; its authors report compile times several times to an order of magnitude below LLVM's, at somewhat lower code quality |
| QBE | moderately: small and simple |
| SPIR-V | as an input: drivers compile it |
| LuaJIT IR | **yes:** microseconds per trace |
| Zig AIR | yes, for debug builds |

## 4. Where NIR stands out

1. **Two levels in one format.** No other IR here serves as both the trace
   recording and the compiler IR. This removes a whole representation and its
   conversion layers.
2. **Wire layout = memory layout, with a canonical form and hash.** Loading
   is validation, caching is hashing, and the definition of "the same module"
   is in the specification.
3. **Provenance on constants and data.** The IR itself knows which modules can
   be persisted and which depend on the current process.
4. **Self-contained images and linking by reference** at the same time,
   with promotion that replaces an exporter under running importers.
5. **Exceptions without catching:** a cleanup-only model that bridges to
   backends without unwind tables.
6. **Extensions with mandatory fallbacks:** every backend runs every
   extension.

## 5. Gaps and lessons for NIR

| Gap | Peers that have it | Recommendation |
|---|---|---|
| Atomic operations | LLVM, MLIR, Wasm, Cranelift, SPIR-V, Zig | Measure atomics through `invoke` on NebulaStream in M1; add `atomic.load`, `atomic.store`, `atomic.rmw`, `cmpxchg` and `fence` with C++ orderings in 1.1 if the cost shows |
| `i128` in core | Cranelift, LLVM, Zig | Keep as an extension type in 1.0; reconsider if the `builtin` plugin's lowering shows overhead on two-register values |
| Structured control flow | Wasm, SPIR-V, MLIR `scf`, Zig AIR | Keep a CFG; measure reducibility and add derived structure plus a restructuring pass for GPU and Wasm consumers |
| Formal semantics | Wasm (mechanized), LLVM (Alive2, Vellvm), Cranelift (Crocus) | SMT verification of folding and extension rules, then symbolic equivalence in the fuzzer |
| Per-access memory flags (`notrap`, `readonly`) | Cranelift, LLVM (`!invariant.load`) | Data objects cover constant tables; an `invariant` flag on `load` for memory the host promises not to change is a cheap minor-version addition |
| Switch / jump tables | LLVM `switch`, Wasm and Cranelift `br_table` | Add a `switch` terminator in a minor version if TPC-H or NebulaStream kernels show long `condbr` chains; tracing produces chains today |
| Relocations in data | LLVM, Cranelift, QBE | Not needed in 0.4; add a relocation table in a minor version if tables of function pointers appear |
| Overflow-checked arithmetic | LLVM `*.with.overflow`, LuaJIT checked ops | Already covered by `builtin.add_overflow` and friends as extension operations |

**Nothing found in the comparison breaks a NIR design choice.** The gaps are
additions that the versioning rules (§8.2) allow in minor versions, except
structured control flow, which stays a derived view.

## 6. References

- LLVM Language Reference Manual; LLVM Bitcode File Format; Alive2
  (Lopes et al., PLDI 2021).
- MLIR Language Reference; MLIR Bytecode Format.
- WebAssembly Core Specification (W3C), including the 3.0 features (exception
  handling, memory64); Watt, *Mechanising and Verifying the WebAssembly
  Specification*, CPP 2018.
- Cranelift IR reference and `cranelift-module` documentation (Bytecode
  Alliance); VanHattum et al., *Lightweight, Modular Verification for
  WebAssembly-to-Native Instruction Selection* (Crocus), ASPLOS 2024.
- QBE IL documentation (c9x.me/compile).
- SPIR-V Specification (Khronos).
- LuaJIT 2 SSA IR documentation (wiki.luajit.org).
- Zig compiler internals: AIR (`src/Air.zig`).
