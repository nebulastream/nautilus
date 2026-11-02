---
name: asm-diff
description: Compare the machine code the asmjit backend generates before and after a change, or against the recorded 0.3 baseline. Use when changing X64NirLowering.cpp, A64NirLowering.cpp, the post-RA peepholes, the constant pool or an asmjit option, or when the asm-diff CI leg reports a regression.
---

# asm-diff

The `asmjit` backend must not generate worse code than 0.3's did
(`docs/plans/nautilus-0.4/05-backends.md` §4.3). Two views:

## 1. Size against the recorded 0.3 corpus (the CI leg)

The latency-corpus driver records `asmjit.codeSize.bytes` per function and size; `tools/asm-diff/asm-diff.py` compares
it with `nautilus/test/baselines/0.3/corpus.jsonl` and fails when a row grew.

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_MLIR_BACKEND=OFF -DENABLE_BENCHMARKS=ON -G Ninja -S . -B /tmp/corpus-bld
cmake --build /tmp/corpus-bld --target nautilus-latency-corpus
/tmp/corpus-bld/nautilus/test/benchmark/nautilus-latency-corpus 1 > corpus.jsonl
python3 tools/asm-diff/asm-diff.py corpus.jsonl
```

## 1b. The loops of the kernels against the 0.3 lowering (the CI leg too)

`nautilus-asm-kernels` (`tools/asm-diff/kernels.cpp`) holds loops that run long enough for code quality to show
(`fib`, `sum`, an internal and an external call loop). `python3 tools/asm-diff/loop-diff.py <binary>` compiles each on
both pipelines (NIR and `tracing.nirTracer=false`) and fails when a loop of the NIR lowering has more instructions.
`nautilus-asm-kernels run [reps]` times them (microseconds per call, best of reps); pin the CPU (`taskset`) and compare
the two pipelines in the same process, since code placement moves a run time by tens of percent.

## 2. The instructions of one function, before and after

```bash
NAUTILUS_OPTIONS="engine.backend=asmjit;dump.after_asmjit_assembly=true;dump.console=true" ./your-program > after.txt
```

Build the program on the base commit and on your change, run both, and diff `after.txt` (labels and register names are
stable between runs of one build). The same dump with `tracing.nirTracer=false` is the 0.3 lowering of the same
function, which is what a "no regression" claim is measured against. Look for extra `mov`s at block edges (parallel
copies), missed immediates (`mov r, imm` before an `add`/`cmp`), a compare that is materialised and tested instead of
fused, and a divisor or shift amount that is masked although it did not have to be.

Counters that tell which optimisation fired are under `asmjit.lowering.*` in `fn.getStatistics()`; the options that
turn each off are in `docs/options.md`.
