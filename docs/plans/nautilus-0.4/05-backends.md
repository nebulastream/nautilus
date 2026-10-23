# Component: Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M4 · Builds on #561
(MLIR/LLVM state reuse)

## 1. Purpose

Every backend reads SSA-level NIR after legalization
([03](03-conversion-and-passes.md)). 0.4 keeps one native backend, the
existing `asmjit` backend:
- it reads NIR, with array side tables;
- it keeps AsmJit's register allocator, so there is **no regression in code
  quality against 0.3**;
- it fixes its call-heavy compile-time cost;
- it is the default on x86-64 and AArch64.

MLIR/LLVM stays for peak performance and persisted native code, and the
remaining backends are ported to NIR. There is no new backend, no second
AsmJit mode and no custom allocator in 0.4.

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
| **`asmjit`** (§4) | **Default** on x86-64 and AArch64 (decision D3) | Reads NIR directly; AsmJit's allocator; code quality of 0.3 |
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

## 4. The AsmJit backend

### 4.1 Decision: keep AsmJit's allocator

The 0.4 `asmjit` backend keeps AsmJit's `Compiler`, its register allocator
and the post-RA peephole passes. What changes is everything around them:
the input (NIR) and the side tables. The call-heavy liveness cost was a bug in
our own AsmJit patch and is already fixed (§4.3).

Why not our own single-pass allocator (P0 and the profile in
[p0-baseline.md](p0-baseline.md)):

- **AsmJit's allocator is not the dominant cost on normal code.** On linear
  code, `finalize()` plus the RA passes are about 10% of all instructions,
  ~40% of backend time (≈ 0.17 µs/op). The rest of the backend is our
  lowering: hash-map frames, node creation, dispatch. NIR fixes most of
  that.
- **Keeping it guarantees no code-quality regression** and avoids writing
  and maintaining an allocator.
- **The one real problem was ours:** the quadratic liveness cost on
  call-heavy functions came from Nautilus's patch to AsmJit's liveness pass,
  and is fixed (§4.3).

A custom linear-scan allocator on `Assembler` stays a possible later step,
taken only if real workloads (TPC-H, NebulaStream) still show the allocator
as a large share of compile time once the frontend and conversion work has
landed.

### 4.2 What stays and what changes

**Stays:**
- the name (`engine.backend=asmjit`);
- the two lowering providers (`X64LoweringProvider`, `A64LoweringProvider`)
  and their instruction selection;
- `x86::Compiler` / `a64::Compiler`, AsmJit's register allocator, and the
  post-RA peephole passes;
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
| Constants | constant-inlining special cases | `ConstPool` operands become immediates where the instruction allows |
| Virtual registers | one per IR value | created on first use and reused after last use where the type allows, to shrink the allocator's work |
| Call-heavy functions | quadratic liveness in our AsmJit patch (§4.3) | fixed on `main` before 0.4 |

### 4.3 The call-heavy liveness cost (found and fixed)

P0 measured `calls` (N runtime calls in one straight-line function) as
superlinear: `BaseRAPass::buildLiveness()` grew 20x for 5x the calls and
took 49% of the compile at N = 5000.

**Cause:** Nautilus's own patch to AsmJit's liveness pass
(`third_party/asmjit/src/asmjit/core/rapass.cpp`, the `[Nautilus]` blocks).
The patch marks registers whose live range spans a call, so that they get a
callee-saved register. To find the first call inside a register's live
range, it walked the call list from the start for every register: O(calls ×
registers).

**Fix** (a separate PR against `main`):
- a binary search for the first call of each span;
- an early exit once a register's survival mask covers the union of every
  call's clobbers.

The generated code is byte-identical on every corpus case checked.

| `calls`, asmjit | `buildLiveness()` before | after | Whole compile before | after |
|---|---|---|---|---|
| N = 1000 | 6.5 M instructions | 1.7 M | 35.6 M | 30.8 M |
| N = 5000 | 132.7 M | 9.0 M (linear) | 268.3 M | 144.7 M (−46%) |

The rest of the call cost is our lowering (`AsmJitLoweringProvider::lower`,
linear). NIR's array side tables and fewer virtual registers (§4.2) address
it.

### 4.4 Targets

Measured with the latency corpus, `ExecutionBenchmark`, and TPC-H
(nautilus-duck):

- **Code quality: no regression against 0.3's `asmjit`.** This holds by
  construction (same allocator and peepholes) and is checked on every
  benchmark. It is a release criterion.
- **Compile time:**
  - backend ≤ 0.25 µs/op on linear code (0.3: ~0.4 µs/op);
  - `calls` linear in N (§4.3, already true after the fix).
- **Coverage:** every `asmjit` test passes on x86-64 and AArch64, including
  exceptions and every in-tree extension.

### 4.5 Steps within M4

1. Port the lowering providers to NIR with array side tables and
   `ConstPool` immediates.
2. Reduce virtual-register churn (create on first use, reuse after last
   use).
3. Confirm the call-heavy fix (§4.3) on NIR on x86-64 and AArch64, with the
   `calls` corpus case in CI.

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
- A custom register allocator, a second AsmJit mode, a new native backend,
  and any TPDE backend, adapter or TPDE-based middle tier.

## 8. Exit criteria (M4)

- Every backend passes the execution tests and the differential fuzzer on
  NIR.
- `asmjit` meets the targets of §4.4.
- End to end ≤ 0.75 µs/op on linear corpus code with the default backend
  (README §1).
- No execution-time regression per backend on `ExecutionBenchmark` and
  TPC-H, `asmjit` included.
- Decision D3 taken on measured results.

## 9. Risks

- **Other superlinear paths in our AsmJit patches.** The call-heavy case was
  one. The corpus (`calls`, `straight`, `loopBody` at 10k+ ops) runs in CI
  and guards against the next one.
- **AsmJit's allocator stays a fixed cost** (~0.17 µs/op on linear code).
  If the frontend and conversion work makes it dominant, a custom allocator
  becomes the next step, decided on TPC-H and NebulaStream data.
- **The plugin lowering API is public surface.** Keep it narrow (operands,
  results, scratch registers, emitter).
