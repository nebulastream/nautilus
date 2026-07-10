#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <string>

namespace nautilus::compiler::ir {

/**
 * @brief Serializes @p graph into the portable Nautilus IR text format.
 *
 * The output uses the same grammar as `IRGraph::toString` but is complete:
 * everything needed to reconstruct the graph in another process is emitted,
 * including proxy-call symbols and names, function attributes, alloca
 * specs, branch probabilities, and select/indirect-call/address-of
 * operations that the pretty printer renders lossily. The resulting text
 * can be persisted to a file and later compiled directly — skipping
 * tracing — via `parseIR` (see IRParser.hpp) or the engine-level
 * `NautilusEngine::loadModuleFromIR`.
 *
 * Runtime function addresses are process-specific and are therefore not
 * serialized; proxy calls are identified by their mangled symbol and
 * human-readable name and must be re-resolved at load time.
 *
 * @throws RuntimeException if the graph contains state that cannot be
 * reconstructed in another process (e.g. a non-null pointer constant).
 */
std::string serializeIR(const IRGraph& graph);

} // namespace nautilus::compiler::ir
