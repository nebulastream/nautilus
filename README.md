# Nautilus: A tracing jit compiler for C++

Nautilus is a lightweight and flexible just-in-time compiler for c++ projects.
It consists of (1.) a high-level code generation api that supports generic c++ control-flow,
(2.) a tracing jit compiler, that generates a light-weight IR from code fragments,
(3.) multiple code-generation backends to choose compilation latency and code quality at runtime.

Nautilus powers the query compiler of NebulaStream,
a data management system developed by the DIMA group at TU Berlin.
Learn more about Nebula Stream at https://www.nebula.stream

## Example

The following [example](./example/) illustrates Nautilus on a simplified aggregation operator.
`ConditionalSum` iterates over an array and only aggregates the values for, which mask is true.
All runtime dependent values are wrappt in Nautilus `val<>` objects.
This allows Nautilus traces the function and to capture all executed operations in an intermediate representation.
During execution it either leverages a bytecode interpreter or generates efficient MLIR or c++ code.
As the code is generated at runtime, Nautilus takes the specific hardware of the target machine into account.

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
cmake --build ./build --target nautilus
```

## Components:

The codebase is structured in the following components:

| Component                         | Description                                                                                                           |
|-----------------------------------|-----------------------------------------------------------------------------------------------------------------------|
| [include](nautilus/include)       | Contains the public api of Nautilus, e.g., `val` objects.                                                             |
| [tracing](nautilus/src/tracing)   | Contains the core functionality to trace generic c++ code that uses nautilus `val` objects.                           |
| [compiler](nautilus/src/compiler) | Contains the implementation of Nautilus compiler, its IR, optimization passes, and the different generation backends. |

## Publication:

The following paper presents an overview of Nautilus design and how it is used as a foundation for NebulaStream`s query
compiler.
> [!NOTE]
> Note that the paper references an old version of the code-generation api.

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