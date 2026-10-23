# Component: Semantics Changes

Part of the [Nautilus 0.4 design](README.md) · Milestone M6

## 1. Purpose

Two semantic commitments of 0.3 cost latency and optimization opportunities
in every compile: how calls declare what they do, and signedness in value
types. 0.4 changes both:

- **Calls keep their exception default** (decision D4 = b) but can
  **declare their effects**, and exception handling gets cheaper where the
  backend allows it. This is not a breaking change for users.
- **Integer values become signless.** This breaks plugins and IR consumers,
  which is acceptable in 0.4 because the IR and the plugin API change
  anyway.

## 2. Root causes

| 0.3 commitment | What it forces |
|---|---|
| **Pessimistic call semantics:** `FunctionAttributes` defaults to `ModRef`, `willReturn=false` and `noUnwind=false` unless the callee is `noexcept` | Exception handling on most calls; `NoThrowInferencePass` and `FunctionAttributeInferencePass` just to recover facts; every call is a barrier for CSE and hoisting |
| **Signedness in value types:** `Type` has `i32` and `ui32`, and so on | No-op cast operations; folding, verifier and opcode tables over 10 integer types instead of 4 widths (`tbc` expands every numeric family over all 10) |

## 3. Call semantics: same default, declared effects, cheaper exceptions

### 3.1 Exception default (D4 = b)

The 0.3 rule stays:

| Callee | 0.3 and 0.4 |
|---|---|
| `noexcept` function type | cannot unwind |
| anything else | may unwind |

New in 0.4: callees that are not `noexcept` but never throw can say so with
`invoke<no_throw>(…)` or the `nothrow` attribute at registration. This is
optional and non-breaking.

The rejected alternative (a) assumed that every non-`noexcept` callee does
not throw. That would remove more exception machinery, but at the price of
a breaking change and a migration step for every throwing callee.

### 3.2 Cheaper exception handling

A call that may unwind needs two things today:
- a **capture thunk**, because interpreted and AsmJit frames have no
  unwind tables, so an exception must be caught before it crosses them;
- a **landing pad** with the destructors to run.

0.4 keeps that contract and removes what is not needed:

- **No landing pad without cleanup.** A call with no live owned values gets
  the "no cleanup" unwind entry ([02 §3.4](02-nir.md)), as today's
  `noLandingPad` call sites do.
- **No capture thunk where unwind tables exist.** MLIR/LLVM code has unwind
  tables, so a may-unwind call with no cleanup can let the exception
  propagate through the generated frame directly. Backends without unwind
  tables (`tbc`, `asmjit`) keep the thunk.
- **Shared landing pads.** Pads are keyed by their ordered destructor set and
  shared per function. The conversion does this during emission
  ([03](03-conversion-and-passes.md)).
- **Annotations where they pay.** The `std` plugin and the hot NebulaStream
  runtime functions are annotated `nothrow` where they cannot throw. Each
  annotation removes the thunk and the exception region from every call
  site of that callee.

### 3.3 Declared effects

- **A call declares its memory behavior:** `pure`, `readonly`, `argmemonly`
  or `ModRef`. `ModRef` stays the default, so this is non-breaking.
- **`pure` and `readonly` calls become values** the conversion can merge and
  hoist ([03](03-conversion-and-passes.md)). Every other call stays a barrier.
- **Declarations and inference coexist.** The analyses of
  `NoThrowInferencePass` and `FunctionAttributeInferencePass` run once
  inside the conversion. A declaration, where present, takes precedence.
- **Debug-mode checks** verify the declarations:
  - a terminate handler catches a `nothrow` callee that throws;
  - shadow checks catch effect violations where feasible.

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
- **Integer semantics are defined in the IR** (D10), so the conversion's
  folding and every backend agree without backend-specific guards:

  | Case | 0.3 | 0.4 |
  |---|---|---|
  | Overflow of `add`, `sub`, `mul` | wraps on every backend | wraps (unchanged) |
  | Division or remainder by zero; signed `MIN / -1`, `MIN % -1` | undefined: a hardware trap on native backends, never folded | **throws `nautilus::ArithmeticException`** through the normal exception path ([02 §3.4](02-nir.md)) |
  | Shift by ≥ the bit width, or by a negative amount | undefined | **the amount is masked** to the bit width (`amount & (width − 1)`) |

  - **Division checks cost a compare and a branch** per division whose
    divisor is not known to be non-zero (or not `-1` for signed `MIN`). The
    conversion drops the check when folding proves the divisor safe. The
    cost is measured on `ExecutionBenchmark`.
  - **Shift masks are emitted only where the target does not mask:** x86
    masks 32- and 64-bit shift counts in hardware, so only 8- and 16-bit
    shifts and MLIR/LLVM (where an oversized shift is poison) need an
    explicit `and`. Constant amounts are masked at conversion time.
  - **Tests:** the fuzzer generates these cases on purpose and checks them
    against the defined semantics ([12](12-validation.md)).

### 4.1 Floating-point semantics (D11)

- **IEEE 754, strict.** No fast-math in 0.4: no reassociation, no
  contraction into FMA unless the program calls one, no assumption that
  values are finite or not NaN.
- **Folding follows IEEE:** NaN results are folded as quiet NaNs (payload
  unspecified), signed zeros are preserved, and `x + 0.0` is not folded to
  `x` (it differs for `-0.0`).
- **Float division by zero** gives ±∞ or NaN as in C++, with no exception.
- **float→int casts** of NaN or out-of-range values give a defined
  saturated result (as Rust and WebAssembly's saturating conversions do),
  instead of today's undefined behavior. Each backend emits the clamp; the
  fuzzer covers it.
- Fast-math remains a possible later, opt-in per-module option.

## 5. Migration

- **Calls:** nothing to migrate. Calls to `noexcept` callees behave as before,
  and everything else may unwind as before. `nothrow` and effect
  annotations are optional additions.
- **Signedness:** plugins that switch on `Type::ui32` and similar map to
  width plus operation flag. A helper `isUnsignedOp(inst)` replaces the type
  checks.
- **Serialized formats are new in 0.4,** so no old images need migrating.

## 6. Exit criteria (M6)

- All suites green, with no change in exception behavior compared with 0.3.
- No capture thunk on MLIR call sites without cleanup; landing pads only for
  calls with live owned values.
- The `std` plugin and the NebulaStream hot runtime functions annotated.
  The share of call sites with exception handling is reported in the
  statistics and lower than in 0.3.
- Differential fuzzer clean across all backends with signless integers,
  including the edge cases of D10 and D11.

## 7. Risks

- **A wrongly declared `nothrow` or `pure` callee is undefined behavior.**
  The debug-mode checks are a requirement, not an extra.
- **Propagating through MLIR frames without a thunk** relies on correct
  unwind tables and personality wiring. Today's MLIR exception tests,
  extended with no-cleanup call sites, gate the change.
- **Hidden signedness assumptions** in plugins and backends. Run the
  fuzzer with every integer width and signedness.
- **Defined division and shift semantics cost cycles** in hot loops. The
  checks are dropped where folding proves them unnecessary, and the cost is
  measured; if it is large on TPC-H, an opt-in `unchecked` division for
  callers that prove safety themselves is the fallback.
