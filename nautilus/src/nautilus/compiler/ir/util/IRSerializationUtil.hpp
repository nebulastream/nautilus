#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <string>

namespace nautilus::compiler::ir {

class Operation;

/**
 * @brief Serializes @p graph into the portable Nautilus IR text format.
 *
 * The single IR grammar has two renderings that share one writer:
 *
 * - **Portable** (this function): complete and unambiguous. Every value
 *   (block argument or value-producing operation) is renumbered with an id
 *   that is unique within its function, external functions are identified
 *   by their mangled symbol and human-readable name, and process-specific
 *   state that cannot be reconstructed elsewhere (a non-null pointer
 *   constant) fails with a descriptive error. The output can be persisted
 *   to a file and later compiled directly — skipping tracing — via
 *   `parseIR` (see IRParser.hpp) or `NautilusEngine::loadModuleFromIR`.
 *
 * - **Display** (`IRGraph::toString`, implemented by @ref printIR):
 *   faithful to the in-memory graph for debugging. Operations keep their
 *   stored identifiers (which the tracer legitimately reuses across
 *   blocks), external function addresses stay hidden behind `func_*`
 *   placeholders unless address logging is enabled, and printing never
 *   throws.
 */
std::string serializeIR(const IRGraph& graph);

/**
 * @brief Renders @p graph in the display form of the IR grammar; the
 * implementation behind `IRGraph::toString`. See @ref serializeIR for how
 * the two renderings relate.
 */
std::string printIR(const IRGraph& graph, const IRPrintOptions& options);

/**
 * @brief Renders a single operation in display form using its stored
 * identifiers (no function context required). Used for GraphViz node
 * labels and log messages.
 */
std::string printOperation(const Operation& operation);

} // namespace nautilus::compiler::ir
