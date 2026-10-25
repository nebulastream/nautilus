# Component: Backends

Part of the [Nautilus 0.4 design](README.md) · Milestone M4 · Builds on #561
(MLIR/LLVM state reuse)

## 1. Purpose

Every backend reads SSA-level NIR after legalization
([03](03-conversion-and-passes.md)). 0.4 keeps one native backend, the
existing `asmjit` backend:
- it reads NIR, with array side tables;
- it keeps AsmJit's register allocator, so code quality stays at 0.3's
  level (**no regression against 0.3**, checked by `asm-diff`, §4.3);
- it fixes its call-heavy compile-time cost;
- it is proposed as the default on x86-64 and AArch64 (D3, decided at the
  M4 exit on the criterion in §9).

MLIR/LLVM stays for peak performance and persisted native code, and the
remaining backends are ported to NIR, except `bc`, which is removed (§6). There is no new backend, no second
AsmJit mode and no custom allocator in 0.4.

## 2. Today and what it costs

| Backend | How it works in 0.3 | P0 / CI measurements |
|---|---|---|
| `bc` | register bytecode interpreter | 0.2–0.3 µs/op lowering |
| `tbc` | threaded bytecode (tail-call dispatch), optional copy-and-patch JIT (`tbc.mode=jit`) | not measured in P0; assumed equal to `bc` until the M1.10 corpus records it |
| `asmjit` | `X64LoweringProvider`/`A64LoweringProvider` emit through `x86::Compiler`/`a64::Compiler`: virtual registers, a builder node list, AsmJit's global RA pass, a post-RA peephole pass, `finalize()` | ~0.4 µs/op normally; **~4 µs per call at 5000 calls (superlinear, 70–76% of the compile)**; Compiler + RA ≈ 10% of all instructions on linear code |
| MLIR → LLVM | builds a `DialectRegistry` and `MLIRContext` per compile; LLVM dialect → LLVM IR; O3 with `CodeGenOptLevel::Aggressive` | 3.3–5 ms for trivial functions on CI; fixed cost 10.2 ms → 4.9 ms per TPC-H kernel with #561 |
| `cpp` | C++ source + external compiler | ~22 ms minimum |
| GPU (CUDA, Metal) | plugin backends | — |

Backends take 11–30% of a compile on `bc`/`tbc`/`asmjit`
([p0-baseline.md](p0-baseline.md)).

## 3. Backend roles in 0.4

| Backend | Role | Notes |
|---|---|---|
| **`asmjit`** (§4) | **Proposed default** on x86-64 and AArch64 (D3, §9) | Reads NIR directly; AsmJit's allocator; code quality of 0.3 |
| `tbc` (+ copy-and-patch) | Portable default; iOS and platforms without JIT; the only interpreting backend | Near 1:1 from NIR |
| ~~`bc`~~ | **Removed in 0.4** (D6, §6) | Not ported to NIR |
| MLIR → LLVM | Peak performance; produces the persisted native payloads ([07](07-serialization.md)) | #561's per-thread state reuse; reads NIR; maps extension types (from M6) |
| `cpp` | Debugging and reference | |
| GPU (CUDA, Metal) | Plugin backends on NIR, ported last (M7, [17](17-gpu.md)) | GPU intrinsics as extension ops; off from the cut-over until M7 |

**Tiered compilation keeps its structure** (`TieredJITCompiler`: tier 0
compiled synchronously, tier 1 in the background). Two things change:
- tier 0 defaults to `asmjit` (`tbc` where AsmJit is not available, replacing
  today's `bc` fallback), and
  tier 1 stays MLIR;
- the IR both tiers share is SSA-level NIR, which is immutable after
  conversion, so tier 1 can read it concurrently without copying.

How promotion runs (worker pool, cancellation, status, opt-in policy) is
in [11-tiered-compilation.md](11-tiered-compilation.md). `engine.backend=<x>` still
selects single-tier compilation with exactly that backend.

### 3.1 Defined arithmetic (D10, D11) in every port

Every backend implements the defined integer and float semantics of
[13 §5.3, §5.5 and §5.10](13-nir-spec.md) **in its M4 port**: checked integer
`div`/`rem`, shift masking and saturating float→int casts. The
conversion already drops the check of a safe constant divisor and masks
constant shift amounts (M3.1/M3.2, [06 §1](06-semantics.md)); M6.1 keeps
only the folding rules for the remaining edge cases (unsafe constants
folded to an unconditional throw, range-based proofs), the cost
measurement and the plugin side.
The per-backend lowering, including the throw path, is the table in
[06 §4.2](06-semantics.md). For the backends without unwind tables
(`asmjit`, `tbc`, `cpp`), the cold block calls `nautilus_rt_throw_arith`
through the runtime's own capture thunk (no `pointers` entry) with the
instruction's unwind entry, exactly
like a captured may-throw call; the tracer attaches the current cleanup
list to every integer `div`/`rem` ([13 §5.10](13-nir-spec.md)).

## 4. The AsmJit backend

### 4.1 Decision: keep AsmJit's allocator

The 0.4 `asmjit` backend keeps AsmJit's `Compiler`, its register allocator
and the post-RA peephole passes. What changes is everything around them:
the input (NIR) and the side tables. The call-heavy liveness cost was a bug in
our own AsmJit patch and is fixed separately on `main` (#565).

Why not our own single-pass allocator (P0 and the profile in
[p0-baseline.md](p0-baseline.md)):

- **AsmJit's allocator is not the dominant cost on normal code.** On linear
  code, `finalize()` plus the RA passes are about 10% of all instructions,
  ~40% of backend time (≈ 0.17 µs/op). The rest of the backend is our
  lowering: hash-map frames, node creation, dispatch. NIR fixes most of
  that.
- **Keeping it avoids the code-quality risk of a new allocator** and the
  cost of writing and maintaining one. The allocator's input still changes
  (§4.2), so code quality is checked, not assumed (§4.3).
- **The one real problem was ours:** the quadratic liveness cost on
  call-heavy functions came from Nautilus's patch to AsmJit's liveness pass,
  and is fixed (#565).

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
| Virtual registers | one per IR value, created through hash maps (`X64LoweringProvider.hpp`) | created on first use; reused only for values defined and dead within one basic block with no use in a successor, never across blocks, so no liveness analysis is needed outside the allocator |
| Checked arithmetic | division traps in hardware; shifts and float→int casts undefined | §3.1: cold-block throw through the capture thunk, masks, saturating casts |
| Call-heavy functions | quadratic liveness in our AsmJit patch | fixed on `main` before 0.4 (#565) |

### 4.3 Targets

Measured with the latency corpus, `ExecutionBenchmark`, and TPC-H
(nautilus-duck):

- **Code quality: no regression against 0.3's `asmjit`.** The allocator
  and peepholes are unchanged, but their input changes (vreg reuse within
  a block, `ConstPool` immediates, the checks of §3.1), so this is not
  true by construction: it is checked by the `asm-diff` leg on every PR
  and on every benchmark. It is a release criterion.
- **Compile time:**
  - backend ≤ 0.25 µs/op on linear code (0.3: ~0.4 µs/op);
  - `calls` linear in N (already true after #565).
- **Coverage:** every `asmjit` test passes on x86-64 and AArch64, including
  exceptions and every in-tree extension.

### 4.4 Steps within M4

1. Port the lowering providers to NIR with array side tables and
   `ConstPool` immediates.
2. Reduce virtual-register churn (create on first use; reuse only within
   one basic block, §4.2), with `asm-diff` before and after.
3. Lower the checked arithmetic of §3.1, and the line table of §7.
4. Confirm the call-heavy fix (#565) on NIR on x86-64 and AArch64, with the
   `calls` corpus case in CI.

## 5. MLIR → LLVM

- **Per-thread state reuse from #561:**
  - host `TargetMachine`s reused across compiles;
  - one `MLIRContext` per thread, replaced every 256 compiles. Promotion
    workers keep theirs for the engine's lifetime, so their contexts are
    created **without their own LLVM thread pool**: multithreading off, or
    one engine-owned `llvm::ThreadPool` shared through
    `MLIRContext::setThreadPool` (which requires a context constructed
    with threading disabled) and sized by
    `engine.tiered.promotionThreads` ([11 §4.1](11-tiered-compilation.md)).
    Today `mlir.enableMultithreading` defaults to `true`
    (`MLIRCompilationBackend.cpp`), which would give each worker a pool of
    `hardware_concurrency` threads. Contexts reused on **caller threads**
    (synchronous MLIR compiles, `engine.backend=mlir`) outlive any one
    engine, so they never take an engine-owned pool: they run with
    multithreading off or share one process-wide pool;
  - the module verified once (`mlir.verifyEachPass` for debugging);
  - `LLJIT` given the data layout.
- **Input is NIR:** blocks and terminators map to `cf`, arithmetic to
  `arith`, extension types through the plugin type mapping.
- **Optimization level:** keep today's options. A size-dependent default
  (O2 below a threshold) is an optional tuning, measured with the corpus.
- **Persisted native payloads** for artifacts are produced here
  ([07](07-serialization.md)): one relocatable object per payload, with the
  import table of #527. How the object refers to everything outside it is
  §5.2; which CPU it targets is §5.1.
- **The `inlining` plugin's LLVM hooks** stay MLIR-only and in-process. A
  module whose compile used them carries process addresses as symbol names
  (`ptrToHex`), so its payload is non-persistable (the artifact carries
  only the NIR image) unless every renamed symbol goes through the import
  table as a `Portable` pointer name; the hash of the inlined bitcode then
  joins the descriptor and the cache keys ([07 §2](07-serialization.md),
  [04 §4](04-extensions.md)).

### 5.1 Target CPU

Today the backend builds its target machine with
`JITTargetMachineBuilder::detectHost()` and stamps `target-cpu`,
`target-features` and `tune-cpu` of the host on every function
(`LLVMIROptimizer.cpp`, `jit/MLIRJit.cpp`). A payload built on an AVX-512
host would fault with SIGILL on an AVX2 host.

- **Option `engine.mlir.targetCpu`** (code-affecting): `host` (default,
  today's behavior), `generic` (the baseline of the target triple, for
  payloads that are shipped to other machines), or an LLVM CPU name
  (M1.4a lists it as code-affecting).
- **The target CPU name and the final feature string** are part of the
  native payload's compatibility descriptor
  ([07 §2, §2.2](07-serialization.md)), of the L1 compatibility identity and of
  the L2 key ([08](08-caching.md)).
- **A payload is executed only if its CPU and features match the host**
  (its feature set is a subset of the host's). On a mismatch the module is
  recompiled from its NIR image; a mismatched payload is never executed.

### 5.2 Native payload linkage

[07 §2.1](07-serialization.md) defines the linkage of a native payload:
the undefined-symbol classes `__nautilus_{ptr,bind,data,stub}_<i>`,
`__nautilus_intr_<i>` (intrinsic fallbacks, below), the `nautilus_rt_*`
runtime helpers and **system symbols** (the libcalls LLVM emits by name,
such as `memcpy`, `_Unwind_Resume`, `__stack_chk_fail`, `__divti3`,
accepted only from a fixed allowlist in `libnautilus`);
position-independent code with the small code model; the accepted
relocations (x86-64 ELF in 0.4). A payload carries **no TLS** and **no
`.debug_*` sections**: debug info is dropped before the payload is stored,
so DWARF and GDB registration (§7) apply to in-process compiles only. This
section lists only what the MLIR backend changes to produce it.

- **Today's lowering embeds addresses:** `funcaddr` of an external function
  is an `i64` constant and an `inttoptr` (`MLIRLoweringProvider.cpp`), and
  external names are mapped with `absoluteSymbols` (`jit/MLIRJit.cpp`).
- **In 0.4, for every module that may be persisted** (an L1 or L2 cache
  is configured), the lowering emits every `pointers`,
  `bindings` and data-object reference, every `funcaddr` stub and every
  runtime helper (capture thunk, `nautilus_rt_throw_arith` of §3.1, the
  personality bridge) as the external symbols of 07 §2.1, declared without
  `dso_local`, so that PIC code reaches them through the GOT and PLT
  (their addresses lie anywhere in the address space, beyond the ±2 GiB
  of a `PC32`). In-process compiles whose result is never stored keep the
  immediates; `saveModule` of such a module writes the NIR image without
  that payload.
- **ORC/JITLink as used today fits this.** `MLIRJit` already links through
  `ObjectLinkingLayer` (JITLink, chosen for its EH-frame handling), which
  builds GOT and PLT entries for ELF x86-64 and AArch64 and Mach-O arm64.
  The changes: the `JITTargetMachineBuilder` (today `detectHost()`, then
  `engine.mlir.targetCpu`, §5.1) gets `setRelocationModel(Reloc::PIC_)`
  and `setCodeModel(CodeModel::Small)` for persistable modules; the object
  is captured before linking through an `ObjectTransformLayer` transform
  (no second compile); and at load, the `__nautilus_*` names are defined
  by the same `absoluteSymbols` mechanism that maps external names today,
  from the image's tables instead of from the IR. LLJIT keeps zero compile
  threads (no ORC thread pool).
- **Intrinsic callees** without an inline lowering
  ([13 §4.6.1](13-nir-spec.md)) are referenced as
  `__nautilus_intr_<i>` (*i* = the intrinsic's `extensions` entry),
  resolved at load to the native fallback registered under that
  `name@version` (07 §2.1). Inline lowerings
  need no symbol.

## 6. `tbc`, `cpp`, GPU, and removing `bc`

- `tbc` lowers NIR almost one to one. Its register allocator works on value
  ids.
- **Extension ops on `tbc`:** one bytecode opcode `EXT` (`a` = result,
  `b` = index of a per-site `ExtSite` record, as `CALL_EXT` uses a
  call-site record) calls an out-of-line handler
  `void(*)(uint64_t* fp, const ExtSite*)` taken from the
  extension-lowering table ([04 §3.5](04-extensions.md)); without one, the
  lowering emits `CALL_EXT` of the native fallback in the pointer
  convention of [04 §3.3](04-extensions.md). It works in all three
  dispatch skins (tail call, computed goto, switch), which a plugin could
  not extend, and in `tbc.mode=jit` through one generic `EXT` stencil
  that, like `stencil_CALL_EXT`, takes the site from a `_JIT_SITE` hole;
  extension ops never throw, so it needs no unwind exit. Plugins never add
  dispatch-table entries or stencils. Extension-typed values live in the
  frame's alloca area at their size and alignment, which needs the
  16-byte alloca alignment limit of today's lowering lifted (M6.5).
- **Stencils:** every new or changed opcode (`EXT`, `data.addr`,
  `funcaddr`, the checked arithmetic and saturating casts of §3.1) needs
  regenerated copy-and-patch stencils for all three targets
  (`jit/generated/stencils_{x86_64_elf,aarch64_elf,arm64_macho}.inc`,
  `tools/stencils/build_stencils.py`, Clang ≥ 19). They are regenerated in
  their own commit in M4.1 and again in M6.1, and a CI check verifies that
  the checked-in tables match the generator's output.
- `tbc` and `cpp` throw from checked arithmetic as in §3.1; `tbc` keeps
  using `third_party/dyncall` for outgoing external calls.
- `cpp` prints from NIR. Extension ops use expression templates or calls.
- GPU backends read NIR from M7 on; until then the GPU plugin is off. The
  design is in [17-gpu.md](17-gpu.md).
- **Every port is built next to the 0.3 lowering** it replaces and tested
  with `nautilus-nir-run` (a `.nir` file on one backend) until the cut-over,
  which deletes the 0.3 lowerings. `tbc` is ported first: the cut-over needs
  only `tbc` ([16 §2](16-implementation-plan.md)).

### 6.1 Removing `bc` (D6)

`tbc` covers everything `bc` is used for: it interprets without generating
code (iOS-safe with `ENABLE_FFI_CLOSURES`), uses the same `NativeClosure`
entry points, and adds the copy-and-patch mode. `bc` shares no sources with
`tbc`, so removing it is self-contained. It is removed in **M1**, before the
NIR port, so it is never ported, and M4 has one backend fewer.

| What | Change |
|---|---|
| `backends/bc/` (interpreter, lowering, its dyncall wrapper) | Deleted |
| `ENABLE_BC_BACKEND` | Removed; `ENABLE_FFI_CLOSURES` and `NativeClosure.cpp` depend on `ENABLE_TBC_BACKEND` only. A configure-time message reports the option as obsolete if it is set |
| `engine.backend=bc`, `engine.tier0.backend=bc` | Error naming `tbc` as the replacement. No silent alias: `bc`-specific options and dumps do not carry over |
| `dump.after_bc_generation` | Removed; `tbc` has its own dump |
| Tier-0 fallback in `TieredJITCompiler` | `asmjit` → `tbc` → interpreter (today `asmjit` → `bc` → interpreter) |
| Tests (`TieredCompilationTest`, `CompilationStatisticsTest`, `InvokeExceptionExecutionTest`, plugin tests) | `bc` cases move to `tbc`, so the code paths they cover stay tested |
| `example/src/DemoBackends.cpp`, `docs/engine.md`, `.github/nebulastream/benchmark.toml` | `bc` entries removed |

Before removal, M1 checks that `tbc` passes every test `bc` passes, and is no
slower on `ExecutionBenchmark`, so no user loses a working configuration.
P0 measurements of `bc` ([p0-baseline.md](p0-baseline.md)) stay as recorded;
`tbc` and `asmjit` are the baselines from M1 on.

## 7. Debug info and profiling (D14)

0.3 has three ways to see generated code: DWARF debug info and GDB JIT
registration from MLIR (`DebugInfoExecutionTest`), perf jitdump
(`PerfJitDumpExecutionTest`), and the in-process `JitSymbolRegistry` behind
`perf.sample` and the `profiling` plugin. They need source locations and
names, which 0.4 carries in NIR:

- **Source locations** live in the optional `debug` section
  ([02 §3.2](02-nir.md)): a source tag per instruction and the region
  table. They are recorded only when a debug or profiling option is set
  (`dump.*`, `perf`, `perf.sample`, debug info), so the default compile
  pays nothing ([01 §5](01-tracing-frontend.md)).
- **MLIR** emits DWARF locations from the `debug` section, as it does from
  today's IR. 0.3's `IRLocationPass` (operation → line of the IR dump) is
  replaced by a side output of the NIR printer, built in M4.5 (MLIR DWARF)
  and M4.8 (jitdump, `perf.sample`); it is not a pass.
- **`asmjit` and `tbc` register their code too.** In 0.3, `perf.sample`
  needs `engine.backend=mlir`, because tier-0 backends register nothing.
  In 0.4, `asmjit` publishes one symbol per function (and per `region()`
  when source locations are on) to `JitSymbolRegistry` and to the jitdump,
  and `tbc` publishes its handler ranges. Profiling then works with the
  default backend.
- **Per-operation attribution on `asmjit`.** With source locations on,
  `asmjit` also publishes a line table (code offset → NIR instruction and
  its source tag), which `JitSymbolRegistry::resolveLine` serves, so the
  `profiling` plugin's per-operation reports (`SampleAggregation.cpp`) work on
  `asmjit` as they do on MLIR today.
- **Tests:** the existing debug-info and jitdump tests run on NIR, plus a
  `perf.sample` test on `asmjit` and a per-operation attribution test on
  `asmjit` (M4.8, [12](12-validation.md)).

## 8. Not in 0.4

- Per-tier IR levels and more than two tiers
  ([11 §9](11-tiered-compilation.md)).
- Lazy per-function compilation and parallel compilation of a module's
  functions.
- A custom register allocator, a second AsmJit mode, a new native backend,
  and any TPDE backend, adapter or TPDE-based middle tier.

## 9. Exit criteria (M4)

- Every backend except GPU (M7) passes the execution tests and the
  differential fuzzer on NIR, including the defined arithmetic of §3.1
  (fuzzer cases for division by zero, `MIN / -1`, oversized shifts and
  out-of-range casts).
- `tbc` stencils regenerated for all three targets; the freshness check
  runs in CI.
- `asmjit` meets the targets of §4.3.
- End to end ≤ 0.75 µs/op on linear corpus code with the default backend
  (README §1).
- No execution-time regression per backend on `ExecutionBenchmark` and
  TPC-H, `asmjit` included.
- Debug info, jitdump and `perf.sample` work on NIR, `perf.sample` also
  with `asmjit` (§7).
- Decision D3 taken on measured results: **`asmjit` becomes the tier-0 and
  single-tier default on x86-64 and AArch64 if it meets every target of
  §4.3 and the end-to-end gate above; otherwise `tbc` is the default** and
  `asmjit` stays selectable.

## 10. Risks

- **Other superlinear paths in our AsmJit patches.** The call-heavy case was
  one. The corpus (`calls`, `straight`, `loopBody` at 10k+ ops) runs in CI
  and guards against the next one.
- **AsmJit's allocator stays a fixed cost** (~0.17 µs/op on linear code).
  If the frontend and conversion work makes it dominant, a custom allocator
  becomes the next step, decided on TPC-H and NebulaStream data.
- **The plugin lowering API is public surface.** Keep it narrow (operands,
  results, scratch registers, emitter).
