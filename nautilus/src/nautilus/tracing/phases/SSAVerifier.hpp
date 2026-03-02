
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nautilus::tracing {

class ExecutionTrace;

/**
 * @brief Result of SSA verification containing validity status and any errors found.
 */
struct SSAVerificationResult {
	bool valid;
	std::vector<std::string> errors;
};

/**
 * @brief Verifies that an execution trace is in valid SSA form.
 *
 * Checks the following properties:
 * 1. No ASSIGN operations remain (they should be eliminated by SSACreationPhase)
 * 2. Every value reference used as input is defined locally (by a block argument or a preceding operation)
 * 3. Every CMP/JMP BlockRef passes the correct number of arguments to the target block
 * 4. Every non-empty block ends with a terminator (JMP, CMP, or RETURN)
 * 5. No duplicate value definitions within a single block (excluding void result ref 0)
 *
 * @param trace The execution trace to verify
 * @return SSAVerificationResult containing validity and any error messages
 */
SSAVerificationResult VerifySSA(const ExecutionTrace& trace);

} // namespace nautilus::tracing
