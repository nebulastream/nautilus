
#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <string>
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief A single invariant violation flagged by the verifier.
 */
struct VerificationError {
	std::string function; ///< enclosing function, or empty if unresolved
	std::string block;    ///< enclosing block id as string, or empty
	std::string message;
};

/**
 * @brief Aggregate result of a verification run.
 */
struct VerificationResult {
	std::vector<VerificationError> errors;
	[[nodiscard]] bool ok() const noexcept {
		return errors.empty();
	}
	[[nodiscard]] std::string toString() const;
};

/**
 * @brief Structural verifier for `IRGraph`s.
 *
 * Checks:
 *  - Each `FunctionOperation` has at least one block.
 *  - Block identifiers are unique within a function.
 *  - Every block ends with exactly one terminator, and has no earlier
 *    terminators.
 *  - Every `BasicBlockInvocation` in a terminator targets a non-null
 *    block that is owned by the same function.
 *  - Every block listed as a target also lists the source block in its
 *    predecessor set (requires `rebuildPredecessorLists` to have run, or
 *    the wiring invariant to have been maintained by passes).
 */
class IRVerifier {
public:
	[[nodiscard]] static VerificationResult verify(const IRGraph& ir);
};

} // namespace nautilus::compiler::ir
