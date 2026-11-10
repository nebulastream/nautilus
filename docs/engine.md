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

### Registration Source Location

`registerFunction` (and `NautilusFunction`, `NautilusModule::registerFunction`) captures the call site it was written at via a defaulted `std::source_location` parameter -- no cost to the caller, no debug info required. It is printed on the compiled function's signature line in the IR dump (`docs/graphs.md`), under the `log::options::setLogSourceLocations` flag, and named in diagnostics raised while tracing the function (e.g. a rejected `region()` body):

```
execute($1:i64) :i64  ; at src/Query.cpp:42:1 {
```

This is where the function was *registered*, not necessarily where its body is *defined*. For `NautilusFunction{"f", fBody}` the two usually coincide; for `engine.registerFunction(myKernel)` the location is the call site in setup code, which may be far from `myKernel`'s own definition. Symbolising the callee's address to recover the true definition site would need debug info and a resolver lookup; that is a separate, opt-in enrichment (see `dump.sourceLocations` and `SourceLocationResolver`), not what this parameter provides.

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

### Threaded bytecode (`tbc`)

The `tbc` backend interprets threaded bytecode at runtime (the former `bc` backend was removed in 0.4). It has the fastest compilation time and does not require native code generation, making it portable across platforms.

```cpp
options.setOption("engine.backend", "tbc");
```

## Disabling Compilation

For debugging purposes, you can disable compilation entirely. When compilation is disabled, registered functions execute directly through the `val<T>` wrappers without tracing or code generation. This is useful for verifying that a function produces correct results before investigating compilation issues.

```cpp
options.setOption("engine.Compilation", false);
```

## Tiered Compilation and Promotion

Without `engine.backend`, the engine compiles in two tiers. `compile()` returns a module that runs on a fast **tier 0**
executable (`asmjit`, else `tbc`, else the interpreter), and the **tier 1** executable (`mlir`) is compiled in the
background and swapped in; the `ModuleFunction` handles you hold pick it up on their next call. A call that is running
when the swap happens finishes in the tier it started in, and a superseded executable lives as long as the handles that
resolved it. `engine.backend=<x>` pins one backend and compiles single-tier, synchronously.

The tiers are set with `engine.tier0.backend` and `engine.tier1.backend`, each alone or both; a backend that is not built
fails when the engine is created.

### The promotion service

Each engine owns a **promotion service**: a pool of `engine.tiered.promotionThreads` worker threads (default a quarter
of the hardware threads, at least one) that compile the tier 1 of all of the engine's modules. The workers are created
with the first promotion, run at a lower OS priority than your threads, and live as long as the engine, so the number
of threads stays the same however many modules you compile, and the state a backend keeps per thread (the MLIR context
and target machines) is reused by every promotion after a worker's first. `engine.tiered.maxQueuedPromotions` caps how
many promotions may wait: a module beyond the cap stays on tier 0 with the status `skipped`.

A promotion whose module is dropped is cancelled: it is dropped when a worker dequeues it, and a running one ends at the
next stage boundary of the backend (after the MLIR generation, after the LLVM optimization), installing nothing.
Destroying the engine cancels the queued promotions and waits only for the compiles that are running, at most one per
worker. No lock is held while waiting, so `compile()` on other threads is never held up by a wait.

With the default policy the promotion is queued right after the trace was converted, **before** the tier-0 code
generation, so a tier 1 that finishes first is installed and the late tier-0 executable is dropped.

### Policy

`engine.tiered.promotion` selects when a module is promoted:

- `eager` (default): every module, right after conversion.
- `manual`: only when you call `CompiledModule::promote()`, for example once a query is known to be long-running.
  `promote()` from several threads at once queues one job.

### Status

```cpp
auto compiled = module.compile();

compiled.tier();             // Tier::interpreted, Tier::tier0 or Tier::tier1
compiled.promotionStatus();  // {PromotionState::queued, ...}: none, queued, running, done, failed, cancelled, skipped
compiled.waitForPromotion(); // waits for this module only; with a timeout, returns false if it passed
compiled.promote();          // policy manual: queue the promotion now

engine.waitForPromotions();  // waits until no promotion of the engine is queued or running
```

A promotion that throws leaves the module on tier 0 with the status `failed` and the message in
`promotionStatus().error`; it is logged once and not retried. `engine.logStatistics` and `NAUTILUS_STATISTICS_FILE`
report a promotion as its own compile: the unit ID is the tier-0 one plus `_tier1`, `compilation.tier0UnitId` links the
two, and `promotion.queueMs`, `promotion.compileMs`, `promotion.timeToTier1Ms` and `promotion.status` (with
`promotion.error` on failure) describe it.

`engine.tiered.backgroundPromotion=false` is a deprecated alias that compiles single-tier with the tier-1 backend; it
wins over `engine.tiered.promotion` when both are set, with a warning. Prefer `engine.backend`.

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

### Reusing One Engine for Many Modules

A single engine is designed to be reused to compile many modules over its lifetime. The engine owns the
compiler infrastructure (and shared arenas); each `CompiledModule` owns its executable and frees it — together with
its JIT-allocated code memory — when the module is destroyed, while the engine stays alive and ready to compile more
modules.

```cpp
engine::NautilusEngine engine; // build once

for (...) {
    auto module = engine.createModule();
    module.registerFunction("execute", myFunc);
    auto compiled = module.compile();
    // use compiled ...
} // each `compiled` (and its executable) is freed here; the engine lives on
```

### Named Modules and Imports

A module with a name is registered with its engine when it is compiled, and modules compiled after it can call its
functions. Use this to compile a library of functions once and share it among many queries:

```cpp
auto math = engine.createModule("lib.math");
math.registerFunction<val<uint64_t>(val<uint64_t>)>("hash", [](val<uint64_t> x) { return x * 31 + 7; });
auto lib = math.compile();                       // registered as "lib.math"

auto query = engine.createModule("nes.query42");
auto hash = query.importFunction<uint64_t(uint64_t)>("lib.math", "hash");
query.registerFunction<val<uint64_t>(val<uint64_t>)>("run", [hash](val<uint64_t> x) { return hash(hash(x)); });
auto compiled = query.compile();                 // links "lib.math"."hash"
```

`importFunction` takes the *raw* signature of the function. The call is traced as a call through an import slot, a
cell that holds the entry point of the exporter's current code, so the exporter can be promoted to another tier without
recompiling the importer: the engine points the slot at the new code, and calls that are running finish in the old.

`compile()` links the imports and fails with a `link::LinkError` that names the import if the module is not
registered or not compiled, the function is not exported, its signature is not exactly the one imported (extension
kinds of narrow integers included), the export does not give the guarantees the import assumes, or the import graph
would have a cycle. Nothing is bound when a link fails, and the name of the module that failed stays free.
`importFunction` checks the signature at once if the exporter is already compiled.

* A name refers to one module of an engine; compiling a second module under a registered name fails.
* An importer pins its exporters: letting go of the exporter's `CompiledModule` keeps it (and its name) alive until the
  last module that imports from it is gone.
* Imports take over the guarantees the export gives (`module.setFunctionAttributes("hash", attributes)` declares
  them for a function, as `invoke(attributes, ...)` does for a native one). Pass `FunctionAttributes` to
  `importFunction` to assume less, or more, which the link then checks. A call that may throw goes through a thunk that
  carries the exception across the tiers, whichever the exporter's current tier is.
* Imports are linked on the default (NIR) pipeline of the `tbc`, `cpp`, `asmjit` and `mlir` backends. With
  `tracing.nirTracer=false` (or `debug` and `perf`, which select the 0.3 pipeline) a call of an imported function
  throws. An engine that interprets (`engine.compilation=false`) calls the exporter by name.

### Memory Use of a Long-Running Engine

Tracing and IR construction take their memory from arenas, which draw fixed-size chunks (64 KiB up to 1 MiB) from one
pool per engine. A request above half a chunk gets a dedicated allocation that is freed when its compilation ends.
The pool keeps at most `engine.arena.retainedBytes` (8 MiB by default) idle: chunks beyond that go back to the OS at
once, idle chunks give up their pages after 200 ms and are unmapped after 5 s. Call `NautilusEngine::trimMemory()` to
give all idle chunks back immediately, for example under memory pressure, and read the pool's counters (bytes held and
in use, peak, chunks trimmed) with `NautilusEngine::arenaStatistics()`. An embedder that wants to charge compiler
memory to its own budget passes a `common::ChunkProvider` to the engine's constructor.

### Per-Module Options

Options have two scopes (see [options.md](options.md)): engine-wide options configure the built-once compiler
(backend, tiers, promotion mode — these cannot change per module), while the per-compile *module* options
(`dump.*`, `ir.*`, `optimizationLevel`, debug info, tracing flags, ...) can be overridden for an individual
module. A module inherits all engine-wide values and overrides only what it sets.

Override options when creating the module, or with `setOption` before compiling:

```cpp
// Engine-wide defaults
engine::Options options;
options.setOption("optimizationLevel", 3);
engine::NautilusEngine engine(options);

// Module A inherits the engine defaults (optimizationLevel = 3)
auto moduleA = engine.createModule();
moduleA.registerFunction("execute", myFunc);
auto compiledA = moduleA.compile();

// Module B overrides a single option for itself only
engine::ModuleOptions overrides;
overrides.setOption("optimizationLevel", 0);
auto moduleB = engine.createModule(overrides);
moduleB.registerFunction("execute", myFunc);
auto compiledB = moduleB.compile();

// ...or override after creation:
auto moduleC = engine.createModule();
moduleC.setOption("dump.after_ir_creation", true);
```

## Runtime Options Reference

The following table lists all available runtime options.

### Engine Options

| Option | Default | Description |
|--------|---------|-------------|
| `engine.compilation` | `true` | Enables compilation of Nautilus functions. When `false`, all functions are executed directly through `val<T>` wrappers without tracing or code generation. Useful for debugging. |
| `engine.backend` | `mlir` | Selects the compilation backend. Valid values: `mlir`, `tbc`, `cpp`, `asmjit`. |
| `engine.normalizeFunctionNames` | `false` | Generates normalized function names (`runtimeFunc0`, `runtimeFunc1`, etc.) during tracing instead of using actual function names. Useful for deterministic output and testing. |
| `engine.foldStaticConstants` | `true` | Folds operations over values C++ already knows while tracing: constant pointer offsets (`ptr + 2` traces one constant and an `ADD`), the `* 1` of byte-pointer offsets, and the `CAST` of a literal in `val OP literal`. Set to `false` to trace them unfolded. See [options.md](options.md). |
| `engine.demangleFunctionNames` | `true` | Demangles C++ function names to human-readable format during tracing. When disabled, uses mangled names as-is. Ignored if `normalizeFunctionNames` is enabled. |
| `engine.resolveFunctionNames` | `false` (`true` if `debug`, `perf` or `perf.sample` is set) | Resolves the symbol names of called runtime functions (`dladdr` + demangling, after tracing and cached process-wide). `dladdr` can cost about 1 ms per callee in large binaries and names never change the generated code, so by default they are only resolved when the compiled code will be inspected; otherwise such callees are named `fnN`. Setting it explicitly overrides the default. |

### Dump Options

These options control the output of intermediate representations at various stages of the compilation pipeline.

| Option | Default | Description |
|--------|---------|-------------|
| `dump.all` | `false` | Dumps intermediate representations at all compilation steps. |
| `dump.after_tracing` | `false` | Dumps traces directly after trace generation. |
| `dump.copySites` | `false` | Dumps `copy_sites.txt`, the copies traced per C++ call site (see [options.md](options.md)). Not part of `dump.all`. |
| `dump.after_ir_creation` | `false` | Dumps the Nautilus IR after generation. |
| `dump.after_mlir_generation` | `false` | Dumps the generated MLIR (MLIR backend only). |
| `dump.after_llvm_generation` | `false` | Dumps the generated LLVM IR (MLIR backend only). |
| `dump.after_cpp_generation` | `false` | Dumps the generated C++ code (C++ backend only). |
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
| `optimizationLevel` | `3` | Sets the optimization level for MLIR code generation (0--3). |
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
