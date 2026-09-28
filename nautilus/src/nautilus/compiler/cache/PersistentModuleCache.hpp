#pragma once

#include "nautilus/compiler/CompilationPipeline.hpp"
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace nautilus::engine {
class ModuleOptions;
}

namespace nautilus::compiler {
class CompilableFunction;
class CompilationStatistics;
class Executable;
namespace ir {
class IRGraph;
}

bool containsNonRelocatablePointer(const ir::IRGraph& graph, const std::vector<std::string>& exports);

std::unique_ptr<Executable> compileWithPersistentModuleCache(const CompilationPipeline& compiler,
                                                             std::list<CompilableFunction>& functions,
                                                             const engine::ModuleOptions& moduleOptions,
                                                             CompilationStatistics* statistics);

} // namespace nautilus::compiler
