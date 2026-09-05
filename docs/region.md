# Regions

## Overview

`nautilus::region()` marks a bounded section of a traced function as an isolated inline scope. Unlike `nautilus::function`, which creates a whole new compiled function (with its own tracing instance and an `invoke` call), a region records into the *same* execution trace as its enclosing function — it's a lighter-weight boundary for cases where you just want to scope tag identity, value liveness, and branch-tracing cost to a sub-block of code, without introducing a real function call.

```cpp
val<int64_t> withRegion() {
    val<int64_t> sum = 0;
    nautilus::region([&]() {
        sum = sum + 42;
    });
    return sum;
}
```

The lambda is captured by reference (`[&]`). Reads and writes to captured `val<T>`/`static_val<T>` variables work exactly as they do anywhere else in a traced function — a write inside the region is visible after it, through the captured reference:

```cpp
val<int64_t> escapingWrite() {
    val<int64_t> out = 0;
    nautilus::region([&]() {
        val<int64_t> t = 7;   // lives only inside the region
        out = t;              // escapes through the captured reference
    });
    return out + 1;
}
```

An optional name can be passed as the first argument (`nautilus::region("name", [&]() { ... })`). It is recorded, together with the call site's source location, as the region's *attributes* — see [Region attributes](#region-attributes) below.

## Region attributes

Every region carries two attributes describing the call site it was written at:

- a **source location** — file, line, column and enclosing function, captured automatically via `std::source_location::current()`;
- an **optional name** — whatever string was passed as the first argument, or none.

```cpp
nautilus::region("accumulate", [&]() { sum = sum + 42; });   // named
nautilus::region([&]() { sum = sum + 42; });                 // unnamed, still located
```

Attributes are metadata. They never take part in tag or snapshot identity, and a named region traces and compiles exactly like an unnamed one. What they do is make a region identifiable, in three places:

**Diagnostics.** A region body that the tracer has to reject is reported against the call site the user wrote, instead of leaving them to find it:

```
Invalid region() "accumulate" at src/Query.cpp:42:9: a value created inside the region
body outlives it ($7). Carry the value out through a val<T> declared outside the region ...
```

**The trace.** Each region traced under `lazyTracing` is recorded in the trace's region table (`ExecutionTrace::getRegions()`), which pairs the attributes with the two blocks that bound the body — the block the body starts in and the block the enclosing scope continues in — and with the region enclosing it. Every operation recorded inside the body points at that entry through `TraceOperation::regionIndex`, and so does every block created while the body was being traced (`Block::regionIndex`) — the body's own blocks and the blocks of any branch or loop inside it. The trace dump prints the attributes in front of the entry block:

```
; region "accumulate" at src/Query.cpp:42:9
B1()
	...
```

Under `exceptionBasedTracing` a region body is traced inline into the enclosing function (see below), so there are no bounding blocks to attach anything to and the region table stays empty. The attributes are still accepted and still cost nothing.

**The IR.** `TraceToIRConversionPhase` carries both attributes across: each function's `FunctionOperation` gets a region table (`getRegionSpecs()`), and both operations and blocks name a region in it.

- **`Operation::getRegionIndex()`** is exact: the innermost region that operation was traced inside.
- **`BasicBlock::getRegionIndex()`** is a summary: the innermost region containing *every* operation in the block. A block starts out as the region its code was traced in and widens to the nearest common ancestor (`FunctionOperation::commonRegionAncestor`) whenever a pass moves operations from another region into it.
- **`RegionSpec::parent`** gives the enclosing chain, and **`RegionSpec::id`** is the region's identity: unique across the module, assigned once at conversion and never reused, so a diagnostic that spans functions can name a region unambiguously — the index alone means nothing outside its function.

The IR dump states a block's region once on the block, and an operation adds a trailer only where its own region is deeper than its block's:

```
Block_3($2:i64): ; region #0 "branching" at src/Query.cpp:42:9
	$7 = 10 :i32
	$9 = $2 + $8 :i64
	br Block_2($9) :void
```

Attributing the operations is what makes the metadata survive the pipeline: the entry and exit block a region adds are single-predecessor seams that `EmptyBlockEliminationPass` and `BlockMergingPass` collapse (see [How regions trace branches](#how-regions-trace-branches)), and a block that swallows a region's code widens to the region containing both — while the operations themselves keep naming their own region wherever they end up. Blocks that survive whole, like the arms of a branch inside a region body, keep naming it directly. Operations a pass mints carry no region at all; constant folding hands the folded operation the provenance of the one it replaced.

`IRVerifier` checks the result: every index names a region of its function, parent chains terminate, and every attributed operation is from the region its block claims or from one nested inside it. A pass that moves code between regions without widening the block it moves into is caught there.

What all of this looks like at each stage of the pipeline is checked in under `nautilus/test/data/region-tests/` — the trace, the trace after SSA, and the IR before and after the block-cleanup passes, for the fixtures in `nautilus/test/common/RegionFunctions.hpp`, plus the same fixture traced by `exceptionBasedTracing` for comparison (`regionNested_inlined`, where no region appears at all). Those dumps are printed with source locations turned off (see below), so they name regions without naming a machine or a compiler; which line each `region()` sits on is asserted in `RegionTest.cpp` instead.

### Turning the source location off

`log::options::setLogSourceLocations(false)` drops the ` at file:line:column` half of every region marker, in both the trace and the IR dump:

```
Block_3($2:i64): ; region #1 "inner"
	; nested in region #0 "outer"
```

It is on by default, because relating traced code back to the source is the point of recording the location at all. Turn it off when a dump has to be identical across machines and compilers — a checked-in golden trace, as above — since the path is wherever the build compiled from and the column is the compiler's own choice: for one and the same `region()` call GCC reports the callee's closing position and Clang the start of the expression. The name, and in the IR the id, are printed either way, so a dump without locations still says which region every block and operation belongs to. The flag changes printing only; the attributes themselves, and the diagnostics that quote them, always carry the full location.

The IR table holds one entry per region() call site per enclosing chain, not one per traced engagement: a region inside a statically unrolled loop is entered once per iteration, and all of those iterations are the same `region()` in the source. The one thing the IR does not keep is the region *boundary* — after the cleanup passes there is no block that starts a region, which is exactly the point of a region costing nothing in the generated code.

No backend reads any of this today; it is provenance for reading, verifying and debugging the IR.

### Naming a region from a helper

A helper that wraps `region()` would otherwise report *its own* body as the call site, the same position for each of its callers. Passing the attributes explicitly lets it describe its caller instead:

```cpp
template <typename F>
void accumulateRegion(const char* name, F&& fn,
                      std::source_location location = std::source_location::current()) {
    nautilus::region(nautilus::RegionAttributes {name, nautilus::SourceLocation::from(location)},
                     std::forward<F>(fn));
}
```

The name is stored, not copied — pass a string literal or another string that outlives the trace.

`example/src/DemoRegions.cpp` is a runnable version of all of this: three nested regions (`scan` > `classify` > `accumulate`, the innermost opened through exactly such a helper) over a small aggregation, with the trace and IR dumps printed so each stage can be read against the source it came from. Build and run it with `cmake --build build --target demo_regions && ./demo_regions` from `example/`.

## Why use a region

- **Liveness isolation.** Intermediate values created inside the region don't extend the liveness of unrelated values around it.
- **Tag isolation.** The region gets its own tag space, so its internal branches and loops never alias tags with the enclosing function.
- **Memoized replay.** Once a region has been fully traced for a given combination of call site and enclosing state, re-entering it with the same state skips re-tracing the body entirely.
- **Bounded branch-tracing cost.** A branch or loop structurally inside a region only costs what's inside that region — it does not require re-running the rest of the enclosing function to explore it (see [How regions trace branches](#how-regions-trace-branches) below).

All four come from the region-local exploration loop, so all four require `engine.traceMode = "lazyTracing"` (the default). Under `"exceptionBasedTracing"` a region is a no-op: the body is traced inline, into the enclosing function's trace, exactly as if `region()` were not there. That tracer restarts the whole enclosing function on every unresolved branch, so there is nothing for a region to bound. Both tracers produce an equivalent trace either way; the difference is only how much work tracing does to get there.

None of this requires a function boundary: region blocks are ordinary basic blocks and region entry/exit are ordinary jumps. There is no region-specific operation anywhere in the trace, the IR, or any backend — a region is metadata on ordinary operations, never an operation of its own.

## Nesting

Regions can nest:

```cpp
val<int64_t> nested() {
    val<int64_t> sum = 0;
    nautilus::region([&]() {
        nautilus::region([&]() { sum = sum + 1; });
        sum = sum + 2;
    });
    return sum;
}
```

A nested region is an ordinary part of the enclosing region's body, so control
flow may come before it, after it, or between two of them:

```cpp
val<int64_t> nestedThenBranch(val<int64_t> x) {
    val<int64_t> sum = 0;
    nautilus::region([&]() {
        nautilus::region([&]() { sum = sum + 1; });
        if (x > 0) {
            sum = sum + 10;
        } else {
            sum = sum + 20;
        }
    });
    return sum;
}
```

## Branches and loops inside a region

Ordinary C++ control flow — `if`/`else`, `while`, `for` — works inside a region body exactly as it does anywhere else in a traced function:

```cpp
val<int64_t> internalBranch(val<int64_t> x) {
    val<int64_t> sum = 0;
    nautilus::region([&]() {
        if (x > 0) {
            sum = sum + 10;
        } else {
            sum = sum - 1;
        }
    });
    return sum;
}
```

This works whether the branch is the only thing in the region or one of several:

```cpp
val<int64_t> chainedBranches(val<int64_t> a, val<int64_t> b, val<int64_t> c) {
    val<int64_t> sum = 0;
    nautilus::region([&]() {
        if (a > 0) { sum = sum + 1; } else { sum = sum + 100; }
        if (b > 0) { sum = sum + 2; } else { sum = sum + 200; }
        if (c > 0) { sum = sum + 4; } else { sum = sum + 400; }
    });
    return sum;
}
```

Control flow does not have to be the last thing in the body — further traced
operations, and further regions, can follow it:

```cpp
val<int64_t> workAfterBranch(val<int64_t> x) {
    val<int64_t> sum = 0;
    nautilus::region([&]() {
        if (x > 0) {
            sum = sum + 10;
        } else {
            sum = sum + 20;
        }
        sum = sum + 1;   // continues in the block the two arms merge into
    });
    return sum;
}
```

and for a native loop inside a region:

```cpp
val<int64_t> internalLoop(val<int64_t> n) {
    val<int64_t> sum = 0;
    nautilus::region([&]() {
        val<int64_t> i = 0;
        while (i < n) {
            sum = sum + 1;
            i = i + 1;
        }
    });
    return sum;
}
```

## How regions trace branches

Tracing a function with an `if` explores both branches by symbolically re-running the function up to the next unresolved decision, using a snapshot-tag map to remember which branches have already been recorded. Without regions, that re-run always restarts from the top of the *enclosing function* — so N independent branches, each needing on the order of one extra pass to resolve, cost roughly O(N²) in total tracing work: O(N) passes, each re-running the whole O(N)-sized function.

A region gives its own body an independent exploration loop with its own snapshot-tag map, scoped to just that region. Resolving a branch inside the region only re-runs the region's body, not the rest of the enclosing function. Wrapping each of N independent branches in its own region turns that O(N²) cost into O(N): each region still costs a small, bounded number of local passes, but those passes are cheap and their number no longer compounds with the size of the surrounding function.

The mechanism is the one the tracer already uses for functions. Tracing a function means exploring a body path by path against its own `TagRecorder`, its own `SymbolicExecutionContext`, its own live-value and `static_val` state, and its own entry block. A region is the *same* loop over the *same* kind of context — the only difference is that a function scope records into its own execution trace and starts at block 0, while a region scope records into the enclosing scope's trace starting at a freshly created block, and ends each pass with a jump to the region's exit block instead of a `return`. Because a region's exploration runs to completion before `region()` returns, a later pass of the enclosing function skips the body entirely and resumes at that exit block.

This is purely a tracing-time (compile-time) effect — it changes how much work the tracer does to build the execution trace, not the runtime behavior. A regioned function and its unregioned equivalent compile to the same control-flow graph: the entry and exit block a region adds are single-predecessor seams, which the IR pipeline's empty-block and block-merging passes collapse. They are not textually identical, and the regioned form is the smaller of the two: exploring a region independently records the code after an internal branch once, in the merge block, where the unregioned tracer re-records it on every incoming path. The `Region IR Matches Unregioned IR` test pins both halves of that — equal block counts, and no more operations than the unregioned form.

## Scope and limitations

- **Regions only do anything under `engine.traceMode = "lazyTracing"`** (the default tracer), as described in [Why use a region](#why-use-a-region). Under `"exceptionBasedTracing"` they are inlined away.
- **No value created inside a region may outlive it, under `lazyTracing`.** Carry values out by assigning to a `val<T>` declared *outside* the region, as in the examples above. Constructing a `val<T>` inside the body and keeping it alive past the body — in a `std::optional`, a `std::vector`, or any object declared outside — is reported as an error rather than traced wrongly, and the error names the values involved. The reason is that such a value's ref is allocated by the region, so it cannot exist before the body runs; a `val<T>` declared outside has a ref allocated before the region and stable across it, which is what makes it safe. Once a region has been traced, re-entering it on a later exploration pass of the enclosing function skips the body entirely — that is the memoization — so a value that only the body can create is simply not there on that pass, and any operation recorded afterwards would reference an object that was never constructed. `exceptionBasedTracing` inlines region bodies and accepts either form.
- **Region lambdas return `void`.** There's no mechanism for a region to hand back a value directly — use a captured reference instead, as in the examples above.
- **Calls inside a region are traced like any other operation.** A runtime call (`invoke`), an indirect call, a `nautilus::function` call, or an `alloca` inside a region body is recorded against the region, the same as arithmetic and control flow. A `nautilus::function` callee is still traced as its own function — the region records only the call — so a region never changes what happens on the other side of that boundary. Local exploration re-invokes the region body once per path, but that only re-*records* the call: an `invoke` target is opaque and is never executed at trace time, inside a region or outside one.
- **Writes to an enclosing `static_val` from inside a region are not detected.** `static_val` writes are invisible to the tracer in general (not a region-specific limitation); avoid writing to a captured `static_val` from inside a region body.
