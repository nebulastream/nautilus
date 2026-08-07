# Exception-Safe Cleanup for Nautilus Struct Values

## Status

Approved design, updated for written review. This document specifies how Nautilus destroys fully constructed
`val<T>` class values when a runtime call, indirect call, or nested Nautilus call throws. A terminal Nautilus IR pass
creates one shared logical exception region per function; backends implement only the transport into and out of that
region.

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

1. `val<T>` tracing registers destructor metadata and updates a tracer-owned ordered active-cleanup state.
2. Trace snapshots include the current cleanup-state identity, and potentially throwing calls retain that state as
   their logical unwind state.
3. Trace-to-IR conversion copies cleanup-state metadata only onto call operations; the existing Nautilus IR
   optimization pipeline then runs normally.
4. A terminal backend-neutral finalizer collects surviving potentially throwing calls, deduplicates their logical
   cleanup pads, and attaches an immutable exception region.
5. The selected backend materializes native exception handling or captured propagation from that shared region.

The exception region is generated only after normal Nautilus IR optimization. The ordinary Nautilus CFG therefore
continues to contain straight-line calls and does not model LLVM exception terminators or hidden exceptional
predecessors. No CFG-mutating Nautilus pass may run after exception-region preparation without first invalidating and
recomputing the region.

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

### 3.2 Interned cleanup states

The tracer owns the current ordered sequence of fully constructed allocations with non-trivial destructors:

```cpp
using CleanupStateId = uint32_t;

struct CleanupState {
	std::vector<AllocaIndex> active;
};
```

States are interned in `ExecutionTrace`. Equal ordered sequences receive the same `CleanupStateId`; the empty state
has a canonical ID. Activation appends an allocation. Deactivation removes a specific `AllocaIndex` while preserving
the relative order of all remaining allocations. The representation is therefore an ordered active list rather than
a strict stack, which is required for move assignment.

Trace-to-IR conversion copies the interned state table onto `FunctionOperation`. A direct or indirect call may carry
an optional `CleanupStateId` into optimized Nautilus IR; the ID is meaningful only relative to that function's table.

`Snapshot` contains the current `CleanupStateId` as an exact identity component in addition to the existing tag and
static/alive-value hash. Two executions reaching the same source tag with different cleanup states do not merge:
the tracer records path-specific copies until the states become equal again. This permits one source call site to
produce multiple trace and IR call operations with different cleanup pads.

Only call operations retain cleanup-state metadata after trace-to-IR conversion. A call marked `noUnwind`, or a call
whose cleanup state is empty, does not reference a cleanup pad. Other Nautilus IR operations carry no lifetime
effects or cleanup-state transitions.

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

`TracingInterface` exposes guarded activation and deactivation hooks for `val<ValueType>`. They mutate only the
tracer-owned state and do not emit trace operations:

```cpp
void activateCleanup(AllocaIndex alloca);
void deactivateCleanup(AllocaIndex alloca);
```

Construction records the constructor call while the allocation is absent, then activates it only after the traced
call returns normally:

```text
$1 = alloca[0]
call construct($1)       snapshot state excludes 0
activateCleanup(0)
```

A throwing constructor therefore never destroys the object whose construction failed. For a trivially constructed
but non-trivially destructible type, activation runs immediately after `traceAlloca` returns.

Destruction deactivates the allocation before recording its destructor call:

```text
deactivateCleanup(0)
call destruct($1)        snapshot state excludes 0
```

A throwing destructor cannot recursively schedule itself for cleanup. The hooks continue maintaining existing
lifetimes while lazy tracing is passive and while exception-based tracing unwinds an internal trace-termination
exception. Each tracing iteration resets the current state to empty and reconstructs it by replaying the same C++
lifetime hooks.

### 4.3 Moves

The cleanup handle follows the underlying allocation:

- Move construction transfers the pointer and handle; the moved-from object emits no deactivation or destructor.
- Move assignment first deactivates and destroys the destination's old allocation, then transfers the source handle.
- Deactivation removes a specific `AllocaIndex`, not necessarily the newest entry. This is required when move
  assignment destroys an older object while a newer source object remains active.
- The transferred allocation remains active; moving ownership does not change the tracer state.

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

The internal construction and destruction wrappers preserve the corresponding type trait in their declarations:

```cpp
static void construct(ValueType*) noexcept(std::is_nothrow_default_constructible_v<ValueType>);
static void destruct(ValueType*) noexcept(std::is_nothrow_destructible_v<ValueType>);

template <typename... Args>
static void construct_with(ValueType*, Args...)
    noexcept(std::is_nothrow_constructible_v<ValueType, Args...>);
```

Backend intrinsic replacements must be `noUnwind` in the initial implementation. A potentially throwing intrinsic is
lowered as an ordinary call or rejected with a backend diagnostic; it cannot consume an exceptional call-site record
and then erase the physical call.

## 6. Terminal exception-region finalization pass

`ExceptionCleanupPreparationPass` remains the final pass over the optimized `IRGraph`, but it no longer performs
lifetime dataflow. Its responsibility is to collect live call metadata, canonicalize pads, and attach an immutable
region to each `FunctionOperation`:

```cpp
using CleanupPadId = uint32_t;

struct CleanupPad {
	CleanupPadId id;
	std::vector<AllocaIndex> active;
};

struct ExceptionalCallSite {
	const Operation* call;
	std::optional<CleanupPadId> cleanup;
};

struct FunctionExceptionRegion {
	std::vector<CleanupPad> pads;
	std::vector<ExceptionalCallSite> call_sites;
};
```

The pass visits only surviving `ProxyCallOperation` and `IndirectCallOperation` nodes. For every potentially throwing
call it creates one `ExceptionalCallSite`. If the call references a non-empty trace cleanup state, the pass looks up
that ordered allocation sequence on `FunctionOperation`, interns it into a dense `CleanupPadId`, and associates the
pad with the call. Calls with identical ordered states share a pad.

Calls removed by CFG optimization contribute neither call sites nor pads. This is why finalization remains terminal
even though active-lifetime computation has moved into tracing. A potentially throwing call remains in `call_sites`
when `cleanup` is absent, because a captured-propagation backend must still prevent the exception from crossing its
generated frame.

The region is an exceptional side table, not part of `FunctionOperation::getBasicBlocks()`. Ordinary dominance,
predecessor, loop, and block-rewriting utilities therefore remain unchanged.

### 6.1 Trace specialization and convergence

The entry cleanup state is empty. Each activation or deactivation interns the new ordered state and updates the
current `CleanupStateId`. `recordSnapshot()` includes that ID in `Snapshot` equality and hashing.

If two paths reach the same source operation with states `[A]` and `[B]`, they remain distinct trace paths:

```text
left:  call throwing()  cleanup [A]
right: call throwing()  cleanup [B]
```

The call is copied into both trace/IR paths. Subsequent operations remain specialized while the states differ, and
normal trace merging resumes once both paths reach the same ordered state. `FOLLOW` mode verifies that the replayed
current state equals the cleanup state stored in the operation's snapshot.

A loop backedge must eventually reproduce the cleanup state recorded for the loop header. A changing state must not
cause unbounded trace specialization; the tracer reports cleanup-state drift with the loop/source tag instead.
Normal returns require the empty cleanup state.

### 6.2 Propagation and cleanup eligibility

The finalizer and backend use separate predicates:

```cpp
mayThrow = !call.attributes.noUnwind;
needsCleanupPad = mayThrow && call.cleanupState != emptyCleanupState;
needsCapture = mayThrow && backend.exceptionPropagationMode() == CapturedHostRethrow;
```

Consequences:

- no destructible allocation means no landing pad;
- destructible allocations with no potentially throwing calls mean no landing pad;
- only `noexcept` calls while objects are active mean no landing pad;
- a constructor's unwind state excludes the object under construction;
- a destructor's unwind state excludes that object but includes remaining outer objects.

On a native-unwind backend, a call with no active destructor remains an ordinary call and unwinds through the
generated frame normally. On a captured-propagation backend, the same call still uses a catching thunk and immediate
pending-exception check, but branches directly to exceptional function exit rather than to a cleanup pad.

## 7. Native-unwind lowering

Backends expose their transport capability explicitly:

```cpp
enum class ExceptionPropagationMode {
	NativeUnwind,
	CapturedHostRethrow,
};
```

MLIR/LLVM and generated C++ use `NativeUnwind`. TBC JIT and AsmJit use `CapturedHostRethrow` until their generated
frames have registered unwind metadata. Interpreters may use native propagation only when no exception crosses a
foreign-call or generated callback frame.

### 7.1 MLIR/LLVM lowering

The initial MLIR lowering continues to emit ordinary calls. Calls selected by the shared exception region receive
private call-site and cleanup-pad attributes.

After Func-to-LLVM conversion, a new `MaterializeCppExceptionHandlingPass` rewrites the marked `LLVM::CallOp`s. If
Func-to-LLVM does not preserve the private attributes, Nautilus supplies a conversion pattern that copies them. The
pass runs before MLIR-to-LLVM-IR translation and LLVM optimization.

This placement ensures that `LLVM::InvokeOp`, `LLVM::LandingpadOp`, and `LLVM::ResumeOp` are created in LLVM dialect
without exposing them to Nautilus IR passes.

### 7.2 MLIR call rewriting

A marked call becomes an invoke with a unique normal continuation:

```llvm
%result = invoke @writeStructOrThrow(...)
    to label %normal.cont
    unwind label %cleanup.state.1
```

Only calls that actually need cleanup split an MLIR/LLVM block. One straight Nautilus block may therefore map to
multiple LLVM blocks.

### 7.3 MLIR cleanup blocks

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

### 7.5 Generated C++ lowering

The C++ backend emits native `try`/`catch (...)` dispatch using the same logical pads. After a pad destroys its active
objects, it uses `throw;` to preserve the original exception. The generated source must not synthesize a replacement
exception.

### 7.6 Throwing cleanup destructors

A destructor marked `noUnwind` is emitted as an ordinary cleanup call. A potentially throwing destructor executed
while another exception is active uses an invoke whose unwind destination terminates the process, matching C++
double-exception behavior.

## 8. Captured host-rethrow transport

AsmJit, bytecode/TBC, and unsupported MLIR targets must not unwind C++ exceptions through generated frames without
suitable unwind metadata.

### 8.1 Runtime exception frames

A typed public invocation for an executable using `CapturedHostRethrow` pushes a thread-local exception frame:

```cpp
struct ExceptionFrame {
	std::exception_ptr pending;
	ExceptionFrame* parent;
};
```

The structure is a TLS stack so nested and reentrant compiled calls remain isolated. A pending primary exception is
never overwritten. Cleanup-destructor failure terminates instead of storing a second exception.

### 8.2 Captured calls

Potentially throwing runtime calls use a signature-specific C++ thunk:

```cpp
template <typename R, typename... Args>
R invokeCatching(ExceptionFrame* frame, R (*target)(Args...), Args... args) noexcept;
```

The thunk directly calls the real C++ target, catches `...`, stores `std::current_exception()`, and returns an
ABI-compatible zero or default value. The target must be invoked inside this typed C++ thunk: wrapping libffi,
dyncall, or another C foreign-call layer is insufficient because the exception would already have crossed frames
that are not guaranteed to support C++ unwinding. Generated code checks the pending flag immediately after the call
and before consuming the sentinel result.

Every potentially throwing call is captured on fallback backends, even when its cleanup state is empty, because the
exception must not cross the generated frame. Calls marked `noUnwind` remain direct and unchecked.

### 8.3 Exceptional cleanup and exit

If the pending flag is set, generated code branches to the call site's logical cleanup pad, or directly to exceptional
exit when the call site has no pad. The pad invokes active destructors in reverse order and then returns an
ABI-compatible default result from the generated function.

A destructor throwing while a primary exception is pending terminates the process. A destructor that throws on the
normal path becomes the primary pending exception after the allocation has been removed from the tracer state; outer
objects are still cleaned.

Nested generated Nautilus calls share the current exception frame. A nested callee returns with the flag set, and its
generated caller observes the flag immediately after the call and performs its own cleanup.

### 8.4 Safe rethrow boundary

`Executable::Invocable` owns the outer exception-frame guard when the executable reports `CapturedHostRethrow`.
After the generated function returns, it rethrows a pending exception from ordinary C++ code.

`ModuleFunction` must route compiled calls through this exception-aware wrapper instead of caching and directly
calling a raw backend pointer whenever the executable reports `CapturedHostRethrow`. Nested generated calls share the
current frame and check it immediately after their callee returns.

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
    CALL construct(struct)
    CALL writeStructOrThrow(struct, 42) cleanup_state[0]
    value = LOAD struct.value
    STORE output, value
    CALL destruct(struct)
    RETURN
```

The activation after `construct` and deactivation before `destruct` are tracing-time state changes and are not
Nautilus IR instructions or operation effects.

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

The tracer, finalizer, and IR verifier enforce:

- only an existing `AllocaSpec` with a destructor may be activated;
- an allocation cannot be activated twice without deactivation;
- deactivation requires the allocation to be active;
- trace snapshots with different ordered cleanup states remain specialized rather than merging;
- `FOLLOW` mode reproduces the cleanup state recorded in each source operation's snapshot;
- loop backedges reproduce the loop-header cleanup state instead of specializing without bound;
- normal returns do not retain active local cleanups;
- destructor descriptors accept the allocation pointer and are `noUnwind`;
- cleanup-state IDs referenced by calls exist in the function state table;
- every cleanup-pad allocation references an existing destructible `AllocaSpec`;
- every potentially throwing call occurs exactly once in the finalized exception region;
- a captured call result is not consumed before its pending-exception check;
- `noUnwind` calls never receive an exceptional successor.

Diagnostics identify the function, block, operation, and allocation index.

## 11. Optimization interaction

- Cleanup-state transitions finish during tracing and do not participate in Nautilus IR optimization.
- Potentially throwing calls retain an immutable `CleanupStateId`; existing call side-effect rules prevent their
  elimination or commoning.
- Branch and unreachable-block folding may delete calls. Terminal exception-region finalization therefore collects
  only call operations that remain in the final CFG and drops unused cleanup states.
- An allocation referenced by a surviving cleanup state is not dead merely because its normal-path value is unused.
- Exception-region finalization runs once after the final Nautilus CFG mutation.
- The immutable exception region does not re-enter Nautilus optimization.
- LLVM may optimize native cleanup CFG while preserving LLVM EH semantics.

## 12. Testing

### 12.1 Type and tracing tests

- Infer `noUnwind` from a `noexcept` runtime function pointer.
- Infer `noUnwind` from a `noexcept` Nautilus lambda or functor.
- Require explicit attributes for `std::function`.
- Activate cleanup state only after successful construction.
- Deactivate cleanup state before recording destruction.
- Transfer one cleanup handle during move construction.
- Remove and transfer the correct allocations during move assignment.
- Include exact `CleanupStateId` in snapshot equality and hashing.
- Produce path-specific copies of the same source call while cleanup states differ.
- Merge subsequent trace operations once the cleanup states converge.
- Reject loop cleanup-state drift without unbounded specialization.

### 12.2 Exception-region finalization tests

- One active object.
- Multiple objects with reverse cleanup order.
- A throwing constructor excludes the object under construction.
- A throwing destructor excludes itself but includes outer objects.
- Path-specific call copies retain distinct cleanup states.
- Calls with equal cleanup states share a finalized pad.
- Non-LIFO deactivation caused by move assignment.
- Calls deleted by CFG optimization do not leave call sites or pads.
- Invalid state IDs and alloca references produce precise diagnostics.

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
- a throwing fallback call with no active destructor propagates without creating a cleanup pad;
- captured call results are not observed on their exceptional continuation;
- concurrent invocations do not share pending exceptions;
- intrinsic replacements are `noUnwind` or remain physical calls;
- typed invocation uses the host-rethrow boundary while raw-pointer behavior remains explicitly unsupported.

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
