
#include "nautilus/tracing/TracingUtil.hpp"
#include "TraceContext.hpp"
#include "nautilus/logging.hpp"
#include <cxxabi.h>
#include <dlfcn.h>
#include <iostream>
#include <spdlog/fmt/fmt.h>
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
	std::size_t sz = 25;
	char* buffer = static_cast<char*>(std::malloc(sz));
	int status;
	char* realname = abi::__cxa_demangle(mangledName.c_str(), buffer, &sz, &status);
	if (realname) {
		return realname;
	}
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

value_ref registerFunctionArgument(Type type, size_t index) {
	return TraceContext::get()->registerFunctionArgument(type, index);
}

value_ref traceLoad(const TypedValueRef& src, Type resultType) {
	return TraceContext::get()->traceLoad(src, resultType);
}

void traceStore(const TypedValueRef& target, const TypedValueRef& src, Type valueType) {
	TraceContext::get()->traceStore(target, src, valueType);
}

value_ref traceConstant(Type type, const ConstantLiteral& value) {
	return TraceContext::get()->traceConstValue(type, value);
}

bool traceBool(const TypedValueRef& state) {
	return TraceContext::get()->traceCmp(state);
}

value_ref traceCast(const TypedValueRef& state, Type resultType) {
	return TraceContext::get()->traceCast(state, resultType);
}

void allocateValRef(ValueRef ref) {
	TraceContext::get()->allocateValRef(ref);
}
void freeValRef(ValueRef ref) {
	TraceContext::get()->freeValRef(ref);
}

value_ref traceCopy(const value_ref& state) {
	if (inTracer()) {
		return TraceContext::get()->traceCopy(state);
	}
	return {};
}

bool inTracer() {
	return TraceContext::get() != nullptr;
}

value_ref traceCall(void* fptn, Type resultType, const std::vector<tracing::value_ref>& arguments) {
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(mangledName);
	return TraceContext::get()->traceCall(functionName, mangledName, fptn, resultType, arguments);
}

value_ref traceBinaryOp(Op operation, Type resultType, const TypedValueRef& leftState, const TypedValueRef& rightState) {
	return TraceContext::get()->traceBinaryOperation(operation, resultType, leftState, rightState);
}

value_ref traceUnaryOp(Op operation, Type resultType, const TypedValueRef& inputState) {
	return TraceContext::get()->traceUnaryOperation(operation, resultType, inputState);
}

template value_ref traceConstant<char>(char value);

template value_ref traceConstant<int8_t>(int8_t value);

template value_ref traceConstant<int16_t>(int16_t value);

template value_ref traceConstant<int32_t>(int32_t value);

template value_ref traceConstant<int64_t>(int64_t value);

template value_ref traceConstant<uint8_t>(uint8_t value);

template value_ref traceConstant<uint16_t>(uint16_t value);

template value_ref traceConstant<uint32_t>(uint32_t value);

template value_ref traceConstant<uint64_t>(uint64_t value);

#if __APPLE__

template value_ref traceConstant<size_t>(size_t value);

#endif

template value_ref traceConstant<float>(float value);

template value_ref traceConstant<double>(double value);

template value_ref traceConstant<bool>(bool value);

template value_ref traceConstant<char*>(char* value);

template value_ref traceConstant<const char*>(const char* value);

// template value_ref traceConstant<char8_t*>(char8_t* value);
template value_ref traceConstant<char16_t*>(char16_t* value);

template value_ref traceConstant<char32_t*>(char32_t* value);

template value_ref traceConstant<wchar_t*>(wchar_t* value);

template value_ref traceConstant<int8_t*>(int8_t* value);

template value_ref traceConstant<int16_t*>(int16_t* value);

template value_ref traceConstant<int32_t*>(int32_t* value);

template value_ref traceConstant<int64_t*>(int64_t* value);

template value_ref traceConstant<uint8_t*>(uint8_t* value);

template value_ref traceConstant<uint16_t*>(uint16_t* value);

template value_ref traceConstant<uint32_t*>(uint32_t* value);

template value_ref traceConstant<uint64_t*>(uint64_t* value);

#if __APPLE__

template value_ref traceConstant<size_t*>(size_t* value);

#endif

std::ostream& operator<<(std::ostream& os, const Op& operation) {
	switch (operation) {
	case FREE:
		os << "FREE";
		break;
	case CAST:
		os << "CAST";
		break;
	case CALL:
		os << "CALL";
		break;
	case LOAD:
		os << "LOAD";
		break;
	case STORE:
		os << "STORE";
		break;
	case JMP:
		os << "JMP";
		break;
	case CMP:
		os << "CMP";
		break;
	case RETURN:
		os << "RETURN";
		break;
	case ASSIGN:
		os << "ASSIGN";
		break;
	case CONST:
		os << "CONST";
		break;
	case EQ:
		os << "EQ";
		break;
	case NEQ:
		os << "NEQ";
		break;
	case LT:
		os << "LT";
		break;
	case LTE:
		os << "LTE";
		break;
	case GT:
		os << "GT";
		break;
	case GTE:
		os << "GTE";
		break;
	case NOT:
		os << "NOT";
		break;
	case AND:
		os << "AND";
		break;
	case OR:
		os << "OR";
		break;
	case ADD:
		os << "ADD";
		break;
	case MUL:
		os << "MUL";
		break;
	case DIV:
		os << "DIV";
		break;
	case SUB:
		os << "SUB";
		break;
	case MOD:
		os << "MOD";
		break;
	case LSH:
		os << "LSH";
		break;
	case RSH:
		os << "RSH";
		break;
	case BOR:
		os << "BOR";
		break;
	case BAND:
		os << "BAND";
		break;
	case BXOR:
		os << "BXOR";
		break;
	case NEGATE:
		os << "NEGATE";
		break;
	}
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

std::string TypedValueRef::toString() const {
	return fmt::format("${}", ref);
}

} // namespace nautilus::tracing

namespace nautilus {
std::ostream& operator<<(std::ostream& os, const Type& type) {
	os << toString(type);
	return os;
}
} // namespace nautilus
