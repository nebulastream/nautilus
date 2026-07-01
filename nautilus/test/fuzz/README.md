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
| `Types.hpp`        | The `TypeId` domain (10 types), imm pack/unpack, type dispatch helpers, value formatting/equality. |
| `ByteReader.hpp`   | Deterministic `FuzzedDataProvider`-style view over the fuzzer buffer. |
| `Ast.hpp`          | Tagged AST, depth/node-budgeted generator (templated on the value type), and a pretty printer. |
| `EvalNative.hpp`   | Independent C++ ground-truth interpreter (the oracle), templated on the value type. |
| `EvalNautilus.hpp` | Walks the same AST emitting `val<T>` ops for tracing/compilation. |
| `Harness.hpp`      | Shared differential check (`checkOne`/`runOne`) used by both drivers; dispatches each input to the right `T`. |
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
    print findings bucketed by `(backend, type, kind)`, to triage how many
    distinct bug classes exist (e.g. an `i8`-only bug breaks out from an
    `f64`-only one instead of merging into one bucket).

## Scope: ten value domains

Each generated program is monomorphic in one of ten types, picked from the
first byte of the fuzzer input: `int8_t/16/32/64_t`, `uint8/16/32/64_t`,
`float`, `double` (`TypeId` in `Types.hpp`). The rest of the input (AST shape,
constants, parameters) is generated and evaluated entirely in that one type,
mirroring how the original `uint64_t`-only fuzzer worked.

* **Integer domain** (8 widths/signs): arithmetic (`+ - * / %`), bitwise
  (`& | ^ << >> ~`), unary negate, comparisons, `Select`/`If`, `Loop`, and
  `Cast` (round-trips the value through another type, e.g. `(T)(To)v`,
  exercising sign/zero-extension/truncation codegen *and* int<->float
  boundary conversion).
* **Float domain** (`float`, `double`): arithmetic (`+ - * /`), unary
  negate, comparisons, `Select`/`If`, `Loop`, and `Cast` (round-trips through
  another integer *or* float type).
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

## Reproducing and pinning a finding

A crash input is a self-contained reproducer:

```bash
./nautilus/test/fuzz/nautilus-fuzz crash-<hash>            # replays the exact case
./nautilus/test/fuzz/nautilus-fuzz -minimize_crash=1 -runs=10000 crash-<hash>
```

On a mismatch the harness prints the offending **backend**, the pretty-printed
**program**, the **args**, and the expected/actual values. Drop the printed
program + args into a fixed `forEachBackend` case under
`test/execution-tests/` so the bug stays covered after it is fixed.

## Known findings

On its first run this fuzzer reproduces a real bug: the IR constant-folding pass
folds unsigned integer comparison/division/modulo/right-shift with **signed**
semantics, so all compiled backends disagree with the interpreter for
constant-foldable `ui64` operands above `INT64_MAX`. Tracked in
nebulastream/nautilus#312. `--survey` confirms every finding reduces to this one
root cause.

The `Loop`/cross-domain-`Cast` extension surfaced a second, pre-existing
finding, **not specific to `Loop` or `Cast`**: minimal repro (no Loop, no
Params, no runtime input at all) --

```
-4.8245185693378968e-132 / (-(0.0 + 0.0))
```

evaluated as `f64` arithmetic. Every native-C++/IEEE-754-faithful evaluation
gives `+inf` (`0.0 + 0.0` is `+0.0`; negating it gives the exactly-defined
`-0.0`; `negative / -0.0` is `+inf`) -- confirmed independently with a plain
C++ program outside Nautilus entirely. Nautilus disagrees in two distinct
ways:

* The **interpreter** backend (`engine.Compilation = false`, no native
  codegen at all) silently returns `-inf` instead of `+inf` -- a signed-zero
  handling bug reachable purely through IR interpretation.
* Every **compiling** backend (`mlir`, `cpp`, `bc`, `asmjit`) **segfaults**
  during `registerFunction` on the exact same minimal kernel, with an
  identical crash signature regardless of which backend is selected --
  strongly suggesting the crash lives in the shared trace-to-IR/optimization
  pipeline that runs before backend-specific lowering, not in any one
  backend's codegen.

This was never hit by the fuzzer before this change because generating a
literal `0.0` used to require an exact 64-bit-zero random draw (vanishingly
unlikely); `ByteReader` returns zero once its buffer is exhausted (see
`ByteReader.hpp`), and the wider/deeper trees this extension generates make
running out of buffer -- and thus landing on an exact `0.0` constant -- far
more common. `Loop` merely made this easy to *stumble into* via survey
(a `Loop` with a zero trip count folds to its `init`, which is often exactly
`0.0` for the same reason); the minimal repro above proves the bug itself
predates and is independent of `Loop`/`Cast`.
