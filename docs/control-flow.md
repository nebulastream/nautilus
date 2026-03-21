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
