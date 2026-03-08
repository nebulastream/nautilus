# Nautilus IR Debugger — Design Document

## Motivation

Nautilus traces C++ functions through `val<T>` wrappers and compiles them via an IR pipeline to native code. Today, once code is compiled, developers have no way to step through the **Nautilus IR** that was actually executed. Standard debuggers (GDB/LLDB) only see the final machine code and LLVM IR — not the high-level Nautilus IR operations the developer authored. This makes it difficult to:

- Understand how a traced function was compiled
- Inspect intermediate values at the Nautilus IR level
- Correlate compiled code back to the original C++ source

This document proposes extending the Nautilus compilation pipeline to make the IR **debuggable** — enabling step-by-step execution through Nautilus IR with variable inspection and source-code references.

---

## Goals

1. **Step debugging** through Nautilus IR operations (step, next, continue, breakpoints)
2. **Variable inspection** — view the current value of any SSA variable at any IR operation
3. **Source mapping** — correlate each IR operation back to the original C++ source line
4. **MLIR backend focus** — initial implementation targets the MLIR → LLVM → native path

## Non-Goals (for now)

- Full GDB/LLDB protocol integration (future work)
- Debugging across multiple Nautilus functions simultaneously
- Reverse debugging / time-travel
- Support for non-MLIR backends (C++, bytecode, AsmJit)

---

## Architecture Overview

The debugger requires changes at three layers:

```
┌─────────────────────────────────────────────────────────┐
│  Layer 1: Source Location Capture                       │
│  (val<T> operators → TracingInterface → ExecutionTrace) │
│  Propagate C++ source_location through the trace        │
└──────────────────────┬──────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────┐
│  Layer 2: IR Debug Metadata                             │
│  (Operation, IRGraph, MLIR Lowering)                    │
│  Attach source locations + variable names to IR ops     │
│  Emit LLVM debug info (DWARF) via MLIR                  │
└──────────────────────┬──────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────┐
│  Layer 3: IR Debugger Interface                         │
│  (New: IRDebugger, IRDebugSession)                      │
│  Interactive or programmatic stepping through IR        │
│  Breakpoints, variable queries, source display          │
└─────────────────────────────────────────────────────────┘
```

---

## Layer 1: Source Location Capture

### Problem

Currently, when `val<T>` operations are traced, **no source location information** is captured. The `TracingInterface` methods (`traceBinaryOp`, `traceUnaryOp`, etc.) accept only operation type, result type, and operand references. The existing `Tag` system captures stack frame addresses via `backtrace()`, but this is used for execution path identification — not for human-readable source locations.

### Approach: `std::source_location` at `val<T>` operators

C++20 provides `std::source_location` which can capture file, line, column, and function name at compile time with zero runtime cost via default arguments.

#### 1.1 New `SourceLocation` type

```cpp
// include/nautilus/common/SourceLocation.hpp
#pragma once
#include <cstdint>
#include <string_view>

namespace nautilus {

/// Lightweight source location captured at val<T> operation sites.
/// Stored by value; string_views point to static string literals
/// from std::source_location (lifetime == program lifetime).
struct SourceLocation {
	std::string_view file_name = {};
	std::string_view function_name = {};
	uint32_t line = 0;
	uint32_t column = 0;

	bool isValid() const { return line != 0; }
};

} // namespace nautilus
```

#### 1.2 Extend `TracingInterface`

Add an optional `SourceLocation` parameter to all tracing methods. This is backward-compatible — existing call sites that don't pass a location get a default empty one.

```cpp
// include/nautilus/tracing/TracingInterface.hpp (changes)
virtual TypedValueRef& traceBinaryOp(
    Op op, Type resultType,
    const TypedValueRef& left,
    const TypedValueRef& right,
    SourceLocation loc = {}) = 0;

virtual TypedValueRef& traceUnaryOp(
    Op op, Type resultType,
    const TypedValueRef& input,
    SourceLocation loc = {}) = 0;

// ... same pattern for traceCall, traceConstant, traceBool, etc.
```

#### 1.3 Capture at `val<T>` operator sites

Use `std::source_location::current()` as a default argument in `val<T>` operators:

```cpp
// include/nautilus/val_arith.hpp (example change)
#include <source_location>

template <typename RightType>
val<ResultType> operator+(
    const val<RightType>& right,
    std::source_location caller = std::source_location::current()) const
{
    if SHOULD_TRACE () {
        SourceLocation loc{caller.file_name(), caller.function_name(),
                           caller.line(), caller.column()};
        auto resultRef = tracing::traceBinaryOp(
            tracing::Op::ADD,
            tracing::TypeResolver<ResultType>::to_type(),
            this->state, right.state, loc);
        return val<ResultType>(resultRef);
    }
    return val<ResultType>(value + right.value);
}
```

**Key insight**: `std::source_location::current()` as a default argument captures the **caller's** location, not the operator's — so it correctly points to the user's code.

#### 1.4 Store in `TraceOperation`

```cpp
// src/nautilus/tracing/TraceOperation.hpp (addition)
class TraceOperation {
    Snapshot tag;
    Op op;
    Type resultType;
    TypedValueRef resultRef;
    std::vector<InputVariant> input;
    SourceLocation sourceLocation;  // NEW
};
```

---

## Layer 2: IR Debug Metadata

### 2.1 Extend `Operation` with debug metadata

Add an optional `SourceLocation` to the base `Operation` class:

```cpp
// src/nautilus/compiler/ir/operations/Operation.hpp (additions)
#include "nautilus/common/SourceLocation.hpp"

class Operation {
public:
    // ... existing interface ...

    void setSourceLocation(const SourceLocation& loc) { sourceLocation_ = loc; }
    const SourceLocation& getSourceLocation() const { return sourceLocation_; }

    /// Optional human-readable name for the result variable (for debugger display).
    void setDebugName(std::string_view name) { debugName_ = name; }
    std::string_view getDebugName() const { return debugName_; }

protected:
    // ... existing fields ...
    SourceLocation sourceLocation_;
    std::string_view debugName_;
};
```

### 2.2 Propagate through `TraceToIRConversionPhase`

During trace → IR conversion, copy `SourceLocation` from each `TraceOperation` to its corresponding `Operation`:

```cpp
// In TraceToIRConversionPhase (pseudo-code)
void processOperation(const TraceOperation& traceOp, ...) {
    auto* irOp = createIROperation(traceOp);
    irOp->setSourceLocation(traceOp.sourceLocation);
    // ...
}
```

### 2.3 Preserve through optimization phases

Optimization phases must preserve source locations when transforming operations:
- **Constant propagation**: When folding `$3 = $1 + $2` into `$3 = 15`, keep the source location of the original add.
- **Dead code elimination**: Removed operations lose their locations (acceptable).
- **Operation replacement**: When an operation is replaced, the replacement inherits the original's source location.

This is a standard principle — LLVM and MLIR follow the same convention for `DILocation` preservation.

### 2.4 MLIR: Emit real `FileLineColLoc` instead of hardcoded locations

The current `getNameLoc()` emits `FileLineColLoc("Query_1", 0, 0)` for **all** operations. Replace this:

```cpp
// src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.cpp
mlir::Location MLIRLoweringProvider::getLocationForOp(const ir::Operation* op) {
    auto& srcLoc = op->getSourceLocation();
    if (srcLoc.isValid()) {
        auto fileLoc = mlir::FileLineColLoc::get(
            builder->getStringAttr(srcLoc.file_name),
            srcLoc.line,
            srcLoc.column);
        // Fuse with a name for the IR operation identifier
        return mlir::FusedLoc::get(
            context,
            {fileLoc, mlir::NameLoc::get(
                builder->getStringAttr(op->getIdentifier().toString()))});
    }
    // Fallback for operations without source info
    return getNameLoc(op->getIdentifier().toString());
}
```

Then, in each `generateMLIR(SomeOperation*, ValueFrame&)` method, use `getLocationForOp(op)` instead of `getNameLoc("...")`.

### 2.5 LLVM Debug Info generation

With real `FileLineColLoc` attached to MLIR operations, we can enable LLVM's debug info generation during the MLIR → LLVM lowering. MLIR's standard LLVM lowering can automatically generate `DILocation` metadata from `FileLineColLoc`. This needs:

1. A `DISubprogram` for the compiled function (created during `FunctionOperation` lowering)
2. A `DICompileUnit` for the compilation module
3. Set the LLVM module's debug info version

```cpp
// During MLIR → LLVM lowering setup
void MLIRCompilationBackend::addDebugInfoPass(mlir::PassManager& pm) {
    // Use MLIR's built-in DI generation or a custom pass
    // that creates DICompileUnit + DISubprogram and maps
    // FileLineColLoc → DILocation on each LLVM dialect op.
}
```

This enables standard debuggers (GDB/LLDB) to step through the JIT'd code and see the original C++ source lines.

---

## Layer 3: IR Debugger Interface

This is the core new component. It provides an interactive debugging experience at the **Nautilus IR level**.

### 3.1 Design: Interpreted IR Execution with Debug Hooks

Rather than relying solely on DWARF/GDB, the IR debugger **interprets the Nautilus IR directly** with debugging hooks. This gives us full control over the debugging experience without depending on external debugger integration.

```
┌────────────────────────────────┐
│         IRDebugSession         │
│  - Holds IRGraph               │
│  - Manages breakpoints         │
│  - Controls execution          │
│  - Tracks variable values      │
├────────────────────────────────┤
│        IRInterpreter           │
│  - Walks BasicBlocks           │
│  - Evaluates Operations        │
│  - Calls debug hooks           │
│  - Manages ValueFrame          │
├────────────────────────────────┤
│       DebugEventHandler        │
│  (abstract interface)          │
│  - onOperationStep()           │
│  - onBreakpointHit()           │
│  - onFunctionEntry/Exit()      │
│  - onVariableChanged()         │
└────────────────────────────────┘
```

### 3.2 Core Classes

```cpp
// src/nautilus/compiler/ir/debug/IRDebugSession.hpp
namespace nautilus::compiler::ir::debug {

/// Identifies a position in the IR for breakpoints.
struct IRPosition {
    std::string blockId;
    uint32_t operationIndex;
};

/// A breakpoint set on the IR.
struct IRBreakpoint {
    uint32_t id;
    IRPosition position;
    bool enabled = true;
    // Optional: conditional breakpoint expression
    // std::string condition;
};

/// Represents the value of a variable during debugging.
struct VariableInfo {
    OperationIdentifier id;        // e.g. $3
    Type type;                     // i64, f32, bool, ptr
    std::string debugName;         // optional user-friendly name
    SourceLocation definedAt;      // where it was defined
    // Value is type-erased; displayed as string
    std::string valueAsString;
};

/// Abstract handler for debug events.
class DebugEventHandler {
public:
    virtual ~DebugEventHandler() = default;

    /// Called before each operation executes.
    /// Return true to continue, false to pause.
    virtual bool onBeforeOperation(
        const Operation& op,
        const std::vector<VariableInfo>& liveVariables) = 0;

    /// Called when a breakpoint is hit.
    virtual void onBreakpointHit(
        const IRBreakpoint& bp,
        const Operation& op,
        const std::vector<VariableInfo>& liveVariables) = 0;

    /// Called on function entry with argument info.
    virtual void onFunctionEntry(
        const FunctionOperation& func,
        const std::vector<VariableInfo>& arguments) = 0;

    /// Called on function return.
    virtual void onFunctionReturn(
        const ReturnOperation& ret,
        const VariableInfo& returnValue) = 0;
};

/// The main debugger session controlling IR execution.
class IRDebugSession {
public:
    explicit IRDebugSession(std::shared_ptr<IRGraph> ir);

    // === Breakpoint Management ===
    uint32_t addBreakpoint(const IRPosition& pos);
    uint32_t addBreakpointAtSourceLine(std::string_view file, uint32_t line);
    void removeBreakpoint(uint32_t id);
    void enableBreakpoint(uint32_t id, bool enabled);
    std::vector<IRBreakpoint> listBreakpoints() const;

    // === Execution Control ===
    /// Run until completion or breakpoint.
    void run(DebugEventHandler& handler);
    /// Execute a single IR operation.
    void step(DebugEventHandler& handler);
    /// Step over a ProxyCall (execute it, stop at next op in current block).
    void next(DebugEventHandler& handler);
    /// Continue until the current block finishes.
    void stepOut(DebugEventHandler& handler);

    // === Inspection ===
    /// List all currently live variables and their values.
    std::vector<VariableInfo> getLiveVariables() const;
    /// Get a specific variable by its identifier.
    std::optional<VariableInfo> getVariable(const OperationIdentifier& id) const;
    /// Get the current IR position.
    IRPosition getCurrentPosition() const;
    /// Get the source location of the current operation.
    SourceLocation getCurrentSourceLocation() const;

    // === IR Listing ===
    /// Pretty-print the IR around the current position.
    std::string listIR(uint32_t contextLines = 5) const;
    /// Pretty-print a specific block.
    std::string listBlock(const std::string& blockId) const;

    // === Invocation ===
    /// Start execution with concrete arguments.
    template <typename... Args>
    void start(Args... args);

private:
    std::shared_ptr<IRGraph> ir_;
    std::unique_ptr<IRInterpreter> interpreter_;
    std::vector<IRBreakpoint> breakpoints_;
    uint32_t nextBreakpointId_ = 1;
};

} // namespace nautilus::compiler::ir::debug
```

### 3.3 IR Interpreter

The IR debugger needs an interpreter that can execute Nautilus IR operations one at a time, maintaining a value frame. This is conceptually similar to the existing **bytecode backend** but with debug hooks:

```cpp
// src/nautilus/compiler/ir/debug/IRInterpreter.hpp
namespace nautilus::compiler::ir::debug {

/// Type-erased runtime value for the interpreter.
using RuntimeValue = std::variant<
    bool, int8_t, int16_t, int32_t, int64_t,
    uint8_t, uint16_t, uint32_t, uint64_t,
    float, double, void*>;

/// Interpreter that walks IR operations with debug support.
class IRInterpreter {
public:
    explicit IRInterpreter(const IRGraph& ir);

    /// Execute a single operation, updating the value frame.
    void executeOperation(const Operation& op);

    /// Get the current value of an operation's result.
    std::optional<RuntimeValue> getValue(const OperationIdentifier& id) const;

    /// Get all live values.
    const std::unordered_map<OperationIdentifier, RuntimeValue>& getValueFrame() const;

    /// Set function arguments before execution.
    void setArgument(size_t index, RuntimeValue value);

    /// Get current block and operation index.
    const BasicBlock* getCurrentBlock() const;
    uint32_t getCurrentOperationIndex() const;

    /// Advance to the next operation. Returns false at function end.
    bool advance();

private:
    const IRGraph& ir_;
    std::unordered_map<OperationIdentifier, RuntimeValue> valueFrame_;
    const BasicBlock* currentBlock_;
    uint32_t currentOpIndex_;

    // Dispatch table for operation evaluation
    RuntimeValue evaluateAdd(const AddOperation& op);
    RuntimeValue evaluateSub(const SubOperation& op);
    RuntimeValue evaluateMul(const MulOperation& op);
    RuntimeValue evaluateCompare(const CompareOperation& op);
    RuntimeValue evaluateLoad(const LoadOperation& op);
    RuntimeValue evaluateStore(const StoreOperation& op);
    RuntimeValue evaluateCall(const ProxyCallOperation& op);
    // ... one per operation type
};

} // namespace nautilus::compiler::ir::debug
```

### 3.4 CLI Debugger Frontend

A simple interactive CLI debugger (modeled after GDB's interface):

```cpp
// src/nautilus/compiler/ir/debug/CLIDebugger.hpp
namespace nautilus::compiler::ir::debug {

/// Interactive CLI debugger for Nautilus IR.
/// Implements DebugEventHandler and provides a REPL.
class CLIDebugger : public DebugEventHandler {
public:
    explicit CLIDebugger(std::shared_ptr<IRGraph> ir);

    /// Start the interactive debug session.
    template <typename... Args>
    void debug(Args... args);

    // DebugEventHandler implementation
    bool onBeforeOperation(
        const Operation& op,
        const std::vector<VariableInfo>& liveVariables) override;
    void onBreakpointHit(
        const IRBreakpoint& bp,
        const Operation& op,
        const std::vector<VariableInfo>& liveVariables) override;
    void onFunctionEntry(
        const FunctionOperation& func,
        const std::vector<VariableInfo>& arguments) override;
    void onFunctionReturn(
        const ReturnOperation& ret,
        const VariableInfo& returnValue) override;

private:
    IRDebugSession session_;

    /// REPL command processing.
    enum class Command { Step, Next, Continue, StepOut,
                         Print, List, Breakpoint, Delete,
                         Info, Help, Quit };
    Command parseCommand(const std::string& input);
    void processCommand(const std::string& input);
    void printPrompt();
    void printCurrentLocation(const Operation& op);
    void printVariables(const std::vector<VariableInfo>& vars);
};

} // namespace nautilus::compiler::ir::debug
```

**Example interactive session:**

```
nautilus-dbg> list
  Block_0($1:i64):
      $2 = 10 :i64
  --> $3 = $1 + $2 :i64          [demo.cpp:42]
      $4 = $3 > 100 :bool        [demo.cpp:43]
      if $4 then Block_1 else Block_2

nautilus-dbg> print $1
  $1 : i64 = 5                   [arg 0]

nautilus-dbg> step
  Block_0, op 2: $3 = $1 + $2 :i64
  => $3 = 15

nautilus-dbg> print $3
  $3 : i64 = 15                  [demo.cpp:42: x + 10]

nautilus-dbg> break Block_1:0
  Breakpoint 1 set at Block_1, operation 0

nautilus-dbg> continue
  Breakpoint 1 hit at Block_1, operation 0
  $5 = $3 * 2 :i64               [demo.cpp:44]

nautilus-dbg> info locals
  $1 : i64 = 5                   [arg 0]
  $2 : i64 = 10                  [constant]
  $3 : i64 = 15                  [demo.cpp:42]
  $4 : bool = false              [demo.cpp:43]
```

### 3.5 Engine Integration

Expose the debugger through the `NautilusEngine` API:

```cpp
// include/nautilus/Engine.hpp (additions)

class NautilusEngine {
public:
    // ... existing interface ...

    /// Register a function and return a debuggable handle instead of compiling.
    template <typename R, typename... FunctionArguments>
    auto registerForDebug(std::function<R(val<FunctionArguments>...)> func) const {
        // 1. Trace the function (same as registerFunction)
        auto wrapper = details::createFunctionWrapper(func);
        auto [trace, ir] = jit.traceAndBuildIR(wrapper);
        // 2. Return a debug session instead of an executable
        return IRDebugSession(ir);
    }
};
```

Alternatively, a standalone entry point:

```cpp
// include/nautilus/debug.hpp
namespace nautilus::debug {

/// Trace a function and open an interactive IR debugger.
template <typename R, typename... FunctionArguments>
void debugFunction(
    std::function<R(val<FunctionArguments>...)> func,
    FunctionArguments... args)
{
    NautilusEngine engine;
    auto session = engine.registerForDebug(func);
    CLIDebugger debugger(session.getIR());
    debugger.debug(args...);
}

} // namespace nautilus::debug
```

---

## Source Code References

### 3.6 Displaying original source

When a `SourceLocation` is available, the debugger can read and display the original C++ source:

```cpp
// In CLIDebugger::printCurrentLocation
void CLIDebugger::printCurrentLocation(const Operation& op) {
    auto& loc = op.getSourceLocation();
    // Print IR operation
    fmt::print("  {} = {} :{}\n", op.getIdentifier().toString(),
               operationToString(op), typeToString(op.getStamp()));
    // Print source reference
    if (loc.isValid()) {
        fmt::print("  at {}:{} in {}\n",
                   loc.file_name, loc.line, loc.function_name);
        // Optionally read and display the source line
        auto sourceLine = readSourceLine(loc.file_name, loc.line);
        if (sourceLine) {
            fmt::print("  > {}\n", *sourceLine);
        }
    }
}
```

### 3.7 Source-level breakpoints

Users can set breakpoints by source file and line:

```
nautilus-dbg> break demo.cpp:42
Breakpoint 1 set at demo.cpp:42 (Block_0, op 2: $3 = $1 + $2)
```

The debugger resolves this by scanning all operations for matching `SourceLocation`.

---

## MLIR Backend: DWARF Debug Info (for GDB/LLDB integration)

As a secondary (future) capability, the MLIR backend can emit proper DWARF debug information so that standard debuggers work:

### 4.1 Debug Info Pass

Add a pass after MLIR lowering to LLVM dialect that:
1. Creates a `DICompileUnit` for the Nautilus compilation
2. Creates a `DISubprogram` for the `execute()` function
3. Maps each operation's `FileLineColLoc` → `DILocation`
4. Optionally creates `DILocalVariable` entries for SSA values

```cpp
// src/nautilus/compiler/backends/mlir/DebugInfoPass.hpp
namespace nautilus::compiler::mlir {

/// MLIR pass that adds LLVM debug metadata to the module.
class DebugInfoPass : public ::mlir::PassWrapper<
    DebugInfoPass, ::mlir::OperationPass<::mlir::ModuleOp>> {
public:
    void runOnOperation() override;

private:
    void createCompileUnit(::mlir::ModuleOp module);
    void createSubprogram(::mlir::LLVM::LLVMFuncOp func);
    void attachLocations(::mlir::LLVM::LLVMFuncOp func);
};

} // namespace nautilus::compiler::mlir
```

### 4.2 Integration with JIT

When debug mode is enabled, the LLVM JIT must be configured to **not** strip debug info:

```cpp
// In MLIRCompilationBackend
if (options.getOptionOrDefault("debug.enabled", false)) {
    // Disable optimizations that strip debug info
    // Set LLVM JIT to preserve debug sections
    // Register with GDB JIT interface (llvm::JITEventListener)
    auto gdbListener = llvm::JITEventListener::createGDBRegistrationListener();
    executionEngine->registerJITEventListener(*gdbListener);
}
```

This allows `gdb -p <pid>` to see source-level information for JIT'd Nautilus functions.

---

## Implementation Plan

### Phase 1: Source Location Capture (Foundation)
1. Add `SourceLocation` struct
2. Extend `TracingInterface` with optional location parameter
3. Update `val<T>` operators to capture `std::source_location`
4. Store in `TraceOperation`
5. Propagate through `TraceToIRConversionPhase` → `Operation`
6. Update IR pretty-printer to show source locations

### Phase 2: IR Interpreter + Debug Session
1. Implement `IRInterpreter` (operation evaluation, value frame)
2. Implement `IRDebugSession` (breakpoints, stepping, inspection)
3. Reuse logic from the bytecode backend where applicable
4. Add tests with simple functions

### Phase 3: CLI Debugger Frontend
1. Implement `CLIDebugger` with REPL
2. Commands: step, next, continue, print, list, break, info, help, quit
3. Source line display when `SourceLocation` is available
4. Integration via `NautilusEngine::registerForDebug()`

### Phase 4: MLIR Debug Info (GDB/LLDB support)
1. Update `MLIRLoweringProvider::getNameLoc()` → `getLocationForOp()`
2. Implement `DebugInfoPass` for DWARF metadata
3. Configure LLVM JIT to preserve debug info + GDB listener
4. Test with GDB/LLDB on JIT'd functions

---

## File Layout

```
nautilus/
├── include/nautilus/
│   ├── common/
│   │   └── SourceLocation.hpp          # New: source location struct
│   └── debug.hpp                       # New: public debug API
│
├── src/nautilus/
│   ├── compiler/
│   │   ├── ir/
│   │   │   ├── operations/
│   │   │   │   └── Operation.hpp       # Modified: add SourceLocation field
│   │   │   └── debug/                  # New directory
│   │   │       ├── IRDebugSession.hpp
│   │   │       ├── IRDebugSession.cpp
│   │   │       ├── IRInterpreter.hpp
│   │   │       ├── IRInterpreter.cpp
│   │   │       ├── CLIDebugger.hpp
│   │   │       └── CLIDebugger.cpp
│   │   │
│   │   └── backends/mlir/
│   │       ├── MLIRLoweringProvider.hpp # Modified: getLocationForOp()
│   │       ├── MLIRLoweringProvider.cpp # Modified: use real locations
│   │       ├── DebugInfoPass.hpp       # New: DWARF generation
│   │       └── DebugInfoPass.cpp
│   │
│   └── tracing/
│       ├── TracingInterface.hpp        # Modified: add SourceLocation param
│       ├── TraceContext.hpp            # Modified: store SourceLocation
│       ├── TraceOperation.hpp          # Modified: add SourceLocation field
│       └── phases/
│           └── TraceToIRConversionPhase.cpp  # Modified: propagate locations
```

---

## Open Questions

1. **Performance impact of `std::source_location`**: While `std::source_location::current()` is evaluated at compile time, adding a default parameter to every `val<T>` operator increases the parameter list. Benchmark the tracing overhead.

2. **Variable naming**: SSA variables are `$1`, `$2`, etc. We could try to recover meaningful names by demangling the `std::source_location::function_name()` or by adding explicit `val<T>::setName("x")` annotations. Which approach is preferred?

3. **Optimization phase location merging**: When constant propagation folds two operations, which source location wins? Proposal: keep the location of the "primary" operation (the one whose result is kept).

4. **Scope of IR interpreter**: Should it support all operations including `ProxyCallOperation` (external function calls)? If yes, it needs access to the original function pointers, which are available in the trace's `FunctionCall` metadata.

5. **Thread safety**: The `CLIDebugger` blocks on stdin. Should we also offer a non-blocking `DebugEventHandler` for integration with IDEs or test frameworks?
