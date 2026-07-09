#pragma once

#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace nautilus::testing {

using OptionsTweak = std::function<void(engine::Options&)>;

/// Applies the `NAUTILUS_IR_VERIFY=1` corpus gate (V7): when set, every
/// engine built through `makeEngine` verifies the IR after each pass and
/// throws on the first violation, turning any test run into a verifier
/// sweep over whatever IR shapes that run exercises. Off by default so
/// ordinary test runs pay no extra verification cost.
inline void applyIrVerifyEnvHook(engine::Options& options) {
	const char* verify = std::getenv("NAUTILUS_IR_VERIFY");
	if (verify != nullptr && std::string(verify) == "1") {
		options.setOption("ir.verifyAfterEachPass", true);
		options.setOption("ir.failOnVerifyError", true);
	}
}

inline std::vector<std::string> availableBackends(bool include_asmjit = true) {
	std::vector<std::string> backends;
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_TBC_BACKEND
	backends.emplace_back("tbc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	if (include_asmjit) {
		backends.emplace_back("asmjit");
	}
#else
	(void) include_asmjit;
#endif
	return backends;
}

inline engine::NautilusEngine makeEngine(const std::string& backend, const OptionsTweak& tweak = {}) {
	engine::Options options;
	if (backend == "interpreter") {
		options.setOption("engine.Compilation", false);
	} else {
		options.setOption("engine.backend", backend);
	}
	applyIrVerifyEnvHook(options);
	if (tweak) {
		tweak(options);
	}
	return engine::NautilusEngine(options);
}

// Runs `body(engine)` once per available backend (and once for the interpreter
// when `include_interpreter` is true), each inside its own Catch2
// DYNAMIC_SECTION named after the backend. `tweak` lets callers layer extra
// options (e.g. trace mode, mlir intrinsics) on top of the defaults.
template <typename Body>
inline void forEachBackend(Body&& body, bool include_interpreter = true, const OptionsTweak& tweak = {},
                           bool include_asmjit = true) {
	if (include_interpreter) {
		DYNAMIC_SECTION("interpreter") {
			auto engine = makeEngine("interpreter", tweak);
			body(engine);
		}
	}
#ifdef ENABLE_TRACING
	for (const auto& backend : availableBackends(include_asmjit)) {
		DYNAMIC_SECTION(backend) {
			auto engine = makeEngine(backend, tweak);
			body(engine);
		}
	}
#else
	(void) include_asmjit;
#endif
}

// Like forEachBackend, but additionally cycles through the supported tracing
// modes (exceptionBasedTracing, lazyTracing) for every backend. The
// interpreter does not trace and is therefore skipped.
template <typename Body>
inline void forEachBackendWithTraceMode(Body&& body, const OptionsTweak& tweak = {}, bool include_asmjit = true) {
#ifdef ENABLE_TRACING
	static const std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (const auto& backend : availableBackends(include_asmjit)) {
		for (const auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				auto engine = makeEngine(backend, [&](engine::Options& opts) {
					opts.setOption("engine.traceMode", traceMode);
					if (tweak) {
						tweak(opts);
					}
				});
				body(engine);
			}
		}
	}
#else
	(void) body;
	(void) tweak;
#endif
}

} // namespace nautilus::testing
