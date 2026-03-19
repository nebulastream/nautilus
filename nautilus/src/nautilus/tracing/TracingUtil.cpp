
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstddef>
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
