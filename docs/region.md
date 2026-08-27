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

An optional debug label can be passed as the first argument (`nautilus::region("name", [&]() { ... })`); it is not recorded in the trace and has no effect on behavior.

## Why use a region

- **Liveness isolation.** Intermediate values created inside the region don't extend the liveness of unrelated values around it.
- **Tag isolation.** The region gets its own tag space, so its internal branches and loops never alias tags with the enclosing function.
- **Memoized replay.** Once a region has been fully traced for a given combination of call site and enclosing state, re-entering it with the same state skips re-tracing the body entirely.
- **Bounded branch-tracing cost.** A branch or loop structurally inside a region only costs what's inside that region — it does not require re-running the rest of the enclosing function to explore it (see [How regions trace branches](#how-regions-trace-branches) below).

All four come from the region-local exploration loop, so all four require `engine.traceMode = "lazyTracing"` (the default). Under `"exceptionBasedTracing"` a region is a no-op: the body is traced inline, into the enclosing function's trace, exactly as if `region()` were not there. That tracer restarts the whole enclosing function on every unresolved branch, so there is nothing for a region to bound. Both tracers produce an equivalent trace either way; the difference is only how much work tracing does to get there.

None of this requires a function boundary: region blocks are ordinary basic blocks and region entry/exit are ordinary jumps. There is no region-specific operation anywhere in the trace, the IR, or any backend.

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

This is purely a tracing-time (compile-time) effect — it changes how much work the tracer does to build the execution trace, not the runtime behavior or the generated code. The trace, IR, and compiled output for a regioned function are equivalent to the unregioned version; regions with branches inside them just get there faster.

## Scope and limitations

- **Regions only do anything under `engine.traceMode = "lazyTracing"`** (the default tracer), as described in [Why use a region](#why-use-a-region). Under `"exceptionBasedTracing"` they are inlined away.
- **A value that escapes a region must be built on every path through it, under `lazyTracing`.** `lazyTracing` explores a region's internal branches by re-invoking the region body once per path, and checks that each pass leaves the same set of values alive at the region's end. Building the escaping value inside one arm of an `if` fails that check — each arm produces its own value with no merge point, so what escapes would depend on which path happened to be explored last — and is reported as an error rather than traced wrongly. Building it unconditionally, or writing it to a `val<T>` declared outside the region (assignment merges across branches, as in the examples above), works. `exceptionBasedTracing` traces one path per engagement and accepts either form.
- **Region lambdas return `void`.** There's no mechanism for a region to hand back a value directly — use a captured reference instead, as in the examples above.
- **Calls inside a region are traced like any other operation.** A runtime call (`invoke`), an indirect call, a `nautilus::function` call, or an `alloca` inside a region body is recorded against the region, the same as arithmetic and control flow. A `nautilus::function` callee is still traced as its own function — the region records only the call — so a region never changes what happens on the other side of that boundary. Local exploration re-invokes the region body once per path, but that only re-*records* the call: an `invoke` target is opaque and is never executed at trace time, inside a region or outside one.
- **Writes to an enclosing `static_val` from inside a region are not detected.** `static_val` writes are invisible to the tracer in general (not a region-specific limitation); avoid writing to a captured `static_val` from inside a region body.
