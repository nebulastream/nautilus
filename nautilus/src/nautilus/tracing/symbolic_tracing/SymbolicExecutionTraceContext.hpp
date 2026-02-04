#pragma once

#include "nautilus/options.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include <functional>
#include <memory>

namespace nautilus::tracing {

class SymbolicExecutionTraceContext : public TraceContext {
public:
	~SymbolicExecutionTraceContext() override = default;

	std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction,
	                                      const engine::Options& options) override;

	TypedValueRef registerFunctionArgument(Type type, size_t index) override;
	TypedValueRef traceConstValue(Type type, const ConstantLiteral& constValue) override;

	void traceReturnOperation(Type resultType, const TypedValueRef& ref) override;

	bool traceCmp(const TypedValueRef& targetRef, double probability) override;

	void resetState() override;

	/**
	 * @brief Default constructor - public to allow thread_local storage.
	 * Initializes with empty state (state == nullptr means not initialized).
	 */
	SymbolicExecutionTraceContext() = default;

private:
	SymbolicExecutionState* symbolicExecutionState {};
	std::unordered_map<uint16_t, Trace::TraceToBlockOffset> blockMap; // track offset for each block
	std::vector<Trace::Operation> currentBlockOps;
	std::vector<Trace::Operation>::iterator currentBlockIterator; // for following the existing trace
	void moveCurrentBlock(u_int16_t targetBlockId);
	void saveCurrentBlock();
	bool isFollowing();
	TypedValueRef& follow(Op op);
	void skipJumps();

	TypedValueRef traceOperation(
	    Op op, const std::function<std::pair<TypedValueRef, Trace::TracerOperationIdentifier>(Snapshot&)>& onCreation)
	    override;
};

} // namespace nautilus::tracing
