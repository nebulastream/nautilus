# The `nautilus-std` Plugin

The `nautilus-std` plugin provides traceable wrappers around a curated subset
of the C and C++ standard libraries so that user code written against `val<T>`
can call familiar standard-library functionality and have it lowered correctly
by every Nautilus backend (MLIR, C++, Bytecode, AsmJit).

It lives in `plugins/std/` and is built as a separate library
(`nautilus-std`) that depends on the core `nautilus` target. Linking against
`nautilus-std` is opt-in: a project that does not need standard-library
support can stay on the core library alone.

```cmake
target_link_libraries(my_query_engine PRIVATE nautilus nautilus-std)
```

All headers live under `nautilus/std/` and are included with their familiar
C/C++ names, e.g.

```cpp
#include <nautilus/std/cmath.h>
#include <nautilus/std/string.h>
#include <nautilus/std/vector.h>
```

Every symbol the plugin exposes lives in the `nautilus` namespace and operates
on `val<T>`-wrapped values, so it can be used directly inside a traced
function passed to `Engine::registerFunction(...)`.

---

## Architecture

The plugin has two layers:

1. **Free functions / wrapper classes** that look like the standard library
   but accept and return `val<T>`. The implementations forward to named
   `extern "C"` thunks via `invoke(...)`, so the resulting trace contains a
   stable function-pointer call.
2. **MLIR intrinsic plugins** (`MLIRMathIntrinsics`, `MLIRBitIntrinsics`,
   `MLIRAtomicIntrinsics`) that recognise those stable thunks and lower them
   directly to native LLVM intrinsics or atomic instructions instead of going
   through a regular function call. Registration happens automatically from a
   static initializer in `src/StdPluginInit.cpp` whenever the MLIR backend is
   enabled.

This split is what allows the same source-level call (e.g. `nautilus::sqrt(x)`)
to lower to `llvm.sqrt.f64` in the MLIR backend while still working in the
C++/Bytecode/AsmJit backends as a normal function call.

The MLIR intrinsic sources are only compiled when `ENABLE_MLIR_BACKEND` is on;
on other configurations the plugin still works, just without the direct
intrinsic lowering.

---

## Module Overview

| Header | Purpose |
|--------|---------|
| `nautilus/std/cmath.h`       | `<cmath>` math functions (trig, exp/log, pow, fma, rounding, classification, …) |
| `nautilus/std/bit.h`         | `<bit>` bit-manipulation (`countl_zero`, `popcount`, `rotl`, `byteswap`, …) |
| `nautilus/std/atomic.h`      | `<atomic>` loads, stores, RMW, fences, `std::atomic<T>` wrapper |
| `nautilus/std/cstdlib.h`     | Allocation (`malloc`/`free`/`realloc`/…) and string-to-number parsing |
| `nautilus/std/cstring.h`     | C string and memory functions (`strcpy`, `memcmp`, `strlen`, …) |
| `nautilus/std/string.h`      | `val<std::basic_string>` wrapper class |
| `nautilus/std/string_view.h` | `val<std::basic_string_view>` wrapper class |
| `nautilus/std/vector.h`      | `val<std::vector<T, Allocator>>` wrapper class |
| `nautilus/std/ostream.h`     | `val<std::basic_ostream>` wrapper, manipulators (`endl`, `hex`, `flush`, …) |
| `nautilus/std/sstream.h`     | `val<std::basic_stringstream>` (inherits the ostream wrapper) |
| `nautilus/std/iostream.h`    | Process-global `cout`/`cerr` helpers |

---

## `cmath.h` — Math functions

Provides traced `val`-aware overloads for the bulk of `<cmath>` for both
`float` and `double` (and integer overloads for `abs`):

- Absolute value / sign / dim / lerp: `abs`, `fabs`, `fdim`, `lerp`
- Modulo / remainder: `fmod`, `remainder`, `remquo`
- Fused multiply / min-max: `fma`, `fmax`, `fmin`, `copysign`, `nextafter`
- Exponential / log: `exp`, `exp2`, `expm1`, `log`, `log2`, `log10`, `log1p`
- Power / roots: `pow`, `sqrt`, `cbrt`, `hypot`
- Trigonometric: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`
- Hyperbolic: `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`
- Error / gamma: `erf`, `erfc`, `tgamma`, `lgamma`
- Rounding / integer conversions: `ceil`, `floor`, `trunc`, `round`,
  `nearbyint`, `rint`, `lround`, `llround`, `lrint`, `llrint`
- Floating-point manipulation: `frexp`, `ldexp`, `modf`, `scalbn`, `ilogb`,
  `logb`
- Classification: `fpclassify`, `isfinite`, `isinf`, `isnan`, `isnormal`,
  `signbit`, plus comparison helpers (`isgreater`, `isless`, …)

When the MLIR backend is active, calls such as `sqrt`, `sin`, `cos`, `pow`,
`fma`, `floor`, `ceil`, `fabs`, `fmin`, `fmax`, `exp`, `log`, `copysign`, … are
recognised by `MLIRMathIntrinsics` and lowered directly to the corresponding
LLVM math intrinsics. Other backends invoke the libm thunks normally. The
`llvm-ir-test` reference files under `plugins/std/test/intrinsic-ir/` show the
expected IR with and without intrinsic lowering for each function.

```cpp
#include <nautilus/std/cmath.h>

val<double> hypot2d(val<double> x, val<double> y) {
    return nautilus::sqrt(x * x + y * y);
}
```

---

## `bit.h` — Bit manipulation (`<bit>`)

Provides the C++20 `<bit>` operations for all unsigned integer widths
(`uint8_t`/`uint16_t`/`uint32_t`/`uint64_t`):

- Counting: `countl_zero`, `countl_one`, `countr_zero`, `countr_one`,
  `popcount`
- Predicates / sizing: `has_single_bit`, `bit_width`, `bit_ceil`, `bit_floor`
- Rotation: `rotl`, `rotr`
- Endianness: `byteswap` (`uint16/32/64`)

The MLIR intrinsic plugin (`MLIRBitIntrinsics`) lowers `countl_zero`,
`countr_zero`, `popcount`, `rotl`, `rotr` and `byteswap` to the corresponding
`llvm.ctlz.*`, `llvm.cttz.*`, `llvm.ctpop.*`, `llvm.fshl.*`, `llvm.fshr.*`,
`llvm.bswap.*` intrinsics. The remaining functions are implemented in terms of
those primitives.

---

## `atomic.h` — Atomic operations (`<atomic>`)

The atomic header offers three flavors of API:

### 1. Free functions on `std::atomic<T>*`

For each of the integer widths `int32_t`, `int64_t`, `uint32_t`, `uint64_t`
the plugin exposes named overloads with an explicit memory order suffix:

```cpp
val<int64_t> atomic_load_seqcst (val<std::atomic<int64_t>*>);
val<int64_t> atomic_load_acquire(val<std::atomic<int64_t>*>);
val<int64_t> atomic_load_relaxed(val<std::atomic<int64_t>*>);

void atomic_store_release(val<std::atomic<int64_t>*>, val<int64_t>);
// ... store_seqcst / store_relaxed
```

The same convention is used for the read–modify–write operations
`atomic_fetch_add`, `atomic_fetch_sub`, `atomic_fetch_and`, `atomic_fetch_or`,
`atomic_fetch_xor`, `atomic_exchange` and for
`atomic_compare_exchange_strong` (CAS is currently runtime-only).

The "named-by-ordering" design exists so that each ordering becomes its own
stable function-pointer call site, which `MLIRAtomicIntrinsics` matches and
lowers to the appropriate `llvm.atomicrmw` / `llvm.load atomic` /
`llvm.store atomic` instruction with the right ordering.

### 2. `atomic_thread_fence_*`

Standalone fences `atomic_thread_fence_seqcst`, `…_acquire`, `…_release`,
`…_acq_rel`, `…_relaxed`. The MLIR backend lowers these to `llvm.fence`.

### 3. Pointer-based operations using `std::atomic_ref`

To avoid forcing callers through a `std::atomic<T>` wrapper, ordering-templated
helpers are provided that operate directly on a `val<T*>`:

```cpp
template <std::memory_order Order = std::memory_order_seq_cst, class T>
val<T> atomic_load(val<T*> ptr);

template <std::memory_order Order = std::memory_order_seq_cst, class T>
void atomic_store(val<T*> ptr, val<T> desired);

template <class T>
val<bool> atomic_compare_exchange_strong(val<T*>, val<T*>, val<T>);
```

These pick the appropriate underlying thunk based on `Order` so call sites
read naturally:

```cpp
auto v = nautilus::atomic_load<std::memory_order_acquire>(ptr);
nautilus::atomic_store<std::memory_order_release>(ptr, v + 1);
```

### 4. `val<std::atomic<T>>` wrapper class

A specialization `val<std::atomic<T>>` is provided that owns a
`val<std::atomic<T>*>` and delegates each member function (`load`, `store`,
`fetch_add`, …) to the named thunks above so the MLIR matcher still sees a
stable callee. Use it like a regular `std::atomic` inside a traced function.

---

## `cstdlib.h` — C runtime allocation and parsing

Wraps the most common entries from `<cstdlib>`:

- Allocation: `malloc`, `aligned_alloc`, `calloc`, `realloc`, `free`
- Parsing: `atof`, `atoi`, `atol`, `atoll`, `strtol`, `strtoll`, `strtoul`,
  `strtoull`, `strtof`, `strtod`

These are pure runtime calls (no MLIR intrinsic lowering) and are exposed so
that traced code can manage memory or convert text to numbers without escaping
the `val<T>` world.

---

## `cstring.h` — C string / memory functions

Provides `val`-aware versions of the usual `<cstring>` entry points:

- Copy / concatenate: `strcpy`, `strncpy`, `strcat`, `strncat`
- Compare: `memcmp`, `strcmp`, `strncmp`, `strcoll`, `strxfrm`
- Search: `memchr`, `strchr`, `strrchr`, `strstr`, `strpbrk`, `strspn`,
  `strcspn`, `strtok`
- Misc: `strerror`, `strlen`

Both `const`- and non-`const`-overloaded variants are available where the
standard library distinguishes them.

---

## `string.h` — `val<std::basic_string>`

A class specialization `val<std::basic_string<CharT, Traits>>` that wraps a
`val<std::basic_string<CharT, Traits>*>` and exposes the familiar string API.
Notable members:

- Construction from a string pointer or from a `const CharT*` literal
- Element access: `at`, `operator[]`, `front`, `back`, `data`, `c_str`
- Conversion to `val<std::basic_string_view<CharT, Traits>>`
- Capacity / size: `empty`, `size`, `length`, `max_size`, `capacity`,
  `reserve`, `clear`
- Modifiers and queries delegated to thunks via `invoke(...)`
- `copy(dest, count, pos)` to extract characters
- `release()` to recover the underlying `val<base_type*>`

Because each method forwards through `invoke`, all string operations are
recorded in the trace and execute correctly under every backend.

---

## `string_view.h` — `val<std::basic_string_view>`

Mirror class specialization for `val<std::basic_string_view<CharT, Traits>>`.
The `val<std::basic_string>` wrapper provides an implicit conversion to it so
strings interoperate with views the same way the underlying STL types do.

---

## `vector.h` — `val<std::vector<T, Allocator>>`

A traced wrapper for `std::vector`. Internally Nautilus does **not** assume a
fixed memory layout for `std::vector`: at static-init time it probes the byte
offsets of the standard `_M_start` / `_M_finish` / `_M_end_of_storage` pointer
fields via `probe_vector_layout<T, Allocator>()` and stores them in
`vector_layout<T, Allocator>`. This lets the wrapper read the live data
pointer through `load_data_ptr()` / `load_finish_ptr()` / `load_end_ptr()`
portably across libstdc++, libc++, and the MSVC STL.

Exposed surface:

- Construction from a `val<std::vector*>`, copy assignment via `invoke`
- Capacity: `empty`, `size`, `capacity`, `max_size`, `reserve`,
  `shrink_to_fit`, `clear`
- Element access: `data()` (returns `val<T*>`)
- Mutation:
  - `push_back(val<T>)` for non-class element types
  - `push_back(val<T*>)` for class element types
    (passes by pointer to avoid copying through the trace)
  - `pop_back`, `resize(count)`, `resize(count, value)`, `swap`
- Comparison: `equals(other)`
- `release()` to access the raw pointer

Iteration is performed by walking `data()` between `load_data_ptr()` and
`load_finish_ptr()` like a standard pointer-based loop.

---

## `ostream.h` — `val<std::basic_ostream>`

Provides a `val<std::basic_ostream<CharT, Traits>>` wrapper plus manipulator
functions `endl`, `ends`, `flush`, `hex`, `dec`, `oct`. Each manipulator is
implemented as a free function that calls a `callXxx` thunk via `invoke` so
that the manipulator becomes part of the trace.

The wrapper exposes:

- `operator<<(const val<T>&)` for arbitrary traced values
- `operator<<(const char*)` for string literals
- `operator<<(manipulator)` for the manipulators above
- `put(ch)`, `write(s, count)`, `tellp()`

This lets traced code stream to any `std::ostream`-compatible target.

---

## `sstream.h` — `val<std::basic_stringstream>`

A specialization that derives from the ostream wrapper, so a
`std::stringstream` constructed by the host code can be passed in as a
`val<std::stringstream*>` and used with the same `operator<<` API. Useful for
building strings during a traced computation.

---

## `iostream.h` — `cout` / `cerr` helpers

Provides `cout_helper` and `cerr_helper` classes that wrap the process-global
`std::cout` and `std::cerr` streams as `val<std::ostream>`s. Use them to print
from inside a traced function for debugging or simple output.

---

## Build flags and dependencies

- The plugin always builds on top of the core `nautilus` library.
- When `ENABLE_MLIR_BACKEND=ON`, the plugin additionally compiles
  `MLIRMathIntrinsics.cpp`, `MLIRBitIntrinsics.cpp`, `MLIRAtomicIntrinsics.cpp`
  and `StdPluginInit.cpp`, links against `MLIRIR`, `MLIRLLVMDialect`, and
  `MLIRMathDialect`, and registers the intrinsic plugins via a static
  initializer in `StdPluginInit.cpp`.
- When `ENABLE_TESTS=ON`, the plugin's own test suite under `plugins/std/test/`
  is added. It contains:
  - `STDProxyTest.cpp` — exercises the C-stdlib wrappers
  - `StringTest.cpp`, `VectorTest.cpp`, `OstreamTest.cpp`, `AtomicTest.cpp`
  - `BitIntrinsicExecutionTest.cpp` and `MemoryIntrinsicExecutionTest.cpp`
  - `LLVMIRBitIntrinsicTest.cpp` and `LLVMIRCMathIntrinsicTest.cpp`, which
    diff generated LLVM IR against the reference files in
    `plugins/std/test/intrinsic-ir/` to verify intrinsic lowering.

---

## Adding new functionality

To extend the plugin with a new standard-library entry point:

1. Add a free-function declaration in the appropriate
   `plugins/std/include/nautilus/std/<header>.h`, taking and returning
   `val<T>` as needed.
2. Implement it in the matching `plugins/std/src/<header>.cpp` by forwarding
   to a named `extern "C"` thunk through `invoke(...)`. The thunk's address
   must be stable across translation units.
3. (Optional) If the operation should map to an LLVM intrinsic, add a matcher
   entry to the appropriate `MLIR*Intrinsics.cpp` file so the MLIR backend
   recognises the thunk and lowers it directly.
4. Add execution tests under `plugins/std/test/`, and (for intrinsic-lowered
   operations) reference IR under `plugins/std/test/intrinsic-ir/`.
5. Run `cmake --build . --target format` and the test suite before
   committing.
