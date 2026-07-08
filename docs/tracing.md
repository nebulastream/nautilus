# Tracing in Nautilus

## Overview

Nautilus uses a tracing-based approach to JIT compilation. Instead of parsing source code or analyzing an abstract syntax tree, Nautilus executes user functions once with special `val<T>` wrapper types that record every operation performed during that execution. The resulting execution trace -- a complete record of all arithmetic, comparison, memory, and control-flow operations -- is then converted to an intermediate representation (IR), optimized through several passes, and compiled to native machine code.

This approach has a key advantage: users write ordinary C++ functions. There is no separate language, no special syntax, and no external compiler toolchain to invoke at build time. The same function can run interpreted (for debugging) or compiled (for performance), with no changes to the source code.

## How Tracing Works

Tracing proceeds through the following steps:

1. **The user writes a function using `val<T>` types.** Instead of `int64_t x`, the function parameter is `val<int64_t> x`. The `val<T>` template wraps the underlying value and overloads all relevant operators.

2. **The user registers the function with the engine.** When `engine.registerFunction()` is called, Nautilus prepares to trace the function by creating symbolic arguments -- `val<T>` instances that carry tracing state rather than meaningful runtime values.

3. **Nautilus executes the function once with symbolic arguments.** Each `val<T>` operation (addition, comparison, memory load, etc.) records itself in a global execution trace managed by the `ExecutionContext`. The function runs to completion, producing a linear sequence of recorded operations.

4. **Control flow is traced by exploring all branches.** When a `val<bool>` is converted to `bool` inside an `if` statement, the tracer records the branch and uses symbolic execution to explore both the true and false paths. Loops are detected by identifying repeated states (snapshot hashes) in the trace.

5. **The complete trace represents the full behavior of the function.** After tracing finishes, the execution trace contains every operation, every branch, and every loop that the function can perform. This trace is the input to the compilation pipeline.

## The Compilation Pipeline

The trace passes through several transformation stages before becoming executable code:

```
User Function (C++ with val<T>)
    |
Tracing (symbolic execution, records all operations)
    |
Execution Trace (linear sequence of recorded operations)
    |
SSA Creation (Static Single Assignment form)
    |
IR Generation (Control Flow Graph with basic blocks)
    |
Optimization Phases
    |-- Constant Propagation
    |-- Dead Code Elimination
    |-- Loop Analysis
    \-- Control Flow Analysis
    |
Backend Code Generation
    |-- MLIR -> LLVM -> Native Code
    |-- C++ Source Code
    \-- Bytecode Interpreter
    |
Executable Function
```

**Tracing** executes the user function with symbolic `val<T>` arguments and captures every operation.

**SSA Creation** converts the flat execution trace into Static Single Assignment form, where each variable is assigned exactly once. This representation makes subsequent analysis and optimization straightforward.

**IR Generation** builds a control flow graph (CFG) from the SSA trace. The IR consists of basic blocks connected by branch and jump edges. Each block contains a sequence of IR operations (arithmetic, comparisons, loads, stores, function calls).

**Optimization Phases** transform the IR to produce more efficient code. Constant propagation evaluates operations on known constants at compile time. Dead code elimination removes operations whose results are never used. Loop analysis identifies loop structures and their properties. Control flow analysis simplifies branch structures and removes unreachable blocks.

**Backend Code Generation** converts the optimized IR into executable form. The MLIR backend (the primary backend) lowers the IR through MLIR dialects to LLVM IR, then compiles to native machine code. The C++ backend emits compilable C++ source code, which is useful for debugging. The bytecode backend produces bytecode for an interpreter, which has minimal dependencies and fast compilation.

## Dual-Mode Execution

Every `val<T>` type supports two modes of operation, controlled by the `engine.Compilation` option:

**Tracing mode** (default, when compilation is enabled): The first time a registered function is called, Nautilus traces it, compiles the trace, and caches the resulting executable. All subsequent calls execute the compiled native code directly, with no tracing overhead.

**Direct mode** (when compilation is disabled): `val<T>` acts as a thin wrapper around the raw value. No tracing occurs, and the function executes as ordinary C++ code. This mode is useful for debugging, since you can set breakpoints, step through the function, and inspect values normally.

```cpp
// Tracing mode (default):
// 1. First call: function is traced, compiled, and cached
// 2. Subsequent calls: execute compiled native code
engine::NautilusEngine engine;
auto f = engine.registerFunction(myFunc);
f(42);  // Traces, compiles, then executes
f(99);  // Executes compiled code directly

// Direct mode:
engine::Options options;
options.setOption("engine.Compilation", false);
engine::NautilusEngine directEngine(options);
auto g = directEngine.registerFunction(myFunc);
g(42);  // Executes myFunc directly as C++ -- no tracing, no compilation
```

## The val&lt;T&gt; Object in Tracing Context

A `val<T>` object stores two pieces of information:

- **`value`** (type `T`): the actual runtime value. This field is always present, in both tracing and direct mode. It ensures that the function produces a correct result even during the tracing execution.

- **`state`** (type `tracing::TypedValueRefHolder`): a reference into the execution trace. This field is only present when the `ENABLE_TRACING` preprocessor macro is defined. It identifies this value's position in the trace graph.

When an operation like `x + y` is performed on two `val<T>` objects, two things happen simultaneously:

1. The runtime values are added, producing the correct numeric result.
2. The tracer records an ADD operation that references the trace states of `x` and `y`, producing a new trace state for the result.

This dual tracking means the function always computes the right answer (through the `value` fields), while the tracer builds a complete record of the computation (through the `state` fields). After tracing completes, the runtime values are discarded -- only the trace matters for compilation.

## static_val&lt;T&gt; in Tracing

`static_val<T>` is a special type for loop induction variables that should be unrolled at trace time rather than compiled as runtime loops. It contrasts with `val<T>` in a fundamental way:

- **`val<T>`** creates trace entries for its operations. A loop controlled by `val<T>` produces a loop in the compiled code.
- **`static_val<T>`** does NOT create trace entries for its own value. Instead, it modifies the trace snapshot hash -- the fingerprint the tracer uses to detect repeated states. Each iteration produces a distinct hash, so the tracer never detects a cycle and never identifies a loop. The result is that the loop body is unrolled into straight-line code in the IR.

The mechanism works through the constructor and destructor of `static_val<T>`. The constructor calls `tracing::pushStaticVal()`, which registers the variable's memory address on a tracing stack. The destructor calls `tracing::popStaticVal()`. While a `static_val` is on the stack, its current value is included in every snapshot hash computation, making each iteration appear as a unique program state.

```cpp
// Loop with val<T> -- compiles to a loop in the generated code:
val<int32_t> sum = 0;
for (val<int32_t> i = 0; i < count; i = i + 1) {
    sum = sum + data[i];
}

// Loop with static_val<T> -- fully unrolled at trace time:
val<int32_t> sum = 0;
for (static_val<int32_t> i = 0; i < 4; i++) {
    sum = sum + data[i];  // Produces 4 separate add operations in the IR
}
```

The iteration count of a `static_val` loop must be determinable at trace time, since the tracer must be able to execute every iteration during the single tracing pass.

Nautilus also provides `static_iterable` and `static_enumerable` wrappers for unrolling range-based for loops over containers:

```cpp
int32_t coefficients[] = {1, 2, 3, 4};

// Fully unrolled -- each iteration becomes straight-line code:
for (auto& coeff : static_iterable(coefficients)) {
    sum = sum + val<int32_t>(coeff);
}
```

## Branch Probability Hints

`val<bool>` supports probability hints that inform the compiler about expected branch behavior. When a `val<bool>` is converted to `bool` for use in a conditional, the probability is passed to the tracer via `traceBool()`, and the backend can use it for branch prediction optimization in the generated code.

```cpp
val<bool> condition = (x > threshold);
condition.setIsTrueProbability(0.95);  // Tell compiler: 95% likely true
if (condition) {
    // Hot path -- compiler optimizes layout for this case
} else {
    // Cold path
}
```

The probability is a `double` in the range [0.0, 1.0], where 0.5 (the default) indicates no prediction. Values closer to 1.0 tell the compiler the true branch is more likely; values closer to 0.0 indicate the false branch is more likely.

| Probability | Meaning |
|-------------|---------|
| 0.0 - 0.2 | Very unlikely to be true (error paths, exceptional cases) |
| 0.2 - 0.4 | Unlikely to be true |
| 0.4 - 0.6 | Unknown / no hint (default is 0.5) |
| 0.6 - 0.8 | Likely to be true |
| 0.8 - 1.0 | Very likely to be true (normal execution paths) |

Setting accurate probabilities can improve generated code quality by reducing branch misprediction penalties and enabling better instruction cache utilization. Inaccurate probabilities may produce suboptimal branch ordering.

## Debugging the Trace

Nautilus provides extensive dump options for inspecting intermediate results at each stage of the compilation pipeline. These are set through `engine::Options` before creating the engine.

### Enabling Dump Output

```cpp
engine::Options options;

// Output destination:
options.setOption("dump.console", true);  // Print to console (default: false)
options.setOption("dump.file", true);     // Write to temp folder (default: true)

// Individual compilation stages:
options.setOption("dump.after_tracing", true);         // Raw execution trace
options.setOption("dump.after_ssa", true);              // After SSA transformation
options.setOption("dump.after_ir_creation", true);      // Nautilus IR (control flow graph)
options.setOption("dump.after_mlir_generation", true);  // MLIR output (MLIR backend only)
options.setOption("dump.after_llvm_generation", true);  // LLVM IR output (MLIR backend only)
options.setOption("dump.after_cpp_generation", true);   // Generated C++ (C++ backend only)
options.setOption("dump.after_bc_generation", true);    // Generated bytecode (BC backend only)

// Or dump everything at once:
options.setOption("dump.all", true);
```

### Graph Visualization

Nautilus can generate visual representations of the IR as either Graphviz or Mermaid diagrams:

```cpp
options.setOption("dump.graph", true);
options.setOption("dump.graph.type", "mermaid");   // Or "graphviz" (default)
options.setOption("dump.graph.full", true);         // Include data flow edges (default: false)
```

When `dump.graph.full` is false, only the control-flow graph is shown (block-to-block edges). When true, the graph includes all IR nodes and their data dependencies within each block.

See [graphs.md](graphs.md) for visualization examples.

## Tracer Implementations (engine.traceMode)

Nautilus ships four tracer implementations selected via `engine.traceMode`. They all
produce byte-identical execution traces and differ in how they explore the paths
through a function.

### Re-executing tracers: lazyTracing (default) and exceptionBasedTracing

The classic tracers explore one control-flow path per execution of the traced
function. When a path terminates, the function is re-executed from the beginning and
the shared decision prefix is replayed (FOLLOW mode) until the next unexplored branch
is reached. Total tracing work is therefore O(paths x path length): for a function
with a chain of N independent branches, the prefix replay alone costs O(N^2)
operations. The two variants differ only in how a finished path stops the traced
function: `exceptionBasedTracing` throws a `TraceTerminationException`,
`lazyTracing` enters a passive mode and lets the function run to its natural return.

### Once-per-path tracers: stackCopyTracing and forkTracing

The snapshot tracers eliminate the prefix replay entirely: at every freshly recorded
branch, the current execution state is snapshotted; when a path terminates, the
oldest pending snapshot is restored and execution resumes directly inside the branch,
taking the false side. Every branch suffix executes exactly once, so total tracing
work is O(size of the execution tree) - asymptotically optimal for trace-all-paths
exploration. Pending branches are resumed in the same FIFO order the re-executing
tracers use, which keeps the produced traces byte-identical across all four modes.

**stackCopyTracing** runs the traced function on a dedicated side stack
(`engine.traceStackSize`). A snapshot is a memcpy of the live stack region plus the
small mutable tracer state (static-variable stack, alive-variable hash, trace
position) - roughly a microsecond per branch. This is the fastest tracer and the
recommended mode for tracing throughput. Constraint: restoring stack bytes
resurrects frames whose destructors already ran on a previously completed path. That
is safe for `val<T>` / `static_val` objects (their destructors only touch tracer
state that the snapshot restores), but plain C++ locals that own heap memory (e.g. a
`std::vector`) must not live across a traced branch, or their destructors run once
per explored path. Under AddressSanitizer, stack restores are incompatible with
use-after-return detection; set `ASAN_OPTIONS=detect_stack_use_after_return=0`.

**forkTracing** snapshots the whole process with `fork()` instead: the forked child
*is* the suspended false-branch continuation - its copied address space already
holds the correct stack, heap and tracer state. On resume, a continuation receives
only the trace mutations it missed since its fork: the trace journals its writes
under a monotonically increasing epoch (dirty blocks, tag-map writes), each
continuation remembers its fork epoch, and the handoff ships exactly the newer
suffix. The payload is staged in an anonymous `MAP_SHARED` region inherited by the
whole worker tree, so the bytes are written once and read in place - the socket
message carries only descriptors (`SCM_RIGHTS`) and the payload size, acting as the
synchronization fence. The delta helps most when forks are recent relative to the
trace (straight-line-heavy functions); on long branch chains the FIFO resume order
and the block relocations performed by control-flow merges keep the shipped deltas
close to the full trace, so the per-branch cost still grows with trace size there.
Considerably slower per branch than a stack copy in every case, but fully isolates
the traced code: destructors run exactly once per process, so code that is unsafe
under stackCopyTracing works here. Constraints: POSIX only; tracing must run
single-threaded (forking with concurrently allocating threads risks deadlocking the
children); nested `NautilusFunction` objects must outlive tracing (declare them
static - objects constructed inside the traced function only exist in one tracing
process and are rejected with a descriptive error). Note that the default tiered
engine promotes modules to the optimized backend on a background thread, so a
promotion of a previously compiled module may still be running when a later module
is traced. The `engine.forkTraceTimeoutMs` watchdog catches a wedged worker tree,
but for strictly single-threaded tracing combine forkTracing with
`engine.tiered.backgroundPromotion=false` or a pinned `engine.backend`.

Rule of thumb: use `stackCopyTracing` for maximum tracing performance with ordinary
`val<T>`-based code, `forkTracing` when traced code keeps heap-owning C++ objects
alive across branches, and the re-executing tracers as the conservative default.

### Constant-branch pruning (engine.tracePruning)

Independently of the trace mode, `engine.tracePruning` skips branches that are
statically dead: when the condition of an `if` is a trace-time-constant boolean
(a `val<bool>` literal or a copy/assignment chain of one), the tracer takes the only
feasible side without recording a CMP and never explores the other side - exactly as
if the condition had been a plain C++ `bool`. This saves one full path exploration
per constant branch. Branch probability hints are never used for pruning; they are
hints for backend code layout, not guarantees. Constant-true loops whose body
contains traced operations still form proper loops through the regular control-flow
merge; constant loops without any traced exit are detected and reported as errors.

## Limitations and Considerations

**Functions must be deterministic with respect to control flow.** The tracer executes the function exactly once with symbolic arguments. All branches must be reachable through this symbolic execution. If a branch depends on external state (e.g., the current time), the tracer will only see the path taken during the tracing run.

**Side effects occur during tracing.** Any side effects in the traced function -- printing to the console, writing to files, modifying global state -- will happen during the tracing execution. For external function calls that should happen at runtime (not trace time), use `invoke()` to register them as opaque runtime calls in the trace.

**`val<T>` values should not escape the traced function scope.** The tracing state references are valid only within the tracing context. Storing a `val<T>` in a global variable or returning it outside the traced function leads to undefined behavior.

**Only overloaded operations are traced.** The tracing system relies on C++ operator overloading. Operations that `val<T>` does not overload (such as certain standard library functions) will not appear in the trace. Use the Nautilus standard library wrappers in `nautilus::std` or `invoke()` for operations that need to be captured.

**`static_val` loops must have deterministic iteration counts.** Since `static_val` causes full unrolling at trace time, the loop bound must be known when the function is traced. A `static_val` loop that depends on a `val<T>` for its termination condition cannot be unrolled and will cause incorrect tracing behavior.
