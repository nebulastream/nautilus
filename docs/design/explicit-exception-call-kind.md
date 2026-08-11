# Explicit Exception-Handling Call Kind

## Status

Approved design. This document is a focused amendment to
`docs/design/exception-safe-struct-cleanup.md`. The existing cleanup-state representation, terminal exception-region
pass, native MLIR lowering, and captured fallback remain authoritative except where this document makes call intent
explicit.

## Objective

Represent calls that require local exceptional cleanup explicitly without turning calls into normal-CFG
terminators or duplicating the direct, indirect, and nested call operation hierarchies.

The representation must preserve these properties:

- exact ordered cleanup state remains part of trace snapshot identity;
- a landing pad is created only for a potentially throwing call with a nonempty cleanup state;
- a potentially throwing call with no active cleanup remains an ordinary call;
- fallback backends still capture every potentially throwing call, including ordinary calls without cleanup;
- direct runtime calls, indirect calls, and nested Nautilus calls follow the same classification rules.

## Shared call kind

Introduce one shared enum used by tracing and Nautilus IR:

```cpp
enum class CallKind : uint8_t {
	Regular,
	WithExceptionHandling,
};
```

`CallKind` describes whether a call needs a local exceptional cleanup edge. It does not replace
`FunctionAttributes::noUnwind` and does not claim that a `Regular` call cannot throw.

The classification is:

```cpp
const auto kind = !attributes.noUnwind && cleanupState != EMPTY_CLEANUP_STATE
                      ? CallKind::WithExceptionHandling
                      : CallKind::Regular;
```

Therefore:

| `noUnwind` | Active cleanup | Kind | Local cleanup pad |
|---|---:|---|---:|
| `true` | no | `Regular` | no |
| `true` | yes | `Regular` | no |
| `false` | no | `Regular` | no |
| `false` | yes | `WithExceptionHandling` | yes |

## Trace representation

Keep the existing trace opcodes (`CALL` and `INDIRECT_CALL`). Add `CallKind` to `FunctionCall` and
`IndirectFunctionCall`. A nested Nautilus call continues to use `FunctionCall` and receives its inferred or declared
function attributes before classification.

The tracer computes the kind through one shared helper after recording the current exact `CleanupStateId`. A call
with `WithExceptionHandling` retains that nonempty state for trace-to-IR conversion. A `Regular` call retains no
call-local cleanup-state reference, although the operation's snapshot still contains the current cleanup state for
path identity and FOLLOW-mode validation.

The trace printer renders semantic names without adding opcodes:

```text
CALL target(args...)
CALL_WITH_EXCEPTION_HANDLING target(args...) cleanup_state[2]
INDIRECT_CALL target(args...)
INDIRECT_CALL_WITH_EXCEPTION_HANDLING target(args...) cleanup_state[3]
```

The explicit spelling is a formatting of `CallKind`; it is not a distinct tracing control-flow operation.

## Nautilus IR representation

Keep `ProxyCallOperation` and `IndirectCallOperation`. Add `CallKind` to both classes rather than adding separate
exception-handling operation classes or backend visitor methods.

Trace-to-IR conversion copies the kind and copies a cleanup-state ID only for `WithExceptionHandling`. IR printing
uses the same explicit names as trace printing.

The IR verifier enforces:

- `WithExceptionHandling` requires `noUnwind == false`;
- `WithExceptionHandling` requires a valid, nonempty cleanup-state ID;
- `Regular` carries no call-local cleanup-state ID;
- cleanup-state allocations exist and have destructor descriptors;
- destructor descriptors are `noUnwind`.

Exception-capture metadata is orthogonal to `CallKind`. It remains available on calls whose signature needs a typed
capturing thunk on fallback backends.

## Terminal exception-region preparation

The terminal pass continues to record every surviving potentially throwing call as an `ExceptionalCallSite` so a
backend can select its safe propagation mechanism.

- A potentially throwing `Regular` call receives no cleanup pad.
- A `WithExceptionHandling` call resolves its cleanup-state ID and receives an interned cleanup pad.
- A `noUnwind` call is absent from the exception region.

Calls with equal ordered cleanup states share a pad. The prepared exception region remains an immutable exceptional
side table and does not add successors to the ordinary Nautilus CFG.

## Backend behavior

### MLIR/LLVM

Only calls with `WithExceptionHandling` receive a cleanup-pad marker and become `llvm.invoke`. Their unwind
destination runs the shared ordered cleanup actions and resumes the original exception. A potentially throwing
`Regular` call remains `llvm.call`; the exception may unwind through the generated frame without local cleanup.

### Generated C++

Only `WithExceptionHandling` calls require a local `try`/`catch` cleanup sequence. A potentially throwing `Regular`
call uses ordinary C++ propagation.

### BC, TBC, TBC JIT, and AsmJit

Every call with `noUnwind == false` uses the typed exception-capture path because a C++ exception must not cross an
unsupported generated or foreign-call frame. After the call:

- `Regular` propagates pending-exception state directly to exceptional function exit;
- `WithExceptionHandling` executes its cleanup pad first and then propagates to exceptional function exit.

The safe host boundary rethrows the original `std::exception_ptr` after generated code returns.

### Nested Nautilus calls

Typed `NautilusFunction` definitions infer `noUnwind` from `std::is_nothrow_invocable_v`. Type-erased definitions
remain conservative unless explicitly attributed. Each generated function owns its own call kinds and cleanup pads.
When a nested call propagates an exception, the callee cleans its active objects before the caller cleans its own.

## Error handling

Invalid combinations are compilation errors rather than backend guesses. Diagnostics identify the function, block,
call operation, kind, and cleanup-state ID. Backends consume the prepared exception region and must not reconstruct
cleanup eligibility from allocation liveness.

## Testing

The implementation must add or update tests for:

- both trace modes selecting `Regular` for `noexcept` calls with active objects;
- both trace modes selecting `Regular` for potentially throwing calls without active objects;
- both trace modes selecting `WithExceptionHandling` for potentially throwing calls with active objects;
- direct, indirect, and nested calls using the same classification;
- `[A]` and `[B]` branch paths producing distinct cleanup states at one source call;
- trace and Nautilus IR golden files showing the explicit call spelling;
- verifier rejection of every invalid kind/state/attribute combination;
- the terminal pass creating no pad for a potentially throwing `Regular` call;
- the terminal pass sharing pads for equal `WithExceptionHandling` states;
- MLIR emitting `call` without cleanup and `invoke` with cleanup;
- every enabled backend preserving exception type/message and reverse destruction order;
- nested Nautilus frames cleaning callee objects before caller objects.

The existing all-backend exception-cleanup suite remains the runtime acceptance gate. The branch must also build with
GCC and Clang so headers cannot depend on transitive standard-library includes.

## Non-goals

- Adding `try`, `catch`, or exception inspection to Nautilus source functions.
- Adding exceptional successors to the ordinary Nautilus CFG.
- Adding separate direct and indirect exception-call operation classes.
- Replacing exact cleanup-state snapshot identity with `CallKind`.
- Emitting lifetime-marker operations.
