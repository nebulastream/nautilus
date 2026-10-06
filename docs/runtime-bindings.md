# Runtime bindings

`<nautilus/RuntimeBinding.hpp>` makes process-local object addresses explicit without adding arguments to a module's exported functions. It is a core API, independent of the optional persistent cache plugin.

A `RuntimeBindings` registry assigns a nonempty, unique string identity to a nonnull `T*`. `bind<T>()` returns a copyable `RuntimeBinding<T>`; `get()` returns `val<T*>`, preserving qualifiers such as `const`. Outside tracing this is an ordinary pointer wrapper. During tracing it records a symbolic binding operation and checks that the exact handle belongs to the module's binding snapshot. Matching names, types and even addresses in an independently created registry do not make its handles interchangeable during tracing.

```cpp
#include <nautilus/Engine.hpp>
#include <nautilus/RuntimeBinding.hpp>
#include <cstdint>
#include <string>

nautilus::engine::CompiledModule compileUpdate(int64_t& total) {
    nautilus::RuntimeBindings bindings;
    auto state = bindings.bind<int64_t>("operator/17/total", &total);
    nautilus::engine::Options options;
    options.setOption("engine.backend", std::string("mlir"));
    nautilus::engine::NautilusEngine engine(options);
    auto module = engine.createModule();
    module.setRuntimeBindings(bindings);
    module.registerFunction<nautilus::val<int64_t>(nautilus::val<int64_t>)>(
        "update", [state](nautilus::val<int64_t> delta) -> nautilus::val<int64_t> {
            *state.get() += delta;
            return *state.get();
        });
    return module.compile();
}
```

Call `module.getFunction<int64_t(int64_t)>("update")` on the result. The public ABI is still `int64_t(int64_t)`, not a function with an extra registry/context argument. No application global, `Entry*` manipulation, source-private header or cache factory is needed.

Alternatively, call `options.setRuntimeBindings(bindings)` before constructing the engine. New modules inherit that snapshot. `ModuleOptions::setRuntimeBindings()` and `module.setRuntimeBindings()` replace a module's environment, including with an explicitly empty registry. Supplying cache options to an ordinary engine does **not** select the cache plugin.

## Snapshots, ownership and aliases

Registry copies and option/module snapshots retain immutable registration records. Adding entries to, resetting or destroying the original registry does not change an existing snapshot. Duplicate identities cannot be rebound in place; construct a new registry and compile/load another module to select new addresses. An already compiled/loaded module keeps its own addresses, independently of later modules.

A handle retains its registration, not its pointee. Engine and registry destruction are safe while a compiled module or `ModuleFunction` handle remains alive; a function handle also retains its executable after module destruction. Interpreted execution retains the captured callable and handles. Captured nested callables must themselves remain alive: the installed consumer owns nested `NautilusFunction` objects through captured `shared_ptr`s rather than references to expired local variables.

The caller must keep every referenced object, subobject and native dependency image alive for every possible invocation. Object contents are **not** snapshotted or serialized. Caller writes between invocations are visible to loads, and traced stores update current pointees. Concurrent access to pointees requires the caller's normal synchronization; registry snapshots do not prevent C++ data races or make stores atomic.

Different identities may have equal addresses, overlap, or alias runtime arguments. A `const` binding may alias a mutable binding; `const` does not promise that another alias cannot write the object. Schema equality imposes no alias/no-alias promise. Ordinary compilation and persisted loads must preserve these relationships even when a producer used equal addresses and a consumer uses distinct addresses, or vice versa.

Default-constructed handles are unbound. `isBound()` permits an explicit caller-selected fallback; unchecked `get()` throws. Empty identities, null addresses and duplicate identities are rejected. Bound handles absent from the tracing snapshot, including registrations added after the snapshot, are rejected even in nested tracing or replay. Interpretation simply executes the captured handle's pointer; it is not the tracing membership validator. An `isBound()` branch is a raw C++ specialization choice and belongs in the caller's semantic identity if it changes persisted code.

## Schema and relocation contract

The full registry schema is sorted by identity and includes **unused registrations**. `artifact::Descriptor::bindingSchema` is a `std::vector<runtime_binding::SchemaEntry>`; each entry contains `identity`, `type` and `symbol`, never the pointee or registration-record address. Symbols are derived deterministically from identities, not registration order, a process-global counter or prior registry history.

The type string records the compiler's C++ type spelling together with `sizeof(T)` and `alignof(T)` (`void` uses size zero and alignment one). Signedness, qualifiers, distinct named types, size and alignment matter. This is a compiler-specific schema, not a portable type-name standard or complete reflection of a class's members. Use stable identities and stable, consistently defined C++ types across consumers. Do not infer compatibility merely from equal sizes, change layouts under the same semantic key, or expect unrelated compiler versions/settings to interoperate. Artifact/cache compatibility additionally checks actual compiler/provider images, LLVM, target, options and extensions.

Changing, removing, renaming or adding an unused entry changes compatibility just as changing a used entry does. Changing **only addresses or pointee contents** does not. Registration order does not matter. Binding symbols resolve directly to the current registry's object addresses at compile/load time; persisted descriptors, native objects and bytecode do not carry producer addresses. Loaded machine code can of course contain its own current-process relocations. There is no shared application binding table or extra exported ABI parameter.

Standalone descriptors and cache manifests use the version-2 runtime-binding format. Old descriptors are rejected and old cache records are not accepted as compatible hits; this is not an automatic migration facility.

## Standalone MLIR artifacts

Use `module.createArtifact()`, then `artifact::encode()`/`decode()` for application-owned transport. Set the consuming registry on the **load options**, not on an unrelated engine:

```cpp
nautilus::RuntimeBindings current;
(void) current.bind<int64_t>("operator/17/total", &currentTotal);
nautilus::engine::Options loadOptions;
loadOptions.setOption("engine.backend", std::string("mlir"));
loadOptions.setRuntimeBindings(current);
auto loaded = nautilus::artifact::loadNative(transported, loadOptions);
```

Here `currentTotal` is live caller-owned `int64_t` storage and `transported` is the previously emitted `ModuleArtifact`. Its complete schema and effective options must match the producer; no handle from the producer is needed for a load. The standalone load API accepts no tracing callable and bypasses the Nautilus frontend.

- `loadNative()` validates the descriptor and native object, resolves imports/bindings and links current addresses without bytecode parsing, optimization or code generation. Missing or corrupt `ModuleArtifact::bytecode` does not prevent a valid native load.
- `loadBytecode()` independently validates/recompiles lowered bytecode, its export ABI and generated object without tracing. It also resolves the current registry, not producer addresses.
- The combined `encode()`/`decode()` envelope requires **both valid payloads**. The consumer's missing/corrupt-bytecode modes decode a valid envelope first, then remove/change only its in-memory bytecode field. They do not claim that a truncated envelope decodes successfully.
- Missing, extra, renamed, differently qualified or otherwise mistyped schemas are rejected before invoking exports. Address changes with an identical schema are allowed. Independent native/bytecode loads keep their own bindings after options and registries are destroyed.

Persistence currently requires MLIR on Linux x86-64 ELF, identifiable compiler/producer/import images and host-target code generation; `artifact::isSupported()` reports capability. Linux consumers here use PIE and SHA-1 ELF build IDs, matching the existing installed-package consumer. Static-core consumers identify the containing executable, so rebuilding that executable invalidates compatibility. Other configured backends remain available for ordinary compilation, and compiler-OFF/tracing-OFF packages execute the interpreter rather than advertising persistence.

## Preflight and legacy cache safety

Bindings certify **where a pointer comes from**, not every other capture or scalar introduction in the program. Standalone emission performs mandatory pre-optimization preflight: declared root ABIs, graph/function ownership, call and branch schemas, all scalar leaves, cleanup-only operand trees and binding operations. Optional IR pass scheduling cannot disable this validation or mandatory exception preparation.

Use runtime arguments or bound storage for process-local values. `cacheLiteral<V>()` and arithmetic-only `cacheInvariant(value)` explicitly certify ordinary address-independent scalar data. They must not certify addresses cast to integers, hashes/fragments of addresses, raw member/static offsets or other process-local encodings. Incorrect certification is a caller bug, not something a warm hit can discover. Folded-away introductions and replay disagreement still count as evidence. Binding one pointer does not sanitize a different captured raw pointer, callback or cleanup input.

The cache plugin first checks mandatory root/scalar evidence. If the complete scalar certificate fails, its existing conservative pointer-relocatability analysis still decides whether narrower legacy code can be persisted; some legacy ordinary-scalar functions can pass that analysis. This is **not** the standalone strict-preflight contract, nor permission for arbitrary opaque calls, stores, encoded-address fragments, callbacks or native cleanup data. Unsafe or unaccounted cases preserve ordinary compilation rather than publishing artifacts. Raw `alloca`/stack-allocation size/alignment metadata lacks scalar-origin evidence: standalone emission rejects it and the cache declines it even if the scalar-leaf certificate otherwise succeeds.

Debug/perf generation, pinned target CPUs, invoke-call inlining (`mlir.inline_invoke_calls`), active LLVM hooks, unknown code-generating extensions, unsupported imports, unregistered external globals and module initializers/finalizers remain outside persistence's supported boundary. Bindings do not relax export ABI, target, exception/unwind or native-import checks. See [module-artifacts.md](module-artifacts.md) for the underlying artifact and scalar contracts.

## Explicit cache selection and warm obligations

Build/install with `ENABLE_CACHE_PLUGIN=ON` and link `nautilus::nautilus-cache`. It requires compiler, tracing and MLIR. Selection is explicit:

```cpp
#include <nautilus/cache/plugin.hpp>

nautilus::engine::Options options;
options.setOption("engine.backend", std::string("mlir"));
options.setOption("engine.cache.directory", cacheDirectory);
options.setOption("engine.cache.key", semanticKey);
options.setRuntimeBindings(currentBindings);
nautilus::engine::NautilusEngine engine(nautilus::cache::createCompiler(options), options);
```

`cacheDirectory` and `semanticKey` are nonempty strings supplied by the caller; `currentBindings` is the registry whose handles the registered callable captures. The factory and engine receive the same options. Module-level keys/bindings may override inherited defaults. An ordinary `NautilusEngine(options)` never implicitly dispatches to this factory, even in a cache-ON installation.

The semantic key must cover interchangeable bodies, captured scalar/configuration values, layouts and every trace-time specialization not otherwise covered by compatibility. Equal schema is not equal program semantics. A native warm hit or bytecode repair does not run the callable and **cannot revalidate captured handles or `cacheInvariant` assertions**. Fresh callable captures must still correspond to the supplied snapshot and semantic key; a warm hit must not be used to hide stale/foreign handles that cold tracing would reject. Changing runtime addresses is supported; changing captured program meaning under an unchanged key is not.

Native hits report `cache.object=hit`, `cache.mlir=not_checked`, `cache.tracingRan=0`, `cache.fallback=none`, object-load timing and no frontend/IR/code-generation timings. A corrupt object repaired from compatible bytecode reports `cache.object=invalid_rewritten`, `cache.mlir=hit`, `cache.tracingRan=0`; this is a repair, **not a native hit**. Count actual requests and repairs separately. The installed consumer checks these statistics alongside independent per-process wrapper counters, including zero for warm and repair.

Trusted-cache ownership/permissions, atomic publication and locking are unchanged; see [cache-plugin.md](cache-plugin.md). Descriptors/payload checksums detect corruption, not hostile executable input or dishonest producers. Keep artifacts and caches in trusted storage. There is no eviction, general object serialization, cross-version/CPU migration or arbitrary live JIT-state restoration.

## Installed consumer and reproducible commands

`nautilus/test/runtime-binding-consumer` is a separate `find_package(nautilus CONFIG REQUIRED)` project. It links only installed public targets and includes only installed public headers. `NAUTILUS_CACHE_EXPECTED=OFF` requires absence of `nautilus::nautilus-cache`; `ON` requires its presence and links it explicitly. Neither setting alters production engine dispatch.

The following commands, run from the repository root, build/install independent cache-OFF, cache-ON, compiler-OFF and tracing-OFF packages with Clang 21. They also build the consumer and run its CTest fixtures. They intentionally create a new workspace; no existing cache directory is reused. MLIR discovery comes from each configured package build, not a source-private include path.

```bash
set -eu
source_dir="$PWD"
work_dir="$(mktemp -d "$HOME/nautilus-bindings.XXXXXX")"
for variant in cache-off cache-on compiler-off tracing-off; do
    compiler=ON; tracing=ON; mlir=ON; cache=OFF
    case "$variant" in
        cache-on) cache=ON ;;
        compiler-off) compiler=OFF; tracing=OFF; mlir=OFF ;;
        tracing-off) tracing=OFF; mlir=OFF ;;
    esac
    build="$work_dir/core-$variant"
    prefix="$work_dir/install-$variant"
    consumer="$work_dir/consumer-$variant"
    cmake -S "$source_dir" -B "$build" \
        -DCMAKE_C_COMPILER=clang-21 -DCMAKE_CXX_COMPILER=clang++-21 \
        -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_LIBDIR=lib \
        -DENABLE_TESTS=OFF -DENABLE_BENCHMARKS=OFF \
        -DENABLE_COMPILER="$compiler" -DENABLE_TRACING="$tracing" \
        -DENABLE_MLIR_BACKEND="$mlir" -DENABLE_CACHE_PLUGIN="$cache" \
        -DENABLE_SIMD_PLUGIN=OFF -DENABLE_STD_PLUGIN=OFF \
        -DENABLE_SPECIALIZATION_PLUGIN=OFF -DENABLE_INLINING_PLUGIN=OFF \
        -DENABLE_GPU_PLUGIN=OFF -DENABLE_BUILTIN_PLUGIN=OFF -DENABLE_PROFILING_PLUGIN=OFF
    cmake --build "$build" --parallel
    cmake --install "$build" --prefix "$prefix"
    mlir_dir="$(awk -F= '/^MLIR_DIR:[^=]*=/{print $2}' "$build/CMakeCache.txt")"
    cmake -S "$source_dir/nautilus/test/runtime-binding-consumer" -B "$consumer" \
        -DCMAKE_C_COMPILER=clang-21 -DCMAKE_CXX_COMPILER=clang++-21 \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$prefix" \
        -DMLIR_DIR="$mlir_dir" -DNAUTILUS_CACHE_EXPECTED="$cache"
    cmake --build "$consumer" --parallel
    ctest --test-dir "$consumer" --output-on-failure
done
```

For standalone fresh-process checks outside CTest, after that installation matrix:

```bash
export LD_LIBRARY_PATH="$work_dir/install-cache-off/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
"$work_dir/consumer-cache-off/runtime-binding-consumer" ordinary all
"$work_dir/consumer-cache-off/runtime-binding-consumer" artifact-roundtrip "$work_dir/off-artifacts"
export LD_LIBRARY_PATH="$work_dir/install-cache-on/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
"$work_dir/consumer-cache-on/runtime-binding-consumer" artifact-roundtrip "$work_dir/on-artifacts"
"$work_dir/consumer-cache-on/runtime-binding-consumer" cache-roundtrip "$work_dir/on-cache"
```

The roundtrip commands fork/**exec** fresh children, not merely forked copies of a producer's address space. They require Linux ASLR, compare producer/current addresses, check pointer-returning exports and scan native/bytecode/descriptor-or-manifest payloads for raw, decimal and hexadecimal binding addresses, including the unused registration. `.report` sidecars intentionally contain addresses/counters/statistics as test evidence and are **not persisted code payloads**. `cold` removes only its explicitly supplied dedicated cache directory; `repair` corrupts its single native object before requesting a bytecode repair. Use disposable test paths.

### Modes and CTest coverage

| Command arguments | CTest name(s) | Checks |
|---|---|---|
| `ordinary [all\|interpreter\|backend]` | `runtime-binding-ordinary-interpreter`, `runtime-binding-ordinary-{mlir,cpp,bc,tbc,asmjit}` for configured backends | Mutable state, caller writes, loop/branch merges, nested functions, equal/distinct and const aliases, module/options snapshots, engine/registry/module destruction, no implicit cache statistics. |
| `ordinary reduced` | `runtime-binding-ordinary-reduced` only when compiler/tracing is disabled | Default compilation option still executes the interpreter; no tracing/executable or persistence capability. |
| `ordinary tbc-auto` | `runtime-binding-ordinary-tbc-auto` when TBC JIT is configured | TBC automatic mode reports the actual `jit` or `interp` result; this is not a strict JIT claim. `ordinary tbc-jit` is an optional strict check and fails when the installed runtime cannot execute JIT code. |
| `emit FILE` | `runtime-binding-artifact-emit` | Version-2 descriptor, full typed schema including unused entry, unchanged exported argument ABI, strict emission and transport roundtrip. |
| `native FILE`, `bytecode FILE` | `runtime-binding-artifact-native`, `runtime-binding-artifact-bytecode` | Fresh current-address resolution, pointee mutation, nested/lifetime behavior, no frontend callable supplied and absent frontend/IR timings. |
| `rebound FILE` | `runtime-binding-artifact-rebound` | Two simultaneously retained independent native loads and a bytecode load; alias topology changes, no global registry replacement. |
| `schema-mismatch FILE` | `runtime-binding-artifact-schema-mismatch` | Native and bytecode reject absent/renamed/extra schemas, unused-type, same-size type, size and const mismatches, and version-1 descriptor. |
| `native-no-bytecode FILE`, `native-corrupt-bytecode FILE` | `runtime-binding-artifact-native-no-bytecode`, `runtime-binding-artifact-native-corrupt-bytecode` | Valid native object loads although in-memory bytecode is absent/corrupt; bytecode load rejects it. |
| `cold DIR`, `warm DIR`, `repair DIR` | `runtime-binding-cache-cold`, `runtime-binding-cache-warm`, `runtime-binding-cache-repair`, `runtime-binding-cache-warm-after-repair` | Explicit factory only, cold publication, native hit, trace-free bytecode repair/republication, then another native hit; independent warm/repair counters are zero and addresses are not persisted. |
| `artifact-roundtrip DIR`, `cache-roundtrip DIR` | Manual orchestration of the corresponding modes | Self-contained fresh-ASLR-process sequences; cache mode requires the cache-ON target. |

Standalone reader reports use `frontend-wrapper=not_supplied`, not an independent zero-counter assertion: the load API takes no frontend callable. In-process artifact tests retain the emitter's real wrapper counter and verify it stays unchanged across standalone loads and execution. Cache modes instead register fresh callables with real per-process counters, including zero for warm and repair.

Artifact fixtures run for configured tracing+MLIR packages on Linux x86-64 with cache OFF **and** ON. All readers require the emission fixture and may run in parallel without changing its envelope. Cache fixtures enforce `cold → warm → repair → warm-after-repair`, each in a separate process; filtered CTest runs automatically include prerequisite fixtures. Every ordinary configured backend is tested regardless of cache availability; unavailable backends are not silently replaced. CTest supplies installed-library search paths on Linux, including the shared core and cache libraries. The existing installed-package CI step runs both this consumer and the cache package consumer on its cache-ON/OFF and reduced-package matrix legs, with the same installed prefix, compiler environment and MLIR/LLVM discovery paths.

These are installed-package/runtime-binding contract checks, not a live NebulaStream/NES integration, worker-lifecycle certification, general application release claim or proof about untested backends/platforms. Applications must still provide correct semantic keys, storage lifetimes and synchronization.
