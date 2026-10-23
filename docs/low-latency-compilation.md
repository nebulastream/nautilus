# Design: Low-Latency Compilation from Trace to Machine Code

Status: proposal · Scope: tracing, IR pipeline, tiered compiler, backends

> **Update:** multi-tier aspects are on hold. The trace and IR
> representation work, including the tracing-cost items of §4.3, is
> designed in detail in [ir-representation-latency.md](ir-representation-latency.md),
> which supersedes this document where they overlap.

## 1. Goal

Make the time from "a function is first called" to "native code is running" as
short as possible, while keeping the peak performance of the MLIR/LLVM tier
for long-running code.

Latency, as used here, is the critical path of a tier-0 compile:

```
T_first = T_trace + T_trace→IR + T_irPasses(critical path) + T_backend + T_firstCallFixups
```

Throughput of the background tier (tier 1) matters too, but only as CPU spent
next to query execution and as the time until peak code is installed.

This matters for NebulaStream and for every interactive workload: many small,
short-lived queries, where tens of milliseconds of LLVM time are larger than
the query itself (Kohn et al., ICDE 2018; Kersten et al., VLDBJ 2021).

## 2. Where the latency is today

The current pipeline (`compiler/CompilationPipeline.cpp`,
`compiler/TieredCompiler.cpp`) is already tiered and arena-based. Reading the
code shows these latency drivers. None of them has been measured for this
document; §6, phase 0, does that measurement before any engineering work.

| Stage | Today | Latency driver |
|---|---|---|
| Tracing | `TraceContext::runScope` re-executes the whole scope body once per explored path (`SymbolicExecutionContext`, up to `MAX_ITERATIONS`), replaying recorded branch decisions in `FOLLOW` mode | Cost is roughly *paths × path length*. Snapshot hashing runs on every operation. `region()` already bounds this to a sub-scope. |
| Trace → IR | Complete `ExecutionTrace` first, then `TraceToIRConversionPhase` builds SSA (Braun et al.) into a second, arena-allocated `IRGraph` | Two materializations of every operation. Each is linear. |
| IR passes | Fixed-point group (constant folding, algebraic simplification, branch folding, empty-block elimination, block merging, DCE, argument pruning), up to `ir.maxPipelineIterations` = 4 rounds | **On the tier-0 critical path the level is `max(tier0, tier1)`** (`compileModule` passes both backends to `irOptimizationLevel`). With default `bc` the result is `Full`, so the fast tier pays for the full pipeline. |
| Tier-0 backend | `bc`/`tbc` lowering. `asmjit` uses `x86::Compiler`/`a64::Compiler`: a builder node list, AsmJit's `RAPass` (global allocator), a post-RA peephole pass, then `finalize()` | AsmJit's Compiler is a multi-pass framework. It builds a node list and runs global liveness and register allocation over it. A direct-emit design (TPDE, Flying Start) does this work in one pass. |
| Tier-1 backend | `MLIRCompilationBackend::compile` builds a `DialectRegistry` and a `MLIRContext` per compile. It lowers to the LLVM dialect, translates to LLVM IR, optimizes at O3 by default and runs codegen at `CodeGenOptLevel::Aggressive` | Fixed setup cost per compile, plus a full LLVM pipeline. LLVM codegen alone costs about 10x TPDE even at -O0. |
| Promotion | `promoteAsync` starts a `std::thread` per module | Thread creation per module. There is no admission control, so every module is promoted, including those that finish before tier 1 is ready. |

## 3. What recent compilers teach us

| System | Idea | Reported result | Applies to Nautilus as |
|---|---|---|---|
| **TPDE** (Schwarz, Kamm, Engelke, CGO 2026) | Back-end framework with one analysis pass and one combined isel + regalloc + encode pass. It reads the IR through a templated *IR adapter*. Instruction snippets are generated at build time from C via LLVM MIR | 8–24x faster than LLVM -O0 at about the same code quality. An Umbra IR adapter of about 3.6 kLOC matches Umbra's hand-written 11 kLOC DirectEmit back-end | Main model for the new tier-0 native backend (§4.2) |
| **Flying Start / DirectEmit** (Umbra; Kersten, Leis, Neumann, VLDBJ 2021) | Single-pass x86 emission from a database IR using asmjit, with stack-slot reuse, simple register allocation and compare-branch fusion | Code about as fast as LLVM -O0, compiled far faster than LLVM | Shows the approach works for query code in exactly this domain |
| **Compile-time analysis of JIT back-ends** (Engelke, Schwarz, CGO 2024) | Compares LLVM, GCC, Cranelift and a single-pass back-end on Umbra queries | Single-pass compiles about 16x faster than Cranelift with similar run time. LLVM is best at run time and gains a lot from compile-time tuning | Supports the tier lattice: single-pass for tier 0, a *tuned* LLVM for tier 1 |
| **MIR** (Makarov) | Small JIT IR. Optimization levels select the pass set (`-O0` uses a fast allocator, `-O1`+ uses linear scan). Lazy per-function generation. The interpreter and JIT code can call each other | About 100x faster to compile than `gcc -O2`, and the code is about 91% of its speed (geomean) | Pass sets scaled to the tier, lazy per-function compilation (§4.5) |
| **Copy-and-patch** (Xu, Kjolstad, OOPSLA 2021; CPython 3.13) | Stencils compiled ahead of time with holes, stitched and patched at run time | Compiles faster than Liftoff while generating better code | Already present as `tbc.mode=jit`. It stays the portable tier 0 (§4.6) |
| **V8 Sparkplug** | Baseline JIT that emits code in one linear pass straight from bytecode, without its own IR | V8 reported 5–15% better real-world benchmark performance | Emit code straight from the IR, with no backend-side IR |
| **Adaptive execution** (Kohn, Leis, Neumann, ICDE 2018) | Starts in a bytecode interpreter and switches morsel by morsel to compiled code once compilation pays off | Low latency on small inputs, full throughput on large ones | Promotion policy (§4.7). The version-counter swap already supports it |

What these systems share:

1. **Direct emission from the IR.** The fast tier does not build another IR.
2. **Linear-time analyses only.** That means RPO with a loop tree and a
   linear liveness algorithm. There is no iterative data flow and no global
   allocator on the fast path.
3. **Machine-description reuse.** Snippets or stencils are generated at build
   time from C++, so the fast tier does not need a second hand-written
   instruction selector.
4. **Optimization is paid for only by code that runs long enough.**

## 4. Design

### 4.1 Split the IR pipeline per tier ("pay on the background thread")

The graph is traced once and shared between the tiers, which is good. The
critical path, however, runs the passes that the *tier-1* backend wants. The
change:

- `compileToIR` runs only the passes the tier-0 backend requests. For the new
  native tier 0 this is a cheap level, see below.
- `promoteAsync` runs the remaining passes on the promotion thread, before
  tier-1 codegen. This requires that the tier-0 executable no longer reads the
  `IRGraph` after `compileIR` returns. That holds for an executable that owns
  its code or bytecode, and needs a check per backend (the dump paths and the
  location map for tbc/asmjit).
- Add an `IROptimizationLevel::Cheap` level. It means one round, with no fixed
  point, of the linear passes that shrink the input for a single-pass
  backend: constant folding with copy propagation, constant branch folding,
  empty-block elimination and argument pruning. Constant folding was recently
  made linear (#545).
- Do as much as possible *during* trace→IR conversion. Braun-style SSA
  construction already folds trivial block arguments. Local constant folding
  and copy propagation can run on the fly at operation creation (a
  "peephole-at-construction" builder), so `Cheap` often has nothing left to
  do.

The expected effect: the fixed-point pipeline leaves the critical path, with
no loss for tier 1.

### 4.2 New tier 0: a single-pass native backend (TPDE-style)

This backend is the core of the proposal. Its structure follows TPDE and
Flying Start:

**Analysis pass (linear):**
1. Number the blocks and lay them out in RPO, keeping loop bodies contiguous
   (the loop tree from Wei et al. tolerates irreducible CFGs; `LoopInfo`
   exists already).
2. Compute liveness with the linear algorithm (Kohn et al.): for every value,
   a start block, an end block, a flag for whether liveness ends inside the
   end block, and a use count.

**Codegen pass (one walk over blocks and operations):**
- A value assignment array indexed by a dense value number. `OperationIdentifier`
  is already a `uint32_t`, so a per-function renumbering suffices. Each entry
  holds a stack slot, the remaining-use count and the register state (16 B
  per value, as in TPDE).
- Greedy local register allocation. Free registers come lowest first.
  Otherwise the allocator evicts a register in round-robin order, and a value
  dead after its last use frees its register immediately. Values in
  innermost loops, such as induction variables, get fixed callee-saved
  registers.
- At edges into merge blocks or non-fall-through targets, values without a
  fixed register are spilled. Block arguments become parallel moves on the
  edge. Critical edges with moves get a split block. Nautilus uses block
  arguments, not phis, so the adapter must split multi-edges whose arguments
  differ (the Cranelift adapter for TPDE does the same).
- Fusion inside a block only: compare + branch, constants into immediates,
  `add`/`mul` into addressing modes for `load`/`store`, and a single-use
  value into its user.
- The prologue reserves the frame and is patched at the end with the frame
  size and the callee-saved set. Epilogues are handled the same way.
- Emit into `asmjit::x86::Assembler` / `a64::Assembler` (*not* `Compiler`),
  so code is encoded at once. This reuses the existing `JitRuntime`, the
  label machinery for internal calls and the intrinsic plugins.

**Instruction selection** has two options:

- *(a) Integrate TPDE itself* via an `IRAdaptor` over `ir::IRGraph`. The
  framework is open source, and the Umbra adapter shows the effort is a few
  kLOC. This gives x86-64 and AArch64, snippet encoders and a tested
  allocator. Costs: an external dependency, the block-argument → phi
  translation, and the need to map Nautilus' exception regions and
  intrinsic plugins onto TPDE.
- *(b) In-house, on AsmJit.* Port the instruction lowering of
  `X64LoweringProvider`/`A64LoweringProvider` onto the new allocator. Snippet
  encoders can come from the stencil tool that already exists for tbc
  (`tools/stencils`), extended to emit register-parameterized encoders
  instead of byte templates.

**Recommendation:** prototype (a) first. It is the shortest way to learn
the achievable latency on Nautilus IR, and it sets the baseline that (b)
must beat. Choose (b) only if the dependency or the exception-handling
mapping becomes a blocker. Either way, the existing `asmjit` backend stays
as a "tier 0.5". It is a multi-pass backend whose global allocator gives
better code at higher cost.

Code from this tier is expected at about LLVM -O0 quality (TPDE: ±9%), which
is far above an interpreter. That makes promotion unnecessary for most short
queries.

### 4.3 Cheaper tracing

Tracing is Nautilus-specific; none of the systems above trace. Proposals,
cheapest first:

1. **Measure paths × length.** Record the number of iterations and replayed
   operations per scope in `CompilationStatistics`. When replay dominates,
   point the user to `region()` (the `nautilus-review` skill can flag it).
2. **Cheaper `FOLLOW` mode.** During replay up to the branch to flip, the
   operations are already in the trace. Replay must only check them and must
   not rebuild operation records or tags. Snapshot hashes can be incremental
   (a rolling hash over the static-val stack) instead of being recomputed per
   operation.
3. **Path order.** Explore depth-first so each replayed prefix is as short as
   possible. Stop at an already fully explored tag at once, which
   `shouldTerminate` already does.
4. **Trace and code caching.** Many NebulaStream queries are structurally
   equal. Offer an opt-in `ModuleOptions` key, set by the user and covering
   the function identity plus every static input. A hit skips tracing *and*
   compilation. Add a second, automatic level keyed by a structural hash of the
   `IRGraph` that skips the backend: it is free to check right after trace→IR
   and is safe because the IR fully determines the code. Without a user
   key, tracing must run to know the IR, so only the automatic level applies.

### 4.4 Cheaper tier 1 (MLIR/LLVM)

Tier 1 runs in the background, but it still costs CPU and delays peak code.

- **Reuse the context.** Keep one `MLIRContext` with registered dialects and
  extensions per thread (or in a pool), together with the
  `llvm::TargetMachine`, instead of building them per compile.
- **Size-dependent optimization level.** Use O2 instead of O3 below a size
  threshold, or a tuned pass list. Engelke & Schwarz show that LLVM gets much
  faster when tuned for compile time.
- **Optional mid tier.** Run LLVM O1 IR passes and then TPDE-LLVM as codegen
  in place of LLVM codegen (TPDE reports about 80x faster than LLVM O1
  back-end time). This is an option for when plain tier 0 is too slow but
  full LLVM is too expensive.
- **Parallelism.** Compile the functions of a module concurrently. ORC
  supports concurrent compilation, and `mlir.enableMultithreading` exists
  already. Use a fixed promotion thread pool instead of a `std::thread` per
  module.

### 4.5 Lazy per-function compilation

This follows MIR's lazy generation and V8/Liftoff lazy compilation. Large
modules contain many functions that are rarely or never called (error paths,
rare operators). For tier 0, compile only the entry function. Internal call
sites go through a stub that compiles its target on first call and patches
the call slot. Tracing still covers the whole module, because tracing defines
the IR. The saving is in backend time, which is proportional to cold code.

### 4.6 Tier lattice and default selection

| Tier | Backend | Codegen cost | Code quality | Use |
|---|---|---|---|---|
| T-1 | interpreter (original callable) | 0 | C++ speed, no specialization | Only while tracing is too expensive |
| T0p | `tbc` (+ copy-and-patch) | very low | interpreter → near baseline | Portable default, iOS, no-JIT platforms |
| **T0** | **single-pass native (new)** | very low | ≈ LLVM -O0 | **Default on x86-64/AArch64** |
| T0.5 | `asmjit` (Compiler + RA) | medium | better than T0 | Optional |
| T1 | MLIR → LLVM (tuned) | high | best | Hot code, in the background |

The default changes from `bc → mlir` to `T0 → T1` where T0 is available,
and `tbc → T1` elsewhere.

### 4.7 Promotion policy

Promotion should only happen when it pays off:

- **Delayed promotion.** Start tier 1 only after tier-0 code has run for
  `engine.tiered.promotionThresholdMs` or a number of calls/invocations. A
  cheap counter in the entry stub is enough. The existing version-counter
  swap installs the new code at the next call boundary, which corresponds to
  Kohn et al.'s morsel boundaries for NebulaStream.
- **Admission control.** Bound the promotion queue. Under load, prefer hot
  modules (by measured tier-0 run time) and drop modules that have already
  finished.

## 5. Measurement plan and targets

Without numbers, none of this should be built. Prerequisites:

- A **compile-latency corpus** in `test/benchmark`: small, medium and large
  straight-line, branchy (many traced paths), loop-heavy and call-heavy
  functions, plus real NebulaStream queries (the NebulaStream CI job already
  exists).
- **Per-stage time and throughput** in `CompilationStatistics`: ns per trace
  operation, ns per IR operation per stage, trace iterations and replayed
  operations, and bytes of code per µs. Keep these in the benchmark history
  with the existing 200% alert.
- **Targets**, to confirm in phase 0. TPDE-class backends reach roughly
  ~10x LLVM -O0, so:
  - tier-0 backend time ≤ 10% of `T_first`, so tracing becomes the dominant
    cost;
  - tier-0 backend at least 10x faster than MLIR at O0;
  - tier-0 code within 1.2x of current `asmjit` code and at least 3x faster
    than `bc` on the loop kernels in `ExecutionBenchmark`.

## 6. Roadmap

| Phase | Work | Exit criterion |
|---|---|---|
| 0 | Corpus, per-stage statistics, baseline report per backend | Breakdown of `T_first` for every corpus entry |
| 1 | §4.1 per-tier IR levels and passes moved to the promotion thread; promotion thread pool | Critical path no longer includes the fixed-point pipeline; tier-1 code unchanged (differential fuzzer) |
| 2 | §4.2 prototype: TPDE adapter over `IRGraph`, x86-64 first | Passes the execution tests and the differential fuzzer; latency compared with `asmjit`/`bc` |
| 3 | Decision TPDE vs. in-house; AArch64; exception regions; intrinsics | Default tier 0 on both architectures |
| 4 | §4.3 tracing cost (incremental hashing, cheaper replay); IR-hash code cache | Tracing ≤ 50% of `T_first` on the branchy corpus |
| 5 | §4.4 MLIR context reuse, tuned levels; §4.7 delayed promotion; §4.5 lazy compilation | Less tier-1 CPU per query at equal steady-state throughput |

## 7. Risks and open questions

- **IR mutation across tiers.** Moving passes to the promotion thread is safe
  only if nothing from tier 0 holds pointers into the graph. That must be
  verified per backend and enforced (for example by a debug-mode freeze
  check).
- **Exceptions.** Exception regions and personality propagation exist for MLIR
  and asmjit. A new tier 0 needs the same unwind information (TPDE supports
  unwind info and personality functions).
- **Code size.** Single-pass code is larger (TPDE: +43–49% against LLVM
  -O0). That is acceptable for short-lived code but affects I-cache with
  many concurrent queries, which is another argument for promotion.
- **SIMD and `__int128`.** TPDE's LLVM back-end excludes vector types. The
  SIMD plugin would have to fall back to tier 0.5 or tier 1 for affected
  functions. Selection per function is a requirement.
- **Two native backends to maintain.** If (a) wins, keep `asmjit` only as
  long as it covers features the new tier lacks.

## References

- T. Schwarz, T. Kamm, A. Engelke. *TPDE: A Fast Adaptable Compiler Back-End
  Framework.* CGO 2026. [arXiv:2505.22610](https://arxiv.org/abs/2505.22610)
- A. Engelke, T. Schwarz. *Compile-Time Analysis of Compiler Frameworks for
  Query Compilation.* CGO 2024.
  [PDF](https://home.cit.tum.de/~engelke/pubs/2403-cgo.pdf)
- T. Kersten, V. Leis, T. Neumann. *Tidy Tuples and Flying Start: Fast
  Compilation and Fast Execution of Relational Queries in Umbra.* VLDB
  Journal 2021. [DOI](https://link.springer.com/doi/10.1007/s00778-020-00643-4)
- A. Kohn, V. Leis, T. Neumann. *Adaptive Execution of Compiled Queries.*
  ICDE 2018. [PDF](https://db.in.tum.de/~leis/papers/adaptiveexecution.pdf)
- V. Makarov. *MIR: a lightweight JIT compiler project.*
  [Repository](https://github.com/vnmakarov/mir),
  [blog](https://developers.redhat.com/blog/2020/01/20/mir-a-lightweight-jit-compiler-project)
- H. Xu, F. Kjolstad. *Copy-and-Patch Compilation.* OOPSLA 2021.
  [arXiv:2011.13127](https://arxiv.org/abs/2011.13127)
- V8 team. *Sparkplug, a non-optimizing JavaScript compiler.* 2021.
  [InfoQ summary](https://www.infoq.com/news/2021/06/v8-sparkplug-compiler/)
- M. Braun et al. *Simple and Efficient Construction of Static Single
  Assignment Form.* CC 2013.
