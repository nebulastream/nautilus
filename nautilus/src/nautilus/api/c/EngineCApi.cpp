#include "CApiInternal.hpp"
#include "nautilus/c/engine.h"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include <memory>

using namespace nautilus::capi;

extern "C" {

NautilusEngineRef nautilus_engine_create(NautilusOptionsRef options) {
	return guarded<NautilusEngineRef>(nullptr, [&] {
		const auto& engineOptions = optionsOf(options);
		// Fail now rather than on every compile: an engine pinned to a backend
		// this build lacks can never compile anything.
		const auto pinned = engineOptions.getOptionOrDefault<std::string>("engine.backend", "");
		check(pinned.empty() || nautilus::compiler::CompilationBackendRegistry::getInstance()->hasBackend(pinned),
		      NAUTILUS_ERROR_UNAVAILABLE, "engine.backend names a backend that is not available in this build");
		return new NautilusOpaqueEngine(engineOptions);
	});
}

void nautilus_engine_dispose(NautilusEngineRef engine) {
	delete engine;
}

NautilusStringRef nautilus_engine_get_backend_name(NautilusEngineRef engine) {
	return engine != nullptr ? borrow(engine->backendName) : NautilusStringRef {"", 0};
}

NautilusExecutableRef nautilus_engine_compile(NautilusEngineRef engine, NautilusIRGraphRef graph,
                                              NautilusOptionsRef overrides) {
	return guarded<NautilusExecutableRef>(nullptr, [&] {
		require(engine != nullptr, "engine is NULL");
		require(graph != nullptr, "graph is NULL");
		check(engine->engine.isCompiled(), NAUTILUS_ERROR_INVALID_STATE,
		      "the engine has compilation disabled (engine.Compilation=false)");
		prepareForPasses(graph);
		const bool runPasses = !graph->optimized;
		// Claimed before compiling: the passes rewrite the graph in place,
		// so even a compile that fails afterwards must not run them again.
		graph->optimized = true;
		auto result = std::make_unique<NautilusOpaqueExecutable>();
		result->executable =
		    compiling([&] { return engine->engine.compileIR(graph->ir, optionsOf(overrides), runPasses); });
		return result.release();
	});
}

} // extern "C"
