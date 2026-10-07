# Persistent cache plugin

The optional `nautilus-cache` library provides an explicitly injected compiler strategy. Build with
`ENABLE_CACHE_PLUGIN=ON`, which requires `ENABLE_COMPILER`, `ENABLE_TRACING` and `ENABLE_MLIR_BACKEND`.
The option defaults to OFF. Installed packages expose `nautilus::nautilus-cache` and the public
`<nautilus/cache/plugin.hpp>` header; consumers do not need private compiler or MLIR headers.

An ordinary `NautilusEngine(options)` never invokes this cache, even when cache options are present.
The cache factory owns real arena pools, a compilation pipeline and the normal tiered compiler for fallback.
It does not register a global strategy or change backend selection. Persistence requires synchronous,
single-tier MLIR on the supported artifact platform, Linux x86-64 ELF. Other backends and background promotion
continue through the configured compiler with cache-bypass statistics. Interpreted modules stay interpreted.

## Explicit selection

Link the installed `nautilus::nautilus-cache` target and pass the same engine options to the factory and engine:

```cpp
#include <nautilus/Engine.hpp>
#include <nautilus/cache/plugin.hpp>
#include <nautilus/val.hpp>
#include <cstdint>
#include <string>

nautilus::engine::CompiledModule compileIncrement(const std::string& cacheDirectory) {
	nautilus::engine::Options options;
	options.setOption("engine.backend", std::string {"mlir"});
	options.setOption("engine.cache.directory", cacheDirectory);
	nautilus::engine::NautilusEngine engine(nautilus::cache::createCompiler(options), options);
	auto module = engine.createModule();
	module.setOption("engine.cache.key", std::string {"increment/i32/v1"});
	module.registerFunction<nautilus::val<int32_t>(nautilus::val<int32_t>)>(
	    "increment", [](nautilus::val<int32_t> value) { return value + nautilus::cacheLiteral<1>(); });
	return module.compile();
}
```

Both `engine.cache.directory` and `engine.cache.key` must be nonempty strings to request persistence.
They are module options: engine values supply defaults, and individual modules can override them.
Direct wrapper calls and function lists without declared export signatures execute the real fallback;
the plugin does not infer a signature or invent a semantic key by tracing.

The factory normalizes options at construction and before every compilation. The deprecated
`engine.Blob.CacheDir` and `engine.Blob.CacheKey` aliases normalize to the canonical names. Equal alias and
canonical values are accepted; conflicting values or non-string values throw before tracing. Use canonical
names consistently for engine defaults and module overrides, rather than retaining a conflicting inherited alias.
Cache-directory and alias spelling changes do not alter generated-code identity.

## Semantic identity and safety

The application must provide the semantic key before compilation. Equal keys must describe interchangeable
function bodies, captured constants, literal/configuration values, layouts, and every trace-time specialization
choice not already covered by Nautilus's compatibility checks. A function name, lambda address or IR hash is
not a replacement. Warm hits cannot inspect captures or revalidate `cacheInvariant` assertions.

Compatibility additionally identifies the core compiler and cache provider separately, LLVM and host target
settings, declared exports, code-generating extensions, and typed effective options. Application options such
as `nes.numberOfWorkerThreads` remain part of compatibility. Unknown identities or changing intrinsic
fingerprints prevent reuse. Debug/profiling metadata, unsupported target settings and unaccounted LLVM hooks
retain ordinary compilation rather than loading or publishing incompatible artifacts.

On a cold compilation, export signatures, typed allocation metadata and scalar evidence are checked before
optimization. These checks are independent of `ir.runPasses`, `ir.runOptimizationPasses` and optimization
iteration limits. Every allocation table entry, including unused entries, must retain verified C++ type evidence
with its actual size and alignment. Raw, unavailable, mismatched or replay-disagreed allocation metadata blocks
persistence through both the strict scalar and legacy pointer paths. Typed owned construction/copy/move/cleanup
can persist
only when its scalar and native-import evidence independently passes; allocation evidence never certifies
constructor arguments, callbacks or encoded addresses.

A failed complete scalar certificate still requires the conservative pointer analysis; optimization cannot turn
missing provenance into a certificate. Captured raw pointers, encoded addresses, unsafe native cleanup/callback
inputs and unresolved native imports cannot be made persistable merely by supplying a key.

Use runtime function arguments for process-local storage. This plugin does not introduce runtime-binding
handles, registries or schemas. See [module-artifacts.md](module-artifacts.md) for artifact capability,
scalar-certification and native-import lifetime contracts.

## Native hits, repair and statistics

A valid native object is checked and loaded before any tracing wrapper runs, without constructing Nautilus IR,
reading/parsing MLIR bytecode, optimization or code generation. A valid object still hits when its bytecode is
missing or corrupt. If the object cannot load, compatible bytecode can regenerate it without tracing; that is
a bytecode repair, not a native-object hit. Otherwise the compiler traces and compiles normally.

Executable statistics retain backend/tier information and cache decisions. A native hit reports
`cache.object=hit`, `cache.mlir=not_checked`, `cache.tracingRan=0` and no frontend or code-generation timings.
Declines retain their reason in `cache.fallback`. Synchronous fallback merges the real compiler's timings
without overwriting the cache decision and emits one final report when `engine.logStatistics` is enabled.
Background tiers decorate their own final statistics before logging and publication; promotion owns its
options and does not retrace or mutate shared IR. Ordinary core compilers have no cache-policy statistics.

Count every actual compilation request when calculating hit rates, including failures, missing keys,
ineligible modules and fallbacks. Report repairs separately from native hits.

## Storage and trust

Entries contain native objects (`.o`), MLIR bytecode (`.mlirbc`) and checksummed manifests (`.manifest`).
The cache directory must be private (`0700`) and owned by the effective user; artifact and lock files must be
private (`0600`), owned, regular and singly linked. Symlink traversal, `..` components and untrusted writable
ancestors are rejected. The plugin creates missing directories with private permissions; existing directories
must already satisfy these requirements.

Publication uses unique temporary files, atomic rename and Linux synchronization, publishing the manifest last.
Per-key mutexes coordinate threads, and Linux advisory file locks coordinate cooperating processes. Lock files
are retained alongside entries. There is no eviction or size management.

Cache directories are trusted executable input. Checksums detect accidental corruption, not malicious changes.
Do not expose a writable cache to untrusted users. Missing or corrupt entries, inaccessible storage and
publication failures preserve ordinary compilation and its selected backend.
