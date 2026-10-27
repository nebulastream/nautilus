# API-compatibility suite

Test cases of 0.3's execution and value tests that use the public API only (M1.10 of
`docs/plans/nautilus-0.4/16-implementation-plan.md`). They are copied unchanged from `nautilus/test/execution-tests/` and
`nautilus/test/val-tests/` and **not edited afterwards**: if one fails, the public API changed. The suite builds and runs
on the 0.3 pipeline and has to keep passing on 0.4 on every enabled backend.

## Selection rule

A file is copied if

- it includes public headers only (`nautilus/include/`) and the function headers of `test/common/` that it needs,
- it names no backend (`"mlir"`, `"tbc"`, ...), reads no dump and parses no IR text,
- it needs no plugin library (so `BitIntrinsicExecutionTest.cpp`, which uses the `std` plugin, stays with the plugin).

Left out, among others: `CompilationStatisticsTest.cpp`, `DebugInfoExecutionTest.cpp`, `PerfJitDumpExecutionTest.cpp`, the
dump checks of `ABIExtensionAttributeTest.cpp`, and every file that branches on a backend name. The largest suite,
`ExecutionTest.cpp`, mixes both kinds of cases and is not copied as a whole; splitting it case by case is the next step
of M1.10.

`common/ExecutionTest.hpp` is 0.3's harness without the `tbc-jit` pseudo-backend (which reaches into the library): it runs
a body on the interpreter and on every enabled backend. The ctest names carry the prefix `api-compat: `.

## ExecutionTest.cpp and the examples

`ExecutionTest.cpp`, the largest suite, is copied with three cuts that the rule requires and nothing else: the "Ref"
check no longer names the backend that removes the duplicate calls (it accepts both counts), and the two compiled cases
set neither a dump option nor a backend-specific option. The examples are in `examples/`, copied unchanged; the CMake file
records which backend each one pins (`DemoJit` and `DemoRegions` pin `cpp`, the SIMD, specialization and profiling
demos pin `mlir`) and builds it only when that backend is enabled. `DemoBackends` (compares every backend),
`DemoInliningPlugin` (needs the Clang pass plugin) and `DemoGPU` (disabled until M7.7) are not copied.
