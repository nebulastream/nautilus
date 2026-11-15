# Control Flow

## Overview

Nautilus supports standard C++ control flow constructs. When conditions involve `val<bool>` values, both branches are traced and compiled into efficient machine code by the JIT compiler. This means you write ordinary C++ if-statements and loops, and Nautilus handles the tracing automatically.

## If Statements

A simple if-statement with a `val<bool>` condition traces the branch and the fall-through path.

```cpp
val<int32_t> ifThenCondition(val<int32_t> value) {
    val<int32_t> result = 1;
    if (value == 42) {
        result = result + 1;
    }
    return result + 42;
}
```

## If-Else

Both the true and false branches are traced and compiled.

```cpp
val<int32_t> ifThenElse(val<int32_t> value) {
    val<int32_t> result = 1;
    if (value == 42) {
        result = result + 1;
    } else {
        result = result + 42;
    }
    return result + 42;
}
```

## Nested If-Else

Branches can be nested to arbitrary depth.

```cpp
val<int32_t> nestedIf(val<int32_t> value, val<int32_t> x) {
    if (value == 42) {
        // first branch
    } else {
        if (x == 8) {
            // nested branch
        } else {
            x = x + 2;
        }
    }
    return x + 2;
}
```

## Else-If Chains

Else-if chains work as expected and are traced as a sequence of conditional branches.

```cpp
val<int32_t> elseIfChain(val<int32_t> x) {
    val<int32_t> result = 0;
    if (x == 1) {
        result = 10;
    } else if (x == 2) {
        result = 20;
    } else {
        result = 30;
    }
    return result;
}
```

## Logical Operators in Conditions

Conditions can use `||` (logical OR) and `&&` (logical AND) to combine `val<bool>` expressions.

### Logical OR

```cpp
val<int32_t> orCondition(val<int32_t> value) {
    val<int32_t> result = 1;
    if (value == 8 || value == 1) {
        result = result + 14;
    }
    return result;
}
```

### Logical AND

```cpp
val<int32_t> andCondition(val<int32_t> x, val<int32_t> y) {
    val<int32_t> result = 1;
    if (x == 8 && y == 1) {
        result = result + 14;
    }
    return result;
}
```

### Logical NOT

```cpp
val<int32_t> notCondition(val<int32_t> x) {
    val<int32_t> result = 0;
    if (!(x == 5)) {
        result = 1;
    }
    return result;
}
```

## Conditional Returns

Functions can return early from within a branch. The tracer records both the early return path and the fall-through path.

```cpp
val<int32_t> conditionalReturn(val<int32_t> value) {
    if (value == 42) {
        return 1;
    }
    return 120;
}
```

## How It Works

When a `val<bool>` is converted to `bool` inside an if-statement, the tracer intercepts the conversion through `operator bool()`. The tracing system then uses symbolic execution to explore both branches of the conditional:

1. The true branch is executed and its operations are recorded.
2. The false branch is executed and its operations are recorded.
3. Both paths are merged back at the point after the if-statement.

The JIT compiler then generates efficient branch code from the traced paths. If probability hints have been set on the `val<bool>` condition (see [val-type.md](val-type.md#probability-hints)), the compiler can further optimize by placing the likely branch on the fast path.

## Explicit Control Flow

Native `if`, `for` and `while` over a `val<bool>` make the tracer run the function again for every path it explores,
and a function with many independent branches replays its prefix once per branch. `nautilus/control_flow.hpp` states the
same control flow explicitly, and the tracer records it in one pass:

```cpp
#include "nautilus/control_flow.hpp"

val<int64_t> clampedSum(val<int64_t*> data, val<int64_t> n, val<int64_t> limit) {
    val<int64_t> sum = 0;
    For(val<int64_t>(0), n, [&](val<int64_t> i) {
        If(data[i] < 0, [&] { Continue(); });
        sum = sum + data[i];
        If(sum > limit, [&] { Break(); });
    });
    return sum;
}
```

| Construct | Meaning |
|---|---|
| `If(c, then)`, `If(c, then, else)` | a branch; the arms are lambdas |
| `val<T> v = If(c, [&] { return a; }, [&] { return b; })` | the value form: only the taken arm is evaluated |
| `While(cond, body)` | `cond` returns a fresh `val<bool>` |
| `For(begin, end, body)`, `For(begin, end, step, body)` | a counted loop over `[begin, end)`; `body` gets the index |
| `For(init, cond, step, body)` | the three-clause loop |
| `Break()`, `Continue()` | leave the innermost explicit loop, or start its next iteration (`For` runs the step first) |
| `Return(v)` | return `v` from the traced function; the function still needs its own `return` for C++ |

**Mixing with native control flow.** Every arm, loop header, body and step is a scope of its own, as a `region()` body
is. A native `if` inside an arm replays only that arm, and a native `if` outside a construct skips the construct when
the enclosing function runs again; it is never recorded twice. A function written entirely with explicit constructs
traces in one pass (`tracing.iterations` counts one pass for the function and one per arm), takes one snapshot per
construct and one per operation outside the constructs (`tracing.snapshots`), and replays nothing
(`tracing.replayedOperations` is 0).

**Early exits are function calls.** C++ `break` cannot leave a lambda, so the code after `Break()`, `Continue()` or
`Return()` in the same lambda still runs, but the tracer stops recording it. The owned values (`val<std::string>`, any
class type with a destructor) between the exit and its target are destroyed on the exit path, as native `break` would
do. An exit cannot leave a `region()` body, and `Break()` and `Continue()` need an enclosing explicit loop. If every path
through an `If` leaves, the code after the `If` is unreachable and not recorded.

**Values leave a construct through `val`s declared outside it**, as with `region()`: a `val` created inside an arm or a
body must not outlive it.

**Without the NIR tracer.** Without tracing the lambdas run natively (`Break()` and `Continue()` unwind out of the body
lambda). With the 0.3 tracer (`tracing.nirTracer=false`) the constructs trace as the native control flow they stand for.
`Return` needs the NIR tracer.

**Generating constructs in a host loop.** Make the loop variable a `static_val`, so the operations of different
iterations get different snapshots (as for any unrolled code).

**Optimistic recording.** The operations of an arm or body are recorded without snapshots. If the scope meets a native
branch it starts over with snapshots, after rolling back everything it recorded (`tracing.constructRestarts` counts
these). As for a `region()` replay, C++ state that the lambda changed is not rolled back, so keep lambdas free of side
effects on the host. `tracing.forceConstructRestarts=true` starts every scope over once, which the tests use.
