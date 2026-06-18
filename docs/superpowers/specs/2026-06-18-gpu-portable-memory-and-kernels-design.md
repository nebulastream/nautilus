# Portable GPU Memory Model, Launch Caching & Shared Memory

**Date:** 2026-06-18
**Scope:** `plugins/gpu` (Metal + CUDA backends), with one small extension point in core (`nautilus/Engine.hpp`).
**Status:** Approved design, ready for implementation planning.

## Background

The GPU plugin lets users write CUDA-style kernels through Nautilus tracing and dispatch
them via `gpu::launch`. Two hardcoded, divergent memory models limit it for larger kernels
over larger data batches:

- **Metal** generates an `MTLBuffer` per pointer argument and copies in/out at a single
  compile-time constant `NAUTILUS_BUFFER_SIZE` (default 4096 bytes). Buffers larger than
  this are silently truncated. Empirically, a 2048-float vecAdd produces wrong results past
  element 1024 unless the global `gpu.metal.bufferSize` option is bumped — and then *every*
  buffer is over-allocated to that size.
- **CUDA** passes raw host pointers straight into `kernel<<<grid,block>>>(args)` and relies on
  the pointers being unified/managed memory (`cudaMallocManaged`); there is no buffer-size
  concept at all.

Additional gaps: Metal rebuilds `MTLDevice`/library/pipeline/queue on **every** launch
(catastrophic for looped/multi-kernel batch workloads), and there is no portable way to
declare block-shared (`threadgroup` / `__shared__`) memory, so block-cooperative algorithms
(tiled reduction, scan) cannot be expressed efficiently.

This design replaces the two hardcoded models with one portable, unified-memory model, caches
the Metal launch context, and adds a portable static shared-memory API. Atomics and device
math intrinsics are explicitly out of scope.

## Goals

1. **Data-driven, copy-free memory** that works identically on Metal and CUDA: allocate once,
   read/write in place, no `memcpy`, no fixed buffer sizes.
2. **Cheap repeated launches** so multi-kernel batch pipelines are practical.
3. **Portable static shared memory** for block-cooperative kernels.
4. All new public APIs lower to **both** Metal and CUDA from the same traced source.

## Non-goals

- Atomics and device math intrinsics (deferred to a follow-up).
- Dynamic (runtime-sized) shared memory (static compile-time `N` only for v1).
- Fat-pointer parameters (one IR/ABI argument expanding into multiple slots).

---

## Section 1 — Portable unified-memory model

### Public API (`plugins/gpu/include/nautilus/gpu/gpu.hpp`)

```cpp
namespace nautilus::gpu {

// Single-slot newtype over val<T*>. raw_type = T*, so it occupies exactly one
// IR/ABI argument slot. Indexable like a pointer inside kernels.
template <class T>
class Array {
public:
    using raw_type = T*;

    // Tracing constructor: built from the function-argument value ref by the
    // arg-factory hook (see "Core extension point"). Single pointer slot.
    explicit Array(val<T*> ptr);

    // Host-handle members (used outside tracing, on the real allocation).
    T*     data()  const;
    size_t size()  const;   // element count
    size_t bytes() const;   // size() * sizeof(T)
    bool   owns()  const;

    // Pointer-like access inside kernels (forwards to the underlying val<T*>).
    auto operator[](val<uint64_t> index);   // mirrors val<T*>::operator[]

    // Exposes tracing state so call sites (getArgumentReferences/StateResolver)
    // treat an Array argument as its underlying pointer.
    tracing::TypedValueRef getState() const;   // == ptr_.getState()

private:
    val<T*> ptr_;
    // Host-side metadata (valid outside tracing); zero/unused during tracing.
    T*     hostPtr_ = nullptr;
    size_t count_   = 0;
    bool   owns_    = false;
};

// Allocation entry points — all return a gpu::Array<T> and register the
// allocation's byte length in the runtime size table (see below).
template <class T> Array<T> allocUnified(size_t count);          // new unified buffer (owns)
template <class T> Array<T> copy(const T* src, size_t count);    // new unified buffer + copy in (owns)
template <class T> Array<T> wrap(T* ptr, size_t count);          // wrap caller's unified ptr, NO copy (non-owning)
template <class T> void     freeUnified(Array<T>& a);            // frees iff owns()

} // namespace nautilus::gpu
```

Semantics:

- **`allocUnified<T>(count)`** — allocates `count*sizeof(T)` of unified, GPU-accessible memory.
  Fill it on the CPU; read results directly after launch. No copy in either direction.
- **`copy<T>(src, count)`** — allocates a unified buffer and performs the single boundary copy
  *in* from arbitrary host memory. Thereafter resident/zero-copy. Owns the allocation.
- **`wrap<T>(ptr, count)`** — wraps a pointer the caller guarantees is already
  unified-compatible (page-aligned on Metal, `cudaMallocManaged` on CUDA). No allocation, no
  copy. **Non-owning** (never frees the underlying memory). If the pointer is not actually
  compatible, that is a caller error (Metal's `newBufferWithBytesNoCopy` rejects non-page-aligned
  pointers).
- **`freeUnified`** — frees the underlying memory iff the `Array` owns it, and removes its size
  table entry. `Array`'s destructor performs the same (RAII).

The previous `in/out/inout` direction descriptors and the `gpu::buffer(ptr, bytes)` escape
hatch are **removed from the design**: external memory enters via `copy`, outputs come from
`allocUnified` read in place, and pre-compatible memory uses `wrap`.

### Backend-neutral allocator implementation

`allocUnified` / `copy` / `wrap` / `freeUnified` are non-traced host functions in the
nautilus-gpu runtime that dispatch on the active backend (Metal vs CUDA), chosen the same way
the compilation backend is selected.

- **Metal:** `posix_memalign` to the system page size (16 KB on Apple Silicon), with the byte
  length rounded up to a page multiple — this is what makes the pointer legal for
  `newBufferWithBytesNoCopy`. `copy` then `memcpy`s `src` in. `wrap` takes the caller's pointer
  as-is.
- **CUDA:** `cudaMallocManaged` (+ `memcpy`/`cudaMemcpy` for `copy`). `wrap` takes the caller's
  managed pointer as-is. `freeUnified` → `cudaFree` for owned allocations.

### Runtime size table

A process-global, mutex-guarded `std::unordered_map<void*, size_t>` in the nautilus-gpu runtime
maps allocation base pointer → byte length. Populated on `allocUnified`/`copy`/`wrap`
construction, removed on destruction/`freeUnified`. Exposed via:

```cpp
extern "C" uint64_t nautilus_gpu_buffer_bytes(void* ptr); // returns size, or aborts/throws if unknown
```

Lookups happen once per launch (not hot). Concurrent allocation is serialized by the mutex.

### Core extension point (`nautilus/Engine.hpp`)

`createTraceableArgument` currently hardcodes constructing `val<raw_type>(ref)`. Replace the
final construction with an argument-factory trait that defaults to the current behavior and can
be specialized per parameter type:

```cpp
// Default factory — unchanged behavior for every existing val<T> parameter.
template <typename ArgValueType>
struct arg_factory {
    static ArgValueType make(tracing::TypedValueRef ref) {
        return val<typename ArgValueType::raw_type>(ref);
    }
};

template <typename ArgValueType, size_t I>
auto createTraceableArgument() {
    auto type     = tracing::TypeResolver<typename ArgValueType::raw_type>::to_type();
    auto valueRef = tracing::registerFunctionArgument(type, I);
    return arg_factory<ArgValueType>::make(valueRef);
}
```

The GPU plugin specializes `arg_factory<gpu::Array<T>>` to build `Array<T>(val<T*>(ref))`. This
is the *only* core change; it is general-purpose (not GPU-specific) and covers both kernel
tracing and host registration, since both route through `createTraceableArgument`.

`gpu::Array<T>` satisfies the machinery's requirements: it provides `raw_type = T*` (so type,
slot, and raw signature derivation via `nautilus_function_traits` all work) and exposes
`getState()` (so `getArgumentReferences`/`StateResolver` treat an `Array` argument as its
pointer). A `StateResolver<gpu::Array<T>>` specialization (or a matching `.state`/`getState()`
surface) is added so call sites extract the underlying pointer ref.

### Kernel and host-entry signatures

Kernels and the host entry function take `gpu::Array<T>` for pointer arguments (scalars stay
`val<S>`), enforcing that every kernel pointer argument is a tracked unified buffer:

```cpp
static auto vecAdd = gpu::NautilusKernelFunction{"vecAdd",
    [](gpu::Array<float> a, gpu::Array<float> b, gpu::Array<float> c) {
        auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
        c[tid] = a[tid] + b[tid];
    }};

void launchVecAdd(gpu::Array<float> a, gpu::Array<float> b, gpu::Array<float> c, val<uint32_t> n) {
    auto blocks = (n + val<uint32_t>(255u)) / val<uint32_t>(256u);
    gpu::launch(vecAdd, gpu::GridDim{blocks}, gpu::BlockDim{256}, a, b, c);
}
```

The kernel's *logical* extent (the bounds `n`) remains an explicit scalar argument: inside the
traced host function the `Array`'s length is host-side metadata, not a traced value, so grid
dimensions and in-kernel bounds are driven by a scalar `n` as today. The size table governs
only host-side allocation/binding.

### Lowering

Both backends look up each pointer argument's length via `nautilus_gpu_buffer_bytes(ptr)`
(function address embedded as a literal, the existing external-call codegen pattern):

- **Metal host code:** for each pointer argument,
  `newBufferWithBytesNoCopy:ptr length:nautilus_gpu_buffer_bytes(ptr) options:MTLResourceStorageModeShared deallocator:nil`.
  No `newBufferWithBytes`, no `memcpy`. `NAUTILUS_BUFFER_SIZE` removed.
- **CUDA host code:** pass the managed pointer straight into the launch; the table is not
  consulted (managed memory needs no sizing).

Removed: `NAUTILUS_BUFFER_SIZE` macro and the `gpu.metal.bufferSize` engine option.

---

## Section 2 — Launch-context caching (Metal codegen; internal, no API)

Today the generated Metal host dispatch block creates `MTLCreateSystemDefaultDevice()`, reloads
the metallib (`newLibraryWithURL`), builds the pipeline state, and creates a command queue on
**every** launch. Restructure the codegen so per-dispatch-site handles are **function-local
`static`** (C++ guarantees thread-safe one-time init):

- `static id<MTLDevice>` — acquired once.
- `static id<MTLCommandQueue>` — created once.
- `static id<MTLLibrary>` — loaded once from the metallib path.
- `static id<MTLComputePipelineState>` — built once per kernel.

Only the command buffer, encoder, buffer binding, dispatch, and `waitUntilCompleted` happen per
call. CUDA is unchanged (nvcc runtime already caches modules; `cudaDeviceSynchronize()` stays).
No user-facing API change.

---

## Section 3 — Static shared memory (portable)

### Public API

```cpp
namespace nautilus::gpu {
template <class T, size_t N> val<T*> sharedArray();   // compile-time N
}
```

Returns a pointer into block-shared memory. Combined with the existing `gpu::syncThreads()`
barrier, this enables tiled reduction/scan.

### Lowering

A new intrinsic carries the compile-time byte size and alignment (`N*sizeof(T)`,
`alignof(T)`), read as IR constants. Each call site emits a uniquely named declaration in the
kernel prologue and returns its address:

- **Metal:** `threadgroup T name[N];`
- **CUDA:** `__shared__ T name[N];`
- **CPU fallback (interpreter):** a `thread_local` static buffer of `N*sizeof(T)` (single-block
  semantics — threads run serially).

The address-space keyword differs per backend, so the intrinsic handler is registered
per-backend (like the existing thread-index intrinsics). Barriers already lower
(`threadgroup_barrier` / `__syncthreads`); no new barrier API.

---

## Testing & rollout

- **Migrate** the 7 kernels in `plugins/gpu/test/common/GPUFunctions.hpp` and their reference
  `.metal` / `.cu` / `.host.cpp` data to the `gpu::Array` API; update codegen reference files.
- **New tests:**
  - Large-batch correctness (the 2048-float vecAdd that currently truncates at 1024) — must
    pass with no size option.
  - `copy` / `allocUnified` / `wrap` round-trips on Metal execution.
  - A shared-memory tiled reduction (Metal execution + Metal/CUDA codegen reference).
  - Launch caching: a kernel launched in a host-side loop produces correct results (behavioral;
    caching is otherwise observed via codegen reference diff).
- **Keep green:** existing `nautilus-gpu-tracing-tests`, `nautilus-gpu-codegen-tests`,
  `nautilus-gpu-execution-tests`, and the core test suite (the `createTraceableArgument` hook
  must not change behavior for any existing `val<T>` parameter).
- **Order of landing (each independently testable):**
  1. §1 core hook (`arg_factory`) + `gpu::Array` + allocator/table + Metal/CUDA lowering.
  2. §2 Metal launch caching.
  3. §3 `sharedArray`.

## Risks

- **Core `arg_factory` hook** must be a strict no-op for all existing parameter types — guarded
  by the full core test suite.
- **`newBufferWithBytesNoCopy` alignment**: only `allocUnified`/`copy` guarantee page alignment;
  `wrap` trusts the caller. Document clearly; the Metal API will reject violations at runtime.
- **Size-table lifetime**: an `Array` must outlive any launch that uses its pointer; freeing
  before dispatch removes the table entry and fails the lookup. This matches normal buffer
  lifetime expectations.
