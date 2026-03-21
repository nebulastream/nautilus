# Loops

## Overview

Nautilus supports standard C++ loop constructs including `for` loops, `while` loops, `break`, and `continue` when used with `val<T>` variables. The Nautilus tracer records loop behavior during execution so that loops can be compiled to efficient native code through the JIT compilation pipeline.

## For Loops

Use `val<T>` for the loop counter and any traced values within the loop body.

```cpp
val<int32_t> sumLoop(val<int32_t> upperLimit) {
    val<int32_t> sum = 1;
    for (val<int32_t> i = 0; i < upperLimit; i = i + 1) {
        sum = sum + 10;
    }
    return sum;
}
```

You can also use the increment operators `i++` or `++i`:

```cpp
for (val<int32_t> i = 0; i < limit; i++) {
    // ...
}
```

## While Loops

While loops work the same way as in standard C++. The loop condition must involve `val<T>` values for the tracer to capture the loop structure.

```cpp
val<int32_t> whileExample(val<int32_t> limit) {
    val<int32_t> result = 0;
    while (result < limit) {
        result = result + 10;
    }
    return result;
}
```

## Nested Loops

Loops can be nested to any depth. Each loop is independently traced and compiled.

```cpp
val<int32_t> nestedLoop(val<int32_t> upperLimit) {
    val<int32_t> sum = 1;
    for (val<int32_t> i = 0; i < upperLimit; i = i + 1) {
        for (val<int32_t> j = 0; j < upperLimit; j = j + 1) {
            sum = sum + 10;
        }
    }
    return sum;
}
```

## Break

The `break` statement exits the innermost loop early. It can be used in both `for` and `while` loops.

### Break in a For Loop

```cpp
val<int32_t> forBreak(val<int32_t> condition) {
    val<int32_t> result = 0;
    for (val<int32_t> i = 0; i < 10; i = i + 1) {
        if (i == condition) {
            break;
        }
        result = result + 10;
    }
    return result;
}
```

### Break in a While Loop

```cpp
val<int32_t> whileBreak(val<int32_t> condition) {
    val<int32_t> result = 0;
    while (result < 100) {
        if (result == condition) {
            break;
        }
        result = result + 10;
    }
    return result;
}
```

## Continue

The `continue` statement skips the rest of the current iteration and proceeds to the next one.

```cpp
val<int32_t> whileContinue(val<int32_t> condition) {
    val<int32_t> i = 0;
    val<int32_t> result = 0;
    while (i < 10) {
        i++;
        if (result > condition) {
            continue;
        }
        result = result + 10;
    }
    return result;
}
```

## Conditionals Inside Loops

Loops and conditionals compose naturally. You can use `if` and `if/else` statements inside any loop.

### If Inside a Loop

```cpp
val<int32_t> ifInsideLoop(val<int32_t> value) {
    val<int32_t> result = 0;
    for (val<int32_t> i = 0; i < value; i = i + 1) {
        if (i % 2 == 0) {
            result += 1;
        }
    }
    return result;
}
```

### If/Else Inside a Loop

```cpp
val<int32_t> ifElseSumLoop() {
    val<int32_t> agg = 1;
    for (val<int32_t> i = 0; i < 10; i = i + 1) {
        if (agg < 50) {
            agg = agg + 10;
        } else {
            agg = agg + 1;
        }
    }
    return agg;
}
```

## Static Loops

Loops that use a `static_val` counter are fully unrolled at trace time. This means the tracer expands every iteration inline rather than emitting a loop in the generated code. See [static-val.md](static-val.md) for details.
