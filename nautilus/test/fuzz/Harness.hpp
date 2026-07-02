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
#include <vector>

namespace nautilus::fuzz {

static_assert(NUM_PARAMS == 3, "The harness wires exactly three same-typed parameters.");

// registerFunction expects the traced (val<>) return type in the signature; the
// resulting CompiledFunction is then callable with -- and returns -- raw scalars.
// The leading val<T*> is the shared BUFFER_ELEMS-element buffer every
// generated program's pointer-domain nodes (Kind::PtrBase et al., see
// Ast.hpp) index into.
template <typename T>
using KernelSignature = val<T>(val<T*>, val<T>, val<T>, val<T>);

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
	return engine::NautilusEngine(options);
}

/// A single backend disagreeing with the native oracle (or throwing). All
/// value fields are pre-formatted strings (built where the concrete type T
/// is still known, inside checkOneTyped<T>) so this struct itself stays
/// type-agnostic across all ten generated domains.
struct Finding {
	std::string backend;
	std::string typeName;
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

inline void printFinding(const Finding& f) {
	if (f.exception) {
		std::fprintf(stderr, "\n==== Nautilus differential fuzzer: EXCEPTION ====\n");
		std::fprintf(stderr, "backend  : %s\n", f.backend.c_str());
		std::fprintf(stderr, "type     : %s\n", f.typeName.c_str());
		std::fprintf(stderr, "program  : %s\n", f.program.c_str());
		printMemory(f.memory);
		std::fprintf(stderr, "what()   : %s\n", f.what.c_str());
		std::fprintf(stderr, "=================================================\n");
	} else {
		std::fprintf(stderr, "\n==== Nautilus differential fuzzer: MISMATCH ====\n");
		std::fprintf(stderr, "backend  : %s\n", f.backend.c_str());
		std::fprintf(stderr, "type     : %s\n", f.typeName.c_str());
		std::fprintf(stderr, "program  : %s\n", f.program.c_str());
		std::fprintf(stderr, "args     : p0=%s p1=%s p2=%s\n", f.args[0].c_str(), f.args[1].c_str(), f.args[2].c_str());
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

/// Core differential check for one input at a fixed concrete type T: build a
/// program + args, evaluate it natively, compile/run on every backend +
/// interpreter, and collect every backend that disagrees or throws. Does not
/// abort -- callers decide. `reader` continues from wherever the caller (the
/// type-dispatching checkOne) left off, so the whole pipeline -- type
/// selection, AST generation, arg generation -- is one linear, deterministic
/// pass over a single buffer.
template <typename T>
std::vector<Finding> checkOneTyped(ByteReader& reader) {
	const Ast ast = generate<T>(reader);
	const bool hasParam = astHasParam(ast);

	std::array<T, NUM_PARAMS> args {};
	for (uint32_t i = 0; i < NUM_PARAMS; ++i) {
		args[i] = reader.consume<T>();
	}

	// Initial contents of the shared buffer every pointer-domain node indexes
	// into (see Kind::PtrBase, Ast.hpp). `memory` is declared once and reused
	// -- reset to `initialMemory` before every run below -- rather than
	// freshly constructed per run, so its address stays identical across the
	// native oracle and every backend; that's what makes Kind::PtrToInt's
	// pointer->integer cast comparable at all (it embeds a real address).
	std::array<T, BUFFER_ELEMS> initialMemory {};
	for (int i = 0; i < BUFFER_ELEMS; ++i) {
		initialMemory[i] = reader.consume<T>();
	}
	std::array<T, BUFFER_ELEMS> memory = initialMemory;

	const T expected = evalNative<T>(ast, args, memory);
	const std::string program = toString<T>(ast);
	std::vector<std::string> argStrs;
	argStrs.reserve(NUM_PARAMS);
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
			std::function<KernelSignature<T>> kernel = [&ast](val<T*> mem, val<T> a, val<T> b, val<T> c) {
				const TracedArgs<T> traced {a, b, c};
				return evalNautilus<T>(ast, mem, traced);
			};
			auto compiled = engine.registerFunction(kernel);
			memory = initialMemory;
			const T got = compiled(memory.data(), args[0], args[1], args[2]);
			if (!valuesEqual<T>(got, expected)) {
				findings.push_back({backend, typeSuffix<T>(), program, argStrs, memoryStrs, formatValue<T>(expected),
				                    formatValue<T>(got), false, "", hasParam});
			}
		} catch (const std::exception& e) {
			findings.push_back(
			    {backend, typeSuffix<T>(), program, argStrs, memoryStrs, "", "", true, e.what(), hasParam});
		}
	}
	return findings;
}

/// Pick one of the ten generated types from the first byte of the buffer,
/// then dispatch into the matching checkOneTyped<T> instantiation, passing
/// the same reader along so determinism/replay is preserved end to end.
inline std::vector<Finding> checkOne(const uint8_t* data, size_t size) {
	ByteReader reader(data, size);
	const TypeId ty = ALL_TYPES[reader.byte() % (sizeof(ALL_TYPES) / sizeof(ALL_TYPES[0]))];
	return dispatchAnyType(ty, [&]<typename T>() { return checkOneTyped<T>(reader); });
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
