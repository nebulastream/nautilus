# Getting Started with Nautilus

## What is Nautilus?

Nautilus is a lightweight, tracing Just-In-Time (JIT) compiler for C++20 projects, developed by the DIMA group at TU Berlin and published at SIGMOD 2024.

With Nautilus, you write imperative C++ functions using `val<T>` wrapper types instead of raw C++ types. When a function is registered with the Nautilus engine, all operations on `val<T>` values are traced and recorded. The resulting execution trace is converted to an intermediate representation (IR), optimized, and compiled to efficient machine code through one of several backends:

- **MLIR** -- Compiles through MLIR and LLVM to native machine code (highest performance).
- **C++** -- Generates compilable C++ source code (useful for debugging).
- **Bytecode** -- Interprets bytecode at runtime (fast compilation, portable).

The compiled function can then be called with ordinary C++ values, executing the optimized code transparently.

## Prerequisites

- A **C++20** compiler: GCC 10+ or Clang 15+
- **CMake** 3.16 or later

MLIR dependencies are downloaded automatically on the first build.

## Building from Source

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target nautilus
```

### Key CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_LOGGING` | ON | Enable spdlog-based logging |
| `ENABLE_TESTS` | ON | Build the test suite |
| `ENABLE_BENCHMARKS` | OFF | Build performance benchmarks |
| `ENABLE_MLIR_BACKEND` | ON | Enable the MLIR code generation backend |
| `ENABLE_C_BACKEND` | ON | Enable the C++ code generation backend |
| `ENABLE_BC_BACKEND` | ON | Enable the bytecode interpreter backend |

For example, to build with benchmarks enabled:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_BENCHMARKS=ON ..
cmake --build . --target nautilus
```

## Quick Start Example

The following example defines a function that computes a conditional sum over an array, compiles it with the Nautilus engine, and executes it.

```cpp
#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/core.hpp>

using namespace nautilus;

val<int32_t> conditionalSum(val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
    val<int32_t> sum = 0;
    for (val<int32_t> i = 0; i < size; i++) {
        if (mask[i]) {
            val<int32_t> value = array[i];
            sum += value;
        }
    }
    return sum;
}

int main(int, char*[]) {
    engine::Options options;
    options.setOption("engine.backend", "mlir");
    auto engine = engine::NautilusEngine(options);
    auto function = engine.registerFunction(conditionalSum);

    auto mask = new bool[4]{true, true, false, true};
    auto array = new int32_t[4]{1, 2, 3, 4};
    auto result = function(4, mask, array);

    std::cout << "Result: " << result << std::endl; // Prints 7
    return 0;
}
```

Key points to notice:

- **`val<T>` wrappers**: The function parameters and local variables use `val<int32_t>`, `val<bool*>`, and `val<int32_t*>` instead of their raw C++ counterparts.
- **Standard C++ syntax**: Loops, conditionals, pointer indexing, and arithmetic all work as expected. The `val<T>` types overload operators to transparently record each operation.
- **Registration and execution**: `engine.registerFunction` traces and compiles the function. The returned callable accepts raw C++ values (`int32_t`, `bool*`, `int32_t*`) and returns a raw `int32_t`.

## How the Pipeline Works

When you register a function with the Nautilus engine, the following steps occur:

1. **Write a function using `val<T>` types.** Replace raw C++ types in function signatures and local variables with their `val<T>` equivalents. The function body uses standard C++ control flow and operators.

2. **Register it with `NautilusEngine`.** Call `engine.registerFunction(yourFunction)` to begin the compilation process.

3. **Nautilus traces the function.** The engine executes the function with symbolic `val<T>` arguments. Every operation (arithmetic, comparison, memory access, control flow) is recorded into an execution trace.

4. **The trace is converted to IR and optimized.** The execution trace is transformed into a control flow graph in Static Single Assignment (SSA) form. Optimization phases run over this IR, including constant propagation, dead code elimination, loop analysis, and control flow simplification.

5. **A backend compiles the IR to executable code.** Depending on the configured backend, the optimized IR is lowered to native machine code (MLIR), C++ source (cpp), or bytecode (bc).

6. **Call the compiled function with raw C++ values.** The returned callable wraps the compiled code. You invoke it with ordinary C++ arguments, and it returns ordinary C++ results.

## Next Steps

- [Value Types (`val<T>`)](val-type.md) -- How the tracing value wrapper works for different types
- [Control Flow](control-flow.md) -- Using if/else and branching with `val<bool>`
- [Loops](loops.md) -- Writing traced for and while loops
- [Pointers](pointers.md) -- Working with `val<T*>` pointer types
- [Functions](functions.md) -- Calling and registering functions
- [Static Values (`static_val<T>`)](static-val.md) -- Compile-time constant values
- [Engine Configuration and Backends](engine.md) -- Backend selection, options, and debugging output
- [Tracing Internals](tracing.md) -- How the tracing system works under the hood
