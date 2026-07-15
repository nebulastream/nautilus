# Nautilus

[![Build Nautilus](https://github.com/nebulastream/nautilus/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/nebulastream/nautilus/actions/workflows/build.yml)
[![Playground](https://img.shields.io/badge/try_it-playground-blue)](https://nautilus.grulich.me)

**Nautilus is a lightweight, tracing just-in-time (JIT) compiler for C++.**
Write imperative C++ using `val<T>` wrapper types, and Nautilus traces the
control flow and operations of your function, converts the trace into an
intermediate representation (IR), optimizes it, and compiles it to efficient
code through one of several interchangeable backends, trading off
compilation latency against code quality at runtime.

Nautilus was developed by the DIMA group at TU Berlin and is used as the
query compiler of [NebulaStream](https://www.nebula.stream), a data
management system. Its design is described in a SIGMOD 2024 paper (see
[Publication](#publication) below).

▶ **[Try Nautilus in your browser: no setup required](https://nautilus.grulich.me)**

## Contents

- [Why Nautilus?](#why-nautilus)
- [Example](#example)
- [Getting Started](#getting-started)
- [Documentation](#documentation)
- [Project Layout](#project-layout)
- [Publication](#publication)
- [Related Work](#related-work)

## Why Nautilus?

- **Write ordinary C++.** Loops, branches, and expressions over `val<T>`
  values look and read like normal C++: no separate DSL or code-generation
  templates to learn.
- **Trace once, compile anywhere.** A single traced function can target
  multiple backends without changes to its source:
  - **MLIR**: lowers through MLIR/LLVM to native machine code for maximum
    performance.
  - **C++**: emits readable, compilable C++ source, useful for debugging
    and inspection.
  - **Bytecode**: interprets directly, optionally accelerated by a
    copy-and-patch JIT that stitches together pre-compiled machine-code
    stencils, for very low compilation latency.
- **Optimizing IR pipeline.** Traced code passes through an SSA-based IR with
  constant propagation, dead code elimination, loop analysis, and
  control-flow simplification before reaching a backend.
- **Built for query compilation.** Designed for the latency/throughput
  trade-offs of database query engines, but general enough for any C++
  project that benefits from JIT-compiled hot paths.

## Example

The example below implements a simplified aggregation operator,
`conditionalSum`, which sums the elements of `array` where the corresponding
entry in `mask` is `true`. Nautilus's `val<T>` types capture every operation
performed on them during tracing, so the function body reads like plain C++
while still producing a full execution trace. This is the same code as
[`example/src/DemoJit.cpp`](example/src/DemoJit.cpp), which you can build and
run directly (see [Getting Started](#getting-started)).

```c++
#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/core.hpp>

using namespace nautilus;

val<int32_t> conditionalSum(val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < size; i++) {
		// check mask
		if (mask[i]) {
			// load value from array at position i
			val<int32_t> value = array[i];
			// add value to sum
			sum += value;
		}
	}
	return sum;
}

int main(int, char*[]) {
	engine::Options options;
	// select the compilation backend, e.g. mlir, cpp, or bc
	options.setOption("engine.backend", "mlir");
	auto engine = engine::NautilusEngine(options);
	// register a function pointer to nautilus
	auto function = engine.registerFunction(conditionalSum);
	auto mask = new bool[4] {true, true, false, true};
	auto array = new int32_t[4] {1, 2, 3, 4};
	auto result = function(4, mask, array);
	// result is 7
	std::cout << "Result: " << result << std::endl;
	return 0;
}
```

Want to run this without installing anything? Try it live in the
**[Nautilus Playground](https://nautilus.grulich.me)**, a Godbolt-style web UI
for exploring the compilation pipeline. Its source lives in
[`tools/playground/`](tools/playground) and can be self-hosted.

## Getting Started

### Prerequisites

- A C++20 compiler: GCC 14+ or Clang 19+ (Clang 21 is used by default in CI)
- CMake 3.16+
- MLIR is downloaded automatically on first build if the MLIR backend is enabled

### Build

```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target nautilus
```

Common build options (see `CMakeLists.txt` for the full list):

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_TESTS` | ON | Build the test suite |
| `ENABLE_BENCHMARKS` | OFF | Build performance benchmarks |
| `ENABLE_MLIR_BACKEND` | ON | Enable the MLIR code generation backend |
| `ENABLE_C_BACKEND` | ON | Enable the C++ code generation backend |
| `ENABLE_BC_BACKEND` | ON | Enable the bytecode interpreter backend |
| `ENABLE_ASMJIT_BACKEND` | ON | Enable the AsmJit backend |
| `ENABLE_GPU_PLUGIN` | OFF | Enable the GPU plugin (CUDA + Metal backends) |

More runnable examples, including backend selection, runtime calls, and
plugins, are available in [`example/`](example).

## Documentation

The [`docs/`](docs) directory has in-depth guides:

| Guide | Description |
|-------|-------------|
| [Getting Started](docs/getting-started.md) | Full walkthrough of the tracing-to-execution pipeline |
| [Value Types (`val<T>`)](docs/val-type.md) | How the tracing value wrapper works for different types |
| [Control Flow](docs/control-flow.md) | Using if/else and branching with `val<bool>` |
| [Loops](docs/loops.md) | Writing traced for and while loops |
| [Pointers](docs/pointers.md) | Working with `val<T*>` pointer types |
| [Functions](docs/functions.md) | Calling and registering functions |
| [Static Values (`static_val<T>`)](docs/static-val.md) | Compile-time constant values |
| [Engine Configuration](docs/engine.md) | Backend selection, options, and debugging output |
| [Tracing Internals](docs/tracing.md) | How the tracing system works under the hood |
| [Graphs](docs/graphs.md) | The IR control-flow graph and SSA representation |
| [SIMD](docs/simd.md) | The SIMD vector plugin |
| [Standard Library Plugin](docs/std-plugin.md) | Wrapping C++ standard library functions |

## Project Layout

```
nautilus/
├── include/nautilus/   Public API: val<T>, Engine, options, std wrappers
├── src/nautilus/
│   ├── tracing/         Captures execution traces from val<T> operations
│   └── compiler/
│       ├── ir/           Control-flow graph, SSA, and optimization phases
│       └── backends/     Code-generation backends (mlir, cpp, bc, asmjit)
└── test/                execution-tests, val-tests, llvm-ir-test, benchmarks
```

| Component | Description |
|-----------|-------------|
| [include](nautilus/include) | Public API of Nautilus, e.g. `val` objects and the `Engine`. |
| [tracing](nautilus/src/tracing) | Core functionality for tracing generic C++ code. |
| [compiler](nautilus/src/compiler) | The Nautilus compiler: IR, optimization passes, and code-generation backends. |

## Publication

This paper discusses Nautilus's architecture and its usage in the
NebulaStream query compiler. Note that it references an earlier version of
the code-generation API, which has since changed.

```bibtex
@article{10.1145/3654968,
    author = {Grulich, Philipp M. and Lepping, Aljoscha P. and Nugroho, Dwi P. A. and Pandey, Varun and Del Monte, Bonaventura and Zeuch, Steffen and Markl, Volker},
    title = {Query Compilation Without Regrets},
    year = {2024},
    issue_date = {June 2024},
    volume = {2},
    number = {3},
    url = {https://doi.org/10.1145/3654968},
    doi = {10.1145/3654968},
    journal = {Proc. ACM Manag. Data},
    articleno = {165},
    numpages = {28},
}
```

## Related Work

The following work is related to Nautilus and influenced its design:

* [Tidy Tuples and Flying Start](db.in.tum.de/~kersten/Tidy%20Tuples%20and%20Flying%20Start%20Fast%20Compilation%20and%20Fast%20Execution%20of%20Relational%20Queries%20in%20Umbra.pdf):
  describes the low-latency query compilation approach of [Umbra](https://umbra-db.com/), one of the
  main motivations for creating Nautilus.

* [Flounder](https://vldb.org/pvldb/vol14/p2691-funke.pdf):
  a simple low-latency JIT compiler based on [AsmJit](https://asmjit.com/), designed for query compilation.

* [Build-It](https://buildit.so/):
  a framework for developing Domain Specific Languages in C++ that pioneered extracting control-flow
  information from imperative C++ code.

* [GraalVM](https://www.graalvm.org/):
  provides a framework to implement AST interpreters that can be turned into high-performance code
  through partial evaluation.

* [MLIR](https://mlir.llvm.org/):
  a novel approach to building reusable and extensible compiler infrastructure. Nautilus leverages it
  as the foundation for its high-performance compilation backend.
