# Builtins

## Overview

All builtins live in the `nautilus::builtin` namespace, except the `val<__int128>` specialization, which lives in `nautilus` like every other `val<T>`.

The builtin plugin (`nautilus-builtin`) exposes compiler builtins that have no natural `val<T>` operator, plus a traceable 128-bit integer type. Each builtin is a plain function that works on every backend: the MLIR and AsmJit backends lower it to a native instruction, all other backends (C++, bytecode, interpreter) call a portable fallback.

Enable it with `-DENABLE_BUILTIN_PLUGIN=ON` (default), link `nautilus-builtin`, and include `<nautilus/builtin/plugin.hpp>` in one translation unit of your executable so the native lowerings are registered when `nautilus-builtin` is linked as a static library.

## Prefetch

```cpp
#include <nautilus/builtin/plugin.hpp>
#include <nautilus/builtin/prefetch/prefetch.hpp>

using namespace nautilus::builtin;

val<int64_t> sum(val<int64_t*> data, val<int64_t> count) {
    val<int64_t> sum = 0;
    for (val<int64_t> i = 0; i < count; i = i + 1) {
        prefetch(data + (i + 16), PrefetchRW::Read, PrefetchLocality::High);
        sum = sum + data[i];
    }
    return sum;
}
```

`prefetch(address, rw, locality)` asks the CPU to pull the cache line containing `address` closer to the core. It is only a hint: it never changes program results and never faults, so it is safe to prefetch past the end of an array or at an unmapped address.

| Argument | Values | Meaning |
|----------|--------|---------|
| `rw` | `Read` (default), `Write` | Whether the line is read or written next |
| `locality` | `None`, `Low`, `Moderate`, `High` (default) | How soon the line is reused; mirrors `__builtin_prefetch` levels 0-3 |

### Lowering

| Backend | Lowering |
|---------|----------|
| MLIR | `llvm.prefetch` |
| AsmJit x86-64 | `PREFETCHT0/T1/T2/NTA` for reads; `PREFETCHW` for writes (read hint of the same locality on CPUs without `PREFETCHW`) |
| AsmJit AArch64 | `PRFM PLDL1KEEP/PLDL2KEEP/PLDL3KEEP/PLDL1STRM` for reads, `PST*` for writes |
| C++, bytecode, interpreter | call to a `__builtin_prefetch` wrapper |

## 128-bit integers

```cpp
#include <nautilus/builtin/plugin.hpp>
#include <nautilus/builtin/int128/int128.hpp>

using I128 = val<__int128>;

val<uint64_t> multiplyHigh(val<uint64_t> x, val<uint64_t> y) {
    return (I128(x) * I128(y)).high();
}
```

`val<__int128>` is a first-class traceable signed 128-bit integer for compilers that support `__int128` (GCC and Clang; not MSVC). It supports arithmetic, bitwise operations, shifts, signed comparisons, casts and memory operations.

### Construction and casts

- `val<__int128>(__int128)` and `val<__int128>(unsigned __int128)` (exact bit pattern; values above `INT128_MAX` wrap)
- `val<__int128>(val<uint64_t> low, val<int64_t> high)`
- `val<__int128>(val<T>)` for any signed/unsigned integral `val<T>` up to 64 bits: signed inputs sign-extend, unsigned inputs zero-extend
- `static_cast<val<T>>(value)` narrows to an integral `val<T>` up to 64 bits, keeping the low bits
- `static_cast<val<bool>>(value)` tests the full 128-bit value for zero
- `low()` / `high()` return the halves as `val<uint64_t>` / `val<int64_t>`

`val<__int128>::Load(val<const void*>)` and `Store(val<void*>)` transfer exactly 16 bytes and accept unaligned addresses.

### Representation and lowering

Nautilus's type system and IR carry at most 64-bit values, so `val<__int128>` travels through the IR as an opaque pointer to a boxed value. Every operation is a traced `invoke` of a runtime helper in `nautilus::builtin::detail`.

| Backend | Lowering |
|---------|----------|
| MLIR | native LLVM `i128` operations; `make` of two constant halves folds to an `i128` constant; loads/stores use alignment 1 |
| AsmJit, C++, bytecode, interpreter | call to the portable runtime helper |

Because a raw `__int128` does not fit the 64-bit invocation ABI, traced functions should take and return 64-bit values (for example via `low()` / `high()`) at their boundary rather than a bare `__int128`.

With `ENABLE_FUZZING=ON`, the `nautilus-int128-fuzz` target differentially checks randomized arithmetic, division, remainder, bitwise and shift kernels against a native `unsigned __int128` oracle.

## Overflow-checked arithmetic

```cpp
#include <nautilus/builtin/plugin.hpp>
#include <nautilus/builtin/overflow/overflow.hpp>

val<int64_t> checkedProduct(val<int64_t> a, val<int64_t> b, val<bool*> overflowed) {
    auto result = nautilus::details::nautilus_alloca<int64_t>();
    *overflowed = nautilus::builtin::mul_overflow(a, b, result);
    return *result;
}
```

`add_overflow`, `sub_overflow` and `mul_overflow` (for `val<int32_t>` and `val<int64_t>`) behave like `__builtin_{add,sub,mul}_overflow`: they write the wrapped result through the pointer and return whether the exact result does not fit. `add_overflow_i128`, `sub_overflow_i128` and `mul_overflow_i128` do the same for signed 128-bit integers held in memory (16 bytes, any alignment; the result may alias an operand).

### Lowering

| Backend | Lowering |
|---------|----------|
| MLIR | `llvm.s{add,sub,mul}.with.overflow` on `i32` / `i64` / `i128` (no division or widening to detect a multiply overflow) |
| AsmJit, C++, bytecode, interpreter | call to the portable runtime helper |
