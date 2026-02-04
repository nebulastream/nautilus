
#pragma once

#include "nautilus/options.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/TracerTrace.hpp"
#include <functional>
#include <memory>

namespace nautilus::tracing {

class ForkTraceContext : public TraceContext {
public:
	~ForkTraceContext() override = default;

	std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction,
	                                      const engine::Options& options) override;

	TypedValueRef registerFunctionArgument(Type type, size_t) override;

	void traceReturnOperation(Type resultType, const TypedValueRef& ref) override;
	bool traceCmp(const TypedValueRef& targetRef, double probability) override;

	void suggestInvertedBranch() override;

	/**
	 * @brief Default constructor - public to allow thread_local storage.
	 * Initializes with empty state (state == nullptr means not initialized).
	 */
	ForkTraceContext() = default;

private:
	TypedValueRef traceOperation(
	    Op op, const std::function<std::pair<TypedValueRef, Trace::TracerOperationIdentifier>(Snapshot&)>& onCreation)
	    override;
};

} // namespace nautilus::tracing
