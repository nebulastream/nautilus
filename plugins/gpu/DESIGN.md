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

### Stub Functions

**Location:** `plugins/gpu/include/nautilus/gpu/gpu_stubs.hpp`

```cpp
// CPU fallback implementations -- also serve as pointer identity for intrinsic registry
inline uint32_t _gpu_threadIdx_x() { return 0; }
inline uint32_t _gpu_blockDim_x() { return 1; }
inline void     _gpu_set_grid(uint32_t, uint32_t, uint32_t) {}
inline void     _gpu_set_block(uint32_t, uint32_t, uint32_t) {}
inline void     _gpu_syncThreads() {}
// ... etc for all 12 indexing intrinsics + syncThreads
```

Public API wraps these:
```cpp
val<uint32_t> threadIdx_x() { return invoke<uint32_t>(&_gpu_threadIdx_x); }
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

All emitted using standard LLVM dialect ops (alloca, store, GEP, call). No new dialects. The runtime function names are the same for both backends -- the actual implementation differs.

### Step 4: Remove kernel function from host module
The kernel function `@myKernel` is no longer needed in the host module (it was compiled separately). Remove it.

---

## 5. GPU Kernel Compiler -- Pluggable Backend

**Abstract interface:** `plugins/gpu/src/GPUKernelCompiler.hpp`

```cpp
class GPUKernelCompiler {
public:
    virtual ~GPUKernelCompiler() = default;

    /// Compile a kernel function (as MLIR func.func) to a GPU binary blob.
    /// Returns the binary data (PTX, MSL source, etc.)
    virtual std::vector<uint8_t> compile(
        mlir::func::FuncOp kernelFunc,
        mlir::MLIRContext& context) = 0;
};
```

### 5a. CUDA Kernel Compiler

**Location:** `plugins/gpu/src/cuda/CUDAKernelCompiler.hpp/.cpp`

Takes a kernel `func::FuncOp`, produces PTX bytes.

**Pipeline:**
1. Clone kernel into fresh MLIR module
2. Replace all GPU intrinsic proxy calls with NVVM LLVM intrinsics (see Intrinsic Mapping table below)
3. Add `nvvm.kernel` attribute to function
4. Run standard lowering: Math->LLVM, Func->LLVM, CF->LLVM, Arith->LLVM
5. Translate MLIR to LLVM IR (target: `nvptx64-nvidia-cuda`, data layout for NVPTX)
6. Run LLVM optimization passes for NVPTX
7. Emit PTX via LLVM NVPTX backend

All stays within LLVM dialect -- NVVM intrinsics are just `llvm.call` ops to well-known LLVM intrinsic names.

### 5b. Metal Kernel Compiler

**Location:** `plugins/gpu/src/metal/MetalKernelCompiler.hpp/.cpp`

Takes a kernel `func::FuncOp`, produces Metal Shading Language (MSL) source code.

**Why MSL source code (not AIR binary)?**
- Apple's LLVM AIR backend is not open-source and not available in standard LLVM builds
- Metal framework can compile MSL source at runtime via `MTLDevice::newLibraryWithSource:`
- MSL is a C++-like language, making code generation straightforward from Nautilus MLIR
- This mirrors the approach of the existing C++ backend (`nautilus/src/nautilus/compiler/backends/cpp/`)

**Pipeline:**
1. Clone kernel into fresh MLIR module
2. Walk the MLIR operations and emit MSL source code
3. The MSL code generator walks MLIR operations and emits corresponding MSL:
   - `arith::AddIOp` -> `+`
   - `arith::CmpIOp` -> comparison operators
   - `LLVM::LoadOp` / `LLVM::StoreOp` -> pointer dereference
   - `cf::BrOp` / `cf::CondBrOp` -> goto / if-else
   - Control flow (if/else, loops) maps directly to MSL control flow
4. Function signature is decorated with Metal kernel attributes:
   ```metal
   kernel void myKernel(
       device uint32_t* a [[buffer(0)]],
       device uint32_t* b [[buffer(1)]],
       device uint32_t* c [[buffer(2)]],
       uint3 threadIdx [[thread_position_in_threadgroup]],
       uint3 blockIdx  [[threadgroup_position_in_grid]],
       uint3 blockDim  [[threads_per_threadgroup]],
       uint3 gridDim   [[threadgroups_per_grid]]
   ) { ... }
   ```
5. Return MSL source as string (compiled at runtime by Metal framework)

**Alternative approach (future):** If Apple opens the AIR backend or MLIR gains official Metal/AIR support, the pipeline could switch to LLVM IR -> AIR -> metallib, similar to the CUDA path.

---

## 6. GPU Runtime Library -- Pluggable Backend

**Abstract interface:** `plugins/gpu/src/GPURuntimeLib.hpp`

```cpp
namespace nautilus::gpu::runtime {
    // These functions have the SAME signature on both platforms.
    // The implementation differs based on which backend is compiled.
    void* gpuLoadModule(const void* kernelData, size_t kernelSize);
    void* gpuGetKernel(void* module, const char* kernelName);
    void  gpuLaunchKernel(void* kernel,
                          uint32_t gridX, uint32_t gridY, uint32_t gridZ,
                          uint32_t blockX, uint32_t blockY, uint32_t blockZ,
                          void** args, size_t sharedMem);
    void  gpuSynchronize();
    void* gpuMalloc(size_t size);
    void  gpuFree(void* ptr);
    void  gpuMemcpyHtoD(void* dst, const void* src, size_t size);
    void  gpuMemcpyDtoH(void* dst, const void* src, size_t size);
}
```

### 6a. CUDA Runtime Implementation

**Location:** `plugins/gpu/src/cuda/CUDARuntimeLib.cpp`

Wraps CUDA Driver API:

| Runtime Function | CUDA Driver API |
|---|---|
| `gpuLoadModule` | `cuModuleLoadData` (loads PTX) |
| `gpuGetKernel` | `cuModuleGetFunction` |
| `gpuLaunchKernel` | `cuLaunchKernel` |
| `gpuSynchronize` | `cuCtxSynchronize` |
| `gpuMalloc` | `cuMemAlloc` |
| `gpuFree` | `cuMemFree` |
| `gpuMemcpyHtoD` | `cuMemcpyHtoD` |
| `gpuMemcpyDtoH` | `cuMemcpyDtoH` |

### 6b. Metal Runtime Implementation

**Location:** `plugins/gpu/src/metal/MetalRuntimeLib.mm` (Objective-C++ for Metal framework)

| Runtime Function | Metal API |
|---|---|
| `gpuLoadModule` | `MTLDevice::newLibraryWithSource:` (compiles MSL at runtime) |
| `gpuGetKernel` | `MTLLibrary::newFunctionWithName:` + `newComputePipelineStateWithFunction:` |
| `gpuLaunchKernel` | `MTLComputeCommandEncoder` + `dispatchThreadgroups:threadsPerThreadgroup:` |
| `gpuSynchronize` | `MTLCommandBuffer::waitUntilCompleted` |
| `gpuMalloc` | `MTLDevice::newBufferWithLength:options:` |
| `gpuFree` | Release MTLBuffer |
| `gpuMemcpyHtoD` | `memcpy` into `MTLBuffer::contents()` (shared memory) |
| `gpuMemcpyDtoH` | `memcpy` from `MTLBuffer::contents()` (shared memory) |

**Note:** Metal's memory model differs from CUDA. With `MTLResourceStorageModeShared`, buffers are accessible from both CPU and GPU without explicit copies. The runtime can optimize by detecting shared memory availability on Apple Silicon.

---

## 7. Directory Structure

```
plugins/
  gpu/
    CMakeLists.txt                           # Top-level: detects platform, includes cuda/ and/or metal/
    DESIGN.md                                # This document
    include/nautilus/gpu/
      gpu.hpp                                # Public API (platform-agnostic)
      gpu_stubs.hpp                          # Internal: stub functions for CPU fallback
    src/
      GPUKernelCompiler.hpp                  # Abstract kernel compiler interface
      GPURuntimeLib.hpp                      # Runtime function declarations (unified interface)
      MLIRGPUPass.hpp/.cpp                   # Platform-agnostic: pattern detection, kernel extraction, host rewrite
      cuda/
        CUDAKernelCompiler.hpp/.cpp          # MLIR -> PTX via LLVM NVPTX
        CUDARuntimeLib.cpp                   # CUDA Driver API wrappers
      metal/
        MetalKernelCompiler.hpp/.cpp         # MLIR -> MSL source code generation
        MetalRuntimeLib.mm                   # Metal framework wrappers (Objective-C++)
    test/
      CMakeLists.txt
      GPULaunchTest.cpp                      # Integration tests
      GPUFallbackTest.cpp                    # CPU fallback tests (no GPU required)
```

---

## 8. CMake Integration

**Root `CMakeLists.txt`:**
```cmake
cmake_dependent_option(ENABLE_GPU_BACKEND "Enable GPU kernel compilation" OFF "ENABLE_MLIR_BACKEND" OFF)

# After add_subdirectory(nautilus):
if (ENABLE_GPU_BACKEND)
    add_subdirectory(plugins/gpu)
endif()
```

**`nautilus/include/nautilus/common/config.h.in`:**
```cmake
#cmakedefine ENABLE_GPU_BACKEND
#cmakedefine ENABLE_GPU_CUDA
#cmakedefine ENABLE_GPU_METAL
```

**`plugins/gpu/CMakeLists.txt`:**
```cmake
# Platform detection and sub-backend selection
if (APPLE)
    option(ENABLE_GPU_METAL "Enable Metal GPU backend" ON)
    option(ENABLE_GPU_CUDA "Enable CUDA GPU backend" OFF)
else()
    option(ENABLE_GPU_METAL "Enable Metal GPU backend" OFF)
    option(ENABLE_GPU_CUDA "Enable CUDA GPU backend" ON)
endif()

# Common GPU sources (added to nautilus target)
target_include_directories(nautilus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_source_files(nautilus
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MLIRGPUPass.cpp
)

# CUDA backend
if (ENABLE_GPU_CUDA)
    find_package(CUDAToolkit REQUIRED)
    add_compile_definitions(ENABLE_GPU_CUDA)
    target_link_libraries(nautilus PRIVATE CUDA::cuda_driver)
    target_link_libraries(nautilus PRIVATE LLVMNVPTXCodeGen LLVMNVPTXDesc LLVMNVPTXInfo)
    add_source_files(nautilus
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/CUDAKernelCompiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/CUDARuntimeLib.cpp
    )
endif()

# Metal backend
if (ENABLE_GPU_METAL)
    add_compile_definitions(ENABLE_GPU_METAL)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    target_link_libraries(nautilus PRIVATE ${METAL_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
    add_source_files(nautilus
        ${CMAKE_CURRENT_SOURCE_DIR}/src/metal/MetalKernelCompiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/metal/MetalRuntimeLib.mm
    )
endif()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif()
```

This approach adds GPU sources directly to the `nautilus` target (same pattern as existing backends like MLIR, AsmJit, BC), avoiding circular dependency issues.

---

## 9. Data Flow Summary

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
  |  Math->LLVM, Func->LLVM, CF->LLVM, Arith->LLVM
  v
LLVM IR Optimization + JIT (unchanged)
  |
  v
Executable
  +-- CUDA: host calls cuModuleLoadData(PTX) + cuLaunchKernel
  +-- Metal: host calls MTLDevice.newLibraryWithSource(MSL) + dispatch
```

---

## 10. CPU Fallback (ENABLE_GPU_BACKEND=OFF)

When GPU backend is disabled:
- `gpu.hpp` is still usable -- stub functions compile and link normally
- No GPU pass runs, so proxy calls pass through as normal function calls
- JIT resolves stubs to their CPU implementations: `threadIdx_x()` returns 0, `blockDim_x()` returns 1
- `launch()` calls `set_grid` (no-op), `set_block` (no-op), then `kernel(args...)` directly
- Kernel runs as single-threaded CPU function (thread 0, block 0)

---

## 11. Key Design Decisions

| Decision | Rationale |
|---|---|
| **Top-level `plugins/gpu/` directory** | Keeps GPU code fully separate from core nautilus. Clean boundary for optional feature. |
| **Platform-agnostic user API** | Users write one kernel, it runs on CUDA, Metal, or CPU. No `#ifdef` in user code. |
| **CUDA: LLVM dialect + NVVM intrinsics** | NVVM intrinsics are expressible as `llvm.call` to `llvm.nvvm.*`. No new MLIR dialects needed. |
| **Metal: MSL source code generation** | Apple's AIR backend is not in open-source LLVM. MSL can be compiled at runtime by Metal framework. Similar to existing C++ backend approach. |
| **Unified runtime interface** | Same function signatures (`gpuLoadModule`, `gpuLaunchKernel`, etc.) for both platforms. GPU pass emits the same host code regardless of target. |
| **Post-lowering MLIR pass** | Launch pattern spans multiple operations. Module-level pass is cleaner than stateful per-operation intrinsic handlers. |
| **Stub functions as pointer identity** | Reuses existing `invoke<>` + `ProxyCallOperation` + intrinsic plugin infrastructure. No new IR operation types. |
| **Separate kernel compilation** | GPU code needs different target/language. Split compilation is standard (CUDA, Metal, OpenCL all do this). |
| **CUDA Driver API** (not Runtime API) | Explicit control over module loading, better fit for JIT compilation. |
| **Metal shared memory by default** | `MTLResourceStorageModeShared` avoids explicit copies on unified memory architectures (Apple Silicon). |

---

## 12. Intrinsic Mapping Summary

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

## 13. Files Modified (Existing)

| File | Change |
|---|---|
| `CMakeLists.txt` (root) | Add `ENABLE_GPU_BACKEND` option + `add_subdirectory(plugins/gpu)` |
| `nautilus/include/nautilus/common/config.h.in` | Add `#cmakedefine ENABLE_GPU_BACKEND`, `ENABLE_GPU_CUDA`, `ENABLE_GPU_METAL` |
| `nautilus/src/nautilus/compiler/backends/mlir/MLIRCompilationBackend.cpp` | Add `#ifdef ENABLE_GPU_BACKEND` block (single insertion point between MLIR generation and lowering passes) |

All other files are **new** in `plugins/gpu/` -- no existing code is modified beyond the 3 files above.

---

## 14. Verification Plan

1. **Unit test: CPU fallback** -- kernel with GPU intrinsics, `ENABLE_GPU_BACKEND=OFF`, verify `threadIdx_x()` returns 0 and kernel executes once.
2. **Unit test: IR generation** -- trace `gpu::launch()`, dump IR, verify `ProxyCallOp` pattern.
3. **Unit test: MLIR generation** -- verify `func.func` operations for host and kernel.
4. **Unit test: GPU pass pattern detection** -- verify kernel extraction and host rewrite.
5. **Unit test: CUDA kernel compilation** -- compile simple kernel to PTX, verify valid PTX.
6. **Unit test: Metal kernel compilation** -- compile simple kernel to MSL, verify valid MSL source.
7. **Integration test (CUDA): Vector add** -- end-to-end on NVIDIA GPU.
8. **Integration test (Metal): Vector add** -- end-to-end on Apple GPU.
9. **Build matrix** -- verify all combinations build cleanly:
   - `ENABLE_GPU_BACKEND=OFF` (any platform)
   - `ENABLE_GPU_BACKEND=ON, ENABLE_GPU_CUDA=ON` (Linux)
   - `ENABLE_GPU_BACKEND=ON, ENABLE_GPU_METAL=ON` (macOS)

Test files: `plugins/gpu/test/`.
