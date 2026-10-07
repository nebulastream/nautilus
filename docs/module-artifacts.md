# Module artifacts

`<nautilus/Artifact.hpp>` transports one native object and one lowered MLIR bytecode module without cache lookup, files, semantic keys, publication, or runtime-binding registries.

```cpp
#include <nautilus/Artifact.hpp>
#include <nautilus/Engine.hpp>

nautilus::engine::Options options;
options.setOption("engine.backend", std::string("mlir"));
nautilus::engine::NautilusEngine engine(options);
auto module = engine.createModule();
module.registerFunction<nautilus::val<int32_t>(nautilus::val<int32_t>)>(
    "increment", [](nautilus::val<int32_t> value) {
        return value + nautilus::cacheLiteral<int32_t{7}>();
    });
auto artifact = module.createArtifact();
auto bytes = nautilus::artifact::encode(artifact);
auto transported = nautilus::artifact::decode(bytes);
auto executable = nautilus::artifact::loadNative(transported, options);
auto increment = executable.getFunction<int32_t(int32_t)>("increment");
int32_t result = increment(5);
```

The application owns transport/storage. `loadNative` verifies the descriptor and native bytes, resolves every import before JIT initialization, and loads the object without tracing, Nautilus IR, MLIR parsing, optimization, or code generation. It does not read or validate bytecode; missing/corrupt bytecode cannot prevent a valid native load. `loadBytecode` independently verifies and recompiles bytecode without tracing, checking its lowered ABI and generated native object before linking. The combined `encode`/`decode` envelope requires both valid payloads.

## Supported boundary

Persistence currently requires MLIR, Linux x86-64 ELF, host-target code generation, identifiable compiler/producer/import images, and exactly one emitted relocatable object. `isSupported()` reports this capability. Reduced builds retain ordinary compilation/interpreted execution and report unsupported persistence. Debug/perf generation, pinned `mlir.targetCpu`, inlining, and active LLVM backend hooks are not supported for artifacts. Unknown intrinsic implementations default to unsupported; an intrinsic must supply its real implementation/state fingerprint and explicitly honor `supportsArtifacts()` before participating. Audited built-in lowerings supply both.

Exports carry declared return/argument stamps, C calling convention, and the checked lowered signature/attributes. B1 rejects user-supplied root attributes and registered names that differ from the compiler's actual emitted root symbol. Narrow integer extension attributes must match the host C ABI. Typed native calls convert arguments to the declared native parameter types after loading element references. Differing truthful call attributes are reconciled conservatively without relaxing ABI checks. Internal helpers are uniquified against registered roots; names conflicting with generated packed wrappers are rejected before packing. Imports carry ELF build IDs and load-bias-relative executable offsets, never serialized process addresses. Exception personality and unwind-resume helpers use identifiable native bridges into the supported system C++ unwind ABI. Compiler-generated helpers are restricted to an explicit runtime/math/memory set and also require executable identities. Unavailable or ambiguous images, external globals, module initializers/finalizers, absolute object symbols, incompatible targets/options/extensions, and malformed records fail closed. A static core identifies its containing executable, so rebuilding that executable invalidates artifacts even if its functions look equivalent.

## Scalar and pointer safety

Emission enables origin recording once for every traced function/region. The mandatory pre-optimization preflight checks declared roots against actual entry-block arguments and return stamps, every scalar leaf, graph/function ownership, call targets/signatures, branch schemas, and destructor-only operand trees. Optional IR scheduling cannot disable this check. Exception cleanup preparation remains mandatory even with `ir.runPasses=false`.

Ordinary raw scalar constructors, raw/static/member offsets, raw captured pointers, and encoded address fragments are not persistable. Pass process-local data as runtime arguments. `cacheLiteral<V>()` and arithmetic-only `cacheInvariant(value)` are explicit caller contracts: the value must be ordinary data independent of process/module addresses, address encodings, hashes, and fragments. They do not certify arbitrary captures or sanitize traced expressions. Incorrect certification can make persisted code incorrect. Internal certification is limited to fixed defaults/steps and `sizeof` scaling; folded-away introduction evidence and replay disagreements are retained. Ordinary compilation leaves recording and artifact preflight disabled.

Native function imports, runtime pointer arguments, internal functions, supported scalar arithmetic/control flow, and audited native exception/cleanup callbacks can round trip. The real `nautilus_alloca<T>()` introduction used by `val<T>` records typed `sizeof(T)`/`alignof(T)` evidence only when origin tracking is enabled. That evidence travels with the allocation table through replay, clones, regions, nested functions and IR conversion. Disagreement in introduction kind, C++ type identity or layout permanently removes it. Type text is diagnostic only; the type identity token exists only in frontend metadata and is never lowered or serialized as an address. Preflight checks every table entry, including unused entries, against its recorded layout before optimization. Typed owned structs with compatible native construction/copy/cleanup imports can therefore round trip. This certifies storage layout, not captured object state, arbitrary constructor arguments or scalar metadata introduced elsewhere.

Raw `traceAlloca(size, align)` and `addAllocaSpec(size, align)` remain uncertified even when their numbers match a typed allocation. The scalar-leaf predicate alone cannot certify allocation metadata; encoded addresses, raw scalar arguments, callbacks and destructor-only captures remain independently checked. Ordinary compilation performs no allocation-origin reconciliation or preflight. The application still owns argument storage and synchronization, keeps native dependency images loaded, and must request functions with the declared ABI. Artifacts are not serialization of arbitrary live JIT state.

## Trust and integrity

Artifacts are **trusted executable input produced under this contract**. SHA-256 covers descriptor content and each payload to detect corruption, not to authenticate authors or prove arbitrary object/MLIR semantics. Editing bytes and recomputing checksums is not a supported producer. Parsing and verifying MLIR cannot reconstruct discarded scalar provenance; lowered validation additionally rejects immediate non-null pointer constructions. Keep artifacts out of untrusted writable transport/storage.

The format bounds descriptors to 64 MiB, each payload to 1 GiB, and individual record counts to 100,000. It records compiler and producer image identities, LLVM version, actual host triple/CPU/features/data layout, pointer width/endianness, typed effective-option digest, and ordered intrinsic identities. Load options must match emission options; `engine.backend=mlir` is normalized with the artifact API's explicit MLIR default. There is no cross-version/CPU migration, filesystem-cache policy, arbitrary-object safety claim, or release claim for untested platforms/backends.
