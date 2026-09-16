# Profiling JIT-compiled code with `perf`

Nautilus's MLIR backend can emit the Linux `perf` "jitdump" protocol for every
JIT-compiled function, so `perf record` produces a profile in which JIT
frames are symbolized, attributed down to individual Nautilus IR operations
and `region()` scopes, and -- unlike a naively debug-instrumented build --
generated from the same optimization level you'd ship.

This is Linux/ELF only. On any other platform `mlir.perf.enable` is a no-op
(a warning is printed once) rather than an error.

## Quick recipe

```sh
JITDUMPDIR=$PWD perf record -k mono -g --call-graph fp -- ./your_app
perf inject --jit -i perf.data -o perf.jit.data
perf report -i perf.jit.data
perf annotate -i perf.jit.data --stdio <symbol>
```

with these engine options set on the `Options` your application configures
Nautilus with:

```cpp
options.setOption("mlir.perf.enable", true);
options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
options.setOption("mlir.debug.source_dir", std::string("."));
options.setOption("engine.backend", std::string("mlir"));
```

A few of these are not optional:

- **`-k mono`** is mandatory. `perf inject --jit` correlates the jitdump's
  timestamps against the perf.data timeline using the monotonic clock; without
  `-k mono` the two cannot be lined up and injection silently produces
  nothing useful.
- **`engine.backend=mlir`**. The default is tiered compilation, which starts
  functions on a tier-0 interpreting backend (`asmjit`/`bc`) that emits no
  jitdump at all, and background-promotes to MLIR later -- see
  [Tiered compilation splits the profile](#tiered-compilation-splits-the-profile)
  below. Pin the backend for a profiling run.
- **`mlir.debug.source_mode=nautilus-ir`**. This is the whole point: it is
  what gets you attribution to individual Nautilus IR operations -- inside
  `region()` bodies too -- rather than to the post-inline MLIR module. The
  `mlir` source mode is not profile-capable and is out of scope for this
  feature; do not expect it to work with `mlir.perf.enable`.
- **`mlir.debug.source_dir=.`** matches the *derived* default for a perf-only
  compile (`mlir.perf.enable` without `mlir.debug.enable` defaults the source
  directory to the working directory rather than `$TMPDIR`, precisely so the
  dump outlives the compile), but set it explicitly anyway: the recipe should
  not depend on that derivation continuing to hold, and the dump must not be
  deleted before `perf inject` runs.

`tools/nautilus-perf.sh` wraps this recipe.

## What you get

`perf report` groups samples by `region()`: a function that wraps its hot loop
in `region("hot", ...)` gets a `execute::outer::hot` symbol, and the region's
share of the profile is its own row.

Regions appear as **sibling symbols, not nested frames**, and this is worth
knowing before reading a flame graph. The DWARF inlined subroutines that give a
region its own frame in a GDB backtrace (`docs/region.md`) cannot reach perf at
all: perf never reads the JIT-registered object holding that DWARF, and the
jitdump it does read has no scope tree to put them in -- its debug record is a
flat line table. So the region nesting is folded into the symbol *name*
instead, one `JIT_CODE_LOAD` per contiguous run of code belonging to a region.
A flame graph therefore shows `execute::outer::hot` beside `execute`, rather
than stacked on top of it. `mlir.perf.region_symbols=false` turns this off and
gets plain per-function symbols back.

A Nautilus-to-Nautilus call inlined into a region counts as part of that
region, rather than being named separately -- naming it would split the
region's code into alternating ranges, and perf does not recombine same-named
ranges (it keys a symbol by address, not name).

`perf annotate` attributes cycles down to individual Nautilus IR operations,
including inside those region bodies -- per-op line numbers are preserved
there; only each enclosing region *frame* shares one line, not the operations
inside it (docs/region.md has the full explanation).

## Caveats

### Single compiled module per process

Every single-function registration lowers to a function named literally
`execute`, and NautilusFunction callees get their own name but are only
unique within one module's function table. Several compiled modules
live in one process therefore contribute several symbols all called
`execute`, plus `_mlir_execute`-style packed-argument wrapper noise.

Profiles are unambiguous for a single compiled module per process -- a
microbenchmark, or repeated runs of one query -- and ambiguous when several
modules are live at once. Splitting the emission/display name from the
lookup key that `Engine.hpp` and `TieredCompiler.cpp` depend on is a change
to the engine's naming, not to the profiling path, and is intentionally not
addressed here.

### Tiered compilation splits the profile

The default is tier-0 (`asmjit`/`bc`/interpreter) with background promotion
to MLIR. One logical function therefore exists as two artifacts at two
addresses, and the tier-0 backends emit no jitdump at all. Profiling runs
should pin `engine.backend=mlir`, as in the recipe above.

### No `JIT_CODE_UNLOAD` record

`PerfSupportPlugin::notifyRemovingResources` is a no-op upstream in LLVM, so
no `JIT_CODE_UNLOAD` record is ever written. After a tier-0/tier-1 swap (not
applicable when `engine.backend=mlir` is pinned) or an `Executable`
destruction, a reused code address could in principle misattribute samples
to the wrong function. `perf inject` orders records by timestamp, so this is
likely benign in practice for a single long-running profiling target, but it
has not been independently verified here -- treat a profile spanning many
short-lived `Executable`s with correspondingly more caution.

### Debug info together with perf

Setting `mlir.debug.enable` and `mlir.perf.enable` together still works: the
jitdump gets a line table, and you can attach a debugger to the same process.
But `mlir.debug.enable`'s optimization clamp (`-O0`, to keep `$N` values
readable while stepping) wins, so the resulting profile measures debug-clamped
code, not the production-optimized code this feature otherwise gets you. A
one-time warning is printed when both are set. Pass `mlir.optimizationLevel`
explicitly to override the clamp if you need both a debugger and a
representative profile at once.
