# Profiling JIT-compiled code with `perf`

A JIT compiler is invisible to a sampling profiler by default. The code Nautilus
generates lives in anonymous memory that no ELF file on disk describes, so `perf
record` collects samples for it and then has nothing to resolve them against —
the profile shows raw addresses under whatever host frame called into the
compiled function, and the part of the program you wanted to measure is the one
part you cannot see.

Setting the `perf` engine option fixes that. The MLIR backend then writes the
Linux **jitdump** protocol for every function it compiles, so `perf record`
produces a profile in which JIT frames are symbolized, attributed down to
individual Nautilus IR operations and `region()` scopes, and — unlike a naively
debug-instrumented build — generated at the same optimization level you would
ship.

This is Linux/ELF only. On any other platform `perf` is a warning-and-continue
no-op rather than an error, so the same application code builds and runs
everywhere.

- [Quick recipe](#quick-recipe)
- [The demo](#the-demo)
- [Reading the profile](#reading-the-profile)
- [Down to individual IR operations](#down-to-individual-ir-operations)
- [Flame graphs with nested regions](#flame-graphs-with-nested-regions)
- [Why regions are not native perf frames](#why-regions-are-not-native-perf-frames)
- [Where the files go](#where-the-files-go)
- [Troubleshooting](#troubleshooting)
- [Caveats](#caveats)

## Quick recipe

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

[`example/src/DemoPerfProfile.cpp`](../example/src/DemoPerfProfile.cpp) is a
runnable version of all of this: a small query-shaped kernel whose work is split
across nested `region()`s of deliberately unequal cost, so the profile has an
obviously right answer to check the tooling against.

```
query                    the whole pipeline
  scan                   one cheap pass: load + predicate
  aggregate              the expensive half
    mix                  per-element hashing -- most of the time
    merge                a strided pass that calls out to the host
```

```sh
cd example
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target demo_perf_profile -j

cd build
../../tools/nautilus-perf.sh record -- ./demo_perf_profile
../../tools/nautilus-perf.sh report
../../tools/nautilus-perf.sh flamegraph perf.jit.data flame.svg
```

The default run takes about a second and a half, which `perf record`'s default
sampling rate turns into a few thousand samples. Pass smaller arguments
(`./demo_perf_profile 1000 64`) for a smoke run that only checks it compiles.

The kernel also has a Nautilus-to-Nautilus call inside `mix` — inlined at `-O3`,
and so attributed to the region rather than named separately — and an `invoke()`
into a `noinline` host function inside `merge`, which puts a real host-side frame
*under* a JIT frame: the case that exercises unwinding out of JIT code.

## Reading the profile

`perf report` groups samples by symbol, and with `perf` set those symbols are
Nautilus's own. A function registered as `execute` that wraps its work in regions
produces one symbol per region, qualified by its nesting. From the demo:

```
# Overhead  Command          Shared Object       Symbol
# ........  ...............  ..................  ....................................
    72.48%  demo_perf_profi  jitted-15257-2.so   [.] execute::query::aggregate::mix
    10.23%  demo_perf_profi  demo_perf_profile   [.] mergeAccumulators(long, long)
     4.17%  demo_perf_profi  jitted-15257-1.so   [.] execute::query::scan
     3.37%  demo_perf_profi  jitted-15257-4.so   [.] execute::query::aggregate::merge
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
    10.23%  demo_perf_profi  demo_perf_profile   [.] mergeAccumulators(long, long)
            |
            ---mergeAccumulators(long, long)
               |
                --10.13%--execute::query::aggregate::merge
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
         : 28 mixStep($1:i64, $2:i64) :i64 [readnone, willreturn, nounwind]  ; at .../DemoPerfProfile.cpp:68:18 {
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
        |  119 | mixStep($1:i64, $2:i64) :i64 [readnone, willreturn, nounwind]  ; at .../DemoPerfProfile.cpp:68:18 {
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
        |  142 | ; region #0 = "query" at .../DemoPerfProfile.cpp:86:2
        |  143 | ; region #1 = "scan" at .../DemoPerfProfile.cpp:87:3, nested in #0
        |  144 | ; region #2 = "aggregate" at .../DemoPerfProfile.cpp:102:3, nested in #0
        |  145 | ; region #3 = "mix" at .../DemoPerfProfile.cpp:103:4, nested in #2
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

Regions appear in `perf report` as **sibling symbols, not nested frames**, and
this is worth knowing before reading a flame graph. A region is not a call, so
there is no return address on the stack for perf's unwinder to stop at, and the
jitdump's debug record is a flat line table with no scope tree to nest frames in
(see [below](#why-regions-are-not-native-perf-frames)). The nesting is folded
into the symbol *name* instead. So a stock flame graph draws
`execute::query::aggregate::mix` next to `execute::query::scan` as two unrelated
boxes, rather than showing `mix` inside `aggregate` inside `query`.

`tools/nautilus-perf.sh flamegraph` reconstructs the nesting downstream, where
the name is all that survives:

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

The folded stacks it produces for the demo read the way the source nests:

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
ordinary stack, not bolted onto the leaf.

Only region-qualified names are split. A demangled C++ symbol also contains `::`
(namespaces) and would otherwise be exploded into one synthetic frame per
namespace component; what tells the two apart is that a Nautilus qualified name
is nothing but `::`-joined identifiers, never carrying a `(` for an argument list
or a `<` for a template, both of which a real C++ signature almost always has by
the time perf's demangler is done with it. A frame carrying either is left as one
frame — which is why `mergeAccumulators(long, long)` above stays a single box.

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

| Symptom | Cause |
|---|---|
| `perf inject` fails with `jitted code must be sampled with perf record -k 1` | `-k mono` missing from `perf record`. Re-record; the existing `perf.data` cannot be salvaged. |
| No JIT symbols at all, raw addresses in the report | `perf inject --jit` was not run, or it ran in a directory where it could not find the jitdump. |
| Part of the run is symbolized and part is raw addresses | The backend was not pinned, so tier-0 code (which emits no jitdump) and the MLIR code promoted from it are both in the profile at different addresses. Set `engine.backend=mlir`. |
| `perf annotate` shows disassembly but no interleaved IR | The `nautilus_debug_<pid>_<n>.ir` dump was deleted, or the working directory changed between record and annotate. |
| The profile is far slower than the application really is | `debug` is set alongside `perf`, clamping codegen to `-O0`. A one-time warning says so; drop `debug`, or set `optimizationLevel` explicitly. |
| A small host callee hangs off `main` instead of off its region | Frame-pointer skid, not a Nautilus problem: samples landing on a tiny leaf's `ret` are taken after the epilogue popped `%rbp`, so the walk starts one frame too high. |
| LLVM and MLIR frames clutter the profile | Compilation happens in-process, so the compile itself is in the profile. Raise the trip count so the measured work dominates, or record only the steady state. |

## Caveats

### Single compiled module per process

Every single-function registration lowers to a function named literally
`execute`, and `NautilusFunction` callees get their own name but are only unique
within one module's function table. Several compiled modules live in one process
therefore contribute several symbols all called `execute`, plus
`_mlir_execute`-style packed-argument wrapper noise.

Profiles are unambiguous for a single compiled module per process — a
microbenchmark, or repeated runs of one query — and ambiguous when several
modules are live at once. Splitting the emission/display name from the lookup key
that `Engine.hpp` and `TieredCompiler.cpp` depend on is a change to the engine's
naming, not to the profiling path, and is intentionally not addressed here.

### Tiered compilation splits the profile

The default is tier-0 (`asmjit`/`bc`/interpreter) with background promotion to
MLIR. One logical function therefore exists as two artifacts at two addresses,
and the tier-0 backends emit no jitdump at all. Profiling runs should pin
`engine.backend=mlir`, as in the recipe above.

### No `JIT_CODE_UNLOAD` record

`notifyRemovingResources` is a no-op, so no `JIT_CODE_UNLOAD` record is ever
written. After a tier-0/tier-1 swap (not applicable when `engine.backend=mlir` is
pinned) or an `Executable` destruction, a reused code address could in principle
misattribute samples to the wrong function. `perf inject` orders records by
timestamp, so this is likely benign in practice for a single long-running
profiling target, but it has not been independently verified — treat a profile
spanning many short-lived `Executable`s with correspondingly more caution.

### Debug info together with perf

Setting `debug` and `perf` together still works: the jitdump gets a line table,
and you can attach a debugger to the same process. But `debug`'s optimization
clamp (`-O0`, to keep `$N` values readable while stepping) wins, so the resulting
profile measures debug-clamped code, not the production-optimized code this
feature otherwise gets you. A one-time warning is printed when both are set. Pass
`optimizationLevel` explicitly to override the clamp if you need both a debugger
and a representative profile at once.

## See also

- [docs/region.md](region.md) — what a `region()` is, and how it reaches DWARF.
- [docs/options.md](options.md) — the `perf`, `debug` and `optimizationLevel` options.
- [docs/engine.md](engine.md) — backend selection and per-module options.
- `nautilus/test/execution-tests/PerfJitDumpExecutionTest.cpp` — pins the jitdump
  behaviour described here.
