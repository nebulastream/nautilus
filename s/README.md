# Scripts Directory

This directory contains utility scripts for the Nautilus project.

## Available Scripts

### `regenerate-test-references.sh`

**Wrapper script** that regenerates all test reference files (both trace and LLVM IR tests).

**Usage:**
```bash
./s/regenerate-test-references.sh [OPTIONS]
```

**Options:**
- `--trace-only` - Only regenerate trace test references
- `--ir-only` - Only regenerate LLVM IR test references
- `--help` - Show help message

**Examples:**
```bash
# Regenerate all references (both trace and IR)
./s/regenerate-test-references.sh

# Only regenerate trace test references
./s/regenerate-test-references.sh --trace-only

# Only regenerate LLVM IR test references
./s/regenerate-test-references.sh --ir-only
```

This script is a convenience wrapper that calls the individual scripts below.

---

### `regenerate-trace-data.sh`

Regenerates **trace test** reference files (tracing, ir, and after_ssa stages).

**Usage:**
```bash
./s/regenerate-trace-data.sh [--help]
```

**What it does:**
- Deletes all `.trace` files from `nautilus/test/data/*/tracing/`, `*/ir/`, and `*/after_ssa/` directories
- Runs the trace tests, which automatically regenerate missing reference files
- Covers all test categories:
  - bool-tests
  - cast-tests
  - control-flow-tests
  - enum-tests
  - expression-tests
  - loop-tests
  - pointer-tests
  - regressions
  - runtime-call-tests
  - static-loop-tests

**Verification after running:**
```bash
cd build
ctest --test-dir nautilus -R "Trace Test" --output-on-failure
```

---

### `regenerate-llvm-ir-data.sh`

Regenerates **LLVM IR test** reference files.

**Usage:**
```bash
./s/regenerate-llvm-ir-data.sh [--help]
```

**What it does:**
- Deletes all `.ll` files from `nautilus/test/llvm-ir-test/reference-ir/`
- Runs the LLVM IR tests, which automatically regenerate missing reference files
- Uses llvm-diff-21 (or llvm-diff) for validation
- Normalizes IR to remove machine-specific attributes

**Verification after running:**
```bash
cd build
ctest --test-dir nautilus -R "LLVM IR Test" --output-on-failure
```

---

## When to Use These Scripts

Use these scripts when:
- Making intentional changes to code generation that affect test outputs
- Updating optimization phases that change IR structure
- Adding new compiler features that modify generated code
- Updating LLVM/MLIR versions
- Fixing bugs that change expected compiler output

**⚠️ Warning:** These scripts delete and regenerate reference files! Always:
1. Review what changed in the generated output
2. Verify changes are intentional and correct
3. Run tests to ensure all pass
4. Commit the updated references with a clear explanation

## Test Reference File Structure

### Trace Test References

Located in: `nautilus/test/data/`

Structure:
```
nautilus/test/data/
├── bool-tests/
│   ├── tracing/       # Raw execution trace
│   ├── ir/            # Generated IR (control flow graph)
│   └── after_ssa/     # Post-SSA transformation
├── cast-tests/
├── control-flow-tests/
├── enum-tests/
├── expression-tests/
├── loop-tests/
├── pointer-tests/
├── regressions/
├── runtime-call-tests/
└── static-loop-tests/
```

Each category contains three subdirectories representing different stages of the compilation pipeline:
- **tracing/** - Direct execution trace output
- **ir/** - Intermediate representation (control flow graph)
- **after_ssa/** - IR after SSA (Static Single Assignment) transformation

### LLVM IR Test References

Located in: `nautilus/test/llvm-ir-test/reference-ir/`

Structure:
```
nautilus/test/llvm-ir-test/
├── reference-ir/
│   ├── int8AddExpression.ll
│   ├── handleEnum.ll
│   ├── boolNot.ll
│   ├── ifThenCondition.ll
│   ├── sumLoop.ll
│   └── ... (200+ .ll files)
└── intrinsic-ir/      # Generated intrinsic IR (not references)
```

These are normalized LLVM IR files used for comparison during testing. The normalization removes machine-specific attributes like:
- `target-cpu` and `target-features`
- `tune-cpu` optimization hints
- `captures()` attributes
- `range()` attributes

## Contributing

When adding new scripts to this directory:
1. Make them executable: `chmod +x s/script-name.sh`
2. Add clear usage documentation with `--help` flag
3. Update this README with script description
4. Follow the existing naming convention (lowercase with hyphens)
5. Include error handling and colored output for better UX
