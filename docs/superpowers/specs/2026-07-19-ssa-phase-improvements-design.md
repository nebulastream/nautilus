# SSA Phase Improvements Design

## Goal

Improve the tracing SSA phase's correctness checks, production-path overhead, asymptotic behavior, scratch-memory
lifetime, and benchmark coverage without changing its generated SSA semantics. Multi-return idempotence is explicitly
out of scope.

## Current constraints

- `ValueRef` and block identifiers are 32-bit dense identifiers.
- A forward scan must distinguish a definition before the current use from a later definition in the same block.
- Non-`ASSIGN` result references have one defining operation, while `ASSIGN` may redefine the same `ValueRef` in
  multiple blocks.
- Trace operations and durable synthetic operations use the trace's existing `common::Arena`.
- The production compiler applies SSA to a `TraceModule`; the legacy single-trace overload remains supported.
- Generated trace and IR output must remain semantically unchanged. Fixture regeneration is allowed only when a
  deterministic ordering change is intentional.

## Design

### Shared value-reference traversal

Add const and mutable helpers for visiting every `TypedValueRef` nested in an `InputVariant`, including direct values,
block arguments, direct-call arguments, and indirect-call function pointers and arguments. Const traversal snapshots a
block argument count and passes references by value so SSA propagation can append arguments to self-loop edges without
invalidating traversal. Mutable traversal permits assignment rewriting without changing container sizes.

`SSACreationPhase`, assignment elimination, and `SSAVerifier` will use these helpers. This removes duplicated variant
dispatch and ensures new operand kinds cannot silently be omitted from verification.

### Verifier correctness

Use `ValueRef` and `uint32_t` instead of `uint16_t`. Validate indirect-call operands through the shared traversal and
treat block arguments as definitions for duplicate-definition checks. Add negative tests for an undefined indirect-call
function pointer, an undefined indirect-call argument, an operation result colliding with a block argument, and
identifiers above 65,535.

### SSA context and traversal state

Make `SSACreationPhaseContext` hold `ExecutionTrace&`. Public `shared_ptr` overloads retain their existing API, while the
module overload passes the owned trace by reference and no longer creates a synthetic non-owning `shared_ptr` control
block.

Replace `processedBlocks` with an arena-backed byte span indexed by dense block ID. Reserve the main and propagation
worklists to avoid repeated growth.

### Locality and definition index

Retain the arena-backed `availableInBlock[ValueRef]` stamp table and forward operation scan for the common
definition-before-use check.

For predecessor propagation, use a hybrid lazy index:

- `uniqueDefinitionBlock[ValueRef]` records the defining block stamp for non-`ASSIGN` results.
- Each queried block stores a compact sorted list of its `ASSIGN` target references.
- A block defines a reference when either the unique owner stamp matches or its assignment list contains the reference.

The index is built lazily per queried block, preserving the fast single-block path while avoiding storage and sorting for
ordinary operation results.

### Linear argument propagation

`propagatedValues` remains the uniqueness authority for `(block, value)` pairs. Once insertion succeeds, append the block
argument directly instead of performing another linear duplicate search. A single propagation visits every matching
predecessor edge and directly appends that edge argument. Debug assertions may validate the uniqueness invariant without
adding release-build scans.

Replace `processBlockRef`'s vector copy with shared const traversal that snapshots the original argument count.

### Scratch-memory lifetime

Extend `common::Arena` with a checkpoint that records its current region, bump pointer, and destructor-stack size.
Rewinding destroys objects registered after the checkpoint, restores the bump pointer, and retains allocated chunks for
reuse. SSA creates the checkpoint after durable return-block normalization, allocates all scratch indexes after it, and
rewinds before returning. No durable trace object may be allocated after this checkpoint.

Add arena tests covering rewind within one region, rewind after chunk growth, destructor handling, and immediate reuse.
Add an SSA regression test confirming scratch capacity is reusable after the phase rather than merely allocated from the
trace arena.

### Cleanup and benchmarks

Remove the unused `makeBlockArgumentsUnique` implementation. Restore `nestedIf10`, `nestedIf100`, `chainedIf10`, and
`chainedIf100` to the existing SSA benchmark matrix. Add:

- a production `TraceModule` SSA benchmark;
- a high-live-in loop benchmark that carries enough values to expose quadratic propagation;
- the existing 1k/2k/4k static scaling benchmarks unchanged for longitudinal comparison.

Benchmark before and after on the same Release build and report both latency and scaling. Correctness verification covers
the dedicated verifier and arena tests, tracing tests, execution tests, formatting, and the full CI matrix.

## Error handling and invariants

- Dense tables are sized from `lastValueRef + 1` and block count; release builds throw `RuntimeException` for an
  out-of-range reference instead of relying only on assertions.
- Arena rewind requires a checkpoint produced by the same arena and used in stack order; debug assertions enforce this.
- Multi-return normalization remains single-application behavior in this change.

## Success criteria

- All new negative verifier tests fail before their fixes and pass afterward.
- SSA output remains accepted by `VerifySSA` and all existing trace/IR fixtures.
- Production module application performs no synthetic `shared_ptr` allocation per function.
- High-live-in propagation scales linearly with argument count.
- SSA scratch allocations are reusable before IR conversion.
- Existing benchmark improvements are preserved without a statistically meaningful regression.
