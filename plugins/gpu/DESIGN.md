# GPU Support in Nautilus via MLIR Backend -- High-Level Design

## Context

Nautilus needs GPU kernel compilation support for **both NVIDIA (CUDA/PTX) and Apple (Metal)** platforms. Users write kernels as normal Nautilus functions using `val<T>` types, launch them via a `gpu::launch()` API, and the MLIR backend compiles kernel code to the appropriate GPU format while generating host-side launch code. The existing CPU MLIR pipeline (LLVM dialect) must remain untouched. All GPU code lives in this separate `plugins/gpu/` directory, only linked when enabled.

---

## 1. User-Facing API (Platform-Agnostic)

**Location:** `plugins/gpu/include/nautilus/gpu/gpu.hpp`

The user API is **identical** regardless of target GPU platform. Users never write CUDA- or Metal-specific code.

- **GPU intrinsics** (`threadIdx_x()`, `blockDim_x()`, `syncThreads()`, etc.) are implemented as `invoke<>()` wrappers around **stub functions** whose addresses serve as identity keys for the intrinsic plugin system.
- **`launch()`** is a template that calls `setGrid()`, `setBlock()`, then `kernel(args...)` -- all traced normally through the existing tracing system.
- **CPU fallback** is automatic: stubs return `0` for thread/block indices, `1` for dimensions, and `launch()` just calls the kernel directly as a single-threaded function.

### Example Usage

```cpp
// Vector scale kernel: out[i] = in[i] * scalar
static auto vecScaleKernel =
    NautilusFunction{"vecScale", [](val<uint32_t*> in, val<uint32_t*> out, val<uint32_t> scalar) {
        auto tid = gpu::threadIdx_x();
        out[tid] = in[tid] * scalar;
    }};

// Host function that launches vector scale
void gpuLaunchVecScale(val<uint32_t*> in, val<uint32_t*> out, val<uint32_t> scalar) {
    gpu::launch(vecScaleKernel, gpu::GridDim{(uint32_t) 1}, gpu::BlockDim{(uint32_t) 256}, in, out, scalar);
}
```

---

## 2. Tracing & IR Generation (No Changes Needed)

The existing tracing system handles everything naturally:

```
gpu::launch(myKernel, grid, block, a, b, c)
  --> traces as:
      ProxyCall(_gpu_set_grid, gx, gy, gz)
      ProxyCall(_gpu_set_block, bx, by, bz)
      NautilusCall("myKernel", a, b, c)    // kernel body traced separately
```

The kernel body (containing `threadIdx_x()` etc.) is traced via `traceNautilusCall()` which adds it as a separate `FunctionOperation` in the IRGraph. All control flow inside the kernel is captured by the standard tracing mechanism.

**IR structure produced:**
```
FunctionOperation("execute"):           // host function
  ProxyCallOp(_gpu_set_grid, gx, gy, gz)
  ProxyCallOp(_gpu_set_block, bx, by, bz)
  CallOp("myKernel", a, b, c)

FunctionOperation("myKernel"):          // kernel function
  ProxyCallOp(_gpu_threadIdx_x) -> tid
  LoadOp, MulOp, StoreOp, ...          // kernel body with full control flow
  ReturnOp
```

---

## 3. MLIR Lowering (No Changes to Existing Pipeline)

`MLIRLoweringProvider` lowers the IRGraph to MLIR as usual. The resulting MLIR module contains:

```mlir
func.func @execute(%a: !llvm.ptr, %b: !llvm.ptr, %c: !llvm.ptr) {
  call @_gpu_set_grid(%gx, %gy, %gz) : (i32, i32, i32) -> ()
  call @_gpu_set_block(%bx, %by, %bz) : (i32, i32, i32) -> ()
  call @myKernel(%a, %b, %c) : (!llvm.ptr, !llvm.ptr, !llvm.ptr) -> ()
  return
}

func.func @myKernel(%a: !llvm.ptr, %b: !llvm.ptr, %c: !llvm.ptr) {
  %tid = call @_gpu_threadIdx_x() : () -> i32
  // ... kernel body (arith, cf, llvm dialect ops) ...
  return
}
```

All in the standard Nautilus dialects (func, arith, cf, llvm). No new dialects.

---

## 4. GPU Pass -- The Core New Component

**Location:** `plugins/gpu/src/MLIRGPUPass.hpp/.cpp`

A new MLIR pass that runs **after** MLIR generation but **before** the standard lowering passes in `MLIRCompilationBackend::compile()`. This is the only modification to the existing compilation backend -- a single conditional insertion point:

```cpp
// In MLIRCompilationBackend::compile(), after generateModuleFromIR():
#ifdef ENABLE_GPU_BACKEND
    gpu::MLIRGPUPass gpuPass;
    gpuPass.run(mlirModule, dumpHandler, options);
#endif
// Existing pipeline continues unchanged
MLIRPassManager::lowerAndOptimizeMLIRModule(mlirModule, {});
```

**What the GPU pass does (platform-agnostic):**

### Step 1: Pattern Detection
Scan the host function for the sentinel sequence:
- `call @_gpu_set_grid(gx, gy, gz)`
- `call @_gpu_set_block(bx, by, bz)`
- `call @myKernel(args...)`

### Step 2: Kernel Extraction & Compilation
- Clone `@myKernel` into a **separate MLIR module**
- Delegate to the appropriate **GPUKernelCompiler** (CUDA or Metal, selected by platform/options)
- Receive compiled kernel binary (PTX or MSL source)

### Step 3: Host Code Replacement
Replace the sentinel sequence + kernel call in the host module with **GPURuntimeLib** calls:

```mlir
// Replace set_grid + set_block + kernel call with:
%mod  = call @_nrt_gpu_load_module(%kernel_data, %kernel_size) -> !llvm.ptr
%kern = call @_nrt_gpu_get_kernel(%mod, %kernel_name) -> !llvm.ptr
// ... pack args into void** array via alloca + store + GEP ...
call @_nrt_gpu_launch_kernel(%kern, %gx, %gy, %gz, %bx, %by, %bz, %args, %shared_mem)
call @_nrt_gpu_synchronize()
```

All emitted using standard LLVM dialect ops (alloca, store, GEP, call). No new dialects.

### Step 4: Remove kernel function from host module
The kernel function `@myKernel` is no longer needed in the host module (it was compiled separately). Remove it.

---

## 5. GPU Kernel Compiler -- Pluggable Backend

### 5a. CUDA Kernel Compiler (`plugins/gpu/src/cuda/`)

Takes a kernel `func::FuncOp`, produces PTX bytes via LLVM NVPTX backend.

**Pipeline:** Clone kernel -> Replace intrinsic calls with NVVM LLVM intrinsics -> Add `nvvm.kernel` attribute -> Run standard lowering passes -> Translate to LLVM IR (target: `nvptx64-nvidia-cuda`) -> Emit PTX.

All stays within LLVM dialect -- NVVM intrinsics are just `llvm.call` ops.

### 5b. Metal Kernel Compiler (`plugins/gpu/src/metal/`)

Takes a kernel `func::FuncOp`, produces Metal Shading Language (MSL) source code.

**Why MSL source code?** Apple's LLVM AIR backend is not open-source. Metal framework compiles MSL at runtime via `MTLDevice::newLibraryWithSource:`. This mirrors the existing C++ backend approach.

**Pipeline:** Clone kernel -> Walk MLIR operations -> Emit MSL source code with `[[thread_position_in_threadgroup]]` etc. kernel attributes -> Return MSL string (compiled at runtime by Metal framework).

---

## 6. GPU Runtime Library -- Pluggable Backend

Unified C interface wrapping platform-specific GPU APIs:

| Function | CUDA Driver API | Metal Framework |
|---|---|---|
| `gpuLoadModule` | `cuModuleLoadData` (PTX) | `MTLDevice::newLibraryWithSource:` (MSL) |
| `gpuGetKernel` | `cuModuleGetFunction` | `MTLLibrary::newFunctionWithName:` |
| `gpuLaunchKernel` | `cuLaunchKernel` | `MTLComputeCommandEncoder` dispatch |
| `gpuSynchronize` | `cuCtxSynchronize` | `MTLCommandBuffer::waitUntilCompleted` |
| `gpuMalloc` | `cuMemAlloc` | `MTLDevice::newBufferWithLength:` |
| `gpuFree` | `cuMemFree` | Release MTLBuffer |
| `gpuMemcpyHtoD` | `cuMemcpyHtoD` | `memcpy` into MTLBuffer (shared memory) |
| `gpuMemcpyDtoH` | `cuMemcpyDtoH` | `memcpy` from MTLBuffer (shared memory) |

---

## 7. Intrinsic Mapping

| Nautilus API | CUDA (NVVM) | Metal (MSL) |
|---|---|---|
| `threadIdx_x()` | `llvm.nvvm.read.ptx.sreg.tid.x` | `thread_position_in_threadgroup.x` |
| `threadIdx_y()` | `llvm.nvvm.read.ptx.sreg.tid.y` | `thread_position_in_threadgroup.y` |
| `threadIdx_z()` | `llvm.nvvm.read.ptx.sreg.tid.z` | `thread_position_in_threadgroup.z` |
| `blockIdx_x()` | `llvm.nvvm.read.ptx.sreg.ctaid.x` | `threadgroup_position_in_grid.x` |
| `blockIdx_y()` | `llvm.nvvm.read.ptx.sreg.ctaid.y` | `threadgroup_position_in_grid.y` |
| `blockIdx_z()` | `llvm.nvvm.read.ptx.sreg.ctaid.z` | `threadgroup_position_in_grid.z` |
| `blockDim_x()` | `llvm.nvvm.read.ptx.sreg.ntid.x` | `threads_per_threadgroup.x` |
| `blockDim_y()` | `llvm.nvvm.read.ptx.sreg.ntid.y` | `threads_per_threadgroup.y` |
| `blockDim_z()` | `llvm.nvvm.read.ptx.sreg.ntid.z` | `threads_per_threadgroup.z` |
| `gridDim_x()` | `llvm.nvvm.read.ptx.sreg.nctaid.x` | `threadgroups_per_grid.x` |
| `gridDim_y()` | `llvm.nvvm.read.ptx.sreg.nctaid.y` | `threadgroups_per_grid.y` |
| `gridDim_z()` | `llvm.nvvm.read.ptx.sreg.nctaid.z` | `threadgroups_per_grid.z` |
| `syncThreads()` | `llvm.nvvm.barrier0` | `threadgroup_barrier(mem_flags::mem_threadgroup)` |

---

## 8. Data Flow

```
User Code (platform-agnostic)
  |  gpu::launch(kernel, grid, block, args...)
  v
Tracing (unchanged)
  |  ProxyCall(set_grid), ProxyCall(set_block), NautilusCall(kernel)
  v
IR Generation (unchanged)
  |  IRGraph with host FunctionOp + kernel FunctionOp
  v
MLIR Lowering (unchanged -- MLIRLoweringProvider)
  |  MLIR module with func.func @execute + func.func @myKernel
  v
+--GPU Pass (platform-agnostic)--------------------------------+
|  1. Detect set_grid/set_block/kernel_call pattern             |
|  2. Extract @myKernel -> separate MLIR module                 |
|  3. Delegate to platform-specific kernel compiler:            |
|     +-- CUDA: replace intrinsics w/ NVVM -> lower -> PTX     |
|     +-- Metal: walk MLIR -> emit MSL source code              |
|  4. Replace host pattern with runtime API calls               |
|  5. Embed kernel binary/source as constant in host module     |
+---------------------------------------------------------------+
  |
  v
Standard MLIR Lowering Passes (unchanged)
  v
LLVM IR Optimization + JIT (unchanged)
  v
Executable
  +-- CUDA: host calls cuModuleLoadData(PTX) + cuLaunchKernel
  +-- Metal: host calls MTLDevice.newLibraryWithSource(MSL) + dispatch
```

---

## 9. Key Design Decisions

| Decision | Rationale |
|---|---|
| **Top-level `plugins/gpu/` directory** | Keeps GPU code fully separate from core nautilus. |
| **Platform-agnostic user API** | One kernel runs on CUDA, Metal, or CPU. No `#ifdef` in user code. |
| **CUDA: LLVM dialect + NVVM intrinsics** | No new MLIR dialects needed. |
| **Metal: MSL source code generation** | Apple's AIR backend is not in open-source LLVM. |
| **Unified runtime interface** | GPU pass emits the same host code regardless of target. |
| **Post-lowering MLIR pass** | Launch pattern spans multiple ops. Module-level pass is cleaner. |
| **Stub functions as pointer identity** | Reuses existing `invoke<>` + `ProxyCallOperation` infrastructure. |
| **CUDA Driver API** (not Runtime API) | Better fit for JIT compilation. |
| **Metal shared memory by default** | Avoids explicit copies on Apple Silicon unified memory. |

---

## 10. Files Modified (Existing)

| File | Change |
|---|---|
| `CMakeLists.txt` (root) | Add `ENABLE_GPU_BACKEND` option + `add_subdirectory(plugins/gpu)` |
| `nautilus/include/nautilus/common/config.h.in` | Add `#cmakedefine ENABLE_GPU_BACKEND`, `ENABLE_GPU_CUDA`, `ENABLE_GPU_METAL` |
| `nautilus/src/nautilus/compiler/backends/mlir/MLIRCompilationBackend.cpp` | Add `#ifdef ENABLE_GPU_BACKEND` block |
