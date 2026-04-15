#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"

namespace nautilus::compiler::ir {
class IfOperation : public Operation {
public:
	IfOperation(Operation* booleanValue, double probability);

	~IfOperation() override = default;

	Operation* getValue() const;

	BasicBlock* getMergeBlock();

	Operation* getBooleanValue();

	void setBooleanValue(Operation* newBooleanValue);

	void setMergeBlock(BasicBlock* mergeBlock);

	BasicBlockInvocation& getTrueBlockInvocation();
	const BasicBlockInvocation& getTrueBlockInvocation() const;
	BasicBlockInvocation& getFalseBlockInvocation();
	const BasicBlockInvocation& getFalseBlockInvocation() const;

	void setTrueBlockInvocation(BasicBlock* trueBlockInvocation);

	void setFalseBlockInvocation(BasicBlock* falseBlockInvocation);

	bool hasFalseCase();

	double getProbability() const;

	static bool classof(const Operation* op);

private:
	BasicBlockInvocation trueBlockInvocation;
	BasicBlockInvocation falseBlockInvocation;
	BasicBlock* mergeBlock = nullptr;
	double probability;
};
} // namespace nautilus::compiler::ir
