# Partial Evaluation plugin

Experimental trace-time partial-evaluation machinery for Nautilus, shipped
as an opt-in plugin gated by the `ENABLE_CONSTANT_TRACER` CMake option.

## Why

Nautilus traces a user-written C++ function by executing it against `val<T>`
wrapper objects and recording every operation into an `ExecutionTrace`. The
recorded trace is the raw material for every downstream phase (SSA, IR
generation, backend codegen). A plain tracer records *everything* — even
operations whose operands are statically known at trace time.

This plugin turns the tracer into an **online partial evaluator**:

- Values constructed from C++ literals start in a `Constant` binding-time
  state.
- Operations whose operands are all `Constant` evaluate at trace time and
  return new `Constant` values without emitting any op.
- Conditional branches whose discriminant is `Constant` collapse to the
  taken path — the dead branch is never traced.
- Compile-time-bounded loops with a `Constant` induction variable fully
  unroll at trace time and collapse to a straight-line sequence (or a
  single `return CONST(N)`).

Every core IR the compiler sees is strictly smaller when the plugin is
on, which propagates through all downstream passes.

## How it's wired in

Unlike `plugins/std`, `plugins/simd`, etc. this plugin is *not* a separate
linkable library. `val<T>`'s state field is a compile-time typedef that
resolves to either `tracing::TypedValueRefHolder` (default) or
`tracing::pe::LazyTracedRef<T>` (when the plugin is on). Every core TU
that touches `val<T>` therefore needs the plugin's headers at compile
time. Decoupling into a runtime-loadable library would require a
`std::variant` or type-erased state slot on every `val<T>` — cost paid
in every op, not justifiable for an experimental feature.

Instead, `plugins/partial_evaluation/CMakeLists.txt`:

- Early-returns when `ENABLE_CONSTANT_TRACER` is `OFF` — nothing new is
  compiled in default builds.
- When `ON`, attaches its sources to the `nautilus` target via
  `target_sources(nautilus PRIVATE …)` and extends its public include
  path. Users link against `nautilus` exactly as before.

### Lazy-tracing only

PE attaches to `LazyTraceContext` exclusively, via a TU-private
`PELazyTraceContext : public LazyTraceContext` subclass registered at
static-init time through `LazyTraceContext::setTraceOverride()`. The
`ExceptionBasedTraceContext` path does **not** have a PE override.

Mixing `engine.traceMode="exceptionBasedTracing"` with PE runtime-enabled
would silently break SSA correctness: `LazyTracedRef<T>` keeps registering
Constants but nothing flushes them, so `CONST` ops never land in the
predecessor blocks. `LegacyCompiler::compileToIR` therefore hard-errors
on that combination. Either keep `engine.traceMode="lazyTracing"` (the
default) or call `nautilus::tracing::pe::setConstantTracerEnabled(false)`
to opt out.

## Layout

```
plugins/partial_evaluation/
├── CMakeLists.txt
├── README.md                        this file
├── include/nautilus/partial_evaluation/
│   ├── api.hpp                      public declarations in nautilus::tracing::pe::
│   ├── LazyTracedRef.hpp            per-val lazy binding-time holder
│   ├── assume_stable.hpp            per-pointer PE annotation
│   └── unroll_scope.hpp             per-site loop-unroll annotation
└── src/
    ├── ConstantTracerState.cpp      thread-local registry, Stage 2 widening,
    │                                observability counters, runtime flags
    └── PELazyTraceContext.cpp       LazyTraceContext subclass: per-op
                                     speculative flush + lifecycle hooks
```

## Current design

### Binding-time lattice (one bit)

Every `LazyTracedRef<T>` is in exactly one of two states:

| state | representation | meaning |
|-------|----------------|---------|
| `Constant` | T value + registry-list hook | value known at trace time; nothing emitted |
| `Materialized` | `TypedValueRef` pointing into the trace | value is a runtime quantity |

Materialization is a one-way transition. Once a val's state becomes
`Materialized`, it never goes back. Values constructed from literals or
copy/move of other `Constant`s start in `Constant`; values constructed
from a `TypedValueRef` (function arguments, results of non-folded ops)
start in `Materialized`.

### Fold fast paths

`val_arith.hpp`, `val_bool.hpp`, `val_enum.hpp`, `val_ptr.hpp` contain
`#ifdef ENABLE_CONSTANT_TRACER`-guarded fast paths inside every operator:

```cpp
if (lhs.state.isConstant() && rhs.state.isConstant()) {
    tracing::pe::noteConstantFoldElided();
    auto folded = evaluate_op_at_cxx_level(lhs, rhs);
    return val<R>(folded);                 // returns a Constant
}
```

Operations covered: binary/unary arithmetic, bitwise, comparison,
cast-on-constant, bool AND/OR/NOT/eq/neq, enum eq/neq, pointer
comparisons. `val<bool>::operator bool()` folds dead-branch
elimination — if the bool is `Constant`, the branch goes the taken way
and `traceBool` is never called, so the dead side never traces.

### Constant registry

Thread-local intrusive doubly-linked list of live `Constant` holders.
Every `LazyTracedRef` embeds a `ConstantRegistryNode` and registers
itself on construction, unregisters on destruction or materialization.
O(1) register/unregister, O(n) walk for `materializeAllConstants()`.

Lives in `plugins/partial_evaluation/src/ConstantTracerState.cpp`.

### Speculative flush at every trace op

Every trace-op wrapper in `nautilus::tracing` (`traceBool`,
`traceBinaryOp`, `traceUnaryOp`, `traceTernaryOp`,
`traceCall/IndirectCall/NautilusCall`, `traceReturnOperation`) calls
`tracing::pe::materializeAllConstants()` before delegating to the
active tracer. This enforces the *"no elision crosses a basic-block
boundary"* invariant: by the time `processControlFlowMerge` might move
ops into a new merge block, every live `Constant` has already emitted
its `CONST` into the current predecessor.

It's conservative — most flushes have nothing to do — but avoids the
harder question of *"which future ops will this Constant's value flow
into"* that a proper per-use BTA would answer.

### Stage 2 widening (termination)

Without a bound, the PE layer would run forever on a loop whose body
keeps folding. Two backstops:

1. **Stratified divergence detection.** On every Constant-to-Constant
   assignment, compute a registry-wide value hash and compare to the
   most-recently-observed hash at the current `TracingInterface::currentTag()`
   program point. A divergent revisit crosses `unrollLimit` → force
   `materializeAllConstants()`.

2. **Blunt iteration cap.** Running count of Constant-to-Constant
   assignments since the last flush; crosses `wideningThreshold`
   (default 1024) → force flush regardless of tag state.

Primary is stratification; the cap covers currentTag()==null edge
cases and pathological programs.

### User-facing opt-ins (per-site annotations)

- **`nautilus::unroll_scope(N)`** — RAII guard that raises the
  stratified unroll limit for its lifetime. Per-site; restores on
  destruction.
- **`nautilus::assume_stable(val<T*>)`** — flips a bit on a pointer
  val authorizing trace-time folding of pointer arithmetic + LOAD.
  The bit propagates through copies and through folded arithmetic so
  `*(assume_stable(p) + 3)` folds the whole chain. Correct only when
  the caller can assert the pointer memory is stable across
  trace→execution; the annotation sits right where that promise is
  being made.

### Observability

Four thread-local counters reset at each trace start:

- `constantTracer.foldsElided` — pure-op ops elided by the fast paths.
- `constantTracer.flushes` — `materializeAllConstants()` calls that
  actually had work (empty-registry flushes don't count).
- `constantTracer.wideningsCap` / `wideningsStrat` — Stage 2
  firings, split by mechanism.

`LegacyCompiler::compileToIR` reads these after `Trace()` returns and
stores them in `CompilationStatistics` under the `constantTracer.*`
namespace.

## Future tasks

### Lattice improvements

- **Proper binding-time analysis.** Replace the one-bit
  `Constant`/`Materialized` lattice with a richer one (Consel &
  Danvy-style): "static value", "static structure with dynamic
  fields", "partially-static pointer", etc. The speculative-flush
  policy would disappear in favor of demand-driven materialization
  (materialize only when a truly dynamic op consumes the value).
  This also closes the "`a + b` doesn't fold after `*ptr = 999`
  forced a materialization" gap the IR-level constant-folding pass
  currently patches over.

- **Polyvariant specialization.** Currently every `invoke()`-style
  nested Nautilus function is traced once. Allow the same callee to
  be specialized multiple times against different static inputs from
  different call sites.

- **Polymorphic static data.** Extend `assume_stable` to whole data
  structures: "this `std::span<const T>` is stable trace→exec" so
  reads through it fold without the user annotating each `&s[i]`.

### Termination

- **Adopt supercompilation techniques.** Stage 2 widening is a blunt
  "widen immediately on divergence" trick. The supercompilation
  literature (Turchin, Sørensen, Glück) offers homeomorphic embedding
  and depth-bounded generalization — more precise termination at the
  cost of more complex bookkeeping.

- **Automatic unroll-budget inference.** Today the user picks via
  `unroll_scope(N)`. A trace-time heuristic (trip count estimate,
  body-complexity estimate) could pick N automatically for the common
  "I know this loop has a small constant bound" case.

### Implementation hygiene

- **Encapsulate per-trace state in a class.** The thread-locals in
  `ConstantTracerState.cpp` (registry head, stratification map,
  counters) should move onto a `ConstantTracerState` object owned by
  the active trace context. Config flags (enabled, thresholds) can
  stay thread-local because they're user-facing knobs, not per-trace
  state. Makes the lifecycle explicit and makes tests easier.

- **Proper pre-merge hook.** The abandoned P0 attempt (see older
  discussion) tried to move materialization from "before every trace
  op" to "before `processControlFlowMerge()` only". It failed because
  iter-1 blocks had already been recorded without the CONSTs. A proper
  solution needs per-snapshot shadow state for retroactive splicing —
  doable, but ~200 LOC of bookkeeping.

### Coverage

- **`val<func_ptr>` folding.** Currently falls through to the generic
  pointer-comparison paths. Dedicated folds for indirect-call
  equality on literal function pointers would shrink some dispatch
  tables.

- **Algebraic identities.** `x * 1`, `x + 0`, `x & x`, `x | 0`, etc.
  These don't require static inputs, so they belong in the IR-level
  constant-folding pass (`ConstantFoldingAndCopyPropagationPass`), not
  here. But the PE layer could opportunistically catch them when one
  operand happens to be Constant with the identity value.

### Integration with the IR-level constant-folding pass

Currently the two layers operate independently. Main's
`ConstantFoldingAndCopyPropagationPass` folds `ConstInt + ConstInt` at
the IR level after this plugin has already folded what it could at
trace time. The IR pass is strictly weaker (can't touch control flow,
loops, or memory) but catches cases the plugin's speculative-flush
conservatism missed.

As the PE plugin's lattice gets richer (above), the overlap shrinks.
Long-term direction: the IR pass becomes a fallback for cases the PE
layer hasn't learned yet, and eventually retires.
