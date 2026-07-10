# IR Serialization: Compiling from an IR File and Skipping Tracing

Nautilus normally traces a C++ function at runtime to derive its IR before compiling it with a backend.
The IR serialization API decouples the two steps: a module's IR can be serialized to a portable text
file in one process (or one run) and compiled directly in another — the tracing frontend is skipped
entirely when loading.

Typical uses:

- **Faster startup**: trace once, persist the IR, and on subsequent runs compile straight from the file.
- **Build-time tracing**: trace in a build step or offline tool and ship only the IR to the process
  that compiles and executes it.
- **Debugging and testing**: hand-inspect, modify, or minimize the IR that a backend compiles.

## Producing an IR file

`NautilusModule::serializeIR()` traces all registered functions and returns the IR in the portable
text format instead of compiling an executable:

```cpp
#include <nautilus/Engine.hpp>

using namespace nautilus;

val<int32_t> addOne(val<int32_t> x) {
    return x + 1;
}

auto engine = engine::NautilusEngine();
auto module = engine.createModule();
module.registerFunction("addOne", addOne);

std::string irText = module.serializeIR();
// Persist it wherever convenient, e.g.:
std::ofstream("addOne.nautilus") << irText;
```

The module remains usable afterwards; calling `compile()` traces again as usual.

## Compiling from an IR file

`NautilusEngine::loadModuleFromIR` (text) and `NautilusEngine::loadModuleFromIRFile` (path) parse the
IR, verify it structurally, and hand it directly to the configured backend — no tracing happens:

```cpp
auto loaded = engine.loadModuleFromIRFile("addOne.nautilus");
auto addOne = loaded.getFunction<int32_t(int32_t)>("addOne");
int32_t r = addOne(41); // 42
```

The loaded module supports the same tiered compilation as the traced path: with background promotion
enabled, tier 0 compiles synchronously and tier 1 swaps in when ready. Because a loaded module has no
original C++ callables, it cannot fall back to interpretation — loading requires compilation to be
enabled (`engine.Compilation`).

## External function calls and symbol resolution

Runtime addresses are process-specific, so they are never serialized. A proxy call (created by
`invoke(...)`) is recorded with its mangled symbol and human-readable name:

```
$4 = call @"_Z8myNativei" "myNative(int)"($3) :i32
```

When loading, each referenced function is resolved in the current process:

1. through the optional `IRSymbolResolver` callback passed to `loadModuleFromIR(File)`, then
2. through `dlsym` on the recorded symbol.

Loading fails with a descriptive error if a function cannot be resolved. Symbols of static or
non-exported functions cannot be recovered by `dlsym`; provide a resolver for those:

```cpp
auto resolver = [](const std::string& symbol, const std::string& name) -> void* {
    if (name.find("myNative") != std::string::npos) {
        return reinterpret_cast<void*>(&myNative);
    }
    return nullptr; // fall through to dlsym
};
auto loaded = engine.loadModuleFromIRFile("module.nautilus", resolver);
```

## One grammar, two renderings

There is a single IR text grammar with one writer behind it. It has two renderings:

- **Portable** (`serializeIR`, used by `NautilusModule::serializeIR`): complete and unambiguous.
  Every block argument and value-producing operation is renumbered with an id that is unique within
  its function, so every reference in the text is exact — the in-memory IR distinguishes values by
  pointer identity and legitimately reuses numeric ids across blocks, and optimization passes create
  cross-block references between them. External functions are printed with their mangled symbol and
  human-readable name (`call @"_Z3addii" "add(int, int)"(...)`). Non-null pointer constants fail
  serialization with a descriptive error (they are raw addresses, meaningless in another process);
  pass such pointers as function parameters instead.

- **Display** (`IRGraph::toString`, i.e. the `dump.after_ir_creation` / `dump.after_ir_passes`
  dumps): faithful to the in-memory graph for debugging. Operations keep their stored identifiers so
  dumps line up with verifier messages, backend register frames, and MLIR debug info; external
  function references stay hidden behind the deterministic `func_*` placeholder (unless address
  logging is enabled); non-null pointer constants print as `*`; and optional `; ...` source-location
  trailers can be appended (`dump.sourceLocations`).

Display dumps parse as long as they contain no `func_*` references, no `*` pointer constants, and no
ambiguous cross-block references — the parser rejects each with a clear error pointing at
`serializeIR`. For reliable round-trips, always produce files with `serializeIR()`.

## Guarantees and limits

- Parsed graphs are verified (`IRVerifier`) before compilation; malformed input fails at load, not
  in the backend.
- The serialized IR is the *optimized* IR (the standard IR passes run before serialization), so
  loading does not re-run the IR pass pipeline.
- The format is text-based and stable under round-trips: `parse(serialize(g))` serializes to the
  identical text.
- Requires a build with `ENABLE_COMPILER` and `ENABLE_TRACING` (serialization needs tracing;
  loading needs the compilation pipeline).
