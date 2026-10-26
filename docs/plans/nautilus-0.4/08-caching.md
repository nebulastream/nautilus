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
| **L0 in-process** | structural hash of the in-memory canonical module ([13 §7.3](13-nir-spec.md)), pointers in whatever mode they resolved to (`ProcessLocal` allowed) + backend + the binding environment (names and addresses) | after conversion | everything after conversion: the executable already in memory is reused | one process, in memory |
| **L1 semantic** | the L1 key (below): caller-supplied `engine.cache.key`, options, declared exports and declared imports (with each exporter's structural hash); the rest of the compatibility identity is checked on the stored entry | **before tracing** | tracing and conversion; code generation too when a native payload matches (otherwise recompiled from the NIR image) | persistent directory |
| **L2 structural** | structural hash of the canonical NIR image ([13 §7.2](13-nir-spec.md); names, not ids; portable pointers; code-affecting options; extension versions) + backend + LLVM target triple, data layout, target CPU and feature string (+ the spliced-bitcode hash for `inlining` payloads) | after conversion | code generation, via a matching native payload; a hit's payload is used only if its compatibility descriptor matches ([07 §2.2](07-serialization.md)), otherwise the entry's NIR image is recompiled | persistent directory |

- **Lookup order.** L1 is looked up before tracing; L0 (by the
  configured backend, §2 "One entry for all tiers") and then L2 after
  conversion; L1 and L2 look for the tier-1 backend's payload first
  ([11 §4.6](11-tiered-compilation.md)).
- **What L1 stores.** An L1 entry holds the canonical NIR image of every
  compiled module, whatever tier and backend compiled it, plus optional
  native payloads per backend; in 0.4 only tier 1 (MLIR) produces payloads,
  so L1 holds no tier-0 payload ([11 §4.6](11-tiered-compilation.md)). On an L1 miss the image (without a
  payload) is stored right after conversion, so a warm L1 hit skips tracing
  even when only tier 0 ever ran (`manual` promotion,
  `engine.backend=asmjit`). A tier-1 payload is added to the entry when
  promotion finishes; the entry is replaced atomically (§3).

### L1: semantic key (#528)

- **The caller's key must cover** interchangeable bodies, captures and
  specialization choices. Neither function names nor an IR hash replace it.
- **The L1 key** is computed before tracing, so it hashes only what is
  known before a trace exists. It is `XXH3_128bits_withSeed(input, length,
  0)` over:
  1. `engine.cache.key`: `u32 length`, bytes;
  2. the code-affecting options block: every option whose registry entry
     has `affectsCode` (M1.4a), with defaults filled in and `resolve` hooks
     applied (so `gpu.device=auto` is hashed as the device it resolved to at
     engine construction, [17 §4.1](17-gpu.md)), encoded exactly as
     item 2 of the structural hash ([13 §7.2](13-nir-spec.md)), so setting
     an option to its default changes nothing;
  3. application options (keys not in the registry), because Nautilus
     cannot tell whether traced code reads them: `u32` count, then per key,
     sorted by name bytes, `u32 nameLength`, name, `u32 valueLength` and the
     value's string form;
  4. the declared exports, the functions registered on the module, sorted by
     name bytes: `u32 nameLength`, name, and the signature's type words
     ([13 §4.5.1](13-nir-spec.md)) as the tracer derives them from the C++
     signature (`u32` result, `u32` count, parameters);
  5. the declared imports (`importFunction`, [13 §4.4.4](13-nir-spec.md)),
     known before tracing because the exporters are compiled first (the
     import graph is acyclic): `u32` count, then per import, sorted by
     (module name, function name) bytes, `u32` length + module name, `u32`
     length + function name, the import's signature type words (as in item
     4), and the exporter's 16-byte structural hash
     ([13 §7.2](13-nir-spec.md), over its self-contained image; computed
     once per compiled exporter and memoized). A module without imports
     writes count 0. Promoting an exporter does not change its structural
     hash, so it does not invalidate its importers' entries.

  The entry stores the key and `engine.cache.key` (descriptor keys 8 and
  7, [07 §2.2](07-serialization.md)); a hit compares both in full.
- **Checked on the stored entry at hit**, because they are either unknown
  before tracing or affect only payloads:
  - extensions (`name@version`): the image's `extensions` section must load
    against this process's registry ([13 §8.4](13-nir-spec.md), `NIR-L010`
    to `NIR-L012`; an unavailable type instance still loads and uses its
    fallbacks, unless it appears in a signature that needs a C ABI type,
    which fails `NIR-V035`); otherwise the hit is a miss with reason `incompatible`,
    and the module is traced and the entry replaced;
  - the actual core and cache-provider builds (descriptor keys 2 and 3);
  - per payload, the LLVM version, target triple, data layout, target CPU
    and feature string (`engine.mlir.targetCpu`,
    [07 §2](07-serialization.md)), and for code spliced by the `inlining`
    plugin's hooks the hash of the spliced bitcode
    ([07 §2.2](07-serialization.md)).

  Missing or changed identities fail closed. A changed option, export, import
  (including a changed exporter), extension or application option is a
  miss. A changed core or provider
  build, target, CPU, feature string or spliced bitcode makes only the
  entry's native payloads unusable: they are never executed, and the entry's NIR image is
  recompiled, still without tracing (D12, [07 §2.2](07-serialization.md)).
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
  NIR is about as cheap as loading, so they use L0 in-process and L1 (which
  stores their NIR image) to skip tracing.
- **Payloads that used the `inlining` hooks** join L2 only with the spliced
  bitcode's hash in the key; payloads that are not persistable
  ([07 §2](07-serialization.md)) are never stored, and their modules use L0
  only.
- **Collisions:** a 128-bit hash, plus a full image comparison on hit
  (images are small and flat).

### L0: in-process (new)

- **Same construction as L2** ([13 §7](13-nir-spec.md)), over the module
  with its pointers in whatever mode they were resolved. L0 and L2 keys
  therefore differ for modules with `ProcessLocal` pointers. Serves
  repeated compiles inside one process (operators recompiled per query)
  with no filesystem involved.
- **The L0 key also hashes the binding environment,** because in-process
  code keeps binding addresses as immediates
  ([07 §2.1](07-serialization.md), [05 §5.2](05-backends.md)): after the
  13 §7.3 hash and the backend name, a `u32` count, then per binding in
  `bindings` order its name bytes (`u32` length + bytes) and its `u64`
  address. Two queries with equal NIR but different operator state
  therefore miss each other.
- **An L0 entry owns** the executable together with its data-object
  placement ([13 §4.10](13-nir-spec.md)), its `funcaddr` stubs
  ([13 §5.7](13-nir-spec.md)), its import slots
  ([13 §4.4.2](13-nir-spec.md)) and its tier state. A module that hits L0
  shares them and holds a reference; promotion
  ([11 §4.6](11-tiered-compilation.md)) happens once per entry and every
  module that shares the entry sees it. The entry's code, data and stubs
  are freed when the last such module is released *and* the entry is
  evicted. Evicting an entry that modules still share only removes it from
  the lookup table. `CompiledModule::promote()` on any sharing module
  promotes the entry for all of them.
- **One entry for all tiers.** The backend in the L0 key is the module's
  configured backend (`engine.backend`, or under tiering the pair
  `engine.tier0.backend`/`engine.tier1.backend`), never the tier currently
  installed. Promotion installs tier 1 into the entry's tier state and
  never creates a second entry, so the data placement, stubs and import
  slots that every tier's code refers to belong to exactly one entry.
- **An L0 entry pins its exporters** like an importer
  ([13 §4.4.2](13-nir-spec.md)), so an import name in its key always
  refers to the module its slots are bound to. Registering a module under
  a name that only L0 entries still pin first evicts those entries.
- **Cost:** enabling L0 adds canonicalization (compaction, reordering,
  interning, itself a full rewrite) and the hash to the compile path. It is
  recorded as `cache.hashMs` ([10 §3](10-observability.md)) and budgeted at
  0.05 µs/op on the corpus (§5).
- **Bounded** by entry count and bytes, with LRU eviction.

### Modules with imports ([13 §4.4](13-nir-spec.md))

- **L0** keys an importer by its in-memory canonical form, where imports
  count only by name, signature and assumed attributes. Promoting an
  exporter does not invalidate it, and the exporter cannot be replaced
  while the entry pins it (§"L0: in-process").
- **L1 and L2** store self-contained artifacts, which bundle every import
  ([13 §8.5](13-nir-spec.md)). The L2 key covers the bundled bodies; the L1
  key covers each exporter's structural hash (item 5 of the L1 key). Either
  way a changed exporter makes its importers miss once and store new
  entries.
- **Inlined imports:** compiled code that inlined an imported function adds
  the exporter's structural hash to its key ([13 §4.4.3](13-nir-spec.md)).

## 3. Storage

Storage keeps #528's model:

- private, effective-user-owned directories and files;
- pinned directory descriptors; no symlink traversal;
- bounded reads;
- atomic, fsynced, manifest-last publication;
- per-key mutexes and cooperative file locks (`flock` on Linux and macOS,
  so the same code runs on both 0.4 platforms).

Caches are trusted executable input. There is no authentication and no
protection against hostile same-UID writers. There are no network-filesystem
guarantees.

### 3.1 Bounds, corruption and failures

- **Size bound.** `engine.cache.maxBytes` (default 1 GiB) bounds the
  directory. On store, entries are evicted oldest access first, under the
  exclusive directory lock, until the new entry fits. Access times are kept
  in the manifest, not taken from the file system (`noatime` mounts); a hit
  updates its entry's access time at most once per minute, under the
  entry's lock only, so hot hits do not serialize on the directory lock.
  The directory's byte total is kept in the manifest and recomputed by a
  scan if the manifest is missing or corrupt.
- **Eviction under concurrency.** Evicting an entry unlinks its manifest
  record first, then its file, while holding the entry's lock. A reader
  that already opened the file keeps reading it (POSIX unlink semantics, on
  Linux and macOS); a reader that loses the race finds no file and records
  a miss with reason `evicted`. An entry being written (its lock held) is
  never evicted.
- **Atomic replace.** A new entry is written to a uniquely named temporary
  file in the same directory, `fsync`ed, and published by `renameat` over
  the entry's name (atomic on Linux and macOS), then the directory is
  `fsync`ed, then the manifest. On macOS, `fsync` does not flush the drive
  cache; durability after power loss is not promised there, and a torn
  entry is caught by its checksum as corrupt.
- **Corrupt or incompatible entries** (checksum, descriptor, stale build
  identity, CPU mismatch) are a miss with their reason. The new result
  replaces the entry atomically (manifest last), so entries left behind by
  an upgrade are replaced on first use and otherwise age out by the size
  bound. For a stale payload with a valid image (D12) the image is still
  used, and only the payload is rebuilt.
- **Store failures** (`ENOSPC`, `EACCES`, a lock timeout) are a miss with
  reason `storeFailed`, never an error. The compiled module is returned
  normally.

## 4. Integration

- **Opt-in:** `ENABLE_CACHE_PLUGIN` (default off) builds the cache provider
  (`nautilus::nautilus-cache`), selected through the injected `JITCompiler`
  strategy (`nautilus::cache::createCompiler(options)`), as in #528. With no
  cache configured, nothing changes.
- **L0 and L2 are part of the same provider.** L0 can be enabled without a
  directory.
- **Ineligible modules** (provenance, [07 §5](07-serialization.md)) compile
  normally from the already-traced NIR. There is never a second trace.
- **Single flight.** Concurrent misses on the same L0 or L2 key are single
  flight: the second caller waits for the first's code generation, bounded
  by `engine.cache.waitMs`, and then compiles itself. L1 stores take the
  per-key file lock; the last writer wins.
- **Statistics:** for every lookup, the hit or miss level and a reason
  (keyless, ineligible, incompatible, cpuMismatch, corrupt, storeFailed,
  evicted, fallback). Failed requests stay in the denominator.

## 5. Exit criteria (M5)

- **Warm wall time ≤ 40% of cold wall time** on the NebulaStream suite
  (§5.1). This is what "60% faster" means: time −60%, not a 1.6x speedup.
  #529 measured −67% against `cold-disabled`.
- An L2 hit for a structurally equal module compiled under a different key.
- An L1 hit with `manual` promotion and no tier 1 skips tracing.
- An importer misses L1 after its exporter changed and was re-registered
  in a new process (L1 key item 5).
- At L0, two modules with equal NIR and binding names but different
  binding addresses miss each other; two modules that share an L0 entry
  keep running after either one is released, and the entry's code, data
  and stubs are freed only after both are released and the entry is
  evicted (ASan clean).
- All of #528's storage, compatibility and concurrency tests ported and
  green, plus tests for §3.1 and single flight: a truncated and a
  bit-flipped entry are a miss and are replaced; the size bound evicts the
  oldest entry; a read-only and a full directory (a small `tmpfs`) give
  `storeFailed` and a working module; an entry from another build or CPU is
  not executed; two threads missing on one key compile once.
- With L0 enabled, canonicalization plus hashing costs at most 0.05 µs/op
  on the `straight` and `loopBody` corpus cases (reference machine; CI
  tracks the instruction count, [10 §2](10-observability.md)).

### 5.1 Warm NebulaStream measurement

- **Cold:** the suite with the cache plugin enabled
  (`ENABLE_CACHE_PLUGIN=ON`) and an empty cache directory, so cold pays
  the store cost.
- **Warm:** a second run of the suite in fresh processes on the same
  directory.
- **Reported alongside:** `cold-disabled` (no cache plugin), #529's
  baseline.
- Both runs on the reference machine, ASLR on, fresh processes per query
  as in #529, with the same builds of Nautilus and NebulaStream (otherwise
  every payload is stale), default options (tiered, `engine.mlir.targetCpu
  = host`), and the OS page cache not dropped between the runs (as in #529).
- **Metric:** the suite's wall time, median of three cold/warm pairs, each
  pair on a fresh empty directory. The warm run's L1 hit rate and payload
  rejections (`cache.level`, `cache.payloadRejected`) are reported with
  it; a warm run with misses other than `keyless` does not count.
- **Harness:** a `nebulastream-cache` workflow (manual or nightly) that
  runs the suite cold and then warm, and the NebulaStream patch that sets
  `engine.cache.key` per query (the port of #529's NebulaStream branch).
  Both are part of M5.7; the existing `nebulastream.yml` runs the suite
  once, with no cache options.

## 6. Risks

- **An incomplete caller key** causes wrong reuse at L1. It is the caller's
  contract, documented with examples; debug mode can re-trace and compare
  on hit.
- **A cache directory shared across users** is unsupported; it is detected
  and rejected.
