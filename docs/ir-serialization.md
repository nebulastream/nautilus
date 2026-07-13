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

## One format

There is exactly one IR text format: `IRGraph::toString` (the `dump.after_ir_creation` /
`dump.after_ir_passes` dumps) and `serializeIR` produce **byte-identical** output. Every dump is a
serialized module and vice versa.

- **Unique value numbering.** Every block argument and value-producing operation is printed with an
  id that is unique within its function (assigned block by block, arguments before operations), so
  every reference in the text is exact. The in-memory IR distinguishes values by pointer identity
  and legitimately reuses numeric ids across blocks, and optimization passes create cross-block
  references between them — printing the stored ids would make the text ambiguous. Consumers that
  correlate operations with the printed text (e.g. the MLIR debug-info source map) translate through
  `computePrintedValueIds`.
- **External symbols.** External functions are printed with their mangled symbol and human-readable
  name: `call @"_Z3addii" "add(int, int)"(...)`.
- **Strictness is the only difference.** `toString` never throws: state that cannot be reconstructed
  in another process — a non-null pointer constant, i.e. a raw address — is rendered as the `*`
  placeholder. `serializeIR` validates the graph first and fails with a descriptive error instead of
  writing a file the parser would reject later. Pass such pointers as function parameters instead.
- Optional `; ...` source-location comment trailers (`dump.sourceLocations`) can be appended to
  dumps; the parser skips them.

The golden test fixtures normalize the volatile parts of external references (mangled symbols depend
on the build) to `@"<symbol>" "<name>"` placeholders; see
`testing::normalizeExternalFunctionReferences`.

## Guarantees and limits

- Parsed graphs are verified (`IRVerifier`) before compilation; malformed input fails at load, not
  in the backend.
- The serialized IR is the *optimized* IR (the standard IR passes run before serialization), so
  loading does not re-run the IR pass pipeline.
- The format is text-based and stable under round-trips: `parse(serialize(g))` serializes to the
  identical text.
- Requires a build with `ENABLE_COMPILER` and `ENABLE_TRACING` (serialization needs tracing;
  loading needs the compilation pipeline).
