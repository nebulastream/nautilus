# 128-bit integer plugin (`val<__int128>`)

The `nautilus-int128` plugin supplies `nautilus::val<__int128>`, a first-class
traceable signed 128-bit integer for platforms whose compiler supports
`__int128` (GCC and Clang; not MSVC). Include `<nautilus/int128.hpp>` and link
`nautilus::nautilus-int128`.

`val<__int128>` behaves like any other arithmetic `val<T>`:

```cpp
val<__int128> product = I128(input) * I128(factor);
val<uint64_t> low = product.low();
val<int64_t> high = product.high();
```

Everything else the type does — arithmetic, bitwise, shifts, signed
comparisons, casts, and memory operations — is provided by the dedicated
`val<__int128>` specialisation.

## Construction

- `val<__int128>()` — zero
- `val<__int128>(__int128)`
- `val<__int128>(val<uint64_t> low, val<int64_t> high)`
- `val<__int128>(val<T>)` for any narrower signed/unsigned integral `val<T>`
  (sign-extends signed inputs, zero-extends unsigned)

All signed and unsigned integral `val<T>` types up to 64 bits can be explicitly
converted to and from `int128`; widening sign-extends signed inputs and
zero-extends unsigned inputs, while narrowing retains the low bits. An explicit
`static_cast<val<bool>>` tests the complete 128-bit value for zero.

## Representation and lowering

`val<__int128>` travels through Nautilus IR as an opaque pointer. Every
operation is recorded as a traced `invoke` call to a portable runtime helper in
`nautilus::detail`; when the MLIR backend is enabled, its intrinsic plugin
intercepts those calls and replaces them with native LLVM `i128` operations
(`int128_make/add/sub/mul/div/rem/and/or/xor/neg/not/shl/shr/eq/lt/low/high`,
plus `load`/`store`). Scalar backends (C++, BC, TBC, AsmJit, interpreter) fall
back to the portable runtime helpers.

`int128::Load(val<const void*>)` and `Store(val<void*>)` (as
`val<__int128>::Load` / `Store`) transfer exactly 16 bytes and accept unaligned
addresses: runtime backends use `memcpy`, and MLIR emits alignment-one `i128`
loads/stores.

Because 128-bit values do not fit Nautilus's 64-bit value/`Type` model, the value travels as an opaque pointer through the IR; they are not exposed as a constant for
folding. This is inherent to an intrinsic-boxed design.

`ENABLE_INT128_PLUGIN` controls the plugin and defaults to `ON`. With
`ENABLE_FUZZING=ON`, the `nautilus-int128-fuzz` target differentially checks
randomized arithmetic, division, remainder, bitwise, and shift kernels against
an independent native `unsigned __int128` oracle.

## Registering a 128-bit function

A traced function that operates on `val<__int128>` registers just like any
other Nautilus function. Because a raw `__int128` return/argument does not fit
the 64-bit invoking ABI, prefer signatures that extract 64-bit halves (`low()`
/ `high()`, or `static_cast<val<uint64_t>>`) at the boundary rather than
returning a bare `__int128`.