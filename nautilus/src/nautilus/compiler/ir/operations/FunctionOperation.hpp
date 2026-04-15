
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
class FunctionOperation : public Operation {
public:
	explicit FunctionOperation(std::string name, std::vector<BasicBlock*> functionBasicBlocks,
	                           std::vector<Type> inputArgs, std::vector<std::string> inputArgNames, Type outputArg);

	~FunctionOperation() override = default;

	[[nodiscard]] const std::string& getName() const;

	const BasicBlock& getFunctionBasicBlock() const;
	const std::vector<BasicBlock*>& getBasicBlocks() const;

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
