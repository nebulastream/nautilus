# Persistent compilation cache

Nautilus can persist a module's native object code and lowered MLIR bytecode. A native hit skips tracing, IR generation, lowering, and code generation; it links the object into a new JIT with the current process's runtime bindings. If the object is missing or corrupt, a valid MLIR artifact can regenerate it without retracing. Otherwise Nautilus compiles normally.

## Enable caching

Caching is opt-in: set both `engine.Blob.CacheDir` and `engine.Blob.CacheKey`. It currently requires **single-tier MLIR compilation on Linux**, with ELF build IDs identifying the compiler and imported native code. Explicitly selecting `engine.backend = "mlir"` selects single-tier compilation. Enabling cache options does not override another backend or background tiered compilation.

The application supplies the semantic key. Equal keys must identify interchangeable traced code, including every code-generation-relevant input, captured constant, function body, and dependency not covered by Nautilus's compatibility fingerprint. Nautilus does not inspect a lambda's captures to construct this key. Do not include process addresses in it; use runtime bindings for process-local state.

```cpp
#include <nautilus/Engine.hpp>
#include <nautilus/RuntimeBinding.hpp>
#include <cstdint>
#include <string>

nautilus::engine::CompiledModule compileIncrement(int64_t& counter, const std::string& cacheDirectory) {
    nautilus::engine::Options options;
    options.setOption("engine.backend", std::string("mlir"));
    options.setOption("engine.Blob.CacheDir", cacheDirectory);
    options.setOption("engine.Blob.CacheKey", std::string("increment-counter"));

    nautilus::RuntimeBindings bindings;
    auto state = bindings.bind<int64_t>("counter", &counter);
    nautilus::engine::NautilusEngine engine(options);
    auto module = engine.createModule();
    module.setRuntimeBindings(bindings);
    module.registerFunction<nautilus::val<int64_t>(nautilus::val<int64_t>)>(
        "increment", [state](nautilus::val<int64_t> delta) {
            auto address = state.get();
            *address = *address + delta;
            return static_cast<nautilus::val<int64_t>>(*address);
        });
    return module.compile();
}
```

Calling `compileIncrement` again with another counter and the same cache directory can reuse the object while binding it to the new counter. Previously compiled modules retain their own bindings.

## Runtime bindings

`RuntimeBindings::bind<T>(identity, address)` returns a typed handle. Inside traced code, `handle.get()` produces a runtime-binding IR operation rather than an address constant. MLIR lowers that operation to a symbol resolved when the module is loaded. Other backends and interpreted execution also support the handle, without persistent caching.

Identities must be stable and unique within a registry. Null addresses, conflicting registrations, unbound handles, and handles absent from the module's registry are rejected. The schema includes identities and compiler-derived type, size, and alignment information, including unused registrations. A changed schema cannot hit the old entry.

A module snapshots the registry, but **does not own the pointed-to data**. That storage must outlive all calls using the compiled module. Access to shared data remains the application's synchronization responsibility.

## Compatibility and fallback

The internal key additionally incorporates the compiler build ID, LLVM version, host target/CPU/features/data layout, export signatures and attributes, module options, runtime-binding schema, and registered MLIR intrinsic-plugin fingerprints.

Native imports are represented by ELF build ID and ELF load-bias-relative offset, not absolute addresses. Cache loading resolves them against the current process and rejects missing, changed, or ambiguous images. C++ exception personality imports use an identifiable native bridge, preserving unwinding through cached JIT frames.

Modules containing captured raw addresses, integer-encoded addresses, or pointer origins that cannot be proved relocatable are compiled without publication. The analysis is deliberately conservative around aliasing memory, opaque native calls, and cleanup metadata. Even an ordinary integer constant passed to an opaque native consumer can prevent publication because the consumer might interpret it as an address. Use runtime data or trustworthy existing function attributes rather than disguising captures.

Debugging and profiling options (`debug`, `perf`, `perf.sample`) bypass persistent caching so each compilation regenerates its source files and metadata. `mlir.inline_invoke_calls = true` also bypasses caching. Missing compiler identity, unsupported intrinsic plugins, unavailable cache storage, invalid artifacts, and publication failures also fall back to compilation. Cache eligibility does not change the selected backend or configured tiers.

## Intrinsic plugins

Custom `MLIRIntrinsicPlugin` implementations are cache-ineligible unless they explicitly implement `cacheFingerprint()`. The fingerprint must identify the implementation build and every code-generation-relevant piece of state or dependency. Equal fingerprints must mean interchangeable generated code. An empty or absent fingerprint disables persistent caching.

Stateless built-in plugins identify their existing registration functions by executable image and offset. Custom plugins may use the protected `cacheFingerprintForAddress()` helper, but must also account for captured state and dependencies. Plugin state must remain stable throughout compilation; fingerprint checks do not replace synchronization of plugin mutation. Registry registration order is part of the fingerprint, and detected changes during compilation prevent publication under the earlier key.

## Storage and trust

A cache directory contains native objects (`.o`), MLIR bytecode (`.mlirbc`), and checksummed manifests (`.manifest`). Publication writes content before the manifest, using unique temporary files and atomic rename; Linux writes are synchronized with `fsync`. Per-key locks coordinate compilers within one process. Use a single writer process per directory: there is no cross-process locking, eviction, authentication, or cache-size management.

**Cache directories are trusted executable input.** Checksums detect accidental corruption, not malicious modification. Anyone who can write the cache can influence JIT-executed code. Do not share writable caches with untrusted users.

`CompiledModule::getStatistics()` exposes `cache.key`, `cache.eligible`, `cache.object`, `cache.mlir`, `cache.tracingRan`, and `cache.fallback`, together with available compilation and backend timings. Unsupported configurations report their fallback reason and compile normally; a native hit reports `cache.object = "hit"` and `cache.tracingRan = 0`.
