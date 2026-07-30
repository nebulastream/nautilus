# 128-bit integer plugin

The `nautilus-int128` plugin supplies `nautilus::int128`, a traceable signed
128-bit integer for platforms whose compiler supports `__int128`. Include
`<nautilus/int128.hpp>` and link `nautilus::nautilus-int128`.

Values can be built from signed 64-bit integers or explicit low and high
halves. The type supports arithmetic, remainder, bitwise operations, shifts,
signed comparisons, and extraction of both 64-bit halves:

```cpp
nautilus::int128 product = nautilus::int128(input) * nautilus::int128(factor);
nautilus::val<uint64_t> low = product.low();
nautilus::val<int64_t> high = product.high();
```

All signed and unsigned integral `val<T>` types up to 64 bits can be explicitly
converted to and from `int128`; widening sign-extends signed inputs and
zero-extends unsigned inputs, while narrowing retains the low bits. An explicit
conversion to `val<bool>` tests the complete 128-bit value for zero.

`int128::Load(val<const void*>)` and `Store(val<void*>)` transfer exactly 16
bytes. They accept unaligned addresses: runtime backends use `memcpy`, and MLIR
emits alignment-one `i128` loads/stores. This also avoids exposing the plugin's
opaque-pointer representation as an application memory layout requirement.

The representation deliberately travels through Nautilus IR as an opaque
pointer. BC, TBC, AsmJit, and interpreter execution therefore use portable
runtime helper calls. When MLIR intrinsics are enabled, the plugin intercepts
the same calls, loads the operands as LLVM `i128`, performs native `i128`
operations, and keeps intermediate results in JIT stack storage. Turning off
`mlir.enableIntrinsics` exercises the runtime fallback like every other
backend.

`ENABLE_INT128_PLUGIN` controls the plugin and defaults to `ON`.

With `ENABLE_FUZZING=ON`, the `nautilus-int128-fuzz` target differentially
checks randomized arithmetic, division, remainder, bitwise, and shift programs
against an independent native `unsigned __int128` oracle.
