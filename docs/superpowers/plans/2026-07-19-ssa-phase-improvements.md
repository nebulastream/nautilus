# SSA Phase Improvements Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Improve SSA verification, production-path overhead, propagation scaling, scratch-memory reuse, and benchmark coverage while preserving generated SSA semantics.

**Architecture:** Centralize nested trace-value traversal, make the SSA context borrow its trace, and keep the forward dense availability table. Use lazy unique-owner plus per-block assignment definitions for predecessor queries, arena-backed dense traversal state, and an arena checkpoint to release phase-local scratch immediately.

**Tech Stack:** C++20, Catch2 benchmarks/tests, Nautilus `common::Arena`, Clang 21, CMake/Ninja.

## Global Constraints

- Multi-return idempotence is out of scope.
- Preserve the existing public `SSACreationPhase::apply` overloads and generated SSA semantics.
- Use Clang 21 and run the CMake `format` target before the final commit.
- Follow strict red-green-refactor: every behavior change gets a failing test before production code.
- Do not regenerate dumps unless an intentional deterministic ordering change requires it.

---

### Task 1: Arena Checkpoint and Rewind

**Files:**
- Modify: `nautilus/src/nautilus/common/Arena.hpp`
- Create: `nautilus/test/tracing-tests/ArenaTest.cpp`
- Modify: `nautilus/test/tracing-tests/CMakeLists.txt`

**Interfaces:**
- Produces: `Arena::Checkpoint Arena::checkpoint() const noexcept`
- Produces: `void Arena::rewind(const Checkpoint&) noexcept`
- Consumes: existing arena region, chunk, and destructor bookkeeping.

- [ ] **Step 1: Add failing checkpoint tests**

Add Catch2 cases that record a checkpoint, allocate within the inline region and across heap-chunk growth, register a tracked object after the checkpoint, rewind, and verify:

```cpp
auto checkpoint = arena.checkpoint();
auto* first = arena.allocate(64, alignof(std::max_align_t));
arena.rewind(checkpoint);
REQUIRE(arena.allocate(64, alignof(std::max_align_t)) == first);
```

Use a small RAII probe with a destruction counter to verify that rewind destroys only objects created after the checkpoint.

- [ ] **Step 2: Run the test and verify RED**

Run:

```bash
cmake --build build --target nautilus-tracing-unit-tests -j2
build/nautilus/test/tracing-tests/nautilus-tracing-unit-tests "Arena*"
```

Expected: compilation fails because `checkpoint` and `rewind` do not exist.

- [ ] **Step 3: Implement checkpoint and rewind**

Add an opaque nested `Checkpoint` carrying arena identity, current pointer/end, current region index, and destructor count. `rewind` must run post-checkpoint destructors in reverse order, shrink the destructor list, and restore the saved bump state while retaining allocated chunks.

- [ ] **Step 4: Run the test and verify GREEN**

Run the command from Step 2. Expected: all `Arena*` cases pass.

- [ ] **Step 5: Commit**

```bash
git add nautilus/src/nautilus/common/Arena.hpp nautilus/test/tracing-tests/ArenaTest.cpp \
  nautilus/test/tracing-tests/CMakeLists.txt
git commit -m "Add reusable arena checkpoints"
```

### Task 2: Shared Value Traversal and Sound SSA Verification

**Files:**
- Modify: `nautilus/src/nautilus/tracing/TraceOperation.hpp`
- Modify: `nautilus/src/nautilus/tracing/phases/SSAVerifier.cpp`
- Create: `nautilus/test/tracing-tests/SSAVerifierTest.cpp`
- Modify: `nautilus/test/tracing-tests/CMakeLists.txt`

**Interfaces:**
- Produces: const `forEachValueRef(const InputVariant&, Fn&&)` traversal by value.
- Produces: mutable `forEachValueRef(InputVariant&, Fn&&)` traversal by reference.
- Consumes: `TypedValueRef`, `BlockRef`, `FunctionCall`, and `IndirectFunctionCall` alternatives.

- [ ] **Step 1: Add failing verifier tests**

Construct small traces directly with arena-allocated operations and verify rejection of:

```cpp
// Undefined function pointer and undefined argument nested inside an indirect call.
IndirectFunctionCall {.fnPtr = TypedValueRef(70001, Type::ptr),
                      .arguments = {TypedValueRef(70002, Type::i32)}};
```

Also construct a valid trace using refs above 65,535, and a trace whose operation result duplicates a block argument.

- [ ] **Step 2: Run the tests and verify RED**

Run:

```bash
cmake --build build --target nautilus-tracing-unit-tests -j2
build/nautilus/test/tracing-tests/nautilus-tracing-unit-tests "SSAVerifier*"
```

Expected: the invalid indirect-call and block-argument collision cases are incorrectly accepted before the fix.

- [ ] **Step 3: Add shared traversal helpers**

Implement exhaustive const and mutable helpers in `TraceOperation.hpp`. Const block-argument traversal captures the original count and copies each `TypedValueRef` before invoking the callback so callbacks may grow self-loop argument vectors safely.

- [ ] **Step 4: Fix verifier identifier types and operand coverage**

Use `uint32_t` for block IDs and `ValueRef` for definition sets. Initialize duplicate-definition tracking with block arguments and route all operation inputs through the const traversal helper.

- [ ] **Step 5: Run verifier and existing function-pointer tests**

Run:

```bash
build/nautilus/test/tracing-tests/nautilus-tracing-unit-tests "SSAVerifier*"
build/nautilus/test/execution-tests/nautilus-tracing-tests "Function Pointer Trace Test"
```

Expected: all selected tests pass.

- [ ] **Step 6: Commit**

```bash
git add nautilus/src/nautilus/tracing/TraceOperation.hpp \
  nautilus/src/nautilus/tracing/phases/SSAVerifier.cpp \
  nautilus/test/tracing-tests/SSAVerifierTest.cpp nautilus/test/tracing-tests/CMakeLists.txt
git commit -m "Strengthen SSA value verification"
```

### Task 3: Refactor SSA Context and Scratch Indexes

**Files:**
- Modify: `nautilus/src/nautilus/tracing/phases/SSACreationPhase.hpp`
- Modify: `nautilus/src/nautilus/tracing/phases/SSACreationPhase.cpp`
- Modify: `nautilus/test/execution-tests/TracingTest.cpp`

**Interfaces:**
- Changes internal context constructor to `explicit SSACreationPhaseContext(ExecutionTrace& trace)`.
- Produces lazy `isDefinedInBlock(uint32_t blockId, ValueRef ref)` using unique owners plus assignment ranges.
- Consumes `Arena::checkpoint`, `Arena::rewind`, and shared value traversal.

- [ ] **Step 1: Add failing scratch-reuse and high-live-in correctness tests**

Replace the existing capacity-growth-only assertion with a checkpoint-reuse assertion: record an external checkpoint after tracing a single-return function, apply SSA, allocate a probe, rewind to the external checkpoint, and require that the same-sized allocation returns the probe address. Add a traced loop carrying at least 64 independent values and require `VerifySSA` success plus correct IR/execution behavior.

- [ ] **Step 2: Run selected tests and verify RED**

Run:

```bash
cmake --build build --target nautilus-tracing-tests -j2
build/nautilus/test/execution-tests/nautilus-tracing-tests \
  "SSA creation reuses arena scratch,SSA creation handles many loop live-ins"
```

Expected: scratch reuse fails because the arena bump pointer remains after the SSA tables.

- [ ] **Step 3: Borrow the trace and remove synthetic ownership**

Store `ExecutionTrace& trace` in the context. The single-trace overload constructs the context from `*trace`; the module overload passes `*tracePtr` directly and performs no custom-deleter `shared_ptr` construction.

- [ ] **Step 4: Add dense traversal state and hybrid definitions**

Allocate and zero arena-backed spans for availability, unique non-assignment definition owner, processed blocks, and per-block lazy assignment ranges. On first query of a block, scan its operations: stamp non-`ASSIGN` results in the owner table and store only `ASSIGN` results in a sorted block-local range.

- [ ] **Step 5: Make argument propagation linear**

Use successful `propagatedValues.insert` as the uniqueness gate and append block/edge arguments directly. Reserve worklists using block count. Replace hand-written variant ladders and `processBlockRef` copies with shared const traversal. Route assignment rewriting through mutable traversal.

- [ ] **Step 6: Rewind scratch and remove dead code**

Take an arena checkpoint after durable return normalization, use an RAII rewind guard so exceptions also release scratch, and remove `makeBlockArgumentsUnique`.

- [ ] **Step 7: Run tracing and execution tests**

Run:

```bash
build/nautilus/test/execution-tests/nautilus-tracing-tests
build/nautilus/test/execution-tests/nautilus-execution-tests
```

Expected: all enabled tests pass with no dump changes unless traversal ordering intentionally changes.

- [ ] **Step 8: Commit**

```bash
git add nautilus/src/nautilus/tracing/phases/SSACreationPhase.hpp \
  nautilus/src/nautilus/tracing/phases/SSACreationPhase.cpp \
  nautilus/test/execution-tests/TracingTest.cpp
git commit -m "Improve SSA propagation and scratch reuse"
```

### Task 4: Production and Adversarial SSA Benchmarks

**Files:**
- Modify: `nautilus/test/benchmark/TracingBenchmark.cpp`

**Interfaces:**
- Produces benchmark cases for production `TraceModule` application and high-live-in propagation.
- Restores nested/chained control-flow cases in the normal SSA matrix.

- [ ] **Step 1: Capture the pre-change benchmark baseline**

Run the current committed binary before rebuilding and save results outside Git tracking:

```bash
build/nautilus/test/benchmark/nautilus-benchmarks \
  "SSA Creation Benchmark,SSA Creation Static Loop Scaling Benchmark" \
  --benchmark-samples 100 --benchmark-no-analysis --benchmark-warmup-time 100
```

Expected: record all means for comparison.

- [ ] **Step 2: Add benchmark coverage**

Remove the obsolete nested/chained skip. Add a module benchmark that traces through `TraceContext::Trace` and measures `SSACreationPhase::apply(shared_ptr<TraceModule>)`. Add high-live-in cases at 32, 64, and 128 carried values using a fresh trace per measured invocation.

- [ ] **Step 3: Build and run the expanded benchmarks**

Run:

```bash
cmake --build build --target nautilus-benchmarks -j2
build/nautilus/test/benchmark/nautilus-benchmarks "SSA Creation*" \
  --benchmark-samples 100 --benchmark-no-analysis --benchmark-warmup-time 100
```

Expected: static scaling remains near-linear, high-live-in doubling avoids quadratic growth, and existing means do not materially regress.

- [ ] **Step 4: Commit**

```bash
git add nautilus/test/benchmark/TracingBenchmark.cpp
git commit -m "Expand SSA performance coverage"
```

### Task 5: Full Verification and Delivery

**Files:**
- Modify generated fixtures only if selected tests demonstrate an intentional deterministic ordering change.

**Interfaces:**
- Consumes all previous tasks.
- Produces a formatted, tested, benchmarked branch and refreshed PR checks.

- [ ] **Step 1: Run formatting and static checks**

```bash
cmake --build build --target format
./format.sh
git diff --check
```

Expected: all commands succeed and no unrelated files change.

- [ ] **Step 2: Run the full local test set**

```bash
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Expected: all configured tests pass.

- [ ] **Step 3: Run platform-sensitive Linux repros**

Use the existing Clang 21 x86_64 Docker repro to run normal and short-circuit static tracing plus GPU tracing/codegen tests. Expected: all assertions pass without new fixture mismatches.

- [ ] **Step 4: Review final performance and diff**

Compare the 100-sample before/after benchmark means, inspect `git diff --stat`, `git diff --check`, and every changed production file. Confirm multi-return idempotence was not added.

- [ ] **Step 5: Commit any final fixture or formatting changes**

```bash
git status --short
git add nautilus/test/data plugins/gpu/test/data nautilus/test/llvm-ir-test/reference-ir
git commit -m "Finalize SSA phase improvements"
```

Run the `git add` command only when the status lists intentional generated fixture changes under those directories; stage
the listed files individually if unrelated files are present. Skip this commit if the worktree is already clean.

- [ ] **Step 6: Push and watch CI**

```bash
git push
gh pr checks 411 --watch --interval 15
```

Expected: all PR checks pass.
