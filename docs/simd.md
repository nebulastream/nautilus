# SIMD Vectors

## Overview

Nautilus provides SIMD (Single Instruction, Multiple Data) support through the `val<vec<T>>` type. This is a specialization of `val<T>` that operates on fixed-width vectors of arithmetic values, allowing multiple elements to be processed in parallel with a single operation.

The SIMD plugin automatically selects the optimal vector width for the current hardware (SSE2, AVX, AVX-512, or ARM NEON). All vector operations integrate with the Nautilus tracing and compilation pipeline, so functions using SIMD vectors are traced, optimized, and compiled just like scalar code.

```cpp
#include <nautilus/vector.hpp>

void addArrays(val<const float*> a, val<const float*> b, val<float*> c) {
    auto va = val<vec<float>>::Load(a);
    auto vb = val<vec<float>>::Load(b);
    (va + vb).Store(c);
}
```

## Vector Types

The `vec<T>` tag represents a SIMD vector of element type `T`. Wrap it in `val<>` to get a traceable vector value.

```cpp
val<vec<float>>    // vector of floats
val<vec<double>>   // vector of doubles
val<vec<int32_t>>  // vector of 32-bit integers
val<vec<int64_t>>  // vector of 64-bit integers
```

The convenience alias `Vector<T>` is equivalent to `val<vec<T>>`:

```cpp
Vector<float> v = Vector<float>::Load(ptr);  // same as val<vec<float>>
```

### Lane Count

The number of lanes per vector is determined at compile time based on the widest SIMD instruction set available on the target CPU:

| Instruction Set | Register Width | `float` Lanes | `double` Lanes | `int32_t` Lanes | `int64_t` Lanes |
|-----------------|---------------|---------------|----------------|-----------------|-----------------|
| AVX-512 | 512-bit | 16 | 8 | 16 | 8 |
| AVX / AVX2 | 256-bit | 8 | 4 | 8 | 4 |
| SSE2 / NEON | 128-bit | 4 | 2 | 4 | 2 |

You can query the lane count at compile time:

```cpp
constexpr size_t lanes = vec<float>::lanes;  // e.g. 8 on AVX
```

## Loading and Storing

### Load

Load a vector from contiguous memory. The pointer must point to at least `vec<T>::lanes` elements.

```cpp
val<vec<float>> v = val<vec<float>>::Load(ptr);
```

### Store

Store a vector to contiguous memory.

```cpp
v.Store(out_ptr);
```

### Broadcast

Create a vector with all lanes set to the same scalar value.

```cpp
val<vec<float>> v = val<vec<float>>::Broadcast(val<float>(1.0f));
```

### Gather

Load elements from non-contiguous memory locations using an index array. Loads `base[indices[0]], base[indices[1]], ...` into the vector lanes.

```cpp
val<vec<float>> v = val<vec<float>>::Gather(base_ptr, index_ptr);
```

### Scatter

Store elements to non-contiguous memory locations. Stores lane `i` to `base[indices[i]]`.

```cpp
v.Scatter(base_ptr, index_ptr);
```

## Lane Access

### Extract

Extract a single scalar value from a specific lane.

```cpp
val<float> lane0 = v.Get(val<int32_t>(0));
```

### Insert

Return a new vector with one lane replaced by a scalar value.

```cpp
val<vec<float>> v2 = v.Set(val<int32_t>(0), val<float>(42.0f));
```

## Arithmetic Operations

All arithmetic operators work element-wise on vectors.

```cpp
auto va = val<vec<float>>::Load(a);
auto vb = val<vec<float>>::Load(b);

auto sum  = va + vb;   // element-wise addition
auto diff = va - vb;   // element-wise subtraction
auto prod = va * vb;   // element-wise multiplication
auto quot = va / vb;   // element-wise division
auto neg  = -va;       // element-wise negation
auto abs  = va.Abs();  // element-wise absolute value
```

### Compound Assignment

```cpp
auto v = val<vec<float>>::Load(ptr);
v += val<vec<float>>::Load(other);   // add in place
v -= val<vec<float>>::Load(other);   // subtract in place
v *= val<vec<float>>::Load(other);   // multiply in place
v /= val<vec<float>>::Load(other);   // divide in place
```

### Min, Max, and Fused Multiply-Add

These are available as free functions:

```cpp
auto vmin = Min(va, vb);         // element-wise minimum
auto vmax = Max(va, vb);         // element-wise maximum
auto vfma = Fma(va, vb, vc);    // element-wise a * b + c
```

`Fma` is only available for floating-point types.

## Reductions

Reduction operations collapse all lanes into a single scalar value.

```cpp
auto v = val<vec<float>>::Load(ptr);
val<float> sum = v.ReduceAdd();   // sum of all lanes
val<float> min = v.ReduceMin();   // minimum across all lanes
val<float> max = v.ReduceMax();   // maximum across all lanes
```

## Comparison Operations

Comparison operators return a mask vector where each lane is all-ones (true) or all-zeros (false).

```cpp
auto va = val<vec<float>>::Load(a);
auto vb = val<vec<float>>::Load(b);

auto eq = (va == vb);   // equal
auto ne = (va != vb);   // not equal
auto lt = (va < vb);    // less than
auto le = (va <= vb);   // less than or equal
auto gt = (va > vb);    // greater than
auto ge = (va >= vb);   // greater than or equal
```

### Blend

Use a mask vector to select elements from two vectors. Where the mask bits are set, elements from the first vector are taken; otherwise elements from the second.

```cpp
auto mask = va > val<vec<float>>::Broadcast(val<float>(0.0f));
auto result = Blend(mask, va, val<vec<float>>::Broadcast(val<float>(0.0f)));
result.Store(out);  // ReLU: keep positive values, zero out negative
```

## Bitwise Operations

Bitwise operators are available for all vector types.

```cpp
auto result_and = va & vb;   // bitwise AND
auto result_or  = va | vb;   // bitwise OR
auto result_xor = va ^ vb;   // bitwise XOR
```

### Shift Operations

Left and right shift operators are available for integer vector types only.

```cpp
val<vec<int32_t>> va = val<vec<int32_t>>::Load(a);
val<vec<int32_t>> vb = val<vec<int32_t>>::Load(b);

auto lshift = va << vb;   // element-wise left shift
auto rshift = va >> vb;   // element-wise right shift

va <<= vb;   // shift left in place
va >>= vb;   // shift right in place
```

## Complete Examples

### Dot Product

Multiply two arrays element-wise and sum the results.

```cpp
val<float> dotProduct(val<const float*> a, val<const float*> b) {
    auto va = val<vec<float>>::Load(a);
    auto vb = val<vec<float>>::Load(b);
    return (va * vb).ReduceAdd();
}
```

### SAXPY (y = alpha * x + y)

A common linear algebra operation using fused multiply-add.

```cpp
void saxpy(val<const float*> alpha, val<const float*> x, val<float*> y) {
    auto valpha = val<vec<float>>::Load(alpha);
    auto vx = val<vec<float>>::Load(x);
    auto vy = val<vec<float>>::Load(y);
    Fma(valpha, vx, vy).Store(y);
}
```

### Clamping Values to a Range

```cpp
void clamp(val<const float*> input, val<float> lo, val<float> hi, val<float*> output) {
    auto v = val<vec<float>>::Load(input);
    auto vlo = val<vec<float>>::Broadcast(lo);
    auto vhi = val<vec<float>>::Broadcast(hi);
    Max(Min(v, vhi), vlo).Store(output);
}
```

### Gather and Reduce

Load values from non-contiguous locations and compute their sum.

```cpp
val<float> gatherSum(val<const float*> base, val<const int32_t*> indices) {
    return val<vec<float>>::Gather(base, indices).ReduceAdd();
}
```

## How It Works

When the MLIR backend is enabled, vector operations are compiled to native SIMD instructions (SSE, AVX, AVX-512, or NEON) through the MLIR and LLVM compilation pipeline. The SIMD plugin registers an intrinsic handler that replaces traced vector operations with the corresponding LLVM vector dialect operations during code generation.

When the MLIR backend is not available, all vector operations fall back to portable scalar C++ implementations that produce correct results on any platform, without requiring hardware SIMD support.

## API Reference

### Static Methods on val\<vec\<T\>\>

| Method | Description |
|--------|-------------|
| `Load(val<const T*> ptr)` | Load a vector from contiguous memory |
| `Broadcast(val<T> scalar)` | Create a vector with all lanes set to the same value |
| `Gather(val<const T*> base, val<const int32_t*> idx)` | Load from non-contiguous memory |

### Member Methods

| Method | Returns | Description |
|--------|---------|-------------|
| `Store(val<T*> ptr)` | `void` | Store to contiguous memory |
| `Scatter(val<T*> base, val<const int32_t*> idx)` | `void` | Store to non-contiguous memory |
| `Get(val<int32_t> idx)` | `val<T>` | Extract a single lane |
| `Set(val<int32_t> idx, val<T> value)` | `val<vec<T>>` | Return vector with one lane replaced |
| `Abs()` | `val<vec<T>>` | Element-wise absolute value |
| `ReduceAdd()` | `val<T>` | Sum all lanes |
| `ReduceMin()` | `val<T>` | Minimum across all lanes |
| `ReduceMax()` | `val<T>` | Maximum across all lanes |

### Operators

| Operator | Types | Description |
|----------|-------|-------------|
| `+`, `-`, `*`, `/` | All | Element-wise arithmetic |
| `-` (unary) | All | Element-wise negation |
| `+=`, `-=`, `*=`, `/=` | All | Compound assignment |
| `==`, `!=`, `<`, `<=`, `>`, `>=` | All | Element-wise comparison (returns mask) |
| `&`, `\|`, `^` | All | Bitwise operations |
| `<<`, `>>` | Integer | Element-wise shift |
| `<<=`, `>>=` | Integer | Compound shift assignment |

### Free Functions

| Function | Description |
|----------|-------------|
| `Min(a, b)` | Element-wise minimum |
| `Max(a, b)` | Element-wise maximum |
| `Fma(a, b, c)` | Fused multiply-add: `a * b + c` (floating-point only) |
| `Blend(mask, a, b)` | Select elements: `a` where mask is set, `b` otherwise |
| `vector_load<T>(ptr)` | Shorthand for `Vector<T>::Load(ptr)` |
