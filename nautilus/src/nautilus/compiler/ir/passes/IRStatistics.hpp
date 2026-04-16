
#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <cstddef>
#include <string>

namespace nautilus::compiler::ir {

/**
 * @brief Aggregate counts of IR entities: functions, blocks, operations.
 *
 * Used by the pass manager to log per-pass deltas. Subtraction yields a
 * signed-free delta representation (values may wrap around if passes add
 * entities, which is fine for logging; the pass manager formats each
 * component independently).
 */
struct IRStatistics {
	size_t numFunctions = 0;
	size_t numBlocks = 0;
	size_t numOperations = 0;

	[[nodiscard]] std::string toString() const;
	[[nodiscard]] IRStatistics operator-(const IRStatistics& rhs) const noexcept;
};

/// Walks @p ir once and returns a fresh `IRStatistics` snapshot.
[[nodiscard]] IRStatistics computeStatistics(const IRGraph& ir);

} // namespace nautilus::compiler::ir
