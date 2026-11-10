
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstddef>
#include <fmt/format.h>
#include <iostream>

namespace nautilus::tracing {

namespace detail {
void allocateValRefSlow(ValueRef ref) {
	if (activeTracer != nullptr) {
		activeTracer->allocateValRef(ref);
	}
}
void freeValRefSlow(ValueRef ref) {
	if (activeTracer != nullptr) {
		activeTracer->freeValRef(ref);
	}
}
} // namespace detail

// `engine.foldStaticConstants` of the function being traced on this thread.
static thread_local bool foldStaticConstants = true;

bool foldsStaticConstants() {
	return foldStaticConstants;
}

void setFoldStaticConstants(bool fold) {
	foldStaticConstants = fold;
}

// --- Guarded wrappers: may be called from val<T> constructors/destructors
//     that run outside a tracing context (e.g. copies, assignments, ref-counting).

void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (detail::activeTracer) {
		detail::activeTracer->traceAssignment(target, source, resultType);
	}
}

TypedValueRef traceCopy(const TypedValueRef& ref) {
	if (detail::activeTracer) {
		return detail::activeTracer->traceCopy(ref);
	}
	return {};
}

TypedValueRef& traceExtOp(uint16_t opcode, uint8_t resultType, const std::vector<TypedValueRef>& inputs) {
	if (!detail::activeTracer) {
		throw RuntimeException("traceExtOp outside of a trace");
	}
	return detail::activeTracer->traceExtOp(opcode, resultType, inputs);
}

TypedValueRef& traceData(std::span<const std::byte> bytes, size_t align) {
	if (!detail::activeTracer) {
		throw RuntimeException("traceData outside of a trace");
	}
	return detail::activeTracer->traceData(bytes, align);
}

TypedValueRef& traceBinding(std::string_view name, size_t size, size_t align) {
	if (!detail::activeTracer) {
		throw RuntimeException("traceBinding outside of a trace");
	}
	return detail::activeTracer->traceBinding(name, size, align);
}

void noteExplicitCopy() {
	if (detail::activeTracer) {
		detail::activeTracer->noteExplicitCopy();
	}
}

void pushStaticVal(void* ptr, size_t size) {
	if (detail::activeTracer) {
		detail::activeTracer->pushStaticVal(ptr, size);
	}
}

void popStaticVal() {
	if (detail::activeTracer) {
		detail::activeTracer->popStaticVal();
	}
}

// --- Unguarded wrappers: only called from within SHOULD_TRACE() blocks,
//     where detail::activeTracer is guaranteed non-null.

void traceReturnOperation(Type type, const TypedValueRef& ref) {
	detail::activeTracer->traceReturnOperation(type, ref);
}

TypedValueRef& registerFunctionArgument(Type type, size_t index) {
	return detail::activeTracer->registerFunctionArgument(type, index);
}

TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) {
	return detail::activeTracer->traceConstant(type, value);
}

bool traceBool(const TypedValueRef& value, double probability) {
	return detail::activeTracer->traceBool(value, probability);
}

TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left, const TypedValueRef& right) {
	return detail::activeTracer->traceBinaryOp(op, resultType, left, right);
}

TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	return detail::activeTracer->traceUnaryOp(op, resultType, input);
}

TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
                              const TypedValueRef& third) {
	return detail::activeTracer->traceTernaryOp(op, resultType, first, second, third);
}

TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                         FunctionAttributes fnAttrs) {
	return detail::activeTracer->traceCall(fptn, resultType, arguments, fnAttrs);
}

TypedValueRef& traceCallWithExceptionHandling(void* fptn, Type resultType,
                                              const std::vector<tracing::TypedValueRef>& arguments,
                                              FunctionAttributes fnAttrs, void* captureFunc) {
	return detail::activeTracer->traceCallWithExceptionHandling(fptn, resultType, arguments, fnAttrs, captureFunc);
}

TypedValueRef& traceImportedCall(std::string_view moduleName, std::string_view functionName, Type resultType,
                                 const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs) {
	return detail::activeTracer->traceImportedCall(moduleName, functionName, resultType, arguments, fnAttrs);
}

void registerDestructor(const TypedValueRef& address, void* destructor) {
	detail::activeTracer->registerDestructor(address, destructor);
}

void unregisterDestructor(const TypedValueRef& address) {
	detail::activeTracer->unregisterDestructor(address);
}

void transferDestructor(const TypedValueRef& from, const TypedValueRef& to) {
	detail::activeTracer->transferDestructor(from, to);
}

TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                 const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs,
                                 void* captureFunc) {
	return detail::activeTracer->traceIndirectCall(fnPtrRef, resultType, arguments, fnAttrs, captureFunc);
}

TypedValueRef& traceIndirectCallWithExceptionHandling(const TypedValueRef& fnPtrRef, Type resultType,
                                                      const std::vector<tracing::TypedValueRef>& arguments,
                                                      FunctionAttributes fnAttrs, void* captureFunc) {
	return detail::activeTracer->traceIndirectCallWithExceptionHandling(fnPtrRef, resultType, arguments, fnAttrs,
	                                                                    captureFunc);
}

TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                                 FunctionAttributes fnAttrs) {
	return detail::activeTracer->traceNautilusCall(definition, fwrapper, resultType, arguments, fnAttrs);
}

TypedValueRef& traceNautilusCallWithExceptionHandling(const NautilusFunctionDefinition* definition,
                                                      std::function<void()> fwrapper, Type resultType,
                                                      const std::vector<tracing::TypedValueRef>& arguments,
                                                      FunctionAttributes fnAttrs) {
	return detail::activeTracer->traceNautilusCallWithExceptionHandling(definition, fwrapper, resultType, arguments,
	                                                                    fnAttrs);
}

TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper) {
	return detail::activeTracer->traceNautilusFunctionPtr(definition, std::move(fwrapper));
}

TypedValueRef& traceAlloca(size_t size, size_t align) {
	return detail::activeTracer->traceAlloca(size, align);
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
