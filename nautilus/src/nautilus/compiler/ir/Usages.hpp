
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <unordered_map>

namespace nautilus::compiler::ir {

/**
 * @brief Counts one use per static SSA reference for every operation
 * reachable from @p entryBlock.
 *
 * Walks every reachable block and accumulates one use per static reference.
 * Operation::getInputs() gives the SSA operand span uniformly — the only
 * values that live outside that span are the arguments attached to the
 * BasicBlockInvocations embedded in IfOperation/BranchOperation terminators,
 * which are visited explicitly.
 *
 * Backends use the result to decide whether a value's materialisation can be
 * folded into its sole consumer (e.g. fusing a compare into the conditional
 * branch that reads it).
 */
std::unordered_map<OperationIdentifier, uint32_t> countUsages(const BasicBlock* entryBlock);

} // namespace nautilus::compiler::ir
