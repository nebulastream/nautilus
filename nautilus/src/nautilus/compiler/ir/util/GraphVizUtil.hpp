#pragma once
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/options.hpp"
#include <map>
namespace nautilus::compiler::ir {

void createGraphVizFromIr(const std::shared_ptr<IRGraph>& graph, const engine::Options& options, const DumpHandler& dumpHandler);

} // namespace nautilus::compiler::ir
