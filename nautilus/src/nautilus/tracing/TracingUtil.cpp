#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cctype>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <fmt/format.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nautilus::tracing {

// Thread-local pointer to the active TracingInterface implementation.
// Set to non-null during trace() and cleared afterwards, so all operations
// within a single trace go through the same implementation object.
static thread_local TracingInterface* activeTracer = nullptr;

// Trace-op pre-hooks (speculative flushes, etc.) live on subclasses of
// the concrete trace context, not in these free-function wrappers. When
// the partial-evaluation plugin is on, a subclassed context is routed
// in via LazyTraceContext::setTraceOverride, and its per-op overrides
// intercept each wrapper's activeTracer->traceXxx() call. Core stays
// unaware of any of that.

TracingInterface* getActiveTracer() {
	return activeTracer;
}

void setActiveTracer(TracingInterface* tracer) {
	// Fire onDeactivate on the outgoing tracer (if any) and onActivate on
	// the incoming one. Both hooks are virtual no-ops on TracingInterface;
	// subclasses (the partial-evaluation plugin in particular) override
	// them for per-trace state init/teardown — e.g. resetting its
	// observability counters on activation, flushing its Constant registry
	// on deactivation. This keeps the core tracer unaware of any
	// subclass's lifecycle concerns.
	if (activeTracer != nullptr) {
		activeTracer->onDeactivate();
	}
	activeTracer = tracer;
	if (tracer != nullptr) {
		tracer->onActivate();
	}
}

bool inTracer() {
	return activeTracer != nullptr;
}

// --- Guarded wrappers: may be called from val<T> constructors/destructors
//     that run outside a tracing context (e.g. copies, assignments, ref-counting).

void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (activeTracer) {
		activeTracer->traceAssignment(target, source, resultType);
	}
}

TypedValueRef traceCopy(const TypedValueRef& ref) {
	if (activeTracer) {
		return activeTracer->traceCopy(ref);
	}
	return {};
}

void allocateValRef(ValueRef ref) {
	if (activeTracer) {
		activeTracer->allocateValRef(ref);
	}
}

void freeValRef(ValueRef ref) {
	if (activeTracer) {
		activeTracer->freeValRef(ref);
	}
}

void pushStaticVal(void* ptr, size_t size) {
	if (activeTracer) {
		activeTracer->pushStaticVal(ptr, size);
	}
}

void popStaticVal() {
	if (activeTracer) {
		activeTracer->popStaticVal();
	}
}

// --- Unguarded wrappers: only called from within SHOULD_TRACE() blocks,
//     where activeTracer is guaranteed non-null.

void traceReturnOperation(Type type, const TypedValueRef& ref) {
	activeTracer->traceReturnOperation(type, ref);
}

TypedValueRef& registerFunctionArgument(Type type, size_t index) {
	return activeTracer->registerFunctionArgument(type, index);
}

TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) {
	return activeTracer->traceConstant(type, value);
}

bool traceBool(const TypedValueRef& value, double probability) {
	return activeTracer->traceBool(value, probability);
}

TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left, const TypedValueRef& right) {
	return activeTracer->traceBinaryOp(op, resultType, left, right);
}

TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	return activeTracer->traceUnaryOp(op, resultType, input);
}

TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
                              const TypedValueRef& third) {
	return activeTracer->traceTernaryOp(op, resultType, first, second, third);
}

TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                         FunctionAttributes fnAttrs) {
	return activeTracer->traceCall(fptn, resultType, arguments, fnAttrs);
}

TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                 const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs) {
	return activeTracer->traceIndirectCall(fnPtrRef, resultType, arguments, fnAttrs);
}

TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                                 FunctionAttributes fnAttrs) {
	return activeTracer->traceNautilusCall(definition, fwrapper, resultType, arguments, fnAttrs);
}

TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper) {
	return activeTracer->traceNautilusFunctionPtr(definition, std::move(fwrapper));
}

TypedValueRef& traceAlloca(size_t allocSize) {
	return activeTracer->traceAlloca(allocSize);
}

std::ostream& operator<<(std::ostream& os, const Op& op) {
	os << toString(op);
	return os;
}

} // namespace nautilus::tracing

namespace nautilus {
std::ostream& operator<<(std::ostream& os, const Type& type) {
	os << toString(type);
	return os;
}
} // namespace nautilus

namespace fmt {
template <>
struct formatter<nautilus::ConstantLiteral> : formatter<std::string_view> {
	auto format(nautilus::ConstantLiteral c, format_context& ctx) const -> format_context::iterator;
};
} // namespace fmt

auto fmt::formatter<nautilus::ConstantLiteral>::format(nautilus::ConstantLiteral lit, format_context& ctx) const
    -> format_context::iterator {
	auto out = ctx.out();
	std::visit(
	    [&](auto&& value) {
		    using T = std::decay_t<decltype(value)>;
		    if constexpr (!std::is_pointer_v<T>) {
			    fmt::format_to(out, "{}\t", value);
		    } else {
			    fmt::format_to(out, "*\t");
		    }
	    },
	    lit);
	return out;
}
