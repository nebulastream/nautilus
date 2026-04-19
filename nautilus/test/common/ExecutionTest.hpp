#pragma once

#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <functional>
#include <string>
#include <vector>

#ifdef ENABLE_CONSTANT_TRACER
#include "nautilus/partial_evaluation/api.hpp"
#endif

namespace nautilus::testing {

using OptionsTweak = std::function<void(engine::Options&)>;

/// Legacy RAII toggle of the PE runtime flag. Kept so the existing
/// plugin-test call sites (`plugins/inlining/test/...`, `plugins/
/// specialization/test/...`, `nautilus/test/llvm-ir-test/...`) keep
/// compiling, but its effect during a `Trace()` call is now nullified
/// by the `pe::ModeGuard` each trace context installs — mode selection
/// is the single source of truth, not this flag. New tests should vary
/// `engine.traceMode` instead of toggling the flag.
struct ConstantTracerFlagGuard {
#ifdef ENABLE_CONSTANT_TRACER
	bool saved_;
	explicit ConstantTracerFlagGuard(bool desired) : saved_(tracing::pe::isConstantTracerEnabled()) {
		tracing::pe::setConstantTracerEnabled(desired);
	}
	~ConstantTracerFlagGuard() {
		tracing::pe::setConstantTracerEnabled(saved_);
	}
#else
	explicit ConstantTracerFlagGuard(bool /*desired*/) {
	}
#endif
	ConstantTracerFlagGuard(const ConstantTracerFlagGuard&) = delete;
	ConstantTracerFlagGuard& operator=(const ConstantTracerFlagGuard&) = delete;
};

/// Legacy runtime-flag iteration axis. Always `{false}` now — callers
/// that want to exercise PE should iterate trace modes
/// (`traceModeNames()`) and set `engine.traceMode` accordingly. Kept
/// to avoid a large mechanical edit of every plugin-test DYNAMIC_SECTION
/// wrapper; scheduled to be retired once those callers switch over.
inline std::vector<bool> constantTracerModes() {
	return {false};
}

/// Trace modes exercised by the test harness. Keys match the names
/// registered in `tracing::TraceContextRegistry`. The PE mode is added
/// only when the plugin is compiled in, so builds without
/// ENABLE_CONSTANT_TRACER don't reference an unregistered name.
///
/// The runtime PE flag used to be a separate axis (`ctr_on`/`ctr_off`)
/// layered on top of the trace modes. With `pe::ModeGuard` pinned to
/// each Trace()'s chosen mode, the flag's value is now a consequence of
/// the selected mode — so iterating modes is the sole way to exercise
/// PE vs non-PE behavior.
inline const std::vector<std::string>& traceModeNames() {
	static const std::vector<std::string> modes = {
	    "exceptionBasedTracing",
	    "lazyTracing",
#ifdef ENABLE_CONSTANT_TRACER
	    "partialEvaluation",
#endif
	};
	return modes;
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
	if (tweak) {
		tweak(options);
	}
	return engine::NautilusEngine(options);
}

// Runs `body(engine)` once per available backend (and once for the interpreter
// when `include_interpreter` is true), each inside its own Catch2
// DYNAMIC_SECTION named after the backend. `tweak` lets callers layer extra
// options (e.g. trace mode, mlir intrinsics) on top of the defaults.
//
// When ENABLE_CONSTANT_TRACER is compiled in, every backend section runs a
// second time under the "partialEvaluation" trace mode so PE-specific
// regressions surface automatically. The interpreter does not trace and is
// therefore skipped from that axis.
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
#ifdef ENABLE_CONSTANT_TRACER
		DYNAMIC_SECTION(backend + "_partialEvaluation") {
			auto engine = makeEngine(backend, [&](engine::Options& opts) {
				opts.setOption("engine.traceMode", "partialEvaluation");
				if (tweak) {
					tweak(opts);
				}
			});
			body(engine);
		}
#endif
	}
#else
	(void) include_asmjit;
#endif
}

// Like forEachBackend, but iterates over every trace mode registered in
// TraceContextRegistry — baseline modes (exceptionBasedTracing,
// lazyTracing) plus the PE mode when available. The interpreter does not
// trace and is skipped.
template <typename Body>
inline void forEachBackendWithTraceMode(Body&& body, const OptionsTweak& tweak = {}, bool include_asmjit = true) {
#ifdef ENABLE_TRACING
	for (const auto& backend : availableBackends(include_asmjit)) {
		for (const auto& traceMode : traceModeNames()) {
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
