# Exception-Safe Cleanup for Nautilus Struct Values

## Status

Approved design. This document specifies how Nautilus destroys fully constructed `val<T>` class values when a
runtime call, indirect call, or nested Nautilus call throws.

## 1. Objective

When a call throws, every active `val<T>` with a non-trivial destructor must be destroyed exactly once, in reverse
construction order, before the original exception continues.

The implementation must preserve these constraints:

- Nautilus IR calls remain ordinary, non-terminating `CallOperation`s.
- No landing pad is emitted unless a potentially throwing call has at least one active destructor.
- Calls whose target is `noexcept` produce no exceptional edge.
- MLIR/LLVM uses native exception handling.
- Other backends capture exceptions, clean up through ordinary control flow, return to a safe C++ boundary, and
  rethrow there.
- Standalone lifetime-marker instructions are not added to Nautilus IR.

Nautilus functions do not gain `try` or `catch` syntax. This feature provides cleanup and propagation only.

## 2. Pipeline

The feature enters the compilation pipeline in five stages:

1. `val<T>` tracing records destructor metadata and cleanup effects.
2. Trace-to-IR conversion preserves that metadata on existing operations.
3. The existing Nautilus IR optimization pipeline runs normally.
4. A backend-neutral analysis computes the active cleanup state at each call.
5. The selected backend materializes native exception handling or the portable fallback.

Cleanup CFG is generated only after normal Nautilus IR optimization. The Nautilus optimizer therefore continues to
see straight-line calls and does not need to model LLVM exception terminators.

## 3. Cleanup representation

### 3.1 Allocation descriptors

Extend the per-function alloca table with an optional destructor descriptor:

```cpp
struct DestructorSpec {
	void* function_ptr;
	std::string function_name;
	FunctionAttributes attributes;
};

struct AllocaSpec {
	size_t size;
	size_t align;
	std::optional<DestructorSpec> destructor;
};
```

`destructor` is absent for trivially destructible types. The target is the existing typed wrapper around
`ptr->~ValueType()`, not a raw C++ ABI destructor. This gives every backend the existing `void(ValueType*)` call
shape.

The trace-side `AllocaSpec` and IR-side `AllocaSpec` must carry equivalent information. Trace-to-IR conversion copies
the complete table onto `FunctionOperation`.

### 3.2 Cleanup effects

An existing operation can carry one cleanup-state transition:

```cpp
enum class CleanupEffectKind {
	ActivateAfterSuccess,
	DeactivateBeforeCall,
};

struct CleanupEffect {
	CleanupEffectKind kind;
	AllocaIndex alloca;
};
```

This is semantic metadata, not an executable operation.

Construction is represented as:

```text
$1 = alloca[0]
call construct($1)       cleanup: activate[0] after success
```

The allocation is absent from the constructor call's unwind state. It becomes active only on the normal
continuation. A throwing constructor therefore never destroys the object whose construction failed.

Destruction is represented as:

```text
call destruct($1)        cleanup: deactivate[0] before call
```

The object is removed before its destructor is called. A throwing destructor cannot recursively schedule itself for
cleanup.

For a trivially constructed but non-trivially destructible type, activation is attached to `AllocaOperation`, because
there is no constructor call.

## 4. Tracing integration

### 4.1 Cleanup handles

In tracing mode, `val<ValueType>` owns a cleanup handle alongside its traced pointer:

```cpp
struct CleanupHandle {
	AllocaIndex alloca;
};
```

The internal allocation helper conceptually returns both values:

```cpp
struct TracedAllocation {
	TypedValueRef pointer;
	CleanupHandle cleanup;
};
```

The concrete API may use references or a lightweight internal result type, but the allocation index must reach
`val<ValueType>` without reconstructing it from function names or neighboring trace operations.

### 4.2 Construction and destruction

`val<ValueType>` uses an internal invoke path that accepts a cleanup effect. Non-trivial construction attaches
`ActivateAfterSuccess`; destruction attaches `DeactivateBeforeCall`. Ordinary public `invoke()` remains unchanged for
callers that do not manage `val<T>` storage.

The tracer records the cleanup effect on the same `TraceOperation` as the call. Trace-to-IR conversion copies it to
the resulting `ProxyCallOperation`, `IndirectCallOperation`, or internal-call representation.

### 4.3 Moves

The cleanup handle follows the underlying allocation:

- Move construction transfers the pointer and handle; the moved-from object emits no cleanup effect or destructor.
- Move assignment first deactivates and destroys the destination's old allocation, then transfers the source handle.
- Deactivation removes a specific `AllocaIndex`, not necessarily the newest entry. This is required when move
  assignment destroys an older object while a newer source object remains active.
- The transferred allocation remains active; moving ownership does not create a second activation.

## 5. Automatic `noexcept` inference

`CallableRuntimeFunction` must retain enough of the exact callable type to evaluate:

```cpp
std::is_nothrow_invocable_v<Callable, RawArguments...>
```

The inferred value is combined with explicit attributes:

```cpp
attributes.noUnwind = attributes.noUnwind || inferred_noexcept;
```

Inference applies to:

- raw function pointers, including `R (*)(Args...) noexcept`;
- lambdas and functors wrapped by `NautilusFunction`;
- typed indirect function pointers whose type retains `noexcept`.

`std::function` and untyped dynamic targets do not retain a `noexcept` contract. Those require explicit
`FunctionAttributes::noUnwind` when appropriate.

An explicit `noUnwind=true` remains a trusted user contract. `willReturn` and `noUnwind` remain independent.

## 6. Cleanup-state analysis

Add a backend-neutral, read-only analysis over the final optimized `IRGraph`:

```cpp
using CleanupStateId = uint32_t;

struct CleanupState {
	std::vector<AllocaIndex> active;
};

struct FunctionCleanupPlan {
	std::vector<CleanupState> states;
	std::unordered_map<const Operation*, CleanupStateId> unwind_state;
};
```

The plan is an analysis result. It does not mutate the Nautilus CFG.

### 6.1 Transfer rules

The entry state is empty.

For `DeactivateBeforeCall`, remove the referenced allocation before recording the call's unwind state. Removal is by
allocation index and preserves the relative order of all remaining entries.

For any potentially throwing call, record the current state as its unwind state.

For `ActivateAfterSuccess`, append the referenced allocation only to the normal successor state. The unwind state
remains unchanged.

Calls marked `noUnwind` do not need an unwind-state entry, but their cleanup effects still update the normal state.
This matters for `noexcept` constructors.

### 6.2 CFG invariants

Every predecessor of a merge block must produce the same ordered cleanup sequence. C++ lexical scopes should make
valid traces satisfy this naturally. A mismatch indicates an unbalanced traced lifetime and is a compilation error.

A loop backedge must produce the same state expected at the loop header. Objects constructed and destroyed inside an
iteration are therefore balanced before the next iteration unless a future feature explicitly models a loop-carried
owner.

Normal returns must have an empty active-cleanup state. A future ownership-escape representation may relax this for
supported struct return-by-value cases; this feature does not infer escapes implicitly.

### 6.3 Landing-pad eligibility

A native landing pad is required at a call exactly when:

```cpp
!call.attributes.noUnwind && !cleanup_plan.unwindState(call).active.empty()
```

Consequences:

- no destructible allocation means no landing pad;
- destructible allocations with no potentially throwing calls mean no landing pad;
- only `noexcept` calls while objects are active mean no landing pad;
- a constructor's unwind state excludes the object under construction;
- a destructor's unwind state excludes that object but includes remaining outer objects.

## 7. Native MLIR/LLVM lowering

### 7.1 Lowering position

The initial MLIR lowering continues to emit ordinary calls. Calls selected by the cleanup plan receive private
call-site and cleanup-state attributes.

After Func-to-LLVM conversion, a new `MaterializeCppExceptionHandlingPass` rewrites the marked `LLVM::CallOp`s. If
Func-to-LLVM does not preserve the private attributes, Nautilus supplies a conversion pattern that copies them. The
pass runs before MLIR-to-LLVM-IR translation and LLVM optimization.

This placement ensures that `LLVM::InvokeOp`, `LLVM::LandingpadOp`, and `LLVM::ResumeOp` are created in LLVM dialect
without exposing them to Nautilus IR passes.

### 7.2 Call rewriting

A marked call becomes an invoke with a unique normal continuation:

```llvm
%result = invoke @writeStructOrThrow(...)
    to label %normal.cont
    unwind label %cleanup.state.1
```

Only calls that actually need cleanup split an MLIR/LLVM block. One straight Nautilus block may therefore map to
multiple LLVM blocks.

### 7.3 Cleanup blocks

Calls with identical ordered cleanup states share one cleanup block. The first implementation deduplicates complete
states; suffix-sharing can be added later if code size warrants it.

```llvm
cleanup.state.1:
    %exception = landingpad { ptr, i32 }
        cleanup
    call void @destruct_B(ptr %slot_b)
    call void @destruct_A(ptr %slot_a)
    resume { ptr, i32 } %exception
```

Destructors run in reverse activation order. The per-function alloca table causes every physical slot to be emitted
in the function prologue, so the slot pointers dominate every cleanup block.

### 7.4 Personality and platform model

A function receives a C++ personality only if it contains at least one landing pad. Initial native support uses the
Itanium C++ exception ABI and `__gxx_personality_v0`, covering the project's Linux and macOS targets.

The JIT must resolve the personality symbol and preserve/register the generated unwind information with the object
layer. Compilation fails with a backend diagnostic if either facility is unavailable; it must not silently emit an
unwind edge that the runtime cannot traverse.

MLIR/LLVM targets requiring a different EH model use the portable fallback until a target-specific emitter exists.

### 7.5 Throwing cleanup destructors

A destructor marked `noUnwind` is emitted as an ordinary cleanup call. A potentially throwing destructor executed
while another exception is active uses an invoke whose unwind destination terminates the process, matching C++
double-exception behavior.

## 8. Portable fallback

AsmJit, bytecode/TBC, and unsupported MLIR targets must not unwind C++ exceptions through generated frames without
suitable unwind metadata.

### 8.1 Runtime exception frames

A typed public invocation pushes a thread-local exception frame:

```cpp
struct ExceptionFrame {
	std::exception_ptr pending;
	ExceptionFrame* parent;
};
```

The structure is a TLS stack so nested and reentrant compiled calls remain isolated.

### 8.2 Captured calls

Potentially throwing runtime calls use a signature-specific C++ thunk:

```cpp
template <typename R, typename... Args>
R invokeCatching(ExceptionFrame* frame, R (*target)(Args...), Args... args) noexcept;
```

The thunk calls the real target, catches `...`, stores `std::current_exception()`, and returns an ABI-compatible zero
or default value. Generated code checks the pending flag immediately after the call.

Every potentially throwing call is captured on fallback backends, even when its cleanup state is empty, because the
exception must not cross the generated frame. Calls marked `noUnwind` remain direct and unchecked.

### 8.3 Exceptional cleanup and exit

If the pending flag is set, generated code branches to the cleanup block for the call's state. The block invokes
active destructors in reverse order and then returns an ABI-compatible default result from the generated function.

A destructor throwing while a primary exception is pending terminates the process. A destructor that throws on the
normal path becomes the primary pending exception after its own `DeactivateBeforeCall` effect has been applied; outer
objects are still cleaned.

Nested generated Nautilus calls share the current exception frame. A nested callee returns with the flag set, and its
generated caller observes the flag immediately after the call and performs its own cleanup.

### 8.4 Safe rethrow boundary

`Executable::Invocable` owns the outer exception-frame guard. After the generated function returns, it rethrows a
pending exception from ordinary C++ code.

`ModuleFunction` must route compiled calls through this exception-aware wrapper instead of caching and directly
calling a raw backend pointer whenever the executable requires the fallback.

The typed Engine and `CompiledModule` invocation APIs guarantee fallback propagation. Direct use of the low-level
`getInvocableFunctionPtr()` is outside this guarantee because it bypasses the safe rethrow boundary.

## 9. Example lowering

The source:

```cpp
nautilus::val<StructResult> result;
nautilus::invoke(writeStructOrThrow, &result, nautilus::val<int32_t> {42});
const auto value = result.get(&StructResult::value);
```

has straight Nautilus IR:

```text
entry:
    struct = ALLOCA[0]
    CALL construct(struct)              activate[0] after success
    CALL writeStructOrThrow(struct, 42)
    value = LOAD struct.value
    STORE output, value
    CALL destruct(struct)               deactivate[0] before call
    RETURN
```

Native LLVM lowering is conceptually:

```text
entry:
    struct = alloca
    call construct(struct)
    invoke writeStructOrThrow(struct, 42)
        normal -> continuation
        unwind -> cleanup_0

continuation:
    value = load struct.value
    call destruct(struct)
    return value

cleanup_0:
    exception = landingpad cleanup
    call destruct(struct)
    resume exception
```

The load and output store do not execute when the runtime call throws.

## 10. Verification

The IR verifier and cleanup analysis enforce:

- every cleanup effect references an existing `AllocaSpec`;
- only an allocation with a destructor may be activated;
- an allocation cannot be activated twice without deactivation;
- deactivation requires the allocation to be active;
- CFG predecessors agree on the complete ordered cleanup state;
- loop backedges preserve the loop-header cleanup state;
- normal returns do not retain active local cleanups;
- destructor descriptors accept the allocation pointer and return `void`;
- cleanup-state IDs consumed by lowering exist in the function plan;
- `noUnwind` calls never receive an exceptional successor.

Diagnostics identify the function, block, operation, and allocation index.

## 11. Optimization interaction

Cleanup effects travel with their owning operation:

- block merging preserves operation order and therefore cleanup-effect order;
- branch folding removes effects in deleted branches;
- calls carrying cleanup effects remain side-effecting and cannot be eliminated or commoned;
- an allocation with a registered destructor is not dead merely because its stored value is unused;
- cleanup analysis runs after every Nautilus CFG mutation;
- backend-generated cleanup blocks do not re-enter Nautilus optimization;
- LLVM may optimize native cleanup CFG while preserving LLVM EH semantics.

## 12. Testing

### 12.1 Type and tracing tests

- Infer `noUnwind` from a `noexcept` runtime function pointer.
- Infer `noUnwind` from a `noexcept` Nautilus lambda or functor.
- Require explicit attributes for `std::function`.
- Record activation after successful construction.
- Record deactivation before destruction.
- Transfer one cleanup handle during move construction.
- Remove and transfer the correct allocations during move assignment.

### 12.2 Cleanup-analysis tests

- One active object.
- Multiple objects with reverse cleanup order.
- A throwing constructor excludes the object under construction.
- A throwing destructor excludes itself but includes outer objects.
- Branch-local values agree at merges.
- Loop-local values are balanced at backedges.
- Non-LIFO deactivation caused by move assignment.
- Invalid merge state produces a precise diagnostic.

### 12.3 MLIR/LLVM reference tests

- No destructor produces an ordinary call, no personality, and no landing pad.
- A destructor plus only `noexcept` calls produces no landing pad.
- A destructor plus a throwing call produces invoke, cleanup landing pad, and resume.
- Calls with the same cleanup sequence share a cleanup block.
- Different cleanup sequences use different blocks.
- Destructors execute in reverse order.
- A constructor's unwind edge excludes its own destructor.
- A potentially throwing cleanup destructor targets a terminate pad.

### 12.4 Cross-backend execution tests

For every enabled backend:

- the original exception type and message reach the typed caller;
- each active destructor executes exactly once;
- multiple destructors run in reverse order;
- a throwing constructor does not run its own destructor;
- earlier objects are destroyed when a later constructor throws;
- operations after the throwing call do not execute;
- nested Nautilus calls clean both callee and caller objects;
- loop-local and branch-local objects are cleaned correctly;
- `noexcept` calls use the direct path;
- nested and reentrant calls isolate exception frames.

A subprocess or death test verifies that a destructor throwing during cleanup of an active exception terminates.

## 13. Scope and non-goals

This feature includes runtime calls, indirect calls, nested Nautilus calls, non-trivial class destructors, native
Itanium EH through MLIR/LLVM, and the portable fallback.

The initial implementation does not include:

- `try`, `catch`, or exception inspection inside Nautilus functions;
- throwing a new exception directly from Nautilus IR;
- Windows-native LLVM EH pads;
- fallback exception propagation through raw backend function pointers;
- changes to existing struct layout, field access, or virtual-dispatch limitations.
