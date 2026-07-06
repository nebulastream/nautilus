#pragma once

#include "Ast.hpp"
#include "ByteReader.hpp"
#include "EvalNative.hpp"
#include "EvalNautilus.hpp"
#include "Types.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <nautilus/Engine.hpp>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace nautilus::fuzz {

/// Backends compiled into this build, mirroring testing::availableBackends().
/// The interpreter is always present and acts as an extra differential peer.
inline std::vector<std::string> configs() {
	std::vector<std::string> result;
	result.emplace_back("interpreter");
#ifdef ENABLE_MLIR_BACKEND
	result.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	result.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	result.emplace_back("bc");
#endif
#ifdef ENABLE_TBC_BACKEND
	result.emplace_back("tbc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	result.emplace_back("asmjit");
#endif
	return result;
}

inline engine::NautilusEngine makeEngine(const std::string& backend) {
	engine::Options options;
	if (backend == "interpreter") {
		options.setOption("engine.Compilation", false);
	} else {
		options.setOption("engine.backend", backend);
	}
	// Triage aid: NAUTILUS_FUZZ_DUMP=1 dumps every compilation stage to the
	// console while replaying a saved finding, so the IR a mismatching
	// backend actually saw can be inspected without editing the harness.
	if (std::getenv("NAUTILUS_FUZZ_DUMP") != nullptr) {
		options.setOption("dump.all", true);
		options.setOption("dump.console", true);
	}
	// V7 corpus gate: NAUTILUS_IR_VERIFY=1 makes the fuzz-replay smoke corpus
	// double as an IR-verifier sweep (mirrors testing::applyIrVerifyEnvHook
	// in test/common/ExecutionTest.hpp, which the fuzz harness doesn't use).
	const char* verify = std::getenv("NAUTILUS_IR_VERIFY");
	if (verify != nullptr && std::string(verify) == "1") {
		options.setOption("ir.verifyAfterEachPass", true);
		options.setOption("ir.failOnVerifyError", true);
	}
	return engine::NautilusEngine(options);
}

/// A single backend disagreeing with the native oracle (or throwing). All
/// value fields are pre-formatted strings (built where the concrete type T
/// is still known, inside the checkOne* functions below) so this struct
/// itself stays type-agnostic across all ten generated domains and every
/// signature shape.
struct Finding {
	std::string backend;
	std::string typeName;
	std::string shape; // which signature shape generated this kernel, see below
	std::string program;
	std::vector<std::string> args;
	std::vector<std::string> memory; // shared buffer's initial contents (see Kind::PtrBase, Ast.hpp)
	std::string expected = "";
	std::string got = "";
	bool exception = false;
	std::string what; // exception message, when exception == true
	bool hasParam = false;
};

inline void printMemory(const std::vector<std::string>& memory) {
	std::fprintf(stderr, "memory   : [");
	for (size_t i = 0; i < memory.size(); ++i) {
		std::fprintf(stderr, "%s%s", i == 0 ? "" : ", ", memory[i].c_str());
	}
	std::fprintf(stderr, "]\n");
}

inline void printArgs(const std::vector<std::string>& args) {
	std::fprintf(stderr, "args     : ");
	for (size_t i = 0; i < args.size(); ++i) {
		std::fprintf(stderr, "%sp%zu=%s", i == 0 ? "" : " ", i, args[i].c_str());
	}
	std::fprintf(stderr, "\n");
}

inline void printFinding(const Finding& f) {
	if (f.exception) {
		std::fprintf(stderr, "\n==== Nautilus differential fuzzer: EXCEPTION ====\n");
		std::fprintf(stderr, "backend  : %s\n", f.backend.c_str());
		std::fprintf(stderr, "type     : %s\n", f.typeName.c_str());
		std::fprintf(stderr, "shape    : %s\n", f.shape.c_str());
		std::fprintf(stderr, "program  : %s\n", f.program.c_str());
		printMemory(f.memory);
		std::fprintf(stderr, "what()   : %s\n", f.what.c_str());
		std::fprintf(stderr, "=================================================\n");
	} else {
		std::fprintf(stderr, "\n==== Nautilus differential fuzzer: MISMATCH ====\n");
		std::fprintf(stderr, "backend  : %s\n", f.backend.c_str());
		std::fprintf(stderr, "type     : %s\n", f.typeName.c_str());
		std::fprintf(stderr, "shape    : %s\n", f.shape.c_str());
		std::fprintf(stderr, "program  : %s\n", f.program.c_str());
		printArgs(f.args);
		printMemory(f.memory);
		std::fprintf(stderr, "expected : %s (native oracle)\n", f.expected.c_str());
		std::fprintf(stderr, "got      : %s\n", f.got.c_str());
		std::fprintf(stderr, "================================================\n");
	}
	std::fflush(stderr);
}

inline bool astHasParam(const Ast& ast) {
	for (const Node& n : ast.nodes) {
		if (n.kind == Kind::Param) {
			return true;
		}
	}
	return false;
}

/**
 * @brief The kernel-boundary signature space (issue #355).
 *
 * Every generated program's *internal* AST evaluation is always monomorphic
 * in one type T (unchanged from before -- see Ast.hpp/EvalNative.hpp/
 * EvalNautilus.hpp). What varies here is the shape of the `registerFunction`
 * signature the AST is compiled into: how many T-valued parameters
 * Kind::Param may reference (the AST's logical arity, `generate`'s
 * `numParams`), whether the kernel's *declared* parameter list matches that
 * arity 1:1 in type, and what the kernel returns.
 *
 * `registerFunction` needs a compile-time-known std::function, so rather
 * than synthesizing arbitrary signatures at runtime this enumerates a small,
 * bounded menu of concrete shapes and dispatches to them -- one instantiation
 * per (shape, T) pair:
 *
 *   - arity{1,2,3,4}: N T-typed parameters, T-typed return (N in the menu
 *     below). arity3 is the harness's original fixed shape.
 *   - mixed: 3 T-typed logical parameters, but the kernel's first *declared*
 *     parameter is a secondary type S cross-domain from T (int when T is
 *     float, double when T is int -- see MixedSecondaryType), converted to T
 *     immediately at the boundary before joining the AST's T-only argument
 *     domain. Exercises the register-class interleaving between float and
 *     integer parameters at the kernel entry, never reached when every
 *     parameter is uniformly T.
 *   - narrowReturn: 3 T-typed parameters, but the kernel returns a fixed
 *     narrow integer type (int8_t) instead of T -- the AST still evaluates
 *     in T, with the result cast down at the return boundary. Exercises the
 *     narrow-integer-return extension/truncation ABI.
 *   - voidReturn: 3 T-typed parameters, `void` return. The AST's result has
 *     no return-value ABI to compare, so it (and, on the traced side,
 *     whatever Store side effects the AST itself performed) is instead
 *     diffed via the shared buffer's final contents: the computed root value
 *     is explicitly written to the buffer's first slot on both the native
 *     and traced side before the buffer is compared elementwise.
 *
 * All conversions across a type boundary (mixed's incoming parameter,
 * narrowReturn's outgoing return) go through Types.hpp's convertClamped /
 * EvalNautilus.hpp's convertClampedTraced -- the same NaN/out-of-range-safe
 * recipe already used for Kind::Cast, applied once more at the ABI boundary
 * instead of inside the AST -- so every boundary conversion stays
 * well-defined for all input bytes and native/traced parity is guaranteed by
 * construction, not by convention.
 */
inline constexpr uint32_t NUM_SIGNATURE_SHAPES = 7;

template <typename T, int N>
struct ArityTraits;

template <typename T>
struct ArityTraits<T, 1> {
	using Signature = val<T>(val<T*>, val<T>);
};
template <typename T>
struct ArityTraits<T, 2> {
	using Signature = val<T>(val<T*>, val<T>, val<T>);
};
template <typename T>
struct ArityTraits<T, 3> {
	using Signature = val<T>(val<T*>, val<T>, val<T>, val<T>);
};
template <typename T>
struct ArityTraits<T, 4> {
	using Signature = val<T>(val<T*>, val<T>, val<T>, val<T>, val<T>);
};

template <typename T, int N>
using ArityKernelSignature = typename ArityTraits<T, N>::Signature;

/// Cross-domain secondary type for the "mixed" shape: a float T pairs with a
/// 32-bit int, an integer T pairs with a double -- always the *other*
/// register class from T, so the mixed parameter genuinely exercises
/// int/float interleaving at the kernel boundary rather than just a
/// different width within the same class.
template <typename T>
using MixedSecondaryType = std::conditional_t<std::is_floating_point_v<T>, int32_t, double>;

template <typename T>
using MixedKernelSignature = val<T>(val<T*>, val<MixedSecondaryType<T>>, val<T>, val<T>);

/// Fixed narrow return type for the narrowReturn shape (see the module
/// comment above); deliberately independent of T; used in the same historical
/// direction (narrow int return) that already produced ProxyCall/IndirectCall
/// ABI bugs for `Call` (see README.md "Known findings").
using NarrowReturnType = int8_t;

template <typename T>
using NarrowReturnKernelSignature = val<NarrowReturnType>(val<T*>, val<T>, val<T>, val<T>);

template <typename T>
using VoidReturnKernelSignature = void(val<T*>, val<T>, val<T>, val<T>);

/// arity{1,2,3,4}: N T-typed parameters, T-typed return -- the generalization
/// of the harness's original fixed 3-parameter shape to a small bounded menu
/// of arities (param-count coverage: 1, 2, 3, and >=4 register/stack-passed
/// arguments).
template <typename T, int N>
std::vector<Finding> checkOneArity(ByteReader& reader) {
	static_assert(N >= 1 && N <= 4, "arity shape menu only covers N in [1, 4]");
	const Ast ast = generate<T>(reader, static_cast<uint32_t>(N));
	const bool hasParam = astHasParam(ast);

	std::array<T, N> args {};
	for (int i = 0; i < N; ++i) {
		args[i] = reader.consume<T>();
	}

	std::array<T, BUFFER_ELEMS> initialMemory {};
	for (int i = 0; i < BUFFER_ELEMS; ++i) {
		initialMemory[i] = reader.consume<T>();
	}
	std::array<T, BUFFER_ELEMS> memory = initialMemory;

	const T expected = evalNative<T>(ast, args, memory);
	const std::string program = toString<T>(ast);
	std::vector<std::string> argStrs;
	argStrs.reserve(N);
	for (T a : args) {
		argStrs.push_back(formatValue<T>(a));
	}
	std::vector<std::string> memoryStrs;
	memoryStrs.reserve(BUFFER_ELEMS);
	for (T v : initialMemory) {
		memoryStrs.push_back(formatValue<T>(v));
	}
	const std::string shapeName = "arity" + std::to_string(N);

	std::vector<Finding> findings;
	for (const auto& backend : configs()) {
		try {
			auto engine = makeEngine(backend);
			std::function<ArityKernelSignature<T, N>> kernel = [&ast](val<T*> mem, auto... a) {
				TracedArgs<T, static_cast<std::size_t>(N)> traced {a...};
				return evalNautilus<T>(ast, mem, traced);
			};
			auto compiled = engine.registerFunction(kernel);
			memory = initialMemory;
			const T got = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
				return compiled(memory.data(), args[Is]...);
			}(std::make_index_sequence<static_cast<std::size_t>(N)> {});
			if (!valuesEqual<T>(got, expected)) {
				findings.push_back({backend, typeSuffix<T>(), shapeName, program, argStrs, memoryStrs,
				                    formatValue<T>(expected), formatValue<T>(got), false, "", hasParam});
			}
		} catch (const std::exception& e) {
			findings.push_back(
			    {backend, typeSuffix<T>(), shapeName, program, argStrs, memoryStrs, "", "", true, e.what(), hasParam});
		}
	}
	return findings;
}

/// mixed: 3 logical T parameters, but the kernel's first declared parameter
/// is MixedSecondaryType<T> and gets converted to T immediately at the
/// boundary (both natively and traced) before joining the AST's T-only
/// argument domain -- see the module comment above.
template <typename T>
std::vector<Finding> checkOneMixed(ByteReader& reader) {
	using S = MixedSecondaryType<T>;
	constexpr int N = 3;
	const Ast ast = generate<T>(reader, N);
	const bool hasParam = astHasParam(ast);

	const S secondaryRaw = reader.consume<S>();
	const T arg1 = reader.consume<T>();
	const T arg2 = reader.consume<T>();
	std::array<T, N> args {convertClamped<S, T>(secondaryRaw), arg1, arg2};

	std::array<T, BUFFER_ELEMS> initialMemory {};
	for (int i = 0; i < BUFFER_ELEMS; ++i) {
		initialMemory[i] = reader.consume<T>();
	}
	std::array<T, BUFFER_ELEMS> memory = initialMemory;

	const T expected = evalNative<T>(ast, args, memory);
	const std::string program = toString<T>(ast);
	const std::vector<std::string> argStrs {formatValue<S>(secondaryRaw), formatValue<T>(arg1), formatValue<T>(arg2)};
	std::vector<std::string> memoryStrs;
	memoryStrs.reserve(BUFFER_ELEMS);
	for (T v : initialMemory) {
		memoryStrs.push_back(formatValue<T>(v));
	}

	std::vector<Finding> findings;
	for (const auto& backend : configs()) {
		try {
			auto engine = makeEngine(backend);
			std::function<MixedKernelSignature<T>> kernel = [&ast](val<T*> mem, val<S> p0, val<T> p1, val<T> p2) {
				TracedArgs<T, N> traced {convertClampedTraced<S, T>(p0), p1, p2};
				return evalNautilus<T>(ast, mem, traced);
			};
			auto compiled = engine.registerFunction(kernel);
			memory = initialMemory;
			const T got = compiled(memory.data(), secondaryRaw, arg1, arg2);
			if (!valuesEqual<T>(got, expected)) {
				findings.push_back({backend, typeSuffix<T>(), "mixed", program, argStrs, memoryStrs,
				                    formatValue<T>(expected), formatValue<T>(got), false, "", hasParam});
			}
		} catch (const std::exception& e) {
			findings.push_back(
			    {backend, typeSuffix<T>(), "mixed", program, argStrs, memoryStrs, "", "", true, e.what(), hasParam});
		}
	}
	return findings;
}

/// narrowReturn: 3 T-typed parameters; the kernel returns NarrowReturnType
/// instead of T. The AST still evaluates in T -- only the final result is
/// cast down at the return boundary, on both the native and traced side.
template <typename T>
std::vector<Finding> checkOneNarrowReturn(ByteReader& reader) {
	constexpr int N = 3;
	const Ast ast = generate<T>(reader, N);
	const bool hasParam = astHasParam(ast);

	std::array<T, N> args {};
	for (int i = 0; i < N; ++i) {
		args[i] = reader.consume<T>();
	}

	std::array<T, BUFFER_ELEMS> initialMemory {};
	for (int i = 0; i < BUFFER_ELEMS; ++i) {
		initialMemory[i] = reader.consume<T>();
	}
	std::array<T, BUFFER_ELEMS> memory = initialMemory;

	const T rawExpected = evalNative<T>(ast, args, memory);
	const NarrowReturnType expected = convertClamped<T, NarrowReturnType>(rawExpected);
	const std::string program = toString<T>(ast);
	std::vector<std::string> argStrs;
	argStrs.reserve(N);
	for (T a : args) {
		argStrs.push_back(formatValue<T>(a));
	}
	std::vector<std::string> memoryStrs;
	memoryStrs.reserve(BUFFER_ELEMS);
	for (T v : initialMemory) {
		memoryStrs.push_back(formatValue<T>(v));
	}

	std::vector<Finding> findings;
	for (const auto& backend : configs()) {
		try {
			auto engine = makeEngine(backend);
			std::function<NarrowReturnKernelSignature<T>> kernel = [&ast](val<T*> mem, val<T> a, val<T> b, val<T> c) {
				TracedArgs<T, N> traced {a, b, c};
				TracedValue<T> result = evalNautilus<T>(ast, mem, traced);
				return convertClampedTraced<T, NarrowReturnType>(result);
			};
			auto compiled = engine.registerFunction(kernel);
			memory = initialMemory;
			const NarrowReturnType got = compiled(memory.data(), args[0], args[1], args[2]);
			if (!valuesEqual<NarrowReturnType>(got, expected)) {
				findings.push_back({backend, typeSuffix<T>(), "narrowReturn", program, argStrs, memoryStrs,
				                    formatValue<NarrowReturnType>(expected), formatValue<NarrowReturnType>(got), false,
				                    "", hasParam});
			}
		} catch (const std::exception& e) {
			findings.push_back({backend, typeSuffix<T>(), "narrowReturn", program, argStrs, memoryStrs, "", "", true,
			                    e.what(), hasParam});
		}
	}
	return findings;
}

/// voidReturn: 3 T-typed parameters, `void` return -- a Store-only kernel.
/// The AST's computed root value has no return-value ABI to exercise, so it
/// is explicitly written to the buffer's first slot on both the native and
/// traced side (in addition to whatever Store side effects the AST itself
/// performs), and the whole buffer is diffed elementwise instead of a scalar
/// return.
template <typename T>
std::vector<Finding> checkOneVoidReturn(ByteReader& reader) {
	constexpr int N = 3;
	const Ast ast = generate<T>(reader, N);
	const bool hasParam = astHasParam(ast);

	std::array<T, N> args {};
	for (int i = 0; i < N; ++i) {
		args[i] = reader.consume<T>();
	}

	std::array<T, BUFFER_ELEMS> initialMemory {};
	for (int i = 0; i < BUFFER_ELEMS; ++i) {
		initialMemory[i] = reader.consume<T>();
	}
	std::array<T, BUFFER_ELEMS> memory = initialMemory;

	const T rootValue = evalNative<T>(ast, args, memory);
	memory[0] = rootValue;
	const std::array<T, BUFFER_ELEMS> expectedMemory = memory;

	const std::string program = toString<T>(ast);
	std::vector<std::string> argStrs;
	argStrs.reserve(N);
	for (T a : args) {
		argStrs.push_back(formatValue<T>(a));
	}
	std::vector<std::string> memoryStrs;
	memoryStrs.reserve(BUFFER_ELEMS);
	for (T v : initialMemory) {
		memoryStrs.push_back(formatValue<T>(v));
	}

	std::vector<Finding> findings;
	for (const auto& backend : configs()) {
		try {
			auto engine = makeEngine(backend);
			std::function<VoidReturnKernelSignature<T>> kernel = [&ast](val<T*> mem, val<T> a, val<T> b, val<T> c) {
				TracedArgs<T, N> traced {a, b, c};
				TracedValue<T> result = evalNautilus<T>(ast, mem, traced);
				*mem = result;
			};
			auto compiled = engine.registerFunction(kernel);
			memory = initialMemory;
			compiled(memory.data(), args[0], args[1], args[2]);
			if (!arraysEqual<T, BUFFER_ELEMS>(memory, expectedMemory)) {
				findings.push_back({backend, typeSuffix<T>(), "voidReturn", program, argStrs, memoryStrs,
				                    formatArray<T, BUFFER_ELEMS>(expectedMemory), formatArray<T, BUFFER_ELEMS>(memory),
				                    false, "", hasParam});
			}
		} catch (const std::exception& e) {
			findings.push_back({backend, typeSuffix<T>(), "voidReturn", program, argStrs, memoryStrs, "", "", true,
			                    e.what(), hasParam});
		}
	}
	return findings;
}

/// Dispatch a single input, at a fixed concrete type T, to the signature
/// shape selected by `shape % NUM_SIGNATURE_SHAPES`. `reader` continues from
/// wherever the caller (checkOne) left off, so the whole pipeline -- type
/// selection, shape selection, AST generation, arg generation -- is one
/// linear, deterministic pass over a single buffer.
template <typename T>
std::vector<Finding> checkOneTyped(ByteReader& reader, uint32_t shape) {
	switch (shape % NUM_SIGNATURE_SHAPES) {
	case 0:
		return checkOneArity<T, 1>(reader);
	case 1:
		return checkOneArity<T, 2>(reader);
	case 2:
		return checkOneArity<T, 3>(reader);
	case 3:
		return checkOneArity<T, 4>(reader);
	case 4:
		return checkOneMixed<T>(reader);
	case 5:
		return checkOneNarrowReturn<T>(reader);
	default:
		return checkOneVoidReturn<T>(reader);
	}
}

/// Pick one of the ten generated types and one of the signature shapes from
/// the first two bytes of the buffer, then dispatch into the matching
/// checkOneTyped<T> instantiation, passing the same reader along so
/// determinism/replay is preserved end to end.
inline std::vector<Finding> checkOne(const uint8_t* data, size_t size) {
	ByteReader reader(data, size);
	const TypeId ty = ALL_TYPES[reader.byte() % (sizeof(ALL_TYPES) / sizeof(ALL_TYPES[0]))];
	const uint32_t shape = reader.byte();
	return dispatchAnyType(ty, [&]<typename T>() { return checkOneTyped<T>(reader, shape); });
}

/// libFuzzer behavior: abort (so the input is captured as a reproducer) on the
/// first backend that disagrees or throws.
inline void runOne(const uint8_t* data, size_t size) {
	const std::vector<Finding> findings = checkOne(data, size);
	if (!findings.empty()) {
		printFinding(findings.front());
		std::abort();
	}
}

} // namespace nautilus::fuzz
