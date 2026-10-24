# Component: GPU Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M7 · Builds on the
0.3 GPU plugin (`plugins/gpu`)

## 1. Purpose

Rebuild the GPU plugin (CUDA and Metal) on NIR as the last port of 0.4, and
make it **production ready**: correct and memory-safe on real hardware,
with GPU errors surfacing as exceptions, without per-launch setup costs,
with the device features real kernels need, and tested on GPUs in CI.

GPU support is **switched off from the cut-over until M7**
([16 §2](16-implementation-plan.md)), so the rest of 0.4 never waits for it.
At the release, the GPU user API is source-compatible with 0.3 and extended.

## 2. Today, and why it is not production ready

The plugin (`plugins/gpu`, about 3,500 lines including tests) generates
CUDA and Metal source from the 0.3 IR:

| Part | How it works in 0.3 |
|---|---|
| User API (`nautilus/gpu/gpu.hpp`) | `NautilusKernelFunction` (a `NautilusFunction` with the attribute `kernel`); `threadIdx_x()`…`gridDim_z()`; `syncThreads()`; `GridDim`/`BlockDim`; `launch(kernel, grid, block, args...)` |
| Selection | `engine.backend=cuda` or `metal`: the whole module, host code included, is compiled by the GPU backend |
| Device intrinsics | `extern "C"` functions (`intrinsic_targets.hpp`) with CPU fallbacks (index 0, dimension 1, barrier no-op), recognized **by function pointer** |
| Launch | `setGrid(...)`, `setBlock(...)`, then a call of the kernel; without a launch configuration, `gpu.gridDimX`… options are used |
| CUDA | one generated `.cu` file with kernels and host code, compiled by an `nvcc` subprocess into a shared library in a temporary directory |
| Metal | generated MSL plus generated host C++ that dispatches through the Metal API, compiled with `xcrun` tools and the system compiler |
| Tests | `GPUTracingTest`, `GPUCodegenTest` (generated sources compared byte for byte), `GPUExecutionTest` (needs a device) |
| CI | three `-DENABLE_GPU_PLUGIN=ON` legs; CUDA compile-only, Metal executes where the runner exposes a device (D15) |

Gaps against production use, from the code:

| # | Gap | Where |
|---|---|---|
| G1 | **Metal copies every pointer argument into a new buffer of a fixed size** (`NAUTILUS_BUFFER_SIZE`, 4096 bytes by default) and copies the same size back: larger arrays are truncated, smaller ones are read and written past their end | `MetalLoweringProvider`, launch emission |
| G2 | **Metal creates the device, library, pipeline state and command queue on every launch** | same |
| G3 | **CUDA has no memory model:** kernels receive whatever pointers the host passes; only managed or device memory works, and nothing checks it (the demo uses `cudaMallocManaged`) | `CUDALoweringProvider`, `example/src/DemoGPU.cpp` |
| G4 | **No error handling:** no `cudaGetLastError` after launches, no Metal command-buffer status check; failures are silent or crash | both |
| G5 | **Every launch synchronizes** (`cudaDeviceSynchronize`, `waitUntilCompleted`); there are no streams or queues | both |
| G6 | **Host code is generated and compiled by the GPU toolchain** (`nvcc`, `xcrun`): seconds per module, temporary files, no tiering, caching or debug info for host code, and the CPU backends' exception handling does not apply | compilers |
| G7 | **No device-side features beyond indices and a barrier:** no shared memory, atomics, warp operations or math functions | `gpu.hpp` |
| G8 | **No GPU execution in CI** for CUDA; Metal only where a device is exposed | D15 |
| G9 | **No user documentation** (`docs/` has no GPU page) | — |

## 3. From the cut-over to M7

- The cut-over deletes the 0.3 IR, and with it `GPULoweringProviderBase`
  and both lowering providers. `ENABLE_GPU_PLUGIN` is forced off with a
  configure-time message naming this document.
- The three GPU CI legs build without the plugin until M7.
- The user API headers stay in the tree unchanged, so code that includes
  them still compiles while the plugin is off.
- No other WP depends on the GPU plugin. The pieces it uses (extension ops,
  required plugin passes, legalization, data objects, D10's division
  semantics) are built in M1–M6.

## 4. Design

### 4.1 Architecture: device compilers and a host runtime

0.3 compiles the host side with the GPU toolchain (G6). 0.4 splits a GPU
module instead:

```
traced module (NIR) ── gpu.partition ──┬─► host functions ─► the normal backend (asmjit / tbc / MLIR)
                                       │        gpu.launch ─► call nautilus_gpu_launch(kernel, dims, args)
                                       └─► device functions ─► device compiler (CUDA: NVRTC → PTX → driver;
                                                                Metal: MSL → MTLLibrary at run time)
```

- **Host code** is compiled like any other module by the selected host
  backend, with tiering, caches, debug info and the normal exception path.
- **Device code** is emitted as CUDA C or MSL by one shared emitter and
  compiled in process: NVRTC to PTX, loaded through the CUDA driver API for
  the current device; MSL through `newLibraryWithSource`. No subprocess and
  no temporary files.
- **The GPU runtime** (`plugins/gpu/src/runtime/`) owns devices, contexts,
  streams or command queues, loaded kernels and pipeline states. Host code
  calls it through a small C ABI.
- **Selection:** `gpu.device=cuda|metal|cpu|auto` (default `auto`: CUDA if
  a CUDA device is present, Metal on macOS, otherwise `cpu`, which runs
  kernels through their fallbacks). `engine.backend` keeps naming the host
  backend. 0.3's `engine.backend=cuda` and `engine.backend=metal` stay as
  deprecated aliases that set `gpu.device` (D17).

### 4.2 Device operations

| Op | Type | Traits | CPU fallback |
|---|---|---|---|
| `gpu.thread_idx`, `gpu.block_idx`, `gpu.block_dim`, `gpu.grid_dim` | `i32`; dimension in `flags` | `Pure` within one kernel invocation | the 0.3 functions (0, 0, 1, 1) |
| `gpu.sync_threads` | `void` | effect `ReadWrite` (barrier) | no-op |
| `gpu.shared_alloca` | `ptr`; size and alignment in `extra` | allocates per block | a stack slot |
| `gpu.atomic.{add,sub,min,max,and,or,xor,xchg,cas}` | `i32`, `i64`, `f32` (add only) | effect `ReadWrite`; relaxed ordering on device scope | `std::atomic_ref` |
| `gpu.warp.{shuffle,shuffle_xor,ballot,any,all}` | `i32`, `i64`, `f32`, `bool` | lane-collective | single lane |

- These are ordinary extension operations with native fallbacks, so a
  kernel still runs on any backend, single-threaded, as in 0.3.
- **Math in device code:** the `std` plugin's math functions (`sqrt`, `exp`,
  `log`, `sin`, `cos`, `pow`, `fabs`, `floor`, `ceil`, `fmin`, `fmax`, …)
  are intrinsics; the GPU emitter maps them to device built-ins.
- **Not in 0.4:** textures, dynamic parallelism, cooperative groups, CUDA
  graphs, half precision, multi-GPU partitioning, HIP/ROCm. A HIP device
  compiler (hiprtc) would fit the same architecture later.

### 4.3 Launch

```
%k = funcaddr @vecAdd
gpu.launch %k, grid(%gx, 1:i32, 1:i32), block(256:i32, 1:i32, 1:i32), %a, %b, %c
```

- **Operands:** the kernel as a `funcaddr` of an internal function, six
  launch dimensions, the kernel's arguments (in `extra`). The `verify` hook
  checks that the kernel operand names an internal `void` function whose
  signature matches.
- **Host lowering** (by legalization when a device is selected): a call of
  `nautilus_gpu_launch(handle, dims, argv, argc)` that **may unwind**, so GPU
  errors become exceptions (§4.6). Extension operations never throw
  ([13 §5.11](13-nir-spec.md)); the runtime call does.
- **CPU fallback:** an expansion into a direct call of the kernel.
- **Synchronous by default,** as in 0.3: a launch returns when the kernel
  has finished. `gpu::launchAsync(...)` enqueues on the calling thread's
  stream or queue, and `gpu::synchronize()` waits for it; kernels from one
  thread run in order.
- `gpu::launch` traces `gpu.launch` through `traceExtOp`; its signature does
  not change. `setGrid` and `setBlock` stay as no-op wrappers; the
  `gpu.gridDimX`… options are deprecated aliases for a launch without
  configuration.
- **Launch validation** before every launch: block size and shared memory
  against the device limits, grid dimensions non-zero.

### 4.4 Memory

0.3 has no memory model (G1, G3). **Proposed (D20):**

- **`gpu::DeviceBuffer<T>`** (new in `gpu.hpp`): an RAII allocation on the
  selected device with `size()`, `copyFrom(span)`, `copyTo(span)` and
  `get()`, which yields the pointer passed to kernels. CUDA allocates with
  `cuMemAlloc`; Metal with a shared-storage `MTLBuffer` (zero copy on Apple
  silicon). `gpu::ManagedBuffer<T>` allocates unified memory, readable on
  host and device.
- **Kernel pointer arguments must point into device-accessible memory:**
  `DeviceBuffer`, `ManagedBuffer`, or memory the application allocated
  through CUDA or Metal itself. The runtime resolves each pointer argument
  to its allocation (CUDA: `cuPointerGetAttributes`; Metal: the runtime's
  own buffer table) and rejects anything else with an exception naming the
  kernel and argument.
- **Compatibility with 0.3's Metal copy:** a host pointer passed to a Metal
  kernel is still accepted for 0.3 programs, but copied with the size the
  application declares (`gpu::hostBuffer(ptr, bytes)`) or, for unchanged 0.3
  code, with the deprecated `gpu.bufferSize` option, with a warning once.
  The copy is never larger than the declared size, which removes G1's
  overruns.
- **Lifetimes:** buffers belong to the application; kernels, pipeline states
  and loaded modules belong to the compiled module and are released with it
  ([13 §4.4.1](13-nir-spec.md)).

### 4.5 Kernels, device functions and the partition pass

- **Kernels are the targets of `gpu.launch`.** `NautilusKernelFunction`
  keeps its `kernel` attribute; a launch of a function without it is a
  diagnostic.
- **Device functions** are the kernels plus every internal function they
  reach. Kernel names are kept, so Nsight and Xcode show them.
- **Device code rules** (diagnostics `NAUT-G001`…, with source locations):
  - no external callees except math intrinsics with a device mapping; no
    `call.indirect`, `bind.addr` or `funcaddr`;
  - every call is `NO_UNWIND`;
  - data objects ([13 §4.10](13-nir-spec.md)) become `__constant__` arrays
    (CUDA) or `constant` buffers (Metal);
  - pointers have one address space each (device, shared, constant),
    inferred from their origin; a value that mixes address spaces is
    rejected, because MSL needs the qualifier;
  - extension operations other than the `gpu` ops need a GPU handler.
- **The `gpu.partition` pass** (required, `beforeLowering`,
  [03 §5](03-conversion-and-passes.md)):
  1. finds the kernels and device functions and checks the rules;
  2. rewrites checked integer division (D10) into the device form of D19
     (§4.7) and keeps saturating float→int casts (D11) as explicit clamps;
  3. moves device functions into a device module and replaces each launch
     target with a kernel handle that the runtime resolves.

### 4.6 Errors

- **Compile errors** from NVRTC or Metal are raised as
  `nautilus::gpu::GPUCompileError` with the device source, the compiler
  log, and the NIR value and source location of the line that failed.
- **Launch and execution errors** (invalid configuration, illegal address,
  out of memory, a failed command buffer) are raised from the launch call as
  `nautilus::gpu::GPUException`, naming the kernel; with `launchAsync`, at
  the next `synchronize()` or launch on the same stream.
- **Device loss** fails every later launch of the module with the same
  exception; the engine stays usable.

### 4.7 Semantics on the device

- **Division (D19, proposed, decided at the M7 exit).** D10 makes integer
  division by zero and `MIN / -1` throw, which device code cannot do. Inside
  device functions they give defined results: `x / 0 = 0`, `x % 0 = x`,
  `MIN / -1 = MIN`, `MIN % -1 = 0`. The partition pass emits them as `select`
  over unchecked operations, so the CPU fallback of a kernel computes the
  same.
- Shifts are masked and float→int casts saturate (D10, D11). Floats are
  IEEE strict: CUDA is compiled without `--use_fast_math`, Metal with
  `fastMathEnabled = NO`.
- Atomics are relaxed; `gpu.sync_threads` orders shared and device memory
  within a block, as CUDA's `__syncthreads` and Metal's `threadgroup_barrier`
  with device and threadgroup scope.

### 4.8 Performance

- **Per launch:** one driver launch call (`cuLaunchKernel` or one encoder
  dispatch) plus argument packing; no allocation, no library or pipeline
  creation (G2), no copies for device buffers.
- **Per module:** device code is compiled once per module and device and
  cached:
  - L0: loaded kernels and pipeline states by the device module's
    structural hash;
  - artifacts and L1/L2 ([07](07-serialization.md), [08](08-caching.md)):
    PTX and the Metal library as GPU payloads, keyed by toolkit and driver
    version; a mismatch recompiles from the NIR image.
- **Targets**, on the reference GPUs (§5):
  - generated kernels for `vecAdd`, `saxpy`, `prefixSum`, a reduction with
    shared memory and warp shuffles, and a TPC-H-style filter and aggregate
    reach at least 90% of the throughput of hand-written CUDA and MSL
    versions compiled with default optimization;
  - launch overhead within 10% of a direct driver launch of the same
    kernel;
  - device compile time recorded per kernel; a warm L0 or artifact hit
    compiles nothing.

### 4.9 Observability

- Statistics: `gpu.compileMs`, `gpu.kernels`, `gpu.launches`,
  `gpu.launchMs`, and `gpu.kernelMs` (device timing, opt-in);
  `explain()` lists kernels, their device and their resource use
  (registers, shared memory) where the toolchain reports it.
- Dumps: `dump.after_gpu_partition`, `dump.after_gpu_generation` (device
  source).

## 5. Validation

Production readiness needs GPU hardware in CI. **Proposed (D21):** D15 is
extended:

- a self-hosted NVIDIA runner (one current data-center or workstation GPU)
  runs the GPU suite on every PR that touches `plugins/gpu`, NIR or the
  conversion, and nightly on `nautilus-0.4`; it needs an owner;
- Metal executes on Apple-silicon macOS runners where a device is exposed,
  otherwise on a self-hosted Mac;
- `compute-sanitizer` (memcheck, racecheck, synccheck) runs nightly on the
  NVIDIA runner; Metal API and shader validation are on in every Metal test;
- the reference GPUs for §4.8 are recorded in the corpus output.

| Test | Content |
|---|---|
| `GPUTracingTest` | traces as pre-SSA NIR; golden dumps regenerated in their own commit |
| `GPUCodegenTest` | generated device sources compared byte for byte (`plugins/gpu/test/data/` stays excluded from `format.sh`) |
| `GPUExecutionTest` | the 0.3 cases plus every op of §4.2, on hardware |
| CPU fallback | every GPU test function on `tbc`, `asmjit` and MLIR through the fallbacks, with the 0.3 results |
| Memory | `DeviceBuffer`/`ManagedBuffer` round trips; host pointers rejected; the 0.3 Metal compatibility path never copies past the declared size |
| Errors | invalid launch configuration, illegal address, out of memory, compile error: each raises its exception with the kernel name |
| Concurrency | launches from several threads; `launchAsync` ordering per thread; module release while launches are queued |
| Device rules and D19 | one negative test per rule; the division cases on the device and in the fallback |
| Fuzzer | generated kernels compared between device, CPU fallback and the C++ oracle on the NVIDIA runner |
| Benchmarks | §4.8 kernels against hand-written versions on the reference GPUs |
| API compatibility | the 0.3 GPU tests and `example/src/DemoGPU.cpp` join `test/api-compat/` |

## 6. Exit criteria (M7)

- G1–G9 closed.
- `ENABLE_GPU_PLUGIN` builds again; the three GPU CI legs are restored; the
  GPU runners of D21 run the suite.
- Every test of §5 passes on hardware; `compute-sanitizer` is clean.
- The performance targets of §4.8 are met and recorded.
- The 0.3 GPU user API compiles and runs unchanged; `docs/gpu.md` documents
  the API, the memory model, errors and limits.
- D19, D20 and D21 decided.

## 7. Risks

| Risk | Mitigation |
|---|---|
| GPU support is broken for most of the release | declared; the headers stay in the tree; the 0.3 tests and reference sources are ported, not rewritten |
| NVRTC or the driver API is unavailable in a deployment | the plugin falls back to `gpu.device=cpu` with a warning, and fails with a clear error if a CUDA device was requested explicitly |
| The memory model breaks 0.3 Metal programs | the compatibility path of §4.4 accepts host pointers with a declared size; the migration guide shows the move to `DeviceBuffer` |
| Address-space inference rejects valid kernels | the rule is narrow (one space per value); diagnostics name the value; `.nir` and fuzzer coverage |
| No GPU hardware in CI | D21; until the runners exist, the GPU part of M7 cannot exit |
| Driver and toolkit drift | toolkit and driver versions in the cache key and the artifact descriptor; a mismatch recompiles |
