# Component: Caching

Part of the [Nautilus 0.4 design](README.md) · Milestone M5 · Builds on #528
(persistent cache plugin) and #529 (measurements)

## 1. Purpose

Skip work that was already done: tracing (when the caller can identify the
module), and passes and code generation (when the IR is known). All caches
produce and consume the artifacts of [07-serialization.md](07-serialization.md).

## 2. Levels

| Level | Key | Lookup point | Hit skips | Scope |
|---|---|---|---|---|
| **L0 in-process** | hash of the canonical NIR image + backend + options; `ProcessLocal` pointers allowed | after conversion | everything after conversion: the executable already in memory is reused | one process, in memory |
| **L1 semantic** | caller-supplied `engine.cache.key` + compatibility identity | **before tracing** | tracing and conversion; code generation too when a native payload matches (otherwise recompiled from the NIR image) | persistent directory |
| **L2 structural** | hash of the canonical NIR image (names, not ids; portable pointers; options; extension versions) + backend | after conversion | code generation, via a matching native payload | persistent directory |

### L1: semantic key (#528)

- **The caller's key must cover** interchangeable bodies, captures and
  specialization choices. Neither function names nor an IR hash replace it.
- **The compatibility identity separately covers:**
  - the actual core and cache-provider builds;
  - the LLVM target and layout;
  - declared exports;
  - typed effective options, including arbitrary application options;
  - extensions (`name@version`).

  Missing or changed identities fail closed.
- **The caller provides the binding environment before lookup.** A native
  hit skips the callable, so captured handles and scalar certifications
  cannot be revalidated at hit time (#529).
- **Measured in #529:** 1500/1500 warm requests were native hits, with no
  tracing, IR, bytecode, optimization or code generation. The full
  NebulaStream suite took 87 s warm against 262 s cold-disabled, **−67%**.
- **Options:** `engine.cache.directory`, `engine.cache.key`. The deprecated
  aliases `engine.Blob.CacheDir` and `engine.Blob.CacheKey` are normalized;
  conflicting values are rejected before tracing.

### L2: structural key (new)

- **No caller key needed, and safe by construction:** NIR fully determines
  the generated code, and the key includes options, extension versions and
  the backend.
- **Cheap:** hashing a flat image is one linear pass over contiguous memory
  after canonicalization ([02 §3.6](02-nir.md)).
- **Catches what L1 cannot express:** structurally equal queries from
  different call sites or sessions.
- **Only for backends with native payloads** (MLIR/LLVM in 0.4,
  [07 §2](07-serialization.md)). For the other backends, recompiling from
  NIR is about as cheap as loading, so they use L0 in-process and L1 to
  skip tracing.
- **Collisions:** a 128-bit hash, plus a full image comparison on hit
  (images are small and flat).

### L0: in-process (new)

- **Same key as L2,** with process-local pointers allowed. Serves repeated
  compiles inside one process (operators recompiled per query) with no
  filesystem involved.
- **Bounded** by entry count and bytes, with LRU eviction.

## 3. Storage

Storage keeps #528's model:

- private, effective-user-owned directories and files;
- pinned directory descriptors; no symlink traversal;
- bounded reads;
- atomic, fsynced, manifest-last publication;
- per-key mutexes and cooperative Linux file locks.

Caches are trusted executable input. There is no authentication and no
protection against hostile same-UID writers. There are no network-filesystem
guarantees. Persistent eviction is not in 0.4.

## 4. Integration

- **Opt-in:** `ENABLE_CACHE_PLUGIN` (default off) builds the cache provider
  (`nautilus::nautilus-cache`), selected through the injected `JITCompiler`
  strategy (`nautilus::cache::createCompiler(options)`), as in #528. With no
  cache configured, nothing changes.
- **L0 and L2 are part of the same provider.** L0 can be enabled without a
  directory.
- **Ineligible modules** (provenance, [07 §5](07-serialization.md)) compile
  normally from the already-traced NIR. There is never a second trace.
- **Statistics:** for every lookup, the hit or miss level and a reason
  (keyless, ineligible, incompatible, corrupt, fallback). Failed requests
  stay in the denominator.

## 5. Exit criteria (M5)

- Warm NebulaStream suite at least 60% faster than cold, measured like #529
  (fresh processes, ASLR, empty cold caches).
- An L2 hit for a structurally equal module compiled under a different key.
- All of #528's storage, compatibility and concurrency tests ported and
  green.

## 6. Risks

- **An incomplete caller key** causes wrong reuse at L1. It is the caller's
  contract, documented with examples; debug mode can re-trace and compare
  on hit.
- **A cache directory shared across users** is unsupported; it is detected
  and rejected.
