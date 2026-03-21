# Engine Configuration and Backends

The `NautilusEngine` is the central entry point for compiling and executing Nautilus functions. It manages tracing, compilation, and backend selection.

## Creating the Engine

Create an engine with default settings or with custom options:

```cpp
#include <nautilus/Engine.hpp>

using namespace nautilus;

// Default configuration (MLIR backend, compilation enabled)
auto engine = engine::NautilusEngine();

// Custom configuration
engine::Options options;
options.setOption("engine.backend", "mlir");
auto engine = engine::NautilusEngine(options);
```

## Registering Functions

Use `registerFunction` to trace, compile, and obtain a callable for any function that uses `val<T>` parameter types. The returned callable accepts and returns raw C++ types.

### Function Pointers

```cpp
val<int32_t> myFunc(val<int32_t> x) {
    return x + 1;
}

auto compiled = engine.registerFunction(myFunc);
int32_t result = compiled(5); // Returns 6
```

### Lambdas

Lambdas work the same way. Wrap the lambda in a `std::function` with the appropriate signature:

```cpp
std::function<val<int32_t>(val<int32_t>)> increment = [](val<int32_t> x) {
    return x + 1;
};

auto compiled = engine.registerFunction(increment);
int32_t result = compiled(5); // Returns 6
```

### Void Return Type

Functions that return void are also supported:

```cpp
void writeResult(val<int32_t*> output, val<int32_t> value) {
    output[0] = value;
}

auto compiled = engine.registerFunction(writeResult);
int32_t buffer = 0;
compiled(&buffer, 42); // buffer is now 42
```

## Backend Selection

Nautilus supports three compilation backends. Set the backend through the `engine.backend` option.

### MLIR (Default)

The MLIR backend compiles through MLIR and LLVM to native machine code. It produces the best runtime performance and is the recommended backend for production use.

```cpp
options.setOption("engine.backend", "mlir");
```

### C++

The C++ backend generates compilable C++ source code. It is useful for debugging because you can inspect the generated code directly.

```cpp
options.setOption("engine.backend", "cpp");
```

### Bytecode

The bytecode backend interprets bytecode at runtime. It has the fastest compilation time and does not require native code generation, making it portable across platforms.

```cpp
options.setOption("engine.backend", "bc");
```

## Disabling Compilation

For debugging purposes, you can disable compilation entirely. When compilation is disabled, registered functions execute directly through the `val<T>` wrappers without tracing or code generation. This is useful for verifying that a function produces correct results before investigating compilation issues.

```cpp
options.setOption("engine.Compilation", false);
```

## Modules

When you need to compile multiple functions together into a single compilation unit, use modules. All functions registered with a module share one IR graph and are compiled by the same backend in one pass.

```cpp
auto module = engine.createModule();

module.registerFunction<val<int32_t>(val<int32_t>)>("inc",
    [](val<int32_t> x) { return x + 1; });

module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("sum",
    [](val<int64_t> a, val<int64_t> b) { return a + b; });

auto compiled = module.compile();

auto inc = compiled.getFunction<int32_t(int32_t)>("inc");
auto sum = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

int32_t r1 = inc(5);      // Returns 6
int64_t r2 = sum(10, 20); // Returns 30
```

The `registerFunction` template parameter specifies the val-typed function signature (e.g., `val<int32_t>(val<int32_t>)`), while `getFunction` uses the corresponding raw C++ signature (e.g., `int32_t(int32_t)`).

## Runtime Options Reference

The following table lists all available runtime options.

### Engine Options

| Option | Default | Description |
|--------|---------|-------------|
| `engine.compilation` | `true` | Enables compilation of Nautilus functions. When `false`, all functions are executed directly through `val<T>` wrappers without tracing or code generation. Useful for debugging. |
| `engine.backend` | `mlir` | Selects the compilation backend. Valid values: `mlir`, `bc`, `cpp`. |
| `engine.normalizeFunctionNames` | `false` | Generates normalized function names (`runtimeFunc0`, `runtimeFunc1`, etc.) during tracing instead of using actual function names. Useful for deterministic output and testing. |
| `engine.demangleFunctionNames` | `true` | Demangles C++ function names to human-readable format during tracing. When disabled, uses mangled names as-is. Ignored if `normalizeFunctionNames` is enabled. |

### Dump Options

These options control the output of intermediate representations at various stages of the compilation pipeline.

| Option | Default | Description |
|--------|---------|-------------|
| `dump.all` | `false` | Dumps intermediate representations at all compilation steps. |
| `dump.after_tracing` | `false` | Dumps traces directly after trace generation. |
| `dump.after_ssa` | `false` | Dumps traces after SSA generation. |
| `dump.after_ir_creation` | `false` | Dumps the Nautilus IR after generation. |
| `dump.after_mlir_generation` | `false` | Dumps the generated MLIR (MLIR backend only). |
| `dump.after_llvm_generation` | `false` | Dumps the generated LLVM IR (MLIR backend only). |
| `dump.after_cpp_generation` | `false` | Dumps the generated C++ code (C++ backend only). |
| `dump.after_bc_generation` | `false` | Dumps the generated bytecode (bytecode backend only). |
| `dump.console` | `false` | Outputs intermediate representations to the console. |
| `dump.file` | `true` | Writes intermediate representations to a temporary folder. |

### Graph Visualization Options

| Option | Default | Description |
|--------|---------|-------------|
| `dump.graph` | `false` | Dumps a graph representation of the IR to a file. |
| `dump.graph.type` | `graphviz` | Sets the graph format. Valid values: `graphviz`, `mermaid`. |
| `dump.graph.full` | `false` | When `true`, represents both data and control flow. When `false`, shows only the control flow graph. |

### MLIR Backend Options

| Option | Default | Description |
|--------|---------|-------------|
| `mlir.optimizationLevel` | `3` | Sets the optimization level for MLIR code generation (0--3). |
| `mlir.enableMultithreading` | `true` | Allows the MLIR backend to use multiple threads during compilation. |
| `mlir.inline_invoke_calls` | `false` | Allows the MLIR backend to inline functions tagged with `NAUTILUS_INLINE`. |

## Debugging with Dump Options

To inspect what Nautilus generates at each stage of the compilation pipeline, enable dump options. The simplest approach is to dump everything to the console:

```cpp
engine::Options options;
options.setOption("dump.all", true);
options.setOption("dump.console", true);
auto engine = engine::NautilusEngine(options);
```

This prints the execution trace, SSA form, Nautilus IR, and backend-specific output (MLIR, LLVM IR, C++ source, or bytecode) to standard output.

To inspect only a specific stage:

```cpp
options.setOption("dump.after_ir_creation", true);
options.setOption("dump.console", true);
```

By default, `dump.file` is enabled, so intermediate representations are also written to a temporary folder even without `dump.console`.

### Visualizing the IR Graph

Enable graph output to produce Graphviz or Mermaid diagrams of the control flow and data flow:

```cpp
options.setOption("dump.graph", true);
options.setOption("dump.graph.type", "graphviz"); // or "mermaid"
options.setOption("dump.graph.full", true);        // include data flow edges
```

See [graphs.md](graphs.md) for detailed examples of generated graph visualizations.
