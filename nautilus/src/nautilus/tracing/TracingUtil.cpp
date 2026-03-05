
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include <fmt/format.h>
#include <iostream>

namespace nautilus::tracing {

// Thread-local pointer to the active TracingInterface implementation.
// Set to non-null during trace() and cleared afterwards, so all operations
// within a single trace go through the same implementation object.
static thread_local TracingInterface* activeTracer = nullptr;

TracingInterface* getActiveTracer() {
	return activeTracer;
}

void setActiveTracer(TracingInterface* tracer) {
	activeTracer = tracer;
}

bool inTracer() {
	return activeTracer != nullptr;
}

void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (activeTracer) {
		activeTracer->traceAssignment(target, source, resultType);
	}
}

void traceValueDestruction(const TypedValueRef& target) {
	activeTracer->traceValueDestruction(target);
}

void traceReturnOperation(Type type, const TypedValueRef& ref) {
	activeTracer->traceReturnOperation(type, ref);
}

TypedValueRef& registerFunctionArgument(Type type, size_t index) {
	return activeTracer->registerFunctionArgument(type, index);
}

TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) {
	return activeTracer->traceConstValue(type, value);
}

bool traceBool(const TypedValueRef& state, const double probability) {
	return activeTracer->traceCmp(state, probability);
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

TypedValueRef traceCopy(const TypedValueRef& state) {
	if (activeTracer) {
		return activeTracer->traceCopy(state);
	}
	return {};
}

TypedValueRef& traceBinaryOp(Op operation, Type resultType, const TypedValueRef& left, const TypedValueRef& right) {
	return activeTracer->traceBinaryOp(operation, resultType, left, right);
}

TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                         const FunctionAttributes fnAttrs) {
	return activeTracer->traceCall(fptn, resultType, arguments, fnAttrs);
}

TypedValueRef& traceUnaryOp(Op operation, Type resultType, const TypedValueRef& input) {
	return activeTracer->traceUnaryOp(operation, resultType, input);
}

TypedValueRef& traceTernaryOp(Op operation, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
                              const TypedValueRef& third) {
	return activeTracer->traceTernaryOp(operation, resultType, first, second, third);
}

std::ostream& operator<<(std::ostream& os, const Op& operation) {
	os << toString(operation);
	return os;
}

void pushStaticVal(void* valPtr) {
	if (activeTracer) {
		activeTracer->pushStaticVal(valPtr);
	}
}

void popStaticVal() {
	if (activeTracer) {
		activeTracer->popStaticVal();
	}
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

auto fmt::formatter<nautilus::ConstantLiteral>::format(nautilus::ConstantLiteral lit,
                                                       format_context& ctx) const -> format_context::iterator {
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
