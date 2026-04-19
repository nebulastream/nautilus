
#pragma once

#include <map>
#include <memory>
#include <string>

namespace nautilus::tracing {
class TraceContextBase;

/**
 * @brief Registry mapping a trace-mode name to the TraceContextBase
 * instance that implements it.
 *
 * Mirrors CompilationBackendRegistry (backends/CompilationBackend.hpp):
 * a Meyer's singleton that owns one long-lived instance per registered
 * mode. Dispatch goes through the instance's `virtual Trace(...)`
 * method, so adding a new mode is a new subclass + a registration
 * call — no if/else edits in core.
 *
 * The registry-owned instance is a *factory*: its per-trace state
 * fields are never populated. Its `Trace()` override forwards to a
 * thread_local worker of the same concrete class, which is where the
 * actual trace state lives.
 *
 * Core modes ("lazyTracing", "exceptionBasedTracing") are registered in
 * the registry constructor. Plugin-provided modes register themselves at
 * static-init via `registerTraceContext` — see
 * `plugins/partial_evaluation/src/PELazyTraceContext.cpp` for the
 * "partialEvaluation" registration.
 *
 * Registration is not thread-safe: expected to happen only from the
 * constructor and from static initializers before main. Lookup
 * (`getTraceContext`) is read-only and thread-safe after initialization.
 */
class TraceContextRegistry {
public:
	static TraceContextRegistry* getInstance();

	/// Look up a registered trace context by name. Returns a raw
	/// non-owning pointer (registry retains ownership). Throws
	/// RuntimeException if the name is not registered — matches the
	/// CompilationBackendRegistry contract so callers see a uniform
	/// error surface.
	TraceContextBase* getTraceContext(const std::string& name) const;

	/// Register a trace context under @p name. Takes ownership.
	/// Overwrites any prior registration for the same name so plugins
	/// can replace core defaults if they really want to.
	void registerTraceContext(std::string name, std::unique_ptr<TraceContextBase> context);

	/// Default mode name when `engine.traceMode` is not set. Always
	/// "lazyTracing" — registered unconditionally by the constructor.
	std::string getDefaultName() const;

private:
	TraceContextRegistry();
	std::map<std::string, std::unique_ptr<TraceContextBase>> items_;
};

} // namespace nautilus::tracing
