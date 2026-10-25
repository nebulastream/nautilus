# Component: GPU Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M7 · Builds on the
0.3 GPU plugin (`plugins/gpu`)

## 1. Purpose

Rebuild the GPU plugin (CUDA and Metal) on NIR as the last port of 0.4, and
make it **production ready**: correct and memory-safe on real hardware,
with GPU errors surfacing as exceptions, without per-launch setup costs,
with the device features real kernels need, and tested on GPUs in CI.

**Status at the release (D21):**
- **Metal: production ready.** It executes on hardware in CI.
- **CUDA: experimental.** It is complete and shares the device code
  generator with Metal, but there is no NVIDIA hardware to run it on; it is
  validated by the grid emulator and by compiling every kernel with NVRTC
  and `ptxas` (§5). `docs/gpu.md` says so. A recorded run on NVIDIA
  hardware (§5.3) lifts the label in a later release.

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
| CI | three `-DENABLE_GPU_PLUGIN=ON` legs; CUDA source generation only, Metal executes on the hosted macOS runner (D15) |

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
| G8 | **No CUDA execution anywhere in CI**; Metal executes on the hosted macOS runner, but only for the 0.3 cases and without validation layers | D15, `plugins/gpu/test` |
| G9 | **No user documentation** (`docs/` has no GPU page) | — |

## 3. From the cut-over to M7

- The cut-over deletes the 0.3 IR, and with it `GPULoweringProviderBase`
  and both lowering providers. `ENABLE_GPU_PLUGIN` is forced off with a
  configure-time message naming this document.
- The three GPU CI legs build without the plugin until M7.
- The user API headers stay in the tree unchanged, so code that includes
  them still compiles while the plugin is off. It does **not link**: the
  device functions (`threadIdx_x()`…, the `nautilus_gpu_*` fallbacks) are
  defined in `plugins/gpu/src/gpu.cpp`, and the `nautilus::nautilus-gpu`
  target does not exist while the plugin is off. The migration notes say
  so.
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
- **Selection:** `gpu.device=cuda|metal|cpu|emulator|auto`. `engine.backend`
  keeps naming the host backend. 0.3's `engine.backend=cuda` and
  `engine.backend=metal` stay as deprecated aliases that set `gpu.device`
  (D17).
  - `auto` (default): CUDA if a CUDA device is present, Metal on macOS with
    a Metal device, **otherwise an error** that names `gpu.device=cpu` and
    `gpu.device=emulator`. It never falls back to the CPU silently. `auto`
    is resolved when a module that contains a `gpu.launch` is compiled,
    because the partition pass (§4.5) produces different code per device;
    the error is raised by that compile. `gpu.device` is code-affecting
    (M1.4a), with the resolved device kind as its value in cache keys.
  - `cpu`: runs the **full grid serially** on the host backend, block by
    block and the threads of a block in order (§4.5). Kernels that use
    `gpu.sync_threads`, `gpu.shared_alloca` or warp operations, directly or
    in a device function they reach, are rejected on `cpu` with a
    compile-time diagnostic, because serial execution cannot honor them;
    they need `emulator` (§5.1).
  - 0.3's single call with index 0 and dimension 1 remains only for direct
    calls of the device functions outside any launch.
- **Runtime loading:** the plugin links neither `libcuda` nor `libnvrtc`.
  The runtime loads them with `dlopen` at the first CUDA device query
  (`libcuda.so.1`; `libnvrtc.so.<major>` of the toolkit the plugin was
  built against, which also loads its `nvrtc-builtins`) and resolves the
  driver and NVRTC entry points it uses with `dlsym`, so the plugin loads
  on machines without a driver (the Linux CI legs, CPU-only deployments); a
  missing library is reported as "no CUDA device". The plugin declares
  that subset of the driver API itself, so building it needs no CUDA
  toolkit; only the CUDA-compile leg (§5) has one.

### 4.2 Device operations

| Op | Type | Traits | CPU fallback |
|---|---|---|---|
| `gpu.thread_idx`, `gpu.block_idx`, `gpu.block_dim`, `gpu.grid_dim` | `i32`; dimension in `flags` | `Pure` within one kernel invocation | the 0.3 functions (0, 0, 1, 1) |
| `gpu.sync_threads` | `void` | effect `ReadWrite` (barrier) | no-op |
| `gpu.shared_alloca` | `ptr`; size and alignment in `extra` | allocates per block | a stack slot |
| `gpu.atomic.{add,sub,min,max,and,or,xor,xchg,cas}` | `i32`, `i64`, `f32` (add only) | effect `ReadWrite`; relaxed ordering on device scope | `std::atomic_ref` |
| `gpu.warp.{shuffle,shuffle_xor,ballot,any,all}` | `i32`, `i64`, `f32`, `bool` | lane-collective | single lane |

- These are ordinary extension operations with native fallbacks, so a
  kernel body compiles on any backend. Inside a launch on `gpu.device=cpu`,
  the index fallbacks read the current thread and block indices from the
  runtime's per-thread launch state; outside a launch they return 0 and 1
  as in 0.3.
- **Math in device code:** the `std` plugin's math functions (`sqrt`, `exp`,
  `log`, `sin`, `cos`, `pow`, `fabs`, `floor`, `ceil`, `fmin`, `fmax`, …)
  are intrinsics registered by name in every build, whatever backends are
  enabled ([04 §3.7](04-extensions.md)); the GPU emitter maps them to device
  built-ins by name, so it does not depend on the MLIR backend.
- **Not in 0.4:** textures, dynamic parallelism, cooperative groups, CUDA
  graphs, half precision, multi-GPU partitioning, HIP/ROCm. A HIP device
  compiler (hiprtc) would fit the same architecture later.

### 4.3 Launch

In the text form of [13 §9](13-nir-spec.md), at the SSA level before
`gpu.partition` (the thunk's symbol name is illustrative):

```
nir 1.0
extension "gpu.launch@1"
callee #c0 = internal @vecAdd : (ptr, ptr, ptr) -> void
callee #c1 = intrinsic gpu.launch : (ptr, ptr, i32 zext, i32 zext, i32 zext, i32 zext, i32 zext, i32 zext, ptr, i32 zext) -> void
pointer #p0 = thunk symbol "nautilus_gpu_launch.thunk"

func @vecAdd : (ptr, ptr, ptr) -> void {
	attr "gpu.kernel" = ""
^b0(%0:ptr, %1:ptr, %2:ptr):
	; kernel body
	ret
}

func export @launchVecAdd : (ptr, ptr, ptr, i32 zext) -> void {
	alloca 24 align 8
^b0(%0:ptr, %1:ptr, %2:ptr, %3:i32):
	%4 = funcaddr #c0
	%5 = ptr.const null
	%6 = alloca 0
	store %6, %0
	%7 = add %6, 8:i64
	store %7, %1
	%8 = add %6, 16:i64
	store %8, %2
	call #c1(%4, %5, %3, 1:i32, 1:i32, 256:i32, 1:i32, 1:i32, %6, 3:i32) unwind caller thunk #p0
	ret
}
```

- **`gpu.launch` is an intrinsic** ([13 §4.6.1](13-nir-spec.md)), not an
  extension operation. A launch may throw (GPU errors become exceptions,
  §4.6), and extension operations never throw
  ([13 §5.11](13-nir-spec.md)). As an intrinsic it has call semantics: the
  tracer records it with the cleanup list of its trace position, so it
  carries an unwind entry, and backends without unwind tables call it
  through the capture thunk, like any may-throw call. Destructors of live
  `val` objects therefore run when a launch fails.
- **Signature** (fixed): `(ptr kernel, ptr hostEntry, i32 zext gx,
  i32 zext gy, i32 zext gz, i32 zext bx, i32 zext by, i32 zext bz,
  ptr argv, i32 zext argc) -> void`. The dimensions and `argc` are
  `uint32_t` in `gpu.hpp`, so they carry `zext` ([13 §4.5.1](13-nir-spec.md));
  the tracer derives the same type words, and a callee whose signature
  differs is rejected. Attributes: none (0: may throw, may read and write
  any memory, [13 §4.6.2](13-nir-spec.md)).
- **Operands as traced:** `kernel` is a `funcaddr` of the internal kernel
  function, `hostEntry` is `ptr.const null`, and `argv` is a stack
  argument block in which argument *i* occupies the 8-byte slot at offset
  8·*i* (a narrow value in its low bytes, as stored by `store`).
- **Operands after `gpu.partition`** (§4.5): the kernel is no longer a
  host function, so `kernel` becomes `data.addr` of a per-kernel
  **descriptor** data object (the device module's structural hash and the
  kernel name; read-only and `Literal`, [13 §4.10](13-nir-spec.md)), which
  stays portable in images. On `cpu` and `emulator`, `hostEntry` becomes
  `funcaddr` of the kernel's host entry `<kernel>$argv`; on CUDA and Metal
  it stays null. The runtime finds the loaded device module by the hash
  (registered by the compiled module that owns it, released with it), so
  two modules with equal descriptors sharing one data address is
  harmless.
- **Native fallback = the runtime entry `nautilus_gpu_launch`** itself, so
  no backend needs a lowering. A non-null `hostEntry` is called once per
  thread (`cpu` serially, `emulator` as in §5.1); otherwise the
  descriptor's kernel is launched on the device the module was compiled
  for.
- **Checks:** intrinsics have no `verify` hook, so the `gpu.partition`
  pass (§4.5) checks that the kernel operand is a `funcaddr` of an
  internal `void` function with the `gpu.kernel` attribute, that
  `hostEntry` is `ptr.const null`, that `argc` equals its parameter count
  and that the stored argument types match its signature.
- **Synchronous by default,** as in 0.3: a launch returns when the kernel
  has finished. `gpu::launchAsync(...)` enqueues on the calling thread's
  stream or queue, and `gpu::synchronize()` waits for it; kernels from one
  thread run in order.
- `gpu::launch` traces a call of the `gpu.launch@1` intrinsic (as a
  traced `invoke` of `nautilus_gpu_launch`); its C++ signature does not
  change. `setGrid` and `setBlock` stay as no-op wrappers; the
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
  code, with the deprecated `gpu.metal.bufferSize` option, with a warning once.
  The copy is never larger than the declared size, which removes G1's
  overruns.
- **Lifetimes:** buffers belong to the application; kernels, pipeline states
  and loaded modules belong to the compiled module and are released with it
  ([13 §4.4.1](13-nir-spec.md)).

### 4.5 Kernels, device functions and the partition pass

- **Kernels are the targets of `gpu.launch`.** `NautilusKernelFunction`
  keeps its 0.3 `kernel` attribute as the NIR function attribute
  `gpu.kernel`, a function string attribute
  ([13 §4.5.2](13-nir-spec.md), section 25); a launch of a function without
  it is a diagnostic.
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
  3. moves device functions into a device module and replaces each
     launch's `kernel` operand with `data.addr` of the kernel's descriptor
     (§4.3);
  4. for `gpu.device=cpu` and `emulator`, keeps the device functions in the
     host module as well and emits, per kernel, an internal entry
     `<kernel>$argv(ptr argv)` that loads the arguments from the block and
     calls the kernel; the launch's `hostEntry` operand becomes its
     `funcaddr`, and the runtime calls it once per thread with the
     thread's indices in its launch state. On `cpu` it rejects kernels
     that use barriers, shared memory or warp operations, directly or in a
     device function they reach (§4.1).

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
- **Targets for Metal**, measured on an Apple-silicon Mac (the hosted
  runner's timings are virtualized and only reported):
  - generated kernels for `vecAdd`, `saxpy`, `prefixSum`, a reduction with
    threadgroup memory and SIMD-group shuffles, and a TPC-H-style filter and
    aggregate reach at least 90% of the throughput of hand-written MSL
    versions;
  - launch overhead within 10% of a direct Metal dispatch of the same
    pipeline;
  - device compile time recorded per kernel; a warm L0 or artifact hit
    compiles nothing.
- **For CUDA** (experimental), the CI compile leg records registers, spills
  and shared memory per kernel from `ptxas` and fails on regressions; there
  are no throughput targets until it runs on hardware.

### 4.9 Observability

- Statistics: `gpu.compileMs`, `gpu.kernels`, `gpu.launches`,
  `gpu.launchMs`, and `gpu.kernelMs` (device timing, opt-in);
  `explain()` lists kernels, their device and their resource use
  (registers, shared memory) where the toolchain reports it.
- Dumps: `dump.after_gpu_partition`, `dump.after_gpu_generation` (device
  source).

## 5. Validation without GPU runners

There is no GPU CI runner and no NVIDIA hardware. **Decided (D21):**
validation uses what the shared runners provide plus a CPU grid emulator;
Metal is production ready, CUDA ships as experimental:

| Layer | Where | What it covers |
|---|---|---|
| **Metal on hardware** | the GitHub-hosted macOS (Apple silicon) runner, every PR | Metal kernels really execute there: 0.3's `GPU Metal Execution Test` computes and checks results on that runner today. The whole Metal suite runs with Metal API validation and shader validation enabled. Timings there are virtualized and only reported |
| **Grid emulator** | Linux runners, every PR | `gpu.device=emulator` runs kernels on the CPU with full grid semantics (§5.1); covers partition, launch, indexing, barriers, shared memory, atomics, warp ops and D19 for both CUDA- and Metal-style code |
| **CUDA compile** | Linux runner with the CUDA toolkit (no driver, no GPU) | NVRTC compiles every test kernel to PTX for each supported architecture; `ptxas` assembles it and reports registers, spills and shared memory; failures and resource regressions fail the leg |
| **Shared emitter** | every leg | CUDA and MSL come from one NIR emitter (§4.5); the Metal execution covers the emitter's logic for both, the CUDA leg covers what differs in syntax |
| **Metal performance** | an Apple-silicon Mac, before PR 18 and the release candidate merge; the machine and its owner are an external prerequisite ([16 §16](16-implementation-plan.md)) | `tools/gpu-validate/` runs the §4.8 benchmarks on bare metal and writes a report with the machine, OS and toolchain; reports are kept in `plugins/gpu/validation/` |

**What is not covered:** CUDA kernels never execute on NVIDIA hardware, so
CUDA-specific runtime behavior (driver errors, real memory faults, timing
and scheduling that the emulator does not reproduce) and CUDA performance
are unverified. That is why CUDA is experimental (§1).

### 5.1 The grid emulator

- A device kind of the GPU runtime (§4.1) that runs the device module on a
  CPU backend (`tbc` or `asmjit`): every GPU thread is a fiber, a block is a
  group of fibers scheduled cooperatively, warps are groups of 32 lanes.
- `gpu.sync_threads` suspends a fiber until every fiber of its block has
  arrived; a barrier that not all threads reach is a divergent barrier,
  which CUDA and Metal leave undefined. The barrier op itself never throws
  (extension ops cannot); the runtime records the divergence and the
  launch raises `GPUException` when the block ends.
- **Fibers and the host backends.** Fibers switch only inside the barrier
  fallback. On `asmjit` and MLIR each fiber runs on its own machine stack.
  `tbc` keeps its frames on one per-thread VM stack (`thread_local
  VMContext`, `TBCVm.cpp`), which interleaved fibers would corrupt, so the
  emulator gives each fiber its own VM stack and swaps the context on
  every switch. An exception thrown in a thread aborts the launch and is
  rethrown on the launching thread; it never crosses a fiber switch.
- `gpu.shared_alloca` gets one buffer per block; atomics use
  `std::atomic_ref`; warp ops exchange values between the lanes of a warp.
- Memory: `DeviceBuffer` and `ManagedBuffer` are host allocations with the
  same API, so the memory, error and concurrency tests run unchanged; out of
  bounds accesses within buffers are checked in debug builds.
- It is a test device, not a performance path; `gpu.device=auto` never
  selects it.

### 5.2 Tests

| Test | Content |
|---|---|
| `GPUTracingTest` | traces as pre-SSA NIR; golden dumps regenerated in their own commit |
| `GPUCodegenTest` | generated device sources compared byte for byte (`plugins/gpu/test/data/` stays excluded from `format.sh`) |
| `GPUExecutionTest` | the 0.3 cases plus every op of §4.2: on Metal and the emulator in CI (CUDA: compile only) |
| CPU device | every GPU test kernel without barriers, shared memory or warp ops on `gpu.device=cpu` with `tbc`, `asmjit` and MLIR as host backends: the full grid runs and the results equal the emulator's (not 0.3's single-thread results, which `GPUExecutionTest` documents as "writes c[0] only"); kernels with barriers are rejected with the diagnostic; direct calls of the device functions outside a launch keep the 0.3 results |
| Selection | compiling a module with a launch under `gpu.device=auto` without a device fails with the error naming `cpu` and `emulator`; the plugin loads without `libcuda`; changing `gpu.device` changes the cache key |
| Memory | `DeviceBuffer`/`ManagedBuffer` round trips; host pointers rejected; the 0.3 Metal compatibility path never copies past the declared size |
| Errors | invalid launch configuration, illegal address, out of memory, compile error: each raises its exception with the kernel name (illegal address and out of memory on Metal) |
| Concurrency | launches from several threads; `launchAsync` ordering per thread; module release while launches are queued |
| Device rules and D19 | one negative test per rule; the division cases on Metal, on the emulator and in the fallback |
| Fuzzer | generated kernels compared between Metal, the emulator, the CPU fallback and the C++ oracle in CI; every generated kernel also compiled for CUDA |
| Benchmarks | §4.8 kernels against hand-written MSL on an Apple-silicon Mac |
| API compatibility | the 0.3 GPU tests and `example/src/DemoGPU.cpp` join `test/api-compat/`; on hosts without a device the leg sets `gpu.device=cpu` through `NAUTILUS_OPTIONS`, not in the test sources, because `auto` fails without a device (§4.1). **Exception:** the two 0.3 "CPU fallback" sections of `GPUExecutionTest` (`gpuLaunchVecAdd`, `gpuLaunchVecAddDynamic`) launch 256 threads over 3-element arrays and rely on the single call; with the full grid they would write out of bounds. They are rewritten with arrays sized to the grid instead of joining `test/api-compat/`, and the migration guide lists the change |

### 5.3 Lifting CUDA out of experimental

Whenever NVIDIA hardware becomes available (a maintainer's machine, cloud
credits, or a GPU runner), `tools/gpu-validate/` runs the GPU suite,
`compute-sanitizer` (memcheck, racecheck, synccheck), the fuzzer mode and
CUDA versions of the §4.8 benchmarks, and records a report. A clean report,
with the §4.8 targets met against hand-written CUDA, removes the label;
keeping it removed needs such a run before each release.

## 6. Exit criteria (M7)

- G1–G9 closed.
- `ENABLE_GPU_PLUGIN` builds again; the three GPU CI legs are restored, plus
  the emulator and CUDA-compile legs.
- Every test of §5.2 passes on Metal (with validation layers) and on the
  emulator in CI; every test kernel compiles for CUDA through NVRTC and
  `ptxas`.
- The Metal performance targets of §4.8 are met on an Apple-silicon Mac.
- The 0.3 GPU user API compiles and runs unchanged, except that a launch
  on a host without a device needs `gpu.device=cpu` and then runs the full
  grid (§4.1); `docs/gpu.md` documents
  the API, the memory model, errors and limits, marks CUDA as experimental
  and says how it is validated.
- D19 and D20 decided.

## 7. Risks

| Risk | Mitigation |
|---|---|
| GPU support is broken for most of the release | declared; the headers stay in the tree; the 0.3 tests and reference sources are ported, not rewritten |
| NVRTC or the driver API is unavailable in a deployment | both are loaded with `dlopen` (§4.1), so the plugin still loads; a launch fails with a clear error unless `gpu.device=cpu` or `emulator` is set. There is no silent fallback |
| 0.3 programs rely on the single-call CPU fallback of a launch | the full-grid `cpu` device is a behavior change, listed in the migration guide; `auto` fails loudly instead of running the old fallback, so no program changes results silently |
| The memory model breaks 0.3 Metal programs | the compatibility path of §4.4 accepts host pointers with a declared size; the migration guide shows the move to `DeviceBuffer` |
| Address-space inference rejects valid kernels | the rule is narrow (one space per value); diagnostics name the value; `.nir` and fuzzer coverage |
| No GPU runner and no NVIDIA hardware | D21: Metal executes on the hosted macOS runner and is production ready; the emulator and the PTX/`ptxas` leg cover CUDA, which ships as experimental (§5.3 lifts it) |
| The emulator hides scheduling bugs that real GPUs expose | it reports divergent barriers; Metal shader validation runs in CI on real hardware; for CUDA this risk is part of why it is experimental |
| CUDA users depend on it despite the label | `docs/gpu.md` and the release notes state the status; `gpu.device=auto` still selects CUDA when a device is present, and logs once that the backend is experimental |
| Driver and toolkit drift | toolkit and driver versions in the cache key and the artifact descriptor; a mismatch recompiles |
