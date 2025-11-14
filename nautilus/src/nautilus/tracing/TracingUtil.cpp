
#include "nautilus/tracing/TracingUtil.hpp"
#include "TraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include <cxxabi.h>
#include <dlfcn.h>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
namespace nautilus::tracing {

std::string getMangledName(void* fnptr) {
	Dl_info info;
	dladdr(reinterpret_cast<void*>(fnptr), &info);
	if (info.dli_sname != nullptr) {
		return info.dli_sname;
	}
	std::stringstream ss;
	ss << fnptr;
	return ss.str();
}

std::string getFunctionName(const std::string& mangledName) {
	// std::size_t sz = 25;
	// char* buffer = static_cast<char*>(std::malloc(sz));
	// int status;
	// char* realname = abi::__cxa_demangle(mangledName.c_str(), buffer, &sz, &status);
	// if (realname) {
	//	return realname;
	// }
	return mangledName;
}

void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	TraceContext::get()->traceAssignment(target, source, resultType);
}

void traceValueDestruction(const TypedValueRef& target) {
	TraceContext::get()->traceValueDestruction(target);
}

void traceReturnOperation(Type type, const TypedValueRef& ref) {
	TraceContext::get()->traceReturnOperation(type, ref);
}

TypedValueRef& registerFunctionArgument(Type type, size_t index) {
	return TraceContext::get()->registerFunctionArgument(type, index);
}

TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) {
	return TraceContext::get()->traceConstValue(type, value);
}

bool traceBool(const TypedValueRef& state) {
	return TraceContext::get()->traceCmp(state);
}

void allocateValRef(ValueRef ref) {
	TraceContext::get()->allocateValRef(ref);
}
void freeValRef(ValueRef ref) {
	TraceContext::get()->freeValRef(ref);
}

TypedValueRef traceCopy(const TypedValueRef& state) {
	if (inTracer()) {
		return TraceContext::get()->traceCopy(state);
	}
	return {};
}

bool inTracer() {
	return TraceContext::get() != nullptr;
}

TypedValueRef& traceBinaryOp(Op operation, Type resultType, const TypedValueRef& left, const TypedValueRef& right) {
	return TraceContext::get()->traceOperation(operation, resultType, {left, right});
}

TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                         const FunctionAttributes fnAttrs) {
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(mangledName);
	return TraceContext::get()->traceCall(functionName, mangledName, fptn, resultType, arguments, fnAttrs);
}

TypedValueRef& traceUnaryOp(Op operation, Type resultType, const TypedValueRef& input) {
	return TraceContext::get()->traceOperation(operation, resultType, {input});
}

std::ostream& operator<<(std::ostream& os, const Op& operation) {
	os << toString(operation);
	return os;
}

void pushStaticVal(void* valPtr) {
	if (inTracer()) {
		TraceContext::get()->getStaticVars().emplace_back((size_t*) valPtr);
	}
}

void popStaticVal() {
	if (inTracer()) {
		TraceContext::get()->getStaticVars().pop_back();
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
