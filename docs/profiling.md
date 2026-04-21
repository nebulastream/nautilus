# Nautilus Profiling

This document describes the design for Perfetto-based profiling support in
Nautilus. It covers the user-facing API, how profiling integrates with each
backend, and the in-process sampler that correlates CPU time with user-defined
regions.

Status: design, not yet implemented. Work happens on branch
`claude/plan-nautilus-perf-support-mdHBe`.

## Goals

1. Let users annotate traced Nautilus code with named regions
   (`Profiler::startRegion` / `Profiler::endRegion`) that appear as boxes on a
   Perfetto timeline.
2. Produce one `.perftrace` file per `NautilusModule`, loadable in
   `ui.perfetto.dev`.
3. Correlate regions with CPU samples so users can see which functions —
   including those reached through `invoke()` into native C++ — consume time
   inside a given region.
4. Work on Linux and macOS with no elevated privileges, no kernel tracer, and
   no external daemons.

## Public API

```cpp
namespace nautilus::profile {

void startRegion(val<const char*> name);
void endRegion(val<const char*> name);

class [[nodiscard]] ScopedRegion {
public:
    explicit ScopedRegion(val<const char*> name);
    ~ScopedRegion();
};

void traceCounter(val<const char*> name, val<int64_t> value);

// Thread registration for the sampler; auto-called from CompiledFunction.
void registerThisThread();
void unregisterThisThread();

} // namespace nautilus::profile
```

`Profiler::startRegion` and `endRegion` are ordinary traced calls. They lower
to `ProxyCallOperation` nodes that invoke C-ABI runtime functions. No new IR
operation types, no dialect work.

## Engine options

| Option                                | Default    | Description                                                              |
|---------------------------------------|------------|--------------------------------------------------------------------------|
| `profile.enabled`                     | `false`    | Master switch. When false, profiler calls are stripped during IR build.  |
| `profile.output_dir`                  | (unset)    | Directory to write per-module `.perftrace` files into.                   |
| `profile.auto_instrument_functions`   | `false`    | Wrap each registered function in a `ScopedRegion` named after it.        |
| `profile.auto_instrument_loops`       | `false`    | Insert regions around loops identified by `LoopAnalysisPhase`.           |
| `profile.sampler`                     | `"off"`    | One of `"off"`, `"inproc"`.                                              |
| `profile.sample_interval_us`          | `1000`     | Sample period in microseconds when sampler is `"inproc"`.                |
| `profile.sample_max_depth`            | `64`       | Max stack frames captured per sample.                                    |
| `profile.sample_ring_slots`           | `4096`     | Per-thread ring-buffer capacity (samples before drop-on-full).           |
| `profile.jit_perf_map`                | `false`    | Also emit `/tmp/perf-<pid>.map` for external `perf` / `sample`.          |

## Runtime library

A new subdirectory `src/nautilus/profile/` contains `libnautilus_profile`, a
thin wrapper over the Perfetto client SDK. It exposes a stable C ABI so every
backend reaches it via the same mechanism already used for any other runtime
call:

```c
void __nautilus_profile_begin(const char* name);
void __nautilus_profile_end(const char* name);
void __nautilus_profile_counter_i64(const char* name, int64_t value);
```

The Perfetto SDK is pulled in via `FetchContent` in a new
`cmake/Perfetto.cmake` module, gated on `ENABLE_PROFILE` (default `OFF`).
When the flag is off, the header-only `Profiler.hpp` expands to empty inline
functions and no runtime dependency is added.

## Perfetto output model

| Nautilus concept         | Perfetto concept                            |
|--------------------------|---------------------------------------------|
| `NautilusEngine` process | Perfetto process (pid)                      |
| `NautilusModule`         | Named Perfetto track                        |
| Registered function name | Default region name when auto-instrumented  |
| `ScopedRegion("X")`      | `TRACE_EVENT("nautilus", "X")` on the track |

`NautilusModule::flushTrace(path)` ends the session and writes the protobuf
file. Users drag it into `ui.perfetto.dev` as-is.

## Backend integration

Each backend already knows how to lower `ProxyCallOperation`. The profiler
calls piggy-back on that code path. The table below records what each backend
needs on top of the shared runtime library.

| Backend | Call lowering                                | Symbol resolution                                              | Unwind-info requirement for sampler |
|---------|----------------------------------------------|----------------------------------------------------------------|-------------------------------------|
| MLIR    | existing `func::CallOp` path                 | register runtime symbols via `LLVMBackendHooks::jitSymbolContributor` | EH frames via ORC `EHFrameRegistrationPlugin` (already on); fallback: force `"frame-pointer"="all"` |
| CPP     | existing function-pointer cast               | linker picks up via `SharedLibrary`                            | Default compile flags keep frame pointers; native `dladdr` resolves frames |
| BC      | existing dyncall dispatch                    | add to interpreter's symbol table at init                      | Not applicable — interpreter frames only appear as dispatch loop |
| AsmJIT  | **not supported — see Decisions**            | —                                                              | —                                   |

## In-process sampler (`profile.sampler=inproc`)

A dedicated pthread wakes on a timer and uses `pthread_kill(tid, SIGPROF)` to
target each registered thread in round-robin. The signal handler captures a
timestamp, walks the stack with `backtrace()`, and writes the raw frame
addresses into a per-thread SPSC ring buffer. A separate consumer thread
drains the rings, symbolicates via `dladdr` plus the JIT symbol map (see
below), and emits Perfetto `StreamingProfilePacket`s with interned
callstacks.

The handler must remain async-signal-safe: no `malloc`, no locks, no string
formatting. The only syscalls are `clock_gettime(CLOCK_MONOTONIC)` and
whatever `backtrace()` performs internally. `backtrace()` is primed once at
sampler startup to avoid lazy initialization inside the handler.

### JIT symbol map

One header exposes the contract between compiler backends and the
symbolicator:

```cpp
namespace nautilus::profile {

struct JitSymbol {
    uintptr_t addr;
    size_t size;
    const char* name;     // interned, process lifetime
    const char* module;   // module track name, interned
};

void registerJitCode(JitSymbol sym);
void unregisterJitCode(uintptr_t addr);

} // namespace
```

Backends call `registerJitCode()` when a function is finalized. Storage is an
immutable sorted-by-address snapshot behind a `std::atomic<const std::vector<JitSymbol>*>`.
Registration takes the existing `llvm_jit_mutex`, rebuilds the snapshot, and
atomically swaps it in. The consumer thread binary-searches snapshots without
locking. Old snapshots are retained until the next trace flush, then freed.

### Thread registration

`CompiledFunction::operator()` calls `registerThisThread()` before invoking
the JIT'd function and `unregisterThisThread()` on exit. Users who obtain a
raw function pointer via `getExecutable()` and dispatch it on their own
worker threads must call these APIs themselves.

## Platform notes

### Linux

`SIGPROF` + `pthread_kill` + `backtrace()` cover everything. For users who
want kernel-assisted high-fidelity profiling, `profile.jit_perf_map=true`
emits a perf map file that lets external `perf record -g` symbolicate frames
that land inside JIT'd code.

### macOS

The same sampler works unchanged. `SIGPROF`, `setitimer`, `pthread_kill`, and
`backtrace()` all exist on macOS with equivalent semantics. No entitlements,
no `task_for_pid`, no SIP concerns. `dladdr` resolves native frames; the JIT
map resolves the rest.

`sample(1)` and Instruments are not integrated. Users who prefer those tools
can still launch them externally, and `profile.jit_perf_map=true` gives them
something to symbolicate with, but the default in-process sampler is expected
to cover every use case.

## Correlating samples with regions

Because track events and callstack samples share a monotonic clock and end up
in the same `.perftrace` file, the Perfetto UI time-aligns them
automatically. Clicking a `ScopedRegion` box filters the flamegraph lane to
stacks collected during that interval. Calls into native C++ via `invoke()`
symbolicate through `dladdr` and appear in the flamegraph without any
additional wiring — this is the primary motivation for adding the sampler.

## Decisions

### D1. Skip AsmJIT backend support for profiling

The AsmJIT backend is out of scope for the initial profiling work. Users who
need profiling must select a different backend
(`engine.backend=mlir|cpp|bc`). This is documented as a known limitation
rather than addressed.

**Rationale.**

- The sampler requires stack frames to be walkable through JIT'd code.
  AsmJIT does not emit `.eh_frame` / `__eh_frame` unwind info by default, and
  does not enable frame-pointer preservation for every emitted function.
  Making its output walkable costs either a measurable
  tight-loop regression (forced frame pointers) or a non-trivial code-gen
  change (emitting DWARF unwind sections from `X64LoweringProvider` /
  `A64LoweringProvider`).
- AsmJIT is currently an experimental backend (see `CLAUDE.md`). MLIR is the
  primary production path.
- Attempting partial support — letting profiler calls work but accepting
  truncated stacks when samples land inside AsmJIT code — produces flamegraphs
  that silently omit AsmJIT time and mislead users.

**Consequences.**

- `ENABLE_PROFILE=ON` and `ENABLE_ASMJIT_BACKEND=ON` are both allowed at
  build time; they do not conflict at compile time.
- At runtime, if `profile.enabled=true` is set on an engine that would
  otherwise use AsmJIT, the engine falls back to the next available backend
  and logs a warning. The fallback order is MLIR, CPP, BC.
- `profile.sampler=inproc` will not install the sampler when the AsmJIT
  backend is active, even if other backends are also registered in the same
  process.

**Revisit when.**

- AsmJIT becomes a supported production backend, or
- a user requests profiling specifically on AsmJIT output, or
- AsmJIT upstream begins emitting unwind info, at which point enabling
  profiling for it becomes a small additive change.

## Rollout order

1. Runtime library with stub `spdlog` implementations of the C ABI; wire the
   CPP backend and add a test.
2. MLIR symbol registration via `jitSymbolContributor`; mirror the CPP test.
3. Replace stubs with real Perfetto SDK calls; add `flushTrace` and the
   per-module track model.
4. Auto-instrumentation at registration time.
5. JIT symbol map + MLIR `registerJitCode()` integration; verify that an
   external `perf record` produces useful flamegraphs.
6. In-process sampler: sampler thread, signal handler, ring buffer,
   consumer, `StreamingProfilePacket` emission.
7. Loop-level auto-instrumentation (optional, as a new IR phase).
8. Compile-time strip pass for zero-overhead production builds.

Steps 1–3 constitute the MVP. 4–6 add the sampler story. 7–8 are additive.
