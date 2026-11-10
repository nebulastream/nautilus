---
name: memory-check
description: Check that a change does not leak or pin memory in a long-running engine, with the arena chunk-pool counters, the 40,000-module probe and ASan/LeakSanitizer. Use when a change touches arenas, the chunk pool, module or executable ownership, promotion, caches or loaders, when RSS grows in an embedder, or before a PR that changes who owns compiler memory.
---

# Check memory use and leaks

Nautilus keeps compiler memory in arenas that take chunks from one pool per engine (`docs/plans/nautilus-0.4/02-nir.md`
§4.1). The pool is bounded by `engine.arena.retainedBytes`, gives idle chunks back to the OS, and counts what it holds.
A leak or a pinned peak shows in those counters before it shows in RSS, so read the counters first and the sanitizers
second.

## 1. The counters

`NautilusEngine::arenaStatistics()` returns `common::ChunkPoolStats`
(`nautilus/include/nautilus/common/ChunkProvider.hpp`):

| Counter | What a healthy engine shows |
|---|---|
| `inUseBytes` | back to 0 when no compile runs; it is the arenas that are alive |
| `retainedBytes` | at most `engine.arena.retainedBytes` plus a few chunks (per-thread caches overshoot) after any number of compiles |
| `heldBytes` | `inUseBytes + retainedBytes`: what the pool holds from the OS |
| `peakInUseBytes` | the largest compile so far; one huge compile must not stay in `retainedBytes` |
| `chunksTrimmed`, `chunksPagesReleased` | grow while the engine runs, so idle memory goes back |
| `dedicatedAllocations` | large requests; freed with their arena, never pooled |

`NautilusEngine::trimMemory()` returns every idle chunk: right after it `retainedBytes` is 0 and the engine keeps
working. Executables and modules are not in these counters: they are owned by `CompiledModule` and its function handles
(`nautilus/include/nautilus/Module.hpp`), so a module kept alive by a forgotten handle shows only in RSS and in the
leak report of §3.

## 2. Run the memory tests

```bash
cmake --build --preset dev --target nautilus-execution-tests
./build/dev/nautilus/test/execution-tests/nautilus-execution-tests "[arena]"
./build/dev/nautilus/test/execution-tests/nautilus-execution-tests "Promotion Service - 40000 Short-Lived Modules"
```

- `[arena]` is `nautilus/test/execution-tests/ArenaMemoryTest.cpp`: the budget, `trimMemory`, an embedder's chunk provider.
- The 40,000-module probe (`nautilus/test/execution-tests/PromotionServiceTest.cpp`) compiles that many short-lived
  modules on one engine, with promotions running, and requires the thread count at the pool size and the virtual memory
  (`VmSize` of `/proc/self/status`) flat after warm-up (less than 256 KiB growth). It runs a tenth of the modules under a
  sanitizer.

To probe your own change, copy the loop of that test: compile and drop N modules, call `waitForPromotions()` so the
workers are idle, then print `arenaStatistics()` and `VmSize` every N/10 iterations. Linear growth of either is the
finding; a plateau is not.

## 3. Run it under ASan and LeakSanitizer

```bash
cmake --preset asan                                   # includes MLIR; the first configure downloads it
cmake --build --preset asan --target nautilus-execution-tests
ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build/asan/nautilus --output-on-failure -R "arena|Short-Lived|Module"
```

LeakSanitizer reports at process exit, so a leak is the end of the output, not a failing assertion. Read the allocation
stack at the top of each report: a chunk allocated in `ChunkPool` that nothing returned means an arena outlived its
engine or a module kept an `Executable` it should not. Never add a suppression to make a leak go away; the shared ones are
`nautilus-sanitizer-suppressions` and are for third-party code.

## 4. Decide and report

| Observation | Meaning | Next step |
|---|---|---|
| `retainedBytes` above the budget by more than a few MiB after many compiles | the pool does not trim | read `ChunkPool.cpp` (`nautilus/src/nautilus/common/`) for the release path your change touched |
| `inUseBytes` not 0 when idle | an arena was not returned | find who holds the `Arena`: a module that kept IR, a promotion job that was not dropped |
| `VmSize` grows linearly in the probe, counters flat | memory outside the pool: executables, code buffers, handles | compare module count and executable lifetime; `CompiledModule::explain()` shows which tier a module holds |
| ASan reports a leak at exit | a real leak | fix the owner; do not suppress |
| everything flat, ASan clean | no regression | put the numbers before and after into the PR description |

For the cost of a change on the compile path use the `perf-investigation` skill; this one is about retention.
