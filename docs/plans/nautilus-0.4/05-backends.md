# Component: Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M4 · Builds on #561
(MLIR/LLVM state reuse)

## 1. Purpose

Every backend reads SSA-level NIR after legalization
([03](03-conversion-and-passes.md)). 0.4 rebuilds the existing `asmjit`
backend as a single-pass backend:
- one emission path with minimal compile latency;
- **no regression in code quality against 0.3's `asmjit`**;
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
| **`asmjit`**, single-pass (§4) | **Default** on x86-64 and AArch64 (decision D3) | Reads NIR directly; linear-scan allocation; code at least as fast as 0.3's `asmjit` |
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
| Registers | virtual registers + AsmJit's global RA pass | linear-scan allocation with splitting, during emission (§4.3) |
| Calls | `cc.invoke` nodes | direct ABI moves and `call`, emitted in place; linear in the number of calls |
| Frames | `FuncNode` frame handling | prologue and epilogue patched at the end |
| Peepholes | post-RA passes (`X64PostRAPeepholePass`, `A64PostRAPeepholePass`) | fusion at selection time |
| Constants | constant-inlining special cases | `ConstPool` operands become immediates |

### 4.2 Design goal: 0.3's code quality at single-pass cost

There is one backend, and it must not regress against 0.3's `asmjit`.
Where 0.3's code quality comes from, and how 0.4 keeps it:

| 0.3 source of quality | How 0.4 keeps it |
|---|---|
| AsmJit's global register allocator: liveness over the whole function; values stay in registers across blocks | **Linear-scan allocation with interval splitting** (§4.3): global live intervals, assigned in one walk |
| Post-RA peephole rule 1: self-move elimination | Direct emission never produces a self-move, except the intentional `mov r32, r32` zero extension, which stays |
| Post-RA peephole rule 2: `mov r, 0` → `xor r32, r32` where flags are dead | Applied at selection time; flag liveness is known there (flags are only live between a fused compare and its branch) |
| Branch fusion, constant inlining | Kept: compare + branch fusion; `ConstPool` operands as immediates; address-mode fusion |

What 0.4 removes is the *cost* of that quality: the node list, the iterative
passes and `finalize()`. P0 measured those as ~10% of all instructions on
linear code, and as superlinear in the number of calls.

### 4.3 Register allocation: linear scan with splitting

This is the allocator family JITs use for exactly this trade-off
(Poletto & Sarkar; Wimmer & Mössenböck's linear scan in HotSpot's client
compiler):

1. **Live intervals in one linear pass.**
   - NIR's layout order numbers every instruction, and `BlockInfo` gives
     loop extents.
   - One backward sweep per block computes live-in sets and the use
     positions of each value, stored in a flat array.
   - A value live into a loop header is extended to the loop end. This
     uses the loop tree from NIR, with no iterative data flow (Kohn et al.).
2. **Assignment during emission, in layout order:**
   - an interval starting at an instruction takes a free register, or its
     hint if that is free;
   - if none is free, the interval whose next use is farthest away is split
     and spilled, and it is reloaded before its next use;
   - the active set is ordered by end position, so each step costs
     O(log R) for R registers.
3. **Hints instead of moves:**
   - block arguments prefer the register of their incoming values
     (coalescing);
   - values crossing calls prefer callee-saved registers;
   - arguments and return values prefer their ABI registers.
4. **Edges.** A value keeps its register across block boundaries. Resolution
   moves are emitted only on edges where a split interval lives in
   different locations on the two sides, and critical edges that need moves
   get a split block.

Total cost: one analysis pass plus one emission pass,
O(n + intervals · log R).

### 4.4 Code generation

- **Value table.** Indexed by value id; per value its interval, current
  location (register per part, or stack slot) and spill slot. Multi-part
  types (`i128`) take one interval per part.
- **Instruction selection** is carried over from the current lowering
  providers. Most of their emission goes through AsmJit's shared emitter
  API, which `Assembler` implements as well. What changes is where operands
  come from:

  ```cpp
  struct OperandSupply {           // backed by the linear-scan allocator
  	Operand use(ValueId v);      // register, stack slot or immediate
  	Reg     def(ValueId v);      // destination register for v
  	Reg     scratch(RegClass c); // temporary for this instruction
  	void    release(ValueId v);  // last use reached
  };
  ```

- **Calls** follow the platform ABI directly: argument moves, then `call`.
  Intervals that cross the call live in callee-saved registers or are
  spilled around it. Internal calls are labels in one code holder. Cost is
  linear in the number of calls.
- **Frame.** The prologue reserves the frame and is patched at the end with
  the frame size and the callee-saved set; epilogues likewise.
- **Extensions** lower through `lower(ctx, inst)` with the same supply and
  the emitter, or are legalized to their fallback.

TPDE, Umbra's Flying Start and the Wasm baseline compilers are related work
for single-pass emission. HotSpot's client compiler is related work for
linear-scan allocation in a JIT. Nautilus implements both inside its AsmJit
backend and takes no TPDE dependency.

### 4.5 Targets

Measured with the latency corpus, `ExecutionBenchmark`, and TPC-H
(nautilus-duck):

- **Code quality: no regression against 0.3's `asmjit`.** Execution time is
  at most 0.3's on every benchmark, beyond measurement noise (3% on the
  shared runners, confirmed by a rerun). This is a release criterion.
- **Compile time:**
  - backend ≤ 0.1 µs/op (0.3: ~0.4 µs/op);
  - linear in the number of calls (0.3: ~4 µs per call at 5000 calls).
- **Coverage:** every `asmjit` test passes on x86-64 and AArch64, including
  exceptions and every in-tree extension.
- **For reference:** execution time compared with LLVM -O0 (the MLIR
  backend at `optimizationLevel=0`) is reported. It is expected to be at
  least as good, because the input is already cleaned by the conversion.

### 4.6 Steps within M4

1. **Port the lowering providers to NIR** with array side tables. The
   `Compiler` path serves as operand supply, so behavior and code quality
   are unchanged. This is the M4 baseline: the reference for "no
   regression", and the fallback if step 3 is not ready.
2. **Factor instruction selection behind `OperandSupply`,** and implement
   the linear-scan allocator, calls and frames on `Assembler` for x86-64.
3. **Compare on every benchmark against the step-1 baseline.** Delete the
   `Compiler` path and the post-RA peephole passes only when §4.5 holds on
   x86-64.
4. The same for AArch64.

If step 3 does not reach parity in time for the release, 0.4 ships step 1:
one `asmjit` backend on NIR with 0.3's allocator and code quality. It still
gains the IR, side-table and conversion improvements, and its call
superlinearity is investigated separately. Either way there is one backend
and no regression.

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
- A second AsmJit mode, a new native backend, and any TPDE backend, adapter
  or TPDE-based middle tier.

## 8. Exit criteria (M4)

- Every backend passes the execution tests and the differential fuzzer on
  NIR.
- `asmjit` meets the targets of §4.5. The `Compiler` path and the post-RA
  peephole passes are deleted, or the step-1 fallback of §4.6 applies.
- End to end ≤ 0.5 µs/op on linear corpus code with the default backend.
- No execution-time regression per backend on `ExecutionBenchmark` and
  TPC-H, `asmjit` included.
- Decision D3 taken on measured results.

## 9. Risks

- **Linear scan falls short of AsmJit's allocator** on some kernels. The
  step-1 baseline is the reference on every benchmark, a shortfall blocks
  deleting the `Compiler` path, and the fallback keeps 0.3's quality. Known
  levers before falling back: better split positions, more hints, and
  rematerializing constants instead of spilling them.
- **Code size.** Single-pass code is larger than optimized LLVM code.
  Measure the I-cache effect on NebulaStream.
- **Late parity on AArch64.** x86-64 can switch first; AArch64 keeps the
  step-1 path until it reaches parity.
- **The plugin lowering API is public surface.** Keep it narrow (operands,
  results, scratch registers, emitter) so the allocator can change.
