
#include "nautilus/tracing/TraceContextRegistry.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include <utility>

namespace nautilus::tracing {

namespace {
// Default mode name returned by getDefaultName(). "partialEvaluation" when
// the PE plugin is compiled in — so building with ENABLE_CONSTANT_TRACER
// keeps the pre-refactor behavior where `engine.traceMode` unset meant
// "give me PE". Users who want the baseline lazy tracer set
// `engine.traceMode = "lazyTracing"` explicitly.
#ifdef ENABLE_CONSTANT_TRACER
constexpr auto DEFAULT_NAME = "partialEvaluation";
#else
constexpr auto DEFAULT_NAME = "lazyTracing";
#endif
} // namespace

TraceContextRegistry::TraceContextRegistry() {
#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
	items_.emplace("lazyTracing", std::make_unique<LazyTraceContext>());
	items_.emplace("exceptionBasedTracing", std::make_unique<ExceptionBasedTraceContext>());
#endif
}

TraceContextRegistry* TraceContextRegistry::getInstance() {
	// Meyer's singleton — same pattern CompilationBackendRegistry uses.
	// Returns a mutable pointer (not const) because plugins call
	// registerTraceContext on it at static-init.
	static auto registry = std::make_unique<TraceContextRegistry>(TraceContextRegistry());
	return registry.get();
}

TraceContextBase* TraceContextRegistry::getTraceContext(const std::string& name) const {
	if (!items_.contains(name)) {
		throw RuntimeException("Trace context \"" + name + "\" is not registered");
	}
	return items_.at(name).get();
}

void TraceContextRegistry::registerTraceContext(std::string name, std::unique_ptr<TraceContextBase> context) {
	items_[std::move(name)] = std::move(context);
}

std::string TraceContextRegistry::getDefaultName() const {
	return DEFAULT_NAME;
}

} // namespace nautilus::tracing
