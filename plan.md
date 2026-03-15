# Plan: Module-based Multi-Function Compilation

## Design Summary

A `NautilusModule` allows users to register multiple named functions, compile them together into a single backend compilation unit, and obtain lightweight callable handles. The entire compilation flow goes through `JITCompiler`.

## User-Facing API

```cpp
auto engine = NautilusEngine(options);

// Create a module from the engine
auto module = engine.createModule();

// Add named functions (different signatures supported via type erasure)
module.addFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("add", addFunc);
module.addFunction<val<int64_t>(val<int64_t>)>("square", squareFunc);

// Compile all functions into one compilation unit
auto compiled = module.compile();

// Get lightweight typed handles (invalid after compiled is destroyed)
auto add = compiled.getFunction<int32_t, int32_t, int32_t>("add");
auto square = compiled.getFunction<int64_t, int64_t>("square");

// Call them
add(1, 2);   // returns 3
square(5);   // returns 25
```

## Interaction with `registerFunction()`

- `registerFunction()` is unchanged — single-function, independent lifecycle
- `createModule()` provides the grouped compilation path
- Both go through `JITCompiler` for tracing + IR generation
- Both use the same `CompilationBackend` interface (extended for modules)

## Implementation Steps

### Step 1: Make function names configurable in tracing → IR conversion

**File:** `nautilus/src/nautilus/tracing/phases/TraceToIRConversionPhase.hpp/.cpp`

Currently hardcodes `"execute"` as the function name (line 53 of .cpp). Add a parameter:
- `TraceToIRConversionPhase::apply(trace, "myFunctionName")` → `FunctionOperation("myFunctionName", ...)`
- Default remains `"execute"` for backward compatibility

### Step 2: Add `compileModule()` to `JITCompiler`

**Files:** `nautilus/include/nautilus/JITCompiler.hpp`, `nautilus/src/nautilus/compiler/JITCompiler.cpp`

New method:
```cpp
struct NamedFunction {
    std::string name;
    wrapper_function wrapper;
};
std::unique_ptr<Executable> compileModule(const std::vector<NamedFunction>& functions) const;
```

Implementation:
1. For each `NamedFunction`: trace → SSA → IR conversion with the given name
2. Collect all `IRGraph`s into a vector of `(name, IRGraph)` pairs
3. Call `backend->compileModule(named_graphs, dumpHandler, options)`
4. Return single `Executable` that contains all functions

### Step 3: Extend `CompilationBackend` with `compileModule()`

**File:** `nautilus/src/nautilus/compiler/backends/CompilationBackend.hpp`

```cpp
virtual std::unique_ptr<Executable> compileModule(
    const std::vector<std::pair<std::string, std::shared_ptr<ir::IRGraph>>>& functions,
    const DumpHandler& dumpHandler,
    const engine::Options& options) const;
```

Default implementation: compile each function separately and wrap in a `CompositeExecutable` (fallback for backends that don't support true module compilation). MLIR overrides this for true single-module compilation.

### Step 4: Implement MLIR module compilation

**Files:**
- `nautilus/src/nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp/.cpp`
- `nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp/.cpp`
- `nautilus/src/nautilus/compiler/backends/mlir/LLVMIROptimizer.cpp`
- `nautilus/src/nautilus/compiler/backends/mlir/MLIRExecutable.hpp/.cpp`

Changes:
1. **MLIRLoweringProvider**: Add `generateModuleFromIRs(vector<pair<name, IRGraph>>)` — generates multiple `func.func` ops in one MLIR module, each with its given name
2. **MLIRCompilationBackend**: Override `compileModule()` — creates one MLIRContext, one lowering provider, calls `generateModuleFromIRs()`, runs passes, JIT compiles, returns one `MLIRExecutable`
3. **LLVMIROptimizer**: Change hardcoded `"execute"` to iterate over all function names and apply attributes to each
4. **MLIRExecutable**: `getInvocableFunctionPtr(member)` already looks up by name via `engine->lookupPacked(member)` — verify this works for non-"execute" names

### Step 5: Create `NautilusModule` and `CompiledModule`

**New file:** `nautilus/include/nautilus/NautilusModule.hpp`

```cpp
namespace nautilus::engine {

// Lightweight callable handle — just a function pointer or GenericInvocable ref
template <typename R, typename... Args>
class ModuleFunction {
public:
    R operator()(Args... args);
private:
    friend class CompiledModule;
    compiler::Executable::Invocable<R, Args...> invocable_;
    ModuleFunction(compiler::Executable::Invocable<R, Args...> invocable);
};

class CompiledModule {
public:
    template <typename R, typename... Args>
    ModuleFunction<R, Args...> getFunction(const std::string& name);
private:
    friend class NautilusModule;
    std::unique_ptr<compiler::Executable> executable_;
    CompiledModule(std::unique_ptr<compiler::Executable> exec);
};

class NautilusModule {
public:
    template <typename FuncType>
    void addFunction(const std::string& name, FuncType func);

    CompiledModule compile();

private:
    friend class NautilusEngine;
    NautilusModule(const compiler::JITCompiler& jit, const Options& options);

    const compiler::JITCompiler& jit_;
    const Options& options_;
    std::vector<compiler::JITCompiler::NamedFunction> functions_;
};
```

### Step 6: Add `createModule()` to `NautilusEngine`

**File:** `nautilus/include/nautilus/Engine.hpp`

```cpp
NautilusModule createModule() const;
```

Only works with `JITCompiler` (not multi-tier). Passes the JITCompiler reference and options to the module.

### Step 7: Create fallback `CompositeExecutable` for non-MLIR backends

**New file:** `nautilus/src/nautilus/compiler/CompositeExecutable.hpp/.cpp`

Wraps multiple individual `Executable` objects (one per function). Used as fallback when backend doesn't override `compileModule()`. Maps function names to their individual executables.

### Step 8: Add tests

**New file:** `nautilus/test/execution-tests/ModuleTest.cpp`

Tests:
- Register 2+ functions, compile module, call each function correctly
- Different function signatures in same module
- Module destruction invalidates handles (document, not necessarily test crash)
- Works with MLIR backend (true module compilation)
- Works with other backends (fallback composite)

## Files Summary

| Action | File |
|--------|------|
| Modify | `TraceToIRConversionPhase.hpp/.cpp` — configurable function name |
| Modify | `JITCompiler.hpp/.cpp` — `compileModule()` |
| Modify | `CompilationBackend.hpp` — virtual `compileModule()` with default |
| Modify | `MLIRCompilationBackend.hpp/.cpp` — override `compileModule()` |
| Modify | `MLIRLoweringProvider.hpp/.cpp` — `generateModuleFromIRs()` |
| Modify | `LLVMIROptimizer.cpp` — handle multiple function names |
| Modify | `MLIRExecutable.hpp/.cpp` — verify named lookup works |
| Modify | `Engine.hpp` + `Engine.cpp` — `createModule()` |
| Create | `NautilusModule.hpp` — Module, CompiledModule, ModuleFunction |
| Create | `CompositeExecutable.hpp/.cpp` — fallback for non-MLIR backends |
| Create | `ModuleTest.cpp` — tests |
| Modify | `nautilus/CMakeLists.txt` or relevant sub-CMakeLists — add new files |
