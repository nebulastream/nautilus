# Scripts Directory

Utility scripts for regenerating test reference files in Nautilus.

## Scripts

### `regenerate-test-references.sh` (Wrapper)

Regenerates all test reference files (trace + LLVM IR).

```bash
./s/regenerate-test-references.sh              # All references
./s/regenerate-test-references.sh --trace-only # Trace only
./s/regenerate-test-references.sh --ir-only    # LLVM IR only
```

Calls the individual scripts below.

---

### `regenerate-trace-data.sh`

Regenerates trace test references for every pipeline stage.

```bash
./s/regenerate-trace-data.sh
```

Deletes the existing reference files under `nautilus/test/data/*/` and reruns tests to regenerate them. Raw execution traces (`tracing/`, `after_ssa/`) live in `.trace` files; Nautilus IR dumps (`ir/`, `after_constant_folding/`, `after_empty_block_elim/`) live in `.nautilus` files.

**Verify:** `cd build && ctest --test-dir nautilus -R "Trace Test" --output-on-failure`

---

### `regenerate-llvm-ir-data.sh`

Regenerates LLVM IR test references.

```bash
./s/regenerate-llvm-ir-data.sh
```

Deletes all `.ll` files from `nautilus/test/llvm-ir-test/reference-ir/` and reruns tests to regenerate them.

**Verify:** `cd build && ctest --test-dir nautilus -R "LLVM IR Test" --output-on-failure`

---

## When to Use

Use these scripts when:
- Intentionally changing code generation or compiler output
- Updating optimization phases
- Upgrading LLVM/MLIR versions
- Fixing bugs that affect generated code

**⚠️ Warning:** These scripts delete reference files. Always review changes before committing.

## Test Reference Structure

**Trace tests:** `nautilus/test/data/{category}/{tracing,after_ssa,ir,after_constant_folding,after_empty_block_elim}/`
- Categories: bool-tests, cast-tests, control-flow-tests, enum-tests, expression-tests, loop-tests, pointer-tests, regressions, runtime-call-tests, static-loop-tests
- Raw execution traces (`tracing`, `after_ssa`) use the `.trace` extension; Nautilus IR dumps (`ir`, `after_constant_folding`, `after_empty_block_elim`) use the `.nautilus` extension

**LLVM IR tests:** `nautilus/test/llvm-ir-test/reference-ir/*.ll`
- Normalized LLVM IR files (machine-specific attributes removed)
