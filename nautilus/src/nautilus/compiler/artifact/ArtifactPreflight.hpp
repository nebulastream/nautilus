#pragma once

#include "nautilus/CompilableFunction.hpp"
#include <list>
#include <string>

namespace nautilus::compiler::ir {
class IRGraph;
}

namespace nautilus::compiler::artifact {

bool hasOnlyInvariantScalars(const ir::IRGraph& graph, std::string* reason = nullptr);
void validateArtifactRoots(const ir::IRGraph& graph, const std::list<CompilableFunction>& functions);
void validateArtifactPreflight(const ir::IRGraph& graph, const std::list<CompilableFunction>& functions);

} // namespace nautilus::compiler::artifact
