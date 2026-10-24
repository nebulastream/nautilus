# Component: GPU Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M7 (before the
release work) · Builds on the 0.3 GPU plugin (`plugins/gpu`)

## 1. Purpose

Bring the GPU plugin (CUDA and Metal) onto NIR as the last backend port of
0.4. GPU support is **switched off from the cut-over until this milestone**
([16 §2](16-implementation-plan.md)): it is the only backend that is allowed
to stay broken for that long, so the rest of 0.4 never waits for it. At the
release, the GPU plugin's user API is source-compatible with 0.3 again.

## 2. Today

The plugin (`plugins/gpu`, about 3,500 lines including tests) is a set of
source-generating backends on the 0.3 IR:

| Part | How it works in 0.3 |
|---|---|
| User API (`nautilus/gpu/gpu.hpp`) | `NautilusKernelFunction` (a `NautilusFunction` with the attribute `kernel`); `threadIdx_x()`…`gridDim_z()`; `syncThreads()`; `GridDim`/`BlockDim`; `launch(kernel, grid, block, args...)` |
| Device intrinsics | `extern "C"` functions in `intrinsic_targets.hpp` (`nautilus_gpu_thread_idx_x`, …) with CPU fallbacks: thread and block index 0, block and grid dimension 1, barrier a no-op. Backends recognize calls to them **by function pointer** |
| Launch | `launch` traces `setGrid(...)`, `setBlock(...)`, then a call of the kernel; the backend pairs the call with the stored launch configuration |
| Lowering | `GPULoweringProviderBase` over `IRGraph`, with `Frame` hash maps and string streams; kernels are functions with the `kernel` attribute; helpers reachable from kernels become device functions |
| CUDA | one `.cu` file (kernels `__global__`, host code), compiled with `nvcc` into a shared library |
| Metal | device code in MSL plus host C++ that dispatches through the Metal API, compiled with `xcrun` tools and the system compiler |
| Entry points | `lowerToCUDA(IRGraph, options)` and `lowerToMetal(IRGraph, options)` in `nautilus/gpu/backends.hpp`; backends registered in `CompilationBackendRegistry` |
| Tests | `GPUTracingTest` (trace dumps), `GPUCodegenTest` (generated host and device sources compared byte for byte with `test/data/gpu-tests/`), `GPUExecutionTest` (needs a device) |
| CI | three `-DENABLE_GPU_PLUGIN=ON` legs (Linux x86-64, Linux ARM, macOS); CUDA compile-only, Metal executes where the runner exposes a device (D15) |

There is no shared memory and no address-space support in 0.3, so none is
needed for compatibility.

## 3. From the cut-over to M7

- The cut-over deletes the 0.3 IR, and with it `GPULoweringProviderBase`
  and both lowering providers. `ENABLE_GPU_PLUGIN` is forced off with a
  configure-time message naming this document.
- The three GPU CI legs build without the plugin until M7.
- The user API headers stay in the tree unchanged, so nothing that
  includes them breaks at compile time while the plugin is off.
- No other WP depends on the GPU plugin. Required plugin passes
  (`afterConversion`, `beforeLowering`, [03 §5](03-conversion-and-passes.md))
  are built in M3 and used here.

## 4. Design on NIR

### 4.1 Device operations

The device intrinsics become extension operations of the `gpu` plugin
([04 §3.3](04-extensions.md)):

| Op | Type | Traits | Fallback |
|---|---|---|---|
| `gpu.thread_idx`, `gpu.block_idx`, `gpu.block_dim`, `gpu.grid_dim` | `i32`; dimension x/y/z as a payload in `flags` | `Pure` (constant within one kernel invocation), so they fold, merge and hoist | the 0.3 `extern "C"` functions, keeping the CPU semantics (0, 0, 1, 1) |
| `gpu.sync_threads` | `void` | effect `ReadWrite`: a barrier, never merged, moved or removed | the 0.3 no-op function |

They keep their CPU fallbacks, so they are ordinary extension operations
rather than target-restricted ones: a kernel still runs on any backend as a
single thread, as in 0.3. Backends recognize them by opcode, not by function
pointer.

### 4.2 Launch

`gpu.launch` is an extension operation that replaces the `setGrid` /
`setBlock` / call pattern:

```
%k = funcaddr @vecAdd
gpu.launch %k, grid(%gx, 1:i32, 1:i32), block(256:i32, 1:i32, 1:i32), %a, %b, %c
```

- Operands: the kernel as a `funcaddr` of an internal function, six launch
  dimensions, then the kernel's arguments (in `extra`). Its `verify` hook
  checks that the kernel operand is a `funcaddr` of an internal function
  whose signature matches the arguments and returns `void`.
- Fallback: an expansion into a direct `call` of the kernel, which runs it
  once on the CPU as in 0.3.
- `gpu::launch` in `gpu.hpp` traces `gpu.launch` through `traceExtOp`
  ([01 §6](01-tracing-frontend.md)). Its signature does not change.
- `setGrid` and `setBlock` remain in the header as no-op wrappers for
  source compatibility and trace nothing.

### 4.3 Kernels and device functions

- **Kernels are the targets of `gpu.launch`.** `NautilusKernelFunction`
  keeps setting its `kernel` attribute; the partition pass (§4.4) checks
  that every launch target carries it and reports a diagnostic naming the
  function otherwise.
- **Device functions** are the kernels plus every internal function they
  reach through calls.
- **Device code rules,** checked by the GPU verifier extension:
  - no calls to external callees and no `call.indirect`, `bind.addr` or
    `funcaddr` (other than as the operand of `gpu.launch` in host code);
  - every call is `NO_UNWIND`; exceptions do not exist on the device;
  - data objects ([13 §4.10](13-nir-spec.md)) are allowed and are emitted as
    `__constant__` arrays (CUDA) or `constant` buffers (Metal);
  - extension operations other than the `gpu` ops need a GPU handler.

### 4.4 The partition pass

A required `beforeLowering` plugin pass, `gpu.partition`
([03 §5](03-conversion-and-passes.md)), runs when a GPU backend is selected:

1. finds the launch targets and computes the device functions;
2. checks the device code rules and rewrites what has a device form:
   - integer `div`/`rem` with the checked bit (D10) become unchecked
     operations guarded by `select` (§4.6);
   - float→int casts keep their saturating semantics (D11) and are emitted
     with an explicit clamp;
3. records the partition (device functions, kernels, host functions) in a
   derived side table that the backend reads.

### 4.5 The backends

- **One shared emitter** over NIR replaces `GPULoweringProviderBase`:
  - array side tables indexed by value id, no `Frame` hash maps;
  - C-like statements from `insts` in layout order, with block labels;
  - signedness from op flags: signed or unsigned C types are chosen per
    operation, not per value ([06 §4](06-semantics.md)).
- **CUDA** emits one `.cu` file: device functions as `__device__`, kernels
  as `__global__`, host code with `<<<grid, block>>>` launches; compiled with
  `nvcc` as in 0.3.
- **Metal** emits MSL for device functions and C++ host code that dispatches
  through the Metal API, as in 0.3.
- **Entry points:** `lowerToCUDA` and `lowerToMetal` take a legalized NIR
  module instead of an `IRGraph`. They are plugin surface, not user-facing
  API, so the signature change needs no shim (README §5).
- **Registration** goes through the extension registry: the `gpu` ops'
  lowering tables (`extLowering`) for the two GPU backends, and handlers on
  the CPU backends are optional (the fallbacks are correct).

### 4.6 Semantics on the device

D10 makes integer division by zero and `MIN / -1` throw, which device code
cannot do. **Proposed (D19, decided at the M7 exit):** inside device
functions these cases have a defined, non-throwing result: `x / 0 = 0`,
`x % 0 = x`, `MIN / -1 = MIN`, `MIN % -1 = 0`. The partition pass emits them
as `select` over unchecked operations, which [13 §5.10](13-nir-spec.md)
defines once the divisor is made safe. Because the rewrite happens in NIR,
the CPU fallback of a kernel uses the same semantics as the device, and the
differential fuzzer can compare them.

Shifts are masked and float→int casts saturate as everywhere else
(D10, D11); IEEE floats are strict, with no fast-math on the device either.

### 4.7 Reuse

- A GPU module's NIR image can be saved, loaded and recompiled like any
  other ([07](07-serialization.md)).
- There are no native GPU payloads in 0.4: the CUDA shared library and the
  Metal pipeline are rebuilt from the image after loading.
- L0 caches the compiled module in process; L1 and L2 store only the NIR
  image for GPU modules.

## 5. Tests

| Test | Change |
|---|---|
| `GPUTracingTest` | traces as pre-SSA NIR; golden dumps regenerated in their own commit |
| `GPUCodegenTest` | the generated CUDA and Metal sources regenerated in their own commit and compared byte for byte, as today (`plugins/gpu/test/data/` stays excluded from `format.sh`) |
| `GPUExecutionTest` | unchanged; runs where a device exists (D15) |
| new: CPU fallback | every GPU test function runs on `tbc`, `asmjit` and MLIR through the fallbacks, single-threaded, with the 0.3 results |
| new: device code rules | one negative test per rule of §4.3 |
| new: division semantics | the D19 cases on the device (where available) and in the CPU fallback |
| fuzzer | kernels generated with `gpu.launch`, compared between the CPU fallback and the C++ oracle |

## 6. Exit criteria (M7, GPU part)

- `ENABLE_GPU_PLUGIN` builds again; the three GPU CI legs are restored.
- Every 0.3 GPU test passes: tracing and codegen on every CI leg, execution
  where a device exists; the manual GPU run of D15 is recorded.
- The user-facing GPU API (`gpu.hpp`) is unchanged from 0.3; its 0.3 tests
  are part of `test/api-compat/` from this milestone on.
- D19 decided and documented in `docs/` and in the migration guide.

## 7. Risks

| Risk | Mitigation |
|---|---|
| GPU support is broken for most of the release | it is the declared plan; the user API headers stay in the tree; the 0.3 tests and reference sources are kept and ported, not rewritten |
| The NIR emitter changes generated sources | byte-for-byte codegen tests, regenerated once in an isolated commit and reviewed |
| `Pure` thread-index ops hoisted out of a kernel | they are pure only within one kernel invocation; device functions are never inlined into host code, and kernels are only reached through `gpu.launch` |
| Semantics differ between device and CPU fallback | the partition pass rewrites division in NIR, so both run the same instructions; the fuzzer compares them |
| No GPU hardware in CI | D15: CUDA compile-only, Metal where the runner exposes a device, a manual run before the release |
