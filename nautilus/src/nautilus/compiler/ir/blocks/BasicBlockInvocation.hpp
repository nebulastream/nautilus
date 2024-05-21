#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class BasicBlockInvocation : public Operation {
public:
	BasicBlockInvocation();

	void setBlock(BasicBlock* block);

	[[nodiscard]] const BasicBlock* getBlock() const;

	void addArgument(Operation* argument);

	void removeArgument(uint64_t argumentIndex);

	void replaceArgument(Operation* toReplace, Operation* replaceWith);

	void replaceArgument(const Operation* toReplace, Operation* replaceWith);

	const std::vector<Operation*>& getArguments() const;

	/**
	 * @brief Iterate over args, find arg that matches Operation* and return index.
	 *
	 * @return int: arg index for provided Operation*. Is -1 if no arg that matches Operation* was found.
	 */
	int getOperationArgIndex(Operation*);

	std::string toString() override;

private:
	BasicBlock* basicBlock;
	std::vector<Operation*> operations;
};

} // namespace nautilus::compiler::ir
