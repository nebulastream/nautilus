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
| `ByteReader.hpp`   | Deterministic `FuzzedDataProvider`-style view over the fuzzer buffer. |
| `Ast.hpp`          | Tagged AST, depth/node-budgeted generator, and a pretty printer. |
| `EvalNative.hpp`   | Independent C++ ground-truth interpreter (the oracle). |
| `EvalNautilus.hpp` | Walks the same AST emitting `val<T>` ops for tracing/compilation. |
| `Harness.hpp`      | Shared differential check (`checkOne`/`runOne`) used by both drivers. |
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
    print findings bucketed by `(backend, kind)`, to triage how many distinct
    bug classes exist.

## Soundness model

All values are `uint64_t`, which never integer-promotes and has defined
wraparound, so native and traced arithmetic match by construction. Partial
operations are made total during generation: division/modulo divisors are
`| 1` (never zero) and shift amounts are masked to `[0, 64)`. This is what makes
the native oracle a valid reference. Floating point and signed overflow are
intentionally excluded (see the plan / follow-ups).

## Build & run

Requires Clang (libFuzzer). Opt-in via `-DENABLE_FUZZING=ON`; the target is kept
out of the default `ctest` run.

```bash
cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++-21 -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target nautilus-fuzz
./nautilus/test/fuzz/nautilus-fuzz -max_total_time=120        # or -runs=100000
```

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
