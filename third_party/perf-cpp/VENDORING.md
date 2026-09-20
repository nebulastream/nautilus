# Vendoring notes for perf-cpp

Used by the profiling plugin (`plugins/profiling`) for in-process hardware
counters and sampling. See [docs/profiling.md](../../docs/profiling.md).

| | |
|---|---|
| Upstream | <https://github.com/jmuehlig/perf-cpp> |
| Version | `v1.1.0-1-g9e41423` |
| Commit | `9e41423afdb9180a2cfbac15e4c2c591063c10d3` (2026-08-23) |
| License | Apache-2.0 (`LICENSE`) |

The sources are **unpatched**: a new upstream release is a clean replacement
rather than a merge. Where the library needed extending -- resolving
instruction pointers inside JIT-compiled code, which its `util::SymbolResolver`
cannot know about -- the extension lives in the plugin instead.

## What was removed, and why

Only `include/`, `src/` and the three documentation/licence files are kept.
Everything else upstream ships is build-time or development scaffolding that a
vendored copy does not need:

| Removed | Size | Why |
|---|---|---|
| `events/` | 2.3 MB | Per-microarchitecture PMU event tables, read **only** by `script/generate_processor_event_provider.py` under `GEN_PROCESSOR_EVENTS=ON`. Nothing reads them at runtime. See below before re-enabling. |
| `test/`, `examples/`, `test_package/` | 508 KB | Not built (`BUILD_TESTS` / `BUILD_EXAMPLES` off, conan unused). |
| `docs/` | 208 KB | mkdocs sources for the upstream website. |
| `script/` | 20 KB | The event generator and a `perf list` helper; both configure-time only. |
| `CMakeLists.txt`, `cmake/` | | Replaced -- see below. |
| `.github/`, `conanfile.py`, `mkdocs.yml`, `.gitignore`, `.clang-format`, `.clang-tidy` | | Upstream CI and tooling. `.clang-format` in particular would be picked up by editors working in this tree. |

## Why the build is ours

`third_party/perf-cpp` carries no `CMakeLists.txt`; the target is defined in the
repository root's `CMakeLists.txt` alongside the other vendored dependencies.
Upstream's build script was dropped rather than `add_subdirectory`'d because it
does three things a vendored dependency should not:

1. **It installs itself.** `install(DIRECTORY include/ ...)` and an
   `install(EXPORT perf-cppTargets ...)` are unconditional, so `cmake --install`
   dropped `include/perfcpp/` and `lib/cmake/perf-cpp/` into the consumer's
   prefix -- next to `include/nautilus/` -- for a library no consumer of
   nautilus ever includes. Our public headers are pimpl'd precisely so that
   perf-cpp stays an implementation detail.
2. **`GEN_PROCESSOR_EVENTS` writes into the source tree.** It runs `python3` at
   configure time and generates
   `src/processor_specific_event_provider.cpp` *inside this directory*,
   dirtying the git working tree. It is off by default upstream and off here,
   but a vendored copy should not carry a foot-gun that only fires for whoever
   flips an innocuous-looking option.
3. **It is a moving target.** Re-syncing would silently adopt whatever new
   options, install rules or global settings upstream had added.

Owning ~15 lines of CMake costs a glob over `src/` and buys a build that does
exactly what this repository wants.

### If you ever want processor-specific events

Restore `events/` and `script/` from upstream at the pinned commit, run the
generator by hand, and add the generated file to the source list with
`-DPERFCPP_HAS_PROCESSOR_SPECIFIC_EVENTS`. Do not wire the generator into
configure: keep generated code out of the source tree.

## Re-syncing

```sh
cd third_party
cp perf-cpp/VENDORING.md /tmp/                     # this file is ours, not upstream's
git clone --depth 1 --branch <tag> https://github.com/jmuehlig/perf-cpp.git perf-cpp.new
rm -rf perf-cpp.new/.git perf-cpp.new/events perf-cpp.new/docs perf-cpp.new/examples \
       perf-cpp.new/test perf-cpp.new/test_package perf-cpp.new/script perf-cpp.new/.github \
       perf-cpp.new/cmake perf-cpp.new/CMakeLists.txt perf-cpp.new/conanfile.py \
       perf-cpp.new/mkdocs.yml perf-cpp.new/.gitignore perf-cpp.new/.clang-format \
       perf-cpp.new/.clang-tidy
rm -rf perf-cpp && mv perf-cpp.new perf-cpp && cp /tmp/VENDORING.md perf-cpp/
```

Then update the version and commit in the table above, check upstream's
`CMakeLists.txt` for source files added outside `src/` (the glob covers
everything under it), and rebuild.

`format.sh` skips every `third_party/` directory, so clang-format never
rewrites these sources.
