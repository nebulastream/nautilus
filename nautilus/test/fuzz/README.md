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
  (`& | ^ << >> ~`), unary negate, comparisons, `Select`/`If`, and `Cast`
  (round-trips the value through another *integer* type, e.g. `(T)(To)v`,
  exercising sign/zero-extension and truncation codegen).
* **Float domain** (`float`, `double`): arithmetic (`+ - * /`), unary
  negate, comparisons, `Select`/`If`, and `Cast` (round-trips through the
  *other* float type only).
* **Not generated, by design**: casts between the integer and float domains.
  Casting an out-of-range float to an integer type is undefined behavior in
  C++, which would need explicit range clamping (matching the native and
  traced sides exactly) to fuzz safely -- left as a follow-up rather than
  built half-way. Loops/control-flow-loop generation are likewise out of
  scope for now -- today's `If`/`Select` already exercise branch lowering;
  loop generation needs a data-dependent, bounded trip count threaded through
  both the oracle and the traced kernel and is a separate piece of work.

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

## Known finding

On its first run this fuzzer reproduces a real bug: the IR constant-folding pass
folds unsigned integer comparison/division/modulo/right-shift with **signed**
semantics, so all compiled backends disagree with the interpreter for
constant-foldable `ui64` operands above `INT64_MAX`. Tracked in
nebulastream/nautilus#312. `--survey` confirms every finding reduces to this one
root cause.
