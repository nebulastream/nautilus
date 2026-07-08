# TBC — Threaded Bytecode Interpreter Backend

`tbc` is a from-scratch bytecode interpreter backend built for high execution
performance and portability. It runs on x86-64 and ARM64 (Linux/macOS) and —
unlike the legacy `bc` backend — **never allocates executable memory**, so it
also works on platforms that forbid runtime code generation (iOS).

Select it with `options.setOption("engine.backend", "tbc")`. It is built when
`ENABLE_TBC_BACKEND` is on (default, dependent on `ENABLE_TRACING`).

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
  marshals arguments + call in one dispatch, through one of two mechanisms
  selected by `tbc.externalCall = auto | dyncall | thunks`:
  - **dyncall** (default when built, `ENABLE_TBC_DYNCALL=ON`): drives the
    outgoing dyncall VM, which builds call frames dynamically without
    executable memory. Covers any signature.
  - **typed thunks** (`TBCThunkCall.hpp`, always compiled, zero
    dependencies): each external site is bound at backend-compile time to one
    of ~1100 pre-compiled template callers keyed on (return kind × integer-arg
    count × f32/f64 pattern). Integer-class arguments travel as extended
    `uint64_t` reordered ahead of the fp arguments — ABI-sound on x86-64 SysV
    and AArch64 (incl. Apple) because the two argument classes are
    register-assigned independently, with caps (≤ 8 integer-class, ≤ 4
    fp-class args) that keep every argument out of ABI-visible stack-layout
    differences. Over-cap or off-platform sites degrade per-site to dyncall;
    without dyncall in the build they are rejected at compile time with a
    clear error. `auto` resolves to thunks when dyncall is not built.
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

Other options: `tbc.dispatch` (see above), `tbc.externalCall` (see Calls;
default `auto`), `tbc.stackSizeKb` (default 1024), `tbc.registerAllocator`
(default true).

## iOS support

iOS forbids runtime code generation (no writable+executable pages), which
rules out the MLIR / C++ / asmjit backends and `bc`'s dyncallback
trampolines. `tbc` is built to run there:

- **No executable memory anywhere**: bytecode is interpreted in place;
  escaping internal function pointers come from the pre-compiled trampoline
  pool (`TBCTrampoline.hpp`).
- **Dispatch**: the strongest skin, tail-call threading, is Clang-based and
  works on iOS; `tbc.dispatch = auto` selects it there.
- **Outgoing external calls**: dyncall's forward-call VM is itself iOS-safe
  (it builds call frames on a data stack and ships an Apple arm64 ABI
  implementation), so a default build works. To avoid cross-compiling
  dyncall's per-target assembly — and to drop tbc's last third-party
  dependency altogether — configure with `-DENABLE_TBC_DYNCALL=OFF`: external
  calls then marshal through the built-in typed thunks (see Calls), subject
  to their signature caps.

## Testing

- The whole execution/val/tracing suite runs on `tbc` via
  `test/common/ExecutionTest.hpp::availableBackends()`.
- `test/execution-tests/TBCDispatchModeTest.cpp` pins every dispatch skin ×
  lowering-option combination against a de-optimized switch reference.
- The differential fuzz harness (`test/fuzz/Harness.hpp`) includes `tbc`,
  continuously cross-checking it against the interpreter and all other
  backends.
- `test/benchmark/{Execution,Tracing}Benchmark.cpp` include `tbc`.

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
- ~~**Zero-dependency typed call thunks**~~ — done (`TBCThunkCall.{hpp,cpp}`):
  outgoing external calls can marshal through pre-compiled template callers
  bound per call site at backend-compile time (`tbc.externalCall = thunks`,
  automatic when dyncall is not built). Caps: ≤ 8 integer-class and ≤ 4
  fp-class arguments, x86-64 SysV / AArch64 only (the Apple arm64
  stack-argument packing rules are sidestepped by keeping every reordered
  argument in registers — or, for integer args 7-8 on SysV x86-64, in
  8-byte-rounded stack slots). Over-cap sites fall back per-site to dyncall
  when it is built. `-DENABLE_TBC_DYNCALL=OFF` drops the dyncall link
  entirely, removing tbc's last third-party dependency; over-cap signatures
  are then rejected at backend-compile time with a clear error.
- **Float-signature trampolines**: extend the escaping-function-pointer pool
  beyond integer-class signatures if a use case appears.
- **Instruction-stream prefetch hints / handler layout experiments** once
  real query workloads are profiled.
