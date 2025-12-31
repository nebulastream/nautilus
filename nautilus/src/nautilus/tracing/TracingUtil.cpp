
#include "nautilus/tracing/TracingUtil.hpp"
#include "TraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include <fmt/format.h>
#include <iostream>
namespace nautilus::tracing {

void traceAssignment(TypedValueRef target, TypedValueRef source, Type resultType) {
	if (auto* ctx = TraceContext::get()) {
		ctx->traceAssignment(target, source, resultType);
	}
}

void traceValueDestruction(TypedValueRef target) {
	TraceContext::get()->traceValueDestruction(target);
}

void traceReturnOperation(Type type, TypedValueRef ref) {
	TraceContext::get()->traceReturnOperation(type, ref);
}

TypedValueRef registerFunctionArgument(Type type, size_t index) {
	return TraceContext::get()->registerFunctionArgument(type, index);
}

TypedValueRef traceConstant(Type type, const ConstantLiteral& value) {
	return TraceContext::get()->traceConstValue(type, value);
}

bool traceBool(TypedValueRef state, double probability) {
	return TraceContext::get()->traceCmp(state, probability);
}

void allocateValRef(ValueRef ref) {
	if (auto* ctx = TraceContext::get()) {
		ctx->allocateValRef(ref);
	}
}
void freeValRef(ValueRef ref) {
	if (auto* ctx = TraceContext::get()) {
		ctx->freeValRef(ref);
	}
}

TypedValueRef traceCopy(TypedValueRef state) {
	if (auto* ctx = TraceContext::get()) {
		return ctx->traceCopy(state);
	}
	return {};
}

bool inTracer() {
	return TraceContext::get() != nullptr;
}

TraceContext* getTracerIfActive() {
	return TraceContext::get();
}

TypedValueRef traceBinaryOp(Op operation, Type resultType, TypedValueRef left, TypedValueRef right) {
	return TraceContext::get()->traceOperation(operation, resultType, {left, right});
}

TypedValueRef traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                        FunctionAttributes fnAttrs) {
	return TraceContext::get()->traceCall(fptn, resultType, arguments, fnAttrs);
}

TypedValueRef traceUnaryOp(Op operation, Type resultType, TypedValueRef input) {
	return TraceContext::get()->traceOperation(operation, resultType, {input});
}

TypedValueRef traceTernaryOp(Op operation, Type resultType, TypedValueRef first, TypedValueRef second,
                             TypedValueRef third) {
	return TraceContext::get()->traceOperation(operation, resultType, {first, second, third});
}

std::ostream& operator<<(std::ostream& os, const Op& operation) {
	os << toString(operation);
	return os;
}

void pushStaticVal(void* valPtr) {
	if (auto* ctx = TraceContext::get()) {
		ctx->getStaticVars().emplace_back((size_t*) valPtr);
	}
}

void popStaticVal() {
	if (auto* ctx = TraceContext::get()) {
		ctx->getStaticVars().pop_back();
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
