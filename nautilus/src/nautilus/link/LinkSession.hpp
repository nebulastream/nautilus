#pragma once

#include "nautilus/Module.hpp"
#include "nautilus/link/ModuleRegistry.hpp"
#include <memory>

namespace nautilus::nir {
class Module;
}

namespace nautilus::link {

/// The compilation pipeline's side of a `LinkSession` (the session is the C++ front end's side: it is created by
/// `NautilusModule::compile` and current on the compiling thread).
///
/// After the SSA module of the compile exists, the pipeline hands it to the session: the module takes the session's
/// name and the guarantees its exports declare, its imports are resolved against the registry (§4.4.2), and the result
/// is the link whose slots the backends call through. The module the pipeline then compiles is the one `commit`
/// registers.
std::shared_ptr<const ImportLink> prepareModule(engine::details::LinkSession& session, nir::Module& ssa);

/// The module that was compiled, shared with the backends, which the registry keeps for inlining and hashing.
void noteCompiledModule(engine::details::LinkSession& session, std::shared_ptr<const nir::Module> module);

} // namespace nautilus::link
