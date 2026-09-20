# Profiling the other backends

The profiling plugin's sampler names JIT frames by resolving instruction
pointers against `JitSymbolRegistry`, which today only the **MLIR backend**
publishes to. This is an investigation of what the other backends would take,
with the goal in [docs/profiling.md](profiling.md) unchanged: a flame graph with
**one frame per module function and per `region()`**, not one opaque frame per
process.

Everything below is measured, not assumed. The numbers come from
`plugins/profiling/test/BackendCoverageTest.cpp`, which samples the same kernel
under each backend and prints what the profile resolves to.

- [Where we are](#where-we-are)
- [Three tiers, not five backends](#three-tiers-not-five-backends)
- [Tier 1: native code in anonymous memory](#tier-1-native-code-in-anonymous-memory)
- [Tier 2: native code in a real ELF](#tier-2-native-code-in-a-real-elf)
- [Tier 3: interpreted](#tier-3-interpreted)
- [Getting region granularity](#getting-region-granularity)
- [Suggested order](#suggested-order)

## Where we are

One kernel, two regions (`hot` dominating `cold`), sampled on `cycles` under
each backend:

| backend | registry ranges | JIT-resolved samples | what the profile shows |
|---|---|---|---|
| `mlir` | 10 | 89% | `execute::hot` — function *and* region ✅ |
| `cpp` | 0 | 0 | unresolved addresses in a mapped-but-deleted `.so` |
| `asmjit` | 0 | 0 | unresolved addresses in anonymous memory |
| `bc` | 0 | 0 | host frames: `BCInterpreter::execute`, `regMov`, `add<long>` |

The `bc` row is the interesting one. It is not unresolved — it resolves
*correctly*, to the interpreter's dispatch loop. That is a true answer to the
question perf was asked, and a useless one for the question a user has.

## Three tiers, not five backends

The backends differ in ways that matter for profiling in exactly one respect:
**where the code a sample lands in comes from**. That collapses five backends
into three problems.

| tier | backends | code lives in | what is missing |
|---|---|---|---|
| 1 | `mlir` ✅, `asmjit`, `tbc` (JIT mode) | anonymous executable memory | nothing describes it — publication |
| 2 | `cpp` | a real `.so`, `dlopen`ed | the file, which is deleted |
| 3 | `bc`, `tbc` (interpreting mode) | nowhere — there is no per-function code | a mechanism other than sampling |

## Tier 1: native code in anonymous memory

This is the tier MLIR already solves, and the solution generalizes: publish
`{start, end, name, moduleIndex}` for every compiled range into
`JitSymbolRegistry`, and the existing resolver, flame graph and module scoping
work unchanged.

**asmjit.** `AsmJitExecutable` holds an `asmjit::JitRuntime` allocation, which
knows both the entry pointer and the code size, so the range is directly to
hand. The measured profile clusters tightly — `0x…0073`, `0x…0080`, `0x…008d`,
`0x…0094` — which is one compact generated loop, exactly the shape a range
would cover.

**tbc in JIT mode** (`ENABLE_TBC_JIT`, `tbc.mode=jit`) stitches pre-compiled
stencils into real machine code, so it has addresses too. Its interpreting mode
is tier 3.

The work is the same in both cases and small: a hook where the backend finishes
producing code, calling `JitSymbolRegistry::addAll`. Neither backend needs
DWARF, because neither goes through LLVM — which also means neither gets region
names for free. See [region granularity](#getting-region-granularity).

## Tier 2: native code in a real ELF

The C++ backend compiles generated source with a real compiler into a `.so` and
`dlopen`s it. Those functions are ordinary ELF symbols in an ordinary mapped
file, so perf-cpp's resolver — which already backs our host-frame fallback —
should name them with no nautilus involvement at all.

It doesn't, for one line:

```cpp
auto sharedLibrary = SharedLibrary::load(libraryFileName);
// ...
std::filesystem::remove(libraryFileName);     // CPPCompiler.cpp:87
```

The library is unlinked immediately after loading. The mapping survives, so the
code runs; but `/proc/self/maps` reports the region as `(deleted)` and no
resolver can open the file to read its symbol table. Hence four-figure sample
counts against bare addresses.

That makes tier 2 the cheapest win available: **function-level attribution for
the C++ backend costs approximately one conditional.** Keeping the file when
profiling is enabled — or publishing ranges from the symbols the backend
already looks up with `dlsym` — turns those addresses into names.

Worth noting the deletion is deliberate housekeeping, not an oversight, and the
commented-out block above it shows it was once conditional on a debugging flag.
Reinstating a condition is in keeping with the original intent.

## Tier 3: interpreted

`bc`, and `tbc` in its interpreting modes, execute a bytecode dispatch loop.
There is no code corresponding to a nautilus function or a region, so there is
no address for a sample to land in and nothing to publish. The profile above is
the honest result: the interpreter is what is running.

Sampling cannot be made to answer this. The options are:

1. **Accept it, and say so.** A one-line note in the report — "this backend is
   interpreted; samples name the interpreter, not your pipeline" — is better
   than a user concluding their pipeline is spending its time in `regMov`.
   Cheap, and strictly an improvement on silence.
2. **Interpreter-side attribution.** The interpreter knows which function and
   region it is executing; it could maintain a thread-local current-region id
   that a *separate timer-driven sampler thread* reads. That is a different
   sampler from the perf-based one — statistical, self-hosted, and only for
   interpreted backends — and it makes the interpreter's inner loop write a
   variable it otherwise would not.
3. **Instrumentation.** Count entries and exits per region. This is the
   per-region counter work that [PR #470's description](profiling.md) scopes
   out, and it perturbs what it measures.

Option 1 is the honest default. Option 2 is the only one that yields a flame
graph, and it should not be confused with the perf-based path: the two would
produce differently derived numbers that a reader would reasonably assume were
comparable.

## Getting region granularity

Publishing a range per *function* is straightforward everywhere in tier 1. A
range per **region** is the harder half, and MLIR gets it in a way the others
cannot copy.

Today the region-qualified name comes from walking the DWARF inline stack per
address (`JitCodeRanges.cpp`): `region()` lowers to a synthetic inlined
subroutine, so LLVM's line table already records which region each instruction
came from, and `computeCodeRanges` only has to read it. asmjit and tbc emit no
DWARF and have no inliner, so there is nothing to read.

What they do have is a **direct correspondence between IR operations and the
code they emit**, which MLIR loses to the optimizer. Both lower operation by
operation, so a lowering provider can record `(code offset, region index)` as
it goes — and `Operation::getRegionIndex()` is documented as exact. That yields
region ranges *without* DWARF, and more precisely than the DWARF path, because
no inlining has smeared them.

Concretely, tier 1 backends need the lowering provider to emit a side table
alongside the code:

```
offset 0x00 .. 0x12   region NO_REGION    (prologue)
offset 0x12 .. 0x4a   region #1 "hot"
offset 0x4a .. 0x5e   region #2 "cold"
```

which is the same shape `computeCodeRanges` produces from DWARF, and feeds the
same `JitSymbolRegistry::addAll`. The naming rules — qualifying by nesting,
folding an inlined callee into its enclosing region — live in `JitCodeRanges`
and should be lifted out of the MLIR-specific header so both paths share them.

For tier 2 there is no equivalent: the C++ compiler chooses the layout, and a
region is not a function in the generated source. Function-level attribution is
the realistic ceiling unless the generator emits each region as its own
function, which would change the code being measured.

## Suggested order

1. **A one-line note for interpreted backends.** Smallest change, removes the
   most misleading output, and needs no design.
2. **The C++ backend's deleted `.so`.** Function-level attribution for a whole
   backend, for roughly a conditional.
3. **asmjit function ranges.** Proves the registry generalizes beyond MLIR and
   covers the default tier-0 backend, where a tiered run currently goes blind
   before promotion.
4. **asmjit region ranges**, via a lowering-provider side table. The first real
   test of whether the naming rules survive being shared.
5. **tbc JIT mode**, which is the same shape as (3) and (4) once those exist.

Each step is independently useful, and none of them requires the next.
