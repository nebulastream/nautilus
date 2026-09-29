#pragma once

#include "nautilus/compiler/CompilationPipeline.hpp"
#include <list>
#include <memory>

namespace nautilus::engine {
class ModuleOptions;
}

namespace nautilus::compiler {
class CompilableFunction;
class CompilationStatistics;
class Executable;

std::unique_ptr<Executable> compileWithPersistentModuleCache(const CompilationPipeline& compiler,
                                                             std::list<CompilableFunction>& functions,
                                                             const engine::ModuleOptions& moduleOptions,
                                                             CompilationStatistics* statistics);

} // namespace nautilus::compiler
