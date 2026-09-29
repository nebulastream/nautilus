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

## Cache-invariant scalar constants

`<nautilus/val.hpp>` exposes two scalar factories:

- `nautilus::cacheLiteral<V>()` returns `val<decltype(V)>` for a compile-time arithmetic or boolean value accepted as a template argument by the C++ compiler.
- `nautilus::cacheInvariant(rawScalar)` returns `val<T>` for the raw arithmetic or boolean type `T`, with cv/ref qualifiers removed. This is an explicit caller assertion for audited program or configuration data.

Both produce ordinary scalar constants while tracing and ordinary values when not tracing, using Nautilus's existing supported scalar representations. They do not call native getter functions or change runtime arithmetic. Neither accepts pointers, enums, `static_val`, or existing `val` expressions. An enum can be explicitly converted to its underlying scalar type before certification.

```cpp
#include <nautilus/val.hpp>

auto zero = nautilus::cacheLiteral<0>();
auto finalChunk = nautilus::cacheLiteral<true>();
```

Certification has a correctness contract:

1. The value must be independent of process and module-instance addresses. This excludes address bits, fragments, hashes, encodings, and address-dependent differences or deltas, even when represented as small integers, booleans, or floating-point values.
2. Equal effective cache keys must imply the same scalar value and every code-generation-relevant choice selecting that value. Captured configuration and trace-time control-flow choices must already be covered by the semantic key supplied before compilation. Warm hits skip tracing, so the assertion site is not executed or revalidated on a hit.
3. The assertion grants no purity, no-alias, ownership, lifetime, synchronization, or exception guarantee. It does not make native function attributes such as `NoModRef` or `noUnwind` true.

Prefer `cacheLiteral<V>()` for fixed literals. Use `cacheInvariant` only at an audited raw-scalar introduction, not to sanitize a computed traced expression. The contract also applies to literal factories: a hard-coded encoded address is not valid ordinary data. Incorrect annotations or incomplete semantic keys can make cache reuse incorrect; these factories are not an information-flow verifier.

Nautilus automatically certifies only fixed internal scalar default zero/false, increment/decrement steps, internal unary zero, and typed-pointer `sizeof` scaling factors. Ordinary raw-value constructors, raw offset arguments, and `static_val` conversions remain uncertified, even when their numeric values match certified literals. Member-offset parameters are not implicitly certified. Mutable or instance-dependent scalar data should instead be loaded from current runtime arguments or `RuntimeBindings` storage, whose lifetime must cover execution.

Scalar provenance is represented by `nautilus::ConstantOrigin::{Unspecified, CacheInvariant}` in `nautilus/common/ConstantOrigin.hpp`. Raw tracing calls and directly constructed scalar IR constants default to `Unspecified`; `ConstIntOperation`, `ConstBooleanOperation`, and `ConstFloatOperation` expose `getConstantOrigin()`. Trace cloning preserves origin, and replay disagreement downgrades it to `Unspecified` rather than upgrading an ordinary constant.

## Compatibility and fallback

The internal key additionally incorporates the compiler build ID, LLVM version, host target/CPU/features/data layout, export signatures and attributes, module options, runtime-binding schema, and registered MLIR intrinsic-plugin fingerprints.

Native imports are represented by ELF build ID and ELF load-bias-relative offset, not absolute addresses. Cache loading resolves them against the current process and rejects missing, changed, or ambiguous images. C++ exception personality imports use an identifiable native bridge, preserving unwinding through cached JIT frames.

Modules containing captured raw addresses, integer-encoded addresses, or pointer origins that cannot be proved relocatable are compiled without publication. A strict pre-optimization proof can accept a module whose embedded scalar leaves are all certified cache-invariant and whose remaining leaves and operations are supported and relocatable. The proof includes internal functions, block arguments, and destructor-only operands; certification never bypasses native-import, binding-schema, plugin, or artifact validation.

If complete scalar certification fails, the existing conservative analysis still applies around aliasing memory, opaque native calls, and cleanup metadata. Even an ordinary integer constant passed to an opaque native consumer can prevent publication because the consumer might interpret it as an address. Use certified ordinary data under the contract above or current runtime data rather than disguising captures or inventing function attributes.

Debugging and profiling options (`debug`, `perf`, `perf.sample`) bypass persistent caching so each compilation regenerates its source files and metadata. `mlir.inline_invoke_calls = true` also bypasses caching. Missing compiler identity, unsupported intrinsic plugins, unavailable cache storage, invalid artifacts, and publication failures also fall back to compilation. Cache eligibility does not change the selected backend or configured tiers.

## Compiler stages and integration boundaries

On a traced cache miss, `CacheScalarValidationPass` runs at `CompilationPipeline`'s pre-optimization hook, before folding or dead-code elimination can discard scalar origins. If its complete-leaf certificate fails, `PointerRelocatabilityPass` runs on the resulting IR, preserving the legacy acceptance rules. Both are read-only `IRPass` analyses in `compiler/ir/passes/CacheSafetyAnalysis`: `apply()` returns `false` because the graph is unchanged; acceptance and rejection details live in each pass instance's result. These mandatory cache checks are invoked directly, outside the optional optimizer and its fixed-point groups. Neither `ir.runPasses`, `ir.runOptimizationPasses`, nor iteration limits disable them.

`MLIRCacheValidation` validates exports, runtime-binding declarations/schema, and external symbols after lowering when generating cache artifacts, or after parsing and verifying a cached MLIR module. It preserves validation order and does not resolve imports or load executable code. A native-object hit never enters this stage or reads/parses MLIR bytecode, and returns before tracing or IR construction.

`PersistentModuleCache` owns compatibility, lookup, import resolution, fallback, and publication; these are not IR passes. `TieredCompiler` retains the single-tier/backend dispatch. Scalar origin and binding identity must still be recorded at the API/tracing boundary, where their meaning is known, and preserved through IR cloning. Backend-specific binding visitors retain their native register/address/symbol representations without rewriting the shared IR used for tier promotion. MLIR keeps binding symbols relocatable and opaque to alias analysis; JIT hooks register fresh per-module addresses before initialization and capture/load native objects.

## Intrinsic plugins

Custom `MLIRIntrinsicPlugin` implementations are cache-ineligible unless they explicitly implement `cacheFingerprint()`. The fingerprint must identify the implementation build and every code-generation-relevant piece of state or dependency. Equal fingerprints must mean interchangeable generated code. An empty or absent fingerprint disables persistent caching.

Stateless built-in plugins identify their existing registration functions by executable image and offset. Custom plugins may use the protected `cacheFingerprintForAddress()` helper, but must also account for captured state and dependencies. Plugin state must remain stable throughout compilation; fingerprint checks do not replace synchronization of plugin mutation. Registry registration order is part of the fingerprint, and detected changes during compilation prevent publication under the earlier key.

## Storage and trust

A cache directory contains native objects (`.o`), MLIR bytecode (`.mlirbc`), and checksummed manifests (`.manifest`). Publication writes content before the manifest, using unique temporary files and atomic rename; Linux writes are synchronized with `fsync`. Per-key locks coordinate compilers within one process. Use a single writer process per directory: there is no cross-process locking, eviction, authentication, or cache-size management.

**Cache directories are trusted executable input.** Checksums detect accidental corruption, not malicious modification. Anyone who can write the cache can influence JIT-executed code. Do not share writable caches with untrusted users.

`CompiledModule::getStatistics()` exposes `cache.key`, `cache.eligible`, `cache.object`, `cache.mlir`, `cache.tracingRan`, and `cache.fallback`, together with available compilation and backend timings. Unsupported configurations report their fallback reason and compile normally; a native hit reports `cache.object = "hit"` and `cache.tracingRan = 0`.

On traced misses, `cache.scalarCertificate` records the complete-leaf proof result. `cache.scalarRejection` identifies the first uncertified or unsupported operation before optimization; a failed certificate can still pass the original provenance guard. A provenance fallback additionally reports `cache.rejection`, identifying the operation and whether rejection came from an immediate input, whole-module memory, or cleanup. Operation identifiers can be matched to IR dumps. `cache.eligible = 1` means the cache path was entered, not that either proof passed or artifacts were published. Include fallback modules when computing hit rates.
