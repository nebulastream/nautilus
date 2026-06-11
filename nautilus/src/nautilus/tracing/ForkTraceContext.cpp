
#include "ForkTraceContext.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"

namespace nautilus::tracing {

ForkTraceContext::~ForkTraceContext() = default;

// Placeholder implementation - replaced by the full fork-based tracer in this change.
TypedValueRef& ForkTraceContext::registerFunctionArgument(Type, size_t) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceConstant(Type, const ConstantLiteral&) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceAlloca(size_t, size_t) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceCopy(const TypedValueRef&) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceBinaryOp(Op, Type, const TypedValueRef&, const TypedValueRef&) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceUnaryOp(Op, Type, const TypedValueRef&) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceTernaryOp(Op, Type, const TypedValueRef&, const TypedValueRef&,
                                                const TypedValueRef&) {
	return dummyRef_;
}
void ForkTraceContext::traceReturnOperation(Type, const TypedValueRef&) {
}
void ForkTraceContext::traceAssignment(const TypedValueRef&, const TypedValueRef&, Type) {
}
TypedValueRef& ForkTraceContext::traceCall(void*, Type, const std::vector<tracing::TypedValueRef>&,
                                           FunctionAttributes) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceIndirectCall(const TypedValueRef&, Type,
                                                   const std::vector<tracing::TypedValueRef>&, FunctionAttributes) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceNautilusCall(const NautilusFunctionDefinition*, std::function<void()>, Type,
                                                   const std::vector<tracing::TypedValueRef>&, FunctionAttributes) {
	return dummyRef_;
}
TypedValueRef& ForkTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition*, std::function<void()>) {
	return dummyRef_;
}
bool ForkTraceContext::traceBool(const TypedValueRef&, double) {
	return false;
}
void ForkTraceContext::allocateValRef(ValueRef) {
}
void ForkTraceContext::freeValRef(ValueRef) {
}
void ForkTraceContext::pushStaticVal(void*, size_t) {
}
void ForkTraceContext::popStaticVal() {
}
void ForkTraceContext::resume() {
}

std::unique_ptr<ExecutionTrace> ForkTraceContext::trace(std::function<void()>&, const engine::Options&, Arena&) {
	throw RuntimeException("forkTracing is not implemented yet");
}

std::unique_ptr<TraceModule> ForkTraceContext::Trace(std::list<compiler::CompilableFunction>&, const engine::Options&,
                                                     Arena&) {
	throw RuntimeException("forkTracing is not implemented yet");
}

std::unique_ptr<TraceModule> ForkTraceContext::startTrace(std::list<compiler::CompilableFunction>&,
                                                          const engine::Options&, Arena&) {
	throw RuntimeException("forkTracing is not implemented yet");
}

} // namespace nautilus::tracing
