# Profiling JIT-compiled code

A JIT compiler is invisible to a sampling profiler by default. The code Nautilus
generates lives in anonymous memory that no ELF file on disk describes, so a
profiler collects samples for it and then has nothing to resolve them against —
the profile shows raw addresses under whatever host frame called into the
compiled function, and the part of the program you wanted to measure is the one
part you cannot see.

Nautilus solves that twice over, for two different ways of asking the question.

**In process**, the profiling plugin (`nautilus-profiling`) measures code you bracket
yourself: hardware counters around exactly one pipeline, and a sampling profiler
that resolves JIT frames to `region()`-qualified names. Nothing external is
involved and the result is a value your program can read.

**Out of process**, the `perf` engine option makes the MLIR backend write the
Linux **jitdump** protocol, so `perf record` + `perf inject --jit` produce a
profile in which JIT frames are symbolized and attributed all the way down to
individual Nautilus IR operations.

|  | in process (the plugin) | out of process (`perf record`) |
|---|---|---|
| Needs | nothing but the plugin | `perf record -k mono`, `perf inject --jit`, the IR dump kept on disk |
| Gives | percentages **and** absolute counter totals, readable programmatically | percentages, per-IR-operation annotation, flame graphs |
| Best for | measuring one pipeline, a regression test, a number to print | reading a whole run, annotating down to an IR line |

They share their symbol machinery, so a region is called the same thing either
way, and they compose: counters give a window's exact totals while sampling —
by either route — gives the breakdown within it.

Both are Linux-only. Off Linux the `perf` option is a warning-and-continue no-op
and the plugin reports itself permanently unavailable, so the same application
code builds and runs everywhere.

**In process, with the profiling plugin**

- [In process, with the profiling plugin](#in-process-with-the-profiling-plugin)
- [Availability](#availability)
- [Counting hardware events](#counting-hardware-events)
- [Sampling](#sampling)
- [Down to individual IR operations, in process](#down-to-individual-ir-operations-in-process)
- [Measuring a thread pool](#measuring-a-thread-pool)
- [How JIT symbolization works](#how-jit-symbolization-works)
- [Composing counters and sampling](#composing-counters-and-sampling)

**Out of process, with `perf record`**

- [Out of process, with `perf record`](#out-of-process-with-perf-record)
- [The demos](#the-demos)
- [Reading the profile](#reading-the-profile)
- [Down to individual IR operations](#down-to-individual-ir-operations)

**Both**

- [Flame graphs with nested regions](#flame-graphs-with-nested-regions)
- [Why regions are not native perf frames](#why-regions-are-not-native-perf-frames)
- [Where the files go](#where-the-files-go)
- [Troubleshooting](#troubleshooting)
- [Caveats](#caveats)

## In process, with the profiling plugin

The profiling plugin (`nautilus-profiling`) measures Nautilus pipelines from **inside the running process**: hardware counters around code you bracket yourself, and a sampling profiler that knows how to name JIT-compiled code.

```cpp
#include <nautilus/profiling/counters.hpp>

profiling::CounterSet counters;
counters.start();
auto result = pipeline(input);   // a compiled nautilus function
counters.stop();
std::cout << counters.result().table();
```

```
event             total
seconds           0.013
cycles         51394890
instructions   93539054
cache-misses       5524
branch-misses     25594
```

Enable it with `-DENABLE_PROFILING_PLUGIN=ON` (default) and link `nautilus-profiling`. Unlike the SIMD, std, specialization and builtin plugins there is no force-link header to include: the plugin registers no backend intrinsics, so linking the library is all it takes.

It is built on [perf-cpp](https://github.com/jmuehlig/perf-cpp), vendored unpatched under `third_party/perf-cpp/` alongside nautilus's other dependencies (see [`VENDORING.md`](../third_party/perf-cpp/VENDORING.md) there for the pinned version, what was pruned, and how to re-sync).

Everything in this half is **Linux-only** — `perf_event_open(2)` does not exist elsewhere — but the headers and the library still build on every platform, reporting themselves permanently unavailable. Application code that measures itself on Linux compiles unchanged on macOS.

## Availability

Hardware counters are a privileged, environment-dependent resource. A container, a hardened `perf_event_paranoid`, a VM without a virtualized PMU, or a non-Linux host all deny them through no fault of the caller.

**Nothing in this plugin throws for that.** Every entry point degrades to a no-op and reports why:

```cpp
#include <nautilus/profiling/availability.hpp>

if (!profiling::available()) {
    std::cerr << "not measuring: " << profiling::unavailableReason() << "\n";
}
```

`profiling::support()` returns the full verdict, probed once per process:

| `Availability` | Meaning |
|---|---|
| `Available` | `perf_event_open` succeeded on a probe event. |
| `UnsupportedPlatform` | Not Linux; there is no perf subsystem to talk to. |
| `PermissionDenied` | The kernel refused the probe (`EACCES`/`EPERM`) — almost always `perf_event_paranoid`, or a seccomp filter. |
| `Unsupported` | The kernel or hardware has no such event (`ENODEV`/`ENOENT`/`ENOSYS`) — typical in a VM with no virtualized PMU. |
| `Unknown` | The probe failed for some other reason. |

The probe is a real `perf_event_open` for a hardware instruction counter, immediately closed again. Reading `/proc/sys/kernel/perf_event_paranoid` alone is not conclusive — container and seccomp policies deny the syscall at a level the sysctl does not describe — so the sysctl is read only to *explain* a denial, never to predict one.

User-space counting needs `perf_event_paranoid` at **2 or lower**. Measuring kernel time would need 1 or lower; the plugin never asks for it.

Per-instance failures are reported the same way, so an unknown event name or an exhausted PMU is never fatal:

```cpp
profiling::CounterSet counters {{"instructions", "not-a-real-event"}};
counters.events();               // {"instructions"} -- the good one survives
counters.unavailableReason();    // "event 'not-a-real-event' dropped: ..."
```

In tests, skip rather than fail — most CI runners forbid counters, and a suite that failed there would be testing the runner:

```cpp
const auto& support = profiling::support();
if (!support) {
    SKIP("perf counters unavailable: " << support.reason);
}
```

## Counting hardware events

`profiling::CounterSet` is `perf stat` scoped to exactly the code you bracket. It is deliberately **not** tied to a compiled function — anything may sit between `start()` and `stop()`:

```cpp
profiling::CounterSet counters {{"cycles", "instructions", "cache-misses"}};
counters.start();
auto partial = scanPipeline(input);      // a compiled nautilus function
hostSideMerge(partial);                  // ordinary host code
auto out = aggregatePipeline(partial);   // another compiled function, another engine
counters.stop();
```

**Counting perturbs nothing.** The counters are programmed before the window and read after it, so the code in between is the code you would have shipped — unmodified and unobserved. That is the difference from instrumenting generated code, and it is why the numbers describe production behaviour rather than a measurement artifact.

### Events

The default set is `seconds`, `cycles`, `instructions`, `cache-misses` and `branch-misses`: four hardware events plus a software timer, which fits the generic PMU slots of every supported microarchitecture without multiplexing, so values come back exact rather than extrapolated.

Any event `perf list` knows can be named, as can derived metrics such as `cycles-per-instruction`. Values are `double` because that is what the kernel returns once a counter has been scaled for multiplexing, and because a ratio is not integral.

> **Request more events than the PMU has slots for** and the kernel time-slices them, reporting extrapolated values. Nothing reports this as an error — keep the set small when the numbers matter.

### Reading the result

| Method | Returns |
|---|---|
| `result().get("cycles")` | `std::optional<double>` for one event |
| `result().entries()` | every event and its value |
| `result().table()` | a human-readable table |
| `result().csv()` | `event,value` rows |

`reset()` discards a latched result while leaving the counters programmed, so one `CounterSet` can measure several windows in turn.

`CounterSet` is not thread-safe and counts **the calling thread only**. A window spanning worker threads needs one `CounterSet` per thread.

## Sampling

`profiling::Sampler` has the same shape — `start()`, arbitrary work, `stop()` — and produces what `perf report` produces:

```cpp
#include <nautilus/profiling/sampler.hpp>

engine::Options options;
options.setOption("perf.sample", true);
options.setOption("engine.backend", std::string("mlir"));
engine::NautilusEngine engine(options);

auto pipeline = engine.registerFunction(query);
pipeline(input);                  // warm up -- see below

profiling::Sampler sampler;
sampler.start();
pipeline(input);
sampler.stop();
std::cout << sampler.report().table();
```

```
# 700 samples,  99.86% in JIT-compiled code
# overhead  origin      symbol
   90.14%  jit         execute::query::aggregate::mix
    7.14%  jit         execute::query::scan
    2.57%  jit         execute::query::aggregate::merge
    0.14%  unresolved  0xffffffff9fdcdccd
```

Those are the same region-qualified names `perf report` shows after a `perf inject --jit` pass, produced without `perf record`, without `-k mono`, without `perf inject`, and without the `nautilus_debug_*.ir` dump having to still exist.

### Three things to get right

- **`perf.sample` on the engine options.** Without it the backend publishes no code ranges and JIT frames come back as bare addresses under `unresolved`.
- **`engine.backend=mlir`.** The default tiered compiler starts every function on an interpreting tier-0 backend that publishes nothing, exactly as it emits no jitdump.
- **Warm up before the window.** LLJIT materializes lazily, so the first call is what triggers LLVM codegen — and that compile runs *in this process*. Sampling across it profiles the compiler, not the compiled code; a cold first run yields a report full of `llvm::SelectionDAG` frames.

`report().total()` and the table's JIT share make all three visible, so none of these mistakes is silent.

> **Give the sampler something to sample.** A loop that merely accumulates `i` has a closed form, and LLVM's scalar evolution replaces the whole thing with one multiply — leaving a kernel that finishes before a single sample fires and a profile with nothing in it. A mixing chain (shift/xor/multiply) or a memory-dependent loop survives `-O3`.

### Options

| Field | Default | Meaning |
|---|---|---|
| `event` | `"cycles"` | The event whose overflow triggers a sample. |
| `period` | `100000` | Sample every Nth event. Lower means more samples and more overhead. |
| `callchain` | `false` | Record each sample's call stack as well as its leaf address. Needs frame pointers, which a `perf.sample` compile requests. |
| `bufferPages` | `64` | Ring-buffer size. Samples overflowing it are dropped by the kernel; raise it for a long window or a short period. |

A short window needs a shorter period than the default: one kernel invocation at `period = 100000` may collect too few samples for the per-region shares to be stable.

### Profiling something other than time

Change the trigger and the same report becomes a per-region breakdown of that event:

```cpp
profiling::Sampler sampler {{"cache-misses", /*period=*/1000}};
```

This is how to get per-region cache behaviour without instrumenting anything: the region symbols are already there, so any PMU event can be attributed through them.

### Reading the report

| Method | Returns |
|---|---|
| `sites()` | every symbol and its sample count, hottest first |
| `total()` | every sample taken, including unresolved ones |
| `jitSamples()` | samples that landed in nautilus JIT-compiled code |
| `table(top)` | a `perf report`-shaped table of the hottest sites |
| `csv()` | `share,samples,origin,module,symbol` rows |
| `foldedStacks()` | folded stacks for `flamegraph.pl` / `inferno-flamegraph` |
| `flameGraph()` / `writeFlameGraph(path)` | a self-contained flame graph, as SVG |
| `stacks()` | the distinct call stacks behind the profile |
| `sourceLines()` / `annotateSource()` | samples per Nautilus-IR line, and the dump with a percentage gutter |

Each `SampleSite` carries a `SymbolOrigin`: `Jit` (a nautilus code range), `Host` (an ordinary ELF symbol) or `Unresolved`, and -- for JIT frames -- the `CompilationUnitID` of the compile it came from in `module`. `modules()` lists the distinct compiles a profile spans.

That module identity is load-bearing once more than one pipeline is live. Every single-function registration lowers to a function named literally `execute`, so two modules compiled in one process produce identically named ranges at different addresses. Samples are therefore aggregated by **module and name**, not by name alone -- otherwise two unrelated pipelines' costs would be silently added together in one row.

The name is only *printed* with its module when it has to be:

```
   61.20%  jit   execute::query @AAAAAAA
   32.80%  jit   execute::query @BBBBBBB
```

A profile holding one module reads exactly as it always did. The suffix is the random part of the `CompilationUnitID`, which is also what names that compile's dump directory and what `compilation.unitId` reports in `CompilationStatistics` -- so a hot row can be traced back to the IR it came from. `site.module` always carries the full id, qualified or not.

Unresolved samples are kept rather than dropped — an unattributed share is information, usually the kernel or a compile that ran without `perf.sample`, and discarding it would make the remaining percentages add up to something misleading.

### Down to individual IR operations, in process

`table()` says which region is hot; `annotateSource()` says which IR operation
inside it is:

```cpp
std::cout << sampler.report().annotateSource(/*path=*/{}, /*context=*/3);
```

```
# annotation of nautilus_debug_212861_0.ir
# 99.73% of all samples land in this file; the rest is host code,
# the kernel, and the in-process MLIR/LLVM compile.
# columns: percent of all samples | IR line | IR
#
       | ... |
       |   75 | 	$57 = 13 :i32
       |   76 | 	$58 = $57 cast_to i64 :i64
       |   77 | 	$59 = $55 << $58 :i64
 22.67 |   78 | 	$60 = $55 + $59 :i64
       |   79 | 	$61 = 7 :i32
       |   80 | 	$62 = $61 cast_to i64 :i64
 13.07 |   81 | 	$63 = $60 >> $62 :i64
       | ... |
```

This is the in-process counterpart of
[`nautilus-perf.sh annotate-ir`](#the-whole-ir-dump-annotated), and it reads the
same way: the "source" is the Nautilus IR, not your C++, and the percentages are
shares of the **whole** profile -- the same scale `table()` uses -- so they do
not sum to 100 over one file. Cost also clusters on a few operations rather than
spreading along a dependency chain, for the ordinary out-of-order reasons
described there.

What it does not need is `perf report`. A `perf.sample` compile publishes its
DWARF line table alongside the code ranges, so the sampler can attribute an
address to an IR line directly; `sourceFile()` is the dump the samples point at,
which is what `annotateSource()` reads when given no path.

The dump is the `nautilus_debug_<pid>_<n>.ir` file the compile leaves in the
**working directory** (see [Where the files go](#where-the-files-go)). Nothing
cleans it up and nothing stops it being moved, so if it is gone by the time the
profile is read, `annotateSource()` says so rather than failing. Pass a
`context` window to elide the cold stretches of a dump that runs to thousands of
lines.

## Measuring a thread pool

`start()` and `stop()` always act on **the calling thread**, because that is what
the perf subsystem can do: a counter is opened against the thread that opens it
and does not follow threads spawned later. Measuring a pool is therefore the
same two calls, made from each worker:

```cpp
profiling::CounterSet counters;
profiling::Sampler sampler;

// in each worker -- no thread index anywhere:
{
    profiling::CounterSet::Scope counterScope {counters};
    profiling::Sampler::Scope sampleScope {sampler};
    runPipelineSlice();
}

// once the pool has joined:
counters.stopAll();
sampler.stopAll();

counters.result();             // summed over every measured thread
counters.perThreadResults();   // one entry per thread, in first-start order
sampler.report();              // every thread's samples, merged
```

There is no separate type for the parallel case and no slot to assign: which
counters a call touches follows from which thread is asking. A thread keeps its
counters across windows, so a worker measuring one window after another costs
nothing extra.

Three things to know about the aggregates:

- **Counters sum across threads.** That includes time events, so `seconds` over
  N busy workers reports roughly N times wall clock -- thread-seconds, not
  elapsed time. Derived metrics are recomputed from the summed events rather
  than averaged, so `cycles-per-instruction` stays meaningful.
- **Samples merge across threads**, so a region's share is its share of all
  measured work rather than of any one thread's. `perThreadResults()` on the
  counter side is what finds the one worker that behaved differently.
- **Capacity is bounded.** `maxThreads` (default
  `std::thread::hardware_concurrency()`) caps how many threads can be measured
  at once, because perf-cpp sizes its slot vector at construction. Unused
  capacity is nearly free for counters -- a slot opens no file descriptor until
  it starts -- but a sampler slot costs a ring buffer once started, so size an
  oversubscribed pool deliberately. A thread that finds no free slot is not
  measured and says so through `unavailableReason()`, rather than quietly
  shrinking the total.

### Threads that come and go

A thread keeps its slot for the life of the `CounterSet`, and `std::thread::id`
may be recycled once a thread exits. A brand-new thread that inherits a dead
one's id would therefore inherit counters bound to a thread that no longer
exists, and silently measure nothing. Call `reset()` when the set of measured
threads changes -- a pool being torn down and replaced -- which closes every
slot so the next thread to arrive opens fresh ones. Steady pools, and anything
single-threaded, never have to think about it.

## How JIT symbolization works

Generated code lives in anonymous memory that no ELF file on disk describes. A symbolizer reading `/proc/self/maps` — which is what perf-cpp's `util::SymbolResolver` does, correctly, for host code — finds nothing to name it by. Out of process, `perf inject --jit` solves this by synthesizing an ELF per code range from the jitdump; in process, there is no such step.

So the plugin resolves in two layers:

1. **`nautilus::compiler::JitSymbolRegistry`** (`nautilus/compiler/JitSymbolRegistry.hpp`), a process-wide, thread-safe map from code address to name. When `perf.sample` is set, the MLIR backend publishes every linked code range into it, named exactly the way the jitdump writer would name it — including `region()` nesting recovered from the DWARF inline stack.
2. **perf-cpp's ELF resolver**, for everything the registry does not claim.

The registry is public, so it is queryable directly — useful for asserting that a compile published what you expected:

```cpp
#include <nautilus/compiler/JitSymbolRegistry.hpp>

for (const auto& symbol : compiler::JitSymbolRegistry::instance().snapshot()) {
    std::cout << symbol.name << " (" << symbol.size() << " bytes)\n";
}
```

Every published range is stamped with an interned handle for its compile, and the owning `MLIRJit` withdraws its module's ranges when it is destroyed -- which is the moment the code they describe is actually freed. That keeps a long-running engine from accumulating ranges for dead code, and stops a later compile landing on the same addresses from being resolved to its predecessor's names.

Publication is a separate JITLink plugin from the jitdump writer, gated on a separate option, so either, both or neither may be active and neither perturbs the other. What they share is `JitCodeRanges`, the part with all the subtlety in it — how a range gets split and named. [docs/region.md](region.md) explains how a region reaches DWARF in the first place; [below](#why-regions-are-not-native-perf-frames) explains why the nesting travels in the *name* rather than as a scope tree.

## Composing counters and sampling

The two bracket the same window and answer different halves of one question: `CounterSet` gives its exact totals, `Sampler` gives the breakdown within it.

```cpp
counters.start();  pipeline(input);  counters.stop();
sampler.start();   pipeline(input);  sampler.stop();

const double totalCycles = *counters.result().get("cycles");
for (const auto& site : sampler.report().sites()) {
    std::cout << site.symbol << ": " << totalCycles * site.share / 100.0 << " cycles\n";
}
```

That yields **absolute per-region numbers with no instrumentation in the generated code at all** — statistical error from the sampling, but no bias, and the measured code is bit-identical to what ships. Run them over separate windows as above, or over one window if the sampler's overhead is acceptable inside the counted region.

## Out of process, with `perf record`

Two options on the `Options` your application configures Nautilus with:

```cpp
engine::Options options;
options.setOption("perf", true);
options.setOption("engine.backend", std::string("mlir"));
engine::NautilusEngine engine(options);
```

and then, around the run:

```sh
JITDUMPDIR=$PWD perf record -k mono -g --call-graph fp -- ./your_app
perf inject --jit -i perf.data -o perf.jit.data
perf report -i perf.jit.data
perf annotate -i perf.jit.data --stdio <symbol>
```

`tools/nautilus-perf.sh` wraps exactly this:

```sh
tools/nautilus-perf.sh record -- ./your_app        # record + inject in one step
tools/nautilus-perf.sh report                      # perf report on perf.jit.data
tools/nautilus-perf.sh annotate 'execute::query'   # perf annotate one symbol
tools/nautilus-perf.sh annotate-ir                 # the whole IR dump, annotated
tools/nautilus-perf.sh flamegraph perf.jit.data flame.svg
```

Four things in that recipe are not optional:

- **`perf` on the engine options.** Without it nothing is written at all: no
  jitdump file, no symbols, and `perf inject --jit` has nothing to inject.
- **`-k mono`.** `perf inject --jit` correlates the jitdump's timestamps against
  the `perf.data` timeline using the monotonic clock, and refuses to run without
  it: `error, jitted code must be sampled with perf record -k 1`.
- **`engine.backend=mlir`.** The default is tiered compilation, which starts
  every function on an interpreting tier-0 backend (`asmjit`/`bc`) that emits no
  jitdump at all and promotes to MLIR in the background — so a profile would be
  split across two artifacts at two addresses, only one of which perf can name.
  See [Tiered compilation splits the profile](#tiered-compilation-splits-the-profile).
- **Staying in the same working directory.** `perf` on its own (without `debug`)
  writes the Nautilus-IR dump its line numbers point into to the *working*
  directory rather than `$TMPDIR`, precisely so it outlives the compile. That
  file has to still be there, unmoved, when `perf annotate` runs — which can be
  minutes or hours later. See [Where the files go](#where-the-files-go).

## The demo

[`example/src/DemoProfiling.cpp`](../example/src/DemoProfiling.cpp) is a
query-shaped kernel measured **in process**. Its work is split across nested
`region()`s of deliberately unequal cost, so the profile has an obviously right
answer to check the tooling against.

```
query                    the whole pipeline
  scan                   one cheap pass: load + predicate
  aggregate              the expensive half
    mix                  per-element hashing -- most of the time
    merge                a strided pass that calls out to the host
```

`mix` calls a separately registered Nautilus function, which the MLIR inliner
pulls back into the loop at `-O3` -- so its body is attributed to the region it
was inlined *into* while its operations keep their own IR lines. `merge` reaches
a `noinline` host function through `invoke()`, which puts a genuine **host**
frame under a JIT one: that exercises the second symbol-resolution path, and is
the case a frame-pointer unwind out of JIT code has to get right.

```sh
cd example
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target demo_profiling -j
./build/demo_profiling
```

It prints counter totals for exactly one call, the per-region breakdown, the IR
dump annotated with those samples, and writes `nautilus-flame.svg` beside
itself:

```
# 978 samples,  78.94% in JIT-compiled code
# overhead  origin      symbol
   67.38%  jit         execute::query::aggregate::mix
   20.65%  host        mergeAccumulators(long, long)
    6.85%  jit         execute::query::scan
    4.70%  jit         execute::query::aggregate::merge
```

Pass smaller arguments (`./build/demo_profiling 1000 64`) for a smoke run that
only checks it works.

There is deliberately **no demo for the out-of-process path**. That path needs
none of a demo's code -- it is an engine option and a `perf` recipe around
whatever application you already have -- and a binary that profiled itself while
`perf` recorded it would mostly show its own profiler: naming host frames means
parsing the binary's symbol table, which dominates a run that tries to do both.
The outputs below come from this kernel compiled with `perf` set instead.

## Reading the profile

`perf report` groups samples by symbol, and with `perf` set those symbols are
Nautilus's own. A function registered as `execute` that wraps its work in regions
produces one symbol per region, qualified by its nesting. From the demo:

```
# Overhead  Command         Shared Object        Symbol
# ........  ..............  ...................  ....................................
    50.29%  demo_profiling  jitted-236351-8.so   [.] execute::query::aggregate::mix
    13.08%  demo_profiling  demo_profiling       [.] mergeAccumulators(long, long)
     3.98%  demo_profiling  jitted-236351-10.so  [.] execute::query::aggregate::merge
     3.51%  demo_profiling  jitted-236351-7.so   [.] execute::query::scan
     2.41%  demo_profiling  [kernel.kallsyms]    [k] read_hpet
```

Nesting is not depth-limited: an outer per-query region containing a per-operator
region containing a per-loop region gets all three levels in the name, in source
order.

The `jitted-<pid>-<n>.so` files in the "Shared Object" column are synthesized by
`perf inject --jit` — one per code range, built from the jitdump. They are not
something Nautilus writes.

With `-g` (which `nautilus-perf.sh record` passes), the call graph resolves
through the JIT frame into the host callee:

```
    13.08%  demo_profiling  demo_profiling       [.] mergeAccumulators(long, long)
            |
            ---mergeAccumulators(long, long)
               |
                --12.94%--execute::query::aggregate::merge
                          main
                          __libc_start_main
                          _start
```

## Down to individual IR operations

`perf annotate` goes one level further and attributes cycles to individual
Nautilus IR operations, because the DWARF line table in the jitdump points into
the Nautilus-IR dump written next to `perf.data`:

```sh
tools/nautilus-perf.sh annotate 'execute::query::aggregate::mix'
```

```
         : 28 mixStep($1:i64, $2:i64) :i64 [readnone, willreturn, nounwind]  ; at .../DemoProfiling.cpp:91:18 {
         : 29 Block_0($1:i64, $2:i64):
         : 30 $3 = 2654435761 :i64
         : 31 $4 = $2 * $3 :i64
    0.00 :   b2:     imul   %rsi,%r8
         : 33 $5 = $1 ^ $4 :i64
    0.00 :   b6:     xor    %rax,%r8
         : 35 $7 = 13 :i32
         : 36 $8 = $7 cast_to i64 :i64
         : 37 $9 = $5 << $8 :i64
         : 38 $10 = $5 + $9 :i64
   14.68 :   b9:     mov    %r8,%rax
    0.04 :   bc:     shl    $0xd,%rax
   10.85 :   c0:     add    %r8,%rax
```

Two things are worth noticing in that output. The "source" interleaved with the
disassembly is the Nautilus IR, not your C++ — that is what the DWARF points at.
And the attribution survives inlining: `mixStep` was a separately registered
Nautilus function that the MLIR inliner pulled into the `mix` loop, and its
operations still carry their own lines.

`perf annotate -l` prints a per-line summary first, which is the fastest way to
find the hot operation:

```
   31.44 nautilus_debug_15257_0.ir:138
   25.53 nautilus_debug_15257_0.ir:127
   16.89 nautilus_debug_15257_0.ir:130
```

### The whole IR dump, annotated

`perf annotate` is scoped to one symbol, and its percentages are local to that
symbol — so two regions' numbers are not comparable, and reading a whole module
means running it once per symbol and mentally re-weighting each result.
`nautilus-perf.sh annotate-ir` gives the other view: the entire Nautilus-IR dump,
every line, with one globally-weighted percentage gutter.

```sh
tools/nautilus-perf.sh annotate-ir              # finds the single IR dump in $PWD
tools/nautilus-perf.sh annotate-ir perf.jit.data nautilus_debug_15257_0.ir
```

```
# perf annotation of nautilus_debug_15257_0.ir
# 79.93% of all samples land in this file; the rest is host code,
# the kernel, and the in-process MLIR/LLVM compile.
# columns: percent of all samples | IR line | IR
#
        |  104 | 	$72 = load($69) :i64
   2.87 |  105 | 	$73 = call 0x56fcdd5a7780#1($6,$72) :i64
        |  106 | 	$74 = 8 :i32
...
        |  119 | mixStep($1:i64, $2:i64) :i64 [readnone, willreturn, nounwind]  ; at .../DemoProfiling.cpp:91:18 {
        |  120 | Block_0($1:i64, $2:i64):
        |  121 | 	$3 = 2654435761 :i64
        |  122 | 	$4 = $2 * $3 :i64
        |  123 | 	$5 = $1 ^ $4 :i64
        |  124 | 	$7 = 13 :i32
        |  125 | 	$8 = $7 cast_to i64 :i64
        |  126 | 	$9 = $5 << $8 :i64
  18.53 |  127 | 	$10 = $5 + $9 :i64
        |  128 | 	$11 = 7 :i32
        |  129 | 	$12 = $11 cast_to i64 :i64
  12.24 |  130 | 	$13 = $10 >> $12 :i64
   0.06 |  131 | 	$14 = $10 ^ $13 :i64
        |  132 | 	$15 = 3 :i32
        |  133 | 	$16 = $15 cast_to i64 :i64
        |  134 | 	$17 = $14 << $16 :i64
   9.86 |  135 | 	$18 = $14 + $17 :i64
        |  136 | 	$19 = 17 :i32
        |  137 | 	$20 = $19 cast_to i64 :i64
  22.79 |  138 | 	$21 = $18 >> $20 :i64
   8.16 |  139 | 	$22 = $18 ^ $21 :i64
        |  140 | 	return ($22) :i64
        |  141 | }
        |  142 | ; region #0 = "query" at .../DemoProfiling.cpp:100:2
        |  143 | ; region #1 = "scan" at .../DemoProfiling.cpp:101:3, nested in #0
        |  144 | ; region #2 = "aggregate" at .../DemoProfiling.cpp:114:3, nested in #0
        |  145 | ; region #3 = "mix" at .../DemoProfiling.cpp:115:4, nested in #2
```

It is built on `perf report --sort srcline`, which buckets every sample by the
`file:line` its DWARF points at, across all symbols at once. That is what makes
the view whole-module: a region's cost lands on the IR operations that make it
up regardless of which `JIT_CODE_LOAD` range the optimizer put them in, and the
region legend the dump ends with is right there to read the block comments
against.

Three things to keep in mind reading it:

- **The percentages are global**, the same scale `perf report` uses, so they do
  not sum to 100 over one file. The header says how much of the profile the file
  accounts for; the remainder is host code, the kernel, and the JIT compile
  itself.
- **Cost clusters on a few operations** rather than spreading evenly over a
  dependency chain. Above, `mixStep`'s six arithmetic ops carry 71.6% between
  them while most of its other lines show nothing at all. That is ordinary out-of-order
  attribution — a sample is charged to the instruction that happened to retire
  at the interrupt, and constant materialization (`$19 = 17`) usually folds into
  the consuming instruction and has no address of its own to be charged.
- **Inlined callees keep their own lines.** `mixStep` is a separately registered
  Nautilus function; the MLIR inliner pulled it into the `mix` loop, and it still
  gets annotated in its own definition rather than smeared over the call site.

Per-op attribution holds inside region bodies too. Only each enclosing region
*frame* shares one line — the line where that region opens — never the operations
inside it; [docs/region.md](region.md) explains how the DWARF is built.

## Flame graphs with nested regions

Regions appear in a profile as **sibling symbols, not nested frames**, and this
is worth knowing before reading a flame graph. A region is not a call, so there
is no return address on the stack for an unwinder to stop at, and the jitdump's
debug record is a flat line table with no scope tree to nest frames in (see
[below](#why-regions-are-not-native-perf-frames)). The nesting is folded into
the symbol *name* instead. So a stock flame graph draws
`execute::query::aggregate::mix` next to `execute::query::scan` as two unrelated
boxes, rather than showing `mix` inside `aggregate` inside `query`.

Both paths reconstruct the nesting downstream, where the name is all that
survives.

In process, `writeFlameGraph()` renders one directly, with no external tooling:

```cpp
profiling::Sampler sampler {{"cycles", 50'000, /*callchain=*/true}};
sampler.start();
pipeline(input);
sampler.stopAll();
sampler.report().writeFlameGraph("flame.svg");
```

Reading a profile then needs nothing but the program that produced it — no
Perl, no FlameGraph clone, nothing to install in a container or a CI job.
Frames are coloured by where the code came from rather than by the usual
arbitrary hues, so JIT-compiled nautilus code, host code and unresolved
addresses are told apart at a glance; hovering a frame shows its full name and
share.

> **Only the MLIR backend publishes symbols.** Under `cpp`, `asmjit` or `bc`,
> JIT frames come back unresolved — or, for the interpreter, as the dispatch
> loop's own host frames. See
> [docs/profiling-backends.md](profiling-backends.md).

**Set `callchain`.** Without it a sample carries only its leaf address, so the
graph has nothing to stack except the nesting a region-qualified name encodes —
correct as far as it goes, but with no host frames beneath it and no `main` at
the bottom. With it, the stacks look like the out-of-process path's:

```
_start;__libc_start_main;main;execute;execute::query;execute::query::aggregate;execute::query::aggregate::mix 663
_start;__libc_start_main;main;execute;execute::query;execute::query::scan 50
```

`SampleReport::foldedStacks()` is still there for anyone who prefers their own
renderer, or wants to merge several runs before rendering:

```sh
./your_app > flame.folded          # whatever writes foldedStacks()
flamegraph.pl flame.folded > flame.svg
```

Out of process, `tools/nautilus-perf.sh flamegraph` does the same downstream of
`perf script`:

```sh
tools/nautilus-perf.sh flamegraph perf.jit.data flame.svg
```

It runs `perf script`, splits each `a::b::c` leaf frame back into stacked `a`,
`a::b`, `a::b::c` levels, and folds the result into the input format
[FlameGraph](https://github.com/brendangregg/FlameGraph)'s `flamegraph.pl` (or
`inferno-flamegraph`) expects. If either is on `PATH` it renders the SVG
directly; if neither is, it writes the folded stacks beside the requested output
and prints how to render them:

```sh
git clone https://github.com/brendangregg/FlameGraph
FlameGraph/flamegraph.pl flame.folded > flame.svg
```

Because it starts from whole sampled stacks rather than from leaf addresses, it
also keeps the host frames around the JIT ones:

```
_start;__libc_start_main;[unknown];main;execute;execute::query;execute::query::aggregate;execute::query::aggregate::mix 2340
_start;__libc_start_main;[unknown];main;execute;execute::query;execute::query::scan 135
_start;__libc_start_main;[unknown];main;execute;execute::query;execute::query::aggregate;execute::query::aggregate::merge 109
_start;__libc_start_main;[unknown];main;execute;execute::query;execute::query::aggregate;execute::query::aggregate::merge;mergeAccumulators(long, long) 328
```

Every one of those stacks came out of `perf script` as `main` plus a *single*
leaf frame — `execute::query::aggregate::mix`, say. The `execute`,
`execute::query` and `execute::query::aggregate` levels in between were
synthesized from that one name. Sketched (widths here are illustrative; in the
rendered SVG the boxes are proportional to the counts above):

```
                                                  +--------------------+
                                                  | mergeAccumulators  |
       +------------------------------------------+--------------------+
       |                   mix                    |       merge        |
+------+---------------------------------------------------------------+
| scan |                           aggregate                           |
+----------------------------------------------------------------------+
|                                query                                 |
+----------------------------------------------------------------------+
|                               execute                                |
+----------------------------------------------------------------------+
|                                 main                                 |
+----------------------------------------------------------------------+
```

Host frames called from inside a region nest under it, as `mergeAccumulators`
does above — the region chain is reconstructed in the middle of an otherwise
ordinary stack, not bolted onto the leaf. The plugin's own stacks are leaf-only
unless `Options::callchain` is set.

Only region-qualified names are split, by either route. A demangled C++ symbol
also contains `::` (namespaces) and would otherwise be exploded into one
synthetic frame per namespace component; what tells the two apart is that a
Nautilus qualified name is nothing but `::`-joined identifiers, never carrying a
`(` for an argument list or a `<` for a template, both of which a real C++
signature almost always has by the time a demangler is done with it. A frame
carrying either is left as one frame — which is why
`mergeAccumulators(long, long)` above stays a single box.

## Why regions are not native perf frames

The short answer is that the jitdump protocol cannot express them, and would not
help if it could.

`region()` already lowers to a real DWARF `DW_TAG_inlined_subroutine`, which is
what gives it its own frame in a GDB backtrace under `debug`. But that DWARF
lives in the JIT-linked object, and **perf never reads that object**. Under
`perf` the profile is built from the jitdump, and `perf inject --jit` synthesizes
its own ELF per code range from that file alone. The format has nowhere to put a
scope tree: `JIT_CODE_DEBUG_INFO` is a flat `(address, line, column, file)`
table, with no subprogram records and no inline nesting. A generated
`jitted-*.so` inspected with `readelf --debug-dump=info` contains a bare
`DW_TAG_compile_unit` carrying only `DW_AT_stmt_list` — no `DW_TAG_subprogram`,
no `DW_TAG_inlined_subroutine`. So a region can never come back as an
inlined-subroutine DIE for `perf report --inline`, `addr2line -i`, or any other
DWARF-aware consumer to expand.

Nor would a real scope tree put a region on its own row in a call-graph view: a
region is not a call, so the unwinder has no return address to stop at, whatever
the debug info says.

Changing either would mean abandoning the jitdump protocol altogether — mapping
each compiled function's code from a real on-disk ELF carrying the same DWARF
`region()` already builds for `debug`'s GDB path. That trades away the reason
jitdump was chosen (no persistent per-compile object files, and a format perf
supports natively) for a capability perf's own tooling does not have anyway. The
flame graph above gets the nested picture without any of it.

What Nautilus does instead is put the nesting into the one channel the format
does carry: symbol names. `PerfJitDumpPlugin` walks the DWARF inline stack per
address and emits one `JIT_CODE_LOAD` per contiguous run of code sharing a region
stack, named for its nesting, splitting the enclosing function's symbol around it
so the ranges stay disjoint as the format requires.

A Nautilus-to-Nautilus call inlined into a region counts as part of that region
rather than being named separately. Naming it would split the region's code into
alternating `...::mix` / `...::mix::callee` ranges, and perf does not recombine
same-named ranges — it keys a symbol by address, not by name — so one region
would shatter into a dozen separate rows. Attributing an inlined callee's body to
the region it was inlined into is also the more useful reading: that time really
is the region's.

## Where the files go

A profiling run leaves three kinds of file behind, in two different places.

| File | Where | Written by | Needed until |
|---|---|---|---|
| `jit-<pid>.dump` | `$JITDUMPDIR/.debug/jit/llvm-IR-jit-<date>-<id>/` | the running application | `perf inject` |
| `jitted-<pid>-<n>.so` | the same directory | `perf inject --jit` | `perf report` / `annotate` |
| `nautilus_debug_<pid>_<n>.ir` | the **working directory** | the Nautilus compile | `perf annotate` |

`JITDUMPDIR` defaults to `$HOME`, which is why the recipe sets it to `$PWD` —
otherwise the dump lands somewhere unrelated to `perf.data`.

The Nautilus IR dump is separate, and its location is *not* controlled by
`JITDUMPDIR`: a perf-only compile writes it to the process's working directory,
where a `debug` compile would use `$TMPDIR`. The difference is deliberate —
`$TMPDIR` is routinely cleaned between recording a profile and reading it, and a
deleted dump costs you source attribution with no warning. Nothing cleans these
files up; delete them yourself once the profile has been read.

## Troubleshooting

### In process

| Symptom | Cause |
|---|---|
| `available()` is false, reason mentions `perf_event_paranoid` | The sysctl is above 2. `sudo sysctl kernel.perf_event_paranoid=2`, or run where counters are permitted. |
| `available()` is false inside a container | The syscall is filtered by seccomp or the container's security profile, whatever the sysctl says. |
| An event is missing from `events()` | The host does not know that name, or the PMU ran out of counters. `unavailableReason()` names it. |
| Counter values look implausibly round or scaled | More events were requested than the PMU has slots for, so the kernel multiplexed and extrapolated. Ask for fewer. |
| Zero samples collected | The window was too short, the period too long, or LLVM folded the kernel away. |
| A thread's work is missing from the totals | It found no free slot. Raise `maxThreads`; `unavailableReason()` names it. |
| A thread measures nothing after a pool was replaced | A recycled `std::thread::id` inherited a dead thread's counters. Call `reset()` when the thread set changes. |
| JIT frames show as `unresolved` hex addresses | `perf.sample` was not set on the engine options. |
| Symbols named `execute` with no region qualification | The region names did not survive the optimizer, or `perf.sample` was set without the MLIR backend. |

### Out of process

| Symptom | Cause |
|---|---|
| `perf inject` fails with `jitted code must be sampled with perf record -k 1` | `-k mono` missing from `perf record`. Re-record; the existing `perf.data` cannot be salvaged. |
| No JIT symbols at all, raw addresses in the report | `perf inject --jit` was not run, or it ran in a directory where it could not find the jitdump. |
| `perf annotate` shows disassembly but no interleaved IR | The `nautilus_debug_<pid>_<n>.ir` dump was deleted, or the working directory changed between record and annotate. |
| The profile is far slower than the application really is | `debug` is set alongside `perf`, clamping codegen to `-O0`. A one-time warning says so; drop `debug`, or set `optimizationLevel` explicitly. |
| A small host callee hangs off `main` instead of off its region | Frame-pointer skid, not a Nautilus problem: samples landing on a tiny leaf's `ret` are taken after the epilogue popped `%rbp`, so the walk starts one frame too high. |

### Either way

| Symptom | Cause |
|---|---|
| Part of the run is symbolized and part is not | The backend was not pinned, so tier-0 code (which publishes and emits nothing) and the MLIR code promoted from it are both in the profile at different addresses. Set `engine.backend=mlir`. |
| LLVM and MLIR frames clutter the profile | Compilation happens in-process, so the compile itself is in the profile. Warm the function up before the window, raise the trip count so the measured work dominates, or record only the steady state. |

## Caveats

These apply to both paths unless noted: they are properties of how Nautilus
names and manages generated code, not of either profiler.

### Single compiled module per process

Every single-function registration lowers to a function named literally
`execute`, and `NautilusFunction` callees get their own name but are only unique
within one module's function table. Several compiled modules live in one process
therefore contribute several symbols all called `execute`, plus
`_mlir_execute`-style packed-argument wrapper noise.

Splitting the emission/display name from the lookup key that `Engine.hpp` and
`TieredCompiler.cpp` depend on is a change to the engine's naming, not to the
profiling path, and is intentionally not addressed here.

**In process** this is worked around rather than suffered: ranges are scoped by
their compile, so samples are aggregated per module and a shared name is printed
with the module that produced it (see [Reading the
report](#reading-the-report)). The engine still emits the ambiguous name; the
profile just stops conflating it.

**Out of process** the ambiguity remains — a jitdump carries symbol names and
nothing else, so several live modules contribute several symbols all called
`execute`. Those profiles are unambiguous for a single compiled module per
process — a microbenchmark, or repeated runs of one query — and ambiguous when
several are live at once.

### Tiered compilation splits the profile

The default is tier-0 (`asmjit`/`bc`/interpreter) with background promotion to
MLIR. One logical function therefore exists as two artifacts at two addresses,
and the tier-0 backends neither emit a jitdump nor publish a symbol. Profiling
runs should pin `engine.backend=mlir`, either way.

### Code ranges outlive their code, out of process

The jitdump writer's `notifyRemovingResources` is a no-op, so no
`JIT_CODE_UNLOAD` record is ever written. After a tier-0/tier-1 swap (not
applicable when `engine.backend=mlir` is pinned) or an `Executable` destruction,
a reused code address could in principle misattribute samples to the wrong
function. `perf inject` orders records by timestamp, so this is likely benign in
practice for a single long-running profiling target, but it has not been
independently verified — treat a profile spanning many short-lived
`Executable`s with correspondingly more caution.

The in-process registry does not share this: `MLIRJit` withdraws its module's
ranges when it is destroyed, so an address is never resolved to code that no
longer lives there, and the registry does not grow without bound in an engine
that compiles query after query.

### Threads have to be measured deliberately, in process

`CounterSet` and `Sampler` measure the calling thread, and do not follow threads
spawned after they start. Each worker therefore has to call `start()` and
`stop()` itself (see [Measuring a thread pool](#measuring-a-thread-pool)) --
the calls are the same ones a single-threaded window uses, but they have to
happen *on* each measured thread, and the number of threads measured at once is
capped by `maxThreads`.

The out-of-process path has no such requirement: `perf record` follows every
thread the process creates, without the program knowing anything about it.

### Debug info together with perf

Setting `debug` and `perf` together still works: the jitdump gets a line table,
and you can attach a debugger to the same process. But `debug`'s optimization
clamp (`-O0`, to keep `$N` values readable while stepping) wins, so the resulting
profile measures debug-clamped code, not the production-optimized code this
feature otherwise gets you. A one-time warning is printed when both are set. Pass
`optimizationLevel` explicitly to override the clamp if you need both a debugger
and a representative profile at once. The same applies to `perf.sample`.

## See also

- [docs/profiling-backends.md](profiling-backends.md) — what the in-process
  path would take for the other backends, and why an interpreted one cannot
  have it.
- [docs/region.md](region.md) — what a `region()` is, and how it reaches DWARF.
- [docs/options.md](options.md) — the `perf`, `perf.sample`, `debug` and
  `optimizationLevel` options.
- [docs/engine.md](engine.md) — backend selection and per-module options.
- `nautilus/test/execution-tests/PerfJitDumpExecutionTest.cpp` — pins the jitdump
  behaviour described here.
- `plugins/profiling/test/` — pins the in-process behaviour: that `perf.sample`
  publishes region-qualified symbols, and that samples resolve against them.
