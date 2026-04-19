
// Plugin-side trace context that subclasses the core LazyTraceContext.
// Registered with the TraceContextRegistry under the name
// "partialEvaluation" at static-init time, so users opt into trace-time
// partial evaluation explicitly by setting
// `engine.traceMode = "partialEvaluation"` — no magic override of the
// baseline "lazyTracing" mode, no core-side knowledge of this subclass.
//
// The class adds:
//   - three lifecycle hooks (onActivate / onDeactivate /
//     beforeInnerFunction) for trace-scoped setup & teardown of the
//     Constant registry and observability counters;
//   - eight per-op trace methods that flush the Constant registry with
//     pe::materializeAllConstants() before delegating to the base.
//
// Compiled only when ENABLE_CONSTANT_TRACER is on (the plugin's
// CMakeLists.txt early-returns otherwise).

#include "nautilus/partial_evaluation/api.hpp"
#include "nautilus/tracing/TracingInterface.hpp"

// Internal core headers; the nautilus target's PRIVATE include path puts
// them on `nautilus/tracing/...` at compile time. Since this plugin's
// sources are compiled *into* the nautilus target (target_sources in
// the plugin's CMakeLists), that PRIVATE include path reaches us.
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/TraceContextRegistry.hpp"

namespace nautilus::tracing::pe {

// Anchor function: prevents the linker from dead-stripping this TU out
// of the static nautilus archive. `ConstantTracerState.cpp` calls this
// (inside a file-scope static's constructor) so the linker must resolve
// the symbol, which pulls this whole TU in and brings the Registrar
// below along with it.
void ensurePELazyTraceContextLinked() noexcept {
}

// ---------------------------------------------------------------------------
// PELazyTraceContext — direct subclass of LazyTraceContext.
//
// The overridden op list mirrors every trace point where the SSA phase
// could later need a CONST op attached to the predecessor block:
// traceBool (branches), traceBinaryOp/Unary/Ternary (merge via
// snapshot-match), traceCall/IndirectCall/NautilusCall (call-site
// boundary), traceReturnOperation (function exit). Ops that don't split
// the trace (traceConstant, traceCopy, traceAlloca, traceAssignment,
// allocate/freeValRef, pushStaticVal/popStaticVal,
// traceNautilusFunctionPtr) are not overridden — no PE bookkeeping is
// required for them.
// ---------------------------------------------------------------------------
class PELazyTraceContext : public LazyTraceContext {
public:
	using LazyTraceContext::LazyTraceContext;

	/// Trace-mode entry point. Installed in the TraceContextRegistry
	/// under the name "partialEvaluation" by the file-scope Registrar
	/// below. Overrides LazyTraceContext::Trace (which inherits the
	/// pure virtual from TraceContextBase) to route to this class's
	/// own thread_local worker.
	std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functions,
	                                   const engine::Options& options, Arena& arena) override;

	/// Signals to core's `setActiveTracer()` that PE's `LazyTracedRef`
	/// fold fast paths and the Constant registry should be live while
	/// this tracer is active. See TracingInterface::isPartialEvaluationMode.
	bool isPartialEvaluationMode() const noexcept override {
		return true;
	}

	// --- Lifecycle hooks ----------------------------------------------------

	void onActivate() noexcept override {
		// Flip the runtime flag on so LazyTracedRef's fold fast paths
		// and the Constant registry become live exactly for the scope
		// of this tracer. No other context touches the flag; the flag
		// defaults to false at thread start and stays false during
		// lazyTracing / exceptionBasedTracing runs.
		pe::setConstantTracerEnabled(true);
		// Fresh trace starting — zero the observability counters so a
		// post-trace read reflects exactly this one trace's events.
		pe::resetCountersForNewTrace();
	}

	void onDeactivate() noexcept override {
		// Tracer ending — flush any lingering Constant-registry entries
		// so the next trace starts clean.
		pe::clearConstantRegistry();
		// Flip the runtime flag back off so a subsequent non-PE trace
		// on the same thread doesn't inherit PE's fold paths.
		pe::setConstantTracerEnabled(false);
	}

	void beforeInnerFunction() noexcept override {
		// Defensive sweep in the multi-function work-list loop. By the
		// time an inner function's trace starts, the outer function's
		// val<T> destructors have already unregistered every Constant;
		// this catches the pathological leaked-val case (e.g. a val
		// stored in a global, or an exception-unwound destructor
		// skipped) before it pollutes the inner trace.
		pe::clearConstantRegistry();
	}

	// --- Per-op speculative flush ------------------------------------------

	TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                             const TypedValueRef& right) override {
		pe::materializeAllConstants();
		return LazyTraceContext::traceBinaryOp(op, resultType, left, right);
	}

	TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) override {
		pe::materializeAllConstants();
		return LazyTraceContext::traceUnaryOp(op, resultType, input);
	}

	TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
	                              const TypedValueRef& third) override {
		pe::materializeAllConstants();
		return LazyTraceContext::traceTernaryOp(op, resultType, first, second, third);
	}

	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs) override {
		pe::materializeAllConstants();
		return LazyTraceContext::traceCall(fptn, resultType, arguments, fnAttrs);
	}

	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<TypedValueRef>& arguments, FunctionAttributes fnAttrs) override {
		pe::materializeAllConstants();
		return LazyTraceContext::traceIndirectCall(fnPtrRef, resultType, arguments, fnAttrs);
	}

	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override {
		pe::materializeAllConstants();
		return LazyTraceContext::traceNautilusCall(definition, std::move(fwrapper), resultType, arguments, fnAttrs);
	}

	bool traceBool(const TypedValueRef& value, double probability) override {
		// Divergence point — every live Constant must already have a
		// stable ref before the branches split.
		pe::materializeAllConstants();
		return LazyTraceContext::traceBool(value, probability);
	}

	void traceReturnOperation(Type type, const TypedValueRef& ref) override {
		pe::materializeAllConstants();
		LazyTraceContext::traceReturnOperation(type, ref);
	}
};

std::unique_ptr<TraceModule> PELazyTraceContext::Trace(std::list<compiler::CompilableFunction>& functions,
                                                      const engine::Options& options, Arena& arena) {
	// Per-thread PE context instance. Thread-local lives in the plugin
	// (no core-side footprint), same pattern the core context uses for
	// its default.
	static thread_local PELazyTraceContext ctx;
	return ctx.startTrace(functions, options, arena);
}

// ---------------------------------------------------------------------------
// Static registrar. Runs at program start (before main) and registers the
// "partialEvaluation" trace mode with TraceContextRegistry. Because this
// TU is compiled into the `nautilus` target (via target_sources — see the
// plugin's CMakeLists.txt) it's never garbage-collected by the linker so
// long as the anchor function above is referenced.
// ---------------------------------------------------------------------------
namespace {

struct Registrar {
	Registrar() noexcept {
		TraceContextRegistry::getInstance()->registerTraceContext("partialEvaluation",
		                                                           std::make_unique<PELazyTraceContext>());
	}
};
[[maybe_unused]] const Registrar kRegistrar {};

} // namespace

} // namespace nautilus::tracing::pe
