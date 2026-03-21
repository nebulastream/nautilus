# Runtime Function Calls

## Overview

Nautilus functions operate on `val<T>` values and are traced for compilation. Sometimes you need to call external C++ functions from within traced code. Nautilus provides `invoke()` and related utilities for this purpose.

## The invoke() Function

Use `invoke()` to call native C++ functions from traced code:

```cpp
int32_t add(int32_t x, int32_t y) {
    return x + y;
}

val<int32_t> myFunction(val<int32_t> x, val<int32_t> y) {
    return invoke(add, x, y);
}
```

The `invoke()` function:

- Takes a function pointer and `val<T>` arguments
- Records the call in the execution trace
- Returns `val<R>` wrapping the result
- At runtime, calls the native function directly

## Void Functions

Functions that return `void` can also be called with `invoke()`:

```cpp
void voidFunc(int32_t x, int32_t y) {
    // side effects...
}

val<int32_t> callVoid(val<int32_t> x, val<int32_t> y) {
    invoke(voidFunc, x, y);
    return x;
}
```

## Lambda Functions

Use `+[]` to convert stateless lambdas to function pointers, which can then be passed to `invoke()`:

```cpp
val<int32_t> lambdaCall(val<int32_t> x) {
    return invoke(+[](int32_t x) { return x * 2; }, x);
}
```

Calls can be nested:

```cpp
val<int32_t> nestedLambda(val<int32_t> x) {
    return invoke(+[](int32_t x) { return x * 2; },
                  invoke(+[](int32_t x) { return x + 2; }, x));
}
```

## The function() Wrapper

Use `function()` to create a reusable callable object for repeated invocations of the same function:

```cpp
int32_t add(int32_t x, int32_t y) { return x + y; }

val<int32_t> callSameFunction(val<int32_t> x) {
    auto func = function(add);
    x = func(x, x);
    x = func(x, x);
    return x;
}
```

## Member Function Calls

Call member functions on traced pointers using `memberFunc<>()`:

```cpp
class MyClass {
public:
    int32_t get() { return value; }
    int32_t value = 42;
};

val<int32_t> callMember(val<MyClass*> obj) {
    auto& func = memberFunc<&MyClass::get>();
    return func(obj);
}
```

## NAUTILUS_INLINE Macro

Mark functions for inlining into JIT-compiled code:

```cpp
#include <nautilus/inline.hpp>

NAUTILUS_INLINE int32_t add(int32_t x, int32_t y) {
    return x + y;
}
```

When a function marked with `NAUTILUS_INLINE` is called via `invoke()`, the MLIR backend can inline the function body directly into the generated code instead of emitting a call. Enable this behavior with:

```cpp
options.setOption("mlir.inline_invoke_calls", true);
```

Note: `NAUTILUS_INLINE` only works with Clang. On GCC, it is a no-op.

## Function Attributes

Control optimization hints by passing `FunctionAttributes` to `invoke()`:

```cpp
#include <nautilus/common/FunctionAttributes.hpp>

// ModRefInfo::ModRef - function may read and modify memory (default)
// ModRefInfo::Ref    - function only reads, does not modify (pure read)
constexpr FunctionAttributes funcAttr{ModRefInfo::ModRef};
invoke(funcAttr, myFunction, arg1, arg2);
```

This helps the compiler optimize around function calls. A `Ref`-only function's result may be cached, while `ModRef` prevents such optimization.

## Functions in Loops

`invoke()` works naturally inside loops:

```cpp
val<int32_t> loopWithCall(val<int32_t> count, val<int32_t> x) {
    val<int32_t> sum = 0;
    for (val<int32_t> i = 0; i < count; i = i + 1) {
        sum = invoke(add, sum, x);
    }
    return sum;
}
```

## NautilusFunction

While `invoke()` calls external C++ functions that operate on raw types, `NautilusFunction` wraps functions that operate on `val<T>` types. This means the wrapped function's body is itself traced and compiled — enabling cross-function optimization and separate compilation of reusable components.

### Header

```cpp
#include <nautilus/nautilus_function.hpp>
```

### Basic Usage

Declare a function that uses `val<T>` parameters, then wrap it with `NautilusFunction`:

```cpp
val<int> add(val<int> a, val<int> b) {
    return a + b;
}

static auto nautilusAdd = NautilusFunction{"add", add};
```

Call it like a regular function inside traced code:

```cpp
val<int> myFunction(val<int> a, val<int> b) {
    return nautilusAdd(a, b);
}
```

### How It Differs from invoke()

| Feature | `invoke()` | `NautilusFunction` |
|---------|-----------|-------------------|
| Wrapped function signature | Raw types (`int`, `float`) | `val<T>` types |
| Function body | Opaque to the tracer | Traced and compiled |
| Cross-function optimization | Limited (call boundary) | Possible (body is visible) |
| Use case | Calling external/library code | Composing traced functions |

### Void Functions

`NautilusFunction` supports functions with no return value:

```cpp
void increment(val<int*> x) {
    *x = *x + val<int>(10);
}

static auto nautilusIncrement = NautilusFunction{"increment", increment};

val<int> use(val<int*> ptr) {
    nautilusIncrement(ptr);
    return *ptr;
}
```

### Lambda Functions

Wrap lambdas directly — CTAD deduces the template parameter:

```cpp
auto nautilusMul = NautilusFunction{
    "mul", [](val<int> x, val<int> y) -> val<int> { return x * y; }
};
```

### Member Functions

Bind a member function to an instance using `bind_instance<>()`:

```cpp
struct Calculator {
    val<int> sub(val<int> a, val<int> b) {
        return a - b;
    }
};

static Calculator calc;
auto nautilusSub = NautilusFunction{
    "sub", bind_instance<&Calculator::sub>(calc)
};
```

### Nesting NautilusFunction Calls

`NautilusFunction` objects can call each other:

```cpp
val<int> inner(val<int> x) {
    return x * val<int>(2);
}

val<int> outer(val<int> x) {
    return nautilusInner(x) + val<int>(5);
}

static auto nautilusInner = NautilusFunction{"inner", inner};
static auto nautilusOuter = NautilusFunction{"outer", outer};

val<int> nested(val<int> a) {
    return nautilusOuter(a);  // calls inner through outer
}
```

### Obtaining a Compiled Function Pointer with getFuncPtr()

`getFuncPtr()` returns a `val<RawFuncPtr>` — a traced function pointer to the compiled version of the wrapped function. This is useful for passing compiled functions as callbacks to external code via `invoke()`:

```cpp
int32_t applyFn(int32_t (*fn)(int32_t, int32_t), int32_t a, int32_t b) {
    return fn(a, b);
}

val<int32_t> useFuncPtr(val<int32_t> a, val<int32_t> b) {
    auto fnPtr = nautilusAdd.getFuncPtr();
    return invoke(applyFn, fnPtr, a, b);
}
```

During tracing, `getFuncPtr()` emits a `FUNC_ADDR` operation that registers the function for compilation. Outside tracing, it JIT-compiles the function on demand, caches the result, and returns the compiled pointer.

### Important Constraints

- **Non-copyable and non-movable.** `NautilusFunction` captures internal state at construction time. Copying would create inconsistent state. Declare instances as `static` or at a scope that outlives all callers.
- **Static declaration pattern.** The typical pattern is to declare `NautilusFunction` objects as `static` variables at file scope, ensuring a single instance with stable lifetime.
- **Inline usage.** `NautilusFunction` can also be constructed inline within a traced function. This is useful for one-off wrappers but the object must remain alive for the duration of the call.
