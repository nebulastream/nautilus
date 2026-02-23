# Nautilus Tracing Abstraction Plan

## Problem Statement

The current Nautilus architecture has tight coupling between three layers:
1. **`val<T>` types** (user-facing value wrappers)
2. **Tracing implementation** (execution trace recording)
3. **IR operations** (compiler intermediate representation)

This coupling manifests as:
- `#ifdef ENABLE_TRACING` blocks duplicating every constructor in every `val<T>` specialization
- Two separate operation enums (`tracing::Op` and `ir::Operation::OperationType`) that must be kept in sync
- Core types (`Type`, `TypeResolver`, `ConstantLiteral`) living in the `tracing` namespace despite being used everywhere
- `val<T>` headers directly depending on tracing internals (`TraceContext`, `TypedValueRefHolder`)
- A large manual switch-based mapping in `TraceToIRConversionPhase`

## Current Architecture (as-is)

```
val<T> operators
    │ direct calls to tracing::traceBinaryOp(), tracing::traceConstant(), etc.
    │ stores tracing::TypedValueRefHolder as member
    │ guarded by #ifdef ENABLE_TRACING in every constructor
    ▼
TracingUtil free functions (TracingUtil.hpp)
    │ delegates to thread-local TraceContext
    ▼
TraceContext → ExecutionTrace (TraceOperation with tracing::Op enum)
    │
    ▼
SSACreationPhase
    │
    ▼
TraceToIRConversionPhase (large switch mapping tracing::Op → ir::OperationType)
    │
    ▼
IRGraph (compiler::ir::Operation with separate OperationType enum)
    │
    ▼
Backend codegen (MLIR, C++, Bytecode, AsmJit)
```

### Coupling Point 1: val<T> ↔ Tracing

Every val<T> specialization (val_arith.hpp, val_bool.hpp, val_ptr.hpp, val_enum.hpp) has:

```cpp
// Every constructor is duplicated:
#ifdef ENABLE_TRACING
    val(ValueType value) : state(tracing::traceConstant(value)), value(value) {}
#else
    val(ValueType value) : value(value) {}
#endif

// Every operator calls tracing directly:
if SHOULD_TRACE() {
    auto tc = tracing::traceBinaryOp(tracing::ADD,
        tracing::TypeResolver<resultType>::to_type(),
        StateResolver<...>::getState(lValue),
        StateResolver<...>::getState(rValue));
    return val<resultType>(tc);
}
```

### Coupling Point 2: Dual Operation Enums

| tracing::Op | ir::Operation::OperationType |
|-------------|------------------------------|
| ADD         | AddOp                        |
| SUB         | SubOp                        |
| MUL         | MulOp                        |
| EQ,NEQ,LT.. | CompareOp (sub-enum)        |
| LSH, RSH   | ShiftOp (sub-enum)           |
| BAND,BOR..  | BinaryComp (sub-enum)       |
| LOAD        | LoadOp                       |
| STORE       | StoreOp                      |
| NOT         | NotOp                        |
| NEGATE      | NegateOp                     |

### Coupling Point 3: Misplaced Core Types

`nautilus::Type`, `TypeResolver<T>`, `ConstantLiteral` are declared in `tracing/Types.hpp`
but used by both tracing and IR layers. The IR `Operation` base class already includes
`nautilus/tracing/Types.hpp` — a compiler layer depending on a tracing header.

## Proposed Architecture (to-be)

### Layer 1: Core Types (new `nautilus::core` or just `nautilus::`)

Extract shared types that both tracing and IR depend on:

```
include/nautilus/
├── core.hpp               # Already exists, extend with:
│   ├── Type enum          # Move from tracing/Types.hpp
│   ├── TypeResolver<T>    # Move from tracing/Types.hpp
│   ├── ConstantLiteral    # Move from tracing/Types.hpp
│   └── OpCode enum        # NEW: unified operation codes
├── ValueRef.hpp           # NEW: extracted from tracing/TypedValueRef.hpp
│   ├── ValueRef (uint16_t)
│   ├── TypedValueRef {ref, type}
│   └── TypedValueRefHolder (RAII)
```

The `Type` enum is already in the `nautilus` namespace — this is just moving the file
and `TypeResolver` out of `tracing/`. Backward-compatible aliases can be added.

### Layer 2: Unified OpCode

Replace the two separate enums with a single source of truth:

```cpp
// include/nautilus/OpCode.hpp
namespace nautilus {

enum class OpCode : uint8_t {
    // Arithmetic
    Add, Sub, Mul, Div, Mod,
    // Comparison
    Eq, Neq, Lt, Lte, Gt, Gte,
    // Logical
    And, Or, Not,
    // Bitwise
    BAnd, BOr, BXor, Negate,
    // Shift
    Lsh, Rsh,
    // Memory
    Load, Store,
    // Control flow
    Jmp, Cmp, Return, Assign,
    // Values
    Const, Cast, Free,
    // Calls
    Call,
    // Ternary
    Select,
};

} // namespace nautilus
```

Both `TraceOperation` and IR `Operation` subclasses use this enum.
The `TraceToIRConversionPhase` switch becomes a structural transformation
(trace blocks → IR basic blocks) rather than an opcode-mapping exercise.

### Layer 3: Trace Backend Interface

Abstract the recording mechanism behind an interface:

```cpp
// include/nautilus/TraceBackend.hpp
namespace nautilus {

class TraceBackend {
public:
    virtual ~TraceBackend() = default;

    // Value creation
    virtual TypedValueRef& recordConstant(Type type, const ConstantLiteral& value) = 0;
    virtual TypedValueRef  recordCopy(const TypedValueRef& source) = 0;

    // Operations
    virtual TypedValueRef& recordBinaryOp(OpCode op, Type resultType,
                                          const TypedValueRef& left,
                                          const TypedValueRef& right) = 0;
    virtual TypedValueRef& recordUnaryOp(OpCode op, Type resultType,
                                         const TypedValueRef& input) = 0;
    virtual TypedValueRef& recordTernaryOp(OpCode op, Type resultType,
                                           const TypedValueRef& a,
                                           const TypedValueRef& b,
                                           const TypedValueRef& c) = 0;

    // Control flow
    virtual bool recordBoolConversion(const TypedValueRef& state, double probability) = 0;
    virtual void recordAssignment(const TypedValueRef& target,
                                  const TypedValueRef& source, Type type) = 0;
    virtual void recordReturn(Type type, const TypedValueRef& ref) = 0;

    // Function interface
    virtual TypedValueRef& registerArgument(Type type, size_t index) = 0;
    virtual TypedValueRef& recordCall(void* fnptr, Type resultType,
                                      const std::vector<TypedValueRef>& args,
                                      FunctionAttributes attrs) = 0;

    // Lifetime
    virtual void allocateRef(ValueRef ref) = 0;
    virtual void freeRef(ValueRef ref) = 0;
};

// Thread-local access (nullptr when not tracing)
TraceBackend* getActiveBackend();
bool isTracing();

} // namespace nautilus
```

The existing `TraceContext` becomes the concrete implementation.
The `TracingUtil.hpp` free functions become thin wrappers:

```cpp
// TracingUtil.cpp - becomes a simple delegation layer
TypedValueRef& traceBinaryOp(OpCode op, Type resultType,
                             const TypedValueRef& left, const TypedValueRef& right) {
    return getActiveBackend()->recordBinaryOp(op, resultType, left, right);
}
```

### Layer 4: TracingMixin for val<T>

Eliminate `#ifdef ENABLE_TRACING` duplication via a CRTP mixin or conditional member:

```cpp
// include/nautilus/val_state.hpp
namespace nautilus {

#ifdef ENABLE_TRACING
struct ValState {
    TypedValueRefHolder state;

    ValState(TypedValueRef ref) : state(ref) {}
    ValState(const ValState& other) : state(traceCopy(other.state)) {}
    ValState(ValState&& other) noexcept : state(std::move(other.state)) {}

    operator const TypedValueRef&() const { return state; }
};
#else
struct ValState {
    // Empty — zero cost when tracing disabled (EBO applies)
    ValState() = default;
    ValState(TypedValueRef) {}
    ValState(const ValState&) = default;
    ValState(ValState&&) = default;
};
#endif

} // namespace nautilus
```

Then `val<T>` becomes:

```cpp
template <typename ValueType>
    requires is_arithmetic<ValueType>
class val<ValueType> {
public:
    using raw_type = ValueType;
    using basic_type = ValueType;

    val() : state_(traceConstant<raw_type>(0)), value_(0) {}
    val(ValueType value) : state_(traceConstant(value)), value_(value) {}
    val(const val& other) : state_(other.state_), value_(other.value_) {}
    // ... single set of constructors, no #ifdef

private:
    [[no_unique_address]] ValState state_;  // Zero-cost when tracing disabled
    ValueType value_;
};
```

`[[no_unique_address]]` ensures `ValState` takes zero bytes when it's empty.

## Implementation Phases

### Phase 1: Extract Core Types (non-breaking)

**Files to change:**
- Create `include/nautilus/OpCode.hpp` with unified operation codes
- Move `Type`, `TypeResolver<T>`, `ConstantLiteral` declarations from
  `include/nautilus/tracing/Types.hpp` into `include/nautilus/core.hpp` or a new
  `include/nautilus/TypeSystem.hpp`
- Add `using` aliases in `tracing/Types.hpp` for backward compatibility
- Update `ir/operations/Operation.hpp` to include from new location

**Risk:** Low — purely additive with backward-compatible aliases.

### Phase 2: Unify OpCode (moderate changes)

**Files to change:**
- `include/nautilus/tracing/Operations.hpp` — replace `tracing::Op` with `nautilus::OpCode` (or alias)
- `src/nautilus/compiler/ir/operations/Operation.hpp` — add `OpCode` mapping to each OperationType
- `src/nautilus/tracing/phases/TraceToIRConversionPhase.cpp` — simplify the switch
- `val_arith.hpp`, `val_bool.hpp` — use new OpCode names in operator macros

**Risk:** Medium — touches many files but is a mechanical rename.

### Phase 3: Introduce TraceBackend Interface (moderate changes)

**Files to create:**
- `include/nautilus/TraceBackend.hpp` — abstract interface
- `src/nautilus/tracing/TraceBackendImpl.cpp` — concrete implementation wrapping TraceContext

**Files to change:**
- `include/nautilus/tracing/TracingUtil.hpp` — delegate through interface
- `src/nautilus/tracing/TracingUtil.cpp` — delegate through interface
- `src/nautilus/tracing/TraceContext.hpp` — implement TraceBackend

**Risk:** Medium — changes are behind the existing free-function API, so val<T> doesn't change yet.

### Phase 4: Simplify val<T> Conditional Compilation (most impactful)

**Files to create:**
- `include/nautilus/val_state.hpp` — ValState wrapper

**Files to change:**
- `include/nautilus/val_arith.hpp` — use ValState, remove #ifdef duplication
- `include/nautilus/val_bool.hpp` — use ValState, remove #ifdef duplication
- `include/nautilus/val_ptr.hpp` — use ValState, remove #ifdef duplication
- `include/nautilus/val_enum.hpp` — use ValState, remove #ifdef duplication
- `include/nautilus/val_details.hpp` — update StateResolver for new state member

**Risk:** Higher — changes the public type layout. Requires thorough testing.

## Benefits Summary

| Problem | Solution | Benefit |
|---------|----------|---------|
| val<T> depends on tracing internals | TraceBackend interface + core type extraction | Clean dependency direction |
| Two operation enums in sync | Unified OpCode | Single source of truth |
| #ifdef duplicates every constructor | ValState + [[no_unique_address]] | Each constructor written once |
| Type/TypeResolver in tracing:: namespace | Move to nautilus:: core | Correct layering |
| Can't mock trace recording | TraceBackend interface | Testability, alternative backends |
| Large switch in TraceToIRConversion | Shared OpCode | Simpler conversion phase |
| Hard to add new operation | Single enum + single interface method | One place to add |
