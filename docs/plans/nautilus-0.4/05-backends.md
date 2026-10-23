# Component: Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M4 · Builds on #561
(MLIR/LLVM state reuse)

## 1. Purpose

Every backend reads SSA-level NIR after legalization
([03](03-conversion-and-passes.md)). 0.4 adds a single-pass native backend
as the default on x86-64 and AArch64, keeps MLIR/LLVM for peak performance
and persisted native code, and ports the remaining backends to NIR.

## 2. Today and what it costs

| Backend | How it works in 0.3 | P0 / CI measurements |
|---|---|---|
| `bc` | register bytecode interpreter | 0.2–0.3 µs/op lowering |
| `tbc` | threaded bytecode (tail-call dispatch), optional copy-and-patch JIT (`tbc.mode=jit`) | 0.2–0.3 µs/op lowering |
| `asmjit` | `x86::Compiler`/`a64::Compiler`: builder node list, AsmJit's global RA pass, post-RA peephole pass, `finalize()` | ~0.4 µs/op normally; **~4 µs per call at 5000 calls (superlinear, 70–76% of the compile)**; Compiler + RA ≈ 10% of all instructions on linear code |
| MLIR → LLVM | builds a `DialectRegistry` and `MLIRContext` per compile; LLVM dialect → LLVM IR; O3 with `CodeGenOptLevel::Aggressive` | 3.3–5 ms for trivial functions on CI; fixed cost 10.2 ms → 4.9 ms per TPC-H kernel with #561 |
| `cpp` | C++ source + external compiler | ~22 ms minimum |
| GPU (CUDA, Metal) | plugin backends | — |

Backends take 11–30% of a compile on `bc`/`tbc`/`asmjit`
([p0-baseline.md](p0-baseline.md)).

## 3. Backend roles in 0.4

| Backend | Role | Notes |
|---|---|---|
| **single-pass native** (§4) | **Default** on x86-64 and AArch64 (decision D3) | Reads NIR and `BlockInfo` directly; value parts for `i128`; extension lowering through a narrow API |
| `tbc` (+ copy-and-patch) | Portable default; iOS and platforms without JIT | Near 1:1 from NIR |
| `bc` | Debugging and simplicity; may later merge into `tbc` (D6) | Near 1:1 from NIR |
| MLIR → LLVM | Peak performance; produces the persisted native payloads ([07](07-serialization.md)) | #561's per-thread state reuse; reads NIR; maps extension types |
| `cpp` | Debugging and reference | |
| `asmjit` | Removed in 0.4 once the single-pass backend reaches parity; otherwise ported to NIR as a fallback | Its superlinear call handling argues against keeping it as a default |
| GPU (CUDA, Metal) | Plugin backends on NIR | GPU intrinsics as extension ops |

**Tiered compilation keeps its structure** (`TieredJITCompiler`, tier 0
compiled synchronously, tier 1 in the background). Two things change:
- tier 0 defaults to the single-pass backend (`tbc` where it is not
  available), and tier 1 stays MLIR;
- the IR both tiers share is SSA-level NIR, which is immutable after
  conversion, so tier 1 can read it concurrently without copying.

Promotion policy is unchanged in 0.4 (§7). `engine.backend=<x>` still
selects single-tier compilation with exactly that backend.

## 4. The single-pass native backend

Nautilus implements its own single-pass backend. There is no TPDE backend
or adapter; TPDE, Umbra's Flying Start and the Wasm baseline compilers are
related work for the technique. What they share:
- direct emission from the IR, with no backend IR;
- linear-time analyses only;
- local register allocation;
- optimization paid for only by code that runs long enough.

### 4.1 Analysis

- NIR's `BlockInfo` already provides layout order (RPO, loop bodies
  contiguous), loop depth and loop end, and `useCount` comes from the
  conversion. Loop data and use counts are derived data, rebuilt in one
  linear pass after an artifact load ([02 §3.6](02-nir.md)).
- The backend's only pass of its own is a linear backward sweep for
  last-use positions. Per value it records the start block, the end block,
  and whether liveness ends inside the end block (Kohn et al.).

### 4.2 Code generation: one walk over blocks and instructions

- **Value table.** Indexed by value id; per value a stack slot, the
  remaining-use count and the register state per part (about 16 B per
  value).
- **Register allocation.** Greedy and local:
  - the lowest free register first; otherwise evict in round-robin order;
  - a value dead after its last use frees its register immediately;
  - values in innermost loops, such as induction variables, get fixed
    callee-saved registers.
- **Edges.** At edges into merge blocks or non-fall-through targets, values
  without a fixed register are spilled. Block arguments become parallel
  moves on the edge, and critical edges that need moves get a split block.
- **Fusion within a block:**
  - compare + branch;
  - immediates from `ConstPool`;
  - `add`/`mul` into the address modes of `load`/`store`;
  - a single-use value into its user.
- **Frame.** The prologue reserves the frame and is patched at the end with
  the frame size and the callee-saved set; epilogues likewise.
- **Encoding** through `asmjit::x86::Assembler` / `a64::Assembler`, which
  are already dependencies, not AsmJit's `Compiler`. The instruction
  lowering in today's `X64LoweringProvider`/`A64LoweringProvider` carries
  over. Encoder snippets can optionally be generated with the
  `tools/stencils` toolchain.
- **Executable memory** comes from AsmJit's `JitRuntime`, as today, which
  already handles W^X and `MAP_JIT` on macOS.
- **Calls** use the platform ABI directly. Internal calls are labels in one
  code holder. Cost is linear in the number of calls.
- **Exceptions** reuse the unwind and personality machinery of today's
  `asmjit` backend (`CapturedExceptionTransport`, exception regions).
- **Extensions** lower through `lower(ctx, inst)`
  ([04 §3.5](04-extensions.md)) or are legalized to their fallback.

### 4.3 Targets

- Backend time ≤ 0.1 µs/op, and linear in the number of calls.
- Code within 1.2x of today's `asmjit`, and ≥ 3x faster than `bc` on the
  `ExecutionBenchmark` loop kernels.
- Feature parity with `asmjit` (AArch64, exceptions, every in-tree
  extension) before D3 makes it the default.

## 5. MLIR → LLVM

- **Per-thread state reuse from #561:**
  - host `TargetMachine`s reused across compiles;
  - one `MLIRContext` per thread, replaced every 256 compiles;
  - the module verified once (`mlir.verifyEachPass` for debugging);
  - `LLJIT` given the data layout.
- **Input is NIR:** blocks and terminators map to `cf`, arithmetic to
  `arith`, extension types through the plugin type mapping.
- **Optimization level:** keep today's options. A size-dependent default
  (O2 below a threshold) is an optional tuning, measured with the corpus.
- **Persisted native payloads** for artifacts are produced here
  ([07](07-serialization.md)): one relocatable object per payload, with the
  import table of #527.

## 6. `bc`, `tbc`, `cpp`, GPU

- `bc`/`tbc` lower NIR almost one to one. Their register allocators work on
  value ids, and their handler tables are indexed by extension opcode.
- `cpp` prints from NIR. Extension ops use expression templates or calls.
- GPU backends read NIR. Address spaces are extension types, and GPU-only
  ops are valid only for GPU targets.

## 7. Not in 0.4

- Changes to multi-tier promotion (per-tier IR levels, delayed promotion,
  admission control, the tier lattice).
- Lazy per-function compilation and parallel compilation of a module's
  functions.
- Any TPDE backend, adapter or TPDE-based middle tier.

## 8. Exit criteria (M4)

- Every backend passes the execution tests and the differential fuzzer on
  NIR.
- End to end ≤ 0.5 µs/op on linear corpus code with the default backend.
- No execution-time regression per backend on `ExecutionBenchmark` and
  TPC-H (nautilus-duck).
- Decision D3 taken on measured parity.

## 9. Risks

- **Code size.** Single-pass code is larger than optimized LLVM code.
  Measure the I-cache effect on NebulaStream; MLIR remains selectable per
  module.
- **Exceptions and unwind** are the most likely parity gap. They are a
  requirement before D3.
- **Late parity.** `tbc` is a complete default; `asmjit` can be ported to
  NIR as a stopgap.
- **The plugin lowering API is public surface.** Keep it narrow (operands,
  results, scratch registers, assembler) so the allocator can change.
