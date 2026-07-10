
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/util/IRSerializationUtil.hpp"
#include <utility>

namespace nautilus::compiler::ir {

IRGraph::IRGraph(const compiler::CompilationUnitID& id) : arena_(common::ArenaPool::makeStandalone()), id(id) {
}

IRGraph::IRGraph(common::ArenaPool::Handle arena, const compiler::CompilationUnitID& id)
    : arena_(std::move(arena)), id(id) {
}

FunctionOperation* IRGraph::addFunctionOperation(FunctionOperation* functionOperation) {
	functionOperations.emplace_back(functionOperation);
	functionOperationsByName.emplace(std::string_view {functionOperation->getName()}, functionOperation);
	return functionOperation;
}

const std::vector<FunctionOperation*>& IRGraph::getFunctionOperations() const {
	return functionOperations;
}

const FunctionOperation* IRGraph::getFunctionOperation(const std::string& name) const {
	auto it = functionOperationsByName.find(std::string_view {name});
	return it != functionOperationsByName.end() ? it->second : nullptr;
}

const CompilationUnitID& IRGraph::getId() const {
	return id;
}

std::string IRGraph::toString() const {
	return printIR(*this, IRPrintOptions {});
}

std::string IRGraph::toString(const IRPrintOptions& options) const {
	return printIR(*this, options);
}

} // namespace nautilus::compiler::ir
