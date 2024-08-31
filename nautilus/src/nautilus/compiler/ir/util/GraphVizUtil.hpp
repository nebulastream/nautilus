#pragma once
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <map>
namespace nautilus::compiler::ir {

std::string createGraphVizFromIr(const std::shared_ptr<IRGraph>& graph);

} // namespace nautilus::compiler::ir
