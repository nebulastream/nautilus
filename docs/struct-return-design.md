# Design Document: Struct Return Support for Nautilus Proxy Functions

## Context

Nautilus currently only supports scalar types (integers, floats, booleans, pointers) as return values from proxy/runtime function calls. When an external C/C++ function returns a struct (e.g., `StructA getObj()`), there is no mechanism to call it from traced Nautilus code and use the returned struct.

This design document analyzes how the C calling convention handles struct returns and proposes how Nautilus can support them, scoped to proxy function calls (external functions called via `invoke()` from traced code). The user also wants type-aware field access on returned structs.

---

## 1. How the C Calling Convention Handles Struct Returns

### System V AMD64 ABI (Linux x86-64)

The ABI classifies struct return values into two categories:

#### Small Structs (≤ 16 bytes, no special alignment)
- Decomposed into "eightbytes" and classified individually
- Integer fields returned in **RAX** and **RDX**
- Float fields returned in **XMM0** and **XMM1**
- Mixed integer+float structs split across both register classes
- Example: `struct { int a; int b; }` (8 bytes) → returned in RAX

#### Large Structs (> 16 bytes or complex layout)
- The **caller** allocates stack space for the return value
- A **hidden pointer** (called `sret`) is passed as an implicit first argument in **RDI**
- The callee writes the struct into that memory and returns the pointer in **RAX**
- Effectively transforms `T func(args...)` → `void func(T* sret, args...)`

```
// What the programmer writes:
StructA getObj(int x);

// What the ABI actually does for large structs:
void getObj(StructA* __sret, int x);  // hidden first parameter
```

#### Key Implication for Nautilus
When Nautilus calls an external function that returns a struct, it must:
1. Know the struct's size and alignment
2. Allocate memory for the return value (stack or heap)
3. Either pass an sret pointer (large structs) or capture register returns (small structs)
4. LLVM handles this automatically IF the function is declared with the correct struct type

### What LLVM Does Automatically
When a function is declared in LLVM IR with a struct return type, LLVM's calling convention lowering automatically:
- Decides register vs memory return based on ABI rules
- Inserts sret parameters when needed
- Generates correct caller/callee code

This means **we don't need to manually implement sret** — we need to declare the function correctly in MLIR/LLVM IR and let LLVM handle the ABI details.

---

## 2. Current Nautilus Limitations

### Type System (`include/nautilus/tracing/Types.hpp`)
- `Type` enum has only 13 scalar types: `v, b, i8, i16, i32, i64, ui8, ui16, ui32, ui64, f32, f64, ptr`
- No composite/aggregate/struct type
- `TypeResolver<T>` only works for `is_compatible_val_type` (fundamental, bool, pointer)

### Proxy Call Pipeline
1. **User API** (`function.hpp`): `CallableRuntimeFunction::operator()` calls `tracing::traceCall()` with `TypeResolver<R>::to_type()` — fails for struct types
2. **Tracing** (`TraceContext.cpp:114-128`): `traceCall()` records result as scalar `Type`
3. **IR** (`ProxyCallOperation.hpp`): Result type is scalar `Type`
4. **MLIR Backend** (`MLIRLoweringProvider.cpp:606-647`): Declares external function with scalar return type, uses `mlir::LLVM::CallOp` with single result
5. **C++ Backend**: Generates scalar-typed function pointer casts
6. **Bytecode Backend**: Uses dyncall which only supports scalar returns

### No Stack / Alloca Support
- No `mlir::LLVM::AllocaOp` usage in the MLIR backend
- No stack frame management at the Nautilus IR level
- This is needed for allocating space for struct return values

---

## 3. Recommended Approach: Opaque Struct Type with Alloca + Struct Layout Registration

### Core Idea

Treat structs as **opaque memory regions** with a known size, alignment, and field layout. When a proxy function returns a struct:

1. **Alloca** stack space for the struct in the generated code
2. **Transform** the call from `T func(args...)` to `void func(T* sret, args...)` at the IR level
3. Return a **pointer** (`val<T*>`) to the stack-allocated struct
4. Use **registered struct layout metadata** to enable field access by name

This approach avoids adding a full first-class struct type to the Nautilus type system and IR. Instead, it leverages the existing pointer and load/store infrastructure.

### Why This Approach

- **Minimal IR changes**: No new IR types needed — struct operations reduce to pointer+load/store
- **ABI-correct**: The sret transformation matches what LLVM would do for large structs
- **Portable**: Works across all backends (MLIR gets alloca, C++ uses stack variables, BC uses heap allocation)
- **Field access**: Layout metadata enables type-aware field access without modifying the IR type system
- **Small struct optimization possible later**: Start with sret for all structs, optimize small structs later

---

## 4. Detailed Design

### 4.1 Struct Layout Registration (User-Facing API)

Users register struct layouts so Nautilus knows field offsets, types, and names:

```cpp
// User's struct
struct StructA {
    int32_t x;
    float y;
    int64_t z;
};

// Registration macro (generates compile-time layout info)
NAUTILUS_REGISTER_STRUCT(StructA,
    (x, int32_t),
    (y, float),
    (z, int64_t)
)
```

This macro would generate:
- A `StructLayout` descriptor with field names, offsets (via `offsetof`), types, and total size/alignment
- A specialization of `TypeResolver<StructA>` that maps to a new struct type
- A specialization of `val<StructA*>` with named field accessors

**New file**: `include/nautilus/val_struct.hpp`

```cpp
// Struct layout metadata (runtime)
struct FieldDescriptor {
    std::string name;
    Type type;
    size_t offset;
    size_t size;
};

struct StructLayout {
    std::string name;
    size_t totalSize;
    size_t alignment;
    std::vector<FieldDescriptor> fields;
};

// Registry for struct layouts (singleton, maps type_info → layout)
class StructLayoutRegistry {
public:
    static StructLayoutRegistry& instance();
    void registerLayout(const std::type_info& type, StructLayout layout);
    const StructLayout* getLayout(const std::type_info& type) const;
};
```

### 4.2 Type System Extension

Add a struct representation to the `Type` system. Two options:

**Option A (Minimal)**: Keep `Type` enum as-is, use `Type::ptr` for struct pointers, carry layout info separately in `ProxyCallOperation`.

**Option B (Cleaner)**: Extend to support struct types.

**Recommended: Option A** — The Type enum stays scalar-only. Struct-returning proxy calls are transformed at the tracing level to return `Type::ptr` (pointer to struct), with the struct layout metadata carried as additional info in the trace operation and IR operation. This minimizes changes.

### 4.3 The sret Trampoline

The key insight: we don't call the original struct-returning function directly. Instead, we create a **trampoline** function that:
1. Takes an extra `R*` first parameter (the sret pointer)
2. Calls the original function
3. Stores the result through the sret pointer

```cpp
// Trampoline: transforms struct-return into void+sret-pointer
template <typename R, typename... Args>
void __attribute__((noinline)) sretTrampoline(R* sret, R(*fnptr)(Args...), Args... args) {
    *sret = fnptr(args...);
}
```

The `noinline` attribute prevents the host compiler from inlining the trampoline, which would confuse the tracing system. The trampoline is registered as a regular proxy function.

**Why always use sret (not register returns for small structs)?**
- The trampoline itself calls the original function, which may return in registers (for small structs). LLVM compiles the trampoline correctly — it handles the ABI.
- From Nautilus's perspective, it's always a void-returning proxy call with a pointer argument.
- Single code path, no ABI classification logic in Nautilus.
- Performance cost is minimal: one extra store + one extra load, which LLVM may optimize away.

### 4.4 Tracing Layer Changes

**File**: `include/nautilus/function.hpp` — `CallableRuntimeFunction`

Add a new concept and specialization for struct return types:

```cpp
// New concept in val_concepts.hpp
template <typename T>
concept is_struct_return = std::is_class_v<T> && std::is_trivially_copyable_v<T>
                           && !std::is_pointer_v<T>;
```

New `invoke` overload for struct-returning functions:

```cpp
template <is_struct_return R, typename... FunctionArguments, typename... ValueArguments>
val<R*> invoke(R (*fnptr)(FunctionArguments...), ValueArguments&&... args) {
#ifdef ENABLE_TRACING
    if (tracing::inTracer()) {
        // 1. Trace alloca for the return buffer
        auto sretRef = tracing::traceAlloca(sizeof(R), alignof(R));

        // 2. Get argument references
        auto functionArgumentReferences = getArgumentReferences(
            std::forward<ValueArguments>(args)...);

        // 3. Prepend sret pointer and fnptr as arguments to the trampoline
        std::vector<tracing::TypedValueRef> trampolineArgs;
        trampolineArgs.push_back(sretRef);
        // fnptr is passed as a constant pointer argument
        auto fnptrRef = tracing::traceConstant((void*)fnptr);
        trampolineArgs.push_back(fnptrRef);
        trampolineArgs.insert(trampolineArgs.end(),
            functionArgumentReferences.begin(), functionArgumentReferences.end());

        // 4. Trace call to the trampoline (void return)
        auto trampolinePtr = reinterpret_cast<void*>(&sretTrampoline<R, FunctionArguments...>);
        tracing::traceCall(trampolinePtr, Type::v, trampolineArgs, fnAttrs);

        return val<R*>(sretRef);
    }
#endif
    // Non-tracing path
    static thread_local R storage;
    storage = fnptr(details::RawValueResolver<FunctionArguments>::getRawValue(
        std::forward<ValueArguments>(args))...);
    return val<R*>(&storage);
}
```

**File**: `src/nautilus/tracing/TraceContext.cpp` — new `traceAlloca()`

Records an ALLOCA trace operation with size and alignment. Returns a `TypedValueRef` of type `ptr`.

**File**: `src/nautilus/tracing/TraceOperation.hpp` — new `AllocaInfo` variant

```cpp
struct AllocaInfo {
    uint64_t sizeInBytes;
    uint64_t alignment;
};
// Added to InputVariant
```

**File**: `include/nautilus/tracing/Operations.hpp` — add `ALLOCA` to `Op` enum

### 4.5 IR Layer Changes

Add `AllocaOperation` to the IR:

```cpp
// New file: src/nautilus/compiler/ir/operations/AllocaOperation.hpp
class AllocaOperation : public Operation {
public:
    AllocaOperation(OperationIdentifier id, uint64_t sizeInBytes, uint64_t alignment);
    uint64_t getSizeInBytes() const;
    uint64_t getAlignment() const;
private:
    uint64_t sizeInBytes_;
    uint64_t alignment_;
};
```

- Result type is always `Type::ptr`
- Add `AllocaOp` to `Operation::OperationType` enum

**No changes to ProxyCallOperation needed.** The sret-transformed call is just a regular void-returning proxy call with the sret pointer and original fnptr as extra arguments. The existing `ProxyCallOperation` handles this without modification.

**Trace-to-IR conversion** (`TraceToIRConversionPhase.cpp`):
- Add `case Op::ALLOCA:` that creates an `AllocaOperation` in the current IR block
- The CALL trace for the trampoline is handled by the existing `processCall` method

**End-to-end IR example** for `Point makePoint(int32_t a, int32_t b)`:
```
%3 = alloca 8, align 4                                // AllocaOperation (sizeof(Point))
call sretTrampoline(%3, makePoint_ptr, %0, %1) : void // ProxyCallOperation
%6 = add %3, 0 : ptr                                  // pointer to field x
%7 = load %6 : i32                                    // LoadOperation
%9 = add %3, 4 : ptr                                  // pointer to field y
%10 = load %9 : i32                                   // LoadOperation
```

### 4.6 Backend Changes

#### MLIR Backend (`MLIRLoweringProvider.cpp`)

**New: Handle `AllocaOperation`**:

Important: The alloca must be placed in the **entry block** of the function to ensure it's only allocated once (not inside loops). The MLIR backend should hoist the alloca:

```cpp
void MLIRLoweringProvider::generateMLIR(ir::AllocaOperation* allocaOp, ValueFrame& frame) {
    // Save current insertion point
    auto savedIP = builder->saveInsertionPoint();

    // Move to entry block of the function for stack allocation
    auto& entryBlock = builder->getBlock()->getParent()->front();
    builder->setInsertionPointToStart(&entryBlock);

    // Create LLVM alloca
    auto one = builder->create<mlir::arith::ConstantOp>(
        getNameLoc("alloca_count"), builder->getI64Type(),
        builder->getIntegerAttr(builder->getI64Type(), 1));
    auto elementType = mlir::LLVM::LLVMArrayType::get(
        builder->getI8Type(), allocaOp->getSizeInBytes());
    auto allocaResult = builder->create<mlir::LLVM::AllocaOp>(
        getNameLoc("sret_alloca"),
        mlir::LLVM::LLVMPointerType::get(context),
        elementType, one, allocaOp->getAlignment());

    // Restore insertion point
    builder->restoreInsertionPoint(savedIP);

    frame.setValue(allocaOp->getIdentifier(), allocaResult);
}
```

**Proxy calls need no changes**: The existing `generateMLIR(ProxyCallOperation*)` already handles void-returning calls and passes all arguments correctly. The sret pointer is just another `ptr`-typed argument.

#### C++ Backend (`CPPLoweringProvider.cpp`)

**New: Handle `AllocaOperation`**:
```cpp
// Generates:
//   alignas(alignment) uint8_t varN_storage[size];
//   uint8_t* varN = varN_storage;
```

**Proxy calls need no changes**: The trampoline is called as a regular void-returning function with pointer arguments.

#### Bytecode Backend (`BCLoweringProvider.cpp`)

**New: Handle `AllocaOperation`**: Allocate memory via `std::aligned_alloc` and store the pointer in a register. Track allocations for cleanup when the function returns.

**New bytecode instruction**: `ALLOCA` — takes size and alignment, produces a pointer in the output register.

**Proxy calls need no changes**: Dyncall already supports `addArgPtr` and `callVoid`. The sret pointer is passed as a regular pointer argument.

### 4.6 Field Access API

With struct layout registration and a `val<StructA*>` in hand, users can access fields:

```cpp
// Generated by NAUTILUS_REGISTER_STRUCT macro:
template <>
class val<StructA*> : public base_ptr_val<StructA*> {
    // ... base constructors ...

    val<int32_t&> x() {
        // Compute field offset, cast to appropriate pointer, create reference
        auto basePtr = static_cast<val<int8_t*>>(*this);
        auto fieldPtr = basePtr + static_cast<val<size_t>>(offsetof(StructA, x));
        auto typedPtr = static_cast<val<int32_t*>>(fieldPtr);
        return *typedPtr;
    }

    val<float&> y() { /* similar with offsetof(StructA, y) */ }
    val<int64_t&> z() { /* similar with offsetof(StructA, z) */ }
};
```

This decomposes entirely into existing pointer arithmetic + load/store operations, requiring NO changes to the IR or backends for field access. The tracing system captures ADD, CAST, LOAD, STORE operations as usual.

### 4.7 User-Facing API Example

```cpp
struct StructA {
    int32_t x;
    float y;
    int64_t z;
};

NAUTILUS_REGISTER_STRUCT(StructA, (x, int32_t), (y, float), (z, int64_t))

// External function
StructA getObj(int32_t input);

// Usage in Nautilus traced function
auto myFunc = engine.registerFunction(+[](val<int32_t> input) -> val<int64_t> {
    auto objPtr = invoke(getObj, input);  // Returns val<StructA*>
    val<int64_t> z = objPtr.z();          // Field access via generated accessor
    val<int32_t> x = objPtr.x();
    return z + static_cast<val<int64_t>>(x);
});
```

---

## 5. Alloca and Stack Considerations

The user noted that "Nautilus has no real stack which is managed explicitly." Adding `AllocaOperation` introduces the first form of explicit stack allocation. Key considerations:

- **Lifetime**: Stack-allocated structs are valid only within the current function scope. This is naturally enforced since the alloca is inside the generated function.
- **MLIR/LLVM**: `mlir::LLVM::AllocaOp` maps directly to LLVM's `alloca` instruction, which is well-understood and optimized.
- **No full stack frame management needed**: We don't need a general-purpose stack. `AllocaOperation` is a targeted addition for struct return buffers. LLVM handles the actual stack layout.
- **Future extensibility**: This same alloca mechanism can later support local variables, arrays on stack, or other aggregate types.

---

## 6. Files to Modify/Create

### New Files
| File | Purpose |
|------|---------|
| `include/nautilus/val_struct.hpp` | Struct layout registration macro, StructLayout, field accessor generation |
| `src/nautilus/compiler/ir/operations/AllocaOperation.hpp` | IR alloca operation declaration |
| `src/nautilus/compiler/ir/operations/AllocaOperation.cpp` | IR alloca operation implementation |

### Modified Files
| File | Changes |
|------|---------|
| `include/nautilus/tracing/Types.hpp` | No change to Type enum (structs use `ptr`). Add StructCallInfo to tracing types. |
| `include/nautilus/function.hpp` | Add struct-returning overload in `CallableRuntimeFunction` |
| `include/nautilus/core.hpp` | Include `val_struct.hpp` |
| `src/nautilus/tracing/TraceContext.hpp/cpp` | Add `traceStructReturningCall()` method |
| `src/nautilus/tracing/TraceOperation.hpp` | Add `StructCallInfo` to `InputVariant` or extend `FunctionCall` |
| `src/nautilus/tracing/phases/TraceToIRConversionPhase.cpp` | Handle struct call → AllocaOp + sret ProxyCallOp |
| `src/nautilus/compiler/ir/operations/Operation.hpp` | Add `AllocaOp` to `OperationType` enum |
| `src/nautilus/compiler/ir/operations/CMakeLists.txt` | Add AllocaOperation source |
| `src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp/cpp` | Handle AllocaOp; handle sret attribute on proxy calls |
| `src/nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp/cpp` | Handle AllocaOp (stack array) |
| `src/nautilus/compiler/backends/bc/BCLoweringProvider.hpp/cpp` | Handle AllocaOp (heap buffer); handle sret dyncall |
| `src/nautilus/compiler/ir/IRGraph.cpp` | Add formatting for AllocaOp |
| `src/nautilus/compiler/ir/util/GraphVizUtil.cpp` | Add visualization for AllocaOp |
| Optimization phases (various) | Ensure alloca operations are not eliminated or reordered incorrectly |

---

## 7. Verification Plan

### Unit Tests
1. **Struct layout registration**: Verify `NAUTILUS_REGISTER_STRUCT` generates correct offsets
2. **Tracing**: Verify struct-returning calls are traced correctly (alloca + sret call)
3. **IR generation**: Verify AllocaOp + sret ProxyCallOp appear in IR
4. **MLIR backend**: End-to-end test calling external struct-returning function
5. **C++ backend**: End-to-end test with C++ code generation
6. **Bytecode backend**: End-to-end test with interpreter

### Test Structure
```
test/data/struct-return-tests/
├── tracing/    # Trace output for struct-returning calls
├── ir/         # Expected IR with AllocaOp + ProxyCallOp
└── after_ssa/  # Post-optimization IR
```

### Build & Run
```bash
cd build
cmake --build . --target nautilus
ctest --test-dir nautilus -R "StructReturn" --output-on-failure
cmake --build . --target format
```

---

## 8. Open Questions / Future Work

1. **Small struct optimization**: For structs ≤ 16 bytes, LLVM can return them in registers. We currently always use sret. Worth optimizing later?
2. **Nested structs**: Should field access support nested structs (e.g., `objPtr.inner().field()`)?
3. **Struct as function argument**: This design focuses on struct returns. Struct arguments are a separate concern (currently users pass `StructA*` as a pointer argument, which already works).
4. **Struct array returns**: Not covered in this design.
5. **Struct member functions**: The existing `memberFunc()` wrapper could be extended to work with registered structs.

---

## 9. Alternative Approach Considered: First-Class LLVM Struct Types

Instead of the sret trampoline, we could declare the external function in MLIR/LLVM IR with a proper `llvm.struct` return type:

```mlir
llvm.func external @getObj(%arg0: i32) -> !llvm.struct<(i32, f32, i64)>
```

LLVM would then handle the ABI classification (register vs sret) automatically.

### Why We Don't Recommend This Approach (for now)

1. **Requires extending the Nautilus Type system**: The `Type` enum and `getMLIRType()` would need to support composite types — a significant refactor touching every layer.
2. **IR needs struct-typed operations**: `ProxyCallOperation` result would be a struct value, requiring new operations to extract fields (`extractvalue` equivalent). Every backend would need to implement these.
3. **Bytecode/C++ backends**: Cannot rely on LLVM's ABI lowering. They'd need their own struct classification logic.
4. **More complex, less incremental**: The trampoline approach works today with minimal changes and can be optimized later.

### When to Revisit

Consider the LLVM struct approach when:
- Nautilus needs to pass structs as arguments (not just returns)
- Nautilus-compiled functions themselves need to return structs (not just proxy calls)
- Performance analysis shows the trampoline overhead is measurable in production workloads

---

## 10. Summary of Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| ABI handling | sret trampoline | Avoids ABI classification logic in Nautilus; single code path |
| Type system | Keep scalar-only `Type` enum | Minimal changes; struct pointers use `Type::ptr` |
| IR representation | New `AllocaOperation` + existing `ProxyCallOperation` | Composable; no changes to proxy call infrastructure |
| Stack support | Targeted `AllocaOperation` only | Not a full stack frame; LLVM manages actual layout |
| Field access | `NAUTILUS_REGISTER_STRUCT` macro | Generates `val<T*>` specialization with typed field accessors |
| Backend strategy | Alloca per-backend; proxy calls unchanged | Each backend implements alloca natively; call path is reused |

Sources:
- [x86 calling conventions - Wikipedia](https://en.wikipedia.org/wiki/X86_calling_conventions)
- [System V ABI - OSDev Wiki](https://wiki.osdev.org/System_V_ABI)
- [MSVC ABI for Structure Return Types](https://blog.aaronballman.com/2012/02/describing-the-msvc-abi-for-structure-return-types/)
