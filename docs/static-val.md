# Static Values and Compile-Time Loop Unrolling

## Overview

`static_val<T>` is an integral type that exists at trace time, not at runtime. Unlike `val<T>`, a `static_val<T>` is evaluated during tracing, which means loops using `static_val` counters are fully unrolled. This is useful for fixed-iteration loops where the iteration count is known at trace time.

## static_val\<T\>

Basic usage:

```cpp
#include <nautilus/static.hpp>

val<int32_t> staticLoop() {
    val<int32_t> sum = 1;
    for (static_val<int> i = 0; i < 10; i = i + 1) {
        sum = sum + 10;
    }
    return sum;
}
```

This loop is fully unrolled during tracing. The generated IR contains 10 sequential additions instead of a loop. The `static_val` variable `i` only exists at trace time; it does not appear as a runtime variable.

## static_val vs val

Key differences:

| Feature | `val<T>` | `static_val<T>` |
|---------|----------|-----------------|
| Exists at runtime | Yes | No |
| Traced as IR variable | Yes | No |
| Loop behavior | Dynamic loop in IR | Fully unrolled |
| Supported types | All arithmetic + bool + ptr + enum | Integral types only |
| Use case | Runtime-dependent values | Compile-time-known iteration |

## Increment and Decrement

Both pre and post operators work with `static_val`:

```cpp
for (static_val<int> i = 0; i < 10; i++) { ... }    // post-increment
for (static_val<int> i = 0; i < 10; ++i) { ... }    // pre-increment
```

While loop with decrement:

```cpp
val<int32_t> countdown() {
    val<int32_t> sum = 1;
    static_val<int32_t> limit = 10;
    while (limit >= 0) {
        sum = sum + limit;
        limit--;
    }
    return sum;
}
```

## Mixing Static and Dynamic Loops

Static and dynamic loops can be freely combined:

```cpp
val<int32_t> mixedLoops(val<int32_t> counter) {
    val<int32_t> sum = 0;
    for (val<int32_t> i = 0; i < counter; i++) {        // dynamic outer loop
        for (static_val<int> j = 0; j < 3; j++) {       // static inner loop (unrolled)
            if (i > 5) {
                sum = sum + 1;
            }
            sum = sum + 10;
        }
    }
    return sum;
}
```

The inner loop is unrolled into 3 sequential operations, while the outer loop remains dynamic.

## static_iterable

Use `static_iterable` to unroll range-for loops over containers:

```cpp
std::vector<int> weights = {1, 2, 3, 4, 5};

val<int32_t> computeSum(val<int32_t> ref) {
    val<int32_t> sum = 0;
    for (auto x : static_iterable(weights)) {
        if (ref > x) {
            sum = sum + 1;
        }
    }
    return sum;
}
```

The `static_iterable` wrapper causes the loop to be fully unrolled at trace time. Each element's value is baked into the generated code as a constant. It works with:

- `std::vector`
- `std::array`
- C-style arrays

```cpp
// C-style array
int rawArray[] = {2, 4, 6, 8, 10};
for (auto x : static_iterable(rawArray)) { ... }

// std::array
std::array<int, 5> stdArray = {3, 6, 9, 12, 15};
for (auto x : static_iterable(stdArray)) { ... }
```

## static_enumerable

`static_enumerable` works like `static_iterable` but also exposes the iteration index via structured bindings:

```cpp
std::vector<int> data = {10, 20, 30};

val<int32_t> weightedSum() {
    val<int32_t> sum = 0;
    for (auto [idx, x] : static_enumerable(data)) {
        sum = sum + x * (int32_t)(int64_t)idx;
    }
    return sum;
}
// Computes: 10*0 + 20*1 + 30*2 = 80
```

The `idx` is a `static_val<int64_t>&` — it exists only at trace time and causes full unrolling.

## When to Use static_val

Guidelines for choosing between `static_val` and `val`:

- Use `static_val` when the number of iterations is known at trace time (compile time).
- Use `val<T>` when the loop count depends on runtime input.
- `static_val` loops produce larger code but avoid loop overhead.
- Ideal for iterating over fixed-size data structures, SIMD-like unrolling, or small fixed iteration counts.
- Nested static loops multiply the unrolling (e.g., 3x4 = 12 unrolled iterations).
