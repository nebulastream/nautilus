#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstddef>
#include <optional>
#include <unordered_map>

namespace nautilus::compiler::ir {

/// Per-function record of one stack allocation.  Each AllocaOperation in the
/// IR references one of these by index; backends emit the real `alloca`
/// instruction (or equivalent) once per entry in the function prologue.
struct AllocaSpec {
	size_t size;
	size_t align;
};

class FunctionOperation : public Operation {
public:
	explicit FunctionOperation(std::string name, std::vector<std::unique_ptr<BasicBlock>>& functionBasicBlocks,
	                           std::vector<Type> inputArgs, std::vector<std::string> inputArgNames, Type outputArg,
	                           std::vector<AllocaSpec> allocaSpecs = {},
	                           std::unordered_map<std::string, std::string> attributes = {});

	~FunctionOperation() override = default;

	[[nodiscard]] const std::string& getName() const;

	BasicBlock* addFunctionBasicBlock(BasicBlockPtr functionBasicBlock);

	const BasicBlock& getFunctionBasicBlock() const;
	const std::vector<std::unique_ptr<BasicBlock>>& getBasicBlocks() const;

	/// Returns the function's entry block, or nullptr if no blocks have been
	/// added yet.  Non-owning pointer; storage stays in functionBasicBlocks.
	[[nodiscard]] BasicBlock* getEntryBlock() const;

	[[nodiscard]] const std::vector<Type>& getInputArgs() const;

	[[nodiscard]] Type getOutputArg() const;

	[[nodiscard]] const std::vector<std::string>& getInputArgNames() const;

	/// Returns the function's alloca table.  Each entry corresponds to one
	/// stack allocation site observed during tracing; AllocaOperation nodes
	/// in this function's body reference entries by index.
	[[nodiscard]] const std::vector<AllocaSpec>& getAllocaSpecs() const;

	[[nodiscard]] bool hasAttribute(const std::string& key) const;
	[[nodiscard]] std::optional<std::string> getAttribute(const std::string& key) const;

	static bool classof(const Operation* Op);

private:
	std::string name;
	std::vector<std::unique_ptr<BasicBlock>> functionBasicBlocks;
	std::vector<Type> inputArgs;
	std::vector<std::string> inputArgNames;
	std::vector<AllocaSpec> allocaSpecs;
	std::unordered_map<std::string, std::string> attributes;
};
} // namespace nautilus::compiler::ir
