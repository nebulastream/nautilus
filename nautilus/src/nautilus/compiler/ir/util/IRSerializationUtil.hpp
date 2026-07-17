#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <string>
#include <unordered_map>

namespace nautilus::compiler::ir {

class Operation;
class FunctionOperation;

/**
 * @brief Serializes @p graph into the Nautilus IR text format.
 *
 * There is exactly one IR text format: `IRGraph::toString` (the dump
 * rendering) and this function produce byte-identical output. Every value
 * (block argument or value-producing operation) is printed with an id that
 * is unique within its function (see @ref computePrintedValueIds), external
 * functions are identified by their mangled symbol and human-readable name,
 * and the output can be persisted to a file and later compiled directly —
 * skipping tracing — via `parseIR` (see IRParser.hpp) or
 * `NautilusEngine::loadModuleFromIR`.
 *
 * The only difference to `toString` is strictness: this function first
 * validates that the graph contains no state that cannot be reconstructed
 * in another process and fails with a descriptive error if it does (e.g. a
 * non-null pointer constant, which `toString` renders as the historic `*`
 * placeholder), so a successfully serialized file is always loadable.
 */
std::string serializeIR(const IRGraph& graph);

/**
 * @brief Renders @p graph in the IR text format; the implementation behind
 * `IRGraph::toString`. Identical output to @ref serializeIR, but never
 * throws (unloadable state is rendered as placeholders) and optionally
 * appends `; ...` source-location comment trailers.
 */
std::string printIR(const IRGraph& graph, const IRPrintOptions& options);

/**
 * @brief Renders a single operation using its stored identifiers (no
 * function context required). Used for GraphViz node labels and log
 * messages; not part of the graph text format.
 */
std::string printOperation(const Operation& operation);

/**
 * @brief The value numbering used for every `$<id>` printed by the writer.
 *
 * The in-memory IR distinguishes values by pointer identity and reuses the
 * same numeric identifier for different values in different blocks (the
 * tracer numbers per value slot, and passes such as block-argument pruning
 * create cross-block references between them). The printed ids are instead
 * unique within their function — assigned block by block, arguments before
 * operations — so every reference in the text is unambiguous. Consumers
 * that need to correlate operations with the printed text (e.g. the MLIR
 * debug-info source map) can recompute the same numbering with this
 * function.
 */
using PrintedValueIds = std::unordered_map<const Operation*, uint32_t>;
PrintedValueIds computePrintedValueIds(const FunctionOperation& function);

} // namespace nautilus::compiler::ir
