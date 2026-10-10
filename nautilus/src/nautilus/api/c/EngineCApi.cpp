#include "CApiInternal.hpp"
#include "nautilus/c/engine.h"
#include <memory>

using namespace nautilus::capi;

extern "C" {

NautilusEngineRef nautilus_engine_create(NautilusIROptionsRef options) {
	return guarded<NautilusEngineRef>(nullptr, [&] { return new NautilusOpaqueEngine(optionsOf(options)); });
}

void nautilus_engine_dispose(NautilusEngineRef engine) {
	delete engine;
}

const char* nautilus_engine_get_backend_name(NautilusEngineRef engine) {
	return engine != nullptr ? engine->backendName.c_str() : nullptr;
}

NautilusIRExecutableRef nautilus_engine_compile(NautilusEngineRef engine, NautilusIRGraphRef graph,
                                                NautilusIROptionsRef overrides) {
	return guarded<NautilusIRExecutableRef>(nullptr, [&] {
		require(engine != nullptr, "engine is NULL");
		require(graph != nullptr, "graph is NULL");
		prepareForPasses(graph);
		const bool runPasses = !graph->optimized;
		// Claimed before compiling: the passes rewrite the graph in place,
		// so even a compile that fails afterwards must not run them again.
		graph->optimized = true;
		auto result = std::make_unique<NautilusIROpaqueExecutable>();
		result->executable = engine->engine.compileIR(graph->ir, optionsOf(overrides), runPasses);
		return result.release();
	});
}

} // extern "C"
