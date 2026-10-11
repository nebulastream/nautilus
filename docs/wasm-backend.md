# WebAssembly Backend (design)

Status: design proposal. Nothing of this is implemented yet. The document describes how a
`wasm` compilation backend would lower Nautilus IR to a WebAssembly module, and in
particular how it would handle the three things that do not map one to one: the IR's
block-based control flow, memory access through host pointers, and calls into runtime
functions that live outside the generated code.

## Why a WebAssembly backend

Every existing backend produces something that runs in the process that traced the
function: machine code (MLIR, AsmJit), a shared library (C++), or bytecode for an
interpreter linked into the same binary (bc, tbc). A WebAssembly backend covers three
situations those cannot:

1. **Nautilus itself runs as WebAssembly.** Built with Emscripten, in a browser or on a
   host that only executes wasm, there is no native JIT at all. The MLIR backend cannot
   exist there, and the interpreting backends would be an interpreter inside an
   interpreter. Emitting a wasm module and instantiating it next to the host module is the
   only way to get compiled code.
2. **Compile here, run there.** A coordinator traces and compiles a query, a worker with
   a wasm runtime executes it. The artifact must not contain anything that only means
   something in the compiling process, which rules out every other backend's output.
3. **Sandboxed execution in a native process.** A wasm runtime gives memory isolation for
   free, so user-supplied Nautilus code (UDFs) can run without trusting it.

The three situations share one lowering but differ in how memory and host calls are
bound. The design therefore separates the **lowering** (IR to wasm, fixed) from two
**policies** (memory model, linkage) and a **hosting** layer (which runtime instantiates
the module). Section [Deployment modes](#deployment-modes) lists the combinations that
make sense; everything after it is written in terms of those policies.

## What the IR hands a backend

The relevant facts about Nautilus IR, with where to read them:

| Aspect | What the IR provides | Where |
|---|---|---|
| Types | `v, b, i8, i16, i32, i64, ui8, ui16, ui32, ui64, f32, f64, ptr`. No aggregates, no vectors in the core IR. | `include/nautilus/tracing/Types.hpp` |
| Functions | `FunctionOperation`: typed parameters, one result type, a block list whose first block is the entry, an alloca table (`AllocaSpec {size, align}`), an optional exception-region side table. | `ir/operations/FunctionOperation.hpp` |
| Control flow | `BasicBlock` with typed **block arguments** (phi replacement). Terminators: `BranchOperation` (one `BasicBlockInvocation` carrying arguments), `IfOperation` (two invocations plus a branch probability), `ReturnOperation`. No switch, no structured loop; loops are back edges. | `ir/blocks/`, `ir/operations/{Branch,If,Return}Operation.hpp` |
| Analyses | Dominator tree with reverse post-order (`Dominators`), natural-loop recognition (`LoopInfo`), predecessor lists (`rebuildPredecessorLists`). | `ir/passes/Dominators.hpp`, `ir/passes/LoopInfo.hpp`, `ir/util/ControlFlowUtil.hpp` |
| Values | SSA. Every op carries an `OperationIdentifier`; block arguments reuse the identifier of the value most commonly passed to them, so a plain pass-through edge often needs no copy. | `ir/operations/Operation.hpp` |
| Memory | `LoadOperation(address: ptr) -> T`, `StoreOperation(value, address)`, pointer arithmetic as `ADD ptr + ui64`, `ConstPtrOperation` holding a raw host address, `AllocaOperation(index)` naming a slot of the function's alloca table. | `ir/operations/{Load,Store,ConstPtr,Alloca}Operation.hpp` |
| Calls | `CallOperation` with a `FunctionId` into the module `FunctionTable`. A `FunctionTarget` is `Internal` (its `FunctionOperation` is the definition), `External` (native address plus signature and `FunctionAttributes`) or `Intrinsic` (External plus an `IntrinsicId` a backend may expand). `IndirectCallOperation` calls through an SSA pointer operand. `FunctionAddressOfOperation` yields the address of a table entry as a `ptr`. | `ir/FunctionTable.hpp`, `ir/IntrinsicRegistry.hpp`, `ir/operations/{Call,IndirectCall,FunctionAddressOf}Operation.hpp` |
| Exceptions | `ExceptionRegionPreparationPass` turns per-call destructor lists into shared landing-pad blocks in a side table; `CapturedExceptionTransport` answers, per call, whether it needs a pending check and which pad it jumps to. Potentially throwing calls carry a `captureThrowingCall` thunk address. | `ir/ExceptionRegion.hpp`, `backends/CapturedExceptionTransport.hpp`, `common/ExceptionTransport.hpp` |
| Backend contract | `CompilationBackend::compile(ir, dumpHandler, options, statistics) -> Executable`. An `Executable` exposes either real function pointers or a `GenericInvocable` with the allocation-free `invokeRaw` slot convention. `NativeClosure` manufactures a real C function pointer for a function that has no machine code. | `backends/CompilationBackend.hpp`, `include/nautilus/Executable.hpp`, `backends/NativeClosure.hpp` |

Two consequences shape the rest of the design:

- The IR is already SSA over a CFG with block arguments. That is exactly the input every
  wasm producer (LLVM, Binaryen, Go, Zig) structures, so the control-flow problem is a
  known one with published algorithms.
- Pointers in the IR are host pointers, and three of the operations embed them as
  constants or hand them to host code (`ConstPtr`, `Call` to an external address,
  `FunctionAddressOf`). Whether such a value means anything inside the wasm module is the
  memory-model and linkage question, and it is a policy, not a lowering detail.

## Deployment modes

### The two policy axes

**Memory model** decides what a `ptr` value is inside the module and how `load`/`store`
reach the bytes:

| Model | `ptr` inside the module | `load`/`store` | Host pointers | Sandbox |
|---|---|---|---|---|
| `identity` | The host address itself. The module's linear memory *is* the host address space. | Plain `i32.load`/`i64.load` | Valid as they are | None |
| `arena` | An offset into one linear memory the runtime owns. Host code allocates everything the compiled function touches inside that memory. | Plain `i32.load`/`i64.load`, bounds-checked by the runtime | Rebased (`p - base`) at every boundary crossing; a pointer outside the arena is an error | Yes |
| `proxy` | The host address itself, as an `i64`. | A host import per width (`nautilus_mem.load_i32(i64) -> i32`, ...) | Valid as they are | None, and slow |

`identity` is only possible where the module and the host share an address space: under
Emscripten, where host and generated module import the same `env.memory`, or with a native
runtime configured to run without bounds checks and with memory base 0 (see
[Hosting](#hosting-which-runtime-instantiates-the-module)). `arena` is the real sandbox and
the only model a native embedding can use safely, but it imposes a data-layout rule on the
engine: anything the compiled code dereferences, including pointers stored *inside* data
structures, must be an arena offset, not a native pointer. `proxy` imposes nothing, works
with every existing test unchanged, and exists for exactly that reason: it is the
correctness reference and the first milestone, not a production mode.

**Linkage** decides how a call to an external function names its target:

| Linkage | Import name | Resolvable by | Use |
|---|---|---|---|
| `address` | `nautilus_fn.f_<emission name>`; the binding table maps the import back to `FunctionTarget::getAddress()` | The compiling process only | In-process execution |
| `symbol` | `nautilus_rt.<mangled symbol>` (from `FunctionName::getMangled()`), intrinsics as `nautilus_intrinsic.<registry name>` | Any process that exports those symbols and registered the same intrinsics | Portable artifacts |

Under `symbol` linkage a callee whose mangled name is unknown (a static function `dladdr`
cannot name, a `+[]` lambda) is a compile error that names the call site; there is nothing
a worker could bind it to. `ConstPtrOperation` is likewise an error under `symbol` linkage
and under `arena` unless the constant lies inside the arena, because a host address is
meaningless on the other side.

### Hosting: which runtime instantiates the module

| Hosting | What runs the module | Memory models | Notes |
|---|---|---|---|
| `emscripten` | The engine that already runs Nautilus (browser, Node, any wasm host). The generated module is instantiated with the host module's `env.memory`, `env.__indirect_function_table` and `env.__stack_pointer`. | `identity` | Function pointers are table indices on both sides, so external calls need no imports at all (see [Runtime functions](#runtime-functions)). |
| `embedded` | A wasm runtime linked into the native process behind a `WasmRuntime` interface. Candidates: WAMR (small, interpreter + fast JIT + AOT, host-native registration with pointer rebasing), wasmtime (C API, Cranelift, memory64), wasm2c (compile the module to C and reuse the C++ backend's compiler driver). | `arena`, `proxy`; `identity` only with an unchecked runtime configuration | The first runtime to integrate; the suite runs here. |
| `none` | Nobody. `compile()` returns a codegen-only executable (like `GPUCodegenOnlyExecutable`) that only serves the module bytes and `.wat` text. | any | For the compile-here-run-there case and for CI jobs without a runtime. |

### Combinations worth building

| Scenario | Hosting | Memory | Linkage |
|---|---|---|---|
| Nautilus in the browser / wasm host | `emscripten` | `identity` | `address` (addresses are table indices, portable within the host module) |
| Test suite, differential fuzzing, debugging the lowering | `embedded` | `proxy` | `address` |
| Sandboxed UDFs in a native engine | `embedded` | `arena` | `address` |
| Query shipped to a wasm worker | `none` on the coordinator, `embedded` on the worker | `arena` | `symbol` |

## Architecture

```
IRGraph
  │  (IR passes as for the interpreting backends: Full optimization level,
  │   BlockArgumentPruning matters most, ExceptionRegionPreparation always)
  ▼
WasmLoweringProvider ──── per function ────▶ WasmStructurer   (CFG → block/loop/if tree)
  │  OperationDispatcher visitXxx hooks       MemoryModel      (ptr type, load/store, const ptr)
  │                                           HostBinding      (imports, intrinsic handlers)
  ▼
WasmModule  (types, imports, functions, table, memory, globals, exports, data, names, branch hints)
  │
  ├──▶ WasmEncoder ──▶ binary (.wasm)  and  text (.wat) for dump.wasm
  │
  ▼
WasmRuntime (emscripten | embedded | none) ──▶ instance per thread
  │
  ▼
WasmExecutable : Executable
      getGenericInvocable(name) → invokeRaw over the export
      getInvocableFunctionPtr(name) → NativeClosure (embedded) or table index (emscripten)
```

Proposed layout, following the tbc backend's conventions:

```
nautilus/src/nautilus/compiler/backends/wasm/
  README.md                       what tbc's README is for tbc
  WasmBackend.{hpp,cpp}           CompilationBackend; registered as "wasm"
  WasmLoweringProvider.{hpp,cpp}  IR → WasmModule, one OperationDispatcher like cpp/mlir
  WasmStructurer.{hpp,cpp}        CFG → structured tree (dispatcher + structured mode)
  WasmModule.hpp                  in-memory module (function bodies as an instruction vector)
  WasmEncoder.{hpp,cpp}           LEB128 binary writer and .wat printer, no dependency
  WasmMemoryModel.hpp             policy interface; Identity / Arena / Proxy
  WasmHostBinding.{hpp,cpp}       import table, native trampolines, capture thunks
  WasmIntrinsics.{hpp,cpp}        IntrinsicId → handler registry plugins fill
  WasmRuntime.hpp                 runtime interface
  runtime/{Wamr,Wasmtime,Emscripten,None}Runtime.cpp
  WasmExecutable.{hpp,cpp}
```

The binary format is small enough to encode by hand (eleven sections, LEB128 integers,
a few hundred lines) and doing so keeps the backend free of a code-generation dependency,
in the spirit of tbc. Binaryen would give a relooper and `wasm-opt` for the price of a
large C++ dependency; it is a reasonable *prototype* shortcut and an optional
post-optimizer, not the plan of record.

Build and options:

- `ENABLE_WASM_BACKEND` (dependent on `ENABLE_TRACING`), plus
  `NAUTILUS_WASM_RUNTIME=wamr|wasmtime|emscripten|none` selecting the hosting layer at
  build time (`emscripten` is implied when `CMAKE_SYSTEM_NAME` is Emscripten).
- Options, in the style of `docs/options.md`:

| Option | Default | Meaning |
|---|---|---|
| `engine.backend=wasm` | | Pins the backend. |
| `wasm.memoryModel=[identity,arena,proxy]` | `identity` under Emscripten, `arena` otherwise | See above. |
| `wasm.linkage=[address,symbol]` | `address` | See above. |
| `wasm.structurer=[dispatch,structured]` | `structured` | `dispatch` forces the universal `br_table` loop, for debugging and for A/B measurements. |
| `wasm.stackSizeKb` | `1024` | Shadow-stack size per thread (allocas), except under Emscripten where the host stack is used. |
| `wasm.branchHints=[true,false]` | `true` | Emit the `metadata.code.branch_hint` custom section from `IfOperation` probabilities. |
| `dump.wasm=[true,false]` | `false` | Writes the `.wat` text and the `.wasm` binary through the dump handler. |

## Control flow

### What WebAssembly offers

A wasm function body is a tree of `block`, `loop` and `if` constructs. A branch
(`br`, `br_if`, `br_table`) names an *enclosing* construct by nesting depth: branching to
a `block` jumps to its end, branching to a `loop` jumps to its start. There is no
`goto`, so a CFG with arbitrary edges must be rebuilt as nested constructs such that
every edge becomes a forward branch out of some block or a backward branch to some loop.
Any CFG can be expressed this way; a *reducible* CFG can be expressed without
duplicating code or introducing a dispatch variable.

Nautilus IR is a good fit for this constraint: it comes out of tracing structured C++
and the IR passes never create irreducible loops, so the structured translation applies
to every graph the backend will see in practice. The design still keeps a universal
fallback so the backend never has to reject a graph.

### Block arguments become locals

WebAssembly has no phi and no block parameters on `br` that are worth the trouble
(multi-value block signatures exist but force the values onto the operand stack at every
edge, which fights the loop form). The backend uses the same scheme as the C++ and
bytecode backends:

- Every SSA value, including every block argument, gets one wasm **local**, keyed by
  `OperationIdentifier`. Because the IR hands a block argument the identifier of the
  value usually passed to it, most pass-through edges need no copy at all; the
  `process(BasicBlockInvocation)` logic in `CPPLoweringProvider` already encodes this and
  carries over unchanged.
- An edge whose invocation arguments differ from the target's arguments is a **parallel
  copy**: read every source into a temp local, then write every target, or sequence the
  copies and break only genuine cycles with one temp as `TBCLoweringProvider` does under
  `tbc.coalescing`. The copies are emitted *before* the `br` that realises the edge.
- The runtime's compiler (Cranelift, LLVM, WAMR's JIT) rebuilds SSA from locals; the
  locals are not a performance cost of their own. Stack-scheduling single-use values
  defined in the same block (no local, leave the value on the operand stack) is a later
  size optimisation.

### Stage 1: the dispatcher loop

The first implementation structures nothing. Each block becomes one arm of a
`br_table` inside a `loop`, and every edge sets the next block index and branches back:

```wat
(local $next i32)
(loop $dispatch
  (block $b3 (block $b2 (block $b1 (block $b0
    local.get $next
    br_table $b0 $b1 $b2 $b3 $b3))
    ;; Block_0 body ... parallel copy for the edge, then
    i32.const 3  local.set $next  br $dispatch)
    ;; Block_1 body ...
  ...)
```

This is the flattening tbc's lowering already does, with `br $dispatch` instead of a
relative jump, and it handles every CFG including irreducible ones. An edge to the block
that is textually next can fall through without touching `$next`. The cost is that every
other edge is an indirect branch through `br_table`, which the runtime's optimizer cannot
turn back into straight-line loops reliably. It is the right first milestone because it
exercises every other part of the backend (values, memory, calls, the runtime, the
executable) with no risk in the structuring, and it stays as the `wasm.structurer=dispatch`
option and as the fallback for an irreducible graph.

### Stage 2: structured translation

The second stage produces real `loop`/`block`/`if` nesting from the dominator tree, after
Ramsey, *Beyond Relooper: Recursive Translation of Unstructured Control Flow to Structured
Control Flow* (ICFP 2022). The algorithm is small, provably correct for reducible graphs,
needs nothing the IR does not already have, and produces the same shapes LLVM's
`CFGStackify` does.

Inputs, all computed once per function:

- Reverse post-order (RPO) numbers and the dominator tree. `Dominators` provides both and
  requires `rebuildPredecessorLists` to have run, which the pass manager guarantees.
- Edge classification: an edge `u → v` is a **back edge** iff `v` dominates `u`.
- A block is a **loop header** iff it is the target of a back edge.
- A block is a **merge node** iff it has two or more incoming *forward* edges. Landing
  pads are forced into this class (see below).

Translation, with `context` a stack of frames (`BlockFollowedBy b`, `LoopHeadedBy b`,
`IfThenElse`) from innermost to outermost; the index of a frame in that stack is the
`br` depth that targets it:

```
doTree(x, context):
    children = merge nodes among x's dominator-tree children, sorted by RPO number descending
    if x is a loop header:
        emit loop; nodeWithin(x, children, LoopHeadedBy x :: context); emit end
    else:
        nodeWithin(x, children, context)

nodeWithin(x, [y, rest...], context):
    emit block; nodeWithin(x, rest, BlockFollowedBy y :: context); emit end
    doTree(y, context)                      ;; y's code follows the block that targets it
nodeWithin(x, [], context):
    lower x's non-terminator operations
    Return            → lower value, emit return
    Branch(y)         → doBranch(x, y, context)
    If(c, yt, yf)     → lower c; emit if; doBranch(x, yt, IfThenElse :: context)
                        emit else; doBranch(x, yf, IfThenElse :: context); emit end

doBranch(x, y, context):
    emit the parallel copy for the edge's invocation arguments
    if x → y is a back edge:   emit br (index of LoopHeadedBy y in context)
    else if y is a merge node: emit br (index of BlockFollowedBy y in context)
    else:                      doTree(y, context)      ;; y has one forward predecessor, inline it
```

Why it works, in one paragraph: a non-merge, non-header successor has exactly one
forward predecessor, which in a reducible graph dominates it, so its code can be placed
right at that branch. A merge node is dominated by some ancestor `x` and is placed after
`x`'s code, wrapped in a `block` that every branch to it can exit from; ordering
`x`'s merge children by descending RPO makes the outermost block end before the
node that has to come last. A loop header is wrapped in a `loop` whose body contains
everything it dominates, so every back edge is a `br` to an enclosing `loop`. Each
`if` adds one label level, which is why `IfThenElse` is pushed on the context.

Irreducible graphs (a back edge whose target does not dominate its source) are detected
while classifying edges; the function then falls back to the dispatcher. Node splitting
to make such a graph reducible is a possible later refinement, but the tracer does not
produce these graphs, so it is not needed to start.

### Landing pads and the exceptional exit

A potentially throwing call (`CapturedExceptionTransport::callNeedsCapture`) is lowered as
the call followed by a pending check and a branch to the call's landing pad
(`getPadIndexForCall`), mid-block rather than at a terminator:

```wat
call $import_f            ;; bound to the captureThrowingCall thunk, never to f itself
local.set $7
call $nautilus_eh.has_pending_exception
br_if $pad_2              ;; label of the pad's BlockFollowedBy frame
```

To make `$pad_2` a legal target the structurer adds the side table's pad blocks and one
synthetic exceptional-exit node to its view of the CFG, with an edge from every capturing
call site to its pad (or straight to the exit when the site has no pad) and from every
pad to the exit, and marks pads and the exit as merge nodes unconditionally. A merge node
is always reached by `br` to an enclosing `block`, which is exactly what a mid-block
`br_if` needs. The pad's body is its destructor calls, lowered as ordinary (non-capturing)
external calls; the exit restores the shadow stack and returns a zero of the function's
result type, after which `Executable::Invocable` rethrows the stored exception, as it does
for the C++, bytecode and AsmJit backends. The backend therefore reports
`ExceptionPropagationMode::CapturedHostRethrow`.

### Branch hints

`IfOperation::getProbability()` has no equivalent in core wasm, but the branch hinting
proposal (a `metadata.code.branch_hint` custom section keyed by instruction offset)
is implemented by the major engines and ignored by the rest. Emitting it costs the
encoder a map from `if`/`br_if` instructions to their byte offsets, which it has anyway
for the names section.

### Worked example

`test/data/loop-tests/ir/gcd.nautilus`:

```
Block_0($1:i32, $2:i32):            Block_1($2:i32, $1:i32):
    $3 = 0 :i32                         $7 = $1 % $2 :i32
    br Block_3($1, $2, $3)              $8 = 0 :i32
Block_3($1:i32, $2:i32, $3:i32):        br Block_3($2, $7, $8)
    $4 = $2 != $3 :bool             Block_2($1:i32):
    if $4 ? Block_1($2, $1) : Block_2($1)   return ($1) :i32
```

RPO is `Block_0, Block_3, Block_2, Block_1`; `Block_1 → Block_3` is the only back edge, so
`Block_3` is a loop header; no block has two forward predecessors, so there are no merge
nodes and no `block` is emitted at all:

```wat
(func $execute (param $1 i32) (param $2 i32) (result i32)
  (local $3 i32) (local $4 i32) (local $7 i32) (local $8 i32) (local $t i32)
  i32.const 0            local.set $3
  loop $Block_3
    local.get $2  local.get $3  i32.ne  local.set $4
    local.get $4
    if
      local.get $1  local.get $2  i32.rem_s  local.set $7
      i32.const 0   local.set $8
      ;; edge Block_1 → Block_3($2, $7, $8) rebinds ($1, $2, $3) := ($2, $7, $8):
      local.get $2  local.set $1
      local.get $7  local.set $2
      local.get $8  local.set $3
      br $Block_3
    else
      local.get $1
      return
    end
  end
  unreachable)
```

The `forBreak` graph in the same directory shows the other shape: `Block_6` has two
forward predecessors (`Block_3` and `Block_2`), so it is a merge child of `Block_5`'s
dominator subtree and gets a `block` that both arms `br` out of, after which `Block_6`'s
`return` follows the `end`.

## Values and types

WebAssembly has four value types. The mapping, and the normalisation rule that keeps
narrow integers correct:

| IR type | wasm type | Representation in a local |
|---|---|---|
| `b` | `i32` | `0` or `1` |
| `i8, i16, i32` | `i32` | sign-extended to 32 bits (canonical) |
| `ui8, ui16, ui32` | `i32` | zero-extended to 32 bits (canonical) |
| `i64, ui64` | `i64` | as is |
| `f32, f64` | `f32, f64` | as is |
| `ptr` | `MemoryModel::ptrType()`: `i32` (wasm32 arena, Emscripten) or `i64` (memory64 arena, identity-native, proxy) | address or offset per model |
| `v` | no value | |

Keeping narrow values canonical means comparisons, widening casts and signed shifts need
no fix-up; only operations that can leave the canonical range re-normalise their result:
`add`, `sub`, `mul`, `shl`, `neg`, `not`, and casts *into* a narrow type. The fix-up is
`i32.extend8_s` / `i32.extend16_s` for signed and `i32.and 0xff` / `0xffff` for unsigned
types; the sign-extension instructions are part of the wasm 2.0 baseline. This is the
same convention the interpreting backends use for their 64-bit register slots, so a value
crossing between wasm and the `invokeRaw` slot format needs no conversion.

| IR operation | wasm |
|---|---|
| `Add/Sub/Mul` | `iNN.add/sub/mul`, `fNN.add/sub/mul`, then normalise if narrow |
| `Div/Mod` | `iNN.div_s/div_u`, `rem_s/rem_u`, `fNN.div`; float `%` as a `fmod` import. Division by zero traps in wasm where native code raises SIGFPE; both are undefined behaviour at the C++ level and the trap is mapped to a C++ exception at the boundary. |
| `And/Or/Not` (boolean) | `i32.and`, `i32.or`, `i32.eqz` |
| `BinaryComp` BAND/BOR/XOR | `iNN.and/or/xor` |
| `Shift` LS/RS | `iNN.shl`, `shr_s` (signed) / `shr_u` (unsigned); the count is masked by wasm as it is by the hardware |
| `Negate` | `iNN.sub` from `0`, `fNN.neg` |
| `Compare` EQ..GE | `iNN.eq/ne/lt_s/lt_u/...`, `fNN.eq/...`; pointers compare unsigned |
| `Cast` | int widening: `i64.extend_i32_s/u` by *source* signedness; narrowing: `i32.wrap_i64` then normalise; int→float: `fNN.convert_iMM_s/u`; float→int: the **saturating** `iNN.trunc_sat_fMM_s/u` (wasm 2.0), never the trapping form, so a NaN or an out-of-range value yields a number like it does on x86 and in every other backend; float↔float: `f64.promote_f32`, `f32.demote_f64`; int→bool: `iNN.ne 0`; bool→int: nothing |
| `Select` | `select` (both arms are evaluated, matching `arith.select`) |
| `ConstInt/Float/Bool` | `iNN.const`, `fNN.const` |
| `ConstPtr` | `MemoryModel::lowerConstPtr` (see below) |

## Memory access

### Pointer representation and arithmetic

The IR computes addresses as `ptr + ui64` (`docs/pointers.md`). The lowering casts the
offset to `ptrType()` first (`i32.wrap_i64` under wasm32), then adds. Under `arena` the
runtime bounds-checks the final access, so an out-of-arena address traps rather than
reading foreign memory; under `identity` and `proxy` it is the host's address and no
check is possible or wanted.

Loads and stores take an alignment hint and a constant offset. The lowering folds a
`ptr + const` whose only use is the address of one load or store into the instruction's
`offset` immediate, as tbc fuses `LOAD_off`/`STORE_off`. The alignment immediate is
declared as 1: it may not exceed the type's natural alignment, the IR does not know the
real alignment of an address, and on the hardware Nautilus targets engines generate the
same access either way. Wasm is little-endian, which every host Nautilus supports is too.

### `load` / `store` per model

| Model | `LoadOperation(addr) : T` | `StoreOperation(v, addr)` |
|---|---|---|
| `identity`, `arena` | `local.get addr; T.load`, with `i32.load8_s`, `i32.load8_u`, `i32.load16_s`, `i32.load16_u` for the narrow types so the loaded value is already canonical | `local.get addr; local.get v; T.store`, with `i32.store8` and `i32.store16` for the narrow types |
| `proxy` | `local.get addr; call $nautilus_mem.load_<T>` | `local.get addr; local.get v; call $nautilus_mem.store_<T>` |

The proxy imports are twelve host functions (one load and one store per non-void type)
bound once by the runtime layer. They make every memory access a host call; the mode is
meant for running the existing suite against the lowering, not for production.

### Constant pointers

`ConstPtrOperation` is where a host address enters the module as a literal. The
traced code produces them for `val<T*>(&global)`, for the `this` pointer of a
`memberFunc` target, for string literals from the std plugin, and for anything
`engine.foldStaticConstants` folded. Per model:

- `identity`, `proxy`: `ptrType().const <address>`.
- `arena`: if the address lies inside the arena at compile time, `const <address - base>`;
  otherwise a `CompilationException` naming the operation and its source tag. Rebasing
  silently would produce an in-bounds offset that points at the wrong bytes.
- `symbol` linkage: always an error (the artifact must not carry addresses), except for
  the one constant the lowering itself produces for `FunctionAddressOf`.

### Allocas: a shadow stack in linear memory

Wasm has no stack-allocation instruction; locals are not addressable. The function's
alloca table becomes one frame on a **shadow stack** in linear memory, addressed through a
mutable global `$__stack_pointer`:

- Frame layout is computed once per function from `getAllocaSpecs()`: slots sorted by
  alignment descending, frame size rounded to 16 bytes. This fixes the slot offsets the
  way the C++ backend fixes its `alloca_buf_N` declarations.
- Prologue: `global.get $sp; const frameSize; sub; local.tee $fp; global.set $sp`, then
  zero the frame with `memory.fill` to match the interpreting backends, which zero their
  alloca area per call. The MLIR, C++ and AsmJit backends leave the slots uninitialised,
  so whether a traced program can observe the difference is open (see
  [Open questions](#open-questions)); until it is answered, match tbc.
- `AllocaOperation(i)` → `local.get $fp; const offset_i; add`.
- Every `return` and the exceptional exit restore `$sp` from `$fp`.

Where the stack lives is a hosting detail. Under Emscripten the module imports the host's
`env.__stack_pointer`, so wasm frames and host frames interleave on one stack and a
pointer to an alloca stays valid when handed to a host function that itself allocates.
Under an embedded runtime the backend reserves `wasm.stackSizeKb` per thread inside the
arena (or, for `identity` and `proxy`, in a host buffer whose address the entry shim
stores into the global before each top-level call).

### Traps

Out-of-bounds access under `arena`, integer division by zero, `unreachable` after a
structured function body, and runtime stack exhaustion all surface as runtime traps. The
runtime layer converts a trap into a `nautilus::RuntimeException` carrying the trap
message and the function name, raised from the entry path, so a sandboxed fault becomes
an ordinary C++ exception at the `Executable` boundary instead of a crash.

## Runtime functions

### Direct external calls

A `CallOperation` whose target is `External` becomes a wasm **import** with the callee's
exact signature (`NativeTarget::paramTypes`, `resultType`, mapped as in the type table),
one import per distinct `FunctionId`, named per the linkage policy. The call itself is
`call $import`. The binding of the import to native code is the hosting layer's job, and
it differs per hosting mode:

**`emscripten`.** A C++ function pointer under Emscripten *is* an index into the host
module's `__indirect_function_table`, and `FunctionTarget::getAddress()` holds exactly
that index. The generated module imports the same table and calls the target with
`call_indirect (type $sig) (i32.const <index>)`. No import, no trampoline, no marshalling:
an `invoke()` costs what an indirect call costs. The same works for intrinsics' fallback
addresses, for `IndirectCallOperation` (the operand *is* the index) and for
`FunctionAddressOf` of an external (the constant *is* the index).

**`embedded`, `address` linkage.** The host binding table records, per import,
`{address, paramTypes, resultType, memoryModel}`. At instantiation every import is bound to
one generic host function whose user data is that record. The host function unpacks the
runtime's argument values, pushes them into the **dyncall** forward VM the bc and tbc
backends already use for external calls (`backends/bc/Dyncall.hpp`, to be hoisted next to
`NativeClosure` as a shared component), calls the address, and packs the result. Under
`arena` the record says which parameters are pointers; those are rebased from offset to
`base + offset` before the native call, and a pointer result is rebased back and
range-checked (a native pointer outside the arena cannot be represented and is an error,
not a silent wrap). WAMR can do this rebasing itself when the native is registered with a
`*` in its signature string; wasmtime's C API hands the arguments as typed values and the
binding does it. Either way the overhead per call is one host transition plus dyncall's
marshalling, tens of nanoseconds in JIT runtimes, which is the same order as tbc's
`CALL_EXT` and is where intrinsic expansion pays off.

**`embedded`, `symbol` linkage.** The module imports `nautilus_rt.<mangled>`; the worker's
runtime resolves each import against the symbols its own binary exports (`dlsym` on the
main program, or an explicit registration table) and binds them through the same generic
trampoline with the signature taken from the import's wasm type. The signature is thus
carried by the module, not by the name, which is what makes a mismatch a link-time
error instead of a crash.

### Intrinsics

An `Intrinsic` target is an external call a backend *may* expand. The backend keeps a
registry keyed by `IntrinsicId`, filled by the plugins exactly as the MLIR and AsmJit
backends do today (`plugins/*/src/MLIR*Intrinsics.cpp`): a `WasmIntrinsics.hpp` entry
point per plugin registers a handler that emits instructions in place of the call. Without
a handler the call lowers as an ordinary external call to the fallback address, honouring
the registry's fallback invariant, except under `symbol` linkage, where the fallback is
imported by the intrinsic's registry *name* (`IntrinsicRegistry::getName`), which is
stable across processes in a way an emission name is not.

Handlers that are cheap and worth having from the start:

| Intrinsic family | wasm lowering |
|---|---|
| builtin `prefetch` | nothing (it is a hint) |
| std bit intrinsics (popcount, clz, ctz) | `iNN.popcnt`, `clz`, `ctz` |
| std math `sqrt, fabs, floor, ceil, trunc, nearbyint, fmin, fmax, copysign` | the matching `fNN.*` instruction; `sin, cos, exp, log, pow` stay imports |
| std atomics | `iNN.atomic.rmw.*` once the arena is a shared memory; imports until then |
| SIMD `vec<T>` | a later `WasmSimdIntrinsics` on the 128-bit `v128` type; the plugin's runtime lane-count detection must report 128 bits under this backend. Until then the scalar `vector_*_impl` fallbacks are called as imports. |

### Indirect calls

`IndirectCallOperation` calls through a `ptr` operand. Under `emscripten` it is a
`call_indirect` through the shared table with the operand as index. Under an embedded
runtime the operand is a native address the module cannot call, so the backend emits one
import per distinct call signature, `nautilus_fn.indirect_<sig>(ptr target, args...)`,
whose trampoline dyncalls the target. Under `arena` this is the one deliberate hole in the
sandbox: a sandboxed program must not be able to call an arbitrary address, so the
trampoline consults an allow-list the embedder fills (every `FunctionAddressOf` the module
produced is on it by construction), and rejects everything else.

### Function addresses

`FunctionAddressOfOperation` yields a `ptr` the program may store or pass to a runtime
function as a callback (`nautilusAdd.getFuncPtr()`).

- External target: the address under `identity`/`proxy`; the table index under
  `emscripten`; an error under `symbol` linkage and under `arena` unless the embedder
  allow-listed it.
- Internal target under `emscripten`: the function is added to the module's element
  segment and its index in the *shared* table (the module imports `env.__table_base` and
  places its functions at `__table_base + i`, which is the standard Emscripten
  side-module convention) is the constant. Host code can call it like any function
  pointer.
- Internal target under an embedded runtime: as in tbc, every exported function gets a
  `NativeClosure` built *before* lowering, so the closure's code address is known when the
  constant is emitted; the closure enters the runtime and calls the export. A host callback
  into the sandbox therefore crosses the boundary twice, which is unavoidable.

### Internal calls

A call to an `Internal` target is a plain `call $f`. Every `FunctionOperation` in the
graph becomes one wasm function, exported under `IRGraph::getEmissionName`. Recursion and
mutual recursion work as they do natively; the only limit is the runtime's call-stack
depth, which traps.

### Exceptions

Covered under [Landing pads](#landing-pads-and-the-exceptional-exit): the backend uses the
captured host-rethrow transport, binds an import for a potentially throwing call to the
call's `captureThrowingCall` thunk (`CallOperation::getCaptureFunc`) rather than to the
callee, and imports `nautilus_eh.has_pending_exception`. Wasm's own exception-handling
instructions do not help here, because the exception is thrown by native code that the
runtime cannot unwind through; the capture-and-check transport sidesteps that exactly as
it does for AsmJit's unwind-table-less machine code. Under Emscripten the thunk and the
pending check are compiled into the host module and the same binding applies.

### The entry path and the `Executable`

`WasmExecutable` implements `Executable` like `TBCExecutable` does:

- `hasInvocableFunctionPtr()` is `false` under an embedded runtime; callers go through
  `getGenericInvocable`, whose `invokeRaw` converts the 64-bit slots to runtime values
  (rebasing pointers under `arena`), calls the export on the calling thread's instance and
  converts the result back. `getInvocableFunctionPtr` still returns the function's
  `NativeClosure` for callers that need a real pointer.
- Under `emscripten` the executable *does* hand out a real function pointer: the entry
  shim grows the shared table by one and stores the export there, and that index is a
  C function pointer to the host. The typed `Invocable` then calls the compiled function
  directly with no marshalling at all.
- `getExceptionPropagationMode(member)` answers `CapturedHostRethrow` only for functions
  `CapturedExceptionTransport::functionsNeedingCapture` lists, as the other captured
  backends do, so a function without throwing calls skips the frame push.

### Threads and reentrancy

A wasm instance is not thread-safe, so the runtime layer keeps one instance per thread
per compiled module (a `thread_local` map, instances created lazily from the module
compiled once). Reentrancy inside one thread (host → wasm → import → host → closure →
wasm) is supported by the candidate runtimes on the same instance and store. The shadow
stack is per instance, hence per thread. Under `arena` with several threads the arena must
be one *shared* memory imported by every instance (the threads proposal); until that is in
place the arena is per thread, which already serves the UDF case where every worker thread
owns its inputs.

## Emitted module layout

```wat
(module
  (type $sig_ii_i (func (param i32 i32) (result i32)))            ;; one per distinct signature
  (import "nautilus_fn" "f_add" (func $f_add (type $sig_ii_i)))   ;; one per external FunctionId
  (import "nautilus_eh" "has_pending_exception" (func $eh_pending (result i32)))
  (import "env" "memory" (memory $mem i64 0))                      ;; or a module-owned memory under arena
  (import "env" "__stack_pointer" (global $sp (mut i32)))          ;; Emscripten; otherwise module-owned
  (table $t funcref (elem $execute $helper))                       ;; FunctionAddressOf targets
  (func $execute (type ...) ...)                                   ;; one per FunctionOperation
  (func $helper ...)
  (export "execute" (func $execute))
  (@custom "name" ...)                                             ;; $N identifiers as local names, for dumps
  (@custom "metadata.code.branch_hint" ...))
```

The names section is emitted whenever `dump.wasm` is on, so a `.wat` dump reads with the
IR's `$N` identifiers and `Block_N` labels; this is what makes golden-file tests of the
lowering readable, the way `test/data/*/ir/` is for the IR.

## Performance expectations

- **Straight-line and loop code**: with the structured translation, a JIT runtime
  (Cranelift, WAMR fast-JIT/AOT) produces code comparable to the C++ backend at `-O1`;
  under `arena` each memory access carries a bounds check (free with guard pages on 64-bit
  hosts in wasm32, an explicit compare under memory64). Interpreting runtimes land in the
  bc/tbc range.
- **Calls into runtime functions** dominate everything else under an embedded runtime:
  each costs a host transition plus marshalling. Intrinsic expansion removes the common
  ones; the rest is the price of the sandbox. Under Emscripten the cost disappears.
- **Compile time**: lowering is linear in the IR; the runtime's own compilation is the
  larger part (milliseconds per function for Cranelift, less for a baseline JIT or an
  interpreter's validation). The backend is a deployment target, not a tier-0 candidate.
- **Module size**: one local per SSA value and no stack scheduling makes modules larger
  than a producer like LLVM would emit; `wasm-opt -O` (Binaryen) as an optional
  post-pass, or in-backend stack scheduling of single-use values, recovers most of it
  when that matters.

## Testing

- Add `"wasm"` to `availableBackends()` in `test/common/ExecutionTest.hpp` so the whole
  execution, val and tracing suite runs on it. The suite passes host pointers to stack
  arrays and globals freely, so it runs under `proxy` memory, which accepts them; a
  smaller arena-aware subset runs under `arena`.
- Golden `.wat` dumps per test-data category, next to the existing `tracing/`, `ir/` and
  `after_ssa/` dumps, produced with the names section on. These pin the structuring.
- The differential fuzz harness (`test/fuzz/Harness.hpp`) gets `wasm` as a peer, which is
  the real guard against narrow-integer and cast semantics drifting from the other
  backends.
- Every emitted module is run through the runtime's validator in debug builds; CI
  additionally validates the dumps with `wasm-validate` from WABT where it is installed.
- `WasmStructurerTest` unit-tests the structurer on hand-built CFGs: diamond, loop with
  break and continue, nested loops, a merge node inside a loop, and an irreducible graph
  that must fall back to the dispatcher.

## Milestones

1. **Lowering core on an embedded runtime, `proxy` memory, dispatcher control flow.**
   Encoder, module builder, value lowering, external calls through dyncall trampolines,
   shadow stack, captured exceptions, `WasmExecutable`, the suite green. Everything
   after this is an optimisation or a policy.
2. **Structured control flow.** Dominator-based structuring with dispatcher fallback,
   golden `.wat` tests, branch hints.
3. **`arena` memory model.** Boundary rebasing, constant-pointer checking, the
   indirect-call allow-list, arena-aware tests; the sandbox is usable.
4. **Emscripten hosting.** Shared memory, table and stack pointer; table-index calls;
   real function-pointer entry. This needs an Emscripten build of Nautilus itself
   (fmt, spdlog and Catch2 build there; MLIR, dyncall and asmjit are simply off), which
   is a CI job of its own.
5. **`symbol` linkage and `none` hosting.** Portable artifacts, worker-side binding.
6. **Intrinsics**: bit and math handlers first, SIMD on `v128` last.

## Open questions

- **Which embedded runtime first.** WAMR is the smaller dependency, has an interpreter
  for platforms without codegen, and rebases pointer arguments itself; wasmtime has the
  more mature memory64 support and a cleaner C API. The `WasmRuntime` interface keeps
  this a build-time choice, but the first one integrated will shape the binding layer.
- **Whether `identity` on a native host is worth having.** It needs a runtime that runs
  without bounds checks with memory base 0 (patching wasm2c's generated runtime is the
  known way), gives up the sandbox, and then competes with tbc on portability alone. It
  is listed because it is the only native mode that leaves the engine's data structures
  untouched, not because it is recommended.
- **Arena data layout in the engine.** `arena` requires that pointers *inside* shared data
  structures are offsets. That is an engine design decision (relative pointers in tuple
  buffers and hash tables) that the backend can only demand, not provide.
- **Zeroing alloca frames.** bc and tbc zero; MLIR, C++ and AsmJit do not. If no traced
  program can observe the difference, the `memory.fill` in the prologue should go.
- **Pointer width under `arena`.** wasm32 caps the arena at 4 GiB and gets guard-page
  bounds checks; memory64 lifts the cap and pays an explicit check per access. Default to
  wasm32 and make memory64 an option once a workload needs it.
