#pragma once

#include "Ast.hpp"
#include "ByteReader.hpp"
#include "EvalNative.hpp"
#include "EvalNautilus.hpp"
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

static_assert(NUM_PARAMS == 3, "The harness wires exactly three uint64_t parameters.");

// registerFunction expects the traced (val<>) return type in the signature; the
// resulting CompiledFunction is then callable with — and returns — raw scalars.
using KernelSignature = val<Value>(val<Value>, val<Value>, val<Value>);

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
	return engine::NautilusEngine(options);
}

/// A single backend disagreeing with the native oracle (or throwing).
struct Finding {
	std::string backend;
	std::string program;
	std::array<Value, NUM_PARAMS> args {};
	Value expected = 0;
	Value got = 0;
	bool exception = false;
	std::string what; // exception message, when exception == true
	bool hasParam = false;
};

inline void printFinding(const Finding& f) {
	if (f.exception) {
		std::fprintf(stderr, "\n==== Nautilus differential fuzzer: EXCEPTION ====\n");
		std::fprintf(stderr, "backend  : %s\n", f.backend.c_str());
		std::fprintf(stderr, "program  : %s\n", f.program.c_str());
		std::fprintf(stderr, "what()   : %s\n", f.what.c_str());
		std::fprintf(stderr, "=================================================\n");
	} else {
		std::fprintf(stderr, "\n==== Nautilus differential fuzzer: MISMATCH ====\n");
		std::fprintf(stderr, "backend  : %s\n", f.backend.c_str());
		std::fprintf(stderr, "program  : %s\n", f.program.c_str());
		std::fprintf(stderr, "args     : p0=%llu p1=%llu p2=%llu\n", (unsigned long long) f.args[0],
		             (unsigned long long) f.args[1], (unsigned long long) f.args[2]);
		std::fprintf(stderr, "expected : %llu (native oracle)\n", (unsigned long long) f.expected);
		std::fprintf(stderr, "got      : %llu\n", (unsigned long long) f.got);
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

/// Core differential check for one input: build a program + args, evaluate it
/// natively, compile/run on every backend + interpreter, and collect every
/// backend that disagrees or throws. Does not abort — callers decide.
inline std::vector<Finding> checkOne(const uint8_t* data, size_t size) {
	ByteReader reader(data, size);
	const Ast ast = generate(reader);
	const bool hasParam = astHasParam(ast);

	std::array<Value, NUM_PARAMS> args {};
	for (uint32_t i = 0; i < NUM_PARAMS; ++i) {
		args[i] = reader.consume<Value>();
	}

	const Value expected = evalNative(ast, args);

	std::vector<Finding> findings;
	for (const auto& backend : configs()) {
		try {
			auto engine = makeEngine(backend);
			std::function<KernelSignature> kernel = [&ast](val<Value> a, val<Value> b, val<Value> c) {
				const TracedArgs traced {a, b, c};
				return evalNautilus(ast, traced);
			};
			auto compiled = engine.registerFunction(kernel);
			const Value got = compiled(args[0], args[1], args[2]);
			if (got != expected) {
				findings.push_back({backend, toString(ast), args, expected, got, false, "", hasParam});
			}
		} catch (const std::exception& e) {
			findings.push_back({backend, toString(ast), args, expected, 0, true, e.what(), hasParam});
		}
	}
	return findings;
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
