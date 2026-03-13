# TPDE Compiler Backend Integration Plan for Nautilus

## Motivation

Nautilus currently relies on MLIR→LLVM→Native as its primary JIT backend. This pipeline produces well-optimized code but incurs significant compilation latency — every MLIR dialect conversion, LLVM pass, and OrcJIT linkage step adds overhead that is paid on every query invocation.

TPDE is a single-pass compiler backend framework that combines instruction selection, register allocation, and instruction encoding into one traversal. It achieves **8–24x faster code generation than LLVM -O0** while producing baseline-quality native code. It already ships with an `ElfMapper` that loads compiled ELF objects directly into executable memory for JIT use.

This is a natural fit for Nautilus because:

1. **Nautilus IR is already SSA-form** (enforced by `SSACreationPhase`) — exactly what TPDE consumes.
2. **TPDE's IR Adaptor concept** is a C++20 concept with ~25 methods — a clean mapping exists from `IRGraph`/`BasicBlock`/`Operation` to TPDE's `IRBlockRef`/`IRInstRef`/`IRValueRef`.
3. **TPDE provides `ElfMapper`** for in-process JIT loading (no `dlopen`, no temp files, no LLVM dependency).
4. **The AsmJit backend slot is effectively vacant** (current impl is a stub returning nullptr) — TPDE would fill this role properly.

The result is a **fast-tier JIT backend**: TPDE for first-call compilation (near-zero latency), with optional background recompilation via MLIR for hot queries.

---

## Structural Mapping: Nautilus IR → TPDE Concepts

### Reference Types

| TPDE Concept | Nautilus Mapping | Representation |
|---|---|---|
| `IRFuncRef` | `uint32_t` | Always 0 (one function per `IRGraph`) |
| `IRBlockRef` | `uint16_t` | `BasicBlock::identifier` (already a `uint16_t`) |
| `IRInstRef` | `uint32_t` | Dense index assigned during adaptor construction |
| `IRValueRef` | `uint32_t` | `OperationIdentifier::getId()` (already a `uint32_t`) |

### Phi Node Mapping

Nautilus uses `BasicBlockArgument` objects stored in `BasicBlock::arguments`. When a `BranchOperation` or `IfOperation` transfers control, its `BasicBlockInvocation` carries a `vector<Operation*>` that map 1:1 to the target block's `BasicBlockArgument` vector.

TPDE's `PHIRef` concept requires:
```cpp
incoming_count()                       → block->getPredecessors().size()
incoming_val_for_slot(u32 slot)        → predecessors[slot]'s BlockInvocation argument at this phi's index
incoming_block_for_slot(u32 slot)      → predecessors[slot]
incoming_val_for_block(IRBlockRef blk) → find blk's BlockInvocation, get argument at this phi's index
```

This requires building a reverse map during adaptor construction: for each block, record which predecessor BlockInvocations target it and at what argument index each `BasicBlockArgument` corresponds to.

### Block Successor Mapping

TPDE calls `block_succs(IRBlockRef)` to get successor blocks. Nautilus encodes this implicitly in terminator operations:

| Terminator | Successors |
|---|---|
| `ReturnOperation` | none |
| `BranchOperation` | `getNextBlockInvocation().getBlock()` → 1 successor |
| `IfOperation` | `getTrueBlockInvocation().getBlock()`, `getFalseBlockInvocation().getBlock()` → 2 successors |

### Instruction/Value Separation

TPDE distinguishes instructions (side-effecting, may produce values) from values (SSA results). In Nautilus every `Operation` is both an instruction and a value (via its `OperationIdentifier`).

Mapping:
- `inst_operands(inst)` → `operation->getInputs()` mapped to `IRValueRef`
- `inst_results(inst)` → the operation's own `OperationIdentifier` as a single-element range (or empty for `StoreOp`, `BranchOp`, `ReturnOp`)
- `inst_fused(inst)` → always `false` (Nautilus doesn't fuse instructions)

### Type → Register Bank Mapping

| Nautilus `Type` | TPDE Register Bank | Width |
|---|---|---|
| `i8`, `ui8` | GP (bank 0) | 1 byte |
| `i16`, `ui16` | GP (bank 0) | 2 bytes |
| `i32`, `ui32` | GP (bank 0) | 4 bytes |
| `i64`, `ui64`, `ptr` | GP (bank 0) | 8 bytes |
| `b` (bool) | GP (bank 0) | 1 byte |
| `f32` | FP (bank 1) | 4 bytes |
| `f64` | FP (bank 1) | 8 bytes |

---

## Implementation Plan

### Phase 0: Dependency Setup

**Goal:** TPDE builds alongside Nautilus without touching existing code.

1. Add TPDE as a git submodule at `third_party/tpde`, pinned to a specific commit.
2. In the top-level `CMakeLists.txt`, add:
   ```cmake
   option(ENABLE_TPDE_BACKEND "Enable TPDE fast JIT backend" OFF)
   if (ENABLE_TPDE_BACKEND)
       add_subdirectory(third_party/tpde/tpde)
   endif()
   ```
3. Add `nautilus/src/nautilus/compiler/backends/tpde/` directory with its own `CMakeLists.txt`.
4. Wire into `nautilus/src/nautilus/compiler/backends/CMakeLists.txt`:
   ```cmake
   if (ENABLE_TPDE_BACKEND)
       add_subdirectory(tpde)
   endif()
   ```

**Verification:** `cmake -DENABLE_TPDE_BACKEND=ON .. && make nautilus` compiles without errors.

### Phase 1: `NautilusIRAdaptor`

**Goal:** Implement the TPDE `IRAdaptor` concept against Nautilus's `IRGraph`.

**File:** `nautilus/src/nautilus/compiler/backends/tpde/NautilusIRAdaptor.hpp`

The adaptor is constructed from a `shared_ptr<ir::IRGraph>` and pre-computes:

1. **Dense value index map** (`unordered_map<uint32_t, ValLocalIdx>`): Assign every `Operation*` (including `BasicBlockArgument`) a sequential `ValLocalIdx`. Walk all blocks and all operations once during construction.

2. **Instruction list per block** (`vector<vector<IRInstRef>>`): Skip `BasicBlockArgument` operations (those are phis, not instructions). All other operations become instructions.

3. **Successor list per block** (`vector<SmallVector<IRBlockRef, 2>>`): Inspect terminator of each block.

4. **Phi reverse map** (`map<BasicBlockArgument*, PhiInfo>`): For each `BasicBlockArgument`, record its index in its owning block's argument list, plus the set of (predecessor_block, incoming_value) pairs by scanning all `BasicBlockInvocation` objects that target that block.

**Key method implementations:**

```cpp
// Module level — single function
u32 func_count() { return 1; }
bool func_extern(IRFuncRef) { return false; }
bool func_only_local(IRFuncRef) { return true; }
string_view func_link_name(IRFuncRef) { return root_op_->getName(); }

// Current function
bool cur_is_vararg() { return false; }  // Nautilus functions are never vararg
bool cur_needs_unwind_info() { return false; }
IRBlockRef cur_entry_block() { return blocks_[0].identifier; }

// Values
bool val_is_phi(IRValueRef v) {
    return ops_[v]->getOperationType() == OperationType::BasicBlockArgument;
}
bool val_ignore_in_liveness_analysis(IRValueRef v) {
    auto ty = ops_[v]->getOperationType();
    return ty == OperationType::ConstIntOp || ty == OperationType::ConstFloatOp
        || ty == OperationType::ConstBooleanOp || ty == OperationType::ConstPtrOp;
}
```

**The `NautilusPHIRef` struct** satisfies TPDE's `PHIRef` concept:
```cpp
struct NautilusPHIRef {
    const BasicBlockArgument* arg;
    const BasicBlock* owner_block;
    uint64_t arg_index;  // position in owner_block->getArguments()
    // Pre-computed incoming edges
    SmallVector<pair<IRBlockRef, IRValueRef>, 4> incomings;

    u32 incoming_count() const { return incomings.size(); }
    IRValueRef incoming_val_for_slot(u32 s) const { return incomings[s].second; }
    IRBlockRef incoming_block_for_slot(u32 s) const { return incomings[s].first; }
    IRValueRef incoming_val_for_block(IRBlockRef b) const { /* linear scan */ }
};
```

**Verification:** `static_assert(tpde::IRAdaptor<NautilusIRAdaptor>)` compiles. Unit tests verify iteration counts, successor relationships, and phi incoming values against hand-constructed `IRGraph` instances.

### Phase 2: `NautilusCompiler` — Instruction Selection

**Goal:** Implement TPDE's `Compiler` concept by dispatching on `OperationType` and emitting x64 machine instructions.

**Files:**
- `nautilus/src/nautilus/compiler/backends/tpde/NautilusCompiler.hpp` — CRTP class inheriting `tpde::x64::CompilerX64<NautilusIRAdaptor, NautilusCompiler>`
- `nautilus/src/nautilus/compiler/backends/tpde/NautilusCompiler.cpp` — `compile_inst()` implementation

**The core method:**
```cpp
bool compile_inst(IRInstRef inst_ref, InstRange remaining) {
    auto* op = adaptor_->getOperation(inst_ref);
    switch (op->getOperationType()) {
        case OperationType::AddOp:     return compileAdd(op->dynCast<AddOperation>());
        case OperationType::SubOp:     return compileSub(op->dynCast<SubOperation>());
        // ... 25+ cases
        case OperationType::ProxyCallOp: return compileProxyCall(op->dynCast<ProxyCallOperation>());
        default: return false;
    }
}
```

**Operation → x64 instruction mapping (all 30 OperationType values):**

| Nautilus OperationType | Handler | x64 Emission |
|---|---|---|
| `AddOp` | `compileAdd` | `ADD r,r` (int), `ADDSS/ADDSD` (float), `LEA` (ptr+int) |
| `SubOp` | `compileSub` | `SUB r,r` / `SUBSS/SUBSD` |
| `MulOp` | `compileMul` | `IMUL r,r` / `MULSS/MULSD` |
| `DivOp` | `compileDiv` | `CDQ+IDIV` (signed), `XOR rdx+DIV` (unsigned), `DIVSS/DIVSD` |
| `ModOp` | `compileMod` | `CDQ+IDIV` → `rdx` (signed), `XOR rdx+DIV` → `rdx` (unsigned) |
| `AndOp` | `compileAnd` | `AND r,r` (logical: both are bool) |
| `OrOp` | `compileOr` | `OR r,r` (logical) |
| `NotOp` | `compileNot` | `XOR r, 1` (logical not on bool) |
| `NegateOp` | `compileNegate` | `NEG r` (int), `XORPS` sign bit (float) |
| `CompareOp` | `compileCompare` | `CMP r,r` + `SETcc` (int), `UCOMISS/UCOMISD` + `SETcc` (float) |
| `BinaryComp` (BAND) | `compileBinaryComp` | `AND r,r` |
| `BinaryComp` (BOR) | | `OR r,r` |
| `BinaryComp` (XOR) | | `XOR r,r` |
| `ShiftOp` (LS) | `compileShift` | `SHL r,cl` |
| `ShiftOp` (RS) | | `SHR r,cl` (unsigned) / `SAR r,cl` (signed) |
| `CastOp` | `compileCast` | `MOVSX/MOVZX` (int widening), `CVTSI2SS/SD` (int→float), `CVTTSS/SD2SI` (float→int), `CVTSS2SD/CVTSD2SS` (float↔double) |
| `SelectOp` | `compileSelect` | `CMP cond,0` + `CMOVcc r,r` |
| `LoadOp` | `compileLoad` | `MOV r,[addr]` with appropriate width |
| `StoreOp` | `compileStore` | `MOV [addr],r` with appropriate width |
| `AllocaOp` | `compileAlloca` | Frame slot allocation via `CompilerBase::allocate_stack_slot()` |
| `ConstIntOp` | `compileConstInt` | `MOV r, imm` (materialise via `materialize_constant`) |
| `ConstFloatOp` | `compileConstFloat` | Load from read-only data section |
| `ConstBooleanOp` | `compileConstBool` | `MOV r, 0/1` |
| `ConstPtrOp` | `compileConstPtr` | `MOV r, imm64` (absolute address) |
| `ProxyCallOp` | `compileProxyCall` | Build call via `CallBuilderBase`: set up args per SysV ABI, `CALL [imm64]` |
| `ReturnOp` | `compileReturn` | Move result to `rax`/`xmm0`, then epilogue via `gen_func_epilog()` |
| `IfOp` | `compileIf` | `CMP cond,0` + `generate_cond_branch(JE/JNE, true_blk, false_blk)` |
| `BranchOp` | `compileBranch` | `generate_uncond_branch(target_blk)` |
| `FunctionOp` | skipped | Handled by `start_func`/`prologue_begin` in CompilerX64 |
| `BasicBlockArgument` | skipped | Handled as phis by TPDE's `CompilerBase` |
| `BlockInvocation` | skipped | Not a real instruction — encoded in Branch/If |
| `LoopOp` | skipped | Not emitted post-SSA (loop structure inferred by TPDE Analyzer) |
| `MLIR_YIELD` | skipped | MLIR-specific, not present in generic IR path |

**ProxyCallOp detail** — the critical case for database JIT:
```cpp
bool compileProxyCall(const ProxyCallOperation* call) {
    // 1. Resolve function address
    void* fn_ptr = call->getFunctionPtr();
    SymRef sym = assembler_.sym_add_undef(call->getFunctionSymbol(), SymBinding::GLOBAL);

    // 2. Build call using TPDE's CallBuilderBase
    auto cb = this->template begin_call<CallBuilder>(sym, num_args, ...);
    for (u32 i = 0; i < call->getInputArguments().size(); ++i) {
        auto vp = this->val_ref_single(operand_ref(call->getInputArguments()[i]));
        cb.arg(i, std::move(vp.second));
    }
    cb.emit();

    // 3. Register the absolute address for relocation resolution
    symbol_addresses_[sym] = fn_ptr;
    return true;
}
```

**Verification:** Compile simple arithmetic functions through the full pipeline and compare output against MLIR backend results.

### Phase 3: `TPDEExecutable` and ELF Loading

**Goal:** Wrap TPDE's `ElfMapper` + `AssemblerElf` output into Nautilus's `Executable` interface.

**Files:**
- `nautilus/src/nautilus/compiler/backends/tpde/TPDEExecutable.hpp`
- `nautilus/src/nautilus/compiler/backends/tpde/TPDEExecutable.cpp`

```cpp
class TPDEExecutable : public Executable {
public:
    TPDEExecutable(tpde::elf::AssemblerElf&& assembler,
                   const SymbolAddressMap& external_symbols) {
        mapper_.map(assembler, [&](std::string_view name) -> void* {
            // Resolve ProxyCallOp function pointers
            auto it = external_symbols.find(name);
            if (it != external_symbols.end()) return it->second;
            return nullptr;
        });
    }

    void* getInvocableFunctionPtr(const std::string& member) override {
        return mapper_.get_sym_addr(func_sym_);
    }

    bool hasInvocableFunctionPtr() override { return true; }

private:
    tpde::ElfMapper mapper_;
    tpde::SymRef func_sym_;
};
```

TPDE's `ElfMapper` handles:
- `mmap` with `PROT_READ|PROT_EXEC`
- ELF section parsing and relocation application
- Symbol address resolution via the callback
- Frame registration for stack unwinding (if needed)

No temp files, no `dlopen`, no LLVM dependency.

**Verification:** End-to-end test: trace a simple function → compile with TPDE → invoke through `TPDEExecutable` → verify correct result.

### Phase 4: Backend Registration

**Goal:** Wire `TPDECompilationBackend` into the existing plugin system.

**File:** `nautilus/src/nautilus/compiler/backends/tpde/TPDECompilationBackend.hpp/cpp`

```cpp
class TPDECompilationBackend : public CompilationBackend {
public:
    std::unique_ptr<Executable> compile(
        const std::shared_ptr<ir::IRGraph>& ir,
        const DumpHandler& dumpHandler,
        const engine::Options& options) const override
    {
        // 1. Build adaptor
        NautilusIRAdaptor adaptor(ir);

        // 2. Create assembler
        tpde::elf::AssemblerElf assembler(tpde::x64::target_info_elf);

        // 3. Create compiler and compile
        NautilusCompiler compiler(&adaptor, &assembler);
        if (!compiler.compile()) {
            throw RuntimeException("TPDE compilation failed");
        }

        // 4. Optionally dump generated assembly
        dumpHandler.dump("tpde_asm", compiler.getDisassembly());

        // 5. Load and return executable
        return std::make_unique<TPDEExecutable>(
            std::move(assembler), compiler.getExternalSymbols());
    }
};
```

**Registration** in `CompilationBackend.cpp`:
```cpp
#ifdef ENABLE_TPDE_BACKEND
#include "nautilus/compiler/backends/tpde/TPDECompilationBackend.hpp"
#endif
// ...
#ifdef ENABLE_TPDE_BACKEND
    items["tpde"] = std::make_unique<tpde::TPDECompilationBackend>();
#endif
```

**Verification:** `engine.setOption("engine.backend", "tpde")` selects the backend. A simple function compiles and runs correctly.

### Phase 5: Testing and Validation

**Goal:** Pass the full Nautilus test suite on the TPDE backend.

1. **Parametric backend testing:** Modify `test/common/ExecutionTest.hpp` to parameterise tests across backends (`mlir`, `tpde`, `bc`, `cpp`). Run `ctest` with `NAUTILUS_BACKEND=tpde`.

2. **Test categories to validate:**
   - `execution-tests/ExpressionTest.cpp` — arithmetic, bitwise, shift, compare
   - `execution-tests/ControlFlowTest.cpp` — if/else, nested branches
   - `execution-tests/LoopTest.cpp` — while loops, for loops
   - `execution-tests/PointerTest.cpp` — load/store, pointer arithmetic
   - `execution-tests/CastTest.cpp` — integer widening/narrowing, int↔float
   - `execution-tests/std/` — standard library function calls (ProxyCallOp)

3. **TPDE-specific dump tests:** Add `test/data/tpde-tests/` with reference disassembly for simple functions to catch regressions in code generation.

4. **Benchmarks:** Extend `test/benchmark/ExecutionBenchmark.cpp`:
   ```
   Metric                   Bytecode    TPDE        MLIR
   Compilation latency      ~0.1ms      ~0.5ms      ~10ms
   Execution throughput     1x          8-15x       15-20x
   ```

### Phase 6 (Future): Tiered Compilation

Once TPDE is validated, implement tiered compilation in the engine:

1. First call: compile with TPDE (fast, baseline code quality).
2. If a function is called >N times: recompile with MLIR in a background thread.
3. Atomically swap the function pointer when MLIR compilation finishes.

This is a separate project that builds on top of the TPDE backend.

---

## File Layout

```
nautilus/src/nautilus/compiler/backends/tpde/
├── CMakeLists.txt
├── TPDECompilationBackend.hpp
├── TPDECompilationBackend.cpp
├── TPDEExecutable.hpp
├── TPDEExecutable.cpp
├── NautilusIRAdaptor.hpp        ← IRAdaptor concept implementation (~300 lines)
├── NautilusIRAdaptor.cpp        ← Pre-computation logic (phi map, dense indices)
├── NautilusCompiler.hpp         ← CRTP class extending CompilerX64 (~100 lines)
├── NautilusCompiler.cpp         ← compile_inst dispatch + all compileXxx methods (~800 lines)
└── NautilusPHIRef.hpp           ← PHIRef concept implementation (~50 lines)

third_party/tpde/                ← git submodule @ pinned commit
```

---

## Risks and Mitigations

| Risk | Impact | Mitigation |
|---|---|---|
| **TPDE is young; API may change** | Adaptor breaks on update | Pin submodule to specific commit. Only update deliberately. |
| **Phi mapping complexity** | Incorrect code for control flow merges | Build comprehensive reverse-map during adaptor construction. Validate against `ControlFlowTest.cpp` early. |
| **ProxyCallOp calls arbitrary C++ functions** | ABI mismatch → crashes | Use TPDE's `CallBuilderBase` which handles SysV ABI. Test with all `runtime-call-tests/` data. |
| **ElfMapper unavailable on non-ELF platforms (macOS)** | Backend won't work on macOS | Gate `ENABLE_TPDE_BACKEND` to Linux only initially. macOS support requires Mach-O mapper. |
| **ARM64 instruction selection** | x64-only initially | Start with `CompilerX64` only. ARM64 can be added later using `tpde::arm64::CompilerArm64`. |
| **ConstFloatOp needs read-only data section** | Float constants must live in memory | Use `AssemblerElf::get_default_section(SectionKind::ReadOnly)` to emit float literals. |
| **Unsigned division/modulo** | x64 DIV vs IDIV confusion | Check `isSignedInteger(op->getStamp())` / `isUnsignedInteger()` to select correct instruction. |

---

## Estimated Effort

| Phase | Scope | Estimate |
|---|---|---|
| Phase 0: Dependency setup | CMake, submodule | 1 day |
| Phase 1: IR Adaptor | ~25 concept methods, phi map, tests | 1–2 weeks |
| Phase 2: Instruction selection | 25+ operation handlers | 3–4 weeks |
| Phase 3: ELF loading + Executable | ElfMapper integration | 2–3 days |
| Phase 4: Backend registration | Boilerplate wiring | 1 day |
| Phase 5: Testing + benchmarks | Full test suite pass | 1–2 weeks |
| **Total** | | **~6–8 weeks** |

Phase 1 and Phase 3 can be developed in parallel.
