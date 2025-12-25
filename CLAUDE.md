# CLAUDE.md - AI Assistant Development Guide for Nautilus

This document provides comprehensive guidance for Claude and other AI assistants working on the Nautilus codebase. It covers architecture, conventions, workflows, and best practices for contributing to this project.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture & Design](#architecture--design)
3. [Directory Structure Guide](#directory-structure-guide)
4. [Code Conventions & Style](#code-conventions--style)
5. [Development Workflows](#development-workflows)
6. [Testing & CI/CD](#testing--cicd)
7. [Key Systems Explained](#key-systems-explained)
8. [Common Development Patterns](#common-development-patterns)
9. [Debugging & Troubleshooting](#debugging--troubleshooting)
10. [Performance Considerations](#performance-considerations)

---

## Project Overview

### What is Nautilus?

Nautilus is a lightweight, tracing Just-In-Time (JIT) compiler for C++ projects developed by the DIMA group at TU Berlin. It is used within NebulaStream for query compilation and enables developers to write imperative C++ functions that are traced and compiled to efficient code using multiple backend technologies.

**Key Characteristics:**
- **Language**: C++20 (required minimum)
- **License**: MIT (Copyright 2024 Philipp Grulich)
- **Primary Use**: Query compilation in database systems
- **Architecture**: Modular, multi-backend compilation pipeline
- **Publication**: SIGMOD 2024 (research-backed)

### Core Purpose

The project allows C++ functions to:
1. Be wrapped in template types (`val<T>`) that track operations
2. Have all operations traced during execution
3. Be compiled to optimized machine code via IR transformation
4. Execute with JIT or interpreted backends
5. Support multiple compilation targets (MLIR, C++, Bytecode, AsmJit)

---

## Architecture & Design

### High-Level Compilation Pipeline

```
User Code (C++ function)
    ↓
Tracing (val<T> wrapper operations)
    ↓
Execution Trace Capture
    ↓
IR Generation (Control Flow Graph + SSA)
    ↓
Optimization Phases
    ├─ Constant Propagation
    ├─ Dead Code Elimination
    ├─ Loop Analysis
    └─ Control Flow Analysis
    ↓
Backend Selection
    ├─ MLIR → LLVM → Native Code (Primary)
    ├─ C++ Source Generation
    ├─ Bytecode Interpretation
    └─ AsmJit Assembly
    ↓
Executable Function
    ↓
Runtime Execution
```

### Design Principles

1. **Dual-Mode Execution**: Code works both with and without tracing enabled
2. **Type Safety**: Extensive use of C++20 concepts and templates
3. **Multiple Code Paths**: Different backends for different performance requirements
4. **Modular Organization**: Clean separation between tracing, compilation, and codegen
5. **Research-Backed**: Based on peer-reviewed academic work

### Core Components

| Component | Location | Purpose |
|-----------|----------|---------|
| **Engine** | `include/nautilus/Engine.hpp` | Central orchestrator for compilation workflow |
| **Value Wrapper (val<T>)** | `include/nautilus/val.hpp` | Template that traces operations on values |
| **Tracing System** | `src/nautilus/tracing/` | Records operations during execution |
| **IR System** | `src/nautilus/compiler/ir/` | Intermediate representation (control flow graphs) |
| **Optimization Phases** | `src/nautilus/compiler/ir/phases/` | IR transformation and optimization passes |
| **MLIR Backend** | `src/nautilus/compiler/backends/mlir/` | Primary code generation via MLIR |
| **Type System** | `include/nautilus/val_*.hpp` | Type definitions and specializations |

---

## Directory Structure Guide

### Root Level

```
/nautilus/
├── CMakeLists.txt           # Main build configuration with feature flags
├── README.md                # Project documentation and paper reference
├── LICENSE                  # MIT License
├── format.sh                # Code formatting validation/enforcement script
├── .clang-format            # Code formatting rules (120-char limit, tab indent)
├── .clang-tidy              # Static analysis configuration (strict checks)
├── .editorconfig            # Editor settings (tabs, UTF-8, max line width)
├── .gitignore               # Git ignore patterns
├── .github/workflows/       # CI/CD pipeline definitions
├── cmake/                   # CMake utility modules and macros
├── docs/                    # Additional documentation
├── nautilus/                # Main library directory
├── example/                 # Example project demonstrating usage
└── third_party/             # Bundled external dependencies
```

### Source Directory (`nautilus/`)

```
nautilus/
├── include/nautilus/        # Public API headers (what users import)
│   ├── val.hpp              # Main header - includes specializations and operators
│   ├── val_details.hpp      # Implementation utilities (RawValueResolver, StateResolver)
│   ├── val_arith.hpp        # Generic val<T> specialization for arithmetic types
│   ├── val_bool.hpp         # Specialized val<bool> with probability tracking
│   ├── val_ptr.hpp          # Pointer specialization
│   ├── val_enum.hpp         # Enumeration value type
│   ├── Engine.hpp           # Main API - orchestrates compilation
│   ├── Executable.hpp       # Wraps compiled executable function
│   ├── JITCompiler.hpp      # JIT compilation interface
│   ├── function.hpp         # Function traits and helpers
│   ├── options.hpp          # Engine runtime options
│   ├── core.hpp             # Core type definitions
│   ├── inline.hpp           # Inline annotation macros
│   ├── common/              # Utility headers (File, TypedValueRefHolder)
│   ├── tracing/             # Public tracing API
│   ├── std/                 # Standard library wrapper (STD namespace)
│   └── profile/             # Profiling utilities
│
├── src/nautilus/            # Implementation files
│   ├── api/                 # API implementation (profile, std)
│   ├── common/              # Utility implementations (logging, file I/O)
│   ├── compiler/            # **MAJOR SUBSYSTEM** - Compilation pipeline
│   │   ├── ir/              # Intermediate Representation (graphs, operations)
│   │   │   ├── IRGraph.hpp  # Core IR graph structure (control flow graph + SSA)
│   │   │   ├── operations/  # 85+ IR operation type definitions
│   │   │   │   ├── Arithmetic* - Math operations
│   │   │   │   ├── Logical* - Boolean operations
│   │   │   │   ├── Memory* - Load/store operations
│   │   │   │   ├── ControlFlow* - Branches, loops
│   │   │   │   └── Call* - Function calls
│   │   │   ├── blocks/      # Basic block structures
│   │   │   ├── phases/      # Optimization passes (7+ optimization phases)
│   │   │   │   ├── SSACreationPhase.hpp - Convert to SSA form
│   │   │   │   ├── ConstantPropagationPhase.hpp - Constant folding
│   │   │   │   ├── DeadCodeElimination.hpp - Remove unused code
│   │   │   │   ├── LoopAnalysisPhase.hpp - Detect and analyze loops
│   │   │   │   └── ControlFlowAnalysisPhase.hpp - Simplify control flow
│   │   │   ├── util/        # Utilities (GraphViz dumping, pretty printing)
│   │   │   └── [core IR files]
│   │   │
│   │   └── backends/        # Code generation backends (pluggable architecture)
│   │       ├── mlir/        # **PRIMARY BACKEND** - MLIR → LLVM → Native
│   │       ├── cpp/         # Generate C++ source code
│   │       ├── bc/          # Bytecode interpreter
│   │       └── asmjit/      # Direct assembly generation (experimental)
│   │
│   ├── tracing/             # **MAJOR SUBSYSTEM** - Execution tracing
│   │   ├── phases/          # Tracing phases (SSA creation, IR conversion)
│   │   ├── tag/             # Tag system for symbolic execution
│   │   ├── symbolic_execution/ # Symbolic execution engine
│   │   ├── exceptions/      # Exception handling during tracing
│   │   ├── ExecutionContext.hpp - Execution state management
│   │   ├── ExecutionTrace.hpp - Recorded trace representation
│   │   ├── [tracing implementation files]
│   │   └── CMakeLists.txt
│   │
│   ├── exceptions/          # Custom exception types
│   ├── llvm-passes/         # Custom LLVM optimization passes
│   ├── profile/             # Profiling implementation
│   ├── logging.cpp/hpp      # Logging initialization (spdlog)
│   └── CMakeLists.txt       # Source compilation configuration
│
├── test/                    # Comprehensive test suite
│   ├── execution-tests/     # Runtime execution behavior tests
│   │   ├── std/             # Standard library wrapper tests
│   │   ├── ControlFlowTest.cpp
│   │   ├── ExpressionTest.cpp
│   │   ├── LoopTest.cpp
│   │   ├── PointerTest.cpp
│   │   └── [15+ test files]
│   │
│   ├── val-tests/           # val<T> template type tests
│   │   ├── IntegerValueTest.cpp
│   │   ├── FloatValueTest.cpp
│   │   ├── PointerValueTest.cpp
│   │   └── FunctionTest.cpp
│   │
│   ├── llvm-ir-test/        # Generated LLVM IR validation tests
│   │   ├── intrinsic-ir/    # Generated intrinsic IR files
│   │   └── reference-ir/    # Reference IR for comparison
│   │
│   ├── benchmark/           # Performance benchmarks (Catch2 format)
│   │   ├── ExecutionBenchmark.cpp
│   │   └── TracingBenchmark.cpp
│   │
│   ├── common/              # Test utilities and helpers
│   │   ├── ExecutionTest.hpp - Base test class with utilities
│   │   ├── DumpHelper.hpp - Dump and formatting utilities
│   │   └── TestExecutable.hpp - Executable wrapper for tests
│   │
│   ├── data/                # Test data (12 categories with expected outputs)
│   │   ├── bool-tests/      # Boolean operation tests
│   │   ├── cast-tests/      # Type casting tests
│   │   ├── control-flow-tests/
│   │   ├── enum-tests/
│   │   ├── expression-tests/
│   │   ├── loop-tests/
│   │   ├── pointer-tests/
│   │   ├── regressions/     # Regression tests for fixed bugs
│   │   ├── runtime-call-tests/
│   │   ├── static-loop-tests/
│   │   └── [each category contains: tracing/, ir/, after_ssa/ subdirectories]
│   │
│   └── CMakeLists.txt       # Test compilation configuration
│
└── CMakeLists.txt           # Nautilus library configuration
```

### Key Files to Study First

1. **`include/nautilus/val.hpp`** (Main Value Header)
   - Orchestrates all val<T> specializations and operator definitions
   - Includes specializations and defines all operator overloads
   - Start here for overview of value wrapper architecture

2. **`include/nautilus/val_arith.hpp`** (Arithmetic Specialization)
   - Partial specialization for arithmetic types (int, float, etc.)
   - Shows how operations are wrapped and traced for numeric types
   - Study for understanding the generic template structure

3. **`include/nautilus/val_bool.hpp`** (Boolean Specialization)
   - Explicit specialization for boolean values
   - Includes probability tracking for branch prediction optimization
   - Study for understanding type-specific optimizations

4. **`include/nautilus/val_details.hpp`** (Implementation Utilities)
   - Core utilities: RawValueResolver, StateResolver, helper macros
   - Breaking point for circular dependencies in template design
   - Study for understanding template decomposition patterns

5. **`include/nautilus/Engine.hpp`** (API Entry Point)
   - Central orchestrator for compilation
   - Shows how users interact with Nautilus
   - Options-based configuration pattern

3. **`src/nautilus/compiler/ir/IRGraph.hpp`** (IR Representation)
   - Core intermediate representation structure
   - Control flow graph + SSA representation
   - Foundation for all optimizations

4. **`example/src/DemoJit.cpp`** (Working Example)
   - Simple, complete example of Nautilus usage
   - Shows aggregation operation with tracing
   - Reference for typical usage patterns

---

## Code Conventions & Style

### Naming Conventions

These are enforced by `.clang-tidy`:

| Element | Convention | Example |
|---------|-----------|---------|
| **Classes** | `CamelCase` | `IRGraph`, `ExecutionTrace`, `ConstantPropagationPhase` |
| **Functions** | `CamelCase` | `AddOperation()`, `GetBasicBlocks()`, `TraceExecution()` |
| **Member Variables** | `lower_case` with underscore suffix | `graph_`, `execution_trace_`, `operation_list_` |
| **Function Parameters** | `lower_case` | `value`, `operation_type`, `is_constant` |
| **Local Variables** | `lower_case` | `result`, `temp_block`, `offset` |
| **Constants** | `UPPER_CASE` | `MAX_OPERATIONS`, `DEFAULT_BUFFER_SIZE` |
| **Macros** | `UPPER_CASE` | `NAUTILUS_INLINE`, `NAUTILUS_EXPORT` |
| **Namespaces** | `lower_case` | `nautilus`, `nautilus::compiler`, `nautilus::tracing` |
| **Typedefs** | `lower_case` with `_t` suffix | `operation_ref_t`, `block_id_t` |
| **Enums** | `CamelCase` | `BackendType`, `OptimizationLevel` |

### Code Style Rules

**Formatting** (enforced by `.clang-format`):
- **Indentation**: Tabs (width = 4 spaces)
- **Line Length**: 120 characters maximum
- **Newlines**: Unix-style LF
- **Trailing Newline**: Mandatory for all files
- **Brace Style**: LLVM style (opening brace on same line)

**Example:**

```cpp
// CORRECT
class IRGraph {
	std::vector<Operation*> operations_;

	void AddOperation(OperationType type, const std::string& name) {
		// Implementation
	}
};

// INCORRECT - wrong naming, long line, spacing issues
class irGraph {
	std::vector<Operation*> op_list;  // should be operations_

	void add_operation(OperationType type, const std::string& name) {
		// Implementation (this line is too long and violates the 120 char limit in this context)
	}
};
```

### C++ Standards

- **Minimum**: C++20 (specified in `CMakeLists.txt`)
- **Compiler Support**: GCC 10+, Clang 15+
- **Modern Features**: Use C++20 concepts, ranges, and other modern features
- **Template Code**: Extensively used for type-safe tracing

### Header Organization

1. Include guards or `#pragma once`
2. System headers (`<vector>`, `<string>`)
3. Third-party headers (fmt, spdlog)
4. Local headers (relative includes)
5. Code

```cpp
#pragma once

#include <vector>
#include <memory>
#include <spdlog/spdlog.h>

#include "nautilus/core.hpp"
#include "nautilus/exceptions/NautilusException.hpp"

namespace nautilus::compiler {

class IRGraph {
	// Implementation
};

} // namespace nautilus::compiler
```

### Comments & Documentation

- **When to comment**: Complex algorithms, non-obvious design decisions, workarounds
- **Avoid**: Comments explaining obvious code (e.g., `i++; // increment i`)
- **Style**: `//` for single-line comments, `/* */` for multi-line
- **Documentation**: Use structured comments for public APIs in headers

### Error Handling

- Use **exceptions** for error conditions
- Custom exception types in `src/nautilus/exceptions/`
- Exceptions for tracing failures and compilation errors
- Return codes only for recoverable conditions in public APIs

---

## Development Workflows

### Building the Project

**Default Compiler:**
- **Default Compiler**: Clang 21
- All development should use Clang 21 unless explicitly building for compatibility testing
- To set Clang 21 explicitly: `cmake -DCMAKE_CXX_COMPILER=clang++-21 ..`

**Standard Build:**
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target nautilus
```

**With Custom Options:**
```bash
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_LOGGING=ON \
      -DENABLE_STACKTRACE=ON \
      -DENABLE_TESTS=ON \
      -DENABLE_BENCHMARKS=OFF \
      ..
```

### CMake Build Options

| Option | Default | Purpose |
|--------|---------|---------|
| `CMAKE_BUILD_TYPE` | Debug | Release or Debug build |
| `ENABLE_LOGGING` | ON | Enable spdlog-based logging |
| `ENABLE_STACKTRACE` | ON | Enable stack trace capture (backward-cpp) |
| `ENABLE_TESTS` | ON | Build test suite |
| `ENABLE_BENCHMARKS` | OFF | Build benchmarks |
| `ENABLE_COMPILER` | ON | Enable compilation pipeline |
| `ENABLE_TRACING` | ON* | Enable tracing (depends on ENABLE_COMPILER) |
| `ENABLE_MLIR_BACKEND` | ON | Enable MLIR code generation |
| `ENABLE_C_BACKEND` | ON | Enable C++ code generation |
| `ENABLE_BC_BACKEND` | ON | Enable bytecode backend |
| `ENABLE_ASMJIT_BACKEND` | ON | Enable AsmJit backend |

### Code Formatting

**IMPORTANT: All changes MUST be formatted using the CMake format target before committing.**

**Using CMake Format Target (Preferred):**
```bash
cd build
cmake --build . --target format
```

This is the authoritative formatting method and must be run for all code changes.

**Check Formatting (Manual):**
```bash
./format.sh
```

**Fix Formatting Issues (Manual):**
```bash
./format.sh -i
```

**Manual with clang-format-21:**
```bash
clang-format-21 -i src/file.cpp
```

**Before Committing:**
1. Run the CMake format target: `cmake --build . --target format`
2. Verify with `./format.sh` (no errors should be reported)
3. Commit only after formatting is complete

### Running Tests

**All Tests:**
```bash
cd build
ctest --test-dir nautilus --output-on-failure
```

**Specific Test Category:**
```bash
ctest --test-dir nautilus -R "ControlFlow" --output-on-failure
```

**With Verbose Output:**
```bash
ctest --test-dir nautilus --output-on-failure -V
```

**Individual Test Executable:**
```bash
./nautilus/test/execution-tests/ExecutionTest
```

### Git Workflow for AI Assistants

**Branch Naming Convention:**
- Feature branches: `claude/<feature-description>-<session-id>`
- Bugfix branches: `claude/fix-<issue-description>-<session-id>`
- Documentation: `claude/docs-<topic>-<session-id>`

**Typical Workflow:**
```bash
# Fetch the assigned branch
git fetch origin claude/add-claude-documentation-EzdpT

# Create/checkout local branch
git checkout -b claude/add-claude-documentation-EzdpT

# Make changes
# ...

# Commit (clear, descriptive messages)
git commit -m "Add CLAUDE.md with comprehensive AI assistant guide"

# Push to assigned branch
git push -u origin claude/add-claude-documentation-EzdpT
```

**Commit Message Guidelines:**
- **Format**: Concise, imperative mood
- **Examples**:
  - "Add CLAUDE.md documentation for AI assistants"
  - "Fix constant propagation phase to handle null pointers"
  - "Optimize MLIR backend for integer arithmetic"
  - "Add regression test for issue #42"

---

## Testing & CI/CD

### Test Organization

Nautilus has a comprehensive testing infrastructure with 90+ test files:

1. **Execution Tests** (Runtime behavior):
   - Control flow (if/else, loops)
   - Expressions and operators
   - Boolean operations
   - Type casting
   - Pointer operations
   - Standard library wrappers
   - Runtime calls

2. **Value Type Tests** (val<T> template):
   - Integer types
   - Float types
   - Pointer types
   - Function registration

3. **LLVM IR Tests** (Intermediate representation):
   - Compare generated IR against reference
   - Requires llvm-diff-21+ for comparison
   - Validates backend code generation

4. **Benchmarks** (Performance):
   - Execution performance
   - Tracing overhead
   - Catch2 benchmark format for CI/CD

### Test Data Structure

Tests use a structured data format with three representations:

```
test-category/
├── tracing/           # Direct execution trace (raw trace output)
├── ir/                # Generated IR representation (control flow graph)
└── after_ssa/         # Post-SSA transformation (optimized IR)
```

### CI/CD Pipeline (.github/workflows/)

**PR/Push Workflow** (`pr.yml`):
- **Format Check**: clang-format-18, newline validation
- **Build Matrix**:
  - GCC 10-14, Clang 15-19
  - Ubuntu 22.04 and 24.04
  - macOS 15
  - ARM builds (Ubuntu 24.04-ARM)
  - Address/Thread sanitizers
- **LLVM IR Test**: llvm-diff-21 validation
- **Caching**: MLIR binaries, CMake cache, ccache

**Benchmark Workflow** (`benchmark.yml`):
- Runs on main branch commits
- Clang, Release build with optimizations
- 200% regression alert threshold
- Results published to GitHub Pages

### Running Tests Locally Before Commit

```bash
# Format check
./format.sh

# Build and run all tests
cd build
cmake --build . --target nautilus
ctest --test-dir nautilus --output-on-failure
```

---

## Key Systems Explained

### 1. The Value Tracing System (val<T>)

**What it does:**
- Wraps values to track operations during execution
- Maintains both runtime value and tracing state
- Specializations for different types (arithmetic, pointers, enums)

**How it works:**
```cpp
// User code
val<int64_t> x = 5;
val<int64_t> y = 10;
auto result = x + y;  // Operation is traced

// Internally
// 1. Addition operation is recorded
// 2. Execution trace captures: operands, operation type, result
// 3. val<T> stores both traced_value (tracing info) and actual value (10)
```

**Key Files:**
- `include/nautilus/val.hpp` - Main header, orchestrates specializations and operators
- `include/nautilus/val_arith.hpp` - Generic partial specialization for arithmetic types
- `include/nautilus/val_bool.hpp` - Explicit specialization for boolean values with probability tracking
- `include/nautilus/val_details.hpp` - Implementation utilities (RawValueResolver, StateResolver)
- `include/nautilus/val_ptr.hpp` - Pointer specialization
- `include/nautilus/val_enum.hpp` - Enumeration specialization

**When modifying:**
- Changes to val_arith.hpp affect all arithmetic type operations
- Changes to val_bool.hpp affect only boolean tracing and probability tracking
- Changes to val_details.hpp affect all specializations (it's a critical utility header)
- Template specialization allows type-specific behavior
- Test changes thoroughly with value type tests

### 2. Intermediate Representation (IR) System

**What it does:**
- Represents program as control flow graph (CFG)
- Uses Static Single Assignment (SSA) form
- Foundation for optimizations and code generation

**Structure:**
```
IRGraph
├── BasicBlock 1
│   ├── Operation (Add)
│   ├── Operation (Constant)
│   └── Operation (Branch)
│
├── BasicBlock 2
│   ├── Operation (Load)
│   └── Operation (Return)
│
└── BasicBlock 3 [unreachable]
```

**Key Files:**
- `src/nautilus/compiler/ir/IRGraph.hpp` - Core structure
- `src/nautilus/compiler/ir/operations/` - 85+ operation types
- `src/nautilus/compiler/ir/phases/` - Optimization passes

**When working with IR:**
- All operations are immutable after creation
- Operations reference other operations by pointer
- SSA property: each variable assigned once
- Optimization passes transform the graph

### 3. Optimization Phases

**Available Phases** (in `src/nautilus/compiler/ir/phases/`):
1. **SSA Creation** - Convert execution trace to SSA form
2. **Constant Propagation** - Compute constants at compile time
3. **Dead Code Elimination** - Remove unused operations
4. **Loop Analysis** - Identify and analyze loops
5. **Control Flow Analysis** - Simplify branch structure
6. **DataFlow Analysis** - Track data dependencies
7. **Memory Optimization** - Optimize load/store operations

**Phase Application Order:**
1. SSA creation (required first)
2. Constant propagation
3. Dead code elimination
4. Loop analysis
5. Control flow analysis
6. Backend-specific optimizations

**Key Principle:** Phases must be **idempotent** and **composable** (order shouldn't break correctness).

### 4. Code Generation Backends

**MLIR Backend** (Primary):
- Converts IR → MLIR → LLVM IR → Native Code
- Location: `src/nautilus/compiler/backends/mlir/`
- Advantages: Multiple optimization passes, mature infrastructure
- Produces most efficient code

**C++ Backend:**
- Generates compilable C++ source code
- Location: `src/nautilus/compiler/backends/cpp/`
- Use case: Debugging, reference implementation

**Bytecode Backend:**
- Interprets bytecode at runtime
- Location: `src/nautilus/compiler/backends/bc/`
- Use case: Portability, fast interpretation

**AsmJit Backend:**
- Direct x86-64 assembly generation
- Location: `src/nautilus/compiler/backends/asmjit/`
- Status: Experimental

### 5. Tracing System

**How it captures operations:**
1. User executes function with `val<T>` wrapped parameters
2. Each operation records itself in execution trace
3. Trace captures: operation type, operands, results
4. After execution, trace is converted to IR

**Key Components:**
- `ExecutionContext` - Tracks current execution state
- `ExecutionTrace` - Records all operations
- `Tag` system - Symbolic execution support
- `SymbolicExecution` - Supports dynamic tracing

**Key Files:**
- `src/nautilus/tracing/ExecutionContext.hpp`
- `src/nautilus/tracing/ExecutionTrace.hpp`
- `src/nautilus/tracing/phases/`

### 6. The Engine API

**Central orchestrator** that manages the entire pipeline:

```cpp
// Typical usage
Engine engine;
auto executable = engine.Compile<int64_t(val<int64_t>)>(
	[](val<int64_t> x) {
		return x + 10;
	}
);
int64_t result = executable(5);  // Returns 15
```

**Configuration:**
- Backend selection (MLIR, C++, Bytecode, AsmJit)
- Optimization level
- Debug output (dump IR, LLVM, etc.)
- Normalization options

**Key File:** `include/nautilus/Engine.hpp`

---

## Common Development Patterns

### Adding a New Operation Type

**When:** Implementing new language feature or optimization

**Steps:**

1. **Define Operation Class** (in `src/nautilus/compiler/ir/operations/`):
```cpp
// MyNewOperation.hpp
namespace nautilus::compiler::ir {

class MyNewOperation : public Operation {
public:
	static constexpr auto type = OperationType::MyNew;

	MyNewOperation(Operation* operand)
		: Operation(type),
		  operand_(operand) {}

	Operation* GetOperand() const { return operand_; }

private:
	Operation* operand_;
};

} // namespace nautilus::compiler::ir
```

2. **Register in OperationType enum** (Operations.hpp)

3. **Add to operation factory** if needed

4. **Create tests** in `test/execution-tests/` and `test/data/`

5. **Add backend support** for each active backend

### Adding an Optimization Phase

**When:** Implementing code improvement or transformation

**Steps:**

1. **Create Phase Class** (in `src/nautilus/compiler/ir/phases/`):
```cpp
class MyOptimizationPhase : public Phase {
public:
	std::string GetName() const override {
		return "My Optimization Phase";
	}

	bool Execute(IR::Graph& graph) override {
		// Transform graph
		// Return true if modified
		return false;
	}
};
```

2. **Register phase** in compilation pipeline

3. **Add tests** with input/output IR examples

4. **Document** optimization benefits and constraints

### Adding Standard Library Support

**When:** Wrapping C++ standard library functions

**Steps:**

1. **Create wrapper** in `include/nautilus/std/` (e.g., `String.hpp`)

2. **Implement tracing support** using `val<T>` template specialization

3. **Add tests** in `test/execution-tests/std/`

4. **Update documentation** in `docs/options.md`

### Debugging a Failed Test

**Typical workflow:**

```bash
# 1. Run specific test with verbose output
cd build
ctest --test-dir nautilus -R "TestName" --output-on-failure -V

# 2. Run test directly for more info
../nautilus/test/execution-tests/ExecutionTest

# 3. Enable debug logging via CMake
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_LOGGING=ON \
      ..

# 4. Check test data in nautilus/test/data/<category>/

# 5. Compare generated IR:
# - Look in nautilus/test/data/
# - Compare tracing/, ir/, after_ssa/ outputs
```

---

## Debugging & Troubleshooting

### Common Issues & Solutions

**Issue: Build fails with "MLIR not found"**
- Solution: MLIR will auto-download on first build
- Allow time for large download (~500MB)
- Check CMake cache: `rm -rf build && mkdir build && cd build && cmake ..`

**Issue: Tests timeout or crash**
- Solutions:
  - Check stack traces: `ENABLE_STACKTRACE=ON`
  - Run with sanitizers: `ENABLE_ADDRESS_SANITIZER=ON`
  - Reduce test dataset temporarily
  - Check for infinite loops in IR

**Issue: Format check fails in CI/CD**
- Solution: Run `./format.sh -i` before committing

**Issue: LLVM IR validation fails**
- Solution: Update reference IR in `test/llvm-ir-test/reference-ir/`
- Verify changes are intentional
- Document why reference changed

### Debugging Tools

**Logging:**
```cpp
#include <spdlog/spdlog.h>

spdlog::info("Operation created: {}", operation->GetName());
spdlog::debug("Graph has {} blocks", graph.GetBasicBlocks().size());
```

**IR Visualization:**
- GraphViz output available via engine options
- Shows control flow and data flow relationships
- Enable via: `dump.ir = true`

**Trace Inspection:**
```cpp
auto trace = engine.GetExecutionTrace();
for (auto& op : trace.GetOperations()) {
	spdlog::info("Op: {}, Type: {}", op->GetName(), op->GetType());
}
```

**LLVM IR Dump:**
- Enable: `dump.llvm_ir = true`
- Outputs generated LLVM IR for inspection
- Compare with reference using llvm-diff

### Performance Profiling

**Compile-Time Profiling:**
```bash
cmake --build . --target nautilus -- VERBOSE=1 2>&1 | grep time
```

**Runtime Profiling:**
- Build with `-DENABLE_BENCHMARKS=ON`
- Run benchmarks in `test/benchmark/`
- Results in Catch2 format

---

## Performance Considerations

### Design Patterns for Efficiency

1. **Avoid Allocations in Hot Paths:**
   - Pre-allocate containers when size is known
   - Use references instead of copies where possible

2. **Operation Caching:**
   - Many operations are reused
   - Check for existing operations before creating new ones

3. **IR Graph Traversal:**
   - Iterate once when possible
   - Maintain indexes for quick lookups

### Backend Selection Guide

| Scenario | Recommended Backend | Reason |
|----------|-------------------|--------|
| Production query compilation | MLIR | Best performance, mature |
| Debugging | C++ | Human-readable, easy to inspect |
| Quick prototyping | Bytecode | Fast compilation, no native code generation |
| Embedded systems | Bytecode | Minimal dependencies |
| Research/Innovation | Custom | Implement new backend |

### Optimization Order Matters

```
SSA Creation
    ↓
Constant Propagation (catch easy wins)
    ↓
Dead Code Elimination (remove replaced constants)
    ↓
Loop Analysis (identify optimization opportunities)
    ↓
Control Flow Analysis (simplify branches)
    ↓
Backend-Specific Optimizations
```

### Monitoring Compilation Time

Enable timing via engine options:
```cpp
engine.SetOption("profile.compilation", true);
// Execution shows timing for each phase
```

---

## Additional Resources

### Key Academic References
- Nautilus paper: SIGMOD 2024 publication
- Contact: Refer to README.md for research group information

### Important Files Summary

**Must Read:**
- `include/nautilus/Engine.hpp` - API contract
- `src/nautilus/compiler/ir/IRGraph.hpp` - Core data structure
- `include/nautilus/val.hpp` - Type system foundation

**Reference for Examples:**
- `example/src/DemoJit.cpp` - Minimal working example
- `test/execution-tests/ExecutionTest.cpp` - Test patterns
- `test/data/*/` - Expected behavior examples

### Getting Help

**For Codebase Questions:**
1. Check `README.md` first
2. Review documented examples in `example/`
3. Inspect test data in `test/data/`
4. Look at similar implementations in codebase

**For Documentation Issues:**
- This file: `CLAUDE.md`
- Technical docs: `docs/graphs.md`, `docs/options.md`
- Comments in headers: Public API documentation

---

## AI Assistant Guidelines

### Before Making Changes

1. **Read relevant files first** - Never propose changes without understanding context
2. **Check patterns** - Follow existing code patterns in the codebase
3. **Understand tradeoffs** - Consider performance and maintainability implications
4. **Run tests** - All changes should pass existing tests
5. **Use Clang 21** - Ensure the default compiler (Clang 21) is used for all builds
6. **Format with CMake target** - Run `cmake --build . --target format` on all code changes before committing

### When Implementing Features

1. **Start small** - Implement minimal working version first
2. **Test incrementally** - Add tests as you implement
3. **Document patterns** - Add comments explaining non-obvious decisions
4. **Commit clearly** - One logical change per commit

### Code Review Checklist

- [ ] Follows naming conventions (CamelCase, lower_case, etc.)
- [ ] **Code formatted using CMake target** (`cmake --build . --target format`)
- [ ] Verified with `./format.sh` (no formatting errors)
- [ ] Respects 120-character line limit
- [ ] Uses tabs (not spaces) for indentation
- [ ] All tests pass locally
- [ ] No obvious performance regressions
- [ ] Comments for complex logic only
- [ ] Commits have clear, imperative messages
- [ ] Built and tested with Clang 21 (default compiler)

---

**Document Version:** 1.1
**Last Updated:** 2025-12-25
**For:** AI Assistant Development Workflow in Nautilus Repository
