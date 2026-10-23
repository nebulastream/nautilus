# Component: Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M4 · Builds on #561
(MLIR/LLVM state reuse)

## 1. Purpose

Every backend reads SSA-level NIR after legalization
([03](03-conversion-and-passes.md)). 0.4 turns the existing `asmjit`
backend into a single-pass backend:
- one emission path, aiming for minimal compile latency at about LLVM -O0
  code quality;
- the default on x86-64 and AArch64.

MLIR/LLVM stays for peak performance and persisted native code, and the
remaining backends are ported to NIR. There is no new backend and no
second AsmJit mode.

## 2. Today and what it costs

| Backend | How it works in 0.3 | P0 / CI measurements |
|---|---|---|
| `bc` | register bytecode interpreter | 0.2–0.3 µs/op lowering |
| `tbc` | threaded bytecode (tail-call dispatch), optional copy-and-patch JIT (`tbc.mode=jit`) | 0.2–0.3 µs/op lowering |
| `asmjit` | `X64LoweringProvider`/`A64LoweringProvider` emit through `x86::Compiler`/`a64::Compiler`: virtual registers, a builder node list, AsmJit's global RA pass, a post-RA peephole pass, `finalize()` | ~0.4 µs/op normally; **~4 µs per call at 5000 calls (superlinear, 70–76% of the compile)**; Compiler + RA ≈ 10% of all instructions on linear code |
| MLIR → LLVM | builds a `DialectRegistry` and `MLIRContext` per compile; LLVM dialect → LLVM IR; O3 with `CodeGenOptLevel::Aggressive` | 3.3–5 ms for trivial functions on CI; fixed cost 10.2 ms → 4.9 ms per TPC-H kernel with #561 |
| `cpp` | C++ source + external compiler | ~22 ms minimum |
| GPU (CUDA, Metal) | plugin backends | — |

Backends take 11–30% of a compile on `bc`/`tbc`/`asmjit`
([p0-baseline.md](p0-baseline.md)).

## 3. Backend roles in 0.4

| Backend | Role | Notes |
|---|---|---|
| **`asmjit`**, single-pass (§4) | **Default** on x86-64 and AArch64 (decision D3) | Reads NIR directly; target: about LLVM -O0 code quality at minimal latency |
| `tbc` (+ copy-and-patch) | Portable default; iOS and platforms without JIT | Near 1:1 from NIR |
| `bc` | Debugging and simplicity; may later merge into `tbc` (D6) | Near 1:1 from NIR |
| MLIR → LLVM | Peak performance; produces the persisted native payloads ([07](07-serialization.md)) | #561's per-thread state reuse; reads NIR; maps extension types |
| `cpp` | Debugging and reference | |
| GPU (CUDA, Metal) | Plugin backends on NIR | GPU intrinsics as extension ops |

**Tiered compilation keeps its structure** (`TieredJITCompiler`: tier 0
compiled synchronously, tier 1 in the background). Two things change:
- tier 0 defaults to `asmjit` (`tbc` where AsmJit is not available), and
  tier 1 stays MLIR;
- the IR both tiers share is SSA-level NIR, which is immutable after
  conversion, so tier 1 can read it concurrently without copying.

Promotion policy is unchanged in 0.4 (§7). `engine.backend=<x>` still
selects single-tier compilation with exactly that backend.

## 4. The single-pass AsmJit backend

### 4.1 What stays and what changes

**Stays:**
- the name (`engine.backend=asmjit`);
- the two lowering providers (`X64LoweringProvider`, `A64LoweringProvider`)
  and their instruction selection;
- the exception transport (`CapturedExceptionTransport`, exception regions);
- `JitRuntime` (executable memory, W^X and `MAP_JIT` on macOS);
- the intrinsic handlers, which become extension lowering
  ([04 §3.5](04-extensions.md)).

**Changes:**

| | 0.3 | 0.4 |
|---|---|---|
| Input | `IRGraph` | SSA-level NIR |
| Side tables | hash maps (`Frame`, `blockLabels`, `processedBlocks`, `usageCounts_`, `inlinedConstParams_`) | arrays indexed by value and block id |
| Use counts, layout | recomputed (`ir::countUsages`) | from NIR (`useCount`, `BlockInfo`) |
| Emitter | `x86::Compiler` / `a64::Compiler` | `x86::Assembler` / `a64::Assembler` |
| Registers | virtual registers + AsmJit's global RA pass | single-pass local allocator (§4.3) |
| Calls | `cc.invoke` nodes | direct ABI moves and `call`, emitted in place; linear in the number of calls |
| Frames | `FuncNode` frame handling | prologue and epilogue patched at the end |
| Peepholes | post-RA passes (`X64PostRAPeepholePass`, `A64PostRAPeepholePass`) | fusion at selection time |
| Constants | constant-inlining special cases | `ConstPool` operands become immediates |

### 4.2 Why one single-pass path is enough

| | AsmJit `Compiler` + global RA (0.3) | Single-pass local allocation (0.4) |
|---|---|---|
| Compile cost | node list, liveness and allocation over the whole function, `finalize()`; superlinear in calls | one walk; linear |
| Values live across blocks | kept in registers | spilled at merges, except fixed registers for innermost-loop values |
| Reference point | between LLVM -O0 and -O1 code generation | LLVM -O0's `RegAllocFast` is also local; TPDE reports ±9% of -O0 with this design |
| Input IR | cleaned by the fixed-point passes | cleaned by the conversion (folding, CSE, dead blocks), unlike -O0, which compiles unoptimized IR |

A global allocator buys fewer spills in large branchy functions: code
quality between -O0 and -O1, at a multiple of the compile cost. Code that
runs long enough to need that should go to MLIR, which is the peak tier in
any case. For the 0.4 goal (minimal latency at about -O0 quality), the
single-pass path is the right and only design.

### 4.3 Code generation

- **Analysis.** Layout and loop data come from NIR. The only pass of its own
  is a linear backward sweep for last-use positions: per value, the start
  block, the end block, and whether liveness ends inside the end block
  (Kohn et al.).
- **Value table.** Indexed by value id; per value a stack slot, the
  remaining-use count and the register state per part (about 16 B per
  value). Multi-part types (`i128`) take one register per part.
- **Register allocation.** Greedy and local:
  - the lowest free register first; otherwise evict in round-robin order;
  - a value dead after its last use frees its register immediately;
  - values in innermost loops, such as induction variables, get fixed
    callee-saved registers. This is where the backend recovers most of what
    a global allocator gives in hot loops.
- **Edges.** At edges into merge blocks or non-fall-through targets, values
  without a fixed register are spilled. Block arguments become parallel
  moves on the edge, and critical edges that need moves get a split block.
- **Fusion within a block:**
  - compare + branch (today's branch fusion);
  - immediates from `ConstPool`;
  - `add`/`mul` into the address modes of `load`/`store`;
  - a single-use value into its user.
- **Calls** follow the platform ABI directly: arguments are moved into
  argument registers or stack slots, and caller-saved values are spilled
  around the call. Internal calls are labels in one code holder.
- **Frame.** The prologue reserves the frame and is patched at the end with
  the frame size and the callee-saved set; epilogues likewise.
- **Instruction selection** is carried over from the current lowering
  providers. Most of their emission goes through AsmJit's shared emitter
  API, which `Assembler` implements as well. What changes is where operands
  come from:

  ```cpp
  struct OperandSupply {           // the single-pass allocator
  	Operand use(ValueId v);      // register, stack slot or immediate
  	Reg     def(ValueId v);      // destination register for v
  	Reg     scratch(RegClass c); // temporary for this instruction
  	void    release(ValueId v);  // last use reached
  };
  ```

- **Extensions** lower through `lower(ctx, inst)` with the same supply and
  the emitter, or are legalized to their fallback.

TPDE, Umbra's Flying Start and the Wasm baseline compilers are related work
for this technique. Nautilus implements it inside its AsmJit backend and
takes no TPDE dependency.

### 4.4 Targets

Measured with the latency corpus, `ExecutionBenchmark` and TPC-H
(nautilus-duck):

- **Compile time:** backend ≤ 0.1 µs/op, and linear in the number of calls.
- **Code quality:**
  - execution time within 1.1x of LLVM -O0 (the MLIR backend with
    `optimizationLevel=0`) on the loop kernels and TPC-H;
  - ≥ 3x faster than `bc`;
  - within 1.3x of 0.3's `asmjit`.
- **Coverage:** every `asmjit` test passes on x86-64 and AArch64, including
  exceptions and every in-tree extension.

### 4.5 Steps within M4

1. Port the lowering providers to NIR with array side tables, and factor
   instruction selection behind `OperandSupply`. Until step 3, the
   `Compiler` path serves as the operand supply, so tests stay green. It is
   an internal step and is not shipped.
2. Implement the single-pass allocator, calls and frames on `Assembler` on
   x86-64.
3. Delete the `Compiler` path and the post-RA peephole passes once x86-64
   passes all tests and meets §4.4.
4. The same for AArch64.

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
  ops are target-restricted extension ops ([04 §3.3](04-extensions.md)).

## 7. Not in 0.4

- Changes to multi-tier promotion (per-tier IR levels, delayed promotion,
  admission control, the tier lattice).
- Lazy per-function compilation and parallel compilation of a module's
  functions.
- A second AsmJit mode with a global register allocator, a new native
  backend, and any TPDE backend, adapter or TPDE-based middle tier.

## 8. Exit criteria (M4)

- Every backend passes the execution tests and the differential fuzzer on
  NIR.
- `asmjit` meets the targets of §4.4. The `Compiler` path and the post-RA
  peephole passes are deleted.
- End to end ≤ 0.5 µs/op on linear corpus code with the default backend.
- No execution-time regression per backend on `ExecutionBenchmark` and
  TPC-H, except the bounded `asmjit` trade of §4.4 (≤ 1.3x of 0.3's
  `asmjit`, within 1.1x of LLVM -O0).
- Decision D3 taken on measured results.

## 9. Risks

- **Spills in large branchy functions.** Local allocation spills at merges.
  Fixed registers for loop values cover the hot case, and the 1.1x-of-O0
  target gates the release. Modules that need better code use MLIR.
- **Code size.** Single-pass code is larger than optimized LLVM code.
  Measure the I-cache effect on NebulaStream.
- **Late parity on AArch64.** x86-64 can switch first; `tbc` remains the
  portable default.
- **The plugin lowering API is public surface.** Keep it narrow (operands,
  results, scratch registers, emitter) so the allocator can change.
