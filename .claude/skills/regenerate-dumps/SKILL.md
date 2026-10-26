---
name: regenerate-dumps
description: Update the reference dumps in nautilus/test/data (traces and IR after each stage) when a change intentionally alters traces or IR. Use when nautilus-tracing-tests fails with a dump mismatch or writes a new reference file, or after changing the tracer, the trace-to-IR conversion, IR passes or the IR printer.
---

# Regenerate reference dumps

`nautilus-tracing-tests` (built with Clang only) compares traces and IR dumps
with files under `nautilus/test/data/<category>/<stage>/<name>.<ext>`, where
`<stage>` is `tracing`, `ir`, `after_constant_folding`, `after_empty_block_elim`
and so on. The helper is `nautilus/test/common/ReferenceDumpHelper.hpp`: when a
reference file is **missing** it writes the current dump and fails, so the
next run passes.

## Rules

- **A dump update is its own commit,** never mixed with the behavior change
  that caused it. Reviewers diff the dumps separately.
- **Every changed dump must be explained** by the change: read the diff of
  each file. An unexpected change is a bug, not a dump to update.
- Short-circuit builds (`-DENABLE_SHORT_CIRCUIT_BOOL=ON`) use sibling files
  named `<name>.shortcircuit.<ext>`; update them with that configuration.

## Steps

1. Build with Clang: `cmake --build build/dev --target nautilus-tracing-tests`.
2. Run and collect the failing cases:
   ```bash
   ./build/dev/nautilus/test/execution-tests/nautilus-tracing-tests 2>&1 | tee /tmp/dumps.log
   ```
3. For each mismatching reference, delete the file and rerun twice: the first
   run writes it, the second must pass.
   ```bash
   git rm -q nautilus/test/data/<category>/<stage>/<name>.<ext>
   ./build/dev/nautilus/test/execution-tests/nautilus-tracing-tests "<test case>"
   ./build/dev/nautilus/test/execution-tests/nautilus-tracing-tests "<test case>"
   ```
4. `git diff --stat nautilus/test/data` and read every changed file.
5. Commit only `nautilus/test/data` with a message naming the cause, for
   example "Regenerate IR dumps for folded branch probabilities".
