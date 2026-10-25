# Component: Extensions

Part of the [Nautilus 0.4 design](README.md) · Milestones M1 (registry M1.7; extensions in the text form M1.3a, in canonical form and images M1.7a), M6
(extension types, plugin migration)

## 1. Purpose

Plugins should be able to define **operations, types, rules and passes**,
and backends should dispatch on them through arrays, at no cost to the core
on the hot path.

## 2. Today

| Mechanism | Used by | Limitation |
|---|---|---|
| **Intrinsics** (`IntrinsicRegistry`): a native function, identified by address, that a backend may replace with instructions; always also a valid call (the fallback invariant) | `builtin` (prefetch: MLIR and AsmJit; overflow, int128: MLIR only), `simd` (MLIR and AsmJit), `specialization` (assume; MLIR only), `std` (math, bit, atomics with orderings; MLIR only), core (`memcpy`, `memmove`, `memset`, `memcmp`; MLIR only, `MLIRMemoryIntrinsics.cpp`) | **The IR only sees a call.** No folding, CSE or hoisting unless effects are declared, and no verifier. Handlers are per backend (`MLIRIntrinsicPlugin`, `AsmJitIntrinsicPlugin`). **An intrinsic exists only if a backend handler registers it:** `IntrinsicRegistry::registerIntrinsic` is called only from `MLIRIntrinsicManager::addIntrinsic` and `AsmJitIntrinsicManager::addIntrinsic`, and the `std` and `specialization` init files are compiled only with MLIR, so a `dev` build has none of them. Entries have no version or signature, and their name is optional. `lookup` takes a mutex |
| **Backends** (`CompilationBackendRegistry`) | `gpu` (CUDA, Metal) | All or nothing |
| **LLVM hooks** (`LLVMBackendHooks`) | `inlining` | MLIR/LLVM only, below the Nautilus IR |
| **New value types** | none possible | `Type` is a closed 13-entry enum. `val<vec<T>>` travels as a pointer, and backends without vector handlers keep results in a **per-thread ring of 32 slots** with a lifetime limit (`docs/simd.md`). `builtin.int128` boxes values the same way (`int128_data*` through `invoke`), in a per-thread ring of 64 slots (`int128.cpp`) |

## 3. Design: "dialects lite"

Plugins register descriptors at static-initialization time, the registry
assigns dense ids, and every consumer dispatches through flat tables. There
are no virtual calls or string lookups per instruction.

### 3.1 Id spaces

- **Opcodes are 16 bits.** Core opcodes take fixed values below 256;
  extension opcodes are assigned densely at registration.
- **Types are 8 bits.** Core types are fixed (ids 0–15); up to 240
  extension types are registered (ids 16–255,
  [13 §2](13-nir-spec.md)).
- **Ids are per process.** Anything persisted (images, artifacts, cache
  keys, dumps) uses `name@version`, and loading maps names to the current
  ids ([07](07-serialization.md)).

### 3.2 Extension types

```cpp
struct TypeInfo {
	std::string_view name;     // "simd.vec<f32,8>", "builtin.i128"
	uint32_t version;
	uint32_t minCompatibleVersion; // oldest image version this type still loads (13 §8.4)
	uint16_t size, align;      // in-memory layout
	RegClass regClass;         // GP, FP, Vector, MultiPart
	uint8_t  parts;            // > 1 for multi-part values (i128 = 2 × GP)
	bool     portable;         // payloads contain no process addresses
	bool     available;        // the host CPU supports this instance natively
	CAbiType cabi;             // C ABI type for native signatures (`__m256`), or none (13 §4.5, calling convention)
};
```

- **Values have real SSA identity.** A backend with support keeps them in
  registers (or register parts). A backend without support keeps them in
  **compiler-managed stack slots**, which removes the ring buffer and its
  lifetime limit.
- **Parameterized types are instantiated at registration, not at trace
  time.** A plugin enumerates every instance it supports when it
  registers (SIMD: each element type × each lane count of the supported
  ISAs, SSE through AVX-512 and NEON), and each instance gets its own
  `TypeId`, so the type table is complete when the registry freezes
  (§3.7). Equal parameters therefore always give the same `TypeId`.
- **CPU-dependent instances** are registered on every host, but marked
  unavailable where the CPU cannot execute them
  (`TypeInfo.available`). Tracing on such a host never creates one (SIMD
  picks its lane count among the available instances). An image that uses
  one still loads, because its name resolves, and backends lower its ops
  through their fallbacks (stack slots, §3.3); `NIR-L013` stays reserved
  ([13 §8.4](13-nir-spec.md)). Such an instance has no C ABI type on that
  host, so it never appears in a native signature there (below).
- **Names:** `<plugin>.<ctor>` optionally followed by `<arg,…>`
  (`simd.vec<f32,8>`); in `name@version`, the version follows the **last**
  `@` ([13 §8.4](13-nir-spec.md)).
- **Plugin names** (the first component of every op, type and intrinsic
  name) follow [13 §9.1](13-nir-spec.md); registration enforces it (§3.7).
- **Extension types in native signatures.** An extension type may appear
  in an exported signature, or as a parameter or result of an external
  callee or `call.indirect`, only if `TypeInfo.cabi` names its C ABI type;
  the verifier rejects other uses ([13 §4.5](13-nir-spec.md), calling convention). Fallbacks
  of extension operations and intrinsics never need one: they use the
  pointer convention of §3.3.
- **Constants of extension types** (an `i128` literal, a vector splat) are
  `ConstPool` entries holding `size` raw bytes plus the type. Serialization
  writes them with the type's `name@version`. Payloads and constants are
  interned **per module** (`ConstPool`, `extra`), never in the registry,
  so tracing never writes to the registry.

### 3.3 Extension operations

```cpp
struct OpInfo {
	std::string_view name;      // "simd.add"
	uint32_t version;
	uint32_t minCompatibleVersion; // oldest image version this op still loads (13 §8.4)
	Signature sig;              // operand/result counts and type constraints
	OperandShape shape;         // kind of a, b, c and the word kinds of the `extra` payload (13 §5.11.1)
	OpTraits traits;            // Pure, Commutative, Idempotent (extension ops never throw, 13 §5.11)
	EffectClass effects;        // None / Read / Write / ReadWrite (joins the effect chain)
	FoldFn fold = nullptr;      // constant folding over ConstPool values
	SimplifyFn simplify = nullptr;
	VerifyFn verify = nullptr;
	PrintFn print = nullptr;    // trailing comment only; the text form comes from `shape` (13 §5.11)
	bool portable = true;       // payloads contain no process addresses
	TargetSet targets = all;    // backends/targets the op is valid on
	Fallback fallback;          // required unless `targets` is restricted
};
```

- **The fallback is mandatory.** It generalizes today's intrinsic invariant:
  either a **native function** with the op's exact signature (in the
  calling convention below), or an
  **expansion** into core ops (and other extension ops) applied by
  legalization ([03](03-conversion-and-passes.md)). Every backend therefore
  supports every extension, and a handler is only an optimization.
- **Fallback calling convention** (needed for extension types, which have
  no C type of their own): every operand or result of an extension type
  is passed as a pointer to a slot of `TypeInfo.size` and `align`, and the
  result is written through a leading out-pointer:
  `void f(R* out, const A* a, int64_t b, …)`. Operands of core types are
  passed by value, and a core-typed result is returned normally. This
  avoids ABI-specific aggregates (`__int128` is two GP registers on SysV
  and by reference on Win64). Legalization materializes the slots
  ([03 §4](03-conversion-and-passes.md)); M3.7's stack-slot fallback uses
  exactly this convention. It applies equally to the native fallback of an
  **intrinsic** whose NIR signature has extension types: "a C function
  with exactly that signature" ([13 §4.6.1](13-nir-spec.md)) means this
  convention for those parameters. Out-slot and operand slots never alias;
  core-typed narrow operands are extended by their signature's extension
  kinds ([13 §4.5.1](13-nir-spec.md)).
- **Operand shape** ([13 §5.11.1](13-nir-spec.md)). `OpInfo.shape`
  declares a field kind for each of `a`, `b` and `c`: `unused` (0),
  `value`, `const` (a value reference that must be a constant), `imm` (a
  `u32` immediate) or, for `c` only, `list` (an `extra` offset). A `list`
  payload `[n, w₀ … wₙ₋₁]` has a fixed prefix of word kinds and an
  optional repeated group; the word kinds are `value`, `const`, `imm`,
  `pointer` (a `pointers` index of kind extension payload address) and
  **`type`** (a `TypeId` in bits 0–7, upper bits 0, as for a vector
  shuffle's element type). The core verifier, canonicalizer, loader,
  printer and parser use only this descriptor to bounds-check, renumber
  (`value` and `type` words) and compact payloads, never plugin code; the
  `verify` hook runs after the core checks ([13 §5.11](13-nir-spec.md)).
  In particular the text form of an extension instruction is printed and
  parsed generically from the shape (M1.3a); the only plugin code the
  printer may call is `OpInfo.print`, which adds a trailing comment that
  the parser ignores.
- **The one exception is target-restricted ops.** Ops that only exist on
  some targets declare `targets` and may omit the
  fallback. The verifier rejects them in a module compiled for any other
  target.
- **Traits make extension ops first-class** for the conversion's folding,
  simplification and CSE. A `Pure` op is merged with identical ops and
  hoisted. An op with effects joins the effect chain.
- **Terminators stay core-only.** Layout, loop info and liveness never need
  to know about plugins. An extension that needs control flow emits core
  branches in its expansion.
- **Payloads** (shuffle masks, GPU address spaces) live in `extra` or
  `ConstPool` and are interned per module, so CSE compares them by index.

### 3.4 Tracing extension ops

```cpp
// in the SIMD plugin's val<vec<T>> operator+
auto& ref = tracing::traceExtOp(SimdOps::add, vecType, {lhs.state, rhs.state});
```

The trace records the op in the same 16 B format. Direct mode (tracing
disabled) is unchanged.

### 3.5 Backend lowering tables

```cpp
// per backend, filled at registration
std::vector<LowerFn> extLowering;     // index = op - 256; nullptr -> fallback
std::vector<TypeLowering> extTypes;   // register class / MLIR type / C++ type
```

| Backend | What a plugin supplies | Without it |
|---|---|---|
| `tbc` | an out-of-line handler `void(*)(uint64_t* fp, const ExtSite*)` (optional), called through **one `tbc` bytecode opcode `EXT`** (`a` = result register, `b` = site index), shaped like `CALL_EXT`: the 8-byte `Instr` cannot hold an extension instruction, so a per-site record `ExtSite {handler, operand and result frame offsets, payload}` holds it. It works in every dispatch skin (tail call, computed goto, switch) and in `tbc.mode=jit` through one generic `EXT` stencil whose `_JIT_SITE` hole is patched per site, as for `CALL_EXT`. Plugins never add dispatch-table entries or stencils | `CALL_EXT` of the native fallback in the §3.3 convention. A value of an extension type lives in the frame's alloca area at its `TypeInfo.size`/`align` (an `i128` in two slots, a vector in an aligned area), not in a 64-bit register slot; `tbc` lifts its current 16-byte alloca alignment limit (`TBCLoweringProvider.cpp`) to the 4096 that `AllocaRecord` allows ([13 §4.5](13-nir-spec.md)) |
| `asmjit` | `lower(ctx, inst)` through a narrow API: operand and result registers or parts, scratch registers, the emitter | call or expansion; values spilled to stack slots |
| MLIR | `lower(builder, inst, operands) -> mlir::Value` plus a type mapping | call or expansion |
| `cpp` | an expression template | call |

This replaces `MLIRIntrinsicPlugin` and `AsmJitIntrinsicPlugin` with one
registration API per backend. The same tables hold intrinsic lowerings,
attached by intrinsic name (§3.7).

### 3.6 Plugin rules and passes

- **Rules** (`fold`, `simplify`) run inside the conversion. This is the
  cheap and preferred way for a plugin to optimize its own ops.
- **Passes** register at `afterConversion` or `beforeLowering`, each marked
  required or optional ([03 §5](03-conversion-and-passes.md)).

### 3.7 Registry lifecycle

- **Registration before the first engine,** from static initializers or
  plugin init functions, as the in-tree plugins already do. Plugin
  libraries are static archives (`add_library` without a type), so a
  registration TU that nothing references is dropped by the linker; each
  plugin anchors its registration TU (an object library, or a symbol the
  plugin's public wrappers reference), and the M1.7 visibility test links
  only the plugin's public API to catch a dropped TU.
- **Frozen at the first `NautilusEngine` construction**, not at the
  first compile, so no trace or compile can overlap registration. Any
  other first reader freezes it too: loading or parsing a NIR image, or
  creating a module through the C API ([09](09-c-api.md)) without an
  engine. The freeze is a release store of a flag; registration checks
  the flag under a mutex; lookups are lock-free acquire loads and need no
  lock after the freeze. The table is complete at that point:
  parameterized types are enumerated at registration (§3.2) and payloads
  are per module. An engine constructed during static initialization
  (a global `NautilusEngine`) freezes the registry before plugins in other
  translation units may have registered, in unspecified order; the
  late-registration error therefore also names the freeze point, and the
  documentation says not to construct engines in static initializers.
- **Plugin names are checked at registration** ([13 §9.1](13-nir-spec.md)),
  so an extension name is never a core token in the text form. A plugin
  name must not be a core mnemonic of [13 §5](13-nir-spec.md) or the first
  dot-separated component of one (`div`, `rem`, `shr`, `cmp`, `cast`,
  `call`, `bind`, `data`, `ptr`), a type name of [13 §2](13-nir-spec.md)
  (`i64`, `ptr`, …), a bare keyword of the grammar
  ([13 §9.2](13-nir-spec.md)), or `nir`. The prefix `core` is reserved for
  the core's own intrinsics (`core.memcpy@1` and the other `mem*`, below);
  a plugin that registers a `core.*` name is rejected.
- **Late registration is an error** (D13). Registering an op, type,
  intrinsic or pass after the freeze, for example from a plugin loaded
  later with `dlopen`, throws an exception that names the extension and
  says that all extensions must be registered before the first engine is
  constructed. The registry is never mutated while engines may read it. An
  engine-scoped registry for late-loaded plugins is a possible later step.
- **Tests:** a test-only `ExtensionRegistry::resetForTesting()`, compiled
  only into a dedicated test binary, lets the freeze and late-registration
  tests run independently of test order. Other test plugins (M3.9)
  register from static initializers.
- **One `ExtensionRegistry`** holds types, ops, intrinsics and passes, and
  absorbs `IntrinsicRegistry`. 0.3's intrinsics become **intrinsics** in the
  sense of [13 §4.6.1](13-nir-spec.md): call targets known by name, with a
  native fallback and optional per-backend lowerings. The plugin API is not
  part of 0.4's compatibility promise (README §5), so there is no shim: the
  in-tree plugins are migrated, and the migration guide covers external
  ones. Plugins move an intrinsic to an extension operation (§3.3) where
  folding, CSE or extension types pay off.
- **Intrinsic identity is backend-independent** (from M1.7). Each
  plugin registers each intrinsic by `name@version`, NIR signature
  (including the per-parameter zext/sext kinds, [13 §4.5.1](13-nir-spec.md)) and effect
  attributes, with its native fallback address, in a **backend-neutral
  init file compiled in every build** (`dev`, `full`, `nocompiler`). The
  core does the same for `core.memcpy@1`, `core.memmove@1`,
  `core.memset@1` and `core.memcmp@1`. Backend handlers are optional, are
  compiled only with their backend, and attach to an intrinsic by name. The
  0.3 address-keyed table is not imported for in-tree intrinsics.
- **Tracing an intrinsic:** a traced `invoke` of a function address that
  is registered as an intrinsic's fallback becomes a callee of kind
  intrinsic at trace time (lookup by address, then name), not an external
  callee ([13 §4.6.1](13-nir-spec.md)).
- **Verification:** `IRVerifier` checks every extension instruction against
  its `Signature`, its `OperandShape` and its `verify` hook, naming the
  plugin on a mismatch. As for the rest of NIR, it runs on every module
  that comes from a deserializer or the C API, and in debug builds after
  every producer ([13 §6.4](13-nir-spec.md)).

## 4. Migration of the in-tree plugins (M6)

| Plugin | Today | In 0.4 |
|---|---|---|
| `builtin` prefetch | intrinsic plus MLIR/AsmJit handlers | intrinsic `builtin.prefetch@1` (call semantics, effect `Read`, locality as a constant argument), with its MLIR and AsmJit lowerings; fallback = the wrapper function. It gains nothing from folding or extension types, so it stays an intrinsic ([13 §4.6.1](13-nir-spec.md) uses it as the example) |
| `builtin` overflow | intrinsic, MLIR handlers only | `builtin.add_overflow` and friends; `Pure`; plugin `fold` |
| `builtin` int128 | a boxed `int128_data*` through `invoke`, a 64-slot thread-local ring, MLIR handlers only | type `builtin.i128` (2 GP parts) plus pure ops; stack-slot fallback (§3.3); **int128 ring removed** |
| `simd` | `vec<T>` as a pointer; ring-buffer fallback | types `simd.vec<T,N>`, enumerated at registration (§3.2); pure ops and mask ops; stack-slot fallback; **ring buffer removed** |
| `specialization` assume | six intrinsics (`nautilus_assume`, `nautilus_assume_aligned`, `_constant`, `_range`, `_nonzero`, `_noalias`, with fallbacks `nautilus_assume_*_stub` in `assume.cpp`), MLIR handlers only | `spec.assume.*`, facts consumed by fold and simplify; fallback = no-op |
| `specialization` `ConditionProfile` | profile with `invoke`, then `setIsTrueProbability` on the condition (`Profile.cpp`) | unchanged; the probability becomes the `condbr` `prob` field ([13 §5.9](13-nir-spec.md)) |
| `std` | wrappers over `invoke`; math, bit (ctlz, cttz, popcount, rotl, rotr, byteswap) and atomics with orderings are intrinsics with MLIR handlers (`StdPluginInit.cpp`, MLIR builds only) | math, bit and atomic functions become intrinsics registered in every build (§3.7), with native fallbacks and MLIR lowerings (AsmJit lowerings optional), plus effect declarations ([06](06-semantics.md)); the atomics are synchronizing intrinsic calls in the memory model ([13 §5.0](13-nir-spec.md)) |
| core `mem*` | MLIR intrinsics registered by the MLIR backend | core intrinsics `core.memcpy@1`, `core.memmove@1`, `core.memset@1`, `core.memcmp@1` with libc fallbacks, registered in every build |
| `gpu` | backend plugin | migrated in M7 ([17](17-gpu.md)): thread-index and barrier ops as extension ops that keep 0.3's CPU fallbacks; `gpu.launch` as an **intrinsic** with call semantics, because a launch may throw ([17 §4.3](17-gpu.md)); a required partition pass |
| `inlining` | `InlineFunctionRegistry` (its own mutex-guarded registry of function bitcode, filled by `NAUTILUS_INLINE` static initializers) and LLVM hooks: `callNameOverride` renames external calls to `ptrToHex(address)`, `preOptModuleTransform` splices registered bitcode, `jitSymbolContributor` maps the hex names back (`InliningPluginInit.cpp`) | unchanged for MLIR in process; `InlineFunctionRegistry` stays outside the `ExtensionRegistry` and is not frozen, because it changes no NIR (only the payload, whose key carries the inlined bitcode's hash); needs MLIR, so its suite runs from the MLIR port (M4.5) on. A module whose MLIR compile used the hooks has process addresses as symbol names, so its native payload is **non-persistable** (the artifact carries only the NIR image) unless every renamed symbol goes through the import table as a `Portable` pointer name; the hash of the inlined bitcode then joins the payload descriptor and the L1/L2 keys ([07 §2](07-serialization.md), [08](08-caching.md)) |
| `profiling` | runtime plugin, no intrinsics; per-operation attribution through `JitSymbolRegistry::resolveLine` (`SampleAggregation.cpp`, MLIR only today) | unchanged; attribution on `asmjit` through its line table ([05 §7](05-backends.md)); its suite runs from M4.8 on |

## 5. Exit criteria

- **M1** (M1.7; text form M1.3a; canonical form and images M1.7a): every in-tree intrinsic visible by name through
  `ExtensionRegistry` in every preset, including `dev` and `nocompiler`; no
  mutex on the lookup path after the freeze. The plugins register with it
  directly from the cut-over on ([16 §2](16-implementation-plan.md)).
- **M4:** every plugin test whose backends are enabled passes on each
  ported backend; the `inlining` suite passes with the MLIR port, the
  `profiling` suite with M4.8.
- **M6:**
  - every plugin suite green on every backend;
  - SIMD tests pass without the 32-value lifetime rule, and int128 tests
    without the 64-value one;
  - the differential fuzzer runs each extension with its handler both
    enabled and disabled.

## 6. Risks

- **Fallbacks that disagree with handlers** cause silent miscompiles. The
  fuzzer runs with handlers enabled and disabled.
- **Dishonest traits** (an impure op marked `Pure`) break CSE. A debug mode
  ignores traits for A/B testing.
- **The plugin lowering API becomes public surface.** Keep it narrow.
- **Id widths:** 65,280 extension opcodes and 240 extension types per
  process. Enumerating parameterized instances (§3.2) spends type ids:
  SIMD needs about 4 element types × 5 lane counts plus masks, well within
  the limit, but registration reports the remaining budget.
