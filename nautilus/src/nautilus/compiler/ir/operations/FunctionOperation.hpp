
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
class FunctionOperation : public Operation {
public:
	explicit FunctionOperation(std::string name, std::vector<BasicBlock*> functionBasicBlocks,
	                           std::vector<Type> inputArgs, std::vector<std::string> inputArgNames, Type outputArg);

	~FunctionOperation() = default;

	[[nodiscard]] const std::string& getName() const;

	const BasicBlock& getFunctionBasicBlock() const;
	const std::vector<BasicBlock*>& getBasicBlocks() const;

	/// Returns the function's entry block (the first entry of
	/// `functionBasicBlocks`). Passes should use this helper rather than
	/// positional access so the entry-block convention is documented at
	/// every use site.
	[[nodiscard]] BasicBlock* getEntryBlock() const;

	/// Unlinks @p block from the function's block list. This is *not* a
	/// free — every block is arena-allocated and persists in the arena
	/// until the enclosing `IRGraph` is destroyed. The method just removes
	/// the pointer from `functionBasicBlocks`, making the block
	/// unreachable from the CFG. Asserts that @p block is neither the
	/// entry block nor still has predecessors (those are pass bugs).
	void detachBasicBlock(BasicBlock* block);

	[[nodiscard]] const std::vector<Type>& getInputArgs() const;

	[[nodiscard]] Type getOutputArg() const;

	[[nodiscard]] const std::vector<std::string>& getInputArgNames() const;

	static bool classof(const Operation* Op);

private:
	std::string name;
	std::vector<BasicBlock*> functionBasicBlocks;
	std::vector<Type> inputArgs;
	std::vector<std::string> inputArgNames;
};
} // namespace nautilus::compiler::ir
