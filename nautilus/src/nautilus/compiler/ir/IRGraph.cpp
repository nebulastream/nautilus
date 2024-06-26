
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/IRDumpHandler.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <sstream>
#include <utility>

namespace nautilus::compiler::ir {

IRGraph::IRGraph() = default;

std::unique_ptr<FunctionOperation>& IRGraph::addRootOperation(std::unique_ptr<FunctionOperation> root) {
	this->rootOperation = std::move(root);
	return this->rootOperation;
}

FunctionOperation* IRGraph::getRootOperation() {
	return rootOperation.get();
}

std::string IRGraph::toString() const {
	std::stringstream ss;
	ss << "NESIR {\n";
	auto dumpHandler = NESIRDumpHandler::create(ss);
	dumpHandler->dump(rootOperation);
	ss << "} //NESIR";
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const IRGraph& graph) {
	os << graph.toString();
	return os;
}

} // namespace nautilus::compiler::ir
