# Differential AST fuzzer

A coverage-guided (libFuzzer) fuzzer that looks for **miscompiles**: it generates
random, well-defined integer programs, realizes each one as a traced Nautilus
kernel, and checks that every backend — and the interpreter — produces exactly
the same result as an independent native C++ evaluation of the same program.

You cannot text-diff generated code across backends (LLVM IR vs C vs bytecode vs
asm are different formats), so equivalence is checked **behaviorally**: same
program + same inputs ⇒ same result everywhere. A disagreement is an unambiguous
bug because every generated program is fully defined by construction.

## Pieces

| File | Role |
|------|------|
| `Types.hpp`        | The `TypeId` domain (twelve root types), imm pack/unpack, type dispatch helpers, value formatting/equality. |
| `ByteReader.hpp`   | Deterministic `FuzzedDataProvider`-style view over the fuzzer buffer. |
| `Ast.hpp`          | Tagged AST, depth/node-budgeted generator (templated on the value type, plus a bespoke enum-domain generator), and a pretty printer. |
| `EvalNative.hpp`   | Independent C++ ground-truth interpreter (the oracle), templated on the value type, plus a bespoke enum-domain oracle. |
| `EvalNautilus.hpp` | Walks the same AST emitting `val<T>` ops for tracing/compilation; plus a bespoke enum-domain evaluator. |
| `Harness.hpp`      | Shared differential check (`checkOne`/`runOne`) used by both drivers; dispatches each input to the right `T` and kernel signature shape (see "Kernel signature space"). |
| `FuzzMain.cpp`     | `LLVMFuzzerTestOneInput`: coverage-guided libFuzzer entry point. |
| `ReplayMain.cpp`   | Plain-`main` driver: replay saved inputs, smoke corpus, or `--survey`. |

## Two drivers

* **`nautilus-fuzz`** — coverage-guided libFuzzer binary (Clang + libFuzzer
  runtime). Built with `-DENABLE_FUZZING=ON`.
* **`nautilus-fuzz-replay`** — libFuzzer-free; runs on any compiler. Built with
  `-DENABLE_FUZZER_REPLAY=ON` (also built by `-DENABLE_FUZZING=ON`). Modes:
  * `nautilus-fuzz-replay` — deterministic built-in smoke corpus (aborts on the
    first finding, like libFuzzer).
  * `nautilus-fuzz-replay <file> ...` — replay specific saved inputs.
  * `nautilus-fuzz-replay --survey [N]` — run N inputs **without** aborting and
    print findings bucketed by `(backend, config, type, shape, kind)`, to triage
    how many distinct bug classes exist (e.g. an `i8`-only bug breaks out from an
    `f64`-only one instead of merging into one bucket; an option-gated miscompile
    breaks out from a plain backend bug — see [Config sweep](#config-sweep)).

## Scope: twelve value domains

Each generated program is monomorphic in one of twelve types, picked from the
first byte of the fuzzer input: `int8_t/16/32/64_t`, `uint8/16/32/64_t`,
`float`, `double`, `bool`, and a small fixed `enum class FuzzEnum` (`TypeId` in
`Types.hpp`, via `ROOT_TYPES`). The rest of the input (AST shape, constants,
parameters) is generated and evaluated entirely in that one type, mirroring
how the original `uint64_t`-only fuzzer worked. For every domain except enum,
the second byte of the input separately picks the kernel *signature* shape
the AST is compiled into -- see "Kernel signature space" below -- independent
of this type selection; the enum domain keeps its own fixed shape (see
`checkOneEnum` in Harness.hpp).

* **Integer domain** (8 widths/signs): arithmetic (`+ - * / %`), bitwise
  (`& | ^ << >> ~`), unary negate, comparisons, logical ops, `Select`/`If`,
  `Loop`/`While` (with conditional `LoopBreak`/`LoopContinue`), and `Cast`
  (round-trips the value through another type, e.g. `(T)(To)v`, exercising
  sign/zero-extension/truncation codegen *and* int<->float boundary
  conversion).
* **Float domain** (`float`, `double`): arithmetic (`+ - * /`), unary
  negate, comparisons, logical ops, `Select`/`If`, `Loop`/`While` (with
  conditional `LoopBreak`/`LoopContinue`), and `Cast` (round-trips through
  another integer *or* float type).
* **Logical ops** (`LAnd`/`LOr`/`LNot`, both domains): each operand becomes a
  `val<bool>` via `!= 0`, the bools are combined with the real
  `val<bool>` `&&`/`||`/`!` operator under test, and the result is selected
  back to 0/1 as `T` (the comparisons' convention). Operands are evaluated
  *before* the bool op on both sides, so even a short-circuiting `&&`
  lowering (`ENABLE_SHORT_CIRCUIT_BOOL`) cannot skip a `Store` side effect.
* **Runtime calls** (`Call`, both domains): a real `nautilus::invoke()` of a
  pure native helper, selected by the node's imm from a small typed registry
  of nine callees (`Callees.hpp`). The native oracle calls the *identical
  instantiation* directly, so the two legs execute the same native code and
  the differential surface is exclusively the backend's call lowering:
  argument/return marshalling, narrow-integer ABI extension, float register
  passing. Each callee has its own `CallDescriptor` (arity, and whether it
  also takes a pointer-domain kid) consulted identically by the generator
  (`Ast.hpp`) and both evaluators (`EvalNative.hpp`/`EvalNautilus.hpp`) so the
  three can never disagree about a callee's kid layout. The registry spans:
  arity 0 (`calleeConstSeven`, pure return-value plumbing through the ABI, no
  argument registers at all), arity 1 (`calleeUnary`), arity 2 same-type
  (`calleeMix`/`calleeMin`, the original pair), arity 3 (`calleeSum3`,
  argument-register exhaustion on some ABIs), a mixed-fundamental-type
  signature (`calleeMixedTypes`: `T` plus a fixed cross-domain type -- `double`
  for integer `T`, `int32_t` for floating-point `T` -- exercising
  integer/floating-point register-class interleaving), a narrower-than-`T`
  return (`calleeNarrowReturn`, the return-extension-direction ABI bug class),
  a void return (`calleeVoidNoop`, invoked purely for the call itself -- the
  node's value falls back to its first argument, mirroring how `Kind::Store`
  evaluates to the value it wrote), and a pointer argument with an observable
  side effect (`calleePtrSwap`, reads/writes through the shared buffer pointer
  already threaded through every kernel, so call *ordering* relative to
  surrounding `Load`/`Store` is under test, not just the return value).
* **`StaticLoop`** (both domains): the trace-time counterpart of `Loop`. The
  trip count is a generation-time constant (imm, in `[0, LOOP_MAX_TRIPS]`),
  and the loop control is a plain C++ `for` over a `static_val<int64_t>`
  counter -- the tracer unrolls the body once per trip, each iteration
  seeing its `LoopIndex` as a constant, exercising `static_val`'s
  snapshot-hash machinery and trace-time unrolling instead of loop
  lowering (the `static-loop-tests` feature).
* **`Cast` across the int/float domain boundary**: `(T)(To)v` still always
  produces a `T` result, exactly like a same-domain cast -- only the
  intermediate type's domain changes. Whichever leg of the round trip is
  float->int (the only UB-prone direction; int->float is precision loss
  only, never UB) is range-clamped instead of invoking C++'s float-to-int
  UB: `NaN -> 0`, out-of-range -> the target type's min/max. The clamp
  compares against a power-of-two boundary rather than
  `static_cast<From>(numeric_limits<To>::max())`, since for wide integer
  types the latter is not exactly representable in `float`/`double` and
  rounds *up* past the true max (e.g. `(double)INT64_MAX` rounds to `2^63`).
  See `hiLimitExclusive`/`loLimitInclusive` in `Types.hpp`, shared verbatim
  by the native oracle (`EvalNative.hpp`) and the traced kernel
  (`EvalNautilus.hpp`) so the boundary math can't drift between the two.
* **`Loop`**: a bounded, data-dependent fold -- `kid[0]` = trip-count
  expression (clamped to `[0, LOOP_MAX_TRIPS]` via the same
  reinterpret-to-unsigned-then-modulo on both sides), `kid[1]` = initial
  accumulator, `kid[2]` = body, re-evaluated `trips` times with `LoopIndex`/
  `LoopAcc` leaves bound to the current iteration (nesting uses simple
  innermost-wins shadowing, no De Bruijn indices). Lowered to a real traced
  `for` loop with a mutated accumulator on the Nautilus side -- the same
  pattern used by `test/common/LoopFunctions.hpp` -- so loop lowering itself
  is under test, not an unrolled approximation. Trip count is bounded but
  the loop body is traced exactly once regardless of how many times the
  compiled loop executes it at runtime, so this doesn't blow up compile
  time.
* **`While`**: the runtime-computed, unclamped-condition counterpart of
  `Loop` -- `kid[0]` = init, `kid[1]` = a loop-carried exit condition
  re-evaluated every iteration (`LoopIndex`/`LoopAcc` bound, same as
  `Loop`'s body), `kid[2]` = body. Unlike `Loop`, whose trip count is a
  single expression clamped *once* up front, the number of iterations here
  is discovered incrementally from `cond`, which can itself depend on values
  computed inside the loop -- a guard-plus-latch loop CFG rather than
  `Loop`'s counted `for`. Still provably terminating: both sides bound the
  loop to `LOOP_MAX_TRIPS` iterations regardless of what `cond` evaluates to,
  so the hard cap -- not the data-dependent condition -- is what guarantees
  termination. Lowered to a real traced, capped `for` with an internal
  data-dependent conditional exit; never unrolled.
* **`LoopBreak`/`LoopContinue`**: conditional early-exit/skip constructs,
  legal only while generating inside a `Loop`/`While` body (never a
  `StaticLoop` body -- its trip count is fixed at generation time and fully
  unrolled at trace time regardless of runtime data, so there is no
  per-iteration branch to hook a data-dependent break/continue into).
  `kid[0]` = cond, `kid[1]` = value: `value` is always evaluated (the same
  unconditional-both-operands convention `Select` uses, so a `Store` inside
  either arm is never silently skipped) and becomes this node's own
  contribution to the surrounding expression; additionally, if `cond` is
  nonzero, the innermost enclosing `Loop`/`While` is signalled to stop
  (`LoopBreak`) or move on to its next iteration (`LoopContinue`) once the
  current iteration's body finishes evaluating. The signal is carried by a
  real `val<bool>` per enclosing breakable loop (a small signal stack,
  mirroring the existing `LoopIndex`/`LoopAcc` stack, so a signal generated
  inside a nested loop can never leak into an outer loop's own check), and
  the enclosing loop reacts to it with a real traced `if (signal) { break;
  }` / `if (signal) { continue; }` immediately after the body -- the same
  proven pattern as `test/common/LoopFunctions.hpp`'s `whileBreak`/
  `whileContinue`. This exercises the exit-edge / loop-live-out merge
  (`LoopBreak`) and the back-edge-from-mid-body merge (`LoopContinue`) that
  a run-to-completion loop never forms. If both signals end up set for the
  same iteration (e.g. two independent conditions both fire), break takes
  priority over continue, identically on both sides.

### Bool domain

`val<bool>` (`val_bool.hpp`) is a much smaller specialization than the
integer/float domains -- no arithmetic, no shifts, no real bitwise
complement -- so it reuses `generateNode<T>`'s shared machinery with its own
restricted `BOOL_KINDS` array (`Ast.hpp`) instead of `INT_KINDS`/`FLOAT_KINDS`:
`And`/`Or`/`Xor` (`&`/`|`/`^`, well-defined bitwise-as-logical over a 0/1
domain), `Not` (special-cased to mean logical negation `!`, not the integer
domain's bitwise `~` -- see the `Kind::Not` handling in
`EvalNative.hpp`/`EvalNautilus.hpp`), `Eq`/`Ne`, `Select`/`If`, `LAnd`/`LOr`/
`LNot`, `Call`, and `Load`/`Store` through a `val<bool*>` buffer. `Cast`,
`Loop`/`StaticLoop`, `Neg`, `PtrToInt`, and the six `Ptr*` comparisons are
excluded -- some because `val<bool>`/`val<bool*>` has no such operator at all
(confirmed by attempting to compile them: several are outright missing, and a
few more are latent ambiguity/compile bugs this work fixed, see below), some
(`PtrToInt`) because `val_ptr.hpp`'s pointer -> integer cast operator
explicitly excludes bool. Every `bool`-domain pointer node is forced to the
buffer's base slot (`generatePtrNode`'s `if constexpr (std::is_same_v<T,
bool>)` branch) since there is no arithmetic to build a `PtrAdd`/`PtrSub`
offset from.

On some generated `If`/`Select` nodes, `EvalNautilus.hpp`'s
`applyProbabilityHint` calls `val<bool>::setIsTrueProbability()` with a value
packed into the (otherwise-unused, for these kinds) `Node::imm` field. This is
a codegen hint only -- the native oracle never reads it -- so the existing
differential check against the oracle is exactly what verifies a hint never
changes the computed result.

`checkOneTyped<T>`'s kernel-signature-space menu (see below) restricts bool
to the `arity{1,2,3,4}`/`voidReturn` shapes -- `mixed` and `narrowReturn` both
convert across a type boundary via a real traced `Cast` (`convertClamped` /
`convertClampedTraced`), and bc/tbc's `Cast` lowering only ever supported the
original ten numeric types as a source or target, the same reason `Kind::Cast`
itself is excluded from `BOOL_KINDS`.

Implementing this domain surfaced several genuine, previously-latent bugs in
`val<bool>`'s supporting machinery (unexercised because bool was never
generated as a top-level, "any operator might get used" domain before):
`convertible_to_integral` (`val_concepts.hpp`) didn't exclude already-wrapped
`val<>` types, so `val<bool> & val<bool>` (satisfying both `is_integral_val`
and, via `operator bool()`'s implicit int promotion, `convertible_to_integral`)
resolved to an ambiguous overload; and the bc/tbc backends' bitwise
AND/OR/XOR lowering (`BCLoweringProvider.cpp`/`TBCLoweringProvider.cpp`) had
no `Type::b` case at all, since no domain had ever generated a bitwise op
stamped bool before. All three are fixed (see git history for this change).

### Enum domain

A small, fixed `enum class FuzzEnum : int32_t` (`Types.hpp`) with a *fixed*
underlying type, so every bit pattern of that underlying type is already a
valid `FuzzEnum` value ([dcl.enum]) -- Const/Param leaves can draw raw bytes
exactly like every other domain, with no need to clamp to the named
enumerators. `val<enum>` (`val_enum.hpp`) supports only construction/copy and
`==`/`!=` -- no arithmetic, bitwise, or pointer-offset operator exists for it
at all, and critically `val<enum*>` has no dereference operator whatsoever
(`val_ptr.hpp`'s `operator*` requires `is_arithmetic<ValType> ||
is_ptr<ValType>`, which no enum satisfies). This is different enough from
every other domain (including bool) that it is **not** an instantiation of
`generateNode<T>`/`evalNativeGeneric<T>`/`evalNautilusGeneric<T>`: it has its
own bespoke, much smaller generator (`generateFuzzEnumAst`/
`detail::generateEnumNode` in `Ast.hpp`) and evaluators
(`evalNativeFuzzEnum`/`evalNautilusFuzzEnum`), covering exactly `Const`,
`Param`, `Eq`, `Ne`, `Load`, and `Store`.

Because `val<enum*>` cannot be dereferenced, `Load`/`Store` instead run
against a shared buffer of `FuzzEnum`'s *underlying* integer type: `Load`
loads a `val<underlying_type_t>` and constructs a `val<enum>` from it, and
`Store` converts a `val<enum>` to `val<underlying_type_t>` before writing --
exactly the two conversions `val<enum>` actually provides. `Eq`/`Ne` similarly
can't use `select()` to materialize their 0/1 result (`select.hpp` only
supports `is_arithmetic`/pointer `T`, and enums are neither), so they
materialize it via a real branch instead, the same real-control-flow
mechanism `Kind::If` already uses elsewhere. `checkOneEnum` (`Harness.hpp`) is
consequently a bespoke differential check rather than a `checkOneTyped<T>`
instantiation, with its own enum-domain kernel signature
(`val<enum>(val<underlying_type_t*>, val<enum>, val<enum>, val<enum>)`).

Implementing this domain surfaced two genuine, previously-latent bugs in
`val<enum>` (unexercised because nothing had ever round-tripped a traced
`val<enum>` through Load/Store or an assignment before): the `val<enum> ->
val<underlying_type_t>` conversion operator tried to construct the target
`val<underlying_type_t>` directly from the source's `TypedValueRefHolder`
`state`, which doesn't compile against `val<underlying_type_t>`'s
trace-state constructor (it wants a non-const `TypedValueRef&`) -- fixed by
routing through a real `traceUnaryOp(CAST, ...)` the way `val_arith.hpp`'s
analogous cast operator already does; and `operator=` assigned through a
`const T value` member, which cannot compile for any assignment at all --
fixed by dropping the `const`.

## Memory and pointer domain

Every generated program is additionally handed a pointer (`val<T*>`) to a
shared, `BUFFER_ELEMS`-element buffer of `T` (`Ast.hpp`), so `val<T*>`
arithmetic, comparisons, casts, and loads/stores are fuzzed identically to
the rest of the value domain -- following the same well-defined-by-construction
principle as everything else here.

This domain still deliberately simplifies two other surfaces raised in
nebulastream/nautilus#357: there is one buffer of one element type `T` (no
type-punned/mixed-width access), and no aliasing between distinct pointers
(there's only ever one buffer to point into). Compounding pointer arithmetic
(the third surface #357 identifies) is covered below; the other two are
left as follow-up work.

* **`Load`/`Store`**: `kid[0]` (both) is a *pointer-domain* node (see below),
  `kid[1]` (`Store` only) is a value-domain node. `Load` yields `*ptr`; `Store`
  writes through the pointer and yields the stored value, i.e. the value of
  the C++ assignment expression `*ptr = v`.
* **`PtrToInt`**: `(T)(uintptr_t)ptr`, the same pointer -> arithmetic
  conversion `base_ptr_val` already exposes (`val_ptr.hpp`). Because the
  harness reuses one buffer object -- reset, not reallocated, between the
  native-oracle run and every backend run (see below) -- its address is
  identical across every leg of the differential check, so this is directly
  comparable rather than merely "equivalent."
* **`PtrEq`/`PtrNe`/`PtrLt`/`PtrLe`/`PtrGt`/`PtrGe`**: both kids are
  pointer-domain nodes; yields 0/1 as `T`, the same convention as the
  value-domain comparisons.
* **Pointer-domain nodes** (`PtrBase`/`PtrAdd`/`PtrSub`, `generatePtrNode` in
  `Ast.hpp`): never chosen as the AST root or by the value-domain generator
  directly -- only ever a kid of one of the nodes above. `PtrAdd`/`PtrSub`
  each take two kids: `kid[0]` is a *nested pointer-domain* node (either the
  `PtrBase` leaf, or -- up to `PTR_MAX_HOPS` deep -- another `PtrAdd`/
  `PtrSub`), `kid[1]` is the value-domain offset expression. When `kid[0]` is
  the bare `PtrBase` leaf (the common, single-hop shape), this reduces to
  exactly `base + clampBufferIndex(x)` or `end - clampBufferIndex(x)` as
  before, via `clampBufferIndex`/`clampBufferIndexTraced` (the same
  reinterpret-to-unsigned-then-modulo recipe as `Loop`'s trip count, just
  modulo `BUFFER_ELEMS`).

  When `kid[0]` is itself a nested `PtrAdd`/`PtrSub` (**compounding pointer
  arithmetic**, e.g. `(base + i) - j`), the outer hop's offset is instead
  reduced modulo however much room is *actually left* between the inner
  pointer's index and the relevant buffer edge --
  `clampIndexMod`/`clampIndexModTraced` (`EvalNative.hpp`/`EvalNautilus.hpp`),
  the generalization of `clampBufferIndex`/`clampBufferIndexTraced` to a
  caller-supplied (possibly runtime-computed) modulus instead of the fixed
  `BUFFER_ELEMS`. Concretely: `PtrAdd` on a nested pointer whose index is
  already known (by induction) to be in `[0, BUFFER_ELEMS)` computes
  `capacity = BUFFER_ELEMS - innerIndex` and adds `offset mod capacity`, so
  the result stays `<= BUFFER_ELEMS - 1`; `PtrSub` mirrors this with
  `capacity = innerIndex + 1` and subtracts, so the result stays `>= 0`.
  Because `capacity` is derived from the *actual* (data-dependent) inner
  index rather than a per-hop generation-time bound, the in-bounds invariant
  holds inductively for a chain of *any* length -- `PTR_MAX_HOPS` (currently
  2, i.e. one level of nesting beyond the original single hop) only bounds
  generation cost/tree size, not soundness. On the traced side, each hop's
  index is threaded alongside its `val<T*>` (`PtrEvalResult<T>` in
  `EvalNautilus.hpp`) purely so an outer hop can compute its own capacity
  without a pointer-minus-pointer operator (which `val<T*>` doesn't have);
  the pointer itself is built by chaining real `val<T*>::operator+`/
  `operator-` calls, one per hop, so a nested pointer genuinely exercises
  pointer arithmetic applied to the *result* of a prior pointer operation,
  not just a recomputed final index.
* **Buffer lifecycle**: the harness declares the buffer once per
  `checkOneTyped<T>` call and resets its contents (not its storage) to the
  same initial values before the native-oracle run and before each backend
  run, so Store side effects from one run never leak into the next, while
  `PtrToInt` still observes one stable address throughout.
* **`Select` and memory side effects**: `Select`'s traced form
  (`select(cond, t, f)`) is a data mux -- both `t` and `f` are always
  evaluated/lowered, unlike a real C++ ternary or `If`. Once `Store` could
  appear inside a `Select` branch, the native oracle's original ternary
  short-circuit (evaluating only the taken branch) would silently skip a
  memory write that the traced kernel still performs. The oracle's `Select`
  case now evaluates both branches unconditionally, matching the traced
  kernel exactly; `If` needed no change since its native/traced forms
  already evaluate the else-branch unconditionally and the then-branch only
  when taken (see the `Kind::If` comment in `EvalNative.hpp`).

## Kernel signature space

Every generated program's internal AST evaluation is monomorphic in one type
`T`, as above -- what additionally varies, per input, is the shape of the
`registerFunction` signature the AST is compiled into. The original harness
wired every kernel to exactly three `T`-typed parameters and a `T`-typed
return; `Harness.hpp` now draws a second byte from the input (right after the
type-select byte) and reduces it modulo `NUM_SIGNATURE_SHAPES` to pick one of
a small, bounded menu of concrete shapes, each a distinct `registerFunction`
instantiation:

* **`arity{1,2,3,4}`**: `N` `T`-typed parameters, `T`-typed return, for `N` in
  `{1, 2, 3, 4}` -- `arity3` is the original fixed shape. Covers parameter-count
  variation: empty/near-empty prologues, argument-register exhaustion, and
  (on `arity4`) stack-passed arguments, exercising `registerFunction`'s
  prologue/epilogue and argument marshalling independent of the AST itself.
* **`mixed`**: 3 logical `T` parameters, but the kernel's first *declared*
  parameter is a secondary type cross-domain from `T` (a 32-bit int when `T`
  is a float type, a `double` when `T` is an integer type -- see
  `MixedSecondaryType` in `Harness.hpp`), converted to `T` immediately at the
  boundary (both natively and traced) before joining the AST's `T`-only
  argument domain. Exercises the register-class interleaving between integer
  and floating-point kernel parameters (SysV/AAPCS64 classification), never
  reached when every declared parameter is uniformly `T`.
* **`narrowReturn`**: 3 `T`-typed parameters, but the kernel returns a fixed
  narrow integer type (`int8_t`) instead of `T`. The AST still evaluates in
  `T`; only the final result is cast down at the return boundary, on both the
  native and traced side. Exercises the same narrow-integer-return
  extension/truncation direction that previously produced real
  `ProxyCall`/`IndirectCall` ABI bugs for `Call` (see "Known findings"), this
  time at the kernel's own return rather than an internal helper call.
* **`voidReturn`**: 3 `T`-typed parameters, `void` return -- a Store-only
  kernel. A `void`-returning kernel has no return-value ABI to compare, so
  the AST's computed root value is instead explicitly written to the shared
  buffer's first slot on both the native and traced side (in addition to
  whatever `Store` side effects the AST itself performs), and the whole
  buffer is diffed elementwise afterward instead of a scalar return.

All conversions across a type boundary (`mixed`'s incoming parameter,
`narrowReturn`'s outgoing return) go through the same NaN/out-of-range-safe
recipe already used for `Kind::Cast` (`Types.hpp::convertClamped` /
`EvalNautilus.hpp::convertClampedTraced`), applied once more at the ABI
boundary instead of inside the AST, so every boundary conversion stays
well-defined for all input bytes and native/traced parity is guaranteed by
construction.

This is the kernel-boundary twin of the `invoke()`-callee coverage fuzzed by
`Kind::Call` (`Callees.hpp`): both marshal arguments/returns, but through
different code paths (`registerFunction` entry vs. `ProxyCall`), so both are
worth fuzzing independently.

## Soundness model

Within the integer domain, unsigned arithmetic uses plain operators (defined
wraparound). Signed arithmetic (`Add`/`Sub`/`Mul`/`Neg`) is computed by
promoting to the type's unsigned counterpart, operating there (defined
wraparound), and converting back (well-defined modulo 2^n) -- plain signed
overflow is still undefined behavior in C++20 even though two's-complement
representation is mandated, so the oracle avoids it explicitly while still
producing the same bit pattern every backend's two's-complement codegen
actually computes. Division/modulo divisors are forced non-zero (`| T(1)`),
and for signed types additionally forced away from `-1` -- the one divisor
value for which `TYPE_MIN / divisor` overflows (and traps via `idiv` on
x86) -- and shift amounts are masked to `[0, bit-width(T))`.

The float domain needs none of this: IEEE 754 arithmetic (including division
by zero, which produces `+-inf`/`NaN` rather than trapping) is fully defined,
so `+ - * /` and comparisons are generated unconstrained. Oracle-vs-backend
equality is NaN-aware (`a == b || (isnan(a) && isnan(b))`), since a generated
program routinely producing `NaN` on both sides is expected behavior, not a
mismatch.

This is what makes the native oracle (`EvalNative.hpp`) a valid reference for
every one of the ten domains.

## Config sweep

The differential oracle is **optimization-level-independent**: the *result* of
a well-defined program must never depend on which IR passes ran or how a value
was lowered. So beyond running each program on every backend and every
[signature shape](#kernel-signature-space), the harness runs it under several
**compiler-option permutations per backend** — each one a free extra
differential peer. The same program under two option sets must produce the same
answer, and both must match the native oracle; a disagreement is a miscompile
that a single default build would never surface.

`configs()` in `Harness.hpp` returns a list of `(backend, label, OptionsTweak)`
`Config`s. The menu is deliberately **curated and bounded** — a small set of
high-value toggles, not a cartesian product (this is a slow, soundness-oriented
fuzzer). Each compiling backend runs:

| Config label | Tweak | Why |
|--------------|-------|-----|
| `default`              | *(none)* | the backend's shipping defaults — the baseline peer |
| `no-dead-code-elim`    | `ir.disableDeadCodeElimination=true` | a default-**on** P0 pass flipped **off** |
| `no-const-branch-fold` | `ir.disableConstantBranchFolding=true` | a default-**on** P0 pass flipped **off** |
| `strength-reduction`   | `ir.enableStrengthReduction=true` | a currently default-**off** pass flipped **on** |
| `no-algebraic-simpl`   | `ir.disableAlgebraicSimplification=true` | a default-**on** P0 pass flipped **off** |

MLIR additionally runs `no-intrinsics` (`mlir.enableIntrinsics=false`). The
interpreter runs uncompiled (`engine.Compilation=false`), so the IR pipeline
never runs for it and it contributes a single `default` peer.

These are per-pass toggles that keep constant folding **on**, so the IR stays
bounded. There is deliberately **no** blanket `ir.runPasses=false` peer:
disabling the whole pipeline also disables constant folding, and the resulting
unoptimized IR of a large generated program overruns the BC backend's 16-bit
register file (see [Known findings](#known-findings)) — a pre-existing
scalability crash unrelated to the miscompiles this fuzzer hunts.

A finding records **which** permutation disagreed (the `config` field on
`Finding`, printed by the harness and part of the `--survey` bucket key), so an
option-gated bug is distinguished from a plain backend bug.

### Targeting a not-yet-default pass for a soak

The IR-pass milestone (`docs/design/ir-pass-improvements.md`, #343 and siblings)
gates flipping a pass default-on on "an extended differential-fuzzer soak before
the default flips … any finding → fix or revert the rule". This sweep is that
gate. To soak a pass that is still default-off, add one line to the
`addCompiling` menu in `configs()` (`Harness.hpp`) flipping its toggle on — the
`strength-reduction` entry is the worked example (`ir.enableStrengthReduction`).
Then run a long soak:

```bash
./nautilus/test/fuzz/nautilus-fuzz -max_total_time=3600     # coverage-guided
./nautilus/test/fuzz/nautilus-fuzz-replay --survey 20000    # or a triage survey
```

Any finding under that config's label is an option-gated miscompile: fix the
pass (or revert the default-flip), then **pin** the minimized reproducer as a
fixed `forEachBackend` regression test with the option tweak applied
(`makeEngine(backend, tweak)` — see `test/common/ExecutionTest.hpp`), and note
it under [Known findings](#known-findings).

## Build & run

Requires Clang (libFuzzer). Opt-in via `-DENABLE_FUZZING=ON`; the target is kept
out of the default `ctest` run.

```bash
cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++-21 -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target nautilus-fuzz
./nautilus/test/fuzz/nautilus-fuzz -max_total_time=120        # or -runs=100000
```

`nautilus-fuzz` needs `-fsanitize=fuzzer` support, so it specifically needs a
recent Clang; `clang++-21` is the project default but any reasonably-recent
Clang works. `nautilus-fuzz-replay` (`-DENABLE_FUZZER_REPLAY=ON`) has no
libFuzzer dependency and builds with any C++20 compiler.

Add memory/UB checking by also configuring with `-DENABLE_ADDRESS_SANITIZER=ON`
(or `-DENABLE_UB_SANITIZER=ON`); those flags are applied globally and compose
with the libFuzzer instrumentation. If Nautilus reports benign leaks during a
run, set `ASAN_OPTIONS=detect_leaks=0`.

Because compiling each random program through the JIT is slow, expect far lower
exec/s than a typical libFuzzer target — this is a soundness fuzzer, not a
throughput one. The AST size bounds in `Ast.hpp` (`MAX_DEPTH`, `MAX_NODES`) keep
each program tractable.

## Continuous fuzzing (CI)

Two scheduled/CI jobs run the harness so its coverage doesn't regress silently
between fuzzing sessions:

- **PR gate** (`.github/workflows/pr.yml`, job `fuzz-replay-smoke`): builds
  `nautilus-fuzz-replay` and runs it with no args on every push/PR. This is the
  deterministic 2000-input built-in corpus — bounded, reproducible, no
  libFuzzer dependency. Any genuine backend disagreement fails the build with
  the `Finding` report in the log (see "Known findings" below for the handful
  of pre-existing exceptions this corpus tolerates). The iteration count is
  configurable via the `NAUTILUS_FUZZ_ITERATIONS` env var if a different bound
  is ever needed.
- **Weekly sweep** (`.github/workflows/fuzz-weekly.yml`): every Monday (plus
  on-demand via `workflow_dispatch`), builds the real coverage-guided
  `nautilus-fuzz` with Clang and runs it for a large, bounded budget
  (`-max_total_time=7200 -runs=5000000` by default, both configurable as
  workflow inputs). Any crash or mismatch uploads the crash input + log as a
  build artifact and automatically files a GitHub issue (labeled
  `fuzzer-finding`) with the `Finding` report and repro steps; a recurrence of
  an already-filed finding gets a comment on the existing issue instead of a
  duplicate.

## Reproducing and pinning a finding

A crash input is a self-contained reproducer:

```bash
./nautilus/test/fuzz/nautilus-fuzz crash-<hash>            # replays the exact case
./nautilus/test/fuzz/nautilus-fuzz -minimize_crash=1 -runs=10000 crash-<hash>
```

On a mismatch the harness prints the offending **backend**, the **config**
(option permutation, see [Config sweep](#config-sweep)), the pretty-printed
**program**, the **args**, and the expected/actual values. Drop the printed
program + args into a fixed `forEachBackend` case under
`test/execution-tests/` so the bug stays covered after it is fixed. When the
`config` is anything other than `default`, the bug is **option-gated**: pass the
same tweak to `makeEngine(backend, tweak)` (e.g.
`[](engine::Options& o) { o.setOption("ir.enableStrengthReduction", true); }`)
in the regression test, or it will not reproduce.

## Known findings

The [config sweep](#config-sweep) itself surfaced a pre-existing BC-backend
scalability crash while it was being built. An early version of the sweep
included a blanket `ir.runPasses=false` peer; on a large generated `f64`
arity-4 program (deeply nested `Cast`/`If`/`Loop`), turning the *whole* IR
pipeline off — constant folding included — leaves an unoptimized IR with tens
of thousands of live SSA values. The BC backend indexes its register file with
16-bit `short`s (`OpCode`/`Code::arguments` in
`compiler/backends/bc/ByteCode.hpp`), so the register index overflows past
32767 and `BCLoweringProvider::lower` writes out of bounds, corrupting a
`bc::Code` vector's control block — the process dies in `Code::~Code()` freeing
a garbage pointer (a SIGSEGV/`free(0x1)`, no differential report). This is a
BC register-space limit, not a miscompile, and the optimization passes normally
keep the IR well under it; it is orthogonal to what this fuzzer hunts. The
sweep therefore does **not** expose `ir.runPasses=false` — it uses per-pass
toggles that keep constant folding on (see [Config sweep](#config-sweep)) — and
the BC 16-bit-register limit is left to be fixed (graceful error, or a wider
index) as a separate change.

On its first run this fuzzer reproduces a real bug: the IR constant-folding pass
folds unsigned integer comparison/division/modulo/right-shift with **signed**
semantics, so all compiled backends disagree with the interpreter for
constant-foldable `ui64` operands above `INT64_MAX`. Tracked in
nebulastream/nautilus#312. `--survey` confirms every finding reduces to this one
root cause.

The `Loop`/cross-domain-`Cast` extension surfaced a second, pre-existing bug,
**not specific to `Loop` or `Cast`** (confirmed by reproducing it with a
minimal, hand-built AST containing no `Loop`, no `Params`, no runtime input
at all) -- now fixed:

```
-4.8245185693378968e-132 / (-(0.0 + 0.0))          // f64
```

Every native-C++/IEEE-754-faithful evaluation gives `+inf` (`0.0 + 0.0` is
`+0.0`; negating it gives the exactly-defined `-0.0`; `negative / -0.0` is
`+inf`). Nautilus's interpreter silently returned `-inf` instead -- a
signed-zero handling bug reachable purely through IR interpretation, with no
backend codegen involved.

Root cause: `val<T>::operator-()` (`include/nautilus/val_arith.hpp`)
implemented unary negation as `(ValueType)0 - *this`. For floats this is not
IEEE-754 negation: subtracting two exactly-equal-magnitude operands rounds to
`+0.0` in the default round-to-nearest mode, so `0.0 - 0.0` is `+0.0`, not
the `-0.0` that negating `+0.0` must produce. This wasn't limited to the
interpreter -- the shared `ConstantFoldingAndCopyPropagationPass`'s `SubOp`
fold (plain `l - r`) reproduces the identical `+0.0` for the same reason,
so the wrong sign was already baked into the IR as a folded constant before
any backend-specific lowering ran. Fixed by negating floats via
multiplication by `-1` instead (`(ValueType)-1 * *this`): IEEE-754
multiplication's sign is always the XOR of its operands' signs, so it
flips the sign bit correctly for zero (and everything else) without going
through cancellation. Integer negation is untouched (`0 - x` is exact
two's-complement negation, no signed-zero concept applies).

This was never hit by the fuzzer before this change because generating a
literal `0.0` used to require an exact 64-bit-zero random draw (vanishingly
unlikely); `ByteReader` returns zero once its buffer is exhausted (see
`ByteReader.hpp`), and the wider/deeper trees this extension generates make
running out of buffer -- and thus landing on an exact `0.0` constant -- far
more common. `Loop` merely made this easy to *stumble into* via survey
(a `Loop` with a zero trip count folds to its `init`, which is often exactly
`0.0` for the same reason); the minimal repro above proves the bug itself
predated and was independent of `Loop`/`Cast`.

Running the harness with the AsmJit backend enabled on ARM (it is off in the
default CI configuration) surfaced a third bug -- now fixed: the A64 lowering
kept every integer in a 64-bit register and re-established the "narrow values
stay sign/zero-extended to 64 bits" invariant only at function entry and
casts, never after arithmetic. A u32 subtraction that wraps (e.g. `p0 -
4279714710u`) left the un-wrapped negative difference in the full X register,
and the next 64-bit `cmp` answered for that value instead of the wrapped
32-bit one -- so `asmjit|u8/u16/u32` (and the signed narrow widths, via left
shift and multiply overflow) could disagree with every other backend on any
comparison, division, modulo, right shift, or int->float conversion fed by
narrow-width wraparound. Fixed by mirroring the x64 provider's
`narrowToStamp` re-normalization after add/sub/mul/shift/bitwise-not (and at
returns); pinned by `u32WrapSubThenCompare` in
`test/common/ExpressionFunctions.hpp`.

The `Call` extension immediately surfaced the same invariant's fourth hole,
in **both** AsmJit providers -- now fixed: narrow integer return values of
`ProxyCall`/`IndirectCall` were bound without re-extension, but the ABI
(AAPCS64 and SysV alike) leaves the upper bits of a sub-register-width
return unspecified. An `i16` helper returning a wrapped-negative value came
back with its positive 32-bit intermediate still in the register, and the
next comparison answered for that instead. Pinned by
`i16NarrowCallReturnCompare` in `test/common/RunctimeCallFunctions.hpp`.

The same `Call` extension then caught the argument-direction twin of that
bug in the **MLIR backend** -- now fixed: `insertExternalFunction` declared
external callees without `llvm.signext`/`llvm.zeroext` argument attributes,
but several C ABIs (Darwin AArch64, x86-64 SysV) require the *caller* to
extend sub-32-bit integer arguments, and the natively compiled callee
assumes it happened. LLVM, seeing no attribute, folded the truncation away
and passed an `i16` argument with live upper register bits (in the fuzzed
case, bits of a `PtrToInt`-derived address -- which also made the mismatch
address-dependent and only reproducible ~5 out of 6 runs). Survey mode's
saved `fuzz-finding-<n>.bin` inputs plus `NAUTILUS_FUZZ_DUMP=1` were added
during this hunt precisely to make such address-dependent findings
replayable and inspectable. Pinned by `i16NarrowCallArgCompare` in
`test/common/RunctimeCallFunctions.hpp` (the truncation-folding shape LLVM
optimizes into the failing form deterministically).

A larger survey with the `StaticLoop` extension then hit the BC backend's
variant of the issue #321 merged-value bug -- now fixed: the merge block's
parameter reuses the SSA identifier that one arm re-defines, the other arm's
earlier-emitted edge binds that identifier to a fresh register first, and the
BC translator's emplace-only `Frame::setValue` silently dropped the later
definition -- so a constant folded after the merge read a register nobody
ever wrote (zero), e.g. `if(...) - 5808310683196811127` returned the merged
value unmodified. The AsmJit backends got `bindResult` for this in #322; the
BC translator now routes result registers through `getResultRegister`, which
returns the already-bound merge-parameter register so definitions write
straight into it. Minimized with plain input-file truncation + byte zeroing
(the saved `fuzz-finding-<n>.bin` shrank from 234 bytes to a 3-block
program); pinned by `zeroTripLoopMergeThenAddConstant` in
`test/common/ControlFlowFunctions.hpp`, verified to fail on the pre-fix
translator.

(An early investigation of this bug also observed every compiling backend
segfaulting on the same minimal kernel; that turned out to be an artifact of
the ad hoc standalone reproduction harness's own call-stack depth tripping
`TagRecorder`'s `__builtin_return_address`-based frame walk
(`src/nautilus/tracing/tag/TagRecorder.cpp`), not a defect introduced by this
PR -- a trivial `a + b` kernel crashes the same way through that harness, and
`--survey` never observed it through the real fuzz harness across 5000+
runs. Worth a maintainer's attention separately, since `__builtin_return_address(N)`
for `N > 0` is documented as unreliable beyond very small `N` by both GCC and
Clang, but out of scope here.)

Expanding `Callees.hpp` from two arity-2 same-type callees into the nine-entry
registry described above (arity 0-3, mixed argument types, a narrower-than-`T`
return, a void return, a pointer argument with a side effect) reshuffles the
byte-stream enough that the fixed-seed smoke corpus (`nautilus-fuzz-replay`,
no arguments) now reaches the *exact* call-stack-depth artifact predicted
above -- this time through the **real** fuzz harness, not an ad hoc one --
plus two further pre-existing defects, on a combined ~6.5% of the 2000-input
corpus (129 findings via `--survey 2000`). All three are in shared
tracer/IR infrastructure well outside `Kind::Call`'s own lowering (the actual
subject of this expansion), so the smoke corpus tolerates exactly these three
diagnosed exception strings (see `isKnownPreExistingFinding` in
`ReplayMain.cpp`) without treating them as pass/fail signal; any other
exception, or any value mismatch, still aborts the smoke corpus immediately,
and `--survey` keeps every occurrence of all three individually visible
(bucketed by backend/type/exact exception text, not merged into one
backend/type bucket) for future investigation.

1. **Tag/Snapshot collision** (`"Invalid trace. This is maybe caused by a
   constant loop."`, ~126 of the 129 findings): the exact fragility the
   paragraph above already predicted, now confirmed reachable through the
   real fuzz harness rather than merely a standalone reproduction script.
   `ExecutionTrace::processControlFlowMerge` treats a Tag/Snapshot match
   against the block currently being built as an unconditional fatal error
   (`ExecutionTrace.cpp`), but Tag identity is a raw
   `__builtin_return_address` chain (`TagRecorder::createReferenceTagBuildin`,
   `TagRecorder.cpp`) that depends only on the *sequence of recursive call
   sites* a tracing evaluator (here: this fuzzer's own `evalNautilusGeneric`)
   takes to reach an `if` -- not on which logical AST node it is. Two
   unrelated `Kind::If` nodes reached via the same shape of recursive descent
   through the evaluator can therefore collide onto the same Tag and falsely
   read as "this exact operation was already traced in the current block."
   Confirmed empirically: forcing every `Kind::Call` selection back to the
   original arity-2 same-type shape (i.e. `imm % NUM_CALLEES` always landing
   on `calleeMix`/`calleeMin`, with the rest of the new registry unreachable)
   keeps the 2000-input smoke corpus perfectly clean, while any of the wider
   shapes being reachable reintroduces the collision on unrelated trees
   elsewhere in the corpus -- i.e. the byte-stream *shift* from having more
   `Kind::Call` diversity is what surfaces it, not anything specific to a
   given new callee's own marshalling.
2. **Empty return list** (`"Invalid trace: no Return operation was
   recorded."`, hardened from a crash): a kernel combining `Kind::If` with a
   pointer `Store` and two nested `nautilus::invoke()` calls (concretely:
   `narrowReturn(A, mix(p0 - (Store(...) + ptrSwap(mem, if(...))), B))`)
   completed tracing without ever recording a `Return` operation.
   `SSACreationPhase::getReturnBlock` (`SSACreationPhase.cpp`) called
   `.front()` on that empty list -- undefined behavior, a real segfault on
   every compiled backend, reproduced deterministically via
   `nautilus-fuzz-replay` and isolated with `gdb`. Hardened here: an empty
   return list now throws a catchable `RuntimeException` instead of reading
   past the end of the vector. The underlying "why does this trace shape
   never record a Return" defect is still open -- a standalone minimization
   attempt (a small hand-written kernel with the same `if`/`Store`/nested-
   `invoke()` shape) reproduced the unrelated `TagRecorder` call-stack-depth
   artifact from finding 1 instead of this bug, confirming that artifact's
   own "a trivial kernel crashes the same way through an ad hoc harness"
   caveat and making this specific defect resistant to standalone
   minimization; it is only reliably reproduced through the real fuzz/replay
   harness's deeper call stack.
3. **Merged-value Frame lookup miss** (`"Key $N does not exists in frame."`):
   originally described (and tracked as nebulastream/nautilus#383) as a
   `Frame<K,V>` lookup miss in the cpp/bc/asmjit lowering providers for a
   `Kind::If` nested inside a `Kind::Call` argument, e.g.
   `if(p0 != 0, sum3(p1, if(...) ^ (...), p0), p2)`, and believed to be the
   same *class* of merged-value bookkeeping bug as the
   `zeroTripLoopMergeThenAddConstant` BC/AsmJit fix above (`getResultRegister`
   / `bindResult`), just a shape neither of those covers.
   A 2026-07-09 audit could not reproduce it (`nautilus-fuzz-replay --survey
   10000` came back clean) and marked it latent.

   **Update, same day, after a wider search (~2M generated programs across
   bc/tbc/asmjit) turned up a live reproducer**
   (`test/fuzz/regressions/finding3-loop-call-continue.bin`, replay with
   `nautilus-fuzz-replay test/fuzz/regressions/finding3-loop-call-continue.bin`):
   this is **not** a lowering-provider register/variable-binding bug at all --
   `getResultRegister`/`bindResult`/the cpp backend's `getVariable` are all
   fine here, and every one of them throws identically for the *same* input,
   including the interpreter-free cpp backend which has no register frame to
   get out of sync (`Frame<K,V>` is also instantiated, with a different `V`,
   by `TraceToIRConversionPhase::IRConversionContext` -- the plain trace ->
   IR conversion that runs once, before any backend sees the IR at all).
   Minimized, the crash reduces to nothing resembling the original `Kind::If`
   shape:

   ```
   loop(count=((0i8 + 0i8) + 0i8), init=0i8, body=sum3(0i8, cont(cond=0i8, value=0i8), 0i8))
   ```

   i.e. a `Kind::Loop` whose trip count traces to (but is not a literal) zero,
   whose body is a `Kind::Call` one of whose arguments is a
   `Kind::LoopContinue` (`cont`) -- no `Kind::If` required. Root cause, traced
   with instrumented builds of `SSACreationPhase.cpp`: the block reached when
   the loop's `i < trips` guard is taken (the loop body) never gets visited by
   `SSACreationPhaseContext::processBlock`'s worklist at all, even though it is
   a live successor of a real `CMP` in the recorded trace. That worklist only
   walks *backward from the recorded `Return` operation* via `Block::predecessors`
   (by design -- it's how the value-propagation in `propagateValue`/
   `processValueRef` decides what needs threading as a block argument, and
   blocks that don't reach any `Return` are legitimately dead code). Here,
   with the trip count resolved to zero, the *only* recorded `Return` is
   reached through the loop's exit edge (`i < trips` false), never through the
   body -- meaning the tracer never recorded a back-edge from the body
   (executed only via the tracer's own speculative/backtracking exploration of
   "what if `i < trips`", needed even though this program's concrete trip
   count is zero, because the compiled function must still be correct for
   whatever trip count a *future* call computes) closing back to the loop's
   own condition check. The body block is therefore real (a genuine `CMP`
   successor with real operations, including the `Kind::LoopContinue`'s own
   internal branch merge) but orphaned from the backward walk, so nothing
   ever threads its free values ($128 in the original repro, $35 in the
   minimized one -- the loop index / accumulator snapshot the `Call`
   argument's `LoopContinue` merge needs) in as block arguments.
   `TraceToIRConversionPhase` visits the orphaned block anyway (it walks every
   `CMP`/`JMP` successor unconditionally, not just backward-from-return), finds
   the value was never threaded, and `Frame::getValue` throws.

   This is a bug in the tracer's loop/back-edge recording for a
   provably-zero-trip loop, one level upstream of `SSACreationPhase`'s own
   backward walk (in the same speculative-exploration machinery already
   implicated by findings 1 and 2 below -- `ExecutionTrace`/
   `TagRecorder`/the exception-based retracing that explores a branch the
   concrete run didn't take), not a register-allocation or variable-naming
   bug in any lowering provider. A fix belongs in that tracing layer, not in
   `Frame<K,V>`, `getResultRegister`, `bindResult`, or the cpp backend's
   `getVariable` -- out of scope to attempt blind here given how fragile that
   machinery already is (see findings 1-2), but the tolerance in
   `ReplayMain.cpp` stays (this is confirmed live, not latent) and the pinned
   reproducer above makes it directly actionable for whoever picks it up.

Adding `val<bool>`/`val<enum>` as generated domains (see "Bool domain"/"Enum
domain" above) immediately surfaced four more latent bugs, all fixed by this
change, none specific to a particular generated program (every bool/enum
kernel using the affected path failed deterministically, so these were
`--survey`'s very first findings, not rare edge cases):

* `convertible_to_integral` (`val_concepts.hpp`) didn't exclude
  already-`val<>`-wrapped types the way its sibling `convertible_to_fundamental`
  does, so `val<bool> & val<bool>` matched two overloads of
  `DEFINE_ARITHMETIC_OPERATOR`'s "integral" category simultaneously (ambiguous
  call) purely because `val<bool>::operator bool()` makes `val<bool>`
  convertible to `int` too.
* The bc and tbc backends' bitwise AND/OR/XOR lowering had no `Type::b` case
  (`BCLoweringProvider.cpp`'s `visitBinaryComp`, `TBCLoweringProvider.cpp`'s
  `visitBinaryComp`) -- both throw `NotImplementedException` for any bool-typed
  `&`/`|`/`^`, since no domain had ever generated one before.
* `val<enum>`'s `operator val<underlying_type_t>()` (`val_enum.hpp`) tried to
  construct the target directly from the source's `TypedValueRefHolder`
  `state`, which doesn't compile (the target's trace-state constructor wants a
  non-const `TypedValueRef&`) -- unexercised because nothing had ever converted
  a *traced* `val<enum>` back to its underlying integer before.
* `val<enum>::operator=` (`val_enum.hpp`) assigned through a `const T value`
  member, which cannot compile for any assignment at all -- unexercised
  because nothing had ever assigned to a `val<enum>` before.

Rebasing this change onto the `Call`-coverage expansion above (which also
restricted bool to a smaller shape menu, since `mixed`/`narrowReturn` need a
Cast neither backend supports for bool -- see "Bool domain") reshuffled the
byte-stream once more and surfaced a fourth pre-existing, `bool`/`enum`-
unrelated defect (confirmed via a bisection against a clean checkout of the
pre-rebase `main`: the exact same finding reproduces there too, through a
different i64 program of its own, once that checkout's own copy of the
tolerance-filter bug below is worked around):

4. **Wrong-alternative variant access** (`"std::get: wrong index for
   variant"`): an internal `std::variant` accessed through the wrong
   alternative somewhere in the cpp backend's lowering of a `voidReturn`-shaped
   `i64` kernel (issue #355/#363) whose AST mixes `Kind::If`, a `StaticLoop`,
   and a `Kind::Call`. In a Debug build (asserts compiled in) the same
   underlying inconsistency instead trips an earlier
   `assert(currentOperation.op == op)` in `LazyTraceContext::follow`
   (`LazyTraceContext.cpp`) before this exception is ever thrown; CI builds
   Release (`NDEBUG` defined, `pr.yml`), where only this catchable exception is
   observable, so it does not block CI. Root-causing the variant misuse is out
   of scope here (it predates and is independent of the bool/enum domains this
   change adds).

Wiring the deterministic corpus into CI for the first time (`fuzz-replay-smoke`,
nebulastream/nautilus#376) immediately surfaced a fifth pre-existing finding on
the very first CI run -- confirmed independent of that PR (reproduces from a
clean `main` checkout the same way):

5. **MLIR module verification failure** (`"verification of MLIR module
   failed!"`) -- **now fixed** (nebulastream/nautilus#377): a `u16`/`mixed`-shape
   kernel --
   ```
   ((((((mem + idx(39420u16)) + idx((i64)(if(p1 != 0, p1, p2)))) <= (mem + idx((uintptr_t)((mem + idx(60247u16)))))) ? 1 : 0) * ((((mem < mem) ? 1 : 0) == p0) ? 1 : 0)) - ((mem <= (mem - idx((while(init=(p1 << 3044u16), cond=cont(cond=3715u16, value=29463u16), body=(*(mem) = p2)) ^ 11218u16)))) ? 1 : 0))
   ```
   -- reached `MLIRLoweringProvider`'s `cf.cond_br` emission
   (`resolveOperand(ifOp->getValue(), frame)`) with an `i32` operand where MLIR
   requires `i1`, so MLIR's own verifier rejected the module before LLVM
   lowering ever ran (`'cf.cond_br' op operand #0 must be 1-bit signless
   integer, but got 'i32'`). Root cause: `val<bool>`'s **default constructor**
   (`include/nautilus/val_bool.hpp`) seeded its initial `false` state via an
   `int` literal -- `tracing::traceConstant(0)` deduces `T = int`, so the
   constant was stamped `Type::i32` rather than `Type::b`, giving every
   default-constructed `val<bool>` an `i32` trace state. The `while` body's
   `cont` (`Kind::LoopContinue`) drives the enclosing loop's default-constructed
   `val<bool>` break/continue signal (`TracedLoopSignal` in `EvalNautilus.hpp`),
   so the `if (signal)` post-body check branched on that mis-stamped `i32`.
   Fixed by seeding the default state with a `bool` literal
   (`tracing::traceConstant(false)`), matching how `val(bool)` and
   `val_arith.hpp`'s `traceConstant<raw_type>(0)` already stamp their constants.
   Pinned by `defaultConstructedBoolBranch` in `test/common/BoolOperations.hpp`
   (exercised across every backend by `BoolExecutionTest.cpp`), and the smoke
   corpus no longer tolerates this exception.

Investigating finding 4 above also caught the tolerance filter itself in
`ReplayMain.cpp`'s `isKnownPreExistingFinding` failing silently: it compared
`Finding::what` against findings 1-3's bare messages with exact (`==`)
equality, but `RuntimeException` (`exceptions/RuntimeException.cpp`)
unconditionally appends a full stack trace to its message, so none of the
three could ever actually match once `ENABLE_STACKTRACE` resolves real
symbols (as it always did while developing this change) -- the smoke corpus
aborted on the very first pre-existing finding it hit instead of tolerating
it. Fixed by switching to prefix matching (`std::string::starts_with`),
consistent with how the "Key $" case already worked.
