# Linear SSA lookup for static loops

## Goal

Make SSA construction scale near-linearly for a straight-line trace created by a `static_val` loop, while preserving the trace and compiled result.

## Problem

`SSACreationPhaseContext::isLocalValueRef` currently scans every earlier operation in a block to decide whether a consumed value is local. A static loop is fully unrolled during tracing, so a 4,000-iteration loop produces one large block and repeats that prefix scan for many operands. The resulting SSA work is quadratic in the number of recorded operations.

## Design

Build a lazily cached index for each block that maps a `ValueRef` produced by an operation to that operation's index. A value is locally available at a consuming operation when either it is a block argument or its indexed definition precedes the consumer. This exactly preserves the existing ordering rule without rescanning a prefix.

Keep the existing cross-block `blockDefinitions` cache and eager propagation logic unchanged. The new index is only used for same-block locality checks and is populated once per accessed block.

## Validation

Add a focused static sum fixture with 1,000, 2,000, and 4,000 iterations. Verify the 4,000-iteration function returns the arithmetic-series result after compilation and that SSA conversion removes all `ASSIGN` operations. Benchmark SSA construction across these sizes and accept the change only when scaling is near-linear rather than quadratic; use repeated samples and compare growth ratios, not one absolute timing.

Run the focused tracing/execution tests, the dedicated benchmark, and the repository formatter before committing.

## Scope

This change does not alter trace semantics, block argument propagation, IR generation, or backend optimizations. It does not introduce a general dominator-based SSA algorithm.
