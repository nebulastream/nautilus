# Constant Data

`nautilus::constData` puts a lookup table, a string or a set of seeds into the compiled module:

```cpp
#include "nautilus/data.hpp"

static const uint8_t kMasks[8] = {1, 2, 4, 8, 16, 32, 64, 128};

val<uint8_t> mask(val<int32_t> bit) {
    val<const uint8_t*> table = constData(kMasks);
    return table[bit];
}
```

`constData` copies the bytes into a read-only data object of the module while the function is traced and returns a
`val<const T*>` to it. It takes a `std::span<const T>`, a C array, a `std::array`, a `std::vector` or, for text, a
`std::string_view` or a string literal (the result is a `val<const char*>` to the characters and a terminating NUL).

Compared with tracing a pointer to the host array (`val<const uint8_t*>(kMasks)`):

* the compiled module no longer reads the host memory, so changing or freeing the array afterwards has no effect;
* a load at a constant offset folds into a constant (`table[3]` is the number 8), which lets the conversion fold what is
  computed from it;
* the module has no traced address in it, so it stays persistable (the data has `Literal` provenance).

The elements must be trivially copyable and must not hold addresses: a data object has no relocations. Without tracing,
`constData` returns the pointer it was given. It needs the NIR tracer (`tracing.nirTracer=true`, the default).
