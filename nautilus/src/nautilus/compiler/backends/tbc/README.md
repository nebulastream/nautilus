# TBC — Threaded Bytecode Interpreter Backend

`tbc` is a from-scratch bytecode interpreter backend built for high execution
performance and portability. It runs on x86-64 and ARM64 (Linux/macOS) and —
unlike the legacy `bc` backend — **never allocates executable memory** in its
default mode, so it also works on platforms that forbid runtime code
generation (iOS).

Select it with `options.setOption("engine.backend", "tbc")`. It is built when
`ENABLE_TBC_BACKEND` is on (default, dependent on `ENABLE_TRACING`).

On top of the interpreter, tbc has an opt-in **copy-and-patch JIT** execution
mode (`tbc.mode = jit`, see [jit/](#copy-and-patch-jit-tbcmode--jit)) that
stitches pre-compiled per-opcode machine-code stencils into executable memory,
eliminating dispatch entirely — ~3–4× faster than the interpreter on loop
kernels while reusing the same bytecode, frames, and lowering.

## Why a second bytecode backend?

The `bc` backend's default configuration dispatches every instruction through
an indirect function-pointer call, routes *every* call — including
Nautilus-to-Nautilus calls in the same module — through the dyncall FFI plus a
dyncallback trampoline into a nested interpreter, copies its whole register
file on every invocation, and heap-allocates and re-zeroes alloca buffers per
call. Its fast paths (threaded dispatch, superinstructions, immediates) are
opt-in. dyncallback also allocates runtime-executable trampolines, which makes
the backend unusable on iOS.

`tbc` starts from the state of the art instead of retrofitting it:

| | bc (default) | tbc (default) |
|---|---|---|
| dispatch | indirect call per op | tail-call threading (`musttail`) / computed goto |
| code layout | per-block vectors + `std::variant` terminators | one flat stream, fixed 8-byte words |
| internal calls | dyncall → dyncallback thunk → nested interpreter | native `CALL`/`RET` in the dispatch loop |
| frames | full register-file copy, heap allocas | contiguous VM stack, in-frame allocas |
| fusion / immediates | opt-in, threaded mode only | on by default |
| executable memory | dyncallback trampolines | none |

Measured on the loop kernels of `test/benchmark/ExecutionBenchmark.cpp`
(Release, x86-64): ~3–4× faster than `bc`'s default *and* best opt-in
configurations; per-call entry ~1.7× faster.

## Design

### Instruction encoding (`TBCOpcodes.hpp`, `TBCCode.hpp`)

One flat `std::vector<Instr>` per function. `Instr` is a fixed 8-byte word:

```
struct Instr { uint16_t op, a, b, c; };   // a = dst, b/c = srcs | imm16
```

- Branch targets are signed 32-bit *relative* instruction offsets packed as
  `b | c<<16` (`JMP`, `CJMP`) or in the second word of 2-word forms (fused
  compare-and-branch, `SELECT`), so dispatch needs only the instruction
  pointer — no code-base register.
- Opcodes are fully type-monomorphized (no runtime type tags). Numeric
  families expand over a canonical 10-type order via X-macros; the lowering
  computes `family base + type index` instead of per-type switches, and
  static_asserts pin the layout.
- Every consumer of the opcode list — the `Op` enum, all three dispatch
  skins, and the disassembler name table — expands the same X-macros, so they
  cannot drift.

### Dispatch (`TBCInterpreter.cpp`)

Three skins, selected via `tbc.dispatch = auto | tailcall | goto | switch`
(default `auto` = strongest supported; unavailable requests degrade silently):

1. **Tail-call threading** (Clang, incl. iOS): each opcode is its own handler
   function with the shared signature
   `uint64_t(const Instr* ip, uint64_t* fp, VMContext* ctx)`; every handler
   ends in `[[clang::musttail]] return kDispatchTable[ip->op](ip, fp, ctx)`.
   The three hot values stay pinned in argument registers, each opcode gets
   its own BTB-friendly indirect-branch site, and the compiler optimizes each
   handler in isolation (the wasm3 / upb / CPython 3.14 technique).
2. **Token-threaded computed goto** (GCC/Clang): `goto *labels[ip->op]`.
3. **Portable `for(;;) switch`** (any compiler).

### Values and frames

Register slots are untyped 64 bits with the same normalization convention as
`bc`: bools stored 0/1, narrow integers zero-extended, floats/pointers
bit-exact. Consumers always read their exact declared width, so upper-bit
garbage (e.g. from native callers of trampolines) is unobservable.

Frames live on one contiguous per-thread `uint64_t` stack:

```
base[0..2]                      caller fp, return ip, caller dst register
fp[0 .. regSlots)               value + constant registers   (fp = base + 3)
fp[regSlots .. frameSlots-3)    alloca area (16-aligned, zeroed per call)
```

Frame setup is one bounds check + one `memcpy` of the function's *constant
image* (constants live in pinned register slots) + alloca pointer fixups.
The stack is reentrancy-safe: an external call that calls back into TBC code
simply pushes further frames at `sp`.

### Calls

- **Internal** (`ProxyCall` to a function in the same module): resolved at
  lowering to a function index; the `CALL` handler pushes a frame, copies the
  argument registers, and continues in the same dispatch loop. `RET` writes
  the result into the caller's destination register and pops. Entry frames
  return to a static `HALT` instruction. No FFI, no C++ recursion.
- **External** (`invoke`): a single `CALL_EXT` instruction referencing a
  per-call-site record `{target, argRegs, argTypes, returnType}`; one handler
  drives the outgoing dyncall VM (arguments + call in one dispatch). Outgoing
  dyncall builds call frames dynamically without executable memory.
- **Indirect** (`CALL_IND`): the target register always holds a real native
  pointer (see trampolines below), so it uses the external path.

### Escaping internal function pointers (`TBCTrampoline.hpp`)

`FunctionAddressOf` of an internal function must produce something native
code can call (e.g. `invoke(applyFnPtr, nautilusAdd.getFuncPtr(), …)`). `bc`
fabricates a dyncallback thunk at runtime; `tbc` instead binds one of a fixed
pool of **pre-compiled template trampolines** — 64 slots × arities 0–8, the
slot index baked in as a template argument — that forward their integer-class
arguments into the interpreter. Zero runtime code generation. Float-valued
signatures throw a clear `NotImplementedException` (no current usage).

### Entry path (`TBCExecutable.cpp`)

`hasInvocableFunctionPtr()` is `false`; callers go through the
`GenericInvocable` machinery in `include/nautilus/Executable.hpp`. The hot
route is the allocation-free `invokeRaw` fast path (raw 64-bit slots on the
stack); the boxed `std::any` route remains as fallback and for exotic types.

### Lowering (`TBCLoweringProvider.cpp`)

Ports the proven structure of `BCLoweringProvider` (block-argument edges as
parallel copies, per-arm landing pads for `IfOp`, free-list linear register
allocation with global use counts, merge-parameter register reuse — issue
#321) and then **flattens** the block-structured code into the final stream:
jumps to the next block in emission order fall through, and branch offsets
are patched once all block positions are known.

Optimizations are **on by default** (flags exist only for A/B benchmarking):

- `tbc.superinstructions` — a single-use comparison feeding a conditional
  branch fuses into one 2-word compare-and-branch instruction.
- `tbc.immediates` — small constant right operands of i32/i64 add/sub/mul
  fold into the 16-bit immediate field; small integer constants materialize
  via `MOV_imm` without occupying a constant slot at all.
- `tbc.coalescing` — block-edge parallel copies emit the minimum number of
  MOVs (one temp only for a genuine permutation cycle).

Other options: `tbc.dispatch` (see above), `tbc.stackSizeKb` (default 1024),
`tbc.registerAllocator` (default true).

## Testing

- The whole execution/val/tracing suite runs on `tbc` via
  `test/common/ExecutionTest.hpp::availableBackends()`.
- `test/execution-tests/TBCDispatchModeTest.cpp` pins every dispatch skin ×
  lowering-option combination against a de-optimized switch reference.
- The differential fuzz harness (`test/fuzz/Harness.hpp`) includes `tbc`,
  continuously cross-checking it against the interpreter and all other
  backends.
- `test/benchmark/{Execution,Tracing}Benchmark.cpp` include `tbc`.

## Copy-and-patch JIT (`tbc.mode = jit`)

The `jit/` subdirectory implements a copy-and-patch JIT (Xu & Kjølstad 2021;
the CPython 3.13+ technique) as an execution mode over the exact same
bytecode, frames, calling convention, and lowering as the interpreter.

At **build** time, every opcode handler is compiled offline into a machine-code
*stencil* with *holes*: relocations against magic `_JIT_*` symbols standing in
for the operand fields and the continuation (`tools/stencils/build_stencils.py`
→ `jit/generated/stencils_<arch>_<fmt>.inc`, checked in; regenerate with the
`tbc-regen-stencils` target, clang ≥ 19 required only for regeneration). The
stencil bodies share the interpreter's semantics 1:1 and are generated from
the same X-macro lists in `TBCOpcodes.hpp`.

At **compile** time (last step of `TBCBackend::compile`), the stitcher
(`jit/TBCStitcher.cpp`) lays out one stencil per instruction in a single
executable span (asmjit `JitAllocator`/`VirtMem` own W^X, MAP_JIT, and icache
flushing), then patches each hole: register fields become frame byte-offsets
baked into the addressing modes, 16-bit immediates become sign-extended
64-bit constants, branch targets become direct jumps to the target
instruction's stitched code. Trailing continue-jumps are elided on
fall-through, and x86-64 `movabs+jmp` pairs relax to direct `jmp rel32` —
after which the interpreter's per-instruction dispatch (indirect branch +
`Instr` load + operand decode) is simply gone.

- **Modes**: `tbc.mode = interp` (default) | `jit` (strict: throws when this
  build cannot execute stitched code, so tests never silently run the wrong
  engine) | `auto` (jit when available, interpreter otherwise). Availability
  requires a generated stencil table for the target, a Clang 19+ host build
  (the entry shim calls through a `preserve_none` pointer), and no ASan.
- **Calls**: internal `CALL` pushes the same VM frame via a plain-ABI helper
  and tail-jumps into the callee's stitched entry; `RET` pops inline and
  tail-jumps to the native continuation stored in the frame header.
  `CALL_EXT`/`CALL_IND` drive the same dyncall path as the interpreter
  through a helper. Trampolines and reentrancy work unchanged (both modes
  share the VM stack; `invoke()` dispatches per program).
- **Exceptions**: stitched code carries no unwind info, so helpers catch into
  `VMContext::pendingException`, exit through the `@UNWIND` stencil, and the
  entry shim rethrows. (Exceptions thrown by `invoke()` targets terminate in
  dyncall's assembly on *every* mode, interpreter included — pre-existing.)
- **Safety**: stencil tables resolve by opcode *name*; a stale table makes
  stitching bail out to the interpreter (module-level), never mis-execute.
  The extractor hard-fails at generation time on any unexpected relocation,
  symbol, or section.
- **Targets**: x86-64 ELF (single `R_X86_64_64` patch kind), aarch64 ELF
  (movz/movk chains, direct `b` imm26, in-span thunks for helper `bl`s),
  arm64 Mach-O (Apple codegen GOT-loads every symbol; the stitcher emulates
  the GOT with per-instruction value slots inside the span).
- **Measured** (Release, x86-64, vs the tail-call interpreter on identical
  bytecode): fibonacci 3.1×, array-sum 4.3×, internal-call loop 1.9×,
  external-call loop 1.4×, per-call entry parity
  (`exec_tbc_*_{interp,jit}` in `ExecutionBenchmark.cpp`).
- **Testing**: the whole execution/val/tracing suite runs on stitched code
  via the `"tbc-jit"` pseudo-backend (`ExecutionTest.hpp`), the differential
  fuzzer includes it as a peer, `TBCJitModeTest.cpp` pins jit against a
  de-optimized interpreter reference across lowering-option combinations,
  and `TBCJitWhiteBoxTest.cpp` covers the exception bridge and stale-table
  bail-out on hand-authored programs.

## Follow-ups / future work

- **`invokeRaw`-style typed entry for tiered compilation**: `tbc` is a
  natural `engine.tier0.backend`; measure and tune the tier-0 handoff.
- ~~**`[[clang::preserve_none]]`** on the tail-call handlers~~ — done:
  applied to the `Handler` type and every tailcall-skin handler in
  `TBCInterpreter.cpp`, guarded by `__has_attribute(preserve_none)` (Clang
  19+; silently a no-op on older Clang) and disabled under ASan, which
  cannot combine dynamic stack realignment with this calling convention
  (`llvm/llvm-project#95928`). Measured impact is narrower than hoped: most
  opcode handlers (arithmetic/compare/cast/branch — the bulk of the
  dispatch table) already fit within the default caller-saved register
  budget, so their generated code is byte-identical either way — only the
  physical registers holding `ip`/`fp` differ. The benefit is real but
  confined to handlers that call a plain-ABI helper across the tail-call
  boundary: `CALL` (internal Nautilus-to-Nautilus calls, via `pushFrame`)
  measurably drops from 7 to 1 register spill/reload pair and runs
  ~4% faster in isolation; `CALL_EXT`/`CALL_IND` see the same codegen
  improvement but no measurable wall-clock gain, since outgoing dyncall's
  own FFI marshalling overhead dominates. `exec_tbc_add/fibonacci/sum` in
  `ExecutionBenchmark.cpp` are pure arithmetic/loop kernels with no internal
  or external calls, so they can't exercise the one path that benefits —
  don't read "flat" there as the change being ineffective.
- ~~**Memory-offset superinstructions** (`LOAD_off` / `STORE_off`)~~ — done:
  the flattener now fuses a single-use `ptr + const` immediately preceding a
  `LOAD`/`STORE` into one instruction, once the add's own constant has been
  immediate-folded (pointer arithmetic previously fell outside immediate
  folding's `i32`/`i64` type check even though it emits the identical
  `ADD_i64`; that check now also accepts `Type::ptr`). Detection matches the
  existing compare-and-branch fusion's pattern: single-use (via the
  lowering's existing use-count map) and adjacency in the emitted stream (so
  nothing could have reused the base pointer's register in between) are both
  required before fusing. Only `ptr + const` is handled, matching the exact
  gap described above — `ptr - const` is not (would need `SUB_imm_i64`
  matched the same way). Offsets beyond `int16_t` (`±32768`) fall back to a
  plain `LOAD`/`STORE` fed by an unfused `ADD`, same as any other constant
  too large to fold.
- ~~**Dead-MOV elimination after immediate folding**~~ — done, and it now
  generalizes to more than `MOV_imm`: the flattener tracks, per block, a set
  of "drop this instruction" indices (originally just the trailing compare
  in a fused branch) and any immediate-folded constant that was materialized
  by a now-single-use `MOV_imm` adds its site to that set. Memory-offset
  fusion above reuses the same set to drop the fused-away `ADD_imm_i64`, so
  a `ptr + const` feeding a `LOAD`/`STORE` collapses from three instructions
  (`MOV_imm`, `ADD`, `LOAD`/`STORE`) to one.
- **Zero-dependency typed call thunks**: replace outgoing dyncall with
  template-instantiated callers selected at lowering time (signatures are
  statically known). Requires a register-only argument cap and care with the
  Apple arm64 stack-argument packing rules; would remove the last third-party
  dependency. Would speed up `CALL_EXT` in both execution modes (the JIT's
  external-call benchmark shows dyncall marshalling dominating that path).
- **Float-signature trampolines**: extend the escaping-function-pointer pool
  beyond integer-class signatures if a use case appears.
- **Instruction-stream prefetch hints / handler layout experiments** once
  real query workloads are profiled.

### Copy-and-patch JIT follow-ups

- **Tiering measurement**: stitching is a linear pass over the bytecode, so
  `tbc` + `tbc.mode=jit` is a candidate for *both* tiers — measure tier-0
  latency (stitch time vs. lowering time) and the handoff against the
  current `asmjit`/`bc` tier-0 defaults before changing any default.
- **Profiling/debugging story for stitched code**: emit a perf map
  (`/tmp/perf-<pid>.map`) and/or the GDB JIT interface behind an option;
  until then stitched frames show as raw addresses and `tbc.mode=interp` is
  the debugger.
- **macOS execution validation**: the macos-15 CI job uses Apple clang
  (no `preserve_none` before LLVM 19), so it only validates that the Mach-O
  stencil table compiles and that `auto` degrades cleanly; the GOT-emulation
  execution path needs a run under a non-Apple clang ≥ 19 on macOS before
  it can be considered exercised.
- **CI stencil freshness check**: regenerate the tables on a clang job and
  `git diff --exit-code` the `.inc` files, so opcode-list changes cannot
  silently ship stale tables (they degrade safely, but to interpreter
  speed).
- **Native-ABI entry stencils**: per-function entry shims with real C
  signatures would let `hasInvocableFunctionPtr()` return true and skip the
  `GenericInvocable` layer on the entry path; deferred because `invokeRaw`
  is already the allocation-free hot path and per-signature marshalling is
  asmjit-backend territory.
- **Windows/MSVC**: out of scope (COFF extraction, different calling
  conventions, no `musttail` guarantee); `tbc.mode=auto` degrades to the
  interpreter there by construction.
