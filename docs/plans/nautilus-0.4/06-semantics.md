# Component: Semantics Changes

Part of the [Nautilus 0.4 design](README.md) · Milestone M6

## 1. Purpose

Two semantic commitments of 0.3 cost latency and optimization opportunities
in every compile. 0.4 changes both: call semantics become optimistic with
declared effects, and integer values become signless. Both break
plugins and IR consumers, which is acceptable in 0.4 because the IR and the
plugin API change anyway.

## 2. Root causes

| 0.3 commitment | What it forces |
|---|---|
| **Pessimistic call semantics:** `FunctionAttributes` defaults to `ModRef`, `willReturn=false` and `noUnwind=false` unless the callee is `noexcept` | Exception regions, landing pads and `ExceptionRegionPreparationPass` on many calls; `NoThrowInferencePass` and `FunctionAttributeInferencePass` just to recover facts; every call is a barrier for CSE and hoisting |
| **Signedness in value types:** `Type` has `i32` and `ui32`, and so on | No-op cast operations; folding, verifier and opcode tables over 10 integer types instead of 4 widths (`tbc` expands every numeric family over all 10) |

## 3. Optimistic call semantics with declared effects

- **Exceptions are opt-in.** The rule for a call through `invoke`:

  | Callee | 0.3 | 0.4 |
  |---|---|---|
  | `noexcept` function type | cannot unwind | cannot unwind (unchanged) |
  | anything else, no annotation | may unwind: exception region, capture thunk | **cannot unwind** |
  | annotated `invoke<may_throw>(…)` or registered with the `throws` attribute | — | may unwind: exception region, capture thunk |

  Most runtime functions are not marked `noexcept` but never throw, so
  0.3's default puts exception machinery on most calls. A callee that does
  throw without the annotation is found by the debug-mode check below, not
  by a compile-time warning: such a warning would fire on almost every
  callee.
- **Declared effects.** A call declares its memory behavior: `pure`,
  `readonly`, `argmemonly` or `ModRef` (today's pessimistic default stays
  available). `pure` and `readonly` calls become values the conversion can
  merge and hoist ([03](03-conversion-and-passes.md)).
- **Trust declarations instead of inferring them.** The inference passes
  leave the critical path.
- **Debug-mode checks** verify the declarations: a terminate handler
  catches a `noUnwind` callee that throws, and shadow checks catch effect
  violations where feasible.
- **Plugins and the `std` wrappers** declare effects for their runtime
  functions.

Decision D4 has two options:

- **(a) Optimistic default, as above.** Most of the latency and
  optimization benefit; needs the debug-mode check and the migration step.
- **(b) Keep 0.3's default,** but drop the exception region for calls with
  no destructors to run and make effect declarations cheap to add.
  Smaller benefit; no migration.

The proposal is (a).

## 4. Signless integer values

- **Values carry a width only:** `i8`, `i16`, `i32`, `i64`, `f32`, `f64`,
  `ptr`, `bool`, plus extension types.
- **Signedness moves onto the operations that need it:** `div`, `rem`,
  shift-right, compare, int↔float casts and extends, as in LLVM and MLIR's
  `arith`.
- **Effects:**
  - casts between `i32` and `ui32` disappear;
  - opcode, folding and verifier tables shrink from 10 integer types to 4
    widths;
  - backend lowering has fewer cases.
- **User-facing semantics are unchanged.** `val<uint32_t>` keeps its C++
  meaning: the operator overloads choose the signed or unsigned operation.
- **Integer semantics are defined in the IR:** wrap-around on overflow, a
  trap on division by zero, and masked shift amounts. The conversion's
  folding and every backend then agree without backend-specific guards.
  The P0 corpus and the differential fuzzer check that the results match
  0.3's observable C++ behavior.

## 5. Migration

- **Calls to `noexcept` callees behave as before.** Callees that actually
  throw need `invoke<may_throw>` or the `throws` attribute. Running the test
  suite once in debug mode lists every callee that threw without one. The
  migration guide lists the patterns.
- **Signedness:** plugins that switch on `Type::ui32` and similar map to
  width plus operation flag. A helper `isUnsignedOp(inst)` replaces the type
  checks.
- **Serialized formats are new in 0.4,** so no old images need migrating.

## 6. Exit criteria (M6)

- NebulaStream suite green with the new `invoke` default.
- Exception regions only where callees opt in. The inference passes are no
  longer on the default path.
- Differential fuzzer clean across all backends with signless integers.

## 7. Risks

- **A wrongly declared `noUnwind` or `pure` callee is undefined behavior.**
  The debug-mode checks are a requirement, not an extra.
- **Hidden signedness assumptions** in plugins and backends. Run the
  fuzzer with every integer width and signedness.
