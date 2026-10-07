#pragma once

#include "nautilus/options.hpp"
#include <list>
#include <memory>
#include <string>

namespace nautilus::compiler {
class CompilationPipeline;
class CompilationStatistics;
class CompilableFunction;
class Executable;
} // namespace nautilus::compiler

namespace nautilus::cache::detail {

std::unique_ptr<compiler::Executable>
compileWithPersistentModuleCache(const compiler::CompilationPipeline& compiler,
                                 std::list<compiler::CompilableFunction>& functions,
                                 const engine::ModuleOptions& options, compiler::CompilationStatistics* statistics);
void recordCacheDecline(compiler::CompilationStatistics& statistics, const engine::ModuleOptions& options,
                        std::string reason, bool tracingRan = true);

} // namespace nautilus::cache::detail
