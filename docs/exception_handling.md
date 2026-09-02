# Exception Handling

## Overview

Nautilus functions are traced and JIT-compiled. When a function called from a
traced Nautilus function throws, Nautilus:

1. runs the destructors of every live `val<T>` with a non-trivial destructor,
   in reverse construction order;
2. propagates the exception to the host code that invoked the compiled
   function, where it is rethrown as the original C++ exception.

Nautilus functions do not gain `try`/`catch` syntax. This feature provides
cleanup and propagation only.

## The user-facing contract

- **Potentially-throwing calls**: a call is treated as potentially throwing
  when the function pointer you pass to `invoke()` is **not** `noexcept`.
- **`noexcept` calls stay on the direct path**: a function declared
  `noexcept` (or invoked through a `noexcept` function pointer) is never
  routed through exception handling — no capture, no pending-exception check,
  no overhead.
- **Cleanup on throw**: on the exceptional path, every live `val<T>` with a
  non-trivial destructor is destroyed exactly once, in reverse construction
  order.
- **Rethrow at the host boundary**: the original exception object is rethrown
  where you invoke the compiled function through the typed invocation API
  (`Executable::Invocable`). Use that API if you need to observe exceptions.

```cpp
struct Guard {
    ~Guard() noexcept { /* cleanup */ }
};

void throwingWork(int32_t*) {
    throw std::runtime_error("boom");
}

val<int32_t> tracedFn() {
    val<Guard> guard;                 // non-trivial destructor
    invoke(throwingWork, &guard);     // potentially-throwing call
    return 42;
}

// host side:
auto fn = engine.registerFunction(tracedFn);
try {
    fn();
} catch (const std::runtime_error& e) {
    // `guard`'s destructor already ran; the original exception is rethrown here.
}
```

## How it works

- **Tracing**: each potentially-throwing call site records the set of live
  destructor-bearing allocations as a `CALL_WITH_EXCEPTION_HANDLING`
  operation.
- **IR**: a terminal `ExceptionRegionPreparationPass` turns the per-call
  destructor lists into shared, backend-neutral landing-pad blocks.
- **Transports**: each backend picks one of two propagation strategies —
  native unwinding (MLIR) or captured host-rethrow (all other compiled
  backends).

## Backend differences

Every backend implements exception handling. The compiled backends use one of
two transports — native unwinding or captured host-rethrow:

| Backend | Transport | Notes |
|---|---|---|
| **MLIR** (native JIT) | Native unwind | Lowered to LLVM `invoke` / `landingpad` / `resume` with `__gxx_personality_v0`. Real unwinder; the most efficient path. Pad-less calls (no live destructors) are lowered as plain calls — no personality or landing pad — since the exception propagates natively through the unwindable LLVM frame. |
| **CPP** (source backend) | Captured host-rethrow | The emitted C++ source routes the call through a capture thunk and a pending-exception check. |
| **BC** (bytecode interpreter) | Captured host-rethrow | Interpreter frames carry no unwind tables, so the exception is caught by a real C++ frame before it crosses the interpreter. |
| **TBC** (TBC VM) | Captured host-rethrow | Same captured transport. |
| **AsmJit** (X64 and A64 JIT) | Captured host-rethrow | JIT machine code has no unwind tables; uses the capture thunk plus a pending-exception check. Implemented for both x86-64 and AArch64. |
| **Interpreter** (no compilation) | Native C++ unwinding | The function runs directly, so exceptions propagate through the host C++ runtime unchanged. |

The captured host-rethrow transport works like this: the call is routed
through `captureThrowingCall<R, Args...>` — a real C++ frame — which catches
the exception and stores it in a thread-local `ExceptionFrame`. The generated
code then checks the frame's pending flag, branches to the landing pad (running
destructors in reverse order), returns an ABI-compatible default value, and the
`Executable` boundary rethrows the stored exception. Nested Nautilus calls
share the frame, so an inner throw unwinds through the outer function's
cleanup as well.

## Notes and limitations

- Only the typed invocation APIs (`Executable::Invocable`) guarantee safe
  rethrow. Raw `getInvocableFunctionPtr()` calls bypass the
  capture/rethrow boundary.
- Destructors are treated as `noUnwind`; a destructor that throws during
  unwind would terminate the process.
- No `try`/`catch` or exception inspection inside Nautilus functions.
