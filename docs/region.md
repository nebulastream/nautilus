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

- **Region lambdas return `void`.** There's no mechanism for a region to hand back a value directly — use a captured reference instead, as in the examples above.
- **Local branch-tracing only covers the operations a region traces directly:** arithmetic, comparisons, assignments, and (as of the current implementation) `if`/`while`/`for` control flow. A runtime call, indirect call, `nautilus::function` call, or `alloca` inside a region still forwards to the enclosing context rather than being resolved locally — regions containing only arithmetic/comparison logic get the full local-exploration benefit; regions that also call out to other traced functions do not lose correctness, just some of the performance benefit for that call.
- **Writes to an enclosing `static_val` from inside a region are not detected.** `static_val` writes are invisible to the tracer in general (not a region-specific limitation); avoid writing to a captured `static_val` from inside a region body.
