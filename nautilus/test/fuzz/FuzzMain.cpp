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

static_assert(NUM_PARAMS == 3, "The libFuzzer harness wires exactly three uint64_t parameters.");

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

[[noreturn]] inline void reportMismatch(const Ast& ast, const std::array<Value, NUM_PARAMS>& args,
                                        const std::string& backend, Value expected, Value got) {
	std::fprintf(stderr, "\n==== Nautilus differential fuzzer: MISMATCH ====\n");
	std::fprintf(stderr, "backend  : %s\n", backend.c_str());
	std::fprintf(stderr, "program  : %s\n", toString(ast).c_str());
	std::fprintf(stderr, "args     : p0=%llu p1=%llu p2=%llu\n", (unsigned long long) args[0],
	             (unsigned long long) args[1], (unsigned long long) args[2]);
	std::fprintf(stderr, "expected : %llu (native oracle)\n", (unsigned long long) expected);
	std::fprintf(stderr, "got      : %llu\n", (unsigned long long) got);
	std::fprintf(stderr, "================================================\n");
	std::fflush(stderr);
	std::abort();
}

inline void runOne(const uint8_t* data, size_t size) {
	ByteReader reader(data, size);
	const Ast ast = generate(reader);

	std::array<Value, NUM_PARAMS> args {};
	for (uint32_t i = 0; i < NUM_PARAMS; ++i) {
		args[i] = reader.consume<Value>();
	}

	const Value expected = evalNative(ast, args);

	for (const auto& backend : configs()) {
		Value got = 0;
		try {
			auto engine = makeEngine(backend);
			std::function<KernelSignature> kernel = [&ast](val<Value> a, val<Value> b, val<Value> c) {
				const TracedArgs traced {a, b, c};
				return evalNautilus(ast, traced);
			};
			auto compiled = engine.registerFunction(kernel);
			got = compiled(args[0], args[1], args[2]);
		} catch (const std::exception& e) {
			std::fprintf(stderr, "\n==== Nautilus differential fuzzer: EXCEPTION ====\n");
			std::fprintf(stderr, "backend  : %s\n", backend.c_str());
			std::fprintf(stderr, "program  : %s\n", toString(ast).c_str());
			std::fprintf(stderr, "what()   : %s\n", e.what());
			std::fprintf(stderr, "=================================================\n");
			std::fflush(stderr);
			std::abort();
		}

		if (got != expected) {
			reportMismatch(ast, args, backend, expected, got);
		}
	}
}

} // namespace nautilus::fuzz

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	nautilus::fuzz::runOne(data, size);
	return 0;
}
