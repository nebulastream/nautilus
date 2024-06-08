# Nautilus: A tracing jit compiler for C++

Nautilus is a lightweight and adaptable just-in-time (JIT) compiler for C++ projects. 
It offers:
1. A high-level code generation API that accommodates C++ control flows.
2. A tracing JIT compiler that produces a lightweight intermediate representation (IR) from imperative code fragments.
3. Multiple code-generation backends, allowing users to balance compilation latency and code quality at runtime.

Nautilus is used for the query compiler of NebulaStream, a data management system from the DIMA group at TU Berlin.
Learn more about Nebula Stream at https://www.nebula.stream

## Example

The example below demonstrates Nautilus with a simplified aggregation operator,
`ConditionalSum`. This function aggregates integer values based on a boolean mask.
Nautilus introduce `val<>` objects to capture all executed operations in an intermediate representation during tracing.
Depending on the execution context, it can utilize a bytecode interpreter or generate efficient MLIR or C++ code.
This enables Nautilus to trade of performance characteristics and to optimize the generated code towards the target hardware.

```c++
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

int main(int, char* []) {
    engine::Options options;
    // select the compilation backend, e.g. mlir, cpp, or bc
    options.setOption("engine.backend", "mlir");
    auto engine = engine::NautilusEngine(options);
    // register a function pointer to nautilus.
    auto function = engine.registerFunction(conditionalSum);
    auto mask = new bool[4]{true, true, false, true};
    auto array = new int32_t[4] {1, 2, 3, 4};
    auto result = function(4, mask, array);
    // result is 7
    std::cout << "Result: " << result << std::endl;
    return 0;
}
```

## Build:

To build Nautilus from source execute use cmake:

```sh
mkdir build
cd build
cmake ..
cmake --build . --target nautilus
```

## Components:

The codebase is structured in the following components:

| Component                         | Description                                                                                               |
|-----------------------------------|-----------------------------------------------------------------------------------------------------------|
| [include](nautilus/include)       | Contains the public api of Nautilus, e.g., `val` objects.                                                 |
| [tracing](nautilus/src/tracing)   | Hosts core functionality for tracing generic C++ code.                                                    |
| [compiler](nautilus/src/compiler) | Implements the Nautilus compiler, including its IR, optimization passes, and various generation backends. |

## Publication:

This paper discusses Nautilus's architecture and its application in the NebulaStream query compiler. 
Note that it references an earlier version of the code-generation API, which has since been updated.

```BibTeX
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

## Related Work:
Nautilus is inspired by following projects: 