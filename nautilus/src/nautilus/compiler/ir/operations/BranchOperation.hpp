
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"

namespace nautilus::compiler::ir {
/**
 * @brief Terminator Operation(Op), must be last Op in BasicBlock(BB). Passes control flow from one BB to another.
 */
class BranchOperation : public Operation {
public:
	explicit BranchOperation();

	~BranchOperation() override = default;

	const BasicBlockInvocation& getNextBlockInvocation() const;
	BasicBlockInvocation& getNextBlockInvocation();

	static bool classof(const Operation* Op);

private:
	BasicBlockInvocation basicBlock;
};
} // namespace nautilus::compiler::ir
