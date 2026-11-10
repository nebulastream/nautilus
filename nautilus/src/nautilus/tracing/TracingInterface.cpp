#include "nautilus/tracing/TracingInterface.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"

namespace nautilus::tracing {

// The hooks of the NIR tracer (docs/plans/nautilus-0.4/01-tracing-frontend.md §6). The 0.3 tracer records into an
// ExecutionTrace that has no place for them.

TypedValueRef& TracingInterface::traceExtOp(uint16_t, uint8_t, const std::vector<TypedValueRef>&) {
	throw NotImplementedException("this tracer cannot record extension operations");
}

TypedValueRef& TracingInterface::traceData(std::span<const std::byte>, size_t) {
	throw NotImplementedException("this tracer cannot record data objects");
}

TypedValueRef& TracingInterface::traceBinding(std::string_view, size_t, size_t) {
	throw NotImplementedException("this tracer cannot record runtime bindings");
}

TypedValueRef& TracingInterface::traceImportedCall(std::string_view, std::string_view, Type,
                                                   const std::vector<TypedValueRef>&, FunctionAttributes) {
	throw NotImplementedException("this tracer cannot record imported calls; modules link on the NIR pipeline "
	                              "(tracing.nirTracer=true)");
}

} // namespace nautilus::tracing
