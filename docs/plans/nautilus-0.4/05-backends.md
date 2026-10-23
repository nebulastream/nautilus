# Component: Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M4 · Builds on #561
(MLIR/LLVM state reuse)

## 1. Purpose

Every backend reads SSA-level NIR after legalization
([03](03-conversion-and-passes.md)). 0.4 improves the existing `asmjit`
backend with a single-pass emission mode and makes it the default on x86-64
and AArch64. It keeps MLIR/LLVM for peak performance and persisted native
code, and ports the remaining backends to NIR. There is no new backend.

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
| **`asmjit`**, improved (§4) | **Default** on x86-64 and AArch64 (decision D3) | Reads NIR directly; new single-pass `fast` mode (default) and today's `optimized` mode |
| `tbc` (+ copy-and-patch) | Portable default; iOS and platforms without JIT | Near 1:1 from NIR |
| `bc` | Debugging and simplicity; may later merge into `tbc` (D6) | Near 1:1 from NIR |
| MLIR → LLVM | Peak performance; produces the persisted native payloads ([07](07-serialization.md)) | #561's per-thread state reuse; reads NIR; maps extension types |
| `cpp` | Debugging and reference | |
| GPU (CUDA, Metal) | Plugin backends on NIR | GPU intrinsics as extension ops |

**Tiered compilation keeps its structure** (`TieredJITCompiler`: tier 0
compiled synchronously, tier 1 in the background). Two things change:
- tier 0 defaults to `asmjit` in `fast` mode (`tbc` where AsmJit is not
  available), and tier 1 stays MLIR;
- the IR both tiers share is SSA-level NIR, which is immutable after
  conversion, so tier 1 can read it concurrently without copying.

Promotion policy is unchanged in 0.4 (§7). `engine.backend=<x>` still
selects single-tier compilation with exactly that backend.

## 4. The improved AsmJit backend

The backend keeps its name (`engine.backend=asmjit`), its two lowering
providers (`X64LoweringProvider`, `A64LoweringProvider`) and their
instruction selection. It also keeps its exception transport
(`CapturedExceptionTransport`, exception regions), its `JitRuntime`
(executable memory, W^X and `MAP_JIT` on macOS) and its intrinsic handlers,
which become extension lowering ([04 §3.5](04-extensions.md)).

Three things change:
- it reads NIR instead of `IRGraph`;
- its side tables become arrays;
- it gets a second, single-pass emission mode.

### 4.1 Reading NIR

- **Value and block maps become arrays** indexed by value and block id:
  `Frame<OperationIdentifier, AsmReg>`, `blockLabels`, `processedBlocks`,
  `usageCounts_` and `inlinedConstParams_` all go
  ([02 §4](02-nir.md)).
- **Use counts and layout come from NIR:** `useCount` and `BlockInfo`
  (layout order, loop depth, loop end). The backend no longer runs
  `ir::countUsages`; loop data is derived data, rebuilt in one linear pass
  after an artifact load ([02 §3.6](02-nir.md)).
- **Constants are immediates:** an operand referencing `ConstPool` is
  emitted as an immediate where the instruction allows it, which replaces
  today's constant-inlining special cases.

### 4.2 Two emission modes, one instruction selection

Most of today's lowering emits instructions through AsmJit's shared emitter
API (`x86::Emitter` / `a64::Emitter`), which `Compiler`, `Builder` and
`Assembler` all implement. Only three things are `Compiler`-specific:
virtual registers (`newGp`), function and call nodes (`FuncNode`,
`cc.invoke`), and `finalize()`. The modes differ exactly there:

| | `optimized` (today's path) | `fast` (new, default) |
|---|---|---|
| Emitter | `x86::Compiler` / `a64::Compiler` | `x86::Assembler` / `a64::Assembler` |
| Registers | virtual registers, AsmJit's global RA pass | **our single-pass local allocator** (§4.3) |
| Calls | `cc.invoke` nodes | direct ABI moves and `call`, emitted in place |
| Frames | `FuncNode` frame handling | prologue and epilogue patched at the end |
| Peephole | post-RA peephole passes (`X64PostRAPeepholePass`, `A64PostRAPeepholePass`) | fusion at selection time (§4.3) |
| Use | when code quality matters more than compile time | default |

Instruction selection is written once against a small operand-supply
interface:

```cpp
// implemented by both modes
struct OperandSupply {
	Operand use(ValueId v);          // register, stack slot or immediate
	Reg     def(ValueId v);          // destination register for v
	Reg     scratch(RegClass cls);   // temporary for this instruction
	void    release(ValueId v);      // last use reached
};
```

In `optimized` mode it hands out virtual registers. In `fast` mode it is the
local allocator. The selected instructions are emitted through the shared
emitter in both modes. The mode is chosen with `asmjit.mode=fast|optimized`.

### 4.3 The fast mode

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
    callee-saved registers.
- **Edges.** At edges into merge blocks or non-fall-through targets, values
  without a fixed register are spilled. Block arguments become parallel
  moves on the edge, and critical edges that need moves get a split block.
- **Fusion within a block** replaces the post-RA peephole passes:
  - compare + branch (today's `enableBranchFusion_`);
  - immediates from `ConstPool`;
  - `add`/`mul` into the address modes of `load`/`store`;
  - a single-use value into its user.
- **Calls** follow the platform ABI directly: arguments are moved into
  argument registers or stack slots, and caller-saved values are spilled
  around the call. Internal calls are labels in one code holder. Cost is
  linear in the number of calls, which removes the superlinear behavior P0
  measured in `optimized` mode.
- **Frame.** The prologue reserves the frame and is patched at the end with
  the frame size and the callee-saved set; epilogues likewise.

TPDE, Umbra's Flying Start and the Wasm baseline compilers are related work
for this technique. Nautilus implements it inside its AsmJit backend and
takes no TPDE dependency.

### 4.4 The optimized mode

`optimized` is today's backend reading NIR. It keeps AsmJit's global
register allocator and the post-RA peephole passes, for modules where code
quality matters more than compile time and where MLIR is too expensive.
The superlinear call cost P0 found in this mode is investigated in M4. Its
likely cause is RA work per `InvokeNode` across long functions. It is
fixed if the fix is local; otherwise it is documented as a property of this
mode.

### 4.5 Targets

- `fast` mode backend time ≤ 0.1 µs/op, and linear in the number of calls.
- `fast` mode code within 1.2x of `optimized` mode, and ≥ 3x faster than `bc`
  on the `ExecutionBenchmark` loop kernels.
- Both modes pass every `asmjit` test on x86-64 and AArch64, including
  exceptions and every in-tree extension, before D3 makes `fast` the
  default.

### 4.6 Steps within M4

1. Port today's `Compiler`-based lowering to NIR (`optimized` mode), with
   array side tables. Behavior is identical by construction.
2. Factor instruction selection behind `OperandSupply`.
3. Add `fast` mode on x86-64, then AArch64.
4. Switch the default once §4.5 holds (D3).

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
- A new native backend, and any TPDE backend, adapter or TPDE-based middle
  tier.

## 8. Exit criteria (M4)

- Every backend passes the execution tests and the differential fuzzer on
  NIR. The fuzzer covers both `asmjit` modes.
- End to end ≤ 0.5 µs/op on linear corpus code with the default backend.
- No execution-time regression per backend on `ExecutionBenchmark` and
  TPC-H (nautilus-duck). For `asmjit`, this holds in `optimized` mode.
- Decision D3 taken on measured results.

## 9. Risks

- **Code size and quality in `fast` mode.** Local allocation produces more
  spills than a global allocator. `optimized` mode and MLIR remain
  selectable per module, and the 1.2x target gates the default.
- **Two modes to maintain.** Shared instruction selection keeps the
  mode-specific code to register supply, calls and frames. Both modes run in
  CI and in the fuzzer.
- **`fast` mode reaches parity late.** `optimized` mode stays the default
  until it does; `tbc` remains the portable default.
- **The plugin lowering API is public surface.** Keep it narrow (operands,
  results, scratch registers, emitter) so the allocator can change.
