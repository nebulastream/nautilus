#pragma once
#include "nautilus/compiler/ir/IRGraph.hpp"
namespace nautilus::compiler::ir {

std::string createGraphVizFromIr(const std::shared_ptr<IRGraph>& graph);

} // namespace nautilus::compiler::ir
