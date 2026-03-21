# The val\<T\> Value Type System

## Overview

`val<T>` is the core abstraction of Nautilus. It is a template wrapper that makes C++ values traceable. When tracing is enabled, all operations on `val<T>` values are recorded, allowing the JIT compiler to analyze and optimize them. When tracing is disabled, `val<T>` acts as a transparent wrapper with minimal overhead, so the same code runs efficiently in both modes.

```cpp
#include "nautilus/val.hpp"

// This function works with both tracing enabled and disabled.
// When traced, the JIT compiler can optimize the generated code.
val<int32_t> compute(val<int32_t> x) {
    return x * 2 + 1;
}
```

## Supported Types

| Category | Types |
|----------|-------|
| **Signed integers** | `int8_t`, `int16_t`, `int32_t`, `int64_t` |
| **Unsigned integers** | `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` |
| **Character** | `char` |
| **Floating-point** | `float`, `double` |
| **Boolean** | `bool` (specialized with probability tracking, see [Boolean Values](#boolean-values)) |
| **Pointers** | `T*` (see [pointers.md](pointers.md)) |
| **Enums** | any `enum` or `enum class` type (see [Enum Values](#enum-values)) |

## Creating Values

```cpp
val<int32_t> x = 5;           // from literal
val<int32_t> y;                // default initialized to 0
val<int32_t> z = x;            // copy
val<float> f = 3.14f;          // float literal
val<double> d = 2.718;         // double literal
```

## Arithmetic Operations

All standard arithmetic operators are supported for integer and floating-point types.

```cpp
val<int32_t> a = 10, b = 3;
auto sum  = a + b;     // 13
auto diff = a - b;     // 7
auto prod = a * b;     // 30
auto quot = a / b;     // 3
auto rem  = a % b;     // 1  (integral types only)
```

Unary operators are also available:

```cpp
val<int32_t> x = 5;
auto pos = +x;         // unary plus (integral promotion for small types)
auto neg = -x;         // unary negation: -5
```

## Comparison Operations

Comparisons return `val<bool>` and work across all arithmetic types.

```cpp
val<int32_t> x = 5, y = 10;
val<bool> eq = (x == y);   // false
val<bool> ne = (x != y);   // true
val<bool> lt = (x < y);    // true
val<bool> gt = (x > y);    // false
val<bool> le = (x <= y);   // true
val<bool> ge = (x >= y);   // false
```

## Bitwise Operations

Bitwise operators are available for integral types only.

```cpp
val<int32_t> a = 0xFF, b = 0x0F;
auto and_result = a & b;    // 0x0F
auto or_result  = a | b;    // 0xFF
auto xor_result = a ^ b;    // 0xF0
auto not_result = ~a;       // bitwise NOT
auto lshift     = a << 4;   // left shift
auto rshift     = a >> 4;   // right shift
```

## Increment and Decrement

Both prefix and postfix forms are supported.

```cpp
val<int32_t> x = 5;
x++;      // post-increment: x becomes 6
++x;      // pre-increment: x becomes 7
x--;      // post-decrement: x becomes 6
--x;      // pre-decrement: x becomes 5
```

## Compound Assignment

Compound assignment operators update the value in place.

```cpp
val<int32_t> x = 10;
x += 5;   // x = 15
x -= 3;   // x = 12
x *= 2;   // x = 24
x /= 4;   // x = 6
x %= 5;   // x = 1  (integral types only)
```

Bitwise compound assignment is also available for integral types:

```cpp
val<int32_t> x = 0xFF;
x &= 0x0F;    // x = 0x0F
x |= 0xF0;    // x = 0xFF
x ^= 0x0F;    // x = 0xF0
x <<= 4;      // left shift assign
x >>= 4;      // right shift assign
```

## Type Casting

Nautilus supports implicit widening conversions and explicit casts between compatible arithmetic types.

```cpp
// Implicit widening (safe, automatic)
val<int8_t> small = 42;
val<int64_t> big = small;        // implicit widening

// Explicit casting
val<float> f = 3.14f;
val<int32_t> i = static_cast<val<int32_t>>(f);   // float to int

val<int32_t> x = 100;
val<double> d = static_cast<val<double>>(x);      // int to double
```

When mixing types in arithmetic expressions, standard C++ promotion rules apply:

```cpp
val<int8_t> a = 10;
val<int32_t> b = 20;
auto result = a + b;   // result type follows C++ integer promotion rules
```

## Boolean Values

`val<bool>` is a specialized template with additional features for branch prediction optimization.

```cpp
val<bool> a = true;
val<bool> b = false;
auto and_r = a && b;    // logical AND
auto or_r  = a || b;    // logical OR
auto not_r = !a;        // logical NOT
```

Equality and inequality comparisons are supported:

```cpp
val<bool> x = true, y = false;
val<bool> eq = (x == y);   // false
val<bool> ne = (x != y);   // true
```

### Probability Hints

`val<bool>` supports probability hints that the JIT compiler uses for branch prediction optimization. Setting accurate probabilities can improve generated code quality by placing the likely branch on the fast path.

```cpp
val<bool> condition = (x > threshold);
condition.setIsTrueProbability(0.95);  // hint: 95% likely true
```

| Probability | Scenario |
|-------------|----------|
| 0.0 - 0.2 | Very unlikely (error or exceptional cases) |
| 0.2 - 0.4 | Unlikely (uncommon cases) |
| 0.4 - 0.6 | Unknown / no hint (default is 0.5) |
| 0.6 - 0.8 | Likely (common cases) |
| 0.8 - 1.0 | Very likely (normal or expected cases) |

## Enum Values

Any `enum` or `enum class` type can be wrapped in `val<T>`.

```cpp
enum class Color { RED, GREEN, BLUE };

val<Color> c = Color::BLUE;
if (c == Color::BLUE) {
    // matched
}
```

Enums support `==` and `!=` comparisons. They can also be converted to their underlying integer type:

```cpp
val<Color> c = Color::GREEN;
val<int> underlying = static_cast<val<int>>(c);  // convert to underlying type
```

## Auto Return Type

Functions returning `val<T>` values can use `auto` as the return type. The result type follows standard C++ promotion rules.

```cpp
auto addValues(val<int8_t> x, val<int32_t> y) {
    return x + y;  // result type follows C++ promotion rules
}
```

## Mixing val\<T\> and Raw Values

Operators accept both `val<T>` and raw C++ values on either side. The raw value is automatically wrapped before the operation is traced.

```cpp
val<int32_t> x = 10;
auto a = x + 5;       // val<int32_t> + int
auto b = 5 + x;       // int + val<int32_t>
val<bool> c = (x == 42);  // val<int32_t> == int
```
