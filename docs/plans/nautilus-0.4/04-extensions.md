# Component: Extensions

Part of the [Nautilus 0.4 design](README.md) · Milestones M1 (registry), M6
(extension types, plugin migration)

## 1. Purpose

Plugins should be able to define **operations, types, rules and passes**,
and backends should dispatch on them through arrays, at no cost to the core
on the hot path.

## 2. Today

| Mechanism | Used by | Limitation |
|---|---|---|
| **Intrinsics** (`IntrinsicRegistry`): a native function, identified by address, that a backend may replace with instructions; always also a valid call (the fallback invariant) | `builtin` (prefetch, overflow, int128), `simd`, `specialization` (assume) | **The IR only sees a call.** No folding, CSE or hoisting unless effects are declared, and no verifier. Handlers are per backend (`MLIRIntrinsicPlugin`, `AsmJitBackendIntrinsic`). `lookup` takes a mutex |
| **Backends** (`CompilationBackendRegistry`) | `gpu` (CUDA, Metal) | All or nothing |
| **LLVM hooks** (`LLVMBackendHooks`) | `inlining` | MLIR/LLVM only, below the Nautilus IR |
| **New value types** | none possible | `Type` is a closed 13-entry enum. `val<vec<T>>` travels as a pointer, and backends without vector handlers keep results in a **per-thread ring of 32 slots** with a lifetime limit (`docs/simd.md`) |

## 3. Design: "dialects lite"

Plugins register descriptors at static-initialization time, the registry
assigns dense ids, and every consumer dispatches through flat tables. There
are no virtual calls or string lookups per instruction.

### 3.1 Id spaces

- **Opcodes are 16 bits.** Core opcodes take fixed values below 256;
  extension opcodes are assigned densely at registration.
- **Types are 8 bits.** Core types are fixed; extension types are
  registered (up to 255 in total).
- **Ids are per process.** Anything persisted (images, artifacts, cache
  keys, dumps) uses `name@version`, and loading maps names to the current
  ids ([07](07-serialization.md)).

### 3.2 Extension types

```cpp
struct TypeInfo {
	std::string_view name;     // "simd.vec<f32,8>", "builtin.i128"
	uint32_t version;
	uint16_t size, align;      // in-memory layout
	RegClass regClass;         // GP, FP, Vector, MultiPart
	uint8_t  parts;            // > 1 for multi-part values (i128 = 2 × GP)
	bool     portable;         // payloads contain no process addresses
};
```

- **Values have real SSA identity.** A backend with support keeps them in
  registers (or register parts). A backend without support keeps them in
  **compiler-managed stack slots**, which removes the ring buffer and its
  lifetime limit.
- **Types are interned,** so equal parameters give the same `TypeId`. SIMD
  picks the lane count per CPU when it creates the type.
- **Constants of extension types** (an `i128` literal, a vector splat) are
  `ConstPool` entries holding `size` raw bytes plus the type. Serialization
  writes them with the type's `name@version`.

### 3.3 Extension operations

```cpp
struct OpInfo {
	std::string_view name;      // "simd.add"
	uint32_t version;
	Signature sig;              // operand/result counts and type constraints
	OpTraits traits;            // Pure, Commutative, Idempotent, NoUnwind
	EffectClass effects;        // None / Read / Write / ReadWrite (joins the effect chain)
	FoldFn fold = nullptr;      // constant folding over ConstPool values
	SimplifyFn simplify = nullptr;
	VerifyFn verify = nullptr;
	PrintFn print = nullptr;    // payload in `extra` (masks, immediates)
	bool portable = true;       // payloads contain no process addresses
	TargetSet targets = all;    // backends/targets the op is valid on
	Fallback fallback;          // required unless `targets` is restricted
};
```

- **The fallback is mandatory.** It generalizes today's intrinsic invariant:
  either a **native function** with the op's exact signature, or an
  **expansion** into core ops (and other extension ops) applied by
  legalization ([03](03-conversion-and-passes.md)). Every backend therefore
  supports every extension, and a handler is only an optimization.
- **The one exception is target-restricted ops.** Ops that only exist on
  some targets (GPU thread ids, barriers) declare `targets` and may omit the
  fallback. The verifier rejects them in a module compiled for any other
  target.
- **Traits make extension ops first-class** for the conversion's folding,
  simplification and CSE. A `Pure` op is merged with identical ops and
  hoisted. An op with effects joins the effect chain.
- **Terminators stay core-only.** Layout, loop info and liveness never need
  to know about plugins. An extension that needs control flow emits core
  branches in its expansion.
- **Payloads** (shuffle masks, prefetch locality, GPU address spaces) live
  in `extra` or `ConstPool` and are interned, so CSE compares them by index.

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
| `tbc` | a handler (optional) | call to the native fallback; values in frame slots |
| `asmjit` | `lower(ctx, inst)` through a narrow API: operand and result registers or parts, scratch registers, the emitter | call or expansion; values spilled to stack slots |
| MLIR | `lower(builder, inst, operands) -> mlir::Value` plus a type mapping | call or expansion |
| `cpp` | an expression template | call |

This replaces `MLIRIntrinsicPlugin` and `AsmJitBackendIntrinsic` with one
registration API per backend.

### 3.6 Plugin rules and passes

- **Rules** (`fold`, `simplify`) run inside the conversion. This is the
  cheap and preferred way for a plugin to optimize its own ops.
- **Passes** register at `afterConversion` or `beforeLowering`, each marked
  required or optional ([03 §5](03-conversion-and-passes.md)).

### 3.7 Registry lifecycle

- **Registration before any tracing,** from static initializers or plugin
  init functions, as the in-tree plugins already do.
- **Frozen after the first compile:** read-only and lock-free.
- **One `ExtensionRegistry`** holds types, ops and passes, and absorbs
  `IntrinsicRegistry`. A deprecated shim registers old-style intrinsics as
  extension ops with `fallback = native function`, so existing plugins keep
  working through 0.4.x.
- **Verification:** `IRVerifier` checks every extension instruction against
  its `Signature` and `verify` hook in debug builds, naming the plugin on a
  mismatch.

## 4. Migration of the in-tree plugins (M6)

| Plugin | Today | In 0.4 |
|---|---|---|
| `builtin` prefetch | intrinsic plus MLIR/AsmJit handlers | `builtin.prefetch`; effect `Read`; payload = locality; fallback = the wrapper function |
| `builtin` overflow | intrinsic | `builtin.add_overflow` and friends; `Pure`; plugin `fold` |
| `builtin` int128 | `val<__int128>` with its own lowering | type `builtin.i128` (2 GP parts) plus pure ops |
| `simd` | `vec<T>` as a pointer; ring-buffer fallback | types `simd.vec<T,N>`; pure ops and mask ops; stack-slot fallback; **ring buffer removed** |
| `specialization` assume | intrinsic | `spec.assume`, a fact consumed by fold and simplify; fallback = no-op |
| `std` | wrappers over `invoke` | unchanged, plus effect declarations ([06](06-semantics.md)) |
| `gpu` | backend plugin | backend on NIR; GPU-only ops (thread id, barriers) as target-restricted extension ops (§3.3) |
| `inlining` | LLVM hooks | unchanged |
| `profiling` | runtime plugin | unchanged |

## 5. Exit criteria

- **M1:** all in-tree intrinsics registered through `ExtensionRegistry`
  with no behavior change; no mutex on the lookup path.
- **M6:**
  - every plugin suite green on every backend;
  - SIMD tests pass without the 32-value lifetime rule;
  - the differential fuzzer runs each extension with its handler both
    enabled and disabled.

## 6. Risks

- **Fallbacks that disagree with handlers** cause silent miscompiles. The
  fuzzer runs with handlers enabled and disabled.
- **Dishonest traits** (an impure op marked `Pure`) break CSE. A debug mode
  ignores traits for A/B testing.
- **The plugin lowering API becomes public surface.** Keep it narrow.
- **Id widths:** 65,280 extension opcodes and 255 types per process.
