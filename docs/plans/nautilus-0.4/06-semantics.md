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
  anyway. NIR and the NIR tracer are signless from the start (M1, M2); M6
  moves the plugin-facing API to widths and op flags.

The defined integer and float semantics (D10, D11, §4) are normative in NIR
from M1 ([13 §5.5, §5.10](13-nir-spec.md)), so they are built where NIR is
consumed, not in M6:
- the tracer records a cleanup list on every integer `div`/`rem` (M2.1,
  [01 §6](01-tracing-frontend.md));
- the conversion keeps the checked bit for every divisor not proven safe
  and clears it for a safe constant divisor (non-zero, and not −1 for a
  signed operation; [13 §5.10](13-nir-spec.md)), and masks constant shift
  amounts (M3.1/M3.2). Without this, `x / 10` would carry a check and an
  unwind entry on every backend from M4 on, and the M3/M4 op-count and
  code-quality gates against 0.3, which has no check at all, could not
  pass;
- **every backend lowers the checked division, the shift masks and the
  saturating casts in its M4 port** (M4.1, M4.3–M4.5), as §4.2 lists.

M6.1 keeps only the folding rules for the edge cases (constant operands
that are unsafe: a constant zero divisor or `MIN / −1` folded to an
unconditional throw, range-based proofs beyond a constant divisor), the
cost measurement and the plugin side.

## 2. Root causes

| 0.3 commitment | What it forces |
|---|---|
| **Pessimistic call semantics:** `FunctionAttributes` defaults to `ModRef`, `willReturn=false` and `noUnwind=false` unless the callee is `noexcept` | Exception handling on most calls; `NoThrowInferencePass` and `FunctionAttributeInferencePass` just to recover facts; every call is a barrier for CSE and hoisting |
| **Signedness in value types:** `Type` has `i32` and `ui32`, and so on | No-op cast operations; folding, verifier and opcode tables over 8 integer types instead of 4 widths (`tbc` expands every numeric family over 10 types: the 8 integer types plus `f32` and `f64`) |

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

Kept from 0.3 (`ExceptionRegionPreparationPass`, `ExceptionRegion.hpp`):

- **No landing pad without cleanup.** A call with no live owned values gets
  the "no cleanup" unwind entry ([02 §3.4](02-nir.md)), as today's
  `noLandingPad` call sites do.
- **Shared landing pads.** Pads are keyed by their ordered destructor set and
  shared per function. 0.3 interns them in a pass; 0.4 does it during
  emission ([03 §3](03-conversion-and-passes.md) step 7).

New in 0.4, removing what is not needed:

- **No capture thunk where unwind tables exist.** MLIR/LLVM code has unwind
  tables, so a may-unwind call with no cleanup can let the exception
  propagate through the generated frame directly. Backends without unwind
  tables (`tbc`, `asmjit`, and `cpp`, which uses the same captured-exception
  transport today) keep the thunk.
- **Annotations where they pay.** The `std` plugin and the hot NebulaStream
  runtime functions are annotated `nothrow` where they cannot throw. Each
  annotation removes the thunk and the exception region from every call
  site of that callee.

### 3.3 Declared effects

- **A call declares its memory behavior:** `pure`, `readonly`, `argmemonly`,
  `argmemonly readonly`, `writeonly` or `ModRef`. `ModRef` stays the
  default, so this is non-breaking. In NIR these are sets of independent
  guarantee bits ([13 §4.6.2](13-nir-spec.md)).
- **What a declaration gives.** `pure` and `readonly` also imply
  `willReturn` ([13 §4.6.2](13-nir-spec.md)): a callee declared to have no
  side effects is also declared to terminate. They do not imply
  `noUnwind`; that comes from `noexcept`, `nothrow` or inference.
- **Merging and hoisting need three things** ([03 §3](03-conversion-and-passes.md)
  step 3):
  - merging an identical call (CSE): `readonly` or `pure`, plus
    `willReturn` (implied) and `noUnwind`;
  - hoisting or removing an unused call: `pure` plus `noUnwind`, so all of
    bits 0–5.

  So an external callee is merged only when declared `readonly nothrow` or
  `pure nothrow` (a `noexcept` function type may stand in for `nothrow`,
  never for `readonly` or `pure`), and hoisted only when `pure nothrow`.
  Every other call stays a barrier.
- **Declarations and inference coexist.** The analyses of
  `NoThrowInferencePass` and `FunctionAttributeInferencePass` run once, as
  a pre-pass over the pre-SSA bodies of all functions of the module before
  any of them is converted ([03 §3.5](03-conversion-and-passes.md)). A
  declaration, where present, takes precedence.
- **Debug-mode checks** verify the declarations:
  - a terminate handler catches a `nothrow` callee that throws;
  - shadow checks catch effect violations where feasible;
  - a `pure` or `readonly` callee that does not return (exits or loops) is
    not checked; it is undefined like any wrong declaration (§7).

## 4. Signless integer values

- **Values carry a width only:** `i8`, `i16`, `i32`, `i64`, `f32`, `f64`,
  `ptr`, `bool`, plus extension types.
- **Signedness moves onto the operations that need it:** `div`, `rem`,
  shift-right, compare, int↔float casts and extends, as in LLVM and MLIR's
  `arith`.
- **Effects:**
  - casts between `i32` and `ui32` disappear;
  - opcode, folding and verifier tables shrink from 8 integer types to 4
    widths;
  - backend lowering has fewer cases.
- **User-facing semantics are unchanged.** `val<uint32_t>` keeps its C++
  meaning: the operator overloads choose the signed or unsigned operation.
- **The ABI keeps its extension kinds.** Platform ABIs make the caller
  sign- or zero-extend narrow integer arguments and returns (Apple AArch64;
  x86-64 SysV in practice, where clang relies on it; LLVM's
  `signext`/`zeroext`). Signless types alone lose this, so signatures carry
  a per-parameter and per-result **extension kind** (none, `zext`, `sext`;
  2 bits each in the signature record, [13 §4.5](13-nir-spec.md)). The
  tracer fills it from the C++ type ([01 §6](01-tracing-frontend.md)),
  `bool` is always `zext`, and the MLIR and `asmjit` backends extend
  accordingly at external calls and exported entries. Signatures are
  otherwise signless. M5.5's load-time signature check compares widths and
  extension kinds.
- **Integer semantics are defined in the IR** (D10), so the conversion's
  folding and every backend agree without backend-specific guards:

  | Case | 0.3 | 0.4 |
  |---|---|---|
  | Overflow of `add`, `sub`, `mul` | wraps on every backend | wraps (unchanged) |
  | Division or remainder by zero; signed `MIN / -1`, `MIN % -1` | undefined: a hardware trap on native backends, never folded | **throws `nautilus::ArithmeticException`** through the normal exception path ([02 §3.4](02-nir.md)) |
  | Shift by ≥ the bit width, or by a negative amount | undefined | **the amount is masked** to the bit width (`amount & (width − 1)`) |

  - **A checked division may throw,** so it carries an unwind entry like a
    call ([13 §5.10](13-nir-spec.md)), and landing pads run destructors
    as for any throwing call. A failing check calls the runtime helper
    `nautilus_rt_throw_arith` on a cold path. Backends without unwind
    tables make that call through the capture thunk, as for any
    may-throw call, so the exception reaches the instruction's landing pad
    by the same exceptional-exit path (§4.2).
  - **Division checks cost a compare and a branch** per division whose
    divisor is not known to be non-zero (or not `-1` for signed `MIN`). The
    conversion drops the check when folding proves the divisor safe. The
    cost is measured on `ExecutionBenchmark`.
  - **Shift masks are emitted only where the target does not mask:** x86
    and AArch64 mask 32- and 64-bit shift counts in hardware (x86 masks
    8- and 16-bit counts to 5 bits, not to the width), so only 8- and
    16-bit shifts there, every shift in MLIR/LLVM (where an oversized
    shift is poison) and every shift in the C++ of `tbc` and `cpp` (where
    it is UB) need an explicit `and`. Constant amounts are masked at
    conversion time.
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
  instead of today's undefined behavior. Each backend emits the clamp
  (§4.2); the fuzzer covers it.
- Fast-math remains a possible later, opt-in per-module option.

### 4.2 Lowering per backend

Every backend implements D10 and D11 in its M4 port (M4.1 `tbc`, M4.3
`cpp`, M4.4 `asmjit`, M4.5 MLIR). [05](05-backends.md) refers to this table.
In every row, a check is emitted only when the conversion has not proven it
unnecessary (the checked bit, [13 §5.10](13-nir-spec.md)), and the cold
path ends in a call of `nautilus_rt_throw_arith`, which throws
`nautilus::ArithmeticException`.

| Backend | Checked `div`/`rem` | Shift amount | `fptosi`/`fptoui` (saturating) | Throw path |
|---|---|---|---|---|
| `asmjit` x86-64 | compare the divisor with 0 (and, signed, with −1 when the dividend is `MIN`), branch to a cold block; then `div`/`idiv`, which would raise `#DE` for either case | none for 32/64-bit (hardware masks); explicit `and` for 8/16-bit, and an 8/16-bit `shr` works on the value sign- or zero-extended to 32 bits as its flag says | `cvttss2si`/`cvttsd2si`, then a fixup when the result is the "integer indefinite" `0x80…0`: NaN → 0, positive overflow → `MAX`; unsigned and 8/16-bit results by a range-compare sequence | cold block calls the helper through the capture thunk; the exceptional exit goes to the instruction's pad |
| `asmjit` AArch64 | the same compares, needed for the semantics rather than to avoid a trap (`sdiv`/`udiv` do not trap: x/0 gives 0, `MIN / −1` gives `MIN`); `rem` as `sdiv`/`udiv` then `msub` | none for 32/64-bit (`lslv`/`lsrv`/`asrv` use the amount modulo the register width); explicit `and` for 8/16-bit, on a 32-bit register holding the value extended as for x86-64 | `fcvtzs`/`fcvtzu` already saturate and give 0 for NaN for 32/64-bit results; 8/16-bit results clamp after a 32-bit conversion | as x86-64 |
| `tbc` (interpreter and copy-and-patch stencils) | checked C++ handler: compare, then the C++ operator; never UB | `amount & (width − 1)` in every shift handler | checked C++ helper with explicit NaN and range tests | the handler calls the helper through the capture thunk; the interpreter routes to the pad as for a captured call. Stencils are regenerated from the same handlers ([05](05-backends.md)) |
| `cpp` | generated code calls checked inline helpers from the emitted prelude | as `tbc` | as `tbc` | as `tbc`, through today's `CapturedExceptionTransport` preamble |
| MLIR | `arith.cmpi` + `cf.cond_br` to a cold block; then `arith.divsi`/`divui`/`remsi`/`remui` | explicit `arith.andi` on every dynamic amount | `llvm.intr.fptosi.sat` / `llvm.intr.fptoui.sat` | the cold block calls the helper directly; native unwinding (`llvm.invoke` to the pad, or a plain call with `NO_CLEANUP`) |

The fuzzer's D10/D11 generators run against every row from the M4 exit on.

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
- No capture thunk on MLIR call sites without cleanup; the number of
  may-unwind call sites and of capture thunks per corpus function is below
  the 0.3 baseline.
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
